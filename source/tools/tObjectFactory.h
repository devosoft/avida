/*
 *  tObjectFactory.h
 *  Avida
 *
 *  Created by David on 6/10/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef tDictionary_h
#include "tDictionary.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

#include <pthread.h>

class cString;


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
  
  tDictionary<CreateObjectFunction> m_create_funcs;
  mutable pthread_mutex_t m_mutex;
  
public:
  tObjectFactory() { pthread_mutex_init(&m_mutex, NULL); }
  virtual ~tObjectFactory() { pthread_mutex_destroy(&m_mutex); }

  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func;
    pthread_mutex_lock(&m_mutex);
    if (m_create_funcs.Find(key, func)) {
      pthread_mutex_unlock(&m_mutex);
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType>);
    pthread_mutex_unlock(&m_mutex);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    pthread_mutex_lock(&m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    pthread_mutex_unlock(&m_mutex);
    return (func != NULL);
  }
  
  virtual BaseType Create(const cString& key)
  {
    CreateObjectFunction func;
    pthread_mutex_lock(&m_mutex);
    if (m_create_funcs.Find(key, func)) {
      pthread_mutex_unlock(&m_mutex);
      return func();
    }
    pthread_mutex_unlock(&m_mutex);
    return NULL;
  }
  
  virtual void CreateAll(tArray<BaseType>& objects)
  {
    tList<cString> names;
    tList<CreateObjectFunction> funcs;
    
    pthread_mutex_lock(&m_mutex);
    
    m_create_funcs.AsLists(names, funcs);
    objects.Resize(names.GetSize());
    
    tListIterator<cString> names_it(names);
    for (int i = 0; names_it.Next() != NULL; i++) {
      CreateObjectFunction func = NULL;
      m_create_funcs.Find(*names_it.Get(), func);
      objects[i] = func();
    }

    pthread_mutex_unlock(&m_mutex);
  }
  
  bool Supports(const cString& key) const
  {
    pthread_mutex_lock(&m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    pthread_mutex_unlock(&m_mutex);
    return supports;
  }
};

template<typename BaseType, typename Arg1Type>
class tObjectFactory<BaseType (Arg1Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type);
  
  tDictionary<CreateObjectFunction> m_create_funcs;
  mutable pthread_mutex_t m_mutex;
  
public:
  tObjectFactory() { pthread_mutex_init(&m_mutex, NULL); }
  virtual ~tObjectFactory() { pthread_mutex_destroy(&m_mutex); }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func;
    pthread_mutex_lock(&m_mutex);
    if (m_create_funcs.Find(key, func)) {
      pthread_mutex_unlock(&m_mutex);
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type>);
    pthread_mutex_unlock(&m_mutex);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    pthread_mutex_lock(&m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    pthread_mutex_unlock(&m_mutex);
    return (func != NULL);
  }
  
  virtual BaseType Create(const cString& key, Arg1Type arg1)
  {
    CreateObjectFunction func;
    pthread_mutex_lock(&m_mutex);
    if (m_create_funcs.Find(key, func)) {
      pthread_mutex_unlock(&m_mutex);
      return func(arg1);
    }
    pthread_mutex_unlock(&m_mutex);
    return NULL;
  }

  bool Supports(const cString& key) const
  {
    pthread_mutex_lock(&m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    pthread_mutex_unlock(&m_mutex);
    return supports;
  }
};

template<typename BaseType, typename Arg1Type, typename Arg2Type>
class tObjectFactory<BaseType (Arg1Type, Arg2Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type, Arg2Type);
  
  tDictionary<CreateObjectFunction> m_create_funcs;
  mutable pthread_mutex_t m_mutex;
  
public:
  tObjectFactory() { pthread_mutex_init(&m_mutex, NULL); }
  virtual ~tObjectFactory() { pthread_mutex_destroy(&m_mutex); }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func;
    pthread_mutex_lock(&m_mutex);
    if (m_create_funcs.Find(key, func)) {
      pthread_mutex_unlock(&m_mutex);
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type, Arg2Type>);
    pthread_mutex_unlock(&m_mutex);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    pthread_mutex_lock(&m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    pthread_mutex_unlock(&m_mutex);
    return (func != NULL);
  }
  
  virtual BaseType Create(const cString& key, Arg1Type arg1, Arg2Type arg2)
  {
    CreateObjectFunction func;
    pthread_mutex_lock(&m_mutex);
    if (m_create_funcs.Find(key, func)) {
      pthread_mutex_unlock(&m_mutex);
      return func(arg1, arg2);
    }
    pthread_mutex_unlock(&m_mutex);
    return NULL;
  }

  bool Supports(const cString& key) const
  {
    pthread_mutex_lock(&m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    pthread_mutex_unlock(&m_mutex);
    return supports;
  }
};

template<typename BaseType, typename Arg1Type, typename Arg2Type, typename Arg3Type>
class tObjectFactory<BaseType (Arg1Type, Arg2Type, Arg3Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type, Arg2Type, Arg3Type);
  
  tDictionary<CreateObjectFunction> m_create_funcs;
  mutable pthread_mutex_t m_mutex;
  
public:
  tObjectFactory() { pthread_mutex_init(&m_mutex, NULL); }
  virtual ~tObjectFactory() { pthread_mutex_destroy(&m_mutex); }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func;
    pthread_mutex_lock(&m_mutex);
    if (m_create_funcs.Find(key, func)) {
      pthread_mutex_unlock(&m_mutex);
      return false;
    }
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type, Arg2Type, Arg3Type>);
    pthread_mutex_unlock(&m_mutex);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    pthread_mutex_lock(&m_mutex);
    CreateObjectFunction func = m_create_funcs.Remove(key);
    pthread_mutex_unlock(&m_mutex);
    return (func != NULL);
  }
  
  virtual BaseType Create(const cString& key, Arg1Type arg1, Arg2Type arg2, Arg3Type arg3)
  {
    CreateObjectFunction func;
    pthread_mutex_lock(&m_mutex);
    if (m_create_funcs.Find(key, func)) {
      pthread_mutex_unlock(&m_mutex);
      return func(arg1, arg2, arg3);
    }
    pthread_mutex_unlock(&m_mutex);
    return NULL;
  }

  bool Supports(const cString& key) const
  {
    pthread_mutex_lock(&m_mutex);
    bool supports = m_create_funcs.HasEntry(key);
    pthread_mutex_unlock(&m_mutex);
    return supports;
  }
};

#endif
