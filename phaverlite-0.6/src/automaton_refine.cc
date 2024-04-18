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

#include "automaton.hh"
#include "stopwatch.hh"
#include "deriv_def.h"
#include "deriv_ops.h"

void get_loc_vert(automaton& aut, loc_ref loc, DoublePoints& pl) {
  pl.clear();
  add_cvs_to_DoublePoints(aut.locations[loc].invariant(), pl);
}

void get_loc_deriv(automaton& aut, loc_ref loc,
                   DoublePoints& vert_list,
                   DoublePoints& deriv_list) {
  // get the derivatives in the vertices in vert_list
  dim_type dim = aut.locations[loc].invariant().dim;
  deriv_list.clear();

  if (REFINE_USE_FP) {
    DoublePoint p(dim, 0.0);
    for (auto& vert : vert_list) {
      get_derivative(aut.name, vert, p);
      deriv_list.push_back(p.copy());
    }
  } else
    add_ccvs_to_DoublePoints(aut.locations[loc].time_post_poly(),
                             deriv_list);
}

void get_loc_deriv_nonfp(automaton& aut, loc_ref loc,
                         const clock_val_set& restr,
                         DoublePoints& deriv_list) {
  add_ccvs_to_DoublePoints(aut.locations[loc].time_post_poly(restr),
                           deriv_list);
}

Cons get_deriv_over_states(automaton& aut, loc_ref loc) {
  // for each ccvs of the invariant, get the dynamics depending on x
  if (REFINE_USE_FP) {
    // for each of the vertices, get the state as well as the derivative
    // and add this to the polyhedron
    convex_clock_val_set ccvs(2*aut.dim, Spec_Elem::EMPTY);
    DoublePoints pl, dl;
    get_loc_vert(aut, loc, pl);
    get_loc_deriv(aut, loc, pl, dl);

    // transform each point in pl, dl to a generator
    DoublePoint p(2*aut.dim);
    auto dj_iter = dl.begin();
    for (const auto& pi : pl) {
      p = append(*dj_iter, pi);
      ++dj_iter;
      ccvs.add_generator(DoublePoint_to_Gen(p));
    }
    return ccvs.constraints();
  }
  else
    return aut.locations[loc].get_mylinvtef_tp();
}


void set_loc_deriv(automaton& aut, loc_ref loc,
                   DoublePoints& deriv_list) {
  // set the derivatives to the vertices in vert_list
  dim_type dim = aut.locations[loc].invariant().dim;

  if (REFINE_DERIVATIVE_METHOD != 1) {
    convex_clock_val_set ccvs(dim);
    DoublePoints_to_ccvs(deriv_list, ccvs, dim);
    aut.locations[loc].time_post_poly_assign(ccvs);
  } else {
    convex_clock_val_set ccvs(dim);
    DoublePoints_to_ccvs(deriv_list, ccvs, dim);
    // ENEA: FIXME: CHECKME using *relative* bounding box
    ccvs.relative_bounding_box();
    aut.locations[loc].time_post_poly_assign(ccvs);
  }
}

convex_clock_val_set
get_loc_deriv(automaton& aut, loc_ref loc) {
  // recompute and assign the derivative from the vertices
  DoublePoints pl,dl;
  get_loc_vert(aut, loc, pl);
  get_loc_deriv(aut, loc, pl, dl);

  dim_type dim = aut.locations[loc].invariant().dim;
  convex_clock_val_set ccvs(dim);
  if (REFINE_DERIVATIVE_METHOD == 0) {
    // convex hull
    DoublePoints_to_ccvs(dl, ccvs, dim);
    return ccvs;
  } else if (REFINE_DERIVATIVE_METHOD == 1) {
    // bounding box
    DoublePoints_to_ccvs(dl, ccvs, dim);
    // ENEA: FIXME: CHECKME: using *relative* bounding box
    ccvs.relative_bounding_box();
    return ccvs;
  } else {
    assert(REFINE_DERIVATIVE_METHOD == 2);
    // center
    DoublePoint p(dim, 0.0);
    get_DoublePoints_center(dl, p);
    dl.clear();
    dl.push_back(p);
    DoublePoints_to_ccvs(dl, ccvs, dim);
    return ccvs;
  }
}

convex_clock_val_set
get_loc_deriv(automaton& aut, loc_ref loc, const clock_val_set& restr) {
  // recompute and assign the derivative from the vertices
  DoublePoints pl, dl;

  if (REFINE_USE_FP) {
    add_cvs_to_DoublePoints(restr, pl);
    get_loc_deriv(aut, loc, pl, dl);
  } else
    get_loc_deriv_nonfp(aut, loc, restr, dl);

  dim_type dim = aut.locations[loc].invariant().dim;
  if (REFINE_DERIVATIVE_METHOD == 0) {
    convex_clock_val_set ccvs(dim);
    DoublePoints_to_ccvs(dl, ccvs, dim);
    return ccvs;
  } else {
    convex_clock_val_set ccvs(dim);
    DoublePoints_to_ccvs(dl, ccvs, dim);
    // ENEA: FIXME: CHECKME: using *relative* bounding box
    ccvs.relative_bounding_box();
    return ccvs;
  }
}

void refine_loc_deriv(automaton& aut, loc_ref loc) {
  DoublePoints pl, dl;
  get_loc_vert(aut, loc, pl);
  get_loc_deriv(aut, loc, pl, dl);
  set_loc_deriv(aut, loc, dl);
}

void refine_loc_deriv(automaton& aut, loc_ref_list& locs) {
  for (auto loc : locs)
    refine_loc_deriv(aut, loc);
}

void refine_loc_deriv(automaton& aut) {
  stopwatch sw(2100,"refine_loc_deriv");
  message(2100, "Refining derivative of " + aut.name + ".");
  for (dim_type i = 0; i < num_rows(aut.locations); ++i)
    refine_loc_deriv(aut, i);
}

double get_loc_angle(automaton& aut, loc_ref loc) {
  // compute the angle spanned by the derivative in the vertices
  DoublePoints pl, dl;
  get_loc_vert(aut,loc,pl);
  get_loc_deriv(aut,loc,pl,dl);
  return get_DoublePoints_angle(dl);
}

double get_loc_angle(automaton& aut, loc_ref loc, clock_val_set& restr) {
  // compute the angle spanned by the derivative in the vertices of restr
  DoublePoints pl, dl;
  if (REFINE_USE_FP) {
    add_cvs_to_DoublePoints(restr, pl);
    get_loc_deriv(aut, loc, pl, dl);
  } else
    get_loc_deriv_nonfp(aut, loc, restr, dl);
  return get_DoublePoints_angle(dl);
}

// Methods for splitting locations

void split_location_new(automaton& aut, label_ref silent_label,
                        loc_ref ploc, Con pos_con, loc_ref_list& new_locs) {
  // split location `ploc' using a closed, rational partition
  // based on constraint `pos_con'.
  // add the newly added locations to the list `new_locs'
  if (pos_con.is_inconsistent())
    return;

  // ENEA: FIXME: move this in the caller?
  if (pos_con.is_equality()) // simply convert it into a non-strict inequality
    pos_con = constraint_to_nonstrict_inequality(pos_con);

  // On naming: ploc and nloc are the location *indexes*
  // for the positive/negative locations resulting from the split.
  // pos_loc and neg_loc are the location *objects*.

  // neg_loc will be pushed back into locations
  auto nloc = num_rows(aut.locations);

  { // Note: scoping needed.
    // This pos_loc reference will be invalidated by the following
    // push_back operation; so, it will be recomputed later on.
    auto& pos_loc = aut.locations[ploc];
    if (pos_loc.invariant().is_empty())
      return;

    // FIXME: no update of names' map?
    std::string pos_suffix = "@" + int2string(nloc) + "+";
    auto neg_loc = pos_loc.split_clone(pos_con, pos_suffix);
    // add neg_loc to aut.locations: this invalidates pos_loc.
    aut.locations.push_back(std::move(neg_loc));
  }

  // have to recompute references
  auto& pos_loc = aut.locations[ploc];
  auto& neg_loc = aut.locations[nloc];

  // NOTE: same call to ploc *has* to be delayed.
  // We do not delay this too, as it could affect efficiency negatively.
  aut.assume_loc_modification(nloc);

  assert(aut.loc_name_to_loc_ref_map.find(neg_loc.name)
         == aut.loc_name_to_loc_ref_map.end());

  // copy all transitions from ploc to nloc
  // incoming transitions
  for (auto t : pos_loc.in_trans) {
    const auto& trx = aut.transitions[t];
    auto sloc = trx.source_loc();
    if (sloc == ploc)
      continue; // don't do self-loops
    if (trx.label() == silent_label) {
      // for silent labels introduced by refinement
      // (which implement an identity post)
      // check if new invariant and source invariant are disjunct
      clock_val_set cvs;
      cvs.intersection_assign_from(neg_loc.invariant(),
                                   aut.locations[sloc].invariant());
      if (cvs.is_empty())
        continue;
      // only copy it if it's at least a hyperplane
      // (otherwise it's redundant with one of the hyperplanes)
      if (REFINE_CHECK_TRANS_DIMS && cvs.get_real_dimension() < cvs.dim-1)
        continue;
    }
    auto mu = trx.unrestricted_mu();
    aut.add_transition(sloc, trx.label(), nloc, std::move(mu), trx.urgency());
  }

  // outgoing transitions
  for (auto t : pos_loc.out_trans) {
    const auto& trx = aut.transitions[t];
    auto tloc = trx.target_loc();
    if (tloc == ploc)
      continue; // don't do self-loops
    // check if new invariant and incoming invariant are disjunct
    if (trx.label() == silent_label) {
      // for silent labels introduced by refinement
      // (which implement an identity post)
      // check if new invariant and target invariant are disjunct
      clock_val_set cvs;
      cvs.intersection_assign_from(neg_loc.invariant(),
                                   aut.locations[tloc].invariant());
      if (cvs.is_empty())
        continue;
      // only copy it if it's at least a hyperplane
      // (otherwise it's redundant with one of the hyperplanes)
      if (REFINE_CHECK_TRANS_DIMS && cvs.get_real_dimension() < cvs.dim-1)
        continue;
    }
    auto mu = trx.unrestricted_mu();
    aut.add_transition(nloc, trx.label(), tloc, std::move(mu), trx.urgency());
  }

  // Make a copy of self-loops,
  // because they're not covered by either of the previous cases
  for (auto t : pos_loc.out_trans) {
    const auto& trx = aut.transitions[t];
    if (trx.target_loc() != ploc)
      continue; // not a self loop
    auto trx_label = trx.label();
    auto trx_urgency = trx.urgency();
    auto mu = trx.unrestricted_mu();
    aut.add_transition(nloc, trx_label, nloc, mu, trx_urgency); // copy mu
    aut.add_transition(ploc, trx_label, nloc, mu, trx_urgency); // copy mu
    aut.add_transition(nloc, trx_label, ploc,
                       std::move(mu), trx_urgency); // move mu
  }

  new_locs.push_back(nloc);
  // Now it is safe to perform this call.
  aut.assume_loc_modification(ploc);

  // to fix incoming and outgoing transitions
  if (REFINE_CHECK_TRANS_DIMS)
    aut.location_remove_nonface_silents(ploc, silent_label);

  // ----------------------------------------
  // add new transitions that connect the two
  // ----------------------------------------
  dim_type dim = pos_loc.invariant().dim;
  Cons mu = identity_trans(dim);
  mu.push_back(constraint_to_equality(pos_con));
  // test of transition is time relevant
  if (REFINE_CHECK_TIME_RELEVANCE) {
    // already existing transitions
    if (REFINE_CHECK_TIME_RELEVANCE_DURING) {
      aut.location_remove_nontimerel_silents(ploc, silent_label);
      aut.location_remove_nontimerel_silents(nloc, silent_label);
    }
    // test transitions that will connect the old and new location
    // note: in pratice, one test should suffice?
    if (REFINE_USE_FP && !pos_loc.is_time_post_poly_uptodate()) {
      convex_clock_val_set ccvs(get_loc_deriv(aut, ploc));
      if (aut.reversed) ccvs.pointmirror_assign();
      tp_limit_cons_or_bits(ccvs);
      pos_loc.time_post_poly_assign(ccvs);
    }
    if (REFINE_USE_FP && !neg_loc.is_time_post_poly_uptodate()) {
      convex_clock_val_set ccvs(get_loc_deriv(aut, nloc));
      if (aut.reversed) ccvs.pointmirror_assign();
      tp_limit_cons_or_bits(ccvs);
      neg_loc.time_post_poly_assign(ccvs);
    }

    const auto& pos_tpp = pos_loc.time_post_poly();
    const auto& neg_tpp = neg_loc.time_post_poly();
    Con neg_con = closed_inequality_complement(pos_con);
    bool pcon_rel_ploc = is_time_relevant(pos_con, pos_tpp);
    bool pcon_rel_nloc = is_time_relevant(pos_con, neg_tpp);
    bool ncon_rel_ploc = is_time_relevant(neg_con, pos_tpp);
    bool ncon_rel_nloc = is_time_relevant(neg_con, neg_tpp);
    if (pcon_rel_ploc && pcon_rel_nloc)
      aut.add_transition(ploc, silent_label, nloc, mu);
    if (ncon_rel_ploc && ncon_rel_nloc)
      aut.add_transition(nloc, silent_label, ploc, std::move(mu));
  } else {
    aut.add_transition(ploc, silent_label, nloc, mu);
    aut.add_transition(nloc, silent_label, ploc, std::move(mu));
  }

  // fix initial states
  if (aut.ini_states.find(ploc) == aut.ini_states.end())
    return;

  clock_val_set& pos_cvs = aut.ini_states[ploc];
  auto neg_cvs = pos_cvs.split(pos_con, Topol::CLOSED);
  // fix positive
  if (pos_cvs.is_empty())
    aut.ini_states.erase(ploc);
  // fig negative
  if (!neg_cvs.is_empty())
    aut.ini_states[nloc] = std::move(neg_cvs);
}

void split_location_old(automaton& aut, label_ref silent_label,
                        loc_ref loc, Con c, loc_ref_list& new_locs) {
  // divide location loc up using a closed-partition based on c
  // add the new locations to the list new_locs
  if (c.is_inconsistent())
    return;

  // increase partition level
  ++aut.locations[loc].partition_level;

  std::string newname = aut.locations[loc].name
    + "@" + int2string(aut.locations.size()) + "-";
  aut.locations[loc].name += "@" + int2string(aut.locations.size()) + "+";
  loc_ref negloc = aut.add_location(newname, aut.locations[loc]);

  // ENEA: FIXME: this is a closed rational poly-split
  // (but the original poly has already been copied in negloc).
  if (c.is_equality()) // simply convert it into a non-strict inequality
    c = constraint_to_nonstrict_inequality(c);
  // add complement of constraint to new loc
  Con cneg = closed_inequality_complement(c);
  aut.locations[negloc].invariant_add_constraint(cneg);
  aut.assume_loc_modification(negloc);

  // copy all transitions from loc to negloc
  // incoming transitions
  for (auto t : aut.locations[loc].in_trans) {
    const auto& trx = aut.transitions[t];
    if (trx.source_loc() == loc)
      continue; // don't do self-loops
    // check if new invariant and incoming invariant are disjunct
    if (trx.label() == silent_label) {
      // only add transition after some checks
      clock_val_set cvs;
      cvs.intersection_assign_from(aut.locations[negloc].invariant(),
                                   aut.locations[trx.source_loc()].invariant());
      if (cvs.is_empty())
        continue;
      // if it's a silent transition,
      // only copy it if it's at least a hyperplane
      // (otherwise it's redundant with one of the hyperplanes)
      if (REFINE_CHECK_TRANS_DIMS && cvs.get_real_dimension() < cvs.dim-1)
        continue;
    }
    auto mu = trx.unrestricted_mu();
    aut.add_transition(trx.source_loc(), trx.label(), negloc, std::move(mu));
  }

  // outgoing transitions
  for (auto t : aut.locations[loc].out_trans) {
    const auto& trx = aut.transitions[t];
    if (trx.target_loc() == loc)
      continue; // don't do self-loops
    // check if new invariant and incoming invariant are disjunct
    if (trx.label() == silent_label) {
      // only add transition after some checks
      clock_val_set cvs;
      cvs.intersection_assign_from(aut.locations[negloc].invariant(),
                                   aut.locations[trx.target_loc()].invariant());
      if (cvs.is_empty())
        continue;
      // if it's a silent transition,
      // only copy it if it's at least a hyperplane
      // (otherwise it's redundant with one of the hyperplanes)
      if (REFINE_CHECK_TRANS_DIMS && cvs.get_real_dimension() < cvs.dim-1)
        continue;
    }
    auto mu = trx.unrestricted_mu();
    aut.add_transition(negloc, trx.label(), trx.target_loc(), std::move(mu));
  }

  // Make a copy of self-loops,
  // because they're not covered by either of the previous cases
  for (auto t : aut.locations[loc].out_trans) {
    const auto& trx = aut.transitions[t];
    if (trx.target_loc() != loc)
      continue; // not a self loop
    auto trx_label = trx.label();
    auto mu = trx.unrestricted_mu();
    aut.add_transition(negloc, trx_label, negloc, mu); // copy mu
    aut.add_transition(loc, trx_label, negloc, mu); // copy mu
    aut.add_transition(negloc, trx_label, loc, std::move(mu)); // move mu
  }

  new_locs.push_back(negloc);
  // add original constraint to original loc
  aut.locations[loc].invariant_add_constraint(c);
  aut.assume_loc_modification(loc);

  // to fix incoming and outgoing transitions
  if (REFINE_CHECK_TRANS_DIMS)
    aut.location_remove_nonface_silents(loc, silent_label);

  // ----------------------------------------
  // add new transitions that connect the two
  // ----------------------------------------
  dim_type dim = aut.locations[loc].invariant().dim;
  Cons mu = identity_trans(dim);
  mu.push_back(constraint_to_equality(c));
  // test of transition is time relevant
  if (REFINE_CHECK_TIME_RELEVANCE) {
    // already existing transitions
    if (REFINE_CHECK_TIME_RELEVANCE_DURING) {
      aut.location_remove_nontimerel_silents(loc, silent_label);
      aut.location_remove_nontimerel_silents(negloc, silent_label);
    }
    // test transitions that will connect the old and new location
    // note: in pratice, one test should suffice?
    if (REFINE_USE_FP && !aut.locations[loc].is_time_post_poly_uptodate()) {
      convex_clock_val_set ccvs(get_loc_deriv(aut, loc));
      if (aut.reversed) ccvs.pointmirror_assign();
      tp_limit_cons_or_bits(ccvs);
      aut.locations[loc].time_post_poly_assign(ccvs);
    }
    if (REFINE_USE_FP && !aut.locations[negloc].is_time_post_poly_uptodate()) {
      convex_clock_val_set ccvs(get_loc_deriv(aut, negloc));
      if (aut.reversed) ccvs.pointmirror_assign();
      tp_limit_cons_or_bits(ccvs);
      aut.locations[negloc].time_post_poly_assign(ccvs);
    }

    const auto& tpp = aut.locations[loc].time_post_poly();
    const auto& neg_tpp = aut.locations[negloc].time_post_poly();
    bool c_rel_loc = is_time_relevant(c, tpp);
    bool c_rel_negloc = is_time_relevant(c, neg_tpp);
    bool cneg_rel_loc = is_time_relevant(cneg, tpp);
    bool cneg_rel_negloc = is_time_relevant(cneg, neg_tpp);
    if (c_rel_loc && c_rel_negloc)
      aut.add_transition(loc, silent_label, negloc, mu);
    if (cneg_rel_loc && cneg_rel_negloc)
      aut.add_transition(negloc, silent_label, loc, std::move(mu));
  } else {
    aut.add_transition(loc, silent_label, negloc, mu);
    aut.add_transition(negloc, silent_label, loc, std::move(mu));
  }

  // fix initial states
  if (aut.ini_states.find(loc) == aut.ini_states.end())
    return;
  // ENEA: FIXME: another poly split.
  // fix negative
  clock_val_set cvs = aut.ini_states[loc];
  cvs.add_constraint(cneg);
  if (!cvs.is_empty())
    aut.ini_states[negloc] = std::move(cvs);
  // fix positive
  aut.ini_states[loc].add_constraint(c);
  if (aut.ini_states[loc].is_empty())
    aut.ini_states.erase(loc);
}

void split_location(automaton& aut, label_ref silent_label,
                    loc_ref ploc, Con pos_con, loc_ref_list& new_locs) {
  if (REFINE_USE_NEW_SPLIT) {
    split_location_new(aut, silent_label, ploc, pos_con, new_locs);
  } else {
    split_location_old(aut, silent_label, ploc, pos_con, new_locs);
  }
}

void split_location(automaton& aut, label_ref silent_label,
                    const Cons& cs, loc_ref_list& locs) {
  loc_ref_list new_locs;
  for (const auto& c : cs) {
    assert(new_locs.empty());
    for (auto& loc : locs)
      split_location(aut, silent_label, loc, c, new_locs);
    locs.splice(locs.end(), new_locs);
  }
}

Cons
get_splitting_constraints(automaton& aut, loc_ref loc,
                          refinement_method method) {
  dim_type dim = aut.locations[loc].invariant().dim;
  DoublePoint p(dim, 0.0);
  DoublePoint dx(dim, 0.0);
  TNT::Array2D<double> Jx(dim, dim);

  DoublePoints dpts;

  switch (method) {
  case carth_center:
  case carth0_center:
  case carth1_center:
  case deriv_center:
  case jacob_center:
    // find geometric center point of vertices of invariant of location loc
    // todo: actual geometric center, for now it's just the arithmetic mean
    get_loc_vert(aut, loc, dpts);
    if (!dpts.empty()) {
      dim = dpts.front().dim();
      p = DoublePoint(dim, 0.0);
      get_DoublePoints_center(dpts, p);
    }
    break;

  case deriv_dcenter:
  case jacob_dcenter:
    // find center point of derivative
    if (method == deriv_dcenter) {
      // note: this yields actually the convex hull of all the derivatives.
      clock_val_set cvs(aut.locations[loc].time_post_poly());
      add_cvs_to_DoublePoints(cvs, dpts);
    } else {
      // use Jacobian * dx instead of dx, everything else stays the same
      get_loc_vert(aut, loc, dpts);
      for (auto& dpt : dpts) {
        get_derivative(aut.name, dpt, dx);
        get_jacobian(aut.name, dpt, Jx);
        dpt = Jx * dx;
      }
    }
    if (!dpts.empty()) {
      dim = dpts.front().dim();
      dx = DoublePoint(dim, 0.0);
      get_DoublePoints_center(dpts, dx);
    }
    break;
  } // switch

  if (dpts.empty())
    return Cons();

  Con c;
  Cons cons;

  switch (method) {
  case carth_center:
    for (dim_type i = 0; i < dim; ++i) {
      c = get_Con_through_DoublePoint(Var(i), p);
      // ENEA: FIXME limit_bitsize(c, CONSTRAINT_BITSIZE);
      cons.push_back(std::move(c));
    }
    break;

  case carth0_center:
    c = get_Con_through_DoublePoint(Var(0), p);
    // ENEA: FIXME limit_bitsize(c, CONSTRAINT_BITSIZE);
    cons.push_back(std::move(c));
    break;

  case carth1_center:
    c = get_Con_through_DoublePoint(Var(1), p);
    // ENEA: FIXME limit_bitsize(c, CONSTRAINT_BITSIZE);
    cons.push_back(std::move(c));
    break;

  case deriv_center:
    // first constraint is the derivative dx/dt
    get_derivative(aut.name, p, dx);
    c = get_Con_through_DoublePoint(dx, p);
    // ENEA: FIXME limit_bitsize(c, CONSTRAINT_BITSIZE);
    cons.push_back(std::move(c));
    break;

  case jacob_center:
    get_derivative(aut.name, p, dx);
    get_jacobian(aut.name, p, Jx);
    {
      DoublePoint ddx(dim, 0.0);
      ddx = Jx * dx;
      c = get_Con_through_DoublePoint(ddx, p);
    }
    // ENEA: FIXME limit_bitsize(c, CONSTRAINT_BITSIZE);
    cons.push_back(std::move(c));
    break;

  case deriv_dcenter:
  case jacob_dcenter:
    // derivative is already known as center of all derivatives
    // now position the plane between the min and max
    dpts.clear();
    get_loc_vert(aut, loc, dpts);
    if (!dpts.empty()) {
      dim = dpts.front().dim();
      p = DoublePoint(dim, 0.0);
      get_DoublePoints_center(dpts, p);
      // find min and max in dpts with respect to dx,
      // i.e., min and max of dpts[i]*dx
      double min_dx, max_dx;
      DoublePoint x_max(dim, 0.0), x_min(dim, 0.0);
      get_DoublePoints_min_max(dpts, dx, min_dx, max_dx, x_min, x_max);
      c = get_Con_through_double(dx, -(min_dx+max_dx)/2);
      // ENEA: FIXME limit_bitsize(c, CONSTRAINT_BITSIZE);
      cons.push_back(std::move(c));
    }
    break;

  default:
    std::cout << "unknown method : " << method << std::endl;
  } // switch

  return cons;
}

void refine_locs(automaton& aut, label_ref silent_label,
                 loc_ref_list& locs, refinement_method method) {
  stopwatch sw(2100,"refine_locs");
  message(2100, "Refining "
          + int2string(locs.size()) + " locations of " + aut.name +".");

  // Refine all locations in locs.
  // Returns in all_locs the set of modified locations
  loc_ref_list all_locs;
  for (auto loc : locs) {
    Cons cs = get_splitting_constraints(aut, loc, method);
    // if there is a split
    if (cs.empty())
      all_locs.push_back(loc);
    else {
      // now split the location
      loc_ref_list dlocs;
      dlocs.push_back(loc);
      split_location(aut, silent_label, cs, dlocs);
      // copy all modified locations to all_locs
      all_locs.splice(all_locs.end(), dlocs);
    }
  }

  locs = std::move(all_locs);

  message(3100, "Resulted in refined " + int2string(locs.size())
          + " locations of " + aut.name
          + ", total " + int2string(aut.locations.size())
          + " locs, " + int2string(aut.transitions.size()) + " trans.");
}

void refine_locs(automaton& aut, label_ref silent_label,
                 refinement_method method, int iter) {
  loc_ref_list locs;
  for (dim_type i = 0; i < num_rows(aut.locations); ++i)
    locs.push_back(i);
  for (int i = 0; i < iter; ++i)
    refine_locs(aut, silent_label, locs, method);
}

bool
get_refinement_constraint(automaton& aut, loc_ref loc,
                          const clock_val_set& inv,
                          const clock_val_set& reached,
                          Con& con,
                          bool& splits_reached) {
  using refine_criterion = std::list<double>;
  using Cand = std::pair<refine_criterion, Con>;
  struct Cand_cmp {
    bool operator()(const Cand& c1, const Cand& c2) const {
      return c1.first < c2.first;
    }
  };
  // ENEA: FIXME: why storing them in a set
  // if only the best one is ever used?
  using Cand_set = std::set<Cand, Cand_cmp>;
  Cand_set cands;

  // for minimization and maximization
  Affine_Expr aexpr;
  Rational min_v, max_v;

  refine_criterion tmp_crit;
  bool refine_only_if_greater_dmax = false;

  if (REFINE_DERIV_MINANGLE < 1
      && get_loc_angle(aut, loc) >= REFINE_DERIV_MINANGLE)
    refine_only_if_greater_dmax = true;

  // Create the candidate list
  for (const auto& rc : REFINEMENT_CONS) {
    auto dmin = rc.min_d;
    auto dmax = rc.max_d;
    // start a fresh list
    tmp_crit.clear();
    if (sgn(dmin) <= 0)
      continue;
    // compute minimum and maximum on *homogeneous* expr.
    aexpr.expr = rc.con.linear_expr();
    bool has_min = inv.minimize(aexpr, min_v);
    bool has_max = inv.maximize(aexpr, max_v);

    if (has_min && has_max) {
      // delta = max - min
      auto delta = max_v - min_v;
      assert(sgn(delta) >= 0);
      if (REFINE_FORCE_SPLITTING
          || (delta > dmin
              && (!refine_only_if_greater_dmax
                  || (Rational(0) < dmax && dmax < delta)))) {
        // constraint is { expr >= (min + max)/2 }
        auto coeff = 2 * min_v.get_den() * max_v.get_den();
        auto inhomo = max_v.get_num() * min_v.get_den()
          + min_v.get_num() * max_v.get_den();
        neg_assign(inhomo);
        Con tmp_con = Con(coeff * aexpr.expr,
                          inhomo,
                          Con::NONSTRICT_INEQUALITY);
        // prioritize the refinement of the max. size
        // that violates the min. cell size dmax
        if (Rational(0) < dmax && dmax < delta)
          // this takes priority over anything else
          tmp_crit.push_back(mpq_class(-delta/dmax).get_d());
        else {
          // this will always be the last choice, since -delta/dmax < 0
          tmp_crit.push_back(0.0);
          if (REFINE_PRIORITIZE_ANGLE) {
            // get the angle of new locations
            // ENEA: FIXME: poly split ???
            auto cvs = aut.locations[loc].invariant();
            cvs.add_constraint(tmp_con);
            auto angle_1 = get_loc_angle(aut, loc, cvs);
            cvs = aut.locations[loc].invariant();
            cvs.add_constraint(closed_inequality_complement(tmp_con));
            auto angle_2 = get_loc_angle(aut, loc, cvs);
            // maximize the worst case (min) angle
            tmp_crit.push_back(-std::min(angle_1, angle_2));
            // todo: if the reachables states are on one side,
            // just consider the reachable states
          }
          if (REFINE_PRIORITIZE_REACH_SPLIT) {
            if (reached.is_split_by(tmp_con))
              // also the default if it wasn't tested
              tmp_crit.push_back(1.0);
            else
              tmp_crit.push_back(0.0);
          }
          if (REFINE_SMALLEST_FIRST)
            tmp_crit.push_back(mpq_class(delta/dmin).get_d());
          else
            tmp_crit.push_back(mpq_class(-delta/dmin).get_d());
        }
        // put onto candidate list
        if (delta > dmax)
          cands.insert(std::make_pair(tmp_crit, tmp_con));
      }
      continue;
    }

    if (has_min && !has_max) {
      // check for maximum in reachable set
      has_max = reached.maximize(aexpr, max_v);
      // inhomo = max (or min) + delta_min
      auto& inhomo = has_max ? max_v : min_v;
      inhomo += dmin;
      // constraint is { expr >= inhomo }
      neg_assign(inhomo);
      Con tmp_con(inhomo.get_den() * aexpr.expr,
                  inhomo.get_num(),
                  Con::NONSTRICT_INEQUALITY);
      tmp_crit.push_back(-1.0);
      cands.insert(std::make_pair(tmp_crit, tmp_con));
      continue;
    }

    if (!has_min && has_max) {
      // check for minimum in reachable set
      has_min = reached.minimize(aexpr, min_v);
      // inhomo = min (or max) - delta_min
      auto& inhomo = has_min ? min_v : max_v;
      inhomo -= dmin;
      // constraint is { expr >= inhomo }
      neg_assign(inhomo);
      Con tmp_con(inhomo.get_den() * aexpr.expr,
                  inhomo.get_num(),
                  Con::NONSTRICT_INEQUALITY);
      tmp_crit.push_back(-1.0);
      cands.insert(std::make_pair(tmp_crit, tmp_con));
      continue;
    }

    assert(!has_min && !has_max);
    // just add expr >= 0
    Con tmp_con = Con(aexpr.expr, 0, Con::NONSTRICT_INEQUALITY);
    tmp_crit.push_back(-1.0);
    cands.insert(std::make_pair(tmp_crit, tmp_con));
  } // for loop

  if (cands.empty()) {
    REFINE_FORCE_SPLITTING = false;
    return false;
  }

  // the best is the first candidate
  const auto& best = *(cands.begin());
  con = best.second;
  if (REFINE_PRIORITIZE_REACH_SPLIT)
    // it was tested in the candidate list
    splits_reached = (best.first.front() > 0);
  else if (REFINE_TEST_REACH_SPLIT)
    // test it now
    splits_reached = reached.is_split_by(con);
  else
    // default case, because it contains all possibilities
    splits_reached = true;
  REFINE_FORCE_SPLITTING = false;
  return true;
}

bool refine_location_otf(automaton& aut, loc_ref tloc,
                         const clock_val_set& reached,
                         symb_state_maplist& states,
                         symb_state_plist& check_states,
                         symb_state_plist& new_states,
                         loc_ref_set& succ_locs,
                         bool convex) {
#if PHAVERLITE_STATS
  static pplite::Local_Stats stats("refine_location_otf");
  pplite::Local_Clock clock(stats);
#endif
  if ((!aut.locations[tloc].is_surface
       || REFINEMENT_CONS.empty()
       || aut.locations[tloc].is_fully_refined)
      && !REFINE_FORCE_SPLITTING) {
    // don't refine
    succ_locs.insert(tloc);
    return false;
  }

  loc_ref_list wait_list, new_locs;
  Con refine_con;
  bool splits_reached = false;

  // put tloc onto wait_list
  wait_list.push_front(tloc);
  int iter=0;

  while (!wait_list.empty()) {
    ++iter;
    // pop loc
    auto loc = wait_list.front();
    wait_list.pop_front();

    bool more_splitting = false;
    if (REFINE_FORCE_SPLITTING
        || !reached.contains(aut.locations[loc].invariant())) {
      more_splitting
        = get_refinement_constraint(aut, loc,
                                    aut.locations[loc].invariant(),
                                    reached, refine_con, splits_reached);
    }
    bool level_blocked = false;
    if (more_splitting &&
        ((REFINE_PARTITION_INSIDE && iter>1)
         ||
         (REFINE_PARTITION_LEVEL_MAX >= 0
          && aut.locations[loc].partition_level >= REFINE_PARTITION_LEVEL_MAX)
         )) {
      // don't refine further than set level
      level_blocked = true;
    }
    if (more_splitting && !level_blocked) {
      new_locs.clear();
      split_location(aut, REFINEMENT_LABEL, loc, refine_con, new_locs);
      const bool has_newlocs = !new_locs.empty();
      if (has_newlocs) {
        auto newloc = new_locs.front();
        // split maplist and waiting lists check_states and new_states
        states.split(loc, aut.locations[loc].invariant(),
                     newloc, aut.locations[newloc].invariant(),
                     check_states, new_states, convex);
        // surface property might not hold any more
        if (REFINE_PARTITION_INSIDE) {
          aut.locations[loc].is_surface=false;
            aut.locations[newloc].is_surface=false;
        }
      }

      if (!splits_reached) {
        // only put the loc back on wait_list list that splits
        // todo: this could be had simpler if we rememberd
        // in get_refinement_constraint which had been on what side...
        // however, this should be reasonably fast anyway
        if (!reached.is_disjoint_from(aut.locations[loc].invariant()))
          wait_list.push_front(loc);
        if (has_newlocs) {
          auto newloc = new_locs.front();
          if (!reached.is_disjoint_from(aut.locations[newloc].invariant()))
            wait_list.push_front(newloc);
        }
      } else {
        // put both on wait_list
        wait_list.push_front(loc);
        if (has_newlocs) {
          auto newloc = new_locs.front();
          wait_list.push_front(newloc);
        }
      }
    } else {
      // no more splitting
      if (!level_blocked)
        aut.locations[loc].is_fully_refined=true;
      succ_locs.insert(loc);
      // test for useless tau transitions
      if (REFINE_CHECK_TIME_RELEVANCE_FINAL)
        aut.location_remove_nontimerel_silents(loc, REFINEMENT_LABEL);
    }
  } // while
  return true;
}

void
refine_states(automaton& aut,
              symb_state_maplist& states,
              symb_state_plist& check_states) {
  // refine the locations in check_states, which refers to states.
  symb_state_plist new_states;
  while (!check_states.empty()) {
    auto state_it = check_states.front();
    check_states.pop_front();

    // succ operator - part that depends on transition
    loc_ref loc = state_it->loc;
    clock_val_set cvs = state_it->cvs;
    // Refine location
    loc_ref_set succ_locs;
    refine_location_otf(aut, loc, cvs, states,
                        check_states, new_states,
                        succ_locs, REACH_USE_CONVEX_HULL);
    for (auto loc2 : succ_locs) {
      clock_val_set cvs2;
      cvs2.intersection_assign_from(cvs, aut.locations[loc2].invariant());
      if (cvs2.is_empty())
        continue;
      states.add(loc2, std::move(cvs2), REACH_USE_CONVEX_HULL,
                 &check_states, &new_states);
    }
  } // end while

  // put the new_states back on the list
  check_states.swap(new_states);
}

void
refine_states(automaton& aut, symb_states_type& s) {
  symb_state_maplist states;
  symb_state_plist check_states;
  states.initialize(s, check_states, false);
  refine_states(aut, states, check_states);
}
