/*
 *  data/cManager.h
 *  avida-core
 *
 *  Created by David on 5/16/11.
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

#ifndef AvidaDataManager_h
#define AvidaDataManager_h

#include "apto/core.h"
#include "apto/platform.h"

class cWorld;


namespace Avida {
  namespace Data {
    
    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    class cProvider;
    
    
    // cManager - Manages available and active data providers for a given world
    // --------------------------------------------------------------------------------------------------------------
    
    class cManager
    {
    public:
      typedef Apto::Functor<cProvider*, Apto::TL::Create<cWorld*> > ProviderActivateFunctor;
      typedef Apto::SmartPtr<const Apto::Set<Apto::String>, Apto::ThreadSafeRefCount> AvailableSetPtr;
      
    private:
      cWorld* m_world;
      Apto::Map<Apto::String, ProviderActivateFunctor> m_provider_map;
      mutable AvailableSetPtr m_available;
      
      Apto::Array<cProvider*> m_active_providers;
      Apto::Map<Apto::String, cProvider*> m_active_map;
      
    public:
      LIB_EXPORT cManager(cWorld* world);
      
      LIB_EXPORT AvailableSetPtr GetAvailable() const;
      LIB_EXPORT bool IsAvailable(const Apto::String& data_id) const;
      LIB_EXPORT bool IsActive(const Apto::String& data_id) const;
      
      
      LIB_EXPORT bool Register(const Apto::String& data_id, ProviderActivateFunctor functor);
      
    public:
      LIB_LOCAL void UpdateState();
    };
    
  };
};

#endif
