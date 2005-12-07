/*
 *  cAvidaDriver_Analyze.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cAvidaDriver_Analyze.h"

#include "cAnalyze.h"

#include <iostream>

using namespace std;


void cAvidaDriver_Analyze::Run()
{
  cout << "In analyze mode!!" << endl;
  cAnalyze analyze(m_world);
  if (m_interactive == true) {
    analyze.RunInteractive();
  }
}
