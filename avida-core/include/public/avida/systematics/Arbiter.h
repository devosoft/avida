/*
 *  systematics/Arbiter.h
 *  avida-core
 *
 *  Created by David on 12/14/11.
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

#ifndef AvidaSystematicsArbiter_h
#define AvidaSystematicsArbiter_h

#include "apto/platform.h"
#include "avida/core/Types.h"
#include "avida/systematics/Unit.h"


namespace Avida {
  namespace Systematics {
    
    // Arbiter
    // --------------------------------------------------------------------------------------------------------------
    
    class Arbiter : virtual public Apto::RefCountObject<Apto::ThreadSafe>
    {
    public:
      class Iterator;
      typedef Apto::SmartPtr<Iterator> IteratorPtr;
      
    private:
      RoleID m_role;
      Apto::Set<Listener*> m_listeners;
      
    public:
      LIB_EXPORT inline Arbiter(const RoleID& role) : m_role(role) { ; }
      LIB_EXPORT virtual ~Arbiter() = 0;
      
      LIB_EXPORT inline const RoleID& Role() const { return m_role; }
      
      
      // Subclass Methods
      LIB_EXPORT virtual GroupPtr ClassifyNewUnit(UnitPtr u, const ClassificationHints* hints = NULL) = 0;
      LIB_EXPORT virtual GroupPtr Group(GroupID g_id) = 0;
      
      LIB_EXPORT virtual void PerformUpdate(Context& ctx, Update current_update) = 0;
      
      LIB_EXPORT virtual IteratorPtr Begin() = 0;
      
      
      // Listeners
      LIB_EXPORT inline void AttachListener(Listener* listener) { m_listeners.Insert(listener); }
      LIB_EXPORT inline void DetachListener(Listener* listener) { m_listeners.Remove(listener); }
      
      // Serialization
      LIB_EXPORT virtual bool Serialize(ArchivePtr ar) const;
      LIB_EXPORT virtual bool LegacySave(void* df) const;
      LIB_EXPORT virtual GroupPtr LegacyLoad(void* props);
      
      
    protected:
      LIB_EXPORT void notifyListeners(GroupPtr g, EventType t, UnitPtr u = UnitPtr(NULL));
      
      
    public:
      class Iterator
      {
      public:
        virtual ~Iterator() = 0;
        
        virtual GroupPtr Get() = 0;
        virtual GroupPtr Next() = 0;        
      };
    };
    
  };
};

#endif
