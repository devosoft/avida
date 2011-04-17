/*
 *  primitive.cc
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

#include "AvidaTools.h"

#include "avida/Avida.h"
#include "avida/util/CmdLine.h"

#include "cAvidaConfig.h"
#include "cDefaultAnalyzeDriver.h"
#include "cDefaultRunDriver.h"
#include "cUserFeedback.h"
#include "cWorld.h"


int main(int argc, char * argv[])
{

  Avida::Initialize();
  
  cout << Avida::Version::Banner() << endl;

  // Initialize the configuration data...
  cAvidaConfig* cfg = new cAvidaConfig();
  Avida::Util::ProcessCmdLineArgs(argc, argv, cfg);
  
  cUserFeedback feedback;
  cWorld* world = cWorld::Initialize(cfg, AvidaTools::FileSystem::GetCWD(), &feedback);

  for (int i = 0; i < feedback.GetNumMessages(); i++) {
    switch (feedback.GetMessageType(i)) {
      case cUserFeedback::ERROR:    cerr << "error: "; break;
      case cUserFeedback::WARNING:  cerr << "warning: "; break;
      default: break;
    };
    cerr << feedback.GetMessage(i) << endl;
  }

  if (!world) return -1;
  
  const int rand_seed = world->GetConfig().RANDOM_SEED.Get();
  cout << "Random Seed: " << rand_seed;
  if (rand_seed != world->GetRandom().GetSeed()) cout << " -> " << world->GetRandom().GetSeed();
  cout << endl;

  if (world->GetConfig().VERBOSITY.Get() > VERBOSE_NORMAL)
    cout << "Data Directory: " << world->GetDataFileManager().GetTargetDir() << endl;

  cout << endl;
  
  if (world->GetConfig().ANALYZE_MODE.Get() > 0) {
    (new cDefaultAnalyzeDriver(world, (world->GetConfig().ANALYZE_MODE.Get() == 2)))->Run();
  } else {
    (new cDefaultRunDriver(world))->Run();
  }
  
  return 0;
}
