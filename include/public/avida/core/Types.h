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
  template <typename T> struct PropertyTraits;
  template <typename T> class ReferenceProperty;
  class StringProperty;
  class World;
  class WorldDriver;
  class WorldFacet;

  
  // Type Declarations
  // --------------------------------------------------------------------------------------------------------------  
  
  typedef int Update; // Discrete unit of activity in Avida
  
  typedef Apto::SmartPtr<Archive> ArchivePtr;
  typedef Apto::SmartPtr<const Archive> ConstArchivePtr;
  
  typedef Apto::String ArchiveObjectID;
  typedef Apto::String ArchiveObjectType;
  typedef Apto::Set<ArchiveObjectID> ArchiveObjectIDSet;
  typedef Apto::SmartPtr<ArchiveObjectIDSet> ArchiveObjectIDSetPtr;
  typedef Apto::SmartPtr<const ArchiveObjectIDSet> ConstArchiveObjectIDSetPtr;
  
  typedef Apto::SmartPtr<GeneticRepresentation> GeneticRepresentationPtr;
  typedef Apto::SmartPtr<const GeneticRepresentation> ConstGeneticRepresentationPtr;
  
  typedef Apto::Functor<bool, Apto::TL::Create<GeneticRepresentationPtr> > GeneticRepresentationProcessFunctor;
  typedef Apto::Functor<bool, Apto::TL::Create<ConstGeneticRepresentationPtr> > ConstGeneticRepresentationProcessFunctor;
  typedef Apto::Map<Apto::String, GeneticRepresentationProcessFunctor> GeneticRepresentationDispatchTable;
  typedef Apto::Map<Apto::String, ConstGeneticRepresentationProcessFunctor> ConstGeneticRepresentationDispatchTable;
  
  typedef Apto::SmartPtr<Genome> GenomePtr;
  typedef Apto::SmartPtr<const Genome> ConstGenomePtr;
  
  typedef int HardwareTypeID;
  
  typedef Apto::String PropertyID;
  typedef Apto::String PropertyTypeID;
  typedef Apto::Set<PropertyID> PropertyIDSet;
  typedef Apto::SmartPtr<PropertyIDSet> PropertyIDSetPtr;
  typedef Apto::SmartPtr<const PropertyIDSet> ConstPropertyIDSetPtr;
  typedef Apto::Map<PropertyID, Property> PropertyMap;
  typedef Apto::SmartPtr<PropertyMap> PropertyMapPtr;
  typedef Apto::SmartPtr<const PropertyMap> ConstPropertyMapPtr;
  
  typedef Apto::String WorldFacetID;
  typedef Apto::SmartPtr<WorldFacet, Apto::InternalRCObject> WorldFacetPtr;
  typedef Apto::Functor<WorldFacetPtr, Apto::TL::Create<ArchivePtr> > WorldFacetDeserializeFunctor;
  
  typedef Apto::Functor<void, Apto::TL::Create<DriverEvent> > DriverCallback;
};

#endif
