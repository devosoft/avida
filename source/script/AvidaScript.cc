/*
 *  AvidaScript.cc
 *  Avida
 *
 *  Created by David on 2/26/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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

#include "AvidaScript.h"


const char* AvidaScript::mapBuiltIn(ASBuiltIn_t builtin)
{
  switch (builtin) {
    case AS_BUILTIN_CAST_BOOL:    return "asbool";
    case AS_BUILTIN_CAST_CHAR:    return "aschar";
    case AS_BUILTIN_CAST_INT:     return "asint";
    case AS_BUILTIN_CAST_FLOAT:   return "asfloat";
    case AS_BUILTIN_CAST_STRING:  return "asstring";
    case AS_BUILTIN_IS_ARRAY:     return "is_array";
    case AS_BUILTIN_IS_BOOL:      return "is_bool";
    case AS_BUILTIN_IS_CHAR:      return "is_char";
    case AS_BUILTIN_IS_DICT:      return "is_dict";
    case AS_BUILTIN_IS_INT:       return "is_int";
    case AS_BUILTIN_IS_FLOAT:     return "is_float";
    case AS_BUILTIN_IS_MATRIX:    return "is_matrix";
    case AS_BUILTIN_IS_STRING:    return "is_string";
    case AS_BUILTIN_CLEAR:        return "clear";
    case AS_BUILTIN_COPY:         return "copy";
    case AS_BUILTIN_HASKEY:       return "haskey";
    case AS_BUILTIN_KEYS:         return "keys";
    case AS_BUILTIN_LEN:          return "len";
    case AS_BUILTIN_REMOVE:       return "remove";
    case AS_BUILTIN_RESIZE:       return "resize";
    case AS_BUILTIN_VALUES:       return "values";
    default:                      return "?";
  }
}

const char* AvidaScript::mapToken(ASToken_t token)
{
  switch (token) {
    case AS_TOKEN_OP_BIT_NOT:   return "~";
    case AS_TOKEN_OP_BIT_AND:   return "&";
    case AS_TOKEN_OP_BIT_OR:    return "|";
    case AS_TOKEN_OP_LOGIC_NOT: return "!";
    case AS_TOKEN_OP_LOGIC_AND: return "&&";
    case AS_TOKEN_OP_LOGIC_OR:  return "||";
    case AS_TOKEN_OP_ADD:       return "+";
    case AS_TOKEN_OP_SUB:       return "-";
    case AS_TOKEN_OP_MUL:       return "*";
    case AS_TOKEN_OP_DIV:       return "/";
    case AS_TOKEN_OP_MOD:       return "%";
    case AS_TOKEN_DOT:          return ".";
    case AS_TOKEN_OP_EQ:        return "==";
    case AS_TOKEN_OP_LE:        return "<=";
    case AS_TOKEN_OP_GE:        return ">=";
    case AS_TOKEN_OP_LT:        return "<";
    case AS_TOKEN_OP_GT:        return ">";
    case AS_TOKEN_OP_NEQ:       return "!=";
    case AS_TOKEN_ARR_RANGE:    return ":";
    case AS_TOKEN_ARR_EXPAN:    return "^";
    case AS_TOKEN_IDX_OPEN:     return "[]";
    default:                    return "?";
  }      
}

const char* AvidaScript::mapType(const sASTypeInfo& type)
{
  switch (type.type) {
    case AS_TYPE_ARRAY:       return "array";
    case AS_TYPE_BOOL:        return "bool";
    case AS_TYPE_CHAR:        return "char";
    case AS_TYPE_DICT:        return "dict";
    case AS_TYPE_FLOAT:       return "float";
    case AS_TYPE_INT:         return "int";
    case AS_TYPE_MATRIX:      return "matrix";
    case AS_TYPE_OBJECT_REF:  return type.info;
    case AS_TYPE_STRING:      return "string";
    case AS_TYPE_VAR:         return "var";
    case AS_TYPE_RUNTIME:     return "-runtime-";
    case AS_TYPE_VOID:        return "void";

    case AS_TYPE_INVALID:
    default:
      return "*INVALID*";
  }
}


