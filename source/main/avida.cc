//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVIDA_HH
#include "avida.hh"
#endif

#ifndef AVIDA_DRIVER_BASE_HH
#include "avida_driver_base.hh"
#endif
#ifndef CALLBACK_UTIL_HH
#include "callback_util.hh"
#endif
#ifndef CONFIG_HH
#include "config.hh"
#endif
#ifndef DEFS_HH
#include "defs.hh"
#endif
#ifndef ENVIRONMENT_HH
#include "environment.hh"
#endif
#ifndef HARDWARE_4STACK_HH
#include "hardware_4stack.hh"
#endif
#ifndef HARDWARE_UTIL_HH
#include "hardware_util.hh"
#endif
#ifndef HARDWARE_CPU_HH
#include "hardware_cpu.hh"
#endif
#ifndef INST_LIB_CPU_HH
#include "inst_lib_cpu.hh"
#endif
#ifndef POPULATION_INTERFACE_HH
#include "population_interface.hh"
#endif
#ifndef RESOURCE_COUNT_HH
#include "resource_count.hh"
#endif
#ifndef TEST_CPU_HH
#include "test_cpu.hh"
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
  if (cConfig::GetHardwareType() == HARDWARE_TYPE_CPU_ORIGINAL) {
    environment.GetInstSet().SetInstLib(cHardwareCPU::GetInstLib());
    cHardwareUtil::LoadInstSet_CPUOriginal(cConfig::GetInstFilename(),
					   environment.GetInstSet());
  }
  else if (cConfig::GetHardwareType() == HARDWARE_TYPE_CPU_4STACK) {
    environment.GetInstSet().SetInstLib(cHardware4Stack::GetInstLib());
    cHardwareUtil::LoadInstSet_CPU4Stack(cConfig::GetInstFilename(),
					   environment.GetInstSet());
  }

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







