/*
 *  cASFunction.h
 *  Avida
 *
 *  Created by David on 3/16/08.
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



#endif
