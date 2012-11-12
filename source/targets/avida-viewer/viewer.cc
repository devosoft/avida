/*
 *  viewer.cc
 *  Avida
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <csignal>

#include "apto/core/FileSystem.h"
#include "avida/Avida.h"
#include "avida/core/World.h"
#include "avida/util/CmdLine.h"

#include "AvidaTools.h"

#include "cAvidaConfig.h"
#include "cTextViewerAnalyzeDriver.h"
#include "cTextViewerDriver.h"
#include "cUserFeedback.h"
#include "cWorld.h"

using namespace std;


int main(int argc, char * argv[])
{
  Avida::Initialize();
  
  cout << Avida::Version::Banner() << endl;
  
  // Initialize the configuration data...
  Apto::Map<Apto::String, Apto::String> defs;
  cAvidaConfig* cfg = new cAvidaConfig();
  Avida::Util::ProcessCmdLineArgs(argc, argv, cfg, defs);
  
  cUserFeedback feedback;
  World* new_world = new World;
  cWorld* world = cWorld::Initialize(cfg, cString(Apto::FileSystem::GetCWD()), new_world, &feedback, &defs);
  
  for (int i = 0; i < feedback.GetNumMessages(); i++) {
    switch (feedback.GetMessageType(i)) {
      case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
      case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
      default: break;
    };
    cerr << feedback.GetMessage(i) << endl;
  }
  
  if (!world) return -1;
  
  cTextViewerDriver_Base* driver = NULL;
  if (world->GetConfig().ANALYZE_MODE.Get() > 0) {
    driver = new cTextViewerAnalyzeDriver(world, (world->GetConfig().ANALYZE_MODE.Get() == 2));
  } else {
    driver = new cTextViewerDriver(world);
  }

  driver->Run();
  
  return 0;
}
