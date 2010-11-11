/*
 *  primitive.cc
 *  Avida
 *
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#include "Avida.h"

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
  cAvidaDriver* driver = NULL;

  if (world->GetConfig().ANALYZE_MODE.Get() > 0) {
    driver = new cDefaultAnalyzeDriver(world, (world->GetConfig().ANALYZE_MODE.Get() == 2));
  } else {
    driver = new cDefaultRunDriver(world);
  }

  cout << endl;
  
  driver->Run();

  return 0;
}

#else
#error Avida-MP can only be built with the Boost C++ libraries available (specifically, boost +mpi).
#endif
