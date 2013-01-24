/*
 *  cPopulationResources.h
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

#ifndef cPopulationResources_h
#define cPopulationResources_h

#include "avida/Avida.h"

#include "nGeometry.h"
#include "tMatrix.h"
#include "cResource.h"
#include "cString.h"

class cAvidaContext;
class cResourceDef;
class cResourcePopulationInterface;
class cWorld;

class cPopulationResources
{
private:
  // A few constants to describe update process...
  static const double UPDATE_STEP;                // Fraction of an update per step
  static const double EPSILON;                    // Tolorance for round off errors
  static const int PRECALC_DISTANCE;              // Number of steps to precalculate

private:
  cWorld* m_world;
  cResourcePopulationInterface* m_interface;

  mutable Apto::Array<cResource*> resources;     // The actual resources in the world
  mutable Apto::Array<cString> resource_names;
  mutable Apto::Array<double> resource_initial;   // Initial quantity of each resource
  mutable Apto::Array<double> resource_count;     // Current quantity of each resource
  Apto::Array<double> decay_rate;                 // Multiplies resource count at each step
  Apto::Array<double> inflow_rate;                // An increment for resource at each step
  tMatrix<double> decay_precalc;                  // Precalculation of decay values
  tMatrix<double> inflow_precalc;                 // Precalculation of inflow values
  Apto::Array<int> geometry;                      // Spatial layout of each resource
  mutable Apto::Array<double> curr_grid_res_val;
  mutable Apto::Array< Apto::Array<double> > curr_diffusion_res_val;
  int verbosity;
  Apto::Array< Apto::Array<int> > cell_lists;

  // Setup the update process to use lazy evaluation...
  mutable double update_time;                     // Portion of an update completed...
  int worldx;
  int worldy;
  
  mutable double spatial_update_time;
  mutable int m_last_updated;
  mutable int m_spatial_update;
  bool m_has_predatory_res;

  int m_hgt_resid; //!< HGT resource ID.


private:
  // PRIVATE POPULATION RESOURCE METHODS //
  void DoUpdates(cAvidaContext& ctx, bool global_only = false) const;         // Update resource count based on update time
  cString GetGeometryName(const int& in_geometry);
  const Apto::Array<int>& GetResourceGeometries() const;
  const Apto::Array<double>& ReadResources(void) const { return resource_count; }
  bool HasPredatoryRes() { return m_has_predatory_res; }
  
  void UpdateResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);

  void SetInflow(const cString& name, const double _inflow);
  void SetDecay(const cString& name, const double _decay);

  cResource GetResource(int id) { return *(resources[id]); }
  const cResource& GetResource(int id) const { return *(resources[id]); }
  int GetResourceGeometry(int res_id) const { return geometry[res_id]; }
  double GetDecay(const cString& name);

  const cResource& GetSpatialResource(int id) const { assert(resources[id]->GetResDef()->IsSpatial()); return *(resources[id]); }
  void Modify(cAvidaContext& ctx, int id, double change);

  // diffusion resources
  const Apto::Array<Apto::Array<double> >& GetDiffusionResVals(cAvidaContext& ctx);
  
  // dynamic resources
  void ResetDynamicRes(cAvidaContext& ctx, cWorld* world, int res_id);

  cPopulationResources(const cPopulationResources&);
  const cPopulationResources& operator=(const cPopulationResources&);


public:
  cPopulationResources(cResourcePopulationInterface* pop, int num_resources = 0);
  ~cPopulationResources();


  // PUBLIC POPULATION RESOURCE METHODS //
  // the pop res object & operations on all resources
  void SetSize(int num_resources);
  void SetCellResources(int cell_id, const Apto::Array<double> & res);
  void SetSpatialUpdate(int update) { m_spatial_update = update; }

  void SetX(int x) { worldx = x; }
  void SetY(int y) { worldy = y; }

  int GetSize(void) const { return resource_count.GetSize(); }
  const Apto::Array<double>& GetResources(cAvidaContext& ctx) const; 
  const Apto::Array<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id) const;
  const Apto::Array<Apto::Array<int> >& GetCellIdLists() const { return cell_lists; }
  const Apto::Array<double>& GetCellResources(int cell_id, cAvidaContext& ctx) const;
  
  void UpdateCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id);
  void UpdateResStats(cAvidaContext& ctx);

  void TriggerDoUpdates(cAvidaContext& ctx) { DoUpdates(ctx, false); }
  void ReinitializeResources(cAvidaContext& ctx, double additional_resource);
  void ResizeSpatialGrids(int in_x, int in_y, int geometry);
  
  // single resources
  void Set(cAvidaContext& ctx, int id, double new_level);
  void SetResource(cAvidaContext& ctx, int id, double new_level);
  void SetResource(cAvidaContext& ctx, const cString res_name, double new_level);
  void SetResourceInflow(const cString res_name, double new_level);
  void SetResourceOutflow(const cString res_name, double new_level);
  
  double Get(cAvidaContext& ctx, int id) const;
  double GetResource(cAvidaContext& ctx, int id) const { return Get(ctx, id); }
  int GetResourceByName(cString name) const;
  int GetResourceCountID(const cString& res_name);
  const cString& GetResName(int id) const { return resource_names[id]; }
  bool IsSpatial(int id) const { return ((geometry[id] != nGeometry::GLOBAL) && (geometry[id] != nGeometry::PARTIAL)); }
  double GetInflow(const cString& name);
  double GetInitialResourceValue(int resourceID) const { return resource_initial[resourceID]; }
  double GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const;
  double GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const;

  cResource GetSpatialResource(int id) { return GetSpatialResource(id); }

  void Update(double in_time);
  void UpdateResource(const int Verbosity, cWorld* world);        
  void UpdateResource(cAvidaContext& ctx, int id, double change);
  
  void Modify(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void ModifyCell(cAvidaContext& ctx, const Apto::Array<double> & res_change, int cell_id);

  // global resources
  void SetupGlobalRes(cWorld* world, const int& id, const cString& name, const double& initial, const double& inflow,
                        const double& decay, const int& in_geometry, const int& verbosity_level);

  void UpdateGlobalResources(cAvidaContext& ctx) { DoUpdates(ctx, true); }
  
  // diffusion resources
  void SetupDiffusionRes(cWorld* world, const int& id, const cString& name, const double& initial, const double& inflow,
                         const double& decay, const int& in_geometry, const double& in_xdiffuse, const double& in_xgravity,
                         const double& in_ydiffuse, const double& in_ygravity,
                         const int& in_inflowX1, const int& in_inflowX2, const int& in_inflowY1, const int& in_inflowY2,
                         const int& in_outflowX1, const int& in_outflowX2, const int& in_outflowY1,
                         const int& in_outflowY2, Apto::Array<cCellResource> *in_cell_list_ptr,
                         Apto::Array<int> *in_cell_id_list_ptr, const int& verbosity_level);

  // dynamic resources
  void SetupDynamicRes(cWorld* world, const int& res_index, cResourceDef* res_def, const int& verbosity_level);
  void SetDynamicResPlatVarInflow(const cString res_name, const double mean, const double variance, const int type);
  void SetPredatoryResource(const cString res_name, const double odds, const int juvsper, const double detection_prob);

  int GetCurrPeakX(cAvidaContext& ctx, int res_id) const;
  int GetCurrPeakY(cAvidaContext& ctx, int res_id) const;
  int GetFrozenPeakX(cAvidaContext& ctx, int res_id) const;
  int GetFrozenPeakY(cAvidaContext& ctx, int res_id) const;
  Apto::Array<int>* GetWallCells(int res_id);
  int GetMinUsedX(int res_id);
  int GetMinUsedY(int res_id);
  int GetMaxUsedX(int res_id);
  int GetMaxUsedY(int res_id);

  void UpdateDynamicRes(cAvidaContext& ctx, cWorld* world, const cString res_name);
  void ExecutePredatoryResource(cAvidaContext& ctx, const int cell_id, const double pred_odds, const int juvs_per);


  // -------- HGT support --------
  int& GetHGTResidID() { return m_hgt_resid; }

  //! Modify current level of the HGT resource.
  void AdjustHGTResource(cAvidaContext& ctx, double delta);
};

#endif
