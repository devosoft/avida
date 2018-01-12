/*
 *  cDeme.cc
 *  Avida
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#include "cDeme.h"

#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Manager.h"

#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cResource.h"
#include "cStats.h"
#include "cWorld.h"
#include "cOrgMessagePredicate.h"
#include "cOrgMovementPredicate.h"
#include "cDemePredicate.h"
#include "cReactionResult.h" //@JJB**
#include "cTaskState.h" //@JJB**

#include <cmath>


/*! Constructor
 */
cDeme::cDeme()
  : _id(0)
  , width(0)
  , replicateDeme(false)
  , treatable(false)
  , cur_birth_count(0)
  , last_birth_count(0)
  , cur_org_count(0)
  , last_org_count(0)
  , injected_count(0)
  , birth_count_perslot(0)
  , _age(0)
  , generation(0)
  , total_org_energy(0.0)
  , time_used(0)
  , gestation_time(0)
  , cur_normalized_time_used(0.0)
  , last_normalized_time_used(0.0)
  , MSG_sendFailed(0)
  , MSG_dropped(0)
  , MSG_SuccessfullySent(0)
  , energyInjectedIntoOrganisms(0.0)
  , energyRemainingInDemeAtReplication(0.0)
  , total_energy_testament(0.0)
  , eventsTotal(0)
  , eventsKilled(0)
  , eventsKilledThisSlot(0)
  , eventKillAttempts(0)
  , eventKillAttemptsThisSlot(0)
  , consecutiveSuccessfulEventPeriods(0)
  , sleeping_count(0)
  , avg_founder_generation(0.0)
  , generations_per_lifetime(0.0)
  , deme_resource_count(0)
  , m_germline_genotype_id(0)
  , points(0)
  , migrations_out(0)
  , migrations_in(0)
  , suicides(0)
  , m_network(0)
  , m_input_pointer(0)
  , m_input_buf(0)
  , m_output_buf(0)
  , m_reaction_result(NULL)
  , m_num_reproductives(0)
{
}

cDeme::~cDeme()
{
  if(m_network) delete m_network;

  // Remove Task States
  for (Apto::Map<void*, cTaskState*>::ValueIterator it = m_task_states.Values(); it.Next();) delete *it.Get();
  delete m_reaction_result;
}

cDeme& cDeme::operator=(const cDeme& in_deme)
{
  m_world                             = in_deme.m_world;
  _id                                 = in_deme._id;
  cell_ids                            = in_deme.cell_ids;
  width                               = in_deme.width;
  replicateDeme                       = in_deme.replicateDeme;
  treatable                           = in_deme.treatable;
  treatment_ages                      = in_deme.treatment_ages;
  cur_birth_count                     = in_deme.cur_birth_count;
  last_birth_count                    = in_deme.last_birth_count;
  cur_org_count                       = in_deme.cur_org_count;
  last_org_count                      = in_deme.last_org_count;
  injected_count                      = in_deme.injected_count;
  birth_count_perslot                 = in_deme.birth_count_perslot;
  _age                                = in_deme._age;
  generation                          = in_deme.generation;
  total_org_energy                    = in_deme.total_org_energy;
  time_used                           = in_deme.time_used;
  gestation_time                      = in_deme.gestation_time;
  cur_normalized_time_used            = in_deme.cur_normalized_time_used;
  last_normalized_time_used           = in_deme.last_normalized_time_used;
  MSG_sendFailed                      = in_deme.MSG_sendFailed;
  MSG_dropped                         = in_deme.MSG_dropped;
  MSG_SuccessfullySent                = in_deme.MSG_SuccessfullySent;
  energyInjectedIntoOrganisms         = in_deme.energyInjectedIntoOrganisms;
  energyRemainingInDemeAtReplication  = in_deme.energyRemainingInDemeAtReplication;
  total_energy_testament              = in_deme.total_energy_testament;
  eventsTotal                         = in_deme.eventsTotal;
  eventsKilled                        = in_deme.eventsKilled;
  eventsKilledThisSlot                = in_deme.eventsKilledThisSlot;
  eventKillAttempts                   = in_deme.eventKillAttempts;
  eventKillAttemptsThisSlot           = in_deme.eventKillAttemptsThisSlot;
  consecutiveSuccessfulEventPeriods   = in_deme.consecutiveSuccessfulEventPeriods;
  sleeping_count                      = in_deme.sleeping_count;
  energyUsage                         = in_deme.energyUsage;
  total_energy_donated                = in_deme.total_energy_donated;
  total_energy_received               = in_deme.total_energy_received;
  total_energy_applied                = in_deme.total_energy_applied;
  cur_task_exe_count                  = in_deme.cur_task_exe_count;
  cur_reaction_count                  = in_deme.cur_reaction_count;
  last_task_exe_count                 = in_deme.last_task_exe_count;
  last_reaction_count                 = in_deme.last_reaction_count;
  cur_org_task_count                  = in_deme.cur_org_task_count;
  cur_org_task_exe_count              = in_deme.cur_org_task_exe_count;
  cur_org_reaction_count              = in_deme.cur_org_reaction_count;
  last_org_task_count                 = in_deme.last_org_task_count;
  last_org_task_exe_count             = in_deme.last_org_task_exe_count;
  last_org_reaction_count             = in_deme.last_org_reaction_count;
  avg_founder_generation              = in_deme.avg_founder_generation;
  generations_per_lifetime            = in_deme.generations_per_lifetime;
  _germline                           = in_deme._germline;
  cell_events                         = in_deme.cell_events;
  event_slot_end_points               = in_deme.event_slot_end_points;
  m_germline_genotype_id              = in_deme.m_germline_genotype_id;
  m_founder_genotype_ids              = in_deme.m_founder_genotype_ids;
  m_founder_phenotypes                = in_deme.m_founder_phenotypes;
  _current_merit                      = in_deme._current_merit;
  _next_merit                         = in_deme._next_merit;
  deme_pred_list                      = in_deme.deme_pred_list;
  message_pred_list                   = in_deme.message_pred_list;
  movement_pred_list                  = in_deme.movement_pred_list;
  points                              = in_deme.points;
  migrations_out                      = in_deme.migrations_out;
  migrations_in                       = in_deme.migrations_in;
  suicides                            = in_deme.suicides;
//m_network                           = in_deme.m_network;
  m_inputs                            = in_deme.m_inputs;
  m_input_buf                         = in_deme.m_input_buf;
  m_output_buf                        = in_deme.m_output_buf;
//m_reaction_result                   = in_deme.m_reaction_result;
  m_task_count                        = in_deme.m_task_count;
  m_reaction_count                    = in_deme.m_reaction_count;
  m_last_task_count                   = in_deme.m_last_task_count;
  m_cur_reaction_add_reward           = in_deme.m_cur_reaction_add_reward;
  m_cur_bonus                         = in_deme.m_cur_bonus;
  m_cur_merit                         = in_deme.m_cur_merit;
  m_total_res_consumed                = in_deme.m_total_res_consumed;
  m_switch_penalties                  = in_deme.m_switch_penalties;
  m_shannon_matrix                    = in_deme.m_shannon_matrix;
  m_num_active                        = in_deme.m_num_active;
  m_num_reproductives                 = in_deme.m_num_reproductives;


  for (Apto::Map<void*, cTaskState*>::ConstIterator it = in_deme.m_task_states.Begin(); it.Next();) {
    cTaskState* new_ts = new cTaskState(**(it.Get()->Value2()));
    m_task_states.Set(it.Get()->Value1(), new_ts);
  }
  
  return *this;
}

void cDeme::Setup(int id, const Apto::Array<int> & in_cells, int in_width, cWorld* world)
{
  _id = id;
  cell_ids = in_cells;
  cur_birth_count = 0;
  last_birth_count = 0;
  cur_org_count = 0;
  last_org_count = 0;
  birth_count_perslot = 0;
  m_world = world;

  replicateDeme = false;

  _current_merit = 1.0;
  _next_merit = 1.0;
  
  const int num_tasks = m_world->GetEnvironment().GetNumTasks();
  const int num_reactions = m_world->GetEnvironment().GetNumReactions();  
  
  cur_task_exe_count.Resize(num_tasks);
  cur_task_exe_count.SetAll(0);
  cur_reaction_count.ResizeClear(num_reactions);
  cur_reaction_count.SetAll(0);
  last_task_exe_count.ResizeClear(num_tasks);
  last_task_exe_count.SetAll(0);
  last_reaction_count.ResizeClear(num_reactions);
  last_reaction_count.SetAll(0);
  
  cur_org_task_count.Resize(num_tasks);
  cur_org_task_count.SetAll(0);
  cur_org_task_exe_count.Resize(num_tasks);
  cur_org_task_exe_count.SetAll(0);
  cur_org_reaction_count.ResizeClear(num_reactions);
  cur_org_reaction_count.SetAll(0);
  last_org_task_count.ResizeClear(num_tasks);
  last_org_task_count.SetAll(0);
  last_org_task_exe_count.ResizeClear(num_tasks);
  last_org_task_exe_count.SetAll(0);
  last_org_reaction_count.ResizeClear(num_reactions);
  last_org_reaction_count.SetAll(0);
  m_total_res_consumed = 0;
  m_switch_penalties = 0;
  m_num_active = 0;
  m_num_reproductives = 0;

  m_input_buf = tBuffer<int>(m_world->GetEnvironment().GetInputSize()); //@JJB**
  m_output_buf = tBuffer<int>(m_world->GetEnvironment().GetOutputSize()); //@JJB**
  m_task_count.ResizeClear(num_tasks); //@JJB**
  m_task_count.SetAll(0);
  m_reaction_count.ResizeClear(m_world->GetEnvironment().GetReactionLib().GetSize()); //@JJB**
  m_reaction_count.SetAll(0);
  m_last_task_count.ResizeClear(num_tasks); //@JJB**
  m_last_task_count.SetAll(0);
  m_cur_reaction_add_reward.ResizeClear(m_world->GetEnvironment().GetReactionLib().GetSize()); //@JJB**
  m_cur_reaction_add_reward.SetAll(0.0);
  m_cur_bonus = m_world->GetConfig().DEFAULT_BONUS.Get(); //@JJB**
  if (m_world->GetConfig().BASE_MERIT_METHOD.Get() == BASE_MERIT_CONST) {
    m_cur_merit = m_world->GetConfig().BASE_CONST_MERIT.Get();
  } else {
    m_cur_merit = 1.0;
  }
  
  total_energy_donated = 0.0;
  total_energy_received = 0.0;
  total_energy_applied = 0.0;
  
  // If width is negative, set it to the full number of cells.
  width = in_width;
  if (width < 1) width = cell_ids.GetSize();
  
  // drain spacial energy resources and place energy in cells
}


int cDeme::GetCellID(int x, int y) const
{
  assert(x >= 0 && x < GetWidth());
  assert(y >= 0 && y < GetHeight());
  
  const int pos = y * width + x;
  return cell_ids[pos];
}


/*! Note that for this method to work, we blatantly assume that IDs are in
 monotonically increasing order!! */
std::pair<int, int> cDeme::GetCellPosition(int cellid) const 
{
  assert(cell_ids.GetSize()>0);
  assert(GetWidth() > 0);
  //  cellid -= cell_ids[0];
  //  return std::make_pair(cellid % GetWidth(), cellid / GetWidth());
  return std::make_pair(cellid % GetWidth(), ( cellid % cell_ids.GetSize() ) / GetWidth());
}

cPopulationCell& cDeme::GetCell(int pos) const
{
  return m_world->GetPopulation().GetCell(cell_ids[pos]);
}

cPopulationCell& cDeme::GetCell(int x, int y) const
{
	return m_world->GetPopulation().GetCell(GetCellID(x,y));
}


cOrganism* cDeme::GetOrganism(int pos) const
{
  return GetCell(pos).GetOrganism();
}

std::vector<int> cDeme::GetGenotypeIDs()
{
  std::vector<int> genotype_ids;
  for (int i = 0; i < GetSize(); i++) {
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied()) genotype_ids.push_back(cell.GetOrganism()->SystematicsGroup("genotype")->ID());
  }

  //assert(genotype_ids.size()>0); // How did we get to replication otherwise?
  //@JEB some germline methods can result in empty source demes if they didn't produce a germ)
  
  return genotype_ids;
}



int cDeme::GetNumOrgsWithOpinion() const
{
  int demeSize = GetSize();
  int count = 0;
  
  for (int pos = 0; pos < demeSize; ++pos) {
    cPopulationCell& cell = GetCell(pos);
    if (cell.IsOccupied() && cell.GetOrganism()->HasOpinion())
      ++count;
  }

  return count;
}

void cDeme::ProcessPreUpdate()
{
  deme_resource_count.SetSpatialUpdate(m_world->GetStats().GetUpdate());
}

void cDeme::ProcessUpdate(cAvidaContext& ctx)
{
  // test deme predicate
  for (int i = 0; i < deme_pred_list.GetSize(); i++) {
    if (deme_pred_list[i]->GetName() == "cDemeResourceThreshold") {
      (*deme_pred_list[i])(ctx, &deme_resource_count);
    }
  }

  energyUsage.Clear();
  
  if(IsEmpty()) {  // deme is not processed if no organisms are present
    total_energy_testament = 0.0;
    return;
  }
  
  if (m_world->GetConfig().ENERGY_ENABLED.Get()) {
    for(int i = 0; i < GetSize(); i++) {
      cPopulationCell& cell = GetCell(i);
      if(cell.IsOccupied()) {
        energyUsage.Add(cell.GetOrganism()->GetPhenotype().GetEnergyUsageRatio());
      }
    }
  }
  
  for(int i = 0; i < cell_events.GetSize(); i++) {
    cDemeCellEvent& event = cell_events[i];
    
    if(event.IsActive() && event.GetDelay() < _age && _age <= event.GetDelay()+event.GetDuration()) {
      //remove energy from cells  (should be done with outflow, but this will work for now)
      int eventCell = event.GetNextEventCellID();
      cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource("CELL_ENERGY");
      
      while(eventCell != -1) {
        cPopulationCell& cell = m_world->GetPopulation().GetCell(GetCellID(eventCell));
        if(event.GetEventID() == cell.GetCellData()){
          if(cell.IsOccupied()) {
            // remove energy from organism
            cPhenotype& orgPhenotype = cell.GetOrganism()->GetPhenotype();
            orgPhenotype.ReduceEnergy(orgPhenotype.GetStoredEnergy()*m_world->GetConfig().ATTACK_DECAY_RATE.Get());
          }
          //remove energy from cell... organism might not takeup all of a cell's energy
          Apto::Array<double> cell_resources = deme_resource_count.GetCellResources(eventCell, ctx);  // uses global cell_id; is this a problem
          cell_resources[res->GetID()] *= m_world->GetConfig().ATTACK_DECAY_RATE.Get();
          deme_resource_count.ModifyCell(ctx, cell_resources, eventCell);
        }
        eventCell = event.GetNextEventCellID();
      }
    }
    
    
    
    if(!event.IsActive() && event.GetDelay() == _age) {
      eventsTotal++;
      event.ActivateEvent(); //start event
      int eventCell = event.GetNextEventCellID();
      while(eventCell != -1) {
        // place event ID in cells' data
        if(event.IsDecayed()) {
          m_world->GetPopulation().GetCell(GetCellID(eventCell)).SetCellData(event.GetEventIDDecay(GetCellPosition(eventCell)));
        } else {
          m_world->GetPopulation().GetCell(GetCellID(eventCell)).SetCellData(event.GetEventID());
        }
        
        // record activation of each cell in stats
//        std::pair<int, int> pos = GetCellPosition(eventCell);
//        m_world->GetStats().IncEventCount(pos.first, pos.second);
        
        
        //TODO // increase outflow of energy from these cells if not event currently present
        
        
        eventCell = event.GetNextEventCellID();
      }
    } else if (event.IsActive() && event.GetDelay()+event.GetDuration() == _age) {
      int eventCell = event.GetNextEventCellID();
      while (eventCell != -1) {
        if (event.GetEventID() == m_world->GetPopulation().GetCell(GetCellID(eventCell)).GetCellData()) { // eventID == CellData
          //set cell data to 0
          m_world->GetPopulation().GetCell(GetCellID(eventCell)).SetCellData(0);
          
          //  TODO // remove energy outflow from these cells
          
        }
        eventCell = event.GetNextEventCellID();
      }
      event.DeactivateEvent();  //event over
    }
  }
  
  for (vector<pair<int, int> >::iterator iter = event_slot_end_points.begin();
       iter < event_slot_end_points.end();
       iter++) {
    if (_age == (*iter).first) {
      // at end point              
      if (GetEventsKilledThisSlot() >=
	 m_world->GetConfig().DEMES_MIM_EVENTS_KILLED_RATIO.Get() * (*iter).second) {
        consecutiveSuccessfulEventPeriods++;
      } else {
        consecutiveSuccessfulEventPeriods = 0;
      }
      
      // update stats.flow_rate_tuples
      std::map<int, flow_rate_tuple>& flowRateTuples = m_world->GetStats().FlowRateTuples();
      
      flowRateTuples[(*iter).second].orgCount.Add(GetOrgCount());
      flowRateTuples[(*iter).second].eventsKilled.Add(GetEventsKilledThisSlot());
      flowRateTuples[(*iter).second].attemptsToKillEvents.Add(GetEventKillAttemptsThisSlot());
      flowRateTuples[(*iter).second].AvgEnergyUsageRatio.Add(energyUsage.Average());
      flowRateTuples[(*iter).second].totalBirths.Add(birth_count_perslot);
      flowRateTuples[(*iter).second].currentSleeping.Add(sleeping_count);
      birth_count_perslot = 0;
      eventsKilledThisSlot = 0;
      eventKillAttemptsThisSlot = 0;
      break;
    }
  }
  ++_age;
	
  // Let the network process the update too, if we have one.
  if (IsNetworkInitialized()) m_network->ProcessUpdate();
}


/*! Called when an organism living in a cell in this deme is about to be killed.
 
 This method is called from cPopulation::KillOrganism().
 */

void cDeme::OrganismDeath(cPopulationCell& cell)
{
  // Clean up this deme's network, if we have one.
  if (IsNetworkInitialized()) m_network->OrganismDeath(cell);
	
  // Add information about the organisms tasks to the deme so that
  // we can use it to compute shannon mutual information and measure
  // division of labor.
  if (m_world->GetConfig().DEMES_TRACK_SHANNON_INFO.Get()) UpdateShannon(cell);
}



void cDeme::Reset(cAvidaContext& ctx, bool resetResources, double deme_energy)
{
  double additional_resource = 0.0;
  // Handle energy model
  if (m_world->GetConfig().ENERGY_ENABLED.Get())
  {
    total_energy_testament = 0.0;
    
    if(m_world->GetConfig().ENERGY_PASSED_ON_DEME_REPLICATION_METHOD.Get() == 1) {
      // split deme energy evenly between cell in deme
      additional_resource = deme_energy;  // spacial resource handles resource division
    }
  }
  
  // Handle stat and resource resets
  _age = 0;
  time_used = 0;
  cur_birth_count = 0;
  cur_normalized_time_used = 0;
  injected_count = 0;
  birth_count_perslot = 0;
  eventsTotal = 0;
  eventsKilled = 0;
  eventsKilledThisSlot = 0;
  eventKillAttempts = 0;
  eventKillAttemptsThisSlot = 0;
  sleeping_count = 0;
  MSG_sendFailed = 0;
  MSG_dropped = 0;
  MSG_SuccessfullySent = 0;
  m_total_res_consumed = 0;
  m_switch_penalties = 0;
  m_num_active = 0;
  m_shannon_matrix.clear();
  m_num_reproductives = 0;

  m_input_pointer = 0; //@JJB**
  m_input_buf.Clear(); //@JJB**
  m_output_buf.Clear(); //@JJB**
  m_last_task_count = m_task_count; //@JJB**
  m_task_count.SetAll(0); //@JJB**
  m_reaction_count.SetAll(0); //@JJB**
  m_cur_reaction_add_reward.SetAll(0.0); //@JJB**
  m_cur_bonus = m_world->GetConfig().DEFAULT_BONUS.Get(); //@JJB**
  if (m_world->GetConfig().BASE_MERIT_METHOD.Get() == BASE_MERIT_CONST) {
    m_cur_merit = m_world->GetConfig().BASE_CONST_MERIT.Get();
  } else {
    m_cur_merit = 1.0;
  }

  // Reset Task States
  for (Apto::Map<void*, cTaskState*>::ValueIterator it = m_task_states.Values(); it.Next();) delete *it.Get();
  m_task_states.Clear();
  
  consecutiveSuccessfulEventPeriods = 0;
  
  replicateDeme = false;
  
  total_energy_donated = 0.0;
  total_energy_received = 0.0;
  total_energy_applied = 0.0;
	
  cur_task_exe_count.SetAll(0);
  cur_reaction_count.SetAll(0);
  
  //reset remaining deme predicates
  for (int i = 0; i < deme_pred_list.GetSize(); i++) {
    deme_pred_list[i]->Reset();
  }	
  //reset remaining message predicates
  for (int i = 0; i < message_pred_list.GetSize(); i++) {
    message_pred_list[i]->Reset();
  }
  //reset remaining message predicates
  for (int i = 0; i < movement_pred_list.GetSize(); i++) {
    movement_pred_list[i]->Reset();
  }
  
  if (resetResources) {
    deme_resource_count.ReinitializeResources(ctx, additional_resource);
  }

  // Instead of polluting cDemeNetwork with Resets, we're just going to delete it,
  // and go ahead and rely on lazy initialization to fill this back in.
  if (m_network) {
    delete m_network;
    m_network = 0;
  }
}


void cDeme::DivideReset(cAvidaContext& ctx, cDeme& parent_deme, bool resetResources, double deme_energy)
{
  // the parent might be us, so save this value...
  double old_avg_founder_generation = parent_deme.GetAvgFounderGeneration();
  
  // update our average founder generation
  cDoubleSum gen;  
  for (int i=0; i< m_founder_phenotypes.GetSize(); i++) {
    gen.Add( m_founder_phenotypes[i].GetGeneration() );
  }
  avg_founder_generation = gen.Average();
  
  // update our generations per lifetime based on current founders and parents generation
  generations_per_lifetime = avg_founder_generation - old_avg_founder_generation;
  
  //Save statistics according to parent before reset.
  generation = parent_deme.GetGeneration() + 1;
  gestation_time = parent_deme.GetTimeUsed();
  last_normalized_time_used = parent_deme.GetNormalizedTimeUsed();
  
  last_task_exe_count = parent_deme.GetCurTaskExeCount();
  last_reaction_count = parent_deme.GetCurReactionCount();
  
  last_org_task_count = parent_deme.GetCurOrgTaskCount();
  last_org_task_exe_count = parent_deme.GetCurOrgTaskExeCount();
  last_org_reaction_count = parent_deme.GetCurOrgReactionCount();
  
  last_org_count = parent_deme.GetLastOrgCount(); // Org count was updated upon KillAll()....
  last_birth_count = parent_deme.GetBirthCount();

  Reset(ctx, resetResources, deme_energy);
}


// Given the input deme founders and original ones,
// calculate how many generations this deme went through to divide.
void cDeme::UpdateGenerationsPerLifetime(double old_avg_founder_generation, Apto::Array<cPhenotype>& new_founder_phenotypes)
{ 
  cDoubleSum gen;
  for (int i=0; i< new_founder_phenotypes.GetSize(); i++) {
    gen.Add( new_founder_phenotypes[i].GetGeneration() );
  }
  double new_avg_founder_generation = gen.Average();
  generations_per_lifetime = new_avg_founder_generation - old_avg_founder_generation;
}

/*! Check every cell in this deme for a living organism.  If found, kill it. */
void cDeme::KillAll(cAvidaContext& ctx) 
{
  last_org_count = GetOrgCount();
  for (int i=0; i<GetSize(); ++i) {
    cPopulationCell& cell = m_world->GetPopulation().GetCell(cell_ids[i]);
    if(cell.IsOccupied()) {
      m_world->GetPopulation().KillOrganism(cell, ctx); 
    }
  }

  // HACK: organism are killed after DivideReset is called.
  // need clear a deme before it is reset.
  sleeping_count = 0;  
}

void cDeme::UpdateStats()
{
  //save stats about what tasks our orgs were doing
  //usually called before KillAll
  
  for (int j = 0; j < cur_org_task_count.GetSize(); j++) {
    int count = 0;
    for (int k=0; k<GetSize(); k++) {
      int cellid = GetCellID(k);
      if(m_world->GetPopulation().GetCell(cellid).IsOccupied()) {
        count += (m_world->GetPopulation().GetCell(cellid).GetOrganism()->GetPhenotype().GetLastTaskCount()[j] > 0);
      }
      cur_org_task_count[j] = count; 
    }
  }
  
  for(int j = 0; j < cur_org_task_exe_count.GetSize(); j++) {
    int count = 0;
    for(int k=0; k<GetSize(); k++) {
      int cellid = GetCellID(k);
      if(m_world->GetPopulation().GetCell(cellid).IsOccupied()) {
        count += m_world->GetPopulation().GetCell(cellid).GetOrganism()->GetPhenotype().GetLastTaskCount()[j];
      }
      cur_org_task_exe_count[j] = count; 
    }
  }
  
  for(int j = 0; j < cur_org_reaction_count.GetSize(); j++) {
    int count = 0;
    for(int k=0; k<GetSize(); k++) {
      int cellid = GetCellID(k);
      if(m_world->GetPopulation().GetCell(cellid).IsOccupied()) {
        count += m_world->GetPopulation().GetCell(cellid).GetOrganism()->GetPhenotype().GetLastReactionCount()[j];
      }
      cur_org_reaction_count[j] = count; 
    }
  }
}


/*! Replacing this deme's germline has the effect of changing the deme's lineage.
 There's still some work to do here; the lineage labels of the Genomes in the germline
 are all messed up.
 */
void cDeme::ReplaceGermline(const cGermline& germline) {
  _germline = germline;
}


/*! If this method is called, this is the "parent" deme.  As with individuals,
 we need to rotate the heritable merit to the current merit.
 */
void cDeme::UpdateDemeMerit() {
  assert(_next_merit.GetDouble()>=1.0);
  _current_merit = _next_merit;
  _next_merit = 1.0;
}


/*! Update this deme's merit from the given source.
 */
void cDeme::UpdateDemeMerit(cDeme& source) {
  _current_merit = source.GetHeritableDemeMerit();
  _next_merit = 1.0;
  assert(_current_merit.GetDouble()>=1.0);
}


void cDeme::ModifyDemeResCount(cAvidaContext& ctx, const Apto::Array<double>& res_change, const int absolute_cell_id) {
  // find relative cell_id in deme resource count
  const int relative_cell_id = GetRelativeCellID(absolute_cell_id);
  deme_resource_count.ModifyCell(ctx, res_change, relative_cell_id);
}

void cDeme::SetupDemeRes(int id, cResource * res, int verbosity, cWorld* world) {               
  const double decay = 1.0 - res->GetOutflow();
  //addjust the resources cell list pointer here if we want CELL env. commands to be replicated in each deme
  
  int* temp = &id;
  
  deme_resource_count.Setup(world, *temp, res->GetName(), res->GetInitial(),                
                            res->GetInflow(), decay,
                            res->GetGeometry(), res->GetXDiffuse(),
                            res->GetXGravity(), res->GetYDiffuse(), 
                            res->GetYGravity(), res->GetInflowX1(), 
                            res->GetInflowX2(), res->GetInflowY1(), 
                            res->GetInflowY2(), res->GetOutflowX1(), 
                            res->GetOutflowX2(), res->GetOutflowY1(), 
                            res->GetOutflowY2(), res->GetCellListPtr(),
                            res->GetCellIdListPtr(), verbosity,
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
                            res->GetMaxX(), res->GetMaxY(), res->GetMinX(), res->GetMinY(), 
                            res->GetAscaler(), res->GetUpdateStep(),
                            res->GetHalo(), res->GetHaloInnerRadius(), res->GetHaloWidth(),
                            res->GetHaloAnchorX(), res->GetHaloAnchorY(), res->GetMoveSpeed(), res->GetMoveResistance(),
                            res->GetPlateauInflow(), res->GetPlateauOutflow(), res->GetConeInflow(), res->GetConeOutflow(),                           
                            res->GetGradientInflow(), res->GetIsPlateauCommon(), res->GetFloor(), res->GetHabitat(), 
                            res->GetMinSize(), res->GetMaxSize(), res->GetConfig(), res->GetCount(), res->GetResistance(), res->GetDamage(),
                            res->GetDeathOdds(), res->IsPath(), res->IsHammer(), res->GetInitialPlatVal(), res->GetThreshold(), res->GetRefuge(), res->GetGradient()
                            ); 
  
  if(res->GetEnergyResource()) {
    energy_res_ids.Push(id);
  }
}

double cDeme::GetCellEnergy(int absolute_cell_id, cAvidaContext& ctx) const
{
  assert(cell_ids[0] <= absolute_cell_id);
  assert(absolute_cell_id <= cell_ids[cell_ids.GetSize()-1]);

  double total_energy = 0.0;
  int relative_cell_id = GetRelativeCellID(absolute_cell_id);
  Apto::Array<double> cell_resources = deme_resource_count.GetCellResources(relative_cell_id, ctx);
  
  // sum all energy resources
  for (int i = 0; i < energy_res_ids.GetSize(); i++) {
    if (cell_resources[energy_res_ids[i]] > 0.0) {
      total_energy += cell_resources[energy_res_ids[i]];
      cell_resources[energy_res_ids[i]] *= -1.0;
    }
  }

  return total_energy;
}

double cDeme::GetAndClearCellEnergy(int absolute_cell_id, cAvidaContext& ctx) 
{
  assert(cell_ids[0] <= absolute_cell_id);
  assert(absolute_cell_id <= cell_ids[cell_ids.GetSize()-1]);
  
  double total_energy = 0.0;
  int relative_cell_id = GetRelativeCellID(absolute_cell_id);
  Apto::Array<double> cell_resources = deme_resource_count.GetCellResources(relative_cell_id, ctx);
  
  // sum all energy resources
  for (int i = 0; i < energy_res_ids.GetSize(); i++) {
    if (cell_resources[energy_res_ids[i]] > 0.0) {
      total_energy += cell_resources[energy_res_ids[i]];
      cell_resources[energy_res_ids[i]] *= -1.0;
    }
  }

  // set energy resources to zero
  deme_resource_count.ModifyCell(ctx, cell_resources, relative_cell_id);

  return total_energy;
}

void cDeme::GiveBackCellEnergy(int absolute_cell_id, double value, cAvidaContext& ctx) 
{
  assert(cell_ids[0] <= absolute_cell_id);
  assert(absolute_cell_id <= cell_ids[cell_ids.GetSize()-1]);
  
  int relative_cell_id = GetRelativeCellID(absolute_cell_id);
  Apto::Array<double> cell_resources = deme_resource_count.GetCellResources(relative_cell_id, ctx);
  
  double amount_per_resource = value / energy_res_ids.GetSize();
  
  // put back energy resources evenly
  for(int i = 0; i < energy_res_ids.GetSize(); i++) {
    cell_resources[energy_res_ids[i]] += amount_per_resource;
  }
  deme_resource_count.ModifyCell(ctx, cell_resources, relative_cell_id);
}

void cDeme::SetCellEvent(int x1, int y1, int x2, int y2,
			 int delay, int duration, bool static_position, int total_events)
{
  for (int i = 0; i < total_events; i++) {
    cDemeCellEvent demeEvent = cDemeCellEvent(x1, y1, x2, y2, delay, duration, width, GetHeight(), static_position, this, m_world);
    cell_events.Push(demeEvent);
  }
}

/*void cDeme::SetCellEventGradient(int x1, int y1, int x2, int y2, int delay, int duration, bool static_pos, int time_to_live) {
 cDemeCellEvent demeEvent = cDemeCellEvent(x1, y1, x2, y2, delay, duration, width, GetHeight(), static_pos, time_to_live, this);
 demeEvent.DecayEventIDFromCenter();
 cell_events.Add(demeEvent);
 }*/

int cDeme::GetNumEvents()
{
  return cell_events.GetSize();
}

void cDeme::SetCellEventSlots(int x1, int y1, int x2, int y2, int delay, int duration, 
                              bool static_position, int m_total_slots, int m_total_events_per_slot_max, 
                              int m_total_events_per_slot_min, int m_tolal_event_flow_levels) {
  assert(cell_events.GetSize() == 0); // not designed to be used with other cell events
  assert(m_world->GetConfig().DEMES_MAX_AGE.Get() >= m_total_slots);
  
  int flow_level_increment = (m_total_events_per_slot_max - m_total_events_per_slot_min) / (m_tolal_event_flow_levels-1);
  int slot_length = m_world->GetConfig().DEMES_MAX_AGE.Get() / m_total_slots;
  
  // setup stats tuples
  
  for (int i = 0; i < m_total_slots; i++) {
    int slot_flow_level = flow_level_increment * m_world->GetRandom().GetInt(m_tolal_event_flow_levels) + m_total_events_per_slot_min; // number of event during this slot
    int slot_delay = i * slot_length;
    event_slot_end_points.push_back(make_pair(slot_delay+slot_length, slot_flow_level)); // last slot is never reached it is == to MAX_AGE
    
    for (int k = 0; k < slot_flow_level; k++) {
      cDemeCellEvent demeEvent = cDemeCellEvent(x1, y1, x2, y2, delay, duration, width, GetHeight(), static_position, this, m_world);
      demeEvent.ConfineToTimeSlot(slot_delay, slot_delay+slot_length);
      cell_events.Push(demeEvent);
    }
  }
  
  // setup stats.flow_rate_tuples
  std::map<int, flow_rate_tuple>& flowRateTuples = m_world->GetStats().FlowRateTuples();
  
  for (int i = m_total_events_per_slot_min; i <= m_total_events_per_slot_max; i+=flow_level_increment) {
    flowRateTuples[i].orgCount.Clear();
    flowRateTuples[i].eventsKilled.Clear();
    flowRateTuples[i].attemptsToKillEvents.Clear();
    flowRateTuples[i].AvgEnergyUsageRatio.Clear();
    flowRateTuples[i].totalBirths.Clear();
    flowRateTuples[i].currentSleeping.Clear();
  }
}

bool cDeme::KillCellEvent(const int eventID)
{
  eventKillAttemptsThisSlot++;
  
  if (eventID <= 0) return false;
  for( int i = 0; i < cell_events.GetSize(); i++) {
    cDemeCellEvent& event = cell_events[i];
    if(event.IsActive() && event.GetEventID() == eventID) {
      // remove event ID from all cells
      int eventCell = event.GetNextEventCellID();
      while(eventCell != -1) {
        if(event.GetEventID() == m_world->GetPopulation().GetCell(GetCellID(eventCell)).GetCellData()) { // eventID == CellData
          //set cell data to 0
          m_world->GetPopulation().GetCell(GetCellID(eventCell)).SetCellData(0);
          
          //  TODO // remove energy outflow from these cells
          
        }
        eventCell = event.GetNextEventCellID();
      }
      event.DeactivateEvent();  //event over
      eventsKilled++;
      eventsKilledThisSlot++;
      eventKillAttempts++;
      return true;
    }
  }
  return false;
}

double cDeme::CalculateTotalEnergy(cAvidaContext& ctx) const
{
  assert(m_world->GetConfig().ENERGY_ENABLED.Get());
  
  double energy_sum = 0.0;
  for (int i=0; i<GetSize(); i++) {
    int cellid = GetCellID(i);
    cPopulationCell& cell = m_world->GetPopulation().GetCell(cellid);
    if(cell.IsOccupied()) {
      cOrganism* organism = cell.GetOrganism();
      cPhenotype& phenotype = organism->GetPhenotype();
      energy_sum += phenotype.GetStoredEnergy();
    } else {
      double energy_in_cell = GetCellEnergy(cellid, ctx);
      energy_sum += energy_in_cell * m_world->GetConfig().FRAC_ENERGY_TRANSFER.Get();
    }
  }
  return energy_sum;
}

double cDeme::CalculateTotalInitialEnergyResources() const
{
  assert(m_world->GetConfig().ENERGY_ENABLED.Get());
  
  double energy_sum = 0.0;
  for(int i = 0; i < energy_res_ids.GetSize(); i++) {
    energy_sum += deme_resource_count.GetInitialResourceValue(i);
  }
  return energy_sum;
}


// --- Founder list management --- //

void cDeme::AddFounder(Systematics::GroupPtr bg, cPhenotype * _in_phenotype)
{
  // save genotype id
  m_founder_genotype_ids.Push( bg->ID() );
  cPhenotype phenotype;
  if (_in_phenotype) phenotype = *_in_phenotype;
  m_founder_phenotypes.Push( phenotype );
  
  bg->AddPassiveReference();
}

void cDeme::ClearFounders()
{
  Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
  
  // check for unused genotypes, now that we're done with these
  for (int i=0; i<m_founder_genotype_ids.GetSize(); i++) {
    Systematics::GroupPtr bg = classmgr->ArbiterForRole("genotype")->Group(m_founder_genotype_ids[i]);
    assert(bg);
    bg->RemovePassiveReference();
  }
  
  // empty our list
  m_founder_genotype_ids.ResizeClear(0);
  m_founder_phenotypes.ResizeClear(0);
}

void cDeme::ReplaceGermline(Systematics::GroupPtr bg)
{
  // same genotype, no changes
  if (m_germline_genotype_id == bg->ID()) return;
  
  // first, save and put a hold on new germline genotype
  int prev_germline_genotype_id = m_germline_genotype_id;
  m_germline_genotype_id = bg->ID();
  bg->AddPassiveReference();

  
  // next, if we previously were saving a germline genotype, free it
  
  Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
  Systematics::GroupPtr pbg = classmgr->ArbiterForRole("genotype")->Group(prev_germline_genotype_id);
  if (pbg) pbg->RemovePassiveReference();
}

bool cDeme::DemePredSatisfiedPreviously()
{
	for(int i = 0; i < deme_pred_list.GetSize(); i++) {
    if(deme_pred_list[i]->PreviouslySatisfied()) {
      deme_pred_list[i]->UpdateStats(m_world->GetStats());
      return true;
    }
  }
  return false;
}

bool cDeme::MsgPredSatisfiedPreviously()
{
  for(int i = 0; i < message_pred_list.GetSize(); i++) {
    if(message_pred_list[i]->PreviouslySatisfied()) {
      message_pred_list[i]->UpdateStats(m_world->GetStats());
      return true;
    }
  }
  return false;
}

bool cDeme::MovPredSatisfiedPreviously()
{
  for(int i = 0; i < movement_pred_list.GetSize(); i++) {
    if(movement_pred_list[i]->PreviouslySatisfied()) {
      movement_pred_list[i]->UpdateStats(m_world->GetStats());
      return true;
    }
  }
  return false;
}

int cDeme::GetNumDemePredicates()
{
  return deme_pred_list.GetSize();
}

int cDeme::GetNumMessagePredicates()
{
  return message_pred_list.GetSize();
}

int cDeme::GetNumMovementPredicates()
{
  return movement_pred_list.GetSize();
}

cDemePredicate* cDeme::GetDemePredicate(int i)
{
  assert(i < deme_pred_list.GetSize());
  return deme_pred_list[i];
}

cOrgMessagePredicate* cDeme::GetMsgPredicate(int i)
{
  assert(i < message_pred_list.GetSize());
  return message_pred_list[i];
}

cOrgMovementPredicate* cDeme::GetMovPredicate(int i)
{
  assert(i < movement_pred_list.GetSize());
  return movement_pred_list[i];
}

void cDeme::AddDemeResourceThresholdPredicate(cString resourceName, cString comparisonOperator, double threasholdValue)
{
  cDemeResourceThresholdPredicate* pred =
    new cDemeResourceThresholdPredicate(resourceName, comparisonOperator, threasholdValue);
  deme_pred_list.Push(pred);
}

void cDeme::AddEventReceivedCenterPred(int times)
{
  if (cell_events.GetSize() == 0) {
    cerr<<"Error: An EventReceivedCenterPred cannot be created until a CellEvent is added.\n";
    exit(1);
  }

  for (int i = 0; i < cell_events.GetSize(); i++) {
    if (!cell_events[i].IsDead()) {
      int sink_cell = GetCellID(GetSize()/2);
      cOrgMessagePred_EventReceivedCenter* pred =
	new cOrgMessagePred_EventReceivedCenter(&cell_events[i], sink_cell, times);
      m_world->GetStats().AddMessagePredicate(pred);
      message_pred_list.Push(pred);
    }
  }
}

void cDeme::AddEventReceivedLeftSidePred(int times)
{
  if (cell_events.GetSize() == 0) {
    cerr<<"Error: An EventReceivedLeftSidePred cannot be created until a CellEvent is added.\n";
    exit(1);
  }

  for (int i = 0; i < cell_events.GetSize(); i++) {
    if (!cell_events[i].IsDead()) {
      cOrgMessagePred_EventReceivedLeftSide* pred =
	new cOrgMessagePred_EventReceivedLeftSide(&cell_events[i], m_world->GetPopulation(), times);
      m_world->GetStats().AddMessagePredicate(pred);
      message_pred_list.Push(pred);
    }
  }
}

void cDeme::AddEventMoveCenterPred(int times)
{
  if (cell_events.GetSize() == 0) {
    cerr<<"Error: An EventMovedIntoCenter cannot be created until a CellEvent is added.\n";
    exit(1);
  }
  for (int i = 0; i < cell_events.GetSize(); i++) {
    if (!cell_events[i].IsDead()) {
      cOrgMovementPred_EventMovedIntoCenter* pred = new cOrgMovementPred_EventMovedIntoCenter(&cell_events[i], m_world->GetPopulation(), times);
      m_world->GetStats().AddMovementPredicate(pred);
      movement_pred_list.Push(pred);
    }
  }
}


void cDeme::AddEventMoveBetweenTargetsPred(int times)
{
  if (cell_events.GetSize() == 0) {
    cerr << "Error: An EventMoveBetweenTargets cannot be created until at least one CellEvent is added.\n";
    exit(1);
  }
  
  Apto::Array<cDemeCellEvent*, Apto::Smart> alive_events;
  
  for (int i = 0; i < cell_events.GetSize(); i++) {
    if (!cell_events[i].IsDead()) {
      alive_events.Push(&cell_events[i]);
    }
  }
  
  cOrgMovementPred_EventMovedBetweenTargets* pred = new cOrgMovementPred_EventMovedBetweenTargets(alive_events, m_world->GetPopulation(), times);
  m_world->GetStats().AddMovementPredicate(pred);
  movement_pred_list.Push(pred);
}


void cDeme::AddEventEventNUniqueIndividualsMovedIntoTargetPred(int times)
{
  if (cell_events.GetSize() == 0) {
    cerr << "Error: An EventMovedIntoCenter cannot be created until a CellEvent is added.\n";
    exit(1);
  }
  for (int i = 0; i < cell_events.GetSize(); i++) {
    if (!cell_events[i].IsDead()) {
      cOrgMovementPred_EventNUniqueIndividualsMovedIntoTarget* pred =
	new cOrgMovementPred_EventNUniqueIndividualsMovedIntoTarget(&cell_events[i], m_world->GetPopulation(), times);
      m_world->GetStats().AddMovementPredicate(pred);
      movement_pred_list.Push(pred);
    }
  }
}

void cDeme::AddPheromone(int absolute_cell_id, double value, cAvidaContext& ctx) 
{
  assert(cell_ids[0] <= absolute_cell_id);
  assert(absolute_cell_id <= cell_ids[cell_ids.GetSize()-1]);
  
  //  cPopulation& pop = m_world->GetPopulation();
  
  int relative_cell_id = GetRelativeCellID(absolute_cell_id);
  Apto::Array<double> cell_resources = deme_resource_count.GetCellResources(relative_cell_id, ctx);
  
  for (int i = 0; i < deme_resource_count.GetSize(); i++) {
    if (strcmp(deme_resource_count.GetResName(i), "pheromone") == 0) {
      // There should only be one "pheromone" resource, so no need to divvy value up
      cell_resources[i] = value;
    }
    else {
      cell_resources[i] = 0;
    }
  }
  
  //It appears that ModifyCell adds the amount of resources specified in the cell_resources array, so I'm just
  //settign the element to the value I want to add instead of setting the element to the current value plus the amount to add
  // Ask Ben why he does it differently in GiveBackCellEnergy()
  
  deme_resource_count.ModifyCell(ctx, cell_resources, relative_cell_id);
  
  // CellData-based version
  //const int newval = pop.GetCell(absolute_cell_id).GetCellData() + (int) round(value);
  //pop.GetCell(absolute_cell_id).SetCellData(newval);
  
} //End AddPheromone()

double cDeme::GetSpatialResource(int rel_cellid, int resource_id, cAvidaContext& ctx) const 
{
  assert(rel_cellid >= 0);
  assert(rel_cellid < GetSize());
  assert(resource_id >= 0);
  assert(resource_id < deme_resource_count.GetSize());
  
  Apto::Array<double> cell_resources = deme_resource_count.GetCellResources(rel_cellid, ctx);
  return cell_resources[resource_id];
}

void cDeme::AdjustSpatialResource(cAvidaContext& ctx, int rel_cellid, int resource_id, double amount)
{
  assert(rel_cellid >= 0);
  assert(rel_cellid < GetSize());
  assert(resource_id >= 0);
  assert(resource_id < deme_resource_count.GetSize());
  
  Apto::Array<double> res_change;
  res_change.Resize(deme_resource_count.GetSize(), 0);
  res_change[resource_id] = amount;
  
  deme_resource_count.ModifyCell(ctx, res_change, rel_cellid);  
}

void cDeme::AdjustResource(cAvidaContext& ctx, int resource_id, double amount)
{
  double new_amount = deme_resource_count.Get(ctx, resource_id) + amount;
  deme_resource_count.Set(ctx, resource_id, new_amount);
}

int cDeme::GetSlotFlowRate() const
{
  vector<pair<int, int> >::const_iterator iter = event_slot_end_points.begin();
  while (iter != event_slot_end_points.end()) {
    if (GetAge() <= (*iter).first) {
      return (*iter).second;
    }
    iter++;
  }

  //  assert(false); // slots must be of equal size and fit perfectally in deme lifetime
  return 0;
}


// Return whether or not this deme is treatable at the given age (updates).
// If a deme is not treatable, this function will always return false.
bool cDeme::IsTreatableAtAge(const int age)
{  
  if (isTreatable()) {
    if (treatment_ages.size() == 0) { return false; } // implies treatable every update
    set<int>::iterator it;
    it = treatment_ages.find(age);
    if(it != treatment_ages.end()) return true;  
  }
  
  return false;
  
} //End cDeme::IsTreatableAtAge()


/*! Retrieve the network object, initializing it as needed.
 */
cDemeNetwork& cDeme::GetNetwork()
{
  InitNetworkCreation(); 
  return *m_network; 
}

void cDeme::ResetInputs(cAvidaContext& ctx)
{
  m_world->GetEnvironment().SetupInputs(ctx, m_inputs);
}

//@JJB**
int cDeme::GetNextDemeInput(cAvidaContext& ctx)
{
  // Hack protection
  if (!m_inputs.GetSize()) ResetInputs(ctx);
  m_input_pointer %= m_inputs.GetSize();
  return m_inputs[m_input_pointer++];
}

//@JJB**
void cDeme::DoDemeInput(int value)
{
  m_input_buf.Add(value);
}

//@JJB**
void cDeme::DoDemeOutput(cAvidaContext& ctx, int value)
{
  // Add value to the output buffer
  m_output_buf.Add(value);

  // Needed to setup taskctx, but will not actually be used
  tList<tBuffer<int> > other_input_list;
  tList<tBuffer<int> > other_output_list;
  Apto::Array<int, Apto::Smart> ext_mem;

  // Setup the task context
  cTaskContext taskctx(NULL, m_input_buf, m_output_buf, other_input_list, other_output_list,
                       ext_mem, false, NULL, this);
  taskctx.SetTaskStates(&m_task_states);

  const cEnvironment& env = m_world->GetEnvironment();
  const int num_resources = env.GetResourceLib().GetSize();
  const int num_tasks = env.GetNumTasks();
  const int num_reactions = env.GetReactionLib().GetSize();

  // Create a new reaction result
  if (!m_reaction_result) m_reaction_result = new cReactionResult(num_resources, num_tasks, num_reactions);
  cReactionResult& result = *m_reaction_result;

  // Not actually set up for deme's using resources during reactions
  Apto::Array<double> res_in;
  Apto::Array<double> rbins_in;

  // The environment, evaluates if a task and if a resulting reaction were completed
  bool found = env.TestOutput(ctx, result, taskctx, m_task_count, m_reaction_count, res_in, rbins_in);

  // No task completed, end here
  if (found == false) {
    result.Invalidate();
    return;
  }

  // Update records with the results..

  // Update deme's task and reaction counters
  for (int i = 0; i < num_tasks; i++) {
    if (result.TaskDone(i) == true) {
      m_task_count[i]++;
    }
  }
  for (int i = 0; i < num_reactions; i++) {
    if (result.ReactionTriggered(i) == true) {
      m_reaction_count[i]++;
    }
  }

  // Update stats task totals
  for (int i = 0; i < num_tasks; i++) {
    if (result.TaskDone(i) && !m_last_task_count[i]) {
      m_world->GetStats().AddNewTaskCount(i);
      int prev_num_tasks = 0;
      int cur_num_tasks = 0;
      for (int j = 0; j < num_tasks; j++) {
        if (m_last_task_count[j] > 0) prev_num_tasks++;
        if (m_task_count[j] > 0) cur_num_tasks++;
      }
      m_world->GetStats().AddOtherTaskCounts(i, prev_num_tasks, cur_num_tasks);
    }
  }

  // Update stats reaction totals
  for (int i = 0; i < num_reactions; i++) {
    m_cur_reaction_add_reward[i] += result.GetReactionAddBonus(i);
    if (result.ReactionTriggered(i) && last_reaction_count[i] == 0) {
      m_world->GetStats().AddNewReactionCount(i);
    }
  }

  // Update deme's merit bonus from reaction
  m_cur_bonus *= result.GetMultBonus(); //**
  m_cur_bonus += result.GetAddBonus(); //**

  //**
  //if (result.GetActiveDeme()) {
  //  double deme_bonus = GetHeritableDemeMerit().GetDouble();
  //  deme_bonus *= result.GetMultDemeBonus();
  //  deme_bonus += result.GetAddDemeBonus();
  //  UpdateHeritableDemeMerit(deme_bonus);
  //}

  // If applying merit changes immediately, update the deme's merit merit
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    UpdateCurMerit();
  }

  //**
  //for (int i = 0; i < num_tasks; i++) {
  //  if (result.TaskDone(i) == true) AddCurTask(i);
  //}
  //for (int i = 0; i < num_reactions; i++) {
  //  if (result.ReactionTriggered(i) == true) AddCurReaction(i);
  //}

  result.Invalidate();
}

//@JJB**
void cDeme::UpdateCurMerit()
{
  double merit_base;
  if (m_world->GetConfig().BASE_MERIT_METHOD.Get() == BASE_MERIT_CONST) {
    merit_base = m_world->GetConfig().BASE_CONST_MERIT.Get();
  } else {
    merit_base = 1.0;
  }
  m_cur_merit = merit_base * m_cur_bonus;
}

//@JJB**
cMerit cDeme::CalcCurMerit()
{
  cMerit cur_merit;
  double merit_base;
  if (m_world->GetConfig().BASE_MERIT_METHOD.Get() == BASE_MERIT_CONST) {
    merit_base = m_world->GetConfig().BASE_CONST_MERIT.Get();
  } else {
    merit_base = 1.0;
  }
  cur_merit = merit_base * m_cur_bonus;
  return cur_merit;
}

// Returns the minimum number of times any of the reactions were performed
int cDeme::MinNumTimesReactionPerformed()
{
  int min = 100000000;
  for (int i = 0; i < cur_reaction_count.GetSize(); i++ ) {
    if (cur_reaction_count[i] < min) { min = cur_reaction_count[i]; }
  }
  return min;
}


// Track the number of resources used. 
double cDeme::GetTotalResourceAmountConsumed() const
{
  double total = m_total_res_consumed;
  return total;
}


/** calculate shannon mutual entropy for the relationship between individuals and tasks. A few notes: 
 - pi - probability of an individual - 1/# orgs that do something
 - pij - probability an individual performs a task:
 (num times ind performs this task / num tasks performed by ind) / num orgs that did stuff
 
 
 Because we are only including organisms that do a task in this calculation, we also need a separate
 stat that tracks the percentage of a deme that does stuff. */

double cDeme::GetShannonMutualInformation()
{	
  // exit if 0 or 1 organism did stuff.
  if ((m_num_active == 0) || (m_num_active ==1)) return 0.0;
	
  int num_org = m_shannon_matrix.size();
  int num_task = m_shannon_matrix[0].size();
  double* ptask_array = new double[num_task];
  // create ptasks

  for (int j=0; j<num_task; j++){
    ptask_array[j] =0;
    
    for (int i=0; i<num_org; i++) {
      ptask_array[j] += m_shannon_matrix[i][j];
    }
    ptask_array[j] /= m_num_active;
  }
	
  double shannon_sum = 0.0;
  double shannon_change = 0.0;
  double pij = 0.0;
  double pi = 1.0/m_num_active;
  double pj = 0;
  double pij_sum = 0.0;
  // calculate shannon mutual information
  for (int i=0; i<num_org; i++) {
    for (int j=0; j<num_task; j++) {
      pij = m_shannon_matrix[i][j]/m_num_active;
      pj = ptask_array[j];
      pij_sum += pij;
      if (pi && pj && pij) {
        shannon_change= (pij * log(pij / (pi * pj)));
        shannon_sum += shannon_change;
      }
    }
  }
	
//  shannon_sum /= log((double)m_num_active);
  delete[] ptask_array;
  return shannon_sum;
}

double cDeme::GetNumOrgsPerformedReaction()
{ 
  return m_num_active;
}


/* Update the task counts for an organism. if we are tracking repro, then the repro count replaces the first task count */
void cDeme::UpdateShannon(cPopulationCell& cell)
{
  int org_react_count = 0;
  vector<double> org_row; 
  if (cell.IsOccupied()) {
    cOrganism* organism = cell.GetOrganism();
    cPhenotype& phenotype = organism->GetPhenotype();			
    const Apto::Array<int> curr_react =  phenotype.GetCumulativeReactionCount();
    org_row.resize(curr_react.GetSize(), 0.0);		
    // track number of reproductives
    if ((phenotype.GetNumDivides() - phenotype.GetNumDivideFailed()) > 0) { 
      ++m_num_reproductives; 
    }
    
    for (int j=0; j<curr_react.GetSize(); j++) {
      
      // we are tracking repro as a task
      if ((m_world->GetConfig().DEMES_TRACK_SHANNON_INFO.Get() == 2) && (j==0)) {
        org_react_count += (phenotype.GetNumDivides() - phenotype.GetNumDivideFailed());
        org_row[j] = (phenotype.GetNumDivides() - phenotype.GetNumDivideFailed());
      } else {
        org_react_count += curr_react[j];
        org_row[j] = curr_react[j];
      }
    }
		
    if (org_react_count > 0) {
      // normalize the data for the current organism.
      for (int j=0; j<curr_react.GetSize(); j++){
        if (org_row[j]) org_row[j] /= org_react_count;
      }
      m_num_active++;
      m_shannon_matrix.push_back(org_row);
    }
  }
}

void cDeme::UpdateShannonAll()
{
  for (int i=0; i<GetSize(); ++i) {
    cPopulationCell& cell = m_world->GetPopulation().GetCell(GetCellID(i));
    UpdateShannon(cell);
  }
}

double cDeme::GetPercentReproductives()
{
  double per = (m_num_reproductives/((double)injected_count + (double)cur_birth_count))*100;
  return per; 
}

void cDeme::ClearShannonInformationStats()
{
  m_total_res_consumed = 0;
  m_switch_penalties = 0;
  m_num_active = 0;
  m_shannon_matrix.clear();
  m_num_reproductives = 0;
}



/* Returns the average number of mutations that have occured to the deme's germline as a the result damage accrued by performing tasks. */
std::pair<double, double> cDeme::GetAveVarGermMut() 
{

  if ((m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() != 7) && 
      (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() != 8)) {
    cPopulationCell& c = GetCell(0);
    if (c.IsOccupied()) { c.GetOrganism()->JoinGermline(); }
  }
  cDoubleSum mut_count;
  
  for (int i=0; i<GetSize(); ++i) {
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied()) {
      cOrganism* o = cell.GetOrganism();
      if (o->IsGermline()) {
        mut_count.Add(o->GetNumOfPointMutationsApplied());
      }
    }
  }
  
  return (make_pair(mut_count.Average(), mut_count.Variance()));
}

std::pair<double, double> cDeme::GetAveVarSomaMut() 
{
  
  if ((m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() != 7) && 
      (m_world->GetConfig().DEMES_ORGANISM_SELECTION.Get() != 8)) {
    cPopulationCell& c = GetCell(0);
    if (c.IsOccupied()) { c.GetOrganism()->JoinGermline(); }
  }
  
  cDoubleSum mut_count;

  for (int i=0; i<GetSize(); ++i) {
    
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied()) {
      cOrganism* o = cell.GetOrganism();
      if (!o->IsGermline()) {
        mut_count.Add(o->GetNumOfPointMutationsApplied());
      }
    }
  }
  
  return (make_pair(mut_count.Average(), mut_count.Variance()));
}

std::pair<double, double> cDeme::GetGermlineNumPercent() {
  double count = 0;
  double total_count = 0;
  for (int i=0; i<GetSize(); ++i) {
    
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied()) {
      cOrganism* o = cell.GetOrganism();
      if (o->IsGermline()) {
        ++count; 
      }
      ++total_count;
    }
  }
  total_count = (count/total_count) * 100;
  return (make_pair(count, total_count));
}

std::pair<double, double> cDeme::GetAveVarGermWorkLoad() {
  cDoubleSum work_load;
  
  for (int i=0; i<GetSize(); ++i) {
    
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied()) {
      cOrganism* o = cell.GetOrganism();
      double cur_org_w = 0;
      if (o->IsGermline()) {
        // Compute workload...
        const Apto::Array<int> curr_react =  o->GetPhenotype().GetCumulativeReactionCount();
        for (int j=0; j<curr_react.GetSize(); j++) {
          double weight = 1.0;
          // weight each reaction according to.
          if (m_world->GetConfig().INST_POINT_MUT_SLOPE.Get() > 0.0) { 
            weight = m_world->GetConfig().INST_POINT_MUT_SLOPE.Get() * j; 
          }
          // The first task never has any work associated with it.
          if (j > 0) {
            cur_org_w += weight * curr_react[j];
          }
        }
        work_load.Add(cur_org_w);
      }
    }
  }
  
  return (make_pair(work_load.Average(), work_load.Variance()));

}

std::pair<double, double> cDeme::GetAveVarSomaWorkLoad() {
  cDoubleSum work_load;
  
  for (int i=0; i<GetSize(); ++i) {
    
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied()) {
      cOrganism* o = cell.GetOrganism();
      double cur_org_w = 0;
      if (!o->IsGermline()) {
        // Compute workload...
        const Apto::Array<int> curr_react =  o->GetPhenotype().GetCumulativeReactionCount();
        for (int j=0; j<curr_react.GetSize(); j++) {
          double weight = 1.0;
          // weight each reaction according to.
          if (m_world->GetConfig().INST_POINT_MUT_SLOPE.Get() > 0.0) { 
            weight = m_world->GetConfig().INST_POINT_MUT_SLOPE.Get() * j; 
          }
          // The first task never has any work associated with it.
          if (j > 0) {
            cur_org_w += weight * curr_react[j];
          }
        }
        work_load.Add(cur_org_w);
      }
    }
  }
  
  return (make_pair(work_load.Average(), work_load.Variance()));

}


std::pair<double, double> cDeme::GetAveVarWorkLoad() {
  cDoubleSum work_load;
  
  for (int i=0; i<GetSize(); ++i) {
    
    cPopulationCell& cell = GetCell(i);
    if (cell.IsOccupied()) {
      cOrganism* o = cell.GetOrganism();
      double cur_org_w = 0;
        // Compute workload...
        const Apto::Array<int> curr_react =  o->GetPhenotype().GetCumulativeReactionCount();
        for (int j=0; j<curr_react.GetSize(); j++) {
          double weight = 1.0;
          // weight each reaction according to.
          if (m_world->GetConfig().INST_POINT_MUT_SLOPE.Get() > 0.0) { 
            weight = m_world->GetConfig().INST_POINT_MUT_SLOPE.Get() * j; 
          }
          // The first task never has any work associated with it.
          if (j > 0) {
            cur_org_w += weight * curr_react[j];
          }
        work_load.Add(cur_org_w);
      }
    }
  }
  
  return (make_pair(work_load.Average(), work_load.Variance()));
  
}

