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

#ifndef AvidaCoreLibrary_h
#define AvidaCoreLibrary_h

#include "avida/core/Types.h"

#include "avida/util/ArgParser.h"


namespace Avida {
  namespace Core {
  
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::SingletonHolder<Library, Apto::CreateWithNew, Apto::DestroyAtExit, Apto::ThreadSafe> LibrarySingleton;
    
    
    // Core::Library
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

      struct EventType
      {
        Util::ArgSchema schema;
        EventCreateFunctor create;
        
        LIB_LOCAL inline EventType(Util::ArgSchema& in_schema, EventCreateFunctor in_create)
          : schema(in_schema), create(in_create) { ; }
      };

      struct EventTriggerType
      {
        EventTriggerFunctor trigger;
        
        LIB_LOCAL inline EventTriggerType(EventTriggerFunctor in_trigger) : trigger(in_trigger) { ; }
      };

      
    private:
      mutable Apto::Mutex m_mutex;
      Apto::Map<Apto::String, EpigeneticObjectType*> m_epi_types;
      Apto::Map<Apto::String, EventType*> m_event_types;
      Apto::Map<Apto::String, EventTriggerType*> m_event_trigger_types;
      
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

      
      
      LIB_EXPORT inline const EventType* EventTypeOf(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_event_types.GetWithDefault(type_name, NULL);
      }
      
      LIB_EXPORT inline bool IsEventType(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_event_types.Has(type_name);
      }
      
      LIB_EXPORT bool RegisterEventType(const Apto::String& type_name, Util::ArgSchema& schema, EventCreateFunctor create);

      

      LIB_EXPORT inline const EventTriggerType* EventTriggerTypeOf(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_event_trigger_types.GetWithDefault(type_name, NULL);
      }
      
      LIB_EXPORT inline bool IsEventTriggerType(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_event_trigger_types.Has(type_name);
      }
      
      LIB_EXPORT bool RegisterEventTriggerType(const Apto::String& type_name, EventTriggerFunctor trigger);

      
    private:
      LIB_LOCAL Library();
      LIB_LOCAL ~Library();
    };
    
    
  };
};

#endif
