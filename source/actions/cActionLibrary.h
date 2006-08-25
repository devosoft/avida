/*
 *  cActionLibrary.h
 *  Avida
 *
 *  Created by David on 4/8/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cActionLibrary_h
#define cActionLibrary_h

#ifndef cAction_h
#include "cAction.h"
#endif
#ifndef tObjectFactory_h
#include "tObjectFactory.h"
#endif

class cWorld;
class cString;

class cActionLibrary
{
private:
  typedef const cString (*ClassDescFunction)();
  
  tObjectFactory<cAction* (cWorld*, const cString&)> m_factory;
  tDictionary<ClassDescFunction> m_desc_funcs;

public:
  cActionLibrary() { ; }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    ClassDescFunction func;
    if (m_desc_funcs.Find(key, func)) return false;
    m_desc_funcs.Add(key, &ClassType::GetDescription);
    return m_factory.Register<ClassType>(key);
  }
  bool Unregister(const cString& key)
  {
    m_desc_funcs.Remove(key);
    return m_factory.Unregister(key);
  }
  
  cAction* Create(const cString& key, cWorld* world, const cString& args) { return m_factory.Create(key, world, args); }
  bool Supports(const cString& key) const { return m_factory.Supports(key); }  
  
  const cString Describe(const cString& key) const
  {
    ClassDescFunction func;
    if (m_desc_funcs.Find(key, func)) return func();
    return "(Not Available)";
  }  
  const cString DescribeAll() const;
  
  static cActionLibrary* ConstructDefaultActionLibrary();
};


#ifdef ENABLE_UNIT_TESTS
namespace nActionLibrary {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
