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
#include "cAvidaDriver_Base.h"
#endif

class cWorld;

class cAvidaDriver_Analyze : public cAvidaDriver_Base {
protected:
  bool interactive;
  cWorld* m_world;
public:
  cAvidaDriver_Analyze(cWorld* world, bool _interactive=false);
  virtual ~cAvidaDriver_Analyze();
  virtual void Run();
};

#endif
