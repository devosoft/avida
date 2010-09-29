//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1999-2009 Michigan State University                        //
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
  
  virtual void Setup(cAvidaContext& ctx, const cString& in_name) = 0;
  virtual void SetViewMode(int in_mode) { (void) in_mode; }

  virtual bool ProcessKeypress(cAvidaContext& ctx, int keypress) = 0;

  virtual void NewUpdate(cAvidaContext& ctx) { ; }
  virtual void NotifyUpdate(cAvidaContext& ctx) { ; }
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

  virtual void Refresh(cAvidaContext& ctx) = 0;

  // Methods called by sub-windows.
  virtual void Redraw() = 0;
};

#endif
