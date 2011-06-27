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

#include "avida/environment/ActionTrigger.h"
#include "avida/environment/Reaction.h"
#include "avida/environment/Resource.h"



Avida::Environment::Manager::Manager()
{
  
}

bool Avida::Environment::Manager::Register(ActionTriggerPtr trigger)
{
  if (m_action_triggers.Has(trigger->GetID())) return false;
  m_action_triggers[trigger->GetID()] = trigger;
  return true;
}

bool Avida::Environment::Manager::Register(ReactionPtr reaction)
{
  if (m_reactions.Has(reaction->GetID())) return false;
  m_reactions[reaction->GetID()] = reaction;
  return true;
}

bool Avida::Environment::Manager::Register(ResourcePtr resource)
{
  if (m_resources.Has(resource->GetID())) return false;
  m_resources[resource->GetID()] = resource;
  return true;
}


Avida::Environment::ConstActionTriggerIDSetPtr Avida::Environment::Manager::GetActionTriggerIDs() const
{
  if (!m_action_trigger_ids) {
    
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
    
  }
  
  return m_resource_ids;
}

Avida::Environment::ConstResourcePtr Avida::Environment::Manager::GetResource(const ResourceID& resource_id) const
{
  ResourcePtr resource;
  m_resources.Get(resource_id, resource);
  return resource;
}
