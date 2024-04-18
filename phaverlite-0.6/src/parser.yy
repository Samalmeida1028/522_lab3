/* PHAVerLite: PHAVer + PPLite. -*- C++ -*-
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

%{
#include "parameters.hh"
#include "convex_clock_val_set.hh"
#include "clock_val_set.hh"
#include "stopwatch.hh"
#include "symb_states_type.hh"
#include "automaton.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>

#ifndef NDEBUG
#define YYDEBUG 1
#endif

#include "parser.hh"

extern FILE* yyin;
int yylex();

void
yyerror(const std::string& str) {
  std::cout << "Parse error in line " << line_number << ": ";
  std::cout << str << "\n";
  exit(1);
}

inline int
atoi_consume(const std::string* ptr) {
  int res = atoi(ptr->c_str());
  delete ptr;
  return res;
}

inline void
check_ident_consume(const std::string* ptr,
                    const std::string& expected) {
  const auto& parsed = *ptr;
  if (parsed != expected)
    yyerror("Expecting `" + expected + "', found `" + parsed + "'.");
  delete ptr;
}

void
parse_file(const std::string& filename) {
  /* doesn't deal with recursive loads */
  yyin = fopen(filename.c_str(), "r");
  if (yyin == nullptr) {
    std::cout << "*** Error opening `" << filename << "' ***\n";
    exit(1);
  }
  stopwatch sw(2001, "file " + filename);
  line_number = 1;
  yyparse();
  fclose(yyin);
}

Cons
identity_dpost(dim_type dim, const var_ref_set& contr_vars) {
  Cons dpost;
  for (auto vid : contr_vars)
    dpost.push_back(Var(vid) == Var(vid + dim));
  return dpost;
}

var_ref_set
get_dpost_unconstrained(dim_type dim, const Cons& dpost,
                        const var_ref_set& contr_vars) {
  var_ref_set res = contr_vars;
  for (const auto& c : dpost) {
    for (auto j = c.space_dim(); j-- > dim; ) {
      if (c.coeff(Var(j)) != 0)
        res.erase(j - dim);
    }
  }
  return res;
}

void
check_dpost(dim_type dim, const Cons& dpost,
            const var_ref_set& contr_vars) {
  auto not_seen = get_dpost_unconstrained(dim, dpost, contr_vars);
  if (!not_seen.empty())
    yyerror("Discrete post does not mention a controlled variable.");
}

void
maybe_fix_dpost(dim_type dim, Cons& dpost, const var_ref_set& contr_vars) {
  if (!PARSER_FIX_DPOST)
    return;
  auto not_seen = get_dpost_unconstrained(dim, dpost, contr_vars);
  if (not_seen.empty())
    return;
  auto fix = identity_dpost(dim, not_seen);
  dpost.insert(dpost.end(), fix.begin(), fix.end());
}

automaton* paut;
symb_states_type* psymb_state;
dim_type val_set_dim;

using RAE_Map = std::map<std::string, Rat_Affine_Expr>;
RAE_Map rae_map;

// Note: these are NON-owning (automaton) pointers.
using SS_Map = std::map<std::string, std::pair<symb_states_type, automaton*>>;
SS_Map ss_map;

// Note: these are owning (automaton) pointers.
using PAUT_Map = std::map<std::string, std::unique_ptr<automaton>>;
PAUT_Map paut_map;

std::string outfilename = "out";
std::ofstream file_out;
std::string current_loc_name;

using std::string;
using std::make_pair;
using std::cout;
using std::endl;

using pplite::Con;
using pplite::Cons;

%}

%union{
 std::string* mystring;
 pplite::Con* con;
 pplite::Cons* con_list;
 RefinementCon* refinement_con;
 RefinementCons* refinement_cons;
 var_ref_set* vrs;
 clock_val_set* cvs;
 std::list<std::string>* ident_list;
 bool mybool;
 Rat_Affine_Expr* rae;
 symb_states_type* symb_state;
};

// Choosing the polyhedron domain
%token par_POLY_KIND

%token MERGE_SPLITTED
%token ASAP
%token par_TIME_POST_ITER
%token par_TIME_POST_CONE_LAMBDA
%token par_TIME_POST_CONE_WIDEN_DELAY
%token par_TIME_POST_CONE_WIDEN_PRECISION
%token par_MEMORY_MODE

%token par_MAINTAIN_BOXED_CCVS

// Parser
%token par_PARSER_FIX_DPOST

// Reachability
%token par_REACH_CHEAP_CONTAINS
%token par_REACH_CHEAP_CONTAINS_USE_BBOX
%token par_REACH_USE_BBOX
%token par_REACH_USE_CONSTRAINT_HULL
%token par_REACH_USE_CONVEX_HULL
%token par_REACH_USE_TIME_ELAPSE
%token par_REACH_STOP_AT_FORB
%token par_REACH_MAX_ITER
%token par_REACH_REPORT_INTERVAL;
%token par_REACH_USE_BBOX_ITER
%token par_REACH_STOP_USE_CONVEX_HULL_ITER
%token par_REACH_STOP_USE_CONVEX_HULL_SETTLE
%token par_REACH_CONSTRAINT_LIMIT
%token par_LIMIT_CONSTRAINTS_METHOD
%token par_TP_CONSTRAINT_LIMIT
%token par_REACH_CONSTRAINT_TRIGGER
%token par_REACH_BITSIZE_TRIGGER
%token par_CONSTRAINT_BITSIZE
%token par_SEARCH_METHOD
%token par_SEARCH_METHOD_TOPSORT_TOKENS
%token par_SNAPSHOT_INTERVAL

// Refinement
%token par_REFINE_DERIVATIVE_METHOD
%token par_REFINE_PRIORITIZE_REACH_SPLIT;
%token par_REFINE_SMALLEST_FIRST;
%token par_REFINE_USE_FP;
%token par_REFINE_DERIV_MINANGLE;
%token par_REFINE_PRIORITIZE_ANGLE;
%token par_REFINE_CHECK_TIME_RELEVANCE;
%token par_REFINE_CHECK_TIME_RELEVANCE_DURING;
%token par_REFINE_CHECK_TIME_RELEVANCE_FINAL;
%token par_REFINE_CHECK_TRANS_DIMS;
%token par_REFINE_PARTITION_INSIDE;
%token par_REACH_FB_REFINE_METHOD;
%token par_REFINE_MAX_CHECKS;
%token par_REFINE_USE_NEW_SPLIT;
%token par_MINIMIZE_FILTER_THRESHOLD;

%token PROJECT_TO
%token DIFFERENCE_ASSIGN
%token RENAME
%token IS_REACHABLE
%token IS_REACHABLE_FB
%token INVARIANT_ASSIGN
%token REVERSE
%token SAVE_FP_SURFACE
%token REACH_FORWARD_ITER
%token UNLOCK_SURFACE_LOCS
%token UNLOCK_LOCS
%token GET_INVARIANTS
%token PRINT_GRAPH
%token INVERSE
%token CONTAINS
%token REFINE_CONSTRAINTS
%token ADD_LABEL
%token WHO
%token SAVE_FP_INVARS
%token INITIAL_STATES
%token REFINE_LOCS
%token REFINE_LOC_DERIV
%token IS_EMPTY
%token IS_INTERSECTING
%token INTERSECTION_ASSIGN
%token LOC_INTERSECTION
%token LOC_UNION
%token REMOVE
 // %token STRING_TEXT
%token MY_ECHO
%token GET_PARAMETERS
%token PRINT
%token SAVE_CON_FP
%token SAVE_GEN_FP
%token REACH
%token REACH_STOP
%token DEFNE
%token AUTOMATON
%token INTERNAL_VAR
%token EXTERNAL_VAR
%token PARAMETER
%token SYNCLABS
%token INITIALLY
%token LOC
%token WHILE
%token WHEN
%token TRUE
%token FALSE
%token ASSIGN

%token <mystring> INT
%token <mystring> IDENT
%token <mystring> STARIDENT
%token <mystring> STRING_TEXT
//%nonassoc '(' ')'
%left '|' '&'
%left GE LE EQ '<' '>'
%left '+' '-'
%left '*' '/'
%left '!'
%left '(' ')'
%left PRIM

%type <con> constr
%type <con_list> constr_list
%type <con_list> constr_list_no_and
%type <refinement_con> refinement_con
%type <refinement_cons> refinement_cons
%type <vrs> var_ref_list
%type <cvs> val_set
%type <cvs> state_val_set
%type <con_list> dpost_cons
%type <ident_list> ident_list
%type <ident_list> compose_list
%type <mybool> bool_type
%type <rae> rat_aff_expr
%type <symb_state> state_list

%%

program:
  commands
	;

commands: commands command
	| command
  ;

///////////////////////////////////////////////////////////////////////////

command:
    prelim
  | automaton
  // --------------------------------------------------------
  // General commands
  // --------------------------------------------------------
  | MY_ECHO STRING_TEXT ';'
    { cout << *($2) << endl; }
  | WHO ';'
    { cout << "Automata in memory:" << endl;
      for (const auto& p : paut_map) {
        cout << p.first << " ";
        cout << "(" << (p.second)->name << "):";
        p.second->print_size();
      }
      cout << endl << "Constants in memory:" << endl;
      for (const auto& p : rae_map) {
        cout << p.first << " := " << p.second << endl;
      }
      cout << endl << "Symbolic states in memory:" << endl;
      for (const auto& p : ss_map) {
        cout << p.first << " : " << p.second.first.size() << " locs, "
             << p.second.first.get_memory() << " bytes" << endl;
      }
      cout << endl;
    }

  // --------------------------------------------------------
  // Copy commands
  // --------------------------------------------------------
  | IDENT '=' IDENT ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      if (ss_map.find(id) != ss_map.end()) {
        ss_map[dst] = ss_map[id];
      } else if (paut_map.find(id) != paut_map.end()) {
        paut = new automaton(*paut_map[id]);
        paut->name = dst;
        paut_map[dst].reset(paut);
      } else
        yyerror("Identifier " + id + " not found.");
      delete $1;
      delete $3;
    }
  | IDENT '.' PRINT ';'
    {
      const auto& id = *($1);
      if (ss_map.find(id) != ss_map.end()) {
        if (ss_map[id].second != nullptr) {
          cout << id << " = "
               << ss_map[id].second->name << ".";
          if (ss_map[id].first.empty())
            cout << "{};" << endl;
          else {
            cout << "{" << endl;
            ss_map[id].first.print_phaver(cout);
            cout << endl << "};" << endl;
          }
        } else
          ss_map[id].first.print_phaver(cout);
      } else if (paut_map.find(id) != paut_map.end()) {
        paut_map[id]->print_phaver(cout);
        // paut_map[id]->print();
      } else
        yyerror("Identifier " + id + " not found.");
      delete $1;
    }
  | IDENT '.' PRINT '(' STRING_TEXT ',' INT ')' ';'
    {
      const auto& id = *($1);
      const auto& fname = *($5);
      int format = atoi_consume($7);
      if (ss_map.find(id) != ss_map.end()) {
        file_out.open(fname);
        file_out.precision(FP_PRECISION);
        if (format == 1) // constraint form
          ss_map[id].first.print_con_fp_raw(file_out);
        else if (format == 2) // generator form
          ss_map[id].first.print_gen_fp_raw(file_out);
        else if (format == 3) // debug form
          ss_map[id].first.print();
        else if (format == 4) // debug form
          ss_map[id].first.print(ss_map[id].second->get_loc_names());
        else {
          file_out << id << " = "
                   << ss_map[id].second->name << ".";
          if (ss_map[id].first.empty())
            file_out << "{};" << endl;
          else {
            file_out << "{" << endl;
            ss_map[id].first.print_phaver(file_out);
            file_out << endl << "};";
          }
        }
        file_out.close();
      } else if (paut_map.find(id) != paut_map.end()) {
        file_out.open(fname);
        if (format == 1) // dot form (graphviz)
          paut_map[id]->print_dot(file_out);
        else
          paut_map[id]->print_phaver(file_out);
        file_out.close();
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete $1;
      delete $5;
    }
  | IDENT '.' SAVE_CON_FP '[' STRING_TEXT ']' ';'
    {
      const auto& id = *($1);
      const auto& fname = *($5);
      if (ss_map.find(id) != ss_map.end()) {
        file_out.open(fname);
        file_out.precision(FP_PRECISION);
        clock_val_set temp_cvs = ss_map[id].first.union_over_locations();
        temp_cvs.print_con_fp_raw(file_out);
        file_out.close();
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete $1;
      delete $5;
    }
  // --------------------------------------------------------
  // symbolic_states_type commands
  // --------------------------------------------------------
  | IDENT '=' IDENT '.'
    {
      const auto& id = *($3);
      if (paut_map.find(id) != paut_map.end())
        paut = paut_map[id].get();
      else
        yyerror("Automaton '" + id + "' not found.");
    }
    '{' state_list '}' ';'
    {
      const auto& dst = *($1);
      const auto& states = *($7);
      ss_map[dst] = make_pair(std::move(states), paut);
      delete $1;
      delete $3;
      delete $7;
    }
  | IDENT '.'
    {
      const auto& id = *($1);
      if (ss_map.find(id) != ss_map.end()) {
        psymb_state = &ss_map[id].first;
        paut = ss_map[id].second;
      } else {
        yyerror("Unknown state identifier '" + id +"'.");
        psymb_state = nullptr;
      }
    } // needed so var_ref_list can refer to the right variables
    REMOVE '(' var_ref_list ')' ';'
    {
      const auto& id = *($1);
      if ($6 != nullptr) {
        if (ss_map.find(id) != ss_map.end())
          ss_map[id].first.remove_space_dimensions(*$6);
        else
          yyerror("Unknown identifier '" + id +"'.");
      }
      psymb_state = nullptr;
      delete $1;
      delete $6;
    }
  | IDENT '.'
    {
      const auto& id = *($1);
      if (ss_map.find(id) != ss_map.end()) {
        psymb_state = &ss_map[id].first;
        paut = ss_map[id].second;
      } else
        yyerror("Unknown symbolic state identifier '" + id +"'.");
      delete $1;
    } // needed so var_ref_list can refer to the right variables
    PROJECT_TO '(' var_ref_list ')' ';'
    {
      if ($6 != nullptr) {
        assert(psymb_state != nullptr);
        psymb_state->project_to_vars(*$6);
      }
      psymb_state = nullptr;
      paut = nullptr;
      delete $6;
    }
  | GET_PARAMETERS '(' IDENT ',' bool_type ')' ';'
    {
      const auto& id = *($3);
      if (ss_map.find(id) != ss_map.end()) {
        auto sstates = ss_map[id].first;
        const var_ref_set& vrs = ss_map[id].second->parameters;
        auto dim = ss_map[id].second->dim;
        sstates.remove_space_dimensions(vrs.range_complement(0, dim));
        clock_val_set tmp;
        if ($5) {
          cout << "Parameters in any of the locations:" << endl;
          tmp = sstates.union_over_locations();
        } else {
          cout << "Parameters common to all locations:" << endl;
          tmp = sstates.intersection_over_locations();
        }
        tmp.print();
      }
      else
        yyerror("Identifier " + id + " not found.");
      delete $3;
    }
  | IDENT '.' LOC_UNION ';'
    {
      const auto& id = *($1);
      if (ss_map.find(id) != ss_map.end()) {
        const auto& mycvs = ss_map[id].first.union_over_locations();
        mycvs.print(ss_map[id].first.var_names);
        mycvs.print_gen_fp_raw(cout);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete $1;
    }
  | IDENT '.' LOC_INTERSECTION ';'
    {
      const auto& id = *($1);
      if (ss_map.find(id) != ss_map.end()) {
        const auto& mycvs = ss_map[id].first.intersection_over_locations();
        mycvs.print(ss_map[id].first.var_names);
        mycvs.print_gen_fp_raw(cout);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete $1;
    }
  | IDENT '=' IDENT '.' LOC_UNION ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      if (ss_map.find(id) != ss_map.end()) {
        auto mycvs = ss_map[id].first.union_over_locations();
        symb_states_type ss(ss_map[id].first.var_names);
        ss.add("$", std::move(mycvs));
        ss_map[dst] = make_pair(std::move(ss), ss_map[id].second);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete $1;
      delete $3;
    }
  | IDENT '=' IDENT '.' LOC_INTERSECTION ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      if (ss_map.find(id) != ss_map.end()) {
        auto mycvs = ss_map[id].first.intersection_over_locations();
        symb_states_type ss(ss_map[id].first.var_names);
        ss.add("$", std::move(mycvs));
        ss_map[dst] = make_pair(std::move(ss), ss_map[id].second);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete $1;
      delete $3;
    }
  | IDENT '=' IDENT '.' MERGE_SPLITTED ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      if (ss_map.find(id) != ss_map.end()) {
        auto sstates = ss_map[id].first.merge_splitted();
        ss_map[dst] = make_pair(std::move(sstates), nullptr);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete $1;
      delete $3;
    }
  | IDENT '.' IS_INTERSECTING '(' IDENT ')' ';'
    {
      const auto& id1 = *($1);
      const auto& id2 = *($5);
      if (ss_map.find(id1) != ss_map.end()) {
        if (ss_map.find(id2) != ss_map.end()) {
          if (ss_map[id1].first.is_intersecting(ss_map[id2].first))
            std::cout << id1 << " is intersecting " << id2 << endl;
          else
            std::cout << id1 << " and " << id2 << " are disjoint" << endl;
        } else
          yyerror("Unknown identifier '" + id2 +"'.");
      } else
        yyerror("Unknown identifier '" + id1 +"'.");
      delete $1;
      delete $5;
    }
  | IDENT '.' INTERSECTION_ASSIGN '(' IDENT ')' ';'
    {
      const auto& id1 = *($1);
      const auto& id2 = *($5);
      if (ss_map.find(id1) != ss_map.end()) {
        if (ss_map.find(id2) != ss_map.end())
          ss_map[id1].first.intersection_assign(ss_map[id2].first);
        else
          yyerror("Unknown identifier '" + id2 +"'.");
      } else
        yyerror("Unknown identifier '" + id1 +"'.");
      delete $1;
      delete $5;
    }
  | IDENT '.' DIFFERENCE_ASSIGN '(' IDENT ')' ';'
    {
      const auto& id1 = *($1);
      const auto& id2 = *($5);
      if (ss_map.find(id1) != ss_map.end()) {
        if (ss_map.find(id2) != ss_map.end())
          ss_map[id1].first.difference_assign(ss_map[id2].first);
        else
          yyerror("Unknown identifier '" + id2 +"'.");
      } else
        yyerror("Unknown identifier '" + id1 +"'.");
      delete $1;
      delete $5;
    }
  | IDENT '.' CONTAINS '(' IDENT ')' ';'
    {
      const auto& id1 = *($1);
      const auto& id2 = *($5);
      if (ss_map.find(id1) != ss_map.end()) {
        if (ss_map.find(id2) != ss_map.end()) {
          if (ss_map[id1].first.contains(ss_map[id2].first))
            cout << id1 << " contains " << id2 << endl;
          else
            cout << id1 << " does not contain " << id2 << endl;
        } else
          yyerror("Unknown identifier '" + id2 +"'.");
      } else
        yyerror("Unknown identifier '" + id1 +"'.");
      delete $1;
      delete $5;
    }
  | IDENT '.' IS_EMPTY ';'
    {
      const auto& id = *($1);
      if (ss_map.find(id) != ss_map.end()) {
        if (ss_map[id].first.is_empty())
          cout << "empty" << endl;
        else
          cout << "not empty" << endl;
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete $1;
    }
  | IDENT '.' RENAME '(' IDENT ',' IDENT ')' ';'
    {
      const auto& id = *($1);
      const auto& name1 = *($5);
      const auto& name2 = *($7);
      if (ss_map.find(id) != ss_map.end()) {
        ss_map[id].first.rename_variable(name1, name2);
        // remove the reference to any automaton because now it's changed
        ss_map[id].second = nullptr;
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete $1;
      delete $5;
      delete $7;
    }
  // --------------------------------------------------------
  // automaton commands
  // --------------------------------------------------------
  | IDENT '=' compose_list ';'
    {
      const auto& comp_name = *($1);
      const auto& aut_names = *($3);
      assert(!aut_names.empty());
      bool first = true;
      for (const auto& aut_name : aut_names) {
        const auto iter = paut_map.find(aut_name);
        if (iter == paut_map.end())
          yyerror("Automaton '" + aut_name + "' not found.");
        assert(iter->second != nullptr);
        const automaton& aut = *(iter->second);
        if (first) {
          paut = new automaton(aut);
          first = false;
          continue;
        }
        automaton* pcomp = new automaton();
        compose_discrete(*pcomp, *paut, aut);
        std::swap(paut, pcomp);
        delete pcomp;
      }
      paut->name = comp_name;
      paut_map[comp_name].reset(paut);
      delete $1;
      delete $3;
    }
  | IDENT '.' SAVE_FP_INVARS '(' STRING_TEXT ')' ';'
    {
      const auto& id = *($1);
      const auto& fname = *($5);
      if (paut_map.find(id) != paut_map.end()) {
        file_out.open(fname);
        file_out.precision(FP_PRECISION);
        paut_map[id]->print_inv_fp_raw(file_out);
        file_out.close();
      } else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
      delete $5;
    }
  | IDENT '.' SAVE_FP_SURFACE '(' STRING_TEXT ')' ';'
    {
      const auto& id = *($1);
      const auto& fname = *($5);
      if (paut_map.find(id) != paut_map.end()) {
        file_out.open(fname);
        file_out.precision(FP_PRECISION);
        paut_map[id]->print_surface_fp_raw(file_out);
        file_out.close();
      }	else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
      delete $5;
    }
  | IDENT '.' PRINT_GRAPH '(' STRING_TEXT ','
    {
      const auto& id = *($1);
      psymb_state = nullptr;
      if (paut_map.find(id) != paut_map.end())
        paut = paut_map[id].get();
      else
        yyerror("Automaton '" + id + "' not found.");
    }
    '{' var_ref_list '}' ',' IDENT ')' ';'
    {
      const auto& id = *($1);
      const auto& fname = *($5);
      const auto& var_list = *($9);
      const auto& id2 = *($12);
      psymb_state = nullptr;
      if (paut_map.find(id) != paut_map.end()) {
        paut = paut_map[id].get();
        file_out.open(fname);
        file_out.precision(FP_PRECISION);
        if (ss_map.find(id2) != ss_map.end())
          paut->print_graph(file_out, var_list, ss_map[id2].first);
        else
          yyerror("Unknown identifier '" + id2 + "'.");
        file_out.close();
      } else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
      delete $5;
      delete $9;
      delete $12;
    }
  | IDENT '.' REVERSE ';'
    {
      const auto& id = *($1);
      if (paut_map.find(id) != paut_map.end())
        paut_map[id]->reverse();
      else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
    }
  | IDENT '.' ADD_LABEL '(' IDENT ')' ';'
    {
      const auto& id = *($1);
      const auto& lab = *($5);
      if (paut_map.find(id) != paut_map.end())
        paut_map[id]->add_label(lab);
      else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
      delete $5;
    }
  | IDENT '=' IDENT '.' REACH_FORWARD_ITER '(' INT ')' ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      int delta = atoi_consume($7);
      if (paut_map.find(id) != paut_map.end())
        ss_map[dst] = make_pair(paut_map[id]->get_reach_set_forwarditer(delta),
                                paut_map[id].get());
      else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
      delete $3;
    }
  | IDENT '=' IDENT '.' REACH ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      if (paut_map.find(id) != paut_map.end())
        ss_map[dst] = make_pair(paut_map[id]->get_reach_set(),
                                paut_map[id].get());
      else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
      delete $3;
    }
  | IDENT '.' INITIAL_STATES '(' IDENT ')' ';'
    {
      const auto& id = *($1);
      const auto& id_init = *($5);
      if (paut_map.find(id) != paut_map.end())
        paut = paut_map[id].get();
      else {
        yyerror("Automaton '" + id + "' not found.");
        paut = nullptr;
      }
      if (paut != nullptr) {
        if (ss_map.find(id_init) != ss_map.end())
          paut->ini_states_assign(ss_map[id_init].first);
        else
          yyerror("Unknown identifier '" + id_init +"'.");
      }
      delete $1;
      delete $5;
    }
  | IDENT '.' INVARIANT_ASSIGN '(' IDENT ')' ';'
    {
      const auto& id = *($1);
      const auto& id_inv = *($5);
      if (paut_map.find(id) != paut_map.end())
        paut = paut_map[id].get();
      else {
        yyerror("Automaton '" + id + "' not found.");
        paut = nullptr;
      }
      if (paut != nullptr) {
        if (ss_map.find(id_inv) != ss_map.end())
          paut->invariant_assign(ss_map[id_inv].first);
        else
          yyerror("Unknown identifier '" + id_inv +"'.");
      }
      delete $1;
      delete $5;
    }
  | IDENT '=' IDENT '.' REACH '(' IDENT ')' ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      const auto& id_ini = *($7);
      if (paut_map.find(id) != paut_map.end())
        paut = paut_map[id].get();
      else {
        yyerror("Automaton '" + id + "' not found.");
        paut = nullptr;
      }
      if (paut != nullptr) {
        if (ss_map.find(id_ini) != ss_map.end()) {
          symb_states_type dummystates;
          bool dummybool;
          ss_map[dst] = make_pair(paut->get_reach_set(ss_map[id_ini].first,
                                                      dummystates, dummybool),
                                  paut);
        }
        else
          yyerror("Unknown identifier '" + id_ini +"'.");
      }
      delete $1;
      delete $3;
      delete $7;
    }
  | IDENT '=' IDENT '.' IS_REACHABLE '(' IDENT ')' ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      const auto& target = *($7);
      if (paut_map.find(id) != paut_map.end())
        paut = paut_map[id].get();
      else {
        yyerror("Automaton '" + id + "' not found.");
        paut = nullptr;
      }
      if (paut != nullptr) {
        if (ss_map.find(target) != ss_map.end()) {
          symb_states_type sstates;
          if (paut->is_reachable(ss_map[target].first, sstates))
            cout << target << " is reachable." << endl;
          else
            cout << target << " not reachable." << endl;
          ss_map[dst] = make_pair(std::move(sstates), paut);
        }
        else
          yyerror("Unknown identifier '" + target +"'.");
      }
      delete $1;
      delete $3;
      delete $7;
    }
  | IDENT '=' IDENT '.' IS_REACHABLE_FB '(' IDENT ')' ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      const auto& target = *($7);
      if (paut_map.find(id) != paut_map.end())
        paut = paut_map[id].get();
      else {
        yyerror("Automaton '" + id + "' not found.");
        paut = nullptr;
      }
      if (paut != nullptr) {
        if (ss_map.find(target) != ss_map.end()) {
          symb_states_type sstates;
          if (paut->is_reachable_fb(ss_map[target].first, sstates))
            cout << target << " is reachable." << endl;
          else
            cout << target << " not reachable." << endl;
          ss_map[dst] = make_pair(std::move(sstates), paut);
        }
        else
          yyerror("Unknown identifier '" + target +"'.");
      }
      delete $1;
      delete $3;
      delete $7;
    }
  | IDENT '.' REFINE_LOC_DERIV ';'
    {
      const auto& id = *($1);
      refine_loc_deriv(*paut_map[id]);
      delete $1;
    }
  | IDENT '.' REFINE_LOCS '(' IDENT ',' IDENT ',' INT ')' ';'
    {
      const auto& id = *($1);
      const auto& label = *($5);
      const auto& method = *($7);
      refinement_method ref_meth;
      if (method == "carth_center")
        ref_meth = carth_center;
      else if (method == "carth0_center")
        ref_meth = carth0_center;
      else if (method == "carth1_center")
        ref_meth = carth1_center;
      else if (method == "deriv_center")
        ref_meth = deriv_center;
      else if (method == "jacob_center")
        ref_meth = jacob_center;
      else if (method == "deriv_dcenter")
        ref_meth = deriv_dcenter;
      else if (method == "jacob_dcenter")
        ref_meth = jacob_dcenter;
      else {
        ref_meth = carth_center;
        yyerror("Unknown refinement method '" + method + "'.");
      }
      paut = paut_map[id].get();
      refine_locs(*paut, paut->get_label_ref(label),
                  ref_meth, atoi_consume($9));
      delete $1;
      delete $5;
      delete $7;
    }
  | IDENT '.' REFINE_LOCS '(' IDENT ')' ';'
    {
      const auto& id = *($1);
      const auto& state = *($5);
      paut = paut_map[id].get();
      refine_states(*paut, ss_map[state].first);
      delete $1;
      delete $5;
    }
  | IDENT '.' REFINE_CONSTRAINTS
    {
      const auto& id = *($1);
      paut = paut_map[id].get();
    }
    '(' refinement_cons ',' IDENT ')' ';'
    {
      const auto& label = *($8);
      REFINEMENT_CONS = *($6);
      REFINEMENT_LABEL = paut->get_label_ref(label);
      delete $1;
      delete $6;
      delete $8;
    }
  | IDENT '=' IDENT '.' INITIAL_STATES ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      if (paut_map.find(id) != paut_map.end())
        ss_map[dst] = make_pair(paut_map[id]->get_ini_states(),
                                paut_map[id].get());
      else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
      delete $3;
    }
  | IDENT '=' IDENT '.' GET_INVARIANTS ';'
    {
      const auto& dst = *($1);
      const auto& id = *($3);
      if (paut_map.find(id) != paut_map.end())
        ss_map[dst] = make_pair(paut_map[id]->get_invariants(),
                                paut_map[id].get());
      else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
      delete $3;
    }
  | IDENT '.' UNLOCK_SURFACE_LOCS '(' IDENT ')' ';'
    // unlock the partitioning flag of locations on the surface of IDENT
    {
      const auto& id = *($1);
      const auto& state = *($5);
      if (paut_map.find(id) != paut_map.end()) {
        if (ss_map.find(state) != ss_map.end())
          paut_map[id]->unlock_surface_locations(ss_map[state].first);
        else
          yyerror("Unknown identifier '" + state +"'.");
      } else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
      delete $5;
    }
  | IDENT '.' UNLOCK_LOCS ';' // unlock the partitioning flag of all locations
    {
      const auto& id = *($1);
      if (paut_map.find(id) != paut_map.end())
        paut_map[id]->unlock_locations();
      else
        yyerror("Automaton '" + id + "' not found.");
      delete $1;
    }
  ;

///////////////////////////////////////////////////////////////////////////

// These are meant to implement *context-dependent* keywords.
// Since these are not real keywords, it is possible to use them
// as location/variable names or labels.
DO:
    IDENT { check_ident_consume($1, "do"); }
END:
    IDENT { check_ident_consume($1, "end"); }
GOTO:
    IDENT { check_ident_consume($1, "goto"); }
SYNC:
    IDENT { check_ident_consume($1, "sync"); }
WAIT:
    IDENT { check_ident_consume($1, "wait"); }

var_ref_list:
  IDENT
    {
      const auto& id = *($1);
      if (psymb_state != nullptr) {
        auto pvrs = new var_ref_set;
        if (psymb_state->var_names.contains_name(id)) {
          pvrs->insert(psymb_state->var_names.get_id(id));
          $$ = pvrs;
        } else {
          yyerror("Unknown state variable '" + id +"'.");
          delete pvrs;
          $$ = nullptr;
        }
      } else if (paut != nullptr) {
        auto pvrs = new var_ref_set;
        if (paut->var_id_map.contains_name(id)) {
          pvrs->insert(paut->var_id_map.get_id(id));
          $$ = pvrs;
        } else {
          yyerror("Unknown automaton variable '" + id +"'.");
          delete pvrs;
          $$ = nullptr;
        }
      } else
        yyerror("Don't know which set of states '"
                + id +"' refers to.");
      delete $1;
    }
  | var_ref_list ',' IDENT
    {
      const auto& id = *($3);
      if (psymb_state != nullptr) {
        if (psymb_state->var_names.contains_name(id)) {
          $1->insert(psymb_state->var_names.get_id(id));
          $$ = $1;
        } else
          yyerror("Unknown state variable '" + id +"'.");
      }	else if (paut != nullptr) {
        if (paut->var_id_map.contains_name(id)) {
          $1->insert(paut->var_id_map.get_id(id));
          $$ = $1;
        } else
          yyerror("Unknown automaton variable '" + id +"'.");
      } else
        yyerror("Don't know which set of states '"
                + id +"' refers to.");
      delete $3;
    }
  ;

prelim:
  par_POLY_KIND '=' IDENT ';'
    {
      const auto& id = *($3);
      if (!set_poly_kind(id, true))
        yyerror("Invalid poly kind name");
      delete $3;
    }
  | IDENT ASSIGN rat_aff_expr ';'
    {
      const auto& id = *($1);
      const auto& rae = *($3);
      rae_map[id] = rae;
      delete $1;
      delete $3;
    }
// Note: here macros of the form xcon:x==1 could be accepted, only that x is not a global variable and depends on the automaton
  | par_TIME_POST_ITER '=' INT ';'
    { TIME_POST_ITER = atoi_consume($3); }
  | par_TIME_POST_CONE_LAMBDA '=' rat_aff_expr ';'
    {
      TIME_POST_CONE_LAMBDA= $3->rat_inhomo_term();
      delete $3;
    }
  | par_TIME_POST_CONE_WIDEN_DELAY '=' INT ';'
    { TIME_POST_CONE_WIDEN_DELAY = atoi_consume($3); }
  | par_TIME_POST_CONE_WIDEN_PRECISION '=' INT ';'
    { TIME_POST_CONE_WIDEN_PRECISION = atoi_consume($3); }
  | par_MEMORY_MODE '=' INT ';'
    { MEMORY_MODE = atoi_consume($3); }
// parser
  | par_PARSER_FIX_DPOST '=' bool_type ';'
    { PARSER_FIX_DPOST=$3; }
// Reachability
  | par_REACH_CHEAP_CONTAINS '=' bool_type ';'
    { REACH_CHEAP_CONTAINS=$3; }
  | par_REACH_CHEAP_CONTAINS_USE_BBOX '=' bool_type ';'
    { REACH_CHEAP_CONTAINS_USE_BBOX=$3; }
  | par_MAINTAIN_BOXED_CCVS '=' bool_type ';'
    { MAINTAIN_BOXED_CCVS=$3; }
  | par_REACH_USE_BBOX '=' bool_type ';'
    { REACH_USE_BBOX=$3; }
  | par_REACH_USE_CONSTRAINT_HULL '=' bool_type ';'
    { REACH_USE_CONSTRAINT_HULL=$3; }
  | par_REACH_USE_CONVEX_HULL '=' bool_type ';'
    { REACH_USE_CONVEX_HULL=$3; }
  | par_REACH_USE_TIME_ELAPSE '=' bool_type ';'
    { REACH_USE_TIME_ELAPSE = $3; }
  | par_REACH_STOP_AT_FORB '=' bool_type ';'
    { REACH_STOP_AT_FORB=$3; }
  | par_REACH_MAX_ITER '=' INT ';'
    { REACH_MAX_ITER = atoi_consume($3); }
  | par_REACH_MAX_ITER '=' '-' INT ';'
    { REACH_MAX_ITER = - atoi_consume($4); }
  | par_REACH_REPORT_INTERVAL '=' INT ';'
    { REACH_REPORT_INTERVAL = atoi_consume($3); }
  | par_REACH_USE_BBOX_ITER '=' INT ';'
    { REACH_USE_BBOX_ITER = atoi_consume($3); }
  | par_REACH_STOP_USE_CONVEX_HULL_ITER '=' INT ';'
    { REACH_STOP_USE_CONVEX_HULL_ITER = atoi_consume($3); }
  | par_REACH_STOP_USE_CONVEX_HULL_SETTLE '=' bool_type ';'
    { REACH_STOP_USE_CONVEX_HULL_SETTLE=$3; }
  | par_LIMIT_CONSTRAINTS_METHOD '=' bool_type ';'
    { LIMIT_CONSTRAINTS_METHOD=$3; }
  | par_REFINE_DERIVATIVE_METHOD '=' INT ';'
    { REFINE_DERIVATIVE_METHOD = atoi_consume($3); }
  | par_REFINE_PRIORITIZE_REACH_SPLIT '=' bool_type ';'
    { REFINE_PRIORITIZE_REACH_SPLIT=$3; }
  | par_REFINE_SMALLEST_FIRST '=' bool_type ';'
    { REFINE_SMALLEST_FIRST=$3; }
  | par_REFINE_USE_FP '=' bool_type ';'
    { REFINE_USE_FP=$3; }
  | par_REFINE_DERIV_MINANGLE '=' rat_aff_expr ';'
    {
      mpq_class q = $3->rat_inhomo_term();
      REFINE_DERIV_MINANGLE = q.get_d();
      delete $3;
    }
  | par_REFINE_PRIORITIZE_ANGLE '=' bool_type ';'
    { REFINE_PRIORITIZE_ANGLE=$3; }
  | par_REACH_CONSTRAINT_LIMIT '=' INT ';'
    { REACH_CONSTRAINT_LIMIT = atoi_consume($3); }
  | par_TP_CONSTRAINT_LIMIT '=' INT ';'
    { TP_CONSTRAINT_LIMIT = atoi_consume($3); }
  | par_REACH_CONSTRAINT_TRIGGER '=' INT ';'
    { REACH_CONSTRAINT_TRIGGER = atoi_consume($3); }
  | par_REACH_BITSIZE_TRIGGER '=' INT ';'
    { REACH_BITSIZE_TRIGGER = atoi_consume($3); }
  | par_CONSTRAINT_BITSIZE '=' INT ';'
    { CONSTRAINT_BITSIZE = atoi_consume($3); }
  | par_SEARCH_METHOD '=' INT ';'
    {
      int sm = atoi_consume($3);
      switch (sm) {
      case 0:
        SEARCH_METHOD = Search_Method::trx_based;
        break;
      case 1:
        SEARCH_METHOD = Search_Method::topsort;
        break;
      case 2:
        SEARCH_METHOD = Search_Method::topsort_reachable;
        break;
      default:
        yyerror("Expecting an integer in {0, 1, 2}");
        break;
      }
    }
  | par_SEARCH_METHOD_TOPSORT_TOKENS '=' INT ';'
    { SEARCH_METHOD_TOPSORT_TOKENS = atoi_consume($3); }
  | par_SNAPSHOT_INTERVAL '=' INT ';'
    { SNAPSHOT_INTERVAL = atoi_consume($3); }
// Refinement
  | par_REFINE_CHECK_TIME_RELEVANCE '=' bool_type ';'
    { REFINE_CHECK_TIME_RELEVANCE=$3; }
  | par_REFINE_CHECK_TIME_RELEVANCE_DURING '=' bool_type ';'
    { REFINE_CHECK_TIME_RELEVANCE_DURING=$3; }
  | par_REFINE_CHECK_TIME_RELEVANCE_FINAL '=' bool_type ';'
    { REFINE_CHECK_TIME_RELEVANCE_FINAL=$3; }
  | par_REFINE_CHECK_TRANS_DIMS'=' bool_type ';'
    { REFINE_CHECK_TRANS_DIMS=$3; }
  | par_REFINE_PARTITION_INSIDE '=' bool_type ';'
    { REFINE_PARTITION_INSIDE=$3; }
  | par_REACH_FB_REFINE_METHOD '=' INT ';'
    { REACH_FB_REFINE_METHOD = atoi_consume($3); }
  | par_REFINE_MAX_CHECKS '=' INT ';'
    { REFINE_MAX_CHECKS = atoi_consume($3); }
  | par_REFINE_USE_NEW_SPLIT '=' bool_type ';'
    { REFINE_USE_NEW_SPLIT = $3; }
  | par_MINIMIZE_FILTER_THRESHOLD '=' INT ';'
    { pplite::Poly::set_minimize_filter_threshold(atoi_consume($3)); }
  ;

bool_type:
    TRUE { $$ = true; }
  | FALSE { $$ = false; }
  ;

automaton:
    AUTOMATON IDENT
    {
      const auto& id = *($2);
      if (paut_map.find(id) == paut_map.end()) {
        paut = new automaton(id);
        paut_map[id].reset(paut);
      } else
        yyerror("automaton " + id + " already defined");
      delete $2;
    }
    automaton_body END
    {
      message(32200, "Parsed automaton: "
              + int2string(paut->locations.size()) + " locs, "
              + int2string(paut->transitions.size()) + " trans.");
    }
  ;

automaton_body:
    declaration location_list initial
  ;

declaration:
    // empty
  | declaration internal_vars
  | declaration external_vars
  | declaration synclab
  | declaration parameters
  ;

synclab:
  SYNCLABS ':' ident_list ';'
    {
      const auto& id_list = *($3);
      if (paut != nullptr) {
        for (const auto& lab : id_list)
          paut->add_label(lab);
      }
      else
        yyerror("synclabs: paut not defined");
      delete $3;
    }
    ;

internal_vars:
    INTERNAL_VAR ':' ivar_list ';'
  ;

external_vars:
    EXTERNAL_VAR ':' evar_list ';'
  ;

parameters:
    PARAMETER ':' param_list ';'
  ;

ivar_list:
    ivar_list ',' IDENT
    {
      const auto& id = *($3);
      paut->add_variable(id);
      delete $3;
    }
  | IDENT
    {
      const auto& id = *($1);
      paut->add_variable(id);
      delete $1;
    }
  ;

evar_list:
    evar_list ',' IDENT
    {
      const auto& id = *($3);
      paut->add_ext_variable(id);
      delete $3;
    }
  | IDENT
    {
      const auto& id = *($1);
      paut->add_ext_variable(id);
      delete $1;
    }
  ;

param_list:
    param_list ',' IDENT
    {
      const auto& id = *($3);
      paut->add_parameter(id);
      delete $3;
    }
  | IDENT
    {
      const auto& id = *($1);
      paut->add_parameter(id);
      delete $1;
    }
  ;

ident_list:
    ident_list ',' IDENT
    {
      const auto& id = *($3);
      $$->push_back(id);
      delete $3;
    }
  | IDENT
    {
      const auto& id = *($1);
      $$ = new std::list<string>;
      $$->push_back(id);
      delete $1;
    }
  | ident_list ',' IDENT '{' ident_list '}'
    {
      const auto& prefix = *($3);
      auto& id_list = *($5);
      for (auto& s : id_list)
        s.insert(0, prefix);
      $$->splice($$->end(), id_list);
      delete $3;
      delete $5;
    }
  | IDENT '{' ident_list '}'
    {
      const auto& prefix = *($1);
      auto& id_list = *($3);
      for (auto& s : id_list)
        s.insert(0, prefix);
      $$ = $3;
      delete $1;
    }
  ;

compose_list:
    compose_list '&' IDENT
    {
      const auto& id = *($3);
      $$->push_back(id);
      delete $3;
    }
  | IDENT '&' IDENT
    {
      const auto& id1 = *($1);
      const auto& id2 = *($3);
      $$ = new std::list<string>;
      $$->push_back(id1);
      $$->push_back(id2);
      delete $1;
      delete $3;
    }
  ;

initial:
    INITIALLY ':' state_list ';'
    {
      paut->ini_states_assign(*$3);
      delete $3;
    }
  | INITIALLY state_list ';'
    {
      paut->ini_states_assign(*$2);
      delete $2;
    }
  ;

state_list:
    IDENT '&' state_val_set
    {
      const auto& loc = *($1);
      auto& state = *($3);
      if (paut != nullptr) {
        psymb_state = new symb_states_type(paut->get_var_names());
        psymb_state->add(loc, std::move(state));
        $$ = psymb_state;
      }	else
        yyerror("Don't know which automaton to attribute states to.");
      delete $1;
      delete $3;
    }
  | state_list ',' IDENT '&' state_val_set
    {
      const auto& loc = *($3);
      auto& state = *($5);
      if (paut != nullptr) {
        psymb_state->add(loc, std::move(state));
        $$ = psymb_state;
      }
      else
        yyerror("Don't know which automaton to attribute states to.");
      delete $3;
      delete $5;
    }
  ;

location_list:
    location_list location transition_list
  | location_list location ';' transition_list
  | location transition_list
  | location ';' transition_list
  ;

location:
    LOC IDENT ':' WHILE state_val_set WAIT '{' constr_list '}'
    {
      const auto& id = *($2);
      const auto& inv = *($5);
      auto& cpost = *($8);
      swap_space_dims(cpost, 0, paut->dim, paut->dim);
      paut->add_location(std::move(inv), id, std::move(cpost));
      // will serve as the source location for the coming transitions
      current_loc_name = id;
      delete $2;
      delete $5;
      delete $8;
    }
  ;

transition_list:
    transition_list transition
  |
  ;


transition:
    // NOTE: ident_list semantics:
    // add a transition for each label in the ident_list;
    WHEN state_val_set SYNC ident_list DO '{' dpost_cons '}' GOTO IDENT ';'
    {
      const auto& guard = *($2);
      const auto& id_list = *($4);
      auto& dpost = *($7);
      const auto& target = *($10);
      maybe_fix_dpost(paut->dim, dpost, paut->variables);
      check_dpost(paut->dim, dpost, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost);
      delete $2;
      delete $4;
      delete $7;
      delete $10;
    }
  | WHEN state_val_set DO '{' dpost_cons '}' SYNC ident_list GOTO IDENT ';'
    {
      const auto& guard = *($2);
      auto& dpost = *($5);
      const auto& id_list = *($8);
      const auto& target = *($10);
      maybe_fix_dpost(paut->dim, dpost, paut->variables);
      check_dpost(paut->dim, dpost, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost);
      delete $2;
      delete $5;
      delete $8;
      delete $10;
    }
  | WHEN state_val_set SYNC ident_list GOTO IDENT ';'
    {
      const auto& guard = *($2);
      const auto& id_list = *($4);
      const auto& target = *($6);
      // controlled variables should remain constant
      Cons dpost = identity_dpost(paut->dim, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost);
      delete $2;
      delete $4;
      delete $6;
    }
  | WHEN state_val_set ASAP SYNC ident_list
    DO '{' dpost_cons '}' GOTO IDENT ';'
    {
      const auto& guard = *($2);
      const auto& id_list = *($5);
      auto& dpost = *($8);
      const auto& target = *($11);
      maybe_fix_dpost(paut->dim, dpost, paut->variables);
      check_dpost(paut->dim, dpost, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost, Urgency::urgent);
      delete $2;
      delete $5;
      delete $8;
      delete $11;
    }
  | WHEN state_val_set ASAP DO '{' dpost_cons '}'
    SYNC ident_list GOTO IDENT ';'
    {
      const auto& guard = *($2);
      auto& dpost = *($6);
      const auto& id_list = *($9);
      const auto& target = *($11);
      maybe_fix_dpost(paut->dim, dpost, paut->variables);
      check_dpost(paut->dim, dpost, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost, Urgency::urgent);
      delete $2;
      delete $6;
      delete $9;
      delete $11;
    }
  | WHEN state_val_set ASAP SYNC ident_list GOTO IDENT ';'
    {
      const auto& guard = *($2);
      const auto& id_list = *($5);
      const auto& target = *($7);
      // controlled variables should remain constant
      Cons dpost = identity_dpost(paut->dim, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost, Urgency::urgent);
      delete $2;
      delete $5;
      delete $7;
    }
  ;

refinement_cons:
    refinement_cons ',' refinement_con
    {
      $1->push_back(*$3);
      $$ = $1;
      delete $3;
    }
  | refinement_con
    {
      $$ = new RefinementCons;
      $$->push_back(*$1);
      delete $1;
    }
  ;

refinement_con:
    '(' rat_aff_expr ',' rat_aff_expr ')'
    {
      $$ = new RefinementCon;
      $$->con = Con($2->aexpr.expr, $2->aexpr.inhomo,
                    Con::NONSTRICT_INEQUALITY);
      $$->min_d = $4->rat_inhomo_term() * Rational($2->den);
      $$->max_d = Rational(0);
      delete $2;
      delete $4;
    }
  | '(' rat_aff_expr ',' rat_aff_expr ',' rat_aff_expr ')'
    {
      $$ = new RefinementCon;
      $$->con = Con($2->aexpr.expr, $2->aexpr.inhomo,
                   Con::NONSTRICT_INEQUALITY);
      $$->min_d = $4->rat_inhomo_term() * Rational($2->den);
      $$->max_d = $6->rat_inhomo_term() * Rational($2->den);
      delete $2;
      delete $4;
      delete $6;
    }
  ;

state_val_set:
    {
      if (paut == nullptr)
        yyerror("paut not defined");
      else
        val_set_dim = paut->dim;
    }
    val_set
    { $$ = $2; }

dpost_cons:
    constr_list

val_set:
    val_set '|' val_set
    {
      $1->union_assign(*($3));
      $$ = $1;
      delete $3;
    }
  | val_set '&' val_set
    {
      $1->intersection_assign(*($3));
      $$ = $1;
      delete $3;
    }
  | '!' val_set
    {
      $2->negate();
      $$ = $2;
    }
  | '(' val_set ')'
    {
      $$ = $2;
    }
  | constr_list_no_and
    {
      const auto& cs = *($1);
      auto ccvs = convex_clock_val_set(val_set_dim, std::move(cs));
      $$ = new clock_val_set(std::move(ccvs));
      delete $1;
    }
  ;


constr_list:
    constr_list '&' constr_list
    {
      $1->insert($1->end(),
                 std::make_move_iterator($3->begin()),
                 std::make_move_iterator($3->end()));
      $$ = $1;
      delete $3;
    }
  | constr_list_no_and;

constr_list_no_and:
     constr
     {
       $$ = new Cons;
       $$->push_back(*$1);
       delete $1;
     }
   | rat_aff_expr LE rat_aff_expr LE rat_aff_expr
     {
       $$ = new Cons;
       Affine_Expr ae = ($1->den)*($3->aexpr) - ($3->den)*($1->aexpr);
       $$->push_back(Con(ae.expr, ae.inhomo, Con::NONSTRICT_INEQUALITY));
       ae = ($3->den)*($5->aexpr) - ($5->den)*($3->aexpr);
       $$->push_back(Con(ae.expr, ae.inhomo, Con::NONSTRICT_INEQUALITY));
       delete $1;
       delete $3;
       delete $5;
     }
   | rat_aff_expr '<' rat_aff_expr LE rat_aff_expr
     {
       $$ = new Cons;
       Affine_Expr ae = ($1->den)*($3->aexpr) - ($3->den)*($1->aexpr);
       $$->push_back(Con(ae.expr, ae.inhomo, Con::STRICT_INEQUALITY));
       ae = ($3->den)*($5->aexpr) - ($5->den)*($3->aexpr);
       $$->push_back(Con(ae.expr, ae.inhomo, Con::NONSTRICT_INEQUALITY));
       delete $1;
       delete $3;
       delete $5;
     }
   | rat_aff_expr LE rat_aff_expr '<' rat_aff_expr
     {
       $$ = new Cons;
       Affine_Expr ae = ($1->den)*($3->aexpr) - ($3->den)*($1->aexpr);
       $$->push_back(Con(ae.expr, ae.inhomo, Con::NONSTRICT_INEQUALITY));
       ae = ($3->den)*($5->aexpr) - ($5->den)*($3->aexpr);
       $$->push_back(Con(ae.expr, ae.inhomo, Con::STRICT_INEQUALITY));
       delete $1;
       delete $3;
       delete $5;
     }
   | rat_aff_expr '<' rat_aff_expr '<' rat_aff_expr
     {
       $$ = new Cons;
       Affine_Expr ae = ($1->den)*($3->aexpr) - ($3->den)*($1->aexpr);
       $$->push_back(Con(ae.expr, ae.inhomo, Con::STRICT_INEQUALITY));
       ae = ($3->den)*($5->aexpr) - ($5->den)*($3->aexpr);
       $$->push_back(Con(ae.expr, ae.inhomo, Con::STRICT_INEQUALITY));
       delete $1;
       delete $3;
       delete $5;
     }
   ;


constr:
    rat_aff_expr '<' rat_aff_expr
    {
      Affine_Expr ae = ($1->den)*($3->aexpr) - ($3->den)*($1->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::STRICT_INEQUALITY);
      $$ = pcon;
      delete $1;
      delete $3;
    }
  | rat_aff_expr '>' rat_aff_expr
    {
      Affine_Expr ae = ($3->den)*($1->aexpr) - ($1->den)*($3->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::STRICT_INEQUALITY);
      $$ = pcon;
      delete $1;
      delete $3;
    }
  | rat_aff_expr GE rat_aff_expr
    {
      Affine_Expr ae = ($3->den)*($1->aexpr) - ($1->den)*($3->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::NONSTRICT_INEQUALITY);
      $$ = pcon;
      delete $1;
      delete $3;
    }
  | rat_aff_expr LE rat_aff_expr
    {
      Affine_Expr ae = ($1->den)*($3->aexpr) - ($3->den)*($1->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::NONSTRICT_INEQUALITY);
      $$ = pcon;
      delete $1;
      delete $3;
    }
  | rat_aff_expr EQ rat_aff_expr
    {
      Affine_Expr ae = ($3->den)*($1->aexpr) - ($1->den)*($3->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::EQUALITY);
      $$ = pcon;
      delete $1;
      delete $3;
    }
  | TRUE
    {
      auto pcon = new Con(Linear_Expr(), 0, Con::EQUALITY);
      $$ = pcon;
    }
  | FALSE
    {
      auto pcon = new Con(Linear_Expr(), 1, Con::EQUALITY);
      $$ = pcon;
    }
  ;

rat_aff_expr:
    INT
    {
      const auto& str = *($1);
      auto prae = new Rat_Affine_Expr(str);
      $$ = prae;
      delete $1;
    }
  | IDENT PRIM
    {
      const auto& id = *($1);
      if (paut->var_id_map.contains_name(id)) {
        dim_type vid = paut->var_id_map.get_id(id);
        // Variable is primed: add automaton dim.
        vid += paut->dim;
        auto prae = new Rat_Affine_Expr(Var(vid));
        $$ = prae;
      } else
        yyerror("Either variable " + id + " not defined in automaton "
                + paut->name + ", or undefined shortcut.");
      delete $1;
    }
  | IDENT
    {
      const auto& id = *($1);
      if (rae_map.find(id) == rae_map.end()) {
        if (paut != nullptr && paut->var_id_map.contains_name(id)) {
          dim_type vid = paut->var_id_map.get_id(id);
          auto prae = new Rat_Affine_Expr(Var(vid));
          $$ = prae;
        } else {
          if (paut != nullptr)
            yyerror("Either variable " + id
                    + " not defined in automaton " + paut->name
                    + ", or undefined shortcut.");
          else
            yyerror("Cannot identify \"" + id +"\".");
        }
      } else {
        auto prae = new Rat_Affine_Expr(rae_map[id]);
        $$ = prae;
      }
      delete $1;
    }
  | rat_aff_expr '*' rat_aff_expr
    {
      if (attempt_multiply(*($1),*($3))) {
        $$ = $1;
        delete $3;
      } else {
        yyerror("Cannot multiply two linear expressions. ");
        cout << *($1) << " and " << *($3) << endl;
      }
    }
  | rat_aff_expr '/' rat_aff_expr
    {
      if (attempt_division(*($1),*($3))) {
        $$ = $1;
        delete $3;
      } else
        yyerror("Cannot divide two linear expressions.");
    }
  | rat_aff_expr '+' rat_aff_expr
    {
      *($1) += *($3);
      $$ = $1;
      delete $3;
    }
  | rat_aff_expr '-' rat_aff_expr
    {
      *($1) -= *($3);
      $$ = $1;
      delete $3;
    }
  | '-' rat_aff_expr %prec '*'
    {
      neg_assign(*$2);
      $$ = $2;
    }
  | '(' rat_aff_expr ')'
    {
      $$ = $2;
    }
  ;

%%
