/*
 *  AvidaScript.cc
 *  Avida
 *
 *  Created by David on 2/26/08.
 *  Copyright 2008 Michigan State University. All rights reserved.
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

const char* AvidaScript::mapType(ASType_t type)
{
  switch (type) {
    case AS_TYPE_ARRAY:       return "array";
    case AS_TYPE_CHAR:        return "char";
    case AS_TYPE_FLOAT:       return "float";
    case AS_TYPE_INT:         return "int";
    case AS_TYPE_MATRIX:      return "matrix";
    case AS_TYPE_STRING:      return "string";
    case AS_TYPE_VOID:        return "void";
    case AS_TYPE_OBJECT_REF:  return "object";
      
    case AS_TYPE_INVALID:
    default:
      return "*INVALID*";
  }
}


