/*
 *  environment/Manager.h
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

#ifndef AvidaEnvironmentManager_h
#define AvidaEnvironmentManager_h

#include "apto/platform.h"
#include "avida/core/World.h"
#include "avida/environment/types.h"


namespace Avida {
  namespace Environment {
    
    // Environment::Manager - holds the top level definition of the environment
    // --------------------------------------------------------------------------------------------------------------
    
    class Manager : public WorldFacet
    {
    private:
      Apto::RWLock m_rwlock;
      
      Apto::Map<ActionTriggerID, ActionTriggerPtr> m_action_triggers;
      Apto::Map<ReactionID, ReactionPtr> m_reactions;
      Apto::Map<ResourceID, ResourcePtr> m_resources;
      
      mutable ActionTriggerIDSetPtr m_action_trigger_ids;
      mutable ReactionIDSetPtr m_reaction_ids;
      mutable ResourceIDSetPtr m_resource_ids;
      
    public:
      LIB_EXPORT Manager();
      
      LIB_EXPORT bool DefineActionTrigger(const ActionTriggerID& trigger_id, const Apto::String& desc, ConstProductPtr product);
      LIB_EXPORT bool Register(ReactionPtr);
      LIB_EXPORT bool Register(ResourcePtr);

      LIB_EXPORT ConstActionTriggerIDSetPtr GetActionTriggerIDs() const;
      LIB_EXPORT ConstActionTriggerPtr GetActionTrigger(const ActionTriggerID& trigger_id) const;
      
      LIB_EXPORT ConstReactionIDSetPtr GetReactionIDs() const;
      LIB_EXPORT ConstReactionPtr GetReaction(const ReactionID& reaction_id) const;
      
      LIB_EXPORT ConstResourceIDSetPtr GetResourceIDs() const;
      LIB_EXPORT ConstResourcePtr GetResource(const ResourceID& resource_id) const;
      
      
      LIB_EXPORT static ManagerPtr Of(World* world);
      
    public:
      LIB_LOCAL void PerformUpdate(Avida::Context& ctx, Update current_update);
    };
    
  };
};

#endif
