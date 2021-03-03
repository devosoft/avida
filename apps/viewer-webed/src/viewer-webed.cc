#include "apto/core/FileSystem.h"

#include "avida/Avida.h"
#include "avida/core/World.h"
#include "avida/util/CmdLine.h"

#include "AvidaTools.h"
#include "cAvidaConfig.h"
#include "cUserFeedback.h"
#include "cWorld.h"
#include "cActionLibrary.h"

#include <cstdlib> 


//WebViewer specific headers
#include "Runtime.h"
#include "WebDriverActions.h"
#include <emscripten.h>


using namespace Avida::WebViewer;

 
extern "C" 
int main(int argc, char* argv[])
{
  
   Avida::Initialize();
   
   //Initialize WebDriverActions separately to keep it out of avida-core library
   cActionLibrary* act_lib = &cActionLibrary::GetInstance();
   Actions::RegisterWebDriverActions(act_lib);   
   D_(D_STATUS, "Avida is globally initialized. (version of 2021-01-25)");
   
   RuntimeLoop(argc, argv);
}
