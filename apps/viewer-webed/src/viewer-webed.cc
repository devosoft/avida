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
   const cString init_msg = "Avida is globally initialized. (version of 2021-03-03a)\nAdded 'repro_' pop stats for total pop only. # of offspring in grid data.";
   Avida::Initialize();
   
   //Initialize WebDriverActions separately to keep it out of avida-core library
   cActionLibrary* act_lib = &cActionLibrary::GetInstance();
   Actions::RegisterWebDriverActions(act_lib);   
   D_(D_STATUS, init_msg);
   
   cerr << init_msg << std::endl;
   
   RuntimeLoop(argc, argv);
}
