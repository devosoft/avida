/*
 *  cAvidaDriver_Population.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAvidaDriver_Population_h
#define cAvidaDriver_Population_h

#ifndef cAvidaDriver_Base_h
#include "cAvidaDriver_Base.h"
#endif
#ifndef cString_h
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
