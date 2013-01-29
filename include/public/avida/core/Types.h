/*
 *  core/Types.h
 *  avida-core
 *
 *  Created by David on 5/20/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
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

#ifndef AvidaCoreTypes_h
#define AvidaCoreTypes_h

#include "apto/core.h"
#include "apto/malloc.h"
#include "avida/core/Definitions.h"


namespace Avida {
  
  // Class Declarations
  // --------------------------------------------------------------------------------------------------------------
  
  class Archive;
  class Context;
  class Feedback;
  class GeneticRepresentation;
  class Genome;
  class GlobalObject;
  class Instruction;
  class InstructionSequence;
  class Property;
  class PropertyMap;
  template <typename T> struct PropertyTraits;
  template <typename T> class ReferenceProperty;
  class StringProperty;
  class Universe;
  class UniverseDriver;
  class UniverseFacet;
  struct Update;

  
  // Enumeration Declarations
  // --------------------------------------------------------------------------------------------------------------  
  
  enum AbortCondition {
    INVALID_CONFIG = 2,
    IO_ERROR = 3,
    INTERNAL_ERROR = 50
  };
  
  enum DriverEvent {
    THREAD_START,
    THREAD_END
  };


  // Type Declarations
  // --------------------------------------------------------------------------------------------------------------  
  
  typedef Apto::Malloc::FixedSegment<32, Apto::Malloc::TCFreeList<Apto::BasicMalloc>, Apto::BasicMalloc> SmallObjectMalloc;
  
  typedef Apto::SmartPtr<Archive> ArchivePtr;
  typedef Apto::SmartPtr<const Archive> ConstArchivePtr;
  
  typedef Apto::String ArchiveObjectID;
  typedef Apto::String ArchiveObjectType;
  typedef Apto::Set<ArchiveObjectID> ArchiveObjectIDSet;
  typedef Apto::SmartPtr<ArchiveObjectIDSet> ArchiveObjectIDSetPtr;
  typedef Apto::SmartPtr<const ArchiveObjectIDSet> ConstArchiveObjectIDSetPtr;
  
  typedef Apto::SmartPtr<GeneticRepresentation> GeneticRepresentationPtr;
  typedef Apto::SmartPtr<const GeneticRepresentation> ConstGeneticRepresentationPtr;
  
  typedef Apto::Functor<bool, Apto::TL::Create<GeneticRepresentationPtr>, SmallObjectMalloc> GeneticRepresentationProcessFunctor;
  typedef Apto::Functor<bool, Apto::TL::Create<ConstGeneticRepresentationPtr>, SmallObjectMalloc> ConstGeneticRepresentationProcessFunctor;
  typedef Apto::Map<Apto::String, GeneticRepresentationProcessFunctor> GeneticRepresentationDispatchTable;
  typedef Apto::Map<Apto::String, ConstGeneticRepresentationProcessFunctor> ConstGeneticRepresentationDispatchTable;
  
  typedef Apto::SmartPtr<InstructionSequence> InstructionSequencePtr;
  typedef Apto::SmartPtr<const InstructionSequence> ConstInstructionSequencePtr;
  
  typedef Apto::SmartPtr<Genome> GenomePtr;
  typedef Apto::SmartPtr<const Genome> ConstGenomePtr;
  
  typedef int HardwareTypeID;
  
  typedef Apto::SmartPtr<InstructionSequence> InstructionSequencePtr;
  typedef Apto::SmartPtr<const InstructionSequence> ConstInstructionSequencePtr;
  
  typedef Apto::SmartPtr<Property> PropertyPtr;
  typedef Apto::String PropertyID;
  typedef Apto::String PropertyTypeID;
  typedef Apto::Set<PropertyID> PropertyIDSet;
  typedef Apto::SmartPtr<PropertyIDSet> PropertyIDSetPtr;
  typedef Apto::SmartPtr<const PropertyIDSet> ConstPropertyIDSetPtr;
  typedef Apto::SmartPtr<PropertyMap> PropertyMapPtr;
  typedef Apto::SmartPtr<const PropertyMap> ConstPropertyMapPtr;
  typedef Apto::Map<PropertyID, Apto::String, Apto::DefaultHashBTree, Apto::ImplicitDefault> PropertyDescriptionMap;
  
  typedef Apto::String UniverseFacetID;
  typedef Apto::SmartPtr<UniverseFacet, Apto::InternalRCObject> UniverseFacetPtr;
  typedef Apto::Functor<UniverseFacetPtr, Apto::TL::Create<ArchivePtr>, SmallObjectMalloc> UniverseFacetDeserializeFunctor;
  
  typedef Apto::Functor<void, Apto::TL::Create<DriverEvent>, SmallObjectMalloc> DriverCallback;

  
  // Constant Declarations
  // --------------------------------------------------------------------------------------------------------------  
  
  extern Update UPDATE_CONCURRENT;
  
  
  
  // Update - Type Definition
  // --------------------------------------------------------------------------------------------------------------
  
  struct Update
  {
  public:
    int discrete;
    float fraction;
    
    LIB_EXPORT inline Update(int d = 0, float f = 0.0f) : discrete(d), fraction(f) { fixup(); }
    LIB_EXPORT inline explicit Update(double u) : discrete(static_cast<int>(u)), fraction(static_cast<float>(u - discrete)) { fixup(); }
    LIB_EXPORT inline explicit Update(const Apto::String& str) : discrete(Apto::StrAs(str)), fraction(0.0f) { ; }
    
    // Conversion operations
    LIB_EXPORT inline operator double() const { return static_cast<double>(fraction) + discrete; }
    LIB_EXPORT inline operator int() const { return discrete; }

    
    // Discrete (integer) operations -- clears the fraction component, regardless of value
    LIB_EXPORT inline Update operator+(int v) { return Update(discrete + v, 0.0f); }
    LIB_EXPORT inline Update operator-(int v) { return Update(discrete - v, 0.0f); }
    
    LIB_EXPORT inline Update& operator+=(int v) { discrete += v; fraction = 0.0f; return *this; }
    LIB_EXPORT inline Update& operator-=(int v) { discrete -= v; fraction = 0.0f; return *this; }

    LIB_EXPORT inline Update& operator++() { discrete++; fraction = 0.0f; return *this; }
    LIB_EXPORT inline Update& operator++(int) { discrete++; fraction = 0.0f; return *this; }
    LIB_EXPORT inline Update& operator--() { discrete--; fraction = 0.0f; return *this; }
    LIB_EXPORT inline Update& operator--(int) { discrete--; fraction = 0.0f; return *this; }

    
    // Fractional operations
    LIB_EXPORT inline Update operator+(float v) { return Update(discrete, fraction + v); }
    LIB_EXPORT inline Update operator-(float v) { return Update(discrete, fraction - v); }
    
    LIB_EXPORT inline Update& operator+=(float v) { fraction += v; fixup(); return *this; }
    LIB_EXPORT inline Update& operator-=(float v) { fraction -= v; fixup(); return *this; }
    
    
    // Full value operations
    LIB_EXPORT inline Update operator+(Update v) { return Update(discrete + v.discrete, fraction + v.fraction); }
    LIB_EXPORT inline Update operator-(Update v) { return Update(discrete + v.discrete, fraction - v.fraction); }
    
    LIB_EXPORT inline Update& operator+=(Update v) { discrete += v.discrete; fraction += v.fraction; fixup(); return *this; }
    LIB_EXPORT inline Update& operator-=(Update v) { discrete -= v.discrete; fraction -= v.fraction; fixup(); return *this; }
    
    
  private:
    LIB_EXPORT inline void fixup()
    {
      while (fraction >= 1.0f) {
        fraction -= 1.0f;
        discrete++;
      }
      while (fraction <= 0.0f) {
        fraction += 1.0f;
        discrete--;
      }
    }
  };

};


namespace Apto {
  template <> class ConvertToStr<Avida::Update>
  {
  private:
    ConvertToStr<int> m_value;
    
  public:
    LIB_EXPORT inline ConvertToStr(Avida::Update value) : m_value(Apto::ConvertToStr<int>(value.discrete)) { ; }
    
    LIB_EXPORT inline operator Apto::BasicString<SingleThreaded>() const { return Apto::BasicString<SingleThreaded>(m_value); }
  };
}

#endif
