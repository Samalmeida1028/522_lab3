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

#include "parameters.hh"

// global variables
int FP_PRECISION = 24;
int GLOBAL_ITER_COUNT = 0;
bool PARSER_FIX_DPOST = false;

unsigned VERBOSE_LEVEL = 8001;
unsigned DEBUG_OUTPUT = 0;

/*
  MEMORY_MODE is a level:
  [-inf, 0]  -->  no attempt to save memory
  [1, +inf]  -->  do NOT cache entry/exit state in transitions
  [2, +inf]  -->  do NOT cache time post poly in locations
  [3, +inf]  -->  keep only (minimized) constraints
                  in locations' invariants and transactions' relations
  [4, +inf]  -->  keep only (minimized) constraints in symb_states
*/
int MEMORY_MODE = 3;

int TIME_POST_ITER = 0;
pplite::Rational TIME_POST_CONE_LAMBDA = pplite::Rational(1);
unsigned TIME_POST_CONE_WIDEN_DELAY = 3;
unsigned TIME_POST_CONE_WIDEN_PRECISION = 2;

bool INTERSECT_MINIMIZED = true;
bool ADD_CONSTRAINT_MINIMIZED =true;
bool BLOCK_OVERAPPROXIMATION = false; // this can be switched on by the search algorithm to improve convergence

// Experimental: maintain tight boxes on operations such as intersection,
// poly hull, con hull, unconstrain, affine images, is_disjoint.
bool MAINTAIN_BOXED_CCVS = false;

// Reachability
bool REACH_CHEAP_CONTAINS = true;
bool REACH_CHEAP_CONTAINS_USE_BBOX = true;
bool REACH_USE_BBOX = false;
bool REACH_USE_CONSTRAINT_HULL = false;
bool REACH_USE_CONVEX_HULL = false;
bool REACH_USE_TIME_ELAPSE = true;
bool REACH_STOP_AT_FORB = false; // check after each iteration and stop reachability if forbidden states encountered
int REACH_MAX_ITER = 0;
int REACH_REPORT_INTERVAL = 10;
int REACH_USE_BBOX_ITER = 1000000000;
int REACH_STOP_USE_CONVEX_HULL_ITER = 1000000000;
bool REACH_STOP_USE_CONVEX_HULL_SETTLE = false;

int REACH_CONSTRAINT_TRIGGER = 0;
int REACH_CONSTRAINT_LIMIT = 0;
int TP_CONSTRAINT_LIMIT = 0;
int LIMIT_CONSTRAINTS_METHOD = 1;
int REACH_BITSIZE_TRIGGER = 0;
int CONSTRAINT_BITSIZE = 0;

// Default search method is based on topological sorting
// of reachable only states (numbered 7 in old phaver).
Search_Method SEARCH_METHOD = Search_Method::topsort_reachable;
int SEARCH_METHOD_TOPSORT_TOKENS = 1;

int SNAPSHOT_INTERVAL = 0; // output snapshot every x iterations
bool DEADLOCK_CHECKING = false;

// Refinement
bool REFINE_CHECK_TIME_RELEVANCE = true; // check the time relevance of newly created transitions
bool REFINE_CHECK_TIME_RELEVANCE_DURING = false; // check the time relevance of existing transitions for cells during the refinement process
bool REFINE_CHECK_TIME_RELEVANCE_FINAL = false; // check the time relevance of existing transitions for cells that are completely refined
bool REFINE_CHECK_TRANS_DIMS = false; // not sound!
int  REFINE_DERIVATIVE_METHOD = 2; // 0 = constraint-based, 1 = bb of 0, 2 = projection-based, 3 = bb of 2
bool REFINE_PRIORITIZE_REACH_SPLIT = false;
bool REFINE_PRIORITIZE_ANGLE = false;
bool REFINE_TEST_REACH_SPLIT = true;
bool REFINE_SMALLEST_FIRST = false;
bool REFINE_USE_FP = false;
double REFINE_DERIV_MINANGLE = 1;
int  REFINE_PARTITION_LEVEL_MAX = -1;
int  REFINE_PARTITION_LEVEL_DELTA = 1;
bool REFINE_PARTITION_INSIDE = false;
int REACH_FB_REFINE_METHOD = 1; // 1: refine 1 constraint at a time, 0: refine all constraints at a time
bool REFINE_FORCE_SPLITTING = true;
int REFINE_MAX_CHECKS = 0;
bool REFINE_USE_NEW_SPLIT = false;

unsigned GENERATOR_TO_DOUBLE_PRECISION = 0;

unsigned line_number = 1;
