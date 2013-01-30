/*
 *  private/environment/resources/Global.h
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

#ifndef AvidaEnvironmentResourcesGlobal_h
#define AvidaEnvironmentResourcesGlobal_h

#include "avida/environment/Resource.h"


namespace Avida {
  namespace Environment {
    namespace Resources {
      
      class Global : public Resource
      {
      private:
        ResourceQuantity m_quantity;
        
      public:
        LIB_EXPORT ~Global();
        
        LIB_EXPORT static void Initialize();
        
        
        LIB_EXPORT ResourceQuantity AmountAt(const Structure::Coord& location, Update current_update);
        
        LIB_EXPORT void PerformUpdate(Avida::Context& ctx, Update current_update);
        
        
        
        
      private:
        LIB_LOCAL Global(ResourceDefinition& def);
        
        LIB_LOCAL static Global* Create(ResourceDefinition& def, Structure::Controller& structure);
      };
      
    };
  };
};

#endif
