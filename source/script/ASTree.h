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
#ifndef cString_h
#include "cString.h"
#endif
#ifndef defs_h
#include "defs.h"
#endif
#ifndef tList_h
#include "tList.h"
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
  cString m_var;
  cASTNode* m_expr;
  
public:
  cASTAssignment(const cString& var) : m_var(var), m_expr(NULL) { ; }
  ~cASTAssignment() { delete m_expr; }
  
  inline const cString& GetVariable() { return m_var; }
  
  inline void SetExpression(cASTNode* expr) { delete m_expr; m_expr = expr; }
  inline cASTNode* GetExpression() { return m_expr; }
  
  void Accept(cASTVisitor& visitor);
};



// --------  Block Nodes  --------

class cASTReturnStatement : public cASTNode
{
private:
  cASTNode* m_expr;
  
public:
  cASTReturnStatement(cASTNode* expr) : m_expr(expr) { ; }
  ~cASTReturnStatement() { delete m_expr; }

  inline cASTNode* GetExpression() { return m_expr; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTStatementList : public cASTNode
{
private:
  tList<cASTNode> m_nodes;
  
public:
  cASTStatementList() { ; }
  ~cASTStatementList();
  
  inline void AddNode(cASTNode* n) { m_nodes.PushRear(n); }
  inline tListIterator<cASTNode> Iterator() { return tListIterator<cASTNode>(m_nodes); }
  
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
  cASTNode* m_expr;
  cASTNode* m_code;
  
public:
  cASTWhileBlock(cASTNode* expr, cASTNode* code) : m_expr(expr), m_code(code) { ; }
  
  inline cASTNode* GetCondition() { return m_expr; }
  inline cASTNode* GetCode() { return m_code; }
  
  void Accept(cASTVisitor& visitor);
};




// --------  Definition Nodes  --------

class cASTFunctionDefinition : public cASTNode
{
private:
  ASType_t m_type;
  cString m_name;
  cASTNode* m_args;
  cASTNode* m_code;
  
public:
  cASTFunctionDefinition(ASType_t type, const cString& name, cASTNode* args)
    : m_type(type), m_name(name), m_args(args), m_code(NULL) { ; }
  
  inline ASType_t GetType() { return m_type; }
  inline const cString& GetName() { return m_name; }
  inline cASTNode* GetArguments() { return m_args; }
  
  inline void SetCode(cASTNode* code) { m_code = code; }
  inline cASTNode* GetCode() { return m_code; }
  
  inline bool IsDefinition() { return (m_code); }
  
  void Accept(cASTVisitor& visitor);
};


class cASTVariableDefinition : public cASTNode
{
private:
  ASType_t m_type;
  cString m_var;
  cASTNode* m_assign;
  
public:
  cASTVariableDefinition(ASType_t type, const cString& var) : m_type(type), m_var(var), m_assign(NULL) { ; }
  ~cASTVariableDefinition() { delete m_assign; }
  
  inline ASType_t GetType() { return m_type; }
  inline const cString& GetVariable() { return m_var; }
  inline void SetAssignmentExpression(cASTNode* assign) { delete m_assign; m_assign = assign; }
  inline cASTNode* GetAssignmentExpression() { return m_assign; }
  
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
  
  inline ASToken_t GetOperator() { return m_op; }
  inline void SetLeft(cASTNode* left) { m_left = left; }
  inline cASTNode* GetLeft() { return m_left; }
  inline void SetRight(cASTNode* right) { m_right = right; }
  inline cASTNode* GetRight() { return m_right; }
  
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

  inline ASToken_t GetOperator() { return m_op; }
  inline void SetExpression(cASTNode* expr) { m_expr = expr; }
  inline cASTNode* GetExpression() { return m_expr; }
  
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
  ASType_t m_type;
  cString m_value;
  
public:
  cASTLiteral(ASType_t t, const cString& v) : m_type(t), m_value(v) { ; }
  
  inline ASType_t GetType() { return m_type; }
  inline const cString& GetValue() { return m_value; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTLiteralArray : public cASTNode
{
private:
  ASType_t m_type;
  cASTNode* m_value;
  
public:
  cASTLiteralArray(ASType_t t, cASTNode* v) : m_type(t), m_value(v) { ; }
  ~cASTLiteralArray() { delete m_value; }
  
  
  inline ASType_t GetType() { return m_type; }
  inline cASTNode* GetValue() { return m_value; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTVariableReference : public cASTNode
{
private:
  cString m_name;
  
public:
  cASTVariableReference(const cString& name) : m_name(name) { ; }
  
  inline const cString& GetName() { return m_name; }
  
  void Accept(cASTVisitor& visitor);
};


#endif
