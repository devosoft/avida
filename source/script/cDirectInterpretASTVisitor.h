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
  // --------  Internal Type Declarations  --------
  class cLocalArray;
  class cLocalMatrix;
  
  typedef union {
    bool as_bool;
    char as_char;
    int as_int;
    double as_float;
    cString* as_string;
    cLocalArray* as_array;
    cLocalMatrix* as_matrix;
  } uAnyType;
  

  // --------  Internal Variables  --------
  cSymbolTable* m_global_symtbl;
  cSymbolTable* m_cur_symtbl;
  
  uAnyType m_rvalue;
  ASType_t m_rtype;
  
  tSmartArray<uAnyType> m_call_stack;
  int m_sp;
  bool m_has_returned;
  
  
  // --------  Private Constructors  --------
  cDirectInterpretASTVisitor(const cDirectInterpretASTVisitor&); // @not_implemented
  cDirectInterpretASTVisitor& operator=(const cDirectInterpretASTVisitor&); // @not_implemented
  
  
public:
  cDirectInterpretASTVisitor(cSymbolTable* global_symtbl);
  
  int Interpret(cASTNode* node);
  
  void VisitAssignment(cASTAssignment&);
  
  void VisitReturnStatement(cASTReturnStatement&);
  void VisitStatementList(cASTStatementList&);
  
  void VisitForeachBlock(cASTForeachBlock&);
  void VisitIfBlock(cASTIfBlock&);
  void VisitWhileBlock(cASTWhileBlock&);
  
  void VisitFunctionDefinition(cASTFunctionDefinition&);
  void VisitVariableDefinition(cASTVariableDefinition&);
  void VisitVariableDefinitionList(cASTVariableDefinitionList&);
  
  void VisitExpressionBinary(cASTExpressionBinary&);
  void VisitExpressionUnary(cASTExpressionUnary&);
  
  void VisitArgumentList(cASTArgumentList&);
  void VisitFunctionCall(cASTFunctionCall&);
  void VisitLiteral(cASTLiteral&);
  void VisitLiteralArray(cASTLiteralArray&);
  void VisitObjectCall(cASTObjectCall&);
  void VisitObjectReference(cASTObjectReference&);
  void VisitVariableReference(cASTVariableReference&);
  void VisitUnpackTarget(cASTUnpackTarget&);


private:
  // --------  Internal Utility Methods  --------
  cLocalArray* asArray(ASType_t type, uAnyType value, cASTNode& node);
  bool asBool(ASType_t type, uAnyType value, cASTNode& node);
  char asChar(ASType_t type, uAnyType value, cASTNode& node);
  int asInt(ASType_t type, uAnyType value, cASTNode& node);
  double asFloat(ASType_t type, uAnyType value, cASTNode& node);
  cString* asString(ASType_t type, uAnyType value, cASTNode& node);

  ASType_t getRuntimeType(ASType_t ltype, ASType_t rtype, bool allow_str = false);
  
  void reportError(ASDirectInterpretError_t err, const cASFilePosition& fp, const int line, ...);
  

  // --------  Internal Type Definitions  --------
  struct sAggregateValue {
    uAnyType value;
    ASType_t type;
  };
  
  class cLocalArray
  {
  private:
    tArray<sAggregateValue>* m_storage;
    int m_ref_count;
    
    
  public:
    inline cLocalArray() : m_storage(new tArray<sAggregateValue>), m_ref_count(1) { ; }
    inline explicit cLocalArray(cLocalArray* in_array); // Create a copy
    inline cLocalArray(cLocalArray* arr1, cLocalArray* arr2); // Concat two arrays
    ~cLocalArray();
    
    inline cLocalArray* GetReference() { m_ref_count++; return this; }
    inline void RemoveReference() { m_ref_count--;  if (m_ref_count == 0) delete this; }
    inline bool IsShared() const { return (m_ref_count > 1); }
    
    inline int GetSize() const { return m_storage->GetSize(); }
    void Resize(int sz);
    
    inline const sAggregateValue& Get(int i) const { return (*m_storage)[i]; }    
    void Set(int i, ASType_t type, uAnyType value);
    
    
  private:
    void copy(int offset, tArray<sAggregateValue>& in_storage);
  };
  
  
  class cLocalMatrix
  {
    
  };
};


inline cDirectInterpretASTVisitor::cLocalArray::cLocalArray(cLocalArray* in_array) : m_ref_count(1)
{
  m_storage = new tArray<sAggregateValue>(in_array->m_storage->GetSize());
  copy(0, *in_array->m_storage);
}

inline cDirectInterpretASTVisitor::cLocalArray::cLocalArray(cLocalArray* arr1, cLocalArray* arr2) : m_ref_count(1)
{
  int sz1 = arr1->m_storage->GetSize();
  m_storage = new tArray<sAggregateValue>(sz1 + arr2->m_storage->GetSize());
  
  copy(0, *arr1->m_storage);
  copy(sz1, *arr2->m_storage);
}



#endif
