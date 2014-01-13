/*
 *  viewer-text.cc
 *  Avida
 *
 *  Created by Charles on 7/1/07
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

#include "avida/Avida.h"
#include "AvidaTools.h"

#include "cAvidaConfig.h"
#include "cDriverManager.h"
#include "cUserFeedback.h"
#include "cWorld.h"

#include "cDriver_TextViewer.h"


using namespace std;


int main(int argc, char * argv[])
{
  Avida::Initialize();
  
  Avida::PrintVersionBanner();
  
  // Initialize the configuration data...
  cAvidaConfig* cfg = new cAvidaConfig();
  Avida::ProcessCmdLineArgs(argc, argv, cfg);
  
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
  
  // And run the driver!
  cDriver_TextViewer* driver = new cDriver_TextViewer(world);
  driver->Run();
  
  return 0;
}
