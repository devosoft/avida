/*
 *  cDiffusionRes.h
 *  Avida
 *
 *  Called "SpatialResCount" prior to 01/18/13.
 *  Called "spatial_res_count.hh" prior to 12/5/05.
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

/*! Class to keep track of amounts of localized resources. */

#ifndef cDiffusionRes_h
#define cDiffusionRes_h

#include "cResource.h"

class cDiffusionRes : public cResource
{
private:
  double xdiffuse, ydiffuse;
  double xgravity, ygravity;
  int    inflowX1, inflowX2, inflowY1, inflowY2;
  int    outflowX1, outflowX2, outflowY1, outflowY2;
  int    curr_peakx, curr_peaky;
  /* instead of creating a new array use the existing one from cResource */
  Apto::Array<cCellResource> *cell_list_ptr;
  
  void FlowMatter(cResourceElement&, cResourceElement&, double, double, double, double, int, int, double);

public:
  cDiffusionRes();
  cDiffusionRes(double inxdiffuse, double inydiffuse, double inxgravity, double inygravity);
  virtual ~cDiffusionRes();
  
  void SetPointers();
  void CheckRanges();
  void SetCellList(Apto::Array<cCellResource> *in_cell_list_ptr);
  int GetCellListSize() const { return cell_list_ptr->GetSize(); }

  void Rate(int x, double ratein) const;
  void Rate(int x, int y, double ratein) const;
  void State(int x);
  void State(int x, int y);
  void RateAll(double ratein); 
  void StateAll();
  void FlowAll(); 
  double SumAll() const;
  void Source(double amount) const;
  void CellInflow() const;
  void Sink(double percent) const;
  void CellOutflow() const;

  void SetXdiffuse(double in_xdiffuse) { xdiffuse = in_xdiffuse; }
  void SetXgravity(double in_xgravity) { xgravity = in_xgravity; }
  void SetYdiffuse(double in_ydiffuse) { ydiffuse = in_ydiffuse; }
  void SetYgravity(double in_ygravity) { ygravity = in_ygravity; }
  void SetInflowX1(int in_inflowX1) { inflowX1 = in_inflowX1; }
  void SetInflowX2(int in_inflowX2) { inflowX2 = in_inflowX2; }
  void SetInflowY1(int in_inflowY1) { inflowY1 = in_inflowY1; }
  void SetInflowY2(int in_inflowY2) { inflowY2 = in_inflowY2; }
  void SetOutflowX1(int in_outflowX1) { outflowX1 = in_outflowX1; }
  void SetOutflowX2(int in_outflowX2) { outflowX2 = in_outflowX2; }
  void SetOutflowY1(int in_outflowY1) { outflowY1 = in_outflowY1; }
  void SetOutflowY2(int in_outflowY2) { outflowY2 = in_outflowY2; }
  void ResetResourceCounts();
};

#endif
