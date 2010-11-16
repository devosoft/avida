/*
 *  ASTree.h
 *  Avida
 *
 *  Created by David on 4/7/07.
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

#ifndef ASTree_h
#define ASTree_h

#include "Avida.h"
#include "AvidaScript.h"

#include "cString.h"

#include "tList.h"
#include "tManagedPointerArray.h"


class cASFunction;
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
  static const sASTypeInfo s_invalid_type;
  
  cASTNode(); // @not_implemented
  cASTNode(const cASTNode&); // @not_implemented
  cASTNode& operator=(const cASTNode&); // @not_implmented

  
protected:
  cASFilePosition m_file_pos;

  cASTNode(const cASFilePosition& fp) : m_file_pos(fp) { ; }

  
public:
  virtual ~cASTNode() { ; }
  
  virtual const sASTypeInfo& GetType() const { return s_invalid_type; }

  inline const cASFilePosition& GetFilePosition() const { return m_file_pos; }
  
  virtual bool IsOutputSuppressed() const { return false; }
  virtual void SuppressOutput() { ; }
  
  virtual void Accept(cASTVisitor& visitor) = 0;
};



// -- Concrete Abstract Syntax Tree Nodes
// ---------------------------------------------------------------------------------------------------------------------

class cASTAssignment;
class cASTArgumentList;
class cASTObjectAssignment;

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

class cASTBuiltInCall;
class cASTFunctionCall;
class cASTLiteral;
class cASTLiteralArray;
class cASTLiteralDict;
class cASTObjectCall;
class cASTObjectReference;
class cASTVariableReference;
class cASTUnpackTarget;



// --------  Assignment Nodes  --------

class cASTAssignment : public cASTNode
{
private:
  cString m_var;
  cASTNode* m_expr;
  int m_id;
  bool m_global;
  
public:
  cASTAssignment(const cASFilePosition& fp, const cString& var)
    : cASTNode(fp), m_var(var), m_expr(NULL), m_id(-1), m_global(false) { ; }
  ~cASTAssignment() { delete m_expr; }
  
  inline const cString& GetVariable() { return m_var; }
  
  inline void SetExpression(cASTNode* expr) { delete m_expr; m_expr = expr; }
  inline cASTNode* GetExpression() { return m_expr; }
  
  inline int GetVarID() const { return m_id; }
  inline bool IsVarGlobal() const { return m_global; }
  inline void SetVar(int in_id, bool global) { m_id = in_id; m_global = global; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTArgumentList : public cASTNode
{
private:
  tList<cASTNode> m_nodes;
  
public:
  cASTArgumentList(const cASFilePosition& fp) : cASTNode(fp) { ; }
  ~cASTArgumentList() { while (m_nodes.GetSize()) delete m_nodes.Pop(); }
  
  inline void AddNode(cASTNode* n) { m_nodes.PushRear(n); }
  inline int GetSize() const { return m_nodes.GetSize(); }
  inline tListIterator<cASTNode> Iterator() { return tListIterator<cASTNode>(m_nodes); }
  
  void Accept(cASTVisitor& visitor);
};


class cASTObjectAssignment : public cASTNode
{
private:
  cASTNode* m_trgt;
  cASTNode* m_expr;

public:
  cASTObjectAssignment(const cASFilePosition& fp, cASTNode* trgt)
    : cASTNode(fp), m_trgt(trgt), m_expr(NULL) { ; }
  ~cASTObjectAssignment() { delete m_expr; }
  
  inline cASTNode* GetTarget() { return m_trgt; }

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
  sASTypeInfo m_type;
  cString m_name;
  cASTVariableDefinitionList* m_args;
  cASTNode* m_code;
  
public:
  cASTFunctionDefinition(const cASFilePosition& fp, const sASTypeInfo& type, const cString& name,
                         cASTVariableDefinitionList* args)
    : cASTNode(fp), m_type(type), m_name(name), m_args(args), m_code(NULL) { ; }
  ~cASTFunctionDefinition();
  
  inline const sASTypeInfo& GetType() const { return m_type; }
  inline const cString& GetName() { return m_name; }
  inline cASTVariableDefinitionList* GetArguments() { return m_args; }
  inline void ClearArguments() { m_args = NULL; }
  
  inline void SetCode(cASTNode* code) { m_code = code; }
  inline cASTNode* GetCode() { return m_code; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTVariableDefinition : public cASTNode
{
private:
  sASTypeInfo m_type;
  cString m_name;
  cASTNode* m_assign;
  cASTArgumentList* m_dims;
  int m_id;
  
public:
  cASTVariableDefinition(const cASFilePosition& fp, const sASTypeInfo& type, const cString& name)
    : cASTNode(fp), m_type(type), m_name(name), m_assign(NULL), m_dims(NULL), m_id(-1) { ; }
  ~cASTVariableDefinition() { delete m_assign; delete m_dims; }
  
  inline const sASTypeInfo& GetType() const { return m_type; }
  inline const cString& GetName() { return m_name; }
  inline void SetAssignmentExpression(cASTNode* assign) { delete m_assign; m_assign = assign; }
  inline cASTNode* GetAssignmentExpression() { return m_assign; }
  inline void SetDimensions(cASTArgumentList* dims) { delete m_dims; m_dims = dims; }
  inline cASTArgumentList* GetDimensions() { return m_dims; }
  
  inline int GetVarID() const { return m_id; }
  inline void SetVar(int in_id) { m_id = in_id; }
  
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
  inline cASTVariableDefinition* GetFirst() { return m_nodes.GetFirst(); }
  
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
  sASTypeInfo m_type;
  sASTypeInfo m_compare_type;
  
public:
  cASTExpressionBinary(const cASFilePosition& fp, ASToken_t op, cASTNode* l, cASTNode* r)
    : cASTNode(fp), m_op(op), m_left(l), m_right(r), m_type(AS_TYPE_INVALID), m_compare_type(AS_TYPE_INVALID) { ; }
  ~cASTExpressionBinary() { delete m_left; delete m_right; }
  
  inline ASToken_t GetOperator() { return m_op; }
  inline void SetLeft(cASTNode* left) { m_left = left; }
  inline cASTNode* GetLeft() { return m_left; }
  inline void SetRight(cASTNode* right) { m_right = right; }
  inline cASTNode* GetRight() { return m_right; }

  const sASTypeInfo& GetType() const { return m_type; }
  inline void SetType(const sASTypeInfo& type) { m_type = type; }

  inline const sASTypeInfo& GetCompareType() const { return m_compare_type; }
  inline void SetCompareType(const sASTypeInfo& type) { m_compare_type = type; }

  void Accept(cASTVisitor& visitor);
};


class cASTExpressionUnary : public cASTNode
{
private:
  ASToken_t m_op;
  cASTNode* m_expr;
  sASTypeInfo m_type;
  
public:
  cASTExpressionUnary(const cASFilePosition& fp, ASToken_t op, cASTNode* e)
    : cASTNode(fp), m_op(op), m_expr(e), m_type(AS_TYPE_INVALID) { ; }
  ~cASTExpressionUnary() { delete m_expr; }

  inline ASToken_t GetOperator() { return m_op; }
  inline void SetExpression(cASTNode* expr) { m_expr = expr; }
  inline cASTNode* GetExpression() { return m_expr; }
  
  const sASTypeInfo& GetType() const { return m_type; }
  inline void SetType(const sASTypeInfo& type) { m_type = type; }
  
  void Accept(cASTVisitor& visitor);
};



// --------  Expression Value Nodes  --------

class cASTBuiltInCall : public cASTNode
{
private:
  cASTArgumentList* m_args;
  sASTypeInfo m_type;
  ASBuiltIn_t m_builtin;
  cASTNode* m_target;
  
public:
  cASTBuiltInCall(const cASFilePosition& fp, const cString& name, cASTNode* target = NULL);
  ~cASTBuiltInCall() { delete m_args; delete m_target; }
  
  ASBuiltIn_t GetBuiltIn() const { return m_builtin; }
  
  void SetArguments(cASTArgumentList* args) { delete m_args; m_args = args; }
  cASTArgumentList* GetArguments() { return m_args; }
  
  inline const sASTypeInfo& GetType() const { return m_type; }
  inline void SetType(const sASTypeInfo& type) { m_type = type; }
  
  inline cASTNode* GetTarget() { return m_target; }
  
  bool HasArguments() const { return (m_args); }
  
  void Accept(cASTVisitor& visitor);
};


class cASTFunctionCall : public cASTNode
{
private:
  cString m_name;
  cASTArgumentList* m_args;
  sASTypeInfo m_type;
  int m_id;
  bool m_global;
  const cASFunction* m_func;
  
public:
  cASTFunctionCall(const cASFilePosition& fp, const cString& name)
    : cASTNode(fp), m_name(name), m_args(NULL), m_type(AS_TYPE_INVALID), m_id(-1), m_global(false), m_func(NULL) { ; }
  ~cASTFunctionCall() { delete m_args; }
  
  const cString& GetName() const { return m_name; }
  
  void SetArguments(cASTArgumentList* args) { delete m_args; m_args = args; }
  cASTArgumentList* GetArguments() { return m_args; }
  
  const sASTypeInfo& GetType() const { return m_type; }
  inline void SetType(const sASTypeInfo& type) { m_type = type; }
  
  inline int GetFuncID() const { return m_id; }
  inline bool IsFuncGlobal() const { return m_global; }
  inline void SetFunc(int in_id, bool global) { m_id = in_id; m_global = global; }
  
  inline const cASFunction* GetASFunction() const { return m_func; }
  inline bool IsASFunction() const { return (m_func); }
  inline void SetASFunction(const cASFunction* func) { m_func = func; }

  bool HasArguments() const { return (m_args); }
  
  void Accept(cASTVisitor& visitor);
};


class cASTLiteral : public cASTNode
{
private:
  sASTypeInfo m_type;
  cString m_value;
  
public:
  cASTLiteral(const cASFilePosition& fp, const sASTypeInfo& t, const cString& v) : cASTNode(fp), m_type(t), m_value(v) { ; }
  
  const sASTypeInfo& GetType() const { return m_type; }
  inline const cString& GetValue() { return m_value; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTLiteralArray : public cASTNode
{
private:
  cASTArgumentList* m_values;
  bool m_is_matrix;
  sASTypeInfo m_type;
  
public:
  cASTLiteralArray(const cASFilePosition& fp, cASTArgumentList* v, bool is_mat)
    : cASTNode(fp), m_values(v), m_is_matrix(is_mat), m_type(m_is_matrix ? AS_TYPE_MATRIX : AS_TYPE_ARRAY) { ; }
  ~cASTLiteralArray() { delete m_values; }  
  
  inline cASTArgumentList* GetValues() { return m_values; }
  inline bool IsMatrix() const { return m_is_matrix; }
  
  const sASTypeInfo& GetType() const { return m_type; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTLiteralDict : public cASTNode
{
public:
  struct sMapping {
    cASTNode* idx;
    cASTNode* val;
    
    sMapping(cASTNode* in_idx, cASTNode* in_val) : idx(in_idx), val(in_val) { ; }
    ~sMapping() { delete idx; delete val; }
  };
  
private:  
  sASTypeInfo m_type;
  tList<sMapping> m_mappings;
  
public:
  cASTLiteralDict(const cASFilePosition& fp) : cASTNode(fp), m_type(AS_TYPE_DICT) { ; }
  ~cASTLiteralDict() { ; }  
  
  void AddMapping(cASTNode* idx, cASTNode* val) { m_mappings.PushRear(new sMapping(idx, val)); }
  tListIterator<sMapping> Iterator() { return tListIterator<sMapping>(m_mappings); }
  
  const sASTypeInfo& GetType() const { return m_type; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTObjectCall : public cASTNode
{
private:
  cASTNode* m_object;
  cString m_name;
  cASTArgumentList* m_args;
  sASTypeInfo m_type;
  
public:
  cASTObjectCall(const cASFilePosition& fp, cASTNode* object, const cString& name)
    : cASTNode(fp), m_object(object), m_name(name), m_args(NULL), m_type(AS_TYPE_RUNTIME) { ; }
  ~cASTObjectCall() { delete m_object; delete m_args; }
  
  cASTNode* GetObject() { return m_object; }
  const cString& GetName() const { return m_name; }
  
  void SetArguments(cASTArgumentList* args) { delete m_args; m_args = args; }
  cASTArgumentList* GetArguments() { return m_args; }
  
  const sASTypeInfo& GetType() const { return m_type; }  
  
  bool HasArguments() const { return (m_args); }
  
  void Accept(cASTVisitor& visitor);
};


class cASTObjectReference : public cASTNode
{
private:
  cASTNode* m_object;
  cString m_name;
  sASTypeInfo m_type;
  
public:
  cASTObjectReference(const cASFilePosition& fp, cASTNode* object, const cString& name)
    : cASTNode(fp), m_object(object), m_name(name), m_type(AS_TYPE_RUNTIME) { ; }
  ~cASTObjectReference() { delete m_object; }
  
  cASTNode* GetObject() { return m_object; }
  inline const cString& GetName() { return m_name; }
  
  const sASTypeInfo& GetType() const { return m_type; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTVariableReference : public cASTNode
{
private:
  cString m_name;
  sASTypeInfo m_type;
  int m_id;
  bool m_global;
  
public:
  cASTVariableReference(const cASFilePosition& fp, const cString& name)
    : cASTNode(fp), m_name(name), m_type(AS_TYPE_INVALID), m_id(-1), m_global(false) { ; }
  
  inline const cString& GetName() { return m_name; }
  
  const sASTypeInfo& GetType() const { return m_type; }
  inline void SetType(const sASTypeInfo& type) { m_type = type; }
  
  inline int GetVarID() const { return m_id; }
  inline bool IsVarGlobal() const { return m_global; }
  inline void SetVar(int in_id, bool global) { m_id = in_id; m_global = global; }
  
  void Accept(cASTVisitor& visitor);
};


class cASTUnpackTarget : public cASTNode
{
private:
  struct sUnpackNode {
    cString name;
    int var_id;
    bool global;
    sASTypeInfo type;
    
    inline sUnpackNode() : name(""), var_id(-1), global(false), type(AS_TYPE_INVALID) { ; }
    inline sUnpackNode(const cString& in_name) : name(in_name), var_id(-1), global(false), type(AS_TYPE_INVALID) { ; }
    inline sUnpackNode(const sUnpackNode& un) : name(un.name), var_id(un.var_id), global(un.global), type(un.type) { ; }
    
    inline void SetVar(int in_vi, bool in_g, const sASTypeInfo& in_t) { var_id = in_vi; global = in_g; type = in_t; }
  };
  tManagedPointerArray<sUnpackNode> m_nodes;
  bool m_last_wild;
  bool m_last_named;
  cASTNode* m_expr;
  
public:
  cASTUnpackTarget(const cASFilePosition& fp) : cASTNode(fp), m_last_wild(false), m_last_named(false), m_expr(NULL) { ; }
  ~cASTUnpackTarget() { delete m_expr; }
  
  inline void AddVar(const cString& name) { m_nodes.Push(sUnpackNode(name)); }
  inline int GetSize() const { return m_nodes.GetSize(); }
  inline const cString& GetVarName(int idx) const { return m_nodes[idx].name; }
  inline int GetVarID(int idx) const { return m_nodes[idx].var_id; }
  inline bool IsVarGlobal(int idx) const { return m_nodes[idx].global; }
  inline const sASTypeInfo& GetVarType(int idx) const { return m_nodes[idx].type; }
  inline void SetVar(int idx, int var_id, bool global, const sASTypeInfo& type)
    { m_nodes[idx].SetVar(var_id, global, type); }
  
  inline bool IsLastNamed() const { return m_last_named; }
  inline bool IsLastWild() const { return m_last_wild; }
  
  inline void SetLastNamed() { m_last_wild = true; m_last_named = true; }
  inline void SetLastWild() { m_last_wild = true; m_last_named = false; }
  
  cASTNode* GetExpression() const { return m_expr; }
  void SetExpression(cASTNode* expr) { delete m_expr; m_expr = expr; }
  
  void Accept(cASTVisitor& visitor);
};


#endif
