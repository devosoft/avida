#include "apto/core/FileSystem.h"
#include "avida/Avida.h"
#include "avida/core/World.h"

#include "AvidaTools.h"

#include "cAvidaConfig.h"
#include "cUserFeedback.h"
#include "cWorld.h"
#include "cWebViewerDriver.h"
#include "avida/util/CmdLine.h"
#include "UI/UI.h"

namespace UI = emp::UI;

extern "C"
void StepDriver(){
   cWebViewerDriver::StepUpdate();
}


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


   //Any errors produced by trying to initialize the world
   //will be made available to the WebViewerDriver for display
   //We're also going to share the same feedback object. 
   cWebViewerDriver* driver = new cWebViewerDriver(world,feedback);
   if (!driver->Ready()){
      emp::Alert("The driver is not ready to run.  Abort.");
      return 1;
   }

   //Not supporting analyze mode at the moment
   //if (world->GetConfig().ANALYZE_MODE.Get() > 0)
   //   return 0; //Silently fail
   
   emp::Alert("About to run the driver.");
   
   //We're going to transfer control to the emscripten
   //event looper and let the browser set the execution
   //speed to avoid starving the rednering engine
   emscripten_set_main_loop(StepDriver, 2, 0);

   return 0;
}

