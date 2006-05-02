/*
 *  cAction.h
 *  Avida
 *
 *  Created by David on 4/8/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cAction_h
#define cAction_h

#ifndef cString_h
#include "cString.h"
#endif

class cAvidaContext;
class cWorld;

class cAction
{
private:
  cAction();  // @not_implemented
  cAction(const cAction&); // @not_implemented
  cAction& operator=(const cAction&); // @not_implemented
  
protected:
  cWorld* m_world;
  cString m_args;
  
public:
  cAction(cWorld* world, const cString& args) : m_world(world), m_args(args) { ; }
  virtual ~cAction() { ; }
  
  const cString& GetArgs() const { return m_args; }
  
  virtual const cString GetDescription() = 0;
  virtual void Process(cAvidaContext& ctx) = 0;

#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif
