/*
 *  cASTDumpVisitor.cc
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

#include "cASTDumpVisitor.h"

#include <iostream>

using namespace std;


cASTDumpVisitor::cASTDumpVisitor() : m_depth(0)
{
  cout << "main:" << endl;
  m_depth++;
}

inline void cASTDumpVisitor::indent()
{
  for (int i = 0; i < m_depth; i++) cout << "  ";
}

void cASTDumpVisitor::printToken(ASToken_t token)
{
  switch (token) {
    case AS_TOKEN_OP_BIT_NOT:   cout << '~';  break;
    case AS_TOKEN_OP_BIT_AND:   cout << '&';  break;
    case AS_TOKEN_OP_BIT_OR:    cout << '|';  break;
    case AS_TOKEN_OP_LOGIC_NOT: cout << '!';  break;
    case AS_TOKEN_OP_LOGIC_AND: cout << "&&"; break;
    case AS_TOKEN_OP_LOGIC_OR:  cout << "||"; break;
    case AS_TOKEN_OP_ADD:       cout << '+';  break;
    case AS_TOKEN_OP_SUB:       cout << '-';  break;
    case AS_TOKEN_OP_MUL:       cout << '*';  break;
    case AS_TOKEN_OP_DIV:       cout << '/';  break;
    case AS_TOKEN_OP_MOD:       cout << '%';  break;
    case AS_TOKEN_OP_EQ:        cout << "=="; break;
    case AS_TOKEN_OP_LE:        cout << "<="; break;
    case AS_TOKEN_OP_GE:        cout << ">="; break;
    case AS_TOKEN_OP_LT:        cout << '<';  break;
    case AS_TOKEN_OP_GT:        cout << '>';  break;
    case AS_TOKEN_OP_NEQ:       cout << "!="; break;
    case AS_TOKEN_ARR_RANGE:    cout << ':';  break;
    case AS_TOKEN_ARR_EXPAN:    cout << '^';  break;
    default:                    cout << '?';  break;
  }
}

const char* cASTDumpVisitor::mapType(ASType_t type)
{
  switch (type) {
    case AS_TYPE_ARRAY:       return "array";
    case AS_TYPE_CHAR:        return "char";
    case AS_TYPE_FLOAT:       return "float";
    case AS_TYPE_INT:         return "int";
    case AS_TYPE_MATRIX:      return "matrix";
    case AS_TYPE_STRING:      return "string";
    case AS_TYPE_VOID:        return "void";
    case AS_TYPE_OBJECT_REF:  return "object";
    
    case AS_TYPE_INVALID:
    default:
      return "*INVALID*";
  }
}


void cASTDumpVisitor::visitAssignment(cASTAssignment& node)
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


void cASTDumpVisitor::visitReturnStatement(cASTReturnStatement& node)
{
  indent();
  cout << "return:" << endl;
  
  m_depth++;
  node.GetExpression()->Accept(*this);
  m_depth--;  
}


void cASTDumpVisitor::visitStatementList(cASTStatementList& node)
{
  tListIterator<cASTNode> it(node.Iterator());
  
  cASTNode* stmt = NULL;
  while ((stmt = it.Next())) {
    stmt->Accept(*this);
  }
}



void cASTDumpVisitor::visitForeachBlock(cASTForeachBlock& node)
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


void cASTDumpVisitor::visitIfBlock(cASTIfBlock& node)
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
  
  if (node.HasElse()) {
    indent();
    cout << "else:" << endl;
    
    m_depth++;
    node.GetElseCode()->Accept(*this);
    m_depth--;
  }
  
  m_depth--;
  
}


void cASTDumpVisitor::visitWhileBlock(cASTWhileBlock& node)
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



void cASTDumpVisitor::visitFunctionDefinition(cASTFunctionDefinition& node)
{
  indent();
  cout << (node.IsDefinition() ? "":"@") << "function: " << mapType(node.GetType()) << " " << node.GetName() << "(";
  
  cout << ")" << endl;
  if (node.IsDefinition()) {
    m_depth++;
    node.GetCode()->Accept(*this);
    m_depth--;
  }
}


void cASTDumpVisitor::visitVariableDefinition(cASTVariableDefinition& node)
{
  indent();
  cout << mapType(node.GetType()) << " " << node.GetVariable() << endl;
  
  if (node.GetAssignmentExpression()) {
    m_depth++;
    indent();
    cout << "=" << endl;
    
    m_depth++;
    node.GetAssignmentExpression()->Accept(*this);

    m_depth -= 2;
  }  
}



void cASTDumpVisitor::visitExpressionBinary(cASTExpressionBinary& node)
{
  m_depth++;
  node.GetLeft()->Accept(*this);
  m_depth--;
  
  indent();
  printToken(node.GetOperator());
  cout << endl;
  
  m_depth++;
  node.GetRight()->Accept(*this);
  m_depth--;  
}


void cASTDumpVisitor::visitExpressionUnary(cASTExpressionUnary& node)
{
  indent();
  printToken(node.GetOperator());
  cout << endl;
  
  m_depth++;
  node.GetExpression()->Accept(*this);
  m_depth--;
}



void cASTDumpVisitor::visitFunctionCall(cASTFunctionCall& node)
{
  
}


void cASTDumpVisitor::visitLiteral(cASTLiteral& node)
{
  indent();
  cout << "(" << mapType(node.GetType()) << ") " << node.GetValue() << endl;
}


void cASTDumpVisitor::visitLiteralArray(cASTLiteralArray& node)
{

}


void cASTDumpVisitor::visitVariableReference(cASTVariableReference& node)
{
  indent();
  cout << node.GetName() << endl;
}
