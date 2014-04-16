/*
 *  biota/Library.h
 *  avida-core
 *
 *  Created by David on 4/16/14.
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

#ifndef AvidaBiotaLibrary_h
#define AvidaBiotaLibrary_h

#include "avida/core/Types.h"

namespace Avida {
  
  // Type Declarations
  // --------------------------------------------------------------------------------------------------------------
  
  typedef Apto::SingletonHolder<Library, Apto::CreateWithNew, Apto::DestroyAtExit, Apto::ThreadSafe> LibrarySingleton;
  
  
  // Biota::Library - Global container of available biota components
  // --------------------------------------------------------------------------------------------------------------
  
  class Library
  {
    friend Apto::CreateWithNew<Library>;
  public:
    struct EpigeneticObjectType
    {
      EpigeneticObjectCreateFunctor create;
      
      LIB_LOCAL inline EpigeneticObjectType(EpigeneticObjectCreateFunctor in_create)
      : create(in_create) { ; }
    };
    
    
  private:
    mutable Apto::Mutex m_mutex;
    Apto::Map<Apto::String, EpigeneticObjectType*> m_epi_types;
    
  public:
    LIB_EXPORT static inline Library& Instance() { return LibrarySingleton::Instance(); }
    
    
    LIB_EXPORT inline const EpigeneticObjectType* EpigeneticObjectTypeOf(const Apto::String& type_name) const
    {
      Apto::MutexAutoLock lock(m_mutex);
      return m_epi_types.GetWithDefault(type_name, NULL);
    }
    
    LIB_EXPORT inline bool IsEpigeneticObjectType(const Apto::String& type_name) const
    {
      Apto::MutexAutoLock lock(m_mutex);
      return m_epi_types.Has(type_name);
    }
    
    
    LIB_EXPORT bool RegisterEpigeneticObjectType(const Apto::String& type_name, EpigeneticObjectCreateFunctor create);
    
    
    
  private:
    LIB_LOCAL Library();
    LIB_LOCAL ~Library();
  };
  
};

#endif
