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
#include "cDefaultAnalyzeDriver.h"
#include "cDriverManager.h"
#include "cTextViewerDriver.h"
#include "cWorld.h"

using namespace std;

int main(int argc, char * argv[])
{
  // Catch Interrupt making sure to close appropriately
  signal(SIGINT, ExitAvida);

  printVersionBanner();
  
  cDriverManager::Initialize();
  
  // Initialize the configuration data...
  cWorld* world = new cWorld(cAvidaConfig::LoadWithCmdLineArgs(argc, argv));
  cAvidaDriver* driver = NULL;
  
  if (world->GetConfig().ANALYZE_MODE.Get() > 0) {
    driver = new cDefaultAnalyzeDriver(world, (world->GetConfig().ANALYZE_MODE.Get() == 2));
  } else {
    driver = new cTextViewerDriver(world);
  }
  
  cout << endl;
  
  driver->Run();
  
  // Exit Nicely
  ExitAvida(0);
}
