/*
 *  cBirthNeighborhoodHandler.cc
 *  Avida
 *
 *  Created by David Bryson on 4/1/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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
 */

#include "cBirthNeighborhoodHandler.h"

#include "cBirthChamber.h"
#include "cBirthEntry.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cWorld.h"


cBirthNeighborhoodHandler::cBirthNeighborhoodHandler(cWorld* world, cBirthChamber* bc) : m_bc(bc)
{
  m_entries.Resize(world->GetPopulation().GetWorldX() * world->GetPopulation().GetWorldY());
}

cBirthNeighborhoodHandler::~cBirthNeighborhoodHandler()
{
}


cBirthEntry* cBirthNeighborhoodHandler::SelectOffspring(cAvidaContext& ctx, const Genome& offspring, cOrganism* parent)
{
  int parent_id = parent->GetOrgInterface().GetCellID();
  
  // Get all neighborhood cell ids
  Apto::Array<int> neighborhood;
  parent->GetOrgInterface().GetNeighborhoodCellIDs(neighborhood);
  
  // Produce a list of all valid offspring waiting
  Apto::Array<int> valid(neighborhood.GetSize());
  int valid_count = 0;
  for (int i = 0; i < neighborhood.GetSize(); i++) {
    // Store the cell id of valid birth entries in the valid list
    if (m_bc->ValidBirthEntry(m_entries[neighborhood[i]])) valid[valid_count++] = neighborhood[i];
  }
  if (m_bc->ValidBirthEntry(m_entries[parent_id])) valid[valid_count++] = parent_id;
  
  // If no valid entries exist, store the current offspring
  if (valid_count == 0) {
    m_bc->StoreAsEntry(offspring, parent, m_entries[parent_id]);
    return NULL; 				
  }

  // Select a random valid entry and return it
  return &(m_entries[valid[ctx.GetRandom().GetUInt(valid_count)]]);
}
