/*
 *  cASFunction.h
 *  Avida
 *
 *  Created by David on 3/16/08.
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

#ifndef cASFunction_h
#define cASFunction_h

#include "AvidaScript.h"

#include "cASCPPParameter.h"
#include "cString.h"


class cASFunction
{
protected:
  cString m_name;
  sASTypeInfo m_rtype;
  
  
public:
  cASFunction(const cString& name) : m_name(name) { ; }
  virtual ~cASFunction() { ; }
  
  const cString& GetName() const { return m_name; }

  virtual int GetArity() const = 0;
  const sASTypeInfo& GetReturnType() const { return m_rtype; }
  virtual const sASTypeInfo& GetArgumentType(int arg) const = 0;
  
  virtual cASCPPParameter Call(cASCPPParameter args[]) const = 0;
};


template<typename FunctionType> class tASFunction;


template<typename ReturnType, typename Arg1Type> 
class tASFunction<ReturnType (Arg1Type)> : public cASFunction
{
private:
  typedef ReturnType (*TrgtFunType)(Arg1Type);

  sASTypeInfo m_signature;
  TrgtFunType m_func;
  
  
public:
  tASFunction(TrgtFunType func, const cString& name) : cASFunction(name), m_func(func)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature = AvidaScript::TypeOf<Arg1Type>();
  }
  
  int GetArity() const { return 1; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  
  cASCPPParameter Call(cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((*m_func)(args[0].Get<Arg1Type>()));
    return rvalue;
  }
};
                  

template<typename Arg1Type> 
class tASFunction<void (Arg1Type)> : public cASFunction
{
private:
  typedef void (*TrgtFunType)(Arg1Type);
  
  sASTypeInfo m_signature;
  TrgtFunType m_func;
  
  
public:
  tASFunction(TrgtFunType func, const cString& name) : cASFunction(name), m_func(func)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature = AvidaScript::TypeOf<Arg1Type>();
  }
  
  int GetArity() const { return 1; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  
  cASCPPParameter Call(cASCPPParameter args[]) const
  {
    (*m_func)(args[0].Get<Arg1Type>());
    
    return cASCPPParameter();
  }
};




template<typename ReturnType, typename Arg1Type, typename Arg2Type> 
class tASFunction<ReturnType (Arg1Type, Arg2Type)> : public cASFunction
{
private:
  typedef ReturnType (*TrgtFunType)(Arg1Type, Arg2Type);
  
  sASTypeInfo m_signature[2];
  TrgtFunType m_func;
  
  
public:
  tASFunction(TrgtFunType func, const cString& name) : cASFunction(name), m_func(func)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
  }
  
  int GetArity() const { return 2; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  
  cASCPPParameter Call(cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((*m_func)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>()));
    return rvalue;
  }
};


template<typename Arg1Type, typename Arg2Type> 
class tASFunction<void (Arg1Type, Arg2Type)> : public cASFunction
{
private:
  typedef void (*TrgtFunType)(Arg1Type, Arg2Type);
  
  sASTypeInfo m_signature[2];
  TrgtFunType m_func;
  
  
public:
  tASFunction(TrgtFunType func, const cString& name) : cASFunction(name), m_func(func)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
  }
  
  int GetArity() const { return 2; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  
  cASCPPParameter Call(cASCPPParameter args[]) const
  {
    (*m_func)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>());
    
    return cASCPPParameter();
  }
};




template<typename ReturnType, typename Arg1Type, typename Arg2Type, typename Arg3Type> 
class tASFunction<ReturnType (Arg1Type, Arg2Type, Arg3Type)> : public cASFunction
{
private:
  typedef ReturnType (*TrgtFunType)(Arg1Type, Arg2Type, Arg3Type);
  
  sASTypeInfo m_signature[3];
  TrgtFunType m_func;
  
  
public:
  tASFunction(TrgtFunType func, const cString& name) : cASFunction(name), m_func(func)
  {
    m_rtype = AvidaScript::TypeOf<ReturnType>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
    m_signature[2] = AvidaScript::TypeOf<Arg3Type>();
  }
  
  int GetArity() const { return 3; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  
  cASCPPParameter Call(cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set((*m_func)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>(), args[2].Get<Arg3Type>()));
    return rvalue;
  }
};


template<typename Arg1Type, typename Arg2Type, typename Arg3Type> 
class tASFunction<void (Arg1Type, Arg2Type, Arg3Type)> : public cASFunction
{
private:
  typedef void (*TrgtFunType)(Arg1Type, Arg2Type, Arg3Type);
  
  sASTypeInfo m_signature[3];
  TrgtFunType m_func;
  
  
public:
  tASFunction(TrgtFunType func, const cString& name) : cASFunction(name), m_func(func)
  {
    m_rtype = AvidaScript::TypeOf<void>();
    m_signature[0] = AvidaScript::TypeOf<Arg1Type>();
    m_signature[1] = AvidaScript::TypeOf<Arg2Type>();
    m_signature[2] = AvidaScript::TypeOf<Arg3Type>();
  }
  
  int GetArity() const { return 2; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature[arg]; }
  
  cASCPPParameter Call(cASCPPParameter args[]) const
  {
    (*m_func)(args[0].Get<Arg1Type>(), args[1].Get<Arg2Type>(), args[2].Get<Arg3Type>());
    
    return cASCPPParameter();
  }
};



#endif
