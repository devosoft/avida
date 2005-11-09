/*
 *  tObjectFactory.h
 *  Avida
 *
 *  Created by David on 6/10/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef TOBJECTFACTORY_H
#define TOBJECTFACTORY_H

#ifndef TDICTIONARY_HH
#include "tDictionary.h"
#endif
#ifndef TARRAY_HH
#include "tArray.h"
#endif
#ifndef TLIST_HH
#include "tList.h"
#endif

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
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    return (m_create_funcs.Remove(key) != NULL);
  }
  
  virtual BaseType Create(const cString& key)
  {
    CreateObjectFunction func;
    if (m_create_funcs.Find(key, func))
      return func();
    
    return NULL;
  }
  
  virtual void CreateAll(tArray<BaseType>& objects)
  {
    tList<cString> names;
    tList<CreateObjectFunction> funcs;
    
    m_create_funcs.AsLists(names, funcs);    
    objects.Resize(names.GetSize());
    
    tListIterator<cString> names_it(names);
    for (int i = 0; names_it.Next() != NULL; i++)
      objects[i] = Create(*names_it.Get());
  }
};

template<typename BaseType, typename Arg1Type>
class tObjectFactory<BaseType (Arg1Type)>
{
protected:
  typedef BaseType (*CreateObjectFunction)(Arg1Type);
  
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
    
    m_create_funcs.Add(key, &nObjectFactory::createObject<BaseType, ClassType, Arg1Type>);
    return true;
  }
  
  bool Unregister(const cString& key)
  {
    return (m_create_funcs.Remove(key) != NULL);
  }
  
  virtual BaseType Create(const cString& key, Arg1Type arg1)
  {
    CreateObjectFunction func;
    if (m_create_funcs.Find(key, func))
      return func(arg1);
    
    return NULL;
  }
};
#endif
