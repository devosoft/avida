%{
/*
 *  cLexer.l
 *  avida_test_language
 *
 *  Created by David on 1/13/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cLexer.h"
  
#include "AvidaScript.h"
  
#include <iostream>

%}

%option c++
%option noyywrap

%%

#.*\n       /* ignore comments */
\/\/        /* ignore comments */
[ \t]+      /* ignore whitespace */


;           return SUPRESS;       // End-of-line
\n          return ENDL;
,           return COMMA;

~           return OP_BIT_NOT;    // Bitwise Operators
&           return OP_BIT_AND;
\|          return OP_BIT_OR;

!           return OP_LOGIC_NOT;  // Logic Operators
&&          return OP_LOGIC_AND;
\|\|        return OP_LOGIC_OR;

\+          return OP_ADD;        // Arithmetic Operators
-           return OP_SUB;
\*          return OP_MUL;
\/          return OP_DIV;
%           return OP_MOD;

\.          return DOT;
=           return ASSIGN;        // Assignment
@           return REF;

==          return OP_EQ;         // Relational Operators
\<=         return OP_LE;
\>=         return OP_GE;
\<          return OP_LT;
\>          return OP_GT;
!=          return OP_NEQ;

\(          return PREC_OPEN;     // Precedence Grouping
\)          return PREC_CLOSE;

\[          return IDX_OPEN;      // Index Grouping
\]          return IDX_CLOSE;

\{          return ARR_OPEN;      // Array Definition Grouping
\}          return ARR_CLOSE;
:           return ARR_RANGE;
\^          return ARR_EXPAN;

bool        return TYPE_BOOL;     // Built-in Types
char        return TYPE_CHAR;
float       return TYPE_FLOAT;
int         return TYPE_INT;
void        return TYPE_VOID;

if          return CMD_IF;        // If Blocks
else        return CMD_ELSE;
endif       return CMD_ENDIF;

while       return CMD_WHILE;     // While Blocks
endwhile    return CMD_ENDWHILE;

foreach     return CMD_FOREACH;   // Foreach Blocks
endforeach  return CMD_ENDFOREACH;

function    return CMD_FUNCTION;  // Function Blocks
endfunction return CMD_ENDFUNCTION;

([a-zA-Z]|_+[a-zA-Z])[a-zA-Z0-9_]*  return ID;      // Identifiers
  
[0-9]*\.[0-9]+([eE][-+]?[0-9]+)?    return FLOAT;   // Literal Values
[0-9]+                              return INT;
\"\"                                return STRING;
\"([^"\\\n]|\\\"|\\\\|\\.)*\"       return STRING;
\'([^'\\\n]|\\\'|\\\\)\'            return CHAR;

.           return ERR;           // Unrecognized Tokens



