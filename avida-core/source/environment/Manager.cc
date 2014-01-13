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
#include "avida/environment/Product.h"
#include "avida/environment/Reaction.h"
#include "avida/environment/Resource.h"


static Avida::WorldFacetPtr DeserializeEnvironmentManager(Avida::ArchivePtr)
{
  // @TODO
  return Avida::WorldFacetPtr();
}

bool Avida::Environment::Manager::s_registered_with_facet_factory =
  Avida::WorldFacet::RegisterFacetType(Avida::Reserved::EnvironmentFacetID, DeserializeEnvironmentManager);



Avida::Environment::Manager::Manager()
{
  
}

bool Avida::Environment::Manager::DefineActionTrigger(const ActionTriggerID& trigger_id, const Apto::String& desc,
                                                      ConstProductPtr product, int tmp_order)
{
  if (m_action_triggers.Has(trigger_id)) return false;
  ActionTriggerPtr trigger(new ActionTrigger(trigger_id, desc, product, tmp_order));
  m_action_triggers[trigger_id] = trigger;
  if (m_action_trigger_ids) m_action_trigger_ids->Insert(trigger_id);
  return true;
}

bool Avida::Environment::Manager::Register(ReactionPtr reaction)
{
  if (m_reactions.Has(reaction->GetID())) return false;
  m_reactions[reaction->GetID()] = reaction;
  if (m_reaction_ids) m_reaction_ids->Insert(reaction->GetID());
  return true;
}

bool Avida::Environment::Manager::Register(ResourcePtr resource)
{
  if (m_resources.Has(resource->GetID())) return false;
  m_resources[resource->GetID()] = resource;
  if (m_resource_ids) m_resource_ids->Insert(resource->GetID());
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


Avida::Environment::ConstReactionIDSetPtr Avida::Environment::Manager::GetReactionIDs() const
{
  if (!m_reaction_ids) {
    ReactionIDSetPtr reaction_ids(new ReactionIDSet);
    for (Apto::Map<ReactionID, ReactionPtr>::KeyIterator it = m_reactions.Keys(); it.Next();) {
      reaction_ids->Insert(*it.Get());
    }
    m_reaction_ids = reaction_ids;
  }
  
  return m_reaction_ids;
}

Avida::Environment::ConstReactionPtr Avida::Environment::Manager::GetReaction(const ReactionID& reaction_id) const
{
  ReactionPtr reaction;
  m_reactions.Get(reaction_id, reaction);
  return reaction;
}


Avida::Environment::ConstResourceIDSetPtr Avida::Environment::Manager::GetResourceIDs() const
{
  if (!m_resource_ids) {
    ResourceIDSetPtr resource_ids(new ResourceIDSet);
    for (Apto::Map<ResourceID, ResourcePtr>::KeyIterator it = m_resources.Keys(); it.Next();) {
      resource_ids->Insert(*it.Get());
    }
    m_resource_ids = resource_ids;
  }
  
  return m_resource_ids;
}

Avida::Environment::ConstResourcePtr Avida::Environment::Manager::GetResource(const ResourceID& resource_id) const
{
  ResourcePtr resource;
  m_resources.Get(resource_id, resource);
  return resource;
}

bool Avida::Environment::Manager::AttachTo(World* world)
{
  WorldFacetPtr ptr(this);
  AddReference();  // explictly add reference, since this is internally creating a smart pointer to itself
  
  if (world->AttachFacet(Reserved::EnvironmentFacetID, ptr)) {
    return true;
  }
  return false;
}


Avida::Environment::ManagerPtr Avida::Environment::Manager::Of(World* world)
{
  ManagerPtr manager;
  manager.DynamicCastFrom(world->Environment());
  return manager;
}

bool Avida::Environment::Manager::Serialize(ArchivePtr) const
{
  // @TODO
  return false;
}


Avida::WorldFacetID Avida::Environment::Manager::UpdateBefore() const
{
  return Reserved::DataManagerFacetID;
}

Avida::WorldFacetID Avida::Environment::Manager::UpdateAfter() const
{
  return "";
}



