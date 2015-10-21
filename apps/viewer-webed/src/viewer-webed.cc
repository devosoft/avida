#include "apto/core/FileSystem.h"
#include "avida/Avida.h"
#include "avida/core/World.h"

#include "AvidaTools.h"

#include "cAvidaConfig.h"
#include "cUserFeedback.h"
#include "cWorld.h"
#include "cActionLibrary.h"

#include "avida/util/CmdLine.h"

#include "Driver.h"
#include "Callbacks.h"
#include "WebDriverActions.h"

#include <emscripten.h>
#include <cstdlib> 

using namespace Avida::WebViewer;
extern Driver* driver;

Driver* driver = nullptr;

 
extern "C" 
int main(int argc, char* argv[])
{
  
   Avida::Initialize();
   
   //Initialize WebDriverActions separately to keep it out of avida-core library
   cActionLibrary* act_lib = &cActionLibrary::GetInstance();
   RegisterWebDriverActions(act_lib);   
   D_(D_STATUS, "Avida is globally initialized.");
   
   cAvidaConfig* cfg = new cAvidaConfig();
   cUserFeedback feedback;

   Apto::Map<Apto::String, Apto::String> defs;
   Avida::Util::ProcessCmdLineArgs(argc, argv, cfg, defs);

   D_(D_STATUS, "Avida is now configured.");

   World* new_world = new World;
   cWorld* world = cWorld::Initialize(cfg, "/", new_world, &feedback, &defs);
   D_(D_STATUS, "The world is located at " << world);
   
   driver = new Driver(world, feedback);
   D_(D_STATUS, "The driver is located at " << &driver);

   //atexit(AvidaExit);
   
   RunDriver();
   
   //emscripten_force_exit(0);
}


