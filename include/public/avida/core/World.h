/*
 *  core/World.h
 *  avida-core
 *
 *  Created by David on 6/29/11.
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

#ifndef AvidaCoreWorld_h
#define AvidaCoreWorld_h

#include "apto/platform.h"
#include "avida/core/Types.h"


namespace Avida {
  
  // World - container for the various facets of an Avida experiment
  // --------------------------------------------------------------------------------------------------------------
  
  class World
  {
  private:
    WorldFacetPtr m_data_manager;
    WorldFacetPtr m_environment;
    WorldFacetPtr m_systematics;
    
    Apto::Map<WorldFacetID, WorldFacetPtr> m_facets;
    Apto::Array<WorldFacetPtr> m_facet_order;
    
  public:
    LIB_EXPORT World();
    LIB_EXPORT World(ConstArchivePtr ar);
    
    // General facet methods
    LIB_EXPORT bool AttachFacet(WorldFacetID facet_id, WorldFacetPtr facet);
    LIB_EXPORT inline WorldFacetPtr Facet(const WorldFacetID& facet_id) const;

    // Specific built-in facet accessors
    LIB_EXPORT inline WorldFacetPtr DataManager() const { return m_data_manager; }
    LIB_EXPORT inline WorldFacetPtr Environment() const { return m_environment; }
    LIB_EXPORT inline WorldFacetPtr Systematics() const { return m_systematics; }
    
    // Actions
    LIB_EXPORT void PerformUpdate(Context& ctx, Update current_update);
    
    LIB_EXPORT bool Serialize(ArchivePtr ar) const;
  };
  

  inline WorldFacetPtr World::Facet(const WorldFacetID& facet_id) const
  {
    WorldFacetPtr facet;
    m_facets.Get(facet_id, facet);
    return facet;
  }
  
  
  // WorldFacet
  // --------------------------------------------------------------------------------------------------------------
  
  class WorldFacet : public Apto::MTRefCountObject
  {
  public:
    LIB_EXPORT virtual WorldFacetID UpdateBefore() const = 0;
    LIB_EXPORT virtual WorldFacetID UpdateAfter() const = 0;
    
    LIB_EXPORT virtual void PerformUpdate(Context& ctx, Update current_update);
    
    LIB_EXPORT virtual bool Serialize(ArchivePtr ar) const = 0;
    
    LIB_EXPORT static WorldFacetPtr Deserialize(ArchivePtr ar);
    LIB_EXPORT static bool RegisterFacetType(WorldFacetID facet_id, WorldFacetDeserializeFunctor facet_func);
  };
  
  
  // Reserved Names
  // --------------------------------------------------------------------------------------------------------------

  namespace Reserved
  {
    LIB_EXPORT extern const WorldFacetID DataManagerFacetID;
    LIB_EXPORT extern const WorldFacetID EnvironmentFacetID;
    LIB_EXPORT extern const WorldFacetID SystematicsFacetID;
  };
};

#endif
