//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVIDA_DRIVER_ANALYZE_HH
#define AVIDA_DRIVER_ANALYZE_HH

#include <cstdlib>

#ifndef AVIDA_DRIVER_BASE_HH
#include "avida_driver_base.hh"
#endif

class cEnvironment;

class cAvidaDriver_Analyze : public cAvidaDriver_Base {
protected:
  bool interactive;
  cEnvironment *d_environment;
public:
  cAvidaDriver_Analyze(bool _interactive=false, cEnvironment* = NULL);
  virtual ~cAvidaDriver_Analyze();
  virtual void Run();
};

#endif
