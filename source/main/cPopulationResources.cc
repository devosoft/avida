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

#include "cResourceElement.h"
#include "cWorld.h"


const double cPopulationResources::UPDATE_STEP(1.0 / 10000.0);
const double cPopulationResources::EPSILON (1.0e-15);
const int cPopulationResources::PRECALC_DISTANCE(100);


void FlowMatter(cResourceElement &elem1, cResourceElement &elem2,
                double inxdiffuse, 
                double inydiffuse, double inxgravity, double inygravity,
                int xdist, int ydist, double dist) {

  /* Routine to calculate the amount of flow from one Element to another.
     Amount of flow is a function of:

       1) Amount of material in each cell (will try to equalize)
       2) Distance between each cell
       3) x and y "gravity"

     This method only effect the delta amount of each element.  The State
     method will need to be called at the end of each time step to complete
     the movement of material.
  */

  double  diff, flowamt, xgravity, xdiffuse, ygravity,  ydiffuse;

  if (((elem1.amount == 0.0) && (elem2.amount == 0.0)) && (dist < 0.0)) return;
  diff = (elem1.amount - elem2.amount);
  if (xdist != 0) {

    /* if there is material to be effected by x gravity */

    if (((xdist>0) && (inxgravity>0.0)) || ((xdist<0) && (inxgravity<0.0))) {
      xgravity = elem1.amount * fabs(inxgravity)/3.0;
    } else {
      xgravity = -elem2.amount * fabs(inxgravity)/3.0;
    }
    
    /* Diffusion uses the diffusion constant x half the difference (as the 
       elements attempt to equalize) / the number of possible neighbors (8) */

    xdiffuse = inxdiffuse * diff / 16.0;
  } else {
    xdiffuse = 0.0;
    xgravity = 0.0;
  }  
  if (ydist != 0) {

    /* if there is material to be effected by y gravity */

    if (((ydist>0) && (inygravity>0.0)) || ((ydist<0) && (inygravity<0.0))) {
      ygravity = elem1.amount * fabs(inygravity)/3.0;
    } else {
      ygravity = -elem2.amount * fabs(inygravity)/3.0;
    }
    ydiffuse = inydiffuse * diff / 16.0;
  } else {
    ydiffuse = 0.0;
    ygravity = 0.0;
  }  

  flowamt = ((xdiffuse + ydiffuse + xgravity + ygravity)/
             (fabs(xdist*1.0) + fabs(ydist*1.0)))/dist;
  elem1.delta -= flowamt;
  elem2.delta += flowamt;
}

cPopulationResources::cPopulationResources(int num_resources)
  : update_time(0.0)
  , spatial_update_time(0.0)
  , m_last_updated(0)
  , m_spatial_update(0)
{
  if(num_resources > 0) {
    SetSize(num_resources);
  }

  return;
}

cPopulationResources::cPopulationResources(const cPopulationResources &rc) {
  *this = rc;

  return;
}

const cPopulationResources &cPopulationResources::operator=(const cPopulationResources &rc) {
  SetSize(rc.GetSize());
  resource_names = rc.resource_names;
  resource_initial = rc.resource_initial;  
  resource_count = rc.resource_count;
  decay_rate = rc.decay_rate;
  inflow_rate = rc.inflow_rate;
  decay_precalc = rc.decay_precalc;
  inflow_precalc = rc.inflow_precalc;
  geometry = rc.geometry;
  
  for (int i = 0; i < rc.resources.GetSize(); i++) { 
    *(resources[i]) = *(rc.resources[i]);
  }
  
  curr_grid_res_val = rc.curr_grid_res_val;
  curr_diffusion_res_val = rc.curr_diffusion_res_val;
  update_time = rc.update_time;
  spatial_update_time = rc.spatial_update_time;
  cell_lists = rc.cell_lists;

  return *this;
}

void cPopulationResources::SetSize(int num_resources)
{
  resource_names.ResizeClear(num_resources);
  resource_initial.ResizeClear(num_resources);
  resource_count.ResizeClear(num_resources);
  decay_rate.ResizeClear(num_resources);
  inflow_rate.ResizeClear(num_resources);
  if(num_resources > 0) {
    decay_precalc.ResizeClear(num_resources, PRECALC_DISTANCE+1);
    inflow_precalc.ResizeClear(num_resources, PRECALC_DISTANCE+1);
  }
  geometry.ResizeClear(num_resources);
  
  for (int i = 0; i < resources.GetSize(); i++) {
    delete resources[i]; 
  }
  
  resources.ResizeClear(num_resources);
  
  for(int i = 0; i < num_resources; i++){
    resources[i] = new cResource; 
  }
    
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
  //DO spacial resources need to be set to zero?
}

cPopulationResources::~cPopulationResources()
{
  for (int i = 0; i < resources.GetSize(); i++) {
    delete resources[i]; 
  }
}

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
  }
  else {
    cerr << "[cResources::Setup] Unknown resource geometry " << in_geometry << ".  Exiting.";
    exit(2);
  }
}

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

void cPopulationResources::SetupDynamicRes(cWorld* world, const int& res_index, cResourceDef* res_def, const int& verbosity_level)
{
  assert(res_index >= 0 && res_index < resource_count.GetSize());
  assert(resources[res_index]->GetSize() > 0);

  cString geo_name = GetGeometryName(res_def->GetGeometry());

  /* If the verbose flag is set print out information about resources */
  verbosity = verbosity_level;
  if (verbosity > VERBOSE_NORMAL) cout << "Setting up resource " << res_def->GetName() << "(" << geo_name << ")" << endl;

  resource_names[res_index] = res_def->GetName();
  resource_initial[res_index] = res_def->GetInitial();
  resource_count[res_index] = 0;
  delete resources[res_index];
  resources[res_index] = new cResource();
  
  geometry[res_index] = res_def->GetGeometry();
}

void cPopulationResources::ResetDynamicRes(cAvidaContext& ctx, cWorld* world, int res_id)
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  int worldx = resources[res_id]->GetX();
  int worldy = resources[res_id]->GetY();
  
  resources[res_id]->ResetDynamicRes(ctx, worldx, worldy);
}

void cPopulationResources::SetDynamicResPlateauInflow(const int& res_id, const double& inflow)
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  resources[res_id]->GetResDef()->SetPlateauInflow(inflow);
}

void cPopulationResources::SetDynamicResPlateauOutflow(const int& res_id, const double& outflow) 
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  resources[res_id]->GetResDef()->SetPlateauOutflow(outflow);
}

void cPopulationResources::SetDynamicResConeInflow(const int& res_id, const double& inflow) 
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  resources[res_id]->GetResDef()->SetConeInflow(inflow);
}

void cPopulationResources::SetDynamicResConeOutflow(const int& res_id, const double& outflow)
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  resources[res_id]->GetResDef()->SetConeOutflow(outflow);
}

void cPopulationResources::SetDynamicResInflow(const int& res_id, const double& inflow) 
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  resources[res_id]->GetResDef()->SetDynamicResInflow(inflow);
}

void cPopulationResources::SetDynamicResPlatVarInflow(const int& res_id, const double& mean, const double& variance, const int& type)
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  resources[res_id]->SetPlatVarInflow(mean, variance, type);
}

void cPopulationResources::SetPredatoryResource(const int& res_id, const double& odds, const int& juvsper) 
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  resources[res_id]->GetResDef()->SetPredatoryResource(odds, juvsper);
}

void cPopulationResources::SetProbabilisticResource(cAvidaContext& ctx, const int& res_id, const double& initial, const double& inflow, 
                                              const double& outflow, const double& lambda, const double& theta, const int& x, const int& y, const int& count) 
{
  assert(res_id >= 0 && res_id < resource_count.GetSize());
  assert(resources[res_id]->GetSize() > 0);
  resources[res_id]->GetResDef()->SetProbabilisticResource(ctx, initial, inflow, outflow, lambda, theta, x, y, count);
  resources[res_id]->BuildProbabilisticRes(ctx, lambda, theta, x, y, count);
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

double cPopulationResources::GetDecay(const cString& name)
{
  int id = GetResourceCountID(name);
  if (id == -1) return -1;
  
  return decay_rate[id];
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

void cPopulationResources::Update(double in_time) 
{ 
  update_time += in_time;
  spatial_update_time += in_time;
 }

 
const Apto::Array<double> & cPopulationResources::GetResources(cAvidaContext& ctx) const
{
  DoUpdates(ctx); 
  return resource_count;
}
 
const Apto::Array<double> & cPopulationResources::GetCellResources(int cell_id, cAvidaContext& ctx) const
  // Get amount of the resource for a given cell in the grid.  If it is a
  // global resource pass out the entire content of that resource.
{
  DoUpdates(ctx);
  return GetFrozenResources(ctx, cell_id);
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

double cPopulationResources::GetCellResVal(cAvidaContext& ctx, int cell_id, int res_id) const
// This differs from GetCellResources by only pulling for res of interest.
{
  DoUpdates(ctx);
  return GetFrozenCellResVal(ctx, cell_id, res_id);
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

const Apto::Array<int> & cPopulationResources::GetResourceGeometries() const
{
  return geometry;
}

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

void cPopulationResources::Modify(cAvidaContext& ctx, const Apto::Array<double> & res_change)
{
  assert(resource_count.GetSize() == res_change.GetSize());

  DoUpdates(ctx);
  for (int i = 0; i < resource_count.GetSize(); i++) {
    resource_count[i] += res_change[i];
    assert(resource_count[i] >= 0.0);
  }
}

void cPopulationResources::Modify(cAvidaContext& ctx, int res_index, double change)
{
  assert(res_index < resource_count.GetSize());

  DoUpdates(ctx);
  resource_count[res_index] += change;
  assert(resource_count[res_index] >= 0.0);
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

double cPopulationResources::Get(cAvidaContext& ctx, int res_index) const
{
  assert(res_index < resource_count.GetSize());
  DoUpdates(ctx);
  if (geometry[res_index] == nGeometry::GLOBAL || geometry[res_index]==nGeometry::PARTIAL) resource_count[res_index];
  else if (geometry[res_index] == nGeometry::DYNAMIC) resources[res_index]->SumAll();
  //else return spacial resource sum
  return resources[res_index]->SumAll();
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

void cPopulationResources::ResizeSpatialGrids(int in_x, int in_y, int geometry)
{
  for (int i = 0; i < resource_count.GetSize(); i++) {
    resources[i]->ResizeClear(in_x, in_y, geometry);
    curr_diffusion_res_val[i].Resize(in_x * in_y);
    resources[i]->ResizeClear(in_x, in_y, geometry);
  }
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

///// Private Methods /////////
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
