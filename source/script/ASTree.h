/*
 *  ASTree.h
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

#ifndef ASTree_h
#define ASTree_h

#ifndef AvidaScript_h
#include "AvidaScript.h"
#endif
#ifndef defs_h
#include "defs.h"
#endif


class cASTVisitor;


// -- Abstract Syntax Tree Base Class
// ---------------------------------------------------------------------------------------------------------------------


//! Abstract base class for all AvidaScript abstract syntax tree nodes
class cASTNode
{
private:
  cASTNode(const cASTNode&); // @not_implemented
  cASTNode& operator=(const cASTNode&); // @not_implmented

protected:
  cASTNode() { ; }

public:
  virtual ~cASTNode() { ; }
  
  virtual void Accept(cASTVisitor& visitor) = 0;
};



// -- Concrete Abstract Syntax Tree Nodes
// ---------------------------------------------------------------------------------------------------------------------


// --------  Assignment Nodes  --------

class cASTAssignment : public cASTNode
{
private:
  
public:
  cASTAssignment() { ; }
  
  void Accept(cASTVisitor& visitor);
};



// --------  Block Nodes  --------

class cASTStatementList : public cASTNode
{
private:
  
public:
  cASTStatementList() { ; }
  
  void Accept(cASTVisitor& visitor);
};




// --------  Conditional Block Nodes  --------

class cASTForeachBlock : public cASTNode
{
private:
  
public:
  cASTForeachBlock() { ; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTIfBlock : public cASTNode
{
private:
  
public:
  cASTIfBlock() { ; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTWhileBlock : public cASTNode
{
private:
  
public:
  cASTWhileBlock() { ; }
  
  void Accept(cASTVisitor& visitor);
};




// --------  Definition Nodes  --------

class cASTFunctionDefinition : public cASTNode
{
private:
  
public:
  cASTFunctionDefinition() { ; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTVariableDefinition : public cASTNode
{
private:
  
public:
  cASTVariableDefinition() { ; }
  
  void Accept(cASTVisitor& visitor);
};




// --------  Expression Operation Nodes  --------

class cASTExpressionBinary : public cASTNode
{
private:
  ASToken_t m_op;
  cASTNode* m_left;
  cASTNode* m_right;
  
public:
  cASTExpressionBinary(ASToken_t op, cASTNode* l, cASTNode* r) : m_op(op), m_left(l), m_right(r) { ; }
  ~cASTExpressionBinary() { delete m_left; delete m_right; }
  
  void SetLeft(cASTNode* left) { m_left = left; }
  void SetRight(cASTNode* right) { m_right = right; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTExpressionUnary : public cASTNode
{
private:
  ASToken_t m_op;
  cASTNode* m_expr;
  
public:
  cASTExpressionUnary(ASToken_t op, cASTNode* e) : m_op(op), m_expr(e) { ; }
  ~cASTExpressionUnary() { delete m_expr; }
  
  void SetExpression(cASTNode* expr) { m_expr = expr; }
  
  void Accept(cASTVisitor& visitor);
};



// --------  Expression Value Nodes  --------

class cASTFunctionCall : public cASTNode
{
private:
  
public:
  cASTFunctionCall() { ; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTLiteral : public cASTNode
{
private:
  ASToken_t m_type;
  
public:
  cASTLiteral(ASToken_t t) : m_type(t) { ; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTVariableReference : public cASTNode
{
private:
  
public:
  cASTVariableReference() { ; }
  
  void Accept(cASTVisitor& visitor);
};


#endif
