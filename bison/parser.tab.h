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

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
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
    COMMA = 258,
    SEMICOLON = 259,
    COLON = 260,
    ANDPERSAND = 261,
    OPARENTHESIS = 262,
    CPARENTHESIS = 263,
    OBRACKET = 264,
    CBRACKET = 265,
    OBRACER = 266,
    CBRACER = 267,
    NOT_DEFINED = 268,
    IF = 269,
    ELSE = 270,
    WHILE = 271,
    DO = 272,
    FOR = 273,
    FUNC = 274,
    RETURN = 275,
    GLOBAL = 276,
    BOOL = 277,
    INT = 278,
    DOUBLE = 279,
    STRING = 280,
    ID = 281,
    ASSIGMENT = 282,
    OR = 283,
    AND = 284,
    EQUAL = 285,
    NOT_EQUAL = 286,
    LT = 287,
    LE = 288,
    GT = 289,
    GE = 290,
    PLUS = 291,
    MINUS = 292,
    MULT = 293,
    DIV = 294,
    MOD = 295,
    NEG = 296,
    POS = 297,
    NOT = 298,
    POW = 299
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 17 "parser.y" /* yacc.c:1915  */

	int 	ival;
	float 	fval;
	char* 	sval;

	void*	n;

#line 107 "parser.tab.h" /* yacc.c:1915  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
