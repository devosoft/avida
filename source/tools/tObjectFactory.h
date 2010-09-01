/*
 *  tObjectFactory.h
 *  Avida
 *
 *  Created by David on 6/10/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef tObjectFactory_h
#define tObjectFactory_h

#ifndef cMutex_h
#include "cMutex.h"
#endif
#ifndef tDictionary_h
#include "tDictionary.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cString;


template<typename CtorSignature> class tObjectFactory;
template<typename CtorSignature> class tObjectFactoryNoCase;

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
  
  tDictionary<CreateObjectFunction> m_create_funcs;
  mutable cMutex m_mutex;
  
public:
  tObjectFactory() { ; }
  ~tObjectFactory() { ; }

  template<typename ClassType> bool Register(const cString& key)
  {
    cMutexAutoLock lock(m_mutex);
    CreateObjectFunction func = NULL;
    if (m_create_funcs.Find(key, func)) {
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    cMutexAutoLock lock(m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    return (func != NULL);
  }
  
  BaseType Create(const cString& key)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return func();
    }
    return NULL;
  }
  
  void CreateAll(tArray<BaseType>& objects)
  {
    tList<cString> names;
    tList<CreateObjectFunction> funcs;
    cMutexAutoLock lock(m_mutex);
    
    m_create_funcs.AsLists(names, funcs);
    objects.Resize(names.GetSize());
    
    tListIterator<cString> names_it(names);
    for (int i = 0; names_it.Next() != NULL; i++) {
      CreateObjectFunction func = NULL;
      m_create_funcs.Find(*names_it.Get(), func);
      objects[i] = func();
    }
  }
  
  bool Supports(const cString& key) const
  {
    cMutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    return supports;
  }
};


template<typename BaseType>
class tObjectFactoryNoCase<BaseType ()>
{
protected:
  typedef BaseType (*CreateObjectFunction)();
  
  tDictionaryNoCase<CreateObjectFunction> m_create_funcs;
  mutable cMutex m_mutex;
  
public:
  tObjectFactoryNoCase() { ; }
  ~tObjectFactoryNoCase() { ; }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    cMutexAutoLock lock(m_mutex);
    CreateObjectFunction func = NULL;
    if (m_create_funcs.Find(key, func)) {
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    cMutexAutoLock lock(m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    return (func != NULL);
  }
  
  BaseType Create(const cString& key)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return func();
    }
    return NULL;
  }
  
  void CreateAll(tArray<BaseType>& objects)
  {
    tList<cString> names;
    tList<CreateObjectFunction> funcs;
    cMutexAutoLock lock(m_mutex);
    
    m_create_funcs.AsLists(names, funcs);
    objects.Resize(names.GetSize());
    
    tListIterator<cString> names_it(names);
    for (int i = 0; names_it.Next() != NULL; i++) {
      CreateObjectFunction func = NULL;
      m_create_funcs.Find(*names_it.Get(), func);
      objects[i] = func();
    }
  }
  
  bool Supports(const cString& key) const
  {
    cMutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    return supports;
  }
};


template<typename BaseType, typename Arg1Type>
class tObjectFactory<BaseType (Arg1Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type);
  
  tDictionary<CreateObjectFunction> m_create_funcs;
  mutable cMutex m_mutex;
  
public:
  tObjectFactory() { ; }
  ~tObjectFactory() { ; }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    cMutexAutoLock lock(m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    return (func != NULL);
  }
  
  BaseType Create(const cString& key, Arg1Type arg1)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return func(arg1);
    }
    return NULL;
  }

  bool Supports(const cString& key) const
  {
    cMutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    return supports;
  }
};


template<typename BaseType, typename Arg1Type>
class tObjectFactoryNoCase<BaseType (Arg1Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type);
  
  tDictionaryNoCase<CreateObjectFunction> m_create_funcs;
  mutable cMutex m_mutex;
  
public:
  tObjectFactoryNoCase() { ; }
  ~tObjectFactoryNoCase() { ; }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    cMutexAutoLock lock(m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    return (func != NULL);
  }
  
  BaseType Create(const cString& key, Arg1Type arg1)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return func(arg1);
    }
    return NULL;
  }
  
  bool Supports(const cString& key) const
  {
    cMutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    return supports;
  }
};



template<typename BaseType, typename Arg1Type, typename Arg2Type>
class tObjectFactory<BaseType (Arg1Type, Arg2Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type, Arg2Type);
  
  tDictionary<CreateObjectFunction> m_create_funcs;
  mutable cMutex m_mutex;
  
public:
  tObjectFactory() { ; }
  ~tObjectFactory() { ; }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type, Arg2Type>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    cMutexAutoLock lock(m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    return (func != NULL);
  }
  
  BaseType Create(const cString& key, Arg1Type arg1, Arg2Type arg2)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return func(arg1, arg2);
    }
    return NULL;
  }

  bool Supports(const cString& key) const
  {
    cMutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    return supports;
  }
};


template<typename BaseType, typename Arg1Type, typename Arg2Type>
class tObjectFactoryNoCase<BaseType (Arg1Type, Arg2Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type, Arg2Type);
  
  tDictionaryNoCase<CreateObjectFunction> m_create_funcs;
  mutable cMutex m_mutex;
  
public:
  tObjectFactoryNoCase() { ; }
  ~tObjectFactoryNoCase() { ; }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return false;
    }
    
    m_create_funcs.Set(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type, Arg2Type>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    cMutexAutoLock lock(m_mutex);
    CreateObjectFunction func = NULL;
    m_create_funcs.Remove(key, func);
    return (func != NULL);
  }
  
  BaseType Create(const cString& key, Arg1Type arg1, Arg2Type arg2)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return func(arg1, arg2);
    }
    return NULL;
  }
  
  bool Supports(const cString& key) const
  {
    cMutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    return supports;
  }
};



template<typename BaseType, typename Arg1Type, typename Arg2Type, typename Arg3Type>
class tObjectFactory<BaseType (Arg1Type, Arg2Type, Arg3Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type, Arg2Type, Arg3Type);
  
  tDictionary<CreateObjectFunction> m_create_funcs;
  mutable cMutex m_mutex;
  
public:
  tObjectFactory() { ; }
  ~tObjectFactory() { ; }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type, Arg2Type, Arg3Type>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    cMutexAutoLock lock(m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    return (func != NULL);
  }
  
  BaseType Create(const cString& key, Arg1Type arg1, Arg2Type arg2, Arg3Type arg3)
  {
    CreateObjectFunction func = NULL;
    cMutexAutoLock lock(m_mutex);
    if (m_create_funcs.Find(key, func)) {
      return func(arg1, arg2, arg3);
    }
    return NULL;
  }

  bool Supports(const cString& key) const
  {
    cMutexAutoLock lock(m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    return supports;
  }
};

#endif
