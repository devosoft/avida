/*
 *  main.cc
 *  avida3
 *
 *  Created by David on 4/16/14.
 *  Copyright 2013-2014 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com
 *
 */

#include "apto/core/FileSystem.h"

#include "avida/core/Avida.h"
#include "avida/core/Universe.h"
#include "avida/core/Version.h"
#include "avida/data/Manager.h"
#include "avida/environment/Manager.h"
#include "avida/hardware/Manager.h"
#include "avida/output/Manager.h"
#include "avida/util/CmdLine.h"
#include "avida/world/Manager.h"

#include "avida/private/structure/types/TwoDimensionalGrid.h"

#include "Avida3Driver.h"


class StdIOFeedback : public Avida::Feedback
{
  void Error(const char* fmt, ...);
  void Warning(const char* fmt, ...);
  void Notify(const char* fmt, ...);
};


int main(int argc, char * argv[])
{
  StdIOFeedback feedback;
  
  Avida::Initialize();
  
  std::cout << Avida::Version::Banner() << std::endl;
  
  // Initialize the configuration data...
  Apto::Map<Apto::String, Apto::String> defs;
  cAvidaConfig* cfg = new cAvidaConfig();
  Avida::Util::ProcessCmdLineArgs(argc, argv, cfg, defs);
  
  // Create the Universe
  Avida::Universe* universe = new Avida::Universe();
  
  // Data::Manager
  Avida::Data::ManagerPtr(new Avida::Data::Manager)->AttachTo(universe);
  
  // Environment::Manager
  Avida::Environment::ManagerPtr(new Avida::Environment::Manager)->AttachTo(universe);
  
  // Output::Manager
  Apto::String opath = Apto::FileSystem::GetAbsolutePath(Apto::String(cfg->DATA_DIR.Get()), Apto::FileSystem::GetCWD());
  Avida::Output::ManagerPtr(new Avida::Output::Manager(opath))->AttachTo(universe);
  
  // Hardware::Manager
  Avida::Hardware::Manager::CreateFor(universe);
  
  // World::Manager
  Avida::Structure::Controller* structure = new Avida::Structure::Types::TwoDimensionalGrid();
  Avida::World::ManagerPtr world(Avida::World::Manager::CreateWithTopLevelStructure(universe, structure));
  
  (new Avida3Driver(universe, feedback))->Run();
  
  return 0;
}


void StdIOFeedback::Error(const char* fmt, ...)
{
  printf("error: ");
  va_list args;
  va_start(args, fmt);
  printf(fmt, args);
  va_end(args);
  printf("\n");
}

void StdIOFeedback::Warning(const char* fmt, ...)
{
  printf("warning: ");
  va_list args;
  va_start(args, fmt);
  printf(fmt, args);
  va_end(args);
  printf("\n");
}

void StdIOFeedback::Notify(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  printf(fmt, args);
  va_end(args);
  printf("\n");
}
