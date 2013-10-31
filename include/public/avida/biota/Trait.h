/*
 *  hardware/Feature.h
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

#ifndef AvidaBiotaTrait_h
#define AvidaBiotaTrait_h

#include "avida/Biota/Organism.h"


namespace Avida {
  namespace Biota {
    
    // Biota::Trait
    // --------------------------------------------------------------------------------------------------------------
    
    class Trait
    {
    protected:
      Organism* m_organism;
      
    public:
      LIB_EXPORT inline Trait(Organism* organism) : m_organism(organism) { ; }
      LIB_EXPORT virtual ~Trait() = 0;
      
    protected:
      static Trait* traitOf(int trait, Organism* organism) { return organism->m_traits[trait]; }
    };
    
  };
};

#endif
