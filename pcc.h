/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_PCC_H_INCLUDED
# define YY_YY_PCC_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    EQ = 258,
    NE = 259,
    LE = 260,
    GE = 261,
    OR = 262,
    AND = 263,
    UNOP = 264,
    IDENT = 265,
    SIN = 266,
    ASIN = 267,
    COS = 268,
    ACOS = 269,
    TAN = 270,
    ATAN = 271,
    EXP = 272,
    LOG = 273,
    LOG10 = 274,
    CEIL = 275,
    FLOOR = 276,
    WRITE = 277,
    READ = 278,
    WHILE = 279,
    IF = 280,
    ELSE = 281,
    FLOAT = 282,
    STR = 283
  };
#endif
/* Tokens.  */
#define EQ 258
#define NE 259
#define LE 260
#define GE 261
#define OR 262
#define AND 263
#define UNOP 264
#define IDENT 265
#define SIN 266
#define ASIN 267
#define COS 268
#define ACOS 269
#define TAN 270
#define ATAN 271
#define EXP 272
#define LOG 273
#define LOG10 274
#define CEIL 275
#define FLOOR 276
#define WRITE 277
#define READ 278
#define WHILE 279
#define IF 280
#define ELSE 281
#define FLOAT 282
#define STR 283

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 35 "pcc.y" /* yacc.c:1909  */

    struct sStackType {
    unsigned char   flag;
        union {
        double         vFloat;
        char          *vStr;
        struct ast_s  *ast;
        }   u;
    }   s;

#line 121 "pcc.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PCC_H_INCLUDED  */
