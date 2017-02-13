/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     HCE_VOID = 258,
     HCE_STRUCT = 259,
     HCE_BOOL = 260,
     HCE_BYTE = 261,
     HCE_SHORT = 262,
     HCE_INT = 263,
     HCE_DOUBLE = 264,
     HCE_FLOAT = 265,
     HCE_LONG = 266,
     HCE_STRING = 267,
     HCE_BINARY = 268,
     HCE_VECTOR = 269,
     HCE_MAP = 270,
     HCE_NAMESPACE = 271,
     HCE_INTERFACE = 272,
     HCE_IDENTIFIER = 273,
     HCE_OP = 274,
     HCE_INTEGER_LITERAL = 275,
     HCE_FLOATING_POINT_LITERAL = 276,
     HCE_FALSE = 277,
     HCE_TRUE = 278,
     HCE_STRING_LITERAL = 279,
     HCE_SCOPE_DELIMITER = 280,
     HCE_CONST = 281,
     BAD_CHAR = 282
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


