/*
 *  world/Container.h
 *  avida-core
 *
 *  Created by David on 1/24/13.
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

#ifndef AvidaWorldContainer_h
#define AvidaWorldContainer_h

#include "avida/biota/Types.h"
#include "avida/environment/Types.h"
#include "avida/hardware/Types.h"
#include "avida/structure/Element.h"
#include "avida/world/Types.h"


namespace Avida {
  namespace World {
    
    // Container
    // --------------------------------------------------------------------------------------------------------------
    
    class Container : public virtual Apto::RefCountObject<Apto::ThreadSafe>
    {
      friend class Manager;
    protected:
      Universe* m_universe;
      
      Apto::Array<Biota::OrganismPtr, Apto::Smart> m_organisms;
      Apto::Array<Hardware::InstancePtr, Apto::Smart> m_hardware;
      Structure::Controller* m_structure;
      Environment::ResourceManager* m_resources;
      
    public:
      LIB_EXPORT ~Container();

      
    protected:
      LIB_EXPORT Container(Universe* universe, Structure::Controller* structure);
    };
    
  };
};

#endif
