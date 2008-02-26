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


#define SEMANTIC_ERROR(code, ...) reportError(true, AS_SEMANTIC_ERR_ ## code, node.GetFilePosition(),  __LINE__, __VA_ARGS__)
#define SEMANTIC_WARNING(code, ...) reportError(false, AS_SEMANTIC_WARN_ ## code, node.GetFilePosition(),  __LINE__, __VA_ARGS__)


cSemanticASTVisitor::cSemanticASTVisitor(cASLibrary* lib, cSymbolTable* global_symtbl)
  : m_library(lib), m_global_symtbl(global_symtbl), m_parent_scope(global_symtbl), m_fun_id(0), m_cur_symtbl(global_symtbl)
{
  // Add internal definition of the global function
  m_global_symtbl->AddFunction("__asmain", AS_TYPE_INT);
}

void cSemanticASTVisitor::visitAssignment(cASTAssignment& node)
{
  node.GetExpression()->Accept(*this);
  int var_id = -1;
  if (m_cur_symtbl->LookupVariable(node.GetVariable(), var_id)) {
    checkCast(node.GetExpression()->GetType(), m_cur_symtbl->GetVariableType(var_id));
  } else if (m_cur_symtbl != m_global_symtbl && m_global_symtbl->LookupVariable(node.GetVariable(), var_id)) {
    checkCast(node.GetExpression()->GetType(), m_global_symtbl->GetVariableType(var_id));    
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
}


void cSemanticASTVisitor::visitExpressionUnary(cASTExpressionUnary& node)
{
}


void cSemanticASTVisitor::visitArgumentList(cASTArgumentList& node)
{
}

void cSemanticASTVisitor::visitFunctionCall(cASTFunctionCall& node)
{
}


void cSemanticASTVisitor::visitLiteral(cASTLiteral& node)
{
}


void cSemanticASTVisitor::visitLiteralArray(cASTLiteralArray& node)
{
}


void cSemanticASTVisitor::visitVariableReference(cASTVariableReference& node)
{
}


void cSemanticASTVisitor::visitUnpackTarget(cASTUnpackTarget& node)
{
}



void cSemanticASTVisitor::checkCast(ASType_t in_type, ASType_t out_type)
{
  
}


void cSemanticASTVisitor::reportError(bool fail, ASSemanticError_t err, const cASFilePosition& fp, const int line, ...)
{
#define ERR_ENDL "  (cSemanticASTVisitor.cc:" << line << ")" << std::endl
  
  if (fail) m_success = false;
  
  std::cerr << fp.GetFilename() << ":" << fp.GetLineNumber();
  if (err < AS_SEMANTIC_WARN__LAST) std::cerr << ": warning: ";
  else std::cerr << ": error: ";
  
  va_list info_list;
  va_start(info_list, line);
  switch (err) {
    case AS_SEMANTIC_WARN_UNREACHABLE:
      std::cerr << "unreachable statement(s)" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_VARIABLE_UNDEFINED:
      {
        cString varname = va_arg(info_list, const char*);
        std::cerr << "'" << varname << "' undefined";
        cString nearmatch = m_cur_symtbl->VariableNearMatch(varname);
        if (nearmatch != "") std::cerr << " - possible match '" << nearmatch << "'";
        std::cerr << ERR_ENDL;
      }
      break;
    case AS_SEMANTIC_ERR_VARIABLE_REDEFINITION:
      std::cerr << "redefining variable '" << va_arg(info_list, const char*) << "'" << ERR_ENDL;
      break;
    case AS_SEMANTIC_ERR_INTERNAL:
      std::cerr << "internal semantic analysis error at cSemanticASTVisitor.cc:" << line << std::endl;
      break;
    case AS_SEMANTIC_ERR_UNKNOWN:
      default:
      std::cerr << "unknown error" << std::endl;
  }
  va_end(info_list);
  
#undef ERR_ENDL
}

#undef SEMANTIC_ERROR()
