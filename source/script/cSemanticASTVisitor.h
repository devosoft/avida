/*
 *  cDumpASTVisitor.h
 *  Avida
 *
 *  Created by David on 7/12/07.
 *  Copyright 2007-2008 Michigan State University. All rights reserved.
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

#ifndef cSemanticASTVisitor_h
#define cSemanticASTVisitor_h

#include "cASTVisitor.h"

#include "tArray.h"
#include "tSmartArray.h"

class cASLibrary;
class cSymbolTable;


class cSemanticASTVisitor : public cASTVisitor
{
private:
  // --------  Internal Type Declarations  --------
  struct sFunctionEntry
  {
    cSymbolTable* parent_scope;
    int fun_id;
    
    cSymbolTable* fun_symtbl;
    
    sFunctionEntry() : parent_scope(NULL), fun_id(-1) { ; }
    sFunctionEntry(cSymbolTable* ps, int in_fun_id) : parent_scope(ps), fun_id(in_fun_id) { ; }
  };
  
  
  // --------  Internal Variables  --------
  cASLibrary* m_library;
  cSymbolTable* m_global_symtbl;
  cSymbolTable* m_parent_scope;
  int m_fun_id;
  cSymbolTable* m_cur_symtbl;
  
  tSmartArray<sFunctionEntry> m_fun_stack;
  
  bool m_success;
  bool m_fun_def;
  bool m_top_level;
  bool m_call_expr;

  
  // --------  Private Constructors  --------
  cSemanticASTVisitor(); // @not_implemented
  cSemanticASTVisitor(const cSemanticASTVisitor&); // @not_implemented
  cSemanticASTVisitor& operator=(const cSemanticASTVisitor&); // @not_implemented
  
  
public:
  cSemanticASTVisitor(cASLibrary* lib, cSymbolTable* global_symtbl, cASTNode* main);
  
  inline bool WasSuccessful() { return m_success; }
  
  
  void VisitAssignment(cASTAssignment&);
  void VisitArgumentList(cASTArgumentList&);
  void VisitObjectAssignment(cASTObjectAssignment&);
  
  void VisitReturnStatement(cASTReturnStatement&);
  void VisitStatementList(cASTStatementList&);
  
  void VisitForeachBlock(cASTForeachBlock&);
  void VisitIfBlock(cASTIfBlock&);
  void VisitWhileBlock(cASTWhileBlock&);
  
  void VisitFunctionDefinition(cASTFunctionDefinition&);
  void VisitVariableDefinition(cASTVariableDefinition&);
  void VisitVariableDefinitionList(cASTVariableDefinitionList&);
  
  void VisitExpressionBinary(cASTExpressionBinary&);
  void VisitExpressionUnary(cASTExpressionUnary&);
  
  void VisitFunctionCall(cASTFunctionCall&);
  void VisitLiteral(cASTLiteral&);
  void VisitLiteralArray(cASTLiteralArray&);
  void VisitObjectCall(cASTObjectCall&);
  void VisitObjectReference(cASTObjectReference&);
  void VisitVariableReference(cASTVariableReference&);
  void VisitUnpackTarget(cASTUnpackTarget&);

  void PostCheck();


private:
  // --------  Internal Utility Methods  --------
  ASType_t getConsensusType(ASType_t t1, ASType_t t2);
  inline bool validArithmeticType(ASType_t type, bool allow_matrix = false) const;
  inline bool validBitwiseType(ASType_t type) const;
  
  inline bool lookupVariable(const cString& name, int& var_id, bool& global) const;
  inline bool lookupFunction(const cString& name, int& fun_id, bool& global) const;
  
  void reportError(ASSemanticError_t err, const cASFilePosition& fp, const int line, ...);
};

#endif
