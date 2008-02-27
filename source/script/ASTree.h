/*
 *  ASTree.h
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

#ifndef ASTree_h
#define ASTree_h

#include "AvidaScript.h"

#include "cString.h"

#include "defs.h"

#include "tList.h"
#include "tManagedPointerArray.h"


class cASTVisitor;


class cASFilePosition
{
private:
  cString m_filename;
  int m_line;
  
  
  cASFilePosition(); // @not_implemented
  cASFilePosition& operator=(const cASFilePosition&); // @not_implemented
  
  
public:
  inline cASFilePosition(const cString& fn, int line) : m_filename(fn), m_line(line) { ; }
  inline cASFilePosition(const cASFilePosition& fp) : m_filename(fp.m_filename), m_line(fp.m_line) { ; }
  
  inline const cString& GetFilename() const { return m_filename; }
  inline int GetLineNumber() const { return m_line; }
  
  inline bool operator==(const cASFilePosition& fp) { return m_line == fp.m_line && m_filename == fp.m_filename; }
  inline bool operator!=(const cASFilePosition& fp) { return m_line != fp.m_line || m_filename != fp.m_filename; }
};

  

// -- Abstract Syntax Tree Base Class
// ---------------------------------------------------------------------------------------------------------------------


//! Abstract base class for all AvidaScript abstract syntax tree nodes
class cASTNode
{
private:
  cASTNode(); // @not_implemented
  cASTNode(const cASTNode&); // @not_implemented
  cASTNode& operator=(const cASTNode&); // @not_implmented

protected:
  cASFilePosition m_file_pos;
  cASTNode(const cASFilePosition& fp) : m_file_pos(fp) { ; }

public:
  virtual ~cASTNode() { ; }
  
  virtual ASType_t GetType() const { return AS_TYPE_INVALID; }

  inline const cASFilePosition& GetFilePosition() const { return m_file_pos; }
  
  virtual void Accept(cASTVisitor& visitor) = 0;
};



// -- Concrete Abstract Syntax Tree Nodes
// ---------------------------------------------------------------------------------------------------------------------

class cASTAssignment;
class cASTArgumentList;

class cASTReturnStatement;
class cASTStatementList;

class cASTForeachBlock;
class cASTIfBlock;
class cASTWhileBlock;

class cASTFunctionDefinition;
class cASTVariableDefinition;
class cASTVariableDefinitionList;

class cASTExpressionBinary;
class cASTExpressionUnary;

class cASTFunctionCall;
class cASTLiteral;
class cASTLiteralArray;
class cASTVariableReference;
class cASTUnpackTarget;



// --------  Assignment Nodes  --------

class cASTAssignment : public cASTNode
{
private:
  cString m_var;
  cASTNode* m_expr;
  
public:
  cASTAssignment(const cASFilePosition& fp, const cString& var) : cASTNode(fp), m_var(var), m_expr(NULL) { ; }
  ~cASTAssignment() { delete m_expr; }
  
  inline const cString& GetVariable() { return m_var; }
  
  inline void SetExpression(cASTNode* expr) { delete m_expr; m_expr = expr; }
  inline cASTNode* GetExpression() { return m_expr; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTArgumentList : public cASTNode
{
private:
  tList<cASTNode> m_nodes;
  
public:
  cASTArgumentList(const cASFilePosition& fp) : cASTNode(fp) { ; }
  ~cASTArgumentList() { ; }
  
  inline void AddNode(cASTNode* n) { m_nodes.PushRear(n); }
  inline tListIterator<cASTNode> Iterator() { return tListIterator<cASTNode>(m_nodes); }
  
  void Accept(cASTVisitor& visitor);
};




// --------  Block Nodes  --------

class cASTReturnStatement : public cASTNode
{
private:
  cASTNode* m_expr;
  
public:
  cASTReturnStatement(const cASFilePosition& fp, cASTNode* expr) : cASTNode(fp), m_expr(expr) { ; }
  ~cASTReturnStatement() { delete m_expr; }

  inline cASTNode* GetExpression() { return m_expr; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTStatementList : public cASTNode
{
private:
  tList<cASTNode> m_nodes;
  
public:
  cASTStatementList(const cASFilePosition& fp) : cASTNode(fp) { ; }
  ~cASTStatementList();
  
  inline void AddNode(cASTNode* n) { m_nodes.PushRear(n); }
  inline tListIterator<cASTNode> Iterator() { return tListIterator<cASTNode>(m_nodes); }
  
  void Accept(cASTVisitor& visitor);
};




// --------  Conditional Block Nodes  --------

class cASTForeachBlock : public cASTNode
{
private:
  cASTVariableDefinition* m_var;
  cASTNode* m_expr;
  cASTNode* m_code;
  
public:
  cASTForeachBlock(const cASFilePosition& fp, cASTVariableDefinition* v, cASTNode* e, cASTNode* c) 
    : cASTNode(fp), m_var(v), m_expr(e), m_code(c) { ; }
  
  inline cASTVariableDefinition* GetVariable() { return m_var; }
  inline cASTNode* GetValues() { return m_expr; }
  inline cASTNode* GetCode() { return m_code; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTIfBlock : public cASTNode
{
public:
  class cElseIf
  {
    friend class cASTIfBlock;
  private:
    cASTNode* m_expr;
    cASTNode* m_code;
    
    cElseIf(cASTNode* expr, cASTNode* code) : m_expr(expr), m_code(code) { ; }
    
  public:
    cASTNode* GetCondition() { return m_expr; }
    cASTNode* GetCode() { return m_code; }
  };
  
private:
  cASTNode* m_expr;
  cASTNode* m_code;
  cASTNode* m_else;
  
  tList<cElseIf> m_elifs;
  
public:
  cASTIfBlock(const cASFilePosition& fp, cASTNode* expr, cASTNode* code)
    : cASTNode(fp), m_expr(expr), m_code(code), m_else(NULL) { ; }
  ~cASTIfBlock()
  {
    delete m_expr;
    delete m_code;
    delete m_else;
    cElseIf* elif = NULL;
    while ((elif = m_elifs.Pop())) delete elif;
  }

  
  inline cASTNode* GetCondition() { return m_expr; }
  inline cASTNode* GetCode() { return m_code; }
  inline void AddElseIf(cASTNode* expr, cASTNode* code) { m_elifs.PushRear(new cElseIf(expr, code)); }
  inline tListIterator<cElseIf> ElseIfIterator() { return tListIterator<cElseIf>(m_elifs); }
  inline void SetElseCode(cASTNode* code) { m_else = code; }
  inline cASTNode* GetElseCode() { return m_else; }
  
  inline bool HasElseIfs() const { return (m_elifs.GetSize()); }
  inline bool HasElse() const { return (m_else); }
  
  void Accept(cASTVisitor& visitor);
};


class cASTWhileBlock : public cASTNode
{
private:
  cASTNode* m_expr;
  cASTNode* m_code;
  
public:
  cASTWhileBlock(const cASFilePosition& fp, cASTNode* expr, cASTNode* code) : cASTNode(fp), m_expr(expr), m_code(code) { ; }
  ~cASTWhileBlock() { delete m_expr; delete m_code; }
  
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
  cASTVariableDefinitionList* m_args;
  cASTNode* m_code;
  
public:
  cASTFunctionDefinition(const cASFilePosition& fp, ASType_t type, const cString& name, cASTVariableDefinitionList* args)
    : cASTNode(fp), m_type(type), m_name(name), m_args(args), m_code(NULL) { ; }
  
  inline ASType_t GetType() { return m_type; }
  inline const cString& GetName() { return m_name; }
  inline cASTVariableDefinitionList* GetArguments() { return m_args; }
  
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
  cASTArgumentList* m_dims;
  
public:
  cASTVariableDefinition(const cASFilePosition& fp, ASType_t type, const cString& var)
    : cASTNode(fp), m_type(type), m_var(var), m_assign(NULL), m_dims(NULL) { ; }
  ~cASTVariableDefinition() { delete m_assign; delete m_dims; }
  
  inline ASType_t GetType() { return m_type; }
  inline const cString& GetVariable() { return m_var; }
  inline void SetAssignmentExpression(cASTNode* assign) { delete m_assign; m_assign = assign; }
  inline cASTNode* GetAssignmentExpression() { return m_assign; }
  inline void SetDimensions(cASTArgumentList* dims) { delete m_dims; m_dims = dims; }
  inline cASTArgumentList* GetDimensions() { return m_dims; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTVariableDefinitionList : public cASTNode
{
private:
  tList<cASTVariableDefinition> m_nodes;
  
public:
  cASTVariableDefinitionList(const cASFilePosition& fp) : cASTNode(fp) { ; }
  ~cASTVariableDefinitionList() { ; }
  
  inline void AddNode(cASTVariableDefinition* n) { m_nodes.PushRear(n); }
  inline tListIterator<cASTVariableDefinition> Iterator() { return tListIterator<cASTVariableDefinition>(m_nodes); }
  
  inline int GetSize() const { return m_nodes.GetSize(); }
  
  void Accept(cASTVisitor& visitor);
};





// --------  Expression Operation Nodes  --------

class cASTExpressionBinary : public cASTNode
{
private:
  ASToken_t m_op;
  cASTNode* m_left;
  cASTNode* m_right;
  ASType_t m_type;
  
public:
  cASTExpressionBinary(const cASFilePosition& fp, ASToken_t op, cASTNode* l, cASTNode* r)
    : cASTNode(fp), m_op(op), m_left(l), m_right(r), m_type(AS_TYPE_INVALID) { ; }
  ~cASTExpressionBinary() { delete m_left; delete m_right; }
  
  inline ASToken_t GetOperator() { return m_op; }
  inline void SetLeft(cASTNode* left) { m_left = left; }
  inline cASTNode* GetLeft() { return m_left; }
  inline void SetRight(cASTNode* right) { m_right = right; }
  inline cASTNode* GetRight() { return m_right; }

  ASType_t GetType() const { return m_type; }
  inline void SetType(ASType_t type) { m_type = type; }

  void Accept(cASTVisitor& visitor);
};


class cASTExpressionUnary : public cASTNode
{
private:
  ASToken_t m_op;
  cASTNode* m_expr;
  ASType_t m_type;
  
public:
  cASTExpressionUnary(const cASFilePosition& fp, ASToken_t op, cASTNode* e)
    : cASTNode(fp), m_op(op), m_expr(e), m_type(AS_TYPE_INVALID) { ; }
  ~cASTExpressionUnary() { delete m_expr; }

  inline ASToken_t GetOperator() { return m_op; }
  inline void SetExpression(cASTNode* expr) { m_expr = expr; }
  inline cASTNode* GetExpression() { return m_expr; }
  
  ASType_t GetType() const { return m_type; }
  inline void SetType(ASType_t type) { m_type = type; }
  
  void Accept(cASTVisitor& visitor);
};



// --------  Expression Value Nodes  --------

class cASTFunctionCall : public cASTNode
{
private:
  cASTNode* m_target;
  cASTArgumentList* m_args;
  ASType_t m_type;
  
public:
  cASTFunctionCall(const cASFilePosition& fp, cASTNode* target)
    : cASTNode(fp), m_target(target), m_args(NULL), m_type(AS_TYPE_INVALID) { ; }
  ~cASTFunctionCall() { delete m_args; }
  
  cASTNode* GetTarget() { return m_target; }
  void SetArguments(cASTArgumentList* args) { delete m_args; m_args = args; }
  cASTArgumentList* GetArguments() { return m_args; }
  
  ASType_t GetType() const { return m_type; }
  inline void SetType(ASType_t type) { m_type = type; }

  bool HasArguments() const { return (m_args); }
  
  void Accept(cASTVisitor& visitor);
};


class cASTLiteral : public cASTNode
{
private:
  ASType_t m_type;
  cString m_value;
  
public:
  cASTLiteral(const cASFilePosition& fp, ASType_t t, const cString& v) : cASTNode(fp), m_type(t), m_value(v) { ; }
  
  ASType_t GetType() const { return m_type; }
  inline const cString& GetValue() { return m_value; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTLiteralArray : public cASTNode
{
private:
  cASTNode* m_value;
  bool m_is_matrix;
  
public:
  cASTLiteralArray(const cASFilePosition& fp, cASTNode* v, bool is_mat) : cASTNode(fp), m_value(v), m_is_matrix(is_mat) { ; }
  ~cASTLiteralArray() { delete m_value; }  
  
  inline cASTNode* GetValue() { return m_value; }
  inline bool IsMatrix() const { return m_is_matrix; }
  
  ASType_t GetType() const { return m_is_matrix ? AS_TYPE_MATRIX : AS_TYPE_ARRAY; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTVariableReference : public cASTNode
{
private:
  cString m_name;
  ASType_t m_type;
  
public:
  cASTVariableReference(const cASFilePosition& fp, const cString& name)
    : cASTNode(fp), m_name(name), m_type(AS_TYPE_INVALID) { ; }
  
  inline const cString& GetName() { return m_name; }
  
  ASType_t GetType() const { return m_type; }
  inline void SetType(ASType_t type) { m_type = type; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTUnpackTarget : public cASTNode
{
private:
  tManagedPointerArray<cString> m_nodes;
  bool m_last_wild;
  bool m_last_named;
  cASTNode* m_expr;
  
public:
  cASTUnpackTarget(const cASFilePosition& fp) : cASTNode(fp), m_last_wild(false), m_last_named(false), m_expr(NULL) { ; }
  ~cASTUnpackTarget() { delete m_expr; }
  
  inline void AddVar(const cString& name) { m_nodes.Push(name); }
  inline int GetSize() const { return m_nodes.GetSize(); }
  inline const cString& GetVar(int idx) const { return m_nodes[idx]; }
  
  inline bool IsLastNamed() const { return m_last_named; }
  inline bool IsLastWild() const { return m_last_wild; }
  
  inline void SetLastNamed() { m_last_wild = true; m_last_named = true; }
  inline void SetLastWild() { m_last_wild = true; m_last_named = false; }
  
  cASTNode* GetExpression() const { return m_expr; }
  void SetExpression(cASTNode* expr) { delete m_expr; m_expr = expr; }
  
  void Accept(cASTVisitor& visitor);
};


#endif
