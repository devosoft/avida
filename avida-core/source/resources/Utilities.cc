//
//  Utilities.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 2/15/18.
//

#include "Utilities.h"
#include "cEnvironment.h"
#include "cWorld.h"


ResourceLibraryInterface* GetResRegInterface(cWorld* m_world)
{
  return &m_world->GetEnvironment().GetResLib();
}
