/*
 *  systematics/Manager.cc
 *  avida-core
 *
 *  Created by David on 12/16/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
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

#include "avida/systematics/Manager.h"

#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Unit.h"


bool Avida::Systematics::Manager::RegisterArbiter(ArbiterPtr arbiter)
{
  for (int i = 0; i < m_arbiters.GetSize(); i++) {
    if (m_arbiters[i]->Role() == arbiter->Role()) return (m_arbiters[i] == arbiter);
  }
  
  m_arbiters.Push(arbiter);
  return true;
}

Avida::Systematics::ArbiterPtr Avida::Systematics::Manager::ArbiterForRole(const RoleID& role)
{
  for (int i = 0; i < m_arbiters.GetSize(); i++) if (m_arbiters[i]->Role() == role) return m_arbiters[i];
  
  return ArbiterPtr(NULL);
}

void Avida::Systematics::Manager::ClassifyNewUnit(UnitPtr u, const RoleClassificationHints* role_hints)
{
  for (int i = 0; i < m_arbiters.GetSize(); i++) {
    const ClassificationHints* hints = NULL;
    if (role_hints && role_hints->Has(m_arbiters[i]->Role())) hints = &(role_hints->Get(m_arbiters[i]->Role()));
    GroupPtr g = m_arbiters[i]->ClassifyNewUnit(u, hints);
    if (g) u->AddClassification(g);
  }
}
bool Avida::Systematics::Manager::AttachTo(World* world)
{
  WorldFacetPtr ptr(this);
  AddReference();  // explictly add reference, since this is internally creating a smart pointer to itself
  
  if (world->AttachFacet(Reserved::SystematicsFacetID, ptr)) {
    return true;
  }
  return false;
}


Avida::Systematics::ManagerPtr Avida::Systematics::Manager::Of(World* world)
{
  ManagerPtr manager;
  manager.DynamicCastFrom(world->Systematics());
  return manager;
}


bool Avida::Systematics::Manager::Serialize(ArchivePtr) const
{
  // @TODO
  return false;
}


Avida::WorldFacetID Avida::Systematics::Manager::UpdateBefore() const
{
  return Avida::Reserved::DataManagerFacetID;
}

Avida::WorldFacetID Avida::Systematics::Manager::UpdateAfter() const
{
  return "";
}


void Avida::Systematics::Manager::PerformUpdate(Context& ctx, Update current_update)
{
  for (int i = 0; i < m_arbiters.GetSize(); i++) m_arbiters[i]->PerformUpdate(ctx, current_update);
}
