/*
 *  cTextViewerDriver.h
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cTextViewerDriver_h
#define cTextViewerDriver_h

#ifndef cAvidaDriver_h
#include "cAvidaDriver.h"
#endif
#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif

class cString;
class cView;
class cWorld;

class cTextViewerDriver : public cAvidaDriver, public cWorldDriver
{
private:
  cTextViewerDriver();  // not implemented
  
protected:
  cWorld* m_world;
  bool m_done;  // This is set to true when run should finish.

  cView* m_view;
  
public:
  cTextViewerDriver(cWorld* world);
  ~cTextViewerDriver();
  
  void Run();
  
  // Driver Actions
  void SignalBreakpoint();
  void SetDone() { m_done = true; }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif
