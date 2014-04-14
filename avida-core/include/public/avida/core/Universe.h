/*
 *  core/Universe.h
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

#ifndef AvidaCoreUniverse_h
#define AvidaCoreUniverse_h

#include "apto/platform.h"
#include "avida/core/Types.h"


namespace Avida {
  
  // Universe
  // --------------------------------------------------------------------------------------------------------------
  //
  // A Universe object contains a collection of facets (UniverseFacet) that implement top-level functionality.  These facets
  // can be retrieved and used from essentially any level of a given experimental run.  Facets also participate in the
  // update cycle of the experiment. Universe schedules the order of execution based on facet dependencies and takes care
  // of issuing PerformUpdate on all facets in the appropriate order.
  //
  // Several core facets have convenience/performance accessors, listed below. All others may be retrieve through the
  // main Facet() method.
  
  class Universe
  {
  private:
    UniverseFacetPtr m_biota_manager;
    UniverseFacetPtr m_data_manager;
    UniverseFacetPtr m_environment;
    UniverseFacetPtr m_hardware_manager;
    UniverseFacetPtr m_output_manager;
    UniverseFacetPtr m_systematics;
    UniverseFacetPtr m_world_manager;
    
    Apto::Map<UniverseFacetID, UniverseFacetPtr> m_facets;
    Apto::Array<UniverseFacetPtr> m_facet_order;
    
  public:
    LIB_EXPORT Universe();
    LIB_EXPORT Universe(ConstArchivePtr ar);
    
    // General facet methods
    LIB_EXPORT bool AttachFacet(UniverseFacetID facet_id, UniverseFacetPtr facet);
    LIB_EXPORT inline UniverseFacetPtr Facet(const UniverseFacetID& facet_id) const;

    // Specific built-in facet accessors
    LIB_EXPORT inline UniverseFacetPtr BiotaManager() const { return m_biota_manager; }
    LIB_EXPORT inline UniverseFacetPtr DataManager() const { return m_data_manager; }
    LIB_EXPORT inline UniverseFacetPtr Environment() const { return m_environment; }
    LIB_EXPORT inline UniverseFacetPtr HardwareManager() const { return m_hardware_manager; }
    LIB_EXPORT inline UniverseFacetPtr OutputManager() const { return m_output_manager; }
    LIB_EXPORT inline UniverseFacetPtr Systematics() const { return m_systematics; }
    LIB_EXPORT inline UniverseFacetPtr WorldManager() const { return m_output_manager; }
    
    // Actions
    LIB_EXPORT void PerformUpdate(Context& ctx, Update current_update);
    
    LIB_EXPORT bool Serialize(ArchivePtr ar) const;
  };
  

  inline UniverseFacetPtr Universe::Facet(const UniverseFacetID& facet_id) const
  {
    UniverseFacetPtr facet;
    m_facets.Get(facet_id, facet);
    return facet;
  }
  
  
  // UniverseFacet
  // --------------------------------------------------------------------------------------------------------------
  //
  // Protocol definition for world facet classes that register with world. These methods define the update order
  // dependencies of the facet as well as the PerformUpdate method signature. Facets (once serialization has been
  // implemented) must register their facet type and deserialization functor with the global factory via 
  // RegisterFacetType().
  
  class UniverseFacet : public Apto::RefCountObject<Apto::ThreadSafe>
  {
  public:
    LIB_EXPORT virtual UniverseFacetID UpdateBefore() const = 0;
    LIB_EXPORT virtual UniverseFacetID UpdateAfter() const = 0;
    
    LIB_EXPORT virtual void PerformUpdate(Context& ctx, Update current_update);
    
    LIB_EXPORT virtual bool Serialize(ArchivePtr ar) const = 0;
    
    LIB_EXPORT static UniverseFacetPtr Deserialize(ArchivePtr ar);
    LIB_EXPORT static bool RegisterFacetType(UniverseFacetID facet_id, UniverseFacetDeserializeFunctor facet_func);
  };
  
  
  // Reserved Names
  // --------------------------------------------------------------------------------------------------------------

  namespace Reserved {
    LIB_EXPORT extern const UniverseFacetID BiotaManagerFacetID;
    LIB_EXPORT extern const UniverseFacetID DataManagerFacetID;
    LIB_EXPORT extern const UniverseFacetID EnvironmentFacetID;
    LIB_EXPORT extern const UniverseFacetID HardwareManagerFacetID;
    LIB_EXPORT extern const UniverseFacetID OutputManagerFacetID;
    LIB_EXPORT extern const UniverseFacetID SystematicsFacetID;
    LIB_EXPORT extern const UniverseFacetID WorldManagerFacetID;
  };
};

#endif
