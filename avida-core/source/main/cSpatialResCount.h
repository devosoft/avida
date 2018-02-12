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

#include "cAvidaContext.h"
#include "cSpatialCountElem.h"
#include "cResource.h"


class cSpatialResCount
{

private:

  
  
  Apto::Array<cSpatialCountElem> grid;
  double m_initial;
  double xdiffuse, ydiffuse;
  double xgravity, ygravity;
  int    inflowX1, inflowX2, inflowY1, inflowY2;
  int    outflowX1, outflowX2, outflowY1, outflowY2;
  int    geometry;
  int    world_x, world_y, num_cells;
  int    curr_peakx, curr_peaky;
  /* instead of creating a new array use the existing one from cResource */
  Apto::Array<cCellResource> *cell_list_ptr;
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
  void SetCellList(Apto::Array<cCellResource> *in_cell_list_ptr);
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
  
  virtual void SetGradInitialPlat(double) { ; }
  virtual void SetGradPeakX(int) { ; }
  virtual void SetGradPeakY(int) { ; }
  virtual void SetGradHeight(int) { ; }
  virtual void SetGradSpread(int) { ; }
  virtual void SetGradPlateau(double) { ; }
  virtual void SetGradDecay(int) { ; }
  virtual void SetGradMaxX(int) { ; }
  virtual void SetGradMaxY(int) { ; }
  virtual void SetGradMinX(int) { ; }
  virtual void SetGradMinY(int) { ; }
  virtual void SetGradMoveScaler(double) { ; }
  virtual void SetGradUpdateStep(int) { ; }
  virtual void SetGradIsHalo(bool) { ; }
  virtual void SetGradHaloInnerRad(int) { ; }
  virtual void SetGradHaloWidth(int) { ; }
  virtual void SetGradHaloX(int) { ; }
  virtual void SetGradHaloY(int) { ; }
  virtual void SetGradMoveSpeed(int) { ; }
  virtual void SetGradMoveResistance(int) { ; }
  virtual void SetGradPlatInflow(double) { ; }
  virtual void SetGradPlatOutflow(double) { ; }
  virtual void SetGradConeInflow(double) { ; }
  virtual void SetGradConeOutflow(double) { ; }
  virtual void SetGradientInflow(double) { ; }
  virtual void SetGradPlatVarInflow(cAvidaContext& ctx, double, double, int) { ; }
  virtual void SetPredatoryResource(double, int) { ; }
  virtual void SetProbabilisticResource(cAvidaContext& ctx, double, double, double, double, double, int, int, int) { ; }
  virtual void SetGradPlatIsCommon(bool) { ; }
  virtual void SetGradFloor(double) { ; }
  virtual void SetGradHabitat(int) { ; }
  virtual void SetGradMinSize(int) { ; }
  virtual void SetGradMaxSize(int) { ; }
  virtual void SetGradConfig(int) { ; }
  virtual void SetGradCount(int) { ; }
  virtual void SetGradResistance(double) { ; }
  virtual void SetGradThreshold(double) { ; }
  virtual void SetGradRefuge(int) { ; }
  virtual void SetGradDamage(double) { ; }
  virtual void SetGradDeathOdds(double) { ; }
  virtual void SetGradIsPath(bool) { ; }
  virtual void SetGradIsHammer(bool) { ; }

  virtual void ResetGradRes(cAvidaContext& ctx, int worldx, int worldy) { (void)ctx, (void)worldx, (void)worldy; }
  
  void SetCurrPeakX(int in_curr_x) { curr_peakx = in_curr_x; }
  void SetCurrPeakY(int in_curr_y) { curr_peaky = in_curr_y; }
  int GetCurrPeakX() { return curr_peakx; } 
  int GetCurrPeakY() { return curr_peaky; }
  
  virtual Apto::Array<int>* GetWallCells() { return NULL; }
  virtual int GetMinUsedX() { return -1; }
  virtual int GetMinUsedY() { return -1; }
  virtual int GetMaxUsedX() { return -1; }
  virtual int GetMaxUsedY() { return -1; }
};

#endif
