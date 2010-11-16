/*
 *  viewer.cc
 *  Avida
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <csignal>

#include "Avida.h"
#include "AvidaTools.h"

#include "cAvidaConfig.h"
#include "cTextViewerAnalyzeDriver.h"
#include "cDriverManager.h"
#include "cTextViewerDriver.h"
#include "cUserFeedback.h"
#include "cWorld.h"

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
  
  cAvidaDriver* driver = NULL;
  if (world->GetConfig().ANALYZE_MODE.Get() > 0) {
    driver = new cTextViewerAnalyzeDriver(world, (world->GetConfig().ANALYZE_MODE.Get() == 2));
  } else {
    driver = new cTextViewerDriver(world);
  }

  driver->Run();
  
  return 0;
}
