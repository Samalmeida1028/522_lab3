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

#include "symb_states.hh"
#include "stopwatch.hh"

void
symb_state_maplist::clear() {
  state_list.clear();
  iter_map.clear();
}

void
symb_state_maplist::initialize(const symb_states_type& states,
                               symb_state_plist& newstates,
                               bool use_convex_hull) {
  clear();
  for (const auto& p : states)
    add(p.first, p.second, use_convex_hull, nullptr, &newstates);
}

symb_state_maplist::iterator
symb_state_maplist::erase(iterator it) {
  // first remove it from iter_map
  loc_ref loc = it->loc;
  auto m_iter = iter_map.find(loc);
  if (m_iter != iter_map.end()) {
    auto& m_vect = m_iter->second;
    auto pos = std::find(m_vect.begin(), m_vect.end(), it);
    if (pos != m_vect.end())
      m_vect.erase(pos);
    if (m_vect.empty())
      iter_map.erase(loc);
  }
  return state_list.erase(it);
}

symb_state_maplist::iterator
symb_state_maplist::add(loc_ref loc, clock_val_set new_cvs,
                        bool use_convex_hull,
                        symb_state_plist* check_states_ptr,
                        symb_state_plist* new_states_ptr) {
  if (new_cvs.size() > 1) {
    symb_state_maplist::iterator res;
    for (auto& new_ccvs : new_cvs.ccvs_list)
      res = add(loc, clock_val_set(new_ccvs),
                use_convex_hull, check_states_ptr, new_states_ptr);
    return res;
  }
#if PHAVERLITE_STATS
  static pplite::Local_Stats stats("sstate::add");
  pplite::Local_Clock clock(stats);
#endif
  // Check if iter_map already contains loc.
  auto m_iter = iter_map.find(loc);

  if (m_iter == iter_map.end()) {
    // Not found: add it to front of list.
    if (use_convex_hull)
      new_cvs.convex_hull_assign();
    if (MEMORY_MODE >= 4)
      new_cvs.minimize_memory();
    state_list.emplace_front(loc, std::move(new_cvs));
    auto s_it = state_list.begin();
    iter_map[loc].push_back(s_it);
    // No need to update check_states.
    if (new_states_ptr)
      new_states_ptr->push_back(s_it);
    return s_it;
  }

  // Found loc: get its iter container.
  auto& m_cont = m_iter->second;
  return use_convex_hull
    ? convex_add(m_cont, std::move(new_cvs), new_states_ptr)
    : set_add(m_cont, std::move(new_cvs),
              check_states_ptr, new_states_ptr);
}

symb_state_maplist::iterator
symb_state_maplist::set_add(symb_state_pvect& m_cont,
                            clock_val_set new_cvs,
                            symb_state_plist* check_states_ptr,
                            symb_state_plist* new_states_ptr) {
  assert(!m_cont.empty());
  const auto loc = m_cont[0]->loc;
  assert(new_cvs.ccvs_list.size() == 1);
  const auto& new_ccvs = new_cvs.ccvs_list.front();

#if PHAVERLITE_STATS
  auto sz = m_cont.size();
  auto& bbc = bbox_contains_stat.time_stats.counter;
  auto& bbc_ph = bbox_contains_ph_stat.time_stats.counter;
  auto old_bbc = bbc;
  auto old_bbc_ph = bbc_ph;
#endif // PHAVERLITE_STATS

#define ENEA_ORDERING 1

#if ENEA_ORDERING
  auto eq_range = std::equal_range(m_cont.begin(), m_cont.end(), new_ccvs);
  // Check for subsumption in [eq_range.first, end).
  for (auto m_it = eq_range.first; m_it != m_cont.end(); ++m_it) {
#else
  for (auto m_it = m_cont.begin(); m_it != m_cont.end(); ++m_it) {
#endif
    const auto& old_ccvs = (*m_it)->cvs.ccvs_list.front();
    if (old_ccvs.boxed_contains(new_ccvs)) {
#if PHAVERLITE_STATS
      {
        auto new_bbc = bbc;
        auto new_bbc_ph = bbc_ph;
        std::cerr << "list size " << sz
                  << " subsumed"
                  << " boxed contains calls " << (new_bbc - old_bbc)
                  << " ph contains calls " << (new_bbc_ph - old_bbc_ph)
                  << " \n";
      }
#endif // PHAVERLITE_STATS
      return end();
    }
  }

  // new_cvs is not subsumed.
  // Detect (without erasing) elements made redundant by new_ccvs.
  pplite::Index_Set tbr;
#if ENEA_ORDERING
  for (auto m_it = m_cont.begin(); m_it != eq_range.second; ++m_it) {
#else
  for (auto m_it = m_cont.begin(); m_it != m_cont.end(); ++m_it) {
#endif
    const auto& old_ccvs = (*m_it)->cvs.ccvs_list.front();
    if (new_ccvs.boxed_contains(old_ccvs))
      tbr.set(std::distance(m_cont.begin(), m_it));
  }

  // Now erase redundant elements
  for (auto idx : tbr) {
    auto rem_it = m_cont[idx];
    if (check_states_ptr)
      check_states_ptr->remove(rem_it);
    state_list.erase(rem_it);
  }
  // Precompute insertion position for new_cvs.
#if ENEA_ORDERING
  auto m_pos = eq_range.second - tbr.size();
#endif
  pplite::erase_using_sorted_indices(m_cont, tbr);
#if PHAVERLITE_STATS
  {
    auto new_bbc = bbc;
    auto new_bbc_ph = bbc_ph;
    auto new_sz = m_cont.size();
    std::cerr << "list old size " << sz
              << " erased " << (sz - new_sz)
              << " not-subsumed"
              << " boxed contains calls " << (new_bbc - old_bbc)
              << " ph contains calls " << (new_bbc_ph - old_bbc_ph)
              << " \n";
  }
#endif // PHAVERLITE_STATS

  // Add new_cvs.
  if (MEMORY_MODE >= 4)
    new_cvs.minimize_memory();
  state_list.emplace_front(loc, std::move(new_cvs));
  auto s_it = state_list.begin();
#if ENEA_ORDERING
  m_cont.insert(m_pos, s_it);
#else
  m_cont.insert(m_cont.begin(), s_it);
#endif
  if (new_states_ptr)
    new_states_ptr->push_back(s_it);
  return s_it;

}

symb_state_maplist::iterator
symb_state_maplist::convex_add(symb_state_pvect& m_cont,
                               clock_val_set new_cvs,
                               symb_state_plist* new_states_ptr) {
  assert(m_cont.size() == 1);
  auto it = m_cont.front();
  auto& cvs = it->cvs;
  // Check for subsumption.
  if (cvs.contains_return_others(new_cvs))
    return end();
  // Not subsumed.
  cvs.convex_hull_assign(new_cvs);
  if (MEMORY_MODE >= 4)
    cvs.minimize_memory();
  if (new_states_ptr) {
    if (new_states_ptr->find(it) == new_states_ptr->end())
      new_states_ptr->push_back(it);
  }
  return it;
}

bool
symb_state_maplist::is_empty(loc_ref loc) const {
  return iter_map.find(loc) == iter_map.end();
}

bool
symb_state_maplist::is_disjoint_from(const symb_states_type& states) const {
  for (const auto& p : states) {
    loc_ref loc = p.first;
    const auto& cvs = p.second;
    auto m_iter = iter_map.find(loc);
    if (m_iter != iter_map.end()) {
      auto& m_list = m_iter->second;
      for (auto it : m_list) {
        if (!cvs.is_disjoint_from(it->cvs))
          return false;
      }
    }
  }
  return true;
}

symb_state_plist
symb_state_maplist::get_all_states() {
  symb_state_plist res;
  // Note: copying iterators, not elements!
  for (auto i = begin(); i != end(); ++i)
    res.push_back(i);
  return res;
}

void
symb_state_maplist::print() {
  for (auto p : iter_map) {
    std::cout << "Location " << p.first << ":" << std::endl;
    for (auto it : p.second)
      it->print();
  }
}

size_t
symb_state_maplist::cvs_size() const {
  size_t sz = 0;
  for (auto p : iter_map)
    for (auto it : p.second)
      sz += it->cvs.size();
  return sz;
}

clock_val_set
symb_state_maplist::get_clock_val_set(loc_ref loc) const {
  if (state_list.empty())
    return clock_val_set(0, Spec_Elem::EMPTY);
  auto m_iter = iter_map.find(loc);
  if (m_iter == iter_map.end()) {
    auto sd = state_list.front().cvs.dim;
    return clock_val_set(sd, Spec_Elem::EMPTY);
  }
  // *this is not empty, and iter_map contains loc
  const auto& m_list = m_iter->second;
  assert(!m_list.empty());
  // Copy first.
  auto it = m_list.begin();
  clock_val_set res = (*it)->cvs;
  // Merge others (starting from second).
  for (++it; it != m_list.end(); ++it)
    res.union_assign((*it)->cvs);
  return res;
}

symb_states_type
symb_state_maplist::transfer_symb_states() {
  symb_states_type states;
  for (auto p : iter_map) {
    loc_ref loc = p.first;
    states[loc] = get_clock_val_set(loc);
  }
  clear();
  return states;
}

void
symb_state_maplist::split(loc_ref oldloc,
                          const clock_val_set& oldinv,
                          loc_ref newloc,
                          const clock_val_set& newinv,
                          symb_state_plist& check_states,
                          symb_state_plist& new_states,
                          bool use_convex_hull) {
  clock_val_set cvs;

  // ENEA: FIXME: why iterating the list without using the map?
  for (auto it_old = begin(); it_old != end(); ) {
    if (it_old->loc != oldloc) {
      ++it_old;
      continue;
    }
    // add intersection with newinv to maplist and get iterator itnew
    cvs.intersection_assign_from(it_old->cvs, newinv);
    auto it_new = cvs.is_empty()
      ? end()
      : add(newloc, std::move(cvs), use_convex_hull, nullptr, nullptr);
    if (it_new != end()) {
      // intersect with invariant of oldloc
      it_old->cvs.intersection_assign(oldinv);
      // if it_old is in newstates/checkstates, add it_new
      if (new_states.contains(it_old) && !new_states.contains(it_new))
        new_states.insert(new_states.find(it_old), it_new);
      if (check_states.contains(it_old) && !check_states.contains(it_new))
        check_states.insert(check_states.find(it_old), it_new);
    }

    // erase it_old if it's empty now
    if (!it_old->cvs.is_empty())
      ++it_old;
    else {
      // erase it.
      new_states.remove(it_old);
      check_states.remove(it_old);
      it_old = erase(it_old);
    }
  }
}
