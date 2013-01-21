/*
 *  cDiffusionRes.cc
 *  Avida
 *
 *  Called "SpatialResCount" prior to 01/18/13.
 *  Called "spatial_res_count.cc" prior to 12/5/05.
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

#include "cDiffusionRes.h"

#include "AvidaTools.h"
#include "nGeometry.h"

#include <cmath>

using namespace AvidaTools;

/* Setup a single diffusion resource with known flows */

cDiffusionRes::cDiffusionRes(double inxdiffuse, double inydiffuse, double inxgravity, double inygravity)
{
  xdiffuse = inxdiffuse;
  ydiffuse = inydiffuse;
  xgravity = inxgravity;
  ygravity = inygravity;
  SetPointers();
}

/* Setup a single diffusion resource using default flow amounts  */

cDiffusionRes::cDiffusionRes()
{
  xdiffuse = 1.0;
  ydiffuse = 1.0;
  xgravity = 0.0;
  ygravity = 0.0;
  SetPointers();
}

cDiffusionRes::~cDiffusionRes() { ; }


void cDiffusionRes::ResizeClear(int x_size, int y_size, int geometry)
{
  int i;
 
  GetElements().ResizeClear(x_size * y_size);
  SetGeometry(geometry);
  for (i = 0; i < GetSize(); i++) {
    cResourceElement tmpelem;
    Element(i) = tmpelem;
   } 
   SetPointers();
}

void cDiffusionRes::SetPointers()
{
  /* Pointer 0 will point to the cell above and to the left the current cell
     and will go clockwise around the cell.                               */

  int     i,ii;
  double  SQRT2 = sqrt(2.0);

  /* First treat all cells like they are in a torus */

  for (i = 0; i < GetSize(); i++) {
    Element(i).SetPtr(0 ,GridNeighbor(i, GetX(), GetY(), -1, -1), -1, -1, SQRT2);
    Element(i).SetPtr(1 ,GridNeighbor(i, GetX(), GetY(),  0, -1),  0, -1, 1.0);
    Element(i).SetPtr(2 ,GridNeighbor(i, GetX(), GetY(), +1, -1), +1, -1, SQRT2);
    Element(i).SetPtr(3 ,GridNeighbor(i, GetX(), GetY(), +1,  0), +1,  0, 1.0);
    Element(i).SetPtr(4 ,GridNeighbor(i, GetX(), GetY(), +1, +1), +1, +1, SQRT2);
    Element(i).SetPtr(5 ,GridNeighbor(i, GetX(), GetY(),  0, +1),  0, +1, 1.0);
    Element(i).SetPtr(6 ,GridNeighbor(i, GetX(), GetY(), -1, +1), -1, +1, SQRT2);
    Element(i).SetPtr(7 ,GridNeighbor(i, GetX(), GetY(), -1,  0), -1,  0, 1.0);
  }
 
  /* Fix links for top, bottom and sides for non-torus */
  
  if (GetGeometry() == nGeometry::GRID) {
    /* Top and bottom */

    for (i = 0; i < GetX(); i++) {
      Element(i).SetPtr(0, -99, -99, -99, -99.0);
      Element(i).SetPtr(1, -99, -99, -99, -99.0);
      Element(i).SetPtr(2, -99, -99, -99, -99.0);
      ii = GetSize()-1-i;
      Element(ii).SetPtr(4, -99, -99, -99, -99.0);
      Element(ii).SetPtr(5, -99, -99, -99, -99.0);
      Element(ii).SetPtr(6, -99, -99, -99, -99.0);
    }

    /* fix links for right and left sides */

    for (i = 0; i < GetY(); i++) {
      ii = i * GetX();
      Element(ii).SetPtr(0, -99, -99, -99, -99.0);
      Element(ii).SetPtr(7, -99, -99, -99, -99.0);
      Element(ii).SetPtr(6, -99, -99, -99, -99.0);
      ii = ((i + 1) * GetX()) - 1;
      Element(ii).SetPtr(2, -99, -99, -99, -99.0);
      Element(ii).SetPtr(3, -99, -99, -99, -99.0);
      Element(ii).SetPtr(4, -99, -99, -99, -99.0);
    }
  }
}


void cDiffusionRes::CheckRanges()
{

  // Check that the x, y ranges of the inflow and outflow rectangles 
  // are valid

  /* check range of inputs */

  if (inflowX1 < 0) { 
    inflowX1 = 0; 
  } else if (inflowX1 > GetX()) { 
    inflowX1 = GetX(); 
  }
  if (inflowX2 < 0) { 
     inflowX2 = 0; 
  } else if (inflowX2 > GetX()) { 
     inflowX2 = GetX();
  }
  if (inflowY1 < 0) { 
    inflowY1 = 0; 
  } else if (inflowY1 > GetY()) { 
    inflowY1 = GetY(); 
  }
  if (inflowY2 < 0) { 
    inflowY2 = 0; 
  } else if (inflowY2 > GetY()) { 
    inflowY2 = GetY(); 
  }

  /* allow for rectangles that cross over the zero X or zero Y boundry */

  if (inflowX2 < inflowX1) { inflowX2 += GetX(); }
  if (inflowY2 < inflowY1) { inflowY2 += GetY(); }

  if (outflowX1 < 0) { 
    outflowX1 = 0; 
  } else if (outflowX1 > GetX()) { 
    outflowX1 = GetX(); 
  }
  if (outflowX2 < 0) { 
     outflowX2 = 0; 
  } else if (outflowX2 > GetX()) { 
     outflowX2 = GetX(); 
  }
  if (outflowY1 < 0) { 
    outflowY1 = 0; 
  } else if (outflowY1 > GetY()) { 
    outflowY1 = GetY(); 
  }
  if (outflowY2 < 0) { 
    outflowY2 = 0; 
  } else if (outflowY2 > GetY()) { 
    outflowY2 = GetY(); 
  }

  /* allow for rectangles that cross over the zero X or zero Y boundry */

  if (outflowX2 < outflowX1) { outflowX2 += GetX(); }
  if (outflowY2 < outflowY1) { outflowY2 += GetY(); }

}

/* Set all the individual cells to their initial values */
void cDiffusionRes::SetCellList(Apto::Array<cCellResource>* in_cell_list_ptr)
{
  cell_list_ptr = in_cell_list_ptr;
  for (int i = 0; i < cell_list_ptr->GetSize(); i++) {
    int cell_id = (*cell_list_ptr)[i].GetId();
    
    /* Be sure the user entered a valid cell id or if the the program is loading
       the resource for the testCPU that does not have a grid set up */
       
    if (cell_id >= 0 && cell_id <= GetElements().GetSize()) {
      Rate((*cell_list_ptr)[i].GetId(), (*cell_list_ptr)[i].GetInitial());
      State((*cell_list_ptr)[i].GetId());
      Element(cell_id).SetInitial((*cell_list_ptr)[i].GetInitial());
    }
  }
}

/* Set the rate variable for one element using the array index */

void cDiffusionRes::Rate(int x, double ratein) const {
  if (x >= 0 && x < GetSize()) {
    RateElement(x, ratein);
  } else {
    assert(false); // x not valid id
  }
}

/* Set the rate variable for one element using the x,y coordinate */

void cDiffusionRes::Rate(int x, int y, double ratein) const {
  if (x >= 0 && x < GetX() && y>= 0 && y < GetY()) {
    RateElement(y * GetX() + x, ratein);
  } else {
    assert(false); // x or y not valid id
  }
}

/* Fold the rate variable into the resource state for one element using 
   the array index */
   
void cDiffusionRes::State(int x) {
  if (x >= 0 && x < GetElements().GetSize()) {
    Element(x).State();
  } else {
    assert(false); // x not valid id
  }
}

/* Fold the rate variable into the resource state for one element using 
   the x,y coordinate */
   
void cDiffusionRes::State(int x, int y) {
  if (x >= 0 && x < GetX() && y >= 0 && y < GetY()) {
    Element(y*GetX() + x).State();
  } else {
    assert(false); // x or y not valid id
  }
}

void cDiffusionRes::RateAll(double ratein) {

  int i;
 
  for (i = 0; i < GetSize(); i++) {
    Element(i).Rate(ratein);
  } 
}

/* For each cell in the grid add the changes stored in the rate variable
   with the total of the resource */

void cDiffusionRes::StateAll() {

  int i;
 
  for (i = 0; i < GetSize(); i++) {
    Element(i).State();
  } 
}

void cDiffusionRes::FlowAll() {

  // @JEB save time if diffusion and gravity off...
  if ((xdiffuse == 0.0) && (ydiffuse == 0.0) && (xgravity == 0.0) && (ygravity == 0.0)) return;

  int     i,k,ii,xdist,ydist;
  double  dist;
 
  for (i = 0; i < GetSize(); i++) {
      
    /* because flow is two way we must check only half the neighbors to 
       prevent double flow calculations */

    for (k = 3; k <= 6; k++) {
      ii = Element(i).GetElemPtr(k);
      xdist = Element(i).GetPtrXdist(k);
      ydist = Element(i).GetPtrYdist(k);
      dist = Element(i).GetPtrDist(k);
      if (ii >= 0) {
        FlowMatter(Element(i),Element(ii),xdiffuse,ydiffuse,xgravity,ygravity,
                   xdist, ydist, dist);
      }
    }
  }
}

void FlowMatter(cResourceElement &elem1, cResourceElement &elem2,
                double inxdiffuse, 
                double inydiffuse, double inxgravity, double inygravity,
                int xdist, int ydist, double dist) {

  /* Routine to calculate the amount of flow from one Element to another.
     Amount of flow is a function of:

       1) Amount of material in each cell (will try to equalize)
       2) Distance between each cell
       3) x and y "gravity"

     This method only effect the delta amount of each element.  The State
     method will need to be called at the end of each time step to complete
     the movement of material.
  */

  double  diff, flowamt, xgravity, xdiffuse, ygravity,  ydiffuse;

  if (((elem1.GetAmount() == 0.0) && (elem2.GetAmount() == 0.0)) && (dist < 0.0)) return;
  diff = (elem1.GetAmount() - elem2.GetAmount());
  if (xdist != 0) {

    /* if there is material to be effected by x gravity */

    if (((xdist>0) && (inxgravity>0.0)) || ((xdist<0) && (inxgravity<0.0))) {
      xgravity = elem1.GetAmount() * fabs(inxgravity)/3.0;
    } else {
      xgravity = -elem2.GetAmount() * fabs(inxgravity)/3.0;
    }
    
    /* Diffusion uses the diffusion constant x half the difference (as the 
       elements attempt to equalize) / the number of possible neighbors (8) */

    xdiffuse = inxdiffuse * diff / 16.0;
  } else {
    xdiffuse = 0.0;
    xgravity = 0.0;
  }  
  if (ydist != 0) {

    /* if there is material to be effected by y gravity */

    if (((ydist>0) && (inygravity>0.0)) || ((ydist<0) && (inygravity<0.0))) {
      ygravity = elem1.GetAmount() * fabs(inygravity)/3.0;
    } else {
      ygravity = -elem2.GetAmount() * fabs(inygravity)/3.0;
    }
    ydiffuse = inydiffuse * diff / 16.0;
  } else {
    ydiffuse = 0.0;
    ygravity = 0.0;
  }  

  flowamt = ((xdiffuse + ydiffuse + xgravity + ygravity)/
             (fabs(xdist*1.0) + fabs(ydist*1.0)))/dist;
  elem1.Rate(-1 * flowamt);
  elem2.Rate(flowamt);
}

/* Total up all the resources in each cell */

double cDiffusionRes::SumAll() const{

  int i;
  double sum = 0.0;

  for (i = 0; i < GetSize(); i++) {
    sum += GetAmount(i);
  } 
  return sum;
}

/* Take a given amount of resource and spread it among all the cells in the 
   inflow rectange */

void cDiffusionRes::Source(double amount) const {
  int     i, j, elem;
  double  totalcells;

  totalcells = (inflowY2 - inflowY1 + 1) * (inflowX2 - inflowX1 + 1) * 1.0;
  amount /= totalcells;

  for (i = inflowY1; i <= inflowY2; i++) {
    for (j = inflowX1; j <= inflowX2; j++) {
      elem = (Mod(i,GetY()) * GetX()) + Mod(j,GetX());
      Rate(elem,amount); 
    }
  }
}

/* Handle the inflow for a list of individual cells */

void cDiffusionRes::CellInflow() const {
  for (int i=0; i < cell_list_ptr->GetSize(); i++) {
    const int cell_id = (*cell_list_ptr)[i].GetId();
    
    /* Be sure the user entered a valid cell id or if the the program is loading
       the resource for the testCPU that does not have a grid set up */
       
    if (cell_id >= 0 && cell_id < GetSize()) {
      Rate(cell_id, (*cell_list_ptr)[i].GetInflow());
    }
  }
}

/* Take away a give percentage of a resource from outflow rectangle */

void cDiffusionRes::Sink(double decay) const {

  int     i, j, elem;
  double  deltaamount;

  if (outflowX1 == -99 || outflowY1 == -99 || outflowX2 == -99 || outflowY2 == -99) return;
  
  for (i = outflowY1; i <= outflowY2; i++) {
    for (j = outflowX1; j <= outflowX2; j++) {
      elem = (Mod(i,GetY()) * GetX()) + Mod(j,GetX());
      deltaamount = Apto::Max((GetAmount(elem) * (1.0 - decay)), 0.0);
      Rate(elem,-deltaamount); 
    }
  }
}

/* Take away a give percentage of a resource from individual cells */

void cDiffusionRes::CellOutflow() const {

  double deltaamount = 0.0;

  for (int i=0; i < cell_list_ptr->GetSize(); i++) {
    const int cell_id = (*cell_list_ptr)[i].GetId();
    
    /* Be sure the user entered a valid cell id or if the the program is loading
       the resource for the testCPU that does not have a grid set up */
       
    if (cell_id >= 0 && cell_id < GetSize()) {
      deltaamount = Apto::Max((GetAmount(cell_id) * (*cell_list_ptr)[i].GetOutflow()), 0.0);
    }                     
    Rate((*cell_list_ptr)[i].GetId(), -deltaamount); 
  }
}

void cDiffusionRes::ResetResourceCounts()
{
  for (int i = 0; i < GetSize(); i++) Element(i).ResetResourceCount(GetInitial());
}
