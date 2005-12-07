/*
 *  cAvidaDriver_Analyze.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAvidaDriver_Analyze_h
#define cAvidaDriver_Analyze_h

#include <cstdlib>

#ifndef cAvidaDriver_Base_h
#include "cAvidaDriver_Base.h"
#endif

class cWorld;

class cAvidaDriver_Analyze : public cAvidaDriver_Base {
protected:
  bool m_interactive;
  cWorld* m_world;
public:
  cAvidaDriver_Analyze(cWorld* world, bool inter = false) : m_interactive(inter), m_world(world) { ; }
  ~cAvidaDriver_Analyze() { delete m_world; }
  
  virtual void Run();
};

#endif
