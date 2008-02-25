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


cSemanticASTVisitor::cSemanticASTVisitor(cASLibrary* lib, cSymbolTable* global_symtbl)
  : m_library(lib), m_global_symtbl(global_symtbl), m_cur_symtbl(NULL)
{
}

void cSemanticASTVisitor::visitAssignment(cASTAssignment& node)
{
}


void cSemanticASTVisitor::visitReturnStatement(cASTReturnStatement& node)
{
}


void cSemanticASTVisitor::visitStatementList(cASTStatementList& node)
{
  tListIterator<cASTNode> it = node.Iterator();
  
  cASTNode* stmt = NULL;
  while ((stmt = it.Next())) {
    stmt->Accept(*this);
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
