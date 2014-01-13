/*
 *  tObjectFactory.h
 *  Avida
 *
 *  Created by David on 6/10/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#ifndef tObjectFactory_h
#define tObjectFactory_h

#include "apto/core/Mutex.h"

#include "tList.h"


template<typename CtorSignature> class tObjectFactory;

namespace nObjectFactory
{
  template<typename BaseType, typename ClassType>
  BaseType createObject()
  {
    return new ClassType();
  }

  template<typename BaseType, typename ClassType, typename Arg1Type>
  BaseType createObject(Arg1Type arg1)
  {
    return new ClassType(arg1);
  }
  
  template<typename BaseType, typename ClassType, typename Arg1Type, typename Arg2Type>
  BaseType createObject(Arg1Type arg1, Arg2Type arg2)
  {
    return new ClassType(arg1, arg2);
  }
  
  template<typename BaseType, typename ClassType, typename Arg1Type, typename Arg2Type, typename Arg3Type>
  BaseType createObject(Arg1Type arg1, Arg2Type arg2, Arg3Type arg3)
  {
    return new ClassType(arg1, arg2, arg3);
  }
}

template<typename BaseType>
class tObjectFactory<BaseType ()>
{
protected:
  typedef BaseType (*CreateObjectFunction)();
  
  Apto::Map<Apto::String, CreateObjectFunction> m_create_funcs;
  mutable Apto::Mutex m_mutex;
  
public:
  tObjectFactory() { ; }
  ~tObjectFactory() { ; }

  template<typename ClassType> bool Register(const Apto::String& key)
  {
    Apto::MutexAutoLock lock(m_mutex);
    CreateObjectFunction func = NULL;
    if (m_create_funcs.Get(key, func)) {
      return false;
    }
    
    m_create_funcs.Set(key, &nObjectFactory::createObject<BaseType, ClassType>);
    return true;
  }
  
  bool Unregister(const Apto::String& key)
  {
    Apto::MutexAutoLock lock(m_mutex);
    return m_create_funcs.Remove(key);
  }
  
  BaseType Create(const Apto::String& key)
  {
    CreateObjectFunction func = NULL;
    Apto::MutexAutoLock lock(m_mutex);
    if (m_create_funcs.Get(key, func)) {
      return func();
    }
    return NULL;
  }
  
  void CreateAll(Apto::Array<BaseType>& objects)
  {
    tList<Apto::String> names;
    tList<CreateObjectFunction> funcs;
    Apto::MutexAutoLock lock(m_mutex);
    
    m_create_funcs.AsLists(names, funcs);
    objects.Resize(names.GetSize());
    
    tListIterator<Apto::String> names_it(names);
    for (int i = 0; names_it.Next() != NULL; i++) {
      CreateObjectFunction func = NULL;
      m_create_funcs.Get(*names_it.Get(), func);
      objects[i] = func();
    }
  }
  
  bool Supports(const Apto::String& key) const
  {
    Apto::MutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.Has(key);
    return supports;
  }
};



template<typename BaseType, typename Arg1Type>
class tObjectFactory<BaseType (Arg1Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type);
  
  Apto::Map<Apto::String, CreateObjectFunction> m_create_funcs;
  mutable Apto::Mutex m_mutex;
  
public:
  tObjectFactory() { ; }
  ~tObjectFactory() { ; }
  
  template<typename ClassType> bool Register(const Apto::String& key)
  {
    CreateObjectFunction func = NULL;
    Apto::MutexAutoLock lock(m_mutex);
    if (m_create_funcs.Get(key, func)) {
      return false;
    }
    
    m_create_funcs.Set(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type>);
    return true;
  }
  
  bool Unregister(const Apto::String& key)
  {
    Apto::MutexAutoLock lock(m_mutex);
    return m_create_funcs.Remove(key);
  }
  
  BaseType Create(const Apto::String& key, Arg1Type arg1)
  {
    CreateObjectFunction func = NULL;
    Apto::MutexAutoLock lock(m_mutex);
    if (m_create_funcs.Get(key, func)) {
      return func(arg1);
    }
    return NULL;
  }

  bool Supports(const Apto::String& key) const
  {
    Apto::MutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.Has(key);
    return supports;
  }
};




template<typename BaseType, typename Arg1Type, typename Arg2Type>
class tObjectFactory<BaseType (Arg1Type, Arg2Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type, Arg2Type);
  
  Apto::Map<Apto::String, CreateObjectFunction> m_create_funcs;
  mutable Apto::Mutex m_mutex;
  
public:
  tObjectFactory() { ; }
  ~tObjectFactory() { ; }
  
  template<typename ClassType> bool Register(const Apto::String& key)
  {
    CreateObjectFunction func = NULL;
    Apto::MutexAutoLock lock(m_mutex);
    if (m_create_funcs.Get(key, func)) {
      return false;
    }
    
    m_create_funcs.Set(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type, Arg2Type>);
    return true;
  }
  
  bool Unregister(const Apto::String& key)
  {
    Apto::MutexAutoLock lock(m_mutex);
    return m_create_funcs.Remove(key);
  }
  
  BaseType Create(const Apto::String& key, Arg1Type arg1, Arg2Type arg2)
  {
    CreateObjectFunction func = NULL;
    Apto::MutexAutoLock lock(m_mutex);
    if (m_create_funcs.Get(key, func)) {
      return func(arg1, arg2);
    }
    return NULL;
  }

  bool Supports(const Apto::String& key) const
  {
    Apto::MutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.Has(key);
    return supports;
  }
};




template<typename BaseType, typename Arg1Type, typename Arg2Type, typename Arg3Type>
class tObjectFactory<BaseType (Arg1Type, Arg2Type, Arg3Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type, Arg2Type, Arg3Type);
  
  Apto::Map<Apto::String, CreateObjectFunction> m_create_funcs;
  mutable Apto::Mutex m_mutex;
  
public:
  tObjectFactory() { ; }
  ~tObjectFactory() { ; }
  
  template<typename ClassType> bool Register(const Apto::String& key)
  {
    CreateObjectFunction func = NULL;
    Apto::MutexAutoLock lock(m_mutex);
    if (m_create_funcs.Get(key, func)) {
      return false;
    }
    
    m_create_funcs.Set(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type, Arg2Type, Arg3Type>);
    return true;
  }
  
  bool Unregister(const Apto::String& key)
  {
    Apto::MutexAutoLock lock(m_mutex);
    return m_create_funcs.Remove(key);
  }
  
  BaseType Create(const Apto::String& key, Arg1Type arg1, Arg2Type arg2, Arg3Type arg3)
  {
    CreateObjectFunction func = NULL;
    Apto::MutexAutoLock lock(m_mutex);
    if (m_create_funcs.Get(key, func)) {
      return func(arg1, arg2, arg3);
    }
    return NULL;
  }

  bool Supports(const Apto::String& key) const
  {
    Apto::MutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.Has(key);
    return supports;
  }
};


#endif
