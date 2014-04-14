/*
 *  biota/Types.h
 *  avida-core
 *
 *  Created by David on 1/25/13.
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

#ifndef AvidaBiotaTypes_h
#define AvidaBiotaTypes_h

#include "avida/core/Types.h"


namespace Avida {
  namespace Biota {
    
    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    class ActionHandler;
    class EventListener;
    class Manager;
    class Organism;
    class PlacementStrategy;
    class Trait;
    
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::SmartPtr<Manager, Apto::InternalRCObject> ManagerPtr;
    typedef Apto::SmartPtr<Organism, Apto::InternalRCObject> OrganismPtr;
    typedef int OrganismEvent;
    typedef Apto::String PlacementStrategyKey;
    typedef Apto::SmartPtr<PlacementStrategy, Apto::InternalRCObject> PlacementStrategyPtr;
    
    
    extern const OrganismEvent OrganismDeath;
    extern const OrganismEvent OrganismReproduction;
    
  };
};

#endif
