/*
 *  ASTree.cpp
 *  Avida
 *
 *  Created by David on 4/7/07.
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

#include "ASTree.h"

#include "cASTVisitor.h"


const sASTypeInfo cASTNode::s_invalid_type(AS_TYPE_INVALID);


void cASTAssignment::Accept(cASTVisitor& visitor) { visitor.VisitAssignment(*this); }
void cASTArgumentList::Accept(cASTVisitor& visitor) { visitor.VisitArgumentList(*this); }
void cASTObjectAssignment::Accept(cASTVisitor& visitor) { visitor.VisitObjectAssignment(*this); }


void cASTReturnStatement::Accept(cASTVisitor& visitor) { visitor.VisitReturnStatement(*this); }
void cASTStatementList::Accept(cASTVisitor& visitor) { visitor.VisitStatementList(*this); }

void cASTForeachBlock::Accept(cASTVisitor& visitor) { visitor.VisitForeachBlock(*this); }
void cASTIfBlock::Accept(cASTVisitor& visitor) { visitor.VisitIfBlock(*this); }
void cASTWhileBlock::Accept(cASTVisitor& visitor) { visitor.VisitWhileBlock(*this); }

void cASTFunctionDefinition::Accept(cASTVisitor& visitor) { visitor.VisitFunctionDefinition(*this); }
void cASTVariableDefinition::Accept(cASTVisitor& visitor) { visitor.VisitVariableDefinition(*this); }
void cASTVariableDefinitionList::Accept(cASTVisitor& visitor) { visitor.VisitVariableDefinitionList(*this); }

void cASTExpressionBinary::Accept(cASTVisitor& visitor) { visitor.VisitExpressionBinary(*this); }
void cASTExpressionUnary::Accept(cASTVisitor& visitor) { visitor.VisitExpressionUnary(*this); }

void cASTBuiltInCall::Accept(cASTVisitor& visitor) { visitor.VisitBuiltInCall(*this); }
void cASTFunctionCall::Accept(cASTVisitor& visitor) { visitor.VisitFunctionCall(*this); }
void cASTLiteral::Accept(cASTVisitor& visitor) { visitor.VisitLiteral(*this); }
void cASTLiteralArray::Accept(cASTVisitor& visitor) { visitor.VisitLiteralArray(*this); }
void cASTObjectCall::Accept(cASTVisitor& visitor) { visitor.VisitObjectCall(*this); }
void cASTObjectReference::Accept(cASTVisitor& visitor) { visitor.VisitObjectReference(*this); }
void cASTVariableReference::Accept(cASTVisitor& visitor) { visitor.VisitVariableReference(*this); }
void cASTUnpackTarget::Accept(cASTVisitor& visitor) { visitor.VisitUnpackTarget(*this); }


cASTStatementList::~cASTStatementList()
{
  cASTNode* node = NULL;
  tListIterator<cASTNode> it(m_nodes);
  while ((node = it.Next())) delete node;
}

cASTFunctionDefinition::~cASTFunctionDefinition()
{
  delete m_args;
  delete m_code;
}


cASTBuiltInCall::cASTBuiltInCall(const cASFilePosition& fp, const cString& name)
  : cASTNode(fp), m_args(NULL), m_type(AS_TYPE_INVALID), m_builtin(AS_BUILTIN_UNKNOWN), m_vr(NULL)
{
  if (name == "asbool") m_builtin = AS_BUILTIN_CAST_BOOL;
  else if (name == "aschar") m_builtin = AS_BUILTIN_CAST_CHAR;
  else if (name == "asint") m_builtin = AS_BUILTIN_CAST_INT;
  else if (name == "asfloat") m_builtin = AS_BUILTIN_CAST_FLOAT;
  else if (name == "asstring") m_builtin = AS_BUILTIN_CAST_STRING;
  else if (name == "len") m_builtin = AS_BUILTIN_LEN;
  else if (name == "resize") m_builtin = AS_BUILTIN_RESIZE;
}

cASTBuiltInCall::~cASTBuiltInCall()
{
  delete m_args;
  delete m_vr;
}

void cASTBuiltInCall::SetVariableReference(cASTVariableReference* vr)
{
  delete m_vr;
  m_vr = vr;
}
