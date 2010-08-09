/*
 *  cASNativeObjectMethod.h
 *  Avida
 *
 *  Created by David on 10/13/08.
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

#ifndef cASNativeObjectMethod_h
#define cASNativeObjectMethod_h

#include "AvidaScript.h"

#include "cASCPPParameter.h"

#include <typeinfo>


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
template<class NativeClass, class FunctionType> class tASNativeObjectMethodConst;
template<class NativeClass, class FunctionType> class tASNativeObjectBoundFunction;


// ---------- void (void) -----------

template<class NativeClass>
class tASNativeObjectMethod<NativeClass, void ()> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  void (NativeClass::*m_method)();
  
public:
  tASNativeObjectMethod(void (NativeClass::*method)()) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature = AvidaScript::TypeOf<void>();
  }
  
  int GetArity() const { return 0; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (object->*m_method)();
    return cASCPPParameter(); // @AS_TODO - return actual void value?
  }
};


template<class NativeClass>
class tASNativeObjectMethodConst<NativeClass, void ()> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  void (NativeClass::*m_method)() const;
  
public:
  tASNativeObjectMethodConst(void (NativeClass::*method)() const) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature = AvidaScript::TypeOf<void>();
  }
  
  int GetArity() const { return 0; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (object->*m_method)();
    return cASCPPParameter(); // @AS_TODO - return actual void value?
  }
};


template<class NativeClass>
class tASNativeObjectBoundFunction<NativeClass, void ()> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  void (*m_method)(NativeClass*);
  
public:
  tASNativeObjectBoundFunction(void (*method)(NativeClass*)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature = AvidaScript::TypeOf<void>();
  }
  
  int GetArity() const { return 0; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (*m_method)(object);
    return cASCPPParameter(); // @AS_TODO - return actual void value?
  }
};


// ---------- ReturnType (void) -----------

template<class NativeClass, class ReturnType>
class tASNativeObjectMethod<NativeClass, ReturnType ()> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  ReturnType (NativeClass::*m_method)();
  
public:
  tASNativeObjectMethod(ReturnType (NativeClass::*method)()) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature = AvidaScript::TypeOf<void>();
  }
  
  int GetArity() const { return 0; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((object->*m_method)());
    return rvalue;
  }
};


template<class NativeClass, class ReturnType>
class tASNativeObjectMethodConst<NativeClass, ReturnType ()> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  ReturnType (NativeClass::*m_method)() const;
  
public:
  tASNativeObjectMethodConst(ReturnType (NativeClass::*method)() const) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature = AvidaScript::TypeOf<void>();
  }
  
  int GetArity() const { return 0; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((object->*m_method)());
    return rvalue;
  }
};


template<class NativeClass, class ReturnType>
class tASNativeObjectBoundFunction<NativeClass, ReturnType ()> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  ReturnType (*m_method)(NativeClass*);
  
public:
  tASNativeObjectBoundFunction(ReturnType (*method)(NativeClass*)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature = AvidaScript::TypeOf<void>();
  }
  
  int GetArity() const { return 0; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((*m_method)(object));
    return rvalue;
  }
};


// ---------- void (Arg1Type) -----------

template<class NativeClass, class Arg1Type>
class tASNativeObjectMethod<NativeClass, void (Arg1Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  void (NativeClass::*m_method)(Arg1Type);
  
public:
  tASNativeObjectMethod(void (NativeClass::*method)(Arg1Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature = AvidaScript::TypeOf<Arg1Type>();
  }
  
  int GetArity() const { return 1; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (object->*m_method)(args[0].Get<Arg1Type>());
    return cASCPPParameter(); // @AS_TODO - return actual void value?
  }
};


template<class NativeClass, class Arg1Type>
class tASNativeObjectMethodConst<NativeClass, void (Arg1Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  void (NativeClass::*m_method)(Arg1Type) const;
  
public:
  tASNativeObjectMethodConst(void (NativeClass::*method)(Arg1Type) const) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature = AvidaScript::TypeOf<Arg1Type>();
  }
  
  int GetArity() const { return 1; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (object->*m_method)(args[0].Get<Arg1Type>());
    return cASCPPParameter(); // @AS_TODO - return actual void value?
  }
};


template<class NativeClass, class Arg1Type>
class tASNativeObjectBoundFunction<NativeClass, void (Arg1Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  void (*m_method)(NativeClass*, Arg1Type);
  
public:
  tASNativeObjectBoundFunction(void (*method)(NativeClass*, Arg1Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature = AvidaScript::TypeOf<Arg1Type>();
  }
  
  int GetArity() const { return 1; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (*m_method)(object, args[0].Get<Arg1Type>());
    return cASCPPParameter(); // @AS_TODO - return actual void value?
  }
};


// ---------- void (Arg1Type, Arg2Type) -----------

template<class NativeClass, class Arg1Type, class Arg2Type>
class tASNativeObjectMethod<NativeClass, void (Arg1Type, Arg2Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  void (NativeClass::*m_method)(Arg1Type, Arg2Type);
  
public:
  tASNativeObjectMethod(void (NativeClass::*method)(Arg1Type, Arg2Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
  }
  
  int GetArity() const { return 2; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (object->*m_method)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>());
    return cASCPPParameter();
  }
};


template<class NativeClass, class Arg1Type, class Arg2Type>
class tASNativeObjectMethodConst<NativeClass, void (Arg1Type, Arg2Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  void (NativeClass::*m_method)(Arg1Type, Arg2Type) const;
  
public:
  tASNativeObjectMethodConst(void (NativeClass::*method)(Arg1Type, Arg2Type) const) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
  }
  
  int GetArity() const { return 2; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (object->*m_method)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>());
    return cASCPPParameter();
  }
};


template<class NativeClass, class Arg1Type, class Arg2Type>
class tASNativeObjectBoundFunction<NativeClass, void (Arg1Type, Arg2Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  void (*m_method)(NativeClass*, Arg1Type, Arg2Type);
  
public:
  tASNativeObjectBoundFunction(void (*method)(NativeClass*, Arg1Type, Arg2Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
  }
  
  int GetArity() const { return 2; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (*m_method)(object, args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>());
    return cASCPPParameter();
  }
};


// ---------- void (Arg1Type, Arg2Type, Arg3Type) -----------

template<class NativeClass, class Arg1Type, class Arg2Type, class Arg3Type>
class tASNativeObjectMethod<NativeClass, void (Arg1Type, Arg2Type, Arg3Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[3];
  void (NativeClass::*m_method)(Arg1Type, Arg2Type, Arg3Type);
  
public:
  tASNativeObjectMethod(void (NativeClass::*method)(Arg1Type, Arg2Type, Arg3Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
    m_signature[2] = AvidaScript::TypeOf<Arg3Type>();
  }
  
  int GetArity() const { return 3; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (object->*m_method)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>(), args[2].Get<Arg3Type>());
    return cASCPPParameter();
  }
};


template<class NativeClass, class Arg1Type, class Arg2Type, class Arg3Type>
class tASNativeObjectMethodConst<NativeClass, void (Arg1Type, Arg2Type, Arg3Type)>
  : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  void (NativeClass::*m_method)(Arg1Type, Arg2Type, Arg3Type) const;
  
public:
  tASNativeObjectMethodConst(void (NativeClass::*method)(Arg1Type, Arg2Type, Arg3Type) const) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
    m_signature[2] = AvidaScript::TypeOf<Arg3Type>();
  }
  
  int GetArity() const { return 3; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (object->*m_method)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>(), args[2].Get<Arg3Type>());
    return cASCPPParameter();
  }
};


template<class NativeClass, class Arg1Type, class Arg2Type, class Arg3Type>
class tASNativeObjectBoundFunction<NativeClass, void (Arg1Type, Arg2Type, Arg3Type)>
  : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  void (*m_method)(NativeClass*, Arg1Type, Arg2Type, Arg3Type);
  
public:
  tASNativeObjectBoundFunction(void (*method)(NativeClass*, Arg1Type, Arg2Type, Arg3Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
    m_signature[2] = AvidaScript::TypeOf<Arg3Type>();
  }
  
  int GetArity() const { return 3; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    (*m_method)(object, args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>(), args[2].Get<Arg3Type>());
    return cASCPPParameter();
  }
};


// ---------- ReturnType (Arg1Type) -----------

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


template<class NativeClass, class ReturnType, class Arg1Type>
class tASNativeObjectMethodConst<NativeClass, ReturnType (Arg1Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  ReturnType (NativeClass::*m_method)(Arg1Type) const;
  
public:
  tASNativeObjectMethodConst(ReturnType (NativeClass::*method)(Arg1Type) const) : m_method(method)
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


template<class NativeClass, class ReturnType, class Arg1Type>
class tASNativeObjectBoundFunction<NativeClass, ReturnType (Arg1Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature;
  ReturnType (*m_method)(NativeClass*, Arg1Type);
  
public:
  tASNativeObjectBoundFunction(ReturnType (*method)(NativeClass*, Arg1Type)) : m_method(method)
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
    rvalue.Set((*m_method)(object, args[0].Get<Arg1Type>()));
    return rvalue;
  }
};


// ---------- ReturnType (Arg1Type, Arg2Type) -----------

template<class NativeClass, class ReturnType, class Arg1Type, class Arg2Type>
class tASNativeObjectMethod<NativeClass, ReturnType (Arg1Type, Arg2Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  ReturnType (NativeClass::*m_method)(Arg1Type, Arg2Type);
  
public:
  tASNativeObjectMethod(ReturnType (NativeClass::*method)(Arg1Type, Arg2Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
  }
  
  int GetArity() const { return 2; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((object->*m_method)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>()));
    return rvalue;
  }
};


template<class NativeClass, class ReturnType, class Arg1Type, class Arg2Type>
class tASNativeObjectMethodConst<NativeClass, ReturnType (Arg1Type, Arg2Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  ReturnType (NativeClass::*m_method)(Arg1Type, Arg2Type) const;
  
public:
  tASNativeObjectMethodConst(ReturnType (NativeClass::*method)(Arg1Type, Arg2Type) const) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
  }
  
  int GetArity() const { return 2; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((object->*m_method)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>()));
    return rvalue;
  }
};


template<class NativeClass, class ReturnType, class Arg1Type, class Arg2Type>
class tASNativeObjectBoundFunction<NativeClass, ReturnType (Arg1Type, Arg2Type)> : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  ReturnType (*m_method)(NativeClass*, Arg1Type, Arg2Type);
  
public:
  tASNativeObjectBoundFunction(ReturnType (*method)(NativeClass*, Arg1Type, Arg2Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
  }
  
  int GetArity() const { return 2; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((*m_method)(object, args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>()));
    return rvalue;
  }
};


// ---------- ReturnType (Arg1Type, Arg2Type, Arg3Type) -----------

template<class NativeClass, class ReturnType, class Arg1Type, class Arg2Type, class Arg3Type>
class tASNativeObjectMethod<NativeClass, ReturnType (Arg1Type, Arg2Type, Arg3Type)>
  : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[3];
  ReturnType (NativeClass::*m_method)(Arg1Type, Arg2Type, Arg3Type);
  
public:
  tASNativeObjectMethod(ReturnType (NativeClass::*method)(Arg1Type, Arg2Type, Arg3Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
    m_signature[2] = AvidaScript::TypeOf<Arg3Type>();
  }
  
  int GetArity() const { return 3; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((object->*m_method)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>(), args[2].Get<Arg3Type>()));
    return rvalue;
  }
};


template<class NativeClass, class ReturnType, class Arg1Type, class Arg2Type, class Arg3Type>
class tASNativeObjectMethodConst<NativeClass, ReturnType (Arg1Type, Arg2Type, Arg3Type)>
  : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  ReturnType (NativeClass::*m_method)(Arg1Type, Arg2Type, Arg3Type) const;
  
public:
  tASNativeObjectMethodConst(ReturnType (NativeClass::*method)(Arg1Type, Arg2Type, Arg3Type) const) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
    m_signature[2] = AvidaScript::TypeOf<Arg3Type>();
  }
  
  int GetArity() const { return 3; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((object->*m_method)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>(), args[2].Get<Arg3Type>()));
    return rvalue;
  }
};


template<class NativeClass, class ReturnType, class Arg1Type, class Arg2Type, class Arg3Type>
class tASNativeObjectBoundFunction<NativeClass, ReturnType (Arg1Type, Arg2Type, Arg3Type)>
  : public cASNativeObjectMethod<NativeClass>
{
private:
  sASTypeInfo m_rtype;
  sASTypeInfo m_signature[2];
  ReturnType (*m_method)(NativeClass*, Arg1Type, Arg2Type, Arg3Type);
  
public:
  tASNativeObjectBoundFunction(ReturnType (*method)(NativeClass*, Arg1Type, Arg2Type, Arg3Type)) : m_method(method)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
    m_signature[2] = AvidaScript::TypeOf<Arg3Type>();
  }
  
  int GetArity() const { return 3; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  
  cASCPPParameter Call(NativeClass* object, cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((*m_method)(object, args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>(), args[2].Get<Arg3Type>()));
    return rvalue;
  }
};


#endif
