#include "apto/core/FileSystem.h"
#include "avida/Avida.h"
#include "avida/core/World.h"

#include "AvidaTools.h"

#include "cAvidaConfig.h"
#include "cUserFeedback.h"
#include "cWorld.h"
#include "avida/util/CmdLine.h"

#include "Driver.h"
#include "Callbacks.h"
#include <emscripten.h>

using namespace Avida::WebViewer;
extern Driver* driver;

Driver* driver = nullptr;

 
extern "C" 
int main(int argc, char* argv[])
{
  
   Avida::Initialize();
   
   Apto::Map<Apto::String, Apto::String> defs;
   cAvidaConfig* cfg = new cAvidaConfig();
   cUserFeedback feedback;

   Avida::Util::ProcessCmdLineArgs(argc, argv, cfg, defs);

   World* new_world = new World;

   cWorld* world = cWorld::Initialize(cfg, "/", new_world, &feedback, &defs);

   driver = new Driver(world, feedback);
   driver->Run();
}


