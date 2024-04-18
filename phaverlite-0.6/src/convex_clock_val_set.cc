/* PHAVerLite: PHAVer + PPLite.
   Copyright (C) 2018 Goran Frehse <goranf@gmail.com>
   Copyright (C) 2019-2023 Enea Zaffanella <enea.zaffanella@unipr.it>

This file is part of PHAVerLite.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <phaverlite-config.h>

#include "convex_clock_val_set.hh"
#include "rat_aff_expr.hh"

using namespace pplite;
using namespace pplite::IO_Operators;

#if PHAVERLITE_STATS
Local_Stats bbox_contains_stat("bbox_contains: all calls");
Local_Stats bbox_contains_ph_stat("bbox_contains: ph calls");
#endif

// ----------------------------------------
// Methods for modifying dimensions

void
convex_clock_val_set::dim_shift_assign(dim_type start, dim_type shift) {
  invalidate_bbox();
  auto old_dim = space_dimension();
  ph_ptr->add_space_dims(shift);
  Dims pfunc(old_dim + shift);
  for (dim_type i = 0; i < start; ++i)
    pfunc[i] = i;
  for (dim_type i = start; i < old_dim; ++i)
    pfunc[i] = i + shift;
  for (dim_type i = 0; i < shift; ++i)
    pfunc[old_dim + i] = start + i;
  ph_ptr->map_space_dims(pfunc);
}

void
convex_clock_val_set::dim_swap_assign(dim_type first, dim_type last,
                                      dim_type dst_first) {
  invalidate_bbox();
  auto dim = space_dimension();
  // Safety checks.
  assert(0 <= first && first <= last && last <= dim);
  auto sz = last - first;
  assert(0 <= dst_first && dst_first + sz <= dim);
  // src and dst ranges are disjoint.
  assert((last <= dst_first) || (dst_first + sz <= first));
  PFunction pfunc(dim, PFunction::identity());
  for (auto i = 0; i < sz; ++i) {
    pfunc.info[first + i] = dst_first + i;
    pfunc.info[dst_first + i] = first + i;
  }
  map_space_dimensions(pfunc);
}

void
convex_clock_val_set::pointmirror_assign() {
  invalidate_bbox();
  // maps all points componentwise by x'=-x
  std::vector<Var> vars;
  std::vector<Linear_Expr> exprs;
  auto sd = space_dimension();
  for (auto i = 0; i < sd; ++i) {
    vars.push_back(Var(i));
    exprs.push_back(-Var(i));
  }
  std::vector<Integer> inhomos(sd, 0);
  std::vector<Integer> dens(sd, 1);
  ph_ptr->parallel_affine_image(vars, exprs, inhomos, dens);
}

void
convex_clock_val_set::set_empty() {
  if (has_valid_bbox())
    bbox_ptr->set_empty();
  ph_ptr->set_empty();
}

void
convex_clock_val_set::maybe_compute_bbox() const {
  if (bbox_ptr == nullptr) {
    bbox_ptr.reset(new bbox_type(ph_ptr->get_bounding_box()));
  }
  else
    assert(*bbox_ptr == ph_ptr->get_bounding_box());
}

bool
convex_clock_val_set::relative_bounding_box() {
  if (is_empty())
    return false;
  // Check if *this is already a relative bounding box.
  // That is, check if all non-singular skel constraints are box constraints.
  using namespace pplite;
  minimize();
  // ENEA: FIXME: find a way to avoid copy.
  const auto& con_sys = ph_ptr->copy_cons();
  bool is_rbb = std::all_of(con_sys.begin(), con_sys.end(),
                            [](const Con& c) {
                              return c.is_equality() || is_interval_con(c);
                            });
  if (is_rbb)
    return false;

  // ph is not a relative bounding box: we have to approximate it.
  maybe_compute_bbox();
  const auto& box = *bbox_ptr;
  // Preserve all equality constraints (i.e., preserve affine dim).
  Cons rbb_cs;
  std::copy_if(con_sys.begin(), con_sys.end(),
               std::back_inserter(rbb_cs),
               std::mem_fn(&Con::is_equality));
  const auto sd = space_dimension();
  for (dim_type i = 0; i != sd; ++i) {
    if (box.inf_lb(i) && box.inf_ub(i))
      continue;
    if (!box.inf_lb(i))
      rbb_cs.push_back(Con(box.lb(i).get_den() * Var(i),
                           -box.lb(i).get_num(),
                           Con::NONSTRICT_INEQUALITY));
    if (!box.inf_ub(i))
      rbb_cs.push_back(Con(-box.ub(i).get_den() * Var(i),
                           box.ub(i).get_num(),
                           Con::NONSTRICT_INEQUALITY));
  }

  if (!REACH_CHEAP_CONTAINS_USE_BBOX) {
    // Clear cache to save some space.
    bbox_ptr = nullptr;
  }

  // Build the relative bounding box.
  ph_ptr->set_universe();
  ph_ptr->add_cons(std::move(rbb_cs));
  return true;
}

void
convex_clock_val_set::minimize_memory() {
  minimize();
  convex_clock_val_set tmp(space_dimension());
  tmp.ph_ptr->add_cons(ph_ptr->copy_cons());
  m_swap(tmp);
}

void
convex_clock_val_set::print() const {
  if (has_valid_bbox())
    bbox_ptr->ascii_dump(std::cout);
  std::cout << *this << std::endl;
}

void print_constraints(const convex_clock_val_set& ccvs) {
  ccvs.print();
}

void
convex_clock_val_set::print(const varid_map& vnvec) const {
  minimize();
  // FIXME: avoid copy.
  const auto& cs = ph_ptr->copy_cons();
  auto first = cs.begin(), last = cs.end();
  for (auto i = first; i != last; ++i) {
    if (i != first)
      std::cout << ", ";
    print_constraint(std::cout, *i, vnvec);
  }
}

void
convex_clock_val_set::print_phaver(std::ostream& s,
                                   const varid_map& vnvec) const {
  // FIXME: avoid copy.
  print_phaver_cons(ph_ptr->copy_cons(), s, vnvec);
}

void
convex_clock_val_set::print_gen_fp_raw(std::ostream& os) const {
  DoublePoints dpts;
  add_ccvs_to_DoublePoints(*this, dpts);
  print_fp_raw(os, dpts);
  os << std::endl;
}

void
convex_clock_val_set::print_con_fp_raw(std::ostream& os) const {
  DoublePoints dpts;
  add_ccvs_cons_to_DoublePoints(*this, dpts);
  print_fp_raw(os, dpts);
  os << std::endl;
}

void add_ccvs_to_DoublePoints(const convex_clock_val_set& ccvs,
                              DoublePoints& pl) {
  dim_type dim = ccvs.space_dimension();
  // Add the *skeleton vertices* of ccvs to pl.
  ccvs.minimize();
  // ENEA: FIXME: find a way to ignore non-skel points.
  const auto& gens = ccvs.ph_ptr->copy_gens();
  for (const auto& g : gens) {
    if (g.is_line_or_ray()) continue;
    auto p = Gen_to_DoublePoint(dim, g);
    pl.push_back(p.copy());
  }
}

void DoublePoints_to_ccvs(const DoublePoints& dpts,
                          convex_clock_val_set& ccvs,
                          dim_type dim) {
  // set ccvs to the convex hull of the points in pl
  ccvs = convex_clock_val_set(dim, Spec_Elem::EMPTY);
  for (const auto& dp : dpts) {
    assert(dim == dp.dim());
    ccvs.add_generator(DoublePoint_to_Gen(dp));
  }
}

void
add_ccvs_cons_to_DoublePoints(const convex_clock_val_set& ccvs,
                              DoublePoints& pl) {
  // the points are of the form [a_1 ... a_n b]
  dim_type dim = ccvs.space_dimension();
  DoublePoint dx_zero(dim+1, 0.0);
  DoublePoint dx(dim+1);

  for (const auto& c : ccvs.minimized_constraints()) {
    Con_to_DoublePoint(dim, c, dx);
    pl.push_back(dx.copy());
    if (c.is_equality()) {
      // also add complement
      pl.push_back((dx_zero - dx).copy());
    }
  }
}

bool is_generator_on_constraint(const Gen& g, const Con& c) {
  return pplite::sp::sign(c, g) == 0;
}

convex_clock_val_set
get_cone(const convex_clock_val_set& p) {
  const auto& p_ph = *p.ph_ptr;
  p_ph.minimize();
  assert(!p_ph.is_empty());
  dim_type dim = p_ph.space_dim();
  // Interpret constraints as rays in dim + 1 space.
  convex_clock_val_set ccvs(dim+1, Spec_Elem::EMPTY);
  ccvs.add_generator(point());
  if (dim == 0)
    return ccvs;
  // FIXME: avoid copy.
  for (const auto& c : p_ph.copy_cons()) {
    Linear_Expr e = c.linear_expr();
    e.set(Var(dim), c.inhomo_term());
    ccvs.add_generator(c.is_equality() ? line(e) : ray(e));
  }
  return ccvs;
}

convex_clock_val_set
map_cone(const convex_clock_val_set& cone,
         const TNT::Array2D<Integer>& A,
         const TNT::Array1D<Integer>& b,
         const TNT::Array1D<Integer>& den,
         const Rational& lambda) {
  // Map cone by replacing each constraint (c, c0)
  // with (A^T*c + lambda*c, lambda*c0 + b^T*c - eps)
  dim_type dim = cone.space_dimension();
  assert(dim == A.dim1()+1 && dim == A.dim2()+1);
  assert(dim == b.dim1()+1 && dim == den.dim1()+1);

  convex_clock_val_set ccvs(dim);
  if (dim == 0)
    return ccvs;

  // Get the mapped affine expressions
  std::vector<Rat_Affine_Expr> raes(dim);
  // Homogeneous part.
  for (dim_type i = 0; i < dim - 1; ++i) {
    auto& r_i = raes[i];
    r_i = lambda * Rat_Affine_Expr(Var(i));
    for (dim_type j = 0; j < dim - 1; ++j)
      r_i += Rational(A[j][i], den[j]) * Rat_Affine_Expr(Var(j));
  }
  // Inhomogeneous part.
  Rat_Affine_Expr& r_dim = raes[dim-1];
  r_dim = lambda * Rat_Affine_Expr(Var(dim-1));
  for (dim_type i = 0; i < dim - 1; ++i)
    r_dim += Rational(b[i], den[i]) * Rat_Affine_Expr(Var(i+1));

  // Rebuild constraints, substituting coefficients with raes
  for (const auto& c : cone.minimized_constraints()) {
    // inhomogeneous term remains unchanged
    Rat_Affine_Expr r(c.inhomo_term());
    // reconstruct each constraint,
    // replacing each original coefficient with the map
    for (dim_type i = 0; i < dim; ++i)
      r += Rational(c.coeff(Var(i))) * raes[i];
    assert(r.den > 0);
    ccvs.add_constraint(Con(std::move(r.aexpr.expr),
                            std::move(r.aexpr.inhomo),
                            Con::NONSTRICT_INEQUALITY));
  }
  return ccvs;
}

convex_clock_val_set
get_poly_from_cone(const convex_clock_val_set& cone) {
  // Given a cone in n+1 space,
  // get the n-space constraints derived from the generators of the cone.
  assert(!cone.is_empty() && cone.space_dimension() > 0);
  auto new_dim = cone.space_dimension() - 1;
  convex_clock_val_set ccvs(new_dim);
  if (new_dim == 0)
    return ccvs;

  const auto& cone_ph = *cone.ph_ptr;
  cone_ph.minimize();
  auto& ph = *ccvs.ph_ptr;
  // ENEA: FIXME: find a way to avoid copy.
  for (const auto& g : cone_ph.copy_gens()) {
    assert(g.is_ray() || g.is_line() || g.linear_expr().is_zero());
    Linear_Expr e = g.linear_expr();
    if (e.space_dim() > new_dim)
      e.set_space_dim(new_dim);
    ph.add_con(Con(std::move(e), g.coeff(Var(new_dim)),
                   g.is_line() ? Con::EQUALITY : Con::NONSTRICT_INEQUALITY));
  }
  return ccvs;
}

bool
limit_bitsize(Con& c, const convex_clock_val_set& ccvs, int bits) {
  assert(bits > 0);
  if (max_bitsize(c) <= bits)
    return false;
  if (ccvs.is_empty())
    return false;

  // get the initial scale: m = 2^(bits+1)-2
  Integer m = 1;
  m <<= bits;
  m -= 2;

  // Compute f_inv := 1/f := ceil(c_max / m)
  Integer c_max = get_max_coeff(c);
  assert(c_max > 0);
  Integer f_inv;
  fmpz_cdiv_q(f_inv.impl(), c_max.impl(), m.impl());
  assert(f_inv > 0);

  while (true) {
    Linear_Expr expr;
    bool expr_is_zero = true;
    // get new homogeneous coefficients by scaling
    for (dim_type i = c.space_dim(); i-- > 0; ) {
      Integer new_coeff = c.coeff(Var(i)) / f_inv;
      if (new_coeff != 0) {
        expr_is_zero = false;
        expr.set(i, new_coeff);
      }
    }
    if (expr_is_zero)
      return false;
    // get new inhomogeneous term by minimization
    Rational value;
    bool has_min = ccvs.minimize(Affine_Expr(expr), value);
    if (!has_min)
      return false;

    // get new inhomo term: n = floor(n / d)
    value.round_down();
    auto num = value.get_num();

    if (abs(num) > m) {
      // decrease scaling factor, i.e., increase f_inv
      f_inv *= 2;
      continue;
    }

    neg_assign(num);
    c = Con(expr, num, c.type());
    return true;
  } // while
  return false;
}

int
convex_clock_val_set::get_max_bitsize() const {
  // Note: when computing bitsize, we only care about skel ineqs.
  minimize();
  if (is_empty())
    return 0;
  int res = 0;
#if 0 // ENEA: FIXME: find a way to ignore non-skel cons
  for (const auto& c : ph.impl().cs.sk_rows)
    res = std::max(res, max_bitsize(c));
#else
  for (const auto& c : ph_ptr->copy_cons())
    res = std::max(res, max_bitsize(c));
#endif
  return res;
}

bool
convex_clock_val_set::limit_bits(int max_bits) {
  if (max_bits == 0)
    return false;
  if (get_max_bitsize() <= max_bits)
    return false;
  Cons cs;
  cs.reserve(ph_ptr->num_min_cons());
  bool changed = false;
  // Note: copy of constraints is meant.
  for (auto& c : ph_ptr->copy_cons()) {
    if (c.is_inequality() && limit_bitsize(c, *this, max_bits))
      changed = true;
    cs.push_back(std::move(c));
  }
  if (changed) {
    ph_ptr->set_universe();
    ph_ptr->add_cons(std::move(cs));
    ph_ptr->minimize();
    invalidate_bbox();
  }
  return changed;
}

bool
convex_clock_val_set::limit_cons(int max_cons, int max_bits) {
  const int old_size = constraint_size();
  if (max_cons == 0 || old_size <= max_cons)
    return limit_bits(max_bits);

  minimize();
  Cons cs = ph_ptr->copy_cons();
  const dim_type cs_size = cs.size();

  // just take the first n and add others until it is bounded
  const dim_type sdim = space_dimension();
  ph_ptr->set_universe();

  DoublePoint dp(sdim, 0.0);
  DoublePoints dps;

  TNT::Array2D<double> ang_array(cs_size, cs_size);

  // the candidate list is a set of indexes referring to cs and dps.
  std::set<dim_type> chosen;
  std::set<dim_type> candidates;

  auto preserve_con = [](const Con& c) -> bool {
    if (c.is_equality())
      return true;
    const auto& ex = c.linear_expr();
    const auto num_coeffs = ex.space_dim() - ex.num_zeroes(0, ex.space_dim());
    if (num_coeffs <= 1)
      return true;
    if (num_coeffs > 2)
      return false;
    // Check if c is octagonal.
    auto nonzero = [](const Integer& z) { return !z.is_zero(); };
    auto first = std::find_if(ex.begin(), ex.end(), nonzero);
    assert(first != ex.end());
    auto second = std::find_if(first + 1, ex.end(), nonzero);
    assert(second != ex.end());
    return abs(*first) == abs(*second);
  };

  // Populate dps using the constraints.
  for (auto i = 0; i < cs_size; ++i) {
    const auto& c = cs[i];
    Con_to_DoublePoint(sdim, c, dp);
    dps.push_back(dp.copy());
    // preserve equalities and octagonal constraints
    if (preserve_con(c)) {
      chosen.insert(i);
      ph_ptr->add_con(c);
    } else
      candidates.insert(i);
  }

  // Populate ang_array (note: triangular form).
  for (auto i = 0; i != cs_size; ++i) {
    const auto& dp_i = dps[i];
    ang_array[i][i] = 0.0;
    for (auto j = i + 1; j != cs_size; ++j)
      ang_array[i][j] = get_DoublePoint_angle(dp_i, dps[j]);
  }

  // Parameter for angle comparison:
  // limit within which two angles are considered equal
  const double ang_eps = 0.0; // 1e-6;
  dim_type all_ang_bs = 0;

  dim_type count = chosen.size();
  while (!candidates.empty()
         && (count < max_cons || !ph_ptr->is_bounded())) {
    dim_type min_i = 0;
    double all_ang = 1.0;
    for (auto i : candidates) {
      double max_ang = -1.0;
      for (auto j : chosen) {
        double ang = (j > i) ? ang_array[i][j] : ang_array[j][i];
        max_ang = std::max(ang, max_ang);
      }

      if (max_ang <= all_ang + ang_eps) {
        if (max_ang >= all_ang - ang_eps) {
          // in close cases, chose the one with less bits
          auto ci_bs = max_bitsize(cs[i]);
          if (all_ang_bs == 0) {
            // not calculated yet
            const auto& c_min = cs[min_i];
            all_ang_bs = max_bitsize(c_min);
          }
          if (ci_bs < all_ang_bs) {
            min_i = i;
            all_ang = max_ang;
            all_ang_bs = ci_bs;
          }
        } else {
          min_i = i;
          all_ang = max_ang;
          all_ang_bs = 0;
        }
      }
    }

    // add cs[min_it] to new_ph and erase it from candidates
    if (max_bits == 0)
      ph_ptr->add_con(cs[min_i]);
    else {
      Con c_min = cs[min_i];
      limit_bitsize(c_min, *this, max_bits);
      ph_ptr->add_con(std::move(c_min));
    }
    ++count;
    chosen.insert(min_i);
    candidates.erase(min_i);
  }

  invalidate_bbox();
  return true;
}

/* Note: this is not a proper widening, just an extrapolation operator. */
void
convex_clock_val_set::dual_widen_cone(const convex_clock_val_set& old,
                                      bool force_widening,
                                      unsigned precision) {
  const auto& old_ph = *old.ph_ptr;
  const auto& new_ph = *ph_ptr;
  old_ph.minimize();
  new_ph.minimize();
  auto old_gs = old_ph.copy_gens();
  auto new_gs = new_ph.copy_gens();
  auto old_size = num_rows(old_gs);
  auto new_size = num_rows(new_gs);
  auto sdim = old_ph.space_dim();
  // No need to widen if size is decreasing.
  if (new_size < old_size)
    return;

  // No need to widen if there are fewer lines.
  auto old_lines = std::count_if(old_gs.begin(), old_gs.end(),
                                 std::mem_fn(&Gen::is_line));
  auto new_lines = std::count_if(new_gs.begin(), new_gs.end(),
                                 std::mem_fn(&Gen::is_line));
  assert(new_lines <= old_lines);
  if (new_lines < old_lines)
    return;

  /* Defining helper type aliases and lambdas. */
  using Bools = std::vector<bool>;
  using Indices = std::vector<dim_type>;
  using Doubles = std::vector<double>;
  using FPGens = std::vector<Doubles>;

  auto get_double = [](const Integer& i) { return fmpz_get_d(i.impl()); };

  // computes the norm of vector fpg (floating-point generator).
  auto get_norm = [](const Doubles& fpg) {
    auto norm = 0.0;
    for (auto d : fpg) norm += d * d;
    return std::sqrt(norm);
  };
  // modifies fpg to have norm 1.
  auto normalize_fpg = [get_norm](Doubles& fpg) {
    auto norm = get_norm(fpg);
    for (auto& d : fpg) d /= norm;
  };
  // normalizes all elements of fpgs.
  auto normalize_fpgs = [normalize_fpg](FPGens& fpgs) {
    for (auto& fpg : fpgs) normalize_fpg(fpg);
  };

  // converts g into an fpg of dimension dim.
  auto get_fpg = [get_double](const Gen& g, dim_type dim) {
    Doubles fpg(dim, 0.0);
    for (auto i = g.space_dim(); i--> 0; )
      fpg[i] = get_double(g.linear_expr().get(i));
    return fpg;
  };

  // converts the elements of gs having indices in idx,
  // returning a vector of fpg's of dimension dim.
  auto get_fpgs = [get_fpg](const Gens& gs,
                            const Indices& idx,
                            dim_type dim) {
    FPGens fpgs;
    fpgs.reserve(idx.size());
    for (auto i : idx)
      fpgs.push_back(get_fpg(gs[i], dim));
    return fpgs;
  };

  // computes the square of Euclidean distance.
  auto square_dist = [](const Doubles& ds1, const Doubles& ds2) {
    double dist = 0.0;
    for (auto i = num_rows(ds1); i-- > 0; ) {
      double d = ds1[i] - ds2[i];
      dist += (d * d);
    }
    return dist;
  };

  // returns a vector containing the unstable indices.
  auto get_idx = [](const Bools& stable) {
    Indices idx;
    for (auto i = 0; i != num_rows(stable); ++i) {
      if (!stable[i]) idx.push_back(i);
    }
    return idx;
  };

  auto get_rational = [](double d, int precision) {
    // scale d so as to round it up to `precision' (decimal) digits
    const auto lg10 = std::log(10.0);
    const int scale = precision
    - static_cast<int>(std::ceil(std::log(std::abs(d)) / lg10));
    const double scale_factor = std::pow(10, scale);
    return Rational(static_cast<long>(d * scale_factor),
                    static_cast<long>(scale_factor));
  };

  // old_fpg evolved into new_fpg: update old_fpg.
  auto adjust_ratios
    = [get_rational, precision](const Doubles& old_fpg,
                                const Doubles& new_fpg) {
    auto dim = num_rows(old_fpg);
    Rationals res(dim);
    if (dim <= 1) {
      res[0] = get_rational(new_fpg[0], precision);
      return res;
    }

    Bools seen(dim, false);
    // flag zero & stable coordinates
    for (auto i = 0; i < dim; ++i) {
      if (old_fpg[i] == 0.0 || new_fpg[i] == 0.0
          || old_fpg[i] == new_fpg[i]) {
        res[i] = get_rational(new_fpg[i], precision);
        seen[i] = true;
      }
    }
    // process unstable coordinates
    double old_ratio = 0.0;
    double new_ratio = 0.0;
    for (auto i = 0; i < dim; ++i) {
      if (seen[i]) continue;
      // find another unstable coordinate
      auto j = i + 1;
      if (j == dim) {
        res[i] = get_rational(new_fpg[i], precision);
        continue;
      }
      for ( ; j < dim; ++j) {
        if (seen[j]) continue;
        old_ratio = std::abs(old_fpg[i] / old_fpg[j]);
        new_ratio = std::abs(new_fpg[i] / new_fpg[j]);
        if (old_ratio == new_ratio) {
          // ratio is stable: preserve it (up to precision).
          // FIXME: why up to precision?
          res[i] = get_rational(new_fpg[i], precision);
          res[j] = get_rational(new_fpg[j], precision);
          seen[i] = true;
          seen[j] = true;
        }
        break;
      }
      if (seen[i]) continue;
      // Found pair of indices (i, j) with ratio changing.
      bool increasing = (new_ratio > old_ratio);
      // scale ratio so as to round it up to `precision' (decimal) digits
      const auto lg10 = std::log(10.0);
      const int scale = precision
        - static_cast<int>(std::ceil(std::log(new_ratio) / lg10));
      const double scale_factor = std::pow(10, scale);
      new_ratio *= scale_factor;
      // apply rounding
      new_ratio = increasing ? std::ceil(new_ratio) : std::floor(new_ratio);
      // scale back after rounding
      new_ratio /= scale_factor;
      // Now change old_fpg[i] and old_fpg[j] to have new_ratio.
      res[j] = get_rational(new_fpg[j], precision);
      res[i] = get_rational(new_fpg[j] * new_ratio, precision);
      seen[i] = true;
      seen[j] = true;
    } // end loop on i
    return res;
  };

  // Find indices of stable rays.
  Bools new_stable(new_size, false);
  Bools old_stable(old_size, false);
  for (auto i = 0; i != new_size; ++i) {
    if (!new_gs[i].is_ray())
      new_stable[i] = true;
  }
  for (auto i = 0; i != old_size; ++i) {
    if (!old_gs[i].is_ray()) {
      old_stable[i] = true;
      continue;
    }
    // Search old_g inside (the unstable) new_gs.
    const auto& old_g = old_gs[i];
    for (auto j = 0; j != new_size; ++j) {
      if (new_stable[j])
        continue;
      const auto& new_g = new_gs[j];
      if (old_g == new_g) {
        old_stable[i] = true;
        new_stable[j] = true;
        break;
      }
    }
  }

  if (force_widening) {
    Indices old_idx = get_idx(old_stable);
    pplite::erase_using_sorted_indices(old_gs, old_idx);
    ph_ptr->set_empty();
    ph_ptr->add_gens(std::move(old_gs));
    ph_ptr->minimize();
    invalidate_bbox();
    return;
  }

  Indices old_idx = get_idx(old_stable);
  Indices new_idx = get_idx(new_stable);
  // convert the unstable rays into fp gens having norm 1.
  FPGens old_fpgs = get_fpgs(old_gs, old_idx, sdim);
  FPGens new_fpgs = get_fpgs(new_gs, new_idx, sdim);
  normalize_fpgs(old_fpgs);
  normalize_fpgs(new_fpgs);
  // For each unstable ray in old_fpgs,
  // find a matching unstable ray in new_fpgs;
  // the matching one has minimal (square) distance from it.
  Indices matched_idx;
  FPGens matched_fpgs;
  for (auto i = 0; i != num_rows(old_idx); ++i) {
    auto& old_fpg = old_fpgs[i];
    const auto& new_fpg = new_fpgs[0];
    auto min_j = 0;
    auto min_dist = square_dist(old_fpg, new_fpg);
    for (auto j = 1; j != num_rows(new_idx); ++j) {
      const auto& new_fpg = new_fpgs[j];
      auto dist = square_dist(old_fpg, new_fpg);
      if (dist < min_dist) {
        min_j = j;
        min_dist = dist;
      }
    }
    // Matched old_fpg with new_fpg having index min_j.
    matched_idx.push_back(new_idx[min_j]);
    matched_fpgs.push_back(std::move(new_fpgs[min_j]));
    // Avoid matching again with same generator.
    new_idx.erase(new_idx.begin() + min_j);
    new_fpgs.erase(new_fpgs.begin() + min_j);
  }

  // Now adjust old_gs using matched ones.
  for (auto i = 0, i_end = num_rows(old_idx); i != i_end; ++i) {
    // Adjust ratios in old_fpg according to new_fpg.
    const auto& old_fpg = old_fpgs[i];
    const auto& new_fpg = matched_fpgs[i];
    static int aaa = 0;
    ++aaa;
    std::cerr << "=== Adjusting ratios " << aaa << " ===\n";
    Rationals rats = adjust_ratios(old_fpg, new_fpg);
    Integer den = 1;
    for (auto i = num_rows(rats); i-- > 0; )
      den *= rats[i].get_den();
    Linear_Expr expr;
    for (auto i = num_rows(rats); i-- > 0; ) {
      auto coeff = rats[i].get_num() * den / rats[i].get_den();
      add_mul_assign(expr, coeff, Var(i));
    }
    Gen new_ray = ray(std::move(expr));
    if (ph_ptr->relation_with(new_ray) == Poly_Gen_Rel::nothing()) {
      // Do something.
      std::cerr << "\n=== skipping ray\n";
      continue;
    }
    // Now modify old_g according to modified ratios.
    auto& old_g = old_gs[old_idx[i]];
    old_g = std::move(new_ray);
  }

  ph_ptr->set_empty();
  ph_ptr->add_gens(std::move(old_gs));
  ph_ptr->minimize();
  invalidate_bbox();
}
