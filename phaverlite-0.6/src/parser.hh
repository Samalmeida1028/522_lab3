/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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

#line 289 "parser.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_HH_INCLUDED  */
