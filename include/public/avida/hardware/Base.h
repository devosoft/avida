/*
 *  hardware/Base.h
 *  avida-core
 *
 *  Created by David on 1/31/13.
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

#ifndef AvidaHardwareBase_h
#define AvidaHardwareBase_h

#include "avida/hardware/Types.h"

#include "avida/biota/Organism.h"
#include "avida/hardware/Feature.h"
#include "avida/structure/Element.h"


namespace Avida {
  namespace Hardware {
    
    // Hardware::Base
    // --------------------------------------------------------------------------------------------------------------
    
    class Base : public Structure::Element
    {
    protected:
      Biota::OrganismPtr m_owner;
      Apto::Array<Biota::OrganismPtr> m_organisms;
      Apto::Array<Feature*> m_features;
      
    public:
      LIB_EXPORT Base(Context& ctx, ConfigPtr cfg, Biota::OrganismPtr owner);
      LIB_EXPORT virtual ~Base() = 0;
      
      LIB_EXPORT virtual bool ProcessCycleStep(Context& ctx, Update current_update, bool speculative) = 0;
    };
    
  };
};

#endif
