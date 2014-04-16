/*
 *  core/Genome.h
 *  avida-core
 *
 *  Created by David Bryson on 3/29/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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

#ifndef AvidaCoreGenome_h
#define AvidaCoreGenome_h

#include "apto/platform.h"
#include "avida/core/GeneticRepresentation.h"
#include "avida/core/Properties.h"

#include <typeinfo>


namespace Avida {
  
  // EpigeneticObject
  // --------------------------------------------------------------------------------------------------------------
  
  class EpigeneticObject
  {
    friend class Genome;
  public:
    LIB_EXPORT virtual ~EpigeneticObject() = 0;
    
    LIB_EXPORT virtual EpigeneticObject* Clone() = 0;
    
    LIB_EXPORT virtual bool Serialize(ArchivePtr ar) const = 0;

  private:
    LIB_EXPORT virtual void AttachTo(Genome& genome) = 0;
  };
  
  
  // Genome - genetic and epi-genetic heritable information
  // --------------------------------------------------------------------------------------------------------------
  
  class Genome
  {
  private:
    HardwareConfigID m_hw_config;
    BiotaTraitSet m_traits;
    GeneticRepresentationPtr m_representation;
    Apto::Map<Apto::String, EpigeneticObject*> m_epigenetic_objs;
    
  public:
    LIB_EXPORT Genome();
    LIB_EXPORT Genome(HardwareConfigID hw, const BiotaTraitSet& traits, const PropertyMap& props, GeneticRepresentationPtr rep);
    LIB_EXPORT explicit Genome(const Apto::String& genome_str);
    LIB_EXPORT Genome(const Genome& genome);
    
    
    // Accessors
    LIB_EXPORT inline HardwareConfigID HardwareConfig() const { return m_hw_config; }
    LIB_EXPORT inline const BiotaTraitSet& Traits() const { return m_traits; }
    
    LIB_EXPORT inline GeneticRepresentationPtr Representation() { return m_representation; }
    LIB_EXPORT inline ConstGeneticRepresentationPtr Representation() const { return const_cast<GeneticRepresentationPtr&>(m_representation); }
    
    
    // Epigenetic Objects
    template <typename T> bool AttachEpigeneticObject(T obj)
    {
      if (m_epigenetic_objs.Has(T::ObjectKey)) return false;
      m_epigenetic_objs.Set(T::ObjectKey, obj);
      obj->AttachTo(*this);
      return true;
    }
    
    template <typename T> T* GetEpigeneticObject()
    {
      return dynamic_cast<T>(m_epigenetic_objs.Get(T::ObjectKey));
    }
    
    
    // Conversion
    LIB_EXPORT Apto::String AsString() const;
    

    // Operations
    LIB_EXPORT bool operator==(const Genome& genome) const;
    LIB_EXPORT Genome& operator=(const Genome& genome);

    LIB_EXPORT bool Serialize(ArchivePtr ar) const;
    LIB_EXPORT static GenomePtr Deserialize(ArchivePtr ar);
    LIB_EXPORT bool LegacySave(void* df) const;
  };
};

#endif
