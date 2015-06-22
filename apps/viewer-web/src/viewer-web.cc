#include "apto/core/FileSystem.h"
#include "avida/Avida.h"
#include "avida/core/World.h"

#include "AvidaTools.h"

#include "cAvidaConfig.h"
#include "cUserFeedback.h"
#include "cWorld.h"
#include "cWebViewerDriver.h"
#include "UI/UI.h"


namespace UI = emp::UI;

extern "C" 
int main()
{
   emp::Alert("Starting.");
   Avida::Initialize();
   
   Apto::Map<Apto::String, Apto::String> defs;
   cAvidaConfig* cfg = new cAvidaConfig();
   cUserFeedback feedback;

   World* new_world = new World;


   emp::Alert("About to initialize the world.");
   cWorld* world = cWorld::Initialize(cfg, "/", new_world, &feedback, &defs);


   //Any errors produced by trying to initialize the world
   //will be made available to the WebViewerDriver for display
   //We're also going to share the same feedback object. 
   emp::Alert("About to create the web driver.");
   cWebViewerDriver* driver = new cWebViewerDriver(world,feedback);

   //Not supporting analyze mode at the moment
   //if (world->GetConfig().ANALYZE_MODE.Get() > 0)
   //   return 0; //Silently fail
   
   emp::Alert("About to run the driver.");
   driver->Run();

   return 0;
}

