/*
 *  biota/Organism.h
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

#ifndef AvidaBiotaOrganism_h
#define AvidaBiotaOrganism_h

#include "avida/biota/Types.h"
#include "avida/core/Genome.h"
#include "avida/systematics/Unit.h"


namespace Avida {
  namespace Biota {
    
    // Internal Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    namespace Internal {
      class OrgPropertyMap;
      class OrgPropRetrievalContainer;
      template <class T> class OrgPorpOfType;
    };
    

    // Organism
    // --------------------------------------------------------------------------------------------------------------
    
    class Organism : public Systematics::Unit
    {
      friend class Trait;
      friend class OrgPropRetrievalContainer;
      template <class T> friend class OrgPropOfType;
      
    private:
      Universe* universe;
      
      Apto::Array<Trait*> m_traits;

      Systematics::Source m_src;
      const Genome m_initial_genome;
      Genome m_offspring_genome;
      Internal::OrgPropertyMap* m_prop_map;
      
    public:
      LIB_EXPORT Organism(Universe* universe, const Genome& genome, Systematics::Source src, int parent_generation);
      LIB_EXPORT ~Organism();
      
      LIB_LOCAL static void Initialize();
      
      // Systematics::Unit 
      LIB_EXPORT Systematics::Source UnitSource() const;
      LIB_EXPORT const Genome& UnitGenome() const;
      LIB_EXPORT const PropertyMap& Properties() const;

    private:
      LIB_LOCAL Apto::String getGenomeString();
      LIB_LOCAL int getSrcTransmissionType();
      LIB_LOCAL int getAge();
      LIB_LOCAL int getGeneration();
      LIB_LOCAL int getLastCopied();
      LIB_LOCAL int getLastExecuted();
      LIB_LOCAL int getLastGestation();
      LIB_LOCAL double getLastMetabolicRate();
      LIB_LOCAL double getLastFitness();
    };
    
  };
};

#endif
