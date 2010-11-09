/*
 *  cBirthGridLocalHandler.cc
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

#include "cBirthGridLocalHandler.h"

#include "cBirthChamber.h"
#include "cOrganism.h"
#include "cWorld.h"

#include "AvidaTools.h"


using namespace AvidaTools;


cBirthGridLocalHandler::cBirthGridLocalHandler(cWorld* world, cBirthChamber* bc)
: m_world(world), m_bc(bc), m_world_x(m_world->GetConfig().WORLD_X.Get()), m_world_y(m_world->GetConfig().WORLD_Y.Get())
{
  m_entries.Resize(m_world_x * m_world_y);
}

cBirthGridLocalHandler::~cBirthGridLocalHandler()
{
  for (int i = 0; i < m_entries.GetSize(); i++) {
    m_bc->ClearEntry(m_entries[i]);
  }
}


cBirthEntry* cBirthGridLocalHandler::SelectOffspring(cAvidaContext& ctx, const cGenome& offspring, cOrganism* parent)
{
  int parent_id = parent->GetOrgInterface().GetCellID();
  
  if (!hasNeighborWaiting(parent_id)) { 
    m_bc->StoreAsEntry(offspring, parent, m_entries[parent_id]);
    return NULL; 				
  }
  
  return &(m_entries[selectRandomNeighbor(ctx, parent_id)]);
}


bool cBirthGridLocalHandler::hasNeighborWaiting(int parent_id)
{
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) { 
      if (m_entries[GridNeighbor(parent_id, m_world_x, m_world_y, i, j)].timestamp >= 0) return true;
    }
  }
  return false;
}


int cBirthGridLocalHandler::selectRandomNeighbor(cAvidaContext& ctx, int parent_id)
{
  while (true) {
    int test_neighbor = ctx.GetRandom().GetUInt(9);
    int i = test_neighbor / 3 - 1; 
    int j = test_neighbor % 3 - 1;
    int test_loc = GridNeighbor(parent_id, m_world_x, m_world_y, i, j);
    
    if (m_entries[test_loc].timestamp >= 0) return test_loc;
  }
  
  return -1;
}
