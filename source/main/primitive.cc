//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef PRIMITIVE_HH
#include "primitive.hh"
#endif

#ifndef AVIDA_HH
#include "avida.hh"
#endif
#ifndef AVIDA_DRIVER_ANALYZE_HH
#include "avida_driver_analyze.hh"
#endif
#ifndef AVIDA_DRIVER_BASE_HH
#include "avida_driver_base.hh"
#endif
#ifndef AVIDA_DRIVER_POPULATION_HH
#include "avida_driver_population.hh"
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
#ifndef HARDWARE_SMT_H
#include "hardware_smt.h"
#endif
#ifndef INST_LIB_CPU_HH
#include "inst_lib_cpu.hh"
#endif
#ifndef POPULATION_HH
#include "population.hh"
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

using namespace std;

int main(int argc, char * argv[])
{
  // Catch Interrupt making sure to close appropriately
  signal(SIGINT, ExitAvida);

  // output copyright message
  cout << "Avida " << AVIDA_VERSION << endl;
  cout << "----------------------------------------------------------------------" << endl;
  cout << "Copyright (C) 1993-2003 California Institute of Technology." << endl;
  cout << "Copyright (C) 1999-2005 Michigan State University." << endl << endl;
  
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
