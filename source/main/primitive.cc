/*
 *  primitive.cc
 *  Avida2
 *
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#include <signal.h>

#include "avida.h"
#include "cAvidaConfig.h"
#include "cAvidaDriver_Analyze.h"
#include "cAvidaDriver_Population.h"
#include "cWorld.h"

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
  cWorld* world = new cWorld(cAvidaConfig::LoadWithCmdLineArgs(argc, argv));
  
  if (world->GetConfig().ANALYZE_MODE.Get() > 0) {
    cAvidaDriver_Base::main_driver = new cAvidaDriver_Analyze(world, (world->GetConfig().ANALYZE_MODE.Get() == 2));
  } else {
    cAvidaDriver_Base::main_driver = new cAvidaDriver_Population(world);
  }

  cout << endl;
  
  cAvidaDriver_Base::main_driver->Run();

  // Exit Nicely
  ExitAvida(0);
}
