/*
 *  ASTree.cpp
 *  Avida
 *
 *  Created by David on 4/7/07.
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

#include "ASTree.h"

#include "cASTVisitor.h"


void cASTAssignment::Accept(cASTVisitor& visitor) { visitor.visitAssignment(*this); }

void cASTReturnStatement::Accept(cASTVisitor& visitor) { visitor.visitReturnStatement(*this); }
void cASTStatementList::Accept(cASTVisitor& visitor) { visitor.visitStatementList(*this); }

void cASTForeachBlock::Accept(cASTVisitor& visitor) { visitor.visitForeachBlock(*this); }
void cASTIfBlock::Accept(cASTVisitor& visitor) { visitor.visitIfBlock(*this); }
void cASTWhileBlock::Accept(cASTVisitor& visitor) { visitor.visitWhileBlock(*this); }

void cASTFunctionDefinition::Accept(cASTVisitor& visitor) { visitor.visitFunctionDefinition(*this); }
void cASTVariableDefinition::Accept(cASTVisitor& visitor) { visitor.visitVariableDefinition(*this); }
void cASTVariableDefinitionList::Accept(cASTVisitor& visitor) { visitor.visitVariableDefinitionList(*this); }

void cASTExpressionBinary::Accept(cASTVisitor& visitor) { visitor.visitExpressionBinary(*this); }
void cASTExpressionUnary::Accept(cASTVisitor& visitor) { visitor.visitExpressionUnary(*this); }

void cASTArgumentList::Accept(cASTVisitor& visitor) { visitor.visitArgumentList(*this); }
void cASTFunctionCall::Accept(cASTVisitor& visitor) { visitor.visitFunctionCall(*this); }
void cASTLiteral::Accept(cASTVisitor& visitor) { visitor.visitLiteral(*this); }
void cASTLiteralArray::Accept(cASTVisitor& visitor) { visitor.visitLiteralArray(*this); }
void cASTVariableReference::Accept(cASTVisitor& visitor) { visitor.visitVariableReference(*this); }


cASTStatementList::~cASTStatementList()
{
  cASTNode* node = NULL;
  tListIterator<cASTNode> it(m_nodes);
  while ((node = it.Next())) delete node;
}
