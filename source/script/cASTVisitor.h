/*
 *  cASTVisitor.h
 *  Avida
 *
 *  Created by David on 7/11/07.
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

#ifndef cASTVisitor_h
#define cASTVisitor_h

#ifndef ASTree_h
#include "ASTree.h"
#endif


class cASTVisitor
{
public:
  cASTVisitor() { ; }
  virtual ~cASTVisitor() { ; }

  
  virtual void visitAssignment(cASTAssignment&) = 0;
  
  virtual void visitStatementList(cASTStatementList&) = 0;
  
  virtual void visitForeachBlock(cASTForeachBlock&) = 0;
  virtual void visitIfBlock(cASTIfBlock&) = 0;
  virtual void visitWhileBlock(cASTWhileBlock&) = 0;
  
  virtual void visitFunctionDefinition(cASTFunctionDefinition&) = 0;
  virtual void visitVariableDefinition(cASTVariableDefinition&) = 0;

  virtual void visitExpressionBinary(cASTExpressionBinary&) = 0;
  virtual void visitExpressionUnary(cASTExpressionUnary&) = 0;

  virtual void visitFunctionCall(cASTFunctionCall&) = 0;
  virtual void visitLiteral(cASTLiteral&) = 0;
  virtual void visitVariableReference(cASTVariableReference&) = 0;
};

#endif
