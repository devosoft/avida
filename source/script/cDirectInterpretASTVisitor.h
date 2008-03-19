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

#include "tHashTable.h"
#include "tSmartArray.h"

class cSymbolTable;


class cDirectInterpretASTVisitor : public cASTVisitor
{
private:
  // --------  Internal Type Declarations  --------
  class cLocalArray;
  class cLocalDict;
  class cLocalMatrix;
  class cObjectRef;
  
  typedef union {
    bool as_bool;
    char as_char;
    int as_int;
    double as_float;
    cString* as_string;
    cLocalArray* as_array;
    cLocalDict* as_dict;
    cLocalMatrix* as_matrix;
    cObjectRef* as_ref;
    void* as_void;
  } uAnyType;
  
  struct sAggregateValue {
    uAnyType value;
    sASTypeInfo type;
    
    sAggregateValue() { ; }
    sAggregateValue(sASTypeInfo in_type, uAnyType in_value) : value(in_value), type(in_type) { ; }
    
    void Cleanup();
    
    bool operator==(const sAggregateValue& rval);
  };
  template<typename HASH_TYPE> friend inline int nHashTable::HashKey(const HASH_TYPE& key, int table_size);
    

  // --------  Internal Variables  --------
  cSymbolTable* m_global_symtbl;
  cSymbolTable* m_cur_symtbl;
  
  uAnyType m_rvalue;
  sASTypeInfo m_rtype;
  
  tSmartArray<sAggregateValue> m_call_stack;
  int m_sp;
  bool m_has_returned;
  bool m_obj_assign;
  
  
  // --------  Private Constructors  --------
  cDirectInterpretASTVisitor(const cDirectInterpretASTVisitor&); // @not_implemented
  cDirectInterpretASTVisitor& operator=(const cDirectInterpretASTVisitor&); // @not_implemented
  
  
public:
  cDirectInterpretASTVisitor(cSymbolTable* global_symtbl);
  ~cDirectInterpretASTVisitor();
  
  int Interpret(cASTNode* node);
  
  void VisitAssignment(cASTAssignment&);
  void VisitObjectAssignment(cASTObjectAssignment&);
  void VisitArgumentList(cASTArgumentList&);
  
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
  
  void VisitBuiltInCall(cASTBuiltInCall&);
  void VisitFunctionCall(cASTFunctionCall&);
  void VisitLiteral(cASTLiteral&);
  void VisitLiteralArray(cASTLiteralArray&);
  void VisitLiteralDict(cASTLiteralDict&);
  void VisitObjectCall(cASTObjectCall&);
  void VisitObjectReference(cASTObjectReference&);
  void VisitVariableReference(cASTVariableReference&);
  void VisitUnpackTarget(cASTUnpackTarget&);


private:
  // --------  Internal Utility Methods  --------
  cLocalArray* asArray(const sASTypeInfo& type, uAnyType value, cASTNode& node);
  bool asBool(const sASTypeInfo& type, uAnyType value, cASTNode& node);
  char asChar(const sASTypeInfo& type, uAnyType value, cASTNode& node);
  cLocalDict* asDict(const sASTypeInfo& type, uAnyType value, cASTNode& node);
  int asInt(const sASTypeInfo& type, uAnyType value, cASTNode& node);
  double asFloat(const sASTypeInfo& type, uAnyType value, cASTNode& node);
  cString* asString(const sASTypeInfo& type, uAnyType value, cASTNode& node);

  ASType_t getRuntimeType(ASType_t ltype, ASType_t rtype, bool allow_str = false);
  
  void reportError(ASDirectInterpretError_t err, const cASFilePosition& fp, const int line, ...);
  

  // --------  Internal Type Definitions  --------
  class cLocalArray
  {
  private:
    tArray<sAggregateValue> m_storage;
    int m_ref_count;
    
    
  public:
    inline cLocalArray() : m_ref_count(1) { ; }
    inline explicit cLocalArray(int sz) : m_storage(sz), m_ref_count(1) { ; }
    inline explicit cLocalArray(cLocalArray* in_array); // Create a copy
    inline cLocalArray(cLocalArray* arr1, cLocalArray* arr2); // Concat two arrays
    ~cLocalArray();
    
    inline cLocalArray* GetReference() { m_ref_count++; return this; }
    inline void RemoveReference() { m_ref_count--;  if (m_ref_count == 0) delete this; }
    inline bool IsShared() const { return (m_ref_count > 1); }
    
    inline int GetSize() const { return m_storage.GetSize(); }
    void Resize(int sz);
    
    inline const sAggregateValue& Get(int i) const { return m_storage[i]; }    
    void Set(int i, const sASTypeInfo& type, uAnyType value);
    
    
  private:
    void copy(int offset, tArray<sAggregateValue>& in_storage);
  };
  
  
  class cLocalDict
  {
  private:
    tHashTable<sAggregateValue, sAggregateValue> m_storage;
    int m_ref_count;
    
    
  public:
    inline cLocalDict() : m_ref_count(1) { ; }
    ~cLocalDict();
    
    inline cLocalDict* GetReference() { m_ref_count++; return this; }
    inline void RemoveReference() { m_ref_count--;  if (m_ref_count == 0) delete this; }
    inline bool IsShared() const { return (m_ref_count > 1); }
    
    inline int GetSize() const { return m_storage.GetSize(); }
    
    bool Get(const sAggregateValue& idx, sAggregateValue& val) const { return m_storage.Find(idx, val); }
    void Set(const sAggregateValue& idx, const sAggregateValue& val);
    void Remove(const sAggregateValue& idx) { m_storage.Remove(idx); }
  };
  
  
  class cLocalMatrix
  {
    
  };
  
  class cObjectRef
  {
  public:
    virtual ~cObjectRef() { ; }

    virtual bool IsWritable() = 0;
    virtual sASTypeInfo GetType() = 0;
    virtual sASTypeInfo GetType(const sAggregateValue& idx) = 0;
    
    virtual uAnyType Get() = 0;
    virtual uAnyType Get(const sAggregateValue& idx) = 0;
    virtual bool Set(ASType_t type, uAnyType value) = 0;
    virtual bool Set(const sAggregateValue& idx, ASType_t type, uAnyType value) = 0;
  };

  class cArrayVarRef : public cObjectRef
  {
  private:
    uAnyType& m_var;
    
  public:
    cArrayVarRef(uAnyType& var) : m_var(var) { ; }
    ~cArrayVarRef() { ; }

    bool IsWritable() { return true; }
    sASTypeInfo GetType() { return AS_TYPE_ARRAY; }
    sASTypeInfo GetType(const sAggregateValue& idx);
    
    uAnyType Get() { return m_var; }
    uAnyType Get(const sAggregateValue& idx);
    bool Set(ASType_t type, uAnyType value) { return false; }
    bool Set(const sAggregateValue& idx, ASType_t type, uAnyType value);
  };
  
  class cObjectIndexRef : public cObjectRef
  {
  private:
    cObjectRef* m_obj;
    sAggregateValue m_idx;
    
  public:
    cObjectIndexRef(cObjectRef* obj, const sAggregateValue& idx) : m_obj(obj), m_idx(idx) { ; }
    ~cObjectIndexRef() { delete m_obj; }
    
    bool IsWritable() { return m_obj->IsWritable(); }
    sASTypeInfo GetType() { return m_obj->GetType(m_idx); }
    sASTypeInfo GetType(const sAggregateValue& idx);
    
    uAnyType Get() { return m_obj->Get(m_idx); }
    uAnyType Get(const sAggregateValue& idx);
    bool Set(ASType_t type, uAnyType value) { return m_obj->Set(m_idx, type, value); }
    bool Set(const sAggregateValue& idx, ASType_t type, uAnyType value);
  };
};

namespace nHashTable {
  template<> inline int HashKey<cDirectInterpretASTVisitor::sAggregateValue>(const cDirectInterpretASTVisitor::sAggregateValue& key, int table_size)
  {
    switch (key.type.type) {
      case AS_TYPE_BOOL:    return HashKey<int>(key.value.as_bool, table_size);
      case AS_TYPE_CHAR:    return HashKey<int>(key.value.as_char, table_size);
      case AS_TYPE_INT:     return HashKey<int>(key.value.as_int, table_size);
      case AS_TYPE_FLOAT:   return HashKey<int>((int)key.value.as_float, table_size);
      case AS_TYPE_STRING:  return HashKey<cString>(*key.value.as_string, table_size);
      default:              return HashKey<void*>(key.value.as_void, table_size);
    }
  }
}


inline cDirectInterpretASTVisitor::cLocalArray::cLocalArray(cLocalArray* in_array)
  : m_storage(in_array->m_storage.GetSize()), m_ref_count(1)
{
  copy(0, in_array->m_storage);
}

inline cDirectInterpretASTVisitor::cLocalArray::cLocalArray(cLocalArray* arr1, cLocalArray* arr2)
  : m_storage(arr1->m_storage.GetSize() + arr2->m_storage.GetSize()), m_ref_count(1)
{
  copy(0, arr1->m_storage);
  copy(arr1->m_storage.GetSize(), arr2->m_storage);
}


#endif
