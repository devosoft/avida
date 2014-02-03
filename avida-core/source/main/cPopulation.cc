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

#include "cCPUTestInfo.h"
#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInitFile.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cTestCPU.h"
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
  
  static Data::ArgumentedProviderPtr Activate(cWorld* world, Universe* new_world)
  {
    (void)new_world;
    cPopulationOrgStatProviderPtr osp(new InstructionExecCountsProvider(world));
    world->GetPopulation().AttachOrgStatProvider(osp);
    return osp;
  }
};


cPopulation::cPopulation(cWorld* world)  
: m_world(world)
, m_scheduler(NULL)
, birth_chamber(world)
, environment(world->GetEnvironment())
, num_organisms(0)
, num_prey_organisms(0)
, num_pred_organisms(0)
, num_top_pred_organisms(0)
, sync_events(false)
{
  world_x = world->GetConfig().WORLD_X.Get();
  world_y = world->GetConfig().WORLD_Y.Get();
  

  SetupCellGrid();
  
  Data::ArgumentedProviderActivateFunctor activate(m_world, &cWorld::GetPopulationProvider);
  m_world->GetDataManager()->Register("core.population.group_id[]", activate);

  Apto::Functor<Data::ArgumentedProviderPtr, Apto::TL::Create<cWorld*, Universe*> > is_activate(&InstructionExecCountsProvider::Activate);
  Data::ArgumentedProviderActivateFunctor isp_activate(Apto::BindFirst(is_activate, m_world));
  m_world->GetDataManager()->Register("core.population.inst_exec_counts[]", isp_activate);
}

void cPopulation::SetupCellGrid()
{
  const int num_cells = world_x * world_y;
  
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
  
  BuildTimeSlicer();
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
  m_scheduler->AdjustPriority(cell.GetID(), merit.GetDouble());
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
  
  
  // Update the parent's phenotype.
  // This needs to be done before the parent goes into the birth chamber
  // or the merit doesn't get passed onto the offspring correctly
  cPhenotype& parent_phenotype = parent_organism->GetPhenotype();
  ConstInstructionSequencePtr seq;
  seq.DynamicCastFrom(parent_organism->GetGenome().Representation());
  parent_phenotype.DivideReset(*seq);
  
  
  birth_chamber.SubmitOffspring(ctx, offspring_genome, parent_organism, offspring_array, merit_array);
  
  // First, setup the genotype of all of the offspring.
  const int parent_id = parent_organism->GetOrgInterface().GetCellID();
  assert(parent_id >= 0 && parent_id < cell_array.GetSize());
  cPopulationCell& parent_cell = cell_array[parent_id];
    
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
      if (m_world->GetConfig().RESET_INPUTS_ON_DIVIDE.Get() > 0) environment.SetupInputs(ctx, parent_cell.m_inputs);
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
      bool org_survived = ActivateOrganism(ctx, offspring_array[i], GetCell(target_cells[i]));
      // only assign an avatar cell if the org lived through birth and it isn't the parent
      if (m_world->GetConfig().USE_AVATARS.Get() && org_survived) {
        int avatar_target_cell = PlaceAvatar(ctx, parent_organism);
        if (avatar_target_cell != -1) {
          offspring_array[i]->GetPhenotype().SetAVBirthCellID(avatar_target_cell);
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
  return parent_alive;
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
  
  
  // Initialize the time-slice for this new organism.
  AdjustSchedule(target_cell, in_organism->GetPhenotype().GetMerit());
  
  // Special handling for certain birth methods.
  if (m_world->GetConfig().BIRTH_METHOD.Get() == POSITION_OFFSPRING_FULL_SOUP_ELDEST) {
    reaper_queue.Push(&target_cell);
  }
  
  // Keep track of statistics for organism counts...
  num_organisms++;
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
    // ft should be nearly always -1 so long as it is not being inherited
    if (in_organism->IsPreyFT()) num_prey_organisms++;
    else if (in_organism->IsTopPredFT()) num_top_pred_organisms++;
    else num_pred_organisms++;
  }
  
  // Statistics...
  m_world->GetStats().RecordBirth(in_organism->GetPhenotype().ParentTrue());
  
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
  
    
    in_organism->GetPhenotype().SetBirthCellID(target_cell.GetID());
    in_organism->GetPhenotype().SetBirthGroupID(op);
    in_organism->GetPhenotype().SetBirthForagerType(in_organism->GetForageTarget());
    Systematics::GenotypePtr genotype;
    genotype.DynamicCastFrom(in_organism->SystematicsGroup("genotype"));
    assert(genotype);    
    
    genotype->SetLastBirthCell(target_cell.GetID());
    genotype->SetLastForagerType(in_organism->GetParentFT());
  }

  bool org_survived = true;
  // For tolerance_window, we cheated by dumping doomed offspring into cell (X * Y) - 1 ...now that we updated the stats, we need to 
  // kill that org. @JJB
  int doomed_cell = (world_x * world_y) - 1;

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

  return org_survived;
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
  
  const int ft = organism->GetForageTarget();

  RemoveLiveOrg(organism);
  
  organism->NotifyDeath(ctx);
  
  // Update count statistics...
  num_organisms--;
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
    if (ft > -2) num_prey_organisms--;
    else if (ft == -2) num_pred_organisms--;
    else num_top_pred_organisms--;
  }
  
  
  
  // And clear it!
  in_cell.RemoveOrganism(ctx); 
  if (!organism->IsRunning()) delete organism;
  else organism->GetPhenotype().SetToDelete();
  
  // Alert the scheduler that this cell has a 0 merit.
  AdjustSchedule(in_cell, cMerit(0));
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
  
  if (birth_method == POSITION_OFFSPRING_PARENT_FACING) {
    return parent_cell.GetCellFaced();
  }
  else if (birth_method == POSITION_OFFSPRING_NEXT_CELL) {
    int out_cell_id = parent_cell.GetID() + 1;
    if (out_cell_id == cell_array.GetSize()) out_cell_id = 0;
    return GetCell(out_cell_id);
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
    cell_idx = m_world->GetRandom().GetUInt(world_size);
    cell_id = cells[cell_idx];
  }
  return cell_id;
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
  
  if (cur_org->GetPhenotype().GetToDelete() == true) delete cur_org;
  
  m_world->GetStats().IncExecuted();
  m_pop_res.Update(step_size);
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
  
  if (cur_org->GetPhenotype().GetToDelete() == true) delete cur_org;
  
  m_world->GetStats().IncExecuted();
  m_pop_res.Update(step_size);
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
    
    stats.SumFitness().Add(cur_fitness);
    stats.SumMerit().Add(cur_merit.GetDouble());
    stats.SumGestation().Add(phenotype.GetGestationTime());
    stats.SumCreatureAge().Add(phenotype.GetAge());
    stats.SumGeneration().Add(phenotype.GetGeneration());
    stats.SumNeutralMetric().Add(phenotype.GetNeutralMetric());
    stats.SumCopyMutRate().Push(organism->MutationRates().GetCopyMutProb());
    stats.SumLogCopyMutRate().Push(log(organism->MutationRates().GetCopyMutProb()));
    stats.SumDivMutRate().Push(organism->MutationRates().GetDivMutProb() / organism->GetPhenotype().GetDivType());
    stats.SumLogDivMutRate().Push(log(organism->MutationRates().GetDivMutProb() /organism->GetPhenotype().GetDivType()));
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
    
    // Increment the counts for all qualities the organism has...
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
  
  m_pop_res.UpdateGlobalResources(ctx);
}





void cPopulation::ProcessPreUpdate()
{
  m_pop_res.SetSpatialUpdate(m_world->GetStats().GetUpdate());
}

void cPopulation::ProcessPostUpdate(cAvidaContext& ctx)
{
  ProcessUpdateCellActions(ctx);
  
  cStats& stats = m_world->GetStats();
  
  stats.SetNumCreatures(GetNumOrganisms());
  
  UpdateOrganismStats(ctx);
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
  sOrgInfo(int c, int o, int in_group, int in_forage, int in_bcell, int in_avcell, int in_av_bcell, int in_parent_ft,
          int in_parent_is_teacher, double in_parent_merit) : 
          cell_id(c), offset(o), curr_group(in_group), curr_forage(in_forage), birth_cell(in_bcell),
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
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, 0, -1, -1, -1, 0, 1));
        } else {
          map_entry = new sGroupInfo(pg, true);
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, 0, -1, -1, -1, 0, 1));
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
            map_entry->orgs.Push(sOrgInfo(cell, offset, -1, -1, 0, -1, -1, -1, 0, 1));
          }
          else if (save_groupings && !save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, curr_group, curr_forage, birth_cell, -1, -1, -1, 0, 1));
          }
          else if (save_groupings && save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, curr_group, curr_forage, birth_cell, avatar_cell, av_bcell, -1, 0, 1));
          }
          else if (!save_groupings && save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, -1, -1, 0, avatar_cell, av_bcell, -1, 0, 1));
          }
        }
        else if (save_rebirth) {
          const int p_ft = org->GetParentFT();
          const int p_teach = (bool) (org->HadParentTeacher());
          const double p_merit = org->GetParentMerit();
          
          map_entry->orgs.Push(sOrgInfo(cell, offset, curr_group, curr_forage, birth_cell, avatar_cell, av_bcell, p_ft, p_teach, p_merit));
        }
      } else {
        map_entry = new sGroupInfo(genotype);
        int curr_group = -1;
        const int curr_forage = org->GetForageTarget();
        const int birth_cell = org->GetPhenotype().GetBirthCell();
        const int avatar_cell = org->GetOrgInterface().GetAVCellID();
        const int av_bcell = org->GetPhenotype().GetAVBirthCell();
        if (!save_rebirth) {
          if (!save_groupings && !save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, -1, -1, 0, -1, -1, -1, 0, 1));
          }
          else if (save_groupings && !save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, curr_group, curr_forage, birth_cell, -1, -1, -1, 0, 1));
          }
          else if (save_groupings && save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, curr_group, curr_forage, birth_cell, avatar_cell, av_bcell, -1, 0, 1));
          }
          else if (!save_groupings && save_avatars) {
            map_entry->orgs.Push(sOrgInfo(cell, offset, -1, -1, 0, avatar_cell, av_bcell, -1, 0, 1));
          }
        }
        else if (save_rebirth) {
          const int p_ft = org->GetParentFT();
          const int p_teach = (bool) (org->HadParentTeacher());
          const double p_merit = org->GetParentMerit();
          map_entry->orgs.Push(sOrgInfo(cell, offset, curr_group, curr_forage, birth_cell, avatar_cell, av_bcell, p_ft, p_teach, p_merit));                  
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
    } else if (save_rebirth) {
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
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, 0, -1, -1, -1, 0, 1));
        } else {
          map_entry = new sGroupInfo(pg, true);
          map_entry->orgs.Push(sOrgInfo(cell, 0, -1, -1, 0, -1, -1, -1, 0, 1));
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

struct sTmpGenotype
{
public:
  int id_num;
  Apto::SmartPtr<Apto::Map<Apto::String, Apto::String> > props;
  
  int num_cpus;
  Apto::Array<int> cells;
  Apto::Array<int> offsets;
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

bool cPopulation::LoadPopulation(const cString& filename, cAvidaContext& ctx, int cellid_offset, int lineage_offset, bool load_groups, bool load_birth_cells, bool load_avatars, bool load_rebirth, bool load_parent_dat)
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
  
  if (some_missing) m_world->GetDriver().Feedback().Warning("Some parents not found in loaded pop file. Defaulting to parent ID of '(none)' for those genomes.");
  
  // Process genotypes, inject into organisms as necessary
  int u_cell_id = 0;
  for (int gen_i = 0; gen_i < genotypes.GetSize(); gen_i++) {
    sTmpGenotype& tmp = genotypes[gen_i];
    // otherwise, we insert as many organisms as we need
    for (int cell_i = 0; cell_i < tmp.num_cpus; cell_i++) {
      int cell_id = 0;
      if (!load_birth_cells && !load_rebirth) cell_id = (structured) ? (tmp.cells[cell_i] + cellid_offset) : (u_cell_id++ + cellid_offset);
      else cell_id = (structured) ? (tmp.birth_cells[cell_i] + cellid_offset) : (u_cell_id++ + cellid_offset);
      
      
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
      
      // Set the phenotype merit from the save file
      assert(tmp.props->Has("merit"));
      double merit = Apto::StrAs(tmp.props->Get("merit"));
      if (load_rebirth && m_world->GetConfig().INHERIT_MERIT.Get() && tmp.props->Has("parent_merit")) { 
        merit = tmp.parent_merit[cell_i]; 
      }
      
      if (merit > 0) {
        phenotype.SetMerit(cMerit(merit));
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
      
      if (org_survived && m_world->GetConfig().USE_AVATARS.Get()) { //**
        int avatar_cell = -1;
        if (tmp.avatar_cells.GetSize() != 0) avatar_cell = tmp.avatar_cells[cell_i];
        if (avatar_cell != -1) {
          new_organism->GetOrgInterface().AddPredPreyAV(ctx, avatar_cell);
          new_organism->GetPhenotype().SetAVBirthCellID(tmp.avatar_cells[cell_i]);
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
 **/

void cPopulation::Inject(const Genome& genome, Systematics::Source src, cAvidaContext& ctx, int cell_id, double merit, double neutral, bool inject_group, int group_id, int forager_type, int trace)
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
  if (!inject_group) InjectGenome(cell_id, src, genome, ctx, true);
  else InjectGenome(cell_id, src, genome, ctx, false);
  
  cPhenotype& phenotype = GetCell(cell_id).GetOrganism()->GetPhenotype();
  phenotype.SetNeutralMetric(neutral);
  
  if (merit > 0) phenotype.SetMerit(cMerit(merit));
  AdjustSchedule(GetCell(cell_id), phenotype.GetMerit());
  
  if (inject_group) {
    cell_array[cell_id].GetOrganism()->SetForageTarget(ctx, forager_type);
    
    cell_array[cell_id].GetOrganism()->GetPhenotype().SetBirthCellID(cell_id);
    cell_array[cell_id].GetOrganism()->GetPhenotype().SetBirthGroupID(group_id);
    cell_array[cell_id].GetOrganism()->GetPhenotype().SetBirthForagerType(forager_type);
  }
  if (m_world->GetConfig().USE_AVATARS.Get()) {
    cell_array[cell_id].GetOrganism()->GetOrgInterface().AddPredPreyAV(cell_id);
  }
}

void cPopulation::InjectGroup(const Genome& genome, Systematics::Source src, cAvidaContext& ctx, int cell_id, double merit, double neutral, int group_id, int forager_type, int trace)
{
  Inject(genome, src, ctx, cell_id, merit, neutral, true, group_id, forager_type, trace);
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
}

void cPopulation::BuildTimeSlicer()
{
  switch (m_world->GetConfig().SLICING_METHOD.Get()) {
    case SLICE_CONSTANT:
      m_scheduler = new Apto::Scheduler::RoundRobin(cell_array.GetSize());
      break;
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


void cPopulation::InjectGenome(int cell_id, Systematics::Source src, const Genome& genome, cAvidaContext& ctx, bool assign_group, Systematics::RoleClassificationHints* hints)
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
  
  Systematics::GenomeTestMetricsPtr metrics = Systematics::GenomeTestMetrics::GetMetrics(m_world, ctx, new_organism->SystematicsGroup("genotype"));
  
  phenotype.SetMerit(cMerit(metrics->GetMerit()));
  
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
  
  // Activate the organism in the population...
  if (assign_group) ActivateOrganism(ctx, new_organism, cell_array[cell_id], true, true);
  else ActivateOrganism(ctx, new_organism, cell_array[cell_id], false, true);
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


bool cPopulation::UpdateMerit(int cell_id, double new_merit)
{
  assert( GetCell(cell_id).IsOccupied() == true);
  assert( new_merit >= 0.0 );
  
  cPhenotype & phenotype = GetCell(cell_id).GetOrganism()->GetPhenotype();
  double old_merit = phenotype.GetMerit().GetDouble();
  
  phenotype.SetMerit( cMerit(new_merit) );
  phenotype.SetLifeFitness(new_merit/phenotype.GetGestationTime());
  AdjustSchedule(GetCell(cell_id), phenotype.GetMerit());
  
  return true;
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
      avatar_target_cell = (world_x * world_y) * 0.5;
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
