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

#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

void
tp_limit_cons_or_bits(convex_clock_val_set& ccvs) {
  bool changed = ccvs.limit_cons(TP_CONSTRAINT_LIMIT, CONSTRAINT_BITSIZE);
  if (!changed)
    ccvs.limit_bits(CONSTRAINT_BITSIZE);
}

bool
reach_limit_cons_or_bits(clock_val_set& cvs,
                         const clock_val_set& inv) {
  if (BLOCK_OVERAPPROXIMATION)
    return false;
  if (REACH_BITSIZE_TRIGGER > 0
      && REACH_CONSTRAINT_TRIGGER > 0
      && max_bitsize(cvs) <= REACH_BITSIZE_TRIGGER
      && max_consize(cvs) <= REACH_CONSTRAINT_TRIGGER)
    return false;
  bool changed = cvs.limit_cons_or_bits(REACH_CONSTRAINT_LIMIT,
                                        CONSTRAINT_BITSIZE);
  if (changed) cvs.intersection_assign(inv);
  return changed;
}

namespace detail {

using Prior = int;
using Priors = std::vector<Prior>;

void
assign_priorities(Priors& priors, Prior& time,
                  const automaton& aut, loc_ref loc,
                  const symb_state_maplist* states_ptr) {
  const auto max_time = std::numeric_limits<Prior>::max();
  priors[loc] = 0; // color gray
  if (time < max_time)
    ++time;
  for (auto trx : aut.locations[loc].out_trans) {
    auto tgt_loc = aut.transitions[trx].target_loc();
    if (priors[tgt_loc] >= 0)
      continue;
    if (states_ptr != nullptr && states_ptr->is_empty(tgt_loc))
      continue;
    assign_priorities(priors, time, aut, tgt_loc, states_ptr);
  }
  if (time < max_time)
    ++time;
  priors[loc] = time;
}

Priors
compute_priorities(const automaton& aut) {
  Priors priors(aut.locations.size(), -1);
  int time = 0;
  for (const auto& p : aut.ini_states) {
    auto loc = p.first;
    if (priors[loc] < 0)
      assign_priorities(priors, time, aut, loc, nullptr);
  }
  return priors;
}

Priors
compute_priorities_reachable_only(const automaton& aut,
                                  const symb_state_plist& new_states,
                                  const symb_state_maplist& states) {
  Priors priors(aut.locations.size(), -1);
  int time = 0;
  for (auto ptr : new_states) {
    auto loc = ptr->loc;
    if (priors[loc] < 0)
      assign_priorities(priors, time, aut, loc, &states);
  }
  return priors;
}

symb_state_plist::const_iterator
topsort_selection(const automaton& aut,
                  const symb_state_plist& new_states,
                  const symb_state_maplist& states) {
  // Note: avoid computing priorities at each iteration.
  // Recompute after SEARCH_METHOD_TOPSORT_TOKENS iterations.
  static Priors priors;
  static int tokens = 0;

  // Recompute priorities if no token left.
  if (tokens == 0) {
    tokens = SEARCH_METHOD_TOPSORT_TOKENS;
    priors = (SEARCH_METHOD == Search_Method::topsort)
      ? compute_priorities(aut)
      : compute_priorities_reachable_only(aut, new_states, states);
  }
  // Consume a token.
  --tokens;

  return std::max_element(new_states.begin(), new_states.end(),
                          [](symb_state_plist::value_type i1,
                             symb_state_plist::value_type i2) {
                            return priors[i1->loc] < priors[i2->loc];
                          });
}

} // namespace detail

symb_state_maplist::iterator
automaton::pop_maplist_iter(const symb_state_maplist& states,
                            symb_state_plist& new_states) {
#if PHAVERLITE_STATS
  static pplite::Local_Stats stats("pop_maplist_iter");
  pplite::Local_Clock clock(stats);
#endif

  assert(!new_states.empty());
  const bool pop_first
    = (SEARCH_METHOD == Search_Method::trx_based)
    || (++new_states.begin() == new_states.end());

  if (pop_first) {
    auto res = new_states.front();
    new_states.pop_front();
    return res;
  }

  assert(SEARCH_METHOD == Search_Method::topsort
         || SEARCH_METHOD == Search_Method::topsort_reachable);
  auto res_iter = detail::topsort_selection(*this, new_states, states);
  auto res = *res_iter;
  new_states.erase(res_iter);
  return res;
}

void
automaton::simplify_successor(loc_ref loc, clock_val_set& cvs) {
  if (BLOCK_OVERAPPROXIMATION)
    return;

  bool changed = false;
  if (REACH_USE_BBOX) {
    changed = cvs.relative_bounding_boxes();
  }
#if 0 // ENEA: FIXME: commented out (needed?)
  else if (REACH_CONSTRAINT_LIMIT > 0 && !REACH_USE_CONVEX_HULL) {
    changed = cvs.limit_cons_or_bits(REACH_CONSTRAINT_LIMIT,
                                     CONSTRAINT_BITSIZE);
  }
  // todo: use invariant information to limit bits only for those constraints
  //       that are not in the invariant
  // for now: limit all constraints
  // don't do this if constraints have been limited
  if (CONSTRAINT_BITSIZE > 0 && REACH_CONSTRAINT_LIMIT <= 0) {
    if (!REACH_USE_CONVEX_HULL) {
      bool changed_now = cvs.limit_significant_bits(CONSTRAINT_BITSIZE);
      changed = changed || changed_now;
    }
  }
#endif

  if (changed)
    cvs.intersection_assign(locations[loc].invariant());
}

void
automaton::time_post_assign_convex(loc_ref loc, clock_val_set& cvs) {

  if (TIME_POST_ITER <= 0) {
    // todo: this is just temp, until nonlinear derivatives
    // are included properly in the location

    // succ operator - part that's independent of transition
    clock_val_set cvs_backup(cvs);

    if (REFINE_USE_FP && !locations[loc].is_time_post_poly_uptodate()) {
      convex_clock_val_set ccvs(get_loc_deriv(*this, loc));
      if (reversed)
        ccvs.pointmirror_assign();
      tp_limit_cons_or_bits(ccvs);
      locations[loc].time_post_poly_assign(ccvs);
    }

    if (REFINE_DERIVATIVE_METHOD == 5)
      // cone fixpoint
      time_post_assign_hscc06(loc, cvs);
    else
      // not cone fixpoint
      cvs.time_elapse_assign(locations[loc].time_post_poly());
    cvs.intersection_assign(locations[loc].invariant());

    // subtract the interiors of ASAP transitions
    for (auto t : locations[loc].out_trans) {
      const auto& trx = transitions[t];
      if (trx.is_urgent()) {
        auto gcvs = trx.exit_set(locations[trx.source_loc()].invariant(),
                                 locations[trx.target_loc()].invariant());
        gcvs.time_elapse_assign(locations[loc].time_post_poly());
        auto cvs2 = cvs;
        cvs2.difference_assign(gcvs);
        cvs2.topological_closure_assign();
        cvs2.intersection_assign(cvs);
        cvs2.union_assign(cvs_backup); // return at least the original states!
        cvs.m_swap(cvs2);
        cvs.simplify();
      }
    }
    return;
  } // TIME_POST_ITER <= 0

  assert(TIME_POST_ITER > 0);
  unsigned iter = 0;

  // succ operator - part that's independent of transition
  clock_val_set cvs_backup(cvs);

  clock_val_set restr(locations[loc].invariant());
  clock_val_set rcvs(dim);
  convex_clock_val_set ccvs(dim);

  // do the first iteration as usual
  if (REFINE_USE_FP && !locations[loc].is_time_post_poly_uptodate()) {
    convex_clock_val_set ccvs(get_loc_deriv(*this,loc));
    if (reversed)
      ccvs.pointmirror_assign();
    tp_limit_cons_or_bits(ccvs);
    locations[loc].time_post_poly_assign(ccvs);
  }

  rcvs = cvs;
  rcvs.time_elapse_assign(locations[loc].time_post_poly());
  restr.intersection_assign_from(rcvs,locations[loc].invariant());

  ++iter;
  while ((int)iter < TIME_POST_ITER) {
    ++iter;
    // todo: this is just temp, until nonlinear derivatives are included properly in the location
    if (REFINE_USE_FP) {
      ccvs = get_loc_deriv(*this, loc, restr);
      if (reversed) ccvs.pointmirror_assign();
    } else {
      ccvs = locations[loc].time_post_poly(restr);
      tp_limit_cons_or_bits(ccvs);
      rcvs = cvs;
      rcvs.time_elapse_assign(ccvs);
      restr.intersection_assign_from(rcvs, locations[loc].invariant());
    }

    cvs.m_swap(restr);
  }

  // subtract the interiors of ASAP transitions
  for (auto t : locations[loc].out_trans) {
    const auto& trx = transitions[t];
    if (trx.is_urgent()) {
      auto gcvs = trx.exit_set(locations[trx.source_loc()].invariant(),
                               locations[trx.target_loc()].invariant());
      gcvs.time_elapse_assign(locations[loc].time_post_poly());
      auto cvs2 = cvs;
      cvs2.difference_assign(gcvs);
      cvs2.topological_closure_assign();
      cvs2.intersection_assign(cvs);
      cvs2.union_assign(cvs_backup); // return at least the original states!
      cvs.m_swap(cvs2);
      cvs.simplify();
    }
  }
}

void
automaton::time_post_assign_hscc06(loc_ref loc, clock_val_set& cvs) {
  assert(TIME_POST_ITER <= 0);
  assert(REFINE_DERIVATIVE_METHOD == 5);

  TNT::Array2D<Integer> A;
  TNT::Array1D<Integer> b, den;
  locations[loc].get_linear_vtef().get_linear_system_matrices(A, b, den);
  const auto& inv = locations[loc].invariant();
  const auto inv_size = inv.ccvs_list.size();
  const auto& tpp = locations[loc].time_post_poly();

#ifndef NDEBUG
  auto debug_print = [](const convex_clock_val_set& cone,
                        const std::string& str) {
    auto ccvs = get_poly_from_cone(cone);
    ccvs.minimize();
    std::cout << "\n======================\n";
    std::cout << "   " << str << "\n";
    std::cout << ccvs;
    std::cout << "\n =====================\n";
  };
#else
  auto debug_print = [](const convex_clock_val_set&,
                        const std::string&) {};
#endif

  for (auto& ccvs_i : cvs.ccvs_list) {

    // Compute static time post.
    auto tp_static = ccvs_i;
    tp_static.time_elapse_assign(tpp);
    // Intersect with (convex hull of) invariant.
    if (inv_size == 1)
      tp_static.intersection_assign(inv.ccvs_list.front());
    else {
      auto inv_cvs = clock_val_set(tp_static);
      inv_cvs.intersection_assign(inv);
      inv_cvs.convex_hull_assign();
      tp_static = std::move(inv_cvs.ccvs_list.front());
    }
    auto tp_static_cs = tp_static.minimized_constraints();
    auto static_cone = get_cone(tp_static);

    // fixpoint base: universe cone.
    auto old_cone = convex_clock_val_set(dim+1);
    // first iterate: init \hull static
    auto new_cone = get_cone(ccvs_i);
    new_cone.poly_hull_assign(static_cone);
    debug_print(new_cone, "initial cone");

    unsigned max_cone_delay = TIME_POST_CONE_WIDEN_DELAY;
    unsigned max_cone_extra = TIME_POST_CONE_WIDEN_DELAY;
    while (true) {
      std::swap(old_cone, new_cone);
      // new cone = ( map(old) \inters old ) \hull static
      new_cone = map_cone(old_cone, A, b, den, TIME_POST_CONE_LAMBDA);
      new_cone.intersection_assign(old_cone);
      new_cone.poly_hull_assign(static_cone);

      // at most max_cone_iter iterations without extrapolation.
      if (max_cone_delay > 0)
        --max_cone_delay;
      else {
        bool force_widening = (max_cone_extra == 0);
        if (!force_widening)
          --max_cone_extra;
        new_cone.dual_widen_cone(old_cone, force_widening,
                                 TIME_POST_CONE_WIDEN_PRECISION);
      }

      debug_print(old_cone, "old cone");
      debug_print(new_cone, "new cone");
      assert(old_cone.contains(new_cone));
      // check for post-fixpoint.
      if (new_cone.contains(old_cone))
        break;
    }
    ccvs_i = get_poly_from_cone(new_cone);
    ccvs_i.intersection_assign(tp_static);
    tp_limit_cons_or_bits(ccvs_i);
  }
}

void
automaton::time_post_assign(loc_ref loc, clock_val_set& cvs) {
#if PHAVERLITE_STATS
  static pplite::Local_Stats stats("time_post_assign");
  pplite::Local_Clock clock(stats);
#endif
  if (locations[loc].invariant().size() > 1)
    throw_error("Cannot deal with nonconvex invariants in time elapse");
  time_post_assign_convex(loc, cvs);
}

void
automaton::trans_and_time_post_assign_aux(loc_ref tloc,
                                          clock_val_set& tcvs,
                                          const symb_state_maplist& states) {
  // Note: tcvs is the new tloc (nonempty) state.
  assert(!tcvs.is_empty());

  if (!REACH_USE_CONVEX_HULL) {
    const auto& tinv = locations[tloc].invariant();
    reach_limit_cons_or_bits(tcvs, tinv);
    if (REACH_USE_TIME_ELAPSE) {
      time_post_assign(tloc, tcvs);
      assert(!tcvs.is_empty() && "empty time elapse");
    }
    return;
  }

  assert(REACH_USE_CONVEX_HULL);
  // check for convergence, and apply widening if necessary
  if (REFINE_MAX_CHECKS > 0
      && locations[tloc].nr_checks > REFINE_MAX_CHECKS) {
    if (!states.get_clock_val_set(tloc).contains(tcvs)) {
      // merge old state into single poly
      auto old_ccvs = states.get_clock_val_set(tloc).get_convex_hull();
      // add it to new state and merge again
      tcvs.union_assign(old_ccvs);
      tcvs.convex_hull_assign();
      // compute widening
      auto& new_ccvs = tcvs.ccvs_list.front();
      new_ccvs.BHRZ03_widening_assign(old_ccvs);
      locations[tloc].nr_checks = 0;
      tcvs.intersection_assign(locations[tloc].invariant());
    }
  }
}

void
automaton::trans_and_time_post_assign(symb_state_maplist& states,
                                      symb_state_plist& check_states,
                                      symb_states_type& f_states,
                                      bool& f_states_reachable) {
  if (check_states.empty())
    return;

  stopwatch sw(10000000, "trans_and_time_post_assign");

  // determine whether to check for intersection with forbidden states
  const bool check_for_forbidden = !f_states.is_empty();
  f_states_reachable = false;

  if (REFINE_PARTITION_INSIDE)
    set_surface_flag(false);

  assert(!check_states.empty());

  symb_state_plist new_states;
  const bool add_to_new_states = (SEARCH_METHOD == Search_Method::trx_based);
  symb_state_plist* check_states_ptr = &check_states;
  symb_state_plist* new_states_ptr
    = add_to_new_states ? &new_states : &check_states;

  int local_iter_count = 0;

  while (!check_states.empty()) {
    ++local_iter_count;
    ++GLOBAL_ITER_COUNT;

    if (!add_to_new_states) {
      // exit loop if too many iterations
      if (0 < REACH_MAX_ITER && REACH_MAX_ITER < GLOBAL_ITER_COUNT)
        break;
    }

    auto state_it = pop_maplist_iter(states, check_states);
    auto loc = state_it->loc;
    // Note: here we take a reference, since we change it.
    auto& cvs = state_it->cvs;

    ++locations[loc].nr_checks;

    if (REACH_USE_CONVEX_HULL) {
      const auto& inv = locations[loc].invariant();
      bool changed = reach_limit_cons_or_bits(cvs, inv);
      if (!changed && REACH_USE_CONSTRAINT_HULL)
        cvs.intersection_assign(inv);
      if (REACH_USE_TIME_ELAPSE)
        time_post_assign(loc, cvs);
    }

    // if requested, check for intersection with forbidden states
    if (check_for_forbidden) {
      if (f_states.is_intersecting(locations[loc].name, cvs)) {
        f_states_reachable = true;
        // Exit from loop.
        break;
      }
    }

    bool deadlock_found = true;

    // Take a copy of cvs if location has a self loop, since the state
    // may be detected as redundant (if a computed new_cvs happens
    // to be extensive) and destroyed; otherwise (no self loop),
    // just take a reference to original cvs.
    const auto& old_cvs
      = location_has_self_loop(loc) ? clock_val_set(cvs) : cvs;

    // check outgoing transitions
    trans_ref_set tr_checked;
    // can't simply iterate through out_trans
    // because they can change through refinement splitting
    while (!tr_checked.contains(locations[loc].out_trans)) {
      // reserve this transition as not removable
      auto t = tr_checked.non_contained_element(locations[loc].out_trans);
      tr_checked.insert(t);
      const auto& trx = transitions[t];

      loc_ref sloc = trx.source_loc();
      loc_ref tloc = trx.target_loc();
      const auto& sloc_inv = locations[sloc].invariant();
      const auto& tloc_inv = locations[tloc].invariant();

      // succ operator - part that depends on transition
      clock_val_set new_cvs = old_cvs;
      trx.apply(new_cvs, sloc_inv, tloc_inv);

      if (new_cvs.is_empty())
        continue;

      deadlock_found = false;

      // doesn't do anything if convex hull is on
      // to do: should be after refinement!!!
      // (only relevant if not convex hull)
      simplify_successor(tloc, new_cvs);

      // Refine location
      loc_ref_set succ_locs;
      bool was_refined = refine_location_otf(*this, tloc, new_cvs, states,
                                             check_states, new_states,
                                             succ_locs, REACH_USE_CONVEX_HULL);
      if (was_refined) {
        for (auto succ_loc : succ_locs) {
          auto succ_cvs = new_cvs;
          succ_cvs.intersection_assign(locations[succ_loc].invariant());
          if (succ_cvs.is_empty())
            continue;
          trans_and_time_post_assign_aux(succ_loc, succ_cvs, states);
          states.add(succ_loc, std::move(succ_cvs), REACH_USE_CONVEX_HULL,
                     check_states_ptr, new_states_ptr);
        } // end for on succ_locs

      } else {
        assert(!was_refined && succ_locs.size() == 1
               && *succ_locs.begin() == tloc);
        // No need to copy new_cvs, intersecting it with invariant
        // and checking for emptiness.
        trans_and_time_post_assign_aux(tloc, new_cvs, states);
        states.add(tloc, std::move(new_cvs), REACH_USE_CONVEX_HULL,
                   check_states_ptr, new_states_ptr);
      }
    } // end while

    if (DEADLOCK_CHECKING && deadlock_found) {
      cout << endl << "deadlock found in loc " << locations[loc].name << endl;
      locations[loc].print();
    }

    if (SNAPSHOT_INTERVAL > 0 && GLOBAL_ITER_COUNT % SNAPSHOT_INTERVAL == 0)
      print_snapshot_fp_raw(states,
                            "out_mov_"
                            + int2string(1000000 + GLOBAL_ITER_COUNT));

    if (!add_to_new_states && local_iter_count % REACH_REPORT_INTERVAL == 0) {
      std::string str
        = "Iter " + int2string(local_iter_count) + ": "
        + "tot " + int2string(states.loc_size()) + " loc, "
        + int2string(states.cvs_size()) + " poly "
        + "(" + int2string(check_states.size()) + " waiting)";
      if (printing_time_info())
        str += (" in " + double2string(sw.delta()) + " sec "
                + "(total " + double2string(sw.value()) + ")");
      message(128200, 4, str);
    }
  } // end while

  // maybe put the new_states back on the list
  if (add_to_new_states) {
    assert(check_states.empty());
    check_states = std::move(new_states);
  }
}

void
automaton::reach_init(symb_states_type& i_states,
                      symb_state_maplist& states,
                      symb_state_plist& check_states) {
  // initialize the reachable states `states' and
  // the waiting list `check_states' with `i_states'.
  assert(states.iter_map.empty() && check_states.empty());

  // clear the is_fully_refined flags;
  unlock_locations();

  i_states.map_locations(get_loc_names_map());

  // initialize and refine states, without using convex hull.
  states.initialize(i_states, check_states, false);
  refine_states(*this, states, check_states);
  if (REACH_USE_CONVEX_HULL) {
    // reinitialize, using convex hull.
    i_states = states.transfer_symb_states();
    states.initialize(i_states, check_states, true);
  }

  if (REACH_USE_TIME_ELAPSE) {
    // update reachable states with time_post
    check_states = states.get_all_states();
    // Note: this is a single pass on check_states,
    // not a fixpoint computation; by passing nullptr
    // to the last argument of method add(), we do not re-insert
    // in `check_states' the newly computed states.
    while (!check_states.empty()) {
      auto state_it = check_states.front();
      check_states.pop_front();
      // Copies are meant.
      loc_ref loc = state_it->loc;
      clock_val_set cvs = state_it->cvs;
      time_post_assign(loc, cvs);
      states.add(loc, std::move(cvs), REACH_USE_CONVEX_HULL,
                 &check_states, nullptr);
    }
  }

  // clear any checking information in all the locations
  for (auto& loc : locations)
    loc.nr_checks = 0;

  // Reset check_states.
  check_states.clear();
  assert(check_states.empty());
  for (auto it = states.begin(); it != states.end(); ++it)
    check_states.push_back(it);
}

symb_states_type
automaton::get_reach_set(symb_states_type& i_states,
                         symb_states_type& f_states,
                         bool& f_states_reachable) {
  stopwatch sw(2001, "get_reach_set");

  int iter_count = 0;
  GLOBAL_ITER_COUNT = 0;

  message(2001, "Computing reachable states of " + name);

  f_states_reachable = false;

  symb_state_maplist states;
  symb_state_plist new_states;
  reach_init(i_states, states, new_states);

  uint old_size_new_states = 0;
  bool manual_REACH_USE_BBOX = REACH_USE_BBOX;
  bool restore_REACH_USE_BBOX = false;
  int bbox_iter_count = 0;
  double last_time_delta = 0;
  double time_delta = sw.delta();
  uint old_loc_size = 0;

  while (!new_states.empty() && !f_states_reachable) {
    ++iter_count;
    ++bbox_iter_count;

    // exit loop if too many iterations
    if (0 < REACH_MAX_ITER && REACH_MAX_ITER < GLOBAL_ITER_COUNT)
      break;

    if (bbox_iter_count >= REACH_USE_BBOX_ITER
        && old_size_new_states <= new_states.size()
        && last_time_delta < time_delta
        && (!REACH_STOP_USE_CONVEX_HULL_SETTLE
            || (old_loc_size == states.loc_size()))
        ) {
      bbox_iter_count = 0;
      REACH_USE_BBOX = true;
      restore_REACH_USE_BBOX = true;
      message(128200,"Applying bounding box to new states in this iteration.");
    }

    if (iter_count == REACH_STOP_USE_CONVEX_HULL_ITER) {
      REACH_USE_CONVEX_HULL = false;
      message(128200, "Stopping to use convex hull.");
    }
#if 0 // ENEA: FIXME
    if (iter_count == REACH_STOP_USE_BITSIZE) {
      CONSTRAINT_BITSIZE = 0;
      message(128200,"Stopping to use CONSTRAINT_BITSIZE.");
    }
#endif

    old_size_new_states = new_states.size();
    last_time_delta = time_delta;
    old_loc_size = states.loc_size();

    trans_and_time_post_assign(states, new_states,
                               f_states, f_states_reachable);

    time_delta = sw.delta();
    if (restore_REACH_USE_BBOX) {
      REACH_USE_BBOX = manual_REACH_USE_BBOX;
      restore_REACH_USE_BBOX = false;
    }

    if (VERBOSE_LEVEL < 128200)
      progress_dot(true);
    else {
      std::string str
        = "Iter " + int2string(iter_count)
        + "(" + int2string(GLOBAL_ITER_COUNT)+ "): "
        + "tot " + int2string(states.loc_size()) + " loc, "
        + int2string(states.cvs_size()) + " poly "
        + "(" + int2string(new_states.size()) + " new)";
      if (printing_time_info())
        str += (" in " + double2string(time_delta) + " sec "
                + "(total " + double2string(sw.value()) + ")");
      message(128200, 4, str);

      if (!REFINEMENT_CONS.empty()) {
        print_size();
        cout << endl;
      }
    }

  } // end while

  if (VERBOSE_LEVEL < 128200)
    progress_dot(false);

  message(32100, 6,
          "Terminated after " + int2string(iter_count) + " iterations.");
  message(32100, 6, "Total "
          + int2string(states.loc_size()) + " loc, "
          + int2string(states.cvs_size()) + " polyhedra.");

  symb_states_type symb_states = states.transfer_symb_states();
  symb_states.var_names_assign(get_var_names());
  symb_states.loc_names_assign(get_loc_names_map());
  return symb_states;
}

symb_states_type
automaton::get_reach_set() {
  ini_states.loc_names_assign(get_loc_names_map());
  symb_states_type dummystates;
  bool dummybool;
  return get_reach_set(ini_states, dummystates, dummybool);
}

bool
automaton::is_reachable(const symb_states_type& target,
                        symb_states_type& reach_set) {
  ini_states.loc_names_assign(get_loc_names_map());
  symb_states_type tgt(target);
  tgt.map_variables(get_var_names());

  bool tgt_reachable = false;
  reach_set = get_reach_set(ini_states, tgt, tgt_reachable);
  return tgt_reachable;
}

bool
automaton::is_reachable_fb(const symb_states_type& orig_target,
                           symb_states_type& reach_set) {
  symb_states_type dummystates;
  bool dummybool;
  // start with initial states
  ini_states.loc_names_assign(get_loc_names_map());

  symb_states_type source_states(ini_states);
  symb_states_type target_states(orig_target);
  target_states.map_locations(get_loc_names_map());

  symb_states_type temp_target;
  symb_states_type before_previous_target_set(target_states);
  symb_states_type previous_target_set(source_states);

  // record original constraints and angle.
  RefinementCons orig_refinement_cons = REFINEMENT_CONS;
  double orig_angle = REFINE_DERIV_MINANGLE;

  bool refinement_possible = true;
  for (auto& rc : REFINEMENT_CONS) {
    if (rc.max_d != Rational(0))
      rc.min_d = rc.max_d; // start with the max size
    else {
      message(0, "Error: Must specify max. size in set_refine_constraints.");
      refinement_possible = false;
    }
  }

  int iteration_count = 0;
  bool is_reachable = true;
  symb_states_type rs2;
  while ((refinement_possible && is_reachable)
         || (REACH_FB_REFINE_METHOD / 10 == 1 && iteration_count % 2 == 0)) {
    message(64000,"F/B-Iteration " + int2string(iteration_count));
    reach_set = get_reach_set(source_states, dummystates, dummybool);
    if (dim > 2) {
      for (dim_type i1 = 0; i1+1 < dim; ++i1) {
        for (dim_type i2 = i1+1; i2 < dim; ++i2) {
          // make clock set
          var_ref_set vrs;
          for (dim_type i = 0; i < dim; ++i)
            if (i != i1 && i != i2)
              vrs.insert(i);

          rs2 = reach_set;
          rs2.remove_space_dimensions(vrs);
          rs2.save_gen_fp_raw("out_mov_x" + int2string(i1)
                              + "x" + int2string(i2)
                              + "_" + int2string(1000000 + iteration_count));
        }
      }
    } else {
      assert(dim <= 2);
      reach_set.save_gen_fp_raw("out_mov_r_"
                                + int2string(1000000 + iteration_count));
    }
    invariant_assign(reach_set, false); // false : no need to map locations
    temp_target = target_states;
    temp_target.intersection_assign(reach_set);
    temp_target.simplify();
    if (temp_target.is_empty())
      is_reachable = false;
    else {
      target_states = source_states;
      if (REACH_FB_REFINE_METHOD / 10 == 1) {
        // intersect the continous target and source states
        clock_val_set mycvs = temp_target.union_over_locations();
        target_states.intersection_assign(mycvs);
      }
      source_states=temp_target;
#if 0 // ENEA: FIXME
      if (REACH_FB_REFINE_METHOD / 100 == 1)
        temp_target.limit_cons_or_bits(REACH_CONSTRAINT_LIMIT,
                                       CONSTRAINT_BITSIZE/4);
#endif // ENEA: FIXME
      if ((REACH_FB_REFINE_METHOD / 100 == 1)
          && !temp_target.contains(before_previous_target_set)) {
        // method 1xx: converge before tightening the partitions
        // note : contains with parameter true because location names
        // must be used!
        before_previous_target_set=previous_target_set;
        previous_target_set=temp_target;
        message(128000,"Improvement detected. Keeping partition size.");
      } else if (REACH_FB_REFINE_METHOD % 2 == 0) {
        // method 0: scale them all
        auto orig_it = orig_refinement_cons.begin();
        refinement_possible = false;
        for (auto& rc : REFINEMENT_CONS) {
          // just a guess: halve it
          rc.min_d /= Rational(2);
          // if it's too small, go back to the original minimum
          if (rc.min_d < orig_it->min_d)
            rc.min_d = orig_it->min_d;
          else
            // at least one of these must have changed, otherwise it's mute
            refinement_possible = true;
          ++orig_it;
        }
      } else if (REACH_FB_REFINE_METHOD % 2 == 1) {
        // method 1: scale the largest one
        auto orig_it = orig_refinement_cons.begin();
        auto max_ptr = &(REFINEMENT_CONS[0]);
        Rational min_ratio(100000000);
        refinement_possible = false;
        for (auto& rc : REFINEMENT_CONS) {
          if (rc.min_d / Rational(2) >= orig_it->min_d
              && rc.max_d / rc.min_d < min_ratio) {
            max_ptr = &rc;
            min_ratio = rc.max_d / rc.min_d;
            refinement_possible = true;
          }
          ++orig_it;
        }
        if (refinement_possible)
          max_ptr->min_d /= Rational(2);
      }
      else {
        throw_error("REACH_FB_REFINE_METHOD="
                    + int2string(REACH_FB_REFINE_METHOD)
                    + " not recognized.");
      }
      ++iteration_count;
      reverse();
    }
  }

  // Restore automaton state and parameters.
  if (iteration_count % 2 != 0)
    reverse();
  REFINEMENT_CONS = std::move(orig_refinement_cons);
  REFINE_DERIV_MINANGLE = orig_angle;

  return is_reachable;
}


// with partitioning level, didn't work
symb_states_type
automaton::get_reach_set_forwarditer(int delta) {
  // refine partitioning by successively increasing partition level by delta
  stopwatch sw(2001, "get_reach_set_forwarditer");

  symb_states_type dummystates;
  bool dummybool;
  symb_states_type ss,ss_old;
  int max_level = REFINE_PARTITION_LEVEL_MAX;

  REFINE_PARTITION_LEVEL_MAX = 0; //3;
  bool unlocked_surface = true;
  int unl_level = 0;

  bool first_time=true;
  std::ofstream file_out;
  const std::string filename = "out_surface_";

  int iteration_count = 0;
  while (unlocked_surface) {
    ++iteration_count;
    REFINE_PARTITION_LEVEL_MAX += delta;
    message(16100,"Setting max. partition level to "
            + int2string(REFINE_PARTITION_LEVEL_MAX));
    ss_old = ss;
    ss = get_reach_set(ini_states, dummystates, dummybool);
    loc_ref_set lrs;
    // add all surface locs to lrs (assign locs to surface)
    // and get min. level of not fully refined loc
    unl_level = add_surface_locations(ss, lrs);
    cout << "unl:" << unl_level << "fr:" << is_fully_refined(lrs);
    if (!is_fully_refined(lrs)
        && (first_time || unl_level <= REFINE_PARTITION_LEVEL_MAX)) {
      unlocked_surface = true;
      REFINE_PARTITION_LEVEL_MAX = unl_level;
    } else
      unlocked_surface = false;
    first_time = false;
  }

  REFINE_PARTITION_LEVEL_MAX = max_level;
  return ss;
}
