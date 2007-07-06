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


class cASTExpressionUnary : public cASTNode
{
private:
  ASToken_t m_op;
  cASTNode* m_expr;
  
public:
  cASTExpressionUnary(ASToken_t op) : m_op(op), m_expr(NULL) { ; }
  
  void SetExpression(cASTNode* expr) { m_expr = expr; }
  
  void Accept(cASTVisitor& visitor);
};

class cASTExpressionBinary : public cASTNode
{
private:
  ASToken_t m_op;
  cASTNode* m_left;
  cASTNode* m_right;
  
public:
  cASTExpressionBinary(ASToken_t op) : m_op(op), m_left(NULL), m_right(NULL) { ; }
  
  void SetLeft(cASTNode* left) { m_left = left; }
  void SetRight(cASTNode* right) { m_right = right; }
  
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


class cASTFunctionCall : public cASTNode
{
private:
  
public:
  cASTFunctionCall() { ; }
  
  void Accept(cASTVisitor& visitor);
};


#endif
