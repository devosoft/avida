/*
 *  cResourceDef.cc
 *  Avida
 *
 *  Called "resource.cc" prior to 12/5/05.
 *  Called "cRsource.cc" prior to 01/17/2013.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cResourceDef.h"

#include "nGeometry.h"

cCellResource::cCellResource()
  : cell_id(-99)
  , initial(0.0)
  , inflow(0.0)
  , outflow(0.0)
{
}

cCellResource::cCellResource(int _cell_id, double _initial, double _inflow, double _outflow)
  : cell_id(_cell_id)
  , initial(_initial)
  , inflow(_inflow)
  , outflow(_outflow)
{
}

cResourceDef::cResourceDef(const cString & _name, int _id)
  : name(_name)
  , id(_id)
  , index(-1)  // index can only be assigned later, give it a flag value for now
  , initial(0.0)
  , inflow(0.0)
  , outflow(0.0)
  , geometry(nGeometry::GLOBAL)
  , isdiffusion(false)
  , isdynamic(false)
  , isspatial(false)
  , inflowX1(-99)
  , inflowX2(-99)
  , inflowY1(-99)
  , inflowY2(-99)
  , outflowX1(-99)
  , outflowX2(-99)
  , outflowY1(-99)
  , outflowY2(-99)
  , xdiffuse(1.0)
  , xgravity(0.0)
  , ydiffuse(1.0)
  , ygravity(0.0)
  , energy_resource(false)
  , m_peakx(-1)
  , m_peaky(-1)
  , m_height(0)
  , m_spread(0)
  , m_plateau(-1.0)
  , m_decay(1)
  , m_max_x(0)
  , m_max_y(0)
  , m_min_x(0)
  , m_min_y(0)
  , m_move_a_scaler(1.0)
  , m_updatestep(1)
  , m_halo(0)
  , m_halo_inner_radius(0)
  , m_halo_width(0)
  , m_halo_anchor_x(0)
  , m_halo_anchor_y(0)
  , m_move_speed(1)
  , m_plateau_inflow(0.0)
  , m_plateau_outflow(0.0)
  , m_cone_inflow(0.0)
  , m_cone_outflow(0.0)
  , m_res_inflow(0.0)
  , m_is_plateau_common(0)
  , m_floor(0.0)
  , m_habitat(0)
  , m_min_size(1)
  , m_max_size(1)
  , m_config(0)
  , m_count(1)
  , m_resistance(1.0)
  , m_initial_plat(-1.0)
  , m_threshold(1.0)
  , m_refuge(0)
  , m_predator_odds(0.0)
  , m_predator(false)
  , m_guard_juvs_per(0)
  , m_prob_detect(1.0)
  , m_probabilistic(false)
  , hgt_metabolize(false)
  , collectable(true)
{
}

/* Return a pointer to cell resource with a given cell id, if there is no
   cell resource with that id return NULL */
cCellResource *cResourceDef::GetCellResourcePtr(int _id) {
  bool found = false;
  int cell_index = 0;
  while (cell_index < cell_list.GetSize() && !found) {
    if (cell_list[cell_index].GetId() == _id) {
      return(&cell_list[cell_index]);
      found = true;
    } else {
      cell_index++;
    }
  }
  return(NULL);
}

void cResourceDef::SetCellIdList(Apto::Array<int>& id_list) {
	cell_id_list.ResizeClear(id_list.GetSize());
	cell_id_list=id_list;
}

/* Update the values of given cell resource */
void cResourceDef::UpdateCellResource(cCellResource *_CellResourcePtr, double _initial,
                        double _inflow, double _outflow) {
  _CellResourcePtr->SetInitial(_initial);
  _CellResourcePtr->SetInflow(_inflow);
  _CellResourcePtr->SetOutflow(_outflow);
}

/* Set if the resource is a energy resource */
bool cResourceDef::SetEnergyResource(cString _energy_resource) {
  _energy_resource.ToLower();
  if ((_energy_resource == "false") || (_energy_resource == "0")) {
    energy_resource = false;
    return true;
  } else if ((_energy_resource == "true") || (_energy_resource == "1")) {
    energy_resource = true;
    return true;
  }
  return false;
}

bool cResourceDef::SetGeometry(cString _geometry)
/* Set the geometry for the resource */
{
  _geometry.ToLower();
  if (_geometry == "global") {
    geometry = nGeometry::GLOBAL;
    return true;
  } else if (_geometry == "grid") {
    geometry = nGeometry::GRID;
    return true;
  } else if (_geometry == "torus") {
    geometry = nGeometry::TORUS;
    return true;
  }
  else if (_geometry == "partial") {
    geometry = nGeometry::PARTIAL;
    return true;
  }
  else if (_geometry == "dynamic") {
    geometry = nGeometry::DYNAMIC;
    return true;
  }
  else {
    return false;
  }
}
