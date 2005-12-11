/*
 *  viewer.cc
 *  Avida
 *
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#include <signal.h>

#include "avida.h"
#include "cAvidaConfig.h"
#include "cAvidaDriver_Analyze.h"
#include "cAvidaDriver_TextPopViewer.h"
#include "cWorld.h"

using namespace std;

int main(int argc, char * argv[])
{
  // Catch Interrupt making sure to close appropriately
  signal(SIGINT, ExitAvida);

  printVersionBanner();
  
  // Initialize the configuration data...
  cWorld* world = new cWorld(cAvidaConfig::LoadWithCmdLineArgs(argc, argv));
  
  if (world->GetConfig().ANALYZE_MODE.Get() > 0) {
    cAvidaDriver_Base::main_driver = new cAvidaDriver_Analyze(world, (world->GetConfig().ANALYZE_MODE.Get() == 2));
  } else {
    cAvidaDriver_Base::main_driver = new cAvidaDriver_TextPopViewer(world);
  }
  
  cout << endl;
  
  cAvidaDriver_Base::main_driver->Run();

  // Exit Nicely
  ExitAvida(0);
}
