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

#ifndef GUARD_parameters_hh
#define GUARD_parameters_hh

#ifndef PHAVERLITE_STATS
#define PHAVERLITE_STATS 0
#endif

/*
  Verbose levels:
  &1==0  no timers
  &1!=0  timers
  &10==0  no progress dots
  &10!=0  progress dots

  1000    main function calls
  2000    main function calls + results
  4000    sublevel 1 calls
  8000    sublevel 1 calls + results
  16000   sublevel 2 calls
  32000   sublevel 2 calls + results
  64000   sublevel 3 calls
  128000  sublevel 3 calls + results
*/

extern unsigned VERBOSE_LEVEL;
extern unsigned DEBUG_OUTPUT;
extern int MEMORY_MODE;

extern int GLOBAL_ITER_COUNT;
extern int FP_PRECISION;

// When true, the parser will automatically add identity assignments
// (i.e., x' = x) on all controlled variables that are NOT mentioned
// in dpost constraints; when false, the parser returns an error.
extern bool PARSER_FIX_DPOST;

extern int TIME_POST_ITER;
#include <pplite/pplite.hh>
extern pplite::Rational TIME_POST_CONE_LAMBDA;
extern unsigned TIME_POST_CONE_WIDEN_DELAY;
extern unsigned TIME_POST_CONE_WIDEN_PRECISION;

extern bool INTERSECT_MINIMIZED;
extern bool ADD_CONSTRAINT_MINIMIZED;
// this can be switched on by the search algorithm to improve convergence
extern bool BLOCK_OVERAPPROXIMATION;

// Experimental.
extern bool MAINTAIN_BOXED_CCVS;

// Reachability
extern bool REACH_CHEAP_CONTAINS;
extern bool REACH_CHEAP_CONTAINS_USE_BBOX;
extern bool REACH_USE_BBOX;
extern bool REACH_USE_CONSTRAINT_HULL;
extern bool REACH_USE_CONVEX_HULL;
// Can be turned off for discrete time systems
extern bool REACH_USE_TIME_ELAPSE;
extern bool REACH_STOP_AT_FORB; // check after each iteration and stop reachability if forbidden states encountered
extern int REACH_MAX_ITER;
extern int REACH_REPORT_INTERVAL;
extern int REACH_USE_BBOX_ITER;
extern int REACH_STOP_USE_CONVEX_HULL_ITER;
extern bool REACH_STOP_USE_CONVEX_HULL_SETTLE;

extern int REACH_CONSTRAINT_TRIGGER;
extern int REACH_CONSTRAINT_LIMIT;
extern int TP_CONSTRAINT_LIMIT;
extern int LIMIT_CONSTRAINTS_METHOD;  // 0: maxdelta, 1: angle
extern int REACH_BITSIZE_TRIGGER;
extern int CONSTRAINT_BITSIZE;

enum class Search_Method { trx_based, topsort, topsort_reachable };
extern Search_Method SEARCH_METHOD;
extern int SEARCH_METHOD_TOPSORT_TOKENS;

extern int SNAPSHOT_INTERVAL; // output snapshot every x iterations
extern bool DEADLOCK_CHECKING;

// Refinement
extern bool REFINE_CHECK_TIME_RELEVANCE; // check the time relevance of newly created transitions
extern bool REFINE_CHECK_TIME_RELEVANCE_DURING; // check the time relevance of existing transitions for cells during the refinement process
extern bool REFINE_CHECK_TIME_RELEVANCE_FINAL; // check the time relevance of existing transitions for cells that are completely refined
extern bool REFINE_CHECK_TRANS_DIMS; // not sound!
extern int  REFINE_DERIVATIVE_METHOD; // 0 = constraint-based, 1 = bb of 0, 2 = projection-based, 3 = bb of 2
extern bool REFINE_PRIORITIZE_REACH_SPLIT;
extern bool REFINE_PRIORITIZE_ANGLE;
extern bool REFINE_TEST_REACH_SPLIT;
extern bool REFINE_SMALLEST_FIRST;
extern bool REFINE_USE_FP;
extern double REFINE_DERIV_MINANGLE;
extern int  REFINE_PARTITION_LEVEL_MAX;
extern int  REFINE_PARTITION_LEVEL_DELTA;
extern bool REFINE_PARTITION_INSIDE;
extern int REACH_FB_REFINE_METHOD; // 1: refine 1 constraint at a time, 0: refine all constraints at a time
extern bool REFINE_FORCE_SPLITTING;
extern int REFINE_MAX_CHECKS;
extern bool REFINE_USE_NEW_SPLIT;

extern unsigned GENERATOR_TO_DOUBLE_PRECISION;

// Used in the parser.
extern unsigned line_number;

#endif // GUARD_parameters_hh
