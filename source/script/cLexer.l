%{
/*
 *  cLexer.l
 *  avida_test_language
 *
 *  Created by David on 1/13/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

@\{         return AS_TOKEN_LITERAL_DICT;  // Aggregate literals
=\>         return AS_TOKEN_DICT_MAPPING;
\$\{        return AS_TOKEN_LITERAL_MATRIX;

array       return AS_TOKEN_TYPE_ARRAY;    // Built-in Types
bool        return AS_TOKEN_TYPE_BOOL;
char        return AS_TOKEN_TYPE_CHAR;
dict        return AS_TOKEN_TYPE_DICT;
float       return AS_TOKEN_TYPE_FLOAT;
int         return AS_TOKEN_TYPE_INT;
matrix      return AS_TOKEN_TYPE_MATRIX;
string      return AS_TOKEN_TYPE_STRING;
var         return AS_TOKEN_TYPE_VAR;
void        return AS_TOKEN_TYPE_VOID;

if          return AS_TOKEN_CMD_IF;        // Blocks
else        return AS_TOKEN_CMD_ELSE;
elseif      return AS_TOKEN_CMD_ELSEIF;
while       return AS_TOKEN_CMD_WHILE;
foreach     return AS_TOKEN_CMD_FOREACH;
function    return AS_TOKEN_CMD_FUNCTION;

asbool      return AS_TOKEN_BUILTIN_CALL;
aschar      return AS_TOKEN_BUILTIN_CALL;
asint       return AS_TOKEN_BUILTIN_CALL;
asfloat     return AS_TOKEN_BUILTIN_CALL;
asstring    return AS_TOKEN_BUILTIN_CALL;
isarray     return AS_TOKEN_BUILTIN_CALL;
isbool      return AS_TOKEN_BUILTIN_CALL;
ischar      return AS_TOKEN_BUILTIN_CALL;
isdict      return AS_TOKEN_BUILTIN_CALL;
isint       return AS_TOKEN_BUILTIN_CALL;
isfloat     return AS_TOKEN_BUILTIN_CALL;
ismatrix    return AS_TOKEN_BUILTIN_CALL;
isstring    return AS_TOKEN_BUILTIN_CALL;
clear       return AS_TOKEN_BUILTIN_METHOD;
copy        return AS_TOKEN_BUILTIN_METHOD;
haskey      return AS_TOKEN_BUILTIN_METHOD;
keys        return AS_TOKEN_BUILTIN_METHOD;
len         return AS_TOKEN_BUILTIN_METHOD;
remove      return AS_TOKEN_BUILTIN_METHOD;
resize      return AS_TOKEN_BUILTIN_METHOD;
values      return AS_TOKEN_BUILTIN_METHOD;

return      return AS_TOKEN_CMD_RETURN;

true        return AS_TOKEN_BOOL;
false       return AS_TOKEN_BOOL;



([a-zA-Z]|_+[a-zA-Z])[a-zA-Z0-9_]*  return AS_TOKEN_ID;      // Identifiers
  
[0-9]*\.[0-9]+([eE][-+]?[0-9]+)?    return AS_TOKEN_FLOAT;   // Literal Values
[0-9]+                              return AS_TOKEN_INT;
\"\"                                return AS_TOKEN_STRING;
\"([^"\\\n]|\\\"|\\\\|\\.)*\"       return AS_TOKEN_STRING;
\'([^'\\\n]|\\\'|\\\\|\\.)\'        return AS_TOKEN_CHAR;

.           return AS_TOKEN_UNKNOWN;           // Unrecognized Tokens



