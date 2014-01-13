/*
 *  cDumpASTVisitor.h
 *  Avida
 *
 *  Created by David on 7/12/07.
 *  Copyright 2007-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cDumpASTVisitor_h
#define cDumpASTVisitor_h

#include "cASTVisitor.h"


class cDumpASTVisitor : public cASTVisitor
{
private:
  int m_depth;
  
public:
  cDumpASTVisitor();
  
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
  
  void VisitBuiltInCall(cASTBuiltInCall&);
  void VisitFunctionCall(cASTFunctionCall&);
  void VisitLiteral(cASTLiteral&);
  void VisitLiteralArray(cASTLiteralArray&);
  void VisitLiteralDict(cASTLiteralDict&);
  void VisitObjectCall(cASTObjectCall&);
  void VisitObjectReference(cASTObjectReference&);
  void VisitVariableReference(cASTVariableReference&);
  void VisitUnpackTarget(cASTUnpackTarget&);

private:
  inline void indent();
};

#endif
