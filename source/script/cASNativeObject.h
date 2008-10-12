/*
 *  cASNativeObject.h
 *  Avida
 *
 *  Created by David Bryson on 9/14/08.
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

#ifndef cASNativeObject_h
#define cASNativeObject_h

#include "AvidaScript.h"

#include "cASCPPParameter.h"
#include "tArray.h"
#include "tDictionary.h"

#include <typeinfo>

class cString;


class cASNativeObject 
{
private:
  int m_ref_count;
  
public:
  cASNativeObject() : m_ref_count(1)  { ; }
  virtual ~cASNativeObject() { ; }
  
  virtual const char* GetType() = 0;
  
  virtual cASCPPParameter CallMethod(int mid, cASCPPParameter args[]) const = 0;
  
  //virtual bool LookupValue(const cString& val_name, int& vid) = 0;
  virtual bool LookupMethod(const cString& meth_name, int& mid) = 0;
  
  virtual int GetArity(int mid) const = 0;
  virtual const sASTypeInfo& GetArgumentType(int mid, int arg) const = 0;
  virtual const sASTypeInfo& GetReturnType(int mid) const = 0;
  
  inline cASNativeObject* GetReference() { m_ref_count++; return this; }
  inline void RemoveReference() { m_ref_count--; if (m_ref_count == 0) delete this; }
  inline bool IsShared() { return (m_ref_count > 1); }
};



template <class NativeClass>
class cASNativeObjectMethod
{
public:
  virtual ~cASNativeObjectMethod() { ; }
  
  virtual int GetArity() const = 0;
  virtual const sASTypeInfo& GetArgumentType(int arg) const = 0;
  virtual const sASTypeInfo& GetReturnType() const = 0;

  virtual cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const = 0;
};


template<class NativeClass, class FunctionType> class tASNativeObjectMethod;

template<class NativeClass, class ReturnType, class Arg1Type>
class tASNativeObjectMethod<NativeClass, ReturnType (Arg1Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  ReturnType (NativeClass::*m_method)(Arg1Type);
  
public:
  tASNativeObjectMethod(ReturnType (NativeClass::*method)(Arg1Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature = AvidaScript::TypeOf<Arg1Type>();
  }
  
  int GetArity() const { return 1; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((object->*m_method)(args[0].Get<Arg1Type>()));
    return rvalue;
  }
};



template<class NativeClass>
class tASNativeObject : public cASNativeObject
{
public:
  
  
private:
  static tArray<cASNativeObjectMethod<NativeClass>*>* s_methods;
  static tDictionary<int>* s_method_dict;
  
  NativeClass* m_object;

  
public:
  tASNativeObject(NativeClass* obj) : m_object(obj) { ; }
  ~tASNativeObject() { delete m_object; }

  const char* GetType() { return typeid(m_object).name(); }
  
  cASCPPParameter CallMethod(int mid, cASCPPParameter args[]) const { return (*s_methods)[mid]->Call(m_object, args); }

  bool LookupMethod(const cString& meth_name, int& mid) { return s_method_dict->Find(meth_name, mid); }

  int GetArity(int mid) const { return (*s_methods)[mid]->GetArity(); }
  const sASTypeInfo& GetArgumentType(int mid, int arg) const { return (*s_methods)[mid]->GetArgumentType(arg); }
  const sASTypeInfo& GetReturnType(int mid) const { return (*s_methods)[mid]->GetReturnType(); }

  static void InitializeMethodRegistrar()
  {
    s_methods = new tArray<cASNativeObjectMethod<NativeClass>*>();
    s_method_dict = new tDictionary<int>();
    
  }
  static void RegisterMethod(cASNativeObjectMethod<NativeClass>* method, const cString& name)
  {
    int mid = s_methods->Push(method);
    s_method_dict->Add(name, mid);
  }

  static void DestroyMethodRegistrar()
  {
    for (int i = 0; i < s_methods->GetSize(); i++) delete *s_methods[i];
    delete s_methods;
    delete s_method_dict;
  }
};
template<class NativeClass> tArray<cASNativeObjectMethod<NativeClass>*>* tASNativeObject<NativeClass>::s_methods = NULL;
template<class NativeClass> tDictionary<int>* tASNativeObject<NativeClass>::s_method_dict = NULL;

  

#endif
