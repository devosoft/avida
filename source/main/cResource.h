/*
 *  cResource.h
 *  Avida
 *
 *  Called "cResourceCount.h" prior to 01/17/13.
 *  Called "resource_count.hh" prior to 12/5/05.
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

#ifndef cResource_h
#define cResource_h

#include "cEnvironment.h"
#include "cResourceElement.h"

class cResourceDef;

class cResource
{
private:
  cWorld* m_world;
  cString m_name;
  int m_id;
  Apto::Array<cResourceElement> grid;
  int geometry;
  int world_x, world_y, num_cells;
  double m_initial;
  bool m_modified;

public:
  cResource();
  cResource(const cString& _name, int _id);
  cResource(const cString& _name, int _id, int inworld_x, int inworld_y, int ingeometry);
  cResource(int inworld_x, int inworld_y, int ingeometry);
  const cResource& operator=(const cResource &resource);
  
  virtual ~cResource() = 0;

  void ResizeClear(int size_x, int size_y, int geometry);

  inline cResourceDef* GetResDef() { return m_world->GetEnvironment().GetResDefLib().GetResDef(m_id); }
  
  cResourceElement& Element(int x) { return grid[x]; }
  Apto::Array<cResourceElement>& GetElements() { return grid; }
  double GetAmount(int x) const;
  double GetAmount(int x, int y) const;
  int GetSize() const { return grid.GetSize(); }
  int GetX() const { return world_x; }
  int GetY() const { return world_y; }
  double GetInitial() const { return m_initial; }
  bool GetModified() { return m_modified; }
  int GetGeometry() { return geometry; }

  void SetCellAmount(int cell_id, double res);
  void SetGeometry(int in_geometry) { geometry = in_geometry; }
  void SetInitial(double initial) { m_initial = initial; }
  void SetModified(bool in_modified) { m_modified = in_modified; }

  void RateElement(int element, int rate) const { grid[element].Rate(rate); }

  // diffusion res only
  virtual double SumAll() const { return 0; }
  virtual int GetCellListSize() const { return 0; }

  virtual void SetCellList(Apto::Array<cCellResource> *in_cell_list_ptr) { ; }
  virtual void StateAll() { ; }
  virtual void Rate(int x, double ratein) const { ; }
  virtual void Rate(int x, int y, double ratein) const { ; }
  virtual void RateAll(double ratein) { ; }
  virtual void State(int x) { ; }
  virtual void State(int x, int y) { ; }
  virtual void Source(double amount) const { ; }
  virtual void Sink(double percent) const { ; }
  virtual void CellInflow() const { ; }
  virtual void CellOutflow() const { ; }
  virtual void FlowAll() { ; }
  virtual void ResetResourceCounts() { ; }

  virtual void SetXdiffuse(double in_xdiffuse) { ; }
  virtual void SetXgravity(double in_xgravity) { ; }
  virtual void SetYdiffuse(double in_ydiffuse) { ; }
  virtual void SetYgravity(double in_ygravity) { ; }
  virtual void SetInflowX1(int in_inflowX1) { ; }
  virtual void SetInflowX2(int in_inflowX2) { ; }
  virtual void SetInflowY1(int in_inflowY1) { ; }
  virtual void SetInflowY2(int in_inflowY2) { ; }
  virtual void SetOutflowX1(int in_outflowX1) { ; }
  virtual void SetOutflowX2(int in_outflowX2) { ; }
  virtual void SetOutflowY1(int in_outflowY1) { ; }
  virtual void SetOutflowY2(int in_outflowY2) { ; }
  virtual void SetPointers() { ; }
  
  // dynamic res only
  virtual int GetCurrPeakX() { return 0; }
  virtual int GetCurrPeakY() { return 0; }
  virtual Apto::Array<int>* GetWallCells() { return NULL; }
  virtual int GetMinUsedX() { return -1; }
  virtual int GetMinUsedY() { return -1; }
  virtual int GetMaxUsedX() { return -1; }
  virtual int GetMaxUsedY() { return -1; }

  virtual void UpdateDynamicRes(cAvidaContext& ctx) { ; }
  virtual void ResetDynamicRes(cAvidaContext& ctx, int worldx, int worldy) { ; }
  virtual void BuildProbabilisticRes(cAvidaContext& ctx, double lambda, double theta, int x, int y, int num_cells) { ; }
  virtual void SetPlatVarInflow(double mean, double variance, int type) { ; }
};
#endif
