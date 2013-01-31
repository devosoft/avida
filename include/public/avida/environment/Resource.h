/*
 *  environment/Resource.h
 *  avida-core
 *
 *  Created by David on 6/24/11.
 *  Copyright 2011-2013 Michigan State University. All rights reserved.
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

#ifndef AvidaEnvironmentResource_h
#define AvidaEnvironmentResource_h

#include "avida/environment/ResourceDefinition.h"
#include "avida/structure/Types.h"


namespace Avida {
  namespace Environment {
    
    // Environment::Resource
    // --------------------------------------------------------------------------------------------------------------
    
    class Resource
    {
    protected:
      ResourceDefinition& m_def;
      
    public:
      // Base Methods
      
      LIB_EXPORT inline Resource(ResourceDefinition& def) : m_def(def) { ; }
      LIB_EXPORT virtual ~Resource() = 0;
      
      LIB_EXPORT inline const ResourceID& GetID() const { return m_def.GetID(); }
      LIB_EXPORT inline const Apto::String& Name() const { return m_def.Name(); }
      

      // Interface Methods
      
      LIB_EXPORT virtual ResourceQuantity AmountAt(const Structure::Coord& location, Update current_update) = 0;
      
      LIB_EXPORT virtual void PerformUpdate(Avida::Context& ctx, Update current_update) = 0;
    };
    
  };
};

#endif
