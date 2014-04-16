/*
 *  hardware/Library.h
 *  avida-core
 *
 *  Created by David on 2/1/13.
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

#ifndef AvidaHardwareLibrary_h
#define AvidaHardwareLibrary_h

#include "avida/hardware/Types.h"
#include "avida/util/ArgParser.h"


namespace Avida {
  namespace Hardware {
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    typedef Apto::SingletonHolder<Library, Apto::CreateWithNew, Apto::DestroyAtExit, Apto::ThreadSafe> LibrarySingleton;
    
    
    // Hardware::Library - Global container of available hardware components
    // --------------------------------------------------------------------------------------------------------------
    
    class Library
    {
      friend Apto::CreateWithNew<Library>;
    public:
      struct HardwareType
      {
        Util::ArgSchema& schema;
        HardwareCreateFunctor create;
        
        LIB_LOCAL inline HardwareType(Util::ArgSchema& in_schema, HardwareCreateFunctor in_create)
        : schema(in_schema), create(in_create) { ; }
      };

      struct FeatureType
      {
        Util::ArgSchema& schema;
        FeatureCreateFunctor create;
        FeatureItemConfigFunctor config;
        Apto::Set<Apto::String> hw_compatibility;
        
        
        LIB_LOCAL inline FeatureType(Util::ArgSchema& in_schema, FeatureCreateFunctor in_create,
                                     FeatureItemConfigFunctor in_config, Apto::Set<Apto::String> in_compat)
        : schema(in_schema), create(in_create), config(in_config), hw_compatibility(in_compat) { ; }
      };

    private:
      mutable Apto::Mutex m_mutex;
      Apto::Map<Apto::String, HardwareType*> m_hardware_types;
      Apto::Map<Apto::String, FeatureType*> m_feature_types;
      
    public:
      LIB_EXPORT static inline Library& Instance() { return LibrarySingleton::Instance(); }
      
      
      LIB_EXPORT inline const HardwareType* HardwareTypeOf(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_hardware_types.GetWithDefault(type_name, NULL);
      }
      
      LIB_EXPORT inline bool IsHardwareType(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_hardware_types.Has(type_name);
      }
      
      
      LIB_EXPORT bool RegisterHardwareType(const Apto::String& type_name, Util::ArgSchema& arg_schema,
                                           HardwareCreateFunctor hw_create);

      
      LIB_EXPORT inline const FeatureType* FeatureTypeOf(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_feature_types.GetWithDefault(type_name, NULL);
      }
      
      LIB_EXPORT inline bool IsFeatureType(const Apto::String& type_name) const
      {
        Apto::MutexAutoLock lock(m_mutex);
        return m_feature_types.Has(type_name);
      }
      
      
      LIB_EXPORT int RegisterFeatureType(const Apto::String& type_name, Util::ArgSchema& arg_schema,
                                         FeatureCreateFunctor feat_create, FeatureItemConfigFunctor feat_conf,
                                         Apto::Set<Apto::String> feat_hw_compat);

    private:
      LIB_LOCAL Library();
      LIB_LOCAL ~Library();
    };
    
  };
};

#endif
