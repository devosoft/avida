//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cAvidaDriver_Analyze.h"

#include "cAnalyze.h"
#include "cWorld.h"

#include <iostream>

using namespace std;

//////////////////////////
//  cAvidaDriver_Analyze
//////////////////////////

cAvidaDriver_Analyze::cAvidaDriver_Analyze(cWorld* world, bool _interactive)
: interactive(_interactive), m_world(world)
{
}

cAvidaDriver_Analyze::~cAvidaDriver_Analyze() { delete m_world; }

void cAvidaDriver_Analyze::Run()
{
  cout << "In analyze mode!!" << endl;
  cAnalyze analyze(m_world);
  if (interactive == true) {
    analyze.RunInteractive();
  }
}
