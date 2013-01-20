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
class cWorld;

class cPopulationResources
{
private:
  mutable Apto::Array<cString> resource_names;
  mutable Apto::Array<double> resource_initial;  // Initial quantity of each resource
  mutable Apto::Array<double> resource_count;  // Current quantity of each resource
  Apto::Array<double> decay_rate;      // Multiplies resource count at each step
  Apto::Array<double> inflow_rate;     // An increment for resource at each step
  tMatrix<double> decay_precalc;  // Precalculation of decay values
  tMatrix<double> inflow_precalc; // Precalculation of inflow values
  Apto::Array<int> geometry;           // Spatial layout of each resource
  mutable Apto::Array<cResource* > resources;
  mutable Apto::Array<double> curr_grid_res_val;
  mutable Apto::Array< Apto::Array<double> > curr_diffusion_res_val;
  int verbosity;
  Apto::Array< Apto::Array<int> > cell_lists;

  // Setup the update process to use lazy evaluation...
  cWorld* m_world;
  mutable double update_time;     // Portion of an update compleated...
  mutable double spatial_update_time;
  mutable int m_last_updated;
  mutable int m_spatial_update;
  bool m_has_predatory_res;

  int m_hgt_resid; //!< HGT resource ID.

  void DoUpdates(cAvidaContext& ctx, bool global_only = false) const;         // Update resource count based on update time

  // A few constants to describe update process...
  static const double UPDATE_STEP;   // Fraction of an update per step
  static const double EPSILON;       // Tolorance for round off errors
  static const int PRECALC_DISTANCE; // Number of steps to precalculate
  
public:
  cPopulationResources(int num_resources = 0);
  cPopulationResources(const cPopulationResources&);
  ~cPopulationResources();

  const cPopulationResources& operator=(const cPopulationResources&);

  void SetSize(int num_resources);
  void SetCellResources(int cell_id, const Apto::Array<double> & res);

  cString GetGeometryName(const int& in_geometry);
  void SetupGlobalRes(cWorld* world, const int& id, const cString& name, const double& initial, const double& inflow, const double& decay,
	   const int& in_geometry, const int& verbosity_level); 
  
  void SetupDiffusionRes(cWorld* world, const int& id, const cString& name, const double& initial, const double& inflow,
                         const double& decay, const int& in_geometry, const double& in_xdiffuse, const double& in_xgravity,
                         const double& in_ydiffuse, const double& in_ygravity,
                         const int& in_inflowX1, const int& in_inflowX2, const int& in_inflowY1, const int& in_inflowY2,
                         const int& in_outflowX1, const int& in_outflowX2, const int& in_outflowY1,
                         const int& in_outflowY2, Apto::Array<cCellResource> *in_cell_list_ptr,
                         Apto::Array<int> *in_cell_id_list_ptr, const int& verbosity_level);
  
  void SetupDynamicRes(cWorld* world, const int& res_index, cResourceDef* res_def, const int& verbosity_level);
  void ResetDynamicRes(cAvidaContext& ctx, cWorld* world, int res_id);

  void SetDynamicResPlateauInflow(const int& res_id, const double& inflow);
  void SetDynamicResPlateauOutflow(const int& res_id, const double& outflow);
  void SetDynamicResConeInflow(const int& res_id, const double& inflow);
  void SetDynamicResConeOutflow(const int& res_id, const double& outflow);
  void SetDynamicResInflow(const int& res_id, const double& inflow);
  void SetDynamicResPlatVarInflow(const int& res_id, const double& mean, const double& variance, const int& type);
  void SetPredatoryResource(const int& res_id, const double& odds, const int& juvsper);
  void SetProbabilisticResource(cAvidaContext& ctx, const int& res_id, const double& initial, const double& inflow, 
                                const double& outflow, const double& lambda, const double& theta, const int& x, const int& y, const int& count);

  int GetResourceCountID(const cString& res_name);
  double GetInflow(const cString& name);
  void SetInflow(const cString& name, const double _inflow);
  double GetDecay(const cString& name);
  void SetDecay(const cString& name, const double _decay);
  int& GetHGTResidID() { return m_hgt_resid; }
  
  void Update(double in_time);

  int GetSize(void) const { return resource_count.GetSize(); }
  const Apto::Array<double>& ReadResources(void) const { return resource_count; }
  const Apto::Array<double>& GetResources(cAvidaContext& ctx) const; 
  const Apto::Array<double>& GetCellResources(int cell_id, cAvidaContext& ctx) const;
  const Apto::Array<double>& GetFrozenResources(cAvidaContext& ctx, int cell_id) const;
  double GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const;
  double GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const;
  const Apto::Array<int>& GetResourceGeometries() const;
  int GetResourceGeometry(int res_id) const { return geometry[res_id]; }
  
  const Apto::Array<Apto::Array<double> >& GetDiffusionResVals(cAvidaContext& ctx);
  const Apto::Array<Apto::Array<int> >& GetCellIdLists() const { return cell_lists; }
  
  void Modify(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void Modify(cAvidaContext& ctx, int id, double change);
  void ModifyCell(cAvidaContext& ctx, const Apto::Array<double> & res_change, int cell_id);
  void Set(cAvidaContext& ctx, int id, double new_level);
  double Get(cAvidaContext& ctx, int id) const;
  void ResizeSpatialGrids(int in_x, int in_y, int geometry);

  cResource GetResource(int id) { return *(resources[id]); }
  const cResource& GetResource(int id) const { return *(resources[id]); }
  cResource GetSpatialResource(int id) { assert(resources[id]->GetResDef()->IsSpatial()); return *(resources[id]); }
  const cResource& GetSpatialResource(int id) const { assert(resources[id]->GetResDef()->IsSpatial()); return *(resources[id]); }

  void ReinitializeResources(cAvidaContext& ctx, double additional_resource);
  double GetInitialResourceValue(int resourceID) const { return resource_initial[resourceID]; }
  const cString& GetResName(int id) const { return resource_names[id]; }
  bool IsSpatial(int id) const { return ((geometry[id] != nGeometry::GLOBAL) && (geometry[id] != nGeometry::PARTIAL)); }
  int GetResourceByName(cString name) const;
  const Apto::Array<double>& GetDemeResources(int deme_id, cAvidaContext& ctx);
  const Apto::Array<double>& GetDemeCellResources(int deme_id, int cell_id, cAvidaContext& ctx);
  
  // dynamic resources
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
  void UpdateResources(cAvidaContext& ctx) { DoUpdates(ctx, false); }
  
  // ported from cPopulation
  void PrintDemeResource(cAvidaContext& ctx); 
  void PrintDemeGlobalResources(cAvidaContext& ctx); 
  void PrintDemeSpatialResData(const cPopulationResources& res, const int i, const int deme_id, cAvidaContext& ctx) const; 
  void UpdateResStats(cAvidaContext& ctx);
  void TriggerDoUpdates(cAvidaContext& ctx) { UpdateResources(ctx); }

  void UpdateResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
  void UpdateResource(cAvidaContext& ctx, int id, double change);
  void UpdateCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id);
  void UpdateDemeCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id);
  
  void SetResource(cAvidaContext& ctx, int id, double new_level);
  void SetResource(cAvidaContext& ctx, const cString res_name, double new_level);
  double GetResource(cAvidaContext& ctx, int id) const { return Get(ctx, id); }
  void SetResourceInflow(const cString res_name, double new_level);
  void SetResourceOutflow(const cString res_name, double new_level);
  
  void SetDemeResource(cAvidaContext& ctx, const cString res_name, double new_level);
  void SetSingleDemeResourceInflow(int deme_id, const cString res_name, double new_level);
  void SetDemeResourceInflow(const cString res_name, double new_level);
  void SetSingleDemeResourceOutflow(int deme_id, const cString res_name, double new_level);
  void SetDemeResourceOutflow(const cString res_name, double new_level);
  
  // Let users change environmental variables durning the run 
  void UpdateResource(const int Verbosity, cWorld* world);        
  
  // Let users change Dynami Resource variables during the run JW
  void UpdateDynamicRes(cAvidaContext& ctx, cWorld* world, const cString res_name);
  void SetDynamicResPlatVarInflow(const cString res_name, const double mean, const double variance, const int type);
  void SetPredatoryResource(const cString res_name, const double odds, const int juvsper, const double detection_prob);
  void ExecutePredatoryResource(cAvidaContext& ctx, const int cell_id, const double pred_odds, const int juvs_per);
  bool HasPredatoryRes() { return m_has_predatory_res; }
 
  // -------- HGT support --------
  //! Modify current level of the HGT resource.
  void AdjustHGTResource(cAvidaContext& ctx, double delta);


};

#endif
