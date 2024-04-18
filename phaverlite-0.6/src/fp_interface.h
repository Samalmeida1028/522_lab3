#ifndef GUARD_fp_interface_h
#define GUARD_fp_interface_h

#include "extended_pplite.hh"
#include "tnt.h"
#include "derivify.h"

#include <iostream>
#include <list>

double Round(double Zahl, int Stellen);

// Convex Sorting

template <class T>
bool is_counterclockwise_order(const TNT::Array1D<T>& A,
                               const TNT::Array1D<T>& B,
                               const TNT::Array1D<T>& C) {
  // Returns whether the points given by 2-dimensional arrays A,B,C
  // are in counterclockwise order
  if (A.dim()==2 && B.dim()==2 && C.dim()==2)
    return A[0]*B[1]-A[1]*B[0]+A[1]*C[0]-A[0]*C[1]+B[0]*C[1]-C[0]*B[1] >= 0;
  throw_error("wrong dimension in is_counterclockwise_order");
  return false;
}

template <class T>
bool compare_second_coord(const TNT::Array1D<T>& p1,
                          const TNT::Array1D<T>& p2) {
  return p1[1] <= p2[1];
}

template <class T>
void sort_second_coord(std::vector<TNT::Array1D<T>> &l) {
  if (l.size() > 1) {
    auto i = l.begin(), j = l.begin(), m = l.end();
    --m;
    TNT::Array1D<T> dum;
    while (m != l.begin()) {
      for (i=l.begin(); i!=m; ++i) {
        j=i;
        ++j;
        if (!compare_second_coord(*i,*j)) {
          // swap
          dum=(*i).copy();
          *i=(*j).copy();
          *j=dum.copy();
        }
      }
      --m;
    }
    // Eliminate doubles
    i=l.begin();
    while (i!=l.end()) {
      j=i;
      while (j!=l.end()) {
        if (i!=j && static_cast<const T*>(*i)==static_cast<const T*>(*j))
          j=l.erase(j);
        else
          ++j;
      }
      ++i;
    }
  }
}

template <class T>
void sort_counterclockwise(std::vector<TNT::Array1D<T>> &l) {
  // ATTENTION: there must not be doubles!
  if (l.size() > 2) {
    sort_second_coord(l);
    auto i=l.begin(), i2=l.begin(), j=l.begin(), m=l.end();
    ++j;
    --m;
    --m;
    TNT::Array1D<T> dum;
    while (m!=l.begin()) {
      for (i=l.begin(); i!=m; ++i) {
        j=i;
        ++j;
        i2=j;
        ++i2;
        if (!is_counterclockwise_order(*i,*j,*i2)) {
          // swap
          dum=(*i).copy();
          *i=(*j).copy();
          *j=dum.copy();
        }
      }
      --m;
    }
  }
}

template <class T>
void print_fp_raw(std::ostream& o, const TNT::Array1D<T>& A) {
  for (int i=0;i<A.dim1();++i)
    o << A[i] << " ";
  o << std::endl;
}

template <class T>
void print_fp_raw(std::ostream& o, std::vector<TNT::Array1D<T>>& l) {
  // if it is two-dimensional, sort it so it's counter-clockwise
  if (!l.empty() && l.begin()->dim()==2) {
    sort_counterclockwise(l);
    // close the curve
    if (l.size()>2)
      l.push_back(l.begin()->copy());
  }
  for (const auto& e : l)
    print_fp_raw(o, e);
}

// Matrix Operations
template <class T>
T scalar_product(const TNT::Array1D<T> &A, const TNT::Array1D<T> &B) {
  // Scalar Prodcut of 2 Vectors, i.e., sum_i(A[i]*B[i])
  int n = A.dim1();
  int m = B.dim1();

  if (n != m || n<=0 )
    return T(0);

  T c = A[0]*B[0];
  for (int i=1; i<n; i++)
    c += A[i]*B[i];
  return c;
}

template <class T>
TNT::Array1D<T>
operator*(const T &B,const TNT::Array1D<T> &A) {
  // Scalar*Vector
  int n = A.dim1();
  TNT::Array1D<T> C(n);

  for (int i=0; i<n; i++)
    C[i] = B*A[i];
  return C;
}

template <class T>
TNT::Array1D<T>
operator*(const TNT::Array2D<T> &A, const TNT::Array1D<T> &B) {
  // Matrix * Vector
  int n = A.dim1();
  int m = A.dim2();

  if (B.dim1() != m || n<=0 || m<=0)
    return TNT::Array1D<T>();

  TNT::Array1D<T> C(n);
  for (int i=0; i<n; i++) {
    C[i]=A[i][0]*B[0];
    for (int j=1; j<m; j++)
      C[i] += A[i][j] * B[j];
  }
  return C;
}

template <class T>
bool operator==(const TNT::Array1D<T> &A, const TNT::Array1D<T> &B) {
  // Scalar Prodcut of 2 Vectors, i.e., sum_i(A[i]*B[i])
  int n = A.dim1();
  int m = B.dim1();

  if (n != m || n<=0 )
    return false;

  for (int i=0; i<n; i++) {
    if (!(A[i]==B[i]))
      return false;
  }
  return true;
}

template <class T>
T norm2(const TNT::Array1D<T> &A) {
  // return |A^T*A|^2=A^T*A
  int n = A.dim1();
  T c = 0;
  if (n > 0) {
    for (int i=0; i<n; i++)
      c += A[i]*A[i];
  }
  return c;
}

// Conversion Methods

using DoublePoint = TNT::Array1D<double>;
using DoublePoints = std::vector<DoublePoint>;

double Gen_to_double(const Gen& g, dim_type pos);
DoublePoint Gen_to_DoublePoint(dim_type dim, const Gen& g);

Affine_Expr DoublePoint_to_Affine_Expr(const DoublePoint& p);

Gen DoublePoint_to_Gen(const DoublePoint& p);

void Con_to_DoublePoint(dim_type dim, const Con& c, DoublePoint& dx);

// Computation Methods

Con get_Con_through_DoublePoint(const Linear_Expr& le,
                                const DoublePoint& p, bool strict = false);
Con get_Con_through_DoublePoint(const DoublePoint& dx,
                                const DoublePoint& p, bool strict = false);
Con get_Con_through_double(const DoublePoint& dx,
                           double q, bool strict = false);

void get_DoublePoints_center(DoublePoints& pl, DoublePoint& p);

void get_DoublePoints_min_max(const DoublePoints& pl,
                              const DoublePoint& dx,
                              double& min_dx,
                              double& max_dx,
                              DoublePoint& x_min,
                              DoublePoint& x_max);

double get_DoublePoint_angle(const DoublePoint& p1, const DoublePoint& p2);

double get_DoublePoints_angle(const DoublePoints& pl);

double get_DoublePoints_angle_vecs(DoublePoints& pl,
                                   DoublePoint& g1,
                                   DoublePoint& g2);

#endif
