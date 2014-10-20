/*
 *  cPopulation.cc
 *  Avida
 *
 *  Called "population.cc" prior to 12/5/05.
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

#include "cPopulation.h"

#include "avida/core/Feedback.h"
#include "avida/core/InstructionSequence.h"
#include "avida/core/Properties.h"
#include "avida/data/Manager.h"
#include "avida/data/Package.h"
#include "avida/data/Util.h"
#include "avida/output/File.h"
#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Manager.h"

#include "avida/private/systematics/GenomeTestMetrics.h"
#include "avida/private/systematics/Genotype.h"

#include "apto/rng.h"
#include "apto/scheduler.h"
#include "apto/stat/Accumulator.h"

#include "AvidaTools.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cCodeLabel.h"
#include "cDemePlaceholderUnit.h"
#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "cMigrationMatrix.h"   
#include "cOrganism.h"
#include "cParasite.h"
#include "cPhenotype.h"
#include "cPopulationCell.h"
#include "cResource.h"
#include "cResourceCount.h"
#include "cStats.h"
#include "cTestCPU.h"
#include "cTopology.h"
#include "cWorld.h"

#include "cHardwareCPU.h"

#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>
#include <cfloat>
#include <cmath>
#include <climits>
#include <limits>

using namespace std;
using namespace AvidaTools;

static const PropertyID s_prop_id_instset("instset");


cPopulationOrgStatProvider::~cPopulationOrgStatProvider() { ; }


class InstructionExecCountsProvider : public cPopulationOrgStatProvider
{
private:
  cWorld* m_world;
  Apto::Map<Apto::String, Apto::Array<Apto::Stat::Accumulator<int> >, Apto::DefaultHashBTree, Apto::ImplicitDefault> m_is_exe_inst_map;
  Data::DataSetPtr m_provides;

public:
  InstructionExecCountsProvider(cWorld* world) : m_world(world), m_provides(new Data::DataSet)
  {
    m_provides->Insert(Apto::String("core.population.inst_exec_counts[]"));

    cHardwareManager& hwm = m_world->GetHardwareManager();
    for (int i = 0; i < hwm.GetNumInstSets(); i++) {
      m_is_exe_inst_map[Apto::String((const char*)hwm.GetInstSet(i).GetInstSetName())].Resize(hwm.GetInstSet(i).GetSize());
    }
  }
  
  Data::ConstDataSetPtr Provides() const { return m_provides; }
  void UpdateProvidedValues(Update current_update) { (void)current_update; }
  
  Apto::String DescribeProvidedValue(const Apto::String& data_id) const
  {
    Apto::String rtn;
    if (data_id == "core.population.inst_exec_counts[]") {
      rtn = "Instruction execution counts for the specified instruction set.";
    }
    return rtn;
  }
  
  void SetActiveArguments(const Data::DataID& data_id, Data::ConstArgumentSetPtr args) { (void)data_id; (void)args; }
  
  Data::ConstArgumentSetPtr GetValidArguments(const Data::DataID& data_id) const
  {
    Data::ArgumentSetPtr args(new Data::ArgumentSet);
    
    for (int i = 0; i < m_world->GetHardwareManager().GetNumInstSets(); i++) {
      args->Insert(Apto::String((const char*)m_world->GetHardwareManager().GetInstSet(i).GetInstSetName()));
    }
    
    return args;
  }

  bool IsValidArgument(const Data::DataID& data_id, Data::Argument arg) const
  {
    return GetValidArguments(data_id)->Has(arg);
  }
  

  Data::PackagePtr GetProvidedValueForArgument(const Data::DataID& data_id, const Data::Argument& arg) const
  {
    Apto::SmartPtr<Data::ArrayPackage, Apto::InternalRCObject> pkg(new Data::ArrayPackage);
    
    const Apto::Array<Apto::Stat::Accumulator<int> >& inst_exe_counts = m_is_exe_inst_map[arg];
    for (int i = 0; i < inst_exe_counts.GetSize(); i++) {
      pkg->AddComponent(Data::PackagePtr(new Data::Wrap<int>(inst_exe_counts[i].Sum())));
    }
    
    return pkg;
  }
  
  
  void UpdateReset()
  {
    for (Apto::Map<Apto::String, Apto::Array<Apto::Stat::Accumulator<int> > >::ValueIterator it = m_is_exe_inst_map.Values(); it.Next();) {
      Apto::Array<Apto::Stat::Accumulator<int> >& inst_counts = (*it.Get());
      for (int i = 0; i < inst_counts.GetSize(); i++) inst_counts[i].Clear();
    }
  }
  
  void HandleOrganism(cOrganism* organism)
  {
    Apto::String inst_set = organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue();
    Apto::Array<Apto::Stat::Accumulator<int> >& inst_exe_counts = m_is_exe_inst_map[inst_set];
    for (int j = 0; j < organism->GetPhenotype().GetLastInstCount().GetSize(); j++) {
      inst_exe_counts[j].Add(organism->GetPhenotype().GetLastInstCount()[j]);
    }
  }
  
  static Data::ArgumentedProviderPtr Activate(cWorld* world, World* new_world)
  {
    (void)new_world;
    cPopulationOrgStatProviderPtr osp(new InstructionExecCountsProvider(world));
    world->GetPopulation().AttachOrgStatProvider(osp);
    return osp;
  }
};

class FromMessageInstructionExecCountsProvider : public cPopulationOrgStatProvider
{
private:
  cWorld* m_world;
  Apto::Map<Apto::String, Apto::Array<Apto::Stat::Accumulator<int> >, Apto::DefaultHashBTree, Apto::ImplicitDefault> m_is_exe_inst_map;
  Data::DataSetPtr m_provides;
  
public:
  FromMessageInstructionExecCountsProvider(cWorld* world) : m_world(world), m_provides(new Data::DataSet)
  {
    m_provides->Insert(Apto::String("core.population.from_message_inst_exec_counts[]"));
    
    cHardwareManager& hwm = m_world->GetHardwareManager();
    for (int i = 0; i < hwm.GetNumInstSets(); i++) {
      m_is_exe_inst_map[Apto::String((const char*)hwm.GetInstSet(i).GetInstSetName())].Resize(hwm.GetInstSet(i).GetSize());
    }
  }
  
  Data::ConstDataSetPtr Provides() const { return m_provides; }
  void UpdateProvidedValues(Update current_update) { (void)current_update; }
  
  Apto::String DescribeProvidedValue(const Apto::String& data_id) const
  {
    Apto::String rtn;
    if (data_id == "core.population.from_message_inst_exec_counts[]") {
      rtn = "Instruction execution counts from message data for the specified instruction set.";
    }
    return rtn;
  }
  
  void SetActiveArguments(const Data::DataID& data_id, Data::ConstArgumentSetPtr args) { (void)data_id; (void)args; }
  
  Data::ConstArgumentSetPtr GetValidArguments(const Data::DataID& data_id) const
  {
    Data::ArgumentSetPtr args(new Data::ArgumentSet);
    
    for (int i = 0; i < m_world->GetHardwareManager().GetNumInstSets(); i++) {
      args->Insert(Apto::String((const char*)m_world->GetHardwareManager().GetInstSet(i).GetInstSetName()));
    }
    
    return args;
  }
  
  bool IsValidArgument(const Data::DataID& data_id, Data::Argument arg) const
  {
    return GetValidArguments(data_id)->Has(arg);
  }
  
  
  Data::PackagePtr GetProvidedValueForArgument(const Data::DataID& data_id, const Data::Argument& arg) const
  {
    Apto::SmartPtr<Data::ArrayPackage, Apto::InternalRCObject> pkg(new Data::ArrayPackage);
    
    const Apto::Array<Apto::Stat::Accumulator<int> >& inst_exe_counts = m_is_exe_inst_map[arg];
    for (int i = 0; i < inst_exe_counts.GetSize(); i++) {
      pkg->AddComponent(Data::PackagePtr(new Data::Wrap<int>(inst_exe_counts[i].Sum())));
    }
    
    return pkg;
  }
  
  
  void UpdateReset()
  {
    for (Apto::Map<Apto::String, Apto::Array<Apto::Stat::Accumulator<int> > >::ValueIterator it = m_is_exe_inst_map.Values(); it.Next();) {
      Apto::Array<Apto::Stat::Accumulator<int> >& inst_counts = (*it.Get());
      for (int i = 0; i < inst_counts.GetSize(); i++) inst_counts[i].Clear();
    }
  }
  
  void HandleOrganism(cOrganism* organism)
  {
    Apto::String inst_set = organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue();
    Apto::Array<Apto::Stat::Accumulator<int> >& inst_exe_counts = m_is_exe_inst_map[inst_set];
    for (int j = 0; j < organism->GetPhenotype().GetLastFromMessageInstCount().GetSize(); j++) {
      inst_exe_counts[j].Add(organism->GetPhenotype().GetLastFromMessageInstCount()[j]);
    }
  }
  
  static Data::ArgumentedProviderPtr Activate(cWorld* world, World* new_world)
  {
    (void)new_world;
    cPopulationOrgStatProviderPtr osp(new FromMessageInstructionExecCountsProvider(world));
    world->GetPopulation().AttachOrgStatProvider(osp);
    return osp;
  }
};



cPopulation::cPopulation(cWorld* world)  
: m_world(world)
, m_scheduler(NULL)
, birth_chamber(world)
, print_mini_trace_genomes(false)
, use_micro_traces(false)
, m_next_prey_q(0)
, m_next_pred_q(0)
, environment(world->GetEnvironment())
, num_organisms(0)
, num_prey_organisms(0)
, num_pred_organisms(0)
, num_top_pred_organisms(0)
, sync_events(false)
, m_hgt_resid(-1)
{
  world_x = world->GetConfig().WORLD_X.Get();
  world_y = world->GetConfig().WORLD_Y.Get();
  
  
  // Validate settings
  if (m_world->GetConfig().ENERGY_CAP.Get() == -1) m_world->GetConfig().ENERGY_CAP.Set(std::numeric_limits<double>::max());
  
  
  // The following combination of options creates an infinite rotate-loop:
  assert(!((m_world->GetConfig().DEMES_ORGANISM_PLACEMENT.Get()==0) && (m_world->GetConfig().DEMES_ORGANISM_FACING.Get()==1)
           && (m_world->GetConfig().WORLD_GEOMETRY.Get()==1)));
  
  // Incompatible deme replication strategies:
  assert(!(m_world->GetConfig().DEMES_REPLICATE_SIZE.Get() && (m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get()>0.0)));
  assert(!(m_world->GetConfig().DEMES_USE_GERMLINE.Get() && (m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get()>0.0)));
  assert(!(m_world->GetConfig().DEMES_USE_GERMLINE.Get() && (m_world->GetConfig().MIGRATION_RATE.Get()>0.0)));
  
  
  SetupCellGrid();
  
  Data::ArgumentedProviderActivateFunctor activate(m_world, &cWorld::GetPopulationProvider);
  m_world->GetDataManager()->Register("core.population.group_id[]", activate);

  Apto::Functor<Data::ArgumentedProviderPtr, Apto::TL::Create<cWorld*, World*> > is_activate(&InstructionExecCountsProvider::Activate);
  Data::ArgumentedProviderActivateFunctor isp_activate(Apto::BindFirst(is_activate, m_world));
  m_world->GetDataManager()->Register("core.population.inst_exec_counts[]", isp_activate);
  
  Apto::Functor<Data::ArgumentedProviderPtr, Apto::TL::Create<cWorld*, World*> > fmis_activate(&FromMessageInstructionExecCountsProvider::Activate);
  Data::ArgumentedProviderActivateFunctor fmisp_activate(Apto::BindFirst(fmis_activate, m_world));
  m_world->GetDataManager()->Register("core.population.from_message_inst_exec_counts[]", fmisp_activate);
  
}


void cPopulation::ClearCellGrid()
{
  delete sleep_log; sleep_log = NULL;
  reaper_queue.Clear();
  delete m_scheduler; m_scheduler = NULL;
}


void cPopulation::SetupCellGrid()
{
  int num_demes = m_world->GetConfig().NUM_DEMES.Get();
  const int num_cells = world_x * world_y;
  const int geometry = m_world->GetConfig().WORLD_GEOMETRY.Get();
  
  if (m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1) sleep_log = new Apto::Array<pair<int,int>, Apto::Smart>(world_x * world_y);
  else sleep_log = NULL;
  
  if (num_demes <= 0) num_demes = 1; // One population == one deme.  
  assert(num_demes == 1 || m_world->GetConfig().BIRTH_METHOD.Get() != POSITION_OFFSPRING_FULL_SOUP_ELDEST);
  
  // Allocate the cells, resources, and market.
  cell_array.ResizeClear(num_cells);
  empty_cell_id_array.ResizeClear(cell_array.GetSize());
  for (int i = 0; i < empty_cell_id_array.GetSize(); i++) {
    empty_cell_id_array[i] = i;
  }
  
  // Setup the cells.  Do things that are not dependent upon topology here.
  bool fill_reaper_queue = (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST);
  for (int i = 0; i < num_cells; i++) {
    cell_array[i].Setup(m_world, i, environment.GetMutRates(), i % world_x, i / world_x);    
    if (fill_reaper_queue) reaper_queue.Push(&(cell_array[i]));
  }
  
  // What are the sizes of the demes that we're creating?
  const int deme_size_x = world_x;
  const int deme_size_y = world_y / num_demes;
  const int deme_size = deme_size_x * deme_size_y;
  deme_array.ResizeClear(num_demes);
  
  // Broken setting:
  assert(m_world->GetConfig().DEMES_REPLICATE_SIZE.Get() <= deme_size);
  
  // Setup the deme structures.
  Apto::Array<int> deme_cells(deme_size);
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    for (int offset = 0; offset < deme_size; offset++) {
      int cell_id = deme_id * deme_size + offset;
      deme_cells[offset] = cell_id;
      cell_array[cell_id].SetDemeID(deme_id);
    }
    deme_array[deme_id].Setup(deme_id, deme_cells, deme_size_x, m_world);
  }
  
  // Setup the topology.
  // What we're doing here is chopping the cell_array up into num_demes pieces.
  // Note that having 0 demes (one population) is the same as having 1 deme.  Then
  // we send the cells that comprise each deme into the topology builder.
  for (int i = 0; i < num_cells; i += deme_size) {
    // We're cheating here; we're using the random access nature of an iterator to index beyond the end of the cell_array.
    switch(geometry) {
      case nGeometry::GRID:
        build_grid(cell_array.Range(i, i + deme_size - 1), deme_size_x, deme_size_y);
        break;
      case nGeometry::TORUS:
        build_torus(cell_array.Range(i, i + deme_size - 1), deme_size_x, deme_size_y);
        break;
      case nGeometry::CLIQUE:
        build_clique(cell_array.Range(i, i + deme_size - 1), deme_size_x, deme_size_y);
        break;
      case nGeometry::HEX:
        build_hex(cell_array.Range(i, i + deme_size - 1), deme_size_x, deme_size_y);
        break;
      case nGeometry::LATTICE:
        build_lattice(cell_array.Range(i, i + deme_size - 1), deme_size_x, deme_size_y, 1);
        break;
      case nGeometry::RANDOM_CONNECTED:
        build_random_connected_network(cell_array.Range(i, i + deme_size - 1), deme_size_x, deme_size_y, m_world->GetRandom());
        break;
      case nGeometry::SCALE_FREE:
        build_scale_free(cell_array.Range(i, i + deme_size - 1), m_world->GetConfig().SCALE_FREE_M.Get(),
                         m_world->GetConfig().SCALE_FREE_ALPHA.Get(), m_world->GetConfig().SCALE_FREE_ZERO_APPEAL.Get(),
                         m_world->GetRandom());
        break;
      default:
        assert(false);
    }
  }
  
  BuildTimeSlicer();
  
  
  // Setup the resources...
  const cResourceLib& resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  int deme_res_index = -1;
  int num_deme_res = 0;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) if (resource_lib.GetResource(i)->GetDemeResource()) num_deme_res++;
  
  cResourceCount tmp_res_count(resource_lib.GetSize() - num_deme_res);
  resource_count = tmp_res_count;
  resource_count.ResizeSpatialGrids(world_x, world_y);
  
  for(int i = 0; i < GetNumDemes(); i++) {
    cResourceCount tmp_deme_res_count(num_deme_res);
    GetDeme(i).SetDemeResourceCount(tmp_deme_res_count);
    GetDeme(i).ResizeSpatialGrids(deme_size_x, deme_size_y);
  }
  
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource* res = resource_lib.GetResource(i);
    
    // check to see if this is the hgt resource:
    if (res->GetHGTMetabolize()) {
      if (m_hgt_resid != -1) {
        m_world->GetDriver().Feedback().Error("Only one HGT resource is currently supported.");
        m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
      }
      m_hgt_resid = i;
    }
    
    if (!res->GetDemeResource()) {
      global_res_index++;
      const double decay = 1.0 - res->GetOutflow();
      resource_count.Setup(m_world, global_res_index, res->GetName(), res->GetInitial(),
                           res->GetInflow(), decay,
                           res->GetGeometry(), res->GetXDiffuse(),
                           res->GetXGravity(), res->GetYDiffuse(),
                           res->GetYGravity(), res->GetInflowX1(),
                           res->GetInflowX2(), res->GetInflowY1(),
                           res->GetInflowY2(), res->GetOutflowX1(),
                           res->GetOutflowX2(), res->GetOutflowY1(),
                           res->GetOutflowY2(), res->GetCellListPtr(),
                           res->GetCellIdListPtr(), m_world->GetVerbosity(),
                           res->GetPeaks(),
                           res->GetMinHeight(), res->GetMinRadius(), res->GetRadiusRange(),
                           res->GetAh(), res->GetAr(),
                           res->GetAcx(), res->GetAcy(),
                           res->GetHStepscale(), res->GetRStepscale(),
                           res->GetCStepscaleX(), res->GetCStepscaleY(),
                           res->GetHStep(), res->GetRStep(),
                           res->GetCStepX(), res->GetCStepY(),
                           res->GetUpdateDynamic(), res->GetPeakX(), res->GetPeakY(),
                           res->GetHeight(), res->GetSpread(), res->GetPlateau(), res->GetDecay(),
                           res->GetMaxX(), res->GetMinX(), res->GetMaxY(), res->GetMinY(), res->GetAscaler(),res->GetUpdateStep(),
                           res->GetHalo(), res->GetHaloInnerRadius(), res->GetHaloWidth(),
                           res->GetHaloAnchorX(), res->GetHaloAnchorY(), res->GetMoveSpeed(), res->GetMoveResistance(),
                           res->GetPlateauInflow(), res->GetPlateauOutflow(), res->GetConeInflow(), res->GetConeOutflow(), 
                           res->GetGradientInflow(), res->GetIsPlateauCommon(), res->GetFloor(), res->GetHabitat(), 
                           res->GetMinSize(), res->GetMaxSize(), res->GetConfig(), res->GetCount(), res->GetResistance(), res->GetDamage(),
                           res->GetDeathOdds(), res->IsPath(), res->IsHammer(), res->GetInitialPlatVal(), res->GetThreshold(), res->GetRefuge(), res->GetGradient()
                           ); 
      m_world->GetStats().SetResourceName(global_res_index, res->GetName());
    } else if (res->GetDemeResource()) {
      deme_res_index++;
      for(int j = 0; j < GetNumDemes(); j++) {
        GetDeme(j).SetupDemeRes(deme_res_index, res, m_world->GetVerbosity(), m_world);                                  
        // could add deme resources to global resource stats here
      }
    } else {
      m_world->GetDriver().Feedback().Error("Resource \"%s\"is not a global or deme resource.", (const char*)res->GetName());
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
  }
  
  // if HGT is on, make sure there's a resource for it:
  if (m_world->GetConfig().ENABLE_HGT.Get() && (m_hgt_resid == -1)) {
    m_world->GetDriver().Feedback().Warning("HGT is enabled, but no HGT resource is defined; add hgt=1 to a single resource in the environment file.");
  }
}



void cPopulation::ResizeCellGrid(int x, int y)
{
  ClearCellGrid();
  world_x = x;
  world_y = y;
  SetupCellGrid();
}




Data::ConstDataSetPtr cPopulation::Provides() const
{
  Data::DataSetPtr provides(new Apto::Set<Apto::String>);
  provides->Insert("core.population.group_id[]");
  return provides;
}

void cPopulation::UpdateProvidedValues(Update)
{
  // Nothing for now, all handled elsewhere
}

Data::PackagePtr cPopulation::GetProvidedValueForArgument(const Apto::String& data_id, const Data::Argument& arg) const
{
  Data::PackagePtr rtn;
  
  if (data_id == "core.population.group_id[]") {
    Apto::String larg = arg;
    Systematics::RoleID role(larg.Pop('@'));
    if (role.GetSize() > 0 && larg.GetSize() > 2) {
      int x = Apto::StrAs(larg.Pop(','));
      if (!larg.GetSize() || !larg.IsNumber(0)) return rtn;
      int y = Apto::StrAs(larg);
      if (x >= 0 && x < world_x && y >= 0 && y < world_y) {
        // Valid X and Y coordinates, return genotype ID @ cell if applicable
        const cPopulationCell& cell = cell_array[x + (y * world_x)];
        if (cell.IsOccupied()) {
          Systematics::GroupPtr grp(cell.GetOrganism()->SystematicsGroup(role));
          if (grp) rtn = Data::PackagePtr(new Data::Wrap<int>(grp->ID()));
        }
      }
    }
  }
  
  return rtn;
}

Apto::String cPopulation::DescribeProvidedValue(const Apto::String& data_id) const
{
  Apto::String rtn;
  if (data_id == "core.population.group_id[]") {
    rtn = "Group ID for specified cell of the specified systemtics role";
  }
  return rtn;
}

bool cPopulation::SupportsConcurrentUpdate() const
{ 
  return true;
}


void cPopulation::SetActiveArguments(const Data::DataID&, Data::ConstArgumentSetPtr)
{
  // Nothing to do here
}


Data::ConstArgumentSetPtr cPopulation::GetValidArguments(const Data::DataID& data_id) const
{
  Data::ArgumentSetPtr arg_set;
  
  if (Data::IsStandardID(data_id)) return arg_set;
  
  Apto::String argument;
  if (data_id == "core.population.group_id[]") {
    for (int y = 0; y < world_y; y++) {
      for (int x = 0; x < world_x; x++) {
        argument = "{systematics_role}@";
        argument += Apto::AsStr(x);
        argument += ",";
        argument += Apto::AsStr(y);
        
        arg_set->Insert(argument);
      }
    }
  }
  
  return arg_set;
}

bool cPopulation::IsValidArgument(const Data::DataID& data_id, Data::Argument arg) const
{
  if (Data::IsStandardID(data_id)) return false;
  
  if (data_id == "core.population.group_id[]") {
    Apto::String larg = arg;
    Systematics::RoleID role(larg.Pop('@'));
    if (role.GetSize() > 0 && larg.GetSize() > 2) {
      int x = Apto::StrAs(larg.Pop(','));
      if (!larg.GetSize() || !larg.IsNumber(0)) return false;
      int y = Apto::StrAs(larg);
      if (x >= 0 && x < world_x && y >= 0 && y < world_y) return true;
    }
  }
  
  return false;
}




cPopulation::~cPopulation()
{
  for (int i = 0; i < cell_array.GetSize(); i++) delete cell_array[i].GetOrganism(); 
  delete m_scheduler;
}


inline void cPopulation::AdjustSchedule(const cPopulationCell& cell, const cMerit& merit)
{
  const int deme_id = cell.GetDemeID();
  const cDeme& deme = deme_array[deme_id];
  m_scheduler->AdjustPriority(cell.GetID(), deme.HasDemeMerit() ? (merit.GetDouble() * deme.GetDemeMerit().GetDouble()) : merit.GetDouble());
}



// Activate the child, given information from the parent.
// Return true if parent lives through this process.
bool cPopulation::ActivateOffspring(cAvidaContext& ctx, const Genome& offspring_genome, cOrganism* parent_organism)
{
  assert(parent_organism != NULL);
  bool is_doomed = false;
  int doomed_cell = (world_x * world_y) - 1; //Also at the end of cPopulation::ActivateOrganism
  Apto::Array<cOrganism*> offspring_array;
  Apto::Array<cMerit> merit_array;
  
  // If divide method is split, parent will be reset to completely tolerant
  // must remove their intolerance from the group's cached total.
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
      int tol_max = m_world->GetConfig().MAX_TOLERANCE.Get();
      int group_id = parent_organism->GetOpinion().first;
      int org_imm_tolerance = parent_organism->GetPhenotype().CalcToleranceImmigrants();
      m_group_intolerances[group_id][0].second -= tol_max - org_imm_tolerance; 
      if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
        if (parent_organism->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) {
          m_group_intolerances_females[group_id][0].second -= tol_max - org_imm_tolerance;
        } else if (parent_organism->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) {
          m_group_intolerances_males[group_id][0].second -= tol_max - org_imm_tolerance;
        } else if (parent_organism->GetPhenotype().GetMatingType() == MATING_TYPE_JUVENILE) {
          m_group_intolerances_juvs[group_id][0].second -= tol_max - org_imm_tolerance;
        }
      }
      m_group_intolerances[group_id][1].second -= tol_max - parent_organism->GetPhenotype().CalcToleranceOffspringOthers();
    }
  }
  
  // Update the parent's phenotype.
  // This needs to be done before the parent goes into the birth chamber
  // or the merit doesn't get passed onto the offspring correctly
  UpdateQs(parent_organism, true);
  cPhenotype& parent_phenotype = parent_organism->GetPhenotype();
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(parent_organism->GetGenome().Representation());
  parent_phenotype.DivideReset(*seq);
  
  GeneticRepresentationPtr tmpHostGenome;
  
  if (m_world->GetConfig().HOST_USE_GENOTYPE_FILE.Get())
  {
    tmpHostGenome = host_genotype_list[m_world->GetRandom().GetInt(host_genotype_list.GetSize())];
  }
  else
  {
    tmpHostGenome = GeneticRepresentationPtr(new InstructionSequence(offspring_genome.Representation()->AsString()));
  }
  
  Genome temp(parent_organism->GetGenome().HardwareType(), parent_organism->GetGenome().Properties(), tmpHostGenome);
  birth_chamber.SubmitOffspring(ctx, temp, parent_organism, offspring_array, merit_array);
    
  // First, setup the genotype of all of the offspring.
  const int parent_id = parent_organism->GetOrgInterface().GetCellID();
  assert(parent_id >= 0 && parent_id < cell_array.GetSize());
  cPopulationCell& parent_cell = cell_array[parent_id];
  
  // If this is multi-process Avida, test to see if we should send the offspring
  // to a different world.  We check this here so that 1) we avoid all the extra
  // work below in the case of a migration event and 2) so that we don't mess up
  // and mistakenly kill the parent.
  if (m_world->GetConfig().ENABLE_MP.Get()) {
    Apto::Array<cOrganism*> non_migrants;
    Apto::Array<cMerit> non_migrant_merits;
    for (int i=0; i<offspring_array.GetSize(); ++i) {
      if (m_world->TestForMigration()) {
        // this offspring is outta here!
        m_world->MigrateOrganism(offspring_array[i], parent_cell, merit_array[i], parent_organism->GetLineageLabel());
        delete offspring_array[i]; // this offspring isn't hanging around.
      } else {
        // boring; stay here.
        non_migrants.Push(offspring_array[i]);
        non_migrant_merits.Push(merit_array[i]);
      }
    }
    offspring_array = non_migrants;
    merit_array = non_migrant_merits;
  }
  
  Apto::Array<int> target_cells(offspring_array.GetSize());
  
  // Loop through choosing the later placement of each offspring in the population.
  bool parent_alive = true;  // Will the parent live through this process?
  
  for (int i = 0; i < offspring_array.GetSize(); i++) {
    target_cells[i] = PositionOffspring(parent_cell, ctx, m_world->GetConfig().ALLOW_PARENT.Get()).GetID(); 
    // Catch the corner case where birth method = 3 and there are 
    // no empty cells. Here, we set the cell to -1 so that the rest of the
    // method can proceed, but we can avoid trying to rotate it.
    if ((target_cells[i] == parent_cell.GetID()) && 
        m_world->GetConfig().ALLOW_PARENT.Get()==0) {
      target_cells[i] = -1;
      continue;
    } 
    // If we replaced the parent, make a note of this.    
    if (target_cells[i] == parent_cell.GetID()) {
      parent_alive = false;
      if (m_world->GetConfig().USE_AVATARS.Get()) parent_organism->GetOrgInterface().RemoveAllAV();
    }
    const int mut_source = m_world->GetConfig().MUT_RATE_SOURCE.Get();
    if (mut_source == 1) {
      // Update the mutation rates of each offspring from the environment....
      offspring_array[i]->MutationRates().Copy(GetCell(target_cells[i]).MutationRates());
    } 
    else {
      // Update the mutation rates of each offspring from its parent.
      offspring_array[i]->MutationRates().Copy(parent_organism->MutationRates());
      // If there is a meta-mutation rate, do tests for it.
      if (offspring_array[i]->MutationRates().GetMetaCopyMutProb() > 0.0) {
        offspring_array[i]->MutationRates().DoMetaCopyMut(ctx);
      }
    }
    
    // Update the phenotypes of each offspring....
    ConstInstructionSequencePtr seq;
    seq.DynamicCastFrom(offspring_array[i]->GetGenome().Representation());
    const InstructionSequence& genome = *seq;
    offspring_array[i]->GetPhenotype().SetupOffspring(parent_phenotype, genome);
    offspring_array[i]->GetPhenotype().SetMerit(merit_array[i]);
    offspring_array[i]->SetLineageLabel(parent_organism->GetLineageLabel());
    
    //By default, store the parent cclade, this may get modified in ActivateOrgansim (@MRR)
    offspring_array[i]->SetCCladeLabel(parent_organism->GetCCladeLabel());
    
    // If inherited reputation is turned on, set the offspring's reputation
    // to that of its parent.
    if (m_world->GetConfig().INHERIT_REPUTATION.Get() == 1) {
      offspring_array[i]->SetReputation(parent_organism->GetReputation());
    } 
    else if (m_world->GetConfig().INHERIT_REPUTATION.Get() == 2) {
      offspring_array[i]->SetTag(parent_organism->GetTag());
    }
    else if (m_world->GetConfig().INHERIT_REPUTATION.Get() == 3) {
      offspring_array[i]->SetTag(parent_organism->GetTag());
      offspring_array[i]->SetReputation(parent_organism->GetReputation());
    }
    
    // If we're using host-controlled parasite virulence, i.e. the host donates cycles to symbiont and evolves that donation amount @AEJ
    if (m_world->GetConfig().VIRULENCE_SOURCE.Get() == 2)
    {
      //mutate virulence
      // m_world->GetConfig().PARASITE_VIRULENCE.Get()
      double oldVir = parent_organism->GetParaDonate();
    
      //default to not mutating
      double newVir = oldVir;
    
      //but if we do mutate...
      if (m_world->GetRandom().GetDouble() < m_world->GetConfig().VIRULENCE_MUT_RATE.Get())
      {
        //get this in a temp variable so we don't have to make the next line huge
        double vir_sd = m_world->GetConfig().VIRULENCE_SD.Get();
      
        //sd^2 = varience
        newVir = m_world->GetRandom().GetRandNormal(oldVir, vir_sd * vir_sd);
      
      }
      offspring_array[i]->SetParaDonate(Apto::Max(Apto::Min(newVir, 1.0), 0.0));
    }
    
    // If spatial groups are used, put the offspring in the
    // parents' group, if tolerances are used check if the offspring
    // is successfully born into the parent's group or successfully immigrates
    // into another group.
    if (m_world->GetConfig().USE_FORM_GROUPS.Get()) {
      if (parent_organism->HasOpinion()) offspring_array[i]->SetParentGroup(parent_organism->GetOpinion().first);
      // If tolerances are on ... 
      if (m_world->GetConfig().TOLERANCE_WINDOW.Get() != 0 && m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 0) {
        bool joins_group = AttemptOffspringParentGroup(ctx, parent_organism, offspring_array[i]);
        if (!joins_group) {
          target_cells[i] = doomed_cell;
          is_doomed = true;
        }
      }
      else {
        // If not using tolerances or using immigrant only tolerance, put the offspring in the parent's group.
        assert(parent_organism->HasOpinion());
        if (m_world->GetConfig().INHERIT_OPINION.Get()) {
          int group = parent_organism->GetOpinion().first;
          offspring_array[i]->SetOpinion(group);
          JoinGroup(offspring_array[i], group);
        }
      }
    }
    if (m_world->GetConfig().SET_FT_AT_BIRTH.Get()) {
      int prop_target = 2;
      if (ctx.GetRandom().P(0.5)) {
        prop_target = 0;
        if (ctx.GetRandom().P(0.5)) prop_target = 1;
      }
      if (m_world->GetConfig().MAX_PREY_BT.Get()) {
        int in_use = 0;
        Apto::Array<cOrganism*> orgs;
        const Apto::Array<cOrganism*, Apto::Smart>& live_orgs = m_world->GetPopulation().GetLiveOrgList();
        for (int i = 0; i < live_orgs.GetSize(); i++) {
          cOrganism* org = live_orgs[i];
          int this_target = org->GetForageTarget();
          if (this_target == prop_target) {
            in_use++;
            orgs.Push(org);
          }
        }
        if (in_use >= m_world->GetConfig().MAX_PREY_BT.Get()) {
          cOrganism* org = orgs[ctx.GetRandom().GetUInt(0, in_use)];
          if (org == parent_organism) {
            parent_alive = false;
          }
          org->Die(ctx);
        }
      }
      offspring_array[i]->SetForageTarget(ctx, prop_target);
      offspring_array[i]->RecordFTSet();
    }
    // if parent org has executed teach_offspring intruction, allow the offspring to learn parent's foraging/targeting behavior
    if (parent_organism->IsTeacher()) offspring_array[i]->SetParentTeacher(true);
    offspring_array[i]->SetParentFT(parent_organism->GetForageTarget());
    // and some rebirth stuff
    offspring_array[i]->SetParentMerit(parent_organism->GetPhenotype().GetMerit().GetDouble());
    offspring_array[i]->SetParentMultiThreaded(parent_organism->GetPhenotype().IsMultiThread());    
  }
  
  // If we're not about to kill the parent, do some extra work on it.
  if (parent_alive == true) {
    if (parent_phenotype.GetMerit().GetDouble() <= 0.0 || m_world->GetConfig().BIRTH_METHOD.Get() == 13) {
      // no weakling parents either!
      parent_organism->GetPhenotype().SetToDie();
      parent_alive = false;
    }
    else {
      // Reset inputs and re-calculate merit if required
      if (m_world->GetConfig().RESET_INPUTS_ON_DIVIDE.Get() > 0){
        environment.SetupInputs(ctx, parent_cell.m_inputs);
        
        int pc_phenotype = m_world->GetConfig().PRECALC_PHENOTYPE.Get();
        if (pc_phenotype) {
          cCPUTestInfo test_info;
          cTestCPU* test_cpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
          test_info.UseManualInputs(parent_cell.GetInputs()); // Test using what the environment will be
          Genome mg(parent_organism->GetGenome().HardwareType(),
                    parent_organism->GetGenome().Properties(),
                    GeneticRepresentationPtr(new InstructionSequence(parent_organism->GetHardware().GetMemory())));
          test_cpu->TestGenome(ctx, test_info, mg); // Use the true genome
          if (pc_phenotype & 1) {  // If we must update the merit
            parent_phenotype.SetMerit(test_info.GetTestPhenotype().GetMerit());
          }
          if (pc_phenotype & 2) {  // If we must update the gestation time
            parent_phenotype.SetGestationTime(test_info.GetTestPhenotype().GetGestationTime());
          }
          if (pc_phenotype & 4) {  // If we must update the last instruction counts
            parent_phenotype.SetTestCPUInstCount(test_info.GetTestPhenotype().GetLastInstCount());
          }
          parent_phenotype.SetFitness(parent_phenotype.GetMerit().CalcFitness(parent_phenotype.GetGestationTime())); // Update fitness
          delete test_cpu;
        }
      }
      AdjustSchedule(parent_cell, parent_phenotype.GetMerit());
      
      if (!is_doomed) {
        // In a local run, face the offspring toward the parent.
        const int birth_method = m_world->GetConfig().BIRTH_METHOD.Get();
        if (birth_method < NUM_LOCAL_POSITION_OFFSPRING || birth_method == POSITION_OFFSPRING_PARENT_FACING) {
          for (int i = 0; i < offspring_array.GetSize(); i++) {
            if (target_cells[i] != -1) {
              GetCell(target_cells[i]).Rotate(parent_cell);
            }
          }
        }
      }
    }
  }
  
  // Do any statistics on the parent that just gave birth...
  parent_organism->HandleGestation();
  
  // Place all of the offspring...
  for (int i = 0; i < offspring_array.GetSize(); i++) {
    if (target_cells[i] != -1) {
      //@JEB - we may want to pass along some state information from parent to offspring
      if ( (m_world->GetConfig().EPIGENETIC_METHOD.Get() == EPIGENETIC_METHOD_OFFSPRING)
          || (m_world->GetConfig().EPIGENETIC_METHOD.Get() == EPIGENETIC_METHOD_BOTH) ) {
        offspring_array[i]->GetHardware().InheritState(parent_organism->GetHardware());
      }
      bool org_survived = ActivateOrganism(ctx, offspring_array[i], GetCell(target_cells[i]));
      // only assign an avatar cell if the org lived through birth and it isn't the parent
      if (m_world->GetConfig().USE_AVATARS.Get() && org_survived) {
        int avatar_target_cell = PlaceAvatar(ctx, parent_organism);
        if (avatar_target_cell != -1) {
          offspring_array[i]->GetPhenotype().SetAVBirthCellID(avatar_target_cell);
          offspring_array[i]->GetOrgInterface().TryWriteBirthLocData(offspring_array[i]->GetOrgIndex());
          if (offspring_array[i] != parent_organism) {
            offspring_array[i]->GetOrgInterface().AddPredPreyAV(ctx, avatar_target_cell);
          }
          if (m_world->GetConfig().AVATAR_BIRTH_FACING.Get() == 1) {
            const int rots = ctx.GetRandom().GetUInt(0,8);
            for (int j = 0; j < rots; j++) offspring_array[i]->Rotate(ctx, rots);
          }
        }
        else KillOrganism(GetCell(target_cells[i]), ctx);
      }
    } else {
      delete offspring_array[i];
    }
  }
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT && parent_alive && m_world->GetConfig().RESET_INPUTS_ON_DIVIDE.Get()) TestForMiniTrace(parent_organism);
  return parent_alive;
}

void cPopulation::UpdateQs(cOrganism* org, bool reproduced)
{
  // yank the org out of any current trace queues, as appropriate (i.e. if dead (==!reproduced) or if reproduced and splitting on divide)
  bool split = (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT && m_world->GetConfig().RESET_INPUTS_ON_DIVIDE.Get());
  
  if (!reproduced || (reproduced && split)) {
    org->GetHardware().PrintMicroTrace(org->SystematicsGroup("genotype")->ID());
    org->GetHardware().DeleteMiniTrace(print_mini_trace_reacs, (reproduced && split));
  }
  
  if (org->GetHardware().IsReproTrace() && repro_q.GetSize()) {
    for (int i = 0; i < repro_q.GetSize(); i++) {
      if (repro_q[i] == org) {
        if (reproduced) m_world->GetStats().PrintReproData(org);
        if ((!reproduced || (reproduced && split)) && repro_q.GetSize()) {
          int last = repro_q.GetSize() - 1;
          repro_q.Swap(i, last);
          repro_q.Pop();
          org->GetHardware().SetReproTrace(false);
        }
        break;
      }
    }
  }
  if (org->GetHardware().IsTopNavTrace() && topnav_q.GetSize()) {
    for (int i = 0; i < topnav_q.GetSize(); i++) {
      if (topnav_q[i] == org) {
        if (reproduced) m_world->GetStats().UpdateTopNavTrace(org);
        if ((!reproduced || (reproduced && split)) && topnav_q.GetSize()) { //nav_q can be wiped in update process
          int last = topnav_q.GetSize() - 1;
          topnav_q.Swap(i, last);
          topnav_q.Pop();
          org->GetHardware().SetTopNavTrace(false);
        }
        break;
      }
    }
  }
}

bool cPopulation::TestForParasiteInteraction(cOrganism* infected_host, cOrganism* target_host)
{
  //default to failing the interaction
  bool interaction_fails = true;
  int infection_mechanism = m_world->GetConfig().INFECTION_MECHANISM.Get();
  
  cPhenotype& parent_phenotype = infected_host->GetPhenotype();
  
  Apto::Array<int> host_task_counts = target_host->GetPhenotype().GetLastHostTaskCount();
  Apto::Array<int> parasite_task_counts = parent_phenotype.GetLastParasiteTaskCount();
  
  
  if (infection_mechanism == 0) {
    interaction_fails = false;
  }
  
  // 1: Parasite must match at least 1 task the host does (Overlap)
  if (infection_mechanism == 1) {
    //handle skipping of first task
    int start = 0;
    if (m_world->GetConfig().INJECT_SKIP_FIRST_TASK.Get()) {
      start += 1;
    }
    
    //find if there is a matching task
    for (int i = start; i < host_task_counts.GetSize(); i++) {
      if (host_task_counts[i] > 0 && parasite_task_counts[i] > 0) {
        //inject should succeed if there is a matching task
        interaction_fails = false;
      }
    }
  }
  
  // 2: Parasite must perform at least one task the host does not (Inverse Overlap)
  if (infection_mechanism == 2) {
    //handle skipping of first task
    int start = 0;
    if (m_world->GetConfig().INJECT_SKIP_FIRST_TASK.Get()) {
      start += 1;
    }
    
    //find if there is a parasite task that the host isn't doing
    for (int i = start; i < host_task_counts.GetSize(); i++) {
      if (host_task_counts[i] == 0 && parasite_task_counts[i] > 0) {
        //inject should succeed if there is a matching task
        interaction_fails = false;
      }
    }
  }
  
  // 3: Parasite tasks must match host tasks exactly. (Matching Alleles) 
  if (infection_mechanism == 3) {
    //handle skipping of first task
    int start = 0;
    if (m_world->GetConfig().INJECT_SKIP_FIRST_TASK.Get()) {
      start += 1;
    }
    
    //This time if we trigger the if statments we DO fail. 
    interaction_fails = false;
    for (int i = start; i < host_task_counts.GetSize(); i++) {
      if ((host_task_counts[i] == 0 && parasite_task_counts[i] > 0) || 
          (host_task_counts[i] > 0 && parasite_task_counts[i] == 0)) {
        //inject should fail if either the host or parasite is doing a task the other isn't.
        interaction_fails = true;
      }
    }
  }
  
  // 4: Parasite tasks must overcome hosts. (GFG) 
  if (infection_mechanism == 4) {
    //handle skipping of first task
    int start = 0;
    if (m_world->GetConfig().INJECT_SKIP_FIRST_TASK.Get()) {
      start += 1;
    }
    
    //This time if we trigger the if statments we DO fail. 
    interaction_fails = false;
    bool parasite_overcomes = false;
    for (int i = start; i < host_task_counts.GetSize(); i++) {
      if (host_task_counts[i] > 0 && parasite_task_counts[i] == 0 ) {
        //inject should fail if the host overcomes the parasite.
        interaction_fails = true;
      }
      
      //check if parasite overcomes at least one task
      if (parasite_task_counts[i] > 0 && host_task_counts[i] == 0) {
        parasite_overcomes = true;
      }
    }
    
    //if host doesn't overcome, infection may still fail if the parasite doesn't overcome the host
    if (interaction_fails == false && parasite_overcomes == false) {
      interaction_fails = true;
    }
  }
  
  // 5: Quantitative Matching Allele -- probability of infection based on phenotype overlap
  if (infection_mechanism == 5) {
    //handle skipping of first task
    int start = 0;
    if(m_world->GetConfig().INJECT_SKIP_FIRST_TASK.Get()) {
      start += 1;
    }
    
    //calculate how many tasks have the same binary phenotype (i.e. how much overlap)
    int num_overlap = 0;
    for (int i = start; i < host_task_counts.GetSize(); i++) {
      if ((host_task_counts[i] > 0 && parasite_task_counts[i] > 0) ||
          (host_task_counts[i] == 0 && parasite_task_counts[i] == 0)) {
        num_overlap += 1;
      }
    }
    
    //turn number into proportion of available tasks that match
    double prop_overlap = double(num_overlap) / (host_task_counts.GetSize() - start);
    
    //use config exponent and calculate probability of infection
    double infection_exponent = m_world->GetConfig().INJECT_QMA_EXPONENT.Get();
    double prob_success = pow(prop_overlap, infection_exponent);
    
    //check if infection should fail based on probability
    double rand = m_world->GetRandom().GetDouble();
    interaction_fails = rand > prob_success;
  }
  
  // TODO: Add other infection mechanisms -LZ
  // : Probabilistic infection based on overlap. (GFG)
  // : Multiplicative GFG (special case of above?)
  // : Randomization of tasks that match between hosts and parasites?
  // : ??
  if (interaction_fails) {
    double prob_success = m_world->GetConfig().INJECT_DEFAULT_SUCCESS.Get();
    double rand = m_world->GetRandom().GetDouble();
    
    if (rand > prob_success) {
      return false;
    }
  }

  return true;
}

bool cPopulation::ActivateParasite(cOrganism* host, Systematics::UnitPtr parent, const cString& label, const InstructionSequence& injected_code)
{
  assert(parent != NULL);
  
  // Quick check for empty parasites
  if (injected_code.GetSize() == 0) return false;
  
  
  // Pull the host cell
  const int host_id = host->GetOrgInterface().GetCellID();
  assert(host_id >= 0 && host_id < cell_array.GetSize());
  cPopulationCell& host_cell = cell_array[host_id];
  
  
  // Select a target organism
  // @TODO - activate parasite target selection should account for hardware type
  cOrganism* target_organism = NULL;
  // If there's any migration turned on ... try this first
  if(m_world->GetConfig().NUM_DEMES.Get() > 1 && m_world->GetConfig().DEMES_PARASITE_MIGRATION_RATE.Get() > 0.0 && m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 4 && m_world->GetRandom().P(m_world->GetConfig().DEMES_PARASITE_MIGRATION_RATE.Get())){
    cDeme& deme = GetDeme(m_world->GetMigrationMatrix().GetProbabilisticDemeID(host_cell.GetDemeID(), m_world->GetRandom(),true));
    
    // Implementation #1 - Picks randomly of ALL cells in to-deme and then finds if the one it chose was occupied
    // -- Not ensured to infect an individual
    cPopulationCell& rand_cell = deme.GetCell(m_world->GetRandom().GetInt(deme.GetSize()));
    if(rand_cell.IsOccupied()){
      target_organism = rand_cell.GetOrganism();
    }    
  }
  else{
    // Else there was no migration ... Resort to the default BIRTH_METHOD
    if (m_world->GetConfig().BIRTH_METHOD.Get() ==  POSITION_OFFSPRING_FULL_SOUP_RANDOM) {
      target_organism = GetCell(m_world->GetRandom().GetUInt(cell_array.GetSize())).GetOrganism();
    } 
    else if(m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_DEME_RANDOM){
      cDeme& deme = GetDeme(host_cell.GetDemeID());
      cPopulationCell& rand_cell = deme.GetCell(m_world->GetRandom().GetInt(deme.GetSize()));
      if(rand_cell.IsOccupied()){
        target_organism = rand_cell.GetOrganism();
      }    
    }
    else {
      target_organism =
      host_cell.ConnectionList().GetPos(m_world->GetRandom().GetUInt(host->GetNeighborhoodSize()))->GetOrganism();
    }     
  }
  
  if (target_organism == NULL) return false;
  
  
  // Pre-check target hardware
  const cHardwareBase& hw = target_organism->GetHardware();
  if (hw.GetType() != parent->UnitGenome().HardwareType() ||
      hw.GetInstSet().GetInstSetName() != (const char*)parent->UnitGenome().Properties().Get(s_prop_id_instset).StringValue() ||
      hw.GetNumThreads() == m_world->GetConfig().MAX_CPU_THREADS.Get()) return false;
  
  //Handle host specific injection
  if(TestForParasiteInteraction(host, target_organism) == false)
    return false;
  
  
  // Attempt actual parasite injection
  // LZ - use parasige_genotype_list for the GenRepPtr instead IF Config says to
  // e.g., use predefined genotypes to hold the frequency constant, or "replay" parasite
  // from one run into another.
  GeneticRepresentationPtr tmpParasiteGenome;
  
  if (m_world->GetConfig().PARASITE_USE_GENOTYPE_FILE.Get())
  {
    tmpParasiteGenome = parasite_genotype_list[m_world->GetRandom().GetInt(parasite_genotype_list.GetSize())];
  }
  else
  {
    tmpParasiteGenome = GeneticRepresentationPtr(new InstructionSequence(injected_code));
  }
  
  Genome mg(parent->UnitGenome().HardwareType(), parent->UnitGenome().Properties(), tmpParasiteGenome);

  Apto::SmartPtr<cParasite, Apto::InternalRCObject> parasite(new cParasite(m_world, mg, Apto::StrAs(parent->Properties().Get("generation")), Systematics::Source(Systematics::HORIZONTAL, (const char*)label)));
  //Handle potential virulence evolution if this parasite is comming from a parasite 
  //and virulence is inhereted from the parent (source == 1)
  if (parent->UnitSource().transmission_type == Systematics::HORIZONTAL && m_world->GetConfig().VIRULENCE_SOURCE.Get() == 1)
  {
    //mutate virulence
    // m_world->GetConfig().PARASITE_VIRULENCE.Get()
    Apto::SmartPtr<cParasite, Apto::InternalRCObject> parent_parasite;
    parent_parasite.DynamicCastFrom(parent);
    double oldVir = parent_parasite->GetVirulence();
    
    //default to not mutating
    double newVir = oldVir;
    
    //but if we do mutate...
    if (m_world->GetRandom().GetDouble() < m_world->GetConfig().VIRULENCE_MUT_RATE.Get())
    {
      //get this in a temp variable so we don't have to make the next line huge
      double vir_sd = m_world->GetConfig().VIRULENCE_SD.Get();
      
      //sd^2 = varience
      newVir = m_world->GetRandom().GetRandNormal(oldVir, vir_sd * vir_sd);
      
    }
    parasite->SetVirulence(Apto::Max(Apto::Min(newVir, 1.0), 0.0));
  }
  else
  {
    //get default virulence
    parasite->SetVirulence(m_world->GetConfig().PARASITE_VIRULENCE.Get());
  }
  if (!target_organism->ParasiteInfectHost(parasite)) {
    return false;
  }
  
  //If parasite was successfully injected, update the phenotype for the parasite in new organism
  target_organism->GetPhenotype().SetLastParasiteTaskCount(host->GetPhenotype().GetLastParasiteTaskCount());
  
  // Classify the parasite
  Systematics::ConstParentGroupsPtr pgrps(new Systematics::ConstParentGroups(1));
  (*pgrps)[0] = parent->SystematicsGroupMembership();
  parasite->SelfClassify(pgrps);
  
  // Handle post injection actions
  if (m_world->GetConfig().INJECT_STERILIZES_HOST.Get()) target_organism->GetPhenotype().Sterilize();
  
  return true;
}

bool cPopulation::ActivateOrganism(cAvidaContext& ctx, cOrganism* in_organism, cPopulationCell& target_cell, bool assign_group, bool is_inject)
{
  assert(in_organism != NULL);
  
  in_organism->SetOrgInterface(ctx, new cPopulationInterface(m_world));
  
  // Update the contents of the target cell.
  KillOrganism(target_cell, ctx); 
  target_cell.InsertOrganism(in_organism, ctx); 
  AddLiveOrg(in_organism); 
  
  // Setup the inputs in the target cell.
  environment.SetupInputs(ctx, target_cell.m_inputs);
  
  // Precalculate the phenotype if requested
  int pc_phenotype = m_world->GetConfig().PRECALC_PHENOTYPE.Get();
  if (pc_phenotype){
    cCPUTestInfo test_info;
    cTestCPU* test_cpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
    test_info.UseManualInputs(target_cell.GetInputs()); // Test using what the environment will be
    Genome mg(in_organism->GetGenome().HardwareType(),
              in_organism->GetGenome().Properties(),
              GeneticRepresentationPtr(new InstructionSequence(in_organism->GetHardware().GetMemory())));
    test_cpu->TestGenome(ctx, test_info, mg);  // Use the true genome
    
    if (pc_phenotype & 1)
      in_organism->GetPhenotype().SetMerit(test_info.GetTestPhenotype().GetMerit());
    if (pc_phenotype & 2)
      in_organism->GetPhenotype().SetGestationTime(test_info.GetTestPhenotype().GetGestationTime());
    in_organism->GetPhenotype().SetFitness(in_organism->GetPhenotype().GetMerit().CalcFitness(in_organism->GetPhenotype().GetGestationTime()));
    delete test_cpu;
  }
  // Update the archive...
  
  
  // Initialize the time-slice for this new organism.
  AdjustSchedule(target_cell, in_organism->GetPhenotype().GetMerit());
  
  // Special handling for certain birth methods.
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST) {
    reaper_queue.Push(&target_cell);
  }
  
  // If neural networking, add input and output avatars.. @JJB**
  if (m_world->GetConfig().USE_AVATARS.Get() && m_world->GetConfig().NEURAL_NETWORKING.Get()) {
    // Add input avatar
    in_organism->GetOrgInterface().AddIOAV(ctx, target_cell.GetID(), 2, true, false);
    // Add input avatar
    in_organism->GetOrgInterface().AddIOAV(ctx, target_cell.GetID(), 2, false, true);
  }
  
  // Keep track of statistics for organism counts...
  num_organisms++;
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
    // ft should be nearly always -1 so long as it is not being inherited
    if (in_organism->IsPreyFT()) num_prey_organisms++;
    else if (in_organism->IsTopPredFT()) num_top_pred_organisms++;
    else num_pred_organisms++;
  }
  if (deme_array.GetSize() > 0) {
    deme_array[target_cell.GetDemeID()].IncOrgCount();
  }
  
  // Statistics...
  m_world->GetStats().RecordBirth(in_organism->GetPhenotype().ParentTrue());
  
  // @MRR Do coalescence clade setup for new organisms.
  CCladeSetupOrganism(in_organism );
  
  //count how many times MERIT_BONUS_INST (rewarded instruction) is in the genome
  //only relevant if merit is proportional to # times MERIT_BONUS_INST is in the genome
  int rewarded_instruction = m_world->GetConfig().MERIT_BONUS_INST.Get();
  int num_rewarded_instructions = 0;
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(in_organism->GetGenome().Representation());
  int genome_length = seq->GetSize();
  
  if (rewarded_instruction == -1){
    //no key instruction, so no bonus
    in_organism->GetPhenotype().SetCurBonusInstCount(0);
  }
  else{
    for(int i = 1; i <= genome_length; i++){
      if ((*seq)[i-1].GetOp() == rewarded_instruction){
        num_rewarded_instructions++;
      }
    }
    in_organism->GetPhenotype().SetCurBonusInstCount(num_rewarded_instructions);
  }
  // ok, after we've gone through all that, there's a catch.  It is possible that the
  // cell into which this organism has been injected is in fact a "gateway" to another
  // world.  if so, we then migrate this organism out of this world and empty the cell.
  if(m_world->IsWorldBoundary(target_cell)) {
    m_world->MigrateOrganism(in_organism, target_cell, in_organism->GetPhenotype().GetMerit(), in_organism->GetLineageLabel());
    KillOrganism(target_cell, ctx);
  }
  
  if (assign_group) {
    int op = m_world->GetConfig().DEFAULT_GROUP.Get();
    if (m_world->GetConfig().USE_FORM_GROUPS.Get() != 0) {
      if (!in_organism->HasOpinion()) {
        if (m_world->GetConfig().DEFAULT_GROUP.Get() != -1) {
          in_organism->SetOpinion(m_world->GetConfig().DEFAULT_GROUP.Get());
          JoinGroup(in_organism, m_world->GetConfig().DEFAULT_GROUP.Get());
        }
        else {
          if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 1) {
            op = (int) abs(ctx.GetRandom().GetDouble());
            in_organism->SetOpinion(op);
            JoinGroup(in_organism, op);
          }
          else if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 2) {
            op = ctx.GetRandom().GetInt(0, m_world->GetEnvironment().GetResourceLib().GetSize() + 1);
            in_organism->SetOpinion(op);
            JoinGroup(in_organism, op);
          }
        }
      }
      else op = in_organism->GetOpinion().first;
    }
    
    in_organism->GetPhenotype().SetBirthCellID(target_cell.GetID());
//    in_organism->GetOrgInterface().TryWriteBirthLocData(in_organism->GetOrgIndex());
    in_organism->GetPhenotype().SetBirthGroupID(op);
    in_organism->GetPhenotype().SetBirthForagerType(in_organism->GetForageTarget());
    Systematics::GenotypePtr genotype;
    genotype.DynamicCastFrom(in_organism->SystematicsGroup("genotype"));
    assert(genotype);    
    
    genotype->SetLastBirthCell(target_cell.GetID());
    if (m_world->GetConfig().INHERIT_OPINION.Get()) genotype->SetLastGroupID(op);
    else genotype->SetLastGroupID(in_organism->GetParentGroup());
    genotype->SetLastForagerType(in_organism->GetParentFT());      
  }

  bool org_survived = true;
  // For tolerance_window, we cheated by dumping doomed offspring into cell (X * Y) - 1 ...now that we updated the stats, we need to 
  // kill that org. @JJB
  int doomed_cell = (world_x * world_y) - 1;
  if ((m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) && (target_cell.GetID() == doomed_cell) && (m_world->GetStats().GetUpdate() > 0)) {
    KillOrganism(target_cell, ctx);
    org_survived = false;
  }
  // Kill org born on deadly world boundaries
  if (m_world->GetConfig().DEADLY_BOUNDARIES.Get() == 1 && m_world->GetConfig().WORLD_GEOMETRY.Get() == 1 && target_cell.GetID() >= 0) {
    int dest_x = target_cell.GetID() % m_world->GetConfig().WORLD_X.Get();  
    int dest_y = target_cell.GetID() / m_world->GetConfig().WORLD_X.Get();
    if (dest_x == 0 || dest_y == 0 || dest_x == m_world->GetConfig().WORLD_X.Get() - 1 || dest_y == m_world->GetConfig().WORLD_Y.Get() - 1) {
      KillOrganism(target_cell, ctx);
      org_survived = false;
    }
  } 
  // don't kill our test org, just it's offspring
  if ((m_world->GetConfig().BIRTH_METHOD.Get() == 12 || m_world->GetConfig().BIRTH_METHOD.Get() == 13) && !is_inject) {
      KillOrganism(target_cell, ctx); 
      org_survived = false; 
  }
  // are there traces we need to test for?
  if (org_survived) {
    if (m_next_prey_q > 0 && in_organism->GetParentFT() > -2) { 
      SetupMiniTrace(in_organism); 
      m_next_prey_q--; 
    }
    else if (m_next_pred_q > 0 && in_organism->GetParentFT() <= -2) { 
      SetupMiniTrace(in_organism); 
      m_next_pred_q--; 
    }
    else if (minitrace_queue.GetSize() > 0) TestForMiniTrace(in_organism);  
  }
  return org_survived;
}

void cPopulation::TestForMiniTrace(cOrganism* in_organism) 
{
  // if the org's genotype is on our to do list, setup the trace and remove the instance of the genotype from the list
  int org_bg_id = in_organism->SystematicsGroup("genotype")->ID();
  for (int i = 0; i < minitrace_queue.GetSize(); i++) {
    if (org_bg_id == minitrace_queue[i]) {
      unsigned int last = minitrace_queue.GetSize() - 1;
      minitrace_queue.Swap(i, last);
      minitrace_queue.Pop();
      SetupMiniTrace(in_organism);
      break;
    }
  }
}

void cPopulation::SetupMiniTrace(cOrganism* in_organism)
{
  const int target = in_organism->GetParentFT();
  const int id = in_organism->GetID();
  int group_id = m_world->GetConfig().DEFAULT_GROUP.Get();
  
  if (in_organism->HasOpinion()) group_id = in_organism->GetOpinion().first;
  else group_id = in_organism->GetParentGroup();
  
  cString filename = cStringUtil::Stringf("minitraces/org%d-ud%d-grp%d_ft%d-gt%d.trc", id, m_world->GetStats().GetUpdate(), group_id, target, in_organism->SystematicsGroup("genotype")->ID());
  
  if (!use_micro_traces) in_organism->GetHardware().SetMiniTrace(filename);
  else in_organism->GetHardware().SetMicroTrace();
  
  if (print_mini_trace_genomes) {
    cString gen_file =  cStringUtil::Stringf("minitraces/trace_genomes/org%d-ud%d-grp%d_ft%d-gt%d.trcgeno", id, m_world->GetStats().GetUpdate(), group_id, target, in_organism->SystematicsGroup("genotype")->ID());
    PrintMiniTraceGenome(in_organism, gen_file);
  }
}

void cPopulation::PrintMiniTraceGenome(cOrganism* in_organism, cString& filename)
{
  // need a random number generator to pass to testcpu that does not affect any other random number pulls (since this is just for printing the genome)
  Apto::RNG::AvidaRNG rng(0);
  cAvidaContext ctx2(&m_world->GetDriver(), rng);
  
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx2);
  testcpu->PrintGenome(ctx2, Genome(in_organism->SystematicsGroup("genotype")->Properties().Get("genome")), filename, m_world->GetStats().GetUpdate());
  delete testcpu;
}

void cPopulation::SetMiniTraceQueue(Apto::Array<int, Apto::Smart> new_queue, const bool print_genomes, const bool print_reacs, const bool use_micro)
{
  minitrace_queue.Resize(0);
  for (int i = 0; i < new_queue.GetSize(); i++) minitrace_queue.Push(new_queue[i]);
  print_mini_trace_genomes = print_genomes;
  print_mini_trace_reacs = print_reacs;
  use_micro_traces = use_micro;
}

void cPopulation::AppendMiniTraces(Apto::Array<int, Apto::Smart> new_queue, const bool print_genomes, const bool print_reacs, const bool use_micro)
{
  for (int i = 0; i < new_queue.GetSize(); i++) minitrace_queue.Push(new_queue[i]); 
  print_mini_trace_genomes = print_genomes;
  print_mini_trace_reacs = print_reacs;
  use_micro_traces = use_micro;
}

void cPopulation::LoadMiniTraceQ(const cString& filename, int orgs_per, bool print_genomes, bool print_reacs)
{
  cInitFile input_file(filename, m_world->GetWorkingDir());
  if (!input_file.WasOpened()) {
    const cUserFeedback& feedback = input_file.GetFeedback();
    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:    m_world->GetDriver().Feedback().Error(feedback.GetMessage(i)); break;
        case cUserFeedback::UF_WARNING:  m_world->GetDriver().Feedback().Warning(feedback.GetMessage(i)); break;
        default:                      m_world->GetDriver().Feedback().Notify(feedback.GetMessage(i)); break;
      };
    }
  }
  
  Apto::Array<int, Apto::Smart> bg_id_list;
  Apto::Array<int, Apto::Smart> queue = m_world->GetPopulation().GetMiniTraceQueue();
  for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
    cString cur_line = input_file.GetLine(line_id);
    
    Apto::SmartPtr<Apto::Map<Apto::String, Apto::String> > line = input_file.GetLineAsDict(line_id);
    int gen_id_num = Apto::StrAs(line->Get("id"));
    
    // setup the genotype 'list' which will be checked in activateorg
    // skip if enough already in the existing trace queue (e.g if loading multiple genotype id files that overlap)
    int add_num = orgs_per;
    for (int i = 0; i < queue.GetSize(); i++) {
      if (gen_id_num == queue[i]) {
        add_num--;
        if (add_num <= 0) break;
      }
    }
    for (int j = 0; j < add_num; j++) {
      bg_id_list.Push(gen_id_num);
    }
  }
  
  if (queue.GetSize() > 0) {
    AppendMiniTraces(bg_id_list, print_genomes, print_reacs);
  }
  else {
    SetMiniTraceQueue(bg_id_list, print_genomes, print_reacs);
  }
}

Apto::Array<int, Apto::Smart> cPopulation::SetRandomTraceQ(int max_samples)
{
  // randomly sample (w/ replacement) bgs in pop
  Apto::Array<int, Apto::Smart> bg_id_list;
  Apto::Array<cOrganism*, Apto::Smart> live_orgs = GetLiveOrgList();

  int max_bgs = 1;
  if (max_samples) max_bgs = max_samples;
  if (max_samples > live_orgs.GetSize()) max_bgs = live_orgs.GetSize();
  
  Apto::Array<bool> used_orgs;
  used_orgs.Resize(live_orgs.GetSize());
  used_orgs.SetAll(false);
  
  while (bg_id_list.GetSize() < max_bgs) {
    Apto::RNG::AvidaRNG rng(0);
    cAvidaContext ctx2(&m_world->GetDriver(), rng);
    int this_rand_sample = ctx2.GetRandom().GetInt(0, live_orgs.GetSize());

    if (!used_orgs[this_rand_sample]) {
      cOrganism* rand_org = live_orgs[this_rand_sample];
      bg_id_list.Push(rand_org->SystematicsGroup("genotype")->ID());
      used_orgs[this_rand_sample] = true;
    }
  } 
  return bg_id_list;
}

Apto::Array<int, Apto::Smart> cPopulation::SetRandomPreyTraceQ(int max_samples)
{
  // randomly sample (w/ replacement) bgs in pop
  Apto::Array<int, Apto::Smart> bg_id_list;
  const Apto::Array<cOrganism*, Apto::Smart> live_orgs = GetLiveOrgList();

  int max_bgs = 1;
  if (max_samples) max_bgs = max_samples;
  if (max_samples > num_prey_organisms) max_bgs = num_prey_organisms;
  
  Apto::Array<bool> used_orgs;
  used_orgs.Resize(live_orgs.GetSize());
  used_orgs.SetAll(false);
  
  while (bg_id_list.GetSize() < max_bgs) {
    Apto::RNG::AvidaRNG rng(0);
    cAvidaContext ctx2(&m_world->GetDriver(), rng);
    int this_rand_sample = ctx2.GetRandom().GetInt(0, live_orgs.GetSize());

    if (!used_orgs[this_rand_sample]) {
      cOrganism* rand_org = live_orgs[this_rand_sample];
      if (rand_org->IsPreyFT()) {
        bg_id_list.Push(rand_org->SystematicsGroup("genotype")->ID());
        used_orgs[this_rand_sample] = true;
      }
    }
  } 
  return bg_id_list;
}

Apto::Array<int, Apto::Smart> cPopulation::SetRandomPredTraceQ(int max_samples)
{
  // randomly sample (w/ replacement) bgs in pop
  Apto::Array<int, Apto::Smart> bg_id_list;
  const Apto::Array<cOrganism*, Apto::Smart> live_orgs = GetLiveOrgList();

  int max_bgs = 1;
  if (max_samples) max_bgs = max_samples;
  if (max_samples > num_pred_organisms) max_bgs = num_pred_organisms;
  
  Apto::Array<bool> used_orgs;
  used_orgs.Resize(live_orgs.GetSize());
  used_orgs.SetAll(false);
  
  while (bg_id_list.GetSize() < max_bgs) {
    Apto::RNG::AvidaRNG rng(0);
    cAvidaContext ctx2(&m_world->GetDriver(), rng);
    int this_rand_sample = ctx2.GetRandom().GetInt(0, live_orgs.GetSize());

    if (!used_orgs[this_rand_sample]) {
      cOrganism* rand_org = live_orgs[this_rand_sample];
      if (!rand_org->IsPreyFT()) {
        bg_id_list.Push(rand_org->SystematicsGroup("genotype")->ID());
        used_orgs[this_rand_sample] = true;
      }
    }
  } 
  return bg_id_list;
}

void cPopulation::SetNextPreyQ(int num_prey, bool print_genomes, bool print_reacs, bool use_micro)
{
  m_next_prey_q = num_prey;
  print_mini_trace_genomes = print_genomes;
  print_mini_trace_reacs = print_reacs;
  use_micro_traces = use_micro;
}

void cPopulation::SetNextPredQ(int num_pred, bool print_genomes, bool print_reacs, bool use_micro)
{
  m_next_pred_q = num_pred;
  print_mini_trace_genomes = print_genomes;
  print_mini_trace_reacs = print_reacs;
  use_micro_traces = use_micro;
}

Apto::Array<int, Apto::Smart> cPopulation::SetTraceQ(int save_dominants, int save_groups, int save_foragers, int orgs_per, int max_samples)
{
  // setup the genotype 'list' which will be checked in activateorg
  // this should setup a 'list' of genotypes at each event update which should be followed (e.g. if orgs_per = 10, save top 10 prey genotypes + top 10 predator genotypes at this update or one org from top 10 most common genotypes over all)
  // items should be removed from list once an org of that type is set to be traced
  // number of items in list should be capped by max_samples, filling the list with the more dominant genotypes first (this is necessary in the case of saving groups because we may not know how many groups there will be at any time during a run and so cannot set orgs_per to function as an absolute cap...should not be neccessary for saving by dominants or saving by foragers)
  // when we go to check if an org is to be traced, all we need to then do is remove the genotype from the list if the org's genotype is there
  // in activateorganism we can just check the size of this array, 
  // if it is 0, there is nothing to check, if it is > 0, there are genotypes waiting
  // this will allow genotypes to wait until the next event (which will overwrite the array contents)
  // only tracing for orgs within threshold (unless none are, then just use first bg)
  Apto::Array<int, Apto::Smart> bg_id_list;
  
  Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
  Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
  Systematics::GroupPtr bg = (it->Next());
  Apto::Array<int, Apto::Smart> fts_to_use;
  Apto::Array<int, Apto::Smart> groups_to_use;
  int num_doms = 0;
  int fts_left = 0;
  int groups_left = 0;
  
  if (save_dominants) num_doms = orgs_per;
  
  // get forager types in pop
  Apto::Array<int, Apto::Smart> ft_check_counts;
  ft_check_counts.Resize(0);
  if (save_foragers) {
    if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
      fts_to_use.Push(-3);
      fts_to_use.Push(-2);
    }
    fts_to_use.Push(-1);  // account for -1 default's
    std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
    set <int>::iterator itr;    
    for(itr = fts_avail.begin();itr!=fts_avail.end();itr++) if (*itr != -1 && *itr != -2 && *itr != -3) fts_to_use.Push(*itr);
    ft_check_counts.Resize(fts_to_use.GetSize());
    ft_check_counts.SetAll(orgs_per);
    fts_left = orgs_per * fts_to_use.GetSize();
  }
  
  // get groups in pop
  Apto::Array<int, Apto::Smart> group_check_counts;
  group_check_counts.Resize(0);
  if (save_groups) {
    map<int,int> groups_formed = m_world->GetPopulation().GetFormedGroups();
    map <int,int>::iterator itr;    
    for(itr = groups_formed.begin();itr!=groups_formed.end();itr++) {
      double cur_size = itr->second;
      if (cur_size > 0) groups_to_use.Push(itr->first);  
    }
    group_check_counts.Resize(groups_to_use.GetSize());
    group_check_counts.SetAll(orgs_per);
    groups_left = orgs_per * groups_to_use.GetSize();
  }
  
  // this will add biogroup genotypes up to max_bgs with priority on dominants, then forager types, then group ids, without repeats
  // priority is non-issue if you don't double up on the settings in one go
  int max_bgs = 1;
  if (max_samples) max_bgs = max_samples;
  else max_bgs = num_doms + (orgs_per * fts_to_use.GetSize()) + (orgs_per * groups_to_use.GetSize());
  int num_types = 3;
  bool doms_done = false;
  bool fts_done = false;
  bool grps_done = false;
  if (!save_dominants) doms_done = true;
  if (!save_foragers) fts_done = true;
  if (!save_groups) grps_done = true;
  for (int i = 0; i < num_types; i++) {
    if (bg_id_list.GetSize() < max_bgs && (!doms_done || !fts_done || !grps_done)) {
      if (i == 0 && save_dominants && num_doms > 0) {
        for (int j = 0; j < num_doms; j++) {
          if (bg && ((bool)Apto::StrAs(bg->Properties().Get("threshold")) || bg_id_list.GetSize() == 0)) {
            bg_id_list.Push(bg->ID());
            if (save_foragers) {
              int ft = Apto::StrAs(bg->Properties().Get("last_forager_type")); 
              if (fts_left > 0) {
                for (int k = 0; k < fts_to_use.GetSize(); k++) {
                  if (ft == fts_to_use[k]) {
                    ft_check_counts[k]--;
                    if (ft_check_counts[k] == 0) {
                      unsigned int last = fts_to_use.GetSize() - 1;
                      fts_to_use.Swap(k, last);
                      fts_to_use.Pop();
                      ft_check_counts.Swap(k, last);
                      ft_check_counts.Pop();
                    }
                    fts_left--;
                    break;
                  }
                }
              }
            }
            if (save_groups) {
              int grp = bg->Properties().Get("last_group_id"); 
              if (groups_left > 0) {
                for (int k = 0; k < groups_to_use.GetSize(); k++) {
                  if (grp == groups_to_use[k]) {
                    group_check_counts[k]--;
                    if (group_check_counts[k] == 0) {
                      unsigned int last = groups_to_use.GetSize() - 1;
                      groups_to_use.Swap(k, last);
                      groups_to_use.Pop();
                      group_check_counts.Swap(k, last);
                      group_check_counts.Pop();
                    }
                    groups_left--;
                    break;
                  }
                }
              }
            }                 
            if (bg == it->Next()) { // no more to check
              doms_done = true; 
              break; 
            }
            else bg = it->Next();
          }
          else if (bg && !((bool)Apto::StrAs(bg->Properties().Get("threshold")))) {      // no more above threshold
            doms_done = true; 
            break; 
          }
        }
        if (doms_done) continue;
      } // end of dominants
      
      else if (i == 1 && save_foragers && fts_left > 0) {
        for (int j = 0; j < fts_left; j++) {
          if (bg && ((bool)Apto::StrAs(bg->Properties().Get("threshold")) || bg_id_list.GetSize() == 0)) {
            int ft = bg->Properties().Get("last_forager_type"); 
            bool found_one = false;
            for (int k = 0; k < fts_to_use.GetSize(); k++) {
              if (ft == fts_to_use[k]) {
                bg_id_list.Push(bg->ID());
                ft_check_counts[k]--;
                if (ft_check_counts[k] == 0) {
                  unsigned int last = fts_to_use.GetSize() - 1;
                  fts_to_use.Swap(k, last);
                  fts_to_use.Pop();
                  ft_check_counts.Swap(k, last);
                  ft_check_counts.Pop();
                }
                found_one = true;
                break;
              }
            }
            if (save_groups) {
              int grp = bg->Properties().Get("last_group_id"); 
              if (groups_left > 0) {
                for (int k = 0; k < groups_to_use.GetSize(); k++) {
                  if (grp == groups_to_use[k]) {
                    group_check_counts[k]--;
                    if (group_check_counts[k] == 0) {
                      unsigned int last = groups_to_use.GetSize() - 1;
                      groups_to_use.Swap(k, last);
                      groups_to_use.Pop();
                      group_check_counts.Swap(k, last);
                      group_check_counts.Pop();
                    }
                    groups_left--;
                    break;
                  }
                }
              }
            }                 
            if (bg == it->Next()) { // no more to check
              fts_done = true; 
              break; 
            }
            else bg = it->Next();
            if (!found_one) j--;
          }
          else if (bg && !((bool)Apto::StrAs(bg->Properties().Get("threshold")))) {  // no more above threshold
            fts_done = true; 
            break; 
          }
        }
        if (fts_done) continue;
      } // end of forage types
      
      else if (i == 2 && save_groups && groups_left > 0) {
        for (int j = 0; j < groups_left; j++) {
          if (bg && ((bool)Apto::StrAs(bg->Properties().Get("threshold")) || bg_id_list.GetSize() == 0)) {
            int grp = bg->Properties().Get("last_group_id"); 
            bool found_one = false;
            for (int k = 0; k < groups_to_use.GetSize(); k++) {
              if (grp == groups_to_use[k]) {
                bg_id_list.Push(bg->ID());
                group_check_counts[k]--;
                if (group_check_counts[k] == 0) {
                  unsigned int last = groups_to_use.GetSize() - 1;
                  groups_to_use.Swap(k, last);
                  groups_to_use.Pop();
                  group_check_counts.Swap(k, last);
                  group_check_counts.Pop();
                }
                found_one = true;
                break;
              }
            }
            if (bg == it->Next()) { // no more to check
              grps_done = true; 
              break; 
            }
            else bg = it->Next();
            if (!found_one) j--;
          }
          else if (bg && !((bool)Apto::StrAs(bg->Properties().Get("threshold")))) {  // no more above threshold
            grps_done = true; 
            break; 
          }
        }           
        if (grps_done) break;     // no more of last type we have
      } // end of group id types
    } // end of while < max_bgs  
  }
  return bg_id_list;
}

Apto::Array<int> cPopulation::GetFormedGroupArray()
{
  Apto::Array<int> group_ids;
  group_ids.Resize(0);
  map<int,int> groups_formed = m_world->GetPopulation().GetFormedGroups();
  map <int,int>::iterator itr;    
  for(itr = groups_formed.begin();itr!=groups_formed.end();itr++) {
    double cur_size = itr->second;
    if (cur_size > 0) group_ids.Push(itr->first);
  }
  return group_ids;
}

void cPopulation::SetTopNavQ()
{
  topnav_q.Resize(live_org_list.GetSize());
  for (int i = 0; i < live_org_list.GetSize(); i++) {
    live_org_list[i]->GetHardware().SetTopNavTrace(true);  
    topnav_q[i] = live_org_list[i];
  }
}

void cPopulation::AppendRecordReproQ(cOrganism* new_org) 
{ 
  repro_q.Push(new_org); 
  new_org->GetHardware().SetReproTrace(true); 
}

// @WRE 2007/07/05 Helper function to take care of side effects of Avidian
// movement that cannot be directly handled in cHardwareCPU.cc
bool cPopulation::MoveOrganisms(cAvidaContext& ctx, int src_cell_id, int dest_cell_id, int true_cell)
{
  cPopulationCell& src_cell = GetCell(src_cell_id);
  cPopulationCell& dest_cell = GetCell(dest_cell_id);
  
  const int dest_x = dest_cell_id % m_world->GetConfig().WORLD_X.Get();  
  const int dest_y = dest_cell_id / m_world->GetConfig().WORLD_X.Get();
  
  // check for boundary effects on movement
  if (m_world->GetConfig().DEADLY_BOUNDARIES.Get() == 1 && m_world->GetConfig().WORLD_GEOMETRY.Get() == 1) {
    // Fail if we're running in the test CPU.
    if (src_cell_id < 0) return false;
    bool faced_is_boundary = false;
    if (dest_x == 0 || dest_y == 0 || 
        dest_x == m_world->GetConfig().WORLD_X.Get() - 1 || 
        dest_y == m_world->GetConfig().WORLD_Y.Get() - 1) faced_is_boundary = true;
    if (faced_is_boundary) {
      if (true_cell != -1) KillOrganism(GetCell(true_cell), ctx);
      else if (true_cell == -1) KillOrganism(src_cell, ctx);
      return false;
    }
  }    
  
  // get the resource library
  const cResourceLib& resource_lib = environment.GetResourceLib();
  
  bool has_path = false;
  bool has_hammer = false;
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (environment.HasPath() && resource_lib.GetResource(i)->IsPath() && GetCellResVal(ctx, dest_cell_id, i) > 0) has_path = true;
    if (environment.HasHammer() && resource_lib.GetResource(i)->IsHammer() && GetCellResVal(ctx, dest_cell_id, i) > 0) has_hammer = true;
  }
  
  if (!has_path || has_hammer) {
    // test for death by predatory resource or injury ... not mutually exclusive
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->IsPredatory() || resource_lib.GetResource(i)->IsDeadly()) {
        // get the destination cell resource levels
        double dest_cell_resources = GetCellResVal(ctx, dest_cell_id, i);
        if (dest_cell_resources > 0) {
          // if you step on a predatory resource, we're going to try to kill you
          if ((resource_lib.GetResource(i)->IsPredatory() && ctx.GetRandom().P(resource_lib.GetResource(i)->GetPredatorResOdds()))
              || (resource_lib.GetResource(i)->IsDeadly() && ctx.GetRandom().P(resource_lib.GetResource(i)->GetDeathOdds()))) {
            if (true_cell != -1) KillOrganism(GetCell(true_cell), ctx);
            else if (true_cell == -1) KillOrganism(src_cell, ctx);
            return false;
          }
        }
      }
      if (resource_lib.GetResource(i)->GetDamage()) {
        double dest_cell_resources = GetCellResVal(ctx, dest_cell_id, i);
        if (dest_cell_resources > resource_lib.GetResource(i)->GetThreshold()) {
          InjureOrg(ctx, GetCell(true_cell), resource_lib.GetResource(i)->GetDamage(), false);
        }
      }
    }
    // if any of the resources have resistance, find the id of the most resistant resource
    int steepest_hill = 0;
    double curr_resistance = 1.0;
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (!resource_lib.GetResource(i)->IsPath() && resource_lib.GetResource(i)->GetResistance() > curr_resistance) {
        if (GetCellResVal(ctx, src_cell_id, i) != 0) {
          curr_resistance = resource_lib.GetResource(i)->GetResistance();
          steepest_hill = i;
        }
      }
    }
    // apply the chance of move failing for the most resistant resource in this cell, if there is one
    if (resource_lib.GetSize() && curr_resistance != 1) {
      if (GetCellResVal(ctx, src_cell_id, steepest_hill) > 0) {
        // we use resistance to determine chance of movement succeeding: 'resistance == # move instructions executed, on average, to move one step/cell'
        double chance_move_success = 1.0/curr_resistance;
        if (!ctx.GetRandom().P(chance_move_success)) return false;
      }
    }

    // movement fails if there are any barrier resources in the faced cell (unless the org is already on a barrier,
    // which would happen if we built a new barrier under an org and we need to let it get off)
    bool curr_is_barrier = false;
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      // get the current cell resource levels
      if (resource_lib.GetResource(i)->GetHabitat() == 2 && !resource_lib.GetResource(i)->IsPath()) {
        if (GetCellResVal(ctx, src_cell_id, i) > 0) {
          curr_is_barrier = true;
          break;
        }
      }
    }
    if (!curr_is_barrier) {
      for (int i = 0; i < resource_lib.GetSize(); i++) {
        if (!resource_lib.GetResource(i)->IsPath() && resource_lib.GetResource(i)->GetHabitat() == 2 && resource_lib.GetResource(i)->GetResistance() != 0) {
          // fail if faced cell has this wall resource
          if (GetCellResVal(ctx, dest_cell_id, i) > 0) return false;
        }
      }
    }
  }

  // effects not applied to avatars:
  if (true_cell == -1) {
    if (m_world->GetConfig().MOVEMENT_COLLISIONS_LETHAL.Get() && dest_cell.IsOccupied()) {
      if (m_world->GetConfig().MOVEMENT_COLLISIONS_LETHAL.Get() == 2) return false;
      bool kill_source = true;
      switch (m_world->GetConfig().MOVEMENT_COLLISIONS_SELECTION_TYPE.Get()) {
        case 0: // 50% chance, no modifiers
        default:
          kill_source = ctx.GetRandom().P(0.5);
          break;
          
        case 1: // binned vitality based on age
          double src_vitality = src_cell.GetOrganism()->GetVitality();
          double dest_vitality = dest_cell.GetOrganism()->GetVitality();
          kill_source = (src_vitality < ctx.GetRandom().GetDouble(src_vitality + dest_vitality));
          break;
      }      
      if (kill_source) {
        KillOrganism(src_cell, ctx); 
        // Killing the moving organism means that we shouldn't actually do the swap, so return
        return false;
      }
      KillOrganism(dest_cell, ctx); 
    }
    SwapCells(src_cell_id, dest_cell_id, ctx); 
    
    // Declarations
    int actualNeighborhoodSize, fromFacing, destFacing, newFacing, success;
#ifdef DEBBUG
    int sID, dID, xx1, yy1, xx2, yy2;
#endif
    
    // Swap inputs between cells to fix bus error when Avidian moves into an unoccupied cell
    // LHZ: Moved to SwapCells function
    //environment.SwapInputs(ctx, src_cell.m_inputs, dest_cell.m_inputs);
    
    // Find neighborhood size for facing
    if (NULL != dest_cell.GetOrganism()) {
      actualNeighborhoodSize = dest_cell.ConnectionList().GetSize();
    } else {
      if (NULL != src_cell.GetOrganism()) {
        actualNeighborhoodSize = src_cell.ConnectionList().GetSize();
      } else {
        // Punt
        actualNeighborhoodSize = 8;
      }
    }
    
    // Swap cell facings between cells, so that if movement is directed, it continues to be associated with
    // the same organism
    // Determine absolute facing for each cell
    fromFacing = src_cell.GetFacing();
    destFacing = dest_cell.GetFacing();
    
    // Set facing in source cell
    success = 0;
    newFacing = destFacing;
    for(int i = 0; i < actualNeighborhoodSize; i++) {
      if (src_cell.GetFacing() != newFacing) {
        src_cell.ConnectionList().CircNext();
        //cout << "MO: src_cell facing not yet at " << newFacing << endl;
      } else {
        //cout << "MO: src_cell facing successfully set to " << newFacing << endl;
        success = 1;
        break;
      }
    }
    
    // Set facing in destinatiion cell
    success = 0;
    newFacing = fromFacing;
    for(int i = 0; i < actualNeighborhoodSize; i++) {
      if (dest_cell.GetFacing() != newFacing) {
        dest_cell.ConnectionList().CircNext();
        // cout << "MO: dest_cell facing not yet at " << newFacing << endl;
      } else {
        // cout << "MO: dest_cell facing successfully set to " << newFacing << endl;
        success = 1;
        break;
      }
    }
  }
  return true;
}

// Kill Random Organism in Group (But Not Self)!! 
void cPopulation::KillGroupMember(cAvidaContext& ctx, int group_id, cOrganism *org)
{
  //Check to make sure we are not killing self!
  if (m_group_list[group_id].GetSize() == 1 && m_group_list[group_id][0] == org) return;
  if (m_group_list[group_id].GetSize() == 0) return;
  int index;
  while(true) {
    index = ctx.GetRandom().GetUInt(0, m_group_list[group_id].GetSize());
    if (m_group_list[group_id][index] == org) continue;
    else break;
  }
  
  int cell_id = m_group_list[group_id][index]->GetCellID();
  KillOrganism(cell_array[cell_id], ctx); 
}

// Attack organism faced by this one, if there is an organism in front.
void cPopulation::AttackFacedOrg(cAvidaContext& ctx, int loser)
{
  cPopulationCell& loser_cell = GetCell(loser);
  KillOrganism(loser_cell, ctx); 
}

void cPopulation::KillRandPred(cAvidaContext& ctx, cOrganism* org)
{
  cOrganism* org_to_kill = org;
  const Apto::Array<cOrganism*, Apto::Smart>& live_org_list = GetLiveOrgList();
  Apto::Array<cOrganism*> TriedIdx(live_org_list.GetSize());
  int list_size = TriedIdx.GetSize();
  for (int i = 0; i < list_size; i ++) { TriedIdx[i] = live_org_list[i]; }
  
  int idx = ctx.GetRandom().GetUInt(list_size);
  while (org_to_kill == org) {
    cOrganism* org_at = TriedIdx[idx];
    // exclude prey
    if (org_at->GetParentFT() <= -2 || !org_at->IsPreyFT()) org_to_kill = org_at;
    else TriedIdx.Swap(idx, --list_size);
    if (list_size == 1) break;
    idx = ctx.GetRandom().GetUInt(list_size);
  }
  if (org_to_kill != org) m_world->GetPopulation().KillOrganism(m_world->GetPopulation().GetCell(org_to_kill->GetCellID()), ctx);
}

void cPopulation::KillRandPrey(cAvidaContext& ctx, cOrganism* org)
{
  cOrganism* org_to_kill = org;
  const Apto::Array<cOrganism*, Apto::Smart>& live_org_list = GetLiveOrgList();
  Apto::Array<cOrganism*> TriedIdx(live_org_list.GetSize());
  int list_size = TriedIdx.GetSize();
  for (int i = 0; i < list_size; i ++) { TriedIdx[i] = live_org_list[i]; }
  
  int idx = ctx.GetRandom().GetUInt(list_size);
  while (org_to_kill == org) {
    cOrganism* org_at = TriedIdx[idx];
    // exclude predators and juvenilles with predatory parents (include juvs with non-predatory parents)
    if (org_at->GetForageTarget() > -1 || (org_at->GetForageTarget() == -1 && org_at->GetParentFT() > -2)) org_to_kill = org_at;
    else TriedIdx.Swap(idx, --list_size);
    if (list_size == 1) break;
    idx = ctx.GetRandom().GetUInt(list_size);
  }
  if (org_to_kill != org) m_world->GetPopulation().KillOrganism(m_world->GetPopulation().GetCell(org_to_kill->GetCellID()), ctx);
}

cOrganism* cPopulation::GetRandPrey(cAvidaContext& ctx, cOrganism* org)
{
  cOrganism* target_org = org;
  const Apto::Array<cOrganism*, Apto::Smart>& live_org_list = GetLiveOrgList();
  Apto::Array<cOrganism*> TriedIdx(live_org_list.GetSize());
  int list_size = TriedIdx.GetSize();
  for (int i = 0; i < list_size; i ++) { TriedIdx[i] = live_org_list[i]; }
  
  int idx = ctx.GetRandom().GetUInt(list_size);
  while (target_org == org) {
    cOrganism* org_at = TriedIdx[idx];
    // exclude predators and juvenilles with predatory parents (include juvs with non-predatory parents)
    if (org_at->GetForageTarget() > -1 || (org_at->GetForageTarget() == -1 && org_at->GetParentFT() > -2)) target_org = org_at;
    else TriedIdx.Swap(idx, --list_size);
    if (list_size == 1) break;
    idx = ctx.GetRandom().GetUInt(list_size);
  }
  return target_org;
}

void cPopulation::KillOrganism(cPopulationCell& in_cell, cAvidaContext& ctx)
{
  // do we actually have something to kill?
  if (in_cell.IsOccupied() == false) return;
  
  // Statistics...
  cOrganism* organism = in_cell.GetOrganism();
  m_world->GetStats().RecordDeath();
  
  // orgs killed during birth wont have avatars
  if (m_world->GetConfig().USE_AVATARS.Get() && organism->GetOrgInterface().GetAVCellID() != -1) {
    organism->GetOrgInterface().RemoveAllAV();
  }
  
  // If neural networking remove all input/output avatars @JJB**
  if (m_world->GetConfig().USE_AVATARS.Get() && m_world->GetConfig().NEURAL_NETWORKING.Get()) {
    organism->GetOrgInterface().RemoveAllAV();
  }

  const int ft = organism->GetForageTarget();

  RemoveLiveOrg(organism);
  UpdateQs(organism, false);
  
  int cellID = in_cell.GetID();
  
  organism->NotifyDeath(ctx);
  
  // @TODO @DMB - this should really move to cOrganism::NotifyDeath
  if (m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1) {
    if (sleep_log[cellID].GetSize() > 0) {
      pair<int,int> p = sleep_log[cellID][sleep_log[cellID].GetSize() - 1];
      if (p.second == -1) {
        AddEndSleep(cellID,m_world->GetStats().GetUpdate());
      }
    }
  }
  
  
  // Update count statistics...
  num_organisms--;
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
    if (ft > -2) num_prey_organisms--;
    else if (ft == -2) num_pred_organisms--;
    else num_top_pred_organisms--;
  }
  
  // Handle deme updates.
  if (deme_array.GetSize() > 0) {
    deme_array[in_cell.GetDemeID()].DecOrgCount();
    deme_array[in_cell.GetDemeID()].OrganismDeath(in_cell);
  }
  
  // If HGT is turned on and there's a possibility of natural competence,
  // this organism's genome needs to be split up into fragments
  // and deposited in its cell.  We then also have to add the size of this genome to
  // the HGT resource.
  if (m_world->GetConfig().ENABLE_HGT.Get()
      && (m_world->GetConfig().HGT_COMPETENCE_P.Get() > 0.0)) {
    ConstInstructionSequencePtr seq;
    seq.DynamicCastFrom(organism->GetGenome().Representation());
    in_cell.AddGenomeFragments(ctx, *seq);
  }
  
  // And clear it!
  in_cell.RemoveOrganism(ctx); 
  if (!organism->IsRunning()) delete organism;
  else organism->GetPhenotype().SetToDelete();
  
  // Alert the scheduler that this cell has a 0 merit.
  AdjustSchedule(in_cell, cMerit(0));
}

void cPopulation::InjureOrg(cAvidaContext& ctx, cPopulationCell& in_cell, double injury, bool ding_reacs)
{
  if (injury == 0) return;
  cOrganism* target = in_cell.GetOrganism();
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    double target_merit = target->GetPhenotype().GetMerit().GetDouble();
    target_merit -= target_merit * injury;
    target->UpdateMerit(ctx, target_merit);
  }
  if (ding_reacs) {
    Apto::Array<int> target_reactions = target->GetPhenotype().GetLastReactionCount();
    for (int i = 0; i < target_reactions.GetSize(); i++) {
      target->GetPhenotype().SetReactionCount(i, target_reactions[i] - (int)((target_reactions[i] * injury)));
    }
  }
  const double target_bonus = target->GetPhenotype().GetCurBonus();
  target->GetPhenotype().SetCurBonus(target_bonus - (target_bonus * injury));
  
  if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
    Apto::Array<double> target_bins = target->GetRBins();
    for (int i = 0; i < target_bins.GetSize(); i++) {
      target->AddToRBin(i, -1 * (target_bins[i] * injury));
    }
  }
}

void cPopulation::Kaboom(cPopulationCell& in_cell, cAvidaContext& ctx, int distance) 
{
  m_world->GetStats().IncKaboom();
  m_world->GetStats().AddHamDistance(distance);
  cOrganism* organism = in_cell.GetOrganism();
  Apto::String ref_genome = organism->GetGenome().Representation()->AsString();
  int bgid = organism->SystematicsGroup("genotype")->ID();
  
  int radius = 2;
  
  for (int i = -1 * radius; i <= radius; i++) {
    for (int j = -1 * radius; j <= radius; j++) {
      cPopulationCell& death_cell = cell_array[GridNeighbor(in_cell.GetID(), world_x, world_y, i, j)];
      
      //do we actually have something to kill?
      if (death_cell.IsOccupied() == false) continue;
      
      cOrganism* org_temp = death_cell.GetOrganism();
      
      if (distance == 0) {
        int temp_id = org_temp->SystematicsGroup("genotype")->ID();
        if (temp_id != bgid){
          KillOrganism(death_cell, ctx);
          m_world->GetStats().IncKaboomKills();
        }

      } else {
        Apto::String genome_temp = org_temp->GetGenome().Representation()->AsString();
        int diff = 0;
        for (int i = 0; i < genome_temp.GetSize(); i++) if (genome_temp[i] != ref_genome[i]) diff++;
        if (diff > distance){
          m_world->GetStats().IncKaboomKills();
          KillOrganism(death_cell, ctx);
        }
      }
    }
  }
  KillOrganism(in_cell, ctx); 
  // @SLG my prediction = 92% and, 28 get equals
}


void cPopulation::SwapCells(int cell_id1, int cell_id2, cAvidaContext& ctx)
{
  // Sanity checks: Don't process if the cells are the same
  if (cell_id1 == cell_id2) return;
  
  cPopulationCell& cell1 = GetCell(cell_id1);
  cPopulationCell& cell2 = GetCell(cell_id2);
  
  // Clear current contents of cells
  cOrganism* org1 = cell1.RemoveOrganism(ctx); 
  cOrganism* org2 = cell2.RemoveOrganism(ctx); 
  
  if (org2 != NULL) {
    cell1.InsertOrganism(org2, ctx); 
    AdjustSchedule(cell1, org2->GetPhenotype().GetMerit());
  } else {
    AdjustSchedule(cell1, cMerit(0));
  }
  
  if (org1 != NULL) {
    cell2.InsertOrganism(org1, ctx); 
    cell2.IncVisits();  // Increment visit count
    AdjustSchedule(cell2, org1->GetPhenotype().GetMerit());
  } else {
    AdjustSchedule(cell2, cMerit(0));
  }
  
  //LHZ: Take organism imputs from the PopulationCell along with the organisms
  environment.SwapInputs(ctx, cell1.m_inputs, cell2.m_inputs);
  
}

// CompeteDemes  probabilistically copies demes into the next generation
// based on their fitness. How deme fitness is estimated is specified by
// competition_type input argument as:
/*
 0: deme fitness = 1 (control, random deme selection)
 1: deme fitness = number of births since last competition (default)
 2: deme fitness = average organism fitness at the current update (uses parent's fitness, so
 does not work with donations)
 3: deme fitness = average mutation rate at the current update
 4: deme fitness = strong rank selection on (parents) fitness (2^-deme fitness rank)
 5: deme fitness = average organism life (current, not parents) fitness (works with donations)
 6: deme fitness = strong rank selection on life (current, not parents) fitness
 */
//  For ease of use, each organism
// is setup as if it we just injected into the population.

void cPopulation::CompeteDemes(cAvidaContext& ctx, int competition_type)
{
  const int num_demes = deme_array.GetSize();
  
  double total_fitness = 0;
  Apto::Array<double> deme_fitness(num_demes);
  
  switch(competition_type) {
    case 0:    // deme fitness = 1;
      total_fitness = (double) num_demes;
      deme_fitness.SetAll(1);
      break;
    case 1:     // deme fitness = number of births
      // Determine the scale for fitness by totaling births across demes.
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        double cur_fitness = (double) deme_array[deme_id].GetBirthCount();
        deme_fitness[deme_id] = cur_fitness;
        total_fitness += cur_fitness;
      }
      break;
    case 2:    // deme fitness = average organism fitness at the current update
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_fitness;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype =
          GetCell(cur_cell).GetOrganism()->GetPhenotype();
          single_deme_fitness.Add(phenotype.GetFitness());
        }
        deme_fitness[deme_id] = single_deme_fitness.Ave();
        total_fitness += deme_fitness[deme_id];
      }
      break;
    case 3: 	// deme fitness = average mutation rate at the current update
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_div_type;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype =
          GetCell(cur_cell).GetOrganism()->GetPhenotype();
          assert(phenotype.GetDivType()>0);
          single_deme_div_type.Add(1/phenotype.GetDivType());
        }
        deme_fitness[deme_id] = single_deme_div_type.Ave();
        total_fitness += deme_fitness[deme_id];
      }
      break;
    case 4: 	// deme fitness = 2^(-deme fitness rank)
      // first find all the deme fitness values ...
    {
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_fitness;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
          single_deme_fitness.Add(phenotype.GetFitness());
        }
        deme_fitness[deme_id] = single_deme_fitness.Ave();
      }
      // ... then determine the rank of each deme based on its fitness
      Apto::Array<double> deme_rank(num_demes);
      deme_rank.SetAll(1);
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        for (int test_deme = 0; test_deme < num_demes; test_deme++) {
          if (deme_fitness[deme_id] < deme_fitness[test_deme]) {
            deme_rank[deme_id]++;
          }
        }
      }
      // ... finally, make deme fitness 2^(-deme rank)
      deme_fitness.SetAll(1);
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        for (int i = 0; i < deme_rank[deme_id]; i++) {
          deme_fitness[deme_id] = deme_fitness[deme_id]/2;
        }
        total_fitness += deme_fitness[deme_id];
      }
    }
      break;
    case 5:    // deme fitness = average organism life fitness at the current update
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_life_fitness;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
          single_deme_life_fitness.Add(phenotype.GetLifeFitness());
        }
        deme_fitness[deme_id] = single_deme_life_fitness.Ave();
        total_fitness += deme_fitness[deme_id];
      }
      break;
    case 6:     // deme fitness = 2^(-deme life fitness rank) (same as 4, but with life fitness)
      // first find all the deme fitness values ...
    {
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        cDoubleSum single_deme_life_fitness;
        const cDeme & cur_deme = deme_array[deme_id];
        for (int i = 0; i < cur_deme.GetSize(); i++) {
          int cur_cell = cur_deme.GetCellID(i);
          if (cell_array[cur_cell].IsOccupied() == false) continue;
          cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
          single_deme_life_fitness.Add(phenotype.GetLifeFitness());
        }
        deme_fitness[deme_id] = single_deme_life_fitness.Ave();
      }
      // ... then determine the rank of each deme based on its fitness
      Apto::Array<double> deme_rank(num_demes);
      deme_rank.SetAll(1);
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        for (int test_deme = 0; test_deme < num_demes; test_deme++) {
          if (deme_fitness[deme_id] < deme_fitness[test_deme]) {
            deme_rank[deme_id]++;
          }
        }
      }
      // ... finally, make deme fitness 2^(-deme rank)
      deme_fitness.SetAll(1);
      for (int deme_id = 0; deme_id < num_demes; deme_id++) {
        for (int i = 0; i < deme_rank[deme_id]; i++) {
          deme_fitness[deme_id] = deme_fitness[deme_id]/2;
        }
        total_fitness += deme_fitness[deme_id];
      }
    }
      break;
  }
  
  // Pick which demes should be in the next generation.
  Apto::Array<int> new_demes(num_demes);
  for (int i = 0; i < num_demes; i++) {
    double birth_choice = (double) ctx.GetRandom().GetDouble(total_fitness);
    double test_total = 0;
    for (int test_deme = 0; test_deme < num_demes; test_deme++) {
      test_total += deme_fitness[test_deme];
      if (birth_choice < test_total) {
        new_demes[i] = test_deme;
        break;
      }
    }
  }
  
  // Track how many of each deme we should have.
  Apto::Array<int> deme_count(num_demes);
  deme_count.SetAll(0);
  for (int i = 0; i < num_demes; i++) {
    deme_count[new_demes[i]]++;
  }
  
  Apto::Array<bool> is_init(num_demes);
  is_init.SetAll(false);
  
  // Copy demes until all deme counts are 1.
  while (true) {
    // Find the next deme to copy...
    int from_deme_id, to_deme_id;
    for (from_deme_id = 0; from_deme_id < num_demes; from_deme_id++) {
      if (deme_count[from_deme_id] > 1) break;
    }
    
    // Stop If we didn't find another deme to copy
    if (from_deme_id == num_demes) break;
    
    for (to_deme_id = 0; to_deme_id < num_demes; to_deme_id++) {
      if (deme_count[to_deme_id] == 0) break;
    }
    
    // We now have both a from and a to deme....
    deme_count[from_deme_id]--;
    deme_count[to_deme_id]++;
    
    cDeme& from_deme = deme_array[from_deme_id];
    cDeme& to_deme   = deme_array[to_deme_id];
    
    // Ideally, the below bit of code would be replaced with a call to ReplaceDeme:
    // ReplaceDeme(from_deme, to_deme);
    //
    // But, use of InjectClone messes that up, breaking consistency.  So the next
    // time that someone comes in here looking to refactor, consider fixing this.
    
    // Do the actual copy!
    for (int i = 0; i < from_deme.GetSize(); i++) {
      int from_cell_id = from_deme.GetCellID(i);
      int to_cell_id = to_deme.GetCellID(i);
      if (cell_array[from_cell_id].IsOccupied() == true) {
        InjectClone(to_cell_id, *(cell_array[from_cell_id].GetOrganism()), Systematics::Source(Systematics::DUPLICATION, ""));
      }
    }
    is_init[to_deme_id] = true;
  }
  
  // Now re-inject all remaining demes into themselves to reset them.
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    if (is_init[deme_id] == true) continue;
    cDeme & cur_deme = deme_array[deme_id];
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell_id = cur_deme.GetCellID(i);
      if (cell_array[cur_cell_id].IsOccupied() == false) continue;
      InjectClone(cur_cell_id, *(cell_array[cur_cell_id].GetOrganism()), cell_array[cur_cell_id].GetOrganism()->UnitSource());
    }
  }
  
  // Reset all deme stats to zero.
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    deme_array[deme_id].Reset(ctx, deme_array[deme_id].GetGeneration()); // increase deme generation by 1
  }
}




/*! Draw a sample (without replacement) from an input range, copying to the output range.
 */
template <typename ForwardIterator, typename OutputIterator, typename RNG>
void sample_without_replacement(ForwardIterator first, ForwardIterator last, OutputIterator ofirst, OutputIterator olast, RNG& rng) {
	std::size_t range = std::distance(first, last);
	std::size_t output_range = std::distance(ofirst, olast);
	
	// if our output range is greater in size than our input range, copy the whole thing.
	if(output_range >= range) {
		std::copy(first, last, ofirst);
		return;
	}
	
	std::vector<std::size_t> rmap(range);
  
  std::vector<std::size_t>::iterator rmap_it = rmap.begin();
  std::size_t value = 0;
  while (rmap_it != rmap.end()) {
    *rmap_it++ = value;
    ++value;
  }
  
	std::random_shuffle(rmap.begin(), rmap.end(), rng);
	
	while(ofirst != olast) {
		*ofirst = *(first + rmap.back());
		++ofirst;
		rmap.pop_back();
	}
}




/*! Compete all demes with each other based on the given vector of fitness values.
 
 This form of compete demes supports both fitness-proportional selection and a
 variant of tournament selection.  It integrates with the various deme replication options
 used in ReplicateDemes.
 
 Note: New deme competition fitness functions are added in PopulationActions.cc by subclassing
 cActionAbstractCompeteDemes and overriding cActionAbstractCompeteDemes::Fitness(cDeme&).  (Don't forget
 to register the action and add it to the events file).
 
 Another note: To mimic the behavior of the other version of CompeteDemes (which is kept around
 for backwards compatibility), change the config option DEMES_REPLICATE_SIZE to be the size of
 each deme.
 */
void cPopulation::CompeteDemes(const std::vector<double>& calculated_fitness, cAvidaContext& ctx) {
  // it's possible that we'll be changing the fitness values of some demes, so make a copy:
  std::vector<double> fitness(calculated_fitness);
  
  // Each deme must have a fitness:
  assert((int)fitness.size() == deme_array.GetSize());
  
  // To prevent sterile demes from replicating, we're going to replace the fitness
  // of all sterile demes with 0; this effectively makes it impossible for a sterile
  // deme to be selected via fitness proportional selection.
  if (m_world->GetConfig().DEMES_PREVENT_STERILE.Get()) {
    for(int i=0; i<deme_array.GetSize(); ++i) {
      if (deme_array[i].GetBirthCount() == 0) {
        fitness[i] = 0.0;
      }
    }
  }
  
  // Stat-tracking:
  m_world->GetStats().CompeteDemes(fitness);
  
  // This is to facilitate testing.  Obviously we can't do competition if there's
  // only one deme, but we do want the stat-tracking.
  if (fitness.size() == 1) {
    return;
  }
  
  // to facilitate control runs, sometimes we want to know what the fitness values
  // are, but we don't want competition to depend on them.
  if (m_world->GetConfig().DEMES_OVERRIDE_FITNESS.Get()) {
    for(int i=0; i<static_cast<int>(fitness.size()); ++i) {
      fitness[i] = 1.0;
    }
  }
  
  // Number of demes (at index) which should wind up in the next generation.
  std::vector<unsigned int> deme_counts(deme_array.GetSize(), 0);
  // Now, compete all demes based on the competition style.
  switch(m_world->GetConfig().DEMES_COMPETITION_STYLE.Get()) {
    case SELECTION_TYPE_PROPORTIONAL: {
      // Fitness-proportional selection.
      // Each deme has a probability equal to its fitness / sum(deme fitnesses)
      // of proceeding to the next generation.
      
      const double total_fitness = std::accumulate(fitness.begin(), fitness.end(), 0.0);
      assert(total_fitness > 0.0); // Must have *some* positive fitnesses...
      
      // Sum up the fitnesses until we reach or exceed the target fitness.
      // Then we're marking that deme as being part of the next generation.
      for (int i=0; i<deme_array.GetSize(); ++i) {
        double running_sum = 0.0;
        double target_sum = ctx.GetRandom().GetDouble(total_fitness);
        for (int j=0; j<deme_array.GetSize(); ++j) {
          running_sum += fitness[j];
          if (running_sum >= target_sum) {
            // j'th deme will be replicated.
            ++deme_counts[j];
            break;
          }
        }
      }
      break;
    }
    case SELECTION_TYPE_TOURNAMENT: {
      // Tournament selection.
      //
      // We run NUM_DEMES tournaments of size DEME_TOURNAMENT_SIZE, and select the
      // **single** winner of the tournament to proceed to the next generation.
      
      // construct a list of all possible deme ids that could participate in a tournament,
      // pruning out sterile and empty demes:
      std::vector<int> deme_ids;
      for (int i=0; i<deme_array.GetSize(); ++i) {
        if ((deme_array[i].GetOrgCount() > 0) && 
            (!m_world->GetConfig().DEMES_PREVENT_STERILE.Get() ||
             (deme_array[i].GetBirthCount() > 0))) {
              deme_ids.push_back(i);
            }
      }
      
      // better have more than deme tournament size, otherwise something is *really* screwed up:
      if (m_world->GetConfig().DEMES_TOURNAMENT_SIZE.Get() > static_cast<int>(deme_ids.size())) {
        ctx.Driver().Feedback().Error("Number of demes available to participate in a tournament < the deme tournament size.");
        ctx.Driver().Abort(Avida::INVALID_CONFIG);
      }
      
      // Run the tournaments.
      for (int i=0; i<m_world->GetConfig().NUM_DEMES.Get(); ++i) {
        // Which demes are in this tournament?
        std::vector<int> tournament(m_world->GetConfig().DEMES_TOURNAMENT_SIZE.Get());
        sample_without_replacement(deme_ids.begin(), deme_ids.end(),
                                   tournament.begin(), tournament.end(),
                                   ctx.GetRandom());
        
        // Now, iterate through the fitnesses of each of the tournament players,
        // capturing the winner's index and fitness.
        //
        // If no deme actually won, meaning no one had fitness greater than 0.0,
        // then the winner is selected at random from the tournament.
        std::pair<int, double> winner(tournament[ctx.GetRandom().GetInt(tournament.size())], 0.0);
        for(std::vector<int>::iterator j=tournament.begin(); j!=tournament.end(); ++j) {
          if (fitness[*j] > winner.second) {
            winner = std::make_pair(*j, fitness[*j]);
          }
        }
        
        // We have a winner!  Increment his replication count.
        ++deme_counts[winner.first];
      }
      break;
    }
    default: {
      // should never get here.
      assert(false);
    }
  }
  
  // Housekeeping: re-inject demes with count of 1 back into self (energy-related).
  for (int i = 0; i < (int)deme_counts.size(); i++) {
    if (deme_counts[i] == 1)
      ReplaceDeme(deme_array[i], deme_array[i], ctx); 
  }
  
  // Ok, the below algorithm relies upon the fact that we have a strict weak ordering
  // of fitness values for all demes.  We're going to loop through, find demes with a
  // count greater than one, and insert them into demes with a count of zero.
  while (true) {
    int source_id=0;
    for(; source_id<(int)deme_counts.size(); ++source_id) {
      if (deme_counts[source_id] > 1) {
        --deme_counts[source_id];
        break;
      }
    }
    
    if (source_id == (int)deme_counts.size()) {
      break; // All done; we looped through the whole list of counts, and didn't find any > 1.
    }
    
    int target_id=0;
    for(; target_id<(int)deme_counts.size(); ++target_id) {
      if (deme_counts[target_id] == 0) {
        ++deme_counts[target_id];
        break;
      }
    }
    
    assert(source_id < deme_array.GetSize());
    assert(target_id < deme_array.GetSize());
    assert(source_id != target_id);
    
    // Replace the target with a copy of the source:
    ReplaceDeme(deme_array[source_id], deme_array[target_id], ctx); 
  }
}


/* Check if any demes have met the critera to be replicated and do so.
 There are several bases this can be checked on:
 
 0: 'all'       - ...all non-empty demes in the population.
 1: 'full_deme' - ...demes that have been filled up.
 2: 'corners'   - ...demes with upper left and lower right corners filled.
 3: 'deme-age'  - ...demes who have reached their maximum age
 4: 'birth-count' ...demes that have had a certain number of births.
 5: 'sat-mov-pred'...demes whose movement predicate was previously satisfied
 6: 'events-killed' ...demes that have killed a certian number of events
 7: 'sat-msg-pred'...demes whose message predicate was previously satisfied
 8: 'sat-deme-predicate'...demes whose predicate has been satisfied; does not include movement or message predicates as those are organisms-level
 9: 'perf-reactions' ...demes that have performed X number of each task are replicated
 10:'consume-res' ...demes that have consumed a sufficienct amount of resources
 */
void cPopulation::ReplicateDemes(int rep_trigger, cAvidaContext& ctx) 
{
  assert(GetNumDemes()>1); // Sanity check.
  
  // Loop through all candidate demes...
  const int num_demes = GetNumDemes();
  for (int deme_id=0; deme_id<num_demes; ++deme_id) {
    cDeme& source_deme = deme_array[deme_id];
    
    // Test this deme to determine if it should be replicated.  If not,
    // continue on to the next deme.
    switch (rep_trigger) {
      case DEME_TRIGGER_ALL: {
        // Replicate all non-empty demes.
        if (source_deme.IsEmpty()) continue;
        break;
      }
      case DEME_TRIGGER_FULL: {
        // Replicate all full demes.
        if (!source_deme.IsFull()) continue;
        break;
      }
      case DEME_TRIGGER_CORNERS: {
        // Replicate all demes with the corners filled in.
        // The first and last IDs represent the two corners.
        const int id1 = source_deme.GetCellID(0);
        const int id2 = source_deme.GetCellID(source_deme.GetSize() - 1);
        if (cell_array[id1].IsOccupied() == false ||
            cell_array[id2].IsOccupied() == false) continue;
        break;
      }
      case DEME_TRIGGER_AGE: {
        // Replicate old demes.
        if (source_deme.GetAge() < m_world->GetConfig().DEMES_MAX_AGE.Get()) continue;
        break;
      }
      case DEME_TRIGGER_BIRTHS: {
        // Replicate demes that have had a certain number of births.
        if (source_deme.GetBirthCount() < m_world->GetConfig().DEMES_MAX_BIRTHS.Get()) continue;
        break;
      }
      case DEME_TRIGGER_MOVE_PREDATORS: {
        if (!(source_deme.MovPredSatisfiedPreviously())) continue;
        break;
      }
      case DEME_TRIGGER_GROUP_KILL: {
        int currentSlotSuccessful = 0;
        double kill_ratio = 0.0;
        
        if (source_deme.GetSlotFlowRate() == 0) {
          kill_ratio = 1.0;
        } else {
          kill_ratio = static_cast<double>(source_deme.GetEventsKilledThisSlot()) /
          static_cast<double>(source_deme.GetSlotFlowRate());
        }
        
        if (kill_ratio >= m_world->GetConfig().DEMES_MIM_EVENTS_KILLED_RATIO.Get()) {
          currentSlotSuccessful = 1;
        }
        
        // Replicate demes that have killed a certain number of event.
        if (source_deme.GetConsecutiveSuccessfulEventPeriods() + currentSlotSuccessful
            < m_world->GetConfig().DEMES_MIM_SUCCESSFUL_EVENT_PERIODS.Get()) {
          continue;
        }
        break;
      }
      case DEME_TRIGGER_MESSAGE_PREDATORS: {
        if (!(source_deme.MsgPredSatisfiedPreviously())) continue;
        break;
      }
      case DEME_TRIGGER_PREDICATE: {
        if (!(source_deme.DemePredSatisfiedPreviously())) continue;
        break;
      }
      case DEME_TRIGGER_PERFECT_REACTIONS: {
        // loop through each reaction. Make sure each has been performed X times.
        if (source_deme.MinNumTimesReactionPerformed() < m_world->GetConfig().REACTION_THRESH.Get()) {
          continue;
        }
        break;
      }
      case DEME_TRIGGER_CONSUME_RESOURCES: {
        // check how many resources have been consumed by the deme
        if (source_deme.GetTotalResourceAmountConsumed() <
            m_world->GetConfig().RES_FOR_DEME_REP.Get()) {
          continue;
        }
        break;
      }
      default: {
        cerr << "ERROR: Invalid replication trigger " << rep_trigger
        << " in cPopulation::ReplicateDemes()" << endl;
        assert(false);
      }
    }
    
    ReplicateDeme(source_deme, ctx); 
  }
}


/*! ReplicateDeme is a helper method for replicating a source deme.
 */
void cPopulation::ReplicateDeme(cDeme& source_deme, cAvidaContext& ctx) 
{
  // Doesn't make sense to try and replicate a deme that *has no organisms*.
  if (source_deme.IsEmpty()) return;
  
  source_deme.UpdateShannonAll();
  
  // Prevent sterile demes from replicating.
  if (m_world->GetConfig().DEMES_PREVENT_STERILE.Get() && (source_deme.GetBirthCount() == 0)) {
    // assumes that all group level tasks cannot be solved by a single organism
    source_deme.KillAll(ctx); 
    return;
  }
  
  // Update stats calculate how many different reactions the deme performed.
  double deme_performed_rx=0;
  Apto::Array<int> deme_reactions = source_deme.GetCurReactionCount();
  for(int i=0; i< deme_reactions.GetSize(); ++i) {
    //HJG
    if (deme_reactions[i] > 0){
      deme_performed_rx++;
    }
  }
  
  // calculate how many penalties were accrued by the orgs on average
  double switch_penalties = source_deme.GetNumSwitchingPenalties();
  double num_orgs_perf_reaction = source_deme.GetNumOrgsPerformedReaction();
  double shannon_div = source_deme.GetShannonMutualInformation();
  double per_reproductives = source_deme.GetPercentReproductives();
  
  if (switch_penalties > 0) {
    switch_penalties = (switch_penalties)/(source_deme.GetInjectedCount() + source_deme.GetBirthCount());
  }
  
  
  m_world->GetStats().IncDemeReactionDiversityReplicationData(deme_performed_rx, switch_penalties, shannon_div, num_orgs_perf_reaction, per_reproductives);
  
  //Option to bridge between kin and group selection.
  if (m_world->GetConfig().DEMES_REPLICATION_ONLY_RESETS.Get()) {
    //Reset deme (resources and births, among other things)
    bool source_deme_resource_reset = m_world->GetConfig().DEMES_RESET_RESOURCES.Get() == 0;
    source_deme.DivideReset(ctx, source_deme, source_deme_resource_reset);
    
    //Reset all organisms in deme, by re-injecting them?
    if (m_world->GetConfig().DEMES_REPLICATION_ONLY_RESETS.Get() == 2) {
      for (int i=0; i<source_deme.GetSize(); i++) {
        int cellid = source_deme.GetCellID(i);
        if (GetCell(cellid).IsOccupied()) {
          int lineage = GetCell(cellid).GetOrganism()->GetLineageLabel();
          const Genome& genome = GetCell(cellid).GetOrganism()->GetGenome();
          InjectGenome(cellid, Systematics::Source(Systematics::DUPLICATION, ""), genome, ctx, lineage); 
        }
      }
    }
    return;
  }
  
  // Pick a target deme to replicate to, making sure that
  // we don't try to replicate over ourself, i.e. DEMES_REPLACE_PARENT 0
  
  int target_id = -1;
  if (m_world->GetConfig().DEMES_PREFER_EMPTY.Get()) {
    
    //@JEB -- use empty_cell_id_array to hold empty demes
    //so we don't have to allocate a list
    int num_empty = 0;
    for (int i=0; i<GetNumDemes(); i++) {
      if (GetDeme(i).IsEmpty()) {
        empty_cell_id_array[num_empty] = i;
        num_empty++;
      }
    }
    if (num_empty > 0) {
      target_id = empty_cell_id_array[ctx.GetRandom().GetUInt(num_empty)];
    }
  }
  
  // if we haven't found one yet, choose a random one
  if (target_id == -1) {
    target_id = source_deme.GetID();
    const int num_demes = GetNumDemes();
    while(target_id == source_deme.GetID()) {
      target_id = ctx.GetRandom().GetUInt(num_demes);
    }
  }
  
  // Write some logging information if LOG_DEMES_REPLICATE is set.
  if ( (m_world->GetConfig().LOG_DEMES_REPLICATE.Get() == 1) &&
      (m_world->GetStats().GetUpdate() >= m_world->GetConfig().DEMES_REPLICATE_LOG_START.Get()) ) {
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_replication.dat");
    cString UpdateStr = cStringUtil::Stringf("%d,%d,%d", m_world->GetStats().GetUpdate(), source_deme.GetDemeID(), target_id);
    df->WriteRaw(UpdateStr);
  }
  
  if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 3) {
    // hjg - this hack is decidedly ugly. However, the current ReplaceDemes method
    // does some strange things to support the energy model (such as resetting demes prior
    // to assessing whether the replication will work) that cause strange behavior as 
    // part of the germ line sequestration code.
    ReplaceDemeFlaggedGermline(source_deme, deme_array[target_id], ctx); 
  } else {
    ReplaceDeme(source_deme, deme_array[target_id], ctx); 
  }
  
}

/*! ReplaceDeme is a helper method that handles all the different configuration
 options related to the replacement of a target deme by a source.  It works with
 both CompeteDemes and ReplicateDemes (and can be called directly via an event if
 so desired).
 
 @refactor Replace manual mutation with strategy pattern.
 */
void cPopulation::ReplaceDeme(cDeme& source_deme, cDeme& target_deme, cAvidaContext& ctx) 
{
  // Stats tracking; pre-replication hook.
  m_world->GetStats().DemePreReplication(source_deme, target_deme);
  
  // used to pass energy to offspring demes (set to zero if energy model is not enabled)
  double source_deme_energy(0.0), deme_energy_decay(0.0), parent_deme_energy(0.0), offspring_deme_energy(0.0);
  if (m_world->GetConfig().ENERGY_ENABLED.Get()) {
    double energyRemainingInSourceDeme = source_deme.CalculateTotalEnergy(ctx); 
    source_deme.SetEnergyRemainingInDemeAtReplication(energyRemainingInSourceDeme); 
    source_deme_energy = energyRemainingInSourceDeme + source_deme.GetTotalEnergyTestament(); 
    
    m_world->GetStats().SumEnergyTestamentAcceptedByDeme().Add(source_deme.GetTotalEnergyTestament());
    deme_energy_decay = 1.0 - m_world->GetConfig().FRAC_ENERGY_DECAY_AT_DEME_BIRTH.Get();
    parent_deme_energy = source_deme_energy * deme_energy_decay * (1.0 - m_world->GetConfig().FRAC_PARENT_ENERGY_GIVEN_TO_DEME_AT_BIRTH.Get());
    offspring_deme_energy = source_deme_energy * deme_energy_decay * m_world->GetConfig().FRAC_PARENT_ENERGY_GIVEN_TO_DEME_AT_BIRTH.Get();
  }
  
  bool target_successfully_seeded = true;
  
  
  bool source_deme_resource_reset(true), target_deme_resource_reset(true);
  switch(m_world->GetConfig().DEMES_RESET_RESOURCES.Get()) {
    case 0:
      // reset resource in both demes
      source_deme_resource_reset = target_deme_resource_reset = true;
      break;
    case 1:
      // reset resource only in target deme
      source_deme_resource_reset = false;
      target_deme_resource_reset = true;
      break;
    case 2:
      // do not reset either deme resource
      source_deme_resource_reset = target_deme_resource_reset = false;
      break;
    default:
      cout << "Undefined value " << m_world->GetConfig().DEMES_RESET_RESOURCES.Get() << " for DEMES_RESET_RESOURCES\n";
      exit(1);
  }
  
  // Reset both demes, in case they have any cleanup work to do.
  // Must reset target first for stats to be correctly updated!
  if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    // Transfer energy from source to target if we're using the energy model.
    if (target_successfully_seeded) target_deme.DivideReset(ctx, source_deme, target_deme_resource_reset, offspring_deme_energy);
    source_deme.DivideReset(ctx, source_deme, source_deme_resource_reset, parent_deme_energy);
  } else {
    // Default; reset both source and target.
    if (target_successfully_seeded) target_deme.DivideReset(ctx, source_deme, target_deme_resource_reset);
    source_deme.DivideReset(ctx, source_deme, source_deme_resource_reset);
  }
  
  
  // Are we using germlines?  If so, we need to mutate the germline to get the
  // genome that we're going to seed the target with.
  if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 1) {
    // @JEB Original germlines
    Genome next_germ(source_deme.GetGermline().GetLatest());
    InstructionSequencePtr seq;
    seq.DynamicCastFrom(next_germ.Representation());
    const cInstSet& instset = m_world->GetHardwareManager().GetInstSet(next_germ.Properties().Get(s_prop_id_instset).StringValue());
    
    if (m_world->GetConfig().GERMLINE_COPY_MUT.Get() > 0.0) {
      for(int i = 0; i < seq->GetSize(); ++i) {
        if (ctx.GetRandom().P(m_world->GetConfig().GERMLINE_COPY_MUT.Get())) {
          (*seq)[i] = instset.GetRandomInst(ctx);
        }
      }
    }
    
    if ((m_world->GetConfig().GERMLINE_INS_MUT.Get() > 0.0)
        && ctx.GetRandom().P(m_world->GetConfig().GERMLINE_INS_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(seq->GetSize() + 1);
      seq->Insert(mut_line, instset.GetRandomInst(ctx));
    }
    
    if ((m_world->GetConfig().GERMLINE_DEL_MUT.Get() > 0.0)
        && ctx.GetRandom().P(m_world->GetConfig().GERMLINE_DEL_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(seq->GetSize());
      seq->Remove(mut_line);
    }
    
    // Replace the target deme's germline with the source deme's, and add the newly-
    // mutated germ to ONLY the target's germline.  The source remains unchanged.
    target_deme.ReplaceGermline(source_deme.GetGermline());
    target_deme.GetGermline().Add(next_germ);
    
    // Germline stats tracking.
    m_world->GetStats().GermlineReplication(source_deme.GetGermline(), target_deme.GetGermline());
    
    // All done with the germline manipulation; seed each deme.
    SeedDeme(source_deme, source_deme.GetGermline().GetLatest(), Systematics::Source(Systematics::DUPLICATION, "germline"), ctx); 
    
    /* MJM - source and target deme could be the same!
     * Seeding the same deme twice probably shouldn't happen.
     */
    if (source_deme.GetDemeID() != target_deme.GetDemeID()) {
      SeedDeme(target_deme, target_deme.GetGermline().GetLatest(), Systematics::Source(Systematics::DUPLICATION, "germline"), ctx); 
    }
    
  } else if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 2) {
    // @JEB -- New germlines using cGenotype
    
    // get germline genotype
    int germline_genotype_id = source_deme.GetGermlineGenotypeID();
    Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
    Systematics::GroupPtr germline_genotype = classmgr->ArbiterForRole("genotype")->Group(germline_genotype_id);
    assert(germline_genotype);
    
    // create a new genome by mutation
    Genome mg(germline_genotype->Properties().Get("genome"));
    InstructionSequencePtr seq;
    seq.DynamicCastFrom(mg.Representation());
    cCPUMemory new_genome(*seq);
    const cInstSet& instset = m_world->GetHardwareManager().GetInstSet(mg.Properties().Get(s_prop_id_instset).StringValue());
    
    if (m_world->GetConfig().GERMLINE_COPY_MUT.Get() > 0.0) {
      for(int i=0; i < new_genome.GetSize(); ++i) {
        if (ctx.GetRandom().P(m_world->GetConfig().GERMLINE_COPY_MUT.Get())) {
          new_genome[i] = instset.GetRandomInst(ctx);
        }
      }
    }
    
    if ((m_world->GetConfig().GERMLINE_INS_MUT.Get() > 0.0)
        && ctx.GetRandom().P(m_world->GetConfig().GERMLINE_INS_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(new_genome.GetSize() + 1);
      new_genome.Insert(mut_line, instset.GetRandomInst(ctx));
    }
    
    if ((m_world->GetConfig().GERMLINE_DEL_MUT.Get() > 0.0)
        && ctx.GetRandom().P(m_world->GetConfig().GERMLINE_DEL_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(new_genome.GetSize());
      new_genome.Remove(mut_line);
    }
    
    *seq = new_genome;
    
    //Create a new genotype which is daughter to the old one.
    Systematics::UnitPtr unit(new cDemePlaceholderUnit(Systematics::Source(Systematics::DUPLICATION, "germline"), mg));
    
    Systematics::GroupMembershipPtr pgrp(new Systematics::GroupMembership(1));
    (*pgrp)[0] = germline_genotype;
    
    Systematics::GroupPtr new_germline_genotype = germline_genotype->ClassifyNewUnit(unit, pgrp);
    source_deme.ReplaceGermline(new_germline_genotype);
    target_deme.ReplaceGermline(new_germline_genotype);
    SeedDeme(source_deme, new_germline_genotype, Systematics::Source(Systematics::DUPLICATION, "germline"), ctx); 
    SeedDeme(target_deme, new_germline_genotype, Systematics::Source(Systematics::DUPLICATION, "germline"), ctx); 
    new_germline_genotype->RemoveUnit();
  } else {
    // Not using germlines; things are much simpler.  Seed the target from the source.
    target_successfully_seeded = SeedDeme(source_deme, target_deme, ctx); 
  }
  
  
  // split energy from parent deme evenly among orgs in child deme
  if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1 && m_world->GetConfig().ENERGY_PASSED_ON_DEME_REPLICATION_METHOD.Get() == 0) {
    assert(source_deme.GetOrgCount() > 0 && target_deme.GetOrgCount() > 0);
    if (offspring_deme_energy > 0.0) {
      // split deme energy evenly between organisms in target deme
      double totalEnergyInjectedIntoOrganisms(0.0);
      for (int i=0; i < target_deme.GetSize(); i++) {
        int cellid = target_deme.GetCellID(i);
        cPopulationCell& cell = m_world->GetPopulation().GetCell(cellid);
        if (cell.IsOccupied()) {
          cOrganism* organism = cell.GetOrganism();
          cPhenotype& phenotype = organism->GetPhenotype();
          phenotype.SetEnergy(phenotype.GetStoredEnergy() + offspring_deme_energy/static_cast<double>(target_deme.GetOrgCount()));
          phenotype.SetMerit(cMerit(phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy() * phenotype.GetEnergyUsageRatio())));
          totalEnergyInjectedIntoOrganisms += phenotype.GetStoredEnergy();
        }
      }
      target_deme.SetEnergyInjectedIntoOrganisms(totalEnergyInjectedIntoOrganisms);
    }
    if (parent_deme_energy > 0.0) {
      // split deme energy evenly between organisms in source deme
      double totalEnergyInjectedIntoOrganisms(0.0);
      for (int i=0; i < source_deme.GetSize(); i++) {
        int cellid = source_deme.GetCellID(i);
        cPopulationCell& cell = m_world->GetPopulation().GetCell(cellid);
        if (cell.IsOccupied()) {
          cOrganism* organism = cell.GetOrganism();
          cPhenotype& phenotype = organism->GetPhenotype();
          phenotype.SetEnergy(phenotype.GetStoredEnergy() + parent_deme_energy/static_cast<double>(source_deme.GetOrgCount()));
          phenotype.SetMerit(cMerit(phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy() * phenotype.GetEnergyUsageRatio())));
          totalEnergyInjectedIntoOrganisms += phenotype.GetStoredEnergy();
        }
      }
      source_deme.SetEnergyInjectedIntoOrganisms(totalEnergyInjectedIntoOrganisms);
    }
  }
  
  
  
  // The source's merit must be transferred to the target, and then the source has
  // to rotate its heritable merit to its current merit.
  if (target_successfully_seeded) target_deme.UpdateDemeMerit(source_deme);
  source_deme.UpdateDemeMerit();
  source_deme.ClearShannonInformationStats();
  target_deme.ClearShannonInformationStats();
  
  // do our post-replication stats tracking.
  m_world->GetStats().DemePostReplication(source_deme, target_deme);
}

/*! ReplaceDemeFlaggedGermline is a helper method that handles deme replication when the organisms are flagging their own germ line. It is similar to ReplaceDeme, but some events are reordered. (Demes are reset only after we know that the replication will work. In addition, it only supports a small subset of the deme replication options.) 
 */
void cPopulation::ReplaceDemeFlaggedGermline(cDeme& source_deme, cDeme& target_deme, cAvidaContext& ctx2) 
{
  
  bool target_successfully_seeded = true;
  
  /* Seed deme part... */
  Apto::Random& random = m_world->GetRandom();
  //bool successfully_seeded = true;
  Apto::Array<cOrganism*> source_founders; // List of organisms we're going to transfer.
  Apto::Array<cOrganism*> target_founders; // List of organisms we're going to transfer.
  
  // Grab a random org from the set of orgs that have
  // flagged themselves as part of the germline.
  Apto::Array<cOrganism*> potential_founders; // List of organisms we might transfer.
  
  // Get list of potential founders
  for (int i = 0; i<source_deme.GetSize(); ++i) {
    int cellid = source_deme.GetCellID(i);
    if (cell_array[cellid].IsOccupied()) {
      cOrganism* o = cell_array[cellid].GetOrganism();
      if (o->IsGermline()) {
        potential_founders.Push(o);
      }
    }
  }
  
  
  if (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() == 8) {
    // pick a random founder...
    if (potential_founders.GetSize() > 0) {
      int r = random.GetUInt(potential_founders.GetSize());
      target_founders.Push(potential_founders[r]);
    } else {
      return;
    }
  } else {
    target_founders = potential_founders;
  }
  // Stats tracking; pre-replication hook.
  m_world->GetStats().DemePreReplication(source_deme, target_deme);
  
  
  bool source_deme_resource_reset(true), target_deme_resource_reset(true);
  switch(m_world->GetConfig().DEMES_RESET_RESOURCES.Get()) {
    case 0:
      // reset resource in both demes
      source_deme_resource_reset = target_deme_resource_reset = true;
      break;
    case 1:
      // reset resource only in target deme
      source_deme_resource_reset = false;
      target_deme_resource_reset = true;
      break;
    case 2:
      // do not reset either deme resource
      source_deme_resource_reset = target_deme_resource_reset = false;
      break;
    default:
      cout << "Undefined value " << m_world->GetConfig().DEMES_RESET_RESOURCES.Get() << " for DEMES_RESET_RESOURCES\n";
      exit(1);
  }
  
  if (target_successfully_seeded) target_deme.DivideReset(ctx2, source_deme, target_deme_resource_reset);
  source_deme.DivideReset(ctx2, source_deme, source_deme_resource_reset);
  
  
  target_deme.ClearFounders();
  target_deme.UpdateStats();
  target_deme.KillAll(ctx2);
  std::vector<std::pair<int, std::string> > track_founders;
  
  for(int i=0; i<target_founders.GetSize(); i++) {
    
    // this is the genome we need to use. However, we only need part of it...since it can include an offspring
    cCPUMemory in_memory_genome = target_founders[i]->GetHardware().GetMemory();
    
    // this is the genotype of the organism, which does not reflect any point mutations that have occurred. 
    // we need to use it to get the right length for the genome
    Systematics::GroupPtr parent_bg = target_founders[i]->SystematicsGroup("genotype");
    Genome mg(parent_bg->Properties().Get("genome"));
    InstructionSequencePtr seq;
    seq.DynamicCastFrom(mg.Representation());
    cCPUMemory new_genome(*seq);

    const cInstSet& instset = m_world->GetHardwareManager().GetInstSet(mg.Properties().Get(s_prop_id_instset).StringValue());
    
    if (m_world->GetConfig().GERMLINE_COPY_MUT.Get() > 0.0) {
      for(int i=0; i<new_genome.GetSize(); ++i) {
        if (m_world->GetRandom().P(m_world->GetConfig().GERMLINE_COPY_MUT.Get())) {
          new_genome[i] = instset.GetRandomInst(ctx2);
        } else if (in_memory_genome.GetSize() > i){
          // this line copies the mutations accured as a result of performing tasks to the new genome
          new_genome[i] = in_memory_genome[i]; 
        }
      }
    }
    
    if ((m_world->GetConfig().GERMLINE_INS_MUT.Get() > 0.0)
        && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_INS_MUT.Get())) {
      const unsigned int mut_line = ctx2.GetRandom().GetUInt(new_genome.GetSize() + 1);
      new_genome.Insert(mut_line, instset.GetRandomInst(ctx2));
    }
    
    if ((m_world->GetConfig().GERMLINE_DEL_MUT.Get() > 0.0)
        && m_world->GetRandom().P(m_world->GetConfig().GERMLINE_DEL_MUT.Get())) {
      const unsigned int mut_line = ctx2.GetRandom().GetUInt(new_genome.GetSize());
      new_genome.Remove(mut_line);
    }
    (*seq) = new_genome;

    int cellid = DemeSelectInjectionCell(target_deme, i);       
    InjectGenome(cellid, Systematics::Source(Systematics::DUPLICATION, "germline"), mg, ctx2, target_founders[i]->GetLineageLabel());
    

    // At this point, the cell had better be occupied...
    assert(GetCell(cellid).IsOccupied());
    cOrganism * organism = GetCell(cellid).GetOrganism();
    
    // For now, just copy the generation...
    organism->GetPhenotype().SetGeneration(target_founders[i]->GetPhenotype().GetGeneration() );
    
    target_deme.AddFounder(organism->SystematicsGroup("genotype"), &organism->GetPhenotype());
    
    //track_founders.push_back(make_pair<int, Apto::String>(organism->SystematicsGroup("genotype")->ID(), Apto::String(new_genome.AsString()))); 
    
    ctx2.Driver().Feedback().Error("Temporarily disabled due to bizarre incompatibility on Windows with Visual Studio 2012");
    ctx2.Driver().Abort(Avida::INVALID_CONFIG);

    DemePostInjection(target_deme, cell_array[cellid]);
  }
  
  
  // For source deme...
  if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 1) {
    
    source_deme.UpdateStats();
    source_deme.KillAll(ctx2); 
    // do not clear or change founder list
    
    // use it to recreate ancestral state of genotypes
    Apto::Array<int>& source_founders = source_deme.GetFounderGenotypeIDs();
    Apto::Array<cPhenotype>& source_founder_phenotypes = source_deme.GetFounderPhenotypes();
    for(int i=0; i<source_founders.GetSize(); i++) {
      
      int cellid = DemeSelectInjectionCell(source_deme, i);
      //cout << "founder: " << source_founders[i] << endl;
      Systematics::GroupPtr bg = Systematics::Manager::Of(m_world->GetNewWorld())->ArbiterForRole("genotype")->Group(source_founders[i]);
      SeedDeme_InjectDemeFounder(cellid, bg, ctx2, &source_founder_phenotypes[i], -1, true); 
      DemePostInjection(source_deme, cell_array[cellid]);
    }
    
  } 
    
  source_deme.ClearTotalResourceAmountConsumed();
  
  source_deme.ClearShannonInformationStats();
  target_deme.ClearShannonInformationStats();
  
  // do our post-replication stats tracking.
  m_world->GetStats().DemePostReplication(source_deme, target_deme);
  m_world->GetStats().TrackDemeGLSReplication(source_deme.GetID(), target_deme.GetID(), track_founders);
  
  
}

/*! Helper method to seed a deme from the given genome.
 If the passed-in deme is populated, all resident organisms are terminated.  The
 deme's germline is left unchanged.
 
 @todo Fix lineage label on injected genomes.
 @todo Different strategies for non-random placement.
 */
void cPopulation::SeedDeme(cDeme& deme, Genome& genome, Systematics::Source src, cAvidaContext& ctx) { 
  // Kill all the organisms in the deme.
  deme.KillAll(ctx); 
  
  // Create the specified number of organisms in the deme.
  for(int i=0; i< m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
    int cellid = DemeSelectInjectionCell(deme, i);
    InjectGenome(cellid, src, genome, ctx, 0); 
    DemePostInjection(deme, cell_array[cellid]);
  }
}

void cPopulation::SeedDeme(cDeme& _deme, Systematics::GroupPtr bg, Systematics::Source src, cAvidaContext& ctx) { 
  // Kill all the organisms in the deme.
  _deme.KillAll(ctx); 
  _deme.ClearFounders();
  
  // Create the specified number of organisms in the deme.
  for(int i=0; i< m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
    int cellid = DemeSelectInjectionCell(_deme, i);
    InjectGenome(cellid, src, Genome(bg->Properties().Get("genome")), ctx); 
    DemePostInjection(_deme, cell_array[cellid]);
    _deme.AddFounder(bg);
  }
  
}

/*! Helper method to seed a target deme from the organisms in the source deme.
 All organisms in the target deme are terminated, and a subset of the organisms in
 the source will be cloned to the target. Returns whether target deme was successfully seeded.
 */
bool cPopulation::SeedDeme(cDeme& source_deme, cDeme& target_deme, cAvidaContext& ctx) { 
  Apto::Random& random = ctx.GetRandom();
  
  bool successfully_seeded = true;
  
  // Check to see if we're doing probabilistic organism replication from source
  // to target deme.
  if (m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get() == 0.0) {
    
    //@JEB -- old method is default for consistency!
    if (m_world->GetConfig().DEMES_SEED_METHOD.Get() == 0) {
      // Here's the idea: store up a list of the genotypes from the source that we
      // need to copy to the target. Then clear both the source and target demes,
      // and finally inject organisms from the saved genotypes into both the source
      // and target.
      //
      // This is a little ugly - Note that if you're trying to get something a little
      // more random, there's also the "fruiting body" option (DEMES_PROB_ORG_TRANSFER),
      // and the even less contrived MIGRATION_RATE.
      //
      // @todo In order to get lineage tracking to work again, we need to change this
      //       from tracking Genomes to tracking cGenotypes.  But that's a pain,
      //       because the cGenotype* from cOrganism::GetGenotype may not live after
      //       a call to cDeme::KillAll.
      std::vector<std::pair<Genome,int> > xfer; // List of genomes we're going to transfer.
      
      switch(m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()) {
        case 0: { // Random w/ replacement (meaning, we don't prevent the same genotype from
          // being selected more than once).
          while((int)xfer.size() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get()) {
            int cellid = source_deme.GetCellID(random.GetUInt(source_deme.GetSize()));
            if (cell_array[cellid].IsOccupied()) {
              xfer.push_back(std::make_pair(cell_array[cellid].GetOrganism()->GetGenome(),
                                            cell_array[cellid].GetOrganism()->GetLineageLabel()));
            }
          }
          break;
        }
        case 1: { // Sequential selection, from the beginning.  Good with DEMES_ORGANISM_PLACEMENT=3.
          for(int i=0; i<m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
            int cellid = source_deme.GetCellID(i);
            if (cell_array[cellid].IsOccupied()) {
              xfer.push_back(std::make_pair(cell_array[cellid].GetOrganism()->GetGenome(),
                                            cell_array[cellid].GetOrganism()->GetLineageLabel()));
            }
          }
          break;
        }
        default: {
          cout << "Undefined value (" << m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()
          << ") for DEMES_ORGANISM_SELECTION." << endl;
          exit(1);
        }
      }
      // We'd better have at *least* one genome.
      assert(xfer.size()>0);
      
      // Clear the demes.
      source_deme.UpdateStats();
      source_deme.KillAll(ctx); 
      
      target_deme.UpdateStats();
      target_deme.KillAll(ctx); 
      
      // And now populate the source and target.
      int j=0;
      for(std::vector<std::pair<Genome,int> >::iterator i=xfer.begin(); i!=xfer.end(); ++i, ++j) {
        int cellid = DemeSelectInjectionCell(source_deme, j);
        InjectGenome(cellid, Systematics::Source(Systematics::DUPLICATION, ""), i->first, ctx, i->second); 
        DemePostInjection(source_deme, cell_array[cellid]);
        
        if (source_deme.GetDemeID() != target_deme.GetDemeID()) {
          cellid = DemeSelectInjectionCell(target_deme, j);
          InjectGenome(cellid, Systematics::Source(Systematics::DUPLICATION, ""), i->first, ctx, i->second); 
          DemePostInjection(target_deme, cell_array[cellid]);
        }
        
      }
    } else /* if (m_world->GetConfig().DEMES_SEED_METHOD.Get() != 0) */{
      
      // @JEB
      // Updated seed deme method that maintains genotype inheritance.
      
      // deconstruct founders into two lists...
      Apto::Array<cOrganism*> source_founders; // List of organisms we're going to transfer.
      Apto::Array<cOrganism*> target_founders; // List of organisms we're going to transfer.
      
      
      switch(m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()) {
        case 0: { // Random w/ replacement (meaning, we don't prevent the same genotype from
          // being selected more than once).
          Apto::Array<cOrganism*> founders; // List of organisms we're going to transfer.
          while(founders.GetSize() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get()) {
            int cellid = source_deme.GetCellID(random.GetUInt(source_deme.GetSize()));
            if (cell_array[cellid].IsOccupied()) {
              founders.Push(cell_array[cellid].GetOrganism());
            }
          }
          source_founders = founders;
          target_founders = founders;
          break;
        }
        case 1: { // Sequential selection, from the beginning.  Good with DEMES_ORGANISM_PLACEMENT=3.
          Apto::Array<cOrganism*> founders; // List of organisms we're going to transfer.
          for(int i=0; i<m_world->GetConfig().DEMES_REPLICATE_SIZE.Get(); ++i) {
            int cellid = source_deme.GetCellID(i);
            if (cell_array[cellid].IsOccupied()) {
              founders.Push(cell_array[cellid].GetOrganism());
            }
          }
          source_founders = founders;
          target_founders = founders;
          break;
        }
        case 6: { // Random w/o replacement.  Take as many for each deme as DEME_REPLICATE_SIZE.
          tList<cOrganism> prospective_founders;
          
          //Die if not >= two organisms.
          if (source_deme.GetOrgCount() >= 2) {
            
            //Collect prospective founder organisms into a list
            for (int i=0; i < source_deme.GetSize(); i++) {
              if ( GetCell(source_deme.GetCellID(i)).IsOccupied() ) {
                prospective_founders.Push( GetCell(source_deme.GetCellID(i)).GetOrganism() );
              }
            }
            
            //add orgs alternately to source and target founders until
            //we run out or each reaches DEMES_REPLICATE_SIZE.
            int num_chosen_orgs = 0;
            while( ((target_founders.GetSize() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get())
                    || (source_founders.GetSize() < m_world->GetConfig().DEMES_REPLICATE_SIZE.Get()))
                  && (prospective_founders.GetSize() > 0) ) {
              
              int chosen_org = random.GetUInt(prospective_founders.GetSize());
              if (num_chosen_orgs % 2 == 0) {
                source_founders.Push(prospective_founders.PopPos(chosen_org));
              } else {
                target_founders.Push(prospective_founders.PopPos(chosen_org));
              }
              
              num_chosen_orgs++;
            }
          }
          break;
        }
        case 7: { // Grab the organisms that have flagged themselves as
          // part of the germline. Ignores replicate size...
          Apto::Array<cOrganism*> founders; // List of organisms we're going to transfer.
          for (int i = 0; i<source_deme.GetSize(); ++i) {
            cPopulationCell& cell = source_deme.GetCell(i);
            if (cell.IsOccupied()) {
              cOrganism* o = cell.GetOrganism();
              if (o->IsGermline()) {
                founders.Push(o);
              }
            }
          }
          
          source_founders = founders;
          target_founders = founders;
          break;
        }
        case 8: { // Grab a random org from the set of orgs that have
          // flagged themselves as part of the germline.
          Apto::Array<cOrganism*> potential_founders; // List of organisms we might transfer.
          Apto::Array<cOrganism*> founders; // List of organisms we're going to transfer.
          
          // Get list of potential founders
          for (int i = 0; i<source_deme.GetSize(); ++i) {
            int cellid = source_deme.GetCellID(i);
            if (cell_array[cellid].IsOccupied()) {
              cOrganism* o = cell_array[cellid].GetOrganism();
              if (o->IsGermline()) {
                potential_founders.Push(o);
              }
            }
          }
          
          // pick a random founder...
          if (potential_founders.GetSize() > 0) {
            int r = random.GetUInt(potential_founders.GetSize());
            founders.Push(potential_founders[r]);
          }
          source_founders = founders;
          target_founders = founders;
          break;          
        }
        case 2:
        case 3:
        case 4:
        case 5:
        { // Selection based on germline propensities.
          // 2: sum the germline propensities of all organisms
          // and pick TWO based on each organism getting a
          // weighted probability of being germline
          // 3: treat germline propensities as zero or nonzero for picking
          // 4: same as 3: but replication to target fails if only one germ.
          // 5: same as 3: but replication fails and source dies if fewer than two germs.
          Apto::Array<cOrganism*> founders; // List of organisms we're going to transfer.
          
          if (source_deme.GetOrgCount() >= 2) {
            
            if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() != 0) {
              ctx.Driver().Feedback().Error("Germline DEMES_ORGANISM_SELECTION methods 2 and 3 can only be used with DEMES_DIVIDE_METHOD 0.");
              ctx.Driver().Abort(Avida::INVALID_CONFIG);
            }
            
            Apto::Array<cOrganism*> prospective_founders;
            
            cDoubleSum gp_sum;
            double min = -1;
            for (int i=0; i < source_deme.GetSize(); i++) {
              if ( GetCell(source_deme.GetCellID(i)).IsOccupied() ) {
                double gp = GetCell(source_deme.GetCellID(i)).GetOrganism()->GetPhenotype().GetPermanentGermlinePropensity();
                if (gp > 0.0) {
                  gp_sum.Add( gp );
                  prospective_founders.Push( GetCell(source_deme.GetCellID(i)).GetOrganism() );
                }
                //cout << gp << " ";
                if ( (min == -1) || (gp < min) ) min = gp;
              }
            }
            
            if (m_world->GetVerbosity() >= VERBOSE_ON) cout << "Germline Propensity Sum: " << gp_sum.Sum() << endl;
            if (m_world->GetVerbosity() >= VERBOSE_ON) cout << "Num prospective founders: " << prospective_founders.GetSize() << endl;
            
            if (prospective_founders.GetSize() < 2) {
              
              // there were not enough orgs with nonzero germlines
              // pick additional orgs at random without replacement,
              // unless our method forbids this
              
              // leave the founder list empty for method 5
              if (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() != 5) {
                
                founders = prospective_founders;
                
                //do not add additional founders randomly for method 4
                if (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() != 4) {
                  
                  while(founders.GetSize() < 2) {
                    int cellid = source_deme.GetCellID(random.GetUInt(source_deme.GetSize()));
                    if ( cell_array[cellid].IsOccupied() ) {
                      cOrganism * org = cell_array[cellid].GetOrganism();
                      bool found = false;
                      for(int i=0; i< founders.GetSize(); i++) {
                        if (founders[i] == org) found = true;
                      }
                      if (!found) founders.Push(cell_array[cellid].GetOrganism());
                    }
                  }
                }
              }
            } else {
              
              // pick two orgs based on germline propensities from prospective founders
              
              while(founders.GetSize() < 2) {
                
                double choice = (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() == 2)
                ? random.GetDouble( gp_sum.Sum() ) : random.GetDouble( gp_sum.Count() );
                
                
                //cout <<  "Count: " << gp_sum.Count() << endl;
                
                // find the next organism to choose
                cOrganism * org = NULL;
                int on = 0;
                while( (choice > 0) && (on < prospective_founders.GetSize()) ) {
                  org = prospective_founders[on];
                  
                  // did we already have this org?
                  bool found = false;
                  for(int i=0; i< founders.GetSize(); i++) {
                    if (founders[i] == org) found = true;
                  }
                  
                  // if it wasn't already chosen, then we count down...
                  if (!found) {
                    choice -= (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() == 2)
                    ? org->GetPhenotype().GetPermanentGermlinePropensity()
                    : (org->GetPhenotype().GetPermanentGermlinePropensity() > 0);
                  }
                  on++;
                }
                
                gp_sum.Subtract(org->GetPhenotype().GetPermanentGermlinePropensity());
                assert(org);
                founders.Push(org);
              }
            }
            
            if (founders.GetSize() > 0) source_founders.Push(founders[0]);
            if (founders.GetSize() > 1) target_founders.Push(founders[1]);
            
            /*
             // Debug Code
             cout << endl;
             cout << "sum " << gp_sum.Sum() << endl;
             cout << "min " << min << endl;
             cout << "choice " << choice << endl;
             */
            
            //cout << "Chose germline propensity " << chosen_org->GetPhenotype().GetLastGermlinePropensity() << endl;
          } else {
            //Failure because we didn't have at least two organisms...
            //m_world->GetDriver().RaiseFatalException(1, "Germline DEMES_ORGANISM_SELECTION method didn't find at least two organisms in deme.");
          }
          
          break;
        }
        default: {
          cout << "Undefined value (" << m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get()
          << ") for DEMES_ORGANISM_SELECTION." << endl;
          exit(1);
        }
      }
      
      
      // We'd better have at *least* one genome.
      // Methods that require a germline can sometimes come up short...
      //assert(source_founders.GetSize()>0);
      //assert(target_founders.GetSize()>0);
      if(source_founders.GetSize() == 0) {
        return false;
      }
      
      // We clear the deme, but trick cPopulation::KillOrganism
      // to NOT delete the organisms, by pretending
      // the orgs are running. This way we can still create
      // clones of them that will track stats correctly.
      // We also need to defer adjusting the genotype
      // or it will be prematurely deleted before we are done!
      
      // cDoubleSum gen;
      Apto::Array<cOrganism*> old_source_organisms;
      for(int i=0; i<source_deme.GetSize(); ++i) {
        int cell_id = source_deme.GetCellID(i);
        
        if (cell_array[cell_id].IsOccupied()) {
          cOrganism * org = cell_array[cell_id].GetOrganism();
          old_source_organisms.Push(org);
          org->SetRunning(true);
        }
      }
      
      
      Apto::Array<cOrganism*> old_target_organisms;
      for(int i=0; i<target_deme.GetSize(); ++i) {
        int cell_id = target_deme.GetCellID(i);
        
        if (cell_array[cell_id].IsOccupied()) {
          cOrganism * org = cell_array[cell_id].GetOrganism();
          old_target_organisms.Push(org);
          org->SetRunning(true);
        }
      }
      
      // Clear the target deme (if we have successfully replaced it).
      if (target_founders.GetSize() > 0) {
        target_deme.ClearFounders();
        target_deme.UpdateStats();
        target_deme.KillAll(ctx);
      } else {
        successfully_seeded = false;
      }
      
      //cout << founders.GetSize() << " founders." << endl;
      
      // Now populate the target (and optionally the source) using InjectGenotype.
      // In the future InjectClone could be used, but this would require the
      // deme keeping complete copies of the founder organisms when
      // we wanted to re-seed from the original founders.
      for(int i=0; i<target_founders.GetSize(); i++) {
        int cellid = DemeSelectInjectionCell(target_deme, i);
        SeedDeme_InjectDemeFounder(cellid, target_founders[i]->SystematicsGroup("genotype"), ctx, &target_founders[i]->GetPhenotype(), target_founders[i]->GetLineageLabel(), false); 
        //target_deme.AddFounder(target_founders[i]->SystematicsGroup("genotype"), &target_founders[i]->GetPhenotype());
        DemePostInjection(target_deme, cell_array[cellid]);
      }
      
      for(int i=0; i<target_deme.GetSize(); ++i) {
        cPopulationCell& cell = target_deme.GetCell(i);
        if(cell.IsOccupied()) {
          target_deme.AddFounder(cell.GetOrganism()->SystematicsGroup("genotype"), &cell.GetOrganism()->GetPhenotype());
        }
      }
      
      // We either repeat this procedure in the source deme,
      // restart the source deme from its old founders,
      // or do nothing to the source deme...
      
      // source deme is replaced in the same way as the target
      if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 0) {
        
        source_deme.ClearFounders();
        source_deme.UpdateStats();
        source_deme.KillAll(ctx); 
        
        for(int i=0; i<source_founders.GetSize(); i++) {
          int cellid = DemeSelectInjectionCell(source_deme, i);
          SeedDeme_InjectDemeFounder(cellid, source_founders[i]->SystematicsGroup("genotype"), ctx, &source_founders[i]->GetPhenotype(), source_founders[i]->GetLineageLabel(), false); 
          source_deme.AddFounder(source_founders[i]->SystematicsGroup("genotype"), &source_founders[i]->GetPhenotype());
          DemePostInjection(source_deme, cell_array[cellid]);
        }
      }
      // source deme is "reset" by re-injecting founder genotypes
      else if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 1) {
        // Do not update the last founder generation, since the founders have not changed.
        
        source_deme.UpdateStats();
        source_deme.KillAll(ctx); 
        // do not clear or change founder list
        
        // use it to recreate ancestral state of genotypes
        Apto::Array<int>& source_founders = source_deme.GetFounderGenotypeIDs();
        Apto::Array<cPhenotype>& source_founder_phenotypes = source_deme.GetFounderPhenotypes();
        for(int i=0; i<source_founders.GetSize(); i++) {
          
          int cellid = DemeSelectInjectionCell(source_deme, i);
          //cout << "founder: " << source_founders[i] << endl;
          Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
          Systematics::GroupPtr bg = classmgr->ArbiterForRole("genotype")->Group(source_founders[i]);
          SeedDeme_InjectDemeFounder(cellid, bg, ctx, &source_founder_phenotypes[i], -1, true); 
          DemePostInjection(source_deme, cell_array[cellid]);
        }
      } else if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 2) {
        // source deme is left untouched
      } else if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 3) {
        source_deme.ClearTotalResourceAmountConsumed();
      } else if (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 4) {
        source_deme.ClearTotalResourceAmountConsumed();
        for(int i=0; i<source_founders.GetSize(); i++) {
          source_founders[i]->Die(ctx);
        }
      } else {
        ctx.Driver().Feedback().Error("Unknown DEMES_DIVIDE_METHOD");
        ctx.Driver().Abort(Avida::INVALID_CONFIG);
      }
      
      
      // remember to delete the old target organisms and adjust their genotypes
      for(int i=0; i<old_target_organisms.GetSize(); ++i) {
        old_target_organisms[i]->SetRunning(false);
        // ONLY delete target orgs if seeding was successful
        // otherwise they still exist in the population!!!
        if (successfully_seeded) delete old_target_organisms[i];
      }
      
      for(int i=0; i<old_source_organisms.GetSize(); ++i) {
        old_source_organisms[i]->SetRunning(false);
        
        // delete old source organisms ONLY if source was replaced
        if ( (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 0)
            || (m_world->GetConfig().DEMES_DIVIDE_METHOD.Get() == 1) ) {
          delete old_source_organisms[i];
        }
      }
      
    } //End DEMES_PROB_ORG_TRANSFER > 0 methods
  } else {
    // Probabilistic organism replication -- aka "fruiting body."
    //
    // This is a little different than the other deme replication styles.  First,
    // in a fruiting body we don't reset the individuals in the source deme; we
    // leave them unchanged.  The idea is that we're pulling random organisms
    // out, and letting the source continue.
    
    // Kill all the organisms in the target deme.
    target_deme.KillAll(ctx); 
    
    // Check all the organisms in the source deme to see if they get transferred
    // to the target.
    for(int i=0,j=0; i<source_deme.GetSize(); ++i) {
      int source_cellid = source_deme.GetCellID(i);
      
      // Does this organism stay with the source or move to the target?
      if (cell_array[source_cellid].IsOccupied() && random.P(m_world->GetConfig().DEMES_PROB_ORG_TRANSFER.Get())) {
        // Moves to the target; save the genome and lineage label of organism being transfered.
        cOrganism* seed = cell_array[source_cellid].GetOrganism();
        const Genome& genome = seed->GetGenome();
        int lineage = seed->GetLineageLabel();
        seed = 0; // We're done with the seed organism.
        
        // Remove this organism from the source.
        KillOrganism(cell_array[source_cellid], ctx); 
        
        // And inject it into target deme.
        int target_cellid = DemeSelectInjectionCell(target_deme, j++);
        InjectGenome(target_cellid, Systematics::Source(Systematics::DUPLICATION, ""), genome, ctx, lineage); 
        DemePostInjection(target_deme, cell_array[target_cellid]);
      }
      //else {
      // Stays with the source.  Nothing to do here yet.
      //}
    }
  }
  
  return successfully_seeded;
}

void cPopulation::SeedDeme_InjectDemeFounder(int _cell_id, Systematics::GroupPtr bg, cAvidaContext& ctx, cPhenotype* _phenotype, int lineage_label, bool reset)
{
  // Mutate the genome?
  if (m_world->GetConfig().DEMES_MUT_ORGS_ON_REPLICATION.Get() == 1 && !reset) {
    // MUTATE!
    
    // create a new genome by mutation
    Genome mg(bg->Properties().Get("genome"));
    InstructionSequencePtr seq;
    seq.DynamicCastFrom(mg.Representation());
    cCPUMemory new_genome(*seq);
    const cInstSet& instset = m_world->GetHardwareManager().GetInstSet(mg.Properties().Get(s_prop_id_instset).StringValue());
    
    if (m_world->GetConfig().GERMLINE_COPY_MUT.Get() > 0.0) {
      for(int i=0; i<new_genome.GetSize(); ++i) {
        if (ctx.GetRandom().P(m_world->GetConfig().GERMLINE_COPY_MUT.Get())) {
          new_genome[i] = instset.GetRandomInst(ctx);
        }
      }
    }
    
    if ((m_world->GetConfig().GERMLINE_INS_MUT.Get() > 0.0)
        && ctx.GetRandom().P(m_world->GetConfig().GERMLINE_INS_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(new_genome.GetSize() + 1);
      new_genome.Insert(mut_line, instset.GetRandomInst(ctx));
    }
    
    if ((m_world->GetConfig().GERMLINE_DEL_MUT.Get() > 0.0)
        && ctx.GetRandom().P(m_world->GetConfig().GERMLINE_DEL_MUT.Get())) {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(new_genome.GetSize());
      new_genome.Remove(mut_line);
    }
    *seq = new_genome;
    
    InjectGenome(_cell_id, Systematics::Source(Systematics::DUPLICATION, ""), mg, ctx, lineage_label); 
    
  } else {    
    // phenotype can be NULL
    InjectGenome(_cell_id, Systematics::Source(Systematics::DUPLICATION, ""), Genome(bg->Properties().Get("genome")), ctx, lineage_label);
  }
  
  // At this point, the cell had better be occupied...
  assert(GetCell(_cell_id).IsOccupied());
  cOrganism * organism = GetCell(_cell_id).GetOrganism();
  
  
  //Now we need to set up the phenotype of this organism...
  if (_phenotype) {
    //If we want founders to have their proper phenotypes
    // then we might do something like this... (untested)
    //organism->SetPhenotype(*_phenotype);
    // Re-initialize the time-slice for this new organism.
    //AdjustSchedule(_cell_id, organism->GetPhenotype().GetMerit());
    
    // For now, just copy the generation...
    organism->GetPhenotype().SetGeneration( _phenotype->GetGeneration() );
    
    // and germline propensity.
    organism->GetPhenotype().SetPermanentGermlinePropensity( _phenotype->GetPermanentGermlinePropensity()  );
    
    if (m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() >= 0.0) {
      organism->GetPhenotype().SetPermanentGermlinePropensity( m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() );
    }
  }
  
  /* It requires much more than this to correctly implement, do later if needed @JEB
   //Optionally, set the first organism's merit to a constant value
   //actually, we need to add so the first organism is seeded this way too...
   if (m_world->GetConfig().DEMES_DEFAULT_MERIT.Get()) {
   organism->GetPhenotype().SetMerit( cMerit(m_world->GetConfig().DEMES_DEFAULT_MERIT.Get()) );
   AdjustSchedule(GetCell(_cell_id), organism->GetPhenotype().GetMerit());
   }
   */
}


/*! Helper method that determines the cell into which an organism will be placed.
 Respects all of the different placement options that are relevant for deme replication.
 
 @param sequence The number of times DemeSelectInjectionCell has been called on this deme
 already.  Used for replicating multiple organisms from the source to the
 target deme in a sensible fashion.
 */
int cPopulation::DemeSelectInjectionCell(cDeme& deme, int sequence) {
  int cellid = -1;
  
  assert(sequence < deme.GetSize()); // cannot inject a sequence bigger then the deme
  
  switch(m_world->GetConfig().DEMES_ORGANISM_PLACEMENT.Get()) {
    case 0: { // Array-middle.
      cellid = deme.GetCellID((deme.GetSize()/2 + sequence) % deme.GetSize());
      break;
    }
    case 1: { // Sequential placement, start in the center of the deme.
      assert(sequence == 0);  // not sure how to handle a sequence in this case (Ben)
      cellid = deme.GetCellID(deme.GetWidth()/2, deme.GetHeight()/2);
      break;
    }
    case 2: { // Random placement.
      cellid = deme.GetCellID(m_world->GetRandom().GetInt(0, deme.GetSize()));
      while(cell_array[cellid].IsOccupied()) {
        cellid = deme.GetCellID(m_world->GetRandom().GetInt(0, deme.GetSize()));
      }
      break;
    }
    case 3: { // Sequential.
      cellid = deme.GetCellID(sequence);
      break;
    }
    default: {
      assert(false); // Shouldn't ever reach here.
    }
  }
  
  assert(cellid >= 0 && cellid < cell_array.GetSize());
  assert(cellid >= deme.GetCellID(0));
  assert(cellid <= deme.GetCellID(deme.GetSize()-1));
  return cellid;
}


/*! Helper method to perform any post-injection fixups on the organism/cell that
 was injected into a deme.  Handles all the rotation / facing options. Also increments
 the number of organisms injected into the deme.
 */
void cPopulation::DemePostInjection(cDeme& deme, cPopulationCell& cell) {
  assert(cell.GetID() >= deme.GetCellID(0));
  assert(cell.GetID() <= deme.GetCellID(deme.GetSize()-1));
  deme.IncInjectedCount();
  switch(m_world->GetConfig().DEMES_ORGANISM_FACING.Get()) {
    case 0: { // Unchanged.
      break;
    }
    case 1: { // Spin cell to face NW.
      cell.Rotate(cell_array[GridNeighbor(cell.GetID()-deme.GetCellID(0),
                                          deme.GetWidth(),
                                          deme.GetHeight(), -1, -1)+deme.GetCellID(0)]);
      break;
    }
    case 2: { // Spin cell to face randomly.
      const int rotate_count = m_world->GetRandom().GetInt(0, cell.ConnectionList().GetSize());
      for(int i=0; i<rotate_count; ++i) {
        cell.ConnectionList().CircNext();
      }
      break;
    }
    default: {
      assert(false);
    }
  }
}


// Loop through all demes to determine if any are ready to be divided.  All
// full demes have 1/2 of their organisms (the odd ones) moved into a new deme.

void cPopulation::DivideDemes(cAvidaContext& ctx) 
{
  // Determine which demes should be replicated.
  const int num_demes = GetNumDemes();
  Apto::Random& random = ctx.GetRandom();
  
  // Loop through all candidate demes...
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cDeme & source_deme = deme_array[deme_id];
    
    // Only divide full demes.
    if (source_deme.IsFull() == false) continue;
    
    // Choose a random target deme to replicate to...
    int target_id = deme_id;
    while (target_id == deme_id) target_id = random.GetUInt(num_demes);
    cDeme & target_deme = deme_array[target_id];
    const int deme_size = target_deme.GetSize();
    
    // Clear out existing cells in target deme.
    for (int i = 0; i < deme_size; i++) {
      KillOrganism(cell_array[ target_deme.GetCellID(i) ], ctx); 
    }
    
    // Setup an array to collect the total number of tasks performed.
    const int num_tasks = cell_array[source_deme.GetCellID(0)].GetOrganism()->
    GetPhenotype().GetLastTaskCount().GetSize();
    Apto::Array<int> tot_tasks(num_tasks);
    tot_tasks.SetAll(0);
    
    // Move over the odd numbered cells.
    for (int pos = 0; pos < deme_size; pos += 2) {
      const int cell1_id = source_deme.GetCellID( pos+1 );
      const int cell2_id = target_deme.GetCellID( pos );
      cOrganism * org1 = cell_array[cell1_id].GetOrganism();
      
      // Keep track of what tasks have been done.
      const Apto::Array<int> & cur_tasks = org1->GetPhenotype().GetLastTaskCount();
      for (int i = 0; i < num_tasks; i++) {
        tot_tasks[i] += cur_tasks[i];
      }
      
      // Inject a copy of the odd organisms into the even cells.
      InjectClone(cell2_id, *org1, Systematics::Source(Systematics::DUPLICATION, ""));
      
      // Kill the organisms in the odd cells.
      KillOrganism( cell_array[cell1_id], ctx); 
    }
    
    // Figure out the merit each organism should have.
    int merit = 100;
    for (int i = 0; i < num_tasks; i++) {
      if (tot_tasks[i] > 0) merit *= 2;
    }
    
    // Setup the merit of both old and new individuals.
    for (int pos = 0; pos < deme_size; pos += 2) {
      cell_array[source_deme.GetCellID(pos)].GetOrganism()->UpdateMerit(ctx, merit);
      cell_array[target_deme.GetCellID(pos)].GetOrganism()->UpdateMerit(ctx, merit);
    }
    
  }
}


// Reset Demes goes through each deme and resets the individual organisms as
// if they were just injected into the population.

void cPopulation::ResetDemes()
{
  // re-inject all demes into themselves to reset them.
  for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
    for (int i = 0; i < deme_array[deme_id].GetSize(); i++) {
      int cur_cell_id = deme_array[deme_id].GetCellID(i);
      if (cell_array[cur_cell_id].IsOccupied() == false) continue;
      InjectClone(cur_cell_id, *(cell_array[cur_cell_id].GetOrganism()), cell_array[cur_cell_id].GetOrganism()->UnitSource());
    }
  }
}


// Copy the full contents of one deme into another.

void cPopulation::CopyDeme(int deme1_id, int deme2_id, cAvidaContext& ctx) 
{
  cDeme & deme1 = deme_array[deme1_id];
  cDeme & deme2 = deme_array[deme2_id];
  
  for (int i = 0; i < deme1.GetSize(); i++) {
    int from_cell = deme1.GetCellID(i);
    int to_cell = deme2.GetCellID(i);
    if (cell_array[from_cell].IsOccupied() == false) {
      KillOrganism(cell_array[to_cell], ctx); 
      continue;
    }
    InjectClone(to_cell, *(cell_array[from_cell].GetOrganism()), Systematics::Source(Systematics::DUPLICATION, ""));
  }
}


// Print out statistics about donations

void cPopulation::PrintDonationStats()
{
  cDoubleSum donation_makers;
  cDoubleSum donation_receivers;
  cDoubleSum donation_cheaters;
  
  cDoubleSum edit_donation_makers;
  cDoubleSum edit_donation_receivers;
  cDoubleSum edit_donation_cheaters;
  
  cDoubleSum kin_donation_makers;
  cDoubleSum kin_donation_receivers;
  cDoubleSum kin_donation_cheaters;
  
  cDoubleSum threshgb_donation_makers;
  cDoubleSum threshgb_donation_receivers;
  cDoubleSum threshgb_donation_cheaters;
  
  cDoubleSum quanta_threshgb_donation_makers;
  cDoubleSum quanta_threshgb_donation_receivers;
  cDoubleSum quanta_threshgb_donation_cheaters;
  
  cStats& stats = m_world->GetStats();
  
  Avida::Output::FilePtr dn_donors = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "donations.dat");
  dn_donors->WriteComment("Info about organisms giving donations in the population");
  dn_donors->WriteTimeStamp();
  dn_donors->Write(stats.GetUpdate(), "update");
  
  
  for (int i = 0; i < cell_array.GetSize(); i++)
  {
    // Only look at cells with organisms in them.
    if (cell_array[i].IsOccupied() == false) continue;
    cOrganism * organism = cell_array[i].GetOrganism();
    const cPhenotype & phenotype = organism->GetPhenotype();
    
    // donors & receivers in general
    if (phenotype.IsDonorLast()) donation_makers.Add(1);  // Found donor
    if (phenotype.IsReceiverLast()) {
      donation_receivers.Add(1);                          // Found receiver
      if (phenotype.IsDonorLast()==0) {
        donation_cheaters.Add(1);                         // Found receiver with non-donor parent
      }
    }
    // edit donors & receivers
    if (phenotype.IsDonorEditLast()) edit_donation_makers.Add(1);  // Found edit donor
    if (phenotype.IsReceiverEditLast()) {
      edit_donation_receivers.Add(1);                              // Found edit receiver
      if (phenotype.IsDonorEditLast()==0) {
        edit_donation_cheaters.Add(1);                             // Found edit receiver whose parent did not make a edit donation
      }
    }
    
    // kin donors & receivers
    if (phenotype.IsDonorKinLast()) kin_donation_makers.Add(1); //found a kin donor
    if (phenotype.IsReceiverKinLast()){
      kin_donation_receivers.Add(1);                            //found a kin receiver
      if (phenotype.IsDonorKinLast()==0){
        kin_donation_cheaters.Add(1);                           //found a kin receiver whose parent did not make a kin donation
      }
    }
    
    // threshgb donors & receivers
    if (phenotype.IsDonorThreshGbLast()) threshgb_donation_makers.Add(1); //found a threshgb donor
    if (phenotype.IsReceiverThreshGbLast()){
      threshgb_donation_receivers.Add(1);                              //found a threshgb receiver
      if (phenotype.IsDonorThreshGbLast()==0){
        threshgb_donation_cheaters.Add(1);                             //found a threshgb receiver whose parent did...
      }                                                              //...not make a threshgb donation
    }
    
    // quanta_threshgb donors & receivers
    if (phenotype.IsDonorQuantaThreshGbLast()) quanta_threshgb_donation_makers.Add(1); //found a quanta threshgb donor
    if (phenotype.IsReceiverQuantaThreshGbLast()){
      quanta_threshgb_donation_receivers.Add(1);                              //found a quanta threshgb receiver
      if (phenotype.IsDonorQuantaThreshGbLast()==0){
        quanta_threshgb_donation_cheaters.Add(1);                             //found a quanta threshgb receiver whose parent did...
      }                                                              //...not make a quanta threshgb donation
    }
    
  }
  
  dn_donors->Write(donation_makers.Sum(), "parent made at least one donation");
  dn_donors->Write(donation_receivers.Sum(), "parent received at least one donation");
  dn_donors->Write(donation_cheaters.Sum(),  "parent received at least one donation but did not make one");
  dn_donors->Write(edit_donation_makers.Sum(), "parent made at least one edit_donation");
  dn_donors->Write(edit_donation_receivers.Sum(), "parent received at least one edit_donation");
  dn_donors->Write(edit_donation_cheaters.Sum(),  "parent received at least one edit_donation but did not make one");
  dn_donors->Write(kin_donation_makers.Sum(), "parent made at least one kin_donation");
  dn_donors->Write(kin_donation_receivers.Sum(), "parent received at least one kin_donation");
  dn_donors->Write(kin_donation_cheaters.Sum(),  "parent received at least one kin_donation but did not make one");
  dn_donors->Write(threshgb_donation_makers.Sum(), "parent made at least one threshgb_donation");
  dn_donors->Write(threshgb_donation_receivers.Sum(), "parent received at least one threshgb_donation");
  dn_donors->Write(threshgb_donation_cheaters.Sum(),  "parent received at least one threshgb_donation but did not make one");
  dn_donors->Write(quanta_threshgb_donation_makers.Sum(), "parent made at least one quanta_threshgb_donation");
  dn_donors->Write(quanta_threshgb_donation_receivers.Sum(), "parent received at least one quanta_threshgb_donation");
  dn_donors->Write(quanta_threshgb_donation_cheaters.Sum(),  "parent received at least one quanta_threshgb_donation but did not make one");
  
  dn_donors->Endl();
}
// Copy a single indvidual out of a deme into a new one (which is first purged
// of existing organisms.)

void cPopulation::SpawnDeme(int deme1_id, cAvidaContext& ctx, int deme2_id) 
{
  // Must spawn into a different deme.
  assert(deme1_id != deme2_id);
  
  const int num_demes = deme_array.GetSize();
  
  // If the second argument is a -1, choose a deme at random.
  Apto::Random& random = ctx.GetRandom();
  while (deme2_id == -1 || deme2_id == deme1_id) {
    deme2_id = random.GetUInt(num_demes);
  }
  
  // Make sure we have all legal values...
  assert(deme1_id >= 0 && deme1_id < num_demes);
  assert(deme2_id >= 0 && deme2_id < num_demes);
  
  // Find the demes that we're working with.
  cDeme & deme1 = deme_array[deme1_id];
  cDeme & deme2 = deme_array[deme2_id];
  
  // Make sure that the deme we're copying from has at least 1 organism.
  assert(deme1.GetOrgCount() > 0);
  
  // Determine the cell to copy from.
  int cell1_id = deme1.GetCellID( random.GetUInt(deme1.GetSize()) );
  while (cell_array[cell1_id].IsOccupied() == false) {
    cell1_id = deme1.GetCellID( random.GetUInt(deme1.GetSize()) );
  }
  
  // Clear out existing cells in target deme.
  for (int i = 0; i < deme2.GetSize(); i++) {
    KillOrganism(cell_array[ deme2.GetCellID(i) ], ctx); 
  }
  
  // And do the spawning.
  int cell2_id = deme2.GetCellID( random.GetUInt(deme2.GetSize()) );
  InjectClone(cell2_id, *(cell_array[cell1_id].GetOrganism()), Systematics::Source(Systematics::DUPLICATION, ""));
}

void cPopulation::AddDemePred(cString type, int times) {
  if (type == "EventReceivedCenter") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventReceivedCenterPred(times);
    }
  } else if (type == "EventReceivedLeftSide") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventReceivedLeftSidePred(times);
    }
  } else if (type == "EventMovedIntoCenter") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventMoveCenterPred(times);
    }
  } else if (type == "EventMovedBetweenTargets") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventMoveBetweenTargetsPred(times);
    }
  } else if (type == "EventNUniqueIndividualsMovedIntoTarget") {
    for (int deme_id = 0; deme_id < deme_array.GetSize(); deme_id++) {
      deme_array[deme_id].AddEventEventNUniqueIndividualsMovedIntoTargetPred(times);
    }
  } else {
    cout << "Unknown Predicate\n";
    exit(1);
  }
}

void cPopulation::CheckImplicitDemeRepro(cDeme& deme, cAvidaContext& ctx) {
  
  if (GetNumDemes() <= 1) return;
  
  if (m_world->GetConfig().DEMES_REPLICATE_CPU_CYCLES.Get()
      && (deme.GetTimeUsed() >= m_world->GetConfig().DEMES_REPLICATE_CPU_CYCLES.Get())) ReplicateDeme(deme, ctx); 
  else if (m_world->GetConfig().DEMES_REPLICATE_TIME.Get() 
           && (deme.GetNormalizedTimeUsed() >= m_world->GetConfig().DEMES_REPLICATE_TIME.Get())) ReplicateDeme(deme, ctx); 
  else if (m_world->GetConfig().DEMES_REPLICATE_BIRTHS.Get() 
           && (deme.GetBirthCount() >= m_world->GetConfig().DEMES_REPLICATE_BIRTHS.Get())) ReplicateDeme(deme, ctx); 
  else if (m_world->GetConfig().DEMES_REPLICATE_ORGS.Get() 
           && (deme.GetOrgCount() >= m_world->GetConfig().DEMES_REPLICATE_ORGS.Get())) ReplicateDeme(deme, ctx);      
}

// Print out all statistics about individual demes
void cPopulation::PrintDemeAllStats(cAvidaContext& ctx)
{
  PrintDemeFitness();
  PrintDemeLifeFitness();
  PrintDemeMerit();
  PrintDemeGestationTime();
  PrintDemeTasks();
  PrintDemeDonor();
  PrintDemeReceiver();
  PrintDemeMutationRate();
  PrintDemeResource(ctx); 
  PrintDemeInstructions();
  
  if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    PrintDemeSpatialEnergyData();
    PrintDemeSpatialSleepData();
  }
}

void cPopulation::PrintDemeTestamentStats(const cString& filename) {
  cStats& stats = m_world->GetStats();
  
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  df->WriteTimeStamp();
  df->Write(m_world->GetStats().GetUpdate(), "Update");
  df->Write(stats.SumEnergyTestamentToDemeOrganisms().Average(),     "Energy Testament to Deme Organisms");
  df->Write(stats.SumEnergyTestamentToFutureDeme().Average(),        "Energy Testament to Future Deme");
  df->Write(stats.SumEnergyTestamentToNeighborOrganisms().Average(), "Energy Testament to Neighbor Organisms");
  df->Write(stats.SumEnergyTestamentAcceptedByDeme().Average(),      "Energy Testament Accepted by Future Deme");
  df->Write(stats.SumEnergyTestamentAcceptedByOrganisms().Average(), "Energy Testament Accepted by Organisms");
  df->Endl();
  
  
  stats.SumEnergyTestamentToDemeOrganisms().Clear();
  stats.SumEnergyTestamentToFutureDeme().Clear();
  stats.SumEnergyTestamentToNeighborOrganisms().Clear();
}

void cPopulation::PrintCurrentMeanDemeDensity(const cString& filename) {
  cDoubleSum demeDensity;
  demeDensity.Clear();
  const int num_demes = deme_array.GetSize();
  for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    const cDeme & cur_deme = deme_array[deme_id];
    demeDensity.Add(cur_deme.GetDensity());
  }
  
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  df->WriteTimeStamp();
  df->Write(m_world->GetStats().GetUpdate(), "Update");
  df->Write(demeDensity.Average(), "Current mean deme density");
  df->Endl();
}

// Print some stats about the energy sharing behavior of each deme
void cPopulation::PrintDemeEnergySharingStats() {
  const int num_demes = deme_array.GetSize();
  cStats& stats = m_world->GetStats();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_enerty_sharing.dat");
  df->WriteComment("Average energy donation statistics for each deme in population");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  
  double num_requestors = 0.0;
  double num_donors = 0.0;
  double num_receivers = 0.0;
  double num_donations = 0.0;
  double num_receptions = 0.0;
  double num_applications = 0.0;
  double amount_donated = 0.0;
  double amount_received = 0.0;
  double amount_applied = 0.0;
  
  for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    const cDeme & cur_deme = deme_array[deme_id];
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      if (phenotype.IsEnergyRequestor()) num_requestors++;
      if (phenotype.IsEnergyDonor()) num_donors++;
      if (phenotype.IsEnergyReceiver()) num_receivers++;
      num_donations += phenotype.GetNumEnergyDonations();
      num_receptions += phenotype.GetNumEnergyReceptions();
      num_applications += phenotype.GetNumEnergyApplications();
      amount_donated += phenotype.GetAmountEnergyDonated();
      amount_received += phenotype.GetAmountEnergyReceived();
      amount_applied += phenotype.GetAmountEnergyApplied();
    }
  }
  
  df->Write(num_requestors/num_demes, "Average number of organisms that have requested energy");
  df->Write(num_donors/num_demes, "Average number of organisms that have donated energy");
  df->Write(num_receivers/num_demes, "Average number of organisms that have received energy");
  df->Write(num_donations/num_demes, "Average number of donations per deme");
  df->Write(num_receptions/num_demes, "Average number of receipts per deme");
  df->Write(num_applications/num_demes, "Average number of applications per deme");
  df->Write(amount_donated/num_demes, "Average total amount of energy donated per deme");
  df->Write(amount_received/num_demes, "Average total amount of energy received per deme");
  df->Write(amount_applied/num_demes, "Average total amount of donated energy applied per deme");
  df->Endl();
  
} //End PrintDemeEnergySharingStats()


// Print some stats about the distribution of energy among the cells in a deme
// If a cell is occupied, the amount returned is that of the organism in that cell
// If a cell is not occupied, the amount returned is the amount of energy resource in the cell
void cPopulation::PrintDemeEnergyDistributionStats(cAvidaContext& ctx) {
  const int num_demes = deme_array.GetSize();
  cStats& stats = m_world->GetStats();
  cString comment;
  
  cDoubleSum deme_energy_distribution;
  
  cDoubleSum overall_average;
  cDoubleSum overall_variance;
  cDoubleSum overall_stddev;
  
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_energy_distribution.dat");
  comment.Set("Average distribution of energy among cells in each of %d %d x %d demes", num_demes, world_x, world_y / num_demes);
  df->WriteComment(comment);
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "Update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    const cDeme & cur_deme = deme_array[deme_id];
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) {
        deme_energy_distribution.Add(cur_deme.GetCellEnergy(cur_cell, ctx));
        continue;
      }
      
      deme_energy_distribution.Add(GetCell(cur_cell).GetOrganism()->GetPhenotype().GetStoredEnergy());
    }
    
    overall_average.Add(deme_energy_distribution.Average());
    overall_variance.Add(deme_energy_distribution.Variance());
    overall_stddev.Add(deme_energy_distribution.StdDeviation());
    deme_energy_distribution.Clear();
    
  }
  
  df->Write(overall_average.Average(), "Average of Average Energy Level");
  df->Write(overall_variance.Average(), "Average of Energy Level Variance");
  df->Write(overall_stddev.Average(), "Average of Energy Level Standard Deviations");
  
  df->Endl();
  
} //End PrintDemeEnergyDistributionStats()


// Print some stats about the distribution of energy among the organism in a deme
// If a cell is occupied, the amount returned is that of the organism in that cell
// If a cell is not occupied, 0 energy is returned for that cell.
void cPopulation::PrintDemeOrganismEnergyDistributionStats() {
  const int num_demes = deme_array.GetSize();
  cStats& stats = m_world->GetStats();
  cString comment;
  
  cDoubleSum deme_energy_distribution;
  
  cDoubleSum overall_average;
  cDoubleSum overall_variance;
  cDoubleSum overall_stddev;
  
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_org_energy_distribution.dat");
  comment.Set("Average distribution of energy among organisms in each of %d %d x %d demes", num_demes, world_x, world_y / num_demes);
  df->WriteComment(comment);
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "Update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    const cDeme & cur_deme = deme_array[deme_id];
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) {
        deme_energy_distribution.Add(0);
        continue;
      }
      
      deme_energy_distribution.Add(GetCell(cur_cell).GetOrganism()->GetPhenotype().GetStoredEnergy());
    }
    
    overall_average.Add(deme_energy_distribution.Average());
    overall_variance.Add(deme_energy_distribution.Variance());
    overall_stddev.Add(deme_energy_distribution.StdDeviation());
    deme_energy_distribution.Clear();
    
  }
  
  df->Write(overall_average.Average(), "Average of Average Energy Level");
  df->Write(overall_variance.Average(), "Average of Energy Level Variance");
  df->Write(overall_stddev.Average(), "Average of Energy Level Standard Deviations");
  
  df->Endl();
  
} //End PrintDemeOrganismEnergyDistributionStats()


void cPopulation::PrintDemeDonor() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_donor.dat");
  df->WriteComment("Num orgs doing doing a donate for each deme in population");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_donor;
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_donor.Add(phenotype.IsDonorLast());
    }
    comment.Set("Deme %d", deme_id);
    df->Write(single_deme_donor.Sum(), comment);
  }
  df->Endl();
}

void cPopulation::PrintDemeFitness() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_fitness.dat");
  df->WriteComment("Average fitnesses for each deme in the population");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_fitness;
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_fitness.Add(phenotype.GetFitness());
    }
    comment.Set("Deme %d", deme_id);
    df->Write(single_deme_fitness.Ave(), comment);
  }
  df->Endl();
}

void cPopulation::PrintDemeTotalAvgEnergy(cAvidaContext& ctx) { 
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_totalAvgEnergy.dat");
  df->WriteComment("Average energy for demes in the population");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  cDoubleSum avg_energy;
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    const cDeme & cur_deme = deme_array[deme_id];
    avg_energy.Add(cur_deme.CalculateTotalEnergy(ctx)); 
  }
  df->Write(avg_energy.Ave(), "Total Average Energy");
  df->Endl();
}

void cPopulation::PrintDemeGestationTime()
{
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_gest_time.dat");
  df->WriteComment("Average gestation time for each deme in population");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_gest_time;
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_gest_time.Add(phenotype.GetGestationTime());
    }
    comment.Set("Deme %d", deme_id);
    df->Write(single_deme_gest_time.Ave(), comment);
  }
  df->Endl();
}

void cPopulation::PrintDemeInstructions()
{
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    for (int is_id = 0; is_id < m_world->GetHardwareManager().GetNumInstSets(); is_id++) {
      const cString& inst_set = m_world->GetHardwareManager().GetInstSet(is_id).GetInstSetName();
      int num_inst = m_world->GetHardwareManager().GetInstSet(is_id).GetSize();
      
      Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), Apto::FormatStr("deme_instruction-%d-%s.dat", deme_id, (const char*)inst_set));
      df->WriteComment(cStringUtil::Stringf("Number of times each instruction is exectued in deme %d", deme_id));
      df->WriteTimeStamp();
      df->Write(stats.GetUpdate(), "update");
      
      Apto::Array<Apto::Stat::Accumulator<int> > single_deme_inst(num_inst);
      
      const cDeme& cur_deme = deme_array[deme_id];
      for (int i = 0; i < cur_deme.GetSize(); i++) {
        int cur_cell = cur_deme.GetCellID(i);
        if (!cell_array[cur_cell].IsOccupied()) continue;
        if (cell_array[cur_cell].GetOrganism()->GetGenome().Properties().Get(s_prop_id_instset).StringValue() != inst_set) continue;
        cPhenotype& phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
        
        for (int j = 0; j < num_inst; j++) single_deme_inst[j].Add(phenotype.GetLastInstCount()[j]);
      }
      
      for (int j = 0; j < num_inst; j++) df->Write((int)single_deme_inst[j].Sum(), cStringUtil::Stringf("Inst %d", j));
      df->Endl();
    }
  }
}

void cPopulation::PrintDemeLifeFitness()
{
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_lifetime_fitness.dat");
  df->WriteComment("Average life fitnesses for each deme in the population");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_life_fitness;
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_life_fitness.Add(phenotype.GetLifeFitness());
    }
    comment.Set("Deme %d", deme_id);
    df->Write(single_deme_life_fitness.Ave(), comment);
  }
  df->Endl();
}

void cPopulation::PrintDemeMerit()
{
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_merit.dat");
  df->WriteComment("Average merits for each deme in population");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme& cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_merit;
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_merit.Add(phenotype.GetMerit().GetDouble());
    }
    comment.Set("Deme %d", deme_id);
    df->Write(single_deme_merit.Ave(), comment);
  }
  df->Endl();
}

//@JJB**
void cPopulation::PrintDemesMeritsData()
{
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_merits.dat");
  df->WriteComment("Each deme's current calculated merit");
  df->WriteTimeStamp();
  df->Write(m_world->GetStats().GetUpdate(), "Update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    comment.Set("Deme %d", deme_id);
    df->Write(deme_array[deme_id].CalcCurMerit().GetDouble(), comment);
  }
  df->Endl();
}

void cPopulation::PrintDemeMutationRate() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_mut_rates.dat");
  df->WriteComment("Average mutation rates for organisms in each deme");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  cDoubleSum total_mut_rate;
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_mut_rate;
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      single_deme_mut_rate.Add(GetCell(cur_cell).GetOrganism()->MutationRates().GetCopyMutProb());
    }
    comment.Set("Deme %d", deme_id);
    df->Write(single_deme_mut_rate.Ave(), comment);
    total_mut_rate.Add(single_deme_mut_rate.Ave());
  }
  df->Write(total_mut_rate.Ave(), "Average deme mutation rate averaged across Demes.");
  df->Endl();
}

void cPopulation::PrintDemeReceiver() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_receiver.dat");
  df->WriteComment("Num orgs doing receiving a donate for each deme in population");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    cDoubleSum single_deme_receiver;
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      single_deme_receiver.Add(phenotype.IsReceiver());
    }
    comment.Set("Deme %d", deme_id);
    df->Write(single_deme_receiver.Sum(), comment);
  }
  df->Endl();
}

void cPopulation::PrintDemeResource(cAvidaContext& ctx) { 
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_resources.dat");
  df->WriteComment("Avida deme resource data");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cDeme & cur_deme = deme_array[deme_id];
    
    cur_deme.UpdateDemeRes(ctx); 
    const cResourceCount& res = GetDeme(deme_id).GetDemeResourceCount();
    for(int j = 0; j < res.GetSize(); j++) {
      const char * tmp = res.GetResName(j);
      df->Write(res.Get(ctx, j), cStringUtil::Stringf("Deme %d Resource %s", deme_id, tmp)); //comment);
      if ((res.GetResourcesGeometry())[j] != nGeometry::GLOBAL && (res.GetResourcesGeometry())[j] != nGeometry::PARTIAL) {
        PrintDemeSpatialResData(res, j, deme_id, ctx); 
      }
    }
  }
  df->Endl();
}

//Write deme global resource levels to a file that can be easily read into Matlab.
//Each time this runs, a Matlab array is created that contains an array.  Each row in the array contains <deme id> <res level 0> ... <res level n>
void cPopulation::PrintDemeGlobalResources(cAvidaContext& ctx) {
  const int num_demes = deme_array.GetSize();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_global_resources.dat");
  df->WriteComment("Avida deme resource data");
  df->WriteTimeStamp();
  
  cString UpdateStr = cStringUtil::Stringf( "deme_global_resources_%07i = [ ...", m_world->GetStats().GetUpdate());
  df->WriteRaw(UpdateStr);
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cDeme & cur_deme = deme_array[deme_id];
    cur_deme.UpdateDemeRes(ctx);
    
    const cResourceCount & res = GetDeme(deme_id).GetDemeResourceCount();
    const int num_res = res.GetSize();
    
    df->WriteBlockElement(deme_id, 0, num_res + 1);
    
    for(int r = 0; r < num_res; r++) {
      if (!res.IsSpatial(r)) {
        df->WriteBlockElement(res.Get(ctx, r), r + 1, num_res + 1);
      }
      
    } //End iterating through resources
    
  } //End iterating through demes
  
  df->WriteRaw("];");
  df->Endl();
}


// Write spatial energy data to a file that can easily be read into Matlab
void cPopulation::PrintDemeSpatialEnergyData() const {
  int cellID = 0;
  int update = m_world->GetStats().GetUpdate();
  
  for(int i = 0; i < m_world->GetPopulation().GetNumDemes(); i++) {
    cString tmpfilename = cStringUtil::Stringf( "deme_%07i_spatial_energy.m", i);  // moved here for easy movie making
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)tmpfilename);
    cString UpdateStr = cStringUtil::Stringf( "deme_%07i_energy_%07i = [ ...", i, update );
    df->WriteRaw(UpdateStr);
    
    int gridsize = m_world->GetPopulation().GetDeme(i).GetSize();
    // write grid to file
    for (int j = 0; j < gridsize; j++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(cellID);
      if (cell.IsOccupied()) {
        df->WriteBlockElement(cell.GetOrganism()->GetPhenotype().GetStoredEnergy(), j, world_x);
      } else {
        df->WriteBlockElement(0.0, j, world_x);
      }
      cellID++;
    }
    df->WriteRaw("];");
    df->Endl();
  }
}

// Write spatial data to a file that can easily be read into Matlab
void cPopulation::PrintDemeSpatialResData(const cResourceCount& res, const int i, const int deme_id, cAvidaContext&) const { 
  const char* tmpResName = res.GetResName(i);
  cString tmpfilename = cStringUtil::Stringf( "deme_spatial_resource_%s.m", tmpResName );
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)tmpfilename);
  cString UpdateStr = cStringUtil::Stringf( "deme_%07i_%s_%07i = [ ...", deme_id, static_cast<const char*>(res.GetResName(i)), m_world->GetStats().GetUpdate() );
  
  df->WriteRaw(UpdateStr);
  
  const cSpatialResCount& sp_res = res.GetSpatialResource(i); 
  int gridsize = sp_res.GetSize();
  
  for (int j = 0; j < gridsize; j++) {
    df->WriteBlockElement(sp_res.GetAmount(j), j, world_x);
  }
  df->WriteRaw("];");
  df->Endl();
}

// Write spatial energy data to a file that can easily be read into Matlab
void cPopulation::PrintDemeSpatialSleepData() const {
  int cellID = 0;
  cString tmpfilename = "deme_spatial_sleep.m";
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)tmpfilename);
  int update = m_world->GetStats().GetUpdate();
  
  for(int i = 0; i < m_world->GetPopulation().GetNumDemes(); i++) {
    cString UpdateStr = cStringUtil::Stringf( "deme_%07i_sleep_%07i = [ ...", i, update);
    df->WriteRaw(UpdateStr);
    
    int gridsize = m_world->GetPopulation().GetDeme(i).GetSize();
    
    // write grid to file
    for (int j = 0; j < gridsize; j++) {
      cPopulationCell cell = m_world->GetPopulation().GetCell(cellID);
      if (cell.IsOccupied()) {
        df->WriteBlockElement(cell.GetOrganism()->IsSleeping(), j, world_x);
      } else {
        df->WriteBlockElement(0.0, j, world_x);
      }
      cellID++;
    }
    df->WriteRaw("];");
    df->Endl();
  }
}

void cPopulation::PrintDemeTasks() {
  cStats& stats = m_world->GetStats();
  const int num_demes = deme_array.GetSize();
  const int num_task = environment.GetNumTasks();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "deme_task.dat");
  df->WriteComment("Num orgs doing each task for each deme in population");
  df->WriteTimeStamp();
  df->Write(stats.GetUpdate(), "update");
  
  for (int deme_id = 0; deme_id < num_demes; deme_id++) {
    cString comment;
    const cDeme & cur_deme = deme_array[deme_id];
    Apto::Array<Apto::Stat::Accumulator<int> > single_deme_task(num_task);
    
    for (int i = 0; i < cur_deme.GetSize(); i++) {
      int cur_cell = cur_deme.GetCellID(i);
      if (cell_array[cur_cell].IsOccupied() == false) continue;
      cPhenotype & phenotype = GetCell(cur_cell).GetOrganism()->GetPhenotype();
      for (int j = 0; j < num_task; j++) {
        // only interested if task is done once!
        if (phenotype.GetLastTaskCount()[j] > 0) {
          single_deme_task[j].Add(1);
        }
      }
    }
    for (int j = 0; j < num_task; j++) {
      comment.Set("Deme %d, Task %d", deme_id, j);
      df->Write((int) single_deme_task[j].Sum(), comment);
    }
  }
  df->Endl();
}

void cPopulation::DumpDemeFounders(ofstream& fp) {
  fp << "#filetype deme_founders" << endl
  << "#format deme_id num_founders genotype_ids" << endl
  << endl
  << "#  1: Deme ID" << endl
  << "#  2: Number of founders" << endl
  << "#  3+: founder genotype ids" << endl << endl;
  
  for(int i=0; i<deme_array.GetSize(); i++) {
    
    if (deme_array[i].IsEmpty()) continue;
    
    Apto::Array<int>& deme_founders = deme_array[i].GetFounderGenotypeIDs();
    
    fp << i << " " << deme_founders.GetSize();
    for(int j=0; j<deme_founders.GetSize(); j++) {
      fp << " " << deme_founders[j];
    }
    
    fp << endl;
  }
}




/**
 This function will set up coalescence clade information.  If this feature is activated in the configuration,
 a list of coalescence genotype ids must be read in initially.  These are furnished by doing an initial run
 with the same seed and setup and retrieving information from the final dominant lineage and coalescence points.
 
 The value is either (by default) inherited from the parent or the organism's genotypeID if it is known
 to be a coalescence id.
 
 Defaulting is established in Inject or ActivateOffspring methods of this class.
 
 @MRR May 2007
 **/
void cPopulation::CCladeSetupOrganism(cOrganism*)
{
  //  int gen_id = organism->GetBioGroup("genotype")->GetID();
  if (m_world->GetConfig().TRACK_CCLADES.Get() > 0) {
    // @TODO - support for IsCCladeFounder?
    //    if (m_world->GetClassificationManager().IsCCladeFounder(gen_id)) organism->SetCCladeLabel(gen_id);
  }
}


/**
 * This function directs which position function should be used.  It
 * could have also been done with a function pointer, but the dividing
 * of an organism takes enough time that this will be a negligible addition,
 * and it gives a centralized function to work with.  The parent_ok flag asks
 * if it is okay to replace the parent.
 **/
//@AWC -- This could REALLY stand some functional abstraction...
cPopulationCell& cPopulation::PositionOffspring(cPopulationCell& parent_cell, cAvidaContext& ctx, bool parent_ok) 
{
  assert(parent_cell.IsOccupied());
  
  const int birth_method = m_world->GetConfig().BIRTH_METHOD.Get();
  
  // Handle Population Cap (if enabled)
  int pop_cap = m_world->GetConfig().POPULATION_CAP.Get();
  if (pop_cap > 0 && num_organisms >= pop_cap) {
    int num_kills = 1;
    
    while (num_kills > 0) {
      int target = ctx.GetRandom().GetUInt(live_org_list.GetSize());
      int cell_id = live_org_list[target]->GetCellID();
      if (cell_id == parent_cell.GetID()) { 
        target++;
        if (target >= live_org_list.GetSize()) target = 0;
        cell_id = live_org_list[target]->GetCellID();
      }
      KillOrganism(cell_array[cell_id], ctx); 
      num_kills--;
    }
  }
  
  // Handle Pop Cap Eldest (if enabled)  
  int pop_eldest = m_world->GetConfig().POP_CAP_ELDEST.Get();
  if (pop_eldest > 0 && num_organisms >= pop_eldest) {
    int num_kills = 1;
    
    while (num_kills > 0) {
      double max_age = 0.0;
      double max_msr = 0.0;
      int cell_id = 0;
      for (int i = 0; i < live_org_list.GetSize(); i++) {
        if (GetCell(live_org_list[i]->GetCellID()).IsOccupied() && live_org_list[i]->GetCellID() != parent_cell.GetID()) {       
          double age = live_org_list[i]->GetPhenotype().GetAge();
          if (age > max_age) {
            max_age = age;
            cell_id = live_org_list[i]->GetCellID();
          }
          else if (age == max_age) {
            double msr = ctx.GetRandom().GetDouble();
            if (msr > max_msr) {
              max_msr = msr;
              cell_id = live_org_list[i]->GetCellID();
            }
          }
        }
      }
      KillOrganism(cell_array[cell_id], ctx);
      num_kills--;
    }
  }
  
  // for juvs with non-predatory parents...
  if (m_world->GetConfig().MAX_PREY.Get() && m_world->GetStats().GetNumPreyCreatures() >= m_world->GetConfig().MAX_PREY.Get() && parent_cell.GetOrganism()->IsPreyFT()) {
    KillRandPrey(ctx, parent_cell.GetOrganism());
  }
  
	// increment the number of births in the **parent deme**.  in the case of a
	// migration, only the origin has its birth count incremented.
  if (deme_array.GetSize() > 0) {
    const int deme_id = parent_cell.GetDemeID();
    deme_array[deme_id].IncBirthCount();
  }
  
  // Decide if offspring will migrate to another deme -- if migrating we ignore the birth method.
  if (m_world->GetConfig().MIGRATION_RATE.Get() > 0.0 &&
      ctx.GetRandom().P(m_world->GetConfig().MIGRATION_RATE.Get())) {
    
    //cerr << "Attempting to migrate with rate " << m_world->GetConfig().MIGRATION_RATE.Get() << "!" << endl;
    int deme_id = parent_cell.GetDemeID();
    
    //get another -unadjusted- deme id
    int rnd_deme_id = ctx.GetRandom().GetInt(deme_array.GetSize()-1);
    
    //if the -unadjusted- id is above the excluded id, bump it up one
    //insures uniform prob of landing in any deme but the parent's
    if (rnd_deme_id >= deme_id) rnd_deme_id++;
    
    //set the new deme_id
    deme_id = rnd_deme_id;
    
    //The rest of this is essentially POSITION_OFFSPRING_DEME_RANDOM
    //@JEB: But note that this will not honor PREFER_EMPTY in the new deme.
    const int deme_size = deme_array[deme_id].GetSize();
    
    int out_pos = ctx.GetRandom().GetUInt(deme_size);
    int out_cell_id = deme_array[deme_id].GetCellID(out_pos);
    while (parent_ok == false && out_cell_id == parent_cell.GetID()) {
      out_pos = ctx.GetRandom().GetUInt(deme_size);
      out_cell_id = deme_array[deme_id].GetCellID(out_pos);
    }
    
    GetCell(out_cell_id).SetMigrant();
    return GetCell(out_cell_id);
  }
  // Fix so range can be arbitrary and we won't accidentally include ourself as a target
  // @AWC If not migrating try out global/full-deme birth methods first...
  
  // Similar to MIGRATION_RATE code above but allows for a bit more flexibility
  // in how migration between demes works. Also, respects PREFER_EMPTY in new deme.
  // Temporary until Deme
  if ((m_world->GetConfig().DEMES_MIGRATION_RATE.Get() > 0.0)
      && ctx.GetRandom().P(m_world->GetConfig().DEMES_MIGRATION_RATE.Get()))
  {
    return PositionDemeMigration(parent_cell, parent_ok);
  }
  
  // This block should be changed to a switch statment with functions handling
  // the cases. For now, a bunch of if's that return if they handle.
  
  if (birth_method == POSITION_OFFSPRING_FULL_SOUP_RANDOM) {
    // Look randomly within empty cells first, if requested
    if (m_world->GetConfig().PREFER_EMPTY.Get()) {
      int cell_id = FindRandEmptyCell(ctx);
      if (cell_id == -1) return GetCell(ctx.GetRandom().GetUInt(cell_array.GetSize()));
      else return GetCell(cell_id);
    }
    
    int out_pos = ctx.GetRandom().GetUInt(cell_array.GetSize());
    while (parent_ok == false && out_pos == parent_cell.GetID()) {
      out_pos = ctx.GetRandom().GetUInt(cell_array.GetSize());
    }
    return GetCell(out_pos);
  }
  
  if (birth_method == POSITION_OFFSPRING_FULL_SOUP_ELDEST) {
    cPopulationCell * out_cell = reaper_queue.PopRear();
    if (parent_ok == false && out_cell->GetID() == parent_cell.GetID()) {
      out_cell = reaper_queue.PopRear();
      reaper_queue.PushRear(&parent_cell);
    }
    return *out_cell;
  }
  
  if (birth_method == POSITION_OFFSPRING_DEME_RANDOM) {
    return PositionDemeRandom(parent_cell.GetDemeID(), parent_cell, parent_ok);
  }
  else if (birth_method == POSITION_OFFSPRING_PARENT_FACING) {
    return parent_cell.GetCellFaced();
  }
  else if (birth_method == POSITION_OFFSPRING_NEXT_CELL) {
    int out_cell_id = parent_cell.GetID() + 1;
    if (out_cell_id == cell_array.GetSize()) out_cell_id = 0;
    return GetCell(out_cell_id);
  }
  else if (birth_method == POSITION_OFFSPRING_FULL_SOUP_ENERGY_USED) {
    tList<cPopulationCell> found_list;
    int max_time_used = 0;
    for  (int i=0; i < cell_array.GetSize(); i++)
    {
      int time_used = cell_array[i].IsOccupied() ? cell_array[i].GetOrganism()->GetPhenotype().GetTimeUsed() : INT_MAX;
      if (time_used == max_time_used)
      {
        found_list.Push(&cell_array[i]);
      }
      else if (time_used > max_time_used)
      {
        max_time_used = time_used;
        found_list.Clear();
        found_list.Push(&cell_array[i]);
      }
    }
    int choice = ctx.GetRandom().GetUInt(found_list.GetSize());
    return *( found_list.GetPos(choice) );
  }
  
  // All remaining methods require us to choose among mulitple local positions.
  
  // Construct a list of equally viable locations to place the child...
  tList<cPopulationCell> found_list;
  
  // First, check if there is an empty organism to work with (always preferred)
  tList<cPopulationCell>& conn_list = parent_cell.ConnectionList();
  
  const bool prefer_empty = m_world->GetConfig().PREFER_EMPTY.Get();
  
  if (birth_method == POSITION_OFFSPRING_DISPERSAL && conn_list.GetSize() > 0) {
    tList<cPopulationCell>* disp_list = &conn_list;
    
    // hop through connection lists based on the dispersal rate
    int hops = ctx.GetRandom().GetRandPoisson(m_world->GetConfig().DISPERSAL_RATE.Get());
    for (int i = 0; i < hops; i++) {
      disp_list = &(disp_list->GetPos(ctx.GetRandom().GetUInt(disp_list->GetSize()))->ConnectionList());
      if (disp_list->GetSize() == 0) break;
    }
    
    // if prefer empty, select an empty cell from the final connection list
    if (prefer_empty) FindEmptyCell(*disp_list, found_list);
    
    // if prefer empty is off, or there are no empty cells, use the whole connection list as possiblities
    if (found_list.GetSize() == 0) {
      found_list = *disp_list;
      // if no hops were taken and ALLOW_PARENT is set, throw the parent cell into the hat for possible selection
      if (hops == 0 && parent_ok) found_list.Push(&parent_cell);
    }
  } else if (prefer_empty) {
    FindEmptyCell(conn_list, found_list);
  }
  
  // If we have not found an empty organism, we must use the specified function
  // to determine how to choose among the filled organisms.
  if (found_list.GetSize() == 0) {
    switch(birth_method) {
      case POSITION_OFFSPRING_AGE:
        PositionAge(parent_cell, found_list, parent_ok);
        break;
      case POSITION_OFFSPRING_MERIT:
        PositionMerit(parent_cell, found_list, parent_ok);
        break;
      case POSITION_OFFSPRING_RANDOM:
        found_list.Append(conn_list);
        if (parent_ok == true) found_list.Push(&parent_cell);
        break;
      case POSITION_OFFSPRING_NEIGHBORHOOD_ENERGY_USED:
        PositionEnergyUsed(parent_cell, found_list, parent_ok);
      case POSITION_OFFSPRING_EMPTY:
        // Nothing is in list if no empty cells are found...
        break;
    }
  }
  
  // If there are no possibilities, return parent.
  if (found_list.GetSize() == 0) return parent_cell;
  
  // Choose the organism randomly from those in the list, and return it.
  int choice = ctx.GetRandom().GetUInt(found_list.GetSize());
  return *( found_list.GetPos(choice) );
}

void cPopulation::PositionAge(cPopulationCell & parent_cell,
                              tList<cPopulationCell> & found_list,
                              bool parent_ok)
{
  // Start with the parent organism as the replacement, and see if we can find
  // anything equivilent or better.
  
  found_list.Push(&parent_cell);
  int max_age = parent_cell.GetOrganism()->GetPhenotype().GetAge();
  if (parent_ok == false) max_age = -1;
  
  // Now look at all of the neighbors.
  tListIterator<cPopulationCell> conn_it( parent_cell.ConnectionList() );
  
  cPopulationCell * test_cell;
  while ( (test_cell = conn_it.Next()) != NULL) {
    const int cur_age = test_cell->GetOrganism()->GetPhenotype().GetAge();
    if (cur_age > max_age) {
      max_age = cur_age;
      found_list.Clear();
      found_list.Push(test_cell);
    }
    else if (cur_age == max_age) {
      found_list.Push(test_cell);
    }
  }
}

void cPopulation::PositionMerit(cPopulationCell & parent_cell,
                                tList<cPopulationCell> & found_list,
                                bool parent_ok)
{
  // Start with the parent organism as the replacement, and see if we can find
  // anything equivilent or better.
  
  found_list.Push(&parent_cell);
  double max_ratio = parent_cell.GetOrganism()->CalcMeritRatio();
  if (parent_ok == false) max_ratio = -1;
  
  // Now look at all of the neighbors.
  tListIterator<cPopulationCell> conn_it( parent_cell.ConnectionList() );
  
  cPopulationCell * test_cell;
  while ( (test_cell = conn_it.Next()) != NULL) {
    const double cur_ratio = test_cell->GetOrganism()->CalcMeritRatio();
    if (cur_ratio > max_ratio) {
      max_ratio = cur_ratio;
      found_list.Clear();
      found_list.Push(test_cell);
    }
    else if (cur_ratio == max_ratio) {
      found_list.Push(test_cell);
    }
  }
}

void cPopulation::PositionEnergyUsed(cPopulationCell & parent_cell,
                                     tList<cPopulationCell> & found_list,
                                     bool parent_ok)
{
  // Start with the parent organism as the replacement, and see if we can find
  // anything equivilent or better.
  
  found_list.Push(&parent_cell);
  int max_energy_used = parent_cell.GetOrganism()->GetPhenotype().GetTimeUsed();
  if (parent_ok == false) max_energy_used = -1;
  
  // Now look at all of the neighbors.
  tListIterator<cPopulationCell> conn_it( parent_cell.ConnectionList() );
  
  cPopulationCell * test_cell;
  while ( (test_cell = conn_it.Next()) != NULL) {
    const int cur_energy_used = test_cell->GetOrganism()->GetPhenotype().GetTimeUsed();
    if (cur_energy_used > max_energy_used) {
      max_energy_used = cur_energy_used;
      found_list.Clear();
      found_list.Push(test_cell);
    }
    else if (cur_energy_used == max_energy_used) {
      found_list.Push(test_cell);
    }
  }
}

// This function handles PositionOffspring() when there is migration between demes
cPopulationCell& cPopulation::PositionDemeMigration(cPopulationCell& parent_cell, bool parent_ok)
{
  int deme_id = parent_cell.GetDemeID();
  int parent_id = parent_cell.GetDemeID();
  GetDeme(deme_id).AddMigrationOut();
  
  // Position randomly in any other deme
  if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 0) {
    
    //get another -unadjusted- deme id
    int rnd_deme_id = m_world->GetRandom().GetInt(deme_array.GetSize()-1);
    
    //if the -unadjusted- id is above the excluded id, bump it up one
    //insures uniform prob of landing in any deme but the parent's
    if (rnd_deme_id >= deme_id) rnd_deme_id++;
    
    //set the new deme_id
    deme_id = rnd_deme_id;
  }
  
  //Position randomly in an adjacent deme in neighborhood (assuming torus)
  //Extremely hacked DEMES_NUM_X config setting simulates grid
  else if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 1) {
    
    //get a random eight-neighbor
    int dir = m_world->GetRandom().GetInt(8);
    
    // 0 = NW, 1=N, continuing clockwise....
    
    // Up one row
    if (m_world->GetConfig().DEMES_NUM_X.Get() == 0) {
      m_world->GetDriver().Feedback().Error("DEMES_NUM_X must be non-zero if DEMES_MIGRATION_METHOD 1 used.");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
    int x_size = m_world->GetConfig().DEMES_NUM_X.Get();
    int y_size = (int) (m_world->GetConfig().NUM_DEMES.Get() / x_size);
    
    assert(y_size * x_size == m_world->GetConfig().NUM_DEMES.Get());
    
    int x = deme_id % x_size;
    int y = (int) (deme_id / x_size);
    
    if ( (dir == 0) || (dir == 1) || (dir == 2) ) y--;
    if ( (dir == 5) || (dir == 6) || (dir == 7) ) y++;
    if ( (dir == 0) || (dir == 3) || (dir == 5) ) x--;
    if ( (dir == 2) || (dir == 4) || (dir == 7) ) x++;
    
    //handle boundary conditions...
    
    x = (x + x_size) % x_size;
    y = (y + y_size) % y_size;
    
    //set the new deme_id
    deme_id = x + x_size * y;
    
    assert(deme_id > 0);
    assert(deme_id > 0);
  }
  
  //Random deme adjacent in list
  else if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 2) {
    
    //get a random direction to move in deme list
    int rnd_deme_id = m_world->GetRandom().GetInt(1);
    if (rnd_deme_id == 0) rnd_deme_id = -1;
    
    //set the new deme_id
    deme_id = (deme_id + rnd_deme_id + GetNumDemes()) % GetNumDemes();
  }
  
  //Proportional-based on a points system (hjg)
  // The odds of a deme being selected are inversely proportional to the
  // number of points it has.
  else if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 3) {
    
    double total_points = 0;
    int num_demes = GetNumDemes();
    
    // Identify how many points are in the population as a whole.
    for (int did = 0; did < num_demes; did++) {
      if (did != parent_id) {
        total_points +=  (1/(1+GetDeme(did).GetNumberOfPoints()));
      }
    }
    // Select a random number from 0 to 1:
    double rand_point = m_world->GetRandom().GetDouble(0, total_points);
    
    // Iterate through the demes until you find the appropriate
    // deme to insert the organism into.
    double lower_point = 0;
    double upper_point = 0;
    
    for (int curr_deme = 0; curr_deme < num_demes; curr_deme++) {
      if (curr_deme != parent_id){
        upper_point = lower_point + (1+GetDeme(curr_deme).GetNumberOfPoints());
        if ((lower_point <= rand_point) && (rand_point < upper_point)) {
          deme_id = curr_deme;
        }
        lower_point = upper_point;
      }
    }
  }
  
  else if (m_world->GetConfig().DEMES_MIGRATION_METHOD.Get() == 4){
    deme_id = m_world->GetMigrationMatrix().GetProbabilisticDemeID(parent_id,m_world->GetRandom(),false);      
  }
  
  GetDeme(deme_id).AddMigrationIn();
  
  // TODO the above choice of deme does not respect PREFER_EMPTY
  // i.e., it does not preferentially pick a deme with empty cells if they are
  // it might make sense for that to happen...
  
  // Now return an empty cell from the chosen deme
  
  cPopulationCell& mig_cell = PositionDemeRandom(deme_id, parent_cell, parent_ok);
  mig_cell.SetMigrant();
  return mig_cell;
}

// This function handles PositionOffspring() by returning a random cell from the entire deme.
cPopulationCell& cPopulation::PositionDemeRandom(int deme_id, cPopulationCell& parent_cell, bool parent_ok)
{
  assert((deme_id >=0) && (deme_id < deme_array.GetSize()));
  
  const int deme_size = deme_array[deme_id].GetSize();
  cDeme& deme = deme_array[deme_id];
  
  // Look randomly within empty cells first, if requested
  if (m_world->GetConfig().PREFER_EMPTY.Get()) {
    
    int num_empty_cells = UpdateEmptyCellIDArray(deme_id);
    if (num_empty_cells > 0) {
      int out_pos = m_world->GetRandom().GetUInt(num_empty_cells);
      return GetCell(empty_cell_id_array[out_pos]);
    }
  }
  
  int out_pos = m_world->GetRandom().GetUInt(deme_size);
  int out_cell_id = deme.GetCellID(out_pos);
  
  while (parent_ok == false && out_cell_id == parent_cell.GetID()) {
    out_pos = m_world->GetRandom().GetUInt(deme_size);
    out_cell_id = deme.GetCellID(out_pos);
  }
  
  return GetCell(out_cell_id);
}

int cPopulation::FindRandEmptyCell(cAvidaContext& ctx)
{
  int world_size = cell_array.GetSize();
  // full world
  if (num_organisms >= world_size) return -1;

  Apto::Array<int>& cells = GetEmptyCellIDArray();
  int cell_idx = ctx.GetRandom().GetUInt(world_size);
  int cell_id = cells[cell_idx];
  while (GetCell(cell_id).IsOccupied()) {
    // no need to pop this cell off the array, just move it and don't check that far anymore
    cells.Swap(cell_idx, --world_size);
    // if ran out of cells to check (e.g. with birth chamber weirdness)
    if (world_size == 1) return -1;
    cell_idx = ctx.GetRandom().GetUInt(world_size); 
    cell_id = cells[cell_idx];
  }
  return cell_id;
}

// This function updates the list of empty cell ids in the population
// and returns the number of empty cells found. Used by global PREFER_EMPTY
// PositionOffspring() methods with demes (only). 
int cPopulation::UpdateEmptyCellIDArray(int deme_id)
{
  int num_empty_cells = 0;
  // Note: empty_cell_id_array was resized to be large enough to hold
  // all cells in the cPopulation when it was created. Using functions
  // that resize it (like Push) will slow this code down considerably.
  // Instead, we keep track of how much of this memory we are using.
  
  // Look at all cells
  if (deme_id == -1) {
    for (int i=0; i<cell_array.GetSize(); i++) {
      if (GetCell(i).IsOccupied() == false) empty_cell_id_array[num_empty_cells++] = i;
    }
  }
  // Look at a specific deme
  else {
    cDeme& deme = deme_array[deme_id];
    for (int i=0; i<deme.GetSize(); i++) {
      if (GetCell(deme.GetCellID(i)).IsOccupied() == false) empty_cell_id_array[num_empty_cells++] = deme.GetCellID(i);
    }
  }
  return num_empty_cells;
}


int cPopulation::ScheduleOrganism()
{
  return m_scheduler->Next();
}

void cPopulation::ProcessStep(cAvidaContext& ctx, double step_size, int cell_id)
{
  assert(step_size > 0.0);
  assert(cell_id < cell_array.GetSize());
  
  // If cell_id is negative, no cell could be found -- stop here.
  if (cell_id < 0) return;
  
  cPopulationCell& cell = GetCell(cell_id);
  assert(cell.IsOccupied()); // Unoccupied cell getting processor time!
  cOrganism* cur_org = cell.GetOrganism();
  
  cell.GetHardware()->SingleProcess(ctx);
  
  double merit = cur_org->GetPhenotype().GetMerit().GetDouble();
  if (cur_org->GetPhenotype().GetToDelete() == true) {
    cur_org->GetHardware().DeleteMiniTrace(print_mini_trace_reacs);
    delete cur_org;
  }
  
  m_world->GetStats().IncExecuted();
  resource_count.Update(step_size);
  
  // These must be done even if there is only one deme.
  for(int i = 0; i < GetNumDemes(); i++) {
    GetDeme(i).Update(step_size);
  }
  
  cDeme & deme = GetDeme(GetCell(cell_id).GetDemeID());
  deme.IncTimeUsed(merit);
  
  if (GetNumDemes() >= 1) {
    CheckImplicitDemeRepro(deme, ctx); 
  }
}


void cPopulation::ProcessStepSpeculative(cAvidaContext& ctx, double step_size, int cell_id)
{
  assert(step_size > 0.0);
  assert(cell_id < cell_array.GetSize());
  
  // If cell_id is negative, no cell could be found -- stop here.
  if (cell_id < 0) return;
  
  cPopulationCell& cell = GetCell(cell_id);
  assert(cell.IsOccupied()); // Unoccupied cell getting processor time!
  
  cOrganism* cur_org = cell.GetOrganism();
  cHardwareBase* hw = cell.GetHardware();
  
  if (cell.GetSpeculativeState()) {
    // We have already executed this instruction, just decrement the counter
    cell.DecSpeculative();
  } else {
    // Execute the actual instruction
    if (hw->SingleProcess(ctx)) {
      // Speculatively execute additional instructions
      int spec_count = 0;
      while (spec_count < 32) {
        if (hw->SingleProcess(ctx, true)) spec_count++;
        else break;
      }
      cell.SetSpeculativeState(spec_count);
      m_world->GetStats().AddSpeculative(spec_count);
    }
  }
  
  // Deme specific
  if (GetNumDemes() > 1) {
    for(int i = 0; i < GetNumDemes(); i++) GetDeme(i).Update(step_size);
    
    cDeme& deme = GetDeme(GetCell(cell_id).GetDemeID());
    deme.IncTimeUsed(cur_org->GetPhenotype().GetMerit().GetDouble());
    CheckImplicitDemeRepro(deme, ctx); 
  }
  
  if (cur_org->GetPhenotype().GetToDelete() == true) {
    cur_org->GetHardware().DeleteMiniTrace(print_mini_trace_reacs);
    delete cur_org;
    cur_org = NULL;
  }
  
  m_world->GetStats().IncExecuted();
  resource_count.Update(step_size);
}

// Loop through all the demes getting stats and doing calculations
// which must be done on a deme by deme basis.
void cPopulation::UpdateDemeStats(cAvidaContext& ctx) { 
  
  // These must be updated, even if there is only one deme
  for(int i = 0; i < GetNumDemes(); i++) {
    GetDeme(i).UpdateDemeRes(ctx); 
  }
  
  // bail early to save time if there are no demes
  if (GetNumDemes() == 1) return ;
  
  cStats& stats = m_world->GetStats();
  
  stats.SumDemeAge().Clear();
  stats.SumDemeBirthCount().Clear();
  stats.SumDemeOrgCount().Clear();
  stats.SumDemeGeneration().Clear();
  
  stats.SumDemeGestationTime().Clear();
  stats.SumDemeNormalizedTimeUsed().Clear();
  stats.SumDemeMerit().Clear();
  
  stats.SumDemeGenerationsPerLifetime().Clear();
  
  stats.ClearNumOccupiedDemes();
  
  stats.SumDemeEventsKilled().Clear();
  stats.SumDemeAttemptsToKillEvents() .Clear();
  
  for(int i = 0; i < GetNumDemes(); i++) {
    cDeme& deme = GetDeme(i);
    if (deme.IsEmpty()) {  // ignore empty demes
      continue;
    }
    stats.IncNumOccupiedDemes();
    
    stats.SumDemeAge().Add(deme.GetAge());
    stats.SumDemeBirthCount().Add(deme.GetBirthCount());
    stats.SumDemeOrgCount().Add(deme.GetOrgCount());
    stats.SumDemeGeneration().Add(deme.GetGeneration());
    
    stats.SumDemeLastBirthCount().Add(deme.GetLastBirthCount());
    stats.SumDemeLastOrgCount().Add(deme.GetLastOrgCount());
    
    stats.SumDemeGestationTime().Add(deme.GetGestationTime());
    stats.SumDemeNormalizedTimeUsed().Add(deme.GetLastNormalizedTimeUsed());
    stats.SumDemeMerit().Add(deme.GetDemeMerit().GetDouble());
    
    stats.SumDemeGenerationsPerLifetime().Add(deme.GetGenerationsPerLifetime());
    
    stats.SumDemeEventsKilled().Add(deme.GetEventsKilled());
    stats.SumDemeAttemptsToKillEvents().Add(deme.GetEventKillAttempts());
  }
}


void cPopulation::UpdateOrganismStats(cAvidaContext& ctx) 
{
  // Loop through all the cells getting stats and doing calculations
  // which must be done on a creature by creature basis.
  
  cStats& stats = m_world->GetStats();
  
  // Clear out organism sums...
  stats.SumFitness().Clear();
  stats.SumGestation().Clear();
  stats.SumMerit().Clear();
  stats.SumCreatureAge().Clear();
  stats.SumGeneration().Clear();
  stats.SumNeutralMetric().Clear();
  stats.SumLineageLabel().Clear();
  stats.SumCopyMutRate().Clear();
  stats.SumDivMutRate().Clear();
  stats.SumCopySize().Clear();
  stats.SumExeSize().Clear();
  stats.SumMemSize().Clear();
  
  stats.ZeroTasks();
  stats.ZeroReactions();
  
  for (int osp_idx = 0; osp_idx < m_org_stat_providers.GetSize(); osp_idx++) m_org_stat_providers[osp_idx]->UpdateReset();

  // Counts...
  int num_breed_true = 0;
  int num_parasites = 0;
  int num_no_birth = 0;
  int num_multi_thread = 0;
  int num_single_thread = 0;
  int num_threads = 0;
  int num_modified = 0;
  
  // Maximums...
  cMerit max_merit(0);
  double max_fitness = 0;
  int max_gestation_time = 0;
  int max_genome_length = 0;
  
  // Minimums...
  cMerit min_merit(FLT_MAX);
  double min_fitness = FLT_MAX;
  int min_gestation_time = INT_MAX;
  int min_genome_length = INT_MAX;
  
  for (int i = 0; i < live_org_list.GetSize(); i++) {  
    cOrganism* organism = live_org_list[i];
    
    for (int osp_idx = 0; osp_idx < m_org_stat_providers.GetSize(); osp_idx++) {
      m_org_stat_providers[osp_idx]->HandleOrganism(organism);
    }
    
    const cPhenotype& phenotype = organism->GetPhenotype();
    const cMerit cur_merit = phenotype.GetMerit();
    const double cur_fitness = phenotype.GetFitness();
    const int cur_gestation_time = phenotype.GetGestationTime();
    const int cur_genome_length = phenotype.GetGenomeLength();
    
    Apto::Array<Apto::Stat::Accumulator<int> >& from_message_exec_counts = stats.InstFromMessageExeCountsForInstSet((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
    for (int j = 0; j < phenotype.GetLastFromMessageInstCount().GetSize(); j++) {
      from_message_exec_counts[j].Add(organism->GetPhenotype().GetLastFromMessageInstCount()[j]);
    }

    stats.SumFitness().Add(cur_fitness);
    stats.SumMerit().Add(cur_merit.GetDouble());
    stats.SumGestation().Add(phenotype.GetGestationTime());
    stats.SumCreatureAge().Add(phenotype.GetAge());
    stats.SumGeneration().Add(phenotype.GetGeneration());
    stats.SumNeutralMetric().Add(phenotype.GetNeutralMetric());
    stats.SumLineageLabel().Add(organism->GetLineageLabel());
    stats.SumCopyMutRate().Push(organism->MutationRates().GetCopyMutProb());
    stats.SumLogCopyMutRate().Push(log(organism->MutationRates().GetCopyMutProb()));
    stats.SumDivMutRate().Push(organism->MutationRates().GetDivMutProb() / organism->GetPhenotype().GetDivType());
    stats.SumLogDivMutRate().Push(log(organism->MutationRates().GetDivMutProb() / organism->GetPhenotype().GetDivType()));
    stats.SumCopySize().Add(phenotype.GetCopiedSize());
    stats.SumExeSize().Add(phenotype.GetExecutedSize());
    
    if (cur_merit > max_merit) max_merit = cur_merit;
    if (cur_fitness > max_fitness) max_fitness = cur_fitness;
    if (cur_gestation_time > max_gestation_time) max_gestation_time = cur_gestation_time;
    if (cur_genome_length > max_genome_length) max_genome_length = cur_genome_length;
    
    if (cur_merit < min_merit) min_merit = cur_merit;
    if (cur_fitness < min_fitness) min_fitness = cur_fitness;
    if (cur_gestation_time < min_gestation_time) min_gestation_time = cur_gestation_time;
    if (cur_genome_length < min_genome_length) min_genome_length = cur_genome_length;
    
    // Test what tasks this creatures has completed.
    for (int j = 0; j < m_world->GetEnvironment().GetNumTasks(); j++) {
      if (phenotype.GetCurTaskCount()[j] > 0) {
        stats.AddCurTask(j);
        stats.AddCurTaskQuality(j, phenotype.GetCurTaskQuality()[j]);
      }
      
      if (phenotype.GetLastTaskCount()[j] > 0) {
        stats.AddLastTask(j);
        stats.AddLastTaskQuality(j, phenotype.GetLastTaskQuality()[j]);
        stats.IncTaskExeCount(j, phenotype.GetLastTaskCount()[j]);
      }
      
      if (phenotype.GetCurHostTaskCount()[j] > 0) {
        stats.AddCurHostTask(j);
      }
      
      if (phenotype.GetLastHostTaskCount()[j] > 0) {
        stats.AddLastHostTask(j);
      }
      
      if (phenotype.GetCurParasiteTaskCount()[j] > 0) {
        stats.AddCurParasiteTask(j);
      }
      
      if (phenotype.GetLastParasiteTaskCount()[j] > 0) {
        stats.AddLastParasiteTask(j);
      }
      
      if (phenotype.GetCurInternalTaskCount()[j] > 0) {
        stats.AddCurInternalTask(j);
        stats.AddCurInternalTaskQuality(j, phenotype.GetCurInternalTaskQuality()[j]);
      }
      
      if (phenotype.GetLastInternalTaskCount()[j] > 0) {
        stats.AddLastInternalTask(j);
        stats.AddLastInternalTaskQuality(j, phenotype.GetLastInternalTaskQuality()[j]);
      }
    }

    if (stats.ShouldCollectEnvTestStats()) {
      Systematics::GroupPtr genotype = organism->SystematicsGroup("genotype");
      Systematics::GenomeTestMetricsPtr metrics(Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, genotype));
      const Apto::Array<int>& test_task_counts = metrics->GetTaskCounts();
      
      for (int j = 0; j < m_world->GetEnvironment().GetNumTasks(); j++) if (test_task_counts[j] > 0) stats.AddTestTask(j);
    }
    
    
    // Record what add bonuses this organism garnered for different reactions
    for (int j = 0; j < m_world->GetEnvironment().GetNumReactions(); j++) {
      if (phenotype.GetCurReactionCount()[j] > 0) {
        stats.AddCurReaction(j);
        stats.AddCurReactionAddReward(j, phenotype.GetCurReactionAddReward()[j]);
      }
      
      if (phenotype.GetLastReactionCount()[j] > 0) {
        stats.AddLastReaction(j);
        stats.IncReactionExeCount(j, phenotype.GetLastReactionCount()[j]);
        stats.AddLastReactionAddReward(j, phenotype.GetLastReactionAddReward()[j]);
      }
    }
    
    // Test what resource combinations this creature has sensed
    for (int j = 0; j < stats.GetSenseSize(); j++) {
      if (phenotype.GetLastSenseCount()[j] > 0) {
        stats.AddLastSense(j);
        stats.IncLastSenseExeCount(j, phenotype.GetLastSenseCount()[j]);
      }
    }
    
    // Increment the counts for all qualities the organism has...
    num_parasites += organism->GetNumParasites();
    if (phenotype.ParentTrue()) num_breed_true++;
    if (phenotype.GetNumDivides() == 0) num_no_birth++;
    if (phenotype.IsMultiThread()) num_multi_thread++;
    else num_single_thread++;
    
    if (phenotype.IsModified()) num_modified++;
    
    cHardwareBase& hardware = organism->GetHardware();
    stats.SumMemSize().Add(hardware.GetMemory().GetSize());
    num_threads += hardware.GetNumThreads();
    
    // Increment the age of this organism.
    organism->GetPhenotype().IncAge();
  }
  
  stats.SetBreedTrueCreatures(num_breed_true);
  stats.SetNumNoBirthCreatures(num_no_birth);
  stats.SetNumParasites(num_parasites);
  stats.SetNumSingleThreadCreatures(num_single_thread);
  stats.SetNumMultiThreadCreatures(num_multi_thread);
  stats.SetNumThreads(num_threads);
  stats.SetNumModified(num_modified);
  
  stats.SetMaxMerit(max_merit.GetDouble());
  stats.SetMaxFitness(max_fitness);
  stats.SetMaxGestationTime(max_gestation_time);
  stats.SetMaxGenomeLength(max_genome_length);
  
  stats.SetMinMerit(min_merit.GetDouble());
  stats.SetMinFitness(min_fitness);
  stats.SetMinGestationTime(min_gestation_time);
  stats.SetMinGenomeLength(min_genome_length);
  
  resource_count.UpdateGlobalResources(ctx);   
}

void cPopulation::UpdateFTOrgStats(cAvidaContext&) 
{
  // Get per-org stats seperately for pred and prey
  cStats& stats = m_world->GetStats();
  
  // Clear out organism sums...
  stats.SumPreyFitness().Clear();
  stats.SumPreyGestation().Clear();
  stats.SumPreyMerit().Clear();
  stats.SumPreyCreatureAge().Clear();
  stats.SumPreyGeneration().Clear();
  
  stats.SumPredFitness().Clear();
  stats.SumPredGestation().Clear();
  stats.SumPredMerit().Clear();
  stats.SumPredCreatureAge().Clear();
  stats.SumPredGeneration().Clear();
  
  stats.SumTopPredFitness().Clear();
  stats.SumTopPredGestation().Clear();
  stats.SumTopPredMerit().Clear();
  stats.SumTopPredCreatureAge().Clear();
  stats.SumTopPredGeneration().Clear();
  
  stats.SumAttacks().Clear();
  stats.SumKills().Clear();

  //  stats.ZeroFTReactions();   ****
  
  stats.ZeroFTInst();
  stats.ZeroGroupAttackInst();
  
  for (int i = 0; i < live_org_list.GetSize(); i++) {
    cOrganism* organism = live_org_list[i];
    const cPhenotype& phenotype = organism->GetPhenotype();
    const cMerit cur_merit = phenotype.GetMerit();
    const double cur_fitness = phenotype.GetFitness();
    
    if (organism->IsPreyFT()) {
      stats.SumPreyFitness().Add(cur_fitness);
      stats.SumPreyGestation().Add(phenotype.GetGestationTime());
      stats.SumPreyMerit().Add(cur_merit.GetDouble());
      stats.SumPreyCreatureAge().Add(phenotype.GetAge());
      stats.SumPreyGeneration().Add(phenotype.GetGeneration());
      
      Apto::Array<Apto::Stat::Accumulator<int> >& prey_inst_exe_counts = stats.InstPreyExeCountsForInstSet((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int j = 0; j < phenotype.GetLastInstCount().GetSize(); j++) {
        prey_inst_exe_counts[j].Add(organism->GetPhenotype().GetLastInstCount()[j]);
      }
      Apto::Array<Apto::Stat::Accumulator<int> >& prey_from_sensor_exec_counts = stats.InstPreyFromSensorExeCountsForInstSet((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int j = 0; j < phenotype.GetLastFromSensorInstCount().GetSize(); j++) {
        prey_from_sensor_exec_counts[j].Add(organism->GetPhenotype().GetLastFromSensorInstCount()[j]);
      }
    }
    else if (organism->IsPredFT()) {
      stats.SumPredFitness().Add(cur_fitness);
      stats.SumPredGestation().Add(phenotype.GetGestationTime());
      stats.SumPredMerit().Add(cur_merit.GetDouble());
      stats.SumPredCreatureAge().Add(phenotype.GetAge());
      stats.SumPredGeneration().Add(phenotype.GetGeneration());
      stats.SumAttacks().Add(phenotype.GetLastAttacks());
      stats.SumKills().Add(phenotype.GetLastKills());

      Apto::Array<Apto::Stat::Accumulator<int> >& pred_inst_exe_counts = stats.InstPredExeCountsForInstSet((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int j = 0; j < phenotype.GetLastInstCount().GetSize(); j++) {
        pred_inst_exe_counts[j].Add(organism->GetPhenotype().GetLastInstCount()[j]);
      }

      Apto::Array<Apto::Stat::Accumulator<int> >& pred_from_sensor_exec_counts = stats.InstPredFromSensorExeCountsForInstSet((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int j = 0; j < phenotype.GetLastFromSensorInstCount().GetSize(); j++) {
        pred_from_sensor_exec_counts[j].Add(organism->GetPhenotype().GetLastFromSensorInstCount()[j]);
      }

      Apto::Array<cString> att_inst = m_world->GetStats().GetGroupAttackInsts((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int k = 0; k < att_inst.GetSize(); k++) {
        Apto::Array<Apto::Stat::Accumulator<int> >& group_attack_inst_exe_counts = stats.ExecCountsForGroupAttackInst((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue(), att_inst[k]);
        for (int j = 0; j < phenotype.GetLastGroupAttackInstCount()[k].GetSize(); j++) {
          group_attack_inst_exe_counts[j].Add(organism->GetPhenotype().GetLastGroupAttackInstCount()[k][j]);
        }
      }
    }
    else {
      stats.SumTopPredFitness().Add(cur_fitness);
      stats.SumTopPredGestation().Add(phenotype.GetGestationTime());
      stats.SumTopPredMerit().Add(cur_merit.GetDouble());
      stats.SumTopPredCreatureAge().Add(phenotype.GetAge());
      stats.SumTopPredGeneration().Add(phenotype.GetGeneration());
      stats.SumAttacks().Add(phenotype.GetLastAttacks());
      stats.SumKills().Add(phenotype.GetLastKills());
     
      Apto::Array<Apto::Stat::Accumulator<int> >& tpred_inst_exe_counts = stats.InstTopPredExeCountsForInstSet((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int j = 0; j < phenotype.GetLastInstCount().GetSize(); j++) {
        tpred_inst_exe_counts[j].Add(organism->GetPhenotype().GetLastInstCount()[j]);
      }
      Apto::Array<Apto::Stat::Accumulator<int> >& tpred_from_sensor_exec_counts = stats.InstTopPredFromSensorExeCountsForInstSet((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int j = 0; j < phenotype.GetLastFromSensorInstCount().GetSize(); j++) {
        tpred_from_sensor_exec_counts[j].Add(organism->GetPhenotype().GetLastFromSensorInstCount()[j]);
      }
      Apto::Array<cString> att_inst = m_world->GetStats().GetGroupAttackInsts((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int k = 0; k < att_inst.GetSize(); k++) {
        Apto::Array<Apto::Stat::Accumulator<int> >& group_attack_inst_exe_counts = stats.ExecCountsForGroupAttackInst((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue(), att_inst[k]);
        for (int j = 0; j < phenotype.GetLastTopPredGroupAttackInstCount()[k].GetSize(); j++) {
          group_attack_inst_exe_counts[j].Add(organism->GetPhenotype().GetLastTopPredGroupAttackInstCount()[k][j]);
        }
      }
    }
    
    // Record what add bonuses this organism garnered for different reactions
    /*    for (int j = 0; j < m_world->GetEnvironment().GetNumReactions(); j++) {
     if (phenotype.GetCurReactionCount()[j] > 0) {
     stats.AddCurReaction(j);
     stats.AddCurReactionAddReward(j, phenotype.GetCurReactionAddReward()[j]);
     }
     
     if (phenotype.GetLastReactionCount()[j] > 0) {
     stats.AddLastReaction(j);
     stats.IncReactionExeCount(j, phenotype.GetLastReactionCount()[j]);
     stats.AddLastReactionAddReward(j, phenotype.GetLastReactionAddReward()[j]);
     }
     }*/
    
  }
}

void cPopulation::UpdateMaleFemaleOrgStats(cAvidaContext& ctx)
{
  // Get per-org stats seperately for males and females
  cStats& stats = m_world->GetStats();
  
  // Clear out organism sums...
  stats.SumMaleFitness().Clear();
  stats.SumMaleGestation().Clear();
  stats.SumMaleMerit().Clear();
  stats.SumMaleCreatureAge().Clear();
  stats.SumMaleGeneration().Clear();
  
  stats.SumFemaleFitness().Clear();
  stats.SumFemaleGestation().Clear();
  stats.SumFemaleMerit().Clear();
  stats.SumFemaleCreatureAge().Clear();
  stats.SumFemaleGeneration().Clear();
  
  stats.ZeroMTInst();
  
  for (int i = 0; i < live_org_list.GetSize(); i++) {  
    cOrganism* organism = live_org_list[i];
    const cPhenotype& phenotype = organism->GetPhenotype();
    const cMerit cur_merit = phenotype.GetMerit();
    const double cur_fitness = phenotype.GetFitness();
    
    if(organism->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) {
      stats.SumMaleFitness().Add(cur_fitness);
      stats.SumMaleGestation().Add(phenotype.GetGestationTime());
      stats.SumMaleMerit().Add(cur_merit.GetDouble());
      stats.SumMaleCreatureAge().Add(phenotype.GetAge());
      stats.SumMaleGeneration().Add(phenotype.GetGeneration());
      
      Apto::Array<Apto::Stat::Accumulator<int> >& male_inst_exe_counts = stats.InstMaleExeCountsForInstSet((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int j = 0; j < phenotype.GetLastInstCount().GetSize(); j++) {
        male_inst_exe_counts[j].Add(organism->GetPhenotype().GetLastInstCount()[j]);
      }
    }
    else if (organism->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) {
      stats.SumFemaleFitness().Add(cur_fitness);
      stats.SumFemaleGestation().Add(phenotype.GetGestationTime());
      stats.SumFemaleMerit().Add(cur_merit.GetDouble());
      stats.SumFemaleCreatureAge().Add(phenotype.GetAge());
      stats.SumFemaleGeneration().Add(phenotype.GetGeneration());
      
      Apto::Array<Apto::Stat::Accumulator<int> >& female_inst_exe_counts = stats.InstFemaleExeCountsForInstSet((const char*)organism->GetGenome().Properties().Get(s_prop_id_instset).StringValue());
      for (int j = 0; j < phenotype.GetLastInstCount().GetSize(); j++) {
        female_inst_exe_counts[j].Add(organism->GetPhenotype().GetLastInstCount()[j]);
      }
    }
  }
}

void cPopulation::UpdateResStats(cAvidaContext& ctx) 
{
  cStats& stats = m_world->GetStats();
  stats.SetResources(resource_count.GetResources(ctx)); 
  stats.SetSpatialRes(resource_count.GetSpatialRes(ctx)); 
  stats.SetResourcesGeometry(resource_count.GetResourcesGeometry()); 
}

void cPopulation::ProcessPreUpdate()
{
  resource_count.SetSpatialUpdate(m_world->GetStats().GetUpdate());
  for (int i = 0; i < deme_array.GetSize(); i++) deme_array[i].ProcessPreUpdate();   
}

void cPopulation::ProcessPostUpdate(cAvidaContext& ctx)
{
  ProcessUpdateCellActions(ctx);
  
  cStats& stats = m_world->GetStats();
  
  stats.SetNumCreatures(GetNumOrganisms());
  
  UpdateDemeStats(ctx); 
  UpdateOrganismStats(ctx);
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
    UpdateFTOrgStats(ctx);
  }
  if (m_world->GetConfig().MATING_TYPES.Get()) {
    UpdateMaleFemaleOrgStats(ctx);
  }
  
  for (int i = 0; i < deme_array.GetSize(); i++) deme_array[i].ProcessUpdate(ctx);   
}

void cPopulation::ProcessUpdateCellActions(cAvidaContext& ctx)
{
  for (int i = 0; i < cell_array.GetSize(); i++) {
    if (cell_array[i].MutationRates().TestDeath(ctx)) KillOrganism(cell_array[i], ctx); 
  }
}


struct sOrgInfo {
  int cell_id;
  int offset;
  int lineage_label;
  int curr_group;
  int curr_forage;
  int birth_cell;
  int avatar_cell;
  int av_bcell;
  // rebirth data
  int parent_ft;
  int parent_is_teacher;
  double parent_merit;
  
  sOrgInfo() { ; }
  sOrgInfo(int c, int o, int l, int in_group, int in_forage, int in_bcell, int in_avcell, int in_av_bcell, int in_parent_ft, 
          int in_parent_is_teacher, double in_parent_merit) : 
          cell_id(c), offset(o), lineage_label(l), curr_group(in_group), curr_forage(in_forage), birth_cell(in_bcell), 
          avatar_cell(in_avcell), av_bcell(in_av_bcell), parent_ft(in_parent_ft), parent_is_teacher(in_parent_is_teacher),
          parent_merit(in_parent_merit) { ; }
};

struct sGroupInfo {
  Systematics::GroupPtr bg;
  Apto::Array<sOrgInfo> orgs;
  bool parasite;
  
  sGroupInfo(Systematics::GroupPtr in_bg, bool is_para = false) : bg(in_bg), parasite(is_para) { ; }
};

bool cPopulation::SavePopulation(const cString& filename, bool save_historic, bool save_groupings, bool save_avatars, bool save_rebirth)
{
  Apto::String file_path((const char*)filename);
  Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), file_path);
  df->SetFileType("genotype_data");
  df->WriteComment("Structured Population Save");
  df->WriteTimeStamp();
  
  // Build up hash table of all current genotypes and the cells in which the organisms reside
  Apto::Map<int, sGroupInfo*> genotype_map;
  
  for (int cell = 0; cell < cell_array.GetSize(); cell++) {
    if (cell_array[cell].IsOccupied()) {
      cOrganism* org = cell_array[cell].GetOrganism();
      
      // Handle any parasites
      const Apto::Array<Systematics::UnitPtr>& parasites = org->GetParasites();
      for (int p = 0; p < parasites.GetSize(); p++) {
        Systematics::GroupPtr pg = parasites[p]->SystematicsGroup("genotype");
        if (pg == NULL) continue;
        
        sGroupInfo* map_entry = NULL;
        if (genotype_map.Get(pg->ID(), map_entry)) {
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, -1, 0, -1, -1, -1, 0, 1));
        } else {
          map_entry = new sGroupInfo(pg, true);
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, -1, 0, -1, -1, -1, 0, 1));
          genotype_map.Set(pg->ID(), map_entry);
        }
      }
      
      
      // Handle the organism itself
      Systematics::GroupPtr genotype = org->SystematicsGroup("genotype");
      if (genotype == NULL) continue;
      
      int offset = org->GetPhenotype().GetCPUCyclesUsed();
      sGroupInfo* map_entry = NULL;
      if (genotype_map.Get(genotype->ID(), map_entry)) {
        int curr_group = -1;
        if (org->HasOpinion()) curr_group = org->GetOpinion().first;
        const int curr_forage = org->GetForageTarget();
        const int birth_cell = org->GetPhenotype().GetBirthCell();
        int avatar_cell = -1;
        int av_bcell = -1;
        if (m_world->GetConfig().USE_AVATARS.Get()) {
          avatar_cell = org->GetOrgInterface().GetAVCellID();
          av_bcell = org->GetPhenotype().GetAVBirthCell();
        }
        if (!save_rebirth) {
          if (!save_groupings && !save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), -1, -1, 0, -1, -1, -1, 0, 1));
          }
          else if (save_groupings && !save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), curr_group, curr_forage, birth_cell, -1, -1, -1, 0, 1));
          }
          else if (save_groupings && save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), curr_group, curr_forage, birth_cell, avatar_cell, av_bcell, -1, 0, 1));          
          }
          else if (!save_groupings && save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), -1, -1, 0, avatar_cell, av_bcell, -1, 0, 1));                    
          }
        }
        else if (save_rebirth) {
          const int p_ft = org->GetParentFT();
          const int p_teach = (bool) (org->HadParentTeacher());
          const double p_merit = org->GetParentMerit();
          
          map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), curr_group, curr_forage, birth_cell, avatar_cell, av_bcell, p_ft, p_teach, p_merit));
        }
      } else {
        map_entry = new sGroupInfo(genotype);
        int curr_group = -1;
        if (org->HasOpinion()) curr_group = org->GetOpinion().first;
        const int curr_forage = org->GetForageTarget();
        const int birth_cell = org->GetPhenotype().GetBirthCell();
        const int avatar_cell = org->GetOrgInterface().GetAVCellID();
        const int av_bcell = org->GetPhenotype().GetAVBirthCell();
        if (!save_rebirth) {
          if (!save_groupings && !save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), -1, -1, 0, -1, -1, -1, 0, 1));
          }
          else if (save_groupings && !save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), curr_group, curr_forage, birth_cell, -1, -1, -1, 0, 1));
          }
          else if (save_groupings && save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), curr_group, curr_forage, birth_cell, avatar_cell, av_bcell, -1, 0, 1));          
          }
          else if (!save_groupings && save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), -1, -1, 0, avatar_cell, av_bcell, -1, 0, 1));                    
          }
        }
        else if (save_rebirth) {
          const int p_ft = org->GetParentFT();
          const int p_teach = (bool) (org->HadParentTeacher());
          const double p_merit = org->GetParentMerit();
          map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), curr_group, curr_forage, birth_cell, avatar_cell, av_bcell, p_ft, p_teach, p_merit));                  
        }
        genotype_map.Set(genotype->ID(), map_entry);
      }
    }
  }
  
  // Output all current genotypes
  for (Apto::Map<int, sGroupInfo*>::ValueIterator it = genotype_map.Values(); it.Next();) {
    sGroupInfo* group_info = *it.Get();
    Systematics::GroupPtr genotype = group_info->bg;
    
    genotype->LegacySave(Apto::GetInternalPtr(df));
    
    Apto::Array<sOrgInfo>& cells = group_info->orgs;
    cString cellstr;
    cString offsetstr;
    cString lineagestr;
    cString groupstr;
    cString foragestr;
    cString birthstr;
    cString avatarstr;
    cString avatarbstr;
    
    cString pforagestr;
    cString pteachstr;
    cString pmeritstr;
    
    cellstr.Set("%d", cells[0].cell_id);
    offsetstr.Set("%d", cells[0].offset);
    lineagestr.Set("%d", cells[0].lineage_label);
    groupstr.Set("%d", cells[0].curr_group);
    foragestr.Set("%d", cells[0].curr_forage);
    birthstr.Set("%d", cells[0].birth_cell);
    avatarstr.Set("%d", cells[0].avatar_cell);
    avatarbstr.Set("%d", cells[0].av_bcell);
    
    pforagestr.Set("%d", cells[0].parent_ft);
    pteachstr.Set("%d", cells[0].parent_is_teacher);
    pmeritstr.Set("%f", cells[0].parent_merit);
    
    for (int cell_i = 1; cell_i < cells.GetSize(); cell_i++) {
      cellstr += cStringUtil::Stringf(",%d", cells[cell_i].cell_id);
      offsetstr += cStringUtil::Stringf(",%d", cells[cell_i].offset);
      lineagestr += cStringUtil::Stringf(",%d", cells[cell_i].lineage_label);
      if (!save_rebirth) {
        if (save_groupings) {
          groupstr += cStringUtil::Stringf(",%d", cells[cell_i].curr_group);
          foragestr += cStringUtil::Stringf(",%d", cells[cell_i].curr_forage);
          birthstr += cStringUtil::Stringf(",%d", cells[cell_i].birth_cell);
        }
        if (save_avatars) {
          avatarstr += cStringUtil::Stringf(",%d",cells[cell_i].avatar_cell);
          avatarbstr += cStringUtil::Stringf(",%d",cells[cell_i].av_bcell);
        }
      }
      else if (save_rebirth) {
        groupstr += cStringUtil::Stringf(",%d", cells[cell_i].curr_group);
        foragestr += cStringUtil::Stringf(",%d", cells[cell_i].curr_forage);
        birthstr += cStringUtil::Stringf(",%d", cells[cell_i].birth_cell);
        avatarstr += cStringUtil::Stringf(",%d",cells[cell_i].avatar_cell);
        avatarbstr += cStringUtil::Stringf(",%d",cells[cell_i].av_bcell);
        
        pforagestr += cStringUtil::Stringf(",%d",cells[cell_i].parent_ft);
        pteachstr += cStringUtil::Stringf(",%d",cells[cell_i].parent_is_teacher);
        pmeritstr += cStringUtil::Stringf(",%f",cells[cell_i].parent_merit);
      }
    }

    df->Write(cellstr, "Occupied Cell IDs", "cells");
    if (group_info->parasite) df->Write("", "Gestation (CPU) Cycle Offsets", "gest_offset");
    else df->Write(offsetstr, "Gestation (CPU) Cycle Offsets", "gest_offset");
    df->Write(lineagestr, "Lineage Label", "lineage");
      if (!save_rebirth) {
        if (save_groupings) {
          df->Write(groupstr, "Current Group IDs", "group_id");
          df->Write(foragestr, "Current Forager Types", "forager_type");
          df->Write(birthstr, "Birth Cells", "birth_cell");
        }
        if (save_avatars) {
          df->Write(avatarstr, "Current Avatar Cell Locations", "avatar_cell");
          df->Write(avatarbstr, "Avatar Birth Cell", "av_bcell");
        }
      }
      else if (save_rebirth) {
        df->Write(groupstr, "Current Group IDs", "group_id");
        df->Write(foragestr, "Current Forager Types", "forager_type");
        df->Write(birthstr, "Birth Cells", "birth_cell");
        df->Write(avatarstr, "Current Avatar Cell Locations", "avatar_cell");
        df->Write(avatarbstr, "Avatar Birth Cell", "av_bcell");
        df->Write(pforagestr, "Parent forager type", "parent_ft");
        df->Write(pteachstr, "Was Parent a Teacher", "parent_is_teach");
        df->Write(pmeritstr, "Parent Merit", "parent_merit");
      }
    df->Endl();
    
    delete group_info;
  }
  
  // Output historic genotypes
  if (save_historic) {
    Systematics::Manager::Of(m_world->GetNewWorld())->ArbiterForRole("genotype")->LegacySave(Apto::GetInternalPtr(df));
  }
  
  return true;
}


bool cPopulation::SaveStructuredSystematicsGroup(const Systematics::RoleID& role, const cString& filename)
{
  Apto::String file_path((const char*)filename);
  Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), file_path);
  df->SetFileType("systematics_data");
  df->WriteComment("Structured Systematics Group Save");
  df->WriteTimeStamp();
  
  // Build up hash table of all current genotypes and the cells in which the organisms reside
  Apto::Map<int, sGroupInfo*> group_map;
  
  for (int cell = 0; cell < cell_array.GetSize(); cell++) {
    if (cell_array[cell].IsOccupied()) {
      cOrganism* org = cell_array[cell].GetOrganism();
      
      // Handle any parasites
      const Apto::Array<Systematics::UnitPtr>& parasites = org->GetParasites();
      for (int p = 0; p < parasites.GetSize(); p++) {
        Systematics::GroupPtr pg = parasites[p]->SystematicsGroup(role);
        if (pg == NULL) continue;
        
        sGroupInfo* map_entry = NULL;
        if (group_map.Get(pg->ID(), map_entry)) {
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, -1, 0, -1, -1, -1, 0, 1));
        } else {
          map_entry = new sGroupInfo(pg, true);
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, -1, 0, -1, -1, -1, 0, 1));
          group_map.Set(pg->ID(), map_entry);
        }
      }
      
      
      // Handle the organism itself
      Systematics::GroupPtr group = org->SystematicsGroup(role);
      if (!group) continue;
      
      int offset = org->GetPhenotype().GetCPUCyclesUsed();
      sGroupInfo* map_entry = NULL;
      if (group_map.Get(group->ID(), map_entry)) {
        map_entry->orgs.Push(sOrgInfo(cell, offset, 0, -1, -1, 0, -1, -1, -1, 0, 1));
      } else {
        map_entry = new sGroupInfo(group);
        map_entry->orgs.Push(sOrgInfo(cell, offset, 0, -1, -1, 0, -1, -1, -1, 0, 1));
        group_map.Set(group->ID(), map_entry);
      }
    }
  }
  
  // Output all current genotypes
  for (Apto::Map<int, sGroupInfo*>::ValueIterator it = group_map.Values(); it.Next();) {
    sGroupInfo* group_info = *it.Get();
    Systematics::GroupPtr group = group_info->bg;
    
    group->LegacySave(Apto::GetInternalPtr(df));
    
    Apto::Array<sOrgInfo>& cells = group_info->orgs;
    cString cellstr;
    cellstr.Set("%d", cells[0].cell_id);
    for (int cell_i = 1; cell_i < cells.GetSize(); cell_i++) cellstr += cStringUtil::Stringf(",%d", cells[cell_i].cell_id);
    df->Write(cellstr, "Occupied Cell IDs", "cells");
    df->Endl();
    
    delete group_info;
  }
  
  return true;
}

bool cPopulation::LoadStructuredSystematicsGroup(cAvidaContext& ctx, const Systematics::RoleID& role, const cString& filename)
{
  cInitFile input_file(filename, m_world->GetWorkingDir(), ctx.Driver().Feedback());
  if (!input_file.WasOpened()) return false;
  
  
  Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
  Systematics::ArbiterPtr arbiter = classmgr->ArbiterForRole(role);

  for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
    cString cur_line = input_file.GetLine(line_id);
    
    // Setup the group for this line...
    Apto::SmartPtr<Apto::Map<Apto::String, Apto::String> > props = input_file.GetLineAsDict(line_id);
    Systematics::GroupPtr grp = arbiter->LegacyLoad(&props);
    
    // Process resident cell ids
    cString cellstr(props->Get("cells"));
    if (cellstr.GetSize()) {
      while (cellstr.GetSize()) {
        int cell_id = cellstr.Pop(',').AsInt();
        if (cell_array[cell_id].IsOccupied()) {
          Systematics::UnitPtr unit(cell_array[cell_id].GetOrganism());
          cell_array[cell_id].GetOrganism()->AddReference(); // creating new smart pointer to org, explicitly add reference
          unit->AddClassification(grp->ClassifyNewUnit(unit, Systematics::ConstGroupMembershipPtr(NULL)));
        }
      }
    }
  }
  
  return true;
}

bool cPopulation::SaveFlameData(const cString& filename)
{
  Apto::String file_path((const char*)filename);
  Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), file_path);
  df->WriteComment("Flame Data Save");
  df->WriteTimeStamp();
  
  // Build up hash table of all current genotypes
  Apto::Map<int, sGroupInfo*> genotype_map;
  
  for (int cell = 0; cell < cell_array.GetSize(); cell++) {
    if (cell_array[cell].IsOccupied()) {
      cOrganism* org = cell_array[cell].GetOrganism();
      
      // Handle any parasites
      const Apto::Array<Systematics::UnitPtr>& parasites = org->GetParasites();
      for (int p = 0; p < parasites.GetSize(); p++) {
        Systematics::GroupPtr pg = parasites[p]->SystematicsGroup("genotype");
        if (pg == NULL) continue;
        
        sGroupInfo* map_entry = NULL;
        if (genotype_map.Get(pg->ID(), map_entry)) {
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, -1, 0, -1, -1, -1, 0, 1));
        } else {
          map_entry = new sGroupInfo(pg, true);
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, -1, 0, -1, -1, -1, 0, 1));
          genotype_map.Set(pg->ID(), map_entry);
        }
      }
      
      
      // Handle the organism itself
      Systematics::GroupPtr genotype = org->SystematicsGroup("genotype");
      if (genotype == NULL) continue;
      
      int offset = org->GetPhenotype().GetCPUCyclesUsed();
      sGroupInfo* map_entry = NULL;
      if (genotype_map.Get(genotype->ID(), map_entry)) {
        map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), -1, -1, 0, -1, -1, -1, 0, 1));
      } else {
        map_entry = new sGroupInfo(genotype);
        map_entry->orgs.Push(sOrgInfo(cell, offset, org->GetLineageLabel(), -1, -1, 0, -1, -1, -1, 0, 1));
        genotype_map.Set(genotype->ID(), map_entry);
      }
    }
  }
  
  // Output all current genotypes
  for (Apto::Map<int, sGroupInfo*>::ValueIterator it = genotype_map.Values(); it.Next();) {
    sGroupInfo* group_info = *it.Get();
    Systematics::GroupPtr genotype = group_info->bg;
    
    df->Write(genotype->ID(), "ID", "genotype_id");
    df->Write(genotype->NumUnits(), "Number of currently living organisms", "num_units");
    df->Write(genotype->Depth(), "Phylogenetic Depth", "depth");
    df->Endl();    
    delete group_info;
  }  

  return true;
}

struct sTmpGenotype
{
public:
  int id_num;
  Apto::SmartPtr<Apto::Map<Apto::String, Apto::String> > props;
  
  int num_cpus;
  Apto::Array<int> cells;
  Apto::Array<int> offsets;
  Apto::Array<int> lineage_labels;
  Apto::Array<int> group_ids;
  Apto::Array<int> forager_types;
  Apto::Array<int> birth_cells;
  Apto::Array<int> avatar_cells;
  Apto::Array<double> parent_merit;
  Apto::Array<bool> parent_teacher;
  Apto::Array<int> parent_ft;
  
  Systematics::GroupPtr bg;
  
  
  inline sTmpGenotype() : id_num(-1), props(NULL) { ; }
  inline bool operator<(const sTmpGenotype& rhs) const { return id_num > rhs.id_num; }
  inline bool operator>(const sTmpGenotype& rhs) const { return id_num < rhs.id_num; }
  inline bool operator<=(const sTmpGenotype& rhs) const { return id_num >= rhs.id_num; }
  inline bool operator>=(const sTmpGenotype& rhs) const { return id_num <= rhs.id_num; }
};

bool cPopulation::LoadGenotypeList(const cString& filename, cAvidaContext& ctx, Apto::Array<GeneticRepresentationPtr>& list_obj)
{
  cInitFile input_file(filename, m_world->GetWorkingDir(), ctx.Driver().Feedback());
  if (!input_file.WasOpened()) return false;

  list_obj.ResizeClear(input_file.GetNumLines());
  
  Apto::Array<sTmpGenotype, Apto::ManagedPointer> genotypes(input_file.GetNumLines());
  
  for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
    cString m_sequence = input_file.GetLine(line_id);
    
    const cInstSet& is = m_world->GetHardwareManager().GetDefaultInstSet();
    HashPropertyMap props;
    cHardwareManager::SetupPropertyMap(props, (const char*)is.GetInstSetName());
    
    list_obj[line_id] = GeneticRepresentationPtr(new InstructionSequence((const char*)m_sequence));
  }

  return true;
}

bool cPopulation::LoadParasiteGenotypeList(const cString& filename, cAvidaContext& ctx)
{
  return LoadGenotypeList(filename, ctx, parasite_genotype_list);
}


bool cPopulation::LoadHostGenotypeList(const cString& filename, cAvidaContext& ctx)
{
  return LoadGenotypeList(filename, ctx, host_genotype_list);
}


bool cPopulation::LoadPopulation(const cString& filename, cAvidaContext& ctx, int cellid_offset, int lineage_offset, bool load_groups, bool load_birth_cells, bool load_avatars, bool load_rebirth, bool load_parent_dat, int traceq)
{
  // @TODO - build in support for verifying population dimensions
  
  cInitFile input_file(filename, m_world->GetWorkingDir(), ctx.Driver().Feedback());
  if (!input_file.WasOpened()) return false;
  
  // Clear out the population, unless an offset is being used
  if (cellid_offset == 0) {
    for (int i = 0; i < cell_array.GetSize(); i++) KillOrganism(cell_array[i], ctx); 
  }
  
  // First, we read in all the genotypes and store them in an array
  Apto::Array<sTmpGenotype, Apto::ManagedPointer> genotypes(input_file.GetNumLines());
  
  bool structured = false;
  for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
    cString cur_line = input_file.GetLine(line_id);
    
    // Setup the genotype for this line...
    sTmpGenotype& tmp = genotypes[line_id];
    tmp.props = input_file.GetLineAsDict(line_id);
    tmp.id_num = Apto::StrAs(tmp.props->Get("id"));

    // Loads "num_units" preferrentially, but will fall back to "num_cpus" if present
    assert(tmp.props->Has("num_cpus") || tmp.props->Has("num_units"));
    tmp.num_cpus = (tmp.props->Has("num_units")) ? Apto::StrAs(tmp.props->Get("num_units")) : Apto::StrAs(tmp.props->Get("num_cpus"));
    
    // Process resident cell ids
    cString cellstr(tmp.props->Get("cells"));
    if (structured || cellstr.GetSize()) {
      structured = true;
      while (cellstr.GetSize()) tmp.cells.Push(cellstr.Pop(',').AsInt());
      assert(tmp.cells.GetSize() == tmp.num_cpus);
    }
    
    // Process gestation time offsets
    if (!load_rebirth) {
      cString offsetstr(tmp.props->Get("gest_offset"));
      if (offsetstr.GetSize()) {
        while (offsetstr.GetSize()) tmp.offsets.Push(offsetstr.Pop(',').AsInt());
        assert(tmp.offsets.GetSize() == tmp.num_cpus);
      }
    }
    // Lineage label (only set if given in file)
    cString lineagestr(tmp.props->Get("lineage"));
    while (lineagestr.GetSize()) tmp.lineage_labels.Push(lineagestr.Pop(',').AsInt());
    // @blw preserve compatability with older .spop files that don't have lineage labels
    assert(tmp.lineage_labels.GetSize() == 0 || tmp.lineage_labels.GetSize() == tmp.num_cpus);
    
    // Other org specs (if given in file)
    if (load_rebirth) {
      if (tmp.props->Has("birth_cell")) {
        cString birthstr(tmp.props->Get("birth_cell"));
        while (birthstr.GetSize()) tmp.birth_cells.Push(birthstr.Pop(',').AsInt());
        assert(tmp.birth_cells.GetSize() == 0 || tmp.birth_cells.GetSize() == tmp.num_cpus);      
      }
      if (tmp.props->Has("av_bcell") && m_world->GetConfig().USE_AVATARS.Get()) {
        cString avatarstr(tmp.props->Get("av_bcell"));
        while (avatarstr.GetSize()) tmp.avatar_cells.Push(avatarstr.Pop(',').AsInt());
        assert(tmp.avatar_cells.GetSize() == 0 || tmp.avatar_cells.GetSize() == tmp.num_cpus);
      }
      if (tmp.props->Has("parent_is_teach")) {
        cString teachstr(tmp.props->Get("parent_is_teach"));
        while (teachstr.GetSize()) tmp.parent_teacher.Push((bool)(teachstr.Pop(',').AsInt()));
        assert(tmp.parent_teacher.GetSize() == 0 || tmp.parent_teacher.GetSize() == tmp.num_cpus);
      }
      if (tmp.props->Has("parent_ft")) {
        cString parentftstr(tmp.props->Get("parent_ft"));
        while (parentftstr.GetSize()) tmp.parent_ft.Push(parentftstr.Pop(',').AsInt());
        assert(tmp.parent_ft.GetSize() == 0 || tmp.parent_ft.GetSize() == tmp.num_cpus);
      }
      if (tmp.props->Has("parent_merit")) {
        cString meritstr(tmp.props->Get("parent_merit"));
        while (meritstr.GetSize()) tmp.parent_merit.Push(meritstr.Pop(',').AsDouble());
        assert(tmp.parent_merit.GetSize() == 0 || tmp.parent_merit.GetSize() == tmp.num_cpus);
      }
    }
    else {
      if (load_groups) {
        if (tmp.props->Has("group_id")) {
          cString groupstr(tmp.props->Get("group_id"));
          while (groupstr.GetSize()) tmp.group_ids.Push(groupstr.Pop(',').AsInt());
          assert(tmp.group_ids.GetSize() == 0 || tmp.group_ids.GetSize() == tmp.num_cpus);
        }
        if (tmp.props->Has("forager_type")) {
          cString foragestr(tmp.props->Get("forager_type"));
          while (foragestr.GetSize()) tmp.forager_types.Push(foragestr.Pop(',').AsInt());
          assert(tmp.forager_types.GetSize() == 0 || tmp.forager_types.GetSize() == tmp.num_cpus);
        }
      }
      if (load_birth_cells) {   
        if (tmp.props->Has("birth_cell")) {
          cString birthstr(tmp.props->Get("birth_cell"));
          while (birthstr.GetSize()) tmp.birth_cells.Push(birthstr.Pop(',').AsInt());
          assert(tmp.birth_cells.GetSize() == 0 || tmp.birth_cells.GetSize() == tmp.num_cpus);
        }
        if (tmp.props->Has("av_bcell") && m_world->GetConfig().USE_AVATARS.Get()) {
          cString avatarstr(tmp.props->Get("av_bcell"));
          while (avatarstr.GetSize()) tmp.avatar_cells.Push(avatarstr.Pop(',').AsInt());
          assert(tmp.avatar_cells.GetSize() == 0 || tmp.avatar_cells.GetSize() == tmp.num_cpus);
        }
      }
      else if (!load_birth_cells && load_avatars && tmp.props->Has("avatar_cell")) {
        cString avatarstr(tmp.props->Get("avatar_cell"));
        while (avatarstr.GetSize()) tmp.avatar_cells.Push(avatarstr.Pop(',').AsInt());
        assert(tmp.avatar_cells.GetSize() == 0 || tmp.avatar_cells.GetSize() == tmp.num_cpus);
      }
    if (load_parent_dat) {
      if (tmp.props->Has("parent_is_teach")) {
        cString teachstr(tmp.props->Get("parent_is_teach"));
        while (teachstr.GetSize()) tmp.parent_teacher.Push((bool)(teachstr.Pop(',').AsInt()));
        assert(tmp.parent_teacher.GetSize() == 0 || tmp.parent_teacher.GetSize() == tmp.num_cpus);
      }
      if (tmp.props->Has("parent_ft")) {
        cString parentftstr(tmp.props->Get("parent_ft"));
        while (parentftstr.GetSize()) tmp.parent_ft.Push(parentftstr.Pop(',').AsInt());
        assert(tmp.parent_ft.GetSize() == 0 || tmp.parent_ft.GetSize() == tmp.num_cpus);
      }
      if (tmp.props->Has("parent_merit")) {
        cString meritstr(tmp.props->Get("parent_merit"));
        while (meritstr.GetSize()) tmp.parent_merit.Push(meritstr.Pop(',').AsDouble());
        assert(tmp.parent_merit.GetSize() == 0 || tmp.parent_merit.GetSize() == tmp.num_cpus);      
      }
    }
    }
    if (m_world->GetConfig().USE_AVATARS.Get() && !tmp.avatar_cells.GetSize()) {
      cString avatarstr(tmp.props->Get("avatar_cell"));
      while (avatarstr.GetSize()) tmp.avatar_cells.Push(avatarstr.Pop(',').AsInt());
      assert(tmp.avatar_cells.GetSize() == 0 || tmp.avatar_cells.GetSize() == tmp.num_cpus);
    }
  }
  
  // Sort genotypes in descending order according to their id_num
  Apto::QSort(genotypes);
  
  Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
  Systematics::ArbiterPtr bgm = classmgr->ArbiterForRole("genotype");
  
  bool some_missing = false;
  for (int i = genotypes.GetSize() - 1; i >= 0; i--) {
    // Fix Parent IDs
    cString nparentstr;
    int pcount = 0;
    cString lparentstr = (const char*)genotypes[i].props->Get("parents");
    if (lparentstr == "(none)") lparentstr = "";
    cStringList opidlist(lparentstr, ',');
    while (opidlist.GetSize()) {
      int opid = opidlist.Pop().AsInt();
      int npid = -1;
      for (int j = i; j < genotypes.GetSize(); j++) {
        if (genotypes[j].id_num == opid) {
          npid = genotypes[j].bg->ID();
          break;
        }
      }
      // only for pop saves that include historic (i.e. parent id found):
      if (npid != -1) {
        if (pcount) nparentstr += ",";
        nparentstr += cStringUtil::Convert(npid);
        pcount++;
      }
    }
    if (!nparentstr.GetSize() && !some_missing) some_missing = true;
    genotypes[i].props->Set("parents", (const char*)nparentstr);
    
    genotypes[i].bg = bgm->LegacyLoad(&genotypes[i].props);
  }  
//  if (some_missing) m_world->GetDriver().Feedback().Warning("Some parents not found in loaded pop file. Defaulting to parent ID of '(none)' for those genomes.");
  
  // Process genotypes, inject into organisms as necessary
  int u_cell_id = 0;
  for (int gen_i = 0; gen_i < genotypes.GetSize(); gen_i++) {
    sTmpGenotype& tmp = genotypes[gen_i];
    // otherwise, we insert as many organisms as we need
    for (int cell_i = 0; cell_i < tmp.num_cpus; cell_i++) {
      int cell_id = 0;
      if (!load_birth_cells && !load_rebirth) cell_id = (structured) ? (tmp.cells[cell_i] + cellid_offset) : (u_cell_id++ + cellid_offset);
      else cell_id = (structured) ? (tmp.birth_cells[cell_i] + cellid_offset) : (u_cell_id++ + cellid_offset);
      
      // Set up lineage, including lineage label (0 if not loaded)
      int lineage_label = 0;
      if (tmp.lineage_labels.GetSize() != 0) {
        lineage_label = tmp.lineage_labels[cell_i] + lineage_offset;
      }
      
      assert(tmp.bg->Properties().Has("genome"));
      Genome mg(tmp.bg->Properties().Get("genome"));
      cOrganism* new_organism = new cOrganism(m_world, ctx, mg, -1, Systematics::Source(Systematics::DIVISION, (const char*)filename, true));
      
      // Setup the phenotype...
      cPhenotype& phenotype = new_organism->GetPhenotype();
      InstructionSequencePtr seq;
      seq.DynamicCastFrom(mg.Representation());
      
      phenotype.SetupInject(*seq);
      
      // Classify this new organism
      Systematics::RoleClassificationHints hints;
      hints["genotype"]["id"] = Apto::FormatStr("%d", tmp.bg->ID());
      Systematics::UnitPtr unit(new_organism);
      new_organism->AddReference(); // creating new smart pointer to org, explicitly add reference      
      classmgr->ClassifyNewUnit(unit, &hints);
      
      // Coalescense Clade Setup
      new_organism->SetCCladeLabel(-1);
      
      if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
        phenotype.SetMerit(cMerit(phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy())));
      } else {
        // Set the phenotype merit from the save file
        assert(tmp.props->Has("merit"));
        double merit = Apto::StrAs(tmp.props->Get("merit"));
        if ((load_rebirth || load_parent_dat) && m_world->GetConfig().INHERIT_MERIT.Get() && tmp.props->Has("parent_merit")) {
          merit = tmp.parent_merit[cell_i]; 
        }
        
        if (merit > 0) {
          phenotype.SetMerit(cMerit(merit));
        } else {
          phenotype.SetMerit(cMerit(new_organism->GetTestMerit(ctx)));
        }
        
        if (tmp.offsets.GetSize() > cell_i && !load_rebirth) {
          // Adjust initial merit to account for organism execution at the time the population was saved
          // - this factors the merit by the fraction of the gestation time remaining
          // - this will be approximate, since gestation time may vary for each organism, but it should work for many cases
          double gest_time = Apto::StrAs(tmp.props->Get("gest_time"));
          double gest_remain = gest_time - (double)tmp.offsets[cell_i];
          if (gest_remain > 0.0 && gest_time > 0.0) {
            double new_merit = phenotype.GetMerit().GetDouble() * (gest_time / gest_remain);
            phenotype.SetMerit(cMerit(new_merit));
          }
        }
      }
      
      new_organism->SetLineageLabel(lineage_label);
      
      // Prep the cell..
      if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST &&
          cell_array[cell_id].IsOccupied() == true) {
        // Have to manually take this cell out of the reaper Queue.
        reaper_queue.Remove( &(cell_array[cell_id]) );
      }
      
      // Setup the child's mutation rates.  Since this organism is being injected
      // and has no parent, we should always take the rate from the environment.
      new_organism->MutationRates().Copy(cell_array[cell_id].MutationRates());
      
      // Activate the organism in the population...
      bool org_survived = false;
      if (!load_rebirth) {
        if (load_groups) {
          // Set up group id and forager type (if loaded)
          int group_id = -1;
          int forager_type = -1;
          if (tmp.group_ids.GetSize() != 0) group_id = tmp.group_ids[cell_i];
          if (tmp.forager_types.GetSize() != 0) forager_type = tmp.forager_types[cell_i]; 
          new_organism->SetOpinion(group_id);
          JoinGroup(new_organism, group_id);
          new_organism->GetPhenotype().SetBirthCellID(cell_id);
          new_organism->GetPhenotype().SetBirthGroupID(group_id);
          new_organism->GetPhenotype().SetBirthForagerType(forager_type);
          new_organism->SetParentGroup(group_id);
          new_organism->SetParentFT(forager_type);
          if (tmp.parent_merit.GetSize()) new_organism->SetParentMerit(tmp.parent_merit[cell_i]);
          org_survived = ActivateOrganism(ctx, new_organism, cell_array[cell_id], false, true);
          if (org_survived) new_organism->SetForageTarget(ctx, forager_type, true);
        }
        else org_survived = ActivateOrganism(ctx, new_organism, cell_array[cell_id], true, true);
        
        if (load_parent_dat) {
          new_organism->SetParentFT(tmp.parent_ft[cell_i]);
          new_organism->SetParentTeacher(tmp.parent_teacher[cell_i]);
          if (tmp.props->Has("parent_merit")) new_organism->SetParentMerit(tmp.parent_merit[cell_i]);        
        }
      }
      else if (load_rebirth) {
        new_organism->SetParentFT(tmp.parent_ft[cell_i]);
        new_organism->SetParentTeacher(tmp.parent_teacher[cell_i]);
        if (tmp.parent_merit.GetSize()) new_organism->SetParentMerit(tmp.parent_merit[cell_i]);
        
        new_organism->GetPhenotype().SetBirthCellID(cell_id);
        org_survived = ActivateOrganism(ctx, new_organism, cell_array[cell_id], false, true);
      }
      
      if (org_survived && m_world->GetConfig().USE_AVATARS.Get() && !m_world->GetConfig().NEURAL_NETWORKING.Get()) { //**
        int avatar_cell = -1;
        if (tmp.avatar_cells.GetSize() != 0) avatar_cell = tmp.avatar_cells[cell_i];
        if (avatar_cell != -1) {
          new_organism->GetOrgInterface().AddPredPreyAV(ctx, avatar_cell);
          new_organism->GetPhenotype().SetAVBirthCellID(tmp.avatar_cells[cell_i]);
        }
      }
      if (org_survived) {
        new_organism->GetOrgInterface().TryWriteBirthLocData(new_organism->GetOrgIndex());
        if (traceq) {
          print_mini_trace_genomes = (traceq == 2);
          SetupMiniTrace(new_organism);
        }
      }
    }
  }
  sync_events = true;  
  return true;
}

/**
 * This function loads a genome from a given file, and initializes
 * a cpu with it.
 *
 * @param filename The name of the file to load.
 * @param in_cpu The grid-position into which the genome should be loaded.
 * @param merit An initial merit value.
 * @param lineage_label A value that allows to track the daughters of
 * this organism.
 **/

void cPopulation::Inject(const Genome& genome, Systematics::Source src, cAvidaContext& ctx, int cell_id, double merit, int lineage_label, double neutral, bool inject_group, int group_id, int forager_type, int trace) 
{
  // If an invalid cell was given, choose a new ID for it.
  if (cell_id < 0) {
    switch (m_world->GetConfig().BIRTH_METHOD.Get()) {
      case POSITION_OFFSPRING_FULL_SOUP_ELDEST:
        cell_id = reaper_queue.PopRear()->GetID();
      default:
        cell_id = 0;
    }
  }
  
  // We can't inject into the boundary of the world:
  if(m_world->IsWorldBoundary(GetCell(cell_id))) {
    cell_id += world_x + 1;
  }
  // Can't inject onto deadly world edges either
  if (m_world->GetConfig().DEADLY_BOUNDARIES.Get() == 1) {
    const int dest_x = cell_id % m_world->GetConfig().WORLD_X.Get();  
    if (dest_x == 0) cell_id += 1;
    else if (dest_x == m_world->GetConfig().WORLD_X.Get() - 1) cell_id -= 1;
    const int dest_y = cell_id / m_world->GetConfig().WORLD_X.Get();
    if (dest_y == 0) cell_id += m_world->GetConfig().WORLD_X.Get();
    else if (dest_y == m_world->GetConfig().WORLD_Y.Get() - 1) cell_id -= m_world->GetConfig().WORLD_X.Get();
  }
  
  // if the injected org already has a group we will assign it to, do not assign group id in activate organism
  if (!inject_group) InjectGenome(cell_id, src, genome, ctx, lineage_label, true);
  else InjectGenome(cell_id, src, genome, ctx, lineage_label, false);
  
  cPhenotype& phenotype = GetCell(cell_id).GetOrganism()->GetPhenotype();
  phenotype.SetNeutralMetric(neutral);
  
  if (merit > 0) phenotype.SetMerit(cMerit(merit));
  AdjustSchedule(GetCell(cell_id), phenotype.GetMerit());
  
  cell_array[cell_id].GetOrganism()->SetLineageLabel(lineage_label);
  
  // the following bit of code is required for proper germline support.
  // even if there's only one deme!!
  if (m_world->GetConfig().DEMES_USE_GERMLINE.Get()) {
    cDeme& deme = deme_array[GetCell(cell_id).GetDemeID()];

    // If we're using germlines, then we have to be a little careful here.
    // This should probably not be within Inject() since we mainly want it to
    // apply to the START_ORGANISM? -- @JEB

    //@JEB This section is very messy to maintain consistency with other deme ways.

    if (m_world->GetConfig().DEMES_SEED_METHOD.Get() == 0) {
      if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 1) {
        if (deme.GetGermline().Size()==0) {
          deme.GetGermline().Add(GetCell(cell_id).GetOrganism()->GetGenome());
        }
      }
    } else if (m_world->GetConfig().DEMES_SEED_METHOD.Get() == 1) {
      if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 2) {
        //find the genotype we just created from the genome, and save it
        deme.ReplaceGermline(GetCell(cell_id).GetOrganism()->SystematicsGroup("genotype"));
      }
      else { // not germlines, save org as founder
        deme.AddFounder(GetCell(cell_id).GetOrganism()->SystematicsGroup("genotype"), &phenotype);
      }

      GetCell(cell_id).GetOrganism()->GetPhenotype().SetPermanentGermlinePropensity
        (m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get());


      if (m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() >= 0.0) {
        GetCell(cell_id).GetOrganism()->GetPhenotype().SetPermanentGermlinePropensity
          ( m_world->GetConfig().DEMES_FOUNDER_GERMLINE_PROPENSITY.Get() );
      }

    }
  } else if (m_world->GetConfig().DEMES_USE_GERMLINE.Get() == 2) {
    //find the genotype we just created from the genome, and save it
    cDeme& deme = deme_array[GetCell(cell_id).GetDemeID()];
    Systematics::UnitPtr unit(new cDemePlaceholderUnit(src, genome));
    Systematics::GroupPtr genotype = Systematics::Manager::Of(m_world->GetNewWorld())->ArbiterForRole("genotype")->ClassifyNewUnit(unit);
    deme.ReplaceGermline(genotype);
    genotype->RemoveUnit();
  }
  
  if (inject_group) {
    cell_array[cell_id].GetOrganism()->SetOpinion(group_id);
    cell_array[cell_id].GetOrganism()->JoinGroup(group_id);
    cell_array[cell_id].GetOrganism()->SetForageTarget(ctx, forager_type);
    
    cell_array[cell_id].GetOrganism()->GetPhenotype().SetBirthCellID(cell_id);
    cell_array[cell_id].GetOrganism()->GetOrgInterface().TryWriteBirthLocData(cell_array[cell_id].GetOrganism()->GetOrgIndex());
    cell_array[cell_id].GetOrganism()->GetPhenotype().SetBirthGroupID(group_id);
    cell_array[cell_id].GetOrganism()->GetPhenotype().SetBirthForagerType(forager_type);
  }
  if (m_world->GetConfig().USE_AVATARS.Get() && !m_world->GetConfig().NEURAL_NETWORKING.Get()) {
    cell_array[cell_id].GetOrganism()->GetOrgInterface().AddPredPreyAV(ctx, cell_id);
  }
  if (trace) SetupMiniTrace(cell_array[cell_id].GetOrganism());    
}

void cPopulation::InjectGroup(const Genome& genome, Systematics::Source src, cAvidaContext& ctx, int cell_id, double merit, int lineage_label, double neutral, int group_id, int forager_type, int trace) 
{
  Inject(genome, src, ctx, cell_id, merit, lineage_label, neutral, true, group_id, forager_type, trace);
}

void cPopulation::InjectParasite(const cString& label, const InstructionSequence& injected_code, int cell_id)
{
  cOrganism* target_organism = cell_array[cell_id].GetOrganism();
  // target_organism-> target_organism->GetHardware().GetCurThread()
  if (target_organism == NULL) return;
  
  GeneticRepresentationPtr rep(new InstructionSequence(injected_code));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)target_organism->GetHardware().GetInstSet().GetInstSetName());
  Genome mg(target_organism->GetHardware().GetType(), props, rep);
  Apto::SmartPtr<cParasite, Apto::InternalRCObject> parasite(new cParasite(m_world, mg, 0, Systematics::Source(Systematics::HORIZONTAL, (const char*)label)));
  
  //default to configured parasite virulence
  parasite->SetVirulence(m_world->GetConfig().PARASITE_VIRULENCE.Get());
  
  if (target_organism->ParasiteInfectHost(parasite)) {
    Systematics::Manager::Of(m_world->GetNewWorld())->ClassifyNewUnit(parasite);
  }
}


void cPopulation::UpdateResources(cAvidaContext& ctx, const Apto::Array<double> & res_change)
{
  resource_count.Modify(ctx, res_change);
}

void cPopulation::UpdateResource(cAvidaContext& ctx, int res_index, double change)
{
  resource_count.Modify(ctx, res_index, change);
}

void cPopulation::UpdateCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id)
{
  resource_count.ModifyCell(ctx, res_change, cell_id);
}

void cPopulation::UpdateDemeCellResources(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int cell_id)
{
  GetDeme(GetCell(cell_id).GetDemeID()).ModifyDemeResCount(ctx, res_change, cell_id);
}

void cPopulation::SetResource(cAvidaContext& ctx, int res_index, double new_level)
{
  resource_count.Set(ctx, res_index, new_level);
}

/* This version of SetResource takes the name of the resource.
 * If a resource by this name does not exist, it does nothing.
 * Otherwise, it sets the resource to the new level, 
 * calling the index version of SetResource().
 */
void cPopulation::SetResource(cAvidaContext& ctx, const cString res_name, double new_level)
{
  cResource* res = environment.GetResourceLib().GetResource(res_name);
  if (res != NULL) SetResource(ctx, res->GetIndex(), new_level);
}

/* This method sets the inflow of the named resource.
 * It changes this value in the environment, then updates it in the
 * actual population's resource count.
 */
void cPopulation::SetResourceInflow(const cString res_name, double new_level)
{
  environment.SetResourceInflow(res_name, new_level);
  resource_count.SetInflow(res_name, new_level);
}

/* This method sets the outflow of the named resource.
 * It changes this value in the enviroment, then updates the
 * decay rate in the resource count (to 1 - the given outflow, as 
 * outflow is different than decay).
 */
void cPopulation::SetResourceOutflow(const cString res_name, double new_level)
{
  environment.SetResourceOutflow(res_name, new_level);
  resource_count.SetDecay(res_name, 1 - new_level);
}

/* This method sets a deme resource to the same level across
 * all demes.  If a resource by the given name does not exist,
 * it does nothing.
 */
void cPopulation::SetDemeResource(cAvidaContext& ctx, const cString res_name, double new_level)
{
  cResource* res = environment.GetResourceLib().GetResource(res_name);
  if (res != NULL) {
    int num_demes = GetNumDemes();
    for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
      cDeme& deme = GetDeme(deme_id);
      deme.SetResource(ctx, res->GetIndex(), new_level);
    }
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
 * cPopulation::SetDemeResourceInflow() -- blw
 */
void cPopulation::SetSingleDemeResourceInflow(int deme_id, const cString res_name, double new_level)
{
  environment.SetResourceInflow(res_name, new_level);
  GetDeme(deme_id).GetDemeResources().SetInflow(res_name, new_level);
}

/* This method sets the inflow for the named deme resource across ALL demes. 
 * It changes the value in the environment, then updates it in the deme resource
 * counts.
 *
 * This maintains the connection between the enviroment value and the resource
 * count values, unlike cPopulation::SetSingleDemeResourceInflow()
 */
void cPopulation::SetDemeResourceInflow(const cString res_name, double new_level)
{
  environment.SetResourceInflow(res_name, new_level);
  int num_demes = GetNumDemes();
  for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    GetDeme(deme_id).GetDemeResources().SetInflow(res_name, new_level);
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
 * cPopulation::SetDemeResourceOutflow() -- blw
 */
void cPopulation::SetSingleDemeResourceOutflow(int deme_id, const cString res_name, double new_level)
{
  environment.SetResourceOutflow(res_name, new_level);
  GetDeme(deme_id).GetDemeResources().SetDecay(res_name, 1 - new_level);
}

/* This method sets the outflow for the named deme resource across ALL demes. 
 * It changes the value in the environment, then updates the decay rate in the 
 * deme resource counts.
 *
 * This maintains the connection between the enviroment value and the resource
 * count values, unlike cPopulation::SetSingleDemeResourceOutflow()
 */
void cPopulation::SetDemeResourceOutflow(const cString res_name, double new_level)
{
  environment.SetResourceOutflow(res_name, new_level);
  int num_demes = GetNumDemes();
  for (int deme_id = 0; deme_id < num_demes; ++deme_id) {
    GetDeme(deme_id).GetDemeResources().SetDecay(res_name, 1 - new_level);
  }
}

void cPopulation::ResetInputs(cAvidaContext& ctx)
{
  for (int i=0; i<GetSize(); i++) {
    cPopulationCell& cell = GetCell(i);
    cell.ResetInputs(ctx);
    if (cell.IsOccupied()) {
      cell.GetOrganism()->ResetInput();
    }
  }
  //@JJB**
  for (int i = 0; i < GetNumDemes(); i++) {
    GetDeme(i).ResetInputs(ctx);
    GetDeme(i).ResetInput();
  }
}

void cPopulation::BuildTimeSlicer()
{
  switch (m_world->GetConfig().SLICING_METHOD.Get()) {
    case SLICE_CONSTANT:
      m_scheduler = new Apto::Scheduler::RoundRobin(cell_array.GetSize());
      break;
//    case SLICE_DEME_PROB_MERIT:
//      schedule = new cDemeProbSchedule(cell_array.GetSize(), ctx.GetRandom().GetInt(0x7FFFFFFF), deme_array.GetSize());
//      break;
//    case SLICE_PROB_DEMESIZE_PROB_MERIT:
//      schedule = new cProbDemeProbSchedule(cell_array.GetSize(), ctx.GetRandom().GetInt(0x7FFFFFFF), deme_array.GetSize());
//      break;
    case SLICE_INTEGRATED_MERIT:
      m_scheduler = new Apto::Scheduler::Integrated(cell_array.GetSize());
      break;
    case SLICE_PROB_MERIT:
    {
      Apto::SmartPtr<Apto::Random> rng(new Apto::RNG::AvidaRNG(m_world->GetRandom().GetInt(0x7FFFFFFF)));
      m_scheduler = new Apto::Scheduler::Probabilistic(cell_array.GetSize(), rng);
    }
      break;
    case SLICE_PROB_INTEGRATED_MERIT:
    {
      Apto::SmartPtr<Apto::Random> rng(new Apto::RNG::AvidaRNG(m_world->GetRandom().GetInt(m_world->GetRandom().MaxSeed())));
      m_scheduler = new Apto::Scheduler::ProbabilisticIntegrated(cell_array.GetSize(), rng);
    }
      break;
    default:
      cout << "error: requested time slicer not found." << endl;
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
      break;
  }
}


void cPopulation::FindEmptyCell(tList<cPopulationCell> & cell_list,
                                tList<cPopulationCell> & found_list)
{
  tListIterator<cPopulationCell> cell_it(cell_list);
  cPopulationCell * test_cell;
  
  while ( (test_cell = cell_it.Next()) != NULL) {
    // If this cell is empty, add it to the list...
    if (test_cell->IsOccupied() == false) found_list.Push(test_cell);
  }
}


// This function injects a new organism into the population at cell_id that
// is an exact clone of the organism passed in.

void cPopulation::InjectClone(int cell_id, cOrganism& orig_org, Systematics::Source src)
{
  assert(cell_id >= 0 && cell_id < cell_array.GetSize());
  
  cAvidaContext& ctx = m_world->GetDefaultContext();
  
  cOrganism* new_organism = new cOrganism(m_world, ctx, orig_org.GetGenome(), orig_org.GetPhenotype().GetGeneration(), src);
  Systematics::UnitPtr unit(new_organism);
  new_organism->AddReference(); // creating new smart pointer to new_organism, explicitly add reference
  
  // Classify the new organism
  Systematics::Manager::Of(m_world->GetNewWorld())->ClassifyNewUnit(unit);
  
  // Setup the phenotype...
  new_organism->GetPhenotype().SetupClone(orig_org.GetPhenotype());
  
  // Prep the cell..
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST &&
      cell_array[cell_id].IsOccupied() == true) {
    // Have to manually take this cell out of the reaper Queue.
    reaper_queue.Remove( &(cell_array[cell_id]) );
  }
  
  // Setup the mutation rate based on the population cell...
  const int mut_source = m_world->GetConfig().MUT_RATE_SOURCE.Get();
  if (mut_source == 1) {
    // Update the mutation rates of each child from the environment....
    new_organism->MutationRates().Copy(cell_array[cell_id].MutationRates());
  } else {
    // Update the mutation rates of each child from its parent.
    new_organism->MutationRates().Copy(orig_org.MutationRates());
  }
  
  // since this is a clone, we want some of the parent data from the genome source (the source is not the parent, the source's parent is)
  if (m_world->GetConfig().USE_FORM_GROUPS.Get()) new_organism->SetParentGroup(orig_org.GetParentGroup());
  if (orig_org.HadParentTeacher()) new_organism->SetParentTeacher(true);
  new_organism->SetParentFT(orig_org.GetParentFT());
  
  // Activate the organism in the population...
  bool org_survived = ActivateOrganism(ctx, new_organism, cell_array[cell_id], true, true);
  // only assign an avatar cell if the org lived through birth
  if (m_world->GetConfig().USE_AVATARS.Get() && org_survived) {
    int avatar_target_cell = PlaceAvatar(ctx, &orig_org);
    if (avatar_target_cell != -1) {
      new_organism->GetPhenotype().SetAVBirthCellID(avatar_target_cell);
      new_organism->GetOrgInterface().TryWriteBirthLocData(new_organism->GetOrgIndex());
      new_organism->GetOrgInterface().AddPredPreyAV(ctx, avatar_target_cell);
      if (m_world->GetConfig().AVATAR_BIRTH_FACING.Get() == 1) {
        const int rots = ctx.GetRandom().GetUInt(0,8);
        for (int j = 0; j < rots; j++) new_organism->Rotate(ctx, rots);
      }
    }
    else KillOrganism(GetCell(cell_id), ctx);
  }
}

// This function injects the offspring genome of an organism into the population at cell_id.
// Takes care of divide mutations.
void cPopulation::CompeteOrganisms_ConstructOffspring(int cell_id, cOrganism& parent)
{
  assert(cell_id >= 0 && cell_id < cell_array.GetSize());
  
  cAvidaContext& ctx = m_world->GetDefaultContext();
  
  // Do mutations on the child genome, but restore it to its current state afterward.
  Genome save_child = parent.OffspringGenome();
  parent.GetHardware().Divide_DoMutations(ctx);
  Genome child_genome = parent.OffspringGenome();
  parent.GetHardware().Divide_TestFitnessMeasures(ctx);
  parent.OffspringGenome() = save_child;
  cOrganism* new_organism = new cOrganism(m_world, ctx, child_genome, parent.GetPhenotype().GetGeneration(), Systematics::Source(Systematics::DUPLICATION, ""));
  
  // Classify the offspring
  Systematics::ConstParentGroupsPtr pgrps(new Systematics::ConstParentGroups(1));
  (*pgrps)[0] = parent.SystematicsGroupMembership();
  new_organism->SelfClassify(pgrps);
  
  // Setup the phenotype...
  InstructionSequencePtr seq;
  seq.DynamicCastFrom(child_genome.Representation());
  new_organism->GetPhenotype().SetupOffspring(parent.GetPhenotype(),*seq);
  
  // Prep the cell..
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST &&
      cell_array[cell_id].IsOccupied() == true) {
    // Have to manually take this cell out of the reaper Queue.
    reaper_queue.Remove( &(cell_array[cell_id]) );
  }
  
  // Setup the mutation rate based on the population cell...
  const int mut_source = m_world->GetConfig().MUT_RATE_SOURCE.Get();
  if (mut_source == 1) {
    // Update the mutation rates of each child from the environment....
    new_organism->MutationRates().Copy(cell_array[cell_id].MutationRates());
  } else {
    // Update the mutation rates of each child from its parent.
    new_organism->MutationRates().Copy(parent.MutationRates());
  }
  
  // Activate the organism in the population...
  ActivateOrganism(ctx, new_organism, cell_array[cell_id], true, true);
}


void cPopulation::InjectGenome(int cell_id, Systematics::Source src, const Genome& genome, cAvidaContext& ctx, int lineage_label, bool assign_group, Systematics::RoleClassificationHints* hints)
{
  assert(cell_id >= 0 && cell_id < cell_array.GetSize());
  if (cell_id < 0 || cell_id >= cell_array.GetSize()) {
    ctx.Driver().Feedback().Error("InjectGenotype into nonexistent cell");
    ctx.Driver().Abort(Avida::INTERNAL_ERROR);
  }
  
  
  cOrganism* new_organism = new cOrganism(m_world, ctx, genome, -1, src);
  
  // Setup the phenotype...
  cPhenotype& phenotype = new_organism->GetPhenotype();
  
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(genome.Representation());
  phenotype.SetupInject(*seq);
  
  // Classify this new organism
  Systematics::UnitPtr unit(new_organism);
  new_organism->AddReference(); // creating new smart pointer to new_organism, explicitly add reference
  Systematics::Manager::Of(m_world->GetNewWorld())->ClassifyNewUnit(unit, hints);
  
  //Coalescense Clade Setup
  new_organism->SetCCladeLabel(-1);
  
  Systematics::GenomeTestMetricsPtr metrics = Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, new_organism->SystematicsGroup("genotype"));
  
  if (m_world->GetConfig().ENERGY_ENABLED.Get() == 1) {
    phenotype.SetMerit(cMerit(phenotype.ConvertEnergyToMerit(phenotype.GetStoredEnergy())));
  } else {
    phenotype.SetMerit(cMerit(metrics->GetMerit()));
  }
  
  phenotype.SetLinesCopied(metrics->GetLinesCopied());
  phenotype.SetLinesExecuted(metrics->GetLinesExecuted());
  phenotype.SetGestationTime(metrics->GetGestationTime());
  
  
  // Prep the cell..
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST &&
      cell_array[cell_id].IsOccupied() == true) {
    // Have to manually take this cell out of the reaper Queue.
    reaper_queue.Remove( &(cell_array[cell_id]) );
  }
  
  // Setup the child's mutation rates.  Since this organism is being injected
  // and has no parent, we should always take the rate from the environment.
  new_organism->MutationRates().Copy(cell_array[cell_id].MutationRates());
  new_organism->SetLineageLabel(lineage_label);
  
  // Activate the organism in the population...
  if (assign_group) ActivateOrganism(ctx, new_organism, cell_array[cell_id], true, true);
  else ActivateOrganism(ctx, new_organism, cell_array[cell_id], false, true);
  
  // Log the injection of this organism if LOG_INJECT is set to 1 and
  // the current update number is >= INJECT_LOG_START
  if ( (m_world->GetConfig().LOG_INJECT.Get() == 1) &&
      (m_world->GetStats().GetUpdate() >= m_world->GetConfig().INJECT_LOG_START.Get()) ){
    
    cString tmpfilename = cStringUtil::Stringf("injectlog.dat");
    Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)tmpfilename);
    
    df->Write(m_world->GetStats().GetUpdate(), "Update");
    df->Write(new_organism->GetID(), "Organism ID");
    df->Write(m_world->GetPopulation().GetCell(cell_id).GetDemeID(), "Deme ID");
    df->Write(new_organism->GetFacing(), "Facing");
    df->Endl();
  }
}

// Note: cPopulation::SerialTransfer does not respect deme boundaries and only acts on a single population.
void cPopulation::SerialTransfer(int transfer_size, bool ignore_deads, cAvidaContext& ctx) 
{
  assert(transfer_size > 0);
  
  // If we are ignoring all dead organisms, remove them from the population.
  if (ignore_deads == true) {
    for (int i = 0; i < GetSize(); i++) {
      cPopulationCell & cell = cell_array[i];
      if (cell.IsOccupied() && cell.GetOrganism()->GetTestFitness(m_world->GetDefaultContext()) == 0.0) {
        KillOrganism(cell, ctx);
      }
    }
  }
  
  // If removing the dead was enough, stop here.
  if (num_organisms <= transfer_size) return;
  
  // Collect a vector of the occupied cells...
  vector<int> transfer_pool;
  transfer_pool.reserve(num_organisms);
  for (int i = 0; i < GetSize(); i++) {
    if (cell_array[i].IsOccupied()) transfer_pool.push_back(i);
  }
  
  // Remove the proper number of cells.
  const int removal_size = num_organisms - transfer_size;
  for (int i = 0; i < removal_size; i++) {
    int j = (int) ctx.GetRandom().GetUInt(transfer_pool.size());
    KillOrganism(cell_array[transfer_pool[j]], ctx); 
    transfer_pool[j] = transfer_pool.back();
    transfer_pool.pop_back();
  }
}

void cPopulation::RemovePredators(cAvidaContext& ctx)
{
  for (int i = 0; i < live_org_list.GetSize(); i++) {
    if (!live_org_list[i]->IsPreyFT()) live_org_list[i]->Die(ctx);
  }
}

void cPopulation::InjectPreyClone(cAvidaContext& ctx, cOrganism* org_to_clone) {
  int target_cell = PositionOffspring(GetCell(org_to_clone->GetCellID()), ctx, 0).GetID();
  InjectClone(target_cell, *org_to_clone, Systematics::Source(Systematics::DUPLICATION, ""));
}

void cPopulation::PrintPhenotypeData(const cString& filename)
{
  set<int> ids;
  set<cString> complete;
  double average_shannon_diversity = 0.0;
  int num_orgs = 0; //could get from elsewhere, but more self-contained this way
  double average_num_tasks = 0.0;
  
  //implementing a very poor man's hash...
  Apto::Array<int> phenotypes;
  Apto::Array<int> phenotype_counts;
  
  for (int i = 0; i < cell_array.GetSize(); i++) {
    // Only look at cells with organisms in them.
    if (cell_array[i].IsOccupied() == false) continue;
    
    num_orgs++;
    const cPhenotype& phenotype = cell_array[i].GetOrganism()->GetPhenotype();
    
    int total_tasks = 0;
    int id = 0;
    cString key;
    for (int j = 0; j < phenotype.GetLastTaskCount().GetSize(); j++) {
      if (phenotype.GetLastTaskCount()[j] > 0) id += (1 << j);
      if (phenotype.GetLastTaskCount()[j] > 0) average_num_tasks += 1.0;
      key += cStringUtil::Stringf("%i-", phenotype.GetLastTaskCount()[j]);
      total_tasks += phenotype.GetLastTaskCount()[j];
    }
    ids.insert(id);
    complete.insert(key);
    
    // add one to our count for this key
    int k;
    for(k=0; k<phenotypes.GetSize(); k++)
    {
      if (phenotypes[k] == id) {
        phenotype_counts[k] = phenotype_counts[k] + 1;
        break;
      }
    }
    // this is a new key
    if (k == phenotypes.GetSize()) {
      phenotypes.Push(id);
      phenotype_counts.Push(1);
    }
    
    // go through again to calculate Shannon Diversity of task counts
    // now that we know the total number of tasks done
    double shannon_diversity = 0;
    for (int j = 0; j < phenotype.GetLastTaskCount().GetSize(); j++) {
      if (phenotype.GetLastTaskCount()[j] == 0) continue;
      double fraction = static_cast<double>(phenotype.GetLastTaskCount()[j]) / static_cast<double>(total_tasks);
      shannon_diversity -= fraction * log(fraction) / log(2.0);
    }
    
    average_shannon_diversity += static_cast<double>(shannon_diversity);
  }
  
  double shannon_diversity_of_phenotypes = 0.0;
  for (int j = 0; j < phenotype_counts.GetSize(); j++) {
    double fraction = static_cast<double>(phenotype_counts[j]) / static_cast<double>(num_orgs);
    shannon_diversity_of_phenotypes -= fraction * log(fraction) / log(2.0);
  }
  
  average_shannon_diversity /= static_cast<double>(num_orgs);
  average_num_tasks /= num_orgs;
  
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  df->WriteTimeStamp();
  df->Write(m_world->GetStats().GetUpdate(), "Update");
  df->Write(static_cast<int>(ids.size()), "Unique Phenotypes (by task done)");
  df->Write(shannon_diversity_of_phenotypes, "Shannon Diversity of Phenotypes (by task done)");
  df->Write(static_cast<int>(complete.size()), "Unique Phenotypes (by task count)");
  df->Write(average_shannon_diversity, "Average Phenotype Shannon Diversity (by task count)");
  df->Write(average_num_tasks, "Average Task Diversity (number of different tasks)");
  df->Endl();
}

void cPopulation::PrintPhenotypeStatus(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Num orgs doing each task for each deme in population");
  df->WriteTimeStamp();
  df->Write(m_world->GetStats().GetUpdate(), "Update");
  
  cString comment;
  
  for (int i = 0; i < cell_array.GetSize(); i++)
  {
    // Only look at cells with organisms in them.
    if (cell_array[i].IsOccupied() == false) continue;
    
    const cPhenotype& phenotype = cell_array[i].GetOrganism()->GetPhenotype();
    
    comment.Set("cur_merit %d;", i);
    df->Write(phenotype.GetMerit().GetDouble(), comment);
    
    comment.Set("cur_merit_base %d;", i);
    df->Write(phenotype.GetCurMeritBase(), comment);
    
    comment.Set("cur_merit_bonus %d;", i);
    df->Write(phenotype.GetCurBonus(), comment);
    
    //    comment.Set("last_merit %d", i);
    //    df->Write(phenotype.GetLastMerit(), comment);
    
    comment.Set("last_merit_base %d", i);
    df->Write(phenotype.GetLastMeritBase(), comment);
    
    comment.Set("last_merit_bonus %d", i);
    df->Write(phenotype.GetLastBonus(), comment);
    
    comment.Set("life_fitness %d", i);
    df->Write(phenotype.GetLifeFitness(), comment);
    
    comment.Set("*");
    df->Write("*", comment);
    
  }
  df->Endl();
  
}

void cPopulation::PrintHostPhenotypeData(const cString& filename)
{
  set<int> ids;
  set<cString> complete;
  double average_shannon_diversity = 0.0;
  int num_orgs = 0; //could get from elsewhere, but more self-contained this way
  double average_num_tasks = 0.0;
  
  //implementing a very poor man's hash...
  Apto::Array<int> phenotypes;
  Apto::Array<int> phenotype_counts;
  
  for (int i = 0; i < cell_array.GetSize(); i++) {
    // Only look at cells with organisms in them.
    if (cell_array[i].IsOccupied() == false) continue;
    
    num_orgs++;
    const cPhenotype& phenotype = cell_array[i].GetOrganism()->GetPhenotype();
    
    int total_tasks = 0;
    int id = 0;
    cString key;
    for (int j = 0; j < phenotype.GetLastHostTaskCount().GetSize(); j++) {
      if (phenotype.GetLastHostTaskCount()[j] > 0) id += (1 << j);
      if (phenotype.GetLastHostTaskCount()[j] > 0) average_num_tasks += 1.0;
      key += cStringUtil::Stringf("%i-", phenotype.GetLastHostTaskCount()[j]);
      total_tasks += phenotype.GetLastHostTaskCount()[j];
    }
    ids.insert(id);
    complete.insert(key);
    
    // add one to our count for this key
    int k;
    for(k=0; k<phenotypes.GetSize(); k++)
    {
      if (phenotypes[k] == id) {
        phenotype_counts[k] = phenotype_counts[k] + 1;
        break;
      }
    }
    // this is a new key
    if (k == phenotypes.GetSize()) {
      phenotypes.Push(id);
      phenotype_counts.Push(1);
    }
    
    // go through again to calculate Shannon Diversity of task counts
    // now that we know the total number of tasks done
    double shannon_diversity = 0;
    for (int j = 0; j < phenotype.GetLastHostTaskCount().GetSize(); j++) {
      if (phenotype.GetLastHostTaskCount()[j] == 0) continue;
      double fraction = static_cast<double>(phenotype.GetLastHostTaskCount()[j]) / static_cast<double>(total_tasks);
      shannon_diversity -= fraction * log(fraction) / log(2.0);
    }
    
    average_shannon_diversity += static_cast<double>(shannon_diversity);
  }
  
  double shannon_diversity_of_phenotypes = 0.0;
  for (int j = 0; j < phenotype_counts.GetSize(); j++) {
    double fraction = static_cast<double>(phenotype_counts[j]) / static_cast<double>(num_orgs);
    shannon_diversity_of_phenotypes -= fraction * log(fraction) / log(2.0);
  }
  
  average_shannon_diversity /= static_cast<double>(num_orgs);
  average_num_tasks /= num_orgs;
  
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  df->WriteTimeStamp();
  df->Write(m_world->GetStats().GetUpdate(), "Update");
  df->Write(static_cast<int>(ids.size()), "Unique Phenotypes (by task done)");
  df->Write(shannon_diversity_of_phenotypes, "Shannon Diversity of Phenotypes (by task done)");
  df->Write(static_cast<int>(complete.size()), "Unique Phenotypes (by task count)");
  df->Write(average_shannon_diversity, "Average Phenotype Shannon Diversity (by task count)");
  df->Write(average_num_tasks, "Average Task Diversity (number of different tasks)");
  df->Endl();
}

void cPopulation::PrintParasitePhenotypeData(const cString& filename)
{
  set<int> ids;
  set<cString> complete;
  double average_shannon_diversity = 0.0;
  int num_orgs = 0; //could get from elsewhere, but more self-contained this way
  double average_num_tasks = 0.0;
  
  //implementing a very poor man's hash...
  Apto::Array<int> phenotypes;
  Apto::Array<int> phenotype_counts;
  
  for (int i = 0; i < cell_array.GetSize(); i++) {
    // Only look at cells with organisms in them.
    if (cell_array[i].IsOccupied() == false) continue;
    
    num_orgs++;
    const cPhenotype& phenotype = cell_array[i].GetOrganism()->GetPhenotype();
    
    int total_tasks = 0;
    int id = 0;
    cString key;
    for (int j = 0; j < phenotype.GetLastParasiteTaskCount().GetSize(); j++) {
      if (phenotype.GetLastParasiteTaskCount()[j] > 0) id += (1 << j);
      if (phenotype.GetLastParasiteTaskCount()[j] > 0) average_num_tasks += 1.0;
      key += cStringUtil::Stringf("%i-", phenotype.GetLastParasiteTaskCount()[j]);
      total_tasks += phenotype.GetLastParasiteTaskCount()[j];
    }
    ids.insert(id);
    complete.insert(key);
    
    // add one to our count for this key
    int k;
    for(k=0; k<phenotypes.GetSize(); k++)
    {
      if (phenotypes[k] == id) {
        phenotype_counts[k] = phenotype_counts[k] + 1;
        break;
      }
    }
    // this is a new key
    if (k == phenotypes.GetSize()) {
      phenotypes.Push(id);
      phenotype_counts.Push(1);
    }
    
    // go through again to calculate Shannon Diversity of task counts
    // now that we know the total number of tasks done
    double shannon_diversity = 0;
    for (int j = 0; j < phenotype.GetLastParasiteTaskCount().GetSize(); j++) {
      if (phenotype.GetLastParasiteTaskCount()[j] == 0) continue;
      double fraction = static_cast<double>(phenotype.GetLastParasiteTaskCount()[j]) / static_cast<double>(total_tasks);
      shannon_diversity -= fraction * log(fraction) / log(2.0);
    }
    
    average_shannon_diversity += static_cast<double>(shannon_diversity);
  }
  
  double shannon_diversity_of_phenotypes = 0.0;
  for (int j = 0; j < phenotype_counts.GetSize(); j++) {
    double fraction = static_cast<double>(phenotype_counts[j]) / static_cast<double>(num_orgs);
    shannon_diversity_of_phenotypes -= fraction * log(fraction) / log(2.0);
  }
  
  average_shannon_diversity /= static_cast<double>(num_orgs);
  average_num_tasks /= num_orgs;
  
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  df->WriteTimeStamp();
  df->Write(m_world->GetStats().GetUpdate(), "Update");
  df->Write(static_cast<int>(ids.size()), "Unique Phenotypes (by task done)");
  df->Write(shannon_diversity_of_phenotypes, "Shannon Diversity of Phenotypes (by task done)");
  df->Write(static_cast<int>(complete.size()), "Unique Phenotypes (by task count)");
  df->Write(average_shannon_diversity, "Average Phenotype Shannon Diversity (by task count)");
  df->Write(average_num_tasks, "Average Task Diversity (number of different tasks)");
  df->Endl();
}

bool cPopulation::UpdateMerit(cAvidaContext& ctx, int cell_id, double new_merit)
{
  assert( GetCell(cell_id).IsOccupied() == true);
  
  if (new_merit <= 0) KillOrganism(ctx, cell_id);
  
  cPhenotype & phenotype = GetCell(cell_id).GetOrganism()->GetPhenotype();
  double old_merit = phenotype.GetMerit().GetDouble();
  
  phenotype.SetMerit( cMerit(new_merit) );
  phenotype.SetLifeFitness(new_merit/phenotype.GetGestationTime());
  if (new_merit <= old_merit) {
    phenotype.SetIsDonorCur(); }
  else  { phenotype.SetIsReceiver(); }
  AdjustSchedule(GetCell(cell_id), phenotype.GetMerit());
  
  return true;
}


void cPopulation::AddBeginSleep(int cellID, int start_time) {
  sleep_log[cellID].Push(make_pair(start_time,-1));
}

void cPopulation::AddEndSleep(int cellID, int end_time) {
  pair<int,int> p = sleep_log[cellID][sleep_log[cellID].GetSize() - 1];
  sleep_log[cellID].RemoveAt(sleep_log[cellID].GetSize() - 1);
  sleep_log[cellID].Push(make_pair(p.first, end_time));
}

// Starts a new trial for each organism in the population
void cPopulation::NewTrial(cAvidaContext& ctx)
{
  for (int i=0; i< GetSize(); i++) {
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied()) {
      cPhenotype & p =  cell.GetOrganism()->GetPhenotype();
      
      // Don't continue if the time used was zero
      if (p.GetTrialTimeUsed() != 0) {
        // Correct gestation time for speculative execution
        p.SetTrialTimeUsed(p.GetTrialTimeUsed() - cell.GetSpeculativeState());
        p.SetTimeUsed(p.GetTimeUsed() - cell.GetSpeculativeState());
        
        cell.GetOrganism()->NewTrial();
        cell.GetOrganism()->GetHardware().Reset(ctx);
        
        cell.SetSpeculativeState(0);
      }
    }
  }
  
  //Recalculate the stats immediately, so that if they are printed before a new update
  //is processed, they accurately reflect this trial only...
  cStats& stats = m_world->GetStats();
  stats.ProcessUpdate();
  ProcessPostUpdate(ctx);
}

/*
 CompeteOrganisms
 
 parents_survive => for any organism represented by >=1 child, the first created is the parent (has no mutations)
 dynamic_scaling => rescale the time interval such that the geometric mean of the highest fitness versus lower fitnesses
 equals a time of 1 unit
 */

void cPopulation::CompeteOrganisms(cAvidaContext& ctx, int competition_type, int parents_survive)
{
  NewTrial(ctx);
  
  double total_fitness = 0;
  int num_cells = GetSize();
  Apto::Array<double> org_fitness(num_cells);
  
  double lowest_fitness = -1.0;
  double average_fitness = 0;
  double highest_fitness = -1.0;
  double lowest_fitness_copied = -1.0;
  double average_fitness_copied = 0;
  double highest_fitness_copied = -1.0;
  int different_orgs_copied = 0;
  int num_competed_orgs = 0;
  
  int num_trials = -1;
  
  int dynamic_scaling = 0;
  
  if (competition_type==3) dynamic_scaling = 1;
  else if  (competition_type==4) dynamic_scaling = 2;
  
  // How many trials were there? -- same for every organism
  // we just need to find one...
  for (int i = 0; i < num_cells; i++) {
    if (GetCell(i).IsOccupied()) {
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      // We trigger a lot of asserts if the copied size is zero...
      p.SetLinesCopied(p.GetGenomeLength());
      
      if ( (num_trials != -1) && (num_trials != p.GetTrialFitnesses().GetSize()) ) {
        cout << "The number of trials is not the same for every organism in the population.\n";
        cout << "You need to remove all normal ways of replicating for CompeteOrganisms to work correctly.\n";
        exit(1);
      }
      
      num_trials = p.GetTrialFitnesses().GetSize();
    }
  }
  
  // If there weren't any trials then end here (but call new trial so things are set up for the next iteration)
  if (num_trials == 0) return;
  
  if (m_world->GetVerbosity() > VERBOSE_SILENT) cout << "==Compete Organisms==" << endl;
  
  Apto::Array<double> min_trial_fitnesses(num_trials);
  Apto::Array<double> max_trial_fitnesses(num_trials);
  Apto::Array<double> avg_trial_fitnesses(num_trials);
  avg_trial_fitnesses.SetAll(0);
  
  bool init = false;
  // What is the min and max fitness in each trial
  for (int i = 0; i < num_cells; i++) {
    if (GetCell(i).IsOccupied()) {
      num_competed_orgs++;
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      Apto::Array<double> trial_fitnesses = p.GetTrialFitnesses();
      for (int t=0; t < num_trials; t++) {
        if ((!init) || (min_trial_fitnesses[t] > trial_fitnesses[t])) {
          min_trial_fitnesses[t] = trial_fitnesses[t];
        }
        if ((!init) || (max_trial_fitnesses[t] < trial_fitnesses[t])) {
          max_trial_fitnesses[t] = trial_fitnesses[t];
        }
        avg_trial_fitnesses[t] += trial_fitnesses[t];
      }
      init = true;
    }
  }
  
  //divide averages for each trial
  for (int t=0; t < num_trials; t++) {
    avg_trial_fitnesses[t] /= num_competed_orgs;
  }
  
  if (m_world->GetVerbosity() > VERBOSE_SILENT) {
    if (min_trial_fitnesses.GetSize() > 1) {
      for (int t=0; t < min_trial_fitnesses.GetSize(); t++) {
        cout << "Trial #" << t << " Min Fitness = " << min_trial_fitnesses[t] << ", Avg fitness = " << avg_trial_fitnesses[t] << " Max Fitness = " << max_trial_fitnesses[t] << endl;
      }
    }
  }
  
  bool using_trials = true;
  for (int i = 0; i < num_cells; i++) {
    if (GetCell(i).IsOccupied()) {
      double fitness = 0.0;
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      //Don't need to reset trial_fitnesses because we will call cPhenotype::OffspringReset on the entire pop
      Apto::Array<double> trial_fitnesses = p.GetTrialFitnesses();
      
      //If there are no trial fitnesses...use the actual fitness.
      if (trial_fitnesses.GetSize() == 0) {
        using_trials = false;
        trial_fitnesses.Push(p.GetFitness());
      }
      switch (competition_type) {
          //Geometric Mean
        case 0:
        case 3:
        case 4:
          //Treat as logs to avoid overflow when multiplying very large fitnesses
          fitness = 0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) {
            fitness += log(trial_fitnesses[t]);
          }
          fitness /= (double)trial_fitnesses.GetSize();
          fitness = exp( fitness );
          break;
          
          //Product
        case 5:
          //Treat as logs to avoid overflow when multiplying very large fitnesses
          fitness = 0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) {
            fitness += log(trial_fitnesses[t]);
          }
          fitness = exp( fitness );
          break;
          
          //Geometric Mean of normalized values
        case 1:
          fitness = 1.0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) {
            fitness*=trial_fitnesses[t] / max_trial_fitnesses[t];
          }
          fitness = exp( (1.0/((double)trial_fitnesses.GetSize())) * log(fitness) );
          break;
          
          //Arithmetic Mean
        case 2:
          fitness = 0;
          for (int t=0; t < trial_fitnesses.GetSize(); t++) {
            fitness+=trial_fitnesses[t];
          }
          fitness /= (double)trial_fitnesses.GetSize();
          break;
          
        default:
          ctx.Driver().Feedback().Error("Unknown CompeteOrganisms method");
          ctx.Driver().Abort(Avida::INVALID_CONFIG);
      }
      if (m_world->GetVerbosity() >= VERBOSE_DETAILS) {
        cout << "Trial fitness in cell " << i << " = " << fitness << endl;
      }
      org_fitness[i] = fitness;
      total_fitness += fitness;
      
      if ((highest_fitness == -1.0) || (fitness > highest_fitness)) highest_fitness = fitness;
      if ((lowest_fitness == -1.0) || (fitness < lowest_fitness)) lowest_fitness = fitness;
    } // end if occupied
  }
  average_fitness = total_fitness / num_competed_orgs;
  
  //Rescale by the geometric mean of the difference from the top score and the average
  if ( dynamic_scaling == 1 ) {
    double dynamic_factor = 1.0;
    if (highest_fitness > 0) {
      dynamic_factor = 2 / highest_fitness;
    }
    
    total_fitness = 0;
    for (int i = 0; i < num_cells; i++) {
      if ( GetCell(i).IsOccupied() ) {
        org_fitness[i] *= dynamic_factor;
        total_fitness += org_fitness[i];
      }
    }
  }
  
  // Rescale geometric mean to 1
  else if ( dynamic_scaling == 2 ) {
    int num_org = 0;
    double dynamic_factor = 1.0;
    for (int i = 0; i < num_cells; i++) {
      if ( GetCell(i).IsOccupied() && (org_fitness[i] > 0.0)) {
        num_org++;
        dynamic_factor += log(org_fitness[i]);
      }
    }
    
    cout << "Scaling factor = " << dynamic_factor << endl;
    if (num_org > 0) dynamic_factor = exp(dynamic_factor / (double)num_org);
    cout << "Scaling factor = " << dynamic_factor << endl;
    
    total_fitness = 0;
    
    for (int i = 0; i < num_cells; i++) {
      if ( GetCell(i).IsOccupied() ) {
        org_fitness[i] /= dynamic_factor;
        total_fitness += org_fitness[i];
      }
    }
  }
  
  // Pick which orgs should be in the next generation. (Filling all cells)
  Apto::Array<int> new_orgs(num_cells);
  for (int i = 0; i < num_cells; i++) {
    double birth_choice = (double) ctx.GetRandom().GetDouble(total_fitness);
    double test_total = 0;
    for (int test_org = 0; test_org < num_cells; test_org++) {
      test_total += org_fitness[test_org];
      if (birth_choice < test_total) {
        new_orgs[i] = test_org;
        if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Propagating from cell " << test_org << " to " << i << endl;
        if ((highest_fitness_copied == -1.0) || (org_fitness[test_org] > highest_fitness_copied)) highest_fitness_copied = org_fitness[test_org];
        if ((lowest_fitness_copied == -1.0) || (org_fitness[test_org] < lowest_fitness_copied)) lowest_fitness_copied = org_fitness[test_org];
        average_fitness_copied += org_fitness[test_org];
        break;
      }
    }
  }
  // average assumes we fill all cells.
  average_fitness_copied /= num_cells;
  
  // Track how many of each org we should have.
  Apto::Array<int> org_count(num_cells);
  org_count.SetAll(0);
  for (int i = 0; i < num_cells; i++) {
    org_count[new_orgs[i]]++;
  }
  
  // Reset organism phenotypes that have successfully divided! Must do before injecting children.
  // -- but not the full reset if we are using trials, the trial reset should already cover things like task counts, etc.
  // calling that twice would erase this information before it could potentially be output between NewTrial and CompeteOrganisms events.
  for (int i = 0; i < num_cells; i++) {
    if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Cell " << i << " has " << org_count[i] << " copies in the next generation" << endl;
    
    if (org_count[i] > 0) {
      different_orgs_copied++;
      cPhenotype& p = GetCell(i).GetOrganism()->GetPhenotype();
      ConstInstructionSequencePtr seq;
      seq.DynamicCastFrom(GetCell(i).GetOrganism()->GetGenome().Representation());
      if (using_trials)
      {
        p.TrialDivideReset(*seq);
      }
      else //trials not used
      {
        //TrialReset has never been called so we need the entire routine to make "last" of "cur" stats.
        p.DivideReset(*seq);
      }
    }
  }
  
  Apto::Array<bool> is_init(num_cells);
  is_init.SetAll(false);
  
  // Copy orgs until all org counts are 1.
  int last_from_cell_id = 0;
  int last_to_cell_id = 0;
  while (true) {
    // Find the next org to copy...
    int from_cell_id, to_cell_id;
    for (from_cell_id = last_from_cell_id; from_cell_id < num_cells; from_cell_id++) {
      if (org_count[from_cell_id] > 1) break;
    }
    last_from_cell_id = from_cell_id;
    
    // Stop if we didn't find another org to copy
    if (from_cell_id == num_cells) break;
    
    for (to_cell_id = last_to_cell_id; to_cell_id < num_cells; to_cell_id++) {
      if (org_count[to_cell_id] == 0) break;
    }
    last_to_cell_id = to_cell_id;
    
    // We now have both a "from" and a "to" org....
    org_count[from_cell_id]--;
    org_count[to_cell_id]++;
    
    cOrganism* organism = GetCell(from_cell_id).GetOrganism();
    organism->OffspringGenome() = organism->GetGenome();
    if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Injecting Offspring " << from_cell_id << " to " << to_cell_id << endl;
    CompeteOrganisms_ConstructOffspring(to_cell_id, *organism);
    
    is_init[to_cell_id] = true;
  }
  
  if (!parents_survive)
  {
    // Now create children from remaining cells into themselves
    for (int cell_id = 0; cell_id < num_cells; cell_id++) {
      if (!is_init[cell_id])
      {
        cOrganism* organism = GetCell(cell_id).GetOrganism();
        organism->OffspringGenome() = organism->GetGenome();
        if (m_world->GetVerbosity() >= VERBOSE_DETAILS) cout << "Re-injecting Self " << cell_id << " to " << cell_id << endl;
        CompeteOrganisms_ConstructOffspring(cell_id, *organism);
      }
    }
  }
  
  
  if (m_world->GetVerbosity() > VERBOSE_SILENT)
  {
    cout << "Competed: Min fitness = " << lowest_fitness << ", Avg fitness = " << average_fitness << " Max fitness = " << highest_fitness << endl;
    cout << "Copied  : Min fitness = " << lowest_fitness_copied << ", Avg fitness = " << average_fitness_copied << ", Max fitness = " << highest_fitness_copied << endl;
    cout << "Copied  : Different organisms = " << different_orgs_copied << endl;
  }
  
  // copy stats to cStats, so that these can be remembered and printed
  m_world->GetStats().SetCompetitionTrialFitnesses(avg_trial_fitnesses);
  m_world->GetStats().SetCompetitionFitnesses(average_fitness, lowest_fitness, highest_fitness, average_fitness_copied, lowest_fitness_copied, highest_fitness_copied);
  m_world->GetStats().SetCompetitionOrgsReplicated(different_orgs_copied);
  
  NewTrial(ctx);
}


/* This routine is designed to change values in the resource count in the
 middle of a run.  This is designed to work with cActionSetGradient Count */
//JW

void cPopulation::UpdateGradientCount(cAvidaContext& ctx, const int verbosity, cWorld* world, const cString res_name)
{
  (void)verbosity;
  
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    
    if (!res->GetDemeResource()) global_res_index++;
    
    if (res->GetName() == res_name) {
      resource_count.SetGradientCount(ctx, world, global_res_index, res->GetPeakX(), res->GetPeakY(),
                           res->GetHeight(), res->GetSpread(), res->GetPlateau(), res->GetDecay(), 
                           res->GetMaxX(), res->GetMinX(), res->GetMaxY(), res->GetMinY(), res->GetAscaler(), res->GetUpdateStep(),
                           res->GetHalo(), res->GetHaloInnerRadius(), res->GetHaloWidth(),
                           res->GetHaloAnchorX(), res->GetHaloAnchorY(), res->GetMoveSpeed(), res->GetMoveResistance(),
                           res->GetPlateauInflow(), res->GetPlateauOutflow(), res->GetConeInflow(), res->GetConeOutflow(),
                           res->GetGradientInflow(), res->GetIsPlateauCommon(), res->GetFloor(), res->GetHabitat(), 
                           res->GetMinSize(), res->GetMaxSize(), res->GetConfig(), res->GetCount(), res->GetResistance(), res->GetDamage(),
                           res->GetDeathOdds(), res->IsPath(), res->IsHammer(), res->GetInitialPlatVal(), res->GetThreshold(), res->GetRefuge());
    } 
  }
}

void cPopulation::UpdateGradientPlatInflow(const cString res_name, const double inflow)
{
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
      res->SetPlateauInflow(inflow);
      resource_count.SetGradientPlatInflow(global_res_index, inflow);
    }
  } 
}

void cPopulation::UpdateGradientPlatOutflow(const cString res_name, const double outflow)
{
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
      res->SetPlateauOutflow(outflow);
      resource_count.SetGradientPlatOutflow(global_res_index, outflow);
    }
  } 
}

void cPopulation::UpdateGradientConeInflow(const cString res_name, const double inflow)
{
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
      res->SetConeInflow(inflow);
      resource_count.SetGradientConeInflow(global_res_index, inflow);
    }
  } 
}

void cPopulation::UpdateGradientConeOutflow(const cString res_name, const double outflow)
{
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
      res->SetConeOutflow(outflow);
      resource_count.SetGradientConeOutflow(global_res_index, outflow);
    }
  } 
}

void cPopulation::UpdateGradientInflow(const cString res_name, const double inflow)
{
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
      res->SetGradientInflow(inflow);
      resource_count.SetGradientInflow(global_res_index, inflow);
    }
  } 
}

void cPopulation::SetGradPlatVarInflow(cAvidaContext& ctx, const cString res_name, const double mean, const double variance, const int type)
{
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
      resource_count.SetGradPlatVarInflow(ctx, global_res_index, mean, variance, type);
    }
  } 
}

void cPopulation::SetPredatoryResource(const cString res_name, const double odds, const int juvsper, const double detection_prob)
{
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource* res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
      res->SetPredatoryResource(odds, juvsper, detection_prob);
      res->SetHabitat(5);
      environment.AddHabitat(5);
      resource_count.SetPredatoryResource(global_res_index, odds, juvsper);
    }
  }
}

void cPopulation::SetProbabilisticResource(cAvidaContext& ctx, const cString res_name, const double initial, const double inflow,
  const double outflow, const double lambda, const double theta, const int x, const int y, const int count)
{
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource* res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) global_res_index++;
    if (res->GetName() == res_name) {
      resource_count.SetProbabilisticResource(ctx, global_res_index, initial, inflow, outflow, lambda, theta, x, y, count);
      break;
    }
  }
}

void cPopulation::UpdateInflow(const cString& res_name, const double change)
{
  const cResourceLib & resource_lib = environment.GetResourceLib();  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource* res = resource_lib.GetResource(i);
    if (res->GetName() == res_name) {
      resource_count.SetInflow(res_name, resource_count.GetInflow(res_name) + change);
    }
  }
  assert(resource_count.GetInflow(res_name) >= 0);
  if (resource_count.GetInflow(res_name) < 0) cout << "WARNING: update to inflow rate results in negative resource inflow!" << endl;
}

void cPopulation::ExecutePredatoryResource(cAvidaContext& ctx, const int cell_id, const double pred_odds, const int juvs_per, const bool hammer)
{
  cPopulationCell& cell = m_world->GetPopulation().GetCell(cell_id);

  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  
  if (!hammer) {
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->IsPath()) {
        double dest_cell_resources = GetCellResVal(ctx, cell_id, i);
        if (dest_cell_resources > 0) return;
      }
    }
  }
    
  const int juv_age = m_world->GetConfig().JUV_PERIOD.Get();
  
  bool cell_has_den = false;
  for (int j = 0; j < resource_lib.GetSize(); j++) {
    if (resource_lib.GetResource(j)->GetHabitat() == 4 || resource_lib.GetResource(j)->GetHabitat() == 3) {
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
            if (!juvs[k]->IsRunning()) KillOrganism(GetCell(juvs[k]->GetCellID()), ctx);
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
        cOrganism* target_org = cell_avs[ctx.GetRandom().GetUInt(cell_avs.GetSize())];
        if (!target_org->IsDead()) {
          if (!target_org->IsRunning()) KillOrganism(GetCell(target_org->GetCellID()), ctx);
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
        if (!target_org->IsRunning()) KillOrganism(GetCell(target_org->GetCellID()), ctx);
        else target_org->GetPhenotype().SetToDie();
      }
    }
  }
}

void cPopulation::ExecuteDamagingResource(cAvidaContext& ctx, const int cell_id, const double damage, const bool hammer)
{
  cPopulationCell& cell = GetCell(cell_id);
  
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  
  if (!hammer) {
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->IsPath()) {
        double dest_cell_resources = GetCellResVal(ctx, cell_id, i);
        if (dest_cell_resources > 0) return;
      }
    }
  }

  if (m_world->GetConfig().USE_AVATARS.Get() && cell.HasAV()) {
    Apto::Array<cOrganism*> cell_avs = cell.GetCellAVs();
    for (int i = 0; i < cell_avs.GetSize(); i++) {
      InjureOrg(ctx, GetCell(cell_avs[i]->GetCellID()), damage, false);
    }
  }
  else if (!m_world->GetConfig().USE_AVATARS.Get() && cell.IsOccupied()) InjureOrg(ctx, GetCell(cell_id), damage, false);
}

void cPopulation::ExecuteDeadlyResource(cAvidaContext& ctx, const int cell_id, const double odds, const bool hammer)
{
  cPopulationCell& cell = GetCell(cell_id);
  
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  
  if (!hammer) {
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->IsPath()) {
        double dest_cell_resources = GetCellResVal(ctx, cell_id, i);
        if (dest_cell_resources > 0) return;
      }
    }
  }
  if (m_world->GetConfig().USE_AVATARS.Get() && cell.HasAV()) {
    Apto::Array<cOrganism*> cell_avs = cell.GetCellAVs();
    for (int i = 0; i < cell_avs.GetSize(); i++) {
      if (ctx.GetRandom().P(odds)) {
        cOrganism* target_org = cell_avs[i];
        if (!target_org->IsDead()) {
          if (!target_org->IsRunning()) KillOrganism(GetCell(target_org->GetCellID()), ctx);
          else target_org->GetPhenotype().SetToDie();
        }
      }
    }
  }
  else if (!m_world->GetConfig().USE_AVATARS.Get() && cell.IsOccupied()) {
    if (ctx.GetRandom().P(odds)) {
      cOrganism* target_org = cell.GetOrganism();
      if (!target_org->IsDead()) {
        if (!target_org->IsRunning()) KillOrganism(GetCell(target_org->GetCellID()), ctx);
        else target_org->GetPhenotype().SetToDie();
      }
    }
  }
}

void cPopulation::UpdateResourceCount(const int Verbosity, cWorld* world) {
  const cResourceLib & resource_lib = environment.GetResourceLib();
  int global_res_index = -1;
  int deme_res_index = -1;
  int num_deme_res = 0;
  
  //setting size of global and deme-level resources
  for(int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (res->GetDemeResource())
      num_deme_res++;
  }
  
  for(int i = 0; i < GetNumDemes(); i++) {
    cResourceCount tmp_deme_res_count(num_deme_res);
    GetDeme(i).SetDemeResourceCount(tmp_deme_res_count);
  }
  
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    cResource * res = resource_lib.GetResource(i);
    if (!res->GetDemeResource()) {
      global_res_index++;
      const double decay = 1.0 - res->GetOutflow();
      resource_count.Setup(world, global_res_index, res->GetName(), res->GetInitial(),
                           res->GetInflow(), decay,
                           res->GetGeometry(), res->GetXDiffuse(),
                           res->GetXGravity(), res->GetYDiffuse(),
                           res->GetYGravity(), res->GetInflowX1(),
                           res->GetInflowX2(), res->GetInflowY1(),
                           res->GetInflowY2(), res->GetOutflowX1(),
                           res->GetOutflowX2(), res->GetOutflowY1(),
                           res->GetOutflowY2(), res->GetCellListPtr(),
                           res->GetCellIdListPtr(), Verbosity,
                           res->GetPeaks(), 
                           res->GetMinHeight(), res->GetMinRadius(), res->GetRadiusRange(),
                           res->GetAh(), res->GetAr(),
                           res->GetAcx(), res->GetAcy(),
                           res->GetHStepscale(), res->GetRStepscale(),
                           res->GetCStepscaleX(), res->GetCStepscaleY(),
                           res->GetHStep(), res->GetRStep(),
                           res->GetCStepX(), res->GetCStepY(),
                           res->GetUpdateDynamic(), res->GetPeakX(), res->GetPeakY(),
                           res->GetHeight(), res->GetSpread(), res->GetPlateau(), res->GetDecay(), 
                           res->GetMaxX(), res->GetMinX(), res->GetMaxY(), res->GetMinY(), res->GetAscaler(), res->GetUpdateStep(),
                           res->GetHalo(), res->GetHaloInnerRadius(), res->GetHaloWidth(),
                           res->GetHaloAnchorX(), res->GetHaloAnchorY(), res->GetMoveSpeed(), res->GetMoveResistance(),
                           res->GetPlateauInflow(), res->GetPlateauOutflow(), res->GetConeInflow(), res->GetConeOutflow(), 
                           res->GetGradientInflow(), res->GetIsPlateauCommon(), res->GetFloor(), res->GetHabitat(), 
                           res->GetMinSize(), res->GetMaxSize(), res->GetConfig(), res->GetCount(), res->GetResistance(), res->GetDamage(),
                           res->GetDeathOdds(), res->IsPath(), res->IsHammer(),
                           res->GetInitialPlatVal(), res->GetThreshold(), res->GetRefuge(), res->GetGradient()
                           ); 
      
    } else if (res->GetDemeResource()) {
      deme_res_index++;
      for(int j = 0; j < GetNumDemes(); j++) {
        GetDeme(j).SetupDemeRes(deme_res_index, res, Verbosity, world);       
        // could add deme resources to global resource stats here
      }
    } else {
      cerr<< "ERROR: Resource \"" << res->GetName() <<"\"is not a global or deme resource.  Exit";
      exit(1);
    }
  }
  
}

// Adds an organism to live org list  
void  cPopulation::AddLiveOrg(cOrganism* org)
{
  live_org_list.Push(org);
  org->SetOrgIndex(live_org_list.GetSize()-1);
}

// Remove an organism from live org list  
void  cPopulation::RemoveLiveOrg(cOrganism* org)
{
  unsigned int last = live_org_list.GetSize() - 1;
  cOrganism* exist_org = live_org_list[last];
  exist_org->SetOrgIndex(org->GetOrgIndex());
  live_org_list.Swap(org->GetOrgIndex(), last);
  live_org_list.Pop();
}

// Adds an organism to a group
void  cPopulation::JoinGroup(cOrganism* org, int group_id)
{
  map<int,int>::iterator it;
  it=m_groups.find(group_id);
  if (it == m_groups.end()) {
    // new group
    m_groups[group_id] = 0;
    Apto::Array<cOrganism*, Apto::Smart> temp;
    m_group_list.Set(group_id, temp);
    // If tolerance is on, create the new group's tolerance cache
    if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
      Apto::Array<pair<int,int> > temp_array(2);
      temp_array[0] = make_pair(-1, -1);
      temp_array[1] = make_pair(-1, -1);
      m_group_intolerances.Set(group_id, temp_array);
      if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
        m_group_intolerances_females.Set(group_id, temp_array);
        m_group_intolerances_males.Set(group_id, temp_array);
        m_group_intolerances_juvs.Set(group_id, temp_array);
      }
    }
  }
  // add to group
  m_groups[group_id]++;
  if (org->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) m_group_females[group_id]++;
  else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) m_group_males[group_id]++;
  
  m_group_list[group_id].Push(org);
  // If tolerance is on, must add the organism's intolerance to the group cache
  if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
    int tol_max = m_world->GetConfig().MAX_TOLERANCE.Get();
    int immigrant_tol = org->GetPhenotype().CalcToleranceImmigrants();
    m_group_intolerances[group_id][0].second += tol_max - immigrant_tol;
    if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
      if (org->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) {
        m_group_intolerances_females[group_id][0].second += tol_max - immigrant_tol;
      } else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) {
        m_group_intolerances_males[group_id][0].second += tol_max - immigrant_tol;
      } else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_JUVENILE) { 
        m_group_intolerances_juvs[group_id][0].second += tol_max - immigrant_tol;
      }
    }
    m_group_intolerances[group_id][1].second += tol_max - org->GetPhenotype().CalcToleranceOffspringOthers();
  }
}

// Makes a new group (highest current group number +1)
void cPopulation::MakeGroup(cOrganism* org)
{
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() != 1) return;
  
  int highest_group;
  if (m_groups.size() > 0) {
    highest_group = m_groups.rbegin()->first;
  } else {
    highest_group = -1;
  }
  
  org->SetOpinion(highest_group + 1);
  JoinGroup(org, highest_group + 1);
}

// Removes an organism from a group
void  cPopulation::LeaveGroup(cOrganism* org, int group_id)
{
  map<int,int>::iterator it = m_groups.find(group_id);
  if (it != m_groups.end()) {
    m_groups[group_id]--;
    if (org->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) m_group_females[group_id]--;
    else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) m_group_males[group_id]--;
    
    // If no restrictions on group ids,
    // removes empty groups so the number of total groups being tracked doesn't become excessive
    // (Removes the highest group even if empty, causes misstep in marching groups). 
    if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 1) {
      if (m_groups[group_id] <= 0) {
        m_groups.erase(group_id);
      }
    }
  }

  // If tolerance is on, remove the organim's intolerance from the group's cache
  if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) { 
    int tol_max = m_world->GetConfig().MAX_TOLERANCE.Get();
    int immigrant_tol = org->GetPhenotype().CalcToleranceImmigrants();
    m_group_intolerances[group_id][0].second -= tol_max - immigrant_tol;
    if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
      if (org->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) {
        m_group_intolerances_females[group_id][0].second -= tol_max - immigrant_tol;
      } else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) {
        m_group_intolerances_males[group_id][0].second -= tol_max - immigrant_tol;
      } else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_JUVENILE) { 
        m_group_intolerances_juvs[group_id][0].second -= tol_max - immigrant_tol; 
      }
    }
    m_group_intolerances[group_id][1].second -= tol_max - org->GetPhenotype().CalcToleranceOffspringOthers();
  }
  
  for (int i = 0; i < m_group_list[group_id].GetSize(); i++) {
    if (m_group_list[group_id][i] == org) {
      unsigned int last = m_group_list[group_id].GetSize() - 1;
      m_group_list[group_id].Swap(i,last);
      m_group_list[group_id].Pop();
      // If no restrictions, removes empty groups. 
      if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 1) {
        if (m_group_list[group_id].GetSize() <= 0) {
          m_group_list.Remove(group_id);
        }
      }
      break;
    }
  }
}

// Identifies the number of organisms in a group
int  cPopulation::NumberOfOrganismsInGroup(int group_id)
{
  map<int,int>::iterator it;
  it=m_groups.find(group_id);
  int num_orgs = 0;
  if (it != m_groups.end()) {
    num_orgs = m_groups[group_id];
  }
  return num_orgs;
}

int  cPopulation::NumberGroupFemales(int group_id)
{
  map<int,int>::iterator it;
  it=m_groups.find(group_id);
  int num_orgs = 0;
  if (it != m_groups.end()) {
    num_orgs = m_group_females[group_id];
  }
  return num_orgs;
}

int  cPopulation::NumberGroupMales(int group_id)
{
  map<int,int>::iterator it;
  it=m_groups.find(group_id);
  int num_orgs = 0;
  if (it != m_groups.end()) {
    num_orgs = m_group_males[group_id];
  }
  return num_orgs;
}

int  cPopulation::NumberGroupJuvs(int group_id)
{
  map<int,int>::iterator it;
  it=m_groups.find(group_id);
  int num_males = 0;
  int num_females = 0;
  int tot_orgs = 0;
  if (it != m_groups.end()) {
    num_males = m_group_males[group_id];
    num_females = m_group_females[group_id];
    tot_orgs = m_groups[group_id];
  }
  return tot_orgs - (num_males + num_females);
}

void  cPopulation::ChangeGroupMatingTypes(cOrganism* org, int group_id, int old_type, int new_type)
{
  if (old_type == new_type) return;
  
  if (old_type == 0) m_group_females[group_id]--;
  else if (old_type == 1) m_group_males[group_id]--;
  
  if (new_type == 0) m_group_females[group_id]++;
  else if (new_type == 1) m_group_males[group_id]++;   
  
  if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) { 
    if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
      int tol_max = m_world->GetConfig().MAX_TOLERANCE.Get();
      int immigrant_tol = org->GetPhenotype().CalcToleranceImmigrants();
      // remove from old
      if (old_type == 0) {
        m_group_intolerances_females[group_id][0].second -= tol_max - immigrant_tol;
      } else if (old_type == 1) {
        m_group_intolerances_males[group_id][0].second -= tol_max - immigrant_tol;
      } else if (old_type == 2) {
        m_group_intolerances_juvs[group_id][0].second -= tol_max - immigrant_tol;   
      }
      // add to new
      if (new_type == 0) {
        m_group_intolerances_females[group_id][0].second += tol_max - immigrant_tol;
      } else if (new_type == 1) {
        m_group_intolerances_males[group_id][0].second += tol_max - immigrant_tol;
      } else if (new_type == 2) {
        m_group_intolerances_juvs[group_id][0].second += tol_max - immigrant_tol;      
      }
    }
  }
}

// Calculates group tolerance towards immigrants 
int cPopulation::CalcGroupToleranceImmigrants(int group_id, int mating_type)
{
  const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();
  
  if (group_id < 0) return tolerance_max;
  if (m_group_list[group_id].GetSize() <= 0) return tolerance_max;
  
  // use cache, if up to date
  int tol_update = m_group_intolerances[group_id][0].first;
  int group_intol = m_group_intolerances[group_id][0].second;
  if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
    if (mating_type == 0) { 
      tol_update = m_group_intolerances_females[group_id][0].first;
      group_intol = m_group_intolerances_females[group_id][0].second;
    } else if (mating_type == 1) { 
      tol_update = m_group_intolerances_males[group_id][0].first;
      group_intol = m_group_intolerances_males[group_id][0].second;
    } else if (mating_type == 2) { 
      tol_update = m_group_intolerances_juvs[group_id][0].first;
      group_intol = m_group_intolerances_juvs[group_id][0].second;
    }
  }  
  int cur_update = m_world->GetStats().GetUpdate();
  if (tol_update == cur_update) return max(0, tolerance_max - group_intol);

  // If can't use cache, sum the total group intolerance
  int group_intolerance = 0;  
  int single_member_intolerance = 0;
  for (int index = 0; index < m_group_list[group_id].GetSize(); index++) {
    bool use_org = true;
    // if using immigrant only tolerance + sex, only update the cache for this current mating type
    if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
      if (mating_type == 0 && m_group_list[group_id][index]->GetPhenotype().GetMatingType() != MATING_TYPE_FEMALE)  use_org = false;
      else if (mating_type == 1 && m_group_list[group_id][index]->GetPhenotype().GetMatingType() != MATING_TYPE_MALE)  use_org = false;
      else if (mating_type == 2 && m_group_list[group_id][index]->GetPhenotype().GetMatingType() != MATING_TYPE_JUVENILE)  use_org = false;
    }
    if (use_org) single_member_intolerance = tolerance_max - m_group_list[group_id][index]->GetPhenotype().CalcToleranceImmigrants();
    group_intolerance += single_member_intolerance;
  }
  
  // Save current update and current intolerance to group cache
  // this is the only time we can do this since this is the only 
  // time we ever look at the entire group (updated every individual) or sub-group (by sex)
  if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() != 2) {
    m_group_intolerances[group_id][0].first = cur_update;
    m_group_intolerances[group_id][0].second = group_intolerance;
  } else {
    if (mating_type == 0) {
      m_group_intolerances_females[group_id][0].first = cur_update;
      m_group_intolerances_females[group_id][0].second = group_intolerance;      
    } else if (mating_type == 1) {
      m_group_intolerances_males[group_id][0].first = cur_update;
      m_group_intolerances_males[group_id][0].second = group_intolerance;      
    } else if (mating_type == 2) {
      m_group_intolerances_juvs[group_id][0].first = cur_update;
      m_group_intolerances_juvs[group_id][0].second = group_intolerance;      
    }
  }
  
  int group_tolerance = tolerance_max - group_intolerance;
  // return zero if totally intolerant (no negative numbers)
  return max(0, group_tolerance);
}

// Calculates group tolerance towards offspring (not including parent) 
int cPopulation::CalcGroupToleranceOffspring(cOrganism* parent_organism)
{
  const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();
  int group_id = parent_organism->GetOpinion().first;
  
  if ((group_id < 0) || (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() > 0)) return tolerance_max;
  if (m_group_list[group_id].GetSize() <= 0) return tolerance_max;
  
  int cur_update = m_world->GetStats().GetUpdate();
  int parent_intolerance = tolerance_max - parent_organism->GetPhenotype().CalcToleranceOffspringOthers();
  
  int group_intolerance = 0;
  if (m_group_intolerances[group_id][1].first == cur_update) {
    group_intolerance = m_group_intolerances[group_id][1].second;
  } else {
    int single_member_intolerance = 0;
    // Sum the total group intolerance
    for (int index = 0; index < m_group_list[group_id].GetSize(); index++) {
      single_member_intolerance = tolerance_max - m_group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOthers();
      group_intolerance += single_member_intolerance;
    }
    // Save current update and current intolerance to cache
    m_group_intolerances[group_id][1].first = cur_update;
    m_group_intolerances[group_id][1].second = group_intolerance;
  }
  
  // Remove the parent intolerance
  group_intolerance -= parent_intolerance;
  int group_tolerance = tolerance_max - group_intolerance;
  return max(0, group_tolerance);
}

// Calculates the odds (out of 1) for successful immigration based on group's tolerance 
double cPopulation::CalcGroupOddsImmigrants(int group_id, int mating_type)
{
  if (group_id < 0) return 1.0;
  
  const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();
  int group_tolerance = CalcGroupToleranceImmigrants(group_id, mating_type);
  double immigrant_odds = (double) group_tolerance / (double) tolerance_max;
  return immigrant_odds;
}

// Returns true if the org successfully passes immigration tolerance and joins the group 
bool cPopulation::AttemptImmigrateGroup(cAvidaContext& ctx, int group_id, cOrganism* org)
{
  bool immigrate = false;
  // If non-standard group, automatic success
  if (group_id < 0) return true;
  
  // If there are no members of the target group, automatic successful immigration
  if (m_world->GetPopulation().NumberOfOrganismsInGroup(group_id) == 0) immigrate = true;
  else if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
    if (org->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE && NumberGroupFemales(group_id) == 0) immigrate = true;
    else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_MALE && NumberGroupMales(group_id) == 0) immigrate = true;
    else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_JUVENILE && NumberGroupJuvs(group_id) == 0) immigrate = true;
  // Calculate chances based on target group tolerance of another org successfully immigrating
  } else {
    int mating_type = -1;
    if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
      if (org->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) mating_type = 0;
      else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) mating_type = 1;
      else if (org->GetPhenotype().GetMatingType() == MATING_TYPE_JUVENILE) mating_type = 2;
    }
    double probability_immigration = CalcGroupOddsImmigrants(group_id, mating_type);
    
    double rand = ctx.GetRandom().GetDouble();
    if (rand <= probability_immigration) immigrate = true;
  }
  
  if (immigrate) {
    int opinion = m_world->GetConfig().DEFAULT_GROUP.Get();
    if (org->HasOpinion()) {
      opinion = org->GetOpinion().first;
      LeaveGroup(org, opinion);
    }
    org->SetOpinion(group_id);
    JoinGroup(org, group_id);
  }
  return immigrate;
}

// Calculates the odds (out of 1) for the organism's offspring to be born into its parent's group 
double cPopulation::CalcGroupOddsOffspring(cOrganism* parent)
{
  assert(parent->HasOpinion());
  
  // If non-standard group, automatic success
  if ((parent->GetOpinion().first < 0) || (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() > 0)) return 1.0;
  
  const double tolerance_max = (double) m_world->GetConfig().MAX_TOLERANCE.Get();
  
  double parent_tolerance = (double) parent->GetPhenotype().CalcToleranceOffspringOwn();
  double parent_group_tolerance = (double) CalcGroupToleranceOffspring(parent);
  
  const double prob_parent_allows =  parent_tolerance / tolerance_max;
  const double prob_group_allows = parent_group_tolerance / tolerance_max;
  
  double prob = prob_parent_allows * prob_group_allows;
  
  return prob;
}

// Calculates the odds (out of 1) for offspring to be born into the group 
double cPopulation::CalcGroupOddsOffspring(int group_id)
{
  // If non-standard group, automatic success
  if ((group_id < 0) || (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() > 0)) return 1.0;
  
  const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();
  
  int group_intolerance = 0;
  int single_member_intolerance = 0;
  for (int index = 0; index < m_group_list[group_id].GetSize(); index++) {
    single_member_intolerance = tolerance_max - m_group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOthers();
    group_intolerance += single_member_intolerance;
    if (group_intolerance >= tolerance_max) {
      group_intolerance = tolerance_max;
      break;
    }
  }
  
  int group_tolerance = tolerance_max - group_intolerance;
  double offspring_odds = (double) group_tolerance / (double) tolerance_max;
  return offspring_odds;
}

bool cPopulation::AttemptOffspringParentGroup(cAvidaContext& ctx, cOrganism* parent, cOrganism* offspring)
{
  // If joining a non-standard group, only immigrant tolerance, or only immigrant + sex tolerance, automatic success
  if ((parent->GetOpinion().first < 0) || (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() > 0)) {
    int parent_group = parent->GetOpinion().first;
    offspring->SetOpinion(parent_group);
    JoinGroup(offspring, parent_group);
    return true;
  }
  
  // If using % chance of random migration
  if (m_world->GetConfig().TOLERANCE_WINDOW.Get() < 0) {
    const int parent_group = parent->GetOpinion().first;
    const double prob_immigrate = ((double) m_world->GetConfig().TOLERANCE_WINDOW.Get() * -1.0) / 100.0;
    double rand = ctx.GetRandom().GetDouble();
    if (rand <= prob_immigrate) {
      const int num_groups = GetResources(ctx).GetSize();
      int target_group; 
      do {
        target_group = ctx.GetRandom().GetUInt(num_groups);
      } while (target_group == parent_group);
      offspring->SetOpinion(target_group);
      JoinGroup(offspring, target_group);
      return true;
    } else {
      // Put the offspring in the parent's group.
      assert(parent->HasOpinion());
      offspring->SetOpinion(parent_group);
      JoinGroup(offspring, parent_group);
      return true;
    }
  // If using standard tolerances
  } else if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
    assert(parent->HasOpinion());
    const double tolerance_max = (double) m_world->GetConfig().MAX_TOLERANCE.Get();
    const int parent_group = parent->GetOpinion().first;
    
    // Retrieve the parent's tolerance for its offspring
    double parent_tolerance = (double) parent->GetPhenotype().CalcToleranceOffspringOwn();
    // Retrieve the parent group's tolerance for offspring
    double parent_group_tolerance = (double) CalcGroupToleranceOffspring(parent);
    
    // Offspring first passes parent vote, then must also pass group vote
    // offspring first attempt to join the parent group and if unsuccessful attempt to immigrate
    const double prob_parent_allows = parent_tolerance / tolerance_max;
    const double prob_group_allows = parent_group_tolerance / tolerance_max;
    double rand2 = ctx.GetRandom().GetDouble();
    double rand = ctx.GetRandom().GetDouble();
    
    bool join_parent_group = false;
    
    if (rand <= prob_parent_allows) {
      // If there is nobody else in the group, the offspring gets in
      join_parent_group = true;
      // If there are others in the group, it's their turn
      if (m_group_list[parent_group].GetSize() > 1) {
        if (rand2 <= prob_group_allows) {
          // Offspring successfully joins parent's group
          join_parent_group = true;                       
        } else join_parent_group = false;
      }
    }
    
    if (join_parent_group) {
      offspring->SetOpinion(parent_group);
      JoinGroup(offspring, parent_group);  
      // Let the parent know that its offspring was born into its group
      parent->GetPhenotype().SetBornParentGroup() = true;
      return true;
    } else {
      // Let the parent know its offspring was not born into its group
      parent->GetPhenotype().SetBornParentGroup() = false;
    }
    
    // If the offspring is rejected by the parent group, and there are no other groups, the offspring is doomed
    const int num_groups = GetResources(ctx).GetSize();
    if (!join_parent_group && num_groups == 1) {
      return false;
    }
    
    // If the offspring is rejected by the parent group, and there are other groups, the offspring attempts to immigrate
    if (!join_parent_group && num_groups > 1) {
      // Find another group at random, which is not the parent's
      int target_group;
      do {
        target_group = ctx.GetRandom().GetUInt(num_groups);
      } while (target_group == parent_group);
      
      // If there are no members currently of the target group, offspring has 100% chance of immigrating
      if (m_group_list[target_group].GetSize() == 0) {
        offspring->SetOpinion(target_group);
        JoinGroup(offspring, target_group);
        return true;
      } else {
        double probability_born_target_group = CalcGroupOddsImmigrants(target_group, -1);
        
        rand = ctx.GetRandom().GetDouble();
        // Calculate if the offspring successfully immigrates
        if (rand <= probability_born_target_group) {
          // Offspring joins target group
          offspring->SetOpinion(target_group);
          JoinGroup(offspring, target_group);
          return true;
        } else {
          // Offspring fails to immigrate and is doomed
          return false;
        }
      }
    }
  }
  return false;
}

// Calculates the average for intra-group tolerance to immigrants
double cPopulation::CalcGroupAveImmigrants(int group_id, int mating_type)
{
  cDoubleSum immigrant_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < m_group_list[group_id].GetSize(); index++) {
    bool count_org = false;
    if (mating_type == -1) count_org = true;
    else if (mating_type == 0 && m_group_list[group_id][index]->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) {
      count_org = true;
    } else if (mating_type == 1 && m_group_list[group_id][index]->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) {
      count_org = true;
    } else if (mating_type == 2 && m_group_list[group_id][index]->GetPhenotype().GetMatingType() == MATING_TYPE_JUVENILE) {
      count_org = true;
    }
    if (count_org) {
      single_member_tolerance = m_group_list[group_id][index]->GetPhenotype().CalcToleranceImmigrants();
      immigrant_tolerance.Add(single_member_tolerance);
    }
  }
  double aveimmigrants = immigrant_tolerance.Average();
  return aveimmigrants;
}

// Calculates the standard deviation for group tolerance to immigrants
double cPopulation::CalcGroupSDevImmigrants(int group_id, int mating_type)
{
  cDoubleSum immigrant_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < m_group_list[group_id].GetSize(); index++) {
    bool count_org = false;
    if (mating_type == -1) count_org = true;
    else if (mating_type == 0 && m_group_list[group_id][index]->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) {
      count_org = true;
    } else if (mating_type == 1 && m_group_list[group_id][index]->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) {
      count_org = true;
    } else if (mating_type == 2 && m_group_list[group_id][index]->GetPhenotype().GetMatingType() == MATING_TYPE_JUVENILE) {
      count_org = true;
    }
    if (count_org) {
      single_member_tolerance = m_group_list[group_id][index]->GetPhenotype().CalcToleranceImmigrants();
      immigrant_tolerance.Add(single_member_tolerance);
    }
  }
  double sdevimmigrants = immigrant_tolerance.StdDeviation();
  return sdevimmigrants;
}

// Calculates the average for intra-group tolerance to own offspring
double cPopulation::CalcGroupAveOwn(int group_id)
{
  cDoubleSum own_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < m_group_list[group_id].GetSize(); index++) {
    single_member_tolerance = m_group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOwn();
    own_tolerance.Add(single_member_tolerance);
  }
  double aveown = own_tolerance.Average();
  return aveown;
}

// Calculates the standard deviation for group tolerance to their own offspring
double cPopulation::CalcGroupSDevOwn(int group_id)
{
  cDoubleSum own_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < m_group_list[group_id].GetSize(); index++) {
    single_member_tolerance = m_group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOwn();
    own_tolerance.Add(single_member_tolerance);
  }
  double sdevown = own_tolerance.StdDeviation();
  return sdevown;
}

// Calculates the average for intra-group tolerance to other offspring
double cPopulation::CalcGroupAveOthers(int group_id)
{
  cDoubleSum others_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < m_group_list[group_id].GetSize(); index++) {
    single_member_tolerance = m_group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOthers();
    others_tolerance.Add(single_member_tolerance);
  }
  double aveothers = others_tolerance.Average();
  return aveothers;
}

// Calculates the standard deviation for group tolerance to other group offspring
double cPopulation::CalcGroupSDevOthers(int group_id)
{
  cDoubleSum others_tolerance;
  int single_member_tolerance = 0;
  for (int index = 0; index < m_group_list[group_id].GetSize(); index++) {
    single_member_tolerance = m_group_list[group_id][index]->GetPhenotype().CalcToleranceOffspringOthers();
    others_tolerance.Add(single_member_tolerance);
  }
  double sdevothers = others_tolerance.StdDeviation();
  return sdevothers;
}

int& cPopulation::GetGroupIntolerances(int group_id, int tol_num, int mating_type)
{
  int& intolerance = m_group_intolerances[group_id][tol_num].second;
  if (mating_type == 0) intolerance = m_group_intolerances_females[group_id][tol_num].second;
  else if (mating_type == 1) intolerance = m_group_intolerances_males[group_id][tol_num].second;
  else if (mating_type == 2) intolerance = m_group_intolerances_juvs[group_id][tol_num].second;
  return intolerance;
}

/*!	Modify current level of the HGT resource.
 */
void cPopulation::AdjustHGTResource(cAvidaContext& ctx, double delta)
{
  if (m_hgt_resid != -1) {
    resource_count.Modify(ctx, m_hgt_resid, delta);
  }
}

/*! Mix all organisms in the population.
 
 This method rearranges the relationship between organisms and cells.  Specifically,
 we take all organisms in the population, and assign them to different randomly-selected
 cells.
 
 This isn't really useful in a single population run.  However, a mixing stage is a
 key component of biologically-inspired approaches to group selection (ie, Wilson's
 and Traulsen's models).
 
 \warning THIS METHOD CHANGES THE ORGANISM POINTERS OF CELLS.
 */
void cPopulation::MixPopulation(cAvidaContext& ctx)
{
  // Get the list of all organism pointers, including nulls:
  std::vector<cOrganism*> population(cell_array.GetSize());
  for(int i=0; i<cell_array.GetSize(); ++i) {
    population[i] = cell_array[i].GetOrganism();
  }
  
  // Shuffle them:
  std::random_shuffle(population.begin(), population.end(), ctx.GetRandom());
  
  // Reset the organism pointers of all cells:
  for(int i=0; i<cell_array.GetSize(); ++i) {
    cell_array[i].RemoveOrganism(ctx);
    if (population[i] == 0) {
      AdjustSchedule(cell_array[i], cMerit(0));
    } else {
      cell_array[i].InsertOrganism(population[i], ctx); 
      AdjustSchedule(cell_array[i], cell_array[i].GetOrganism()->GetPhenotype().GetMerit());
    }
  }
}

int cPopulation::PlaceAvatar(cAvidaContext& ctx, cOrganism* parent)
{
  int avatar_target_cell = -1;
  
  switch (m_world->GetConfig().AVATAR_BIRTH.Get()) {
    // Random
    case 1:
      avatar_target_cell = ctx.GetRandom().GetUInt(world_x * world_y);
      break;
      
    // Parent Facing
    case 2:
      avatar_target_cell = parent->GetOrgInterface().GetAVFacedCellID();
      break;
      
    // Next Cell
    case 3:
      avatar_target_cell += 1;
      if (avatar_target_cell >= world_x * world_y) avatar_target_cell = 0;
      break;
      
    // World Center
    case 4:
      avatar_target_cell = (int) (world_x * world_y) * 0.5;
      break;
      
    // Parent Facing
    default:
      avatar_target_cell = parent->GetOrgInterface().GetAVCellID();
      break;
  }
  
  if (m_world->GetConfig().DEADLY_BOUNDARIES.Get() == 1 && m_world->GetConfig().WORLD_GEOMETRY.Get() == 1 && avatar_target_cell >= 0) {
    int dest_x = avatar_target_cell % m_world->GetConfig().WORLD_X.Get();
    int dest_y = avatar_target_cell / m_world->GetConfig().WORLD_X.Get();
    if (dest_x == 0 || dest_y == 0 || dest_x == m_world->GetConfig().WORLD_X.Get() - 1 || dest_y == m_world->GetConfig().WORLD_Y.Get() - 1) {
      return -1;
    }
  }
  
  return avatar_target_cell;
}
