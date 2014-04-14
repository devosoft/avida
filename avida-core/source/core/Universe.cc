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

#include "avida/core/Universe.h"

#include "avida/core/Archive.h"


static const int WORLD_ARCHIVE_VERSION = 1;

const Avida::UniverseFacetID Avida::Reserved::BiotaManagerFacetID("biotamanager");
const Avida::UniverseFacetID Avida::Reserved::DataManagerFacetID("datamanager");
const Avida::UniverseFacetID Avida::Reserved::EnvironmentFacetID("environment");
const Avida::UniverseFacetID Avida::Reserved::HardwareManagerFacetID("hardwaremanager");
const Avida::UniverseFacetID Avida::Reserved::OutputManagerFacetID("outputmanager");
const Avida::UniverseFacetID Avida::Reserved::SystematicsFacetID("systematics");
const Avida::UniverseFacetID Avida::Reserved::WorldManagerFacetID("worldmanager");


Avida::Universe::Universe()
{
  
  
}


bool Avida::Universe::AttachFacet(UniverseFacetID facet_id, UniverseFacetPtr facet)
{
  if (m_facets.Has(facet_id)) return false;
  
  // @TODO handle facet order
  int min = 0;
  int max = m_facet_order.GetSize();
  
  if (facet->UpdateAfter() != "" && m_facets.Has(facet->UpdateAfter())) {
    UniverseFacetPtr after = m_facets[facet->UpdateAfter()];
    for (int i = 0; i < m_facet_order.GetSize(); i++) {
      if (m_facet_order[i] == after) {
        min = i + 1;
        break;
      }
    }
  }
  if (facet->UpdateBefore() != "" && m_facets.Has(facet->UpdateBefore())) {
    UniverseFacetPtr before = m_facets[facet->UpdateBefore()];
    for (int i = 0; i < m_facet_order.GetSize(); i++) {
      if (m_facet_order[i] == before) {
        max = i;
        break;
      }
    }
  }
  
  for (int i = 0; i < m_facet_order.GetSize(); i++) {
    if (i < max && m_facet_order[i]->UpdateAfter() == facet_id) max = i;
    if (i > min && m_facet_order[i]->UpdateBefore() == facet_id) min = i + 1;
  }

  // Conflict in facet scheduling
  if (min > max) return false;

  int insert_at = min;
  
  // Push new facet onto the end, then shift values until it is in position.
  m_facet_order.Push(facet);
  for (int i = m_facet_order.GetSize() - 1; i > insert_at; i--) m_facet_order.Swap(i, i - 1);
  
  if (facet_id == Reserved::BiotaManagerFacetID) m_biota_manager = facet;
  else if (facet_id == Reserved::DataManagerFacetID) m_data_manager = facet;
  else if (facet_id == Reserved::EnvironmentFacetID) m_environment = facet;
  else if (facet_id == Reserved::HardwareManagerFacetID) m_hardware_manager = facet;
  else if (facet_id == Reserved::OutputManagerFacetID) m_output_manager = facet;
  else if (facet_id == Reserved::SystematicsFacetID) m_systematics = facet;
  else if (facet_id == Reserved::WorldManagerFacetID) m_world_manager = facet;
  
  m_facets[facet_id] = facet;
        
        
  return true;
}

void Avida::Universe::PerformUpdate(Context& ctx, Update current_update)
{
  for (int i = 0; i < m_facet_order.GetSize(); i++) {
    m_facet_order[i]->PerformUpdate(ctx, current_update);
  }
}


bool Avida::Universe::Serialize(ArchivePtr ar) const
{
  ar->SetObjectType("core.world");
  ar->SetVersion(WORLD_ARCHIVE_VERSION);
  
  Apto::Map<UniverseFacetID, UniverseFacetPtr>::KeyIterator kit = m_facets.Keys();
  while (kit.Next()) {
    ArchivePtr facet_ar = ar->DefineSubObject(*kit.Get());
    this->Facet(*kit.Get())->Serialize(facet_ar);
  }
  
  return true;
}


void Avida::UniverseFacet::PerformUpdate(Context&, Update)
{
}


Avida::UniverseFacetPtr Avida::UniverseFacet::Deserialize(ArchivePtr)
{
  // @TODO
  return UniverseFacetPtr();
}

bool Avida::UniverseFacet::RegisterFacetType(UniverseFacetID, UniverseFacetDeserializeFunctor)
{
  // @TODO
  return false;
}
