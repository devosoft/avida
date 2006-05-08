/*
 *  cAnalyzeCommandAction.h
 *  Avida
 *
 *  Created by David on 4/10/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cAnalyzeCommandAction_h
#define cAnalyzeCommandAction_h

#ifndef cAction_h
#include "cAction.h"
#endif
#ifndef cActionLibrary_h
#include "cActionLibrary.h"
#endif
#ifndef cAnalyzeCommandDefBase_h
#include "cAnalyzeCommandDefBase.h"
#endif
#ifndef cWorld_h
#include "cWorld.h"
#endif

class cAnalyze;
class cAnalyzeCommand;

class cAnalyzeCommandAction : public cAnalyzeCommandDefBase
{
private:
  cWorld* m_world;

public:
  cAnalyzeCommandAction(const cString& in_name, cWorld* world)
    : cAnalyzeCommandDefBase(in_name), m_world(world) { ; }
  ~cAnalyzeCommandAction() { ; }
  
  void Run(cAnalyze* analyze, const cString& args, cAnalyzeCommand& command) const
  {
    cAction* action = m_world->GetActionLibrary().Create(name, m_world, args);
    cAvidaContext& ctx = m_world->GetDefaultContext();
    action->Process(ctx);
  }
};


#ifdef ENABLE_UNIT_TESTS
namespace nAnalyzeCommandAction {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
