/*
 *  nGeometry.h
 *  Avida2
 *
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993 - 2001 California Institute of Technology.
 *
 */

#ifndef avida_h
#include "avida.h"
#endif
#ifndef AVIDA_DRIVER_ANALYZE_HH
#include "cAvidaDriver_Analyze.h"
#endif
#ifndef AVIDA_DRIVER_BASE_HH
#include "cAvidaDriver_Base.h"
#endif
#ifndef AVIDA_DRIVER_POPULATION_HH
#include "cAvidaDriver_Population.h"
#endif
#ifndef CALLBACK_UTIL_HH
#include "cCallbackUtil.h"
#endif
#ifndef CONFIG_HH
#include "cConfig.h"
#endif
#ifndef DEFS_HH
#include "defs.hh"
#endif
#ifndef ENVIRONMENT_HH
#include "cEnvironment.h"
#endif
#ifndef HARDWARE_4STACK_HH
#include "cHardware4Stack.h"
#endif
#ifndef HARDWARE_UTIL_HH
#include "cHardwareUtil.h"
#endif
#ifndef HARDWARE_CPU_HH
#include "cHardwareCPU.h"
#endif
#ifndef HARDWARE_SMT_H
#include "cHardwareSMT.h"
#endif
#ifndef INST_LIB_CPU_HH
#include "cInstLibCPU.h"
#endif
#ifndef POPULATION_HH
#include "cPopulation.h"
#endif
#ifndef POPULATION_INTERFACE_HH
#include "cPopulationInterface.h"
#endif
#ifndef RESOURCE_COUNT_HH
#include "cResourceCount.h"
#endif
#ifndef TEST_CPU_HH
#include "cTestCPU.h"
#endif

#include <signal.h>

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
