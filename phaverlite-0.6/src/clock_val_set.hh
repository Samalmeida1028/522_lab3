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

#ifndef GUARD_clock_val_set_hh
#define GUARD_clock_val_set_hh

#include "parameters.hh"
#include "convex_clock_val_set.hh"

#include <pplite/pplite.hh>

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>

// clock_val_set: disjunction of convex_clock_val_set
struct clock_val_set {
  using List = std::list<convex_clock_val_set>;
  using List_Iter = List::iterator;
  using List_CIter = List::const_iterator;

  List ccvs_list; // list of convex clock valuation sets
  dim_type dim; // dimension of the clock valuation space

  clock_val_set() noexcept : dim(pplite::not_a_dim()) {}
  explicit clock_val_set(dim_type d, Spec_Elem kind = Spec_Elem::UNIVERSE)
    : dim(d) {
    ccvs_list.emplace_back(d, kind);
  }
  explicit clock_val_set(convex_clock_val_set ccvs)
    : dim(ccvs.space_dimension()) {
      ccvs_list.push_back(std::move(ccvs));
  }

  void m_swap(clock_val_set& y) noexcept {
    using std::swap;
    swap(ccvs_list, y.ccvs_list);
    swap(dim, y.dim);
  }

  size_t get_memory() const;
  bool is_empty() const;
  size_t size() const;
  dim_type get_real_dimension() const;

  // ----------------------------------------
  // Methods for modifying dimensions

  void map_space_dimensions(const PFunction& pfunc) {
    for (auto& ccvs : ccvs_list)
      ccvs.map_space_dimensions(pfunc);
    dim = ccvs_list.front().space_dimension(); // might be safer
  }

  void dim_swap_assign(dim_type first, dim_type last, dim_type dst_first);

  void add_space_dimensions(dim_type m);
  void add_space_dimensions_before(dim_type m);

  // Note: last *not* included.
  void remove_space_dimensions(dim_type first, dim_type last);
  void remove_space_dimensions(const var_ref_set& to_be_removed);
  void remove_higher_space_dimensions(dim_type new_dim);

  void concatenate_assign(const clock_val_set& cvs);

  void add_constraint(const Con& c);
  void add_constraints(const Cons& cs);

  void union_assign(const convex_clock_val_set& ccvs);
  void union_assign(const clock_val_set& cvs);
  void intersection_assign_from(const clock_val_set& cvs,
                                const clock_val_set& cvs2);
  void intersection_assign(const clock_val_set& cvs);
  void intersection_assign_adapt_dim(const clock_val_set& cvs);

  convex_clock_val_set get_convex_hull() const;
  void convex_hull_assign();
  void convex_hull_assign(const clock_val_set& cvs);
  void constraint_hull_assign(const clock_val_set& cvs);

  void remove_redundant();
  void remove_empty();

  void join_convex_ccvs();
  void minimize_constraints();
  void minimize_memory();
  void simplify();

  void old_difference_assign(const convex_clock_val_set& ccvs);
  void split_based_difference_assign(const convex_clock_val_set& ccvs);
  void difference_assign(const convex_clock_val_set& ccvs);

  void difference_assign(const clock_val_set& cvs);
  bool difference_is_empty(const convex_clock_val_set& ccvs);

  void negate();

  bool is_disjoint() const;
  bool is_disjoint_from(const clock_val_set& cvs) const;

  bool is_split_by(const Con& con) const;
  clock_val_set split(const Con& con, Topol topol);

  bool cheap_contains(const clock_val_set& cvs) const;
  bool contains(const clock_val_set& cvs) const;
  bool contains_return_others(clock_val_set& cvs) const;

  void time_elapse_assign(const convex_clock_val_set &tp);
  void time_elapse_assign(const convex_clock_val_set &tp,
                          const clock_val_set& inv);

  void topological_closure_assign();

  convex_clock_val_set append(const convex_clock_val_set& ccvs);

  clock_val_set append(const clock_val_set &cvs);

  clock_val_set disj_union_assign(const convex_clock_val_set &ccvs);
  clock_val_set disj_union_assign(const clock_val_set &cvs);

  bool operator==(const clock_val_set& cvs) const {
    if (size() == 1 && cvs.size() == 1)
      return ccvs_list.front().equals(cvs.ccvs_list.front());
    return contains(cvs) && cvs.contains(*this);
  }

  void set_empty() { *this = clock_val_set(dim, Spec_Elem::EMPTY); }

  // linear programming
  bool maximize(const Affine_Expr &expr, Rational& value,
                bool* included_ptr = nullptr) const;
  bool minimize(const Affine_Expr &expr, Rational& value,
                bool* included_ptr = nullptr) const;

  int max_constraint_size() const;
  int constraint_size() const;
  int get_max_bitsize() const;

  bool limit_bits(int max_bits);
  bool limit_cons(int max_cons, int max_bits);
  bool limit_cons_or_bits(int max_cons, int max_bits);

  // Assigns to each ccvs its relative bounding box.
  // Returns true if something has changed.
  bool relative_bounding_boxes();

  void print(std::ostream& os = std::cout) const;
  void print(const varid_map& vnvec) const;
  void print_phaver(std::ostream& s, const varid_map& vnvec) const;
  void print_gen_fp_raw(std::ostream& s) const;
  void print_con_fp_raw(std::ostream& s) const;
}; // clock_val_set

NOTHROW_DEFAULT_AND_MOVES(clock_val_set);

inline void
swap(clock_val_set& x, clock_val_set& y) noexcept {
  x.m_swap(y);
}

std::ostream& operator<<( std::ostream& os, const clock_val_set &c );

clock_val_set zero_cvs(const dim_type& dim);

void print_constraints(const clock_val_set& cvs);

// -------------------------


clock_val_set clock_val_set_complement(dim_type dim, const Con& cs);

clock_val_set complement(const convex_clock_val_set& ccvs);

clock_val_set complement(const clock_val_set& cvs);

// -------------------------

clock_val_set
var_ref_sets_to_clock_val_set(const var_ref_set& crs,
                              dim_type dim,
                              const var_ref_set& vars);

void add_cvs_to_DoublePoints(const clock_val_set& cvs,
                             DoublePoints& pl);

bool is_time_relevant(const Con& cs, const convex_clock_val_set& tp);

clock_val_set closed_faces(const convex_clock_val_set& ccvs);
clock_val_set closed_faces(const clock_val_set& cvs);

clock_val_set
cvs_adj_faces(const convex_clock_val_set& cI, const clock_val_set& E);

void
minimize_constraint_from_dim(const Con& c, const clock_val_set& cvs,
                             dim_type start_dim,
                             Con& c1, bool& active1,
                             Con& c2, bool& active2);

inline int
max_bitsize(const clock_val_set& cvs) {
  return cvs.get_max_bitsize();
}

inline int
max_consize(const clock_val_set& cvs) {
  return cvs.max_constraint_size();
}

#endif // GUARD_clock_val_set_hh
