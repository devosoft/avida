/*
 *  biota/Manager.cc
 *  avida-core
 *
 *  Created by David on 4/9/14.
 *  Copyright 2014 Michigan State University. All rights reserved.
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

#include "avida/biota/Manager.h"


Avida::Biota::Manager::Manager(Universe* universe) : m_universe(universe)
{
  ;
}

Avida::Biota::Manager::~Manager()
{
  
}


Avida::Biota::ManagerPtr Avida::Biota::Manager::CreateFor(Universe* universe)
{
  ManagerPtr mgr(new Manager(universe));
  
  if (universe->AttachFacet(Reserved::BiotaManagerFacetID, mgr)) return mgr;
  
  return ManagerPtr(NULL);
}


Avida::Biota::ManagerPtr Avida::Biota::Manager::Of(Universe* universe)
{
  ManagerPtr manager;
  manager.DynamicCastFrom(universe->HardwareManager());
  return manager;
}


bool Avida::Biota::Manager::Serialize(ArchivePtr) const
{
  // @TODO
  return false;
}


Avida::UniverseFacetID Avida::Biota::Manager::UpdateBefore() const
{
  return "";
}

Avida::UniverseFacetID Avida::Biota::Manager::UpdateAfter() const
{
  return "";
}
