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
  
  OP_BIT_NOT,
  OP_BIT_AND,
  OP_BIT_OR,
  
  OP_LOGIC_NOT,
  OP_LOGIC_AND,
  OP_LOGIC_OR,
  
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  
  DOT,
  ASSIGN,
  REF,
  
  OP_EQ,
  OP_LE,
  OP_GE,
  OP_LT,
  OP_GT,
  OP_NEQ,
  
  PREC_OPEN,
  PREC_CLOSE,
  
  IDX_OPEN,
  IDX_CLOSE,
  
  ARR_OPEN,
  ARR_CLOSE,
  ARR_RANGE,
  ARR_EXPAN,
  ARR_WILD,
  
  MAT_MODIFY,
  
  TYPE_ARRAY,
  TYPE_CHAR,
  TYPE_FLOAT,
  TYPE_INT,
  TYPE_MATRIX,
  TYPE_STRING,
  TYPE_VOID,
  
  CMD_IF,
  CMD_ELSE,
  CMD_ENDIF,
  
  CMD_WHILE,
  CMD_ENDWHILE,
  
  CMD_FOREACH,
  CMD_ENDFOREACH,
  
  CMD_FUNCTION,
  CMD_ENDFUNCTION,
  
  CMD_RETURN,
  
  ID,
  
  FLOAT,
  INT,
  STRING,
  CHAR,
  
  UNKNOWN,
  INVALID
} ASToken_t;

typedef enum eASParseErrors {
  AS_PARSE_ERR_UNEXPECTED_TOKEN,
  AS_PARSE_ERR_UNTERMINATED_EXPR,
  AS_PARSE_ERR_NULL_EXPR,
  AS_PARSE_ERR_EOF,
  AS_PARSE_ERR_INTERNAL,
  AS_PARSE_ERR_UNKNOWN
} ASParseError_t;

#endif
