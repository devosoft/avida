/*
 *  cPhenotype.cc
 *  Avida
 *
 *  Called "phenotype.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cPhenotype.h"

#include "cEnvironment.h"
#include "cDeme.h"
#include "cOrganism.h"
#include "cReactionResult.h"
#include "cTaskState.h"
#include "cWorld.h"
#include "tList.h"

#include <fstream>

using namespace std;


cPhenotype::cPhenotype(cWorld* world, int parent_generation, int num_nops)
: m_world(world)
, initialized(false)
, energy_store(0.0)
, cur_task_count(m_world->GetEnvironment().GetNumTasks())
, cur_para_tasks(m_world->GetEnvironment().GetNumTasks())
, cur_host_tasks(m_world->GetEnvironment().GetNumTasks())
, cur_internal_task_count(m_world->GetEnvironment().GetNumTasks())
, eff_task_count(m_world->GetEnvironment().GetNumTasks())
, cur_task_quality(m_world->GetEnvironment().GetNumTasks())  
, cur_task_value(m_world->GetEnvironment().GetNumTasks())  
, cur_internal_task_quality(m_world->GetEnvironment().GetNumTasks())
, cur_rbins_total(m_world->GetEnvironment().GetResourceLib().GetSize())
, cur_rbins_avail(m_world->GetEnvironment().GetResourceLib().GetSize())
, cur_reaction_count(m_world->GetEnvironment().GetReactionLib().GetSize())
, cur_reaction_add_reward(m_world->GetEnvironment().GetReactionLib().GetSize())
, cur_sense_count(m_world->GetStats().GetSenseSize())
, sensed_resources(m_world->GetEnvironment().GetResourceLib().GetSize())
, cur_task_time(m_world->GetEnvironment().GetNumTasks())   // Added for tracking time; WRE 03-18-07
, m_reaction_result(NULL)
, last_task_count(m_world->GetEnvironment().GetNumTasks())
, last_para_tasks(m_world->GetEnvironment().GetNumTasks())
, last_host_tasks(m_world->GetEnvironment().GetNumTasks())
, last_internal_task_count(m_world->GetEnvironment().GetNumTasks())
, last_task_quality(m_world->GetEnvironment().GetNumTasks())
, last_task_value(m_world->GetEnvironment().GetNumTasks())
, last_internal_task_quality(m_world->GetEnvironment().GetNumTasks())
, last_rbins_total(m_world->GetEnvironment().GetResourceLib().GetSize())
, last_rbins_avail(m_world->GetEnvironment().GetResourceLib().GetSize())
, last_collect_spec_counts()
, last_reaction_count(m_world->GetEnvironment().GetReactionLib().GetSize())
, last_reaction_add_reward(m_world->GetEnvironment().GetReactionLib().GetSize())  
, last_sense_count(m_world->GetStats().GetSenseSize())
, generation(0)
, last_task_id(-1)
, num_new_unique_reactions(0)
, res_consumed(0)
{
  if (parent_generation >= 0) {
    generation = parent_generation;
    if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
  }
  
  double num_resources = m_world->GetEnvironment().GetResourceLib().GetSize();
  if (num_resources <= 0 || num_nops <= 0) return;
  double most_nops_needed = ceil(log(num_resources) / log((double)num_nops));
  cur_collect_spec_counts.Resize((pow((double)num_nops, most_nops_needed + 1.0) - 1.0) / ((double)num_nops - 1.0));
  
}

cPhenotype::~cPhenotype()
{
  // Remove Task States
  tArray<cTaskState*> task_states(0);
  m_task_states.GetValues(task_states);
  for (int i = 0; i < task_states.GetSize(); i++) delete task_states[i];
  delete m_reaction_result;
}


cPhenotype::cPhenotype(const cPhenotype& in_phen) : m_reaction_result(NULL)
{
  *this = in_phen;
}


cPhenotype& cPhenotype::operator=(const cPhenotype& in_phen)
{
  
  m_world                  = in_phen.m_world;
  initialized              = in_phen.initialized;
  
  
  // 1. These are values calculated at the last divide (of self or offspring)
  merit                    = in_phen.merit;
  executionRatio           = in_phen.executionRatio;
  energy_store             = in_phen.energy_store;    
  energy_tobe_applied      = in_phen.energy_tobe_applied;
  energy_testament         = in_phen.energy_testament;
  energy_received_buffer   = in_phen.energy_received_buffer;
  genome_length            = in_phen.genome_length;        
  bonus_instruction_count  = in_phen.bonus_instruction_count; 
  copied_size              = in_phen.copied_size;          
  executed_size            = in_phen.executed_size;       
  gestation_time           = in_phen.gestation_time;       
  
  gestation_start          = in_phen.gestation_start;     
  fitness                  = in_phen.fitness;           
  div_type                 = in_phen.div_type;          
  
  // 2. These are "in progress" variables, updated as the organism operates
  cur_bonus                = in_phen.cur_bonus;                           
  cur_energy_bonus         = in_phen.cur_energy_bonus;                   
  cur_num_errors           = in_phen.cur_num_errors;                         
  cur_num_donates          = in_phen.cur_num_donates;                       
  cur_task_count           = in_phen.cur_task_count;  
  cur_para_tasks           = in_phen.cur_para_tasks;
  cur_host_tasks           = in_phen.cur_host_tasks;
  eff_task_count           = in_phen.eff_task_count;
  cur_internal_task_count  = in_phen.cur_internal_task_count;
  cur_task_quality         = in_phen.cur_task_quality;    
  cur_internal_task_quality= in_phen.cur_internal_task_quality;       
  cur_task_value           = in_phen.cur_task_value;			
  cur_rbins_total          = in_phen.cur_rbins_total;
  cur_rbins_avail          = in_phen.cur_rbins_avail;
  cur_collect_spec_counts  = in_phen.cur_collect_spec_counts;
  cur_reaction_count       = in_phen.cur_reaction_count;            
  cur_reaction_add_reward  = in_phen.cur_reaction_add_reward;     
  cur_inst_count           = in_phen.cur_inst_count;                 
  cur_sense_count          = in_phen.cur_sense_count;                 
  sensed_resources         = in_phen.sensed_resources;            
  cur_task_time            = in_phen.cur_task_time;   
  cur_child_germline_propensity = in_phen.cur_child_germline_propensity;
  
  // Dynamically allocated m_task_states requires special handling
  tList<cTaskState*> hash_values;
  tList<void*>       hash_keys;
  in_phen.m_task_states.AsLists(hash_keys, hash_values);
  tListIterator<cTaskState*> vit(hash_values);
  tListIterator<void*>       kit(hash_keys);
  while(vit.Next() && kit.Next())
  {
    cTaskState* new_ts = new cTaskState(**(vit.Get()));
    m_task_states.Set(*(kit.Get()), new_ts);
  }
  
  // 3. These mark the status of "in progess" variables at the last divide.
  last_merit_base          = in_phen.last_merit_base;       
  last_bonus               = in_phen.last_bonus;
  last_energy_bonus        = in_phen.last_energy_bonus; 
  last_num_errors          = in_phen.last_num_errors; 
  last_num_donates         = in_phen.last_num_donates;
  last_task_count          = in_phen.last_task_count;
  last_host_tasks          = in_phen.last_host_tasks;
  last_para_tasks          = in_phen.last_para_tasks;
  last_internal_task_count = in_phen.last_internal_task_count;
  last_task_quality        = in_phen.last_task_quality;
  last_internal_task_quality=in_phen.last_internal_task_quality;
  last_task_value          = in_phen.last_task_value;
  last_rbins_total         = in_phen.last_rbins_total;
  last_rbins_avail         = in_phen.last_rbins_avail;
  last_collect_spec_counts = in_phen.last_collect_spec_counts;
  last_reaction_count      = in_phen.last_reaction_count;
  last_reaction_add_reward = in_phen.last_reaction_add_reward; 
  last_inst_count          = in_phen.last_inst_count;	  
  last_sense_count         = in_phen.last_sense_count;   
  last_fitness             = in_phen.last_fitness;            
  last_child_germline_propensity = in_phen.last_child_germline_propensity;
  total_energy_donated     = in_phen.total_energy_donated;
  total_energy_received    = in_phen.total_energy_received;
  total_energy_applied     = in_phen.total_energy_applied;
  
  // 4. Records from this organisms life...
  num_divides              = in_phen.num_divides;   
  num_divides_failed       = in_phen.num_divides_failed;
  generation               = in_phen.generation;        
  cpu_cycles_used          = in_phen.cpu_cycles_used;   
  time_used                = in_phen.time_used;         
  age                      = in_phen.age;               
  fault_desc               = in_phen.fault_desc;    
  neutral_metric           = in_phen.neutral_metric; 
  life_fitness             = in_phen.life_fitness; 	
  exec_time_born           = in_phen.exec_time_born;
  birth_update             = in_phen.birth_update;
	num_new_unique_reactions = in_phen.num_new_unique_reactions;
  last_task_id             = in_phen.last_task_id;
	res_consumed             = in_phen.res_consumed;  
  
  // 5. Status Flags...  (updated at each divide)
  to_die                  = in_phen.to_die;		 
  to_delete               = in_phen.to_delete;        
  is_injected             = in_phen.is_injected;      
  is_donor_cur            = in_phen.is_donor_cur;     
  is_donor_last           = in_phen.is_donor_last;     
  is_donor_rand           = in_phen.is_donor_rand;    
  is_donor_rand_last      = in_phen.is_donor_rand_last;
  is_donor_null           = in_phen.is_donor_null;    
  is_donor_null_last      = in_phen.is_donor_null_last;
  is_donor_kin            = in_phen.is_donor_kin;    
  is_donor_kin_last       = in_phen.is_donor_kin_last;
  is_donor_edit           = in_phen.is_donor_edit;   
  is_donor_edit_last      = in_phen.is_donor_edit_last; 
  is_donor_gbg            = in_phen.is_donor_gbg;     
  is_donor_gbg_last       = in_phen.is_donor_gbg_last;
  is_donor_truegb         = in_phen.is_donor_truegb; 
  is_donor_truegb_last    = in_phen.is_donor_truegb_last;
  is_donor_threshgb       = in_phen.is_donor_threshgb;  
  is_donor_threshgb_last  = in_phen.is_donor_threshgb_last;
  is_donor_quanta_threshgb        = in_phen.is_donor_quanta_threshgb;  
  is_donor_quanta_threshgb_last   = in_phen.is_donor_quanta_threshgb_last;
  is_donor_shadedgb       = in_phen.is_donor_shadedgb;  
  is_donor_shadedgb_last  = in_phen.is_donor_shadedgb_last;	
  is_donor_locus          = in_phen.is_donor_locus;
  is_donor_locus_last     = in_phen.is_donor_locus_last;
  num_thresh_gb_donations         = in_phen.num_thresh_gb_donations;  
  num_thresh_gb_donations_last    = in_phen.num_thresh_gb_donations_last;  
  num_quanta_thresh_gb_donations = in_phen.num_quanta_thresh_gb_donations;
  num_quanta_thresh_gb_donations_last = in_phen.num_quanta_thresh_gb_donations_last;
  num_shaded_gb_donations         = in_phen.num_shaded_gb_donations;  
  num_shaded_gb_donations_last    = in_phen.num_shaded_gb_donations_last; 
  num_donations_locus     = in_phen.num_donations_locus;
  num_donations_locus_last = in_phen.num_donations_locus_last;
  is_receiver             = in_phen.is_receiver;   
  is_receiver_last        = in_phen.is_receiver_last;      
  is_receiver_rand        = in_phen.is_receiver_rand;
  is_receiver_kin         = in_phen.is_receiver_kin; 
  is_receiver_kin_last    = in_phen.is_receiver_kin_last; 
  is_receiver_edit        = in_phen.is_receiver_edit; 
  is_receiver_edit_last   = in_phen.is_receiver_edit_last; 
  is_receiver_gbg         = in_phen.is_receiver_gbg; 
  is_receiver_truegb      = in_phen.is_receiver_truegb;
  is_receiver_truegb_last = in_phen.is_receiver_truegb_last;
  is_receiver_threshgb    = in_phen.is_receiver_threshgb;
  is_receiver_threshgb_last    = in_phen.is_receiver_threshgb_last;
  is_receiver_quanta_threshgb  = in_phen.is_receiver_quanta_threshgb;
  is_receiver_quanta_threshgb_last = in_phen.is_receiver_quanta_threshgb_last;
  is_receiver_shadedgb    = in_phen.is_receiver_shadedgb;
  is_receiver_shadedgb_last    = in_phen.is_receiver_shadedgb_last;	
  is_receiver_gb_same_locus = in_phen.is_receiver_gb_same_locus;
  is_receiver_gb_same_locus_last = in_phen.is_receiver_gb_same_locus_last;
  is_modifier             = in_phen.is_modifier;      
  is_modified             = in_phen.is_modified;      
  is_fertile              = in_phen.is_fertile;      
  is_mutated              = in_phen.is_mutated;       
  is_multi_thread         = in_phen.is_multi_thread; 
  parent_true             = in_phen.parent_true;     
  parent_sex              = in_phen.parent_sex;      
  parent_cross_num        = in_phen.parent_cross_num; 
  
  is_energy_requestor     = in_phen.is_energy_requestor;
  is_energy_donor         = in_phen.is_energy_donor;
  is_energy_receiver      = in_phen.is_energy_receiver;
  has_used_donated_energy = in_phen.has_used_donated_energy;
  has_open_energy_request = in_phen.has_open_energy_request;
  total_energy_donated    = in_phen.total_energy_donated;
  total_energy_received   = in_phen.total_energy_received;
  total_energy_applied    = in_phen.total_energy_applied;
  
  // 6. Child information...
  copy_true               = in_phen.copy_true;       
  divide_sex              = in_phen.divide_sex;       
  mate_select_id          = in_phen.mate_select_id;    
  cross_num               = in_phen.cross_num;     
  child_fertile           = in_phen.child_fertile;  
  last_child_fertile      = in_phen.last_child_fertile; 
  child_copied_size       = in_phen.child_copied_size;
  
  // 7. Permanent information...
  permanent_germline_propensity = in_phen.permanent_germline_propensity;
  
  return *this;
}

bool cPhenotype::OK()
{
  assert(genome_length >= 0);
  assert(copied_size >= 0);
  assert(gestation_time >= 0);
  assert(gestation_start >= 0);
  assert(fitness >= 0.0);
  assert(div_type >= 0.0);
  assert(cur_bonus >= 0.0);
  assert(cur_num_errors >= 0);
  assert(cur_num_donates >= 0);
  assert(last_merit_base >= 0.0);
  assert(last_bonus >= 0.0);
  assert(last_num_errors >= 0);
  assert(last_num_donates >= 0);
  assert(last_fitness >= 0.0);
  assert(num_divides >= 0);
  assert(num_divides_failed >= 0);
  assert(generation >= 0);
  assert(cpu_cycles_used >= 0);  
  assert(time_used >= 0);
  assert(age >= 0);
  assert(child_copied_size >= 0);
  assert(energy_received_buffer >= 0);
  // assert(to_die == false);
  return (m_world);
}


/**
 * This function is run whenever a new organism is being constructed inside
 * of its parent.
 *
 * Assumptions:
 *     - parent_phenotype has had DivideReset run on it already!
 *     - this is the first method run on an otherwise freshly built phenotype.
 **/

void cPhenotype::SetupOffspring(const cPhenotype& parent_phenotype, const cSequence& _genome)
{
  // Copy divide values from parent, which should already be setup.
  merit = parent_phenotype.merit;
  if(m_world->GetConfig().INHERIT_EXE_RATE.Get() == 0)
    executionRatio = 1.0;
  else 
    executionRatio = parent_phenotype.executionRatio;
  
  energy_store    = min(energy_store, m_world->GetConfig().ENERGY_CAP.Get());
  energy_tobe_applied = 0.0;
  energy_testament = 0.0;
  energy_received_buffer = 0.0;
  genome_length   = _genome.GetSize();
  copied_size     = parent_phenotype.child_copied_size;
  executed_size   = parent_phenotype.executed_size;
  
  gestation_time  = parent_phenotype.gestation_time;
  gestation_start = 0;
  cpu_cycles_used = 0;
  fitness         = parent_phenotype.fitness;
  div_type        = parent_phenotype.div_type;
  
  assert(genome_length > 0);
  assert(copied_size > 0);
  assert(gestation_time >= 0); //@JEB 0 valid for some fitness methods
  assert(div_type > 0);
  
  // Initialize current values, as neeeded.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_energy_bonus = 0.0;
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  cur_internal_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_host_tasks.SetAll(0);
  cur_para_tasks.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_task_value.SetAll(0);
  cur_internal_task_quality.SetAll(0);
  cur_rbins_total.SetAll(0);  // total resources collected in lifetime
  // parent's resources have already been halved or reset in DivideReset;
  // offspring gets that value (half or 0) too.
  for (int i = 0; i < cur_rbins_avail.GetSize(); i++)
    cur_rbins_avail[i] = parent_phenotype.cur_rbins_avail[i];
  cur_collect_spec_counts.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_sense_count.SetAll(0);  
  cur_task_time.SetAll(0.0);  // Added for time tracking; WRE 03-18-07
  for (int j = 0; j < sensed_resources.GetSize(); j++)
    sensed_resources[j] =  parent_phenotype.sensed_resources[j];
  cur_trial_fitnesses.Resize(0); 
  cur_trial_bonuses.Resize(0); 
  cur_trial_times_used.Resize(0); 
  trial_time_used = 0;
  trial_cpu_cycles_used = 0;
  cur_child_germline_propensity = m_world->GetConfig().DEMES_DEFAULT_GERMLINE_PROPENSITY.Get();
  
  // Copy last values from parent
  last_merit_base           = parent_phenotype.last_merit_base;
  last_bonus                = parent_phenotype.last_bonus;
  last_cpu_cycles_used      = parent_phenotype.last_cpu_cycles_used;
  last_num_errors           = parent_phenotype.last_num_errors;
  last_num_donates          = parent_phenotype.last_num_donates;
  last_task_count           = parent_phenotype.last_task_count;
  last_host_tasks           = parent_phenotype.last_host_tasks;
  last_para_tasks           = parent_phenotype.last_para_tasks;
  last_internal_task_count  = parent_phenotype.last_internal_task_count;
  last_task_quality         = parent_phenotype.last_task_quality;
  last_task_value           = parent_phenotype.last_task_value;
  last_internal_task_quality= parent_phenotype.last_internal_task_quality;
  last_rbins_total          = parent_phenotype.last_rbins_total;
  last_rbins_avail          = parent_phenotype.last_rbins_avail;
  last_collect_spec_counts  = parent_phenotype.last_collect_spec_counts;
  last_reaction_count       = parent_phenotype.last_reaction_count;
  last_reaction_add_reward  = parent_phenotype.last_reaction_add_reward;
  last_inst_count           = parent_phenotype.last_inst_count;
  last_sense_count          = parent_phenotype.last_sense_count;
  last_fitness              = CalcFitness(last_merit_base, last_bonus, gestation_time, last_cpu_cycles_used);
  last_child_germline_propensity = parent_phenotype.last_child_germline_propensity;   // chance of child being a germline cell; @JEB
  
  // Setup other miscellaneous values...
  num_divides     = 0;
  num_divides_failed = 0;
  generation      = parent_phenotype.generation;
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
  cpu_cycles_used = 0;
  time_used       = 0;
  age             = 0;
  fault_desc      = "";
  neutral_metric  = parent_phenotype.neutral_metric + m_world->GetRandom().GetRandNormal();
  life_fitness    = fitness; 
  exec_time_born  = parent_phenotype.exec_time_born;  //@MRR treating offspring and parent as siblings; already set in DivideReset
  birth_update    = parent_phenotype.birth_update;    
	num_new_unique_reactions = 0;
	last_task_id             = -1;
	res_consumed             = 0;
  
  num_thresh_gb_donations = 0;
  num_thresh_gb_donations_last = parent_phenotype.num_thresh_gb_donations_last;
  num_quanta_thresh_gb_donations = 0;
  num_quanta_thresh_gb_donations_last = parent_phenotype.num_thresh_gb_donations_last;
  num_shaded_gb_donations = 0;
  num_shaded_gb_donations_last = parent_phenotype.num_shaded_gb_donations_last;
  num_donations_locus = 0;
  num_donations_locus_last = parent_phenotype.num_donations_locus_last;
  
	
  // Setup flags...
  is_injected   = false;
  is_donor_cur  = false;
  is_donor_last = parent_phenotype.is_donor_last;
  is_donor_rand = false;
  is_donor_rand_last = parent_phenotype.is_donor_rand_last;
  is_donor_null = false;
  is_donor_null_last = parent_phenotype.is_donor_null_last;
  is_donor_kin  = false;
  is_donor_kin_last = parent_phenotype.is_donor_kin_last;
  is_donor_edit  = false;
  is_donor_edit_last = parent_phenotype.is_donor_edit_last;
  is_donor_gbg  = false;
  is_donor_gbg_last = parent_phenotype.is_donor_gbg_last;
  is_donor_truegb  = false;
  is_donor_truegb_last = parent_phenotype.is_donor_truegb_last;
  is_donor_threshgb  = false;
  is_donor_threshgb_last = parent_phenotype.is_donor_threshgb_last;
  is_donor_quanta_threshgb  = false;
  is_donor_quanta_threshgb_last = parent_phenotype.is_donor_quanta_threshgb_last;
  is_donor_shadedgb  = false;
  is_donor_shadedgb_last = parent_phenotype.is_donor_shadedgb_last;	
  is_donor_locus.SetAll(false);
  is_donor_locus_last = parent_phenotype.is_donor_locus_last;
  
  is_receiver   = false;  
  is_receiver_last = parent_phenotype.is_receiver_last;
  is_receiver_rand   = false;
  is_receiver_kin    = false;
  is_receiver_kin_last    = parent_phenotype.is_receiver_kin_last;
  is_receiver_edit   = false;
  is_receiver_edit_last    = parent_phenotype.is_receiver_edit_last;
  is_receiver_gbg    = false;
  is_receiver_truegb = false;
  is_receiver_truegb_last = parent_phenotype.is_receiver_truegb_last;
  is_receiver_threshgb = false;
  is_receiver_threshgb_last = parent_phenotype.is_receiver_threshgb_last;
  is_receiver_quanta_threshgb = false;
  is_receiver_quanta_threshgb_last = parent_phenotype.is_receiver_quanta_threshgb_last;
  is_receiver_shadedgb = false;
  is_receiver_shadedgb_last = parent_phenotype.is_receiver_shadedgb_last;	
  is_receiver_gb_same_locus = false;
  is_receiver_gb_same_locus_last = parent_phenotype.is_receiver_gb_same_locus;
  
  is_modifier   = false;
  is_modified   = false;
  is_fertile    = parent_phenotype.last_child_fertile;
  is_mutated    = false;
  if(m_world->GetConfig().INHERIT_MULTITHREAD.Get()) {
    is_multi_thread = parent_phenotype.is_multi_thread;
  } else {
    is_multi_thread = false;
  }
  
  parent_true   = parent_phenotype.copy_true;
  parent_sex    = parent_phenotype.divide_sex;
  parent_cross_num    = parent_phenotype.cross_num;
  to_die = false;
  to_delete = false;
  
  is_energy_requestor = false;
  is_energy_donor = false;
  is_energy_receiver = false;
  has_used_donated_energy = false;
  has_open_energy_request = false;
  total_energy_donated = 0.0;
  total_energy_received = 0.0; 
  total_energy_applied = 0.0;
  
  // Setup child info...
  copy_true          = false;
  divide_sex         = false;
  mate_select_id     = -1;
  cross_num          = 0;
  last_child_fertile = is_fertile;
  child_fertile      = true;
  child_copied_size  = 0;
  
  // permanently set germline propensity of org (since DivideReset is called first, it is now in the "last" slot...)
  permanent_germline_propensity  = parent_phenotype.last_child_germline_propensity;
  
  initialized = true;
}


/**
 * This function is run whenever a new organism is being constructed via
 * some form of injection into the population, or in a test environment.
 *
 * Assumptions:
 *     - Updates to these values (i.e. resetting of merit) will occur afterward
 *     - This is the first method run on an otherwise freshly built phenotype.
 **/

void cPhenotype::SetupInject(const cSequence & _genome)
{
  // Setup reasonable initial values injected organism...
  genome_length   = _genome.GetSize();
  merit           = genome_length;
  copied_size     = genome_length;
  executed_size   = genome_length;
  energy_store    = min(m_world->GetConfig().ENERGY_GIVEN_ON_INJECT.Get(), m_world->GetConfig().ENERGY_CAP.Get());
  energy_tobe_applied = 0.0;
  energy_testament = 0.0;
  energy_received_buffer = 0.0;
  executionRatio = 1.0;
  gestation_time  = 0;
  gestation_start = 0;
  fitness         = 0;
  div_type	  = 1;
  
  // Initialize current values, as neeeded.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_energy_bonus = 0.0;
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  cur_para_tasks.SetAll(0);
  cur_host_tasks.SetAll(0);
  cur_internal_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_task_value.SetAll(0);
  cur_internal_task_quality.SetAll(0);
  cur_rbins_total.SetAll(0);
  cur_rbins_avail.SetAll(0);
  cur_collect_spec_counts.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  sensed_resources.SetAll(0);
  cur_sense_count.SetAll(0);
  cur_task_time.SetAll(0.0);
  cur_trial_fitnesses.Resize(0);
  cur_trial_bonuses.Resize(0); 
  cur_trial_times_used.Resize(0); 
  trial_time_used = 0;
  trial_cpu_cycles_used = 0;
  cur_child_germline_propensity = m_world->GetConfig().DEMES_DEFAULT_GERMLINE_PROPENSITY.Get();
  
  // New organism has no parent and so cannot use its last values; initialize as needed
  last_merit_base = genome_length;
  last_bonus      = 1;
  last_cpu_cycles_used = 0;
  last_num_errors = 0;
  last_num_donates = 0;
  last_task_count.SetAll(0);
  last_host_tasks.SetAll(0);
  last_para_tasks.SetAll(0);
  last_internal_task_count.SetAll(0);
  last_task_quality.SetAll(0);
  last_task_value.SetAll(0);
  last_internal_task_quality.SetAll(0);
  last_rbins_total.SetAll(0);
  last_rbins_avail.SetAll(0);
  last_collect_spec_counts.SetAll(0);
  last_reaction_count.SetAll(0);
  last_reaction_add_reward.SetAll(0);
  last_inst_count.SetAll(0);
  last_sense_count.SetAll(0);
  last_child_germline_propensity = m_world->GetConfig().DEMES_DEFAULT_GERMLINE_PROPENSITY.Get();
  
  // Setup other miscellaneous values...
  num_divides     = 0;
  num_divides_failed = 0;
  generation      = 0;
  cpu_cycles_used = 0;
  time_used       = 0;
  age             = 0;
  fault_desc      = "";
  neutral_metric  = 0;
  life_fitness    = 0;
  exec_time_born  = 0;
  birth_update     = m_world->GetStats().GetUpdate();
  
  num_thresh_gb_donations = 0;
  num_thresh_gb_donations_last = 0;
  num_quanta_thresh_gb_donations = 0;
  num_quanta_thresh_gb_donations_last = 0;
  num_shaded_gb_donations = 0;
  num_shaded_gb_donations_last = 0;	
  num_donations_locus = 0;
  num_donations_locus_last = 0;
	
  // Setup flags...
  is_injected   = true;
  is_donor_last = false;
  is_donor_cur  = false;
  is_donor_rand = false;
  is_donor_rand_last = false;
  is_donor_null = false;
  is_donor_null_last = false;
  is_donor_kin = false;
  is_donor_kin_last = false;
  is_donor_edit = false;
  is_donor_edit_last = false;
  is_donor_gbg = false;
  is_donor_gbg_last = false;
  is_donor_truegb = false;
  is_donor_truegb_last = false;
  is_donor_threshgb = false;
  is_donor_threshgb_last = false;
  is_donor_quanta_threshgb = false;
  is_donor_quanta_threshgb_last = false;
  is_donor_shadedgb = false;
  is_donor_shadedgb_last = false;
  is_donor_locus.SetAll(false);
  is_donor_locus_last.SetAll(false);
  
  is_receiver   = false;
  is_receiver_last   = false;
  is_receiver_rand   = false;
  is_receiver_kin   = false;
  is_receiver_kin_last   = false;
  is_receiver_edit   = false;
  is_receiver_edit_last   = false;
  is_receiver_gbg   = false;
  is_receiver_truegb   = false;
  is_receiver_truegb_last   = false;
  is_receiver_threshgb   = false;
  is_receiver_threshgb_last   = false;
  is_receiver_quanta_threshgb   = false;
  is_receiver_quanta_threshgb_last   = false;
  is_receiver_shadedgb   = false;
  is_receiver_shadedgb_last   = false;	
  is_receiver_gb_same_locus = false;
  is_receiver_gb_same_locus_last = false;
  
  is_modifier   = false;
  is_modified   = false;
  is_fertile    = true;
  is_mutated    = false;
  is_multi_thread = false;
  parent_true   = true;
  parent_sex    = false;
  parent_cross_num    = 0;
  to_die = false;
  to_delete = false;
  
  is_energy_requestor = false;
  is_energy_donor = false;
  is_energy_receiver = false;
  has_used_donated_energy = false;
  has_open_energy_request = false;
  total_energy_donated = 0.0;
  total_energy_received = 0.0;
  total_energy_applied = 0.0;
  
  // Setup child info...
  copy_true         = false;
  divide_sex        = false;
  mate_select_id    = -1;
  cross_num         = 0;
  child_fertile     = true;
  last_child_fertile = true;
  child_copied_size = 0;
  
  permanent_germline_propensity = m_world->GetConfig().DEMES_DEFAULT_GERMLINE_PROPENSITY.Get();
  
  initialized = true;
}


void cPhenotype::ResetMerit(const cSequence & _cgenome)
{
  int cur_merit_base = CalcSizeMerit();
  const int merit_default_bonus = m_world->GetConfig().MERIT_DEFAULT_BONUS.Get();
  if (merit_default_bonus) {
    cur_bonus = merit_default_bonus;
  }
	merit = cur_merit_base * cur_bonus;
	
  if(m_world->GetConfig().INHERIT_MERIT.Get() == 0)
    merit = cur_merit_base;
  
}


/**
 * This function is run whenever an organism executes a successful divide.
 **/

void cPhenotype::DivideReset(const cSequence & _genome)
{
  assert(time_used >= 0);
  assert(initialized == true);
  
  // Update these values as needed...
  int cur_merit_base = CalcSizeMerit();
  
  // If we are resetting the current merit, do it here
  // and it will also be propagated to the child
  const int merit_default_bonus = m_world->GetConfig().MERIT_DEFAULT_BONUS.Get();
  if (merit_default_bonus) {
    cur_bonus = merit_default_bonus;
  }
	merit = cur_merit_base * cur_bonus;
	
  if(m_world->GetConfig().INHERIT_MERIT.Get() == 0)
    merit = cur_merit_base;
  
  SetEnergy(energy_store + cur_energy_bonus);
  m_world->GetStats().SumEnergyTestamentAcceptedByOrganisms().Add(energy_testament);
  energy_testament = 0.0;
  energy_received_buffer = 0.0;  // If donated energy not applied, it's lost here
  
  genome_length   = _genome.GetSize();
  (void) copied_size;          // Unchanged
  (void) executed_size;        // Unchanged
  gestation_time  = time_used - gestation_start;
  gestation_start = time_used;
  fitness = CalcFitness( cur_merit_base, cur_bonus, gestation_time, cpu_cycles_used); 
  
  // Lock in cur values as last values.
  last_merit_base           = cur_merit_base;
  last_bonus                = cur_bonus;
  last_cpu_cycles_used      = cpu_cycles_used;
  //TODO?  last_energy         = cur_energy_bonus;
  last_num_errors           = cur_num_errors;
  last_num_donates          = cur_num_donates;
  last_task_count           = cur_task_count;
  last_host_tasks           = cur_host_tasks;
  last_para_tasks           = cur_para_tasks;
  last_internal_task_count  = cur_internal_task_count;
  last_task_quality         = cur_task_quality;
  last_task_value           = cur_task_value;
  last_internal_task_quality= cur_internal_task_quality;
  last_rbins_total          = cur_rbins_total;
  last_rbins_avail          = cur_rbins_avail;
  last_collect_spec_counts  = cur_collect_spec_counts;
  last_reaction_count       = cur_reaction_count;
  last_reaction_add_reward  = cur_reaction_add_reward;
  last_inst_count           = cur_inst_count;
  last_sense_count          = cur_sense_count;
  last_child_germline_propensity = cur_child_germline_propensity;
  
  // Reset cur values.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cpu_cycles_used = 0;
  cur_energy_bonus = 0.0;
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  cur_host_tasks.SetAll(0);
  
  //LZ: figure out when and where to reset cur_para_tasks, depending on the divide method, and resonable assumptions
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT)
  {     
    last_para_tasks = cur_para_tasks;
    cur_para_tasks.SetAll(0);
  }
  cur_internal_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_task_value.SetAll(0);
  cur_internal_task_quality.SetAll(0);
  cur_rbins_total.SetAll(0);  // total resources collected in lifetime
  if (m_world->GetConfig().SPLIT_ON_DIVIDE.Get()) {
    // resources available are split in half -- the offspring gets the other half
    for (int i = 0; i < cur_rbins_avail.GetSize(); i++) {cur_rbins_avail[i] /= 2.0;}
  }
  else {
    cur_rbins_avail.SetAll(0);
  }
  cur_collect_spec_counts.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_sense_count.SetAll(0);
  cur_task_time.SetAll(0.0);
  cur_child_germline_propensity = m_world->GetConfig().DEMES_DEFAULT_GERMLINE_PROPENSITY.Get();
  
  // Setup other miscellaneous values...
  num_divides++;
  (void) generation;
  (void) time_used;
  age             = 0;
  fault_desc      = "";
  (void) neutral_metric;
  life_fitness = fitness; 
  exec_time_born += gestation_time;  //@MRR Treating organism as sibling
  birth_update = m_world->GetStats().GetUpdate();   
	num_new_unique_reactions = 0;
	last_task_id             = -1;
	res_consumed             = 0;
  
  num_thresh_gb_donations_last = num_thresh_gb_donations;
  num_thresh_gb_donations = 0;
  num_quanta_thresh_gb_donations_last = num_quanta_thresh_gb_donations;
  num_quanta_thresh_gb_donations = 0;
  num_shaded_gb_donations_last = num_shaded_gb_donations;
  num_shaded_gb_donations = 0;
  num_donations_locus_last = num_donations_locus;
  num_donations_locus = 0;
	
  // Leave flags alone...
  (void) is_injected;
  is_donor_last = is_donor_cur;
  is_donor_cur = false;
  is_donor_rand_last = is_donor_rand;
  is_donor_rand = false;
  is_donor_null_last = is_donor_null;
  is_donor_null = false;
  is_donor_kin_last = is_donor_kin;
  is_donor_kin = false;
  is_donor_edit_last = is_donor_edit;
  is_donor_edit = false;
  is_donor_gbg_last = is_donor_gbg;
  is_donor_gbg = false;
  is_donor_truegb_last = is_donor_truegb;
  is_donor_truegb = false;
  is_donor_threshgb_last = is_donor_threshgb;
  is_donor_threshgb = false;
  is_donor_quanta_threshgb_last = is_donor_quanta_threshgb;
  is_donor_quanta_threshgb = false;
  is_donor_shadedgb_last = is_donor_shadedgb;
  is_donor_shadedgb = false;	
  is_donor_locus_last = is_donor_locus;
  is_donor_locus.SetAll(false);
  
  is_receiver_last = is_receiver;
  is_receiver = false;
  is_receiver_rand = false;
  is_receiver_kin_last = is_receiver_kin;
  is_receiver_kin = false;
  is_receiver_edit_last = is_receiver_edit;
  is_receiver_edit = false;
  is_receiver_gbg = false;
  is_receiver_truegb_last = is_receiver_truegb;
  is_receiver_truegb = false;
  is_receiver_threshgb_last = is_receiver_threshgb;
  is_receiver_threshgb = false;
  is_receiver_quanta_threshgb_last = is_receiver_quanta_threshgb;
  is_receiver_quanta_threshgb = false;
  is_receiver_shadedgb_last = is_receiver_shadedgb;
  is_receiver_shadedgb = false;	
  is_receiver_gb_same_locus_last = is_receiver_gb_same_locus;
  is_receiver_gb_same_locus = false;
  
  (void) is_modifier;
  (void) is_modified;
  (void) is_fertile;
  (void) is_mutated;
  (void) is_multi_thread;
  (void) parent_true;
  (void) parent_sex;
  (void) parent_cross_num;
  
  // Reset child info...
  (void) copy_true;
  (void) divide_sex;
  (void) mate_select_id;
  (void) cross_num;
  last_child_fertile = child_fertile;
  child_fertile     = true;
  (void) child_copied_size;;
  
  // A few final changes if the parent was supposed to be be considered
  // a second child on the divide.
  if ((m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) || (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_BIRTH)) {
    gestation_start = 0;
    cpu_cycles_used = 0;
    time_used = 0;
    neutral_metric += m_world->GetRandom().GetRandNormal();
  }
  
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() == GENERATION_INC_BOTH) generation++;
  
  // Reset Task States
  tArray<cTaskState*> task_states(0);
  m_task_states.GetValues(task_states);
  for (int i = 0; i < task_states.GetSize(); i++) delete task_states[i];
  m_task_states.ClearAll();
}

/**
 * This function runs whenever a *test* CPU divides. It processes much of
 * the information for that CPU in order to actively reflect its executed
 * and copied size in its merit.
 **/

void cPhenotype::TestDivideReset(const cSequence & _genome)
{
  assert(time_used > 0);
  assert(initialized == true);
  
  // Update these values as needed...
  int cur_merit_base = CalcSizeMerit();
  const int merit_default_bonus = m_world->GetConfig().MERIT_DEFAULT_BONUS.Get();
  if (merit_default_bonus) {
    cur_bonus = merit_default_bonus;
  }
  merit = cur_merit_base * cur_bonus;
  
  if(m_world->GetConfig().INHERIT_MERIT.Get() == 0)
    merit = cur_merit_base;
  
  genome_length   = _genome.GetSize();
  (void) copied_size;                            // Unchanged
  (void) executed_size;                          // Unchanged
  gestation_time  = time_used - gestation_start;
  gestation_start = time_used;
  fitness         = CalcFitness(cur_merit_base, cur_bonus, gestation_time, cpu_cycles_used);
  (void) div_type; 				// Unchanged
  
  // Lock in cur values as last values.
  last_merit_base           = cur_merit_base;
  last_bonus                = cur_bonus;
  last_cpu_cycles_used      = cpu_cycles_used;
  last_num_errors           = cur_num_errors;
  last_num_donates          = cur_num_donates;
  last_task_count           = cur_task_count;
  last_host_tasks           = cur_host_tasks;
  last_para_tasks           = cur_para_tasks;
  last_internal_task_count  = cur_internal_task_count;
  last_task_quality         = cur_task_quality;
  last_task_value			= cur_task_value;
  last_internal_task_quality= cur_internal_task_quality;
  last_rbins_total          = cur_rbins_total;
  last_rbins_avail          = cur_rbins_avail;
  last_collect_spec_counts  = cur_collect_spec_counts;
  last_reaction_count       = cur_reaction_count;
  last_reaction_add_reward  = cur_reaction_add_reward;
  last_inst_count           = cur_inst_count;
  last_sense_count          = cur_sense_count;  
  last_child_germline_propensity = cur_child_germline_propensity;
  
  // Reset cur values.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cpu_cycles_used = 0;
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  cur_host_tasks.SetAll(0);
  //LZ: figure out when and where to reset cur_para_tasks, depending on the divide method, and resonable assumptions
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT)
  {
    last_para_tasks = cur_para_tasks;
    cur_para_tasks.SetAll(0);
  }
  cur_internal_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_task_value.SetAll(0);
  cur_internal_task_quality.SetAll(0);
  cur_rbins_total.SetAll(0);  // total resources collected in lifetime
  if (m_world->GetConfig().SPLIT_ON_DIVIDE.Get()) {
    // resources available are split in half -- the offspring gets the other half
    for (int i = 0; i < cur_rbins_avail.GetSize(); i++) {cur_rbins_avail[i] /= 2.0;}
  }
  else {
    cur_rbins_avail.SetAll(0);
  }
  cur_collect_spec_counts.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_sense_count.SetAll(0); 
  cur_task_time.SetAll(0.0);
  sensed_resources.SetAll(-1.0);
  cur_trial_fitnesses.Resize(0); 
  cur_trial_bonuses.Resize(0); 
  cur_trial_times_used.Resize(0); 
  trial_time_used = 0;
  trial_cpu_cycles_used = 0;
  cur_child_germline_propensity = m_world->GetConfig().DEMES_DEFAULT_GERMLINE_PROPENSITY.Get();
  
  // Setup other miscellaneous values...
  num_divides++;
  generation++;
  (void) time_used;
  (void) age;
  (void) fault_desc;
  (void) neutral_metric;
  life_fitness = fitness; 
  exec_time_born += gestation_time;  //@MRR See DivideReset 
  birth_update  = m_world->GetStats().GetUpdate();
	num_new_unique_reactions = 0;
	last_task_id             = -1;
	res_consumed             = 0;
  
  num_thresh_gb_donations_last = num_thresh_gb_donations;
  num_thresh_gb_donations = 0;
  num_quanta_thresh_gb_donations_last = num_quanta_thresh_gb_donations;
  num_quanta_thresh_gb_donations = 0;
  num_shaded_gb_donations_last = num_shaded_gb_donations;
  num_shaded_gb_donations = 0;
  num_donations_locus_last = num_donations_locus;
  num_donations_locus = 0;
	
  // Leave flags alone...
  (void) is_injected;
  is_donor_last = is_donor_cur;
  is_donor_cur = false;
  is_donor_rand_last = is_donor_rand;
  is_donor_rand = false;
  is_donor_null_last = is_donor_null;
  is_donor_null = false;
  is_donor_kin_last = is_donor_kin;
  is_donor_kin = false;
  is_donor_edit_last = is_donor_edit;
  is_donor_edit = false;
  is_donor_gbg_last = is_donor_gbg;
  is_donor_gbg = false;
  is_donor_truegb_last = is_donor_truegb;
  is_donor_truegb = false;
  is_donor_threshgb_last = is_donor_threshgb;
  is_donor_threshgb = false;
  is_donor_quanta_threshgb_last = is_donor_quanta_threshgb;
  is_donor_quanta_threshgb = false;
  is_donor_shadedgb_last = is_donor_shadedgb;
  is_donor_shadedgb = false;
  is_donor_locus_last = is_donor_locus;
  is_donor_locus.SetAll(false);
  
  is_receiver_last = is_receiver;
  is_receiver = false;
  is_receiver_rand = false;
  is_receiver_kin_last = is_receiver_kin;
  is_receiver_kin = false;
  is_receiver_edit_last = is_receiver_edit;
  is_receiver_edit = false;
  is_receiver_gbg = false;
  is_receiver_truegb_last = is_receiver_truegb;
  is_receiver_truegb = false;
  is_receiver_threshgb_last = is_receiver_threshgb;
  is_receiver_threshgb = false;
  is_receiver_quanta_threshgb_last = is_receiver_quanta_threshgb;
  is_receiver_quanta_threshgb = false;
  is_receiver_shadedgb_last = is_receiver_shadedgb;
  is_receiver_shadedgb = false;	
  is_receiver_gb_same_locus_last = is_receiver_gb_same_locus;
  is_receiver_gb_same_locus = false;
  
  (void) is_modifier;
  (void) is_modified;
  (void) is_fertile;
  (void) is_mutated;
  (void) is_multi_thread;
  (void) parent_true;
  (void) parent_sex;
  (void) parent_cross_num;
  
  // Reset child info...
  (void) copy_true;
  (void) divide_sex;
  (void) mate_select_id;
  (void) cross_num;
  (void) child_fertile;
  (void) last_child_fertile;
  (void) child_copied_size;
}


/**
 * This function is run when an organism is being forced to replicate, but
 * not at the end of its replication cycle.
 *
 * Assumptions:
 *   - new organism is an exact clone of the parent, with *same* last info.
 *   - this is the first method run on an otherwise freshly built phenotype.
 **/

void cPhenotype::SetupClone(const cPhenotype & clone_phenotype)
{
  // Copy divide values from parent, which should already be setup.
  merit           = clone_phenotype.merit;
  
  energy_store    = clone_phenotype.energy_store;
  energy_tobe_applied = 0.0;
  energy_testament = 0.0;
  energy_received_buffer = 0.0;
  
  if(m_world->GetConfig().INHERIT_EXE_RATE.Get() == 0)
    executionRatio = 1.0;
  else 
    executionRatio = clone_phenotype.executionRatio;
  
  genome_length   = clone_phenotype.genome_length;
  copied_size     = clone_phenotype.copied_size;
  // copied_size     = clone_phenotype.child_copied_size;
  executed_size   = clone_phenotype.executed_size;
  gestation_time  = clone_phenotype.gestation_time;
  gestation_start = 0;
  fitness         = clone_phenotype.fitness;
  div_type        = clone_phenotype.div_type;
  
  assert(genome_length > 0);
  assert(copied_size > 0);
  assert(gestation_time >= 0); //@JEB 0 valid for some fitness methods
  assert(div_type > 0);
  
  // Initialize current values, as neeeded.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cpu_cycles_used = 0;
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  cur_host_tasks.SetAll(0);
  cur_para_tasks.SetAll(0);
  cur_internal_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_rbins_total.SetAll(0);
  cur_rbins_avail.SetAll(0);
  cur_collect_spec_counts.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_sense_count.SetAll(0);  
  cur_task_time.SetAll(0.0);
  for (int j = 0; j < sensed_resources.GetSize(); j++)
    sensed_resources[j] =  clone_phenotype.sensed_resources[j];
  cur_trial_fitnesses.Resize(0); 
  cur_trial_bonuses.Resize(0); 
  cur_trial_times_used.Resize(0); 
  trial_time_used = 0;
  trial_cpu_cycles_used = 0;
  cur_child_germline_propensity = m_world->GetConfig().DEMES_DEFAULT_GERMLINE_PROPENSITY.Get();
  
  // Copy last values from parent
  last_merit_base          = clone_phenotype.last_merit_base;
  last_bonus               = clone_phenotype.last_bonus;
  last_cpu_cycles_used     = clone_phenotype.last_cpu_cycles_used;
  last_num_errors          = clone_phenotype.last_num_errors;
  last_num_donates         = clone_phenotype.last_num_donates;
  last_task_count          = clone_phenotype.last_task_count;
  last_host_tasks          = clone_phenotype.last_host_tasks;
  last_para_tasks          = clone_phenotype.last_para_tasks;
  last_internal_task_count = clone_phenotype.last_internal_task_count;
  last_rbins_total         = clone_phenotype.last_rbins_total;
  last_rbins_avail         = clone_phenotype.last_rbins_avail;
  last_collect_spec_counts = clone_phenotype.last_collect_spec_counts;
  last_reaction_count      = clone_phenotype.last_reaction_count;
  last_reaction_add_reward = clone_phenotype.last_reaction_add_reward;
  last_inst_count          = clone_phenotype.last_inst_count;
  last_sense_count         = clone_phenotype.last_sense_count;  
  last_fitness             = CalcFitness(last_merit_base, last_bonus, gestation_time, last_cpu_cycles_used);
  last_child_germline_propensity = clone_phenotype.last_child_germline_propensity;
  
  // Setup other miscellaneous values...
  num_divides     = 0;
  num_divides_failed = 0;
  generation      = clone_phenotype.generation;
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
  cpu_cycles_used = 0;
  time_used       = 0;
  age             = 0;
  fault_desc      = "";
  neutral_metric  = clone_phenotype.neutral_metric + m_world->GetRandom().GetRandNormal();
  life_fitness    = fitness; 
  exec_time_born  = 0;
  birth_update    = m_world->GetStats().GetUpdate();
	num_new_unique_reactions = clone_phenotype.num_new_unique_reactions;
	last_task_id             = clone_phenotype.last_task_id;
	res_consumed             = clone_phenotype.res_consumed;
  
  num_thresh_gb_donations_last = clone_phenotype.num_thresh_gb_donations_last;
  num_thresh_gb_donations  = clone_phenotype.num_thresh_gb_donations;
  num_quanta_thresh_gb_donations_last = clone_phenotype.num_quanta_thresh_gb_donations_last;
  num_quanta_thresh_gb_donations  = clone_phenotype.num_quanta_thresh_gb_donations;
  num_shaded_gb_donations_last = clone_phenotype.num_shaded_gb_donations_last;
  num_shaded_gb_donations  = clone_phenotype.num_shaded_gb_donations;
  num_donations_locus = clone_phenotype.num_donations_locus;
  num_donations_locus_last = clone_phenotype.num_donations_locus_last;
	
  // Setup flags...
  is_injected   = false;
  is_donor_last = clone_phenotype.is_donor_last;
  is_donor_cur  = clone_phenotype.is_donor_cur;
  is_receiver = false;
  is_donor_rand_last = clone_phenotype.is_donor_rand_last;
  is_donor_rand  = clone_phenotype.is_donor_rand;
  is_donor_null_last = clone_phenotype.is_donor_null_last;
  is_donor_null  = clone_phenotype.is_donor_null;
  is_donor_kin_last = clone_phenotype.is_donor_kin_last;
  is_donor_kin  = clone_phenotype.is_donor_kin;
  is_donor_edit_last = clone_phenotype.is_donor_edit_last;
  is_donor_edit  = clone_phenotype.is_donor_edit;
  is_donor_gbg_last = clone_phenotype.is_donor_gbg_last;
  is_donor_gbg  = clone_phenotype.is_donor_gbg;
  is_donor_truegb_last = clone_phenotype.is_donor_truegb_last;
  is_donor_truegb  = clone_phenotype.is_donor_truegb;
  is_donor_threshgb_last = clone_phenotype.is_donor_threshgb_last;
  is_donor_threshgb  = clone_phenotype.is_donor_threshgb;
  is_donor_quanta_threshgb_last = clone_phenotype.is_donor_quanta_threshgb_last;
  is_donor_quanta_threshgb  = clone_phenotype.is_donor_quanta_threshgb;
  is_donor_shadedgb_last = clone_phenotype.is_donor_shadedgb_last;
  is_donor_shadedgb  = clone_phenotype.is_donor_shadedgb;
  is_donor_locus_last = clone_phenotype.is_donor_locus_last;
  is_donor_locus = clone_phenotype.is_donor_locus;
  
  is_receiver = clone_phenotype.is_receiver;
  is_receiver_last = clone_phenotype.is_receiver_last;
  is_receiver_rand = clone_phenotype.is_receiver_rand;
  is_receiver_kin = clone_phenotype.is_receiver_kin;
  is_receiver_kin_last = clone_phenotype.is_receiver_kin_last;
  is_receiver_edit = clone_phenotype.is_receiver_edit;
  is_receiver_edit_last = clone_phenotype.is_receiver_edit_last;
  is_receiver_gbg = clone_phenotype.is_receiver_gbg;
  is_receiver_truegb = clone_phenotype.is_receiver_truegb;
  is_receiver_truegb_last = clone_phenotype.is_receiver_truegb_last;
  is_receiver_threshgb = clone_phenotype.is_receiver_threshgb;
  is_receiver_threshgb_last = clone_phenotype.is_receiver_threshgb_last;
  is_receiver_quanta_threshgb = clone_phenotype.is_receiver_quanta_threshgb;
  is_receiver_quanta_threshgb_last = clone_phenotype.is_receiver_quanta_threshgb_last;
  is_receiver_shadedgb = clone_phenotype.is_receiver_shadedgb;
  is_receiver_shadedgb_last = clone_phenotype.is_receiver_shadedgb_last;
  is_receiver_gb_same_locus = clone_phenotype.is_receiver_gb_same_locus;
  
  is_modifier   = false;
  is_modified   = false;
  is_fertile    = clone_phenotype.last_child_fertile;
  is_mutated    = false;
  is_multi_thread = clone_phenotype.is_multi_thread;
  parent_true   = clone_phenotype.copy_true;
  parent_sex    = clone_phenotype.divide_sex;
  parent_cross_num    = clone_phenotype.cross_num;
  to_die = false;
  to_delete = false;
  is_energy_requestor = false;
  is_energy_donor = false;
  is_energy_receiver = false;
  has_used_donated_energy = false;
  has_open_energy_request = false;
  
  // Setup child info...
  copy_true          = false;
  divide_sex         = false;
  mate_select_id     = 0;
  cross_num          = 0;
  last_child_fertile = is_fertile;
  child_fertile      = true;
  child_copied_size  = 0;
  permanent_germline_propensity = clone_phenotype.permanent_germline_propensity;
  
  initialized = true;
}




bool cPhenotype::TestInput(tBuffer<int>& inputs, tBuffer<int>& outputs)
{
  assert(initialized == true);
  // For the moment, lets not worry about inputs...
  return false; // Nothing happened...
}

bool cPhenotype::TestOutput(cAvidaContext& ctx, cTaskContext& taskctx,
                            const tArray<double>& res_in, const tArray<double>& rbins_in, tArray<double>& res_change,
                            tArray<cString>& insts_triggered, bool is_parasite)
{
  assert(initialized == true);
  taskctx.SetTaskStates(&m_task_states);
  
  const cEnvironment& env = m_world->GetEnvironment();
  const int num_resources = env.GetResourceLib().GetSize();
  const int num_tasks = env.GetNumTasks();
  const int num_reactions = env.GetReactionLib().GetSize();
  
  // For refractory period @WRE 03-20-07
  const int cur_update_time = m_world->GetStats().GetUpdate();
  const double task_refractory_period = m_world->GetConfig().TASK_REFRACTORY_PERIOD.Get();
  double refract_factor;
  
  if (!m_reaction_result) m_reaction_result = new cReactionResult(num_resources, num_tasks, num_reactions);
  cReactionResult& result = *m_reaction_result;
  
  // Run everything through the environment.
  bool found = env.TestOutput(ctx, result, taskctx, eff_task_count, cur_reaction_count, res_in, rbins_in, is_parasite); //NEED different eff_task_count and cur_reaction_count for deme resource
  
  // If nothing was found, stop here.
  if (found == false) {
    result.Invalidate();
    res_change.SetAll(0.0);
    return false;  // Nothing happened.
  }
  
  // Update the phenotype with the results...
  // Start with updating task and reaction counters
  for (int i = 0; i < num_tasks; i++) {
    // Calculate refractory period factor @WRE
    // Modify TaskQuality amount based on refractory period
    // Logistic equation using refractory period
    // in update units from configuration file.  @WRE 03-20-07, 04-17-07
		
    if (task_refractory_period == 0.0) {
      refract_factor = 1.0;
    } else {
      refract_factor = 1.0 - (1.0 / (1.0 + exp((cur_update_time - cur_task_time[i]) - task_refractory_period * 0.5)));
    }
		
    if (result.TaskDone(i) == true) {
      cur_task_count[i]++;
      eff_task_count[i]++;
      
      //update parasite/host task tracking appropriately
      if(is_parasite)
      {
        cur_para_tasks[i]++;
      }
      else
      {
        cur_host_tasks[i]++;
      }
      
      
      if (result.UsedEnvResource() == false) { cur_internal_task_count[i]++; }
      
      // if we want to generate and age-task histogram
      if (m_world->GetConfig().AGE_POLY_TRACKING.Get()) {
        m_world->GetStats().AgeTaskEvent(taskctx.GetOrganism()->GetID(), i, time_used);
      }
    }
    
    if (result.TaskQuality(i) > 0) {
      cur_task_quality[i] += result.TaskQuality(i) * refract_factor;
      if (result.UsedEnvResource() == false) {
        cur_internal_task_quality[i] += result.TaskQuality(i) * refract_factor;
      }
    }
		
    cur_task_value[i] = result.TaskValue(i);
    cur_task_time[i] = cur_update_time; // Find out time from context
  }
	
  for (int i = 0; i < num_tasks; i++) {
    if (result.TaskDone(i) && !last_task_count[i]) {
      m_world->GetStats().AddNewTaskCount(i);
      int prev_num_tasks = 0;
      int cur_num_tasks = 0;
      for (int j=0; j< num_tasks; j++) {
        if (last_task_count[j]>0) prev_num_tasks++;
        if (cur_task_count[j]>0) cur_num_tasks++;
      }
      m_world->GetStats().AddOtherTaskCounts(i, prev_num_tasks, cur_num_tasks);
    }
  }
  
  for (int i = 0; i < num_reactions; i++) {
    cur_reaction_add_reward[i] += result.GetReactionAddBonus(i);
    if (result.ReactionTriggered(i) && last_reaction_count[i]==0) {
      m_world->GetStats().AddNewReactionCount(i);
    }
    if (result.ReactionTriggered(i) == true) {
      // If the organism has not performed this task,
      // then consider it to be a task switch.
      // If applicable, add in the penalty.
      switch(m_world->GetConfig().TASK_SWITCH_PENALTY_TYPE.Get()) {
        case 0: { // no penalty
          break;
        }
        case 1: { // "learning" cost
          if (cur_reaction_count[i] == 1) {
            ++num_new_unique_reactions;
          }
          break;
        }
        case 2: { // "retooling" cost
          if (last_task_id == -1) {
            last_task_id = i;
          }					
          if (last_task_id != i) {
            num_new_unique_reactions++;
            last_task_id = i;
          }
          break;
        }
        case 3: { // centrifuge
          // task switching cost is calculated based on 
          // the distance between the two tasks.
          
          int distance = abs(i - last_task_id);
          num_new_unique_reactions += distance;
          last_task_id = i;
          
          break;
        }
        default: {
          assert(false);
          break;
        }
      }
    }		
  }
  
  // Update the merit bonus
  cur_bonus *= result.GetMultBonus();
  cur_bonus += result.GetAddBonus();
  
  // update the germline propensity
  cur_child_germline_propensity += result.GetAddGermline();
  cur_child_germline_propensity *= result.GetMultGermline();
  
  // Update deme merit (guard against running in the test CPU, where there is
  // no deme object.  Don't touch deme merit if there is no deme frac component.
  cDeme* deme = taskctx.GetOrganism()->GetDeme();
  if (deme) {
    
    if (result.GetActiveDeme()) {
      double deme_bonus = deme->GetHeritableDemeMerit().GetDouble();
      deme_bonus *= result.GetMultDemeBonus();
      deme_bonus += result.GetAddDemeBonus();
      deme->UpdateHeritableDemeMerit(deme_bonus);
    }
    
    //also count tasks/reactions
    for (int i = 0; i < num_tasks; i++) {
      if (result.TaskDone(i) == true) deme->AddCurTask(i);
    }
    for (int i = 0; i < num_reactions; i++) {
      if (result.ReactionTriggered(i) == true) deme->AddCurReaction(i);
    }
  }
  
  // Update the energy bonus
  cur_energy_bonus += result.GetAddEnergy();
  
  // Denote consumed resources...
  for (int i = 0; i < res_in.GetSize(); i++) {
    res_change[i] = result.GetProduced(i) - result.GetConsumed(i);
    res_consumed += result.GetConsumed(i);
  }
  
  // Update rbins as necessary
  if (result.UsedEnvResource() == false) {
    double rbin_diff;
    for (int i = 0; i < num_resources; i++) {
      rbin_diff = result.GetInternalConsumed(i);
      cur_rbins_avail[i] -= rbin_diff;
      if(rbin_diff > 0) { cur_rbins_total[i] += rbin_diff; }
    }
  }
  
  // Save the instructions that should be triggered...
  insts_triggered = result.GetInstArray();
  
  //Put in detected resources
  for (int j = 0; j < res_in.GetSize(); j++) {
    if(result.GetDetected(j) != -1.0) {
      sensed_resources[j] = result.GetDetected(j);
    }
  }
  
  //Kill any cells that did lethal reactions
  to_die = result.GetLethal();
  
  // Sterilize organisms that have performed a sterilizing task.
  if(result.GetSterilize()) {
    is_fertile = false;
  }
  
  result.Invalidate();
  return true;
}


void cPhenotype::Sterilize()
{
  is_fertile = false;
}


void cPhenotype::PrintStatus(ostream& fp) const
{
  fp << "  MeritBase:"
  << CalcSizeMerit()
  << " Bonus:" << cur_bonus
  << " Errors:" << cur_num_errors
  << " Donates:" << cur_num_donates
  << '\n';
  
  fp << "  Task Count (Quality):";
  for (int i = 0; i < cur_task_count.GetSize(); i++) {
    fp << " " << cur_task_count[i] << " (" << cur_task_quality[i] << ")";
  }
  fp << '\n';
  
  // if using resoruce bins, print the relevant stats
  if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
    fp << "  Used-Internal-Resources Task Count (Quality):";
    for (int i = 0; i < cur_internal_task_count.GetSize(); i++) {
      fp << " " << cur_internal_task_count[i] << " (" << cur_internal_task_quality[i] << ")";
    }
    fp << endl;
 		
    fp << "  Available Internal Resource Bin Contents (Total Ever Collected):";
    for(int i = 0; i < cur_rbins_avail.GetSize(); i++) {
      fp << " " << cur_rbins_avail[i] << " (" << cur_rbins_total[i] << ")";
    }
    fp << endl;
  }
}

int cPhenotype::CalcSizeMerit() const
{
  assert(genome_length > 0);
  assert(copied_size > 0);
  
  int out_size;
  
  switch (m_world->GetConfig().BASE_MERIT_METHOD.Get()) {
    case BASE_MERIT_COPIED_SIZE:
      out_size = copied_size;
      break;
    case BASE_MERIT_EXE_SIZE:
      out_size = executed_size;
      break;
    case BASE_MERIT_FULL_SIZE:
      out_size = genome_length;
      break;
    case BASE_MERIT_LEAST_SIZE:
      out_size = genome_length;
      if (out_size > copied_size) out_size = copied_size;
      if (out_size > executed_size)    out_size = executed_size;
      break;
    case BASE_MERIT_SQRT_LEAST_SIZE:
      out_size = genome_length;
      if (out_size > copied_size) out_size = copied_size;
      if (out_size > executed_size)    out_size = executed_size;
      out_size = (int) sqrt((double) out_size);
      break;
    case BASE_MERIT_NUM_BONUS_INST:
      if (m_world->GetConfig().FITNESS_VALLEY.Get()){
        if (bonus_instruction_count >= m_world->GetConfig().FITNESS_VALLEY_START.Get() && 
            bonus_instruction_count <= m_world->GetConfig().FITNESS_VALLEY_STOP.Get()){
          out_size = 1;
          break;
        }            
      }
      if (m_world->GetConfig().MERIT_BONUS_EFFECT.Get()>0) {
        out_size = 1 + bonus_instruction_count;
      }
      else if (m_world->GetConfig().MERIT_BONUS_EFFECT.Get()<0) {
        out_size = genome_length - (bonus_instruction_count -1);
      }
      else {
        out_size = 1; // The extra 1 point is so the orgs are not jilted by the scheduler.
      }
      break;
    case BASE_MERIT_GESTATION_TIME:
      out_size = cpu_cycles_used;
      break;
    case BASE_MERIT_CONST:
    default:
      out_size = m_world->GetConfig().BASE_CONST_MERIT.Get();
      break;
  }
  
  return out_size;
}

double cPhenotype::CalcCurrentMerit() const
{
  int merit_base = CalcSizeMerit();
  
  return merit_base * cur_bonus;  
}


double cPhenotype::CalcFitness(double _merit_base, double _bonus, int _gestation_time, int _cpu_cycles) const
{
  double out_fitness = 0;
  switch (m_world->GetConfig().FITNESS_METHOD.Get()) {
    case 0: // Normal
      assert(_gestation_time > 0);
      out_fitness = _merit_base * _bonus / _gestation_time;
      break;
      
    case 1: // Sigmoidal returns (should be used with an additive reward)
    {
      assert(_gestation_time > 0);
      out_fitness = 0;
      //Note: this operates on accumulated bonus and ignores the default bonus.
      double converted_bonus = (_bonus - m_world->GetConfig().DEFAULT_BONUS.Get()) * m_world->GetConfig().FITNESS_COEFF_2.Get() / (1 + _bonus * m_world->GetConfig().FITNESS_COEFF_2.Get() ) ;
      out_fitness = _merit_base * exp(converted_bonus * log(m_world->GetConfig().FITNESS_COEFF_1.Get())) / _gestation_time;
    }
      break;
      
    case 2: //Activity of one enzyme in pathway altered (with diminishing returns and a cost for each executed instruction)
    {
      out_fitness = 0;
      double net_bonus = _bonus +  - m_world->GetConfig().DEFAULT_BONUS.Get();
      out_fitness = net_bonus / (net_bonus + 1)* exp (_gestation_time * log(1 - m_world->GetConfig().FITNESS_COEFF_1.Get())); 
    }
      break;
      
    default:
      cout << "Unknown FITNESS_METHOD!" << endl;
      exit(1);
  }
  
  return out_fitness;
}

void cPhenotype::ReduceEnergy(const double cost) {
  SetEnergy(energy_store - cost);
}

void cPhenotype::SetEnergy(const double value) {
  energy_store = max(0.0, min(value, m_world->GetConfig().ENERGY_CAP.Get()));
}

void cPhenotype::DoubleEnergyUsage() {
  executionRatio *= 2.0;
}

void cPhenotype::HalveEnergyUsage() {
  executionRatio *= 0.5;
}

void cPhenotype::DefaultEnergyUsage() {
  executionRatio = 1.0;
}

void cPhenotype::DivideFailed() {
  num_divides_failed++;
}


/**
 Credit organism with energy reward, but only update energy store if APPLY_ENERGY_METHOD = "on task completion" (1)
 */
void cPhenotype::RefreshEnergy() {
  if(cur_energy_bonus > 0) {
    if(m_world->GetConfig().APPLY_ENERGY_METHOD.Get() == 0 || // on divide
       m_world->GetConfig().APPLY_ENERGY_METHOD.Get() == 2) {  // on sleep
      energy_tobe_applied += cur_energy_bonus;
    } else if(m_world->GetConfig().APPLY_ENERGY_METHOD.Get() == 1) {
      SetEnergy(energy_store + cur_energy_bonus);
      m_world->GetStats().SumEnergyTestamentAcceptedByOrganisms().Add(energy_testament);
      energy_testament = 0.0;
    } else {
      cerr<< "Unknown APPLY_ENERGY_METHOD value " << m_world->GetConfig().APPLY_ENERGY_METHOD.Get();
      exit(-1);
    }
    cur_energy_bonus = 0;
  }
}

void cPhenotype::ApplyToEnergyStore() {
  SetEnergy(energy_store + energy_tobe_applied);
  m_world->GetStats().SumEnergyTestamentAcceptedByOrganisms().Add(energy_testament);
  energy_testament = 0.0;
  energy_tobe_applied = 0.0;
  energy_testament = 0.0;
}

void cPhenotype::EnergyTestament(const double value) {
  assert(value > 0.0);
  energy_tobe_applied += value;
  energy_testament += value;
} //! external energy given to organism


void cPhenotype::ApplyDonatedEnergy() {
  double energy_cap = m_world->GetConfig().ENERGY_CAP.Get();
  
  if((energy_store + energy_received_buffer) >= energy_cap) {
    IncreaseEnergyApplied(energy_cap - energy_store);
    SetEnergy(energy_store + (energy_cap - energy_received_buffer));
  } else {
    IncreaseEnergyApplied(energy_received_buffer);
    SetEnergy(energy_store + energy_received_buffer);
  }
  
  IncreaseNumEnergyApplications();
  SetHasUsedDonatedEnergy();
  
  energy_received_buffer = 0.0;
  
} //End AppplyDonatedEnergy()


void cPhenotype::ReceiveDonatedEnergy(const double donation) {
  assert(donation >= 0.0);  
  energy_received_buffer += donation;
  IncreaseEnergyReceived(donation);
  SetIsEnergyReceiver();
  IncreaseNumEnergyReceptions();
} //End ReceiveDonatedEnergy()


double cPhenotype::ExtractParentEnergy() {
  assert(m_world->GetConfig().ENERGY_ENABLED.Get() > 0);
  // energy model config variables
  double energy_given_at_birth = m_world->GetConfig().ENERGY_GIVEN_AT_BIRTH.Get();
  double frac_parent_energy_given_at_birth = m_world->GetConfig().FRAC_PARENT_ENERGY_GIVEN_TO_ORG_AT_BIRTH.Get();
  double frac_energy_decay_at_birth = m_world->GetConfig().FRAC_ENERGY_DECAY_AT_ORG_BIRTH.Get();
  double energy_cap = m_world->GetConfig().ENERGY_CAP.Get();
  
  // apply energy if APPLY_ENERGY_METHOD is set to "on divide" (0)
  if(m_world->GetConfig().APPLY_ENERGY_METHOD.Get() == 0) {
    RefreshEnergy();
    ApplyToEnergyStore();
  }
  
  // decay of energy in parent
  ReduceEnergy(GetStoredEnergy() * frac_energy_decay_at_birth);
  
  // calculate energy to be given to child
  double child_energy = max(0.0, min(GetStoredEnergy() * frac_parent_energy_given_at_birth + energy_given_at_birth, energy_cap));
  assert(GetStoredEnergy()>0.0);
  // adjust energy in parent
  ReduceEnergy(child_energy - 2*energy_given_at_birth); // 2*energy_given_at_birth: 1 in child_energy & 1 for parent
  
  //TODO: add energy_given_at_birth to Stored_energy
  cMerit parentMerit(ConvertEnergyToMerit(GetStoredEnergy() * GetEnergyUsageRatio()));
	if(parentMerit.GetDouble() > 0.0)
		SetMerit(parentMerit);
  else
		SetToDie();
	
  return child_energy;
}

// Save the current fitness and reset relevant parts of the phenotype
void cPhenotype::NewTrial()
{ 
  //Return if a complete trial has not occurred.
  //(This will happen if CompeteOrganisms was called before in the same update
  if (trial_cpu_cycles_used == 0) return;
  
  //Record the merit of this trial
  fitness = CalcFitness( GetCurMeritBase(), GetCurBonus() , trial_time_used, trial_cpu_cycles_used); // This is a per-trial fitness @JEB
  cur_trial_fitnesses.Push(fitness);
  cur_trial_bonuses.Push(GetCurBonus());
  cur_trial_times_used.Push(trial_time_used);
  
  //The rest of the function, resets the phenotype like DivideReset(), but without
  //incrementing the generation or child statistics.
  
  //Most importantly, this does (below):
  // trial_time_used = 0;
  // trial_cpu_cycles_used = 0;
  // SetCurBonus(m_world->GetConfig().DEFAULT_BONUS.Get());
  
  // Update these values as needed...
  int cur_merit_base = CalcSizeMerit();
  
  // If we are resetting the current merit, do it here
  // and it will also be propagated to the child
  int merit_default_bonus = m_world->GetConfig().MERIT_DEFAULT_BONUS.Get();
  if (merit_default_bonus) {
    cur_bonus = merit_default_bonus;
  }
  merit = cur_merit_base * cur_bonus;
  
  // update energy store
  energy_store += cur_energy_bonus;
  energy_store = m_world->GetConfig().ENERGY_GIVEN_AT_BIRTH.Get(); // We reset to what they had at birth
  cur_energy_bonus = 0;
  // to be perfectly accurate, this should be from a last_energy value??
  
  
  // genome_length   = _genome.GetSize();  //No child! @JEB
  (void) copied_size;          // Unchanged
  (void) executed_size;        // Unchanged
  gestation_time  = time_used - gestation_start;  //Keep gestation referring to actual replication time! @JEB
  gestation_start = time_used;                    //Keep gestation referring to actual replication time! @JEB
  // fitness         = merit.GetDouble() / gestation_time; //Use fitness measure that is per-trial @JEB
  
  // Lock in cur values as last values.
  last_merit_base           = cur_merit_base;
  last_bonus                = cur_bonus;
  last_cpu_cycles_used      = cpu_cycles_used;
  //TODO?  last_energy         = cur_energy_bonus;
  last_num_errors           = cur_num_errors;
  last_num_donates          = cur_num_donates;
  last_task_count           = cur_task_count;
  last_host_tasks           = cur_host_tasks;
  last_para_tasks           = cur_para_tasks;
  last_internal_task_count  = cur_internal_task_count;
  last_task_quality         = cur_task_quality;
  last_internal_task_quality= cur_internal_task_quality;
  last_task_value			      = cur_task_value;
  last_rbins_total          = cur_rbins_total;
  last_rbins_avail          = cur_rbins_avail;
  last_collect_spec_counts  = cur_collect_spec_counts;
  last_reaction_count       = cur_reaction_count;
  last_reaction_add_reward  = cur_reaction_add_reward;
  last_inst_count           = cur_inst_count;
  last_sense_count          = cur_sense_count;
  
  // Reset cur values.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cpu_cycles_used = 0;
  cur_energy_bonus = 0.0;
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  cur_host_tasks.SetAll(0);
  cur_para_tasks.SetAll(0);
  cur_internal_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_internal_task_quality.SetAll(0);
  cur_task_value.SetAll(0);
  cur_rbins_total.SetAll(0);
  cur_rbins_avail.SetAll(0);
  cur_collect_spec_counts.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_sense_count.SetAll(0);
  //cur_trial_fitnesses.Resize(0); Don't throw out the trial fitnesses! @JEB
  trial_time_used = 0;
  trial_cpu_cycles_used = 0;
  
  // Setup other miscellaneous values...
  num_divides++;
  (void) generation;
  (void) time_used;
  age             = 0;
  fault_desc      = "";
  (void) neutral_metric;
  life_fitness = fitness; 
  
  
  num_thresh_gb_donations_last = num_thresh_gb_donations;
  num_thresh_gb_donations = 0;
  num_quanta_thresh_gb_donations_last = num_quanta_thresh_gb_donations;
  num_quanta_thresh_gb_donations = 0;
  num_shaded_gb_donations_last = num_shaded_gb_donations;
  num_shaded_gb_donations = 0;
  num_donations_locus_last = num_donations_locus;
  num_donations_locus = 0;
	
  // Leave flags alone...
  (void) is_injected;
  is_donor_last = is_donor_cur;
  is_donor_cur = false;
  is_donor_rand_last = is_donor_rand;
  is_donor_rand = false;
  is_donor_null_last = is_donor_null;
  is_donor_null = false;
  is_donor_kin_last = is_donor_kin;
  is_donor_kin = false;
  is_donor_edit_last = is_donor_edit;
  is_donor_edit = false;
  is_donor_gbg_last = is_donor_gbg;
  is_donor_gbg = false;
  is_donor_truegb_last = is_donor_truegb;
  is_donor_truegb = false;
  is_donor_threshgb_last = is_donor_threshgb;
  is_donor_threshgb = false;
  is_donor_quanta_threshgb_last = is_donor_quanta_threshgb;
  is_donor_quanta_threshgb = false;
  is_donor_shadedgb_last = is_donor_shadedgb;
  is_donor_shadedgb = false;
  is_donor_locus_last = is_donor_locus;
  is_donor_locus.SetAll(false);
  
  is_receiver_last = is_receiver;
  is_receiver = false;
  is_receiver_rand = false;
  is_receiver_kin_last = is_receiver_kin;
  is_receiver_kin = false;
  is_receiver_edit_last = is_receiver_edit;
  is_receiver_edit = false;
  is_receiver_gbg = false;
  is_receiver_truegb_last = is_receiver_truegb;
  is_receiver_truegb = false;
  is_receiver_threshgb_last = is_receiver_threshgb;
  is_receiver_threshgb = false;
  is_receiver_quanta_threshgb_last = is_receiver_quanta_threshgb;
  is_receiver_quanta_threshgb = false;
  is_receiver_shadedgb_last = is_receiver_shadedgb;
  is_receiver_shadedgb = false;
  is_receiver_gb_same_locus_last = is_receiver_gb_same_locus;
  is_receiver_gb_same_locus = false;
  
  is_energy_requestor = false;
  is_energy_donor = false;
  is_energy_receiver = false;
  (void) is_modifier;
  (void) is_modified;
  (void) is_fertile;
  (void) is_mutated;
  (void) is_multi_thread;
  (void) parent_true;
  (void) parent_sex;
  (void) parent_cross_num;
}

/**
 * This function is run to reset an organism whose task counts (etc) have already been moved from cur to last
 * by another call (like NewTrial). It is a subset of DivideReset @JEB
 **/

void cPhenotype::TrialDivideReset(const cSequence & _genome)
{
  int cur_merit_base = CalcSizeMerit();
  
  // If we are resetting the current merit, do it here
  // and it will also be propagated to the child
  const int merit_default_bonus = m_world->GetConfig().MERIT_DEFAULT_BONUS.Get();
  if (merit_default_bonus) {
    cur_bonus = merit_default_bonus;
  }
  merit = cur_merit_base * cur_bonus;
  
  SetEnergy(energy_store + cur_energy_bonus);
  m_world->GetStats().SumEnergyTestamentAcceptedByOrganisms().Add(energy_testament);
  energy_testament = 0.0;
  
  genome_length   = _genome.GetSize();
  gestation_start = time_used;
  cur_trial_fitnesses.Resize(0); 
  cur_trial_bonuses.Resize(0); 
  cur_trial_times_used.Resize(0); 
  
  // Reset child info...
  (void) copy_true;
  (void) divide_sex;
  (void) mate_select_id;
  (void) cross_num;
  last_child_fertile = child_fertile;
  child_fertile     = true;
  (void) child_copied_size;
  
  // A few final changes if the parent was supposed to be be considered
  // a second child on the divide.
  if ((m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) || (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_BIRTH)) {    
    gestation_start = 0;
    cpu_cycles_used = 0;
    time_used = 0;
    neutral_metric += m_world->GetRandom().GetRandNormal();
  }
  
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() == GENERATION_INC_BOTH) generation++;
}

// C O M P A R I S O N    O P E R A T O R S

bool cPhenotype::operator<(const cPhenotype&  rhs) const
{
  if (this->GetMerit() < rhs.GetMerit())
    return true;
  
  if ( this->GetGestationTime() < rhs.GetGestationTime() )
    return true;
  
  tArray<int> lhsTasks = this->GetLastTaskCount();
  tArray<int> rhsTasks = rhs.GetLastTaskCount();
  for (int k = 0; k < lhsTasks.GetSize(); k++)
    if (lhsTasks[k] < rhsTasks[k])
      return true;
  
  return false;
}

bool cPhenotype::operator==(const cPhenotype& rhs) const
{
  if (this->GetMerit() != rhs.GetMerit())
    return false;
  
  if ( this->GetGestationTime() != rhs.GetGestationTime() )
    return false;
  
  tArray<int> lhsTasks = this->GetLastTaskCount();
  tArray<int> rhsTasks = rhs.GetLastTaskCount();
  for (int k = 0; k < lhsTasks.GetSize(); k++)
    if (lhsTasks[k] != rhsTasks[k])
      return false;
  
  return true;
}

bool cPhenotype::operator!=(const cPhenotype& rhs) const
{
  return !(*this == rhs);
}

bool cPhenotype::operator>(const cPhenotype&  rhs) const
{
  if (this->GetMerit() > rhs.GetMerit())
    return true;
  
  if ( this->GetGestationTime() > rhs.GetGestationTime() )
    return true;
  
  tArray<int> lhsTasks = this->GetLastTaskCount();
  tArray<int> rhsTasks = rhs.GetLastTaskCount();
  for (int k = 0; k < lhsTasks.GetSize(); k++)
    if (lhsTasks[k] > rhsTasks[k])
      return true;
  
  return false;
}

// Return an integer classifying the organism's energy level as -1=error,0=low,1=med,2=high
int cPhenotype::GetDiscreteEnergyLevel() const {
  double max_energy = m_world->GetConfig().ENERGY_CAP.Get();
  double high_pct = m_world->GetConfig().ENERGY_THRESH_HIGH.Get();
  double low_pct = m_world->GetConfig().ENERGY_THRESH_LOW.Get();
	
  assert(max_energy >= 0);
  assert(high_pct <= 1);
  assert(high_pct >= 0);
  assert(low_pct <= 1);
  assert(low_pct >= 0);
  assert(low_pct <= high_pct);
	
  if (energy_store < (low_pct * max_energy)) {
    return ENERGY_LEVEL_LOW;
  } else if ( (energy_store >= (low_pct * max_energy)) && (energy_store <= (high_pct * max_energy)) ) {
    return ENERGY_LEVEL_MEDIUM;
  } else if (energy_store > (high_pct * max_energy)) {
    return ENERGY_LEVEL_HIGH;
  } else {
    return -1;
  }			 
	
} //End GetDiscreteEnergyLevel()


double cPhenotype::ConvertEnergyToMerit(double energy) const
{
  assert(m_world->GetConfig().ENERGY_ENABLED.Get() == 1);
  
	double FIX_METABOLIC_RATE = m_world->GetConfig().FIX_METABOLIC_RATE.Get();
	if (FIX_METABOLIC_RATE > 0.0) return 100 * FIX_METABOLIC_RATE;
  
  return 100 * energy / m_world->GetConfig().NUM_CYCLES_EXC_BEFORE_0_ENERGY.Get();
}



double cPhenotype::GetResourcesConsumed() 
{
	double r = res_consumed; 
	res_consumed =0; 
	return r; 
}

//Deep copy parasite task count
void cPhenotype::SetLastParasiteTaskCount(tArray<int> oldParaPhenotype)
{
  assert(initialized == true);
  
  for(int i=0;i<oldParaPhenotype.GetSize();i++)
  {
    last_para_tasks[i] = oldParaPhenotype[i];
  }
}
