/*
 *  cSpatialCountElem.cc
 *  Avida
 *
 *  Called "spatial_count_elem.cc" prior to 12/5/05.
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

/* Class to store information about amount of local/spatial resource in a
   given cell. */

#include "cSpatialCountElem.h"

const int MAXFLOWPTS = 8;


/* Intial spatial resource count elements with an initial amount */

cSpatialCountElem::cSpatialCountElem(double initamount)
{
  amount = initamount;
  delta = 0.0;
  initial = initamount;
  elempt.Resize(MAXFLOWPTS);
  xdist.Resize(MAXFLOWPTS);
  ydist.Resize(MAXFLOWPTS);
  dist.Resize(MAXFLOWPTS);
}

/* Intial spatial resource count elements without an initial amount */

cSpatialCountElem::cSpatialCountElem()
{
  amount = 0.0;
  delta = 0.0;
  initial = 0.0;
  elempt.Resize(MAXFLOWPTS);
  xdist.Resize(MAXFLOWPTS);
  ydist.Resize(MAXFLOWPTS);
  dist.Resize(MAXFLOWPTS);
}

/* Set pointer to one other elements from the current element */  

void cSpatialCountElem::SetPtr(int innum, int inelempt, int inxdist, int inydist, double indist)
{
  elempt[innum] = inelempt;
  xdist[innum] = inxdist;
  ydist[innum] = inydist;
  dist[innum] = indist;
}
