//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef VIEWERS_HH
#define VIEWERS_HH

#ifndef AVIDA_HH
#include "avida.hh"
#endif

#ifndef AVIDA_DRIVER_ANALYZE_HH
#include "avida_driver_analyze.hh"
#endif
#ifndef AVIDA_DRIVER_BASE_HH
#include "avida_driver_base.hh"
#endif
#ifndef AVIDA_DRIVER_POPULATION_HH
#include "avida_driver_population.hh"
#endif

class cView;

class cAvidaDriver_TextPopViewer : public cAvidaDriver_Population {
protected:
  cView * viewer;
public:
  cAvidaDriver_TextPopViewer(cEnvironment & environment);
  ~cAvidaDriver_TextPopViewer();

  virtual void ProcessOrganisms();

  virtual void NotifyComment(const cString & in_string);
  virtual void NotifyWarning(const cString & in_string);
  virtual void NotifyError(const cString & in_string);
  virtual void NotifyUpdate();

  virtual void SignalBreakpoint();
};

#endif
