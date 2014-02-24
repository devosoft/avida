/*
 *  environment/Library.h
 *  avida-core
 *
 *  Created by David on 1/30/13.
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

#ifndef AvidaEnvironmentLibrary_h
#define AvidaEnvironmentLibrary_h

#include "avida/environment/Types.h"
#include "avida/util/ArgParser.h"


namespace Avida {
  namespace Environment {
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::SingletonHolder<Library, Apto::CreateWithNew, Apto::DestroyAtExit, Apto::ThreadSafe> LibrarySingleton;
    
    
    // Environment::Library - Global container of available environment components
    // --------------------------------------------------------------------------------------------------------------
    
    class Library
    {
      friend Apto::CreateWithNew<Library>;
    public:
      struct ActionType
      {
        Util::ArgSchema schema;
        ActionSetup setup;
        ActionTest test;
        
        LIB_LOCAL inline ActionType(Util::ArgSchema& in_schema, ActionSetup in_setup, ActionTest in_test)
        : schema(in_schema), setup(in_setup), test(in_test) { ; }
      };

      struct ProcessType
      {
        Util::ArgSchema schema;
        ProcessCreate create;
        
        LIB_LOCAL inline ProcessType(Util::ArgSchema& in_schema, ProcessCreate in_create)
        : schema(in_schema), create(in_create) { ; }
      };

      struct ResourceType
      {
        Util::ArgSchema schema;
        ResourceCreateFunctor create;
        
        LIB_LOCAL inline ResourceType(Util::ArgSchema& in_schema, ResourceCreateFunctor in_create)
        : schema(in_schema), create(in_create) { ; }
      };
      
    private:
      mutable Apto::Mutex m_mutex;
      Apto::Map<Apto::String, ActionType*> m_action_types;
      Apto::Map<Apto::String, ProcessType*> m_process_types;
      Apto::Map<Apto::String, ResourceType*> m_resource_types;

    public:
      LIB_EXPORT static inline Library& Instance() { return LibrarySingleton::Instance(); }
      
      // ---------- Action Type Handling ----------
      LIB_EXPORT inline const ActionType* ActionTypeOf(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_action_types.GetWithDefault(type_name, NULL);
      }
      
      LIB_EXPORT inline bool IsActionType(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_action_types.Has(type_name);
      }
      

      LIB_EXPORT bool RegisterActionType(const Apto::String& type_name, Util::ArgSchema& arg_schema, ActionSetup in_setup,
                                         ActionTest in_test);
      
      
      // ---------- Process Type Handling ----------
      LIB_EXPORT inline const ProcessType* ProcessTypeOf(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_process_types.GetWithDefault(type_name, NULL);
      }
      
      LIB_EXPORT inline bool IsProcessType(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_process_types.Has(type_name);
      }
      
      LIB_EXPORT bool RegisterProcessType(const Apto::String& type_name, Util::ArgSchema& arg_schema, ProcessCreate in_create);

      
      
      // ---------- Resource Type Handling ----------
      LIB_EXPORT inline const ResourceType* ResourceTypeOf(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_resource_types.GetWithDefault(type_name, NULL);
      }
      
      LIB_EXPORT inline bool IsResourceType(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_resource_types.Has(type_name);
      }
      
      
      LIB_EXPORT bool RegisterResourceType(const Apto::String& type_name, Util::ArgSchema& arg_schema,
                                           ResourceCreateFunctor res_create);
      
    private:
      LIB_LOCAL Library();
      LIB_LOCAL ~Library();
    };
    
    
  };
};

#endif
