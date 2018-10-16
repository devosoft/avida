/*
 *  cSpatialCountElem.h
 *  Avida
 *
 *  Called "spatial_count_elem.hh" prior to 12/5/05.
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
 *
 */

#ifndef cSpatialCountElem_h
#define cSpatialCountElem_h

#include "avida/core/Types.h"


class cSpatialCountElem
{
private:
  mutable double amount, delta, initial;  //@DJB how much have, delta - cache difference before a flow; what was initial for the run
  Apto::Array<int> elempt, xdist, ydist;  //each 8 elements long. these are the cells arround me
                                          // 0 1 2  elempt = cell ID at that location
                                          // 7 _ 3  xdist = how far away in x dim (-1, 0, 1)
                                          // 6 5 4  ydist = how far away in y dim (-1, 0, 1)
  Apto::Array<double> dist;    // dist sqrt(2), 1, or 0  0 is itself
                               // every thing about dist can be special values -99 or cResource::NONE = -99
                               // arbitrary numbar that means no neighbor in that position
  
public:
  cSpatialCountElem();
  cSpatialCountElem(double initamount);
  
  void Rate(double ratein) const { delta += ratein; }
  void State() { amount += delta; delta = 0.0; }
  double GetAmount() const { return amount; }
  void SetAmount(double res) const { amount = res; }
  void SetPtr(int innum, int inelempt, int inxdist, int  inydist, double indist);
  int GetElemPtr(int innum) { return elempt[innum]; }
  int GetPtrXdist(int innum) { return xdist[innum]; }
  int GetPtrYdist(int innum) { return ydist[innum]; }
  double GetPtrDist(int innum) { return dist[innum]; }
  friend void FlowMatter(cSpatialCountElem&, cSpatialCountElem&, double, double, double, double,
                         int, int, double);
  void SetInitial(double init) { initial = init; }
  double GetInitial() { return initial; }
  
  inline void ResetResourceCount(double res_initial) { amount = res_initial + initial; }
};

#endif
