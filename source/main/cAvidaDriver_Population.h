//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVIDA_DRIVER_POPULATION_HH
#define AVIDA_DRIVER_POPULATION_HH

#ifndef AVIDA_DRIVER_BASE_HH
#include "cAvidaDriver_Base.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef cWorld_h
#include "cWorld.h"
#endif

class cChangeList;

class cAvidaDriver_Population : public cAvidaDriver_Base {
protected:
  cWorld* m_world;

public:
  cWorld& GetWorld() { return *m_world; }

  /**
   * Processes one complete update.
   *
   * Returns true when finished.
   **/
  virtual bool ProcessUpdate();
  virtual void ProcessOrganisms();
			
  cAvidaDriver_Population(cWorld* world, cChangeList* change_list = 0) : m_world(world) { ; }
  virtual ~cAvidaDriver_Population() { delete m_world; }
  
  virtual void Run();
  virtual void NotifyUpdate();
};

#endif
