/*
 *  world/Container.cc
 *  avida-core
 *
 *  Created by David on 1/24/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/world/Container.h"

#include "avida/biota/PlacementStrategy.h"
#include "avida/data/Package.h"
#include "avida/data/Util.h"
#include "avida/environment/Manager.h"
#include "avida/environment/ResourceManager.h"
#include "avida/structure/Controller.h"
#include "avida/systematics/Group.h"


Avida::World::Container::Container(Universe* universe, Structure::Controller* structure)
: m_universe(universe), m_structure(structure)
{
  Environment::ManagerPtr env(Environment::Manager::Of(universe));
  assert(env);
  
  m_resources = env->CreateResourceManagerForStructure(structure);
  
}


Avida::World::Container::~Container()
{
  delete m_structure;
  delete m_resources;
}


void Avida::World::Container::PerformUpdate(Context& ctx, Update current_update)
{
  
}




void Avida::World::Container::recordBirth(bool breed_true)
{
  m_stat_tot_organisms++;
  m_stat_num_births++;
  m_stat_tot_births++;
  
  if (breed_true) m_stat_num_breed_true++;
  else m_stat_num_breed_in++;
}

