/*
 *  cDefaultRunDriver.h
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cDefaultRunDriver_h
#define cDefaultRunDriver_h

#ifndef cAvidaDriver_h
#include "cAvidaDriver.h"
#endif
#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif

class cString;
class cWorld;

class cDefaultRunDriver : public cAvidaDriver, public cWorldDriver
{
private:
  cDefaultRunDriver();  // not implemented

protected:
  cWorld* m_world;
  bool m_done;  // This is set to true when run should finish.

public:
  cDefaultRunDriver(cWorld* world);
  ~cDefaultRunDriver();
  
  void Run();
  
  // Driver Actions
  void SignalBreakpoint() { return; }
  void SetDone() { m_done = true; }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);
};

#endif
