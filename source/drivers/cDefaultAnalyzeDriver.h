/*
 *  cDefaultAnalyzeDriver.h
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cDefaultAnalyzeDriver_h
#define cDefaultAnalyzeDriver_h

#ifndef cAvidaDriver_h
#include "cAvidaDriver.h"
#endif
#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif

class cString;
class cWorld;

class cDefaultAnalyzeDriver : public cAvidaDriver, public cWorldDriver
{
private:
  cDefaultAnalyzeDriver(); // @not_implemented
  cDefaultAnalyzeDriver(const cDefaultAnalyzeDriver&); // @not_implemented
  cDefaultAnalyzeDriver& operator=(const cDefaultAnalyzeDriver&); // @not_implemented
  
protected:
  cWorld* m_world;
  bool m_interactive;
  
public:
  cDefaultAnalyzeDriver(cWorld* world, bool inter = false);
  ~cDefaultAnalyzeDriver();
  
  void Run();
  
  // Driver Actions
  void SignalBreakpoint() { return; }
  void SetDone() { return; }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);
};


#ifdef ENABLE_UNIT_TESTS
namespace nDefaultAnalyzeDriver {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
