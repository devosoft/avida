/*
 *  cDumpASTVisitor.cc
 *  Avida
 *
 *  Created by David on 7/12/07.
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

#include "cDumpASTVisitor.h"

#include <iostream>

using namespace std;
using namespace AvidaScript;


cDumpASTVisitor::cDumpASTVisitor() : m_depth(0)
{
  cout << "main:" << endl;
  m_depth++;
}

inline void cDumpASTVisitor::indent()
{
  for (int i = 0; i < m_depth; i++) cout << "  ";
}

void cDumpASTVisitor::VisitAssignment(cASTAssignment& node)
{
  m_depth++;
  indent();
  cout << node.GetVariable() << endl;
  m_depth--;
  
  indent();
  cout << "=" << endl;

  m_depth++;
  node.GetExpression()->Accept(*this);
  m_depth--;
}


void cDumpASTVisitor::VisitArgumentList(cASTArgumentList& node)
{
  m_depth++;
  
  tListIterator<cASTNode> it = node.Iterator();
  cASTNode* val = NULL;
  while ((val = it.Next())) val->Accept(*this);
  
  m_depth--;
}

void cDumpASTVisitor::VisitObjectAssignment(cASTObjectAssignment& node)
{
  m_depth++;
  node.GetTarget()->Accept(*this);
  m_depth--;
  
  indent();
  cout << "=" << endl;
  
  m_depth++;
  node.GetExpression()->Accept(*this);
  m_depth--;
}

void cDumpASTVisitor::VisitReturnStatement(cASTReturnStatement& node)
{
  indent();
  cout << "return:" << endl;
  
  m_depth++;
  node.GetExpression()->Accept(*this);
  m_depth--;  
}


void cDumpASTVisitor::VisitStatementList(cASTStatementList& node)
{
  tListIterator<cASTNode> it = node.Iterator();
  
  cASTNode* stmt = NULL;
  while ((stmt = it.Next())) {
    stmt->Accept(*this);
  }
}



void cDumpASTVisitor::VisitForeachBlock(cASTForeachBlock& node)
{
  indent();
  cout << "foreach:" << endl;
  
  m_depth++;
  node.GetVariable()->Accept(*this);
  
  indent();
  cout << "values:" << endl;
  m_depth++;
  node.GetValues()->Accept(*this);
  
  m_depth--;
  indent();
  cout << "code:" << endl;

  m_depth++;
  node.GetCode()->Accept(*this);
  m_depth--;
  
  m_depth--;
}


void cDumpASTVisitor::VisitIfBlock(cASTIfBlock& node)
{
  indent();
  cout << "if:" << endl;
  
  m_depth++;
  indent();
  cout << "condition:" << endl;
  
  m_depth++;
  node.GetCondition()->Accept(*this);
  m_depth--;
  
  indent();
  cout << "do:" << endl;
  
  m_depth++;
  node.GetCode()->Accept(*this);
  m_depth--;
  
  if (node.HasElseIfs()) {
    tListIterator<cASTIfBlock::cElseIf> it = node.ElseIfIterator();
    cASTIfBlock::cElseIf* elif = NULL;
    while ((elif = it.Next())) {
      indent();
      cout << "elseif:" << endl;

      m_depth++;
      indent();
      cout << "condition:" << endl;
      
      m_depth++;
      elif->GetCondition()->Accept(*this);
      m_depth--;
      
      indent();
      cout << "do:" << endl;
      
      m_depth++;
      elif->GetCode()->Accept(*this);
      m_depth--;
      
      m_depth--;
    }
  }
  
  if (node.HasElse()) {
    indent();
    cout << "else:" << endl;
    
    m_depth++;
    node.GetElseCode()->Accept(*this);
    m_depth--;
  }
  
  m_depth--;
  
}


void cDumpASTVisitor::VisitWhileBlock(cASTWhileBlock& node)
{
  indent();
  cout << "while:" << endl;
  
  m_depth++;
  indent();
  cout << "condition:" << endl;
  
  m_depth++;
  node.GetCondition()->Accept(*this);
  m_depth--;
  
  indent();
  cout << "do:" << endl;
  
  m_depth++;
  node.GetCode()->Accept(*this);
  m_depth--;
  
  m_depth--;
}



void cDumpASTVisitor::VisitFunctionDefinition(cASTFunctionDefinition& node)
{
  indent();
  cout << (node.GetCode() ? "":"@") << "function: " << mapType(node.GetType()) << " " << node.GetName() << "(";
  if (node.GetArguments()->GetSize()) { 
    cout << endl;
    node.GetArguments()->Accept(*this);
    indent();
  }
  cout << ")" << endl;
  
  if (node.GetCode()) {
    indent();
    cout << "{" << endl;
    
    m_depth++;
    node.GetCode()->Accept(*this);
    m_depth--;

    indent();
    cout << "}" << endl;
  }
}


void cDumpASTVisitor::VisitVariableDefinition(cASTVariableDefinition& node)
{
  indent();
  cout << mapType(node.GetType()) << " " << node.GetName() << endl;
  
  if (node.GetDimensions()) {
    m_depth++;
    indent();
    cout << "dimensions:" << endl;
    
    m_depth++;
    node.GetDimensions()->Accept(*this);

    m_depth -= 2;
  }
  
  if (node.GetAssignmentExpression()) {
    m_depth++;
    indent();
    cout << "=" << endl;
    
    m_depth++;
    node.GetAssignmentExpression()->Accept(*this);

    m_depth -= 2;
  }  
}


void cDumpASTVisitor::VisitVariableDefinitionList(cASTVariableDefinitionList& node)
{
  m_depth++;
  
  tListIterator<cASTVariableDefinition> it = node.Iterator();
  cASTNode* val = NULL;
  while ((val = it.Next())) val->Accept(*this);
  
  m_depth--;
}



void cDumpASTVisitor::VisitExpressionBinary(cASTExpressionBinary& node)
{
  m_depth++;
  node.GetLeft()->Accept(*this);
  m_depth--;
  
  indent();
  cout << mapToken(node.GetOperator());
  cout << endl;
  
  m_depth++;
  node.GetRight()->Accept(*this);
  m_depth--;  
}


void cDumpASTVisitor::VisitExpressionUnary(cASTExpressionUnary& node)
{
  indent();
  cout << mapToken(node.GetOperator());
  cout << endl;
  
  m_depth++;
  node.GetExpression()->Accept(*this);
  m_depth--;
}


void cDumpASTVisitor::VisitBuiltInCall(cASTBuiltInCall& node)
{
  indent();
  cout << "builtin:" << endl;
  m_depth++;
  
  indent();
  cout << "method:" << endl;
  
  m_depth++;
  indent();
  cout << mapBuiltIn(node.GetBuiltIn()) << endl;
  m_depth--;
  
  if (node.HasArguments()) {
    indent();
    cout << "with:" << endl;
    
    m_depth++;
    node.GetArguments()->Accept(*this);
    m_depth--;
  }
  
  m_depth--;
}


void cDumpASTVisitor::VisitFunctionCall(cASTFunctionCall& node)
{
  indent();
  cout << "call:" << endl;
  m_depth++;
  
  indent();
  cout << "target:" << endl;
  
  m_depth++;
  indent();
  cout << node.GetName() << endl;
  m_depth--;
  
  if (node.HasArguments()) {
    indent();
    cout << "with:" << endl;
    
    m_depth++;
    node.GetArguments()->Accept(*this);
    m_depth--;
  }
  
  m_depth--;
}


void cDumpASTVisitor::VisitLiteral(cASTLiteral& node)
{
  indent();
  cout << "(" << mapType(node.GetType()) << ") " << node.GetValue() << endl;
}


void cDumpASTVisitor::VisitLiteralArray(cASTLiteralArray& node)
{
  indent();
  if (node.IsMatrix()) cout << "$";
  cout << "{" << endl;
  m_depth++;
  
  node.GetValues()->Accept(*this);
  
  m_depth--;
  indent();
  cout << "}" << endl;
}


void cDumpASTVisitor::VisitLiteralDict(cASTLiteralDict& node)
{
  indent();
  cout << "@{" << endl;
  m_depth++;
  
  m_depth++;
  tListIterator<cASTLiteralDict::sMapping> it = node.Iterator();
  cASTLiteralDict::sMapping* mapping = NULL;
  while ((mapping = it.Next())) {
    mapping->idx->Accept(*this);
    m_depth--;
    indent();
    cout << "=>" << endl;
    m_depth++;
    mapping->val->Accept(*this);
  }
  m_depth--;

  m_depth--;
  indent();
  cout << "}" << endl;
  
}


void cDumpASTVisitor::VisitObjectCall(cASTObjectCall& node)
{
  indent();
  cout << "call:" << endl;
  m_depth++;
  
  indent();
  cout << "target:" << endl;
  
  m_depth++;
  node.GetObject()->Accept(*this);
  m_depth--;
  
  if (node.HasArguments()) {
    indent();
    cout << "with:" << endl;
    
    m_depth++;
    node.GetArguments()->Accept(*this);
    m_depth--;
  }
  
  m_depth--;

}

void cDumpASTVisitor::VisitObjectReference(cASTObjectReference& node)
{
  m_depth++;
  node.GetObject()->Accept(*this);
  m_depth--;
  
  indent();
  cout << mapToken(AS_TOKEN_DOT) << endl;
  
  m_depth++;
  indent();
  cout << node.GetName() << endl;
  m_depth--;  
}

void cDumpASTVisitor::VisitVariableReference(cASTVariableReference& node)
{
  indent();
  cout << node.GetName() << endl;
}


void cDumpASTVisitor::VisitUnpackTarget(cASTUnpackTarget& node)
{
  m_depth++;
  
  // Array unpack portion
  indent();
  cout << "@{";
  m_depth++;
  
  for (int i = 0; i < node.GetSize(); i++) {
    cout << endl;
    indent();
    cout << node.GetVarName(i);
  }
  if (node.IsLastNamed()) {
    cout << "..";
  } else if (node.IsLastWild()) {
    cout << endl;
    indent();
    cout << "..";
  }
  cout << endl;
  m_depth--;
  indent();
  cout << "}" << endl;
  
  // Equals
  m_depth--;
  indent();
  cout << "=" << endl;
  m_depth++;
  
  // Expression portion
  node.GetExpression()->Accept(*this);
  
  m_depth--;
}
