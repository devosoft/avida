/*
 *  cBirthGridLocalHandler.cc
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

#include "cBirthGridLocalHandler.h"

#include "cBirthChamber.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cWorld.h"

#include "AvidaTools.h"


using namespace AvidaTools;


cBirthGridLocalHandler::cBirthGridLocalHandler(cWorld* world, cBirthChamber* bc)
: m_world(world), m_bc(bc), m_world_x(m_world->GetPopulation().GetWorldX()), m_world_y(m_world->GetPopulation().GetWorldY())
{
  m_entries.Resize(m_world_x * m_world_y);
  m_occupied.Resize(m_world_x * m_world_y);
}

cBirthGridLocalHandler::~cBirthGridLocalHandler()
{
}


cBirthEntry* cBirthGridLocalHandler::SelectOffspring(cAvidaContext& ctx, const Genome& offspring, cOrganism* parent)
{
  int parent_id = parent->GetOrgInterface().GetCellID();
  
  if (!hasNeighborWaiting(parent_id)) { 
    m_bc->StoreAsEntry(offspring, parent, m_entries[parent_id]);
    m_occupied[parent_id] = true;
    return NULL; 				
  }
  
  int selected = selectRandomNeighbor(ctx, parent_id); 
  m_occupied[selected] = false;
  return &(m_entries[selected]);
}


bool cBirthGridLocalHandler::hasNeighborWaiting(int parent_id)
{
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) { 
      if (m_occupied[GridNeighbor(parent_id, m_world_x, m_world_y, i, j)]) return true;
    }
  }
  return false;
}


int cBirthGridLocalHandler::selectRandomNeighbor(cAvidaContext& ctx, int parent_id)
{
  int selected = -1;
  
  while (true) {
    int test_neighbor = ctx.GetRandom().GetUInt(9);
    int i = test_neighbor / 3 - 1; 
    int j = test_neighbor % 3 - 1;
    selected = GridNeighbor(parent_id, m_world_x, m_world_y, i, j);
    
    if (m_occupied[selected]) break;
  }
  
  return selected;
}
