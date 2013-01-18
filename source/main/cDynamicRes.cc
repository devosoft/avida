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

#include "AvidaTools.h"
#include "nGeometry.h"

#include <cmath>

using namespace std;
using namespace AvidaTools;

/* Setup a single spatial resource using defaults  */

cDynamicRes::cDynamicRes(int inworld_x, int inworld_y, int ingeometry)
: grid(inworld_x * inworld_y), m_initial(0.0), m_modified(false)
{
  int i;
 
  world_x = inworld_x;
  world_y = inworld_y;
  geometry = ingeometry;
  num_cells = world_x * world_y;
  for (i = 0; i < GetSize(); i++) {
    cDynamicResElement tmpelem;
    grid[i] = tmpelem;
   } 
   SetPointers();
}

cDynamicRes::cDynamicRes() : m_initial(0.0), m_modified(false)
{
  geometry = nGeometry::GLOBAL;
}

cDynamicRes::~cDynamicRes() { ; }


void cDynamicRes::ResizeClear(int inworld_x, int inworld_y, int ingeometry)
{
  int i;
 
  grid.ResizeClear(inworld_x * inworld_y); 
  world_x = inworld_x;
  world_y = inworld_y;
  geometry = ingeometry;
  num_cells = world_x * world_y;
  for (i = 0; i < GetSize(); i++) {
    cDynamicResElement tmpelem;
    grid[i] = tmpelem;
   } 
   SetPointers();
}

void cDynamicRes::SetPointers()
{
  /* Pointer 0 will point to the cell above and to the left the current cell
     and will go clockwise around the cell.                               */

  int     i,ii;
  double  SQRT2 = sqrt(2.0);

  /* First treat all cells like they are in a torus */

  for (i = 0; i < GetSize(); i++) {
    grid[i].SetPtr(0 ,GridNeighbor(i, world_x, world_y, -1, -1), -1, -1, SQRT2);
    grid[i].SetPtr(1 ,GridNeighbor(i, world_x, world_y,  0, -1),  0, -1, 1.0);
    grid[i].SetPtr(2 ,GridNeighbor(i, world_x, world_y, +1, -1), +1, -1, SQRT2);
    grid[i].SetPtr(3 ,GridNeighbor(i, world_x, world_y, +1,  0), +1,  0, 1.0);
    grid[i].SetPtr(4 ,GridNeighbor(i, world_x, world_y, +1, +1), +1, +1, SQRT2);
    grid[i].SetPtr(5 ,GridNeighbor(i, world_x, world_y,  0, +1),  0, +1, 1.0);
    grid[i].SetPtr(6 ,GridNeighbor(i, world_x, world_y, -1, +1), -1, +1, SQRT2);
    grid[i].SetPtr(7 ,GridNeighbor(i, world_x, world_y, -1,  0), -1,  0, 1.0);
  }
 
  /* Fix links for top, bottom and sides for non-torus */
  
  if (geometry == nGeometry::GRID) {
    /* Top and bottom */

    for (i = 0; i < world_x; i++) {
      grid[i].SetPtr(0, -99, -99, -99, -99.0);
      grid[i].SetPtr(1, -99, -99, -99, -99.0);
      grid[i].SetPtr(2, -99, -99, -99, -99.0);
      ii = num_cells-1-i;
      grid[ii].SetPtr(4, -99, -99, -99, -99.0);
      grid[ii].SetPtr(5, -99, -99, -99, -99.0);
      grid[ii].SetPtr(6, -99, -99, -99, -99.0);
    }

    /* fix links for right and left sides */

    for (i = 0; i < world_y; i++) {
      ii = i * world_x;    
      grid[ii].SetPtr(0, -99, -99, -99, -99.0);
      grid[ii].SetPtr(7, -99, -99, -99, -99.0);
      grid[ii].SetPtr(6, -99, -99, -99, -99.0);
      ii = ((i + 1) * world_x) - 1;
      grid[ii].SetPtr(2, -99, -99, -99, -99.0);
      grid[ii].SetPtr(3, -99, -99, -99, -99.0);
      grid[ii].SetPtr(4, -99, -99, -99, -99.0);
    }
  }
}

/* Set all the individual cells to their initial values */
void cDynamicRes::SetCellList(Apto::Array<cCellResource>* in_cell_list_ptr)
{
  cell_list_ptr = in_cell_list_ptr;
  for (int i = 0; i < cell_list_ptr->GetSize(); i++) {
    int cell_id = (*cell_list_ptr)[i].GetId();
    
    /* Be sure the user entered a valid cell id or if the the program is loading
       the resource for the testCPU that does not have a grid set up */
       
    if (cell_id >= 0 && cell_id <= grid.GetSize()) {
      Rate((*cell_list_ptr)[i].GetId(), (*cell_list_ptr)[i].GetInitial());
      State((*cell_list_ptr)[i].GetId());
      Element(cell_id).SetInitial((*cell_list_ptr)[i].GetInitial());
    }
  }
}

/* Set the rate variable for one element using the array index */

void cDynamicRes::Rate(int x, double ratein) const {
  if (x >= 0 && x < grid.GetSize()) {
    grid[x].Rate(ratein);
  } else {
    assert(false); // x not valid id
  }
}

/* Set the rate variable for one element using the x,y coordinate */

void cDynamicRes::Rate(int x, int y, double ratein) const { 
  if (x >= 0 && x < world_x && y>= 0 && y < world_y) {
    grid[y * world_x + x].Rate(ratein);
  } else {
    assert(false); // x or y not valid id
  }
}

/* Fold the rate variable into the resource state for one element using 
   the array index */
   
void cDynamicRes::State(int x) { 
  if (x >= 0 && x < grid.GetSize()) {
    grid[x].State();
  } else {
    assert(false); // x not valid id
  }
}

/* Fold the rate variable into the resource state for one element using 
   the x,y coordinate */
   
void cDynamicRes::State(int x, int y) { 
  if (x >= 0 && x < world_x && y >= 0 && y < world_y) {
    grid[y*world_x + x].State();
  } else {
    assert(false); // x or y not valid id
  }
}

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

void cDynamicRes::RateAll(double ratein) {

  int i;
 
  for (i = 0; i < num_cells; i++) {
    grid[i].Rate(ratein);
  } 
}

/* For each cell in the grid add the changes stored in the rate variable
   with the total of the resource */

void cDynamicRes::StateAll() {

  int i;
 
  for (i = 0; i < num_cells; i++) {
    grid[i].State();
  } 
}

/* Total up all the resources in each cell */

double cDynamicRes::SumAll() const{

  int i;
  double sum = 0.0;

  for (i = 0; i < num_cells; i++) {
    sum += GetAmount(i);
  } 
  return sum;
}

void cDynamicRes::SetCellAmount(int cell_id, double res)
{
  if (cell_id >= 0 && cell_id < grid.GetSize())
  {
    Element(cell_id).SetAmount(res);
  }
}


void cDynamicRes::ResetResourceCounts()
{
  for (int i = 0; i < grid.GetSize(); i++) grid[i].ResetResourceCount(m_initial);
}
