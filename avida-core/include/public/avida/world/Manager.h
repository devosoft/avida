/*
 *  world/Manager.h
 *  avida-core
 *
 *  Created by David on 1/24/13.
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

#ifndef AvidaWorldManager_h
#define AvidaWorldManager_h

#include "avida/core/Universe.h"
#include "avida/structure/Types.h"
#include "avida/world/Types.h"


namespace Avida {
  namespace World {
    
    // Manager
    // --------------------------------------------------------------------------------------------------------------
    
    class Manager : public UniverseFacet
    {
    private:
      Universe* m_universe;
      ContainerPtr m_top_level;
      
    public:
      LIB_EXPORT static ManagerPtr CreateWithStructure(Universe* universe, Structure::Controller* structure);
      LIB_EXPORT ~Manager();
      
      
      LIB_EXPORT inline Container& TopLevelContainer() { return *m_top_level; }
      LIB_EXPORT inline const Container& TopLevelContainer() const { return *m_top_level; }
      
      
      LIB_EXPORT static ManagerPtr Of(Universe* universe);
      
    public:
      LIB_EXPORT bool Serialize(ArchivePtr ar) const;
      
    public:
      LIB_LOCAL UniverseFacetID UpdateBefore() const;
      LIB_LOCAL UniverseFacetID UpdateAfter() const;
      
      
    private:
      LIB_EXPORT Manager(Universe* universe, Structure::Controller* structure);
    };
    
  };
};

#endif
