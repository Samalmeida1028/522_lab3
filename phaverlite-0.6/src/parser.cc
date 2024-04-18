/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 21 "../../../phaver/src/parser.yy"

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


#line 201 "parser.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_PARSER_HH_INCLUDED
# define YY_YY_PARSER_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    par_POLY_KIND = 258,           /* par_POLY_KIND  */
    MERGE_SPLITTED = 259,          /* MERGE_SPLITTED  */
    ASAP = 260,                    /* ASAP  */
    par_TIME_POST_ITER = 261,      /* par_TIME_POST_ITER  */
    par_TIME_POST_CONE_LAMBDA = 262, /* par_TIME_POST_CONE_LAMBDA  */
    par_TIME_POST_CONE_WIDEN_DELAY = 263, /* par_TIME_POST_CONE_WIDEN_DELAY  */
    par_TIME_POST_CONE_WIDEN_PRECISION = 264, /* par_TIME_POST_CONE_WIDEN_PRECISION  */
    par_MEMORY_MODE = 265,         /* par_MEMORY_MODE  */
    par_MAINTAIN_BOXED_CCVS = 266, /* par_MAINTAIN_BOXED_CCVS  */
    par_PARSER_FIX_DPOST = 267,    /* par_PARSER_FIX_DPOST  */
    par_REACH_CHEAP_CONTAINS = 268, /* par_REACH_CHEAP_CONTAINS  */
    par_REACH_CHEAP_CONTAINS_USE_BBOX = 269, /* par_REACH_CHEAP_CONTAINS_USE_BBOX  */
    par_REACH_USE_BBOX = 270,      /* par_REACH_USE_BBOX  */
    par_REACH_USE_CONSTRAINT_HULL = 271, /* par_REACH_USE_CONSTRAINT_HULL  */
    par_REACH_USE_CONVEX_HULL = 272, /* par_REACH_USE_CONVEX_HULL  */
    par_REACH_USE_TIME_ELAPSE = 273, /* par_REACH_USE_TIME_ELAPSE  */
    par_REACH_STOP_AT_FORB = 274,  /* par_REACH_STOP_AT_FORB  */
    par_REACH_MAX_ITER = 275,      /* par_REACH_MAX_ITER  */
    par_REACH_REPORT_INTERVAL = 276, /* par_REACH_REPORT_INTERVAL  */
    par_REACH_USE_BBOX_ITER = 277, /* par_REACH_USE_BBOX_ITER  */
    par_REACH_STOP_USE_CONVEX_HULL_ITER = 278, /* par_REACH_STOP_USE_CONVEX_HULL_ITER  */
    par_REACH_STOP_USE_CONVEX_HULL_SETTLE = 279, /* par_REACH_STOP_USE_CONVEX_HULL_SETTLE  */
    par_REACH_CONSTRAINT_LIMIT = 280, /* par_REACH_CONSTRAINT_LIMIT  */
    par_LIMIT_CONSTRAINTS_METHOD = 281, /* par_LIMIT_CONSTRAINTS_METHOD  */
    par_TP_CONSTRAINT_LIMIT = 282, /* par_TP_CONSTRAINT_LIMIT  */
    par_REACH_CONSTRAINT_TRIGGER = 283, /* par_REACH_CONSTRAINT_TRIGGER  */
    par_REACH_BITSIZE_TRIGGER = 284, /* par_REACH_BITSIZE_TRIGGER  */
    par_CONSTRAINT_BITSIZE = 285,  /* par_CONSTRAINT_BITSIZE  */
    par_SEARCH_METHOD = 286,       /* par_SEARCH_METHOD  */
    par_SEARCH_METHOD_TOPSORT_TOKENS = 287, /* par_SEARCH_METHOD_TOPSORT_TOKENS  */
    par_SNAPSHOT_INTERVAL = 288,   /* par_SNAPSHOT_INTERVAL  */
    par_REFINE_DERIVATIVE_METHOD = 289, /* par_REFINE_DERIVATIVE_METHOD  */
    par_REFINE_PRIORITIZE_REACH_SPLIT = 290, /* par_REFINE_PRIORITIZE_REACH_SPLIT  */
    par_REFINE_SMALLEST_FIRST = 291, /* par_REFINE_SMALLEST_FIRST  */
    par_REFINE_USE_FP = 292,       /* par_REFINE_USE_FP  */
    par_REFINE_DERIV_MINANGLE = 293, /* par_REFINE_DERIV_MINANGLE  */
    par_REFINE_PRIORITIZE_ANGLE = 294, /* par_REFINE_PRIORITIZE_ANGLE  */
    par_REFINE_CHECK_TIME_RELEVANCE = 295, /* par_REFINE_CHECK_TIME_RELEVANCE  */
    par_REFINE_CHECK_TIME_RELEVANCE_DURING = 296, /* par_REFINE_CHECK_TIME_RELEVANCE_DURING  */
    par_REFINE_CHECK_TIME_RELEVANCE_FINAL = 297, /* par_REFINE_CHECK_TIME_RELEVANCE_FINAL  */
    par_REFINE_CHECK_TRANS_DIMS = 298, /* par_REFINE_CHECK_TRANS_DIMS  */
    par_REFINE_PARTITION_INSIDE = 299, /* par_REFINE_PARTITION_INSIDE  */
    par_REACH_FB_REFINE_METHOD = 300, /* par_REACH_FB_REFINE_METHOD  */
    par_REFINE_MAX_CHECKS = 301,   /* par_REFINE_MAX_CHECKS  */
    par_REFINE_USE_NEW_SPLIT = 302, /* par_REFINE_USE_NEW_SPLIT  */
    par_MINIMIZE_FILTER_THRESHOLD = 303, /* par_MINIMIZE_FILTER_THRESHOLD  */
    PROJECT_TO = 304,              /* PROJECT_TO  */
    DIFFERENCE_ASSIGN = 305,       /* DIFFERENCE_ASSIGN  */
    RENAME = 306,                  /* RENAME  */
    IS_REACHABLE = 307,            /* IS_REACHABLE  */
    IS_REACHABLE_FB = 308,         /* IS_REACHABLE_FB  */
    INVARIANT_ASSIGN = 309,        /* INVARIANT_ASSIGN  */
    REVERSE = 310,                 /* REVERSE  */
    SAVE_FP_SURFACE = 311,         /* SAVE_FP_SURFACE  */
    REACH_FORWARD_ITER = 312,      /* REACH_FORWARD_ITER  */
    UNLOCK_SURFACE_LOCS = 313,     /* UNLOCK_SURFACE_LOCS  */
    UNLOCK_LOCS = 314,             /* UNLOCK_LOCS  */
    GET_INVARIANTS = 315,          /* GET_INVARIANTS  */
    PRINT_GRAPH = 316,             /* PRINT_GRAPH  */
    INVERSE = 317,                 /* INVERSE  */
    CONTAINS = 318,                /* CONTAINS  */
    REFINE_CONSTRAINTS = 319,      /* REFINE_CONSTRAINTS  */
    ADD_LABEL = 320,               /* ADD_LABEL  */
    WHO = 321,                     /* WHO  */
    SAVE_FP_INVARS = 322,          /* SAVE_FP_INVARS  */
    INITIAL_STATES = 323,          /* INITIAL_STATES  */
    REFINE_LOCS = 324,             /* REFINE_LOCS  */
    REFINE_LOC_DERIV = 325,        /* REFINE_LOC_DERIV  */
    IS_EMPTY = 326,                /* IS_EMPTY  */
    IS_INTERSECTING = 327,         /* IS_INTERSECTING  */
    INTERSECTION_ASSIGN = 328,     /* INTERSECTION_ASSIGN  */
    LOC_INTERSECTION = 329,        /* LOC_INTERSECTION  */
    LOC_UNION = 330,               /* LOC_UNION  */
    REMOVE = 331,                  /* REMOVE  */
    MY_ECHO = 332,                 /* MY_ECHO  */
    GET_PARAMETERS = 333,          /* GET_PARAMETERS  */
    PRINT = 334,                   /* PRINT  */
    SAVE_CON_FP = 335,             /* SAVE_CON_FP  */
    SAVE_GEN_FP = 336,             /* SAVE_GEN_FP  */
    REACH = 337,                   /* REACH  */
    REACH_STOP = 338,              /* REACH_STOP  */
    DEFNE = 339,                   /* DEFNE  */
    AUTOMATON = 340,               /* AUTOMATON  */
    INTERNAL_VAR = 341,            /* INTERNAL_VAR  */
    EXTERNAL_VAR = 342,            /* EXTERNAL_VAR  */
    PARAMETER = 343,               /* PARAMETER  */
    SYNCLABS = 344,                /* SYNCLABS  */
    INITIALLY = 345,               /* INITIALLY  */
    LOC = 346,                     /* LOC  */
    WHILE = 347,                   /* WHILE  */
    WHEN = 348,                    /* WHEN  */
    TRUE = 349,                    /* TRUE  */
    FALSE = 350,                   /* FALSE  */
    ASSIGN = 351,                  /* ASSIGN  */
    INT = 352,                     /* INT  */
    IDENT = 353,                   /* IDENT  */
    STARIDENT = 354,               /* STARIDENT  */
    STRING_TEXT = 355,             /* STRING_TEXT  */
    GE = 356,                      /* GE  */
    LE = 357,                      /* LE  */
    EQ = 358,                      /* EQ  */
    PRIM = 359                     /* PRIM  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define par_POLY_KIND 258
#define MERGE_SPLITTED 259
#define ASAP 260
#define par_TIME_POST_ITER 261
#define par_TIME_POST_CONE_LAMBDA 262
#define par_TIME_POST_CONE_WIDEN_DELAY 263
#define par_TIME_POST_CONE_WIDEN_PRECISION 264
#define par_MEMORY_MODE 265
#define par_MAINTAIN_BOXED_CCVS 266
#define par_PARSER_FIX_DPOST 267
#define par_REACH_CHEAP_CONTAINS 268
#define par_REACH_CHEAP_CONTAINS_USE_BBOX 269
#define par_REACH_USE_BBOX 270
#define par_REACH_USE_CONSTRAINT_HULL 271
#define par_REACH_USE_CONVEX_HULL 272
#define par_REACH_USE_TIME_ELAPSE 273
#define par_REACH_STOP_AT_FORB 274
#define par_REACH_MAX_ITER 275
#define par_REACH_REPORT_INTERVAL 276
#define par_REACH_USE_BBOX_ITER 277
#define par_REACH_STOP_USE_CONVEX_HULL_ITER 278
#define par_REACH_STOP_USE_CONVEX_HULL_SETTLE 279
#define par_REACH_CONSTRAINT_LIMIT 280
#define par_LIMIT_CONSTRAINTS_METHOD 281
#define par_TP_CONSTRAINT_LIMIT 282
#define par_REACH_CONSTRAINT_TRIGGER 283
#define par_REACH_BITSIZE_TRIGGER 284
#define par_CONSTRAINT_BITSIZE 285
#define par_SEARCH_METHOD 286
#define par_SEARCH_METHOD_TOPSORT_TOKENS 287
#define par_SNAPSHOT_INTERVAL 288
#define par_REFINE_DERIVATIVE_METHOD 289
#define par_REFINE_PRIORITIZE_REACH_SPLIT 290
#define par_REFINE_SMALLEST_FIRST 291
#define par_REFINE_USE_FP 292
#define par_REFINE_DERIV_MINANGLE 293
#define par_REFINE_PRIORITIZE_ANGLE 294
#define par_REFINE_CHECK_TIME_RELEVANCE 295
#define par_REFINE_CHECK_TIME_RELEVANCE_DURING 296
#define par_REFINE_CHECK_TIME_RELEVANCE_FINAL 297
#define par_REFINE_CHECK_TRANS_DIMS 298
#define par_REFINE_PARTITION_INSIDE 299
#define par_REACH_FB_REFINE_METHOD 300
#define par_REFINE_MAX_CHECKS 301
#define par_REFINE_USE_NEW_SPLIT 302
#define par_MINIMIZE_FILTER_THRESHOLD 303
#define PROJECT_TO 304
#define DIFFERENCE_ASSIGN 305
#define RENAME 306
#define IS_REACHABLE 307
#define IS_REACHABLE_FB 308
#define INVARIANT_ASSIGN 309
#define REVERSE 310
#define SAVE_FP_SURFACE 311
#define REACH_FORWARD_ITER 312
#define UNLOCK_SURFACE_LOCS 313
#define UNLOCK_LOCS 314
#define GET_INVARIANTS 315
#define PRINT_GRAPH 316
#define INVERSE 317
#define CONTAINS 318
#define REFINE_CONSTRAINTS 319
#define ADD_LABEL 320
#define WHO 321
#define SAVE_FP_INVARS 322
#define INITIAL_STATES 323
#define REFINE_LOCS 324
#define REFINE_LOC_DERIV 325
#define IS_EMPTY 326
#define IS_INTERSECTING 327
#define INTERSECTION_ASSIGN 328
#define LOC_INTERSECTION 329
#define LOC_UNION 330
#define REMOVE 331
#define MY_ECHO 332
#define GET_PARAMETERS 333
#define PRINT 334
#define SAVE_CON_FP 335
#define SAVE_GEN_FP 336
#define REACH 337
#define REACH_STOP 338
#define DEFNE 339
#define AUTOMATON 340
#define INTERNAL_VAR 341
#define EXTERNAL_VAR 342
#define PARAMETER 343
#define SYNCLABS 344
#define INITIALLY 345
#define LOC 346
#define WHILE 347
#define WHEN 348
#define TRUE 349
#define FALSE 350
#define ASSIGN 351
#define INT 352
#define IDENT 353
#define STARIDENT 354
#define STRING_TEXT 355
#define GE 356
#define LE 357
#define EQ 358
#define PRIM 359

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 151 "../../../phaver/src/parser.yy"

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

#line 476 "parser.cc"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_HH_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_par_POLY_KIND = 3,              /* par_POLY_KIND  */
  YYSYMBOL_MERGE_SPLITTED = 4,             /* MERGE_SPLITTED  */
  YYSYMBOL_ASAP = 5,                       /* ASAP  */
  YYSYMBOL_par_TIME_POST_ITER = 6,         /* par_TIME_POST_ITER  */
  YYSYMBOL_par_TIME_POST_CONE_LAMBDA = 7,  /* par_TIME_POST_CONE_LAMBDA  */
  YYSYMBOL_par_TIME_POST_CONE_WIDEN_DELAY = 8, /* par_TIME_POST_CONE_WIDEN_DELAY  */
  YYSYMBOL_par_TIME_POST_CONE_WIDEN_PRECISION = 9, /* par_TIME_POST_CONE_WIDEN_PRECISION  */
  YYSYMBOL_par_MEMORY_MODE = 10,           /* par_MEMORY_MODE  */
  YYSYMBOL_par_MAINTAIN_BOXED_CCVS = 11,   /* par_MAINTAIN_BOXED_CCVS  */
  YYSYMBOL_par_PARSER_FIX_DPOST = 12,      /* par_PARSER_FIX_DPOST  */
  YYSYMBOL_par_REACH_CHEAP_CONTAINS = 13,  /* par_REACH_CHEAP_CONTAINS  */
  YYSYMBOL_par_REACH_CHEAP_CONTAINS_USE_BBOX = 14, /* par_REACH_CHEAP_CONTAINS_USE_BBOX  */
  YYSYMBOL_par_REACH_USE_BBOX = 15,        /* par_REACH_USE_BBOX  */
  YYSYMBOL_par_REACH_USE_CONSTRAINT_HULL = 16, /* par_REACH_USE_CONSTRAINT_HULL  */
  YYSYMBOL_par_REACH_USE_CONVEX_HULL = 17, /* par_REACH_USE_CONVEX_HULL  */
  YYSYMBOL_par_REACH_USE_TIME_ELAPSE = 18, /* par_REACH_USE_TIME_ELAPSE  */
  YYSYMBOL_par_REACH_STOP_AT_FORB = 19,    /* par_REACH_STOP_AT_FORB  */
  YYSYMBOL_par_REACH_MAX_ITER = 20,        /* par_REACH_MAX_ITER  */
  YYSYMBOL_par_REACH_REPORT_INTERVAL = 21, /* par_REACH_REPORT_INTERVAL  */
  YYSYMBOL_par_REACH_USE_BBOX_ITER = 22,   /* par_REACH_USE_BBOX_ITER  */
  YYSYMBOL_par_REACH_STOP_USE_CONVEX_HULL_ITER = 23, /* par_REACH_STOP_USE_CONVEX_HULL_ITER  */
  YYSYMBOL_par_REACH_STOP_USE_CONVEX_HULL_SETTLE = 24, /* par_REACH_STOP_USE_CONVEX_HULL_SETTLE  */
  YYSYMBOL_par_REACH_CONSTRAINT_LIMIT = 25, /* par_REACH_CONSTRAINT_LIMIT  */
  YYSYMBOL_par_LIMIT_CONSTRAINTS_METHOD = 26, /* par_LIMIT_CONSTRAINTS_METHOD  */
  YYSYMBOL_par_TP_CONSTRAINT_LIMIT = 27,   /* par_TP_CONSTRAINT_LIMIT  */
  YYSYMBOL_par_REACH_CONSTRAINT_TRIGGER = 28, /* par_REACH_CONSTRAINT_TRIGGER  */
  YYSYMBOL_par_REACH_BITSIZE_TRIGGER = 29, /* par_REACH_BITSIZE_TRIGGER  */
  YYSYMBOL_par_CONSTRAINT_BITSIZE = 30,    /* par_CONSTRAINT_BITSIZE  */
  YYSYMBOL_par_SEARCH_METHOD = 31,         /* par_SEARCH_METHOD  */
  YYSYMBOL_par_SEARCH_METHOD_TOPSORT_TOKENS = 32, /* par_SEARCH_METHOD_TOPSORT_TOKENS  */
  YYSYMBOL_par_SNAPSHOT_INTERVAL = 33,     /* par_SNAPSHOT_INTERVAL  */
  YYSYMBOL_par_REFINE_DERIVATIVE_METHOD = 34, /* par_REFINE_DERIVATIVE_METHOD  */
  YYSYMBOL_par_REFINE_PRIORITIZE_REACH_SPLIT = 35, /* par_REFINE_PRIORITIZE_REACH_SPLIT  */
  YYSYMBOL_par_REFINE_SMALLEST_FIRST = 36, /* par_REFINE_SMALLEST_FIRST  */
  YYSYMBOL_par_REFINE_USE_FP = 37,         /* par_REFINE_USE_FP  */
  YYSYMBOL_par_REFINE_DERIV_MINANGLE = 38, /* par_REFINE_DERIV_MINANGLE  */
  YYSYMBOL_par_REFINE_PRIORITIZE_ANGLE = 39, /* par_REFINE_PRIORITIZE_ANGLE  */
  YYSYMBOL_par_REFINE_CHECK_TIME_RELEVANCE = 40, /* par_REFINE_CHECK_TIME_RELEVANCE  */
  YYSYMBOL_par_REFINE_CHECK_TIME_RELEVANCE_DURING = 41, /* par_REFINE_CHECK_TIME_RELEVANCE_DURING  */
  YYSYMBOL_par_REFINE_CHECK_TIME_RELEVANCE_FINAL = 42, /* par_REFINE_CHECK_TIME_RELEVANCE_FINAL  */
  YYSYMBOL_par_REFINE_CHECK_TRANS_DIMS = 43, /* par_REFINE_CHECK_TRANS_DIMS  */
  YYSYMBOL_par_REFINE_PARTITION_INSIDE = 44, /* par_REFINE_PARTITION_INSIDE  */
  YYSYMBOL_par_REACH_FB_REFINE_METHOD = 45, /* par_REACH_FB_REFINE_METHOD  */
  YYSYMBOL_par_REFINE_MAX_CHECKS = 46,     /* par_REFINE_MAX_CHECKS  */
  YYSYMBOL_par_REFINE_USE_NEW_SPLIT = 47,  /* par_REFINE_USE_NEW_SPLIT  */
  YYSYMBOL_par_MINIMIZE_FILTER_THRESHOLD = 48, /* par_MINIMIZE_FILTER_THRESHOLD  */
  YYSYMBOL_PROJECT_TO = 49,                /* PROJECT_TO  */
  YYSYMBOL_DIFFERENCE_ASSIGN = 50,         /* DIFFERENCE_ASSIGN  */
  YYSYMBOL_RENAME = 51,                    /* RENAME  */
  YYSYMBOL_IS_REACHABLE = 52,              /* IS_REACHABLE  */
  YYSYMBOL_IS_REACHABLE_FB = 53,           /* IS_REACHABLE_FB  */
  YYSYMBOL_INVARIANT_ASSIGN = 54,          /* INVARIANT_ASSIGN  */
  YYSYMBOL_REVERSE = 55,                   /* REVERSE  */
  YYSYMBOL_SAVE_FP_SURFACE = 56,           /* SAVE_FP_SURFACE  */
  YYSYMBOL_REACH_FORWARD_ITER = 57,        /* REACH_FORWARD_ITER  */
  YYSYMBOL_UNLOCK_SURFACE_LOCS = 58,       /* UNLOCK_SURFACE_LOCS  */
  YYSYMBOL_UNLOCK_LOCS = 59,               /* UNLOCK_LOCS  */
  YYSYMBOL_GET_INVARIANTS = 60,            /* GET_INVARIANTS  */
  YYSYMBOL_PRINT_GRAPH = 61,               /* PRINT_GRAPH  */
  YYSYMBOL_INVERSE = 62,                   /* INVERSE  */
  YYSYMBOL_CONTAINS = 63,                  /* CONTAINS  */
  YYSYMBOL_REFINE_CONSTRAINTS = 64,        /* REFINE_CONSTRAINTS  */
  YYSYMBOL_ADD_LABEL = 65,                 /* ADD_LABEL  */
  YYSYMBOL_WHO = 66,                       /* WHO  */
  YYSYMBOL_SAVE_FP_INVARS = 67,            /* SAVE_FP_INVARS  */
  YYSYMBOL_INITIAL_STATES = 68,            /* INITIAL_STATES  */
  YYSYMBOL_REFINE_LOCS = 69,               /* REFINE_LOCS  */
  YYSYMBOL_REFINE_LOC_DERIV = 70,          /* REFINE_LOC_DERIV  */
  YYSYMBOL_IS_EMPTY = 71,                  /* IS_EMPTY  */
  YYSYMBOL_IS_INTERSECTING = 72,           /* IS_INTERSECTING  */
  YYSYMBOL_INTERSECTION_ASSIGN = 73,       /* INTERSECTION_ASSIGN  */
  YYSYMBOL_LOC_INTERSECTION = 74,          /* LOC_INTERSECTION  */
  YYSYMBOL_LOC_UNION = 75,                 /* LOC_UNION  */
  YYSYMBOL_REMOVE = 76,                    /* REMOVE  */
  YYSYMBOL_MY_ECHO = 77,                   /* MY_ECHO  */
  YYSYMBOL_GET_PARAMETERS = 78,            /* GET_PARAMETERS  */
  YYSYMBOL_PRINT = 79,                     /* PRINT  */
  YYSYMBOL_SAVE_CON_FP = 80,               /* SAVE_CON_FP  */
  YYSYMBOL_SAVE_GEN_FP = 81,               /* SAVE_GEN_FP  */
  YYSYMBOL_REACH = 82,                     /* REACH  */
  YYSYMBOL_REACH_STOP = 83,                /* REACH_STOP  */
  YYSYMBOL_DEFNE = 84,                     /* DEFNE  */
  YYSYMBOL_AUTOMATON = 85,                 /* AUTOMATON  */
  YYSYMBOL_INTERNAL_VAR = 86,              /* INTERNAL_VAR  */
  YYSYMBOL_EXTERNAL_VAR = 87,              /* EXTERNAL_VAR  */
  YYSYMBOL_PARAMETER = 88,                 /* PARAMETER  */
  YYSYMBOL_SYNCLABS = 89,                  /* SYNCLABS  */
  YYSYMBOL_INITIALLY = 90,                 /* INITIALLY  */
  YYSYMBOL_LOC = 91,                       /* LOC  */
  YYSYMBOL_WHILE = 92,                     /* WHILE  */
  YYSYMBOL_WHEN = 93,                      /* WHEN  */
  YYSYMBOL_TRUE = 94,                      /* TRUE  */
  YYSYMBOL_FALSE = 95,                     /* FALSE  */
  YYSYMBOL_ASSIGN = 96,                    /* ASSIGN  */
  YYSYMBOL_INT = 97,                       /* INT  */
  YYSYMBOL_IDENT = 98,                     /* IDENT  */
  YYSYMBOL_STARIDENT = 99,                 /* STARIDENT  */
  YYSYMBOL_STRING_TEXT = 100,              /* STRING_TEXT  */
  YYSYMBOL_101_ = 101,                     /* '|'  */
  YYSYMBOL_102_ = 102,                     /* '&'  */
  YYSYMBOL_GE = 103,                       /* GE  */
  YYSYMBOL_LE = 104,                       /* LE  */
  YYSYMBOL_EQ = 105,                       /* EQ  */
  YYSYMBOL_106_ = 106,                     /* '<'  */
  YYSYMBOL_107_ = 107,                     /* '>'  */
  YYSYMBOL_108_ = 108,                     /* '+'  */
  YYSYMBOL_109_ = 109,                     /* '-'  */
  YYSYMBOL_110_ = 110,                     /* '*'  */
  YYSYMBOL_111_ = 111,                     /* '/'  */
  YYSYMBOL_112_ = 112,                     /* '!'  */
  YYSYMBOL_113_ = 113,                     /* '('  */
  YYSYMBOL_114_ = 114,                     /* ')'  */
  YYSYMBOL_PRIM = 115,                     /* PRIM  */
  YYSYMBOL_116_ = 116,                     /* ';'  */
  YYSYMBOL_117_ = 117,                     /* '='  */
  YYSYMBOL_118_ = 118,                     /* '.'  */
  YYSYMBOL_119_ = 119,                     /* ','  */
  YYSYMBOL_120_ = 120,                     /* '['  */
  YYSYMBOL_121_ = 121,                     /* ']'  */
  YYSYMBOL_122_ = 122,                     /* '{'  */
  YYSYMBOL_123_ = 123,                     /* '}'  */
  YYSYMBOL_124_ = 124,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 125,                 /* $accept  */
  YYSYMBOL_program = 126,                  /* program  */
  YYSYMBOL_commands = 127,                 /* commands  */
  YYSYMBOL_command = 128,                  /* command  */
  YYSYMBOL_129_1 = 129,                    /* $@1  */
  YYSYMBOL_130_2 = 130,                    /* $@2  */
  YYSYMBOL_131_3 = 131,                    /* $@3  */
  YYSYMBOL_132_4 = 132,                    /* $@4  */
  YYSYMBOL_133_5 = 133,                    /* $@5  */
  YYSYMBOL_DO = 134,                       /* DO  */
  YYSYMBOL_END = 135,                      /* END  */
  YYSYMBOL_GOTO = 136,                     /* GOTO  */
  YYSYMBOL_SYNC = 137,                     /* SYNC  */
  YYSYMBOL_WAIT = 138,                     /* WAIT  */
  YYSYMBOL_var_ref_list = 139,             /* var_ref_list  */
  YYSYMBOL_prelim = 140,                   /* prelim  */
  YYSYMBOL_bool_type = 141,                /* bool_type  */
  YYSYMBOL_automaton = 142,                /* automaton  */
  YYSYMBOL_143_6 = 143,                    /* $@6  */
  YYSYMBOL_automaton_body = 144,           /* automaton_body  */
  YYSYMBOL_declaration = 145,              /* declaration  */
  YYSYMBOL_synclab = 146,                  /* synclab  */
  YYSYMBOL_internal_vars = 147,            /* internal_vars  */
  YYSYMBOL_external_vars = 148,            /* external_vars  */
  YYSYMBOL_parameters = 149,               /* parameters  */
  YYSYMBOL_ivar_list = 150,                /* ivar_list  */
  YYSYMBOL_evar_list = 151,                /* evar_list  */
  YYSYMBOL_param_list = 152,               /* param_list  */
  YYSYMBOL_ident_list = 153,               /* ident_list  */
  YYSYMBOL_compose_list = 154,             /* compose_list  */
  YYSYMBOL_initial = 155,                  /* initial  */
  YYSYMBOL_state_list = 156,               /* state_list  */
  YYSYMBOL_location_list = 157,            /* location_list  */
  YYSYMBOL_location = 158,                 /* location  */
  YYSYMBOL_transition_list = 159,          /* transition_list  */
  YYSYMBOL_transition = 160,               /* transition  */
  YYSYMBOL_refinement_cons = 161,          /* refinement_cons  */
  YYSYMBOL_refinement_con = 162,           /* refinement_con  */
  YYSYMBOL_state_val_set = 163,            /* state_val_set  */
  YYSYMBOL_164_7 = 164,                    /* $@7  */
  YYSYMBOL_dpost_cons = 165,               /* dpost_cons  */
  YYSYMBOL_val_set = 166,                  /* val_set  */
  YYSYMBOL_constr_list = 167,              /* constr_list  */
  YYSYMBOL_constr_list_no_and = 168,       /* constr_list_no_and  */
  YYSYMBOL_constr = 169,                   /* constr  */
  YYSYMBOL_rat_aff_expr = 170              /* rat_aff_expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  106
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   636

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  125
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  46
/* YYNRULES -- Number of rules.  */
#define YYNRULES  184
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  573

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   359


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   112,     2,     2,     2,     2,   102,     2,
     113,   114,   110,   108,   119,   109,   118,   111,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   124,   116,
     106,   117,   107,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   120,     2,   121,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   122,   101,   123,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   103,   104,   105,   115
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   302,   302,   305,   306,   312,   313,   317,   319,   341,
     356,   379,   419,   438,   437,   455,   454,   479,   478,   498,
     520,   531,   542,   556,   570,   582,   599,   613,   627,   644,
     656,   674,   701,   715,   730,   729,   761,   770,   781,   794,
     806,   825,   844,   870,   897,   924,   930,   961,   971,   970,
     984,   996,  1008,  1023,  1040,  1042,  1044,  1046,  1048,  1051,
    1079,  1102,  1109,  1118,  1120,  1125,  1127,  1129,  1132,  1135,
    1137,  1139,  1141,  1143,  1145,  1147,  1149,  1151,  1153,  1155,
    1157,  1159,  1161,  1163,  1165,  1167,  1169,  1171,  1173,  1179,
    1181,  1183,  1185,  1187,  1189,  1191,  1209,  1211,  1214,  1216,
    1218,  1220,  1222,  1224,  1226,  1228,  1230,  1235,  1236,  1241,
    1240,  1259,  1262,  1264,  1265,  1266,  1267,  1271,  1285,  1289,
    1293,  1297,  1303,  1312,  1318,  1327,  1333,  1342,  1348,  1355,
    1365,  1377,  1383,  1396,  1401,  1409,  1422,  1438,  1439,  1440,
    1441,  1445,  1461,  1462,  1469,  1485,  1501,  1515,  1532,  1549,
    1566,  1572,  1581,  1591,  1605,  1605,  1615,  1618,  1624,  1630,
    1635,  1639,  1650,  1658,  1661,  1667,  1678,  1689,  1700,  1715,
    1724,  1733,  1742,  1751,  1760,  1765,  1773,  1780,  1794,  1816,
    1826,  1834,  1840,  1846,  1851
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "par_POLY_KIND",
  "MERGE_SPLITTED", "ASAP", "par_TIME_POST_ITER",
  "par_TIME_POST_CONE_LAMBDA", "par_TIME_POST_CONE_WIDEN_DELAY",
  "par_TIME_POST_CONE_WIDEN_PRECISION", "par_MEMORY_MODE",
  "par_MAINTAIN_BOXED_CCVS", "par_PARSER_FIX_DPOST",
  "par_REACH_CHEAP_CONTAINS", "par_REACH_CHEAP_CONTAINS_USE_BBOX",
  "par_REACH_USE_BBOX", "par_REACH_USE_CONSTRAINT_HULL",
  "par_REACH_USE_CONVEX_HULL", "par_REACH_USE_TIME_ELAPSE",
  "par_REACH_STOP_AT_FORB", "par_REACH_MAX_ITER",
  "par_REACH_REPORT_INTERVAL", "par_REACH_USE_BBOX_ITER",
  "par_REACH_STOP_USE_CONVEX_HULL_ITER",
  "par_REACH_STOP_USE_CONVEX_HULL_SETTLE", "par_REACH_CONSTRAINT_LIMIT",
  "par_LIMIT_CONSTRAINTS_METHOD", "par_TP_CONSTRAINT_LIMIT",
  "par_REACH_CONSTRAINT_TRIGGER", "par_REACH_BITSIZE_TRIGGER",
  "par_CONSTRAINT_BITSIZE", "par_SEARCH_METHOD",
  "par_SEARCH_METHOD_TOPSORT_TOKENS", "par_SNAPSHOT_INTERVAL",
  "par_REFINE_DERIVATIVE_METHOD", "par_REFINE_PRIORITIZE_REACH_SPLIT",
  "par_REFINE_SMALLEST_FIRST", "par_REFINE_USE_FP",
  "par_REFINE_DERIV_MINANGLE", "par_REFINE_PRIORITIZE_ANGLE",
  "par_REFINE_CHECK_TIME_RELEVANCE",
  "par_REFINE_CHECK_TIME_RELEVANCE_DURING",
  "par_REFINE_CHECK_TIME_RELEVANCE_FINAL", "par_REFINE_CHECK_TRANS_DIMS",
  "par_REFINE_PARTITION_INSIDE", "par_REACH_FB_REFINE_METHOD",
  "par_REFINE_MAX_CHECKS", "par_REFINE_USE_NEW_SPLIT",
  "par_MINIMIZE_FILTER_THRESHOLD", "PROJECT_TO", "DIFFERENCE_ASSIGN",
  "RENAME", "IS_REACHABLE", "IS_REACHABLE_FB", "INVARIANT_ASSIGN",
  "REVERSE", "SAVE_FP_SURFACE", "REACH_FORWARD_ITER",
  "UNLOCK_SURFACE_LOCS", "UNLOCK_LOCS", "GET_INVARIANTS", "PRINT_GRAPH",
  "INVERSE", "CONTAINS", "REFINE_CONSTRAINTS", "ADD_LABEL", "WHO",
  "SAVE_FP_INVARS", "INITIAL_STATES", "REFINE_LOCS", "REFINE_LOC_DERIV",
  "IS_EMPTY", "IS_INTERSECTING", "INTERSECTION_ASSIGN", "LOC_INTERSECTION",
  "LOC_UNION", "REMOVE", "MY_ECHO", "GET_PARAMETERS", "PRINT",
  "SAVE_CON_FP", "SAVE_GEN_FP", "REACH", "REACH_STOP", "DEFNE",
  "AUTOMATON", "INTERNAL_VAR", "EXTERNAL_VAR", "PARAMETER", "SYNCLABS",
  "INITIALLY", "LOC", "WHILE", "WHEN", "TRUE", "FALSE", "ASSIGN", "INT",
  "IDENT", "STARIDENT", "STRING_TEXT", "'|'", "'&'", "GE", "LE", "EQ",
  "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'!'", "'('", "')'", "PRIM",
  "';'", "'='", "'.'", "','", "'['", "']'", "'{'", "'}'", "':'", "$accept",
  "program", "commands", "command", "$@1", "$@2", "$@3", "$@4", "$@5",
  "DO", "END", "GOTO", "SYNC", "WAIT", "var_ref_list", "prelim",
  "bool_type", "automaton", "$@6", "automaton_body", "declaration",
  "synclab", "internal_vars", "external_vars", "parameters", "ivar_list",
  "evar_list", "param_list", "ident_list", "compose_list", "initial",
  "state_list", "location_list", "location", "transition_list",
  "transition", "refinement_cons", "refinement_con", "state_val_set",
  "$@7", "dpost_cons", "val_set", "constr_list", "constr_list_no_and",
  "constr", "rat_aff_expr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-498)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-58)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     183,  -110,   -82,   -75,   -21,     0,    10,    40,    49,    65,
     130,   172,   174,   179,   196,   197,   215,   216,   217,   218,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   -91,   -30,    42,    70,   -70,
     170,   183,  -498,  -498,  -498,   169,   265,    56,   266,   267,
     268,    34,    34,    34,    34,    34,    34,    34,    34,    34,
       2,   269,   270,   271,    34,   272,    34,   273,   274,   275,
     276,   277,   278,   279,   280,    34,    34,    34,    56,    34,
      34,    34,    34,    34,    34,   281,   282,    34,   284,  -498,
     213,   285,  -498,    56,   286,    51,  -498,  -498,   283,   287,
    -498,   194,    56,    56,   154,   289,   290,   291,  -498,  -498,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     288,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   323,   163,   324,
     326,   327,   328,   329,   330,   331,   332,   333,   334,  -498,
     263,  -498,   167,   -31,    32,   338,   339,   340,   341,   342,
     343,   344,   345,   346,  -498,   348,   349,   350,   351,   352,
     353,   354,   357,   355,   356,    33,   359,   260,   337,  -498,
    -498,  -498,  -498,    53,    56,    56,    56,    56,  -498,  -498,
    -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,
    -498,  -498,   358,  -498,  -498,  -498,  -498,  -498,  -498,  -498,
    -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,
    -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,
    -498,    34,   367,   219,  -498,   368,  -498,    16,   375,  -498,
     382,   383,   384,  -498,   385,   386,  -498,   387,   388,   370,
     390,   389,   392,   393,  -498,  -498,   394,   395,  -498,  -498,
     396,  -498,   397,   381,   391,  -498,   126,   126,  -498,  -498,
    -498,   398,  -498,  -498,   264,   371,   374,   376,   401,  -498,
    -498,  -498,  -498,   155,   399,  -498,   400,   404,   405,   406,
     407,   408,   409,   410,    71,   379,  -498,   413,   335,   414,
     415,   416,   402,   417,   419,   420,   421,   422,   -47,   423,
     424,   403,   412,   411,   411,   425,   441,   442,   444,   445,
     378,   -74,  -498,   428,  -498,   418,  -498,   447,   448,   450,
    -498,  -498,  -498,  -498,   451,  -498,   452,   432,   453,   436,
     437,   438,  -498,   439,    56,   440,  -498,   446,   449,   454,
     455,   458,   456,   457,   460,   459,  -498,   -25,   -22,  -498,
    -498,   150,  -498,   205,  -498,   206,   461,   207,   466,   462,
     452,   211,  -498,   418,   418,  -498,  -498,   463,   464,   465,
     467,    62,  -498,   468,  -498,  -498,  -498,   469,  -498,   131,
      -5,  -498,  -498,  -498,  -498,   470,  -498,  -498,   471,  -498,
     472,   476,   474,  -498,   478,  -498,   482,  -498,   486,   445,
    -498,   488,  -498,  -498,   212,  -498,   489,   418,    -3,    38,
     477,   479,   480,   481,   483,   484,   411,    56,   487,  -498,
     495,   490,  -498,  -498,  -498,  -498,  -498,  -498,   125,   485,
     496,  -498,  -498,   500,   505,   506,   491,   445,  -498,  -498,
      38,    38,   201,  -498,  -498,   148,  -498,  -498,  -498,  -498,
    -498,  -498,   192,   124,   492,   497,  -498,  -498,   445,  -498,
     493,  -498,   494,   445,    47,   -92,  -498,    57,    69,    38,
      38,    56,    56,    56,    56,    56,   498,  -498,    56,  -498,
     502,   193,    47,  -498,    47,   -92,   380,   503,  -498,   511,
     499,   512,  -498,  -498,  -498,   209,   176,   209,   184,   209,
     514,   190,  -498,  -498,   -72,   443,   501,   516,   521,    47,
      47,   504,    56,    56,    56,    56,   508,  -498,  -498,   521,
      47,   509,  -498,   445,  -498,   475,  -498,   209,   209,   209,
     209,   510,   445,   507,  -498,   -33,   526,  -498,   -33,   526,
    -498,   529,   530,   531,   533,   517,   518,   519,   520,  -498,
    -498,  -498,  -498
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     2,     4,     5,     6,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     8,
       0,     0,   109,     0,     0,    15,     1,     3,     0,     0,
     176,   178,     0,     0,     0,     0,     0,     0,   107,   108,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     7,
       0,   112,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    61,
      63,   177,   183,     0,     0,     0,     0,     0,    64,    65,
      66,    67,    71,    68,    69,    70,    72,    73,    74,    75,
      76,    77,     0,    79,    80,    81,    82,    90,    83,    91,
      92,    93,    94,    95,    96,    97,    84,    85,    86,    87,
      88,    89,    98,    99,   100,   101,   102,   103,   104,   105,
     106,     0,     0,     0,    62,     0,     9,    13,     0,    31,
       0,     0,     0,    36,     0,     0,    53,     0,     0,     0,
       0,     0,     0,     0,    45,    29,     0,     0,    21,    20,
       0,    10,     0,     0,     0,   184,   181,   182,   179,   180,
      78,     0,    55,   110,     0,     0,     0,     0,     0,   115,
     113,   114,   116,     0,   143,   132,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   131,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   111,   143,   143,   139,    24,     0,     0,     0,
      51,    50,    23,    22,     0,    39,     0,     0,     0,     0,
       0,     0,    34,     0,     0,     0,   151,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    59,     0,     0,    19,
     122,     0,   124,     0,   126,     0,   128,     0,     0,     0,
       0,     0,   143,   137,   140,   154,   142,     0,     0,     0,
       0,     0,    27,     0,    41,    33,    52,     0,    28,     0,
       0,    37,    32,    40,    47,     0,    25,    26,     0,    12,
       0,     0,     0,   118,     0,   119,     0,   120,     0,     0,
     117,     0,   154,   154,     0,   134,     0,   138,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   150,
       0,     0,    16,    60,    18,   121,   123,   125,     0,   127,
       0,   135,   133,     0,     0,    54,     0,     0,   174,   175,
       0,     0,   155,   161,   164,     0,    43,    44,    38,    42,
      14,    30,     0,     0,     0,     0,    11,   130,     0,    58,
       0,   154,     0,     0,     0,     0,   159,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   152,     0,    49,
       0,     0,     0,   136,     0,     0,     0,   156,   163,    54,
       0,     0,   160,   157,   158,   171,   172,   173,   169,   170,
       0,     0,    46,   129,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   153,   141,     0,
       0,     0,    57,     0,   162,     0,   146,   165,   167,   166,
     168,     0,     0,     0,   149,     0,     0,    35,     0,     0,
      56,     0,     0,     0,     0,     0,     0,     0,     0,   145,
     144,   148,   147
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -498,  -498,  -498,   336,  -498,  -498,  -498,  -498,  -498,  -425,
    -498,  -497,  -451,  -498,  -323,  -498,   -53,  -498,  -498,  -498,
    -498,  -498,  -498,  -498,  -498,  -498,  -498,  -498,  -400,  -498,
    -498,  -301,  -498,    96,  -316,  -498,  -498,   -10,  -418,  -498,
    -476,  -408,  -480,  -386,  -498,   -57
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    50,    51,    52,   305,   187,   188,   397,   259,   456,
     283,   511,   457,   480,   367,    53,   120,    54,   161,   242,
     243,   289,   290,   291,   292,   371,   373,   375,   377,   164,
     332,   381,   293,   294,   335,   386,   355,   356,   428,   429,
     506,   462,   507,   508,   464,   465
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     114,   368,   454,   483,   450,   451,   509,    55,   527,   121,
     122,   123,   124,   125,   126,   127,   128,   383,   384,   448,
     296,   134,   524,   136,   379,    99,   103,   421,   525,   482,
     529,   148,   145,   146,   147,    56,   149,   150,   151,   152,
     153,   154,    57,   463,   157,   391,   162,   104,   105,   544,
     380,   538,   486,   487,   545,   192,   193,   485,   561,   562,
     510,   563,   564,   503,   553,   560,   427,   360,   297,   298,
     100,   245,   361,   299,   463,   463,   300,   543,   501,   424,
     526,   513,   514,   505,   301,   246,   421,   247,   552,   410,
     302,   303,   412,   438,   411,   455,    58,   411,   304,   129,
     -17,   165,   166,   463,   463,   167,   168,   169,   354,   170,
     171,   130,   172,   472,   173,   174,   175,    59,   176,   177,
     178,   179,   180,   181,   182,   183,   184,    60,   118,   119,
     185,   186,   458,   459,   248,   110,   111,   276,   277,   278,
     279,   458,   459,   555,   110,   111,   270,   112,   249,   271,
     460,   461,   558,   110,   111,   101,   112,    61,   489,   490,
     113,   194,   195,   196,   197,   112,    62,   275,   102,   113,
     106,   512,   491,   492,   493,   494,   495,   194,   195,   196,
     197,   426,    63,   275,   344,   434,     1,   345,   281,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,   194,   195,   196,   197,   196,   197,   497,   194,
     195,   196,   197,   498,   421,   331,   288,    64,   477,    45,
     437,   491,   492,   493,   494,   495,   194,   195,   196,   197,
      46,    47,   194,   195,   196,   197,   413,   108,    48,   414,
     198,   194,   195,   196,   197,   194,   195,   196,   197,   230,
     532,    49,   533,   244,   194,   195,   196,   197,   534,    65,
     535,    66,   194,   195,   196,   197,    67,   399,   194,   195,
     196,   197,   489,   490,   537,   284,   285,   286,   287,   191,
     288,   411,   421,    68,    69,   496,   523,   194,   195,   196,
     197,   415,   417,   420,   416,   418,   421,   425,   452,   159,
     426,   426,    70,    71,    72,    73,   273,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,   109,   115,   116,   117,   131,   132,   133,   135,
     137,   138,   139,   140,   141,   142,   143,   144,   155,   156,
     473,   158,   241,   160,   163,   212,   274,   107,   326,   333,
     439,     0,     0,     0,     0,     0,     0,     0,     0,   189,
       0,     0,     0,   190,   488,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   515,   516,   517,   518,   519,   229,
     231,   521,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   250,   251,   252,   348,   254,   255,   253,   257,   258,
     256,   260,   261,   262,   263,   282,   295,   266,   264,   265,
     267,   268,   269,   306,   280,   547,   548,   549,   550,   272,
     307,   308,   309,   314,   311,   310,   313,   312,   315,   316,
     317,   318,   319,   320,   323,   327,   321,   322,   328,   330,
     329,   346,   378,   528,   324,     0,     0,     0,     0,   366,
       0,   385,   325,     0,     0,   334,   336,   337,   338,   339,
       0,   352,   364,   340,   341,   342,   343,   347,   349,   350,
     351,   353,   354,   365,   357,   358,   359,   362,   363,   370,
     372,   369,   374,   376,   382,   387,   388,   389,   392,   390,
     379,   393,   394,   395,   396,   398,   405,   408,   422,   400,
       0,     0,   401,     0,   423,   402,   539,     0,     0,     0,
     403,   404,   406,   407,   443,   409,   445,   430,   431,   432,
     446,   433,   435,   419,   447,   441,   449,   453,   442,   440,
     444,   436,   475,   466,   479,   467,   468,   469,   556,   470,
     471,   474,   481,   455,   -57,   529,   476,   478,   499,   -56,
     531,   500,   536,   484,   541,   502,   504,   520,   522,   542,
     546,   530,   551,   540,   560,   554,   557,   565,   566,   567,
     559,   568,     0,   569,   570,   571,   572
};

static const yytype_int16 yycheck[] =
{
      57,   324,     5,   454,   422,   423,    98,   117,   505,    62,
      63,    64,    65,    66,    67,    68,    69,   333,   334,   419,
       4,    74,   502,    76,    98,   116,    96,   119,   504,   454,
     102,    88,    85,    86,    87,   117,    89,    90,    91,    92,
      93,    94,   117,   429,    97,   346,   103,   117,   118,   529,
     124,   123,   460,   461,   530,   112,   113,   457,   555,   556,
     485,   558,   559,   481,   540,    98,   382,   114,    52,    53,
     100,   102,   119,    57,   460,   461,    60,   528,   478,   380,
     505,   489,   490,   483,    68,   116,   119,   118,   539,   114,
      74,    75,   114,    98,   119,    98,   117,   119,    82,    97,
      49,    50,    51,   489,   490,    54,    55,    56,   113,    58,
      59,   109,    61,   436,    63,    64,    65,   117,    67,    68,
      69,    70,    71,    72,    73,    74,    75,   117,    94,    95,
      79,    80,    94,    95,   102,    97,    98,   194,   195,   196,
     197,    94,    95,   543,    97,    98,   113,   109,   116,   116,
     112,   113,   552,    97,    98,   113,   109,   117,   101,   102,
     113,   108,   109,   110,   111,   109,   117,   114,    98,   113,
       0,   114,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   119,   117,   114,   113,   123,     3,   116,   241,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,   108,   109,   110,   111,   110,   111,   114,   108,
     109,   110,   111,   119,   119,    90,    91,   117,   123,    66,
     119,   103,   104,   105,   106,   107,   108,   109,   110,   111,
      77,    78,   108,   109,   110,   111,   116,    98,    85,   119,
     116,   108,   109,   110,   111,   108,   109,   110,   111,   116,
     104,    98,   106,   116,   108,   109,   110,   111,   104,   117,
     106,   117,   108,   109,   110,   111,   117,   354,   108,   109,
     110,   111,   101,   102,   114,    86,    87,    88,    89,   115,
      91,   119,   119,   117,   117,   123,   123,   108,   109,   110,
     111,   116,   116,   116,   119,   119,   119,   116,   116,   116,
     119,   119,   117,   117,   117,   117,    76,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,    97,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,    97,    97,    97,
     437,    97,   119,    98,    98,    97,    49,    51,   124,   293,
     400,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,
      -1,    -1,    -1,   116,   461,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   491,   492,   493,   494,   495,   116,
     116,   498,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   113,   113,   113,   119,   113,   113,   116,   113,   113,
     116,   113,   113,   113,   113,    98,    98,   113,   116,   116,
     113,   116,   116,    98,   116,   532,   533,   534,   535,   120,
      98,    98,    98,   113,    98,   100,    98,   100,    98,   100,
      98,    98,    98,    98,   113,   124,   100,   100,   124,    98,
     124,   122,   124,   123,   113,    -1,    -1,    -1,    -1,    98,
      -1,    93,   114,    -1,    -1,   116,   116,   113,   113,   113,
      -1,   119,   119,   116,   116,   116,   116,   114,   114,   114,
     114,   114,   113,   121,   114,   114,   114,   114,   114,    98,
      98,   116,    98,    98,   116,    98,    98,    97,   116,    98,
      98,    98,   116,   116,   116,   116,    98,    97,    92,   119,
      -1,    -1,   116,    -1,   102,   116,   123,    -1,    -1,    -1,
     116,   116,   116,   116,    98,   116,    98,   114,   114,   114,
      98,   114,   114,   122,    98,   114,    98,    98,   116,   119,
     116,   122,    97,   116,    98,   116,   116,   116,   123,   116,
     116,   114,   102,    98,    98,   102,   116,   122,   116,    98,
      98,   114,    98,   122,    98,   122,   122,   119,   116,    98,
     116,   122,   114,   122,    98,   116,   116,    98,    98,    98,
     123,    98,    -1,   116,   116,   116,   116
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    66,    77,    78,    85,    98,
     126,   127,   128,   140,   142,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   116,
     100,   113,    98,    96,   117,   118,     0,   128,    98,    97,
      97,    98,   109,   113,   170,    97,    97,    97,    94,    95,
     141,   141,   141,   141,   141,   141,   141,   141,   141,    97,
     109,    97,    97,    97,   141,    97,   141,    97,    97,    97,
      97,    97,    97,    97,    97,   141,   141,   141,   170,   141,
     141,   141,   141,   141,   141,    97,    97,   141,    97,   116,
      98,   143,   170,    98,   154,    50,    51,    54,    55,    56,
      58,    59,    61,    63,    64,    65,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    79,    80,   130,   131,   116,
     116,   115,   170,   170,   108,   109,   110,   111,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,    97,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   119,   144,   145,   116,   102,   116,   118,   102,   116,
     113,   113,   113,   116,   113,   113,   116,   113,   113,   133,
     113,   113,   113,   113,   116,   116,   113,   113,   116,   116,
     113,   116,   120,    76,    49,   114,   170,   170,   170,   170,
     116,   141,    98,   135,    86,    87,    88,    89,    91,   146,
     147,   148,   149,   157,   158,    98,     4,    52,    53,    57,
      60,    68,    74,    75,    82,   129,    98,    98,    98,    98,
     100,    98,   100,    98,   113,    98,   100,    98,    98,    98,
      98,   100,   100,   113,   113,   114,   124,   124,   124,   124,
      98,    90,   155,   158,   116,   159,   116,   113,   113,   113,
     116,   116,   116,   116,   113,   116,   122,   114,   119,   114,
     114,   114,   119,   114,   113,   161,   162,   114,   114,   114,
     114,   119,   114,   114,   119,   121,    98,   139,   139,   116,
      98,   150,    98,   151,    98,   152,    98,   153,   124,    98,
     124,   156,   116,   159,   159,    93,   160,    98,    98,    97,
      98,   156,   116,    98,   116,   116,   116,   132,   116,   170,
     119,   116,   116,   116,   116,    98,   116,   116,    97,   116,
     114,   119,   114,   116,   119,   116,   119,   116,   119,   122,
     116,   119,    92,   102,   156,   116,   119,   159,   163,   164,
     114,   114,   114,   114,   123,   114,   122,   119,    98,   162,
     119,   114,   116,    98,   116,    98,    98,    98,   153,    98,
     163,   163,   116,    98,     5,    98,   134,   137,    94,    95,
     112,   113,   166,   168,   169,   170,   116,   116,   116,   116,
     116,   116,   139,   170,   114,    97,   116,   123,   122,    98,
     138,   102,   134,   137,   122,   153,   166,   166,   170,   101,
     102,   103,   104,   105,   106,   107,   123,   114,   119,   116,
     114,   153,   122,   163,   122,   153,   165,   167,   168,    98,
     134,   136,   114,   166,   166,   170,   170,   170,   170,   170,
     119,   170,   116,   123,   167,   165,   134,   136,   123,   102,
     122,    98,   104,   106,   104,   106,    98,   114,   123,   123,
     122,    98,    98,   137,   167,   165,   116,   170,   170,   170,
     170,   114,   137,   165,   116,   153,   123,   116,   153,   123,
      98,   136,   136,   136,   136,    98,    98,    98,    98,   116,
     116,   116,   116
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   125,   126,   127,   127,   128,   128,   128,   128,   128,
     128,   128,   128,   129,   128,   130,   128,   131,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   132,   128,   128,   128,   128,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   133,   128,
     128,   128,   128,   128,   134,   135,   136,   137,   138,   139,
     139,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   141,   141,   143,
     142,   144,   145,   145,   145,   145,   145,   146,   147,   148,
     149,   150,   150,   151,   151,   152,   152,   153,   153,   153,
     153,   154,   154,   155,   155,   156,   156,   157,   157,   157,
     157,   158,   159,   159,   160,   160,   160,   160,   160,   160,
     161,   161,   162,   162,   164,   163,   165,   166,   166,   166,
     166,   166,   167,   167,   168,   168,   168,   168,   168,   169,
     169,   169,   169,   169,   169,   169,   170,   170,   170,   170,
     170,   170,   170,   170,   170
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     3,     2,     4,
       4,     9,     7,     0,     9,     0,     8,     0,     8,     7,
       4,     4,     6,     6,     6,     7,     7,     7,     7,     4,
       9,     4,     7,     7,     0,    14,     4,     7,     9,     6,
       7,     7,     9,     9,     9,     4,    11,     7,     0,    10,
       6,     6,     7,     4,     1,     1,     1,     1,     1,     1,
       3,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     5,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     1,     1,     0,
       5,     3,     0,     2,     2,     2,     2,     4,     4,     4,
       4,     3,     1,     3,     1,     3,     1,     3,     1,     6,
       4,     3,     3,     4,     3,     3,     5,     3,     4,     2,
       3,     9,     2,     0,    11,    11,     7,    12,    12,     8,
       3,     1,     5,     7,     0,     2,     1,     3,     3,     2,
       3,     1,     3,     1,     1,     5,     5,     5,     5,     3,
       3,     3,     3,     3,     1,     1,     1,     2,     1,     3,
       3,     3,     3,     2,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 7: /* command: MY_ECHO STRING_TEXT ';'  */
#line 318 "../../../phaver/src/parser.yy"
    { cout << *((yyvsp[-1].mystring)) << endl; }
#line 2025 "parser.cc"
    break;

  case 8: /* command: WHO ';'  */
#line 320 "../../../phaver/src/parser.yy"
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
#line 2047 "parser.cc"
    break;

  case 9: /* command: IDENT '=' IDENT ';'  */
#line 342 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-3].mystring));
      const auto& id = *((yyvsp[-1].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        ss_map[dst] = ss_map[id];
      } else if (paut_map.find(id) != paut_map.end()) {
        paut = new automaton(*paut_map[id]);
        paut->name = dst;
        paut_map[dst].reset(paut);
      } else
        yyerror("Identifier " + id + " not found.");
      delete (yyvsp[-3].mystring);
      delete (yyvsp[-1].mystring);
    }
#line 2066 "parser.cc"
    break;

  case 10: /* command: IDENT '.' PRINT ';'  */
#line 357 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-3].mystring));
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
      delete (yyvsp[-3].mystring);
    }
#line 2093 "parser.cc"
    break;

  case 11: /* command: IDENT '.' PRINT '(' STRING_TEXT ',' INT ')' ';'  */
#line 380 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-8].mystring));
      const auto& fname = *((yyvsp[-4].mystring));
      int format = atoi_consume((yyvsp[-2].mystring));
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
      delete (yyvsp[-8].mystring);
      delete (yyvsp[-4].mystring);
    }
#line 2137 "parser.cc"
    break;

  case 12: /* command: IDENT '.' SAVE_CON_FP '[' STRING_TEXT ']' ';'  */
#line 420 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-6].mystring));
      const auto& fname = *((yyvsp[-2].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        file_out.open(fname);
        file_out.precision(FP_PRECISION);
        clock_val_set temp_cvs = ss_map[id].first.union_over_locations();
        temp_cvs.print_con_fp_raw(file_out);
        file_out.close();
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2156 "parser.cc"
    break;

  case 13: /* $@1: %empty  */
#line 438 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-1].mystring));
      if (paut_map.find(id) != paut_map.end())
        paut = paut_map[id].get();
      else
        yyerror("Automaton '" + id + "' not found.");
    }
#line 2168 "parser.cc"
    break;

  case 14: /* command: IDENT '=' IDENT '.' $@1 '{' state_list '}' ';'  */
#line 446 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-8].mystring));
      const auto& states = *((yyvsp[-2].symb_state));
      ss_map[dst] = make_pair(std::move(states), paut);
      delete (yyvsp[-8].mystring);
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].symb_state);
    }
#line 2181 "parser.cc"
    break;

  case 15: /* $@2: %empty  */
#line 455 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-1].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        psymb_state = &ss_map[id].first;
        paut = ss_map[id].second;
      } else {
        yyerror("Unknown state identifier '" + id +"'.");
        psymb_state = nullptr;
      }
    }
#line 2196 "parser.cc"
    break;

  case 16: /* command: IDENT '.' $@2 REMOVE '(' var_ref_list ')' ';'  */
#line 466 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-7].mystring));
      if ((yyvsp[-2].vrs) != nullptr) {
        if (ss_map.find(id) != ss_map.end())
          ss_map[id].first.remove_space_dimensions(*(yyvsp[-2].vrs));
        else
          yyerror("Unknown identifier '" + id +"'.");
      }
      psymb_state = nullptr;
      delete (yyvsp[-7].mystring);
      delete (yyvsp[-2].vrs);
    }
#line 2213 "parser.cc"
    break;

  case 17: /* $@3: %empty  */
#line 479 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-1].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        psymb_state = &ss_map[id].first;
        paut = ss_map[id].second;
      } else
        yyerror("Unknown symbolic state identifier '" + id +"'.");
      delete (yyvsp[-1].mystring);
    }
#line 2227 "parser.cc"
    break;

  case 18: /* command: IDENT '.' $@3 PROJECT_TO '(' var_ref_list ')' ';'  */
#line 489 "../../../phaver/src/parser.yy"
    {
      if ((yyvsp[-2].vrs) != nullptr) {
        assert(psymb_state != nullptr);
        psymb_state->project_to_vars(*(yyvsp[-2].vrs));
      }
      psymb_state = nullptr;
      paut = nullptr;
      delete (yyvsp[-2].vrs);
    }
#line 2241 "parser.cc"
    break;

  case 19: /* command: GET_PARAMETERS '(' IDENT ',' bool_type ')' ';'  */
#line 499 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-4].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        auto sstates = ss_map[id].first;
        const var_ref_set& vrs = ss_map[id].second->parameters;
        auto dim = ss_map[id].second->dim;
        sstates.remove_space_dimensions(vrs.range_complement(0, dim));
        clock_val_set tmp;
        if ((yyvsp[-2].mybool)) {
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
      delete (yyvsp[-4].mystring);
    }
#line 2267 "parser.cc"
    break;

  case 20: /* command: IDENT '.' LOC_UNION ';'  */
#line 521 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-3].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        const auto& mycvs = ss_map[id].first.union_over_locations();
        mycvs.print(ss_map[id].first.var_names);
        mycvs.print_gen_fp_raw(cout);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete (yyvsp[-3].mystring);
    }
#line 2282 "parser.cc"
    break;

  case 21: /* command: IDENT '.' LOC_INTERSECTION ';'  */
#line 532 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-3].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        const auto& mycvs = ss_map[id].first.intersection_over_locations();
        mycvs.print(ss_map[id].first.var_names);
        mycvs.print_gen_fp_raw(cout);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete (yyvsp[-3].mystring);
    }
#line 2297 "parser.cc"
    break;

  case 22: /* command: IDENT '=' IDENT '.' LOC_UNION ';'  */
#line 543 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-5].mystring));
      const auto& id = *((yyvsp[-3].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        auto mycvs = ss_map[id].first.union_over_locations();
        symb_states_type ss(ss_map[id].first.var_names);
        ss.add("$", std::move(mycvs));
        ss_map[dst] = make_pair(std::move(ss), ss_map[id].second);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete (yyvsp[-5].mystring);
      delete (yyvsp[-3].mystring);
    }
#line 2315 "parser.cc"
    break;

  case 23: /* command: IDENT '=' IDENT '.' LOC_INTERSECTION ';'  */
#line 557 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-5].mystring));
      const auto& id = *((yyvsp[-3].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        auto mycvs = ss_map[id].first.intersection_over_locations();
        symb_states_type ss(ss_map[id].first.var_names);
        ss.add("$", std::move(mycvs));
        ss_map[dst] = make_pair(std::move(ss), ss_map[id].second);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete (yyvsp[-5].mystring);
      delete (yyvsp[-3].mystring);
    }
#line 2333 "parser.cc"
    break;

  case 24: /* command: IDENT '=' IDENT '.' MERGE_SPLITTED ';'  */
#line 571 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-5].mystring));
      const auto& id = *((yyvsp[-3].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        auto sstates = ss_map[id].first.merge_splitted();
        ss_map[dst] = make_pair(std::move(sstates), nullptr);
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete (yyvsp[-5].mystring);
      delete (yyvsp[-3].mystring);
    }
#line 2349 "parser.cc"
    break;

  case 25: /* command: IDENT '.' IS_INTERSECTING '(' IDENT ')' ';'  */
#line 583 "../../../phaver/src/parser.yy"
    {
      const auto& id1 = *((yyvsp[-6].mystring));
      const auto& id2 = *((yyvsp[-2].mystring));
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
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2370 "parser.cc"
    break;

  case 26: /* command: IDENT '.' INTERSECTION_ASSIGN '(' IDENT ')' ';'  */
#line 600 "../../../phaver/src/parser.yy"
    {
      const auto& id1 = *((yyvsp[-6].mystring));
      const auto& id2 = *((yyvsp[-2].mystring));
      if (ss_map.find(id1) != ss_map.end()) {
        if (ss_map.find(id2) != ss_map.end())
          ss_map[id1].first.intersection_assign(ss_map[id2].first);
        else
          yyerror("Unknown identifier '" + id2 +"'.");
      } else
        yyerror("Unknown identifier '" + id1 +"'.");
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2388 "parser.cc"
    break;

  case 27: /* command: IDENT '.' DIFFERENCE_ASSIGN '(' IDENT ')' ';'  */
#line 614 "../../../phaver/src/parser.yy"
    {
      const auto& id1 = *((yyvsp[-6].mystring));
      const auto& id2 = *((yyvsp[-2].mystring));
      if (ss_map.find(id1) != ss_map.end()) {
        if (ss_map.find(id2) != ss_map.end())
          ss_map[id1].first.difference_assign(ss_map[id2].first);
        else
          yyerror("Unknown identifier '" + id2 +"'.");
      } else
        yyerror("Unknown identifier '" + id1 +"'.");
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2406 "parser.cc"
    break;

  case 28: /* command: IDENT '.' CONTAINS '(' IDENT ')' ';'  */
#line 628 "../../../phaver/src/parser.yy"
    {
      const auto& id1 = *((yyvsp[-6].mystring));
      const auto& id2 = *((yyvsp[-2].mystring));
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
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2427 "parser.cc"
    break;

  case 29: /* command: IDENT '.' IS_EMPTY ';'  */
#line 645 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-3].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        if (ss_map[id].first.is_empty())
          cout << "empty" << endl;
        else
          cout << "not empty" << endl;
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete (yyvsp[-3].mystring);
    }
#line 2443 "parser.cc"
    break;

  case 30: /* command: IDENT '.' RENAME '(' IDENT ',' IDENT ')' ';'  */
#line 657 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-8].mystring));
      const auto& name1 = *((yyvsp[-4].mystring));
      const auto& name2 = *((yyvsp[-2].mystring));
      if (ss_map.find(id) != ss_map.end()) {
        ss_map[id].first.rename_variable(name1, name2);
        // remove the reference to any automaton because now it's changed
        ss_map[id].second = nullptr;
      } else
        yyerror("Unknown identifier '" + id +"'.");
      delete (yyvsp[-8].mystring);
      delete (yyvsp[-4].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2462 "parser.cc"
    break;

  case 31: /* command: IDENT '=' compose_list ';'  */
#line 675 "../../../phaver/src/parser.yy"
    {
      const auto& comp_name = *((yyvsp[-3].mystring));
      const auto& aut_names = *((yyvsp[-1].ident_list));
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
      delete (yyvsp[-3].mystring);
      delete (yyvsp[-1].ident_list);
    }
#line 2493 "parser.cc"
    break;

  case 32: /* command: IDENT '.' SAVE_FP_INVARS '(' STRING_TEXT ')' ';'  */
#line 702 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-6].mystring));
      const auto& fname = *((yyvsp[-2].mystring));
      if (paut_map.find(id) != paut_map.end()) {
        file_out.open(fname);
        file_out.precision(FP_PRECISION);
        paut_map[id]->print_inv_fp_raw(file_out);
        file_out.close();
      } else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2511 "parser.cc"
    break;

  case 33: /* command: IDENT '.' SAVE_FP_SURFACE '(' STRING_TEXT ')' ';'  */
#line 716 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-6].mystring));
      const auto& fname = *((yyvsp[-2].mystring));
      if (paut_map.find(id) != paut_map.end()) {
        file_out.open(fname);
        file_out.precision(FP_PRECISION);
        paut_map[id]->print_surface_fp_raw(file_out);
        file_out.close();
      }	else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2529 "parser.cc"
    break;

  case 34: /* $@4: %empty  */
#line 730 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-5].mystring));
      psymb_state = nullptr;
      if (paut_map.find(id) != paut_map.end())
        paut = paut_map[id].get();
      else
        yyerror("Automaton '" + id + "' not found.");
    }
#line 2542 "parser.cc"
    break;

  case 35: /* command: IDENT '.' PRINT_GRAPH '(' STRING_TEXT ',' $@4 '{' var_ref_list '}' ',' IDENT ')' ';'  */
#line 739 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-13].mystring));
      const auto& fname = *((yyvsp[-9].mystring));
      const auto& var_list = *((yyvsp[-5].vrs));
      const auto& id2 = *((yyvsp[-2].mystring));
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
      delete (yyvsp[-13].mystring);
      delete (yyvsp[-9].mystring);
      delete (yyvsp[-5].vrs);
      delete (yyvsp[-2].mystring);
    }
#line 2569 "parser.cc"
    break;

  case 36: /* command: IDENT '.' REVERSE ';'  */
#line 762 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-3].mystring));
      if (paut_map.find(id) != paut_map.end())
        paut_map[id]->reverse();
      else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-3].mystring);
    }
#line 2582 "parser.cc"
    break;

  case 37: /* command: IDENT '.' ADD_LABEL '(' IDENT ')' ';'  */
#line 771 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-6].mystring));
      const auto& lab = *((yyvsp[-2].mystring));
      if (paut_map.find(id) != paut_map.end())
        paut_map[id]->add_label(lab);
      else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2597 "parser.cc"
    break;

  case 38: /* command: IDENT '=' IDENT '.' REACH_FORWARD_ITER '(' INT ')' ';'  */
#line 782 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-8].mystring));
      const auto& id = *((yyvsp[-6].mystring));
      int delta = atoi_consume((yyvsp[-2].mystring));
      if (paut_map.find(id) != paut_map.end())
        ss_map[dst] = make_pair(paut_map[id]->get_reach_set_forwarditer(delta),
                                paut_map[id].get());
      else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-8].mystring);
      delete (yyvsp[-6].mystring);
    }
#line 2614 "parser.cc"
    break;

  case 39: /* command: IDENT '=' IDENT '.' REACH ';'  */
#line 795 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-5].mystring));
      const auto& id = *((yyvsp[-3].mystring));
      if (paut_map.find(id) != paut_map.end())
        ss_map[dst] = make_pair(paut_map[id]->get_reach_set(),
                                paut_map[id].get());
      else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-5].mystring);
      delete (yyvsp[-3].mystring);
    }
#line 2630 "parser.cc"
    break;

  case 40: /* command: IDENT '.' INITIAL_STATES '(' IDENT ')' ';'  */
#line 807 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-6].mystring));
      const auto& id_init = *((yyvsp[-2].mystring));
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
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2653 "parser.cc"
    break;

  case 41: /* command: IDENT '.' INVARIANT_ASSIGN '(' IDENT ')' ';'  */
#line 826 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-6].mystring));
      const auto& id_inv = *((yyvsp[-2].mystring));
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
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2676 "parser.cc"
    break;

  case 42: /* command: IDENT '=' IDENT '.' REACH '(' IDENT ')' ';'  */
#line 845 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-8].mystring));
      const auto& id = *((yyvsp[-6].mystring));
      const auto& id_ini = *((yyvsp[-2].mystring));
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
      delete (yyvsp[-8].mystring);
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2706 "parser.cc"
    break;

  case 43: /* command: IDENT '=' IDENT '.' IS_REACHABLE '(' IDENT ')' ';'  */
#line 871 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-8].mystring));
      const auto& id = *((yyvsp[-6].mystring));
      const auto& target = *((yyvsp[-2].mystring));
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
      delete (yyvsp[-8].mystring);
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2737 "parser.cc"
    break;

  case 44: /* command: IDENT '=' IDENT '.' IS_REACHABLE_FB '(' IDENT ')' ';'  */
#line 898 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-8].mystring));
      const auto& id = *((yyvsp[-6].mystring));
      const auto& target = *((yyvsp[-2].mystring));
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
      delete (yyvsp[-8].mystring);
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2768 "parser.cc"
    break;

  case 45: /* command: IDENT '.' REFINE_LOC_DERIV ';'  */
#line 925 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-3].mystring));
      refine_loc_deriv(*paut_map[id]);
      delete (yyvsp[-3].mystring);
    }
#line 2778 "parser.cc"
    break;

  case 46: /* command: IDENT '.' REFINE_LOCS '(' IDENT ',' IDENT ',' INT ')' ';'  */
#line 931 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-10].mystring));
      const auto& label = *((yyvsp[-6].mystring));
      const auto& method = *((yyvsp[-4].mystring));
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
                  ref_meth, atoi_consume((yyvsp[-2].mystring)));
      delete (yyvsp[-10].mystring);
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-4].mystring);
    }
#line 2813 "parser.cc"
    break;

  case 47: /* command: IDENT '.' REFINE_LOCS '(' IDENT ')' ';'  */
#line 962 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-6].mystring));
      const auto& state = *((yyvsp[-2].mystring));
      paut = paut_map[id].get();
      refine_states(*paut, ss_map[state].first);
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2826 "parser.cc"
    break;

  case 48: /* $@5: %empty  */
#line 971 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-2].mystring));
      paut = paut_map[id].get();
    }
#line 2835 "parser.cc"
    break;

  case 49: /* command: IDENT '.' REFINE_CONSTRAINTS $@5 '(' refinement_cons ',' IDENT ')' ';'  */
#line 976 "../../../phaver/src/parser.yy"
    {
      const auto& label = *((yyvsp[-2].mystring));
      REFINEMENT_CONS = *((yyvsp[-4].refinement_cons));
      REFINEMENT_LABEL = paut->get_label_ref(label);
      delete (yyvsp[-9].mystring);
      delete (yyvsp[-4].refinement_cons);
      delete (yyvsp[-2].mystring);
    }
#line 2848 "parser.cc"
    break;

  case 50: /* command: IDENT '=' IDENT '.' INITIAL_STATES ';'  */
#line 985 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-5].mystring));
      const auto& id = *((yyvsp[-3].mystring));
      if (paut_map.find(id) != paut_map.end())
        ss_map[dst] = make_pair(paut_map[id]->get_ini_states(),
                                paut_map[id].get());
      else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-5].mystring);
      delete (yyvsp[-3].mystring);
    }
#line 2864 "parser.cc"
    break;

  case 51: /* command: IDENT '=' IDENT '.' GET_INVARIANTS ';'  */
#line 997 "../../../phaver/src/parser.yy"
    {
      const auto& dst = *((yyvsp[-5].mystring));
      const auto& id = *((yyvsp[-3].mystring));
      if (paut_map.find(id) != paut_map.end())
        ss_map[dst] = make_pair(paut_map[id]->get_invariants(),
                                paut_map[id].get());
      else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-5].mystring);
      delete (yyvsp[-3].mystring);
    }
#line 2880 "parser.cc"
    break;

  case 52: /* command: IDENT '.' UNLOCK_SURFACE_LOCS '(' IDENT ')' ';'  */
#line 1010 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-6].mystring));
      const auto& state = *((yyvsp[-2].mystring));
      if (paut_map.find(id) != paut_map.end()) {
        if (ss_map.find(state) != ss_map.end())
          paut_map[id]->unlock_surface_locations(ss_map[state].first);
        else
          yyerror("Unknown identifier '" + state +"'.");
      } else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-6].mystring);
      delete (yyvsp[-2].mystring);
    }
#line 2898 "parser.cc"
    break;

  case 53: /* command: IDENT '.' UNLOCK_LOCS ';'  */
#line 1024 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-3].mystring));
      if (paut_map.find(id) != paut_map.end())
        paut_map[id]->unlock_locations();
      else
        yyerror("Automaton '" + id + "' not found.");
      delete (yyvsp[-3].mystring);
    }
#line 2911 "parser.cc"
    break;

  case 54: /* DO: IDENT  */
#line 1040 "../../../phaver/src/parser.yy"
          { check_ident_consume((yyvsp[0].mystring), "do"); }
#line 2917 "parser.cc"
    break;

  case 55: /* END: IDENT  */
#line 1042 "../../../phaver/src/parser.yy"
          { check_ident_consume((yyvsp[0].mystring), "end"); }
#line 2923 "parser.cc"
    break;

  case 56: /* GOTO: IDENT  */
#line 1044 "../../../phaver/src/parser.yy"
          { check_ident_consume((yyvsp[0].mystring), "goto"); }
#line 2929 "parser.cc"
    break;

  case 57: /* SYNC: IDENT  */
#line 1046 "../../../phaver/src/parser.yy"
          { check_ident_consume((yyvsp[0].mystring), "sync"); }
#line 2935 "parser.cc"
    break;

  case 58: /* WAIT: IDENT  */
#line 1048 "../../../phaver/src/parser.yy"
          { check_ident_consume((yyvsp[0].mystring), "wait"); }
#line 2941 "parser.cc"
    break;

  case 59: /* var_ref_list: IDENT  */
#line 1052 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      if (psymb_state != nullptr) {
        auto pvrs = new var_ref_set;
        if (psymb_state->var_names.contains_name(id)) {
          pvrs->insert(psymb_state->var_names.get_id(id));
          (yyval.vrs) = pvrs;
        } else {
          yyerror("Unknown state variable '" + id +"'.");
          delete pvrs;
          (yyval.vrs) = nullptr;
        }
      } else if (paut != nullptr) {
        auto pvrs = new var_ref_set;
        if (paut->var_id_map.contains_name(id)) {
          pvrs->insert(paut->var_id_map.get_id(id));
          (yyval.vrs) = pvrs;
        } else {
          yyerror("Unknown automaton variable '" + id +"'.");
          delete pvrs;
          (yyval.vrs) = nullptr;
        }
      } else
        yyerror("Don't know which set of states '"
                + id +"' refers to.");
      delete (yyvsp[0].mystring);
    }
#line 2973 "parser.cc"
    break;

  case 60: /* var_ref_list: var_ref_list ',' IDENT  */
#line 1080 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      if (psymb_state != nullptr) {
        if (psymb_state->var_names.contains_name(id)) {
          (yyvsp[-2].vrs)->insert(psymb_state->var_names.get_id(id));
          (yyval.vrs) = (yyvsp[-2].vrs);
        } else
          yyerror("Unknown state variable '" + id +"'.");
      }	else if (paut != nullptr) {
        if (paut->var_id_map.contains_name(id)) {
          (yyvsp[-2].vrs)->insert(paut->var_id_map.get_id(id));
          (yyval.vrs) = (yyvsp[-2].vrs);
        } else
          yyerror("Unknown automaton variable '" + id +"'.");
      } else
        yyerror("Don't know which set of states '"
                + id +"' refers to.");
      delete (yyvsp[0].mystring);
    }
#line 2997 "parser.cc"
    break;

  case 61: /* prelim: par_POLY_KIND '=' IDENT ';'  */
#line 1103 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-1].mystring));
      if (!set_poly_kind(id, true))
        yyerror("Invalid poly kind name");
      delete (yyvsp[-1].mystring);
    }
#line 3008 "parser.cc"
    break;

  case 62: /* prelim: IDENT ASSIGN rat_aff_expr ';'  */
#line 1110 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-3].mystring));
      const auto& rae = *((yyvsp[-1].rae));
      rae_map[id] = rae;
      delete (yyvsp[-3].mystring);
      delete (yyvsp[-1].rae);
    }
#line 3020 "parser.cc"
    break;

  case 63: /* prelim: par_TIME_POST_ITER '=' INT ';'  */
#line 1119 "../../../phaver/src/parser.yy"
    { TIME_POST_ITER = atoi_consume((yyvsp[-1].mystring)); }
#line 3026 "parser.cc"
    break;

  case 64: /* prelim: par_TIME_POST_CONE_LAMBDA '=' rat_aff_expr ';'  */
#line 1121 "../../../phaver/src/parser.yy"
    {
      TIME_POST_CONE_LAMBDA= (yyvsp[-1].rae)->rat_inhomo_term();
      delete (yyvsp[-1].rae);
    }
#line 3035 "parser.cc"
    break;

  case 65: /* prelim: par_TIME_POST_CONE_WIDEN_DELAY '=' INT ';'  */
#line 1126 "../../../phaver/src/parser.yy"
    { TIME_POST_CONE_WIDEN_DELAY = atoi_consume((yyvsp[-1].mystring)); }
#line 3041 "parser.cc"
    break;

  case 66: /* prelim: par_TIME_POST_CONE_WIDEN_PRECISION '=' INT ';'  */
#line 1128 "../../../phaver/src/parser.yy"
    { TIME_POST_CONE_WIDEN_PRECISION = atoi_consume((yyvsp[-1].mystring)); }
#line 3047 "parser.cc"
    break;

  case 67: /* prelim: par_MEMORY_MODE '=' INT ';'  */
#line 1130 "../../../phaver/src/parser.yy"
    { MEMORY_MODE = atoi_consume((yyvsp[-1].mystring)); }
#line 3053 "parser.cc"
    break;

  case 68: /* prelim: par_PARSER_FIX_DPOST '=' bool_type ';'  */
#line 1133 "../../../phaver/src/parser.yy"
    { PARSER_FIX_DPOST=(yyvsp[-1].mybool); }
#line 3059 "parser.cc"
    break;

  case 69: /* prelim: par_REACH_CHEAP_CONTAINS '=' bool_type ';'  */
#line 1136 "../../../phaver/src/parser.yy"
    { REACH_CHEAP_CONTAINS=(yyvsp[-1].mybool); }
#line 3065 "parser.cc"
    break;

  case 70: /* prelim: par_REACH_CHEAP_CONTAINS_USE_BBOX '=' bool_type ';'  */
#line 1138 "../../../phaver/src/parser.yy"
    { REACH_CHEAP_CONTAINS_USE_BBOX=(yyvsp[-1].mybool); }
#line 3071 "parser.cc"
    break;

  case 71: /* prelim: par_MAINTAIN_BOXED_CCVS '=' bool_type ';'  */
#line 1140 "../../../phaver/src/parser.yy"
    { MAINTAIN_BOXED_CCVS=(yyvsp[-1].mybool); }
#line 3077 "parser.cc"
    break;

  case 72: /* prelim: par_REACH_USE_BBOX '=' bool_type ';'  */
#line 1142 "../../../phaver/src/parser.yy"
    { REACH_USE_BBOX=(yyvsp[-1].mybool); }
#line 3083 "parser.cc"
    break;

  case 73: /* prelim: par_REACH_USE_CONSTRAINT_HULL '=' bool_type ';'  */
#line 1144 "../../../phaver/src/parser.yy"
    { REACH_USE_CONSTRAINT_HULL=(yyvsp[-1].mybool); }
#line 3089 "parser.cc"
    break;

  case 74: /* prelim: par_REACH_USE_CONVEX_HULL '=' bool_type ';'  */
#line 1146 "../../../phaver/src/parser.yy"
    { REACH_USE_CONVEX_HULL=(yyvsp[-1].mybool); }
#line 3095 "parser.cc"
    break;

  case 75: /* prelim: par_REACH_USE_TIME_ELAPSE '=' bool_type ';'  */
#line 1148 "../../../phaver/src/parser.yy"
    { REACH_USE_TIME_ELAPSE = (yyvsp[-1].mybool); }
#line 3101 "parser.cc"
    break;

  case 76: /* prelim: par_REACH_STOP_AT_FORB '=' bool_type ';'  */
#line 1150 "../../../phaver/src/parser.yy"
    { REACH_STOP_AT_FORB=(yyvsp[-1].mybool); }
#line 3107 "parser.cc"
    break;

  case 77: /* prelim: par_REACH_MAX_ITER '=' INT ';'  */
#line 1152 "../../../phaver/src/parser.yy"
    { REACH_MAX_ITER = atoi_consume((yyvsp[-1].mystring)); }
#line 3113 "parser.cc"
    break;

  case 78: /* prelim: par_REACH_MAX_ITER '=' '-' INT ';'  */
#line 1154 "../../../phaver/src/parser.yy"
    { REACH_MAX_ITER = - atoi_consume((yyvsp[-1].mystring)); }
#line 3119 "parser.cc"
    break;

  case 79: /* prelim: par_REACH_REPORT_INTERVAL '=' INT ';'  */
#line 1156 "../../../phaver/src/parser.yy"
    { REACH_REPORT_INTERVAL = atoi_consume((yyvsp[-1].mystring)); }
#line 3125 "parser.cc"
    break;

  case 80: /* prelim: par_REACH_USE_BBOX_ITER '=' INT ';'  */
#line 1158 "../../../phaver/src/parser.yy"
    { REACH_USE_BBOX_ITER = atoi_consume((yyvsp[-1].mystring)); }
#line 3131 "parser.cc"
    break;

  case 81: /* prelim: par_REACH_STOP_USE_CONVEX_HULL_ITER '=' INT ';'  */
#line 1160 "../../../phaver/src/parser.yy"
    { REACH_STOP_USE_CONVEX_HULL_ITER = atoi_consume((yyvsp[-1].mystring)); }
#line 3137 "parser.cc"
    break;

  case 82: /* prelim: par_REACH_STOP_USE_CONVEX_HULL_SETTLE '=' bool_type ';'  */
#line 1162 "../../../phaver/src/parser.yy"
    { REACH_STOP_USE_CONVEX_HULL_SETTLE=(yyvsp[-1].mybool); }
#line 3143 "parser.cc"
    break;

  case 83: /* prelim: par_LIMIT_CONSTRAINTS_METHOD '=' bool_type ';'  */
#line 1164 "../../../phaver/src/parser.yy"
    { LIMIT_CONSTRAINTS_METHOD=(yyvsp[-1].mybool); }
#line 3149 "parser.cc"
    break;

  case 84: /* prelim: par_REFINE_DERIVATIVE_METHOD '=' INT ';'  */
#line 1166 "../../../phaver/src/parser.yy"
    { REFINE_DERIVATIVE_METHOD = atoi_consume((yyvsp[-1].mystring)); }
#line 3155 "parser.cc"
    break;

  case 85: /* prelim: par_REFINE_PRIORITIZE_REACH_SPLIT '=' bool_type ';'  */
#line 1168 "../../../phaver/src/parser.yy"
    { REFINE_PRIORITIZE_REACH_SPLIT=(yyvsp[-1].mybool); }
#line 3161 "parser.cc"
    break;

  case 86: /* prelim: par_REFINE_SMALLEST_FIRST '=' bool_type ';'  */
#line 1170 "../../../phaver/src/parser.yy"
    { REFINE_SMALLEST_FIRST=(yyvsp[-1].mybool); }
#line 3167 "parser.cc"
    break;

  case 87: /* prelim: par_REFINE_USE_FP '=' bool_type ';'  */
#line 1172 "../../../phaver/src/parser.yy"
    { REFINE_USE_FP=(yyvsp[-1].mybool); }
#line 3173 "parser.cc"
    break;

  case 88: /* prelim: par_REFINE_DERIV_MINANGLE '=' rat_aff_expr ';'  */
#line 1174 "../../../phaver/src/parser.yy"
    {
      mpq_class q = (yyvsp[-1].rae)->rat_inhomo_term();
      REFINE_DERIV_MINANGLE = q.get_d();
      delete (yyvsp[-1].rae);
    }
#line 3183 "parser.cc"
    break;

  case 89: /* prelim: par_REFINE_PRIORITIZE_ANGLE '=' bool_type ';'  */
#line 1180 "../../../phaver/src/parser.yy"
    { REFINE_PRIORITIZE_ANGLE=(yyvsp[-1].mybool); }
#line 3189 "parser.cc"
    break;

  case 90: /* prelim: par_REACH_CONSTRAINT_LIMIT '=' INT ';'  */
#line 1182 "../../../phaver/src/parser.yy"
    { REACH_CONSTRAINT_LIMIT = atoi_consume((yyvsp[-1].mystring)); }
#line 3195 "parser.cc"
    break;

  case 91: /* prelim: par_TP_CONSTRAINT_LIMIT '=' INT ';'  */
#line 1184 "../../../phaver/src/parser.yy"
    { TP_CONSTRAINT_LIMIT = atoi_consume((yyvsp[-1].mystring)); }
#line 3201 "parser.cc"
    break;

  case 92: /* prelim: par_REACH_CONSTRAINT_TRIGGER '=' INT ';'  */
#line 1186 "../../../phaver/src/parser.yy"
    { REACH_CONSTRAINT_TRIGGER = atoi_consume((yyvsp[-1].mystring)); }
#line 3207 "parser.cc"
    break;

  case 93: /* prelim: par_REACH_BITSIZE_TRIGGER '=' INT ';'  */
#line 1188 "../../../phaver/src/parser.yy"
    { REACH_BITSIZE_TRIGGER = atoi_consume((yyvsp[-1].mystring)); }
#line 3213 "parser.cc"
    break;

  case 94: /* prelim: par_CONSTRAINT_BITSIZE '=' INT ';'  */
#line 1190 "../../../phaver/src/parser.yy"
    { CONSTRAINT_BITSIZE = atoi_consume((yyvsp[-1].mystring)); }
#line 3219 "parser.cc"
    break;

  case 95: /* prelim: par_SEARCH_METHOD '=' INT ';'  */
#line 1192 "../../../phaver/src/parser.yy"
    {
      int sm = atoi_consume((yyvsp[-1].mystring));
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
#line 3241 "parser.cc"
    break;

  case 96: /* prelim: par_SEARCH_METHOD_TOPSORT_TOKENS '=' INT ';'  */
#line 1210 "../../../phaver/src/parser.yy"
    { SEARCH_METHOD_TOPSORT_TOKENS = atoi_consume((yyvsp[-1].mystring)); }
#line 3247 "parser.cc"
    break;

  case 97: /* prelim: par_SNAPSHOT_INTERVAL '=' INT ';'  */
#line 1212 "../../../phaver/src/parser.yy"
    { SNAPSHOT_INTERVAL = atoi_consume((yyvsp[-1].mystring)); }
#line 3253 "parser.cc"
    break;

  case 98: /* prelim: par_REFINE_CHECK_TIME_RELEVANCE '=' bool_type ';'  */
#line 1215 "../../../phaver/src/parser.yy"
    { REFINE_CHECK_TIME_RELEVANCE=(yyvsp[-1].mybool); }
#line 3259 "parser.cc"
    break;

  case 99: /* prelim: par_REFINE_CHECK_TIME_RELEVANCE_DURING '=' bool_type ';'  */
#line 1217 "../../../phaver/src/parser.yy"
    { REFINE_CHECK_TIME_RELEVANCE_DURING=(yyvsp[-1].mybool); }
#line 3265 "parser.cc"
    break;

  case 100: /* prelim: par_REFINE_CHECK_TIME_RELEVANCE_FINAL '=' bool_type ';'  */
#line 1219 "../../../phaver/src/parser.yy"
    { REFINE_CHECK_TIME_RELEVANCE_FINAL=(yyvsp[-1].mybool); }
#line 3271 "parser.cc"
    break;

  case 101: /* prelim: par_REFINE_CHECK_TRANS_DIMS '=' bool_type ';'  */
#line 1221 "../../../phaver/src/parser.yy"
    { REFINE_CHECK_TRANS_DIMS=(yyvsp[-1].mybool); }
#line 3277 "parser.cc"
    break;

  case 102: /* prelim: par_REFINE_PARTITION_INSIDE '=' bool_type ';'  */
#line 1223 "../../../phaver/src/parser.yy"
    { REFINE_PARTITION_INSIDE=(yyvsp[-1].mybool); }
#line 3283 "parser.cc"
    break;

  case 103: /* prelim: par_REACH_FB_REFINE_METHOD '=' INT ';'  */
#line 1225 "../../../phaver/src/parser.yy"
    { REACH_FB_REFINE_METHOD = atoi_consume((yyvsp[-1].mystring)); }
#line 3289 "parser.cc"
    break;

  case 104: /* prelim: par_REFINE_MAX_CHECKS '=' INT ';'  */
#line 1227 "../../../phaver/src/parser.yy"
    { REFINE_MAX_CHECKS = atoi_consume((yyvsp[-1].mystring)); }
#line 3295 "parser.cc"
    break;

  case 105: /* prelim: par_REFINE_USE_NEW_SPLIT '=' bool_type ';'  */
#line 1229 "../../../phaver/src/parser.yy"
    { REFINE_USE_NEW_SPLIT = (yyvsp[-1].mybool); }
#line 3301 "parser.cc"
    break;

  case 106: /* prelim: par_MINIMIZE_FILTER_THRESHOLD '=' INT ';'  */
#line 1231 "../../../phaver/src/parser.yy"
    { pplite::Poly::set_minimize_filter_threshold(atoi_consume((yyvsp[-1].mystring))); }
#line 3307 "parser.cc"
    break;

  case 107: /* bool_type: TRUE  */
#line 1235 "../../../phaver/src/parser.yy"
         { (yyval.mybool) = true; }
#line 3313 "parser.cc"
    break;

  case 108: /* bool_type: FALSE  */
#line 1236 "../../../phaver/src/parser.yy"
          { (yyval.mybool) = false; }
#line 3319 "parser.cc"
    break;

  case 109: /* $@6: %empty  */
#line 1241 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      if (paut_map.find(id) == paut_map.end()) {
        paut = new automaton(id);
        paut_map[id].reset(paut);
      } else
        yyerror("automaton " + id + " already defined");
      delete (yyvsp[0].mystring);
    }
#line 3333 "parser.cc"
    break;

  case 110: /* automaton: AUTOMATON IDENT $@6 automaton_body END  */
#line 1251 "../../../phaver/src/parser.yy"
    {
      message(32200, "Parsed automaton: "
              + int2string(paut->locations.size()) + " locs, "
              + int2string(paut->transitions.size()) + " trans.");
    }
#line 3343 "parser.cc"
    break;

  case 117: /* synclab: SYNCLABS ':' ident_list ';'  */
#line 1272 "../../../phaver/src/parser.yy"
    {
      const auto& id_list = *((yyvsp[-1].ident_list));
      if (paut != nullptr) {
        for (const auto& lab : id_list)
          paut->add_label(lab);
      }
      else
        yyerror("synclabs: paut not defined");
      delete (yyvsp[-1].ident_list);
    }
#line 3358 "parser.cc"
    break;

  case 121: /* ivar_list: ivar_list ',' IDENT  */
#line 1298 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      paut->add_variable(id);
      delete (yyvsp[0].mystring);
    }
#line 3368 "parser.cc"
    break;

  case 122: /* ivar_list: IDENT  */
#line 1304 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      paut->add_variable(id);
      delete (yyvsp[0].mystring);
    }
#line 3378 "parser.cc"
    break;

  case 123: /* evar_list: evar_list ',' IDENT  */
#line 1313 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      paut->add_ext_variable(id);
      delete (yyvsp[0].mystring);
    }
#line 3388 "parser.cc"
    break;

  case 124: /* evar_list: IDENT  */
#line 1319 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      paut->add_ext_variable(id);
      delete (yyvsp[0].mystring);
    }
#line 3398 "parser.cc"
    break;

  case 125: /* param_list: param_list ',' IDENT  */
#line 1328 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      paut->add_parameter(id);
      delete (yyvsp[0].mystring);
    }
#line 3408 "parser.cc"
    break;

  case 126: /* param_list: IDENT  */
#line 1334 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      paut->add_parameter(id);
      delete (yyvsp[0].mystring);
    }
#line 3418 "parser.cc"
    break;

  case 127: /* ident_list: ident_list ',' IDENT  */
#line 1343 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      (yyval.ident_list)->push_back(id);
      delete (yyvsp[0].mystring);
    }
#line 3428 "parser.cc"
    break;

  case 128: /* ident_list: IDENT  */
#line 1349 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      (yyval.ident_list) = new std::list<string>;
      (yyval.ident_list)->push_back(id);
      delete (yyvsp[0].mystring);
    }
#line 3439 "parser.cc"
    break;

  case 129: /* ident_list: ident_list ',' IDENT '{' ident_list '}'  */
#line 1356 "../../../phaver/src/parser.yy"
    {
      const auto& prefix = *((yyvsp[-3].mystring));
      auto& id_list = *((yyvsp[-1].ident_list));
      for (auto& s : id_list)
        s.insert(0, prefix);
      (yyval.ident_list)->splice((yyval.ident_list)->end(), id_list);
      delete (yyvsp[-3].mystring);
      delete (yyvsp[-1].ident_list);
    }
#line 3453 "parser.cc"
    break;

  case 130: /* ident_list: IDENT '{' ident_list '}'  */
#line 1366 "../../../phaver/src/parser.yy"
    {
      const auto& prefix = *((yyvsp[-3].mystring));
      auto& id_list = *((yyvsp[-1].ident_list));
      for (auto& s : id_list)
        s.insert(0, prefix);
      (yyval.ident_list) = (yyvsp[-1].ident_list);
      delete (yyvsp[-3].mystring);
    }
#line 3466 "parser.cc"
    break;

  case 131: /* compose_list: compose_list '&' IDENT  */
#line 1378 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      (yyval.ident_list)->push_back(id);
      delete (yyvsp[0].mystring);
    }
#line 3476 "parser.cc"
    break;

  case 132: /* compose_list: IDENT '&' IDENT  */
#line 1384 "../../../phaver/src/parser.yy"
    {
      const auto& id1 = *((yyvsp[-2].mystring));
      const auto& id2 = *((yyvsp[0].mystring));
      (yyval.ident_list) = new std::list<string>;
      (yyval.ident_list)->push_back(id1);
      (yyval.ident_list)->push_back(id2);
      delete (yyvsp[-2].mystring);
      delete (yyvsp[0].mystring);
    }
#line 3490 "parser.cc"
    break;

  case 133: /* initial: INITIALLY ':' state_list ';'  */
#line 1397 "../../../phaver/src/parser.yy"
    {
      paut->ini_states_assign(*(yyvsp[-1].symb_state));
      delete (yyvsp[-1].symb_state);
    }
#line 3499 "parser.cc"
    break;

  case 134: /* initial: INITIALLY state_list ';'  */
#line 1402 "../../../phaver/src/parser.yy"
    {
      paut->ini_states_assign(*(yyvsp[-1].symb_state));
      delete (yyvsp[-1].symb_state);
    }
#line 3508 "parser.cc"
    break;

  case 135: /* state_list: IDENT '&' state_val_set  */
#line 1410 "../../../phaver/src/parser.yy"
    {
      const auto& loc = *((yyvsp[-2].mystring));
      auto& state = *((yyvsp[0].cvs));
      if (paut != nullptr) {
        psymb_state = new symb_states_type(paut->get_var_names());
        psymb_state->add(loc, std::move(state));
        (yyval.symb_state) = psymb_state;
      }	else
        yyerror("Don't know which automaton to attribute states to.");
      delete (yyvsp[-2].mystring);
      delete (yyvsp[0].cvs);
    }
#line 3525 "parser.cc"
    break;

  case 136: /* state_list: state_list ',' IDENT '&' state_val_set  */
#line 1423 "../../../phaver/src/parser.yy"
    {
      const auto& loc = *((yyvsp[-2].mystring));
      auto& state = *((yyvsp[0].cvs));
      if (paut != nullptr) {
        psymb_state->add(loc, std::move(state));
        (yyval.symb_state) = psymb_state;
      }
      else
        yyerror("Don't know which automaton to attribute states to.");
      delete (yyvsp[-2].mystring);
      delete (yyvsp[0].cvs);
    }
#line 3542 "parser.cc"
    break;

  case 141: /* location: LOC IDENT ':' WHILE state_val_set WAIT '{' constr_list '}'  */
#line 1446 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-7].mystring));
      const auto& inv = *((yyvsp[-4].cvs));
      auto& cpost = *((yyvsp[-1].con_list));
      swap_space_dims(cpost, 0, paut->dim, paut->dim);
      paut->add_location(std::move(inv), id, std::move(cpost));
      // will serve as the source location for the coming transitions
      current_loc_name = id;
      delete (yyvsp[-7].mystring);
      delete (yyvsp[-4].cvs);
      delete (yyvsp[-1].con_list);
    }
#line 3559 "parser.cc"
    break;

  case 144: /* transition: WHEN state_val_set SYNC ident_list DO '{' dpost_cons '}' GOTO IDENT ';'  */
#line 1470 "../../../phaver/src/parser.yy"
    {
      const auto& guard = *((yyvsp[-9].cvs));
      const auto& id_list = *((yyvsp[-7].ident_list));
      auto& dpost = *((yyvsp[-4].con_list));
      const auto& target = *((yyvsp[-1].mystring));
      maybe_fix_dpost(paut->dim, dpost, paut->variables);
      check_dpost(paut->dim, dpost, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost);
      delete (yyvsp[-9].cvs);
      delete (yyvsp[-7].ident_list);
      delete (yyvsp[-4].con_list);
      delete (yyvsp[-1].mystring);
    }
#line 3579 "parser.cc"
    break;

  case 145: /* transition: WHEN state_val_set DO '{' dpost_cons '}' SYNC ident_list GOTO IDENT ';'  */
#line 1486 "../../../phaver/src/parser.yy"
    {
      const auto& guard = *((yyvsp[-9].cvs));
      auto& dpost = *((yyvsp[-6].con_list));
      const auto& id_list = *((yyvsp[-3].ident_list));
      const auto& target = *((yyvsp[-1].mystring));
      maybe_fix_dpost(paut->dim, dpost, paut->variables);
      check_dpost(paut->dim, dpost, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost);
      delete (yyvsp[-9].cvs);
      delete (yyvsp[-6].con_list);
      delete (yyvsp[-3].ident_list);
      delete (yyvsp[-1].mystring);
    }
#line 3599 "parser.cc"
    break;

  case 146: /* transition: WHEN state_val_set SYNC ident_list GOTO IDENT ';'  */
#line 1502 "../../../phaver/src/parser.yy"
    {
      const auto& guard = *((yyvsp[-5].cvs));
      const auto& id_list = *((yyvsp[-3].ident_list));
      const auto& target = *((yyvsp[-1].mystring));
      // controlled variables should remain constant
      Cons dpost = identity_dpost(paut->dim, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost);
      delete (yyvsp[-5].cvs);
      delete (yyvsp[-3].ident_list);
      delete (yyvsp[-1].mystring);
    }
#line 3617 "parser.cc"
    break;

  case 147: /* transition: WHEN state_val_set ASAP SYNC ident_list DO '{' dpost_cons '}' GOTO IDENT ';'  */
#line 1517 "../../../phaver/src/parser.yy"
    {
      const auto& guard = *((yyvsp[-10].cvs));
      const auto& id_list = *((yyvsp[-7].ident_list));
      auto& dpost = *((yyvsp[-4].con_list));
      const auto& target = *((yyvsp[-1].mystring));
      maybe_fix_dpost(paut->dim, dpost, paut->variables);
      check_dpost(paut->dim, dpost, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost, Urgency::urgent);
      delete (yyvsp[-10].cvs);
      delete (yyvsp[-7].ident_list);
      delete (yyvsp[-4].con_list);
      delete (yyvsp[-1].mystring);
    }
#line 3637 "parser.cc"
    break;

  case 148: /* transition: WHEN state_val_set ASAP DO '{' dpost_cons '}' SYNC ident_list GOTO IDENT ';'  */
#line 1534 "../../../phaver/src/parser.yy"
    {
      const auto& guard = *((yyvsp[-10].cvs));
      auto& dpost = *((yyvsp[-6].con_list));
      const auto& id_list = *((yyvsp[-3].ident_list));
      const auto& target = *((yyvsp[-1].mystring));
      maybe_fix_dpost(paut->dim, dpost, paut->variables);
      check_dpost(paut->dim, dpost, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost, Urgency::urgent);
      delete (yyvsp[-10].cvs);
      delete (yyvsp[-6].con_list);
      delete (yyvsp[-3].ident_list);
      delete (yyvsp[-1].mystring);
    }
#line 3657 "parser.cc"
    break;

  case 149: /* transition: WHEN state_val_set ASAP SYNC ident_list GOTO IDENT ';'  */
#line 1550 "../../../phaver/src/parser.yy"
    {
      const auto& guard = *((yyvsp[-6].cvs));
      const auto& id_list = *((yyvsp[-3].ident_list));
      const auto& target = *((yyvsp[-1].mystring));
      // controlled variables should remain constant
      Cons dpost = identity_dpost(paut->dim, paut->variables);
      for (const auto& id : id_list)
        paut->add_transition(current_loc_name, id, target,
                             guard, dpost, Urgency::urgent);
      delete (yyvsp[-6].cvs);
      delete (yyvsp[-3].ident_list);
      delete (yyvsp[-1].mystring);
    }
#line 3675 "parser.cc"
    break;

  case 150: /* refinement_cons: refinement_cons ',' refinement_con  */
#line 1567 "../../../phaver/src/parser.yy"
    {
      (yyvsp[-2].refinement_cons)->push_back(*(yyvsp[0].refinement_con));
      (yyval.refinement_cons) = (yyvsp[-2].refinement_cons);
      delete (yyvsp[0].refinement_con);
    }
#line 3685 "parser.cc"
    break;

  case 151: /* refinement_cons: refinement_con  */
#line 1573 "../../../phaver/src/parser.yy"
    {
      (yyval.refinement_cons) = new RefinementCons;
      (yyval.refinement_cons)->push_back(*(yyvsp[0].refinement_con));
      delete (yyvsp[0].refinement_con);
    }
#line 3695 "parser.cc"
    break;

  case 152: /* refinement_con: '(' rat_aff_expr ',' rat_aff_expr ')'  */
#line 1582 "../../../phaver/src/parser.yy"
    {
      (yyval.refinement_con) = new RefinementCon;
      (yyval.refinement_con)->con = Con((yyvsp[-3].rae)->aexpr.expr, (yyvsp[-3].rae)->aexpr.inhomo,
                    Con::NONSTRICT_INEQUALITY);
      (yyval.refinement_con)->min_d = (yyvsp[-1].rae)->rat_inhomo_term() * Rational((yyvsp[-3].rae)->den);
      (yyval.refinement_con)->max_d = Rational(0);
      delete (yyvsp[-3].rae);
      delete (yyvsp[-1].rae);
    }
#line 3709 "parser.cc"
    break;

  case 153: /* refinement_con: '(' rat_aff_expr ',' rat_aff_expr ',' rat_aff_expr ')'  */
#line 1592 "../../../phaver/src/parser.yy"
    {
      (yyval.refinement_con) = new RefinementCon;
      (yyval.refinement_con)->con = Con((yyvsp[-5].rae)->aexpr.expr, (yyvsp[-5].rae)->aexpr.inhomo,
                   Con::NONSTRICT_INEQUALITY);
      (yyval.refinement_con)->min_d = (yyvsp[-3].rae)->rat_inhomo_term() * Rational((yyvsp[-5].rae)->den);
      (yyval.refinement_con)->max_d = (yyvsp[-1].rae)->rat_inhomo_term() * Rational((yyvsp[-5].rae)->den);
      delete (yyvsp[-5].rae);
      delete (yyvsp[-3].rae);
      delete (yyvsp[-1].rae);
    }
#line 3724 "parser.cc"
    break;

  case 154: /* $@7: %empty  */
#line 1605 "../../../phaver/src/parser.yy"
    {
      if (paut == nullptr)
        yyerror("paut not defined");
      else
        val_set_dim = paut->dim;
    }
#line 3735 "parser.cc"
    break;

  case 155: /* state_val_set: $@7 val_set  */
#line 1612 "../../../phaver/src/parser.yy"
    { (yyval.cvs) = (yyvsp[0].cvs); }
#line 3741 "parser.cc"
    break;

  case 157: /* val_set: val_set '|' val_set  */
#line 1619 "../../../phaver/src/parser.yy"
    {
      (yyvsp[-2].cvs)->union_assign(*((yyvsp[0].cvs)));
      (yyval.cvs) = (yyvsp[-2].cvs);
      delete (yyvsp[0].cvs);
    }
#line 3751 "parser.cc"
    break;

  case 158: /* val_set: val_set '&' val_set  */
#line 1625 "../../../phaver/src/parser.yy"
    {
      (yyvsp[-2].cvs)->intersection_assign(*((yyvsp[0].cvs)));
      (yyval.cvs) = (yyvsp[-2].cvs);
      delete (yyvsp[0].cvs);
    }
#line 3761 "parser.cc"
    break;

  case 159: /* val_set: '!' val_set  */
#line 1631 "../../../phaver/src/parser.yy"
    {
      (yyvsp[0].cvs)->negate();
      (yyval.cvs) = (yyvsp[0].cvs);
    }
#line 3770 "parser.cc"
    break;

  case 160: /* val_set: '(' val_set ')'  */
#line 1636 "../../../phaver/src/parser.yy"
    {
      (yyval.cvs) = (yyvsp[-1].cvs);
    }
#line 3778 "parser.cc"
    break;

  case 161: /* val_set: constr_list_no_and  */
#line 1640 "../../../phaver/src/parser.yy"
    {
      const auto& cs = *((yyvsp[0].con_list));
      auto ccvs = convex_clock_val_set(val_set_dim, std::move(cs));
      (yyval.cvs) = new clock_val_set(std::move(ccvs));
      delete (yyvsp[0].con_list);
    }
#line 3789 "parser.cc"
    break;

  case 162: /* constr_list: constr_list '&' constr_list  */
#line 1651 "../../../phaver/src/parser.yy"
    {
      (yyvsp[-2].con_list)->insert((yyvsp[-2].con_list)->end(),
                 std::make_move_iterator((yyvsp[0].con_list)->begin()),
                 std::make_move_iterator((yyvsp[0].con_list)->end()));
      (yyval.con_list) = (yyvsp[-2].con_list);
      delete (yyvsp[0].con_list);
    }
#line 3801 "parser.cc"
    break;

  case 164: /* constr_list_no_and: constr  */
#line 1662 "../../../phaver/src/parser.yy"
     {
       (yyval.con_list) = new Cons;
       (yyval.con_list)->push_back(*(yyvsp[0].con));
       delete (yyvsp[0].con);
     }
#line 3811 "parser.cc"
    break;

  case 165: /* constr_list_no_and: rat_aff_expr LE rat_aff_expr LE rat_aff_expr  */
#line 1668 "../../../phaver/src/parser.yy"
     {
       (yyval.con_list) = new Cons;
       Affine_Expr ae = ((yyvsp[-4].rae)->den)*((yyvsp[-2].rae)->aexpr) - ((yyvsp[-2].rae)->den)*((yyvsp[-4].rae)->aexpr);
       (yyval.con_list)->push_back(Con(ae.expr, ae.inhomo, Con::NONSTRICT_INEQUALITY));
       ae = ((yyvsp[-2].rae)->den)*((yyvsp[0].rae)->aexpr) - ((yyvsp[0].rae)->den)*((yyvsp[-2].rae)->aexpr);
       (yyval.con_list)->push_back(Con(ae.expr, ae.inhomo, Con::NONSTRICT_INEQUALITY));
       delete (yyvsp[-4].rae);
       delete (yyvsp[-2].rae);
       delete (yyvsp[0].rae);
     }
#line 3826 "parser.cc"
    break;

  case 166: /* constr_list_no_and: rat_aff_expr '<' rat_aff_expr LE rat_aff_expr  */
#line 1679 "../../../phaver/src/parser.yy"
     {
       (yyval.con_list) = new Cons;
       Affine_Expr ae = ((yyvsp[-4].rae)->den)*((yyvsp[-2].rae)->aexpr) - ((yyvsp[-2].rae)->den)*((yyvsp[-4].rae)->aexpr);
       (yyval.con_list)->push_back(Con(ae.expr, ae.inhomo, Con::STRICT_INEQUALITY));
       ae = ((yyvsp[-2].rae)->den)*((yyvsp[0].rae)->aexpr) - ((yyvsp[0].rae)->den)*((yyvsp[-2].rae)->aexpr);
       (yyval.con_list)->push_back(Con(ae.expr, ae.inhomo, Con::NONSTRICT_INEQUALITY));
       delete (yyvsp[-4].rae);
       delete (yyvsp[-2].rae);
       delete (yyvsp[0].rae);
     }
#line 3841 "parser.cc"
    break;

  case 167: /* constr_list_no_and: rat_aff_expr LE rat_aff_expr '<' rat_aff_expr  */
#line 1690 "../../../phaver/src/parser.yy"
     {
       (yyval.con_list) = new Cons;
       Affine_Expr ae = ((yyvsp[-4].rae)->den)*((yyvsp[-2].rae)->aexpr) - ((yyvsp[-2].rae)->den)*((yyvsp[-4].rae)->aexpr);
       (yyval.con_list)->push_back(Con(ae.expr, ae.inhomo, Con::NONSTRICT_INEQUALITY));
       ae = ((yyvsp[-2].rae)->den)*((yyvsp[0].rae)->aexpr) - ((yyvsp[0].rae)->den)*((yyvsp[-2].rae)->aexpr);
       (yyval.con_list)->push_back(Con(ae.expr, ae.inhomo, Con::STRICT_INEQUALITY));
       delete (yyvsp[-4].rae);
       delete (yyvsp[-2].rae);
       delete (yyvsp[0].rae);
     }
#line 3856 "parser.cc"
    break;

  case 168: /* constr_list_no_and: rat_aff_expr '<' rat_aff_expr '<' rat_aff_expr  */
#line 1701 "../../../phaver/src/parser.yy"
     {
       (yyval.con_list) = new Cons;
       Affine_Expr ae = ((yyvsp[-4].rae)->den)*((yyvsp[-2].rae)->aexpr) - ((yyvsp[-2].rae)->den)*((yyvsp[-4].rae)->aexpr);
       (yyval.con_list)->push_back(Con(ae.expr, ae.inhomo, Con::STRICT_INEQUALITY));
       ae = ((yyvsp[-2].rae)->den)*((yyvsp[0].rae)->aexpr) - ((yyvsp[0].rae)->den)*((yyvsp[-2].rae)->aexpr);
       (yyval.con_list)->push_back(Con(ae.expr, ae.inhomo, Con::STRICT_INEQUALITY));
       delete (yyvsp[-4].rae);
       delete (yyvsp[-2].rae);
       delete (yyvsp[0].rae);
     }
#line 3871 "parser.cc"
    break;

  case 169: /* constr: rat_aff_expr '<' rat_aff_expr  */
#line 1716 "../../../phaver/src/parser.yy"
    {
      Affine_Expr ae = ((yyvsp[-2].rae)->den)*((yyvsp[0].rae)->aexpr) - ((yyvsp[0].rae)->den)*((yyvsp[-2].rae)->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::STRICT_INEQUALITY);
      (yyval.con) = pcon;
      delete (yyvsp[-2].rae);
      delete (yyvsp[0].rae);
    }
#line 3884 "parser.cc"
    break;

  case 170: /* constr: rat_aff_expr '>' rat_aff_expr  */
#line 1725 "../../../phaver/src/parser.yy"
    {
      Affine_Expr ae = ((yyvsp[0].rae)->den)*((yyvsp[-2].rae)->aexpr) - ((yyvsp[-2].rae)->den)*((yyvsp[0].rae)->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::STRICT_INEQUALITY);
      (yyval.con) = pcon;
      delete (yyvsp[-2].rae);
      delete (yyvsp[0].rae);
    }
#line 3897 "parser.cc"
    break;

  case 171: /* constr: rat_aff_expr GE rat_aff_expr  */
#line 1734 "../../../phaver/src/parser.yy"
    {
      Affine_Expr ae = ((yyvsp[0].rae)->den)*((yyvsp[-2].rae)->aexpr) - ((yyvsp[-2].rae)->den)*((yyvsp[0].rae)->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::NONSTRICT_INEQUALITY);
      (yyval.con) = pcon;
      delete (yyvsp[-2].rae);
      delete (yyvsp[0].rae);
    }
#line 3910 "parser.cc"
    break;

  case 172: /* constr: rat_aff_expr LE rat_aff_expr  */
#line 1743 "../../../phaver/src/parser.yy"
    {
      Affine_Expr ae = ((yyvsp[-2].rae)->den)*((yyvsp[0].rae)->aexpr) - ((yyvsp[0].rae)->den)*((yyvsp[-2].rae)->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::NONSTRICT_INEQUALITY);
      (yyval.con) = pcon;
      delete (yyvsp[-2].rae);
      delete (yyvsp[0].rae);
    }
#line 3923 "parser.cc"
    break;

  case 173: /* constr: rat_aff_expr EQ rat_aff_expr  */
#line 1752 "../../../phaver/src/parser.yy"
    {
      Affine_Expr ae = ((yyvsp[0].rae)->den)*((yyvsp[-2].rae)->aexpr) - ((yyvsp[-2].rae)->den)*((yyvsp[0].rae)->aexpr);
      auto pcon = new Con(std::move(ae.expr), std::move(ae.inhomo),
                          Con::EQUALITY);
      (yyval.con) = pcon;
      delete (yyvsp[-2].rae);
      delete (yyvsp[0].rae);
    }
#line 3936 "parser.cc"
    break;

  case 174: /* constr: TRUE  */
#line 1761 "../../../phaver/src/parser.yy"
    {
      auto pcon = new Con(Linear_Expr(), 0, Con::EQUALITY);
      (yyval.con) = pcon;
    }
#line 3945 "parser.cc"
    break;

  case 175: /* constr: FALSE  */
#line 1766 "../../../phaver/src/parser.yy"
    {
      auto pcon = new Con(Linear_Expr(), 1, Con::EQUALITY);
      (yyval.con) = pcon;
    }
#line 3954 "parser.cc"
    break;

  case 176: /* rat_aff_expr: INT  */
#line 1774 "../../../phaver/src/parser.yy"
    {
      const auto& str = *((yyvsp[0].mystring));
      auto prae = new Rat_Affine_Expr(str);
      (yyval.rae) = prae;
      delete (yyvsp[0].mystring);
    }
#line 3965 "parser.cc"
    break;

  case 177: /* rat_aff_expr: IDENT PRIM  */
#line 1781 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[-1].mystring));
      if (paut->var_id_map.contains_name(id)) {
        dim_type vid = paut->var_id_map.get_id(id);
        // Variable is primed: add automaton dim.
        vid += paut->dim;
        auto prae = new Rat_Affine_Expr(Var(vid));
        (yyval.rae) = prae;
      } else
        yyerror("Either variable " + id + " not defined in automaton "
                + paut->name + ", or undefined shortcut.");
      delete (yyvsp[-1].mystring);
    }
#line 3983 "parser.cc"
    break;

  case 178: /* rat_aff_expr: IDENT  */
#line 1795 "../../../phaver/src/parser.yy"
    {
      const auto& id = *((yyvsp[0].mystring));
      if (rae_map.find(id) == rae_map.end()) {
        if (paut != nullptr && paut->var_id_map.contains_name(id)) {
          dim_type vid = paut->var_id_map.get_id(id);
          auto prae = new Rat_Affine_Expr(Var(vid));
          (yyval.rae) = prae;
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
        (yyval.rae) = prae;
      }
      delete (yyvsp[0].mystring);
    }
#line 4009 "parser.cc"
    break;

  case 179: /* rat_aff_expr: rat_aff_expr '*' rat_aff_expr  */
#line 1817 "../../../phaver/src/parser.yy"
    {
      if (attempt_multiply(*((yyvsp[-2].rae)),*((yyvsp[0].rae)))) {
        (yyval.rae) = (yyvsp[-2].rae);
        delete (yyvsp[0].rae);
      } else {
        yyerror("Cannot multiply two linear expressions. ");
        cout << *((yyvsp[-2].rae)) << " and " << *((yyvsp[0].rae)) << endl;
      }
    }
#line 4023 "parser.cc"
    break;

  case 180: /* rat_aff_expr: rat_aff_expr '/' rat_aff_expr  */
#line 1827 "../../../phaver/src/parser.yy"
    {
      if (attempt_division(*((yyvsp[-2].rae)),*((yyvsp[0].rae)))) {
        (yyval.rae) = (yyvsp[-2].rae);
        delete (yyvsp[0].rae);
      } else
        yyerror("Cannot divide two linear expressions.");
    }
#line 4035 "parser.cc"
    break;

  case 181: /* rat_aff_expr: rat_aff_expr '+' rat_aff_expr  */
#line 1835 "../../../phaver/src/parser.yy"
    {
      *((yyvsp[-2].rae)) += *((yyvsp[0].rae));
      (yyval.rae) = (yyvsp[-2].rae);
      delete (yyvsp[0].rae);
    }
#line 4045 "parser.cc"
    break;

  case 182: /* rat_aff_expr: rat_aff_expr '-' rat_aff_expr  */
#line 1841 "../../../phaver/src/parser.yy"
    {
      *((yyvsp[-2].rae)) -= *((yyvsp[0].rae));
      (yyval.rae) = (yyvsp[-2].rae);
      delete (yyvsp[0].rae);
    }
#line 4055 "parser.cc"
    break;

  case 183: /* rat_aff_expr: '-' rat_aff_expr  */
#line 1847 "../../../phaver/src/parser.yy"
    {
      neg_assign(*(yyvsp[0].rae));
      (yyval.rae) = (yyvsp[0].rae);
    }
#line 4064 "parser.cc"
    break;

  case 184: /* rat_aff_expr: '(' rat_aff_expr ')'  */
#line 1852 "../../../phaver/src/parser.yy"
    {
      (yyval.rae) = (yyvsp[-1].rae);
    }
#line 4072 "parser.cc"
    break;


#line 4076 "parser.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1857 "../../../phaver/src/parser.yy"

