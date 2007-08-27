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

#ifndef cASTDumpVisitor_h
#define cASTDumpVisitor_h

#ifndef cASTVisitor_h
#include "cASTVisitor.h"
#endif


class cASTDumpVisitor : public cASTVisitor
{
private:
  int m_depth;
  
public:
  cASTDumpVisitor();
  
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

private:
  inline void indent();
  void printToken(ASToken_t token);
  const char* mapType(ASType_t type);
};

#endif
