/*
 *  cBirthGridLocalHandler.h
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

#ifndef cBirthGridLocalHandler_h
#define cBirthGridLocalHandler_h

#include "avida/core/Types.h"

#include "cBirthSelectionHandler.h"
#include "cBitArray.h"

class cWorld;
class cBirthChamber;


class cBirthGridLocalHandler : public cBirthSelectionHandler
{
private:
  cWorld* m_world;
  cBirthChamber* m_bc;
  int m_world_x;
  int m_world_y;
  Apto::Array<cBirthEntry> m_entries;
  cBitArray m_occupied;
  
  
public:
  cBirthGridLocalHandler(cWorld* world, cBirthChamber* bc);
  ~cBirthGridLocalHandler();
  
  cBirthEntry* SelectOffspring(cAvidaContext& ctx, const Genome& offspring, cOrganism* parent);
  
  
private:
  bool hasNeighborWaiting(int parent_id);
  int selectRandomNeighbor(cAvidaContext& ctx, int parent_id);
};

#endif
