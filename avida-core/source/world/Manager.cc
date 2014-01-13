/*
 *  world/Manager.cc
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

#include "avida/world/Manager.h"

#include "avida/world/Container.h"


Avida::World::Manager::Manager(Universe* universe, Structure::Controller* structure) : m_universe(universe)
{
  m_top_level = ContainerPtr(new Container(m_universe, structure));
}

Avida::World::Manager::~Manager()
{

}


Avida::World::ManagerPtr Avida::World::Manager::CreateWithTopLevelStructure(Universe* universe, Structure::Controller* structure)
{
  ManagerPtr mgr(new Manager(universe, structure));
  
  if (universe->AttachFacet(Reserved::WorldManagerFacetID, mgr)) return mgr;

  return ManagerPtr(NULL);
}


Avida::World::ManagerPtr Avida::World::Manager::Of(Universe* universe)
{
  ManagerPtr manager;
  manager.DynamicCastFrom(universe->WorldManager());
  return manager;
}


bool Avida::World::Manager::Serialize(ArchivePtr) const
{
  // @TODO
  return false;
}


Avida::UniverseFacetID Avida::World::Manager::UpdateBefore() const
{
  return Reserved::EnvironmentFacetID;
}

Avida::UniverseFacetID Avida::World::Manager::UpdateAfter() const
{
  return "";
}
