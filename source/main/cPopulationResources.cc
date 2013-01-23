/*
 *  cPopulationResources.cc
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

#include "cPopulationResources.h"

#include "cDeme.h"
#include "cDynamicRes.h"
#include "cDiffusionRes.h"
#include "cResourceElement.h"
#include "cResourcePopulationInterface.h"
#include "cWorld.h"

const double cPopulationResources::UPDATE_STEP(1.0 / 10000.0);
const double cPopulationResources::EPSILON(1.0e-15);
const int cPopulationResources::PRECALC_DISTANCE(100);


cPopulationResources::cPopulationResources(cResourcePopulationInterface* pop, int num_resources)
  : m_interface(pop)
  , update_time(0.0)
  , spatial_update_time(0.0)
  , m_last_updated(0)
  , m_spatial_update(0)
  , m_has_predatory_res(false)
  , m_hgt_resid(-1)
{
  if (num_resources > 0) SetSize(num_resources);
}

cPopulationResources::~cPopulationResources()
{
  for (int i = 0; i < resources.GetSize(); i++) {
    delete resources[i]; 
  }
}

void cPopulationResources::SetSize(int num_resources)
{
  for (int i = 0; i < resources.GetSize(); i++) delete resources[i];
  resources.ResizeClear(num_resources);
  resource_names.ResizeClear(num_resources);
  resource_initial.ResizeClear(num_resources);
  resource_count.ResizeClear(num_resources);
  decay_rate.ResizeClear(num_resources);
  inflow_rate.ResizeClear(num_resources);
  if (num_resources > 0) {
    decay_precalc.ResizeClear(num_resources, PRECALC_DISTANCE+1);
    inflow_precalc.ResizeClear(num_resources, PRECALC_DISTANCE+1);
  }
  geometry.ResizeClear(num_resources);
  
  for (int i = 0; i < num_resources; i++) resources[i] = new cDiffusionRes(); //APW
    
  curr_grid_res_val.ResizeClear(num_resources);
  curr_diffusion_res_val.ResizeClear(num_resources);
  cell_lists.ResizeClear(num_resources);
  resource_names.SetAll("");
  resource_initial.SetAll(0.0);
  resource_count.SetAll(0.0);
  decay_rate.SetAll(0.0);
  inflow_rate.SetAll(0.0);
  decay_precalc.SetAll(1.0); // This is 1-inflow, so there should be no inflow by default, JEB
  inflow_precalc.SetAll(0.0);
  geometry.SetAll(nGeometry::GLOBAL);
  curr_grid_res_val.SetAll(0.0);
}

// --- PRIVATE METHODS --- //
void cPopulationResources::DoUpdates(cAvidaContext& ctx, bool global_only) const
{
  assert(update_time >= -EPSILON);
  
  // Determine how many update steps have progressed
  int num_steps = (int) (update_time / UPDATE_STEP);
  
  // Preserve remainder of update_time
  update_time -=  num_steps * UPDATE_STEP;
  
  
  while (num_steps > PRECALC_DISTANCE) {
    for (int i = 0; i < resource_count.GetSize(); i++) {
      if (geometry[i] == nGeometry::GLOBAL || geometry[i]==nGeometry::PARTIAL) {
        resource_count[i] *= decay_precalc(i, PRECALC_DISTANCE);
        resource_count[i] += inflow_precalc(i, PRECALC_DISTANCE);
      }
    }
    num_steps -= PRECALC_DISTANCE;
  }
  
  for (int i = 0; i < resource_count.GetSize(); i++) {
    if (geometry[i] == nGeometry::GLOBAL || geometry[i]==nGeometry::PARTIAL) {
      resource_count[i] *= decay_precalc(i, num_steps);
      resource_count[i] += inflow_precalc(i, num_steps);
    }
  }
  
  if (global_only) return;
  
  // If one (or more) complete update has occured update the spatial resources
  while (m_spatial_update > m_last_updated) {
    m_last_updated++;
    for (int i = 0; i < resource_count.GetSize(); i++) {
      if (geometry[i] != nGeometry::GLOBAL && geometry[i] != nGeometry::PARTIAL) {
        if (geometry[i] == nGeometry::DYNAMIC) resources[i]->UpdateDynamicRes(ctx);
        else {
          resources[i]->Source(inflow_rate[i]);
          resources[i]->Sink(decay_rate[i]);
          if (resources[i]->GetCellListSize() > 0) {
            resources[i]->CellInflow();
            resources[i]->CellOutflow();
          }
          resources[i]->FlowAll();
          resources[i]->StateAll();
        }
      }
    }
  }
}

cString cPopulationResources::GetGeometryName(const int& in_geometry)
{
  cString geo_name;
  if (in_geometry == nGeometry::GLOBAL) {
    geo_name = "GLOBAL";
    return geo_name;
  } else if (in_geometry == nGeometry::GRID) {
    geo_name = "GRID";
    return geo_name;
  } else if (in_geometry == nGeometry::TORUS) {
    geo_name = "TORUS";
    return geo_name;
  } else if (in_geometry == nGeometry::PARTIAL) {
	geo_name = "PARTIAL";
    return geo_name;
  } else if (in_geometry == nGeometry::DYNAMIC) {
	geo_name = "DYNAMIC";
    return geo_name;
  }
  else {
    cerr << "[cPopulationResources::Setup] Unknown resource geometry " << in_geometry << ".  Exiting.";
    exit(2);
  }
}

const Apto::Array<int> & cPopulationResources::GetResourceGeometries() const
{
  return geometry;
}

void cPopulationResources::UpdateResources(cAvidaContext& ctx, const Apto::Array<double> & res_change)
{
  Modify(ctx, res_change);
}

void cPopulationResources::SetInflow(const cString& name, const double _inflow)
{
  int id = GetResourceCountID(name);
  if (id == -1) return;

  inflow_rate[id] = _inflow;
  double step_inflow = _inflow * UPDATE_STEP;
  double step_decay = pow(decay_rate[id], UPDATE_STEP);

  inflow_precalc(id, 0) = 0.0;
  for (int i = 1; i <= PRECALC_DISTANCE; i++) {
    inflow_precalc(id, i) = inflow_precalc(id, i-1) * step_decay + step_inflow;
  }
}

void cPopulationResources::SetDecay(const cString& name, const double _decay)
{
  int id = GetResourceCountID(name);
  if (id == -1) return;

  decay_rate[id] = _decay;
  double step_decay = pow(_decay, UPDATE_STEP);
  decay_precalc(id, 0) = 1.0;
  for (int i = 1; i <= PRECALC_DISTANCE; i++) {
    decay_precalc(id, i)  = decay_precalc(id, i-1) * step_decay;
  }
}

double cPopulationResources::GetDecay(const cString& name)
{
  int id = GetResourceCountID(name);
  if (id == -1) return -1;
  
  return decay_rate[id];
}

void cPopulationResources::Modify(cAvidaContext& ctx, int res_index, double change)
{
  assert(res_index < resource_count.GetSize());

  DoUpdates(ctx);
  resource_count[res_index] += change;
  assert(resource_count[res_index] >= 0.0);
}

// diffusion resources
const Apto::Array< Apto::Array<double> >& cPopulationResources::GetDiffusionResVals(cAvidaContext& ctx)
{
  const int num_resources = resources.GetSize();
  if (num_resources > 0) {
    const int num_cells = resources[0]->GetSize();
    DoUpdates(ctx);
    for (int i = 0; i < num_resources; i++) {
      for (int j = 0; j < num_cells; j++) {
        curr_diffusion_res_val[i][j] = resources[i]->GetAmount(j);
      }
    }
  }
  return curr_diffusion_res_val;
}

// dynamic resources
void cPopulationResources::ResetDynamicRes(cAvidaContext& ctx, cWorld* world, int res_id)
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  int worldx = resources[res_id]->GetX();
  int worldy = resources[res_id]->GetY();
  
  resources[res_id]->ResetDynamicRes(ctx, worldx, worldy);
}

// deme resources
// Write spatial data to a file that can easily be read into Matlab
void cPopulationResources::PrintDemeSpatialResData(const cPopulationResources& res, const int i, const int deme_id, cAvidaContext&) const {
  const char* tmpResName = res.GetResName(i);
  cString tmpfilename = cStringUtil::Stringf( "deme_spatial_resource_%s.m", tmpResName );
  cDataFile& df = m_world->GetDataFile(tmpfilename);
  cString UpdateStr = cStringUtil::Stringf( "deme_%07i_%s_%07i = [ ...", deme_id, static_cast<const char*>(res.GetResName(i)), m_world->GetStats().GetUpdate() );
  
  df.WriteRaw(UpdateStr);
  
  const cResource& sp_res = res.GetResource(i);
  int gridsize = sp_res.GetSize();
  
  for (int j = 0; j < gridsize; j++) {
    df.WriteBlockElement(sp_res.GetAmount(j), j, worldx);
  }
  df.WriteRaw("];");
  df.Endl();
}

// --- PUBLIC METHODS --- //
void cPopulationResources::SetCellResources(int cell_id, const Apto::Array<double> & res)
{
  assert(resource_count.GetSize() == res.GetSize());

  for (int i = 0; i < resource_count.GetSize(); i++) {
     if (geometry[i] == nGeometry::GLOBAL || geometry[i]==nGeometry::PARTIAL) {
        // Set global quantity of resource
    } else {
      resources[i]->SetCellAmount(cell_id, res[i]);

      /* Ideally the state of the cell's resource should not be set till
         the end of the update so that all processes (inflow, outflow, 
         diffision, gravity and organism demand) have the same weight.  However
         waiting can cause problems with negative resources so we allow
         the organism demand to work immediately on the state of the resource */ 
    }
  }
}

const Apto::Array<double> & cPopulationResources::GetResources(cAvidaContext& ctx) const
{
  DoUpdates(ctx); 
  return resource_count;
}
 
const Apto::Array<double> & cPopulationResources::GetFrozenResources(cAvidaContext& ctx, int cell_id) const
// This differs from GetCellResources by leaving out DoUpdates which is
// useful inside methods that repeatedly call this before cells can change.
{
  int num_resources = resource_count.GetSize();
  
  for (int i = 0; i < num_resources; i++) {
    if (geometry[i] == nGeometry::GLOBAL || geometry[i]==nGeometry::PARTIAL) curr_grid_res_val[i] = resource_count[i];
    else if (geometry[i] == nGeometry::DYNAMIC) curr_grid_res_val[i] = resources[i]->GetAmount(cell_id);
    else curr_grid_res_val[i] = resources[i]->GetAmount(cell_id);
  }
  return curr_grid_res_val;
}

const Apto::Array<double> & cPopulationResources::GetCellResources(int cell_id, cAvidaContext& ctx) const
  // Get amount of the resource for a given cell in the grid.  If it is a
  // global resource pass out the entire content of that resource.
{
  DoUpdates(ctx);
  return GetFrozenResources(ctx, cell_id);
}

const Apto::Array<double>& cPopulationResources::GetDemeCellResources(int deme_id, int cell_id, cAvidaContext& ctx) const
{
  int deme_cell_id = m_interface->GetDeme(deme_id).GetRelativeCellID(cell_id);
  return GetCellResources(deme_cell_id, ctx);
}

void cPopulationResources::UpdateCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id)
{
  ModifyCell(ctx, res_change, cell_id);
}

void cPopulationResources::UpdateResStats(cAvidaContext& ctx) 
{
  cStats& stats = m_world->GetStats();
  stats.SetResources(GetResources(ctx)); 
//  stats.SetSpatialRes(GetSpatialRes(ctx)); //APW
  stats.SetResourceGeometries(GetResourceGeometries());
}

void cPopulationResources::ReinitializeResources(cAvidaContext& ctx, double additional_resource)
{
  for(int i = 0; i < resource_names.GetSize(); i++) {
    Set(ctx, i, resource_initial[i] + additional_resource); //will cause problem if more than one resource is used. -- why?  each resource is stored separately (BDC)

    // Additionally, set any initial values given by the CELL command
    resources[i]->ResetResourceCounts();
    if (additional_resource != 0.0) {
      resources[i]->RateAll(additional_resource);
      resources[i]->StateAll();
    }
  } //End going through the resources
}

void cPopulationResources::ResizeSpatialGrids(int in_x, int in_y, int geometry)
{
  for (int i = 0; i < resource_count.GetSize(); i++) {
    resources[i]->ResizeClear(in_x, in_y, geometry);
    curr_diffusion_res_val[i].Resize(in_x * in_y);
  }
}

void cPopulationResources::Set(cAvidaContext& ctx, int res_index, double new_level)
{
  assert(res_index < resource_count.GetSize());
  DoUpdates(ctx);
  if (geometry[res_index] == nGeometry::GLOBAL || geometry[res_index]==nGeometry::PARTIAL) resource_count[res_index] = new_level;
  else if (geometry[res_index] == nGeometry::DYNAMIC) {
    for (int i = 0; i < resources[res_index]->GetSize(); i++) {
      resources[res_index]->SetCellAmount(i, new_level/resources[res_index]->GetSize());
    }
  }
  else {
    for (int i = 0; i < resources[res_index]->GetSize(); i++) {
      resources[res_index]->SetCellAmount(i, new_level/resources[res_index]->GetSize());
    }
  }
}

void cPopulationResources::SetResource(cAvidaContext& ctx, int res_index, double new_level)
{
  Set(ctx, res_index, new_level);
}

void cPopulationResources::SetResource(cAvidaContext& ctx, const cString res_name, double new_level)
{
  cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(res_name);
  if (res != NULL) SetResource(ctx, res->GetIndex(), new_level);
}

/* This method sets the inflow of the named resource.
 * It changes this value in the environment, then updates it in the
 * actual population's resource count.
 */
void cPopulationResources::SetResourceInflow(const cString res_name, double new_level)
{
  m_world->GetEnvironment().SetResourceInflow(res_name, new_level);
  SetInflow(res_name, new_level);
}

/* This method sets the outflow of the named resource.
 * It changes this value in the enviroment, then updates the
 * decay rate in the resource count (to 1 - the given outflow, as
 * outflow is different than decay).
 */
void cPopulationResources::SetResourceOutflow(const cString res_name, double new_level)
{
  m_world->GetEnvironment().SetResourceOutflow(res_name, new_level);
  SetDecay(res_name, 1 - new_level);
}

double cPopulationResources::Get(cAvidaContext& ctx, int res_index) const
{
  assert(res_index < resource_count.GetSize());
  DoUpdates(ctx);
  if (geometry[res_index] == nGeometry::GLOBAL || geometry[res_index]==nGeometry::PARTIAL) resource_count[res_index];
  else if (geometry[res_index] == nGeometry::DYNAMIC) resources[res_index]->SumAll();
  //else return spacial resource sum
  return resources[res_index]->SumAll();
}

/* 
 * TODO: This is a duplicate of GetResourceCountID()
 * Follow the same steps to get rid of it.
 */
int cPopulationResources::GetResourceByName(cString name) const
{
  int result = -1;
  for (int i = 0; i < resource_names.GetSize(); i++)
  {
    if (resource_names[i] == name) result = i;
  }
  return result;
}

/*
 * This is unnecessary now that a resource has an index
 * TODO: 
 *  - Change name to GetResourceCountIndex
 *  - Fix anything that breaks by just using the index of the resource (not id)
 *  - Get rid of this function
 */
int cPopulationResources::GetResourceCountID(const cString& res_name)
{
    for (int i = 0; i < resource_names.GetSize(); i++) {
      if (resource_names[i] == res_name) return i;
    }
    cerr << "Error: Unknown resource '" << res_name << "'." << endl;
    return -1;
}

double cPopulationResources::GetInflow(const cString& name)
{
  int id = GetResourceCountID(name);
  if (id == -1) return -1;
  
  return inflow_rate[id];
}

double cPopulationResources::GetFrozenCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const
// This differs from GetFrozenCellResources by only pulling for res of interest.
{
  double res_val = 0;
  if (geometry[res_id] == nGeometry::GLOBAL || geometry[res_id]==nGeometry::PARTIAL) res_val = resource_count[res_id];
  else if (geometry[res_id] == nGeometry::DYNAMIC) res_val = resources[res_id]->GetAmount(cell_id);
  else res_val = resources[res_id]->GetAmount(cell_id);
  return res_val;
}

double cPopulationResources::GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const
// This differs from GetCellResources by only pulling for res of interest.
{
  DoUpdates(ctx);
  return GetFrozenCellResVal(ctx, cell_id, res_id);
}

void cPopulationResources::Update(double in_time) 
{ 
  update_time += in_time;
  spatial_update_time += in_time;
}

void cPopulationResources::UpdateResource(cAvidaContext& ctx, int res_index, double change)
{
  Modify(ctx, res_index, change);
}

void cPopulationResources::UpdateResource(const int Verbosity, cWorld* world) {
  const cResourceDefLib & resource_lib = m_world->GetEnvironment().GetResDefLib();
  int global_res_index = -1;
  int deme_res_index = -1;
  int num_deme_res = 0;
  
  //setting size of global and deme-level resources
  for(int i = 0; i < resource_lib.GetSize(); i++) {
    cResourceDef* res = resource_lib.GetResDef(i);
    if (res->GetDemeResource())
      num_deme_res++;
  }
  
  for(int i = 0; i < m_interface->GetNumDemes(); i++) {
    cPopulationResources tmp_deme_res_count(m_interface, num_deme_res); // APW
//    m_interface->GetDeme(i).SetDemeResourceCount(tmp_deme_res_count);
  }
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResourceDef* res = resource_lib.GetResDef(i);
    if (!res->GetDemeResource()) {
      global_res_index++;
      const double decay = 1.0 - res->GetOutflow();
      if (!res->IsSpatial()) {
        SetupGlobalRes(m_world, global_res_index, res->GetName(), res->GetInitial(),
                                 res->GetInflow(), decay, res->GetGeometry(), m_world->GetVerbosity());
      }
      else {
        if (res->IsDynamic()) {
          SetupDynamicRes(m_world, global_res_index, res, m_world->GetVerbosity());
        }
        else if (res->IsDiffusion()) {
          SetupDiffusionRes(m_world, global_res_index, res->GetName(), res->GetInitial(), res->GetInflow(), decay,
                                      res->GetGeometry(), res->GetXDiffuse(),
                                      res->GetXGravity(), res->GetYDiffuse(),
                                      res->GetYGravity(), res->GetInflowX1(),
                                      res->GetInflowX2(), res->GetInflowY1(),
                                      res->GetInflowY2(), res->GetOutflowX1(),
                                      res->GetOutflowX2(), res->GetOutflowY1(),
                                      res->GetOutflowY2(), res->GetCellListPtr(),
                                      res->GetCellIdListPtr(), m_world->GetVerbosity());
        }
        else
          cerr<< "ERROR: Spatial resource \"" << res->GetName() <<"\"is not a diffusion or dynamic resource.  Exit";
        exit(1);
      }
    } else if (res->GetDemeResource()) {
      deme_res_index++;
      for(int j = 0; j < m_interface->GetNumDemes(); j++) {
        m_interface->GetDeme(j).SetupDemeRes(deme_res_index, res, Verbosity, world);
        // could add deme resources to global resource stats here
      }
    } else {
      cerr<< "ERROR: Resource \"" << res->GetName() <<"\"is not a global or deme resource.  Exit";
      exit(1);
    }
  }  
}

void cPopulationResources::Modify(cAvidaContext& ctx, const Apto::Array<double> & res_change)
{
  assert(resource_count.GetSize() == res_change.GetSize());

  DoUpdates(ctx);
  for (int i = 0; i < resource_count.GetSize(); i++) {
    resource_count[i] += res_change[i];
    assert(resource_count[i] >= 0.0);
  }
}

void cPopulationResources::ModifyCell(cAvidaContext& ctx, const Apto::Array<double> & res_change, int cell_id)
{
  assert(resource_count.GetSize() == res_change.GetSize());
  
  DoUpdates(ctx);
  for (int i = 0; i < resource_count.GetSize(); i++) {
    if (geometry[i] == nGeometry::GLOBAL || geometry[i]==nGeometry::PARTIAL) {
      resource_count[i] += res_change[i];
      assert(resource_count[i] >= 0.0);
    }
    else if (geometry[i] == nGeometry::DYNAMIC) {
      double temp = resources[i]->Element(cell_id).GetAmount();
      if (resources[i]->Element(cell_id).GetAmount() != temp) resources[i]->SetModified(true);
      assert(resources[i]->Element(cell_id).GetAmount() >= 0.0);
    }
    else {
      double temp = resources[i]->Element(cell_id).GetAmount();
      resources[i]->Rate(cell_id, res_change[i]);
      /* Ideally the state of the cell's resource should not be set till
       the end of the update so that all processes (inflow, outflow,
       diffision, gravity and organism demand) have the same weight.  However
       waiting can cause problems with negative resources so we allow
       the organism demand to work immediately on the state of the resource */
      
      resources[i]->State(cell_id);
      if(resources[i]->Element(cell_id).GetAmount() != temp){
        resources[i]->SetModified(true);
      }
      assert(resources[i]->Element(cell_id).GetAmount() >= 0.0);
    }
  }
}

// --- GLOBAL RESOURCES --- //
void cPopulationResources::SetupGlobalRes(cWorld* world, const int& res_index, const cString& name, const double& initial, const double& inflow, const double& decay,
				const int& in_geometry, const int& verbosity_level)
{
  assert(res_index >= 0 && res_index < resource_count.GetSize());
  assert(initial >= 0.0);
  assert(decay >= 0.0);
  assert(inflow >= 0.0);

  cString geo_name = GetGeometryName(in_geometry);

  /* If the verbose flag is set print out information about resources */
  verbosity = verbosity_level;
  if (verbosity > VERBOSE_NORMAL) {
    cout << "Setting up resource " << name
         << "(" << geo_name 
         << ") with initial quantity=" << initial
         << ", decay=" << decay
         << ", inflow=" << inflow
         << endl;
  }   
  
  /* recource_count gets only the values for global resources */

  resource_names[res_index] = name;
  resource_initial[res_index] = initial;
  if (in_geometry == nGeometry::GLOBAL) resource_count[res_index] = initial;
  else if (in_geometry == nGeometry::PARTIAL) resource_count[res_index]=initial;
  else resource_count[res_index] = 0;

  decay_rate[res_index] = decay;
  inflow_rate[res_index] = inflow;
  geometry[res_index] = in_geometry;

  double step_decay = pow(decay, UPDATE_STEP);
  double step_inflow = inflow * UPDATE_STEP;
  
  decay_precalc(res_index, 0) = 1.0;
  inflow_precalc(res_index, 0) = 0.0;
  for (int i = 1; i <= PRECALC_DISTANCE; i++) {
    decay_precalc(res_index, i)  = decay_precalc(res_index, i-1) * step_decay;
    inflow_precalc(res_index, i) = inflow_precalc(res_index, i-1) * step_decay + step_inflow;
  }
}

// --- DIFFUSION RESOURCES --- //
void cPopulationResources::SetupDiffusionRes(cWorld* world, const int& res_index, const cString& name, const double& initial, const double& inflow, const double& decay,
				const int& in_geometry, const double& in_xdiffuse, const double& in_xgravity, 
				const double& in_ydiffuse, const double& in_ygravity,
				const int& in_inflowX1, const int& in_inflowX2, const int& in_inflowY1, const int& in_inflowY2,
				const int& in_outflowX1, const int& in_outflowX2, const int& in_outflowY1, 
				const int& in_outflowY2, Apto::Array<cCellResource> *in_cell_list_ptr,
				Apto::Array<int> *in_cell_id_list_ptr, const int& verbosity_level)
{
  assert(res_index >= 0 && res_index < resource_count.GetSize());
  assert(initial >= 0.0);
  assert(decay >= 0.0);
  assert(inflow >= 0.0);
  assert(resources[res_index]->GetSize() > 0);

  cString geo_name = GetGeometryName(in_geometry);

  /* If the verbose flag is set print out information about resources */
  verbosity = verbosity_level;
  if (verbosity > VERBOSE_NORMAL) {
    cout << "Setting up resource " << name
         << "(" << geo_name 
         << ") with initial quantity=" << initial
         << ", decay=" << decay
         << ", inflow=" << inflow
         << endl;
    if ((in_geometry == nGeometry::GRID) || (in_geometry == nGeometry::TORUS)) {
      cout << "  Inflow rectangle (" << in_inflowX1 
           << "," << in_inflowY1 
           << ") to (" << in_inflowX2 
           << "," << in_inflowY2 
           << ")" << endl; 
      cout << "  Outflow rectangle (" << in_outflowX1 
           << "," << in_outflowY1 
           << ") to (" << in_outflowX2 
           << "," << in_outflowY2 
           << ")" << endl;
      cout << "  xdiffuse=" << in_xdiffuse
           << ", xgravity=" << in_xgravity
           << ", ydiffuse=" << in_ydiffuse
           << ", ygravity=" << in_ygravity
           << endl;
    }   
  }
  

  /* recource_count gets only the values for global resources */
  
  resource_names[res_index] = name;
  resource_initial[res_index] = initial;

  delete resources[res_index];
  resources[res_index] = new cDiffusionRes();
  
  if (in_geometry == nGeometry::GLOBAL) {
    resource_count[res_index] = initial;
	resources[res_index]->RateAll(0);
  }
  else if (in_geometry == nGeometry::PARTIAL) {
    resource_count[res_index]=initial;    
    resources[res_index]->RateAll(0);
    // want to set list of cell ids here
    cell_lists[res_index].Resize(in_cell_id_list_ptr->GetSize());
    for (int i = 0; i < in_cell_id_list_ptr->GetSize(); i++)
      cell_lists[res_index][i] = (*in_cell_id_list_ptr)[i];
  }
  else {
    resource_count[res_index] = 0; 
    resources[res_index]->SetInitial(initial / resources[res_index]->GetSize());
    resources[res_index]->RateAll(resources[res_index]->GetInitial());
  }
  resources[res_index]->StateAll();  
  decay_rate[res_index] = decay;
  inflow_rate[res_index] = inflow;
  geometry[res_index] = in_geometry;
  resources[res_index]->SetGeometry(in_geometry);
  resources[res_index]->SetPointers();
  resources[res_index]->SetCellList(in_cell_list_ptr);

  double step_decay = pow(decay, UPDATE_STEP);
  double step_inflow = inflow * UPDATE_STEP;
  
  decay_precalc(res_index, 0) = 1.0;
  inflow_precalc(res_index, 0) = 0.0;
  for (int i = 1; i <= PRECALC_DISTANCE; i++) {
    decay_precalc(res_index, i)  = decay_precalc(res_index, i-1) * step_decay;
    inflow_precalc(res_index, i) = inflow_precalc(res_index, i-1) * step_decay + step_inflow;
  }
  resources[res_index]->SetXdiffuse(in_xdiffuse);
  resources[res_index]->SetXgravity(in_xgravity);
  resources[res_index]->SetYdiffuse(in_ydiffuse);
  resources[res_index]->SetYgravity(in_ygravity);
  resources[res_index]->SetInflowX1(in_inflowX1);
  resources[res_index]->SetInflowX2(in_inflowX2);
  resources[res_index]->SetInflowY1(in_inflowY1);
  resources[res_index]->SetInflowY2(in_inflowY2);
  resources[res_index]->SetOutflowX1(in_outflowX1);
  resources[res_index]->SetOutflowX2(in_outflowX2);
  resources[res_index]->SetOutflowY1(in_outflowY1);
  resources[res_index]->SetOutflowY2(in_outflowY2);
}

// --- DYNAMIC RESOURCES --- //
void cPopulationResources::SetupDynamicRes(cWorld* world, const int& res_index, cResourceDef* res_def, const int& verbosity_level)
{
  assert(res_index >= 0 && res_index < resource_count.GetSize());
//  assert(resources[res_index]->GetSize() > 0); // APW

  cString geo_name = GetGeometryName(res_def->GetGeometry());

  /* If the verbose flag is set print out information about resources */
  verbosity = verbosity_level;
  if (verbosity > VERBOSE_NORMAL) cout << "Setting up resource " << res_def->GetName() << "(" << geo_name << ")" << endl;

  resource_names[res_index] = res_def->GetName();
  resource_initial[res_index] = res_def->GetInitial();
  resource_count[res_index] = 0;

  delete resources[res_index];
  resources[res_index] = new cDynamicRes(m_world, *res_def, m_world->GetConfig().WORLD_X.Get(), m_world->GetConfig().WORLD_Y.Get());
  m_world->GetEnvironment().GetResDefLib().SetResDef(*res_def, res_index);

  geometry[res_index] = res_def->GetGeometry();
}

void cPopulationResources::SetDynamicResPlatVarInflow(const cString res_name, const double mean, const double variance, const int type)
{
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResourceDef* res = resource_lib.GetResDef(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
    resources[global_res_index]->SetPlatVarInflow(mean, variance, type);
    }
  } 
}

void cPopulationResources::SetPredatoryResource(const cString res_name, const double odds, const int juvsper, const double detection_prob)
{
  const cResourceDefLib & resource_lib = m_world->GetEnvironment().GetResDefLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResourceDef* res = resource_lib.GetResDef(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
      res->SetPredatoryResource(odds, juvsper, detection_prob);
      res->SetHabitat(5);
      m_world->GetEnvironment().AddHabitat(5);
      resources[global_res_index]->GetResDef()->SetPredatoryResource(odds, juvsper);
      break;
    }
  }
  m_has_predatory_res = true; 
}

int cPopulationResources::GetCurrPeakX(cAvidaContext& ctx, int res_id) const
{ 
  DoUpdates(ctx);
  return resources[res_id]->GetCurrPeakX();
}

int cPopulationResources::GetCurrPeakY(cAvidaContext& ctx, int res_id) const
{ 
  DoUpdates(ctx);
  return resources[res_id]->GetCurrPeakY();
}

int cPopulationResources::GetFrozenPeakX(cAvidaContext&, int res_id) const
{ 
  return resources[res_id]->GetCurrPeakX();
}

int cPopulationResources::GetFrozenPeakY(cAvidaContext&, int res_id) const
{ 
  return resources[res_id]->GetCurrPeakY();
}

Apto::Array<int>* cPopulationResources::GetWallCells(int res_id)
{
  return resources[res_id]->GetWallCells();
}

int cPopulationResources::GetMinUsedX(int res_id)
{
  return resources[res_id]->GetMinUsedX();
}

int cPopulationResources::GetMinUsedY(int res_id)
{
  return resources[res_id]->GetMinUsedY();
}

int cPopulationResources::GetMaxUsedX(int res_id)
{
  return resources[res_id]->GetMaxUsedX();
}

int cPopulationResources::GetMaxUsedY(int res_id)
{
  return resources[res_id]->GetMaxUsedY();
}

/* This routine is designed to change resource definitions in the middle of a run */
void cPopulationResources::UpdateDynamicRes(cAvidaContext& ctx, cWorld* world, const cString res_name)
{
  const cResourceDefLib & resource_lib = m_world->GetEnvironment().GetResDefLib();
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResourceDef* res_def = resource_lib.GetResDef(i);
    if (res_def->GetName() == res_name) {
      ResetDynamicRes(ctx, world, res_def->GetID());
    } 
  }
}

void cPopulationResources::ExecutePredatoryResource(cAvidaContext& ctx, const int cell_id, const double pred_odds, const int juvs_per)
{
  cPopulationCell& cell = m_interface->GetCell(cell_id);
  const int juv_age = m_world->GetConfig().JUV_PERIOD.Get();
  
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  
  bool cell_has_den = false;
  for (int j = 0; j < resource_lib.GetSize(); j++) {
    if (resource_lib.GetResDef(j)->GetHabitat() == 4 || resource_lib.GetResDef(j)->GetHabitat() == 3) {
      if (GetCellResVal(ctx, cell_id, j) > 0) {
        cell_has_den = true;
        break;
      }
    }
  }
  
  if (m_world->GetConfig().USE_AVATARS.Get() && cell.HasAV()) {
    Apto::Array<cOrganism*> cell_avs = cell.GetCellAVs();
    
    // on den, kill juvs only
    if (cell_has_den) {
      Apto::Array<cOrganism*> juvs;   
      juvs.Resize(0);
      int num_juvs = 0;
      int num_guards = 0;
      for (int k = 0; k < cell_avs.GetSize(); k++) {
        if (cell_avs[k]->GetPhenotype().GetTimeUsed() < juv_age) { 
          num_juvs++;
          juvs.Push(cell_avs[k]);
        }
        else if (cell_avs[k]->IsGuard()) num_guards++;
      }
      if (num_juvs > 0) {
        int guarded_juvs = num_guards * juvs_per;
        int unguarded_juvs = num_juvs - guarded_juvs;
        for (int k = 0; k < unguarded_juvs; k++) {
          if (ctx.GetRandom().P(pred_odds) && !juvs[k]->IsDead()) {
            if (!juvs[k]->IsRunning()) m_interface->KillOrganism(ctx, juvs[k]->GetCellID());
            else {
                juvs[k]->GetPhenotype().SetToDie();
                m_world->GetStats().IncJuvKilled();
            }
          }
        }
      }
    }
    // away from den, kill anyone
    else {
      if (ctx.GetRandom().P(pred_odds)) {
        cOrganism* target_org = cell_avs[m_world->GetRandom().GetUInt(cell_avs.GetSize())];
        if (!target_org->IsDead()) {
          if (!target_org->IsRunning()) m_interface->KillOrganism(ctx, target_org->GetCellID());
          else target_org->GetPhenotype().SetToDie();
        }
      }
    }
  }
  else if (!m_world->GetConfig().USE_AVATARS.Get() && cell.IsOccupied()) {
    cOrganism* target_org = cell.GetOrganism();
    // if not avatars, a juv will be killed regardless of whether it is on a den
    // an adult would only be targeted off of a den
    if (target_org->GetPhenotype().GetTimeUsed() < juv_age || !cell_has_den) {
      if (ctx.GetRandom().P(pred_odds) && !target_org->IsDead()) {
          if (!target_org->IsRunning()) m_interface->KillOrganism(ctx, target_org->GetCellID());
          else target_org->GetPhenotype().SetToDie();
      }
    }
  }
}

/* This method sets a deme resource to the same level across
 * all demes.  If a resource by the given name does not exist,
 * it does nothing.
 */
void cPopulationResources::SetDemeResource(cAvidaContext& ctx, const cString res_name, double new_level)
{
  cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(res_name);
  if (res != NULL) {
    int num_demes = m_interface->GetNumDemes();
    for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
      m_interface->GetDeme(deme_id).SetResource(ctx, res->GetIndex(), new_level);
    }
  }
}

/* This method sets the inflow for the named deme resource across ALL demes. 
 * It changes the value in the environment, then updates it in the deme resource
 * counts.
 *
 * This maintains the connection between the enviroment value and the resource
 * count values, unlike cPopulationResources::SetSingleDemeResourceInflow()
 */
void cPopulationResources::SetDemeResourceInflow(const cString res_name, double new_level)
{
  m_world->GetEnvironment().SetResourceInflow(res_name, new_level);
  int num_demes = m_interface->GetNumDemes();
  for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    m_interface->GetDeme(deme_id).GetDemeResources().SetInflow(res_name, new_level);
  }
}

/* This method sets the inflow for the named deme resource in a specific deme.
 * It changes the value in the environment, then updates it in the specified deme's
 * resource count.
 * 
 * ATTENTION: This leads to the rather bizzare consequence that the inflow rate
 * in the environment may not match the inflow rate in each deme's resource count.
 * This is not my own decision, simply a reflection of how the SetDemeResourceInflow
 * action (for which I am writing this as a helper) works.  Unless you have a specific
 * reason NOT to change the inflow for all demes, it is probably best to use
 * cPopulationResources::SetDemeResourceInflow() -- blw
 */
void cPopulationResources::SetSingleDemeResourceInflow(int deme_id, const cString res_name, double new_level)
{
  m_world->GetEnvironment().SetResourceInflow(res_name, new_level);
  m_interface->GetDeme(deme_id).GetDemeResources().SetInflow(res_name, new_level);
}

/* This method sets the outflow for the named deme resource across ALL demes.
 * It changes the value in the environment, then updates the decay rate in the 
 * deme resource counts.
 *
 * This maintains the connection between the enviroment value and the resource
 * count values, unlike cPopulationResources::SetSingleDemeResourceOutflow()
 */
void cPopulationResources::SetDemeResourceOutflow(const cString res_name, double new_level)
{
  m_world->GetEnvironment().SetResourceOutflow(res_name, new_level);
  int num_demes = m_interface->GetNumDemes();
  for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    m_interface->GetDeme(deme_id).GetDemeResources().SetDecay(res_name, 1 - new_level);
  }
}

/* This method sets the outflow for the named deme resource in a specific deme. 
 * It changes the value in the environment, then updates the decay rate in the 
 * specified deme's resource count.
 * 
 * ATTENTION: This leads to the rather bizzare consequence that the outflow rate
 * in the environment may not match the decay (1-outflow) rate in each deme's resource count.
 * This is not my own decision, simply a reflection of how the SetDemeResourceOutflow
 * action (for which I am writing this as a helper) works.  Unless you have a specific
 * reason NOT to change the outflow for all demes, it is probably best to use
 * cPopulationResources::SetDemeResourceOutflow() -- blw
 */
void cPopulationResources::SetSingleDemeResourceOutflow(int deme_id, const cString res_name, double new_level)
{
  m_world->GetEnvironment().SetResourceOutflow(res_name, new_level);
  m_interface->GetDeme(deme_id).GetDemeResources().SetDecay(res_name, 1 - new_level);
}

void cPopulationResources::UpdateDemeCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id)
{
  m_interface->GetDeme(m_interface->GetCell(cell_id).GetDemeID()).ModifyDemeResCount(ctx, res_change, cell_id);
}

void cPopulationResources::PrintDemeResource(cAvidaContext& ctx) {
  cStats& stats = m_world->GetStats();
  const int num_demes = m_interface->GetNumDemes();
  cDataFile & df_resources = m_world->GetDataFile("deme_resources.dat");
  df_resources.WriteComment("Avida deme resource data");
  df_resources.WriteTimeStamp();
  df_resources.Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    m_interface->GetDeme(deme_id).UpdateDemeRes(ctx);
    
    const cPopulationResources& res = m_interface->GetDeme(deme_id).GetDemeResourceCount();
    for(int j = 0; j < res.GetSize(); j++) {
      const char * tmp = res.GetResName(j);
      df_resources.Write(res.Get(ctx, j), cStringUtil::Stringf("Deme %d Resource %s", deme_id, tmp)); //comment);
      if ((res.GetResourceGeometries())[j] != nGeometry::GLOBAL && (res.GetResourceGeometries())[j] != nGeometry::PARTIAL) {
        PrintDemeSpatialResData(res, j, deme_id, ctx);
      }
    }
  }
  df_resources.Endl();
}

//Write deme global resource levels to a file that can be easily read into Matlab.
//Each time this runs, a Matlab array is created that contains an array.  Each row in the array contains <deme id> <res level 0> ... <res level n>
void cPopulationResources::PrintDemeGlobalResources(cAvidaContext& ctx) { 
  const int num_demes = m_interface->GetNumDemes();
  cDataFile & df = m_world->GetDataFile("deme_global_resources.dat");
  df.WriteComment("Avida deme resource data");
  df.WriteTimeStamp();
  
  cString UpdateStr = cStringUtil::Stringf( "deme_global_resources_%07i = [ ...", m_world->GetStats().GetUpdate());
  df.WriteRaw(UpdateStr);
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    m_interface->GetDeme(deme_id).UpdateDemeRes(ctx);
    
    const cPopulationResources & res = m_interface->GetDeme(deme_id).GetDemeResourceCount();
    const int num_res = res.GetSize();
    
    df.WriteBlockElement(deme_id, 0, num_res + 1);
    
    for(int r = 0; r < num_res; r++) {
      if (!res.IsSpatial(r)) {
        df.WriteBlockElement(res.Get(ctx, r), r + 1, num_res + 1);
      }
      
    } //End iterating through resources
    
  } //End iterating through demes
  
  df.WriteRaw("];");
  df.Endl();
}

// --- DYNAMIC RESOURCES --- //

/*!	Modify current level of the HGT resource. */
void cPopulationResources::AdjustHGTResource(cAvidaContext& ctx, double delta)
{
  if (m_hgt_resid != -1) {
    Modify(ctx, m_hgt_resid, delta);
  }
}
