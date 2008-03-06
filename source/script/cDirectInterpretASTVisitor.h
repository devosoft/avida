/*
 *  cDirectInterpretASTVisitor.h
 *  Avida
 *
 *  Created by David on 3/4/08.
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

#ifndef cDirectInterpretASTVisitor_h
#define cDirectInterpretASTVisitor_h

#include "cASTVisitor.h"

#include "tSmartArray.h"

class cSymbolTable;


class cDirectInterpretASTVisitor : public cASTVisitor
{
private:
  cSymbolTable* m_global_symtbl;
  cSymbolTable* m_cur_symtbl;
  
  typedef union {
    bool as_bool;
    char as_char;
    int as_int;
    double as_float;
    cString* as_string;
  } uAnyType;
  
  uAnyType m_rvalue;
  ASType_t m_rtype;
  
  tSmartArray<uAnyType> m_call_stack;
  int m_sp;
  bool m_has_returned;
  
  
  cDirectInterpretASTVisitor(const cDirectInterpretASTVisitor&); // @not_implemented
  cDirectInterpretASTVisitor& operator=(const cDirectInterpretASTVisitor&); // @not_implemented
  
  
public:
  cDirectInterpretASTVisitor(cSymbolTable* global_symtbl);
  
  
  void visitAssignment(cASTAssignment&);
  
  void visitReturnStatement(cASTReturnStatement&);
  void visitStatementList(cASTStatementList&);
  
  void visitForeachBlock(cASTForeachBlock&);
  void visitIfBlock(cASTIfBlock&);
  void visitWhileBlock(cASTWhileBlock&);
  
  void visitFunctionDefinition(cASTFunctionDefinition&);
  void visitVariableDefinition(cASTVariableDefinition&);
  void visitVariableDefinitionList(cASTVariableDefinitionList&);
  
  void visitExpressionBinary(cASTExpressionBinary&);
  void visitExpressionUnary(cASTExpressionUnary&);
  
  void visitArgumentList(cASTArgumentList&);
  void visitFunctionCall(cASTFunctionCall&);
  void visitLiteral(cASTLiteral&);
  void visitLiteralArray(cASTLiteralArray&);
  void visitObjectCall(cASTObjectCall&);
  void visitObjectReference(cASTObjectReference&);
  void visitVariableReference(cASTVariableReference&);
  void visitUnpackTarget(cASTUnpackTarget&);

private:
  bool asBool(ASType_t type, uAnyType value, cASTNode& node);
  char asChar(ASType_t type, uAnyType value, cASTNode& node);
  int asInt(ASType_t type, uAnyType value, cASTNode& node);
  double asFloat(ASType_t type, uAnyType value, cASTNode& node);
  cString* asString(ASType_t type, uAnyType value, cASTNode& node);

  ASType_t getRuntimeType(ASType_t ltype, ASType_t rtype, bool allow_str = false);
  
  void reportError(ASDirectInterpretError_t err, const cASFilePosition& fp, const int line, ...);
};


#endif
