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
  cASLibrary* m_library;
  cSymbolTable* m_global_symtbl;
  cSymbolTable* m_parent_scope;
  int m_fun_id;
  cSymbolTable* m_cur_symtbl;
  
  struct sFunctionEntry
  {
    cSymbolTable* scope;
    int fun_id;

    cSymbolTable* fun_symtbl;
    
    sFunctionEntry() : scope(NULL), fun_id(-1) { ; }
  };
  tSmartArray<sFunctionEntry> m_fun_stack;

  
  bool m_success;

  
  cSemanticASTVisitor(); // @not_implemented
  cSemanticASTVisitor(const cSemanticASTVisitor&); // @not_implemented
  cSemanticASTVisitor& operator=(const cSemanticASTVisitor&); // @not_implemented
  
  
public:
  cSemanticASTVisitor(cASLibrary* lib, cSymbolTable* global_symtbl);
  
  void visitAssignment(cASTAssignment&);
  
  void visitReturnStatement(cASTReturnStatement&);
  void visitStatementList(cASTStatementList&);
  
  void visitForeachBlock(cASTForeachBlock&);
  void visitIfBlock(cASTIfBlock&);
  void visitWhileBlock(cASTWhileBlock&);
  
  void visitFunctionDefinition(cASTFunctionDefinition&);
  void visitVariableDefinition(cASTVariableDefinition&);
  void visitVariableDefinitionList(cASTVariableDefinitionList&);
  
  void visitExpressionBinary(cASTExpressionBinary&);
  void visitExpressionUnary(cASTExpressionUnary&);
  
  void visitArgumentList(cASTArgumentList&);
  void visitFunctionCall(cASTFunctionCall&);
  void visitLiteral(cASTLiteral&);
  void visitLiteralArray(cASTLiteralArray&);
  void visitVariableReference(cASTVariableReference&);
  void visitUnpackTarget(cASTUnpackTarget&);

private:
  ASType_t getConsensusType(ASType_t t1, ASType_t t2);
  inline bool validArithmeticType(ASType_t type, bool allow_matrix = false) const;
  inline bool validBitwiseType(ASType_t type) const;
  void reportError(bool fail, ASSemanticError_t err, const cASFilePosition& fp, const int line, ...);
};
#endif
