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
      struct ResourceType
      {
        Util::ArgSchema& schema;
        ResourceCreateFunctor create;
        
        LIB_LOCAL inline ResourceType(Util::ArgSchema& in_schema, ResourceCreateFunctor in_create)
        : schema(in_schema), create(in_create) { ; }
      };
      
    private:
      mutable Apto::Mutex m_mutex;
      Apto::Map<Apto::String, ResourceType*> m_resource_types;

    public:
      LIB_EXPORT static inline Library& Instance() { return LibrarySingleton::Instance(); }
      
      
      LIB_EXPORT inline const ResourceType* ResourceTypeOf(const Apto::String& res_type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_resource_types.GetWithDefault(res_type_name, NULL);
      }
      
      LIB_EXPORT inline bool IsResourceType(const Apto::String& res_type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_resource_types.Has(res_type_name);
      }
      
      
      LIB_EXPORT bool RegisterResourceType(const Apto::String& res_type_name, Util::ArgSchema& arg_schema,
                                           ResourceCreateFunctor res_create);
      
    private:
      LIB_LOCAL Library();
      LIB_LOCAL ~Library();
    };
    
    
  };
};

#endif
