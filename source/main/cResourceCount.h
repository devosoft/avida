/*
 *  cResourceCount.h
 *  Avida
 *
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

#ifndef cResourceCount_h
#define cResourceCount_h

#include "avida/Avida.h"

#include "cSpatialResCount.h"
#include "cString.h"
#include "tArray.h"
#include "cAvidaContext.h"
#include "tMatrix.h"
#include "nGeometry.h"
#ifndef tArrayMap_h
#include "tArrayMap.h"
#endif

class cWorld;

class cResourceCount
{
private:
  mutable tArray<cString> resource_name;
  mutable tArray<double> resource_initial;  // Initial quantity of each resource
  mutable tArray<double> resource_count;  // Current quantity of each resource
  tArray<double> decay_rate;      // Multiplies resource count at each step
  tArray<double> inflow_rate;     // An increment for resource at each step
  tMatrix<double> decay_precalc;  // Precalculation of decay values
  tMatrix<double> inflow_precalc; // Precalculation of inflow values
  tArray<int> geometry;           // Spatial layout of each resource
  mutable tArray<cSpatialResCount* > spatial_resource_count;
  mutable tArray<double> curr_grid_res_cnt;
  mutable tArray< tArray<double> > curr_spatial_res_cnt;
  int verbosity;
  tArray< tArray<int> > cell_lists;	

  // Setup the update process to use lazy evaluation...
  mutable double update_time;     // Portion of an update compleated...
  mutable double spatial_update_time;

  void DoUpdates(cAvidaContext& ctx, bool global_only = false) const;         // Update resource count based on update time

  // A few constants to describe update process...
  static const double UPDATE_STEP;   // Fraction of an update per step
  static const double EPSILON;       // Tolorance for round off errors
  static const int PRECALC_DISTANCE; // Number of steps to precalculate
  
public:
  cResourceCount(int num_resources = 0);
  cResourceCount(const cResourceCount&);
  ~cResourceCount();

  const cResourceCount& operator=(const cResourceCount&);

  void SetSize(int num_resources);
  void SetCellResources(int cell_id, const tArray<double> & res);

  void Setup(cWorld* world, const int& id, const cString& name, const double& initial, const double& inflow, const double& decay,                      
	   const int& in_geometry, const double& in_xdiffuse, const double& in_xgravity, 
	   const double& in_ydiffuse, const double& in_ygravity,
	   const int& in_inflowX1, const int& in_inflowX2, const int& in_inflowY1, const int& in_inflowY2,
	   const int& in_outflowX1, const int& in_outflowX2, const int& in_outflowY1, 
	   const int& in_outflowY2, tArray<cCellResource> *in_cell_list_ptr,
	   tArray<int> *in_cell_id_list_ptr, const int& verbosity_level,
	   const bool& isdynamic, const int& in_peaks,
	   const double& in_min_height, const double& in_min_radius, const double& in_radius_range,
	   const double& in_ah, const double& in_ar,
	   const double& in_acx, const double& in_acy,
	   const double& in_hstepscale, const double& in_rstepscale,
	   const double& in_cstepscalex, const double& in_cstepscaley,
	   const double& in_hstep, const double& in_rstep,
	   const double& in_cstepx, const double& in_cstepy,
	   const int& in_update_dynamic, const int& in_peakx, const int& in_peaky,
	   const int& in_height, const int& in_spread, const double& in_plateau, const int& in_decay, 
     const int& in_max_x, const int& in_min_x, const int& in_max_y, const int& in_min_y, const double& in_move_a_scaler,
     const int& in_updatestep, const int& in_halo, const int& in_halo_inner_radius, const int& in_halo_width,
     const int& in_halo_anchor_x, const int& in_halo_anchor_y, const int& in_move_speed, 
     const double& in_plateau_inflow, const double& in_plateau_outflow, const int& in_is_plateau_common, 
     const double& in_floor, const int& in_habitat, const int& in_min_size, const int& in_max_size,
     const int& in_config, const int& in_count, const double& in_resistance, const double& in_init_plat, 
     const double& in_threshold, const bool& isgradient
	   ); 
  
  void SetGradientCount(cAvidaContext& ctx, cWorld* world, const int& res_id, const int& peakx, const int& peaky,
    const int& height, const int& spread, const double& plateau, const int& decay, 
    const int& max_x, const int& min_x, const int& max_y, const int& min_y, const double& move_a_scaler,
    const int& updatestep, const int& halo, const int& halo_inner_radius, const int& halo_width,
    const int& halo_anchor_x, const int& halo_anchor_y, const int& move_speed, 
    const double& plateau_inflow, const double& plateau_outflow, const int& is_plateau_common, 
    const double& floor, const int& habitat, const int& min_size, const int& max_size,
    const int& config, const int& count, const double& resistance, const int& plat_val, const double& threshold); 
  void SetGradientInflow(const int& res_id, const double& inflow);
  void SetGradientOutflow(const int& res_id, const double& outflow);
  int GetResourceCountID(const cString& res_name);
  double GetInflow(const cString& name);
  void SetInflow(const cString& name, const double _inflow);
  double GetDecay(const cString& name);
  void SetDecay(const cString& name, const double _decay);
  
  void Update(double in_time);

  int GetSize(void) const { return resource_count.GetSize(); }
  const tArray<double>& ReadResources(void) const { return resource_count; }
  const tArray<double>& GetResources(cAvidaContext& ctx) const; 
  const tArray<double>& GetCellResources(int cell_id, cAvidaContext& ctx) const;
  const tArray<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id) const;
  const tArray<int>& GetResourcesGeometry() const;
  int GetResourceGeometry(int res_id) const { return geometry[res_id]; }
  const tArray<tArray<double> >& GetSpatialRes(cAvidaContext& ctx); 
  const tArray<tArray<int> >& GetCellIdLists() const { return cell_lists; }
  void Modify(cAvidaContext& ctx, const tArray<double>& res_change);
  void Modify(cAvidaContext& ctx, int id, double change);
  void ModifyCell(cAvidaContext& ctx, const tArray<double> & res_change, int cell_id);
  void Set(cAvidaContext& ctx, int id, double new_level);
  double Get(cAvidaContext& ctx, int id) const;
  void ResizeSpatialGrids(int in_x, int in_y);
  cSpatialResCount GetSpatialResource(int id) { return *(spatial_resource_count[id]); }
  const cSpatialResCount& GetSpatialResource(int id) const { return *(spatial_resource_count[id]); }
  void ReinitializeResources(cAvidaContext& ctx, double additional_resource);
  double GetInitialResourceValue(int resourceID) const { return resource_initial[resourceID]; }
  const cString& GetResName(int id) const { return resource_name[id]; }
  bool IsSpatial(int id) const { return ((geometry[id] != nGeometry::GLOBAL) && (geometry[id] != nGeometry::PARTIAL)); }
  int GetResourceByName(cString name) const;
  
  int GetCurrPeakX(cAvidaContext& ctx, int res_id) const;
  int GetCurrPeakY(cAvidaContext& ctx, int res_id) const;
  int GetFrozenPeakX(cAvidaContext& ctx, int res_id) const;
  int GetFrozenPeakY(cAvidaContext& ctx, int res_id) const;
  
  void UpdateGlobalResources(cAvidaContext& ctx) { DoUpdates(ctx, true); }
};

#endif
