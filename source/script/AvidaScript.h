/*
 *  AvidaScript.h
 *  avida_test_language
 *
 *  Created by David on 1/14/06.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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
  AS_TOKEN_SUPPRESS = 1,
  AS_TOKEN_ENDL,
  AS_TOKEN_COMMA,
  
  AS_TOKEN_OP_BIT_NOT, // 4
  AS_TOKEN_OP_BIT_AND,
  AS_TOKEN_OP_BIT_OR,
  
  AS_TOKEN_OP_LOGIC_NOT, // 7
  AS_TOKEN_OP_LOGIC_AND,
  AS_TOKEN_OP_LOGIC_OR,
  
  AS_TOKEN_OP_ADD, // 10
  AS_TOKEN_OP_SUB,
  AS_TOKEN_OP_MUL,
  AS_TOKEN_OP_DIV,
  AS_TOKEN_OP_MOD,
  
  AS_TOKEN_DOT, // 15
  AS_TOKEN_ASSIGN,
  AS_TOKEN_REF,
  
  AS_TOKEN_OP_EQ, // 18
  AS_TOKEN_OP_LE,
  AS_TOKEN_OP_GE,
  AS_TOKEN_OP_LT,
  AS_TOKEN_OP_GT,
  AS_TOKEN_OP_NEQ,
  
  AS_TOKEN_PREC_OPEN, // 24
  AS_TOKEN_PREC_CLOSE,
  
  AS_TOKEN_IDX_OPEN, // 26
  AS_TOKEN_IDX_CLOSE,
  
  AS_TOKEN_ARR_OPEN, // 28
  AS_TOKEN_ARR_CLOSE,
  AS_TOKEN_ARR_RANGE,
  AS_TOKEN_ARR_EXPAN,
  AS_TOKEN_ARR_WILD,
  
  AS_TOKEN_MAT_MODIFY, // 33
  
  AS_TOKEN_TYPE_ARRAY, // 34
  AS_TOKEN_TYPE_CHAR,
  AS_TOKEN_TYPE_FLOAT,
  AS_TOKEN_TYPE_INT,
  AS_TOKEN_TYPE_MATRIX,
  AS_TOKEN_TYPE_STRING,
  AS_TOKEN_TYPE_VOID,
  
  AS_TOKEN_CMD_IF, // 41
  AS_TOKEN_CMD_ELSE,
  AS_TOKEN_CMD_ELSEIF,
  AS_TOKEN_CMD_WHILE,
  AS_TOKEN_CMD_FOREACH,
  AS_TOKEN_CMD_FUNCTION,
  
  AS_TOKEN_CMD_RETURN, // 47
  
  AS_TOKEN_ID, // 48
  
  AS_TOKEN_FLOAT, // 49
  AS_TOKEN_INT,
  AS_TOKEN_STRING,
  AS_TOKEN_CHAR,
  
  AS_TOKEN_UNKNOWN, // 53
  AS_TOKEN_INVALID
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


typedef enum eASSemanticErrors {
  AS_SEMANTIC_WARN_LOSS_OF_PRECISION,
  AS_SEMANTIC_WARN_NO_DIMENSIONS,
  AS_SEMANTIC_WARN_NO_RETURN,
  AS_SEMANTIC_WARN_UNREACHABLE,
  AS_SEMANTIC_WARN__LAST,
  
  AS_SEMANTIC_ERR_CANNOT_CAST,
  AS_SEMANTIC_ERR_FUNCTION_DEFAULT_REQUIRED,
  AS_SEMANTIC_ERR_FUNCTION_REDEFINITION,
  AS_SEMANTIC_ERR_FUNCTION_RTYPE_MISMATCH,
  AS_SEMANTIC_ERR_FUNCTION_SIGNATURE_MISMATCH,
  AS_SEMANTIC_ERR_FUNCTION_UNDEFINED,
  AS_SEMANTIC_ERR_TOO_MANY_ARGUMENTS,
  AS_SEMANTIC_ERR_UNDEFINED_TYPE_OP,
  AS_SEMANTIC_ERR_UNPACK_WILD_NONARRAY,
  AS_SEMANTIC_ERR_VARIABLE_DIMENSIONS_INVALID,
  AS_SEMANTIC_ERR_VARIABLE_UNDEFINED,
  AS_SEMANTIC_ERR_VARIABLE_REDEFINITION,
  AS_SEMANTIC_ERR_INTERNAL,
  
  AS_SEMANTIC_ERR_UNKNOWN
} ASSemanticError_t;


typedef enum eASTypes {
  AS_TYPE_ARRAY = 0,
  AS_TYPE_BOOL,
  AS_TYPE_CHAR,
  AS_TYPE_FLOAT,
  AS_TYPE_INT,
  AS_TYPE_OBJECT_REF,
  AS_TYPE_MATRIX,
  AS_TYPE_STRING,
  
  AS_TYPE_RUNTIME,

  AS_TYPE_VOID,

  AS_TYPE_INVALID
} ASType_t;


namespace AvidaScript {
  const char* mapToken(ASToken_t type);
  const char* mapType(ASType_t type);
};

#endif
