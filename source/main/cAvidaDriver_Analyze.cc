//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVIDA_DRIVER_ANALYZE_HH
#include "cAvidaDriver_Analyze.h"
#endif

#ifndef ANALYZE_HH
#include "cAnalyze.h"
#endif

#ifndef ENVIRONMENT_HH
#include "cEnvironment.h"
#endif

#include <iostream>

using namespace std;

//////////////////////////
//  cAvidaDriver_Analyze
//////////////////////////

cAvidaDriver_Analyze::cAvidaDriver_Analyze(bool _interactive, cEnvironment *e)
  : interactive(_interactive), d_environment(e)
{
}

cAvidaDriver_Analyze::~cAvidaDriver_Analyze()
{
}

void cAvidaDriver_Analyze::Run()
{
  cout << "In analyze mode!!" << endl;
  // DDD - get analyze config file from cAvidaConfig
  cAnalyze analyze("analyze.cfg", d_environment);
  if (interactive == true) {
    analyze.RunInteractive();
  }
}
