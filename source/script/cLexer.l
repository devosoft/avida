%{
/*
 *  cLexer.l
 *  avida_test_language
 *
 *  Created by David on 1/13/06.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cLexer.h"
  
#include "AvidaScript.h"
  
#include <iostream>

%}

%option c++
%option noyywrap
%option yylineno

%%

#.*\n       /* ignore comments */
\/\/.*\n    /* ignore comments */
[ \t]+      /* ignore whitespace */


;           return AS_TOKEN_SUPPRESS;       // End-of-line
\n          return AS_TOKEN_ENDL;
,           return AS_TOKEN_COMMA;

~           return AS_TOKEN_OP_BIT_NOT;    // Bitwise Operators
&           return AS_TOKEN_OP_BIT_AND;
\|          return AS_TOKEN_OP_BIT_OR;

!           return AS_TOKEN_OP_LOGIC_NOT;  // Logic Operators
&&          return AS_TOKEN_OP_LOGIC_AND;
\|\|        return AS_TOKEN_OP_LOGIC_OR;

\+          return AS_TOKEN_OP_ADD;        // Arithmetic Operators
-           return AS_TOKEN_OP_SUB;
\*          return AS_TOKEN_OP_MUL;
\/          return AS_TOKEN_OP_DIV;
%           return AS_TOKEN_OP_MOD;

\.          return AS_TOKEN_DOT;
=           return AS_TOKEN_ASSIGN;        // Assignment
@           return AS_TOKEN_REF;

==          return AS_TOKEN_OP_EQ;         // Relational Operators
\<=         return AS_TOKEN_OP_LE;
\>=         return AS_TOKEN_OP_GE;
\<          return AS_TOKEN_OP_LT;
\>          return AS_TOKEN_OP_GT;
!=          return AS_TOKEN_OP_NEQ;

\(          return AS_TOKEN_PREC_OPEN;     // Precedence Grouping
\)          return AS_TOKEN_PREC_CLOSE;

\[          return AS_TOKEN_IDX_OPEN;      // Index Grouping
\]          return AS_TOKEN_IDX_CLOSE;

\{          return AS_TOKEN_ARR_OPEN;      // Array Definition Grouping
\}          return AS_TOKEN_ARR_CLOSE;
:           return AS_TOKEN_ARR_RANGE;
\^          return AS_TOKEN_ARR_EXPAN;
\.\.        return AS_TOKEN_ARR_WILD;

\$          return AS_TOKEN_MAT_MODIFY;    // Matrix Modifier

array       return AS_TOKEN_TYPE_ARRAY;    // Built-in Types
char        return AS_TOKEN_TYPE_CHAR;
float       return AS_TOKEN_TYPE_FLOAT;
int         return AS_TOKEN_TYPE_INT;
matrix      return AS_TOKEN_TYPE_MATRIX;
string      return AS_TOKEN_TYPE_STRING;
void        return AS_TOKEN_TYPE_VOID;

if          return AS_TOKEN_CMD_IF;        // Blocks
else        return AS_TOKEN_CMD_ELSE;
elseif      return AS_TOKEN_CMD_ELSEIF;
while       return AS_TOKEN_CMD_WHILE;
foreach     return AS_TOKEN_CMD_FOREACH;
function    return AS_TOKEN_CMD_FUNCTION;

return      return AS_TOKEN_CMD_RETURN;

([a-zA-Z]|_+[a-zA-Z])[a-zA-Z0-9_]*  return AS_TOKEN_ID;      // Identifiers
  
[0-9]*\.[0-9]+([eE][-+]?[0-9]+)?    return AS_TOKEN_FLOAT;   // Literal Values
[0-9]+                              return AS_TOKEN_INT;
\"\"                                return AS_TOKEN_STRING;
\"([^"\\\n]|\\\"|\\\\|\\.)*\"       return AS_TOKEN_STRING;
\'([^'\\\n]|\\\'|\\\\)\'            return AS_TOKEN_CHAR;

.           return AS_TOKEN_UNKNOWN;           // Unrecognized Tokens



