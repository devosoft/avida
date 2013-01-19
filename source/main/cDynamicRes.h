/*
 *  cDynamicRes.h
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
 *
 */

/*! Class to keep track of amounts of localized resources. */

#ifndef cDynamicRes_h
#define cDynamicRes_h

#include "cAvidaContext.h"
#include "cResource.h"
#include "cResourceDef.h"
#include "cResourceElement.h"

class cDynamicRes : public cResource
{
private:
  Apto::Array<cResourceElement> grid;
  double m_initial;
  int    geometry;
  int    world_x, world_y, num_cells;
  int    curr_peakx, curr_peaky;
  /* instead of creating a new array use the existing one from cResource */
  Apto::Array<cCellResource> *cell_list_ptr;
  bool m_modified;
  
public:
  cDynamicRes();
  cDynamicRes(int inworld_x, int inworld_y, int ingeometry);
  virtual ~cDynamicRes();
  
  void UpdateDynamicRes(cAvidaContext&) { ; }

  cResourceElement& Element(int x) { return grid[x]; }
  int GetSize() const { return grid.GetSize(); }
  int GetX() const { return world_x; }
  int GetY() const { return world_y; }
  double GetAmount(int x) const;
  double GetAmount(int x, int y) const;
  double GetInitial() const { return m_initial; }
  bool GetModified() { return m_modified; }
  int GetCurrPeakX() { return curr_peakx; }
  int GetCurrPeakY() { return curr_peaky; }
  
  virtual Apto::Array<int>* GetWallCells();
  virtual int GetMinUsedX();
  virtual int GetMinUsedY();
  virtual int GetMaxUsedX();
  virtual int GetMaxUsedY();
  
  void SetCurrPeakX(int in_curr_x) { curr_peakx = in_curr_x; }
  void SetCurrPeakY(int in_curr_y) { curr_peaky = in_curr_y; }
  void SetGeometry(int in_geometry) { geometry = in_geometry; }
  void SetModified(bool in_modified) { m_modified = in_modified; }
  void SetCellAmount(int cell_id, double res);
  void SetInitial(double initial) { m_initial = initial; }
};

#endif
