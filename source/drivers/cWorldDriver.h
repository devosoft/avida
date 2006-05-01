/*
 *  cWorldDriver.h
 *  Avida
 *
 *  Created by David on 12/10/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cWorldDriver_h
#define cWorldDriver_h

// This class is an abstract base class that is used by actions within
// a cWorld to notify its driver of various states and conditions.

class cString;

class cWorldDriver
{
private:
  cWorldDriver(const cWorldDriver&); // @not_implemented
  cWorldDriver& operator=(const cWorldDriver&); // @not_implemented

public:
  cWorldDriver() { ; }
  virtual ~cWorldDriver() { ; }
  
  // Driver Actions
  virtual void SignalBreakpoint() = 0;
  virtual void SetDone() = 0;

  virtual void RaiseException(const cString& in_string) = 0;
  virtual void RaiseFatalException(int exit_code, const cString& in_string) = 0;
  
  // Notifications
  virtual void NotifyComment(const cString& in_string) = 0;
  virtual void NotifyWarning(const cString& in_string) = 0;

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif
