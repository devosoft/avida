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
  class World;
  class WorldDriver;
  class WorldFacet;

  
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
  
  typedef int Update; // Discrete unit of activity in Avida
    
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
  
  typedef Apto::String WorldFacetID;
  typedef Apto::SmartPtr<WorldFacet, Apto::InternalRCObject> WorldFacetPtr;
  typedef Apto::Functor<WorldFacetPtr, Apto::TL::Create<ArchivePtr>, SmallObjectMalloc> WorldFacetDeserializeFunctor;
  
  typedef Apto::Functor<void, Apto::TL::Create<DriverEvent>, SmallObjectMalloc> DriverCallback;

  
  // Constant Declarations
  // --------------------------------------------------------------------------------------------------------------  
  
  extern Update UPDATE_CONCURRENT;
};

#endif
