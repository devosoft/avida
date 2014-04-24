/*
 *  systematics/Manager.h
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

#ifndef AvidaSystematicsManager_h
#define AvidaSystematicsManager_h

#include "apto/platform.h"
#include "avida/core/World.h"
#include "avida/systematics/Types.h"


namespace Avida {
  namespace Systematics {
    
    // Manager
    // --------------------------------------------------------------------------------------------------------------
    
    class Manager : public WorldFacet
    {
    private:
      Apto::Array<ArbiterPtr> m_arbiters;
      
    public:
      LIB_EXPORT inline Manager() { ; }
      LIB_EXPORT inline ~Manager() { ; }
      
      bool RegisterArbiter(ArbiterPtr a);
      ArbiterPtr ArbiterForRole(const RoleID& role);
      
      LIB_EXPORT void ClassifyNewUnit(UnitPtr u, const RoleClassificationHints* role_hints = NULL);
      
      LIB_EXPORT bool AttachTo(World* world);
      LIB_EXPORT static ManagerPtr Of(World* world);
      
      
      LIB_EXPORT bool Serialize(ArchivePtr ar) const;
      
      
    public:
      // WorldFacet Support Methods
      LIB_LOCAL WorldFacetID UpdateBefore() const;
      LIB_LOCAL WorldFacetID UpdateAfter() const;

      LIB_LOCAL void PerformUpdate(Context& ctx, Update current_update);
    };
    
  };
};

#endif
