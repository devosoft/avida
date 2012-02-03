/*
 *  cSpatialResCount.h
 *  Avida
 *
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

#ifndef cSpatialResCount_h
#define cSpatialResCount_h

#ifndef cAvidaContext_h
#include "cAvidaContext.h"
#endif

#ifndef cSpatialCountElem_h
#include "cSpatialCountElem.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

#ifndef cResource_h
#include "cResource.h"
#endif

class cSpatialResCount
{
private:
  tArray<cSpatialCountElem> grid;
  double m_initial;
  double xdiffuse, ydiffuse;
  double xgravity, ygravity;
  int    inflowX1, inflowX2, inflowY1, inflowY2;
  int    outflowX1, outflowX2, outflowY1, outflowY2;
  int    geometry;
  int    world_x, world_y, num_cells;
  int    curr_peakx, curr_peaky;
  /* instead of creating a new array use the existing one from cResource */
  tArray<cCellResource> *cell_list_ptr;
  bool m_modified;
  
public:
  cSpatialResCount();
  cSpatialResCount(int inworld_x, int inworld_y, int ingeometry);
  cSpatialResCount(int inworld_x, int inworld_y, int ingeometry, 
                   double inxdiffuse, double inydiffuse,
                   double inxgravity, double inygravity);
  virtual ~cSpatialResCount();
  
  void ResizeClear(int inworld_x, int inworld_y, int ingeometry);
  void SetPointers();
  void CheckRanges();
  void SetCellList(tArray<cCellResource> *in_cell_list_ptr);
  int GetSize() const { return grid.GetSize(); }
  int GetX() const { return world_x; }
  int GetY() const { return world_y; }
  int GetCellListSize() const { return cell_list_ptr->GetSize(); }
  cSpatialCountElem& Element(int x) { return grid[x]; }
  void Rate(int x, double ratein) const;
  void Rate(int x, int y, double ratein) const;
  void State(int x);
  void State(int x, int y);
  double GetAmount(int x) const;
  double GetAmount(int x, int y) const;
  void RateAll(double ratein); 
  virtual void StateAll();
  void FlowAll(); 
  double SumAll() const;
  void Source(double amount) const;
  void CellInflow() const;
  void Sink(double percent) const;
  void CellOutflow() const;
  void SetCellAmount(int cell_id, double res);
  void SetInitial(double initial) { m_initial = initial; }
  double GetInitial() const { return m_initial; }
  void SetGeometry(int in_geometry) { geometry = in_geometry; }
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
  virtual void UpdateCount(cAvidaContext&) { ; }
  void ResetResourceCounts();
  void SetModified(bool in_modified) { m_modified = in_modified; }
  bool GetModified() { return m_modified; }
  
  virtual void SetGradInitialPlatVal(double plat_val) {}
  virtual void SetGradPeakX(int peakx) {}
  virtual void SetGradPeakY(int peaky) {}
  virtual void SetGradHeight(int height) {}
  virtual void SetGradSpread(int spread) {}
  virtual void SetGradPlateau(double plateau) {}
  virtual void SetGradDecay(int decay) {}
  virtual void SetGradMaxX(int max_x) {}
  virtual void SetGradMaxY(int max_y) {}
  virtual void SetGradMinX(int min_x) {}
  virtual void SetGradMinY(int min_y) {}
  virtual void SetGradMoveScaler(double move_a_scaler) {}
  virtual void SetGradUpdateStep(int updatestep) {}
  virtual void SetGradIsHalo(bool halo) {}
  virtual void SetGradHaloInnerRad(int halo_inner_radius) {}
  virtual void SetGradHaloWidth(int halo_width) {}
  virtual void SetGradHaloX(int halo_anchor_x) {}
  virtual void SetGradHaloY(int halo_anchor_y) {}
  virtual void SetGradMoveSpeed(int move_speed) {}
  virtual void SetGradPlatInflow(double plateau_inflow) {}
  virtual void SetGradPlatOutflow(double plateau_outflow) {}
  virtual void SetGradPlatIsCommon(bool is_plateau_common) {}
  virtual void SetGradFloor(double floor) {}
  virtual void SetGradHabitat(int habitat) {}
  virtual void SetGradMinSize(int min_size) {}
  virtual void SetGradMaxSize(int max_size) {}
  virtual void SetGradConfig(int config) {}
  virtual void SetGradCount(int count) {}
  virtual void SetGradResistance(double resistance) {}
  virtual void SetGradThreshold(double threshold) {}
  virtual void SetGradRefuge(int refuge) {}

  virtual void ResetGradRes(cAvidaContext& ctx, int worldx, int worldy) {}
  
  void SetCurrPeakX(int in_curr_x) { curr_peakx = in_curr_x; }
  void SetCurrPeakY(int in_curr_y) { curr_peaky = in_curr_y; }
  int GetCurrPeakX() { return curr_peakx; } 
  int GetCurrPeakY() { return curr_peaky; }
  
};

#endif
