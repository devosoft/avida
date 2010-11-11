/*
 *  cDirectInterpretASTVisitor.cc
 *  Avida
 *
 *  Created by David on 3/4/08.
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

#include "cDirectInterpretASTVisitor.h"

#include <cassert>
#include <cmath>

#include "Avida.h"
#include "AvidaScript.h"

#include "cASFunction.h"
#include "cStringUtil.h"
#include "cSymbolTable.h"

#include "tMatrix.h"

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
      case TYPE(STRING):      m_call_stack[i].value.as_string = NULL; break;

      case TYPE(OBJECT_REF):
        m_call_stack[i].value.as_ref = NULL;
        m_call_stack[i].type.info = m_global_symtbl->GetVariableType(i).info;
        break;
      
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
      case TYPE(MATRIX):      m_call_stack[i].value.as_matrix->RemoveReference(); break;
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
    
    case TYPE(OBJECT_REF):
      m_call_stack[sp + var_id].value.as_nobj->RemoveReference();
      m_call_stack[sp + var_id].value.as_nobj = asNativeObject(symtbl->GetVariableType(var_id).info, m_rtype, m_rvalue, node);

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
      
      
    case TYPE(MATRIX):
      m_call_stack[sp + var_id].value.as_matrix->RemoveReference();
      m_call_stack[sp + var_id].value.as_matrix = asMatrix(m_rtype, m_rvalue, node);
      break;
      
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
  
  sAggregateValue val(m_rtype, m_rvalue);
  if (!obj->Set(val)) {
    val.Cleanup();
    INTERPRET_ERROR(OBJECT_ASSIGN_FAIL);
  }
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
  cLocalArray* arr = asArray(m_rtype, m_rvalue, node);

  int var_idx = m_sp + var_id;
  for (int i = 0; i < arr->GetSize(); i++) {
    // Set the variable value for this iteration
    const sAggregateValue& val = arr->Get(i);
    switch (var_type.type) {
      case TYPE(BOOL):        m_call_stack[var_idx].value.as_bool = asBool(val.type, val.value, node); break;
      case TYPE(CHAR):        m_call_stack[var_idx].value.as_char = asChar(val.type, val.value, node); break;
      case TYPE(FLOAT):       m_call_stack[var_idx].value.as_float = asFloat(val.type, val.value, node); break;
      case TYPE(INT):         m_call_stack[var_idx].value.as_int = asInt(val.type, val.value, node); break;
      case TYPE(OBJECT_REF):
        m_call_stack[var_idx].value.as_nobj->RemoveReference();
        m_call_stack[var_idx].value.as_nobj = asNativeObject(var_type.info, val.type, val.value, node);
        break;
        
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
        
      case TYPE(MATRIX):
        m_call_stack[var_idx].value.as_matrix->RemoveReference();
        m_call_stack[var_idx].value.as_matrix = asMatrix(val.type, val.value, node);
        break;
        
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
      case TYPE(OBJECT_REF):  m_call_stack[m_sp + var_id].value.as_nobj = asNativeObject(node.GetType().info, m_rtype, m_rvalue, node); break;
      case TYPE(MATRIX):      m_call_stack[m_sp + var_id].value.as_matrix = asMatrix(m_rtype, m_rvalue, node); break;
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
    } else if (node.GetType() == TYPE(MATRIX)) {
      tListIterator<cASTNode> it = node.GetDimensions()->Iterator();
      it.Next()->Accept(*this);
      int sz_x = asInt(m_rtype, m_rvalue, node);
      it.Next()->Accept(*this);
      int sz_y = asInt(m_rtype, m_rvalue, node);
      
      cLocalMatrix* mat = new cLocalMatrix();
      mat->Resize(sz_x, sz_y);
      m_call_stack[m_sp + node.GetVarID()].value.as_matrix = mat;      
    } else {
      INTERPRET_ERROR(INTERNAL);
    }
  } else if (node.GetType().type == TYPE(OBJECT_REF)) {
    // @AS_TODO - set native object ref to special NULL value
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
          case TYPE(CHAR):    m_rvalue.as_char = asChar(ltype, lval, node) + asChar(rtype, rval, node); break;
          case TYPE(INT):     m_rvalue.as_int = asInt(ltype, lval, node) + asInt(rtype, rval, node); break;
          case TYPE(FLOAT):   m_rvalue.as_float = asFloat(ltype, lval, node) + asFloat(rtype, rval, node); break;            
          case TYPE(MATRIX):  matrixAdd(asMatrix(ltype, lval, node), asMatrix(rtype, rval, node), node); break;
            
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
          case TYPE(CHAR):    m_rvalue.as_char = asChar(ltype, lval, node) - asChar(rtype, rval, node); break;
          case TYPE(INT):     m_rvalue.as_int = asInt(ltype, lval, node) - asInt(rtype, rval, node); break;
          case TYPE(FLOAT):   m_rvalue.as_float = asFloat(ltype, lval, node) - asFloat(rtype, rval, node); break;            
          case TYPE(MATRIX):  matrixSubtract(asMatrix(ltype, lval, node), asMatrix(rtype, rval, node), node); break;
                        
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
          case TYPE(CHAR):    m_rvalue.as_char = asChar(ltype, lval, node) * asChar(rtype, rval, node); break;
          case TYPE(INT):     m_rvalue.as_int = asInt(ltype, lval, node) * asInt(rtype, rval, node); break;
          case TYPE(FLOAT):   m_rvalue.as_float = asFloat(ltype, lval, node) * asFloat(rtype, rval, node); break;            
          case TYPE(MATRIX):  matrixMultiply(asMatrix(ltype, lval, node), asMatrix(rtype, rval, node), node); break;
            
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
        sAggregateValue idx(rtype, rval);
        sAggregateValue o_val;
        
        if (!obj->Get(o_val)) {
          idx.Cleanup();
          INTERPRET_ERROR(INDEX_ERROR);
        }
        
        switch (o_val.type.type) {
          case TYPE(ARRAY):
          case TYPE(DICT):
          case TYPE(MATRIX):
          case TYPE(STRING):
            break;
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(IDX_OPEN)), mapType(o_val.type));
        }
        
        m_rvalue.as_ref = new cObjectIndexRef(obj, idx);
        m_rtype = TYPE(OBJECT_REF);
      } else {
        
        switch (ltype.type) {
          case TYPE(ARRAY):
            {
              cLocalArray* arr = lval.as_array;
              int idx = asInt(rtype, rval, node);
              
              if (idx < 0 || idx >= arr->GetSize()) INTERPRET_ERROR(INDEX_OUT_OF_BOUNDS);
              
              const sAggregateValue val = arr->Get(idx);
              m_rtype = val.type;
              m_rvalue = val.value;
              
              arr->RemoveReference();
            }
            break;
            
          case TYPE(DICT):
            {
              cLocalDict* dict = lval.as_dict;
              sAggregateValue idx(rtype, rval);
              
              sAggregateValue val;
              if (!dict->Get(idx, val)) {
                idx.Cleanup();
                INTERPRET_ERROR(KEY_NOT_FOUND);
              }
              
              idx.Cleanup();

              m_rtype = val.type;
              m_rvalue = val.value;
              
              dict->RemoveReference();
            }
            break;
            
          case TYPE(MATRIX):
            {
              cLocalMatrix* mat = lval.as_matrix;
              int idx = asInt(rtype, rval, node);
              
              if (idx < 0 || idx >= mat->GetNumRows()) INTERPRET_ERROR(INDEX_OUT_OF_BOUNDS);
              
              m_rtype = TYPE(ARRAY);
              m_rvalue.as_array = mat->GetRow(idx);
              
              mat->RemoveReference();
            }
            break;
            
          case TYPE(STRING):
            {
              cString* str = lval.as_string;
              int idx = asInt(rtype, rval, node);
              if (idx < 0 || idx >= str->GetSize()) INTERPRET_ERROR(INDEX_OUT_OF_BOUNDS);

              m_rtype = TYPE(CHAR);
              m_rvalue.as_char = (*str)[idx];
              delete str;
            }
            break;
            
          default:
            INTERPRET_ERROR(TYPE_CAST, mapType(ltype.type), mapType(TYPE(ARRAY)));
        }
        
        
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
  cASTNode* trgt = node.GetTarget();
  
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
      
      
    case AS_BUILTIN_IS_ARRAY:
      args->Iterator().Next()->Accept(*this);
      {
        sAggregateValue val(m_rtype, m_rvalue);
        val.Cleanup();
      }
      m_rvalue.as_bool = m_rtype.type == TYPE(ARRAY);
      m_rtype = TYPE(BOOL);
      break;
      
    case AS_BUILTIN_IS_BOOL:
      args->Iterator().Next()->Accept(*this);
      {
        sAggregateValue val(m_rtype, m_rvalue);
        val.Cleanup();
      }
      m_rvalue.as_bool = m_rtype.type == TYPE(BOOL);
      m_rtype = TYPE(BOOL);
      break;
      
    case AS_BUILTIN_IS_CHAR:
      args->Iterator().Next()->Accept(*this);
      {
        sAggregateValue val(m_rtype, m_rvalue);
        val.Cleanup();
      }
      m_rvalue.as_bool = m_rtype.type == TYPE(CHAR);
      m_rtype = TYPE(BOOL);
      break;
      
    case AS_BUILTIN_IS_DICT:
      args->Iterator().Next()->Accept(*this);
      {
        sAggregateValue val(m_rtype, m_rvalue);
        val.Cleanup();
      }
      m_rvalue.as_bool = m_rtype.type == TYPE(DICT);
      m_rtype = TYPE(BOOL);
      break;
      
    case AS_BUILTIN_IS_INT:
      args->Iterator().Next()->Accept(*this);
      {
        sAggregateValue val(m_rtype, m_rvalue);
        val.Cleanup();
      }
      m_rvalue.as_bool = m_rtype.type == TYPE(INT);
      m_rtype = TYPE(BOOL);
      break;
      
    case AS_BUILTIN_IS_FLOAT:
      args->Iterator().Next()->Accept(*this);
      {
        sAggregateValue val(m_rtype, m_rvalue);
        val.Cleanup();
      }
      m_rvalue.as_bool = m_rtype.type == TYPE(FLOAT);
      m_rtype = TYPE(BOOL);
      break;
      
    case AS_BUILTIN_IS_MATRIX:
      args->Iterator().Next()->Accept(*this);
      {
        sAggregateValue val(m_rtype, m_rvalue);
        val.Cleanup();
      }
      m_rvalue.as_bool = m_rtype.type == TYPE(MATRIX);
      m_rtype = TYPE(BOOL);
      break;
      
    case AS_BUILTIN_IS_STRING:
      args->Iterator().Next()->Accept(*this);
      {
        sAggregateValue val(m_rtype, m_rvalue);
        val.Cleanup();
      }
      m_rvalue.as_bool = m_rtype.type == TYPE(STRING);
      m_rtype = TYPE(BOOL);
      break;
      
      
    case AS_BUILTIN_CLEAR:
      trgt->Accept(*this);
      if (m_rtype.type == TYPE(DICT)) {
        m_rvalue.as_dict->Clear();
        m_rvalue.as_dict->RemoveReference();
      } else if (m_rtype.type == TYPE(MATRIX)) {
        m_rvalue.as_matrix->Resize(0, 0);
        m_rvalue.as_matrix->RemoveReference();
      } else if (m_rtype.type == TYPE(ARRAY)) {
        m_rvalue.as_array->Resize(0);
        m_rvalue.as_array->RemoveReference();
      } else {
        INTERPRET_ERROR(UNDEFINED_TYPE_OP, "clear", mapType(m_rtype));
      }
      break;
      
    case AS_BUILTIN_COPY:
      trgt->Accept(*this);
      if (m_rtype.type == TYPE(ARRAY)) {
        cLocalArray* arr = new cLocalArray(m_rvalue.as_array);
        m_rvalue.as_array->RemoveReference();
        m_rvalue.as_array = arr;
      } else {
        INTERPRET_ERROR(UNDEFINED_TYPE_OP, "copy", mapType(m_rtype));
      }
      break;
      
    case AS_BUILTIN_HASKEY:
      trgt->Accept(*this);
      if (m_rtype.type == TYPE(DICT)) {
        cLocalDict* dict = m_rvalue.as_dict;

        args->Iterator().Next()->Accept(*this);
        sAggregateValue idx(m_rtype, m_rvalue);
        m_rvalue.as_bool = dict->HasKey(idx);
        idx.Cleanup();
        dict->RemoveReference();
        
        m_rtype = TYPE(BOOL);
      } else {
        INTERPRET_ERROR(UNDEFINED_TYPE_OP, "keys", mapType(m_rtype));
      }
      break;
      
      
    case AS_BUILTIN_KEYS:
      trgt->Accept(*this);
      if (m_rtype.type == TYPE(DICT)) {
        cLocalArray* arr = new cLocalArray();
        arr->SetWithKeys(m_rvalue.as_dict);
        m_rvalue.as_dict->RemoveReference();
        m_rvalue.as_array = arr;
        m_rtype = TYPE(ARRAY);
      } else {
        INTERPRET_ERROR(UNDEFINED_TYPE_OP, "keys", mapType(m_rtype));
      }
      break;
      
    case AS_BUILTIN_VALUES:
      trgt->Accept(*this);
      if (m_rtype.type == TYPE(DICT)) {
        cLocalArray* arr = new cLocalArray();
        arr->SetWithValues(m_rvalue.as_dict);
        m_rvalue.as_dict->RemoveReference();
        m_rvalue.as_array = arr;
        m_rtype = TYPE(ARRAY);
      } else {
        INTERPRET_ERROR(UNDEFINED_TYPE_OP, "values", mapType(m_rtype));
      }
      break;
      
      
    case AS_BUILTIN_LEN:
      trgt->Accept(*this);
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
      
    case AS_BUILTIN_REMOVE:
      trgt->Accept(*this);
      if (m_rtype.type == TYPE(DICT)) {
        cLocalDict* dict = m_rvalue.as_dict;
        
        if (!dict->IsShared()) {
          dict->RemoveReference();
          break;
        }
        
        args->Iterator().Next()->Accept(*this);
        sAggregateValue idx(m_rtype, m_rvalue);
        dict->Remove(idx);
        idx.Cleanup();
        dict->RemoveReference();
      } else if (m_rtype.type == TYPE(ARRAY)) {
        cLocalArray* arr = m_rvalue.as_array;
        
        if (!arr->IsShared()) {
          arr->RemoveReference();
          break;
        }
        
        args->Iterator().Next()->Accept(*this);
        int i = asInt(m_rtype, m_rvalue, node);
        
        if (i < 0 || i >= arr->GetSize()) INTERPRET_ERROR(INDEX_OUT_OF_BOUNDS);
        
        for (; i < (arr->GetSize() - 1); i++) arr->Set(i, arr->Get(i + 1));
        arr->Resize(arr->GetSize() - 1);
      } else {
        INTERPRET_ERROR(UNDEFINED_TYPE_OP, "remove", mapType(m_rtype));
      }
      break;
      
    case AS_BUILTIN_RESIZE:
      trgt->Accept(*this);
      if (m_rtype.type == TYPE(MATRIX)) {
        cLocalMatrix* mat = m_rvalue.as_matrix;
        
        if (args->GetSize() != 2) INTERPRET_ERROR(INVALID_ARRAY_SIZE);
        
        if (!mat->IsShared()) {
          mat->RemoveReference();
          break;
        }
        
        tListIterator<cASTNode> it = args->Iterator();
        it.Next()->Accept(*this);
        int sz_x = asInt(m_rtype, m_rvalue, node);
        it.Next()->Accept(*this);
        int sz_y = asInt(m_rtype, m_rvalue, node);
        
        mat->Resize(sz_x, sz_y);
        mat->RemoveReference();
      } else if (m_rtype.type == TYPE(ARRAY)) {
        cLocalArray* arr = m_rvalue.as_array;
        
        if (!arr->IsResizable()) INTERPRET_ERROR(CANNOT_RESIZE_MATRIX_ROW);

        if (args->GetSize() != 1) INTERPRET_ERROR(INVALID_ARRAY_SIZE);
        
        if (!arr->IsShared()) {
          arr->RemoveReference();
          break;
        }
        
        args->Iterator().Next()->Accept(*this);
        int sz = asInt(m_rtype, m_rvalue, node);
        
        arr->Resize(sz);
        arr->RemoveReference();
      } else {
        INTERPRET_ERROR(UNDEFINED_TYPE_OP, "resize", mapType(m_rtype));
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
    cASCPPParameter* args = new cASCPPParameter[func->GetArity()];
    if (func->GetArity()) {
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
          case TYPE(OBJECT_REF):  args[i].Set(asNativeObject(func->GetArgumentType(i).info, m_rtype, m_rvalue, node)); break;
            
          default:
            INTERPRET_ERROR(INTERNAL);
        }
      }
    }
    
    // Call the function
    cASCPPParameter rvalue = func->Call(args);

    // Handle the return value
    switch (node.GetType().type) {
      case TYPE(BOOL):        m_rvalue.as_bool = rvalue.Get<bool>(); break;
      case TYPE(CHAR):        m_rvalue.as_char = rvalue.Get<char>(); break;
      case TYPE(FLOAT):       m_rvalue.as_float = rvalue.Get<double>(); break;
      case TYPE(INT):         m_rvalue.as_int = rvalue.Get<int>(); break;
      case TYPE(STRING):      m_rvalue.as_string = rvalue.Get<cString*>(); break;
      case TYPE(OBJECT_REF):  m_rvalue.as_nobj = rvalue.Get<cASNativeObject*>(); break;
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
        case TYPE(OBJECT_REF):
          args[i].Get<cASNativeObject*>()->RemoveReference(); break;
          
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
    int o_sp = m_sp;
    int sp = m_call_stack.GetSize();
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

      switch (func_symtbl->GetVariableType(var_id).type) {
        case TYPE(ARRAY):       m_call_stack[sp + var_id].value.as_array = asArray(m_rtype, m_rvalue, node); break;
        case TYPE(BOOL):        m_call_stack[sp + var_id].value.as_bool = asBool(m_rtype, m_rvalue, node); break;
        case TYPE(CHAR):        m_call_stack[sp + var_id].value.as_char = asChar(m_rtype, m_rvalue, node); break;
        case TYPE(DICT):        m_call_stack[sp + var_id].value.as_dict = asDict(m_rtype, m_rvalue, node); break;
        case TYPE(FLOAT):       m_call_stack[sp + var_id].value.as_float = asFloat(m_rtype, m_rvalue, node); break;
        case TYPE(INT):         m_call_stack[sp + var_id].value.as_int = asInt(m_rtype, m_rvalue, node); break;
        case TYPE(OBJECT_REF):  m_call_stack[sp + var_id].value.as_nobj = asNativeObject(func_symtbl->GetVariableType(var_id).info, m_rtype, m_rvalue, node); break;
        case TYPE(MATRIX):      m_call_stack[sp + var_id].value.as_matrix = asMatrix(m_rtype, m_rvalue, node); break;
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
      case TYPE(OBJECT_REF):  m_rvalue.as_nobj = asNativeObject(node.GetType().info, m_rtype, m_rvalue, node); break;
      case TYPE(MATRIX):      m_rvalue.as_matrix = asMatrix(m_rtype, m_rvalue, node); break;
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
        case TYPE(MATRIX):      m_call_stack[sp + i].value.as_matrix->RemoveReference(); break;
        case TYPE(OBJECT_REF):  delete m_call_stack[sp + i].value.as_ref; break;
        case TYPE(STRING):      delete m_call_stack[sp + i].value.as_string; break;
        default: break;
      }
    }
    
    // Restore previous scope
    m_has_returned = false;
    m_call_stack.Resize(m_call_stack.GetSize() - m_cur_symtbl->GetNumVariables());
    m_sp = o_sp;
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
    cLocalMatrix* mat = new cLocalMatrix();
    
    int sz_y = 1;
    bool first = true;
    
    tListIterator<cASTNode> it = vals->Iterator();
    cASTNode* val = NULL;
    int i = 0;
    while ((val = it.Next())) {
      val->Accept(*this);
      if (first && m_rtype.type == TYPE(ARRAY)) {
        sz_y = m_rvalue.as_array->GetSize();
      }
      
      mat->Resize(i + 1, sz_y);
      
      if (sz_y > 1) {
        if (m_rtype.type != TYPE(ARRAY)) INTERPRET_ERROR(INVALID_ARRAY_SIZE);
        mat->Set(i, m_rvalue.as_array);
      } else {
        mat->GetRow(i)->Set(0, m_rtype, m_rvalue);
      }

      sAggregateValue val(m_rtype, m_rvalue);
      val.Cleanup();
      first = false;
      i++;
    }
    
    m_rvalue.as_matrix = mat;
    m_rtype = TYPE(MATRIX);
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
  node.GetObject()->Accept(*this);
 
  if (m_rtype.type != TYPE(OBJECT_REF))
    INTERPRET_ERROR(TYPE_CAST, mapType(m_rtype.type), mapType(TYPE(OBJECT_REF)));
  
  cASNativeObject* nobj = m_rvalue.as_nobj;
  
  int mid = -1;
  if (!nobj->LookupMethod(node.GetName(), mid))
    INTERPRET_ERROR(NOBJ_METHOD_LOOKUP_FAILED, *node.GetName(), *m_rtype.info);
    
  int arity = nobj->GetArity(mid);
  // Setup arguments
  cASCPPParameter* args = new cASCPPParameter[arity];
  if (arity) {
    tListIterator<cASTNode> cit = node.GetArguments()->Iterator();
    cASTNode* an = NULL;
    for (int i = 0; i < arity; i++) {
      an = cit.Next();
      an->Accept(*this);
      
      switch (nobj->GetArgumentType(mid, i).type) {
        case TYPE(BOOL):        args[i].Set(asBool(m_rtype, m_rvalue, node)); break;
        case TYPE(CHAR):        args[i].Set(asChar(m_rtype, m_rvalue, node)); break;
        case TYPE(FLOAT):       args[i].Set(asFloat(m_rtype, m_rvalue, node)); break;
        case TYPE(INT):         args[i].Set(asInt(m_rtype, m_rvalue, node)); break;
        case TYPE(STRING):      args[i].Set(asString(m_rtype, m_rvalue, node)); break;
          
        default:
          INTERPRET_ERROR(INTERNAL);
      }
    }
  }
  
  // Call the function
  cASCPPParameter rvalue = nobj->CallMethod(mid, args);
  
  // Handle the return value
  m_rtype = nobj->GetReturnType(mid);
  switch (m_rtype.type) {
    case TYPE(BOOL):        m_rvalue.as_bool = rvalue.Get<bool>(); break;
    case TYPE(CHAR):        m_rvalue.as_char = rvalue.Get<char>(); break;
    case TYPE(FLOAT):       m_rvalue.as_float = rvalue.Get<double>(); break;
    case TYPE(INT):         m_rvalue.as_int = rvalue.Get<int>(); break;
    case TYPE(STRING):      m_rvalue.as_string = rvalue.Get<cString*>(); break;
    case TYPE(OBJECT_REF):  m_rvalue.as_nobj = rvalue.Get<cASNativeObject*>(); break;
    case TYPE(VOID):        break;
      
    default:
      INTERPRET_ERROR(INTERNAL);
  }
  
  // Clean up arguments
  for (int i = 0; i < arity; i++) {
    switch (nobj->GetArgumentType(mid, i).type) {
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
  
}

void cDirectInterpretASTVisitor::VisitObjectReference(cASTObjectReference& node)
{
  // @AS_TODO - handle object reference
  INTERPRET_ERROR(INTERNAL);
}

void cDirectInterpretASTVisitor::VisitVariableReference(cASTVariableReference& node)
{
  int var_id = node.GetVarID();
  int sp = node.IsVarGlobal() ? 0 : m_sp;
  
  if (m_obj_assign) {
    switch (node.GetType().type) {
      case TYPE(ARRAY):       m_rvalue.as_ref = new cArrayVarRef(m_call_stack[sp + var_id].value); break;
      case TYPE(DICT):        m_rvalue.as_ref = new cDictVarRef(m_call_stack[sp + var_id].value); break;
      case TYPE(OBJECT_REF):  m_rvalue.as_ref = new cNativeObjectVarRef(m_call_stack[sp + var_id].value); break;
      case TYPE(MATRIX):      m_rvalue.as_ref = new cMatrixVarRef(m_call_stack[sp + var_id].value); break;
      case TYPE(STRING):      m_rvalue.as_ref = new cStringVarRef(m_call_stack[sp + var_id].value); break;
        
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
      case TYPE(MATRIX):      m_rvalue.as_matrix = m_call_stack[sp + var_id].value.as_matrix->GetReference(); break;
      case TYPE(STRING):      m_rvalue.as_string = new cString(*m_call_stack[sp + var_id].value.as_string); break;

      
      case TYPE(OBJECT_REF):
        m_rvalue.as_nobj = m_call_stack[sp + var_id].value.as_nobj->GetReference(); 
        m_rtype.info = m_call_stack[sp + var_id].type.info;
        break;
        
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
      case TYPE(OBJECT_REF):
        m_call_stack[var_idx].value.as_nobj->RemoveReference();
        m_call_stack[var_idx].value.as_nobj = asNativeObject(var_type.info, val.type, val.value, node);
        break;
        
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
                
      case TYPE(MATRIX):
        m_call_stack[var_idx].value.as_matrix->RemoveReference();
        m_call_stack[var_idx].value.as_matrix = asMatrix(val.type, val.value, node);
        break;
        
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

    case TYPE(MATRIX):
      {
        cLocalMatrix* mat = value.as_matrix;
        cLocalArray* arr = new cLocalArray(mat->GetNumRows());
        uAnyType val;
        for (int i = 0; i < mat->GetNumRows(); i++) {
          val.as_array = new cLocalArray(mat->GetRow(i));
          arr->Set(i, TYPE(ARRAY), val);
        }
        mat->RemoveReference();
        return arr;
      }
      
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
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(ARRAY)));
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
      
    case TYPE(MATRIX):
      {
        bool rval = (value.as_matrix->GetNumRows() && value.as_matrix->GetNumCols());
        value.as_matrix->RemoveReference();
        return rval;
      }
      
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
      

    case TYPE(OBJECT_REF): // @AS_TODO - implement asBool
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


cDirectInterpretASTVisitor::cLocalDict* cDirectInterpretASTVisitor::asDict(const sASTypeInfo& type, uAnyType value,
                                                                           cASTNode& node)
{
  switch (type.type) {
    case TYPE(DICT):
      return value.as_dict;
      
    case TYPE(ARRAY):
      {
        cLocalDict* dict = new cLocalDict();
        cLocalArray* arr = value.as_array;
        
        sAggregateValue idx;
        idx.type.type = TYPE(INT);
        
        for (idx.value.as_int = 0; idx.value.as_int < arr->GetSize(); idx.value.as_int++) {
          dict->Set(idx, arr->Get(idx.value.as_int));
        }
        
        arr->RemoveReference();
        return dict;
      }
      
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
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(FLOAT)));
  }
  
  return 0.0;
}


cDirectInterpretASTVisitor::cLocalMatrix* cDirectInterpretASTVisitor::asMatrix(const sASTypeInfo& type, uAnyType value,
                                                                               cASTNode& node)
{
  switch (type.type) {
    case TYPE(BOOL):
    case TYPE(CHAR):
    case TYPE(FLOAT):
    case TYPE(INT):
      {
        sAggregateValue val(type, value);
        cLocalMatrix* mat = new cLocalMatrix();
        mat->Resize(1, 1);
        mat->GetRow(0)->Set(0, val);
        return mat;
      }
      
    case TYPE(MATRIX):
      return value.as_matrix;
     
    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(MATRIX)));
  }
  
  return NULL;
}


cASNativeObject* cDirectInterpretASTVisitor::asNativeObject(const cString& info, const sASTypeInfo& type, uAnyType value,
                                                            cASTNode& node)                                                           
{
  switch (type.type) {
    case TYPE(OBJECT_REF):
      if (type.info != info) INTERPRET_ERROR(NOBJ_TYPE_MISMATCH, *info, *type.info);
      return value.as_nobj;
      
    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(OBJECT_REF)));
  }
  
  return NULL;
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
      
    case TYPE(MATRIX):
      {
        cString* str = new cString(cStringUtil::Stringf("< matrix(%d, %d) >", value.as_matrix->GetNumRows(), value.as_matrix->GetNumCols()));
        return str;
      }

    case TYPE(OBJECT_REF):
    {
      cString* str = new cString(cStringUtil::Stringf("< %s object @ %p >", value.as_nobj->GetType(), value.as_nobj));
      return str;
    }
      
    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(STRING)));
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


void cDirectInterpretASTVisitor::matrixAdd(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node)
{
  INTERPRET_ERROR(INTERNAL); // @AS_TODO - handle matrix add
}


void cDirectInterpretASTVisitor::matrixSubtract(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node)
{
  INTERPRET_ERROR(INTERNAL); // @AS_TODO - handle matrix sub
}


void cDirectInterpretASTVisitor::matrixMultiply(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node)
{
  // Validate all values in matrix operands and determine operation type
  ASType_t op_type = TYPE(INT);
  for (int i = 0; i < m1->GetNumRows(); i++) {
    cLocalArray* row = m1->GetRow(i);
    for (int j = 0; j < m1->GetNumCols(); j++) {
      switch (row->Get(j).type.type) {
        case TYPE(BOOL):
        case TYPE(CHAR):
        case TYPE(INT):
        case TYPE(STRING):
          break;
          
        case TYPE(FLOAT):
          op_type = TYPE(FLOAT);
          break;
          
        default:
          INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(row->Get(j).type));
      }
    }
  }
  for (int i = 0; i < m2->GetNumRows(); i++) {
    cLocalArray* row = m2->GetRow(i);
    for (int j = 0; j < m2->GetNumCols(); j++) {
      switch (row->Get(j).type.type) {
        case TYPE(BOOL):
        case TYPE(CHAR):
        case TYPE(INT):
        case TYPE(STRING):
          break;
          
        case TYPE(FLOAT):
          op_type = TYPE(FLOAT);
          break;
          
        default:
          INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(row->Get(j).type));
      }
    }
  }
  
  
  
  if (m1->GetNumRows() == 1 && m1->GetNumCols() == 1) {
    // left op scalar multiply
    
    if (op_type == TYPE(INT)) matrixMultiply_ScalarInt(m1, m2, node);
    else matrixMultiply_ScalarFloat(m1, m2, node);
    
  } else if (m2->GetNumRows() == 1 && m2->GetNumCols() == 1) {
    // right op scalar multiply
    
    if (op_type == TYPE(INT)) matrixMultiply_ScalarInt(m2, m1, node);
    else matrixMultiply_ScalarFloat(m2, m1, node);
    
  } else {
    // full multiply
    
    if (m1->GetNumCols() != m2->GetNumRows()) INTERPRET_ERROR(MATRIX_SIZE_MISMATCH, mapToken(TOKEN(OP_MUL)));
    
    if (op_type == TYPE(INT)) matrixMultiply_FullInt(m1, m2, node);
    else matrixMultiply_FullFloat(m1, m2, node);
  }
}

void cDirectInterpretASTVisitor::matrixMultiply_ScalarInt(cLocalMatrix* s, cLocalMatrix* m, cASTNode& node)
{
  int scalar = 0;
  tMatrix<int> op(m->GetNumRows(), m->GetNumCols());
  
  const sAggregateValue& val = s->GetRow(0)->Get(0);
  switch (val.type.type) {
    case TYPE(BOOL):    scalar = (val.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):    scalar = (int)val.value.as_char; break;
    case TYPE(INT):     scalar = val.value.as_int; break;
    case TYPE(STRING):  scalar = val.value.as_string->AsInt(); break;
      
    default:
      INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(val.type));
  }

  for (int i = 0; i < m->GetNumRows(); i++) {
    cLocalArray* row = m->GetRow(i);
    for (int j = 0; j < m->GetNumCols(); j++) {
      const sAggregateValue& val = row->Get(j);
      switch (val.type.type) {
        case TYPE(BOOL):    op[i][j] = (val.value.as_bool) ? 1 : 0; break;
        case TYPE(CHAR):    op[i][j] = (int)val.value.as_char; break;
        case TYPE(INT):     op[i][j] = val.value.as_int; break;
        case TYPE(STRING):  op[i][j] = val.value.as_string->AsInt(); break;
          
        default:
          INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(row->Get(j).type));
      }
    }
  }
  
  int sz_x = m->GetNumRows();
  int sz_y = m->GetNumCols();
  cLocalMatrix* mat = new cLocalMatrix();
  mat->Resize(sz_x, sz_y);
  
  sASTypeInfo op_type(TYPE(INT));
  for (int i = 0; i < sz_x; i++) {
    cLocalArray* row = mat->GetRow(i);
    for (int j = 0; j < sz_y; j++) {
      uAnyType val;
      val.as_int = scalar * op[i][j];
      row->Set(j, op_type, val);
    }
  }
  
  m_rvalue.as_matrix = mat;
  m_rtype = TYPE(MATRIX);  
}

void cDirectInterpretASTVisitor::matrixMultiply_ScalarFloat(cLocalMatrix* s, cLocalMatrix* m, cASTNode& node)
{
  double scalar = 0.0;
  tMatrix<double> op(m->GetNumRows(), m->GetNumCols());
  
  const sAggregateValue& val = s->GetRow(0)->Get(0);
  switch (val.type.type) {
    case TYPE(BOOL):    scalar = (val.value.as_bool) ? 1.0 : 0.0; break;
    case TYPE(CHAR):    scalar = (double)val.value.as_char; break;
    case TYPE(INT):     scalar = (double)val.value.as_int; break;
    case TYPE(FLOAT):   scalar = val.value.as_float; break;
    case TYPE(STRING):  scalar = val.value.as_string->AsDouble(); break;
      
    default:
      INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(val.type));
  }
  
  for (int i = 0; i < m->GetNumRows(); i++) {
    cLocalArray* row = m->GetRow(i);
    for (int j = 0; j < m->GetNumCols(); j++) {
      const sAggregateValue& val = row->Get(j);
      switch (val.type.type) {
        case TYPE(BOOL):    op[i][j] = (val.value.as_bool) ? 1.0 : 0.0; break;
        case TYPE(CHAR):    op[i][j] = (double)val.value.as_char; break;
        case TYPE(INT):     op[i][j] = (double)val.value.as_int; break;
        case TYPE(FLOAT):   op[i][j] = val.value.as_float; break;
        case TYPE(STRING):  op[i][j] = val.value.as_string->AsDouble(); break;
          
        default:
          INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(row->Get(j).type));
      }
    }
  }
  
  int sz_x = m->GetNumRows();
  int sz_y = m->GetNumCols();
  cLocalMatrix* mat = new cLocalMatrix();
  mat->Resize(sz_x, sz_y);
  
  sASTypeInfo op_type(TYPE(FLOAT));
  for (int i = 0; i < sz_x; i++) {
    cLocalArray* row = mat->GetRow(i);
    for (int j = 0; j < sz_y; j++) {
      uAnyType val;
      val.as_float = scalar * op[i][j];
      row->Set(j, op_type, val);
    }
  }
  
  m_rvalue.as_matrix = mat;
  m_rtype = TYPE(MATRIX);
}

void cDirectInterpretASTVisitor::matrixMultiply_FullInt(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node)
{
  tMatrix<int> op1(m1->GetNumRows(), m1->GetNumCols());
  tMatrix<int> op2(m2->GetNumRows(), m2->GetNumCols());
  
  for (int i = 0; i < m1->GetNumRows(); i++) {
    cLocalArray* row = m1->GetRow(i);
    for (int j = 0; j < m1->GetNumCols(); j++) {
      const sAggregateValue& val = row->Get(j);
      switch (val.type.type) {
        case TYPE(BOOL):    op1[i][j] = (val.value.as_bool) ? 1 : 0; break;
        case TYPE(CHAR):    op1[i][j] = (int)val.value.as_char; break;
        case TYPE(INT):     op1[i][j] = val.value.as_int; break;
        case TYPE(STRING):  op1[i][j] = val.value.as_string->AsInt(); break;
          
        default:
          INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(row->Get(j).type));
      }
    }
  }
  for (int i = 0; i < m2->GetNumRows(); i++) {
    cLocalArray* row = m2->GetRow(i);
    for (int j = 0; j < m2->GetNumCols(); j++) {
      const sAggregateValue& val = row->Get(j);
      switch (val.type.type) {
        case TYPE(BOOL):    op2[i][j] = (val.value.as_bool) ? 1 : 0; break;
        case TYPE(CHAR):    op2[i][j] = (int)val.value.as_char; break;
        case TYPE(INT):     op2[i][j] = val.value.as_int; break;
        case TYPE(STRING):  op2[i][j] = val.value.as_string->AsInt(); break;
          
        default:
          INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(row->Get(j).type));
      }
    }
  }

  int sz_x = m1->GetNumRows();
  int sz_y = m2->GetNumCols();
  cLocalMatrix* mat = new cLocalMatrix();
  mat->Resize(sz_x, sz_y);
  
  sASTypeInfo op_type(TYPE(INT));
  for (int i = 0; i < sz_x; i++) {
    cLocalArray* row = mat->GetRow(i);
    for (int j = 0; j < sz_y; j++) {
      uAnyType sum;
      sum.as_int = 0;
      for (int k = 0; k < m1->GetNumCols(); k++) sum.as_int += op1[i][k] * op2[k][j];
      row->Set(j, op_type, sum);
    }
  }
  
  m_rvalue.as_matrix = mat;
  m_rtype = TYPE(MATRIX);
}


void cDirectInterpretASTVisitor::matrixMultiply_FullFloat(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node)
{
  tMatrix<double> op1(m1->GetNumRows(), m1->GetNumCols());
  tMatrix<double> op2(m2->GetNumRows(), m2->GetNumCols());
  
  for (int i = 0; i < m1->GetNumRows(); i++) {
    cLocalArray* row = m1->GetRow(i);
    for (int j = 0; j < m1->GetNumCols(); j++) {
      const sAggregateValue& val = row->Get(j);
      switch (val.type.type) {
        case TYPE(BOOL):    op1[i][j] = (val.value.as_bool) ? 1.0 : 0.0; break;
        case TYPE(CHAR):    op1[i][j] = (double)val.value.as_char; break;
        case TYPE(INT):     op1[i][j] = (double)val.value.as_int; break;
        case TYPE(FLOAT):   op1[i][j] = val.value.as_float; break;
        case TYPE(STRING):  op1[i][j] = val.value.as_string->AsDouble(); break;
          
        default:
          INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(row->Get(j).type));
      }
    }
  }
  for (int i = 0; i < m2->GetNumRows(); i++) {
    cLocalArray* row = m2->GetRow(i);
    for (int j = 0; j < m2->GetNumCols(); j++) {
      const sAggregateValue& val = row->Get(j);
      switch (val.type.type) {
        case TYPE(BOOL):    op2[i][j] = (val.value.as_bool) ? 1.0 : 0.0; break;
        case TYPE(CHAR):    op2[i][j] = (double)val.value.as_char; break;
        case TYPE(INT):     op2[i][j] = (double)val.value.as_int; break;
        case TYPE(FLOAT):   op2[i][j] = val.value.as_float; break;
        case TYPE(STRING):  op2[i][j] = val.value.as_string->AsDouble(); break;
          
        default:
          INTERPRET_ERROR(MATRIX_OP_TYPE_MISMATCH, mapToken(TOKEN(OP_MUL)), mapType(row->Get(j).type));
      }
    }
  }
  
  int sz_x = m1->GetNumRows();
  int sz_y = m2->GetNumCols();
  cLocalMatrix* mat = new cLocalMatrix();
  mat->Resize(sz_x, sz_y);
  
  sASTypeInfo op_type(TYPE(FLOAT));
  for (int i = 0; i < sz_x; i++) {
    cLocalArray* row = mat->GetRow(i);
    for (int j = 0; j < sz_y; j++) {
      uAnyType sum;
      sum.as_float = 0.0;
      for (int k = 0; k < m1->GetNumCols(); k++) sum.as_float += op1[i][k] * op2[k][j];
      row->Set(j, op_type, sum);
    }
  }
  
  m_rvalue.as_matrix = mat;
  m_rtype = TYPE(MATRIX);
}




void cDirectInterpretASTVisitor::sAggregateValue::Cleanup()
{
  switch (type.type) {
    case TYPE(ARRAY):       value.as_array->RemoveReference(); break;
    case TYPE(DICT):        value.as_dict->RemoveReference(); break;
    case TYPE(MATRIX):      value.as_matrix->RemoveReference(); break;
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
      case TYPE(MATRIX):  return value.as_matrix == lval.value.as_matrix;
      case TYPE(OBJECT_REF): return value.as_nobj == lval.value.as_nobj;
      
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
      
    case TYPE(MATRIX):
      m_storage[i].value.as_matrix = value.as_matrix->GetReference();
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
      case TYPE(MATRIX):  m_storage[i + offset].value.as_matrix = in_storage[i].value.as_matrix->GetReference(); break;
        
      default: break;
    }
  }
}


void cDirectInterpretASTVisitor::cLocalArray::Resize(int sz)
{
  int o_sz = m_storage.GetSize();

  // Cleanup values if we are shrinking
  for (int i = sz; i < o_sz; i++) m_storage[i].Cleanup();
  
  m_storage.Resize(sz);
  
  // Initialize values if we are growing
  for (int i = o_sz; i < sz; i++) {
    m_storage[i].value.as_int = 0;
    m_storage[i].type = TYPE(INT);
  }  
}

void cDirectInterpretASTVisitor::cLocalArray::SetWithArray(cLocalArray* arr)
{
  Resize(arr->GetSize());
  
  for (int i = 0; i < arr->GetSize(); i++) Set(i, arr->Get(i));
}

void cDirectInterpretASTVisitor::cLocalArray::SetWithKeys(cLocalDict* dict)
{
  Resize(0);
  
  dict->GetKeys(m_storage);
  
  for (int i = 0; i < m_storage.GetSize(); i++) {
    switch (m_storage[i].type.type) {
      case TYPE(STRING):  m_storage[i].value.as_string = new cString(*m_storage[i].value.as_string); break;
      case TYPE(ARRAY):   m_storage[i].value.as_array->GetReference(); break;
      case TYPE(DICT):    m_storage[i].value.as_dict->GetReference(); break;
      case TYPE(MATRIX):  m_storage[i].value.as_matrix->GetReference(); break;
        
      default: break;
    }
  }
}


void cDirectInterpretASTVisitor::cLocalArray::SetWithValues(cLocalDict* dict)
{
  Resize(0);
  
  dict->GetValues(m_storage);
  
  for (int i = 0; i < m_storage.GetSize(); i++) {
    switch (m_storage[i].type.type) {
      case TYPE(STRING):  m_storage[i].value.as_string = new cString(*m_storage[i].value.as_string); break;
      case TYPE(ARRAY):   m_storage[i].value.as_array->GetReference(); break;
      case TYPE(DICT):    m_storage[i].value.as_dict->GetReference(); break;
      case TYPE(MATRIX):  m_storage[i].value.as_matrix->GetReference(); break;
        
      default: break;
    }
  }
}


cDirectInterpretASTVisitor::cLocalArray::~cLocalArray()
{
  int sz = m_storage.GetSize();
  
  // Cleanup values stored in the array
  for (int i = 0; i < sz; i++) m_storage[i].Cleanup();
}


void cDirectInterpretASTVisitor::cLocalMatrix::Resize(int sz_x, int sz_y)
{
  int o_sz = m_storage.GetSize();
  m_storage.Resize(sz_x);
  
  // Resize the columns
  for (int i = (m_sz_y == sz_y) ? o_sz : 0; i < sz_x; i++) {
    m_storage[i].SetNonResizable();
    m_storage[i].Resize(sz_y);
  }

  m_sz_y = sz_y;
}




void cDirectInterpretASTVisitor::cLocalDict::Set(const sAggregateValue& idx, const sAggregateValue& val)
{
  sAggregateValue o_val;
  if (m_storage.Find(idx, o_val)) {
    o_val.Cleanup();
    m_storage.SetValue(idx, val);
  } else {
    m_storage.SetValue(idx, val);
  }
  
}

void cDirectInterpretASTVisitor::cLocalDict::Clear()
{
  tList<sAggregateValue> keys;
  tList<sAggregateValue> vals;
  
  m_storage.AsListsUnsorted(keys, vals);
  
  sAggregateValue* av = NULL;
  
  tListIterator<sAggregateValue> kit(keys);
  while ((av = kit.Next())) av->Cleanup();
  
  tListIterator<sAggregateValue> vit(vals);
  while ((av = vit.Next())) av->Cleanup();
  
  m_storage.ClearAll();
}

cDirectInterpretASTVisitor::cLocalDict::~cLocalDict()
{
  tList<sAggregateValue> keys;
  tList<sAggregateValue> vals;
  
  m_storage.AsListsUnsorted(keys, vals);
  
  sAggregateValue* av = NULL;
  
  tListIterator<sAggregateValue> kit(keys);
  while ((av = kit.Next())) av->Cleanup();

  tListIterator<sAggregateValue> vit(vals);
  while ((av = vit.Next())) av->Cleanup();
}


bool cDirectInterpretASTVisitor::cArrayVarRef::Get(const sAggregateValue& idx, sAggregateValue& val)
{
  int idxi = -1;
  switch (idx.type.type) {
    case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
    case TYPE(INT):         idxi = idx.value.as_int; break;
    case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
    case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); break;
    default: break;
  } 

  if (idxi < 0 || idxi >= m_var.as_array->GetSize()) return false;
  
  val = m_var.as_array->Get(idxi);
  return true;
}

bool cDirectInterpretASTVisitor::cArrayVarRef::Set(sAggregateValue& idx, sAggregateValue& val)
{
  int idxi = -1;
  switch (idx.type.type) {
    case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
    case TYPE(INT):         idxi = idx.value.as_int; break;
    case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
    case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); delete idx.value.as_string; break;
    case TYPE(ARRAY):       idx.value.as_array->RemoveReference(); break;
    case TYPE(DICT):        idx.value.as_dict->RemoveReference(); break;
    case TYPE(MATRIX):      idx.value.as_matrix->RemoveReference(); break;
    case TYPE(OBJECT_REF):  delete idx.value.as_ref; break;
    default: break;
  } 
  
  if (idxi < 0 || idxi >= m_var.as_array->GetSize()) return false;
    
  m_var.as_array->Set(idxi, val.type, val.value);
  
  return true;
}


bool cDirectInterpretASTVisitor::cDictVarRef::Get(const sAggregateValue& idx, sAggregateValue& val)
{
  return m_var.as_dict->Get(idx, val);
}

bool cDirectInterpretASTVisitor::cDictVarRef::Set(sAggregateValue& idx, sAggregateValue& val)
{
  m_var.as_dict->Set(idx, val);  
  return true;
}


bool cDirectInterpretASTVisitor::cMatrixVarRef::Get(const sAggregateValue& idx, sAggregateValue& val)
{
  int idxi = -1;
  switch (idx.type.type) {
    case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
    case TYPE(INT):         idxi = idx.value.as_int; break;
    case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
    case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); break;
    default: break;
  } 
  
  if (idxi < 0 || idxi >= m_var.as_matrix->GetNumRows()) return false;
  
  val.value.as_array = m_var.as_matrix->GetRow(idxi);
  val.type = TYPE(ARRAY);
  return true;
}

bool cDirectInterpretASTVisitor::cMatrixVarRef::Set(sAggregateValue& idx, sAggregateValue& val)
{
  int idxi = -1;
  switch (idx.type.type) {
    case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
    case TYPE(INT):         idxi = idx.value.as_int; break;
    case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
    case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); delete idx.value.as_string; break;
    case TYPE(ARRAY):       idx.value.as_array->RemoveReference(); break;
    case TYPE(DICT):        idx.value.as_dict->RemoveReference(); break;
    case TYPE(MATRIX):      idx.value.as_matrix->RemoveReference(); break;
    case TYPE(OBJECT_REF):  delete idx.value.as_ref; break;
    default: break;
  } 
  
  if (idxi < 0 || idxi >= m_var.as_matrix->GetNumRows()) return false;
  
  if (m_var.as_matrix->GetNumCols() == 0) return false;
  if (val.type.type != TYPE(ARRAY)) return false;
  if (val.value.as_array->GetSize() != m_var.as_matrix->GetNumCols()) return false;
  m_var.as_matrix->Set(idxi, val.value.as_array);
  
  return true;
}


bool cDirectInterpretASTVisitor::cNativeObjectVarRef::Get(const sAggregateValue& idx, sAggregateValue& val)
{
  // @AS_TODO - get indexed native var
  return false;
}

bool cDirectInterpretASTVisitor::cNativeObjectVarRef::Set(sAggregateValue& idx, sAggregateValue& val)
{
  // @AS_TODO - set indexed native var
  return false;
}




bool cDirectInterpretASTVisitor::cObjectIndexRef::Get(const sAggregateValue& idx, sAggregateValue& val)
{
  sAggregateValue o_val;
  
  if (!m_obj->Get(m_idx, o_val)) return false;
  
  switch (o_val.type.type) {
    case TYPE(ARRAY):
      {
        cLocalArray* arr = o_val.value.as_array;
        
        int idxi = -1;
        switch (idx.type.type) {
          case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
          case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
          case TYPE(INT):         idxi = idx.value.as_int; break;
          case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
          case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); break;
          default: break;
        }      
        if (idxi < 0 || idxi >= arr->GetSize()) return false;
        val = arr->Get(idxi);
        return true;
      }
      
    case TYPE(DICT):
      return o_val.value.as_dict->Get(idx, val);
      
    case TYPE(MATRIX):
      {
        cLocalMatrix* mat = o_val.value.as_matrix;
        
        int idxi = -1;
        switch (idx.type.type) {
          case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
          case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
          case TYPE(INT):         idxi = idx.value.as_int; break;
          case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
          case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); break;
          default: break;
        }      
        if (idxi < 0 || idxi >= mat->GetNumRows()) return false;
        val.value.as_array = mat->GetRow(idxi);
        val.type = TYPE(ARRAY);
        return true;
      }
      
    default:
      break;
  }  

  
  return false;
}


bool cDirectInterpretASTVisitor::cObjectIndexRef::Set(sAggregateValue& idx, sAggregateValue& val)
{
  sAggregateValue o_val;
  
  if (!m_obj->Get(m_idx, o_val)) {
    idx.Cleanup();
    return false;
  }
  
  switch (o_val.type.type) {
    case TYPE(ARRAY):
      {
        cLocalArray* arr = o_val.value.as_array;
        
        int idxi = -1;
        switch (idx.type.type) {
          case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
          case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
          case TYPE(INT):         idxi = idx.value.as_int; break;
          case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
          case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); delete idx.value.as_string; break;
          case TYPE(ARRAY):       idx.value.as_array->RemoveReference(); break;
          case TYPE(DICT):        idx.value.as_dict->RemoveReference(); break;
          case TYPE(MATRIX):      idx.value.as_matrix->RemoveReference(); break;
          case TYPE(OBJECT_REF):  delete idx.value.as_ref; break;
          default: break;
        }      
        if (idxi < 0 || idxi >= arr->GetSize()) return false;
        
        arr->Set(idxi, val.type, val.value);
      }
      return true;
      
    case TYPE(DICT):
      o_val.value.as_dict->Set(idx, val);
      return true;
      
    case TYPE(MATRIX):
      {
        cLocalMatrix* mat = o_val.value.as_matrix;
        
        int idxi = -1;
        switch (idx.type.type) {
          case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
          case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
          case TYPE(INT):         idxi = idx.value.as_int; break;
          case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
          case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); delete idx.value.as_string; break;
          case TYPE(ARRAY):       idx.value.as_array->RemoveReference(); break;
          case TYPE(DICT):        idx.value.as_dict->RemoveReference(); break;
          case TYPE(MATRIX):      idx.value.as_matrix->RemoveReference(); break;
          case TYPE(OBJECT_REF):  delete idx.value.as_ref; break;
          default: break;
        }      
        if (idxi < 0 || idxi >= mat->GetNumRows()) return false;
        
        if (mat->GetNumCols() == 0) return false;
        if (val.type.type != TYPE(ARRAY)) return false;
        if (val.value.as_array->GetSize() != mat->GetNumCols()) return false;
        mat->Set(idxi, val.value.as_array);
      }
      return true;
      
    default:
      break;
  }  

  idx.Cleanup();
  return false;
}


bool cDirectInterpretASTVisitor::cStringVarRef::Get(const sAggregateValue& idx, sAggregateValue& val)
{
  int idxi = -1;
  switch (idx.type.type) {
    case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
    case TYPE(INT):         idxi = idx.value.as_int; break;
    case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
    case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); break;
    default: break;
  } 
  
  if (idxi < 0 || idxi >= m_var.as_string->GetSize()) return false;
  
  val.value.as_char = (*m_var.as_string)[idxi];
  val.type = TYPE(CHAR);
  return true;
}

bool cDirectInterpretASTVisitor::cStringVarRef::Set(sAggregateValue& idx, sAggregateValue& val)
{
  int idxi = -1;
  switch (idx.type.type) {
    case TYPE(BOOL):        idxi = (idx.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):        idxi = (int)idx.value.as_char; break;
    case TYPE(INT):         idxi = idx.value.as_int; break;
    case TYPE(FLOAT):       idxi = (int)idx.value.as_float; break;
    case TYPE(STRING):      idxi = idx.value.as_string->AsInt(); delete idx.value.as_string; break;
    case TYPE(ARRAY):       idx.value.as_array->RemoveReference(); break;
    case TYPE(DICT):        idx.value.as_dict->RemoveReference(); break;
    case TYPE(MATRIX):      idx.value.as_matrix->RemoveReference(); break;
    case TYPE(OBJECT_REF):  delete idx.value.as_ref; break;
    default: break;
  } 
  
  if (idxi < 0 || idxi >= m_var.as_string->GetSize()) return false;
  
  switch (val.type.type) {
    case TYPE(BOOL):  (*m_var.as_string)[idxi] = (val.value.as_bool) ? 1 : 0; break;
    case TYPE(CHAR):  (*m_var.as_string)[idxi] = val.value.as_char; break;
    case TYPE(INT):   (*m_var.as_string)[idxi] = (char)val.value.as_int; break;
      
    default:
      return false;
  }
  
  return true;
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
    case AS_DIRECT_INTERPRET_ERR_CANNOT_RESIZE_MATRIX_ROW:
      std::cerr << "cannot directly resize matrix row" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_DIVISION_BY_ZERO:
      std::cerr << "division by zero" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_INDEX_ERROR:
      std::cerr << "index operation failed" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_INDEX_OUT_OF_BOUNDS:
      std::cerr << "array index out of bounds" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_INVALID_ARRAY_SIZE:
      std::cerr << "invalid array dimension" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_KEY_NOT_FOUND:
      std::cerr << "key not found" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_MATRIX_OP_TYPE_MISMATCH:
      {
        const char* op = VA_ARG_STR;
        const char* type = VA_ARG_STR;
        std::cerr << "matrix '" << op << "' undefined for contained value of type '" << type << "'" << ERR_ENDL;
      }
      break;
    case AS_DIRECT_INTERPRET_ERR_MATRIX_SIZE_MISMATCH:
      std::cerr << "matrix size mismatch for '" << VA_ARG_STR << "' operation" << ERR_ENDL;
      break;
    case AS_DIRECT_INTERPRET_ERR_NOBJ_METHOD_LOOKUP_FAILED:
      {
        const char* meth = VA_ARG_STR;
        const char* itype = VA_ARG_STR;
        std::cerr << "method '" << meth << "' not supported by '" << itype << "'" << ERR_ENDL;
      }
      break;
    case AS_DIRECT_INTERPRET_ERR_NOBJ_TYPE_MISMATCH:
      {
        const char* otype = VA_ARG_STR;
        const char* itype = VA_ARG_STR;
        std::cerr << "expected object of type '" << otype << "', received '" << itype << "'" << ERR_ENDL;
      }
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
  
  exit(AS_EXIT_FAIL_INTERPRET);
  
#undef ERR_ENDL
#undef VA_ARG_STR
}

#undef INTERPRET_ERROR()
#undef TOKEN()
#undef TYPE()
