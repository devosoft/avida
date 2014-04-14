/*
 *  world/OrganismDataProvider.h
 *  avida-core
 *
 *  Created by David on 4/11/14.
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

#ifndef AvidaWorldOrganismDataProvider_h
#define AvidaWorldOrganismDataProvider_h

#include "avida/biota/Types.h"
#include "avida/data/Provider.h"
#include "avida/world/Types.h"


namespace Avida {
  namespace World {
    
    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    class OrganismDataProvider: public Data::ArgumentedProvider
    {
    public:
      ~OrganismDataProvider() = 0;
      
      virtual void UpdateReset() = 0;
      virtual void HandleOrganism(Biota::Organism* org) = 0;
    };
    
    
  };
};


#endif
