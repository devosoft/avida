/*
 *  cBirthGridLocalHandler.h
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

#ifndef cBirthGridLocalHandler_h
#define cBirthGridLocalHandler_h

#ifndef cBirthSelectionHandler_h
#include "cBirthSelectionHandler.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif


class cWorld;
class cBirthChamber;


class cBirthGridLocalHandler : public cBirthSelectionHandler
{
private:
  cWorld* m_world;
  cBirthChamber* m_bc;
  int m_world_x;
  int m_world_y;
  tArray<cBirthEntry> m_entries;
  
  
public:
  cBirthGridLocalHandler(cWorld* world, cBirthChamber* bc);
  ~cBirthGridLocalHandler();
  
  cBirthEntry* SelectOffspring(cAvidaContext& ctx, const cGenome& offspring, cOrganism* parent);
  
  
private:
  bool hasNeighborWaiting(int parent_id);
  int selectRandomNeighbor(cAvidaContext& ctx, int parent_id);
};

#endif
