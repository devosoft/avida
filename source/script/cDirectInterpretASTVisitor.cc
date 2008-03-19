/*
 *  cDirectInterpretASTVisitor.cc
 *  Avida
 *
 *  Created by David on 3/4/08.
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

#include "cDirectInterpretASTVisitor.h"

#include <cassert>
#include <cmath>

#include "avida.h"
#include "AvidaScript.h"

#include "cASFunction.h"
#include "cStringUtil.h"
#include "cSymbolTable.h"

using namespace AvidaScript;


#ifndef DEBUG_AS_DIRECT_INTERPRET
#define DEBUG_AS_DIRECT_INTERPRET 1
#endif

#define INTERPRET_ERROR(code, ...) reportError(AS_DIRECT_INTERPRET_ERR_ ## code, node.GetFilePosition(),  __LINE__, ##__VA_ARGS__)

#define TOKEN(x) AS_TOKEN_ ## x
#define TYPE(x) AS_TYPE_ ## x


cDirectInterpretASTVisitor::cDirectInterpretASTVisitor(cSymbolTable* global_symtbl)
  : m_global_symtbl(global_symtbl), m_cur_symtbl(global_symtbl), m_rtype(TYPE(INVALID)), m_call_stack(0, 2048), m_sp(0)
  , m_has_returned(false), m_obj_assign(false)
{
  m_call_stack.Resize(m_global_symtbl->GetNumVariables());
  for (int i = 0; i < m_global_symtbl->GetNumVariables(); i++) {
    switch (m_global_symtbl->GetVariableType(i).type) {
      case TYPE(ARRAY):       m_call_stack[i].value.as_array = new cLocalArray; break;
      case TYPE(BOOL):        m_call_stack[i].value.as_bool = false; break;
      case TYPE(CHAR):        m_call_stack[i].value.as_char = 0; break;
      case TYPE(DICT):        m_call_stack[i].value.as_dict = new cLocalDict; break;
      case TYPE(INT):         m_call_stack[i].value.as_int = 0; break;
      case TYPE(FLOAT):       m_call_stack[i].value.as_float = 0.0; break;
      case TYPE(MATRIX):      m_call_stack[i].value.as_matrix = NULL; break;
      case TYPE(OBJECT_REF):  m_call_stack[i].value.as_ref = NULL; break;
      case TYPE(STRING):      m_call_stack[i].value.as_string = NULL; break;
      default: break;
    }
  }
}

cDirectInterpretASTVisitor::~cDirectInterpretASTVisitor()
{
  for (int i = 0; i < m_global_symtbl->GetNumVariables(); i++) {
    ASType_t type = m_global_symtbl->GetVariableType(i).type;
    if (type == TYPE(VAR)) type = m_call_stack[i].type.type;
    
    switch (type) {
      case TYPE(ARRAY):       m_call_stack[i].value.as_array->RemoveReference(); break;
      case TYPE(BOOL):        break;
      case TYPE(CHAR):        break;
      case TYPE(DICT):        m_call_stack[i].value.as_dict->RemoveReference(); break;
      case TYPE(INT):         break;
      case TYPE(FLOAT):       break;
      case TYPE(MATRIX):      break; // @TODO - cleanup scope
      case TYPE(OBJECT_REF):  delete m_call_stack[i].value.as_ref; break;
      case TYPE(STRING):      delete m_call_stack[i].value.as_string; break;
      default: break;
    }
  }
  
}


int cDirectInterpretASTVisitor::Interpret(cASTNode* node)
{
  node->Accept(*this);
  
  if (m_has_returned) return asInt(m_rtype, m_rvalue, *node);

  return 0;
}


void cDirectInterpretASTVisitor::VisitAssignment(cASTAssignment& node)
{
  cSymbolTable* symtbl = node.IsVarGlobal() ? m_global_symtbl : m_cur_symtbl;
  int sp = node.IsVarGlobal() ? 0 : m_sp;
  int var_id = node.GetVarID();
  
  node.GetExpression()->Accept(*this);
  
  switch (symtbl->GetVariableType(var_id).type) {
    case TYPE(BOOL):        m_call_stack[sp + var_id].value.as_bool = asBool(m_rtype, m_rvalue, node); break;
    case TYPE(CHAR):        m_call_stack[sp + var_id].value.as_char = asChar(m_rtype, m_rvalue, node); break;
    case TYPE(FLOAT):       m_call_stack[sp + var_id].value.as_float = asFloat(m_rtype, m_rvalue, node); break;
    case TYPE(INT):         m_call_stack[sp + var_id].value.as_int = asInt(m_rtype, m_rvalue, node); break;
    case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - assignment

    case TYPE(VAR):
      m_call_stack[sp + var_id].value = m_rvalue;
      m_call_stack[sp + var_id].type = m_rtype;
      break;
      
    case TYPE(ARRAY):
      m_call_stack[sp + var_id].value.as_array->RemoveReference();
      m_call_stack[sp + var_id].value.as_array = asArray(m_rtype, m_rvalue, node);
      break;
      

    case TYPE(DICT):
      m_call_stack[sp + var_id].value.as_dict->RemoveReference();
      m_call_stack[sp + var_id].value.as_dict = asDict(m_rtype, m_rvalue, node);
      break;
      
      
    case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - assignment

    case TYPE(STRING):
      delete m_call_stack[sp + var_id].value.as_string;
      m_call_stack[sp + var_id].value.as_string = asString(m_rtype, m_rvalue, node);
      break;
      
    default:
      INTERPRET_ERROR(INTERNAL);
  }
}


void cDirectInterpretASTVisitor::VisitArgumentList(cASTArgumentList& node)
{
  // Should never recurse into here.  Argument lists are processed by their owners as needed.
  INTERPRET_ERROR(INTERNAL);
}


void cDirectInterpretASTVisitor::VisitObjectAssignment(cASTObjectAssignment& node)
{
  m_obj_assign = true;
  node.GetTarget()->Accept(*this);
  m_obj_assign = false;
  cObjectRef* obj = m_rvalue.as_ref;
  
  node.GetExpression()->Accept(*this);
  
  // @TODO - check object assign type for validity
  
  if (!obj->Set(m_rtype.type, m_rvalue)) INTERPRET_ERROR(OBJECT_ASSIGN_FAIL);
}



void cDirectInterpretASTVisitor::VisitReturnStatement(cASTReturnStatement& node)
{
  node.GetExpression()->Accept(*this);
  m_has_returned = true;
}


void cDirectInterpretASTVisitor::VisitStatementList(cASTStatementList& node)
{
  tListIterator<cASTNode> it = node.Iterator();
  
  cASTNode* stmt = NULL;
  while (!m_has_returned && (stmt = it.Next())) {
    stmt->Accept(*this);
  }
}



void cDirectInterpretASTVisitor::VisitForeachBlock(cASTForeachBlock& node)
{
  int var_id = node.GetVariable()->GetVarID();
  sASTypeInfo var_type = node.GetVariable()->GetType();
  
  node.GetValues()->Accept(*this);
  cLocalArray* arr = m_rvalue.as_array;

  int var_idx = m_sp + var_id;
  for (int i = 0; i < arr->GetSize(); i++) {
    // Set the variable value for this iteration
    const sAggregateValue& val = arr->Get(i);
    switch (var_type.type) {
      case TYPE(BOOL):        m_call_stack[var_idx].value.as_bool = asBool(val.type, val.value, node); break;
      case TYPE(CHAR):        m_call_stack[var_idx].value.as_char = asChar(val.type, val.value, node); break;
      case TYPE(FLOAT):       m_call_stack[var_idx].value.as_float = asFloat(val.type, val.value, node); break;
      case TYPE(INT):         m_call_stack[var_idx].value.as_int = asInt(val.type, val.value, node); break;
      case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - foreach assignment
        
      case TYPE(ARRAY):
        m_call_stack[var_idx].value.as_array->RemoveReference();
        m_call_stack[var_idx].value.as_array = asArray(val.type, val.value, node);
        break;
        
      case TYPE(DICT):
        m_call_stack[var_idx].value.as_dict->RemoveReference();
        m_call_stack[var_idx].value.as_dict = asDict(val.type, val.value, node);
        break;
        
      case TYPE(VAR):
        m_call_stack[var_idx].value = val.value;
        m_call_stack[var_idx].type = val.type;
        break;
        
      case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - foreach assignment
        
      case TYPE(STRING):
        delete m_call_stack[var_idx].value.as_string;
        m_call_stack[var_idx].value.as_string = asString(val.type, val.value, node);
        break;
        
      default:
        INTERPRET_ERROR(INTERNAL);
    }
    
    // Execute the body
    node.GetCode()->Accept(*this);
  }

  arr->RemoveReference();
}


void cDirectInterpretASTVisitor::VisitIfBlock(cASTIfBlock& node)
{
  node.GetCondition()->Accept(*this);
  
  if (asBool(m_rtype, m_rvalue, node)) {
    node.GetCode()->Accept(*this);
  } else {
    bool exec = false;
    tListIterator<cASTIfBlock::cElseIf> it = node.ElseIfIterator();
    cASTIfBlock::cElseIf* ei = NULL;
    while ((ei = it.Next())) {
      ei->GetCondition()->Accept(*this);
      if (asBool(m_rtype, m_rvalue, node)) {
        exec = true;
        ei->GetCode()->Accept(*this);
        break;
      }
    }
    
    if (!exec && node.HasElse()) node.GetElseCode()->Accept(*this);
  }
}


void cDirectInterpretASTVisitor::VisitWhileBlock(cASTWhileBlock& node)
{
  node.GetCondition()->Accept(*this);
  while (asBool(m_rtype, m_rvalue, node)) {
    node.GetCode()->Accept(*this);
    node.GetCondition()->Accept(*this);
  }
}



void cDirectInterpretASTVisitor::VisitFunctionDefinition(cASTFunctionDefinition& node)
{
  // Nothing to do here
}


void cDirectInterpretASTVisitor::VisitVariableDefinition(cASTVariableDefinition& node)
{
  if (node.GetAssignmentExpression()) {
    int var_id = node.GetVarID();
    
    node.GetAssignmentExpression()->Accept(*this);
    
    switch (node.GetType().type) {
      case TYPE(ARRAY):       m_call_stack[m_sp + var_id].value.as_array = asArray(m_rtype, m_rvalue, node); break;
      case TYPE(BOOL):        m_call_stack[m_sp + var_id].value.as_bool = asBool(m_rtype, m_rvalue, node); break;
      case TYPE(CHAR):        m_call_stack[m_sp + var_id].value.as_char = asChar(m_rtype, m_rvalue, node); break;
      case TYPE(DICT):        m_call_stack[m_sp + var_id].value.as_dict = asDict(m_rtype, m_rvalue, node); break;
      case TYPE(FLOAT):       m_call_stack[m_sp + var_id].value.as_float = asFloat(m_rtype, m_rvalue, node); break;
      case TYPE(INT):         m_call_stack[m_sp + var_id].value.as_int = asInt(m_rtype, m_rvalue, node); break;
      case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - var def assignment
      case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - var def assignment
      case TYPE(STRING):
        delete m_call_stack[m_sp + var_id].value.as_string;
        m_call_stack[m_sp + var_id].value.as_string = asString(m_rtype, m_rvalue, node);
        break;
        
      case TYPE(VAR):
        m_call_stack[m_sp + var_id].value = m_rvalue;
        m_call_stack[m_sp + var_id].type = m_rtype;
        break;
        
      default:
        INTERPRET_ERROR(INTERNAL);
    }
  } else if (node.GetDimensions()) {
    if (node.GetType() == TYPE(ARRAY)) {
      cASTNode* szn = node.GetDimensions()->Iterator().Next();
      szn->Accept(*this);
      
      cLocalArray* arr = new cLocalArray();
      arr->Resize(asInt(m_rtype, m_rvalue, node));
      m_call_stack[m_sp + node.GetVarID()].value.as_array = arr;
    } else if (node.GetType() == TYPE(MATRIX)) { // @TODO - variable def dimensions
      
    } else {
      INTERPRET_ERROR(INTERNAL);
    }
  }
}


void cDirectInterpretASTVisitor::VisitVariableDefinitionList(cASTVariableDefinitionList& node)
{
  // Should never recurse into here.  Variable definition lists are processed by function definitions.
  INTERPRET_ERROR(INTERNAL);
}



void cDirectInterpretASTVisitor::VisitExpressionBinary(cASTExpressionBinary& node)
{
  // Process the left and right side expressions
  node.GetLeft()->Accept(*this);
  uAnyType lval = m_rvalue;
  sASTypeInfo ltype = m_rtype;
  node.GetRight()->Accept(*this);
  uAnyType rval = m_rvalue;
  sASTypeInfo rtype = m_rtype;
  
  
  switch (node.GetOperator()) {
    case TOKEN(ARR_RANGE):
      {
        int l = asInt(ltype, lval, node);
        int r = asInt(rtype, rval, node);
        int sz = abs(r - l) + 1;

        cLocalArray* arr = new cLocalArray(sz);
        uAnyType val;
        if (r > l) {
          for (int i = 0; i < sz; i++) {
            val.as_int = l + i;
            arr->Set(i, TYPE(INT), val);
          }
        } else {
          for (int i = 0; i < sz; i++) {
            val.as_int = l - i;
            arr->Set(i, TYPE(INT), val);
          }
        }
        
        m_rvalue.as_array = arr;
        m_rtype = TYPE(ARRAY);
      }
      break;
      
    case TOKEN(ARR_EXPAN):
      {
        int n = asInt(rtype, rval, node);
        if (n < 0) INTERPRET_ERROR(INVALID_ARRAY_SIZE);
        
        cLocalArray* arr = new cLocalArray(n);
        for (int i = 0; i < n; i++) arr->Set(i, ltype.type, lval);
        
        m_rvalue.as_array = arr;
        m_rtype = TYPE(ARRAY);
      }
      break;
    
    case TOKEN(OP_LOGIC_AND):
    case TOKEN(OP_LOGIC_OR):
      {
        bool l = asBool(ltype, lval, node);
        bool r = asBool(rtype, rval, node);
        m_rvalue.as_bool = (node.GetOperator() == TOKEN(OP_LOGIC_AND)) ? (l && r) : (l || r);
        m_rtype = TYPE(BOOL);
      }
      break;
      
    case TOKEN(OP_BIT_AND):
    case TOKEN(OP_BIT_OR):
      {
        sASTypeInfo rettype = node.GetType();
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype.type, rtype.type);

        if (rettype == TYPE(CHAR)) {
          int l = asChar(ltype, lval, node);
          int r = asChar(rtype, rval, node);
          m_rvalue.as_char = (node.GetOperator() == TOKEN(OP_BIT_AND)) ? (l & r) : (l | r);        
          m_rtype = TYPE(CHAR);
        } else if (rettype == TYPE(INT)) {
          int l = asInt(ltype, lval, node);
          int r = asInt(rtype, rval, node);
          m_rvalue.as_int = (node.GetOperator() == TOKEN(OP_BIT_AND)) ? (l & r) : (l | r);
          m_rtype = TYPE(INT);
        } else {
          INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(rettype));
        }
      }
      break;
      
    case TOKEN(OP_EQ):
    case TOKEN(OP_NEQ):
      {
        ASType_t comptype = node.GetCompareType().type;
        
        // Determine the operation type if it is a runtime decision
        if (comptype == TYPE(RUNTIME)) comptype = getRuntimeType(ltype.type, rtype.type);
             
        switch (comptype) {
          case TYPE(BOOL):
            {
              bool l = asBool(ltype, lval, node);
              bool r = asBool(rtype, rval, node);
              m_rvalue.as_bool = (node.GetOperator() == TOKEN(OP_EQ)) ? (l == r) : (l != r);
            }
            break;
            
          case TYPE(CHAR):
          case TYPE(INT):
            // Handle both char and int as integers
            {
              int l = asInt(ltype, lval, node);
              int r = asInt(rtype, rval, node);
              m_rvalue.as_bool = (node.GetOperator() == TOKEN(OP_EQ)) ? (l == r) : (l != r);
            }
            break;
            
          case TYPE(FLOAT):
            {
              double l = asFloat(ltype, lval, node);
              double r = asFloat(rtype, rval, node);
              m_rvalue.as_bool = (node.GetOperator() == TOKEN(OP_EQ)) ? (l == r) : (l != r);
            }
            break;            
            
          case TYPE(STRING):
            {
              cString* l = asString(ltype, lval, node);
              cString* r = asString(rtype, rval, node);
              m_rvalue.as_bool = (node.GetOperator() == TOKEN(OP_EQ)) ? (*l == *r) : (*l != *r);
              delete l;
              delete r;
            }
            break;
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(comptype));
        }
        
        m_rtype = TYPE(BOOL);
      }
      break;

    case TOKEN(OP_LE):
    case TOKEN(OP_GE):
    case TOKEN(OP_LT):
    case TOKEN(OP_GT):
      {
        ASType_t comptype = node.GetCompareType().type;
        
        // Determine the operation type if it is a runtime decision
        if (comptype == TYPE(RUNTIME)) comptype = getRuntimeType(ltype.type, rtype.type);
             
        switch (comptype) {
          case TYPE(CHAR):
          case TYPE(INT):
            // Handle both char and int as integers
            {
              int l = asInt(ltype, lval, node);
              int r = asInt(rtype, rval, node);
              switch (node.GetOperator()) {
                case TOKEN(OP_LE): m_rvalue.as_bool = (l <= r); break;
                case TOKEN(OP_GE): m_rvalue.as_bool = (l >= r); break;
                case TOKEN(OP_LT): m_rvalue.as_bool = (l < r); break;
                case TOKEN(OP_GT): m_rvalue.as_bool = (l > r); break;
                default: INTERPRET_ERROR(INTERNAL);
              }
            }
            break;
            
          case TYPE(FLOAT):
            {
              double l = asFloat(ltype, lval, node);
              double r = asFloat(rtype, rval, node);
              switch (node.GetOperator()) {
                case TOKEN(OP_LE): m_rvalue.as_bool = (l <= r); break;
                case TOKEN(OP_GE): m_rvalue.as_bool = (l >= r); break;
                case TOKEN(OP_LT): m_rvalue.as_bool = (l < r); break;
                case TOKEN(OP_GT): m_rvalue.as_bool = (l > r); break;
                default: INTERPRET_ERROR(INTERNAL);
              }
            }
            break;            
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(comptype));
        }
        
        m_rtype = TYPE(BOOL);
      }
      break;
      
      
    case TOKEN(OP_ADD):
      {
        ASType_t rettype = node.GetType().type;
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype.type, rtype.type, true);
             
        switch (rettype) {
          case TYPE(CHAR):  m_rvalue.as_char = asChar(ltype, lval, node) + asChar(rtype, rval, node); break;
          case TYPE(INT):   m_rvalue.as_int = asInt(ltype, lval, node) + asInt(rtype, rval, node); break;
          case TYPE(FLOAT): m_rvalue.as_float = asFloat(ltype, lval, node) + asFloat(rtype, rval, node); break;            

          case TYPE(STRING):
            {
              cString* l = asString(ltype, lval, node);
              cString* r = asString(rtype, rval, node);
              m_rvalue.as_string = new cString(*l + *r);
              delete l;
              delete r;
            }
            break;
            
          case TYPE(ARRAY):
            {
              cLocalArray* l = asArray(ltype, lval, node);
              cLocalArray* r = asArray(rtype, rval, node);
              m_rvalue.as_array = new cLocalArray(l, r);
              l->RemoveReference();
              r->RemoveReference();
            }
            break;
            
          case TYPE(MATRIX): // @TODO - addition
            INTERPRET_ERROR(INTERNAL);
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(OP_SUB):
      {
        ASType_t rettype = node.GetType().type;
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype.type, rtype.type);
             
        switch (rettype) {
          case TYPE(CHAR):  m_rvalue.as_char = asChar(ltype, lval, node) - asChar(rtype, rval, node); break;
          case TYPE(INT):   m_rvalue.as_int = asInt(ltype, lval, node) - asInt(rtype, rval, node); break;
          case TYPE(FLOAT): m_rvalue.as_float = asFloat(ltype, lval, node) - asFloat(rtype, rval, node); break;            

          case TYPE(MATRIX): // @TODO - subtraction
            INTERPRET_ERROR(INTERNAL);
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(OP_MUL):
      {
        ASType_t rettype = node.GetType().type;
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype.type, rtype.type);
             
        switch (rettype) {
          case TYPE(CHAR):  m_rvalue.as_char = asChar(ltype, lval, node) * asChar(rtype, rval, node); break;
          case TYPE(INT):   m_rvalue.as_int = asInt(ltype, lval, node) * asInt(rtype, rval, node); break;
          case TYPE(FLOAT): m_rvalue.as_float = asFloat(ltype, lval, node) * asFloat(rtype, rval, node); break;            

          case TYPE(MATRIX): // @TODO - multiplication
            INTERPRET_ERROR(INTERNAL);
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(OP_DIV):
      {
        ASType_t rettype = node.GetType().type;
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype.type, rtype.type);
             
        switch (rettype) {
          case TYPE(CHAR):
            {
              char r = asChar(rtype, rval, node);
              if (r == 0) INTERPRET_ERROR(DIVISION_BY_ZERO);
              m_rvalue.as_char = asChar(ltype, lval, node) / r;
            }
            break;
          case TYPE(INT):
            {
              int r = asInt(rtype, rval, node);
              if (r == 0) INTERPRET_ERROR(DIVISION_BY_ZERO);
              m_rvalue.as_int = asInt(ltype, lval, node) / r;
            }
            break;
          case TYPE(FLOAT):
            {
              double r = asFloat(rtype, rval, node);
              if (r == 0.0) INTERPRET_ERROR(DIVISION_BY_ZERO);
              m_rvalue.as_float = asFloat(ltype, lval, node) / r;
            }
            break;            

          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(OP_MOD):
      {
        ASType_t rettype = node.GetType().type;
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype.type, rtype.type);
             
        switch (rettype) {
          case TYPE(CHAR):
            {
              char r = asChar(rtype, rval, node);
              if (r == 0) INTERPRET_ERROR(DIVISION_BY_ZERO);
              m_rvalue.as_char = asChar(ltype, lval, node) % r;
            }
            break;
          case TYPE(INT):
            {
              int r = asInt(rtype, rval, node);
              if (r == 0) INTERPRET_ERROR(DIVISION_BY_ZERO);
              m_rvalue.as_int = asInt(ltype, lval, node) % r;
            }
            break;
          case TYPE(FLOAT):
            {
              double r = asFloat(rtype, rval, node);
              if (r == 0.0) INTERPRET_ERROR(DIVISION_BY_ZERO);
              m_rvalue.as_float = fmod(asFloat(ltype, lval, node), r);
            }
            break;            
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(IDX_OPEN):
      if (m_obj_assign) {
        cObjectRef* obj = lval.as_ref;
        sAggregateValue idx(m_rtype, m_rvalue);
        
        if (obj->GetType() != TYPE(ARRAY))
          INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(IDX_OPEN)), mapType(obj->GetType()));
        
        m_rvalue.as_ref = new cObjectIndexRef(obj, idx);
        m_rtype = TYPE(OBJECT_REF);
      } else {
        
        // @TODO - handle dict indexing
        cLocalArray* arr = asArray(ltype, lval, node);
        int idx = asInt(rtype, rval, node);
        
        if (idx < 0 || idx >= arr->GetSize()) INTERPRET_ERROR(INDEX_OUT_OF_BOUNDS);
        
        const sAggregateValue val = arr->Get(idx);
        m_rtype = val.type;
        m_rvalue = val.value;
      }
      break;
      
    default:
      // Parser should not allow an invalid operator to pass
      INTERPRET_ERROR(INTERNAL);
  }
}


void cDirectInterpretASTVisitor::VisitExpressionUnary(cASTExpressionUnary& node)
{
  node.GetExpression()->Accept(*this);
  
  switch (node.GetOperator()) {
    case TOKEN(OP_BIT_NOT):
      switch (m_rtype.type) {
        case TYPE(CHAR):
          m_rvalue.as_char = ~m_rvalue.as_char;
          break;
        case TYPE(INT):
          m_rvalue.as_int = ~m_rvalue.as_int;
          break;
          
        default:
          INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_BIT_NOT)), mapType(m_rtype));
      }
      break;
    
    case TOKEN(OP_LOGIC_NOT):
      m_rvalue.as_bool = !asBool(m_rtype, m_rvalue, node);
      m_rtype = TYPE(BOOL);
      break;
    
    case TOKEN(OP_SUB):
      switch (m_rtype.type) {
        case TYPE(CHAR):
          m_rvalue.as_char = -m_rvalue.as_char;
          break;
        case TYPE(INT):
          m_rvalue.as_int = -m_rvalue.as_int;
          break;
        case TYPE(FLOAT):
          m_rvalue.as_float = -m_rvalue.as_float;
          break;
          
        default:
          INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_BIT_NOT)), mapType(m_rtype));
      }
      break;
      
    default:
      INTERPRET_ERROR(INTERNAL);
  }
}


void cDirectInterpretASTVisitor::VisitBuiltInCall(cASTBuiltInCall& node)
{
  cASTArgumentList* args = node.GetArguments();
  
  switch (node.GetBuiltIn()) {
    case AS_BUILTIN_CAST_BOOL:
      args->Iterator().Next()->Accept(*this);
      m_rvalue.as_bool = asBool(m_rtype, m_rvalue, node);
      m_rtype = TYPE(BOOL);
      break;

    case AS_BUILTIN_CAST_CHAR:
      args->Iterator().Next()->Accept(*this);
      m_rvalue.as_char = asChar(m_rtype, m_rvalue, node);
      m_rtype = TYPE(CHAR);
      break;
      
    case AS_BUILTIN_CAST_INT:
      args->Iterator().Next()->Accept(*this);
      m_rvalue.as_int = asInt(m_rtype, m_rvalue, node);
      m_rtype = TYPE(INT);
      break;
      
    case AS_BUILTIN_CAST_FLOAT:
      args->Iterator().Next()->Accept(*this);
      m_rvalue.as_float = asFloat(m_rtype, m_rvalue, node);
      m_rtype = TYPE(FLOAT);
      break;
      
    case AS_BUILTIN_CAST_STRING:
      args->Iterator().Next()->Accept(*this);
      m_rvalue.as_string = asString(m_rtype, m_rvalue, node);
      m_rtype = TYPE(STRING);
      break;
      
    case AS_BUILTIN_LEN:
      args->Iterator().Next()->Accept(*this);
      if (m_rtype == TYPE(STRING)) {
        int sz = m_rvalue.as_string->GetSize();
        delete m_rvalue.as_string;
        m_rvalue.as_int = sz;
      } else {
        cLocalArray* arr = asArray(m_rtype, m_rvalue, node);
        m_rvalue.as_int = arr->GetSize();
        arr->RemoveReference();
      }
      m_rtype = TYPE(INT);
      break;
      
    case AS_BUILTIN_RESIZE:
      cASTVariableReference* vr = node.GetVariableReference();
      if (vr->GetType() == TYPE(ARRAY)) {
        int var_idx = (vr->IsVarGlobal() ? 0 : m_sp) + vr->GetVarID();
        
        args->Iterator().Next()->Accept(*this);
        int sz = asInt(m_rtype, m_rvalue, node);
        
        cLocalArray* arr = m_call_stack[var_idx].value.as_array;
        if (arr->IsShared()) {
          arr = new cLocalArray(arr);
          m_call_stack[var_idx].value.as_array->RemoveReference();
          m_call_stack[var_idx].value.as_array = arr;         
        }
        m_call_stack[var_idx].value.as_array->Resize(sz);
      } else {
        // @TODO - resize matrix
      }
      break;
      
    default:
      INTERPRET_ERROR(INTERNAL);
      break;
  }
}


void cDirectInterpretASTVisitor::VisitFunctionCall(cASTFunctionCall& node)
{
  if (node.IsASFunction()) {
    // Call internal function
    const cASFunction* func = node.GetASFunction();
    
    // Setup arguments
    cASFunction::cParameter* args = new cASFunction::cParameter[func->GetArity()];
    tListIterator<cASTNode> cit = node.GetArguments()->Iterator();
    cASTNode* an = NULL;
    for (int i = 0; i < func->GetArity(); i++) {
      an = cit.Next();
      an->Accept(*this);
     
      switch (func->GetArgumentType(i).type) {
        case TYPE(BOOL):        args[i].Set(asBool(m_rtype, m_rvalue, node)); break;
        case TYPE(CHAR):        args[i].Set(asChar(m_rtype, m_rvalue, node)); break;
        case TYPE(FLOAT):       args[i].Set(asFloat(m_rtype, m_rvalue, node)); break;
        case TYPE(INT):         args[i].Set(asInt(m_rtype, m_rvalue, node)); break;
        case TYPE(STRING):      args[i].Set(asString(m_rtype, m_rvalue, node)); break;
          
        default:
          INTERPRET_ERROR(INTERNAL);
      }
    }
    
    // Call the function
    cASFunction::cParameter rvalue = func->Call(args);

    // Handle the return value
    switch (node.GetType().type) {
      case TYPE(BOOL):        m_rvalue.as_bool = rvalue.Get<bool>(); break;
      case TYPE(CHAR):        m_rvalue.as_char = rvalue.Get<char>(); break;
      case TYPE(FLOAT):       m_rvalue.as_float = rvalue.Get<double>(); break;
      case TYPE(INT):         m_rvalue.as_int = rvalue.Get<int>(); break;
      case TYPE(STRING):      m_rvalue.as_string = rvalue.Get<cString*>(); break;
      case TYPE(VOID):        break;
        
      default:
        INTERPRET_ERROR(INTERNAL);
    }
    m_rtype = node.GetType();
    
    // Clean up arguments
    for (int i = 0; i < func->GetArity(); i++) {
      switch (func->GetArgumentType(i).type) {
        case TYPE(BOOL):    break;
        case TYPE(CHAR):    break;
        case TYPE(FLOAT):   break;
        case TYPE(INT):     break;
        case TYPE(STRING):  delete args[i].Get<cString*>(); break;
          
        default:
          INTERPRET_ERROR(INTERNAL);
      }
    }
    delete [] args;
    
  } else {
    // Save previous scope information
    cSymbolTable* prev_symtbl = m_cur_symtbl;
    
    // Get function information
    cSymbolTable* func_src_symtbl = node.IsFuncGlobal() ? m_global_symtbl : m_cur_symtbl;
    int fun_id = node.GetFuncID();
    
    // Set current scope to the function symbol table
    cSymbolTable* func_symtbl = func_src_symtbl->GetFunctionSymbolTable(fun_id);
    int sp = m_sp + prev_symtbl->GetNumVariables();
    m_call_stack.Resize(m_call_stack.GetSize() + func_symtbl->GetNumVariables());
    for (int i = 0; i < func_symtbl->GetNumVariables(); i++) {
      switch (func_symtbl->GetVariableType(i).type) {
        case TYPE(ARRAY):       m_call_stack[sp + i].value.as_array = new cLocalArray; break;
        case TYPE(BOOL):        m_call_stack[sp + i].value.as_bool = false; break;
        case TYPE(CHAR):        m_call_stack[sp + i].value.as_char = 0; break;
        case TYPE(DICT):        m_call_stack[sp + i].value.as_dict = new cLocalDict; break;
        case TYPE(INT):         m_call_stack[sp + i].value.as_int = 0; break;
        case TYPE(FLOAT):       m_call_stack[sp + i].value.as_float = 0.0; break;
        case TYPE(MATRIX):      m_call_stack[sp + i].value.as_matrix = NULL; break;
        case TYPE(OBJECT_REF):  m_call_stack[sp + i].value.as_ref = NULL; break;
        case TYPE(STRING):      m_call_stack[sp + i].value.as_string = NULL; break;
        case TYPE(VAR):         m_call_stack[sp + i].type = TYPE(INVALID); break;
        default: break;
      }
    }
    
    // Process the arguments to the function
    tListIterator<cASTVariableDefinition> sit = func_src_symtbl->GetFunctionSignature(fun_id)->Iterator();
    tListIterator<cASTNode> cit = node.GetArguments()->Iterator();
    cASTVariableDefinition* arg_def = NULL;
    while ((arg_def = sit.Next())) {
      cASTNode* arg = cit.Next();
      if (arg) arg->Accept(*this);
      else arg_def->GetAssignmentExpression()->Accept(*this);
      
      int var_id = arg_def->GetVarID();

      switch (m_cur_symtbl->GetVariableType(var_id).type) {
        case TYPE(ARRAY):       m_call_stack[sp + var_id].value.as_array = asArray(m_rtype, m_rvalue, node); break;
        case TYPE(BOOL):        m_call_stack[sp + var_id].value.as_bool = asBool(m_rtype, m_rvalue, node); break;
        case TYPE(CHAR):        m_call_stack[sp + var_id].value.as_char = asChar(m_rtype, m_rvalue, node); break;
        case TYPE(DICT):        m_call_stack[sp + var_id].value.as_dict = asDict(m_rtype, m_rvalue, node); break;
        case TYPE(FLOAT):       m_call_stack[sp + var_id].value.as_float = asFloat(m_rtype, m_rvalue, node); break;
        case TYPE(INT):         m_call_stack[sp + var_id].value.as_int = asInt(m_rtype, m_rvalue, node); break;
        case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - func call arg assignment
        case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - func call arg assignment
        case TYPE(STRING):
          {
            m_call_stack[sp + var_id].value.as_string = asString(m_rtype, m_rvalue, node);
          }
          break;
          
        case TYPE(VAR):
          m_call_stack[sp + var_id].value = m_rvalue;
          m_call_stack[sp + var_id].type = m_rtype;
          break;
          
        default:
          INTERPRET_ERROR(INTERNAL);
      }
    }
    
    
    // Execute the function
    m_cur_symtbl = func_symtbl;
    m_sp = sp;
    func_src_symtbl->GetFunctionDefinition(fun_id)->Accept(*this);
    
    // Handle function return value
    switch (node.GetType().type) {
      case TYPE(ARRAY):       m_rvalue.as_array = asArray(m_rtype, m_rvalue, node); break;
      case TYPE(BOOL):        m_rvalue.as_bool = asBool(m_rtype, m_rvalue, node); break;
      case TYPE(CHAR):        m_rvalue.as_char = asChar(m_rtype, m_rvalue, node); break;
      case TYPE(DICT):        m_rvalue.as_dict = asDict(m_rtype, m_rvalue, node); break;
      case TYPE(FLOAT):       m_rvalue.as_float = asFloat(m_rtype, m_rvalue, node); break;
      case TYPE(INT):         m_rvalue.as_int = asInt(m_rtype, m_rvalue, node); break;
      case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - return
      case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - return
      case TYPE(STRING):      m_rvalue.as_string = asString(m_rtype, m_rvalue, node); break;
      case TYPE(VAR):         break;
      case TYPE(VOID):        break;
        
      default:
        INTERPRET_ERROR(INTERNAL);
    }
    if (node.GetType() != TYPE(VAR)) m_rtype = node.GetType();

    // Clean up variables in the current scope
    for (int i = 0; i < func_symtbl->GetNumVariables(); i++) {
      ASType_t type = func_symtbl->GetVariableType(i).type;
      if (type == TYPE(VAR)) type = m_call_stack[sp + i].type.type;
      
      switch (type) {
        case TYPE(ARRAY):       m_call_stack[sp + i].value.as_array->RemoveReference(); break;
        case TYPE(DICT):        m_call_stack[sp + i].value.as_dict->RemoveReference(); break;
        case TYPE(MATRIX):      break; // @TODO - cleanup scope
        case TYPE(OBJECT_REF):  delete m_call_stack[sp + i].value.as_ref; break;
        case TYPE(STRING):      delete m_call_stack[sp + i].value.as_string; break;
        default: break;
      }
    }
    
    // Restore previous scope
    m_has_returned = false;
    m_call_stack.Resize(m_call_stack.GetSize() - m_cur_symtbl->GetNumVariables());
    m_sp -= prev_symtbl->GetNumVariables();
    m_cur_symtbl = prev_symtbl;
  }
}


void cDirectInterpretASTVisitor::VisitLiteral(cASTLiteral& node)
{
  switch (node.GetType().type) {
    case TYPE(BOOL):
      if (node.GetValue() == "true") m_rvalue.as_bool = true;
      else m_rvalue.as_bool = false;
      m_rtype = TYPE(BOOL);
      break;
    case TYPE(CHAR):
      m_rvalue.as_char = node.GetValue()[0];
      m_rtype = TYPE(CHAR);
      break;
      case TYPE(INT):
      m_rvalue.as_int = node.GetValue().AsInt();
      m_rtype = TYPE(INT);
      break;
    case TYPE(FLOAT):
      m_rvalue.as_float = node.GetValue().AsDouble();
      m_rtype = TYPE(FLOAT);
      break;
    case TYPE(STRING):
      m_rvalue.as_string = new cString(node.GetValue());
      m_rtype = TYPE(STRING);
      break;
    default:
      INTERPRET_ERROR(INTERNAL);
  }
}


void cDirectInterpretASTVisitor::VisitLiteralArray(cASTLiteralArray& node)
{
  cASTArgumentList* vals = node.GetValues();
  
  if (node.IsMatrix()) {
    // @TODO - literal matrix
  } else {
    cLocalArray* arr = new cLocalArray(vals->GetSize());
    
    tListIterator<cASTNode> it = vals->Iterator();
    cASTNode* val = NULL;
    int i = 0;
    while ((val = it.Next())) {
      val->Accept(*this);
      arr->Set(i++, m_rtype.type, m_rvalue);
    }
    
    m_rvalue.as_array = arr;    
    m_rtype = TYPE(ARRAY);
  }
}

void cDirectInterpretASTVisitor::VisitLiteralDict(cASTLiteralDict& node)
{
  cLocalDict* dict = new cLocalDict();

  sAggregateValue idx,val;
  tListIterator<cASTLiteralDict::sMapping> it(node.Iterator());
  cASTLiteralDict::sMapping* mapping = NULL;
  while ((mapping = it.Next())) {
    mapping->idx->Accept(*this);
    idx.type = m_rtype;
    idx.value = m_rvalue;
    mapping->val->Accept(*this);
    val.type = m_rtype;
    val.value = m_rvalue;
    dict->Set(idx, val);
  }
  
  m_rvalue.as_dict = dict;    
  m_rtype = TYPE(DICT);
}

void cDirectInterpretASTVisitor::VisitObjectCall(cASTObjectCall& node)
{
  // @TODO - handle object call
  INTERPRET_ERROR(INTERNAL);
}

void cDirectInterpretASTVisitor::VisitObjectReference(cASTObjectReference& node)
{
  // @TODO - handle object reference
  INTERPRET_ERROR(INTERNAL);
}

void cDirectInterpretASTVisitor::VisitVariableReference(cASTVariableReference& node)
{
  int var_id = node.GetVarID();
  int sp = node.IsVarGlobal() ? 0 : m_sp;
  
  if (m_obj_assign) {
    switch (node.GetType().type) {
      case TYPE(ARRAY):       m_rvalue.as_ref = new cArrayVarRef(m_call_stack[sp + var_id].value); break;
      case TYPE(DICT):        INTERPRET_ERROR(INTERNAL); // @TODO - var ref object assignment
      case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - var ref object assignment
      case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - var ref object assignment
      case TYPE(STRING):      INTERPRET_ERROR(INTERNAL); // @TODO - var ref object assignment
        
      default:
        INTERPRET_ERROR(INTERNAL);
    }
    m_rtype = TYPE(OBJECT_REF);
  } else {
    ASType_t type = node.GetType().type;
    if (type == TYPE(VAR)) type = m_call_stack[sp + var_id].type.type;
    
    m_rtype = sASTypeInfo(type);
    
    switch (type) {
      case TYPE(ARRAY):       m_rvalue.as_array = m_call_stack[sp + var_id].value.as_array->GetReference(); break;
      case TYPE(BOOL):        m_rvalue.as_bool = m_call_stack[sp + var_id].value.as_bool; break;
      case TYPE(CHAR):        m_rvalue.as_char = m_call_stack[sp + var_id].value.as_char; break;
      case TYPE(DICT):        m_rvalue.as_dict = m_call_stack[sp + var_id].value.as_dict->GetReference(); break;
      case TYPE(FLOAT):       m_rvalue.as_float = m_call_stack[sp + var_id].value.as_float; break;
      case TYPE(INT):         m_rvalue.as_int = m_call_stack[sp + var_id].value.as_int; break;
      case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - var ref assignment
      case TYPE(STRING):      m_rvalue.as_string = new cString(*m_call_stack[sp + var_id].value.as_string); break;

      
      case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - var ref assignment
        
      default:
        INTERPRET_ERROR(INTERNAL);
    }
  }
}


void cDirectInterpretASTVisitor::VisitUnpackTarget(cASTUnpackTarget& node)
{
  node.GetExpression()->Accept(*this);
  cLocalArray* arr = m_rvalue.as_array;
  
  int unpack_size = node.GetSize();
  if (node.IsLastNamed()) unpack_size--;
  if (unpack_size > arr->GetSize()) INTERPRET_ERROR(UNPACK_VALUE_TOO_SMALL);
  if (unpack_size < arr->GetSize() && !node.IsLastWild()) INTERPRET_ERROR(UNPACK_VALUE_TOO_LARGE);
  
  // Unpack the values up to the last non-wild value
  for (int i = 0; i < unpack_size; i++) {
    sASTypeInfo var_type = (node.IsVarGlobal(i) ? m_global_symtbl : m_cur_symtbl)->GetVariableType(node.GetVarID(i));
    int var_idx = (node.IsVarGlobal(i) ? 0 : m_sp) + node.GetVarID(i);
    
    // Set the variable value for this iteration
    const sAggregateValue& val = arr->Get(i);
    switch (var_type.type) {
      case TYPE(BOOL):        m_call_stack[var_idx].value.as_bool = asBool(val.type, val.value, node); break;
      case TYPE(CHAR):        m_call_stack[var_idx].value.as_char = asChar(val.type, val.value, node); break;
      case TYPE(FLOAT):       m_call_stack[var_idx].value.as_float = asFloat(val.type, val.value, node); break;
      case TYPE(INT):         m_call_stack[var_idx].value.as_int = asInt(val.type, val.value, node); break;
      case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - assignment
        
      case TYPE(VAR):
        m_call_stack[var_idx].value = val.value;
        m_call_stack[var_idx].type = val.type;
        break;
        
      case TYPE(ARRAY):
        m_call_stack[var_idx].value.as_array->RemoveReference();
        m_call_stack[var_idx].value.as_array = asArray(val.type, val.value, node);
        break;
        
      case TYPE(DICT):
        m_call_stack[var_idx].value.as_dict->RemoveReference();
        m_call_stack[var_idx].value.as_dict = asDict(val.type, val.value, node);
        break;
        
        
      case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - assignment
        
      case TYPE(STRING):
        delete m_call_stack[var_idx].value.as_string;
        m_call_stack[var_idx].value.as_string = asString(val.type, val.value, node);
        break;
        
      default:
        INTERPRET_ERROR(INTERNAL);
    }
  }
  
  if (node.IsLastNamed()) {
    int var_idx = (node.IsVarGlobal(unpack_size) ? 0 : m_sp) + node.GetVarID(unpack_size);
    int sz = arr->GetSize() - unpack_size;
    
    cLocalArray* wild = new cLocalArray(sz);
    
    for (int i = 0; i < sz; i++) {
      const sAggregateValue& val = arr->Get(i + unpack_size);
      wild->Set(i, val.type, val.value);
    }
    
    m_call_stack[var_idx].value.as_array->RemoveReference();
    m_call_stack[var_idx].value.as_array = wild;
  }
  
  arr->RemoveReference();
}

cDirectInterpretASTVisitor::cLocalArray* cDirectInterpretASTVisitor::asArray(const sASTypeInfo& type, uAnyType value, cASTNode& node)
{
  switch (type.type) {
    case TYPE(ARRAY):
      return value.as_array;

    case TYPE(MATRIX): // @TODO - asArray 
      INTERPRET_ERROR(INTERNAL);
    
    case TYPE(STRING):
      {
        cString* str = value.as_string;
        cLocalArray* arr = new cLocalArray(str->GetSize());
        for (int i = 0; i < str->GetSize(); i++) {
          uAnyType val;
          val.as_char = (*str)[i];
          arr->Set(i, TYPE(CHAR), val);
        }
        delete str;
        return arr;
      }
      
    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(CHAR)));
  }
  
  return false;
}

bool cDirectInterpretASTVisitor::asBool(const sASTypeInfo& type, uAnyType value, cASTNode& node)
{
  switch (type.type) {
    case TYPE(ARRAY):
      {
        bool rval = (value.as_array->GetSize());
        value.as_array->RemoveReference();
        return rval;
      }
      
    case TYPE(DICT):
    {
      bool rval = (value.as_dict->GetSize());
      value.as_dict->RemoveReference();
      return rval;
    }
      
    case TYPE(MATRIX): // @TODO - implement asBool
      INTERPRET_ERROR(INTERNAL);
      
    case TYPE(BOOL):
      return value.as_bool;
    case TYPE(CHAR):
      return (value.as_char);
    case TYPE(FLOAT):
      return (value.as_float != 0);
    case TYPE(INT):
      return (value.as_int);
    case TYPE(STRING):
      {
        bool rval = (*value.as_string != "");
        delete value.as_string;
        return rval;
      }
      

    case TYPE(OBJECT_REF): // @TODO - implement asBool
      INTERPRET_ERROR(INTERNAL);

    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(BOOL)));
  }
  
  return false;
}


char cDirectInterpretASTVisitor::asChar(const sASTypeInfo& type, uAnyType value, cASTNode& node)
{
  switch (type.type) {
    case TYPE(BOOL):
      return (value.as_bool) ? 1 : 0;
    case TYPE(CHAR):
      return value.as_char;
    case TYPE(INT):
      return (char)value.as_int;

    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(CHAR)));
  }
  
  return 0;
}


cDirectInterpretASTVisitor::cLocalDict* cDirectInterpretASTVisitor::asDict(const sASTypeInfo& type, uAnyType value, cASTNode& node)
{
  switch (type.type) {
    case TYPE(DICT):
      return value.as_dict;
      
    case TYPE(ARRAY): // @TODO - asDict
      INTERPRET_ERROR(INTERNAL);
      
    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(CHAR)));
  }
  
  return false;
}

int cDirectInterpretASTVisitor::asInt(const sASTypeInfo& type, uAnyType value, cASTNode& node)
{
  switch (type.type) {
    case TYPE(BOOL):
      return (value.as_bool) ? 1 : 0;
    case TYPE(CHAR):
      return (int)value.as_char;
    case TYPE(INT):
      return value.as_int;
    case TYPE(FLOAT):
      return (int)value.as_float;
    case TYPE(STRING):
      {
        int rval = value.as_string->AsInt();
        delete value.as_string;
        return rval;
      }
      
    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(INT)));
  }
  
  return 0;
}


double cDirectInterpretASTVisitor::asFloat(const sASTypeInfo& type, uAnyType value, cASTNode& node)
{
  switch (type.type) {
    case TYPE(BOOL):
      return (value.as_bool) ? 1.0 : 0.0;
    case TYPE(CHAR):
      return (double)value.as_char;
    case TYPE(INT):
      return (double)value.as_int;
    case TYPE(FLOAT):
      return value.as_float;
    case TYPE(STRING):
      {
        double rval = value.as_string->AsDouble();
        delete value.as_string;
        return rval;
      }
      
    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(INT)));
  }
  
  return 0.0;
}


cString* cDirectInterpretASTVisitor::asString(const sASTypeInfo& type, uAnyType value, cASTNode& node)
{
  switch (type.type) {
    case TYPE(BOOL):        return new cString(cStringUtil::Convert(value.as_bool));
    case TYPE(CHAR):        { cString* str = new cString(1); (*str)[0] = value.as_char; return str; }
    case TYPE(INT):         return new cString(cStringUtil::Convert(value.as_int));
    case TYPE(FLOAT):       return new cString(cStringUtil::Convert(value.as_float));
    case TYPE(STRING):      return value.as_string;
      
    case TYPE(ARRAY):
      {
        cString* str = new cString(cStringUtil::Stringf("< array(%d) >", value.as_array->GetSize()));
        value.as_array->RemoveReference();
        return str;
      }

    case TYPE(DICT):
    {
      cString* str = new cString(cStringUtil::Stringf("< dict(%d) >", value.as_dict->GetSize()));
      value.as_dict->RemoveReference();
      return str;
    }
      
    case TYPE(MATRIX): // @TODO - as string
      {
        cString* str = new cString(cStringUtil::Stringf("< matrix(,) >"));
        return str;
      }

    case TYPE(OBJECT_REF): // @TODO - as string
    {
      cString* str = new cString(cStringUtil::Stringf("< object >"));
      return str;
    }
      
    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(INT)));
  }
  
  return NULL;
}



ASType_t cDirectInterpretASTVisitor::getRuntimeType(ASType_t ltype, ASType_t rtype, bool allow_str)
{
  switch (ltype) {
    case TYPE(ARRAY):
      return TYPE(ARRAY);
    case TYPE(BOOL):
      switch (rtype) {
        case TYPE(ARRAY):
        case TYPE(BOOL):
        case TYPE(CHAR):
        case TYPE(FLOAT):
        case TYPE(INT):
        case TYPE(MATRIX):
        case TYPE(OBJECT_REF):
        case TYPE(STRING):
          return TYPE(BOOL);
          
        default: break;
      }
      break;
    case TYPE(CHAR):
      switch (rtype) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(CHAR);
        case TYPE(CHAR):      return TYPE(CHAR);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(INT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    if (allow_str) return TYPE(STRING); break;
        default: break;
      }
      break;
    case TYPE(DICT):
      return TYPE(DICT);
    case TYPE(FLOAT):
      switch (rtype) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(FLOAT);
        case TYPE(CHAR):      return TYPE(FLOAT);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(FLOAT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    if (allow_str) return TYPE(FLOAT); break;
        default: break;
      }
      break;
    case TYPE(INT):
      switch (rtype) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(INT);
        case TYPE(CHAR):      return TYPE(INT);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(INT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    if (allow_str) return TYPE(INT); break;
        default: break;
      }
      break;
    case TYPE(MATRIX):
      return TYPE(MATRIX);
    case TYPE(STRING):
      if (allow_str) return TYPE(STRING); break;
      
    default: break;
  }
  
  return TYPE(INVALID);
}


void cDirectInterpretASTVisitor::sAggregateValue::Cleanup()
{
  switch (type.type) {
    case TYPE(ARRAY):       value.as_array->RemoveReference(); break;
    case TYPE(DICT):        value.as_dict->RemoveReference(); break;
    case TYPE(MATRIX):      break; // @TODO - aggregate value cleanup
    case TYPE(OBJECT_REF):  delete value.as_ref; break;
    case TYPE(STRING):      delete value.as_string; break;
    default: break;
  }
}

bool cDirectInterpretASTVisitor::sAggregateValue::operator==(const sAggregateValue& lval)
{
  if (type == lval.type) {
    switch (type.type) {
      case TYPE(BOOL):    return value.as_bool == lval.value.as_bool;
      case TYPE(CHAR):    return value.as_char == lval.value.as_char;
      case TYPE(INT):     return value.as_int == lval.value.as_int;
      case TYPE(FLOAT):   return value.as_float == lval.value.as_float;
      case TYPE(STRING):  return *value.as_string == *lval.value.as_string;
      case TYPE(ARRAY):   return value.as_array == lval.value.as_array;
      case TYPE(DICT):    return value.as_dict == lval.value.as_dict;
        
      case TYPE(MATRIX): // @TODO - aggregate value compare
      case TYPE(OBJECT_REF): // @TODO - aggregate value compare
        return value.as_void == lval.value.as_void;
      
      default:
        break;
    }
  }
  
  return false;
}

void cDirectInterpretASTVisitor::cLocalArray::Set(int i, const sASTypeInfo& type, uAnyType value)
{
  m_storage[i].Cleanup();
  
  m_storage[i].type = type;

  switch (type.type) {
    case TYPE(BOOL):
    case TYPE(CHAR):
    case TYPE(INT):
    case TYPE(FLOAT):
      m_storage[i].value = value;
      break;
      
    case TYPE(STRING):
      m_storage[i].value.as_string = new cString(*value.as_string);
      break;
    
    case TYPE(ARRAY):
      m_storage[i].value.as_array = value.as_array->GetReference();
      break;
      
    case TYPE(DICT):
      m_storage[i].value.as_dict = value.as_dict->GetReference();
      break;
      
    case TYPE(MATRIX): // @TODO - array set
      break;
      
    default: break;
  }
  
}

void cDirectInterpretASTVisitor::cLocalArray::copy(int offset, tArray<sAggregateValue>& in_storage)
{
  for (int i = 0; i < in_storage.GetSize(); i++) {
    m_storage[i + offset].type = in_storage[i].type;
    switch (in_storage[i].type.type) {
      case TYPE(BOOL):    m_storage[i + offset].value.as_bool = in_storage[i].value.as_bool; break;
      case TYPE(CHAR):    m_storage[i + offset].value.as_char = in_storage[i].value.as_char; break;
      case TYPE(INT):     m_storage[i + offset].value.as_int = in_storage[i].value.as_int; break;
      case TYPE(FLOAT):   m_storage[i + offset].value.as_float = in_storage[i].value.as_float; break;
      case TYPE(STRING):  m_storage[i + offset].value.as_string = new cString(*in_storage[i].value.as_string); break;
      case TYPE(ARRAY):   m_storage[i + offset].value.as_array = in_storage[i].value.as_array->GetReference(); break;
      case TYPE(DICT):    m_storage[i + offset].value.as_dict = in_storage[i].value.as_dict->GetReference(); break;
      case TYPE(MATRIX):  // @TODO - array copy
        break;
        
      default: break;
    }
  }
}


void cDirectInterpretASTVisitor::cLocalArray::Resize(int sz)
{
  int o_sz = m_storage.GetSize();
  
  m_storage.Resize(sz);
  
  for (int i = o_sz; i < sz; i++) {
    m_storage[i].value.as_int = 0;
    m_storage[i].type = TYPE(INT);
  }  
}

cDirectInterpretASTVisitor::cLocalArray::~cLocalArray()
{
  int sz = m_storage.GetSize();
  
  // Cleanup values stored in the array
  for (int i = 0; i < sz; i++) m_storage[i].Cleanup();
}


void cDirectInterpretASTVisitor::cLocalDict::Set(const sAggregateValue& idx, const sAggregateValue& val)
{
  sAggregateValue o_val;
  if (m_storage.Find(idx, o_val)) o_val.Cleanup();
  
  m_storage.SetValue(idx, val);
}


cDirectInterpretASTVisitor::cLocalDict::~cLocalDict()
{
  tList<sAggregateValue> keys;
  tList<sAggregateValue> vals;
  
  sAggregateValue* av = NULL;
  
  tListIterator<sAggregateValue> kit(keys);
  while ((av = kit.Next())) av->Cleanup();

  tListIterator<sAggregateValue> vit(vals);
  while ((av = vit.Next())) av->Cleanup();
}


sASTypeInfo cDirectInterpretASTVisitor::cArrayVarRef::GetType(const sAggregateValue& idx)
{
  cLocalArray* arr = m_var.as_array;

  int idxi = -1;
  switch (idx.type.type) {
    case TYPE(BOOL):    idxi = (idx.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):    idxi = (int)idx.value.as_char; break;
    case TYPE(INT):     idxi = idx.value.as_int; break;
    case TYPE(FLOAT):   idxi = (int)idx.value.as_float; break;
    case TYPE(STRING):  idxi = idx.value.as_string->AsInt(); delete idx.value.as_string; break;
    default: break;
  } 
  if (idxi < 0 || idxi >= arr->GetSize()) return TYPE(INVALID);

  ASType_t type = m_var.as_array->Get(idxi).type.type;
  if (type == TYPE(OBJECT_REF)) return m_var.as_array->Get(idxi).value.as_ref->GetType();
  else return type;
}

cDirectInterpretASTVisitor::uAnyType cDirectInterpretASTVisitor::cArrayVarRef::Get(const sAggregateValue& idx)
{
  int idxi = -1;
  switch (idx.type.type) {
    case TYPE(BOOL):    idxi = (idx.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):    idxi = (int)idx.value.as_char; break;
    case TYPE(INT):     idxi = idx.value.as_int; break;
    case TYPE(FLOAT):   idxi = (int)idx.value.as_float; break;
    case TYPE(STRING):  idxi = idx.value.as_string->AsInt(); delete idx.value.as_string; break;
    default: break;
  } 

  assert(idxi >= 0 && idxi < m_var.as_array->GetSize());
  
  return m_var.as_array->Get(idxi).value;
}

bool cDirectInterpretASTVisitor::cArrayVarRef::Set(const sAggregateValue& idx, ASType_t type, uAnyType value)
{
  assert(idx.type.type == AS_TYPE_INT);
  int idxi = idx.value.as_int;
  
  cLocalArray* arr = m_var.as_array;
  if (idxi < 0 || idxi >= arr->GetSize()) return false;

  if (arr->IsShared()) {
    arr = new cLocalArray(arr);
    m_var.as_array->RemoveReference();
    m_var.as_array = arr;
  }
  
  arr->Set(idxi, type, value);
  
  return true;
}


sASTypeInfo cDirectInterpretASTVisitor::cObjectIndexRef::GetType(const sAggregateValue& idx)
{
  switch (m_obj->GetType(m_idx).type) {
    case TYPE(ARRAY):
      {
        cLocalArray* arr = m_obj->Get(m_idx).as_array;
        
        int idxi = -1;
        switch (idx.type.type) {
          case TYPE(BOOL):    idxi = (idx.value.as_bool) ? 1 : 0; break;
          case TYPE(CHAR):    idxi = (int)idx.value.as_char; break;
          case TYPE(INT):     idxi = idx.value.as_int; break;
          case TYPE(FLOAT):   idxi = (int)idx.value.as_float; break;
          case TYPE(STRING):  idxi = idx.value.as_string->AsInt(); delete idx.value.as_string; break;
          default: break;
        } 
        if (idxi < 0 || idxi >= arr->GetSize()) return TYPE(INVALID);
        
        return arr->Get(idxi).type;
      }
      
    case TYPE(DICT):
      {
        cLocalDict* dict = m_obj->Get(m_idx).as_dict;
        sAggregateValue val;
        if (dict->Get(idx, val)) return val.type;
      }
      return TYPE(INVALID);
      
    case TYPE(MATRIX):
      return TYPE(ARRAY);
      
    default:
      return TYPE(INVALID);
  }  
}


cDirectInterpretASTVisitor::uAnyType cDirectInterpretASTVisitor::cObjectIndexRef::Get(const sAggregateValue& idx)
{
  switch (m_obj->GetType(m_idx).type) {
    case TYPE(ARRAY):
      {
        cLocalArray* arr = m_obj->Get(m_idx).as_array;
        
        int idxi = -1;
        switch (idx.type.type) {
          case TYPE(BOOL):    idxi = (idx.value.as_bool) ? 1 : 0; break;
          case TYPE(CHAR):    idxi = (int)idx.value.as_char; break;
          case TYPE(INT):     idxi = idx.value.as_int; break;
          case TYPE(FLOAT):   idxi = (int)idx.value.as_float; break;
          case TYPE(STRING):  idxi = idx.value.as_string->AsInt(); delete idx.value.as_string; break;
          default: break;
        }      
        assert(idxi >= 0 || idxi < arr->GetSize());
        return arr->Get(idxi).value;
      }
      
    case TYPE(DICT):
      {
        cLocalDict* dict = m_obj->Get(m_idx).as_dict;
        sAggregateValue val;
        if (dict->Get(idx, val)) return val.value;
      }
      break;
      
    case TYPE(MATRIX): // @TODO - object index ref get
      break;
      
    default:
      break;
  }  

  Avida::Exit(AS_EXIT_FAIL_INTERPRET);
  
  uAnyType novalue;
  return novalue;
}


bool cDirectInterpretASTVisitor::cObjectIndexRef::Set(const sAggregateValue& idx, ASType_t type, uAnyType value)
{
  switch (m_obj->GetType(m_idx).type) {
    case TYPE(ARRAY):
      {
        cLocalArray* arr = m_obj->Get(m_idx).as_array;
        
        int idxi = -1;
        switch (idx.type.type) {
          case TYPE(BOOL):    idxi = (idx.value.as_bool) ? 1 : 0; break;
          case TYPE(CHAR):    idxi = (int)idx.value.as_char; break;
          case TYPE(INT):     idxi = idx.value.as_int; break;
          case TYPE(FLOAT):   idxi = (int)idx.value.as_float; break;
          case TYPE(STRING):  idxi = value.as_string->AsInt(); delete value.as_string; break;
          default: break;
        }      
        if (idxi < 0 || idxi >= arr->GetSize()) return false;
        
        arr->Set(idxi, type, value);
      }
      return true;
      
    case TYPE(DICT):
      {
        cLocalDict* dict = m_obj->Get(m_idx).as_dict;
        sAggregateValue val(type, value);
        dict->Set(idx, val);
      }
      return true;
      
    case TYPE(MATRIX): // @TODO - object index ref set
      break;
      
    default:
      break;
  }  
  
  return false;
}


void cDirectInterpretASTVisitor::reportError(ASDirectInterpretError_t err, const cASFilePosition& fp, const int line, ...)
{
#if DEBUG_AS_DIRECT_INTERPRET
# define ERR_ENDL "  (cDirectInterpretASTVisitor.cc:" << line << ")" << std::endl
#else
# define ERR_ENDL std::endl
#endif
  
#define VA_ARG_STR va_arg(vargs, const char*)

  std::cerr << fp.GetFilename() << ":" << fp.GetLineNumber() << ": error: ";
  
  va_list vargs;
  va_start(vargs, line);
  switch (err) {
    case AS_DIRECT_INTERPRET_ERR_DIVISION_BY_ZERO:
      std::cerr << "division by zero" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_INDEX_OUT_OF_BOUNDS:
      std::cerr << "array index out of bounds" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_INVALID_ARRAY_SIZE:
      std::cerr << "invalid array dimension" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_OBJECT_ASSIGN_FAIL:
      std::cerr << "aggregate assignment failed" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_TYPE_CAST:
      {
        const char* type1 = VA_ARG_STR;
        const char* type2 = VA_ARG_STR;
        std::cerr << "cannot convert '" << type1 << "' to '" << type2 << "'" << ERR_ENDL;
      }
      break;
    case AS_DIRECT_INTERPRET_ERR_UNDEFINED_TYPE_OP:
      {
        const char* op = VA_ARG_STR;
        const char* type = VA_ARG_STR;
        std::cerr << "'" << op << "' operation undefined for type '" << type << "'" << ERR_ENDL;
      }
      break;
    case AS_DIRECT_INTERPRET_ERR_UNPACK_VALUE_TOO_LARGE:
      std::cerr << "unpack value too large" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_UNPACK_VALUE_TOO_SMALL:
      std::cerr << "unpack value too small" << ERR_ENDL;
      break;
      
    case AS_DIRECT_INTERPRET_ERR_INTERNAL:
      std::cerr << "internal interpreter error at cDirectInterpretASTVisitor.cc:" << line << std::endl;
      break;
    case AS_DIRECT_INTERPRET_ERR_UNKNOWN:
    default:
      std::cerr << "unknown error" << std::endl;
  }
  va_end(vargs);
  
  Avida::Exit(AS_EXIT_FAIL_INTERPRET);
  
#undef ERR_ENDL
#undef VA_ARG_STR
}

#undef INTERPRET_ERROR()
#undef TOKEN()
#undef TYPE()
