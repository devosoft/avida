/*
 *  cSemanticASTVisitor.cc
 *  Avida
 *
 *  Created by David on 2/24/08.
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

#include "cSemanticASTVisitor.h"

#include "cASLibrary.h"
#include "cSymbolTable.h"

#include <cstdarg>

using namespace AvidaScript;


#define SEMANTIC_ERROR(code, ...) reportError(true, AS_SEMANTIC_ERR_ ## code, node.GetFilePosition(),  __LINE__, ##__VA_ARGS__)
#define SEMANTIC_WARNING(code, ...) reportError(false, AS_SEMANTIC_WARN_ ## code, node.GetFilePosition(),  __LINE__, ##__VA_ARGS__)

#define TOKEN(x) AS_TOKEN_ ## x
#define TYPE(x) AS_TYPE_ ## x

namespace AvidaScript {
  static const bool valid_cast[11][11] = {
  // ARRAY, BOOL , CHAR , FLOAT, INT  , @OBJ , MATRX, STRNG, RUNTM, VOID , INVLD
    {true , true , false, false, false, false, false, true , true , false, false}, // TYPE(ARRAY)
    {true , true , true , true , true , false, true , true , true , false, false}, // TYPE(BOOL)
    {true , true , true , true , true , false, true , true , true , false, false}, // TYPE(CHAR)
    {true , true , false, true , true , false, true , true , true , false, false}, // TYPE(FLOAT)
    {true , true , true , true , true , false, true , true , true , false, false}, // TYPE(INT)
    {true , true , false, false, false, true , false, true , false, false, false}, // TYPE(OBJECT_REF)
    {true , true , false, false, false, false, true , true , true , false, false}, // TYPE(MATRIX)
    {true , true , false, true , true , false, false, true , true , false, false}, // TYPE(STRNG)
    {true , true , true , true , true , true , true , true , true , false, false}, // TYPE(RUNTIME)
    {false, false, false, false, false, false, false, false, false, false, false}, // TYPE(VOID)
    {false, false, false, false, false, false, false, false, false, false, false}  // TYPE(INVALID)
  };
}


#define checkCast(in_type, out_type) { \
  if (valid_cast[in_type][out_type]) { \
    if ((in_type == TYPE(FLOAT) && out_type == TYPE(INT)) || (in_type == TYPE(INT) && out_type == TYPE(CHAR))) \
      SEMANTIC_WARNING(LOSS_OF_PRECISION, mapType(in_type), mapType(out_type)); \
  } else { \
    SEMANTIC_ERROR(CANNOT_CAST, mapType(in_type), mapType(out_type)); \
  } \
}


cSemanticASTVisitor::cSemanticASTVisitor(cASLibrary* lib, cSymbolTable* global_symtbl)
  : m_library(lib), m_global_symtbl(global_symtbl), m_parent_scope(global_symtbl), m_fun_id(0), m_cur_symtbl(global_symtbl)
{
  // Add internal definition of the global function
  m_global_symtbl->AddFunction("__asmain", TYPE(INT));
}


void cSemanticASTVisitor::visitAssignment(cASTAssignment& node)
{
  node.GetExpression()->Accept(*this);
  int var_id = -1;
  bool global = false;
  if (lookupVariable(node.GetVariable(), var_id, global)) {
    checkCast(node.GetExpression()->GetType(), (global ? m_global_symtbl : m_cur_symtbl)->GetVariableType(var_id));
  } else {
    SEMANTIC_ERROR(VARIABLE_UNDEFINED, (const char*)node.GetVariable());
  }
}


void cSemanticASTVisitor::visitReturnStatement(cASTReturnStatement& node)
{
  node.GetExpression()->Accept(*this);
  checkCast(m_parent_scope->GetFunctionRType(m_fun_id), node.GetExpression()->GetType());
  // @TODO - mark scope as containing return
}


void cSemanticASTVisitor::visitStatementList(cASTStatementList& node)
{
  tListIterator<cASTNode> it = node.Iterator();
  
  cASTNode* stmt = NULL;
  while ((stmt = it.Next())) {
    stmt->Accept(*this);
    // @TODO - check for unreachable statements
  }
}


void cSemanticASTVisitor::visitForeachBlock(cASTForeachBlock& node)
{
}


void cSemanticASTVisitor::visitIfBlock(cASTIfBlock& node)
{
}


void cSemanticASTVisitor::visitWhileBlock(cASTWhileBlock& node)
{
}



void cSemanticASTVisitor::visitFunctionDefinition(cASTFunctionDefinition& node)
{
}


void cSemanticASTVisitor::visitVariableDefinition(cASTVariableDefinition& node)
{
  if (!m_cur_symtbl->AddVariable(node.GetVariable(), node.GetType())) {
    SEMANTIC_ERROR(VARIABLE_REDEFINITION, (const char*)node.GetVariable());
  }
}


void cSemanticASTVisitor::visitVariableDefinitionList(cASTVariableDefinitionList& node)
{
}



void cSemanticASTVisitor::visitExpressionBinary(cASTExpressionBinary& node)
{
  node.GetLeft()->Accept(*this);
  node.GetRight()->Accept(*this);
  
  switch (node.GetOperator()) {
    case TOKEN(IDX_OPEN):
      checkCast(node.GetLeft()->GetType(), TYPE(ARRAY));
      checkCast(node.GetRight()->GetType(), TYPE(INT));
      node.SetType(TYPE(RUNTIME));
      break;
    case TOKEN(ARR_RANGE):
      checkCast(node.GetLeft()->GetType(), TYPE(INT));
      checkCast(node.GetRight()->GetType(), TYPE(INT));
      node.SetType(TYPE(ARRAY));
      break;
    case TOKEN(ARR_EXPAN):
      checkCast(node.GetRight()->GetType(), TYPE(INT));
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
    case TOKEN(OP_EQ):
    case TOKEN(OP_LE):
    case TOKEN(OP_GE):
    case TOKEN(OP_LT):
    case TOKEN(OP_GT):
    case TOKEN(OP_NEQ):
      checkCast(node.GetLeft()->GetType(), TYPE(BOOL));
      checkCast(node.GetRight()->GetType(), TYPE(BOOL));
      node.SetType(TYPE(BOOL));
      break;
      
    case TOKEN(OP_ADD):
      if (validArithmeticType(node.GetLeft()->GetType(), true) || validArithmeticType(node.GetRight()->GetType(), true) ||
          node.GetLeft()->GetType() == TYPE(STRING) || node.GetLeft()->GetType() == TYPE(ARRAY)) {
        node.SetType(getConsensusType(node.GetLeft()->GetType(), node.GetRight()->GetType()));
      } else {
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetLeft()->GetType()));
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetRight()->GetType()));        
      }
      break;
      
    case TOKEN(OP_SUB):
    case TOKEN(OP_MUL):
      if (validArithmeticType(node.GetLeft()->GetType(), true) || validArithmeticType(node.GetRight()->GetType(), true)) {
        node.SetType(getConsensusType(node.GetLeft()->GetType(), node.GetRight()->GetType()));
      } else {
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetLeft()->GetType()));
        SEMANTIC_ERROR(UNDEFINED_TYPE_OP, mapToken(node.GetOperator()), mapType(node.GetRight()->GetType()));        
      }
      break;
    
    case TOKEN(OP_DIV):
    case TOKEN(OP_MOD):
      if (validArithmeticType(node.GetLeft()->GetType()) || validArithmeticType(node.GetRight()->GetType())) {
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
  
  if (node.GetType() == TYPE(INVALID) && m_success == true) {
    SEMANTIC_ERROR(INTERNAL);
  }
}


void cSemanticASTVisitor::visitExpressionUnary(cASTExpressionUnary& node)
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
      checkCast(node.GetExpression()->GetType(), TYPE(BOOL));
      node.SetType(TYPE(BOOL));
      break;
    case TOKEN(OP_SUB):
      switch (node.GetExpression()->GetType()) {
        case TYPE(ARRAY):
        case TYPE(MATRIX):
          // Array and Matrix meta-op, validity must be determined at runtime
        case TYPE(CHAR):
        case TYPE(FLOAT):
        case TYPE(INT):
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


void cSemanticASTVisitor::visitArgumentList(cASTArgumentList& node)
{
}

void cSemanticASTVisitor::visitFunctionCall(cASTFunctionCall& node)
{
}


void cSemanticASTVisitor::visitLiteral(cASTLiteral& node)
{
  // Nothing to do here...   type already determined by the parser
}


void cSemanticASTVisitor::visitLiteralArray(cASTLiteralArray& node)
{
}


void cSemanticASTVisitor::visitObjectCall(cASTObjectCall& node)
{
}


void cSemanticASTVisitor::visitObjectReference(cASTObjectReference& node)
{
}


void cSemanticASTVisitor::visitVariableReference(cASTVariableReference& node)
{
  int var_id = -1;
  bool global = false;
  if (lookupVariable(node.GetName(), var_id, global)) {
    node.SetVar(var_id, global);
    node.SetType(m_cur_symtbl->GetVariableType(var_id));
  } else {
    SEMANTIC_ERROR(VARIABLE_UNDEFINED, (const char*)node.GetName());
  }
}


void cSemanticASTVisitor::visitUnpackTarget(cASTUnpackTarget& node)
{
  node.GetExpression()->Accept(*this);
  
  // Make sure that the expression can be used as an array
  checkCast(node.GetExpression()->GetType(), TYPE(ARRAY));
  
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

ASType_t cSemanticASTVisitor::getConsensusType(ASType_t left, ASType_t right)
{
  switch (left) {
    case TYPE(ARRAY):
      return TYPE(ARRAY);
    case TYPE(BOOL):
      switch (right) {
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
          return TYPE(RUNTIME);
        default: break;
      }
      break;
    case TYPE(CHAR):
      switch (right) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(CHAR);
        case TYPE(CHAR):      return TYPE(CHAR);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(INT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    return TYPE(STRING);
        case TYPE(RUNTIME):   return TYPE(RUNTIME);
        default: break;
      }
      break;
    case TYPE(FLOAT):
      switch (right) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(FLOAT);
        case TYPE(CHAR):      return TYPE(FLOAT);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(FLOAT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    return TYPE(FLOAT);
        case TYPE(RUNTIME):   return TYPE(RUNTIME);
        default: break;
      }
      break;
    case TYPE(INT):
      switch (right) {
        case TYPE(ARRAY):     return TYPE(ARRAY);
        case TYPE(BOOL):      return TYPE(INT);
        case TYPE(CHAR):      return TYPE(INT);
        case TYPE(FLOAT):     return TYPE(FLOAT);
        case TYPE(INT):       return TYPE(INT);
        case TYPE(MATRIX):    return TYPE(MATRIX);
        case TYPE(STRING):    return TYPE(INT);
        case TYPE(RUNTIME):   return TYPE(RUNTIME);
        default: break;
      }
      break;
    case TYPE(MATRIX):
      return TYPE(MATRIX);
    case TYPE(STRING):
      return TYPE(STRING);
    
    case TYPE(RUNTIME):
      return TYPE(RUNTIME);

    default: break;
  }
  
  return TYPE(INVALID);
}

inline bool cSemanticASTVisitor::validArithmeticType(ASType_t type, bool allow_matrix) const {
  switch (type) {
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


inline bool cSemanticASTVisitor::validBitwiseType(ASType_t type) const {
  switch (type) {
    case TYPE(ARRAY):
    case TYPE(MATRIX):
    case TYPE(RUNTIME):
      // Array and Matrix meta-op, validity must be determined at runtime
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



void cSemanticASTVisitor::reportError(bool fail, ASSemanticError_t err, const cASFilePosition& fp, const int line, ...)
{
#define ERR_ENDL "  (cSemanticASTVisitor.cc:" << line << ")" << std::endl
  
  if (fail) m_success = false;
  
  std::cerr << fp.GetFilename() << ":" << fp.GetLineNumber();
  if (err < AS_SEMANTIC_WARN__LAST) std::cerr << ": warning: ";
  else std::cerr << ": error: ";
  
  va_list vargs;
  va_start(vargs, line);
  switch (err) {
    case AS_SEMANTIC_WARN_LOSS_OF_PRECISION:
      std::cerr << "loss of precision occuring in cast of " << va_arg(vargs, const char*) << " to " 
                << va_arg(vargs, const char*) << ERR_ENDL;
      break;
    case AS_SEMANTIC_WARN_UNREACHABLE:
      std::cerr << "unreachable statement(s)" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_CANNOT_CAST:
      std::cerr << "cannot cast " << va_arg(vargs, const char*) << " to " << va_arg(vargs, const char*) << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_VARIABLE_UNDEFINED:
      {
        cString varname = va_arg(vargs, const char*);
        std::cerr << "'" << varname << "' undefined";
        cString nearmatch = m_cur_symtbl->VariableNearMatch(varname);
        if (nearmatch != "") std::cerr << " - possible match '" << nearmatch << "'";
        std::cerr << ERR_ENDL;
      }
      break;
    case AS_SEMANTIC_ERR_UNDEFINED_TYPE_OP:
      std::cerr << "'" << va_arg(vargs, const char*) << "' operation undefined for type '"
                << va_arg(vargs, const char*) << "'" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_UNPACK_WILD_NONARRAY:
      std::cerr << "cannot unpack ... items into '" << va_arg(vargs, const char*) << "', variable must be an array" << ERR_ENDL;
    case AS_SEMANTIC_ERR_VARIABLE_REDEFINITION:
      std::cerr << "redefining variable '" << va_arg(vargs, const char*) << "'" << ERR_ENDL;
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
}
                
#undef SEMANTIC_ERROR()
#undef SEMANTIC_WARNING()
#undef TOKEN()
#undef TYPE()
