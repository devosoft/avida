//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef avida_h
#include "avida.h"
#endif

#ifndef AVIDA_DRIVER_BASE_HH
#include "cAvidaDriver_Base.h"
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
#ifndef INST_LIB_CPU_HH
#include "cInstLibCPU.h"
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
#include <stdio.h>

using namespace std;


void ExitAvida(int exit_code)
{
  signal(SIGINT, SIG_IGN);          // Ignore all future interupts.
  delete cAvidaDriver_Base::main_driver;
  exit(exit_code);
}


void SetupAvida(
		cEnvironment &environment,
		cPopulationInterface &test_interface )
{
  // Build the instruction set.
  cHardwareUtil::LoadInstSet(cConfig::GetInstFilename(), environment.GetInstSet());

  // Initialize the default environment...
  if (environment.Load(cConfig::GetEnvironmentFilename()) == false) {
    cerr << "Unable to load environment... aborting!" << endl;
    ExitAvida(-1);
  }

  cConfig::SetNumInstructions(environment.GetInstSet().GetSize());
  cConfig::SetNumTasks(environment.GetTaskLib().GetSize());
  cConfig::SetNumReactions(environment.GetReactionLib().GetSize());
  cConfig::SetNumResources(environment.GetResourceLib().GetSize());

  // Build a test population interface.
  test_interface.SetFun_NewHardware(&cCallbackUtil::CB_NewHardware);
  test_interface.SetFun_Recycle(&cCallbackUtil::CB_RecycleHardware);
  test_interface.SetFun_Divide(&cCallbackUtil::CB_TestDivide);
  test_interface.SetFun_GetInput(&cCallbackUtil::CB_GetInput);
  test_interface.SetFun_GetInputAt(&cCallbackUtil::CB_GetInputAt);
  test_interface.SetFun_GetResources(&cCallbackUtil::CB_GetResources);
  test_interface.SetFun_UpdateResources(&cCallbackUtil::CB_UpdateResources);
  test_interface.SetFun_ReceiveValue(&cCallbackUtil::CB_ReceiveValue);


  int rSize = environment.GetResourceLib().GetSize();

  // Setup the test CPUs.
  cTestCPU::Setup(&(environment.GetInstSet()), &environment, rSize,
		   test_interface);
}







