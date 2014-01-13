/*
 *  primitive.cc
 *  Avida
 *
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#include "avida/Avida.h"

/* By default, Boost is not available.  To enable Boost, either modify your environment,
 alter your build settings, or change this value -- BUT BE CAREFUL NOT TO CHECK IT IN LIKE THAT!
 */
#ifndef BOOST_IS_AVAILABLE
#define BOOST_IS_AVAILABLE 0
#endif

#if BOOST_IS_AVAILABLE
#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "cAvidaConfig.h"
#include "AvidaTools.h"
#include "cDefaultAnalyzeDriver.h"
#include "cDefaultRunDriver.h"
#include "cMultiProcessWorld.h"
#include "Platform.h"

using namespace std;

#include <iostream>

int main(int argc, char * argv[])
{
  Avida::Initialize();
  
  Avida::PrintVersionBanner();

  // Initialize the configuration data...
  cAvidaConfig* cfg = new cAvidaConfig();
  Avida::ProcessCmdLineArgs(argc, argv, cfg);

	boost::mpi::environment mpi_env; //!< MPI environment.
	boost::mpi::communicator mpi_world; //!< World-wide MPI communicator.
	
	cfg->RANDOM_SEED.Set(mpi_world.rank() + cfg->RANDOM_SEED.Get());
	cout << "Random seed overwritten for Avida-MP: " << cfg->RANDOM_SEED.Get() << endl;
	
	ostringstream dirname;
	dirname << cfg->DATA_DIR.Get() << "_" << mpi_world.rank();	
	cfg->DATA_DIR.Set(dirname.str().c_str());
	cout << "Data directory overwritten for Avida-MP: " << cfg->DATA_DIR.Get() << endl;
  
  cWorld* world = cMultiProcessWorld::Initialize(cfg, AvidaTools::FileSystem::GetCWD(), mpi_env, mpi_world);

  cout << endl;
  

  if (world->GetConfig().ANALYZE_MODE.Get() > 0) {
    (new cDefaultAnalyzeDriver(world, (world->GetConfig().ANALYZE_MODE.Get() == 2)))->Run();
  } else {
    (new cDefaultRunDriver(world))->Run();
  }

  return 0;
}

#else
#error Avida-MP can only be built with the Boost C++ libraries available (specifically, boost +mpi).
#endif
