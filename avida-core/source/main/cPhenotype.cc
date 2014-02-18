/*
 *  cPhenotype.cc
 *  Avida
 *
 *  Called "phenotype.cc" prior to 12/5/05.
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

#include "cPhenotype.h"
#include "avida/systematics/Types.h"
#include "cEnvironment.h"
#include "cOrganism.h"
#include "cReactionResult.h"
#include "cTaskState.h"
#include "cWorld.h"

#include <fstream>

using namespace std;


cPhenotype::cPhenotype(cWorld* world, int parent_generation, int num_nops)
: m_world(world)
, initialized(false)
, cur_task_count(m_world->GetEnvironment().GetNumTasks())
, eff_task_count(m_world->GetEnvironment().GetNumTasks())
, cur_task_quality(m_world->GetEnvironment().GetNumTasks())  
, cur_task_value(m_world->GetEnvironment().GetNumTasks())  
, cur_reaction_count(m_world->GetEnvironment().GetReactionLib().GetSize())
, first_reaction_cycles(m_world->GetEnvironment().GetReactionLib().GetSize())
, first_reaction_execs(m_world->GetEnvironment().GetReactionLib().GetSize())
, cur_stolen_reaction_count(m_world->GetEnvironment().GetReactionLib().GetSize())
, cur_reaction_add_reward(m_world->GetEnvironment().GetReactionLib().GetSize())
, cur_sense_count(m_world->GetStats().GetSenseSize())
, sensed_resources(m_world->GetEnvironment().GetResDefLib().GetSize())
, cur_task_time(m_world->GetEnvironment().GetNumTasks())   // Added for tracking time; WRE 03-18-07
, m_reaction_result(NULL)
, last_task_count(m_world->GetEnvironment().GetNumTasks())
, last_task_quality(m_world->GetEnvironment().GetNumTasks())
, last_task_value(m_world->GetEnvironment().GetNumTasks())
, last_reaction_count(m_world->GetEnvironment().GetReactionLib().GetSize())
, last_reaction_add_reward(m_world->GetEnvironment().GetReactionLib().GetSize())  
, generation(0)
, birth_cell_id(0)
, av_birth_cell_id(0)
, birth_group_id(0)
, birth_forager_type(-1)
, last_task_id(-1)
, num_new_unique_reactions(0)
, res_consumed(0)
, last_task_time(0)

{ 
  if (parent_generation >= 0) {
    generation = parent_generation;
    if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
  }
}

cPhenotype::~cPhenotype()
{
  // Remove Task States
  for (Apto::Map<void*, cTaskState*>::ValueIterator it = m_task_states.Values(); it.Next();) delete (*it.Get());
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
  cur_num_errors           = in_phen.cur_num_errors;
  cur_task_count           = in_phen.cur_task_count;
  eff_task_count           = in_phen.eff_task_count;
  cur_task_quality         = in_phen.cur_task_quality;
  cur_task_value           = in_phen.cur_task_value;
  cur_reaction_count       = in_phen.cur_reaction_count;
  first_reaction_cycles    = in_phen.first_reaction_cycles;            
  first_reaction_execs     = first_reaction_execs;            
  cur_reaction_add_reward  = in_phen.cur_reaction_add_reward;     
  cur_inst_count           = in_phen.cur_inst_count;                 
  cur_from_sensor_count    = in_phen.cur_from_sensor_count;
  cur_group_attack_count    = in_phen.cur_group_attack_count;
  cur_top_pred_group_attack_count    = in_phen.cur_top_pred_group_attack_count;
  cur_killed_targets       = in_phen.cur_killed_targets;
  cur_sense_count          = in_phen.cur_sense_count;
  sensed_resources         = in_phen.sensed_resources;            
  cur_task_time            = in_phen.cur_task_time;
  cur_stolen_reaction_count       = in_phen.cur_stolen_reaction_count;
  
  // Dynamically allocated m_task_states requires special handling
  for (Apto::Map<void*, cTaskState*>::ConstIterator it = in_phen.m_task_states.Begin(); it.Next();) {
    cTaskState* new_ts = new cTaskState(**((*it.Get()).Value2()));
    m_task_states.Set((*it.Get()).Value1(), new_ts);
  }
  
  // 3. These mark the status of "in progess" variables at the last divide.
  last_merit_base          = in_phen.last_merit_base;       
  last_bonus               = in_phen.last_bonus;
  last_num_errors          = in_phen.last_num_errors;
  last_task_count          = in_phen.last_task_count;
  last_task_quality        = in_phen.last_task_quality;
  last_task_value          = in_phen.last_task_value;
  last_reaction_count      = in_phen.last_reaction_count;
  last_reaction_add_reward = in_phen.last_reaction_add_reward; 
  last_inst_count          = in_phen.last_inst_count;	  
  last_from_sensor_count   = in_phen.last_from_sensor_count;
  last_killed_targets      = in_phen.last_killed_targets;
  last_from_sensor_count   = in_phen.last_from_sensor_count;
  last_fitness             = in_phen.last_fitness;
  
  // 4. Records from this organisms life...
  num_divides              = in_phen.num_divides;   
  num_divides_failed       = in_phen.num_divides_failed;
  generation               = in_phen.generation;        
  cpu_cycles_used          = in_phen.cpu_cycles_used;   
  time_used                = in_phen.time_used; 
  num_execs                = in_phen.num_execs;
  age                      = in_phen.age;               
  neutral_metric           = in_phen.neutral_metric;
  life_fitness             = in_phen.life_fitness; 	
  exec_time_born           = in_phen.exec_time_born;
  birth_update             = in_phen.birth_update;
  num_new_unique_reactions = in_phen.num_new_unique_reactions;
  last_task_id             = in_phen.last_task_id;
  last_task_time           = in_phen.last_task_time;
  res_consumed             = in_phen.res_consumed; 
  
  // 5. Status Flags...  (updated at each divide)
  to_die                  = in_phen.to_die;		 
  to_delete               = in_phen.to_delete;        
  is_injected             = in_phen.is_injected;      
  is_clone                = in_phen.is_clone;
  is_modifier             = in_phen.is_modifier;
  is_modified             = in_phen.is_modified;      
  is_fertile              = in_phen.is_fertile;      
  is_mutated              = in_phen.is_mutated;       
  is_multi_thread         = in_phen.is_multi_thread; 
  parent_true             = in_phen.parent_true;     
  parent_sex              = in_phen.parent_sex;      
  parent_cross_num        = in_phen.parent_cross_num; 
  
  // 6. Child information...
  copy_true               = in_phen.copy_true;       
  divide_sex              = in_phen.divide_sex;       
  mate_select_id          = in_phen.mate_select_id;    
  cross_num               = in_phen.cross_num;     
  child_fertile           = in_phen.child_fertile;  
  last_child_fertile      = in_phen.last_child_fertile; 
  child_copied_size       = in_phen.child_copied_size;
  
  // 7. Permanent information...
  
  return *this;
}


/**
 * This function is run whenever a new organism is being constructed inside
 * of its parent.
 *
 * Assumptions:
 *     - parent_phenotype has had DivideReset run on it already!
 *     - this is the first method run on an otherwise freshly built phenotype.
 **/

void cPhenotype::SetupOffspring(const cPhenotype& parent_phenotype, const InstructionSequence& _genome)
{
  // Copy divide values from parent, which should already be setup.
  merit = parent_phenotype.merit;
  executionRatio = parent_phenotype.executionRatio;
  
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
  cur_num_errors  = 0;
  cur_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_task_value.SetAll(0);
  
  cur_reaction_count.SetAll(0);
  first_reaction_cycles.SetAll(-1);
  first_reaction_execs.SetAll(-1);
  cur_stolen_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_from_sensor_count.SetAll(0);
  for (int r = 0; r < cur_group_attack_count.GetSize(); r++) {
    cur_group_attack_count[r].SetAll(0);
    cur_top_pred_group_attack_count[r].SetAll(0);
  }
  cur_killed_targets.SetAll(0);
  cur_sense_count.SetAll(0);
  cur_task_time.SetAll(0.0);  // Added for time tracking; WRE 03-18-07
  for (int j = 0; j < sensed_resources.GetSize(); j++) {
    sensed_resources[j] =  parent_phenotype.sensed_resources[j];
  }
  
  // Copy last values from parent
  last_merit_base           = parent_phenotype.last_merit_base;
  last_bonus                = parent_phenotype.last_bonus;
  last_cpu_cycles_used      = parent_phenotype.last_cpu_cycles_used;
  last_num_errors           = parent_phenotype.last_num_errors;
  last_task_count           = parent_phenotype.last_task_count;
  last_task_quality         = parent_phenotype.last_task_quality;
  last_task_value           = parent_phenotype.last_task_value;
  last_reaction_count       = parent_phenotype.last_reaction_count;
  last_reaction_add_reward  = parent_phenotype.last_reaction_add_reward;
  last_inst_count           = parent_phenotype.last_inst_count;
  last_from_sensor_count    = parent_phenotype.last_from_sensor_count;
  last_killed_targets      = parent_phenotype.last_killed_targets;
  last_fitness              = CalcFitness(last_merit_base, last_bonus, gestation_time, last_cpu_cycles_used);
  
  // Setup other miscellaneous values...
  num_divides     = 0;
  num_divides_failed = 0;
  generation      = parent_phenotype.generation;
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
  cpu_cycles_used = 0;
  time_used       = 0;
  num_execs       = 0;
  age             = 0;
  neutral_metric  = parent_phenotype.neutral_metric + m_world->GetRandom().GetRandNormal();
  life_fitness    = fitness; 
  exec_time_born  = parent_phenotype.exec_time_born;  //@MRR treating offspring and parent as siblings; already set in DivideReset
  birth_update    = parent_phenotype.birth_update;    
  num_new_unique_reactions = 0;
  last_task_id             = -1;
  res_consumed             = 0;
  last_task_time           = 0;
  
	
  // Setup flags...
  is_injected   = false;
  is_clone   = false;
  
  is_modifier   = false;
  is_modified   = false;
  is_fertile    = parent_phenotype.last_child_fertile;
  is_mutated    = false;
  if (m_world->GetConfig().INHERIT_MULTITHREAD.Get()) {
    is_multi_thread = parent_phenotype.is_multi_thread;
  } else {
    is_multi_thread = false;
  }
  
  parent_true   = parent_phenotype.copy_true;
  parent_sex    = parent_phenotype.divide_sex;
  parent_cross_num    = parent_phenotype.cross_num;
  to_die = false;
  to_delete = false;
  
  // Setup child info...
  copy_true          = false;
  divide_sex         = false;
  mate_select_id     = -1;
  cross_num          = 0;
  last_child_fertile = is_fertile;
  child_fertile      = true;
  child_copied_size  = 0;
  
  
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
void cPhenotype::SetupInject(const InstructionSequence& _genome)
{
  // Setup reasonable initial values injected organism...
  genome_length   = _genome.GetSize();
  merit           = genome_length;
  copied_size     = genome_length;
  executed_size   = genome_length;
  executionRatio = 1.0;
  gestation_time  = 0;
  gestation_start = 0;
  fitness         = 0;
  div_type	  = 1;
  
  // Initialize current values, as neeeded.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_num_errors  = 0;
  cur_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_task_value.SetAll(0);
  cur_reaction_count.SetAll(0);
  first_reaction_cycles.SetAll(-1);
  first_reaction_execs.SetAll(-1);
  cur_stolen_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_from_sensor_count.SetAll(0);
  for (int r = 0; r < cur_group_attack_count.GetSize(); r++) {
    cur_group_attack_count[r].SetAll(0);
    cur_top_pred_group_attack_count[r].SetAll(0);
  }
  cur_killed_targets.SetAll(0);
  sensed_resources.SetAll(0);
  cur_sense_count.SetAll(0);
  cur_task_time.SetAll(0.0);
  
  // New organism has no parent and so cannot use its last values; initialize as needed
  last_merit_base = genome_length;
  last_bonus      = 1;
  last_cpu_cycles_used = 0;
  last_num_errors = 0;
  last_task_count.SetAll(0);
  last_task_quality.SetAll(0);
  last_task_value.SetAll(0);
  last_reaction_count.SetAll(0);
  last_reaction_add_reward.SetAll(0);
  last_inst_count.SetAll(0);
  last_from_sensor_count.SetAll(0);
  last_killed_targets.SetAll(0);
  
  // Setup other miscellaneous values...
  num_divides     = 0;
  num_divides_failed = 0;
  generation      = 0;
  cpu_cycles_used = 0;
  time_used       = 0;
  num_execs       = 0;
  age             = 0;
  neutral_metric  = 0;
  life_fitness    = 0;
  exec_time_born  = 0;
  birth_update     = m_world->GetStats().GetUpdate();
  
  // Setup flags...
  is_injected   = true;
  is_clone   = false;
  
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
  
  // Setup child info...
  copy_true         = false;
  divide_sex        = false;
  mate_select_id    = -1;
  cross_num         = 0;
  child_fertile     = true;
  last_child_fertile = true;
  child_copied_size = 0;
  
  
  initialized = true;
}


void cPhenotype::ResetMerit()
{
  int cur_merit_base = CalcSizeMerit();
  const int merit_default_bonus = m_world->GetConfig().MERIT_DEFAULT_BONUS.Get();
  if (merit_default_bonus) {
    cur_bonus = merit_default_bonus;
  }
  merit = cur_merit_base * cur_bonus;

  if (m_world->GetConfig().INHERIT_MERIT.Get() == 0) {
    merit = cur_merit_base;
  }
}


/**
 * This function is run whenever an organism executes a successful divide.
 **/
void cPhenotype::DivideReset(const InstructionSequence& _genome)
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
  last_num_errors           = cur_num_errors;
  last_task_count           = cur_task_count;
  last_task_quality         = cur_task_quality;
  last_task_value           = cur_task_value;
  last_reaction_count       = cur_reaction_count;
  last_reaction_add_reward  = cur_reaction_add_reward;
  last_inst_count           = cur_inst_count;
  last_from_sensor_count    = cur_from_sensor_count;
  last_killed_targets       = cur_killed_targets;
  
  
  // Reset cur values.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cpu_cycles_used = 0;
  cur_num_errors  = 0;
  cur_task_count.SetAll(0);
  
  
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_task_value.SetAll(0);
  cur_reaction_count.SetAll(0);
  first_reaction_cycles.SetAll(-1);
  first_reaction_execs.SetAll(-1);
  cur_stolen_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_from_sensor_count.SetAll(0);
  for (int r = 0; r < cur_group_attack_count.GetSize(); r++) {
    cur_group_attack_count[r].SetAll(0);
    cur_top_pred_group_attack_count[r].SetAll(0);
  }
  cur_killed_targets.SetAll(0);
  cur_sense_count.SetAll(0);
  cur_task_time.SetAll(0.0);
  
  // Setup other miscellaneous values...
  num_divides++;
  (void) generation;
  (void) time_used;
  num_execs       = 0;
  age             = 0;
  (void) neutral_metric;
  life_fitness = fitness; 
  exec_time_born += gestation_time;  //@MRR Treating organism as sibling
  birth_update = m_world->GetStats().GetUpdate();   
  num_new_unique_reactions = 0;
  last_task_id             = -1;
  res_consumed             = 0;
  last_task_time           = 0;
  
  // Leave flags alone...
  (void) is_injected;
  is_clone = false; // has legitimately reproduced
  
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
  (void) child_copied_size;
  
  // A few final changes if the parent was supposed to be be considered
  // a second child on the divide.
  if ((m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) ||
      (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_BIRTH)) {
    gestation_start = 0;
    cpu_cycles_used = 0;
    time_used = 0;
    neutral_metric += m_world->GetRandom().GetRandNormal();
  }
  
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() == GENERATION_INC_BOTH) generation++;
  
  // Reset Task States
  for (Apto::Map<void*, cTaskState*>::ValueIterator it = m_task_states.Values(); it.Next();) delete *it.Get();
  m_task_states.Clear();
}

/**
 * This function runs whenever a *test* CPU divides. It processes much of
 * the information for that CPU in order to actively reflect its executed
 * and copied size in its merit.
 **/
void cPhenotype::TestDivideReset(const InstructionSequence& _genome)
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
  
  if (m_world->GetConfig().INHERIT_MERIT.Get() == 0) {
    merit = cur_merit_base;
  }
  
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
  last_task_count           = cur_task_count;
  last_task_quality         = cur_task_quality;
  last_task_value			= cur_task_value;
  last_reaction_count       = cur_reaction_count;
  last_reaction_add_reward  = cur_reaction_add_reward;
  last_inst_count           = cur_inst_count;
  last_from_sensor_count    = cur_from_sensor_count;
  last_killed_targets       = cur_killed_targets;
  
  // Reset cur values.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cpu_cycles_used = 0;
  cur_num_errors  = 0;
  cur_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_task_value.SetAll(0);
  cur_reaction_count.SetAll(0);
  first_reaction_cycles.SetAll(-1);
  first_reaction_execs.SetAll(-1);
  cur_stolen_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_from_sensor_count.SetAll(0);
  for (int r = 0; r < cur_group_attack_count.GetSize(); r++) {
    cur_group_attack_count[r].SetAll(0);
    cur_top_pred_group_attack_count[r].SetAll(0);
  }
  cur_killed_targets.SetAll(0);
  cur_sense_count.SetAll(0);
  cur_task_time.SetAll(0.0);
  sensed_resources.SetAll(-1.0);
  
  // Setup other miscellaneous values...
  num_divides++;
  generation++;
  (void) time_used;
  (void) num_execs;
  (void) age;
  (void) neutral_metric;
  life_fitness = fitness; 
  exec_time_born += gestation_time;  //@MRR See DivideReset 
  birth_update  = m_world->GetStats().GetUpdate();
  num_new_unique_reactions = 0;
  last_task_id             = -1;
  res_consumed             = 0;
  last_task_time           = 0;

  
  // Leave flags alone...
  (void) is_injected;
  is_clone = false; // has legitimately reproduced
  
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

void cPhenotype::SetupClone(const cPhenotype& clone_phenotype)
{
  // Copy divide values from parent, which should already be setup.
  merit           = clone_phenotype.merit;
  
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
  cur_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_reaction_count.SetAll(0);
  first_reaction_cycles.SetAll(-1);
  first_reaction_execs.SetAll(-1);
  cur_stolen_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_from_sensor_count.SetAll(0);
  for (int r = 0; r < cur_group_attack_count.GetSize(); r++) {
    cur_group_attack_count[r].SetAll(0);
    cur_top_pred_group_attack_count[r].SetAll(0);
  }
  cur_killed_targets.SetAll(0);
  cur_sense_count.SetAll(0);
  cur_task_time.SetAll(0.0);
  for (int j = 0; j < sensed_resources.GetSize(); j++) {
    sensed_resources[j] = clone_phenotype.sensed_resources[j];
  }
  
  // Copy last values from parent
  last_merit_base          = clone_phenotype.last_merit_base;
  last_bonus               = clone_phenotype.last_bonus;
  last_cpu_cycles_used     = clone_phenotype.last_cpu_cycles_used;
  last_num_errors          = clone_phenotype.last_num_errors;
  last_task_count          = clone_phenotype.last_task_count;
  last_reaction_count      = clone_phenotype.last_reaction_count;
  last_reaction_add_reward = clone_phenotype.last_reaction_add_reward;
  last_inst_count          = clone_phenotype.last_inst_count;
  last_from_sensor_count   = clone_phenotype.last_from_sensor_count;
  last_killed_targets      = clone_phenotype.last_killed_targets;
  last_fitness             = CalcFitness(last_merit_base, last_bonus, gestation_time, last_cpu_cycles_used);
  
  // Setup other miscellaneous values...
  num_divides     = 0;
  num_divides_failed = 0;
  generation      = clone_phenotype.generation;
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
  cpu_cycles_used = 0;
  time_used       = 0;
  num_execs       = 0;
  age             = 0;
  neutral_metric  = clone_phenotype.neutral_metric + m_world->GetRandom().GetRandNormal();
  life_fitness    = fitness; 
  exec_time_born  = 0;
  birth_update    = m_world->GetStats().GetUpdate();
  num_new_unique_reactions = clone_phenotype.num_new_unique_reactions;
  last_task_id             = clone_phenotype.last_task_id;
  res_consumed             = clone_phenotype.res_consumed;
  last_task_time           = clone_phenotype.last_task_time;

  
  // Setup flags...
  is_injected   = false;
  is_clone   = true;
  
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
  
  // Setup child info...
  copy_true          = false;
  divide_sex         = false;
  mate_select_id     = 0;
  cross_num          = 0;
  last_child_fertile = is_fertile;
  child_fertile      = true;
  child_copied_size  = 0;
  
  initialized = true;
}




bool cPhenotype::TestOutput(cAvidaContext& ctx, cTaskContext& taskctx,
                            const Apto::Array<double>& res_in, const Apto::Array<double>& rbins_in,
                            Apto::Array<double>& res_change, Apto::Array<cString>& insts_triggered)
{
  assert(initialized == true);
  taskctx.SetTaskStates(&m_task_states);
  
  const cEnvironment& env = m_world->GetEnvironment();
  const int num_resources = env.GetResDefLib().GetSize();
  const int num_tasks = env.GetNumTasks();
  const int num_reactions = env.GetReactionLib().GetSize();
  
  // For refractory period @WRE 03-20-07
  const int cur_update_time = m_world->GetStats().GetUpdate();
  const double task_refractory_period = m_world->GetConfig().TASK_REFRACTORY_PERIOD.Get();
  double refract_factor;
  
  if (!m_reaction_result) m_reaction_result = new cReactionResult(num_resources, num_tasks, num_reactions);
  cReactionResult& result = *m_reaction_result;
  
  // Run everything through the environment.
  bool found = env.TestOutput(ctx, result, taskctx, eff_task_count, cur_reaction_count, res_in, rbins_in);
  
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
      
      
    }
    
    if (result.TaskQuality(i) > 0) {
      cur_task_quality[i] += result.TaskQuality(i) * refract_factor;
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
  }
  
  // Update the merit bonus
  cur_bonus *= result.GetMultBonus();
  cur_bonus += result.GetAddBonus();
  
  
  
  // Denote consumed resources...
  for (int i = 0; i < res_in.GetSize(); i++) {
    res_change[i] = result.GetProduced(i) - result.GetConsumed(i);
    res_consumed += result.GetConsumed(i);
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
  << '\n';
  
  fp << "  Task Count (Quality):";
  for (int i = 0; i < cur_task_count.GetSize(); i++) {
    fp << " " << cur_task_count[i] << " (" << cur_task_quality[i] << ")";
  }
  fp << '\n';
  
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


double cPhenotype::CalcFitness(double _merit_base, double _bonus, int _gestation_time, int) const
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





// Arbitrary (but consistant) ordering.
// Return -1 if lhs is "less", +1 is it is "greater", and 0 otherwise.
int cPhenotype::Compare(const cPhenotype* lhs, const cPhenotype* rhs) {
  // Compare first based on merit...
  if ( lhs->GetMerit() < rhs->GetMerit() ) return -1;
  else if ( lhs->GetMerit() > rhs->GetMerit() ) return 1;
  
  // If merits are equal, compare gestation time...
  if ( lhs->GetGestationTime() < rhs->GetGestationTime() ) return -1;
  else if ( lhs->GetGestationTime() > rhs->GetGestationTime() ) return 1;
  
  // If gestation times are also equal, compare each task
  Apto::Array<int> lhsTasks = lhs->GetLastTaskCount();
  Apto::Array<int> rhsTasks = rhs->GetLastTaskCount();
  for (int k = 0; k < lhsTasks.GetSize(); k++) {
    if (lhsTasks[k] < rhsTasks[k]) return -1;
    else if (lhsTasks[k] > rhsTasks[k]) return 1;
  }
  
  // Assume they are identical.
  return 0;
}

bool cPhenotype::PhenotypeCompare::operator()(const cPhenotype* lhs, const cPhenotype* rhs) const {
  return cPhenotype::Compare(lhs, rhs) < 0;
}


  



double cPhenotype::GetResourcesConsumed() 
{
	double r = res_consumed; 
	res_consumed =0; 
	return r; 
}


/* Return the cumulative reaction count if we aren't resetting on divide. */
Apto::Array<int> cPhenotype::GetCumulativeReactionCount()
{ 
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == 0) { 
    Apto::Array<int> cum_react;
    for (int i=0; i<cur_reaction_count.GetSize(); ++i) 
    {
      cum_react.Push(cur_reaction_count[i] + last_reaction_count[i]);
    }
//    return (cur_reaction_count + last_reaction_count); 
    return cum_react;
  } else {
    return cur_reaction_count;
  }
}
