/*
 *  cAvidaDriver_Base.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAvidaDriver_Base_h
#define cAvidaDriver_Base_h

/**
 *  cAvidaDriver_Base
 *
 *  This class is the base class for driving an avida run (be it in analyze
 *  mode or as a population).  It includes all of the functions that should
 *  be overloaded to make any viewer work.
 **/

class cString;

class cAvidaDriver_Base {
protected:
  bool done_flag;   // This is set to true when run should finish.

public:
  static cAvidaDriver_Base * main_driver;

  cAvidaDriver_Base();
  virtual ~cAvidaDriver_Base();

  virtual void Run() = 0;  // Called when Driver should take over execution.
  virtual void ExitProgram(int exit_code);  // Called on an interupt,

  // There functions are ideally called by avida whenever a message needs to
  // be sent to the user.
  virtual void NotifyComment(const cString & in_string);
  virtual void NotifyWarning(const cString & in_string);
  virtual void NotifyError(const cString & in_string);

  // Called when the driver should stop what its doing for the moment, to
  // let the user see what just happened.
  virtual void SignalBreakpoint() { ; }

  void SetDone() { done_flag = true; }
};

#endif
