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

#include "linear_vtef.hh"

linear_vtef::linear_vtef(const Cons& tp, dim_type dim)
  : mydim(dim), mytp(tp) {
  // add the extra dimensions to the variables that must be quantified
  dim_type mytp_dim = max_space_dim(tp);
  for (dim_type i = dim; i < mytp_dim; ++i)
    vars.insert(i);

  static_tp = convex_clock_val_set(dim);
  // transfer static constraints to static_tp
  for (auto i = mytp.begin(); i != mytp.end(); ) {
    if (get_true_dimension(*i) > dim) {
      ++i;
      continue;
    }
    static_tp.add_constraint(Con_restrict_to_dim(*i, dim));
    i = mytp.erase(i);
  }
}

void
linear_vtef::intersection_assign(const linear_vtef& lv) {
  if (mydim != lv.mydim)
    throw_error("Dimensions incompatible at linear_vtef::intersection_assign");
  if (static_tp.space_dimension() != lv.static_tp.space_dimension())
    throw_error("Incompatible dimension static_tp "
                + int2string(static_tp.space_dimension())
                + " vs " + int2string(lv.static_tp.space_dimension()));
  vars.union_assign(lv.vars);
  mytp.insert(mytp.end(), lv.mytp.begin(), lv.mytp.end());
  static_tp.intersection_assign(lv.static_tp);
}

void
linear_vtef::add_space_dimensions(dim_type ndims) {
  if (ndims == 0)
    return;
  static_tp.add_space_dimensions(ndims);
  // shift up the quantified variables
  var_ref_set newvars;
  for (auto i : vars)
    newvars.insert(ndims + i);
  vars = std::move(newvars);
  // shift up the quantified variables in mytp
  for (auto& c : mytp)
    c.shift_space_dims(mydim, ndims);
  // update mydim
  mydim += ndims;
}

void
linear_vtef::map_space_dimensions(const PFunction& pfunc) {
  dim_type newdim = pfunc.codomain_space_dim();
  static_tp.map_space_dimensions(pfunc);
  // double pfunc, since the variable coefficients must be changed, too
  PFunction pfunc2 = double_PFunction(pfunc, mydim);
  var_ref_set newvars;
  for (auto v : vars)
    newvars.insert(pfunc2.get_map(v));
  mydim = newdim;
  vars = std::move(newvars);
  permute_space_dims(mytp, pfunc2);
}

void
linear_vtef::print() const {
  using std::cout;
  using std::endl;
  using namespace pplite::IO_Operators;
  cout << "Dimension: " << mydim << endl;
  cout << "Dynamic: " << mytp << endl;
  if (!static_tp.is_universe())
    cout << "Static: " << static_tp << endl;
  else
    cout << "Static: none" << endl;
}

convex_clock_val_set
linear_vtef::time_post(const clock_val_set& inv) const {
  // check for emptiness
  if (inv.is_empty())
    return convex_clock_val_set(mydim, Spec_Elem::EMPTY);

  // From now on we know that inv is not empty
  convex_clock_val_set ccvs(mydim);

  if (REFINE_DERIVATIVE_METHOD == 0
      || REFINE_DERIVATIVE_METHOD == 1) {
    // push constraints to outsidex
    Con c1, c2;
    bool active1, active2;
    for (const auto& c : mytp) {
      // push c towards the outside
      minimize_constraint_from_dim(c, inv, mydim,
                                   c1, active1, c2, active2);
      if (active1)
        ccvs.add_constraint_and_minimize(c1);
      if (active2)
        ccvs.add_constraint_and_minimize(c2);
    };
    //cout << "in: " << inv << endl;
    //cout << "tp: " << ccvs << endl;
  }

  if (REFINE_DERIVATIVE_METHOD == 2
      || REFINE_DERIVATIVE_METHOD == 3
      || REFINE_DERIVATIVE_METHOD == 4) {
    // exact
    clock_val_set cvs(inv);
    cvs.add_space_dimensions_before(mydim);
    cvs.add_constraints(mytp);
    //cout << "before static: " << cvs;
    clock_val_set cvs2(static_tp);
    //cout << "static: " << static_tp;
    cvs.intersection_assign_adapt_dim(cvs2);
    //cout << "after static: " << cvs;
    cvs.remove_space_dimensions(mydim, 2*mydim);
    ccvs=cvs.get_convex_hull();
  }

  if (REFINE_DERIVATIVE_METHOD == 1
      || REFINE_DERIVATIVE_METHOD == 3) {
    // bounding box of 1 or 2
    // use bounding box
    ccvs.relative_bounding_box();
  }

  if (REFINE_DERIVATIVE_METHOD == 6) {
    // vertice based
    // ATTENTION: incomplete and obsolete
    assert(false);
    abort();
  }

  // ENEA, FIXME: unconditional?
  // ccvs.limit_significant_bits(CONSTRAINT_BITSIZE);
  if (!static_tp.is_universe())
    ccvs.intersection_assign_and_minimize(static_tp);
  return ccvs;
}

void
linear_vtef::get_linear_system_matrices(TNT::Array2D<Integer>& A,
                                        TNT::Array1D<Integer>& b,
                                        TNT::Array1D<Integer>& den) const {
  // reset matrices.
  A = TNT::Array2D<Integer>(mydim, mydim, Integer(0));
  b = TNT::Array1D<Integer>(mydim, Integer(0));
  den = TNT::Array1D<Integer>(mydim, Integer(0));

  std::vector<bool> found(mydim, false);
  // Go through the coefficients of mytp
  // to get the coefficients of A, b and den.
  for (const auto& c : mytp) {
    // Note: constraints define dynamics as in c dx/dt = A x + b
    // Need to have an equality.
    if (!c.is_equality())
      throw_error("linear_vtef: constraint is not an equality");
    dim_type c_dim = c.space_dim();
    assert(0 < c_dim && c_dim <= 2*mydim);
    // search for a single dotted var in [0, mydim).
    dim_type i = 0;
    const dim_type i_end = std::min(mydim, c_dim);
    for ( ; i != i_end; ++i) {
      if (c.coeff(Var(i)) != 0)
        break;
    }
    // Need to have one and only one dotted variable.
    if (!(i < mydim && c.linear_expr().all_zeroes(i+1, i_end)))
      throw_error("linear_vtef: constraint mentions "
                  "more than a single dotted variable");
    // Need to be different from those found before.
    if (found[i])
      throw_error("linear_vtef: dotted variable "
                  "defined in two or more equations");
    found[i] = true;
    den[i] = c.coeff(Var(i));
    for (dim_type j = 0; j < i_end; ++j) {
      A[i][j] = -c.coeff(Var(j + mydim));
      b[i] = c.inhomo_term();
    }
  }
  // check that all rows were found
  if (std::find(found.begin(), found.end(), false) != found.end())
    throw_error("linear_vtef: some dotted variables are unconstrained");
}

size_t
linear_vtef::get_memory() const {
  size_t m = 0;
  m += 1 + vars.size();
  m += static_tp.get_memory()
    + pplite::total_memory_in_bytes(mytp);
  return m;
}
