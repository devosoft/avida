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
#include "avida/systematics/Types.h"


namespace Avida {
  namespace Systematics {
    
    // Arbiter
    // --------------------------------------------------------------------------------------------------------------
    
    class Arbiter
    {
      friend class Manager;
    protected:
      RoleID m_role;
      Apto::Array<GroupListenerPtr> m_listeners;
      
    public:
      LIB_EXPORT virtual ~Arbiter() = 0;
      
      LIB_EXPORT inline RoleID Role() const {
      
      LIB_EXPORT virtual GroupPtr ClassifyNewUnit(UnitPtr u, const ClassificationHints* hints = NULL) = 0;
      LIB_EXPORT virtual GroupPtr Group(GroupID g_id) = 0;
      
      LIB_EXPORT void UpdateReset() = 0;
      
      LIB_EXPORT virtual bool Serialize(ArchivePtr ar) const;
    };
    
  };
};

#endif
