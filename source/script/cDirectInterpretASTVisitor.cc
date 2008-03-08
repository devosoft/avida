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

#include <cmath>

#include "avida.h"
#include "AvidaScript.h"

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
  : m_global_symtbl(global_symtbl), m_cur_symtbl(global_symtbl), m_call_stack(0, 2048), m_sp(0), m_has_returned(false)
{
  m_call_stack.Resize(m_global_symtbl->GetNumVariables());
  for (int i = 0; i < m_global_symtbl->GetNumVariables(); i++) m_call_stack[i].as_string = NULL;
}


int cDirectInterpretASTVisitor::Interpret(cASTNode* node)
{
  node->Accept(*this);
  
  if (m_has_returned) return asInt(m_rtype, m_rvalue, *node);

  return 0;
}


void cDirectInterpretASTVisitor::visitAssignment(cASTAssignment& node)
{
  cSymbolTable* symtbl = node.IsVarGlobal() ? m_global_symtbl : m_cur_symtbl;
  int sp = node.IsVarGlobal() ? 0 : m_sp;
  int var_id = node.GetVarID();
  
  node.GetExpression()->Accept(*this);
  
  switch (symtbl->GetVariableType(var_id)) {
    case TYPE(ARRAY):       INTERPRET_ERROR(INTERNAL); // @TODO - assignment
    case TYPE(BOOL):        m_call_stack[sp + var_id].as_bool = asBool(m_rtype, m_rvalue, node); break;
    case TYPE(CHAR):        m_call_stack[sp + var_id].as_char = asChar(m_rtype, m_rvalue, node); break;
    case TYPE(FLOAT):       m_call_stack[sp + var_id].as_float = asFloat(m_rtype, m_rvalue, node); break;
    case TYPE(INT):         m_call_stack[sp + var_id].as_int = asInt(m_rtype, m_rvalue, node); break;
    case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - assignment
    case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - assignment
    case TYPE(STRING):
      delete m_call_stack[sp + var_id].as_string;
      m_call_stack[sp + var_id].as_string = asString(m_rtype, m_rvalue, node);
      break;
      
    default:
      INTERPRET_ERROR(INTERNAL);
  }
}


void cDirectInterpretASTVisitor::visitReturnStatement(cASTReturnStatement& node)
{
  node.GetExpression()->Accept(*this);
  m_has_returned = true;
}


void cDirectInterpretASTVisitor::visitStatementList(cASTStatementList& node)
{
  tListIterator<cASTNode> it = node.Iterator();
  
  cASTNode* stmt = NULL;
  while (!m_has_returned && (stmt = it.Next())) {
    stmt->Accept(*this);
  }
}



void cDirectInterpretASTVisitor::visitForeachBlock(cASTForeachBlock& node)
{
  //int var_id = node.GetVariable()->GetVarID();
  
  // @TODO - foreach block
}


void cDirectInterpretASTVisitor::visitIfBlock(cASTIfBlock& node)
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


void cDirectInterpretASTVisitor::visitWhileBlock(cASTWhileBlock& node)
{
  node.GetCondition()->Accept(*this);
  while (asBool(m_rtype, m_rvalue, node)) {
    node.GetCode()->Accept(*this);
    node.GetCondition()->Accept(*this);
  }
}



void cDirectInterpretASTVisitor::visitFunctionDefinition(cASTFunctionDefinition& node)
{
  // Nothing to do here
}


void cDirectInterpretASTVisitor::visitVariableDefinition(cASTVariableDefinition& node)
{
  if (node.GetAssignmentExpression()) {
    int var_id = node.GetVarID();
    
    node.GetAssignmentExpression()->Accept(*this);
    
    switch (m_cur_symtbl->GetVariableType(var_id)) {
      case TYPE(ARRAY):       INTERPRET_ERROR(INTERNAL); // @TODO - assignment
      case TYPE(BOOL):        m_call_stack[m_sp + var_id].as_bool = asBool(m_rtype, m_rvalue, node); break;
      case TYPE(CHAR):        m_call_stack[m_sp + var_id].as_char = asChar(m_rtype, m_rvalue, node); break;
      case TYPE(FLOAT):       m_call_stack[m_sp + var_id].as_float = asFloat(m_rtype, m_rvalue, node); break;
      case TYPE(INT):         m_call_stack[m_sp + var_id].as_int = asInt(m_rtype, m_rvalue, node); break;
      case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - assignment
      case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - assignment
      case TYPE(STRING):
        delete m_call_stack[m_sp + var_id].as_string;
        m_call_stack[m_sp + var_id].as_string = asString(m_rtype, m_rvalue, node);
        break;
        
      default:
        INTERPRET_ERROR(INTERNAL);
    }
  } else if (node.GetDimensions()) {
    INTERPRET_ERROR(INTERNAL); // @TODO - array/matrix dimensions
  }
}


void cDirectInterpretASTVisitor::visitVariableDefinitionList(cASTVariableDefinitionList& node)
{
  // Should never recurse into here.  Variable definition lists are processed by function definitions.
  INTERPRET_ERROR(INTERNAL);
}



void cDirectInterpretASTVisitor::visitExpressionBinary(cASTExpressionBinary& node)
{
  // Process the left and right side expressions
  node.GetLeft()->Accept(*this);
  uAnyType lval = m_rvalue;
  ASType_t ltype = m_rtype;
  node.GetRight()->Accept(*this);
  uAnyType rval = m_rvalue;
  ASType_t rtype = m_rtype;
  
  
  switch (node.GetOperator()) {
    case TOKEN(ARR_RANGE):
    case TOKEN(ARR_EXPAN):
      // @TODO - implement array range/expand
      INTERPRET_ERROR(INTERNAL);
    
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
        ASType_t rettype = node.GetType();
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype, rtype);

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
        ASType_t comptype = node.GetCompareType();
        
        // Determine the operation type if it is a runtime decision
        if (comptype == TYPE(RUNTIME)) comptype = getRuntimeType(ltype, rtype);
             
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
        ASType_t comptype = node.GetCompareType();
        
        // Determine the operation type if it is a runtime decision
        if (comptype == TYPE(RUNTIME)) comptype = getRuntimeType(ltype, rtype);
             
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
        ASType_t rettype = node.GetType();
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype, rtype, true);
             
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
            // @TODO - array concatenation
            INTERPRET_ERROR(INTERNAL);
            
          case TYPE(MATRIX):
            // @TODO - matrix addition
            INTERPRET_ERROR(INTERNAL);
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(OP_SUB):
      {
        ASType_t rettype = node.GetType();
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype, rtype);
             
        switch (rettype) {
          case TYPE(CHAR):  m_rvalue.as_char = asChar(ltype, lval, node) - asChar(rtype, rval, node); break;
          case TYPE(INT):   m_rvalue.as_int = asInt(ltype, lval, node) - asInt(rtype, rval, node); break;
          case TYPE(FLOAT): m_rvalue.as_float = asFloat(ltype, lval, node) - asFloat(rtype, rval, node); break;            

          case TYPE(MATRIX):
            // @TODO - matrix subtraction
            INTERPRET_ERROR(INTERNAL);
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(OP_MUL):
      {
        ASType_t rettype = node.GetType();
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype, rtype);
             
        switch (rettype) {
          case TYPE(CHAR):  m_rvalue.as_char = asChar(ltype, lval, node) * asChar(rtype, rval, node); break;
          case TYPE(INT):   m_rvalue.as_int = asInt(ltype, lval, node) * asInt(rtype, rval, node); break;
          case TYPE(FLOAT): m_rvalue.as_float = asFloat(ltype, lval, node) * asFloat(rtype, rval, node); break;            

          case TYPE(MATRIX):
            // @TODO - matrix multiplication
            INTERPRET_ERROR(INTERNAL);
            
          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(OP_DIV):
      {
        ASType_t rettype = node.GetType();
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype, rtype);
             
        // @TODO - handle division by zero
        switch (rettype) {
          case TYPE(CHAR):  m_rvalue.as_char = asChar(ltype, lval, node) / asChar(rtype, rval, node); break;
          case TYPE(INT):   m_rvalue.as_int = asInt(ltype, lval, node) / asInt(rtype, rval, node); break;
          case TYPE(FLOAT): m_rvalue.as_float = asFloat(ltype, lval, node) / asFloat(rtype, rval, node); break;            

          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(OP_MOD):
      {
        ASType_t rettype = node.GetType();
        
        // Determine the operation type if it is a runtime decision
        if (rettype == TYPE(RUNTIME)) rettype = getRuntimeType(ltype, rtype);
             
        // @TODO - handle division by zero
        switch (rettype) {
          case TYPE(CHAR):  m_rvalue.as_char = asChar(ltype, lval, node) % asChar(rtype, rval, node); break;
          case TYPE(INT):   m_rvalue.as_int = asInt(ltype, lval, node) % asInt(rtype, rval, node); break;
          case TYPE(FLOAT): m_rvalue.as_float = fmod(asFloat(ltype, lval, node), asFloat(rtype, rval, node)); break;

          default:
            INTERPRET_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_ADD)), mapType(rettype));
        }
        
        m_rtype = rettype;
      }
      break;

    case TOKEN(IDX_OPEN):
      break;
      
    default:
      // Parser should not allow an invalid operator to pass
      INTERPRET_ERROR(INTERNAL);
  }
}


void cDirectInterpretASTVisitor::visitExpressionUnary(cASTExpressionUnary& node)
{
  node.GetExpression()->Accept(*this);
  
  switch (node.GetOperator()) {
    case TOKEN(OP_BIT_NOT):
      switch (m_rtype) {
        case TYPE(ARRAY):
        case TYPE(MATRIX):
          // @TODO - implement ~ for array/matrix
          INTERPRET_ERROR(INTERNAL);
          
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
      switch (m_rtype) {
        case TYPE(ARRAY):
        case TYPE(MATRIX):
          // @TODO - implement unary- for array/matrix
          INTERPRET_ERROR(INTERNAL);
          
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


void cDirectInterpretASTVisitor::visitArgumentList(cASTArgumentList& node)
{
  // Should never recurse into here.  Argument lists are processed by their owners as needed.
  INTERPRET_ERROR(INTERNAL);
}

void cDirectInterpretASTVisitor::visitFunctionCall(cASTFunctionCall& node)
{
  // Save previous scope information
  cSymbolTable* prev_symtbl = m_cur_symtbl;
  
  // Get function information
  cSymbolTable* func_src_symtbl = node.IsFuncGlobal() ? m_global_symtbl : m_cur_symtbl;
  int fun_id = node.GetFuncID();
  
  // Set current scope to the function symbol table
  cSymbolTable* func_symtbl = func_src_symtbl->GetFunctionSymbolTable(fun_id);
  int sp = m_sp + prev_symtbl->GetNumVariables();
  m_call_stack.Resize(m_call_stack.GetSize() + func_symtbl->GetNumVariables());
  for (int i = 0; i < func_symtbl->GetNumVariables(); i++) m_call_stack[sp + i].as_string = NULL;
  
  // Process the arguments to the function
  tSmartArray<int> str_var_idxs;
  tListIterator<cASTVariableDefinition> sit = func_src_symtbl->GetFunctionSignature(fun_id)->Iterator();
  tListIterator<cASTNode> cit = node.GetArguments()->Iterator();
  cASTVariableDefinition* arg_def = NULL;
  while ((arg_def = sit.Next())) {
    cASTNode* arg = cit.Next();
    if (arg) arg->Accept(*this);
    else arg_def->GetAssignmentExpression()->Accept(*this);
    
    int var_id = arg_def->GetVarID();

    switch (m_cur_symtbl->GetVariableType(var_id)) {
      case TYPE(ARRAY):       INTERPRET_ERROR(INTERNAL); // @TODO - assignment
      case TYPE(BOOL):        m_call_stack[sp + var_id].as_bool = asBool(m_rtype, m_rvalue, node); break;
      case TYPE(CHAR):        m_call_stack[sp + var_id].as_char = asChar(m_rtype, m_rvalue, node); break;
      case TYPE(FLOAT):       m_call_stack[sp + var_id].as_float = asFloat(m_rtype, m_rvalue, node); break;
      case TYPE(INT):         m_call_stack[sp + var_id].as_int = asInt(m_rtype, m_rvalue, node); break;
      case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - assignment
      case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - assignment
      case TYPE(STRING):
        {
          m_call_stack[sp + var_id].as_string = asString(m_rtype, m_rvalue, node);
          str_var_idxs.Push(var_id);
        }
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
  switch (node.GetType()) {
    case TYPE(ARRAY):       INTERPRET_ERROR(INTERNAL); // @TODO - return
    case TYPE(BOOL):        m_rvalue.as_bool = asBool(m_rtype, m_rvalue, node); break;
    case TYPE(CHAR):        m_rvalue.as_char = asChar(m_rtype, m_rvalue, node); break;
    case TYPE(FLOAT):       m_rvalue.as_float = asFloat(m_rtype, m_rvalue, node); break;
    case TYPE(INT):         m_rvalue.as_int = asInt(m_rtype, m_rvalue, node); break;
    case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - return
    case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - return
    case TYPE(STRING):      m_rvalue.as_string = asString(m_rtype, m_rvalue, node); break;
    case TYPE(VOID):        break;
      
    default:
      INTERPRET_ERROR(INTERNAL);
  }
  m_rtype = node.GetType();

  // Clean up string variables in the current scope
  for (int i = 0; i < str_var_idxs.GetSize(); i++) delete m_call_stack[m_sp + str_var_idxs[i]].as_string;
  
  // Restore previous scope
  m_has_returned = false;
  m_call_stack.Resize(m_call_stack.GetSize() - m_cur_symtbl->GetNumVariables());
  m_sp -= prev_symtbl->GetNumVariables();
  m_cur_symtbl = prev_symtbl;
}


void cDirectInterpretASTVisitor::visitLiteral(cASTLiteral& node)
{
  switch (node.GetType()) {
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


void cDirectInterpretASTVisitor::visitLiteralArray(cASTLiteralArray& node)
{
  // @TODO - handle literal array
  INTERPRET_ERROR(INTERNAL);
}


void cDirectInterpretASTVisitor::visitObjectCall(cASTObjectCall& node)
{
  // @TODO - handle object call
  INTERPRET_ERROR(INTERNAL);
}

void cDirectInterpretASTVisitor::visitObjectReference(cASTObjectReference& node)
{
  // @TODO - handle object reference
  INTERPRET_ERROR(INTERNAL);
}

void cDirectInterpretASTVisitor::visitVariableReference(cASTVariableReference& node)
{
  int var_id = node.GetVarID();
  int sp = node.IsVarGlobal() ? 0 : m_sp;
  
  switch (node.GetType()) {
    case TYPE(ARRAY):       INTERPRET_ERROR(INTERNAL); // @TODO - assignment
    case TYPE(BOOL):        m_rvalue.as_bool = m_call_stack[sp + var_id].as_bool; break;
    case TYPE(CHAR):        m_rvalue.as_char = m_call_stack[sp + var_id].as_char; break;
    case TYPE(FLOAT):       m_rvalue.as_float = m_call_stack[sp + var_id].as_float; break;
    case TYPE(INT):         m_rvalue.as_int = m_call_stack[sp + var_id].as_int; break;
    case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - assignment
    case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - assignment
    case TYPE(STRING):      m_rvalue.as_string = new cString(*m_call_stack[sp + var_id].as_string); break;
      
    default:
      INTERPRET_ERROR(INTERNAL);
  }
  m_rtype = node.GetType();
}


void cDirectInterpretASTVisitor::visitUnpackTarget(cASTUnpackTarget& node)
{
  // @TODO - handle unpack target
}


bool cDirectInterpretASTVisitor::asBool(ASType_t type, uAnyType value, cASTNode& node)
{
  switch (type) {
    case TYPE(ARRAY):
    case TYPE(MATRIX):
      // @TODO - implement asBool for array/matrix
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
      

    case TYPE(OBJECT_REF):
      // @TODO - implement asBool for object ref
      INTERPRET_ERROR(INTERNAL);

    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(BOOL)));
  }
  
  return false;
}


char cDirectInterpretASTVisitor::asChar(ASType_t type, uAnyType value, cASTNode& node)
{
  switch (type) {
    case TYPE(BOOL):
      return (value.as_bool) ? 1 : 0;
    case TYPE(CHAR):
      return value.as_char;
    case TYPE(INT):
      return (char)value.as_int;

    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(CHAR)));
  }
  
  return false;
}


int cDirectInterpretASTVisitor::asInt(ASType_t type, uAnyType value, cASTNode& node)
{
  switch (type) {
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
  
  return false;
}


double cDirectInterpretASTVisitor::asFloat(ASType_t type, uAnyType value, cASTNode& node)
{
  switch (type) {
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
  
  return false;
}


cString* cDirectInterpretASTVisitor::asString(ASType_t type, uAnyType value, cASTNode& node)
{
  switch (type) {
    case TYPE(ARRAY):       INTERPRET_ERROR(INTERNAL); // @TODO - as string
    case TYPE(BOOL):        return new cString(cStringUtil::Convert(value.as_bool));
    case TYPE(CHAR):        return new cString(value.as_char);
    case TYPE(INT):         return new cString(cStringUtil::Convert(value.as_int));
    case TYPE(FLOAT):       return new cString(cStringUtil::Convert(value.as_float));
    case TYPE(MATRIX):      INTERPRET_ERROR(INTERNAL); // @TODO - as string
    case TYPE(OBJECT_REF):  INTERPRET_ERROR(INTERNAL); // @TODO - as string
    case TYPE(STRING):      return value.as_string;
      
    default:
      INTERPRET_ERROR(TYPE_CAST, mapType(type), mapType(TYPE(INT)));
  }
  
  return false;
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
          
        case TYPE(RUNTIME):
          return TYPE(INVALID);
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
        case TYPE(RUNTIME):   return TYPE(INVALID);
        default: break;
      }
      break;
    case TYPE(FLOAT):
      switch (rtype) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(FLOAT);
        case TYPE(CHAR):      return TYPE(FLOAT);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(FLOAT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    if (allow_str) return TYPE(FLOAT); break;
        case TYPE(RUNTIME):   return TYPE(INVALID);
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
        case TYPE(RUNTIME):   return TYPE(INVALID);
        default: break;
      }
      break;
    case TYPE(MATRIX):
      return TYPE(MATRIX);
    case TYPE(STRING):
      if (allow_str) return TYPE(STRING); break;
      
    case TYPE(RUNTIME):
      return TYPE(INVALID);
      
    default: break;
  }
  
  return TYPE(INVALID);
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
