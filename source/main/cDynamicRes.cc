/*
 *  cDynamicRes.cc
 *  Avida
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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
 */

#include "cDynamicRes.h"

cDynamicRes::cDynamicRes(int inworld_x, int inworld_y, int ingeometry)
: grid(inworld_x * inworld_y), m_initial(0.0), m_modified(false)
{
  int i;
 
  world_x = inworld_x;
  world_y = inworld_y;
  geometry = ingeometry;
  num_cells = world_x * world_y;
  for (i = 0; i < GetSize(); i++) {
    cResourceElement tmpelem;
    grid[i] = tmpelem;
  } 
}

cDynamicRes::cDynamicRes() : m_initial(0.0), m_modified(false)
{
  geometry = nGeometry::DYNAMIC;
}

cDynamicRes::~cDynamicRes() { ; }


/* Get the state of one element using the array index */

double cDynamicRes::GetAmount(int x) const { 
  if (x >= 0 && x < grid.GetSize()) {
    return grid[x].GetAmount(); 
  } else {
    return -99.9;
  }
}

/* Get the state of one element using the the x,y coordinate */

double cDynamicRes::GetAmount(int x, int y) const { 
  if (x >= 0 && x < world_x && y >= 0 && y < world_y) {
    return grid[y*world_x + x].GetAmount(); 
  } else {
    return -99.9;
  }
}

void cDynamicRes::SetCellAmount(int cell_id, double res)
{
  if (cell_id >= 0 && cell_id < grid.GetSize())
  {
    Element(cell_id).SetAmount(res);
  }
}
