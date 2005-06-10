/*
 *  tObjectFactory.h
 *  Avida2
 *
 *  Created by David on 6/10/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef TOBJECTFACTORY_H
#define TOBJECTFACTORY_H

#ifndef TDICTIONARY_HH
#include "tDictionary.hh"
#endif

class cString;

namespace nObjectFactory
{
  template<typename BaseType, typename ClassType, typename ArgType>
  BaseType* createObject(const cString& key, ArgType arg)
  {
    return new ClassType(key, arg);
  }
}

template<typename BaseType, typename ArgType>
class tObjectFactory
{
protected:
  typedef BaseType* (*CreateObjectFunction)(const cString&, ArgType);
  
  tDictionary<CreateObjectFunction> m_create_funcs;
  int m_factory_id;
  
public:
    tObjectFactory() : m_factory_id(0) { ; }
  virtual ~tObjectFactory() { ; }
  
  void SetFactoryId(int id) { m_factory_id = id; }
  int GetFactoryId() { return m_factory_id; }

  template<typename ClassType> bool Register(const cString& key)
  {
    CreateObjectFunction func;
    if (m_create_funcs.Find(key, func)) return false;
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType, ArgType>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    return (m_create_funcs.Remove(key) != NULL);
  }
  
  virtual BaseType* Create(const cString& key, ArgType arg)
  {
    CreateObjectFunction func;
    if (m_create_funcs.Find(key, func))
      return func(key, arg);
    
    return NULL;
  }
};

#endif
