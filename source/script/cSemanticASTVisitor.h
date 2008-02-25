/*
 *  cASTDumpVisitor.h
 *  Avida
 *
 *  Created by David on 7/12/07.
 *  Copyright 2007 Michigan State University. All rights reserved.
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

#include "tSmartArray.h"

class cASLibrary;
class cSymbolTable;


class cSemanticASTVisitor : public cASTVisitor
{
private:
  cASLibrary* m_library;
  cSymbolTable* m_global_symtbl;
  cSymbolTable* m_cur_symtbl;
  tSmartArray<cSymbolTable*> m_symtbl_stack;
  
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

};

#endif
