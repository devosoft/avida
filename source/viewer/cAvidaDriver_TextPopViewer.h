//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cAvidaDriver_TextPopViewer_h
#define cAvidaDriver_TextPopViewer_h

#ifndef cAvidaDriver_Population_h
#include "cAvidaDriver_Population.h"
#endif

class cView;

class cAvidaDriver_TextPopViewer : public cAvidaDriver_Population {
protected:
  cView* viewer;
public:
  cAvidaDriver_TextPopViewer(cWorld* world);
  ~cAvidaDriver_TextPopViewer();

  virtual void ProcessOrganisms();

  virtual void NotifyComment(const cString & in_string);
  virtual void NotifyWarning(const cString & in_string);
  virtual void NotifyError(const cString & in_string);
  virtual void NotifyUpdate();

  virtual void SignalBreakpoint();
};

#endif
