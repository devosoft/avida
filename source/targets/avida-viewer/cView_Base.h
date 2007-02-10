//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1999-2007 Michigan State University                             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cView_Base_h
#define cView_Base_h

class cString;

class cView_Base {
public:
  cView_Base() { ; }
  virtual ~cView_Base() { ; }
  
  virtual void Setup(const cString & in_name) = 0;
  virtual void SetViewMode(int in_mode) { (void) in_mode; }

  virtual bool ProcessKeypress(int keypress) = 0;

  virtual void NewUpdate() { ; }
  virtual void NotifyUpdate() { ; }
  virtual void NotifyError(const cString & in_string) = 0;
  virtual void NotifyWarning(const cString & in_string) = 0;
  virtual void NotifyComment(const cString & in_string) = 0;
  virtual void NotifyOutput(const cString & in_string) = 0;
  virtual void Pause() { ; }
  virtual void DoBreakpoint() { ; }

  virtual int Confirm(const cString & message) = 0;
  virtual void Notify(const cString & message) = 0;

  virtual int GetStepOrganism() { return -1; }
  virtual void SetStepOrganism(int in_id) { (void) in_id; }

  virtual void Refresh() = 0;

  // Methods called by sub-windows.
  virtual void Redraw() = 0;
};

#endif
