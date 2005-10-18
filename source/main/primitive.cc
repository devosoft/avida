/*
 *  nGeometry.h
 *  Avida2
 *
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993 - 2001 California Institute of Technology.
 *
 */

#include <signal.h>

#include "avida.h"
#include "cAvidaConfig.h"
#include "cAvidaDriver_Analyze.h"
#include "cAvidaDriver_Base.h"
#include "cAvidaDriver_Population.h"
#include "cCallbackUtil.h"
#include "cConfig.h"
#include "cEnvironment.h"
#include "cHardware4Stack.h"
#include "cHardwareUtil.h"
#include "cHardwareCPU.h"
#include "cHardwareSMT.h"
#include "cInstLibCPU.h"
#include "cPopulation.h"
#include "cPopulationInterface.h"
#include "cResourceCount.h"
#include "cTestCPU.h"
#include "cWorld.h"
#include "defs.h"

using namespace std;

int main(int argc, char * argv[])
{
  // Catch Interrupt making sure to close appropriately
  signal(SIGINT, ExitAvida);

  // output copyright message
  cout << AvidaVersion() << endl;
  cout << "----------------------------------------------------------------------" << endl;
  cout << "Copyright (C) 1999-2005 Michigan State University." << endl;
  cout << "Copyright (C) 1993-2003 California Institute of Technology." << endl << endl;
  
  cout << "Avida comes with ABSOLUTELY NO WARRANTY." << endl;
  cout << "This is free software, and you are welcome to redistribute it" << endl;
  cout << "under certain conditions. See file COPYING for details." << endl << endl;
  
  // Initialize the configuration data...
  //cerr << "<newconfig>" << endl;
  //cWorld* world = new cWorld(cAvidaConfig::LoadWithCmdLineArgs(argc, argv));
  //cerr << "</newconfig>" << endl << endl;
  
  cConfig::InitGroupList();
  cConfig::Setup(argc, argv);
  cConfig::SetupMS();

  cEnvironment environment;
  cPopulationInterface test_interface;

  SetupAvida(environment, test_interface);

  if (cConfig::GetAnalyzeMode() == true) {
    cAvidaDriver_Base::main_driver =
      new cAvidaDriver_Analyze(cConfig::GetInteractiveAnalyze(), &environment);
  }
  else {
    cAvidaDriver_Base::main_driver = new cAvidaDriver_Population(environment);
  }

  cout << endl;
  
  cAvidaDriver_Base::main_driver->Run();

  // Exit Nicely
  ExitAvida(0);
}
