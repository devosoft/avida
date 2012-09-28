/*
 *  cASNativeObject.h
 *  Avida
 *
 *  Created by David Bryson on 9/14/08.
 *  Copyright 2008-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cASNativeObject_h
#define cASNativeObject_h

#include "AvidaScript.h"

#include "cASCPPParameter.h"
#include "cASNativeObjectMethod.h"

class cString;


class cASNativeObject 
{
private:
  int m_ref_count;
  
public:
  cASNativeObject() : m_ref_count(1)  { ; }
  virtual ~cASNativeObject() { ; }
  
  virtual const char* GetType() = 0;
  virtual void* GetObject() = 0;
  
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



template<class NativeClass>
class tASNativeObject : public cASNativeObject
{
public:
  
  
private:
  static Apto::Array<cASNativeObjectMethod<NativeClass>*>* s_methods;
  static Apto::Map<Apto::String, int>* s_method_dict;
  
  NativeClass* m_object;
  
  
public:
  tASNativeObject(NativeClass* obj) : m_object(obj) { ; }
  ~tASNativeObject() { delete m_object; }

  const char* GetType() { return typeid(m_object).name(); }
  void* GetObject() { return (void*)m_object; }
  
  cASCPPParameter CallMethod(int mid, cASCPPParameter args[]) const { return (*s_methods)[mid]->Call(m_object, args); }

  bool LookupMethod(const cString& meth_name, int& mid) { return s_method_dict->Find(meth_name, mid); }

  int GetArity(int mid) const { return (*s_methods)[mid]->GetArity(); }
  const sASTypeInfo& GetArgumentType(int mid, int arg) const { return (*s_methods)[mid]->GetArgumentType(arg); }
  const sASTypeInfo& GetReturnType(int mid) const { return (*s_methods)[mid]->GetReturnType(); }

  static void InitializeMethodRegistrar()
  {
    s_methods = new Apto::Array<cASNativeObjectMethod<NativeClass>*>();
    s_method_dict = new Apto::Map<Apto::String, int>();
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
template<class NativeClass> Apto::Array<cASNativeObjectMethod<NativeClass>*>* tASNativeObject<NativeClass>::s_methods = NULL;
template<class NativeClass> Apto::Map<Apto::String, int>* tASNativeObject<NativeClass>::s_method_dict = NULL;

  

#endif
