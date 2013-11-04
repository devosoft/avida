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
#include "avida/core/Universe.h"
#include "avida/environment/ActionTrigger.h"


namespace Avida {
  namespace Environment {
    
    // Environment::Manager - holds the top level definition of the environment
    // --------------------------------------------------------------------------------------------------------------
    
    class Manager : public UniverseFacet
    {
    private:
      Apto::RWLock m_rwlock;
      
      Apto::Map<ActionTriggerID, ActionTriggerPtr> m_action_triggers;

      Apto::Array<ResourceDefinition*> m_res_defs;
      Apto::Map<Apto::String, int> m_res_name_map;
      
      mutable ActionTriggerIDSetPtr m_action_trigger_ids;
      
      static bool s_registered_with_facet_factory;
      
    public:
      LIB_EXPORT Manager();
      LIB_EXPORT ~Manager();
      
      LIB_EXPORT bool DefineActionTrigger(const ActionTriggerID& trigger_id, const Apto::String& desc, int tmp_order = -1);

      LIB_EXPORT ConstActionTriggerIDSetPtr GetActionTriggerIDs() const;
      LIB_EXPORT ConstActionTriggerPtr GetActionTrigger(const ActionTriggerID& trigger_id) const;
      
      LIB_EXPORT ResourceManager* CreateResourceManagerForStructure(Structure::Controller* structure);
      LIB_EXPORT ResourceManager* CreateLocalResourceManager();
      
      
      LIB_EXPORT bool AttachTo(Universe* universe);
      LIB_EXPORT static ManagerPtr Of(Universe* universe);
      
    public:
      LIB_EXPORT bool Serialize(ArchivePtr ar) const;
      
    public:
      LIB_LOCAL UniverseFacetID UpdateBefore() const;
      LIB_LOCAL UniverseFacetID UpdateAfter() const;
    };
    
  };
};

#endif
