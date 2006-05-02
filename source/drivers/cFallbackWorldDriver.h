/*
 *  cFallbackWorldDriver.h
 *  Avida
 *
 *  Created by David on 12/10/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cFallbackWorldDriver_h
#define cFallbackWorldDriver_h

#ifndef cDriverManager_h
#include "cDriverManager.h"
#endif
#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif

class cFallbackWorldDriver : public cWorldDriver
{
private:
  cFallbackWorldDriver(const cFallbackWorldDriver&); // @not_implemented
  cFallbackWorldDriver& operator=(const cFallbackWorldDriver&); // @not_implemented
  
public:
  cFallbackWorldDriver() { cDriverManager::Register(this); }
  ~cFallbackWorldDriver() { cDriverManager::Unregister(this); }
  
  // Driver Actions
  void SignalBreakpoint() { return; }
  void SetDone() { return; }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);


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
