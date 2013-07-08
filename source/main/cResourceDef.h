/*
 *  cResourceDef.h
 *  Avida
 *
 *  Called "cResource.h" prior to 01/17/13.
 *  Called "resource.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef cResourceDef_h
#define cResourceDef_h

#include "cString.h"

class cAvidaContext;

/*! class to hold resource information for individual cells (mini-chemostats) */
class cCellResource
{
private:
  int cell_id;
  double initial;
  double inflow;
  double outflow;

public:
  cCellResource();
  cCellResource(int _cell_id, double _initial, double _inflow, double _outflow);
  int GetId() const { return cell_id; }
  double GetInitial() const { return initial; }
  double GetInflow() const { return inflow; }
  double GetOutflow() const { return outflow; }
  void SetInitial(double _initial) { initial = _initial; }
  void SetInflow(double _inflow) { inflow = _inflow; }
  void SetOutflow(double _outflow) { outflow = _outflow; }
};

/* class to hold all information for a single resource */
class cResourceDef
{
private:
  cString name;
  int id;    // 0-based, order of appearance in environment file; resource library index
  int index; // 0-based, order of appearance of THIS TYPE of resource in environment file; resource count index
  double initial;
  double inflow;
  double outflow;
  int geometry;

  bool isdiffusion;
  bool isdynamic;
  bool isspatial;

  int inflowX1;
  int inflowX2;
  int inflowY1;
  int inflowY2;
  int outflowX1;
  int outflowX2;
  int outflowY1;
  int outflowY2;
  double xdiffuse;
  double xgravity;
  double ydiffuse;
  double ygravity;
  bool org_resources;

  int m_peakx;
  int m_peaky;
  int m_height;
  int m_spread;    
  double m_plateau;
  int m_decay;
  int m_max_x;
  int m_max_y;
  int m_min_x;
  int m_min_y;
  double m_move_a_scaler;
  int m_updatestep; 
  int m_halo;
  int m_halo_inner_radius;
  int m_halo_width;
  int m_halo_anchor_x;
  int m_halo_anchor_y;
  int m_move_speed;
  double m_plateau_inflow;
  double m_plateau_outflow;
  double m_cone_inflow;
  double m_cone_outflow;
  double m_res_inflow;
  int m_is_plateau_common;
  double m_floor;
  int m_habitat;
  int m_min_size;
  int m_max_size;
  int m_config;
  int m_count; 
  double m_resistance;
  double m_initial_plat;
  double m_threshold;
  int m_refuge;
  Apto::Array<cCellResource> cell_list;
  Apto::Array<int> cell_id_list;
  double m_predator_odds;
  bool m_predator;
  double m_guard_juvs_per;
  double m_prob_detect;
  bool m_probabilistic;
  
  bool collectable;

  cResourceDef(); // @not_implemented

public:
  cResourceDef(const cString& _name, int _id);
  ~cResourceDef() { ; }

  void SetCellIdList(Apto::Array<int>& id_list); //SLG partial resources
  cCellResource* GetCellResourcePtr(int _id);
  void UpdateCellResource(cCellResource *_CellResoucePtr, double _initial, double _inflow, double _outflow);
  bool SetGeometry(cString _geometry);

  const cString& GetName() const { return name; }
  int GetID() const { return id; }
  int GetIndex() const { return index; }
  double GetInitial() const { return initial; }
  double GetInflow() const { return inflow; }
  double GetOutflow() const { return outflow; }
  int GetGeometry() const { return geometry; }
  bool IsSpatial() { return isspatial; }

  void SetIndex(int _index) { if (index < 0) index = _index; } // can only be assigned once
  void SetInitial(double _initial) { initial = _initial; }
  void SetInflow (double _inflow ) { inflow  = _inflow; }
  void SetOutflow(double _outflow) { outflow = _outflow; }
  void SetSpatial(bool _spatial) { isspatial = _spatial; }

  bool GetCollectable() { return collectable; }
  void AddCellResource(cCellResource new_cell) { cell_list.Push(new_cell); }

  // diffusion resources (old spatial resources)
  bool IsDiffusion() { return isdiffusion; }
  int GetInflowX1() const { return inflowX1; }
  int GetInflowX2() const { return inflowX2; }
  int GetInflowY1() const { return inflowY1; }
  int GetInflowY2() const { return inflowY2; }
  int GetOutflowX1() const { return outflowX1; }
  int GetOutflowX2() const { return outflowX2; }
  int GetOutflowY1() const { return outflowY1; }
  int GetOutflowY2() const { return outflowY2; }
  double GetXDiffuse() const { return xdiffuse; }
  double GetXGravity() const { return xgravity; }
  double GetYDiffuse() const { return ydiffuse; }
  double GetYGravity() const { return ygravity; }

  void SetDiffusion(bool _diffusion) { isdiffusion = _diffusion; }
  void SetInflowX1(int _inflowX1) { inflowX1 = _inflowX1; }
  void SetInflowX2(int _inflowX2) { inflowX2 = _inflowX2; }
  void SetInflowY1(int _inflowY1) { inflowY1 = _inflowY1; }
  void SetInflowY2(int _inflowY2) { inflowY2 = _inflowY2; }
  void SetOutflowX1(int _outflowX1) { outflowX1 = _outflowX1; }
  void SetOutflowX2(int _outflowX2) { outflowX2 = _outflowX2; }
  void SetOutflowY1(int _outflowY1) { outflowY1 = _outflowY1; }
  void SetOutflowY2(int _outflowY2) { outflowY2 = _outflowY2; }
  void SetXDiffuse(double _xdiffuse) { xdiffuse = _xdiffuse; }
  void SetXGravity(double _xgravity) { xgravity = _xgravity; }
  void SetYDiffuse(double _ydiffuse) { ydiffuse = _ydiffuse; }
  void SetYGravity(double _ygravity) { ygravity = _ygravity; }
  void SetCollectable(int _collectable) { collectable = _collectable; }
  
  // dynamic resources
  bool IsDynamic() { return isdynamic; }
  int& GetPeakX() { return m_peakx; }
  int& GetPeakY() { return m_peaky; }
  int GetHeight() { return m_height; }
  int GetSpread() { return m_spread; }
  double GetPlateau() { return m_plateau; }
  int GetDecay() { return m_decay; }
  int GetMaxX() { return m_max_x; }
  int GetMaxY() { return m_max_y; }
  int GetMinX() { return m_min_x; }
  int GetMinY() { return m_min_y; }
  double GetMoveScaler() { return m_move_a_scaler; }
  int GetUpdateStep() const { return m_updatestep; } 
  int IsHalo() { return m_halo;}
  int GetHaloInnerRadius() { return m_halo_inner_radius; }
  int GetHaloWidth() { return m_halo_width; }
  int GetHaloX() { return m_halo_anchor_x; }
  int GetHaloY() { return m_halo_anchor_y; }
  int& GetMoveSpeed() { return m_move_speed; }
  double GetPlateauInflow() { return m_plateau_inflow; }
  double GetPlateauOutflow() { return m_plateau_outflow; }
  double GetConeInflow() { return m_cone_inflow; }
  double GetConeOutflow() { return m_cone_outflow; }
  double GetDynamicResInflow() { return m_res_inflow; }
  int IsPlateauCommon() { return m_is_plateau_common; }
  double GetFloor() { return m_floor; }
  int GetHabitat() { return m_habitat; }
  int GetMinSize() { return m_min_size; }
  int GetMaxSize() { return m_max_size; }
  int GetConfig() { return m_config; }
  int GetCount() { return m_count; }
  double GetResistance() { return m_resistance; }
  double GetInitialPlatVal() { return m_initial_plat; }
  double GetThreshold() { return m_threshold; }
  int GetRefuge() { return m_refuge; }
  Apto::Array<cCellResource>* GetCellListPtr() { return &cell_list; }
  Apto::Array<int>* GetCellIdListPtr() { return &cell_id_list; }
  bool IsPredatory() { return m_predator; }
  double GetPredatorResOdds() { return m_predator_odds; }
  double GetJuvAdultGuardRatio() { return m_guard_juvs_per; }
  double GetDetectionProb() { return m_prob_detect; }
  bool IsProbabilistic() { return m_probabilistic; } 
  
  void SetDynamic(bool _dynamic) { isdynamic = _dynamic; }
  void SetPeakX(int _peakx) { m_peakx = _peakx; }
  void SetPeakY(int _peaky) { m_peaky = _peaky; }
  void SetHeight(int _height) { m_height = _height; }
  void SetSpread(int _spread) { m_spread = _spread; }
  void SetPlateau(double _plateau) { m_plateau = _plateau; }
  void SetDecay(int _decay) { m_decay = _decay; }
  void SetMaxX(int _max_x) { m_max_x = _max_x; }
  void SetMaxY(int _max_y) { m_max_y = _max_y; }
  void SetMinX(int _min_x) { m_min_x = _min_x; }
  void SetMinY(int _min_y) { m_min_y = _min_y; }
  void SetMoveScaler(double _move_a_scaler) { m_move_a_scaler = _move_a_scaler; }
  void SetUpdateStep(int _updatestep) { m_updatestep = _updatestep; } 
  void SetIsHalo(int _halo) { m_halo = _halo; }
  void SetHaloInnerRadius(int _halo_inner_radius) { m_halo_inner_radius = _halo_inner_radius; }
  void SetHaloWidth(int _halo_width) { m_halo_width = _halo_width; }
  void SetHaloX(int _halo_anchor_x) { m_halo_anchor_x = _halo_anchor_x; }
  void SetHaloY(int _halo_anchor_y) { m_halo_anchor_y = _halo_anchor_y; }
  void SetMoveSpeed(int _move_speed) { m_move_speed = _move_speed; }
  void SetPlateauInflow(double _plateau_inflow) { m_plateau_inflow = _plateau_inflow; }  
  void SetPlateauOutflow(double _plateau_outflow) { m_plateau_outflow = _plateau_outflow; }
  void SetConeInflow(double _cone_inflow) { m_cone_inflow = _cone_inflow; }  
  void SetConeOutflow(double _cone_outflow) { m_cone_outflow = _cone_outflow; }
  void SetDynamicResInflow(double _res_inflow) { m_res_inflow = _res_inflow; }
  void SetInitialPlat(double _initial_plat_val) { m_initial_plat = _initial_plat_val; }
  void SetIsPlateauCommon(int _is_plateau_common) { m_is_plateau_common = _is_plateau_common; }
  void SetFloor(double _floor) { m_floor = _floor; }
  void SetHabitat(int _habitat) { m_habitat = _habitat; }
  void SetMinSize(int _min_size) { m_min_size = _min_size; }
  void SetMaxSize(int _max_size) { m_max_size = _max_size; }
  void SetConfig(int _config) { m_config = _config; }
  void SetCount(int _count) { m_count = _count; }
  void SetResistance(double _resistance) { m_resistance = _resistance; }
  void SetThreshold(double _threshold) { m_threshold = _threshold; } 
  void SetRefuge(int _refuge) { m_refuge = _refuge; }
  inline void SetPredatoryResource(double _odds, int _juvsper, double _prob) { m_predator = true; m_predator_odds = _odds; m_guard_juvs_per = _juvsper; m_prob_detect = _prob; }
  inline void SetPredatoryResource(double _odds, int _juvsper) { m_predator = true; m_predator_odds = _odds; m_guard_juvs_per = _juvsper; }
  inline void SetProbabilisticResource(cAvidaContext& ctx, double initial, double inflow, double outflow, double lambda, double theta, int x, int y, int num_cells) {
    m_probabilistic = true;
    m_initial_plat = initial;
    m_plateau_inflow = inflow;
    m_plateau_outflow = outflow;
  }
};

#endif
