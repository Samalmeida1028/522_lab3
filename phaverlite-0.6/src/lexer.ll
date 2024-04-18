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
  #include <cstdlib>
  #include <cstring>
  #include <list>
  #include <string>
  #include "clock_val_set.hh"
  #include "rat_aff_expr.hh"
  #include "symb_states_type.hh"

  #include "parser.hh"
  void yyerror(const std::string&);
%}

%option nounput
%option noyywrap

%x comment_mode

DIGIT      [0-9]
LETTER     [a-zA-Z]

%%
"POLY_KIND" {return par_POLY_KIND;}
"PARSER_FIX_DPOST" {return par_PARSER_FIX_DPOST;}
"merge_splitted" { return MERGE_SPLITTED; }
"ASAP" { return ASAP; }
"MEMORY_MODE" {return par_MEMORY_MODE;}
"LIMIT_CONSTRAINTS_METHOD" {return par_LIMIT_CONSTRAINTS_METHOD;}
"TIME_POST_ITER" {return par_TIME_POST_ITER;}
"TIME_POST_CONE_LAMBDA" {return par_TIME_POST_CONE_LAMBDA;}
"TIME_POST_CONE_WIDEN_DELAY" {return par_TIME_POST_CONE_WIDEN_DELAY;}
"TIME_POST_CONE_WIDEN_PRECISION" {return par_TIME_POST_CONE_WIDEN_PRECISION;}
"project_to" {return PROJECT_TO;}
"rename" {return RENAME;}
"difference_assign" {return DIFFERENCE_ASSIGN;}
"REFINE_MAX_CHECKS" {return par_REFINE_MAX_CHECKS;}
"REFINE_USE_NEW_SPLIT" {return par_REFINE_USE_NEW_SPLIT;}
"MINIMIZE_FILTER_THRESHOLD" {return par_MINIMIZE_FILTER_THRESHOLD;}
"REACH_FB_REFINE_METHOD" {return par_REACH_FB_REFINE_METHOD;}
"is_reachable_fb" {return IS_REACHABLE_FB;}
"is_reachable" {return IS_REACHABLE;}
"invariant_assign" {return INVARIANT_ASSIGN;}
"reverse" {return REVERSE;}
"SNAPSHOT_INTERVAL" {return par_SNAPSHOT_INTERVAL;}
"REFINE_PARTITION_INSIDE" {return par_REFINE_PARTITION_INSIDE;}
"reachable_forward_iter" {return REACH_FORWARD_ITER;}
"unlock_surface_locs" {return UNLOCK_SURFACE_LOCS;}
"unlock_locs" {return UNLOCK_LOCS;}
"get_invariants" {return GET_INVARIANTS;}
"print_graph" {return PRINT_GRAPH;}
"REFINE_CHECK_TIME_RELEVANCE" {return par_REFINE_CHECK_TIME_RELEVANCE;}
"REFINE_CHECK_TIME_RELEVANCE_DURING" {return par_REFINE_CHECK_TIME_RELEVANCE_DURING;}
"REFINE_CHECK_TIME_RELEVANCE_FINAL" {return par_REFINE_CHECK_TIME_RELEVANCE_FINAL;}
"REFINE_CHECK_TRANS_DIMS" {return par_REFINE_CHECK_TRANS_DIMS;}
"PARTITION_CHECK_TIME_RELEVANCE" {return par_REFINE_CHECK_TIME_RELEVANCE;}
"PARTITION_CHECK_TIME_RELEVANCE_DURING" {return par_REFINE_CHECK_TIME_RELEVANCE_DURING;}
"PARTITION_CHECK_TIME_RELEVANCE_FINAL" {return par_REFINE_CHECK_TIME_RELEVANCE_FINAL;}
"PARTITION_CHECK_TRANS_DIMS" {return par_REFINE_CHECK_TRANS_DIMS;}
"SEARCH_METHOD" {return par_SEARCH_METHOD;}
"SEARCH_METHOD_TOPSORT_TOKENS" {return par_SEARCH_METHOD_TOPSORT_TOKENS;}
"get_inverse" {return INVERSE;}
"contains" {return CONTAINS;}
"REACH_CONSTRAINT_TRIGGER" {return par_REACH_CONSTRAINT_TRIGGER;}
"REFINE_PRIORITIZE_ANGLE" {return par_REFINE_PRIORITIZE_ANGLE;}
"REFINE_DERIV_MINANGLE" {return par_REFINE_DERIV_MINANGLE;}
"PARTITION_PRIORITIZE_ANGLE" {return par_REFINE_PRIORITIZE_ANGLE;}
"PARTITION_DERIV_MINANGLE" {return par_REFINE_DERIV_MINANGLE;}
"REACH_CONSTRAINT_LIMIT" {return par_REACH_CONSTRAINT_LIMIT;}
"TP_CONSTRAINT_LIMIT" {return par_TP_CONSTRAINT_LIMIT;}
"REACH_BITSIZE_TRIGGER" {return par_REACH_BITSIZE_TRIGGER;}
"CONSTRAINT_BITSIZE" {return par_CONSTRAINT_BITSIZE;}
"REFINE_USE_FP" {return par_REFINE_USE_FP;}
"REFINE_PRIORITIZE_REACH_SPLIT" {return par_REFINE_PRIORITIZE_REACH_SPLIT;}
"REFINE_SMALLEST_FIRST" {return par_REFINE_SMALLEST_FIRST;}
"PARTITION_PRIORITIZE_REACH_SPLIT" {return par_REFINE_PRIORITIZE_REACH_SPLIT;}
"PARTITION_SMALLEST_FIRST" {return par_REFINE_SMALLEST_FIRST;}
"REACH_STOP_USE_CONVEX_HULL_SETTLE" {return par_REACH_STOP_USE_CONVEX_HULL_SETTLE;}
"REACH_STOP_USE_CONVEX_HULL_ITER" {return par_REACH_STOP_USE_CONVEX_HULL_ITER;}
"REACH_USE_BBOX_ITER" {return par_REACH_USE_BBOX_ITER;}
"set_refine_constraints" {return REFINE_CONSTRAINTS;}
"set_partition_constraints" {return REFINE_CONSTRAINTS;}
"save_fp_invars" {return SAVE_FP_INVARS;}
"save_fp_surface_inv" {return SAVE_FP_SURFACE;}
"initial_states" {return INITIAL_STATES;}
"refine_locs" {return REFINE_LOCS;}
"refine_loc_deriv" {return REFINE_LOC_DERIV;}
"REACH_CHEAP_CONTAINS" {return par_REACH_CHEAP_CONTAINS;}
"MAINTAIN_BOXED_CCVS" {return par_MAINTAIN_BOXED_CCVS;}
"REACH_CHEAP_CONTAINS_USE_BBOX" {return par_REACH_CHEAP_CONTAINS_USE_BBOX;}
"REACH_USE_BBOX" {return par_REACH_USE_BBOX;}
"REACH_USE_CONSTRAINT_HULL" {return par_REACH_USE_CONSTRAINT_HULL;}
"REACH_USE_CONVEX_HULL" {return par_REACH_USE_CONVEX_HULL;}
"REACH_USE_TIME_ELAPSE" {return par_REACH_USE_TIME_ELAPSE;}
"REACH_STOP_AT_FORB" {return par_REACH_STOP_AT_FORB;}
"REACH_MAX_ITER" {return par_REACH_MAX_ITER;}
"REACH_REPORT_INTERVAL" {return par_REACH_REPORT_INTERVAL;}
"REFINE_DERIVATIVE_METHOD" {return par_REFINE_DERIVATIVE_METHOD;}
"is_empty" {return IS_EMPTY;}
"is_intersecting" {return IS_INTERSECTING;}
"intersection_assign" {return INTERSECTION_ASSIGN;}
"loc_intersection" {return LOC_INTERSECTION;}
"loc_union" {return LOC_UNION;}
"remove" {return REMOVE;}
"echo" {return MY_ECHO;}
"get_parameters" {return GET_PARAMETERS;}
"print" {return PRINT;}
"save_gen_fp" {return SAVE_GEN_FP;}
"save_con_fp" {return SAVE_CON_FP;}
"reachable" {return REACH;}
"define" {return DEFNE;}
"automaton" {return AUTOMATON;}
"state_var" {return INTERNAL_VAR;}
"contr_var" {return INTERNAL_VAR;}
"input_var" {return EXTERNAL_VAR;}
"parameter" {return PARAMETER;}
"synclabs" {return SYNCLABS;}
"initially" {return INITIALLY;}
"loc" {return LOC;}
"while" {return WHILE;}
"when" {return WHEN;}
"true"  {return TRUE;}
"false"  {return FALSE;}
"who" {return WHO;}
"add_label" {return ADD_LABEL;}

  /* rules for operators */

"==" {return EQ;}
">=" {return GE;}
"<=" {return LE;}
"'"  {return PRIM;}
":=" {return ASSIGN;}

  /* rules for float */

([0-9]+)(("."[0-9]+)?)((([eE])([+-]?)([0-9]+))?) {
  yylval.mystring = new std::string(yytext); return INT;
}

  /* rule for characters */

[-+<>*/&|(){}:;,.=!]|"["|"]" {return *yytext;}


  /* rules for integers commented out, using rule for floats */
  /*  [0-9]+  {*yylval.mystring = new std::string(yytext); return INT;} */

  /* rules for identifiers */

(["$""?"a-zA-Z]|[_])("$"|"?"|[a-zA-Z]|[0-9]|[_]|[~])* {
  yylval.mystring = new std::string(yytext); return IDENT;
}

  /* rules for single line comments */

"//".*                         /* skip single line comment */
"--".*                         /* skip single line comment */

  /* rules for C-style comments */

"/*" { BEGIN(comment_mode); }             /* enter comment mode */
<comment_mode>[^*\n]*                     /* discard all but * or \n */
<comment_mode>"*"+[^*/\n]*                /* discard * (unless ...) */
<comment_mode>\n { ++line_number; }       /* bump line number */
<comment_mode>"*"+"/" { BEGIN(INITIAL); } /* exit comment mode */

  /* rules for quoted strings */

\"(.*)\" {
  auto len = strlen(yytext);
  yylval.mystring = new std::string(yytext+1, len-2);
  return STRING_TEXT;
}

  /* rules for whitespace */
[\n] { ++line_number; }        /* bump line number */
[ \t\r]                        /* skip whitespace */

  /* catch all rule for errors */
. {
    yyerror(yytext);
  }
