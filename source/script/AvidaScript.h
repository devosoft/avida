/*
 *  AvidaScript.h
 *  avida_test_language
 *
 *  Created by David on 1/14/06.
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

#ifndef AvidaScript_h
#define AvidaScript_h

typedef enum eASTokens {
  SUPPRESS = 1,
  ENDL,
  COMMA,
  
  OP_BIT_NOT, // 4
  OP_BIT_AND,
  OP_BIT_OR,
  
  OP_LOGIC_NOT, // 7
  OP_LOGIC_AND,
  OP_LOGIC_OR,
  
  OP_ADD, // 10
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  
  DOT, // 15
  ASSIGN,
  REF,
  
  OP_EQ, // 18
  OP_LE,
  OP_GE,
  OP_LT,
  OP_GT,
  OP_NEQ,
  
  PREC_OPEN, // 24
  PREC_CLOSE,
  
  IDX_OPEN, // 26
  IDX_CLOSE,
  
  ARR_OPEN, // 28
  ARR_CLOSE,
  ARR_RANGE,
  ARR_EXPAN,
  ARR_WILD,
  
  MAT_MODIFY, // 33
  
  TYPE_ARRAY, // 34
  TYPE_CHAR,
  TYPE_FLOAT,
  TYPE_INT,
  TYPE_MATRIX,
  TYPE_STRING,
  TYPE_VOID,
  
  CMD_IF, // 41
  CMD_ELSE,
  CMD_ENDIF,
  
  CMD_WHILE, // 44
  CMD_ENDWHILE,
  
  CMD_FOREACH, // 46
  CMD_ENDFOREACH,
  
  CMD_FUNCTION, // 48
  CMD_ENDFUNCTION,
  
  CMD_RETURN, // 50
  
  ID, // 51
  
  FLOAT, // 52
  INT,
  STRING,
  CHAR,
  
  UNKNOWN, // 56
  INVALID
} ASToken_t;

typedef enum eASParseErrors {
  AS_PARSE_ERR_UNEXPECTED_TOKEN,
  AS_PARSE_ERR_UNTERMINATED_EXPR,
  AS_PARSE_ERR_NULL_EXPR,
  AS_PARSE_ERR_EOF,
  AS_PARSE_ERR_EMPTY,
  AS_PARSE_ERR_INTERNAL,
  AS_PARSE_ERR_UNKNOWN
} ASParseError_t;

#endif
