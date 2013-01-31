/*
 *  environment/ResourceManager.h
 *  avida-core
 *
 *  Created by David on 1/29/13.
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

#ifndef AvidaEnvironmentResourceManager_h
#define AvidaEnvironmentResourceManager_h

#include "avida/environment/Types.h"


namespace Avida {
  namespace Environment {
    
    // Environment::ResourceManager - Container for Resource instances (created by Environment::Manager)
    // --------------------------------------------------------------------------------------------------------------
    
    class ResourceManager
    {
      friend class Manager;
    private:
      Apto::Array<Resource*> m_resources;
      Apto::Array<ResourceListener*> m_listeners;
      
      LIB_LOCAL inline ResourceManager(int num_resources) : m_resources(num_resources) { ; }

    public:
      LIB_EXPORT ~ResourceManager();
      
      LIB_EXPORT inline Resource& GetResource(ResourceID res_id) { return *m_resources[res_id]; }

      LIB_EXPORT void AttachListener(ResourceListener* listener);
      LIB_EXPORT void DetachListener(ResourceListener* listener);
      
      LIB_EXPORT void PerformUpdate(Avida::Context& ctx, Update current_update);
    };
    
  };
};

#endif
