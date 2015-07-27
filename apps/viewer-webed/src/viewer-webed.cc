#include "apto/core/FileSystem.h"
#include "avida/Avida.h"
#include "avida/core/World.h"

#include "AvidaTools.h"

#include "cAvidaConfig.h"
#include "cUserFeedback.h"
#include "cWorld.h"
#include "Driver.h"
#include "avida/util/CmdLine.h"

static Avida::WebViewer::Driver* driver;

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

   Avida::WebViewer::Driver* driver = 
      new Avida::WebViewer::Driver(world, feedback);
   
   return 0;
}

extern "C"
void PlayPause()
{
   driver->PlayPause();
}

extern "C"
void Finish()
{
   driver->Finish();
}

extern "C"
void GetPopulationData()
{
   driver->GetPopulationData();
}
