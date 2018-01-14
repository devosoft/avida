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
#include "cAvidaContext.h"
#include "tMatrix.h"
#include "nGeometry.h"

class cWorld;


/*
  @MRR January 2018

  cResourceCount is the class responsible for resource accounting.
  
  Resources come in a variety of flavors: global, partial, traditionally spatial, and
  gradient-spatial.  This handles the accountancy of all these resource types.  Demes 
  also this class to keep track of their resources.
  
  All resources are added with a unique resource id (res_id).  This class contains a 
  set of containers that use a resource's id as an index into these containers to 
  retrieve the value of each property.  For example, resource_name[2] will give yield
  the name of a resource with the resource id of 2.
  
  Not all resources may have properties fully defined in each container.  They will, however,
  occupy a space at that index.  For example, a global resource will have a spatial resource
  count (a set of resource abundances for each cell) available, but it may not be initialized
  or return an undefined result.  
  
  It is up to the user of this class to make sure that calls to get or set values for properties
  of a particular resource are sensicle.

*/

class cResourceCount
{
private:
  mutable Apto::Array<cString> resource_name;    // The name of each resource
  mutable Apto::Array<double> resource_initial;  // Initial quantity of each resource
  mutable Apto::Array<double> resource_count;  // Current quantity of each resource
  Apto::Array<double> decay_rate;      // Multiplies resource count at each step
  Apto::Array<double> inflow_rate;     // An increment for resource at each step
  tMatrix<double> decay_precalc;  // Precalculation of decay values
  tMatrix<double> inflow_precalc; // Precalculation of inflow values
  Apto::Array<int> geometry;           // Spatial layout of each resource
  mutable Apto::Array<cSpatialResCount* > spatial_resource_count;
  mutable Apto::Array<double> curr_grid_res_cnt;
  mutable Apto::Array< Apto::Array<double> > curr_spatial_res_cnt;
  int verbosity;
  Apto::Array< Apto::Array<int> > cell_lists;

  // Setup the update process to use lazy evaluation...
  mutable double update_time;     // Portion of an update compleated...
  mutable double spatial_update_time;
  mutable int m_last_updated;
  mutable int m_spatial_update;

  void DoUpdates(cAvidaContext& ctx, bool global_only = false) const;         // Update resource count based on update time
  
  void DoNonSpatialUpdates(cAvidaContext& ctx, const int res_id, int num_steps) const;
  void DoSpatialUpdates(cAvidaContext& ctx, const int res_id, int num_updates) const;

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
  void SetCellResources(int cell_id, const Apto::Array<double> & res);

  void Setup(cWorld* world, const int& id, const cString& name, const double& initial, const double& inflow, const double& decay,                      
	   const int& in_geometry, const double& in_xdiffuse, const double& in_xgravity, 
	   const double& in_ydiffuse, const double& in_ygravity,
	   const int& in_inflowX1, const int& in_inflowX2, const int& in_inflowY1, const int& in_inflowY2,
	   const int& in_outflowX1, const int& in_outflowX2, const int& in_outflowY1, 
	   const int& in_outflowY2, Apto::Array<cCellResource> *in_cell_list_ptr,
	   Apto::Array<int> *in_cell_id_list_ptr, const int& verbosity_level,
     const int& in_peaks,
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
     const int& in_halo_anchor_x, const int& in_halo_anchor_y, const int& in_move_speed, const int& in_move_resistance,
     const double& in_plateau_inflow, const double& in_plateau_outflow, const double& in_cone_inflow, const double& in_cone_outflow,
     const double& in_gradient_inflow, const int& in_is_plateau_common, const double& in_floor, const int& in_habitat, 
     const int& in_min_size, const int& in_max_size, const int& in_config, const int& in_count, const double& in_resistance, 
     const double& in_damage, const double& in_death_odds, const int& in_path, const int& in_hammer,
     const double& in_init_plat, const double& in_threshold, const int& in_refuge, const bool& isgradient
	   ); 
  
  void SetGradientCount(cAvidaContext& ctx, cWorld* world, const int& res_id, const int& peakx, const int& peaky,
    const int& height, const int& spread, const double& plateau, const int& decay, 
    const int& max_x, const int& min_x, const int& max_y, const int& min_y, const double& move_a_scaler,
    const int& updatestep, const int& halo, const int& halo_inner_radius, const int& halo_width,
    const int& halo_anchor_x, const int& halo_anchor_y, const int& move_speed, const int& move_resistance,
    const double& plateau_inflow, const double& plateau_outflow, const double& cone_inflow, const double& cone_outflow, 
    const double& gradient_inflow, const int& is_plateau_common, const double& floor, const int& habitat, 
    const int& min_size, const int& max_size, const int& config, const int& count, const double& resistance, const double& damage,
    const double& death_odds, const int& in_path, const int& in_hammer, const double& plat_val, const double& threshold, const int& refuge);
  void SetGradientPlatInflow(const int& res_id, const double& inflow);
  void SetGradientPlatOutflow(const int& res_id, const double& outflow);
  void SetGradientConeInflow(const int& res_id, const double& inflow);
  void SetGradientConeOutflow(const int& res_id, const double& outflow);
  void SetGradientInflow(const int& res_id, const double& inflow);
  void SetGradPlatVarInflow(cAvidaContext& ctx, const int& res_id, const double& mean, const double& variance, const int& type);
  void SetPredatoryResource(const int& res_id, const double& odds, const int& juvsper);
  void SetProbabilisticResource(cAvidaContext& ctx, const int& res_id, const double& initial, const double& inflow, 
                                const double& outflow, const double& lambda, const double& theta, const int& x, const int& y, const int& count);

  int GetResourceCountID(const cString& res_name);
  double GetInflow(const cString& name);
  void SetInflow(const cString& name, const double _inflow);
  double GetDecay(const cString& name);
  void SetDecay(const cString& name, const double _decay);
  
  void Update(double in_time);

  int GetSize(void) const { return resource_count.GetSize(); }
  const Apto::Array<double>& ReadResources(void) const { return resource_count; }
  const Apto::Array<double>& GetResources(cAvidaContext& ctx) const; 
  const Apto::Array<double>& GetCellResources(int cell_id, cAvidaContext& ctx) const;
  const Apto::Array<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id) const;
  double GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const;
  double GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const;
  const Apto::Array<int>& GetResourcesGeometry() const;
  int GetResourceGeometry(int res_id) const { return geometry[res_id]; }
  const Apto::Array<Apto::Array<double> >& GetSpatialRes(cAvidaContext& ctx);
  const Apto::Array<Apto::Array<int> >& GetCellIdLists() const { return cell_lists; }
  void Modify(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void Modify(cAvidaContext& ctx, int id, double change);
  void ModifyCell(cAvidaContext& ctx, const Apto::Array<double> & res_change, int cell_id);
  void Set(cAvidaContext& ctx, int id, double new_level);
  double Get(cAvidaContext& ctx, int id) const;
  void ResizeSpatialGrids(int in_x, int in_y);
  cSpatialResCount GetSpatialResource(int id) { return *(spatial_resource_count[id]); }
  const cSpatialResCount& GetSpatialResource(int id) const { return *(spatial_resource_count[id]); }
  void ReinitializeResources(cAvidaContext& ctx, double additional_resource);
  double GetInitialResourceValue(int resourceID) const { return resource_initial[resourceID]; }
  const cString& GetResName(int id) const { return resource_name[id]; }
  bool IsSpatialResource(int res_id) const {
    int geom = geometry[res_id];
    return geom != nGeometry::GLOBAL && geom != nGeometry::PARTIAL; 
  }
  int GetResourceByName(cString name) const;
  
  int GetCurrPeakX(cAvidaContext& ctx, int res_id) const;
  int GetCurrPeakY(cAvidaContext& ctx, int res_id) const;
  int GetFrozenPeakX(cAvidaContext& ctx, int res_id) const;
  int GetFrozenPeakY(cAvidaContext& ctx, int res_id) const;
  Apto::Array<int>* GetWallCells(int res_id);
  int GetMinUsedX(int res_id);
  int GetMinUsedY(int res_id);
  int GetMaxUsedX(int res_id);
  int GetMaxUsedY(int res_id);
  
  void SetSpatialUpdate(int update) { m_spatial_update = update; }
  void UpdateGlobalResources(cAvidaContext& ctx) { DoUpdates(ctx, true); }
  void UpdateRandomResources(cAvidaContext& ctx) { DoUpdates(ctx, false); }
  void UpdateResources(cAvidaContext& ctx) { DoUpdates(ctx, false); }
};

#endif
