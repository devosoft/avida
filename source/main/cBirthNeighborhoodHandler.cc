/*
 *  cBirthNeighborhoodHandler.cc
 *  Avida
 *
 *  Created by David Bryson on 4/1/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cBirthNeighborhoodHandler.h"

#include "cBirthChamber.h"
#include "cBirthEntry.h"
#include "cOrganism.h"
#include "cWorld.h"


cBirthNeighborhoodHandler::cBirthNeighborhoodHandler(cWorld* world, cBirthChamber* bc) : m_bc(bc)
{
  m_entries.Resize(world->GetConfig().WORLD_X.Get() * world->GetConfig().WORLD_Y.Get());
}

cBirthNeighborhoodHandler::~cBirthNeighborhoodHandler()
{
  for (int i = 0; i < m_entries.GetSize(); i++) {
    m_bc->ClearEntry(m_entries[i]);
  }
}


cBirthEntry* cBirthNeighborhoodHandler::SelectOffspring(cAvidaContext& ctx, const cGenome& offspring, cOrganism* parent)
{
  int parent_id = parent->GetOrgInterface().GetCellID();
  
  // Get all neighborhood cell ids
  tArray<int> neighborhood;
  parent->GetOrgInterface().GetNeighborhoodCellIDs(neighborhood);
  
  // Produce a list of all valid offspring waiting
  tArray<int> valid(neighborhood.GetSize());
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
