/*
 *  cASTVisitor.h
 *  Avida
 *
 *  Created by David on 7/11/07.
 *  Copyright 2007-2010 Michigan State University. All rights reserved.
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

#ifndef cASTVisitor_h
#define cASTVisitor_h

#include "ASTree.h"


class cASTVisitor
{
public:
  cASTVisitor() { ; }
  virtual ~cASTVisitor() { ; }

  
  virtual void VisitAssignment(cASTAssignment&) = 0;
  virtual void VisitArgumentList(cASTArgumentList&) = 0;
  virtual void VisitObjectAssignment(cASTObjectAssignment&) = 0;  
  
  virtual void VisitReturnStatement(cASTReturnStatement&) = 0;
  virtual void VisitStatementList(cASTStatementList&) = 0;
  
  virtual void VisitForeachBlock(cASTForeachBlock&) = 0;
  virtual void VisitIfBlock(cASTIfBlock&) = 0;
  virtual void VisitWhileBlock(cASTWhileBlock&) = 0;
  
  virtual void VisitFunctionDefinition(cASTFunctionDefinition&) = 0;
  virtual void VisitVariableDefinition(cASTVariableDefinition&) = 0;
  virtual void VisitVariableDefinitionList(cASTVariableDefinitionList&) = 0;

  virtual void VisitExpressionBinary(cASTExpressionBinary&) = 0;
  virtual void VisitExpressionUnary(cASTExpressionUnary&) = 0;

  virtual void VisitBuiltInCall(cASTBuiltInCall&) = 0;
  virtual void VisitFunctionCall(cASTFunctionCall&) = 0;
  virtual void VisitLiteral(cASTLiteral&) = 0;
  virtual void VisitLiteralArray(cASTLiteralArray&) = 0;
  virtual void VisitLiteralDict(cASTLiteralDict&) = 0;
  virtual void VisitObjectCall(cASTObjectCall&) = 0;
  virtual void VisitObjectReference(cASTObjectReference&) = 0;
  virtual void VisitVariableReference(cASTVariableReference&) = 0;
  virtual void VisitUnpackTarget(cASTUnpackTarget&) = 0;
};

#endif
