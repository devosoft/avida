/*
 *  cSemanticASTVisitor.cc
 *  Avida
 *
 *  Created by David on 2/24/08.
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

#include "cSemanticASTVisitor.h"

#include "cASLibrary.h"
#include "cSymbolTable.h"

#include <cstdarg>

using namespace AvidaScript;


#ifndef DEBUG_AS_SEMANTIC
#define DEBUG_AS_SEMANTIC 1
#endif

#define SEMANTIC_ERROR(code, ...) reportError(AS_SEMANTIC_ERR_ ## code, node.GetFilePosition(),  __LINE__, ##__VA_ARGS__)
#define SEMANTIC_WARNING(code, ...) reportError(AS_SEMANTIC_WARN_ ## code, node.GetFilePosition(),  __LINE__, ##__VA_ARGS__)

#define TOKEN(x) AS_TOKEN_ ## x
#define TYPE(x) AS_TYPE_ ## x
#define TYPEINFO(x) sASTypeInfo(AS_TYPE_ ## x)

namespace AvidaScript {
  static const bool valid_cast[13][13] = {
  // ARRAY, BOOL , CHAR , DICT , FLOAT, INT  , @OBJ , MATRX, STRNG, VAR  , RUNTM, VOID , INVLD
    {true , true , false, true , false, false, false, false, true , true , true , false, false}, // TYPE(ARRAY)
    {false, true , true , false, true , true , false, true , true , true , true , false, false}, // TYPE(BOOL)
    {false, true , true , false, true , true , false, true , true , true , true , false, false}, // TYPE(CHAR)
    {false, false, false, true , false, false, false, false, true , true , true , false, false}, // TYPE(DICT)
    {false, true , false, false, true , true , false, true , true , true , true , false, false}, // TYPE(FLOAT)
    {false, true , true , false, true , true , false, true , true , true , true , false, false}, // TYPE(INT)
    {false, true , false, false, false, false, true , false, true , true , false, false, false}, // TYPE(OBJECT_REF)
    {true , true , false, false, false, false, false, true , true , true , true , false, false}, // TYPE(MATRIX)
    {true , true , false, false, true , true , false, false, true , true , true , false, false}, // TYPE(STRNG)
    {true , true , true , true , true , true , true , true , true , true , true , false, false}, // TYPE(VAR)
    {true , true , true , true , true , true , true , true , true , true , true , false, false}, // TYPE(RUNTIME)
    {false, false, false, false, false, false, false, false, false, false, false, false, false}, // TYPE(VOID)
    {false, false, false, false, false, false, false, false, false, false, false, false, false}  // TYPE(INVALID)
  };
}


#define checkCast(in_type, out_type) { \
  if (valid_cast[in_type.type][out_type.type]) { \
    if ((in_type.type == TYPE(FLOAT) && out_type.type == TYPE(INT)) || \
        (in_type.type == TYPE(INT) && out_type.type == TYPE(CHAR))) \
      SEMANTIC_WARNING(LOSS_OF_PRECISION, mapType(in_type), mapType(out_type)); \
  } else { \
    SEMANTIC_ERROR(CANNOT_CAST, mapType(in_type), mapType(out_type)); \
  } \
}


cSemanticASTVisitor::cSemanticASTVisitor(cASLibrary* lib, cSymbolTable* global_symtbl, cASTNode* main)
  : m_library(lib), m_global_symtbl(global_symtbl), m_parent_scope(global_symtbl), m_fun_id(0), m_cur_symtbl(global_symtbl)
  , m_success(true), m_fun_def(false), m_fun_def_arg(false), m_top_level(true), m_obj_assign(false)
{
  // Add internal definition of the global function
  int fun_id = -1;
  m_global_symtbl->AddFunction("__asmain", TYPE(INT), fun_id);
  m_global_symtbl->SetFunctionDefinition(0, main);
}


void cSemanticASTVisitor::VisitAssignment(cASTAssignment& node)
{
  node.GetExpression()->Accept(*this);
  int var_id = -1;
  bool global = false;
  if (lookupVariable(node.GetVariable(), var_id, global)) {
    checkCast(node.GetExpression()->GetType(), (global ? m_global_symtbl : m_cur_symtbl)->GetVariableType(var_id));
    node.SetVar(var_id, global);
  } else {
    SEMANTIC_ERROR(VARIABLE_UNDEFINED, (const char*)node.GetVariable());
  }
}


void cSemanticASTVisitor::VisitArgumentList(cASTArgumentList& node)
{
  // Should never recurse into here.  Argument lists are processed by their owners as needed.
  SEMANTIC_ERROR(INTERNAL);
}


void cSemanticASTVisitor::VisitObjectAssignment(cASTObjectAssignment& node)
{
  m_obj_assign = true;
  node.GetTarget()->Accept(*this);
  m_obj_assign = false;
  
  if (node.GetTarget()->GetType().type != TYPE(OBJECT_REF)) SEMANTIC_ERROR(INVALID_ASSIGNMENT_TARGET);
  
  node.GetExpression()->Accept(*this);
}



void cSemanticASTVisitor::VisitReturnStatement(cASTReturnStatement& node)
{
  node.GetExpression()->Accept(*this);
  checkCast(m_parent_scope->GetFunctionRType(m_fun_id), node.GetExpression()->GetType());
  m_cur_symtbl->SetScopeReturn();
}


void cSemanticASTVisitor::VisitStatementList(cASTStatementList& node)
{
  tListIterator<cASTNode> it = node.Iterator();
  
  bool should_pop = false;
  if (m_top_level) {
    m_top_level = false;
  } else {
    m_cur_symtbl->PushScope();
    should_pop = true;
  }
  
  bool has_return = false;
  bool warn_unreach = false;
  cASTNode* stmt = NULL;
  while ((stmt = it.Next())) {
    m_fun_def = false;
    stmt->Accept(*this);
    if (!has_return && m_cur_symtbl->ScopeHasReturn()) has_return = true;
    else if (has_return && !m_fun_def && !warn_unreach) {
      reportError(AS_SEMANTIC_WARN_UNREACHABLE, stmt->GetFilePosition(),  __LINE__);
      warn_unreach = true;
    }
  }
  
  if (should_pop) {
    // Check all functions in the current scope level and make sure they have been defined
    cSymbolTable::cFunctionIterator fit = m_cur_symtbl->ActiveFunctionIterator();
    while (fit.Next()) if (!fit.HasCode()) SEMANTIC_ERROR(FUNCTION_UNDEFINED, (const char*)fit.GetName());
    m_cur_symtbl->PopScope();    
  }
}


void cSemanticASTVisitor::VisitForeachBlock(cASTForeachBlock& node)
{
  // Check values and make sure we can process it as an array
  node.GetValues()->Accept(*this);
  checkCast(node.GetValues()->GetType(), TYPEINFO(ARRAY));
  
  m_cur_symtbl->PushScope();
  
  // Check and define the variable in this scope
  node.GetVariable()->Accept(*this);
  
  // Check the code
  m_top_level = true;
  node.GetCode()->Accept(*this);
  
  // Check all functions in the current scope level and make sure they have been defined
  cSymbolTable::cFunctionIterator fit = m_cur_symtbl->ActiveFunctionIterator();
  while (fit.Next()) if (!fit.HasCode()) SEMANTIC_ERROR(FUNCTION_UNDEFINED, (const char*)fit.GetName());
  m_cur_symtbl->PopScope();
}


void cSemanticASTVisitor::VisitIfBlock(cASTIfBlock& node)
{
  // Check main condition and code
  node.GetCondition()->Accept(*this);
  checkCast(node.GetCondition()->GetType(), TYPEINFO(BOOL));
  
  node.GetCode()->Accept(*this);
  
  // Check all elseif blocks
  tListIterator<cASTIfBlock::cElseIf> it = node.ElseIfIterator();
  cASTIfBlock::cElseIf* ei = NULL;
  while ((ei = it.Next())) {
    ei->GetCondition()->Accept(*this);
    checkCast(ei->GetCondition()->GetType(), TYPEINFO(BOOL));
    ei->GetCode()->Accept(*this);
  }
  
  // Check else block if there is one
  if (node.GetElseCode()) node.GetElseCode()->Accept(*this);
}


void cSemanticASTVisitor::VisitWhileBlock(cASTWhileBlock& node)
{
  node.GetCondition()->Accept(*this);
  checkCast(node.GetCondition()->GetType(), TYPEINFO(BOOL));
  node.GetCode()->Accept(*this);
}



void cSemanticASTVisitor::VisitFunctionDefinition(cASTFunctionDefinition& node)
{
  if (m_library->HasFunction(node.GetName())) {
    SEMANTIC_ERROR(CANNOT_OVERRIDE_LIB_FUNCTION, (const char*)node.GetName());
    return;
  }
  
  int fun_id = -1;
  bool added = m_cur_symtbl->AddFunction(node.GetName(), node.GetType(), fun_id);
  
  m_fun_stack.Push(sFunctionEntry(m_parent_scope, m_fun_id));
  m_parent_scope = m_cur_symtbl;
  m_fun_id = fun_id;

  if (added) {
    // Create new symtbl and scope
    m_cur_symtbl = new cSymbolTable;
    m_parent_scope->SetFunctionSymbolTable(fun_id, m_cur_symtbl);
    
    // Check call signature
    cASTVariableDefinitionList* args = node.GetArguments();
    if (args) {
      // Move the args to the symbol table entry
      m_parent_scope->SetFunctionSignature(fun_id, args);
      node.ClearArguments();
      
      tListIterator<cASTVariableDefinition> it = args->Iterator();
      cASTVariableDefinition* vd = NULL;
      bool def_val_start = false;
      while ((vd = it.Next())) {
        vd->Accept(*this);
        if (def_val_start) {
          if (!vd->GetAssignmentExpression())
            SEMANTIC_ERROR(ARGUMENT_DEFAULT_REQUIRED, (const char*)vd->GetName(), (const char*)node.GetName());
        } else {
          if (vd->GetAssignmentExpression()) {
            def_val_start = true;
            m_fun_def_arg = true;
            vd->GetAssignmentExpression()->Accept(*this);
            m_fun_def_arg = false;
          }
        }
      }
    }
  } else {
    if (m_parent_scope->GetFunctionRType(fun_id) != node.GetType())
      SEMANTIC_ERROR(FUNCTION_RTYPE_MISMATCH, (const char*)node.GetName());
    
    // Lookup previously defined symbol table
    m_cur_symtbl = m_parent_scope->GetFunctionSymbolTable(fun_id);
    
    // Verify that the call signatures match
    cASTVariableDefinitionList* ssig = m_parent_scope->GetFunctionSignature(fun_id);
    if (ssig && node.GetArguments()) {
      if (ssig->GetSize() == node.GetArguments()->GetSize()) {
        tListIterator<cASTVariableDefinition> sit = ssig->Iterator();
        tListIterator<cASTVariableDefinition> nit = node.GetArguments()->Iterator();
        while (true) {
          cASTVariableDefinition* svd = sit.Next();
          cASTVariableDefinition* nvd = nit.Next();
          if (!svd || !nvd) break;
          
          if (svd->GetName() != nvd->GetName() || svd->GetType() != nvd->GetType() || nvd->GetAssignmentExpression()) {
            SEMANTIC_ERROR(FUNCTION_SIGNATURE_MISMATCH, (const char*)node.GetName());
            break;
          }
        }
      } else {
        SEMANTIC_ERROR(FUNCTION_SIGNATURE_MISMATCH, (const char*)node.GetName());
      }
    } else if ((ssig && !node.GetArguments()) || (!ssig && node.GetArguments())) {
      SEMANTIC_ERROR(FUNCTION_SIGNATURE_MISMATCH, (const char*)node.GetName());
    }
  }

  // If this is the definition process the code
  if (node.GetCode()) {
    if (!m_parent_scope->GetFunctionDefinition(fun_id)) {
      m_top_level = true;
      node.GetCode()->Accept(*this);
      
      if (node.GetType().type != TYPE(VOID) && !m_cur_symtbl->ScopeHasReturn()) SEMANTIC_WARNING(NO_RETURN);
      
      // Move the code to the symbol table entry
      m_parent_scope->SetFunctionDefinition(fun_id, node.GetCode());
      node.SetCode(NULL);
    } else {
      SEMANTIC_ERROR(FUNCTION_REDEFINITION, (const char*)node.GetName());
    }
  }
  
  // Check all functions in the current scope level and make sure they have been defined
  cSymbolTable::cFunctionIterator fit = m_cur_symtbl->ActiveFunctionIterator();
  while (fit.Next()) if (!fit.HasCode()) SEMANTIC_ERROR(FUNCTION_UNDEFINED, (const char*)fit.GetName());
  
  // Pop function stack
  sFunctionEntry prev = m_fun_stack.Pop();
  m_cur_symtbl = m_parent_scope;
  m_parent_scope = prev.parent_scope;
  m_fun_id = prev.fun_id;
  
  m_fun_def = true;
}


void cSemanticASTVisitor::VisitVariableDefinition(cASTVariableDefinition& node)
{
  int var_id = -1;
  if (m_cur_symtbl->AddVariable(node.GetName(), node.GetType(), var_id)) node.SetVar(var_id);
  else SEMANTIC_ERROR(VARIABLE_REDEFINITION, (const char*)node.GetName());
  
  // Process matrix/array dimensions
  cASTArgumentList* al = node.GetDimensions();
  if (al) {
    if (node.GetType().type == TYPE(MATRIX) || node.GetType().type == TYPE(ARRAY)) {
      // Check individual arguments for validity
      tListIterator<cASTNode> it = al->Iterator();
      cASTNode* alnode = NULL;
      while ((alnode = it.Next())) {
        alnode->Accept(*this);
        checkCast(alnode->GetType(), TYPEINFO(INT));
      }
      
      // If empty, warn...
      if (al->GetSize() == 0) SEMANTIC_WARNING(NO_DIMENSIONS);
      
      // If dimensions exceed type limits
      if ((node.GetType().type == TYPE(ARRAY) && al->GetSize() > 1) ||
          (node.GetType().type == TYPE(MATRIX) && al->GetSize() > 2)) {
        SEMANTIC_ERROR(TOO_MANY_ARGUMENTS);
        SEMANTIC_ERROR(VARIABLE_DIMENSIONS_INVALID, (const char*)node.GetName(), mapType(node.GetType()));
      }
    } else {
      SEMANTIC_ERROR(VARIABLE_DIMENSIONS_INVALID, (const char*)node.GetName(), mapType(node.GetType()));
    }
  }
  
  // Process assignment
  cASTNode* ae = node.GetAssignmentExpression();
  if (ae) {
    ae->Accept(*this);
    checkCast(ae->GetType(), node.GetType());
  }
}


void cSemanticASTVisitor::VisitVariableDefinitionList(cASTVariableDefinitionList& node)
{
  // Should never recurse into here.  Variable definition lists are processed by function definitions.
  SEMANTIC_ERROR(INTERNAL);
}



void cSemanticASTVisitor::VisitExpressionBinary(cASTExpressionBinary& node)
{
  node.GetLeft()->Accept(*this);
  node.GetRight()->Accept(*this);
  
  switch (node.GetOperator()) {
    case TOKEN(IDX_OPEN):
      switch (node.GetLeft()->GetType().type) {
        case TYPE(ARRAY):
        case TYPE(MATRIX):
        case TYPE(STRING):
          checkCast(node.GetRight()->GetType(), TYPEINFO(INT));
          break;
          
        case TYPE(DICT):
        case TYPE(RUNTIME):
          break;
          
        case TYPE(OBJECT_REF):
          if (m_obj_assign) break;
          
        default:
          SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetLeft()->GetType()));
          break;
      }
      node.SetType(m_obj_assign ? TYPEINFO(OBJECT_REF) : TYPEINFO(RUNTIME));
      break;
    case TOKEN(ARR_RANGE):
      checkCast(node.GetLeft()->GetType(), TYPEINFO(INT));
      checkCast(node.GetRight()->GetType(), TYPEINFO(INT));
      node.SetType(TYPE(ARRAY));
      break;
    case TOKEN(ARR_EXPAN):
      checkCast(node.GetRight()->GetType(), TYPEINFO(INT));
      node.SetType(TYPE(ARRAY));
      break;
      
    case TOKEN(OP_BIT_AND):
    case TOKEN(OP_BIT_OR):
      {
        bool valid_types = true;
        if (!validBitwiseType(node.GetLeft()->GetType())) {
          valid_types = false;
          SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetLeft()->GetType())); 
        }
        if (!validBitwiseType(node.GetRight()->GetType())) {
          valid_types = false;
          SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetRight()->GetType())); 
        }
        
        if (valid_types) node.SetType(getConsensusType(node.GetLeft()->GetType(), node.GetRight()->GetType()));
      }      
      break;
      
    case TOKEN(OP_LOGIC_AND):
    case TOKEN(OP_LOGIC_OR):
      checkCast(node.GetLeft()->GetType(), TYPEINFO(BOOL));
      checkCast(node.GetRight()->GetType(), TYPEINFO(BOOL));
      node.SetType(TYPE(BOOL));
      break;
    
    case TOKEN(OP_EQ):
    case TOKEN(OP_NEQ):
      if ((validArithmeticType(node.GetLeft()->GetType(), true) && validArithmeticType(node.GetRight()->GetType(), true)) ||
          (node.GetLeft()->GetType().type == TYPE(STRING) && node.GetRight()->GetType().type == TYPE(STRING))) {
        node.SetCompareType(getConsensusType(node.GetLeft()->GetType(), node.GetRight()->GetType()));
        node.SetType(TYPE(BOOL));
      } else if (node.GetLeft()->GetType().type == TYPE(BOOL) || node.GetRight()->GetType().type == TYPE(BOOL)) {
        node.SetCompareType(TYPE(BOOL));
        node.SetType(TYPE(BOOL));
      } else {
        SEMANTIC_ERROR(CANNOT_COMPARE);
      }
      break;
      
    case TOKEN(OP_LE):
    case TOKEN(OP_GE):
    case TOKEN(OP_LT):
    case TOKEN(OP_GT):
      if (validArithmeticType(node.GetLeft()->GetType(), true) && validArithmeticType(node.GetRight()->GetType(), true)) {
        node.SetCompareType(getConsensusType(node.GetLeft()->GetType(), node.GetRight()->GetType()));
        node.SetType(TYPE(BOOL));
      } else {
        SEMANTIC_ERROR(CANNOT_COMPARE);
      }
      break;
      
    case TOKEN(OP_ADD):
      if ((validArithmeticType(node.GetLeft()->GetType(), true) && validArithmeticType(node.GetRight()->GetType(), true)) ||
          (node.GetLeft()->GetType().type == TYPE(STRING) && node.GetRight()->GetType().type == TYPE(STRING)) ||
          (node.GetLeft()->GetType().type == TYPE(ARRAY) && node.GetRight()->GetType().type == TYPE(ARRAY))) {
        node.SetType(getConsensusType(node.GetLeft()->GetType(), node.GetRight()->GetType()));
      } else {
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetLeft()->GetType()));
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetRight()->GetType()));        
      }
      break;
      
    case TOKEN(OP_SUB):
    case TOKEN(OP_MUL):
      if (validArithmeticType(node.GetLeft()->GetType(), true) && validArithmeticType(node.GetRight()->GetType(), true)) {
        node.SetType(getConsensusType(node.GetLeft()->GetType(), node.GetRight()->GetType()));
      } else {
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetLeft()->GetType()));
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetRight()->GetType()));        
      }
      break;
    
    case TOKEN(OP_DIV):
    case TOKEN(OP_MOD):
      if (validArithmeticType(node.GetLeft()->GetType()) && validArithmeticType(node.GetRight()->GetType())) {
        node.SetType(getConsensusType(node.GetLeft()->GetType(), node.GetRight()->GetType()));
      } else {
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetLeft()->GetType()));
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetRight()->GetType()));        
      }
      break;

    default:
      SEMANTIC_ERROR(INTERNAL);
      break;
  }
  
  if (node.GetType().type == TYPE(INVALID) && m_success == true) {
    SEMANTIC_ERROR(INTERNAL);
  }
}


void cSemanticASTVisitor::VisitExpressionUnary(cASTExpressionUnary& node)
{
  node.GetExpression()->Accept(*this);
  
  switch (node.GetOperator()) {
    case TOKEN(OP_BIT_NOT):
      if (validBitwiseType(node.GetExpression()->GetType())) {
        node.SetType(node.GetExpression()->GetType());
      } else {
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_BIT_NOT)), mapType(node.GetExpression()->GetType())); 
      }
      break;
    case TOKEN(OP_LOGIC_NOT):
      checkCast(node.GetExpression()->GetType(), TYPEINFO(BOOL));
      node.SetType(TYPE(BOOL));
      break;
    case TOKEN(OP_SUB):
      switch (node.GetExpression()->GetType().type) {
        case TYPE(CHAR):
        case TYPE(FLOAT):
        case TYPE(INT):
        case TYPE(RUNTIME):
          // Char, Float and Int Okay
          
          node.SetType(node.GetExpression()->GetType());
          break;
        default:
          SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(TOKEN(OP_SUB)), mapType(node.GetExpression()->GetType())); 
          break;
      };
      
      break;
    default:
      SEMANTIC_ERROR(INTERNAL);
      break;
  }
}


void cSemanticASTVisitor::checkBuiltInCast(cASTBuiltInCall& node, ASType_t type)
{
  cASTArgumentList* args = node.GetArguments();
  if (!args || args->GetSize() != 1) SEMANTIC_ERROR(BUILTIN_CALL_SIGNATURE_MISMATCH, mapBuiltIn(node.GetBuiltIn()));
  else {
    cASTNode* argn = args->Iterator().Next();
    argn->Accept(*this);
    checkCast(argn->GetType(), sASTypeInfo(type));
  }  
  node.SetType(type);
}

void cSemanticASTVisitor::VisitBuiltInCall(cASTBuiltInCall& node)
{
#define ERR_BUILTIN_MISMATCH SEMANTIC_ERROR(BUILTIN_CALL_SIGNATURE_MISMATCH, mapBuiltIn(node.GetBuiltIn()))
  cASTArgumentList* args = node.GetArguments();
  cASTNode* trgt = node.GetTarget();
  bool allow_dict = false;
  
  switch (node.GetBuiltIn()) {
    case AS_BUILTIN_CAST_BOOL:    checkBuiltInCast(node, TYPE(BOOL)); break;
    case AS_BUILTIN_CAST_CHAR:    checkBuiltInCast(node, TYPE(CHAR)); break;
    case AS_BUILTIN_CAST_INT:     checkBuiltInCast(node, TYPE(INT)); break;
    case AS_BUILTIN_CAST_FLOAT:   checkBuiltInCast(node, TYPE(FLOAT)); break;
    case AS_BUILTIN_CAST_STRING:  checkBuiltInCast(node, TYPE(STRING)); break;
      
    case AS_BUILTIN_IS_ARRAY:
    case AS_BUILTIN_IS_BOOL:
    case AS_BUILTIN_IS_CHAR:
    case AS_BUILTIN_IS_DICT:
    case AS_BUILTIN_IS_INT:
    case AS_BUILTIN_IS_FLOAT:
    case AS_BUILTIN_IS_MATRIX:
    case AS_BUILTIN_IS_STRING:
      if (!args || args->GetSize() != 1) ERR_BUILTIN_MISMATCH;
      else {
        cASTNode* argn = args->Iterator().Next();
        argn->Accept(*this);
      }
      node.SetType(TYPE(BOOL));
      break;
      
    case AS_BUILTIN_CLEAR:
      trgt->Accept(*this);
      {
        ASType_t ttype = trgt->GetType().type;
        if (ttype != TYPE(ARRAY) && ttype != TYPE(DICT) && ttype != TYPE(MATRIX) && ttype != TYPE(RUNTIME)) ERR_BUILTIN_MISMATCH;
      }
      if (args) ERR_BUILTIN_MISMATCH;
      node.SetType(TYPE(VOID));
      break;
      
      
    case AS_BUILTIN_COPY:
      trgt->Accept(*this);
      {
        ASType_t ttype = trgt->GetType().type;
        if (ttype != TYPE(ARRAY) && ttype != TYPE(RUNTIME)) ERR_BUILTIN_MISMATCH;
      }
      if (args) ERR_BUILTIN_MISMATCH;
      node.SetType(TYPE(VOID));
      break;
      
    case AS_BUILTIN_HASKEY:
      trgt->Accept(*this);
      checkCast(trgt->GetType(), TYPEINFO(DICT));
      if (!args || args->GetSize() != 1) ERR_BUILTIN_MISMATCH;
      else {
        cASTNode* argn = args->Iterator().Next();
        argn->Accept(*this);
      }
      node.SetType(TYPE(BOOL));
      break;
      
    case AS_BUILTIN_KEYS:
    case AS_BUILTIN_VALUES:
      trgt->Accept(*this);
      checkCast(trgt->GetType(), TYPEINFO(DICT));
      if (args) ERR_BUILTIN_MISMATCH;
      node.SetType(TYPE(ARRAY));
      break;
      
    case AS_BUILTIN_LEN:
      trgt->Accept(*this);
      checkCast(trgt->GetType(), TYPEINFO(ARRAY));
      if (args) ERR_BUILTIN_MISMATCH;
      node.SetType(TYPE(INT));
      break;
      

    case AS_BUILTIN_REMOVE:
      allow_dict = true;
    case AS_BUILTIN_RESIZE:
      trgt->Accept(*this);

      {
        ASType_t ttype = trgt->GetType().type;
        if ((allow_dict && ttype != TYPE(ARRAY) && ttype != TYPE(DICT) && ttype != TYPE(RUNTIME)) || 
            (!allow_dict && ttype != TYPE(ARRAY) && ttype != TYPE(MATRIX) && ttype != TYPE(RUNTIME)))
          ERR_BUILTIN_MISMATCH;
        
        if (ttype == TYPE(ARRAY)) {
          if (args->GetSize() == 1) {
            cASTNode* argn = args->Iterator().Next();
            argn->Accept(*this);
            checkCast(argn->GetType(), TYPEINFO(INT));
          } else {
            ERR_BUILTIN_MISMATCH;
          }
        } else if (ttype == TYPE(DICT)) {
          if (args->GetSize() == 1) {
            cASTNode* argn = args->Iterator().Next();
            argn->Accept(*this);
          } else {
            ERR_BUILTIN_MISMATCH;
          }
        } else if (ttype == TYPE(MATRIX)) {
          if (args->GetSize() == 2) {
            tListIterator<cASTNode> it = args->Iterator();
            cASTNode* argn = it.Next();
            argn->Accept(*this);
            checkCast(argn->GetType(), TYPEINFO(INT));
            argn = it.Next();
            argn->Accept(*this);
            checkCast(argn->GetType(), TYPEINFO(INT));
          } else {
            ERR_BUILTIN_MISMATCH;
          }          
        }
      }

      node.SetType(TYPE(VOID));
      break;
      

    default:
      SEMANTIC_ERROR(INTERNAL);
      break;
  }

#undef ERR_BUILTIN_MISMATCH
}


void cSemanticASTVisitor::VisitFunctionCall(cASTFunctionCall& node)
{
  if (m_fun_def_arg) {
    SEMANTIC_ERROR(FUNCTION_DEFAULT_CALL_INVALID);
    return;
  }
  
  const cASFunction* libfun = NULL;
  int fun_id = -1;
  bool global = false;

  if (m_library->LookupFunction(node.GetName(), libfun)) {
    // Check function parameters for match to the signature
    cASTArgumentList* args = node.GetArguments();
    if (args && libfun->GetArity() == args->GetSize()) { 
      bool err = false;
      tListIterator<cASTNode> cit = node.GetArguments()->Iterator();
      cASTNode* an = NULL;
      for (int i = 0; i < libfun->GetArity(); i++) {
        an = cit.Next();
        an->Accept(*this);
        
        ASType_t in_type = an->GetType().type;
        ASType_t out_type = libfun->GetArgumentType(i).type;
        if (valid_cast[in_type][out_type]) {
          if ((in_type == TYPE(FLOAT) && out_type == TYPE(INT)) || (in_type == TYPE(INT) && out_type == TYPE(CHAR))) 
            SEMANTIC_WARNING(LOSS_OF_PRECISION, mapType(in_type), mapType(out_type)); 
        } else { 
          if (!err) {
            SEMANTIC_ERROR(FUNCTION_CALL_SIGNATURE_MISMATCH, (const char*)node.GetName());
            err = true;
          }
          SEMANTIC_ERROR(CANNOT_CAST, mapType(in_type), mapType(out_type)); 
        }
      }
    } else if (libfun->GetArity()) {
      SEMANTIC_ERROR(FUNCTION_CALL_SIGNATURE_MISMATCH, (const char*)node.GetName());
    }
    
    node.SetASFunction(libfun);
    node.SetType(libfun->GetReturnType());
    
  } else if (lookupFunction(node.GetName(), fun_id, global)) {
    cASTVariableDefinitionList* sig = (global ? m_global_symtbl : m_cur_symtbl)->GetFunctionSignature(fun_id);
    
    // Check function parameters for match to the signature
    cASTArgumentList* args = node.GetArguments();
    if (args) {
      if (sig && args->GetSize() <= sig->GetSize()) {
        
        bool err = false;
        tListIterator<cASTVariableDefinition> sit = sig->Iterator();
        tListIterator<cASTNode> cit = node.GetArguments()->Iterator();
        cASTNode* an = NULL;
        while ((an = cit.Next())) {
          an->Accept(*this);
          
          ASType_t in_type = an->GetType().type;
          ASType_t out_type = sit.Next()->GetType().type;
          if (valid_cast[in_type][out_type]) { 
            if ((in_type == TYPE(FLOAT) && out_type == TYPE(INT)) || (in_type == TYPE(INT) && out_type == TYPE(CHAR))) 
              SEMANTIC_WARNING(LOSS_OF_PRECISION, mapType(in_type), mapType(out_type)); 
          } else { 
            if (!err) {
              SEMANTIC_ERROR(FUNCTION_CALL_SIGNATURE_MISMATCH, (const char*)node.GetName());
              err = true;
            }
            SEMANTIC_ERROR(CANNOT_CAST, mapType(in_type), mapType(out_type)); 
          }
        }
        
        if (!sit.AtEnd() && !sit.Next()->GetAssignmentExpression()) 
          SEMANTIC_ERROR(ARGUMENT_MISSING_REQUIRED, (const char*)sit.Get()->GetName());
      } else {
        SEMANTIC_ERROR(FUNCTION_CALL_SIGNATURE_MISMATCH, (const char*)node.GetName());
      }
    } else {
      if (sig && sig->GetSize() && !sig->GetFirst()->GetAssignmentExpression()) {
        SEMANTIC_ERROR(FUNCTION_CALL_SIGNATURE_MISMATCH, (const char*)node.GetName());
        tListIterator<cASTVariableDefinition> it = sig->Iterator();
        cASTVariableDefinition* vd = NULL;
        while ((vd == it.Next())) {
          if (!vd->GetAssignmentExpression()) SEMANTIC_ERROR(ARGUMENT_MISSING_REQUIRED, (const char*)vd->GetName());
          else break;
        }
      }
    }
    
    node.SetFunc(fun_id, global);
    node.SetType((global ? m_global_symtbl : m_cur_symtbl)->GetFunctionRType(fun_id));
  } else {
    SEMANTIC_ERROR(FUNCTION_UNDECLARED, (const char*)node.GetName());
  }
}


void cSemanticASTVisitor::VisitLiteral(cASTLiteral& node)
{
  if (node.GetType() == TYPE(CHAR) && node.GetValue().GetSize() != 1) {
    SEMANTIC_ERROR(INVALID_CHAR_LITERAL);
  }
}


void cSemanticASTVisitor::VisitLiteralArray(cASTLiteralArray& node)
{
  cASTArgumentList* al = node.GetValues();
  if (al) {
    tListIterator<cASTNode> it = al->Iterator();
    cASTNode* alnode = NULL;
    while ((alnode = it.Next())) alnode->Accept(*this);
  }
  
  // Matrix dimension check must be performed at runtime
}


void cSemanticASTVisitor::VisitLiteralDict(cASTLiteralDict& node)
{
  tListIterator<cASTLiteralDict::sMapping> it = node.Iterator();
  cASTLiteralDict::sMapping* mapping = NULL;
  while ((mapping = it.Next())) {
    mapping->idx->Accept(*this);
    mapping->val->Accept(*this);
  }
}


void cSemanticASTVisitor::VisitObjectCall(cASTObjectCall& node)
{
  node.GetObject()->Accept(*this);
  checkCast(node.GetObject()->GetType(), TYPEINFO(OBJECT_REF));
  
  if (node.HasArguments()) { 
    tListIterator<cASTNode> it = node.GetArguments()->Iterator();
    cASTNode* an = NULL;
    while((an = it.Next())) an->Accept(*this);
  }
}


void cSemanticASTVisitor::VisitObjectReference(cASTObjectReference& node)
{
  node.GetObject()->Accept(*this);
  checkCast(node.GetObject()->GetType(), TYPEINFO(OBJECT_REF));
}


void cSemanticASTVisitor::VisitVariableReference(cASTVariableReference& node)
{
  if (m_fun_def_arg) {
    SEMANTIC_ERROR(FUNCTION_DEFAULT_VARIABLE_REF_INVALID);
    return;
  }

  int var_id = -1;
  bool global = false;
  if (lookupVariable(node.GetName(), var_id, global)) {
    node.SetVar(var_id, global);
    node.SetType(m_cur_symtbl->GetVariableType(var_id));
  } else {
    SEMANTIC_ERROR(VARIABLE_UNDEFINED, (const char*)node.GetName());
  }
}


void cSemanticASTVisitor::VisitUnpackTarget(cASTUnpackTarget& node)
{
  node.GetExpression()->Accept(*this);
  
  // Make sure that the expression can be used as an array
  checkCast(node.GetExpression()->GetType(), TYPEINFO(ARRAY));
  
  // Check each named variable and determine if it exists
  for (int var = 0; var < node.GetSize(); var++) {
    int var_id = -1;
    bool global = false;
    if (lookupVariable(node.GetVarName(var), var_id, global)) {
      node.SetVar(var, var_id, global, (global ? m_global_symtbl : m_cur_symtbl)->GetVariableType(var_id));
    } else {
      SEMANTIC_ERROR(VARIABLE_UNDEFINED, (const char*)node.GetVarName(var));
    }
  }
  
  // Check if last named is of array type
  const int last = node.GetSize() - 1;
  if (node.IsLastNamed() && node.GetVarID(last) != -1 && node.GetVarType(last) != TYPE(ARRAY))
    SEMANTIC_ERROR(UNPACK_WILD_NONARRAY, (const char*)node.GetVarName(last));
  
  // Rest of variable type checking must be done at runtime
}

void cSemanticASTVisitor::PostCheck()
{
  // Check all functions in the current scope level and make sure they have been defined
  cSymbolTable::cFunctionIterator fit = m_global_symtbl->ActiveFunctionIterator();
  while (fit.Next()) if (!fit.HasCode())
    reportError(AS_SEMANTIC_ERR_FUNCTION_UNDEFINED, m_global_symtbl->GetFunctionDefinition(0)->GetFilePosition()
                ,  __LINE__, (const char*)fit.GetName());
}



ASType_t cSemanticASTVisitor::getConsensusType(const sASTypeInfo& left, const sASTypeInfo& right)
{
  switch (left.type) {
    case TYPE(ARRAY):
      return TYPE(ARRAY);
    case TYPE(BOOL):
      switch (right.type) {
        case TYPE(ARRAY):
        case TYPE(BOOL):
        case TYPE(CHAR):
        case TYPE(FLOAT):
        case TYPE(INT):
        case TYPE(MATRIX):
        case TYPE(OBJECT_REF):
        case TYPE(STRING):
          return TYPE(BOOL);

        case TYPE(VAR):
        case TYPE(RUNTIME):
          return TYPE(RUNTIME);
        default: break;
      }
      break;
    case TYPE(CHAR):
      switch (right.type) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(CHAR);
        case TYPE(CHAR):      return TYPE(CHAR);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(INT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    return TYPE(STRING);
        case TYPE(VAR):       return TYPE(RUNTIME);
        case TYPE(RUNTIME):   return TYPE(RUNTIME);
        default: break;
      }
      break;
    case TYPE(FLOAT):
      switch (right.type) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(FLOAT);
        case TYPE(CHAR):      return TYPE(FLOAT);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(FLOAT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    return TYPE(FLOAT);
        case TYPE(VAR):       return TYPE(RUNTIME);
        case TYPE(RUNTIME):   return TYPE(RUNTIME);
        default: break;
      }
      break;
    case TYPE(INT):
      switch (right.type) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(INT);
        case TYPE(CHAR):      return TYPE(INT);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(INT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    return TYPE(INT);
        case TYPE(VAR):       return TYPE(RUNTIME);
        case TYPE(RUNTIME):   return TYPE(RUNTIME);
        default: break;
      }
      break;
    case TYPE(MATRIX):
      return TYPE(MATRIX);
    case TYPE(STRING):
      return TYPE(STRING);
      
    case TYPE(VAR):
      return TYPE(RUNTIME);
    
    case TYPE(RUNTIME):
      return TYPE(RUNTIME);

    default: break;
  }
  
  return TYPE(INVALID);
}

inline bool cSemanticASTVisitor::validArithmeticType(const sASTypeInfo& type, bool allow_matrix) const {
  switch (type.type) {
    case TYPE(MATRIX):
      return allow_matrix;
      
    case TYPE(RUNTIME):
    case TYPE(INT):
    case TYPE(CHAR):
    case TYPE(FLOAT):
      return true;
      
    default:
      return false;
  }
}


inline bool cSemanticASTVisitor::validBitwiseType(const sASTypeInfo& type) const {
  switch (type.type) {
    case TYPE(RUNTIME):
    case TYPE(INT):
    case TYPE(CHAR):
      // Char and Int Okay
      return true;
      
    default:
      return false;
  }
}

inline bool cSemanticASTVisitor::lookupVariable(const cString& name, int& var_id, bool& global) const
{
  if (m_cur_symtbl->LookupVariable(name, var_id)) {
    global = false;
    return true;
  } else if (m_cur_symtbl != m_global_symtbl && m_global_symtbl->LookupVariable(name, var_id)) {
    global = true;
    return true;
  } 
  
  return false;
}


inline bool cSemanticASTVisitor::lookupFunction(const cString& name, int& fun_id, bool& global) const
{
  if (m_cur_symtbl->LookupFunction(name, fun_id)) {
    global = false;
    return true;
  } else if (m_cur_symtbl != m_global_symtbl && m_global_symtbl->LookupFunction(name, fun_id)) {
    global = true;
    return true;
  } 
  
  return false;
}



void cSemanticASTVisitor::reportError(ASSemanticError_t err, const cASFilePosition& fp, const int line, ...)
{
#if DEBUG_AS_SEMANTIC
# define ERR_ENDL "  (cSemanticASTVisitor.cc:" << line << ")" << std::endl
#else
# define ERR_ENDL std::endl
#endif
  
#define VA_ARG_STR va_arg(vargs, const char*)
  
  std::cerr << fp.GetFilename() << ":" << fp.GetLineNumber();
  if (err < AS_SEMANTIC_WARN__LAST) std::cerr << ": warning: ";
  else { m_success = false; std::cerr << ": error: "; }
  
  va_list vargs;
  va_start(vargs, line);
  switch (err) {
    case AS_SEMANTIC_WARN_LOSS_OF_PRECISION:
      {
        const char* type1 = VA_ARG_STR;
        const char* type2 = VA_ARG_STR;
        std::cerr << "loss of precision occuring in cast of " << type1 << " to " << type2 << ERR_ENDL;
      }
      break;
    case AS_SEMANTIC_WARN_NO_DIMENSIONS:
      std::cerr << "no dimensions specified" << ERR_ENDL;
      break;
    case AS_SEMANTIC_WARN_NO_RETURN:
      std::cerr << "control reaches the end of non-void function" << ERR_ENDL;
      break;
    case AS_SEMANTIC_WARN_UNREACHABLE:
      std::cerr << "unreachable statement(s)" << ERR_ENDL;
      break;
      
    case AS_SEMANTIC_ERR_ARGUMENT_DEFAULT_REQUIRED:
      {
        const char* arg = VA_ARG_STR;
        const char* fun = VA_ARG_STR;
        std::cerr << "'" << arg << "' argument of '" << fun << "()' requires a default value" << ERR_ENDL;
      }
      break;
    case AS_SEMANTIC_ERR_ARGUMENT_MISSING_REQUIRED:
      std::cerr << "required argument " << VA_ARG_STR << " not found" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_BUILTIN_CALL_SIGNATURE_MISMATCH:
      std::cerr << "invalid invocation of builtin method '" << VA_ARG_STR << "()'" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_CANNOT_CAST:
      {
        const char* type1 = VA_ARG_STR;
        const char* type2 = VA_ARG_STR;
        std::cerr << "cannot cast " << type1 << " to " << type2 << ERR_ENDL;
      }
      break;
    case AS_SEMANTIC_ERR_CANNOT_COMPARE:
      std::cerr << "cannot compare values" << ERR_ENDL; 
      break;
    case AS_SEMANTIC_ERR_CANNOT_OVERRIDE_LIB_FUNCTION:
      std::cerr << "cannot override library method '" << VA_ARG_STR << "()'" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_FUNCTION_CALL_SIGNATURE_MISMATCH:
      std::cerr << "invalid call signature for '" << VA_ARG_STR << "()'" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_FUNCTION_DEFAULT_CALL_INVALID:
      std::cerr << "function call invalid in default value" << ERR_ENDL; 
      break;
    case AS_SEMANTIC_ERR_FUNCTION_DEFAULT_VARIABLE_REF_INVALID:
      std::cerr << "variable reference invalid in default value" << ERR_ENDL; 
      break;
    case AS_SEMANTIC_ERR_FUNCTION_REDEFINITION:
      std::cerr << "redefinition of '" << VA_ARG_STR << "()'" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_FUNCTION_RTYPE_MISMATCH:
      std::cerr << "return type of '" << VA_ARG_STR << "()' does not match declaration" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_FUNCTION_SIGNATURE_MISMATCH:
      std::cerr << "call signature of '" << VA_ARG_STR << "()' does not match declaration" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_FUNCTION_UNDECLARED:
      {
        cString varname = VA_ARG_STR;
        std::cerr << "'" << varname << "()' undeclared";
        cString nearmatch1 = m_cur_symtbl->FunctionNearMatch(varname);
        cString nearmatch2 = m_global_symtbl->FunctionNearMatch(varname);
        if (nearmatch1 != "") std::cerr << " - possible match '" << nearmatch1 << "()'";
        if (nearmatch2 != "" && nearmatch1 != nearmatch2) std::cerr << " or '" << nearmatch2 << "()'";
        std::cerr << ERR_ENDL;
      }
      break;
    case AS_SEMANTIC_ERR_FUNCTION_UNDEFINED:
      std::cerr << "'" << VA_ARG_STR << "()' declared but not defined" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_INVALID_ASSIGNMENT_TARGET:
      std::cerr << "invalid assignment target" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_INVALID_CHAR_LITERAL:
      std::cerr << "invalid char literal" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_TOO_MANY_ARGUMENTS:
      std::cerr << "too many arguments" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_UNDEFINED_TYPE_OP:
      {
        const char* op = VA_ARG_STR;
        const char* type = VA_ARG_STR;
        std::cerr << "'" << op << "' operation undefined for type '" << type << "'" << ERR_ENDL;
      }
      break;
    case AS_SEMANTIC_ERR_UNPACK_WILD_NONARRAY:
      std::cerr << "cannot unpack .. items into '" << VA_ARG_STR << "', variable must be an array" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_VARIABLE_DIMENSIONS_INVALID:
      {
        const char* var = VA_ARG_STR;
        const char* type = VA_ARG_STR;
        std::cerr << "dimensions of '" << var << "' invalid for type " << type << ERR_ENDL;
      }
      break;
    case AS_SEMANTIC_ERR_VARIABLE_UNDEFINED:
      {
        cString varname = VA_ARG_STR;
        std::cerr << "'" << varname << "' undefined";
        cString nearmatch = m_cur_symtbl->VariableNearMatch(varname);
        if (nearmatch != "") std::cerr << " - possible match '" << nearmatch << "'";
        std::cerr << ERR_ENDL;
      }
      break;
    case AS_SEMANTIC_ERR_VARIABLE_REDEFINITION:
      std::cerr << "redefining variable '" << VA_ARG_STR << "'" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_INTERNAL:
      std::cerr << "internal semantic analysis error at cSemanticASTVisitor.cc:" << line << std::endl;
      break;
    case AS_SEMANTIC_ERR_UNKNOWN:
      default:
      std::cerr << "unknown error" << std::endl;
  }
  va_end(vargs);
  
#undef ERR_ENDL
#undef VA_ARG_STR
}
                
#undef SEMANTIC_ERROR()
#undef SEMANTIC_WARNING()
#undef TOKEN()
#undef TYPE()
