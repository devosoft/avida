/*
 *  cSpatialResCount.cc
 *  Avida
 *
 *  Called "spatial_res_count.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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
 *  along with this program; if not, write to the Free Software Foundation, 
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cSpatialResCount.h"

#include "AvidaTools.h"
#include "nGeometry.h"

#include <cmath>

using namespace std;
using namespace AvidaTools;

/* Setup a single spatial resource with known flows */

cSpatialResCount::cSpatialResCount(int inworld_x, int inworld_y, int ingeometry, double inxdiffuse, double inydiffuse,
                                   double inxgravity, double inygravity)
  : grid(inworld_x * inworld_y), m_initial(0.0)
{
  int i;
 
  xdiffuse = inxdiffuse;
  ydiffuse = inydiffuse;
  xgravity = inxgravity;
  ygravity = inygravity;
  world_x = inworld_x;
  world_y = inworld_y;
  geometry = ingeometry;
  num_cells = world_x * world_y;
  for (i = 0; i < GetSize(); i++) {
    cSpatialCountElem tmpelem;
    grid[i] = tmpelem;
  } 
  SetPointers();
}

/* Setup a single spatial resource using default flow amounts  */

cSpatialResCount::cSpatialResCount(int inworld_x, int inworld_y, int ingeometry)
  : grid(inworld_x * inworld_y), m_initial(0.0)
{
  int i;
 
  xdiffuse = 1.0;
  ydiffuse = 1.0;
  xgravity = 0.0;
  ygravity = 0.0;
  world_x = inworld_x;
  world_y = inworld_y;
  geometry = ingeometry;
  num_cells = world_x * world_y;
  for (i = 0; i < GetSize(); i++) {
    cSpatialCountElem tmpelem;
    grid[i] = tmpelem;
   } 
   SetPointers();
}

cSpatialResCount::cSpatialResCount() : m_initial(0.0), xdiffuse(1.0), ydiffuse(1.0), xgravity(0.0), ygravity(0.0)
{
  geometry = nGeometry::GLOBAL;
}


void cSpatialResCount::ResizeClear(int inworld_x, int inworld_y, int ingeometry)
{
  int i;
 
  grid.ResizeClear(inworld_x * inworld_y); 
  world_x = inworld_x;
  world_y = inworld_y;
  geometry = ingeometry;
  num_cells = world_x * world_y;
  for (i = 0; i < GetSize(); i++) {
    cSpatialCountElem tmpelem;
    grid[i] = tmpelem;
   } 
   SetPointers();
}

void cSpatialResCount::SetPointers()
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


void cSpatialResCount::CheckRanges()
{

  // Check that the x, y ranges of the inflow and outflow rectangles 
  // are valid

  /* check range of inputs */

  if (inflowX1 < 0) { 
    inflowX1 = 0; 
  } else if (inflowX1 > world_x) { 
    inflowX1 = world_x; 
  }
  if (inflowX2 < 0) { 
     inflowX2 = 0; 
  } else if (inflowX2 > world_x) { 
     inflowX2 = world_x; 
  }
  if (inflowY1 < 0) { 
    inflowY1 = 0; 
  } else if (inflowY1 > world_y) { 
    inflowY1 = world_y; 
  }
  if (inflowY2 < 0) { 
    inflowY2 = 0; 
  } else if (inflowY2 > world_y) { 
    inflowY2 = world_y; 
  }

  /* allow for rectangles that cross over the zero X or zero Y boundry */

  if (inflowX2 < inflowX1) { inflowX2 += world_x; }
  if (inflowY2 < inflowY1) { inflowY2 += world_y; }

  if (outflowX1 < 0) { 
    outflowX1 = 0; 
  } else if (outflowX1 > world_x) { 
    outflowX1 = world_x; 
  }
  if (outflowX2 < 0) { 
     outflowX2 = 0; 
  } else if (outflowX2 > world_x) { 
     outflowX2 = world_x; 
  }
  if (outflowY1 < 0) { 
    outflowY1 = 0; 
  } else if (outflowY1 > world_y) { 
    outflowY1 = world_y; 
  }
  if (outflowY2 < 0) { 
    outflowY2 = 0; 
  } else if (outflowY2 > world_y) { 
    outflowY2 = world_y; 
  }

  /* allow for rectangles that cross over the zero X or zero Y boundry */

  if (outflowX2 < outflowX1) { outflowX2 += world_x; }
  if (outflowY2 < outflowY1) { outflowY2 += world_y; }

}

/* Set all the individual cells to their initial values */
void cSpatialResCount::SetCellList(tArray<cCellResource>* in_cell_list_ptr)
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

void cSpatialResCount::Rate(int x, double ratein) const {
  if (x >= 0 && x < grid.GetSize()) {
    grid[x].Rate(ratein);
  } else {
    assert(false); // x not valid id
  }
}

/* Set the rate variable for one element using the x,y coordinate */

void cSpatialResCount::Rate(int x, int y, double ratein) const { 
  if (x >= 0 && x < world_x && y>= 0 && y < world_y) {
    grid[y * world_x + x].Rate(ratein);
  } else {
    assert(false); // x or y not valid id
  }
}

/* Fold the rate variable into the resource state for one element using 
   the array index */
   
void cSpatialResCount::State(int x) { 
  if (x >= 0 && x < grid.GetSize()) {
    grid[x].State();
  } else {
    assert(false); // x not valid id
  }
}

/* Fold the rate variable into the resource state for one element using 
   the x,y coordinate */
   
void cSpatialResCount::State(int x, int y) { 
  if (x >= 0 && x < world_x && y >= 0 && y < world_y) {
    grid[y*world_x + x].State();
  } else {
    assert(false); // x or y not valid id
  }
}

/* Get the state of one element using the array index */

double cSpatialResCount::GetAmount(int x) const { 
  if (x >= 0 && x < grid.GetSize()) {
    return grid[x].GetAmount(); 
  } else {
    return -99.9;
  }
}

/* Get the state of one element using the the x,y coordinate */

double cSpatialResCount::GetAmount(int x, int y) const { 
  if (x >= 0 && x < world_x && y >= 0 && y < world_y) {
    return grid[y*world_x + x].GetAmount(); 
  } else {
    return -99.9;
  }
}

void cSpatialResCount::RateAll(double ratein) {

  int i;
 
  for (i = 0; i < num_cells; i++) {
    grid[i].Rate(ratein);
  } 
}

/* For each cell in the grid add the changes stored in the rate variable
   with the total of the resource */

void cSpatialResCount::StateAll() {

  int i;
 
  for (i = 0; i < num_cells; i++) {
    grid[i].State();
  } 
}

void cSpatialResCount::FlowAll() {

  // @JEB save time if diffusion and gravity off...
  if ((xdiffuse == 0.0) && (ydiffuse == 0.0) && (xgravity == 0.0) && (ygravity == 0.0)) return;

  int     i,k,ii,xdist,ydist;
  double  dist;
 
  for (i = 0; i < num_cells; i++) {
      
    /* because flow is two way we must check only half the neighbors to 
       prevent double flow calculations */

    for (k = 3; k <= 6; k++) {
      ii = grid[i].GetElemPtr(k);
      xdist = grid[i].GetPtrXdist(k);
      ydist = grid[i].GetPtrYdist(k);
      dist = grid[i].GetPtrDist(k);
      if (ii >= 0) {
        FlowMatter(grid[i],grid[ii],xdiffuse,ydiffuse,xgravity,ygravity,
                   xdist, ydist, dist);
      }
    }
  }
}

/* Total up all the resources in each cell */

double cSpatialResCount::SumAll() const{

  int i;
  double sum = 0.0;

  for (i = 0; i < num_cells; i++) {
    sum += GetAmount(i);
  } 
  return sum;
}

/* Take a given amount of resource and spread it among all the cells in the 
   inflow rectange */

void cSpatialResCount::Source(double amount) const {
  int     i, j, elem;
  double  totalcells;

  totalcells = (inflowY2 - inflowY1 + 1) * (inflowX2 - inflowX1 + 1) * 1.0;
  amount /= totalcells;

  for (i = inflowY1; i <= inflowY2; i++) {
    for (j = inflowX1; j <= inflowX2; j++) {
      elem = (Mod(i,world_y) * world_x) + Mod(j,world_x);
      Rate(elem,amount); 
    }
  }
}

/* Handle the inflow for a list of individual cells */

void cSpatialResCount::CellInflow() const {
  for (int i=0; i < cell_list_ptr->GetSize(); i++) {
    const int cell_id = (*cell_list_ptr)[i].GetId();
    
    /* Be sure the user entered a valid cell id or if the the program is loading
       the resource for the testCPU that does not have a grid set up */
       
    if (cell_id >= 0 && cell_id < grid.GetSize()) {
      Rate(cell_id, (*cell_list_ptr)[i].GetInflow());
    }
  }
}

/* Take away a give percentage of a resource from outflow rectangle */

void cSpatialResCount::Sink(double decay) const {

  int     i, j, elem;
  double  deltaamount;


  for (i = outflowY1; i <= outflowY2; i++) {
    for (j = outflowX1; j <= outflowX2; j++) {
      elem = (Mod(i,world_y) * world_x) + Mod(j,world_x);
      deltaamount = Max((GetAmount(elem) * (1.0 - decay)), 0.0);
      Rate(elem,-deltaamount); 
    }
  }
}

/* Take away a give percentage of a resource from individual cells */

void cSpatialResCount::CellOutflow() const {

  double deltaamount = 0.0;

  for (int i=0; i < cell_list_ptr->GetSize(); i++) {
    const int cell_id = (*cell_list_ptr)[i].GetId();
    
    /* Be sure the user entered a valid cell id or if the the program is loading
       the resource for the testCPU that does not have a grid set up */
       
    if (cell_id >= 0 && cell_id < grid.GetSize()) {
      deltaamount = Max((GetAmount(cell_id) *
                         (*cell_list_ptr)[i].GetOutflow()), 0.0);
    }                     
    Rate((*cell_list_ptr)[i].GetId(), -deltaamount); 
  }
}

void cSpatialResCount::SetCellAmount(int cell_id, double res)
{
  if (cell_id >= 0 && cell_id < grid.GetSize())
  {
    Element(cell_id).SetAmount(res);
  }
}


void cSpatialResCount::ResetResourceCounts()
{
  for (int i = 0; i < grid.GetSize(); i++) grid[i].ResetResourceCount(m_initial);
}
