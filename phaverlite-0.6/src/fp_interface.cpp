#include <phaverlite-config.h>

#include "fp_interface.h"
#include "parameters.hh"

double Round(double Zahl, int Stellen) {
  if (Stellen > 0)
    return floor(Zahl * pow(10.0, (double)Stellen) + 0.5)
      * pow(10.0, -(double)Stellen);
  else
    return Zahl;
}

double Gen_to_double(const Gen& g, dim_type pos) {
  mpq_class q(g.coeff(Var(pos)), g.divisor());
  return Round(q.get_d(), GENERATOR_TO_DOUBLE_PRECISION);
}

DoublePoint
Gen_to_DoublePoint(dim_type dim, const Gen& g) {
  assert(g.is_point() || g.is_closure_point());
  assert(g.space_dim() <= dim);
  DoublePoint p(dim, 0.0);
  for (dim_type i = 0; i < g.space_dim(); ++i)
    p[i] = Gen_to_double(g, i);
  return p;
}

void Con_to_DoublePoint(dim_type dim, const Con& c, DoublePoint& dx) {
  assert(c.space_dim() <= dim);
  // Returns the constraint as a normal vector in dx = [a_1 ... a_dim b]
  if (dx.dim() != dim + 1)
    dx = DoublePoint(dim + 1, 0.0);

  // get maximum coefficient so the numbers don't get too big for double
  Integer d = get_max_coeff(c);
  if (d == 0) d = 1;

  mpq_class q(0);
  for (auto i = 0; i < dim; ++i) {
    q.get_num() = c.coeff(Var(i));
    q.get_den() = d;
    dx[i] = q.get_d();
  }
  // normalize dx before adding the inhomogeneous term
  dx[dim] = 0.0;
  // norm the vector
  double length = sqrt(norm2(dx));
  if (length > 0)
    dx /= length;

  // inhomogenous term
  q.get_num() = c.inhomo_term();
  q.get_den() = d;
  dx[dim] = q.get_d();
  if (length > 0)
    dx[dim] /= length;
}

Affine_Expr
DoublePoint_to_Affine_Expr(const DoublePoint& p) {
  Affine_Expr res;
  auto& le = res.expr;
  le.set_space_dim(p.dim());
  auto& lcm = res.inhomo;
  lcm = 1;

  Integers num(p.dim());
  Integers den(p.dim());

  if (p.dim() > 0) {
    mpq_class q;
    // extract numerators and common denominator
    for (auto i = 0; i < p.dim(); ++i) {
      // For each dimension
      q = mpq_class(p[i]);
      num[i] = q.get_num();
      den[i] = q.get_den();
      lcm_assign(lcm, lcm, den[i]);
    }

    Integer factor;
    for (auto i = 0; i < p.dim(); ++i) {
      exact_div_assign(factor, lcm, den[i]);
      num[i] *= factor;
      le.set(i, num[i]);
    }
  }
  return res;
}

Gen DoublePoint_to_Gen(const DoublePoint& p) {
  Affine_Expr aexpr = DoublePoint_to_Affine_Expr(p);
  return pplite::point(std::move(aexpr.expr), std::move(aexpr.inhomo));
}

Con get_Con_through_DoublePoint(const Linear_Expr& le,
                                const DoublePoint& p,
                                bool strict) {
  // Returns the constraint given by le (sign) le * p,
  // i.e. the plane defined by le that goes through the point p.
  // The (sign) is "<" if strict==true, and "<=" othewise.
  // if le = sum(a_i*x_i), then c:= sum(d*a_i*x_i)-sum(a_i*c_i)==0.
  // Attention: dimension is taken from linex!!!

  // First, convert p into a generator to get at the integer coefficients
  // todo: this can probably be done more efficiently
  Gen g = DoublePoint_to_Gen(p);
  Integer inhomo = 0;
  for (auto i = 0; i < le.space_dim(); ++i)
    inhomo += le.get(i) * g.coeff(Var(i));

  neg_assign(inhomo);
  auto type = strict ? Con::STRICT_INEQUALITY : Con::NONSTRICT_INEQUALITY;
  return Con(g.divisor() * le, inhomo, type);
}

Con get_Con_through_DoublePoint(const DoublePoint& dx,
                                const DoublePoint& p,
                                bool strict) {
  // convert dx into a generator to get at the integer coefficients
  Gen g = DoublePoint_to_Gen(dx);
  Linear_Expr le = g.linear_expr();
  return get_Con_through_DoublePoint(le, p, strict);
}

Con get_Con_through_double(const DoublePoint& dx, double q, bool strict) {
  // find a variable with non-zero coefficient to determine b
  int i = 0;
  while (dx[i] == 0.0 && i < dx.dim())
    ++i;
  if (i == dx.dim()) abort();
  assert(dx[i] != 0.0);
  // dx[i]*x=q	-> x=q/dx[i]
  DoublePoint p(dx.dim(), 0.0);
  p[i] = -q/dx[i];
  return get_Con_through_DoublePoint(dx, p, strict);
}

void get_DoublePoints_center(DoublePoints& dps, DoublePoint& p) {
  // Computes the arithmetic center of the points in pl.
  // Returns a zero point if pl is empty.

  // ATTENTION: the dimension of p must be equal to
  // the dimension of the points in pl
  p = DoublePoint(p.dim(), 0.0); // initialize p to zero

  if (dps.size() == 0)
    return;
  for (const auto& dp : dps)
    p += dp;
  for (int i = 0; i < p.dim(); ++i)
    p[i] /= dps.size();
}

void get_DoublePoints_min_max(const DoublePoints& pl, const DoublePoint& dx,
                              double& min_dx, double& max_dx,
                              DoublePoint& x_min, DoublePoint& x_max) {
  // find the min and max of pl[i]*dx
  bool no_min = true;
  bool no_max = true;
  for (const auto& p : pl) {
    double v = scalar_product(p, dx);
    if (no_min || v < min_dx) {
      min_dx = v;
      no_min = false;
      x_min = p.copy();
    }
    if (no_max || v > max_dx) {
      max_dx = v;
      no_max = false;
      x_max = p.copy();
    }
  }
}

double
get_DoublePoint_angle(const DoublePoint& p1, const DoublePoint& p2) {
  // Computes the angle formed by p1 and p2; theta = a^T b / |a||b|
  const double den = sqrt(norm2(p1)) * sqrt(norm2(p2));
  return (den != 0) ? scalar_product(p1, p2) / den : 1.0;
}

double
get_DoublePoints_angle(const DoublePoints& dps) {
  // Computes the minimum cos(theta) between any two points in dps;
  // returns zero if dps is empty.
  if (dps.empty())
    return 0.0;

  double pmin = 1.0;
  const auto dps_end = dps.end();
  for (auto i = dps.begin(); i != dps_end; ++i) {
    auto j = i;
    for (++j; j != dps_end; ++j) {
      // angle is equal to a^T b / |a||b|
      double den = sqrt(norm2(*i)) * sqrt(norm2(*j));
      double p = (den != 0) ? scalar_product(*i, *j) / den : 1.0;
      if (p < pmin)
        pmin = p;
    }
  }
  return pmin;
}

double
get_DoublePoints_angle_vecs(DoublePoints& pl,
                            DoublePoint& g1, DoublePoint& g2) {
  // Computes the minimum cos(theta) between any two points in pl
  // Returns the two points that have the maximal angle
  // Returns one if pl is empty.
  // ATTENTION: the dimension of p must be equal to the dimension of the points in pl

  double p,n;
  double pmin=1;

  if (pl.size()>1)
    {
      g1=*pl.begin();
      g2=*pl.begin();
      DoublePoints::const_iterator j;
      for (DoublePoints::const_iterator i=pl.begin();i!=pl.end();++i)
        {
          j=i;
          ++j;
          while (j!=pl.end())
            {
              // angle is equal to a^T b / |a||b|
              n=sqrt(norm2(*i))*sqrt(norm2(*j));
              if (n!=0)
                p=scalar_product(*i,*j)/n;
              else
                p=1;

              if (p<pmin)
                {
                  g1=*i;
                  g2=*j;
                  pmin=p;
                };

              ++j;
            };
        };
      //cout << pmin << endl;
      return pmin;
    };

  return 1;
}


