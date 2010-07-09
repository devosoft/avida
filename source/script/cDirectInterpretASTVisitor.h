/*
 *  cDirectInterpretASTVisitor.h
 *  Avida
 *
 *  Created by David on 3/4/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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

#include "cASNativeObject.h"
#include "cASTVisitor.h"

#include "tHashMap.h"
#include "tManagedPointerArray.h"
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
    cASNativeObject* as_nobj;
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
  cLocalMatrix* asMatrix(const sASTypeInfo& type, uAnyType value, cASTNode& node);
  cASNativeObject* asNativeObject(const cString& info, const sASTypeInfo& type, uAnyType value, cASTNode& node);
  cString* asString(const sASTypeInfo& type, uAnyType value, cASTNode& node);
  
  ASType_t getRuntimeType(ASType_t ltype, ASType_t rtype, bool allow_str = false);
  
  void matrixAdd(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node);
  void matrixSubtract(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node);
  
  void matrixMultiply(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node);
  void matrixMultiply_ScalarInt(cLocalMatrix* s, cLocalMatrix* m, cASTNode& node);
  void matrixMultiply_ScalarFloat(cLocalMatrix* s, cLocalMatrix* m, cASTNode& node);
  void matrixMultiply_FullInt(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node);
  void matrixMultiply_FullFloat(cLocalMatrix* m1, cLocalMatrix* m2, cASTNode& node);
  
  void reportError(ASDirectInterpretError_t err, const cASFilePosition& fp, const int line, ...);
  

  // --------  Internal Type Definitions  --------
  class cLocalArray
  {
  private:
    tArray<sAggregateValue> m_storage;
    int m_ref_count;
    bool m_resizable;
    
    
  public:
    inline cLocalArray() : m_ref_count(1), m_resizable(true) { ; }
    inline explicit cLocalArray(int sz) : m_storage(sz), m_ref_count(1), m_resizable(true) { ; }
    inline explicit cLocalArray(cLocalArray* in_array); // Create a copy
    inline cLocalArray(cLocalArray* arr1, cLocalArray* arr2); // Concat two arrays
    ~cLocalArray();
    
    inline cLocalArray* GetReference() { m_ref_count++; return this; }
    inline void RemoveReference() { m_ref_count--;  if (m_ref_count == 0) delete this; }
    inline bool IsShared() const { return (m_ref_count > 1); }
    
    inline int GetSize() const { return m_storage.GetSize(); }
    inline bool IsResizable() const { return m_resizable; }
    inline void SetNonResizable() { m_resizable = false; }
    void Resize(int sz);
    
    inline const sAggregateValue& Get(int i) const { return m_storage[i]; }    
    void Set(int i, const sASTypeInfo& type, uAnyType value);
    inline void Set(int i, const sAggregateValue& val) { Set(i, val.type, val.value); }
    
    void SetWithArray(cLocalArray* arr);    
    void SetWithKeys(cLocalDict* dict);
    void SetWithValues(cLocalDict* dict);
    
    
  private:
    void copy(int offset, tArray<sAggregateValue>& in_storage);
  };
  
  
  class cLocalDict
  {
  private:
    tHashMap<sAggregateValue, sAggregateValue> m_storage;
    int m_ref_count;
    
    
  public:
    inline cLocalDict() : m_ref_count(1) { ; }
    ~cLocalDict();
    
    inline cLocalDict* GetReference() { m_ref_count++; return this; }
    inline void RemoveReference() { m_ref_count--;  if (m_ref_count == 0) delete this; }
    inline bool IsShared() const { return (m_ref_count > 1); }
    
    inline int GetSize() const { return m_storage.GetSize(); }
    
    void Clear();

    bool Get(const sAggregateValue& idx, sAggregateValue& val) const { return m_storage.Find(idx, val); }
    void Set(const sAggregateValue& idx, const sAggregateValue& val);
    void Remove(const sAggregateValue& idx) { sAggregateValue val = m_storage.Remove(idx); val.Cleanup(); }
    
    inline bool HasKey(const sAggregateValue& idx) { return m_storage.HasEntry(idx); }
    inline void GetKeys(tArray<sAggregateValue>& out_array) { m_storage.GetKeys(out_array); }
    inline void GetValues(tArray<sAggregateValue>& out_array) { m_storage.GetValues(out_array); }
  };
  
  
  class cLocalMatrix
  {
  private:
    tManagedPointerArray<cLocalArray> m_storage;
    int m_sz_y;
    int m_ref_count;
    
    
  public:
    inline cLocalMatrix() : m_sz_y(0), m_ref_count(1) { ; }
    explicit cLocalMatrix(cLocalMatrix* in_matrix); // Create a copy
    ~cLocalMatrix() { ; }
    
    inline cLocalMatrix* GetReference() { m_ref_count++; return this; }
    inline void RemoveReference() { m_ref_count--;  if (m_ref_count == 0) delete this; }
    inline bool IsShared() const { return (m_ref_count > 1); }
    
    inline int GetNumRows() const { return m_storage.GetSize(); }
    inline int GetNumCols() const { return m_sz_y; }
    void Resize(int sz_x, int sz_y);
    
    inline cLocalArray* GetRow(int i) { return &m_storage[i]; }
    void Set(int i, cLocalArray* arr) { m_storage[i].SetWithArray(arr); }
  };
  

  class cObjectRef
  {
  public:
    virtual ~cObjectRef() { ; }

    virtual bool IsWritable() = 0;
    
    virtual bool Get(sAggregateValue& val) = 0;
    virtual bool Get(const sAggregateValue& idx, sAggregateValue& val) = 0;
    virtual bool Set(sAggregateValue& val) = 0;
    virtual bool Set(sAggregateValue& idx, sAggregateValue& val) = 0;
  };

  class cArrayVarRef : public cObjectRef
  {
  private:
    uAnyType& m_var;
    
  public:
    cArrayVarRef(uAnyType& var) : m_var(var) { ; }
    ~cArrayVarRef() { ; }

    bool IsWritable() { return true; }
    
    bool Get(sAggregateValue& val) { val.value = m_var; val.type = AS_TYPE_ARRAY; return true; }
    bool Get(const sAggregateValue& idx, sAggregateValue& val);
    bool Set(sAggregateValue& val) { return false; }
    bool Set(sAggregateValue& idx, sAggregateValue& val);
  };
  
  class cDictVarRef : public cObjectRef
  {
  private:
    uAnyType& m_var;
    
  public:
    cDictVarRef(uAnyType& var) : m_var(var) { ; }
    ~cDictVarRef() { ; }
    
    bool IsWritable() { return true; }
    
    bool Get(sAggregateValue& val) { val.value = m_var; val.type = AS_TYPE_DICT; return true;}
    bool Get(const sAggregateValue& idx, sAggregateValue& val);
    bool Set(sAggregateValue& val) { return false; }
    bool Set(sAggregateValue& idx, sAggregateValue& val);
  };
  
  class cMatrixVarRef : public cObjectRef
  {
  private:
    uAnyType& m_var;
    
  public:
    cMatrixVarRef(uAnyType& var) : m_var(var) { ; }
    ~cMatrixVarRef() { ; }
    
    bool IsWritable() { return true; }
    
    bool Get(sAggregateValue& val) { val.value = m_var; val.type = AS_TYPE_MATRIX; return true; }
    bool Get(const sAggregateValue& idx, sAggregateValue& val);
    bool Set(sAggregateValue& val) { return false; }
    bool Set(sAggregateValue& idx, sAggregateValue& val);
  };
  
  class cNativeObjectVarRef : public cObjectRef
  {
  private:
    uAnyType& m_var;
    
  public:
    cNativeObjectVarRef(uAnyType& var) : m_var(var) { ; }
    ~cNativeObjectVarRef() { ; }
    
    bool IsWritable() { return false; } 
    bool Get(sAggregateValue& val) { val.value = m_var; val.type = AS_TYPE_OBJECT_REF; return false; }
    bool Get(const sAggregateValue& idx, sAggregateValue& val);
    bool Set(sAggregateValue& val) { return false; }
    bool Set(sAggregateValue& idx, sAggregateValue& val);
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
    
    bool Get(sAggregateValue& val) { return m_obj->Get(m_idx, val); }
    bool Get(const sAggregateValue& idx, sAggregateValue& val);
    bool Set(sAggregateValue& val) { return m_obj->Set(m_idx, val); }
    bool Set(sAggregateValue& idx, sAggregateValue& val);
  };

  class cStringVarRef : public cObjectRef
  {
  private:
    uAnyType& m_var;
    
  public:
    cStringVarRef(uAnyType& var) : m_var(var) { ; }
    ~cStringVarRef() { ; }
    
    bool IsWritable() { return true; }
    
    bool Get(sAggregateValue& val) { val.value = m_var; val.type = AS_TYPE_STRING; return true; }
    bool Get(const sAggregateValue& idx, sAggregateValue& val);
    bool Set(sAggregateValue& val) { return false; }
    bool Set(sAggregateValue& idx, sAggregateValue& val);
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
  : m_storage(in_array->m_storage.GetSize()), m_ref_count(1), m_resizable(true)
{
  copy(0, in_array->m_storage);
}

inline cDirectInterpretASTVisitor::cLocalArray::cLocalArray(cLocalArray* arr1, cLocalArray* arr2)
  : m_storage(arr1->m_storage.GetSize() + arr2->m_storage.GetSize()), m_ref_count(1), m_resizable(true)
{
  copy(0, arr1->m_storage);
  copy(arr1->m_storage.GetSize(), arr2->m_storage);
}


#endif
