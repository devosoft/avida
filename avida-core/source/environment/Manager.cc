/*
 *  environment/Manager.cc
 *  avida-core
 *
 *  Created by David on 6/24/11.
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

#include "avida/environment/Manager.h"

#include "avida/core/Archive.h"
#include "avida/environment/ActionTrigger.h"
#include "avida/environment/Reaction.h"
#include "avida/environment/Resource.h"


static Avida::UniverseFacetPtr DeserializeEnvironmentManager(Avida::ArchivePtr)
{
  // @TODO
  return Avida::UniverseFacetPtr();
}

bool Avida::Environment::Manager::s_registered_with_facet_factory =
  Avida::UniverseFacet::RegisterFacetType(Avida::Reserved::EnvironmentFacetID, DeserializeEnvironmentManager);



Avida::Environment::Manager::Manager()
{
  
}

Avida::Environment::Manager::~Manager()
{
  for (int i = 0; i < m_res_defs.GetSize(); i++) delete m_res_defs[i];
}


bool Avida::Environment::Manager::DefineActionTrigger(const ActionTriggerID& trigger_id, const Apto::String& desc, int tmp_order)
{
  if (m_action_triggers.Has(trigger_id)) return false;
  ActionTriggerPtr trigger(new ActionTrigger(trigger_id, desc, tmp_order));
  m_action_triggers[trigger_id] = trigger;
  if (m_action_trigger_ids) m_action_trigger_ids->Insert(trigger_id);
  return true;
}


Avida::Environment::ConstActionTriggerIDSetPtr Avida::Environment::Manager::GetActionTriggerIDs() const
{
  if (!m_action_trigger_ids) {
    ActionTriggerIDSetPtr action_trigger_ids(new ActionTriggerIDSet);
    for (Apto::Map<ActionTriggerID, ActionTriggerPtr>::KeyIterator it = m_action_triggers.Keys(); it.Next();) {
      action_trigger_ids->Insert(*it.Get());
    }
    m_action_trigger_ids = action_trigger_ids;
  }
  
  return m_action_trigger_ids;
}

Avida::Environment::ConstActionTriggerPtr Avida::Environment::Manager::GetActionTrigger(const ActionTriggerID& trigger_id) const
{
  ActionTriggerPtr trigger;
  m_action_triggers.Get(trigger_id, trigger);
  return trigger;
}




bool Avida::Environment::Manager::AttachTo(Universe* universe)
{
  UniverseFacetPtr ptr(this);
  AddReference();  // explictly add reference, since this is internally creating a smart pointer to itself
  
  if (universe->AttachFacet(Reserved::EnvironmentFacetID, ptr)) {
    return true;
  }
  return false;
}


Avida::Environment::ManagerPtr Avida::Environment::Manager::Of(Universe* universe)
{
  ManagerPtr manager;
  manager.DynamicCastFrom(universe->Environment());
  return manager;
}

bool Avida::Environment::Manager::Serialize(ArchivePtr) const
{
  // @TODO
  return false;
}


Avida::UniverseFacetID Avida::Environment::Manager::UpdateBefore() const
{
  return Reserved::DataManagerFacetID;
}

Avida::UniverseFacetID Avida::Environment::Manager::UpdateAfter() const
{
  return "";
}



