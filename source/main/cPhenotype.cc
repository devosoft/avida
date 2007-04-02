/*
 *  cPhenotype.cc
 *  Avida
 *
 *  Called "phenotype.cc" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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
#include "cHardwareManager.h"
#include "cInstSet.h"
#include "cReactionResult.h"
#include "cTaskState.h"
#include "cTools.h"
#include "cWorld.h"

#include <fstream>

using namespace std;


cPhenotype::cPhenotype(cWorld* world)
  : m_world(world)
  , initialized(false)
  , cur_task_count(m_world->GetEnvironment().GetNumTasks())
  , eff_task_count(m_world->GetEnvironment().GetNumTasks())
  , cur_task_quality(m_world->GetEnvironment().GetNumTasks())  
  , cur_reaction_count(m_world->GetEnvironment().GetReactionLib().GetSize())
  , cur_reaction_add_reward(m_world->GetEnvironment().GetReactionLib().GetSize())
  , cur_inst_count(world->GetHardwareManager().GetInstSet().GetSize())
  , cur_sense_count(m_world->GetStats().GetSenseSize())
  , sensed_resources(m_world->GetEnvironment().GetResourceLib().GetSize())
  , last_task_count(m_world->GetEnvironment().GetNumTasks())
  , last_reaction_add_reward(m_world->GetEnvironment().GetReactionLib().GetSize())  
  , last_task_quality(m_world->GetEnvironment().GetNumTasks())
  , last_reaction_count(m_world->GetEnvironment().GetReactionLib().GetSize())
  , last_inst_count(world->GetHardwareManager().GetInstSet().GetSize())
  , last_sense_count(m_world->GetStats().GetSenseSize())
{
}

cPhenotype::~cPhenotype()
{
  // Remove Task States
  tArray<cTaskState*> task_states(0);
  m_task_states.GetValues(task_states);
  for (int i = 0; i < task_states.GetSize(); i++) delete task_states[i];
}

bool cPhenotype::OK()
{
  assert(genome_length >= 0);
  assert(copied_size >= 0);
  assert(executed_size >= 0);
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
  assert(generation >= 0);
  assert(cpu_cycles_used >= 0);  
  assert(time_used >= 0);
  assert(age >= 0);
  assert(child_copied_size >= 0);
  // assert(to_die == false);
  return true;
}


/**
 * This function is run whenever a new organism is being constructed inside
 * of its parent.
 *
 * Assumptions:
 *     - parent_phenotype has had DivideReset run on it already!
 *     - this is the first method run on an otherwise freshly built phenotype.
 **/

void cPhenotype::SetupOffspring(const cPhenotype & parent_phenotype,
				const cGenome & _genome)
{
  // Copy divide values from parent, which should already be setup.
  merit           = parent_phenotype.merit;
  genome_length   = _genome.GetSize();
  copied_size     = parent_phenotype.child_copied_size;
  executed_size   = parent_phenotype.executed_size;
  assert (executed_size > 0);
  
  gestation_time  = parent_phenotype.gestation_time;
  gestation_start = 0;
  fitness         = parent_phenotype.fitness;
  div_type        = parent_phenotype.div_type;

  assert(genome_length > 0);
  assert(copied_size > 0);
  assert(executed_size > 0);
  assert(gestation_time > 0);
  assert(div_type > 0);

  // Initialize current values, as neeeded.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_sense_count.SetAll(0);  
  for (int j = 0; j < sensed_resources.GetSize(); j++)
	      sensed_resources[j] =  parent_phenotype.sensed_resources[j];
  SetupPromoterWeights(_genome, true);
  
  // Copy last values from parent
  last_merit_base           = parent_phenotype.last_merit_base;
  last_bonus                = parent_phenotype.last_bonus;
  last_num_errors           = parent_phenotype.last_num_errors;
  last_num_donates          = parent_phenotype.last_num_donates;
  last_task_count           = parent_phenotype.last_task_count;
  last_task_quality         = parent_phenotype.last_task_quality;
  last_reaction_count       = parent_phenotype.last_reaction_count;
  last_reaction_add_reward  = parent_phenotype.last_reaction_add_reward;
  last_inst_count           = parent_phenotype.last_inst_count;
  last_sense_count          = parent_phenotype.last_sense_count;
  last_fitness              = last_merit_base * last_bonus / gestation_time;

  // Setup other miscellaneous values...
  num_divides     = 0;
  generation      = parent_phenotype.generation;
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
  cpu_cycles_used = 0;
  time_used       = 0;
  age             = 0;
  fault_desc      = "";
  neutral_metric  = parent_phenotype.neutral_metric + m_world->GetRandom().GetRandNormal();
  life_fitness    = fitness; 

  // Setup flags...
  is_injected   = false;
  is_parasite   = false;
  is_donor_cur  = false;
  is_donor_last = parent_phenotype.is_donor_last;
  is_receiver   = false;
  is_modifier   = false;
  is_modified   = false;
  is_fertile    = parent_phenotype.last_child_fertile;
  is_mutated    = false;
  is_multi_thread = parent_phenotype.is_multi_thread;
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

void cPhenotype::SetupInject(const cGenome & _genome)
{
  // Setup reasonable initial values injected organism...
  genome_length   = _genome.GetSize();
  merit           = genome_length;
  copied_size     = genome_length;
  executed_size   = genome_length;
  gestation_time  = 0;
  gestation_start = 0;
  fitness         = 0;
  div_type	  = 1;

  // Initialize current values, as neeeded.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  sensed_resources.SetAll(0);
  cur_sense_count.SetAll(0);
  SetupPromoterWeights(_genome, true);
  
  // Copy last values from parent
  last_merit_base = genome_length;
  last_bonus      = 1;
  last_num_errors = 0;
  last_num_donates = 0;
  last_task_count.SetAll(0);
  last_task_quality.SetAll(0);
  last_reaction_count.SetAll(0);
  last_reaction_add_reward.SetAll(0);
  last_sense_count.SetAll(0);

  // Setup other miscellaneous values...
  num_divides     = 0;
  generation      = 0;
  cpu_cycles_used = 0;
  time_used       = 0;
  age             = 0;
  fault_desc      = "";
  neutral_metric  = 0;
  life_fitness    = 0; 

  // Setup flags...
  is_injected   = true;
  is_parasite   = false;
  is_donor_last = false;
  is_donor_cur  = false;
  is_receiver   = false;
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
  mate_select_id    = 0;
  cross_num         = 0;
  child_fertile     = true;
  last_child_fertile = true;
  child_copied_size = 0;

  initialized = true;
}


/**
 * This function is run whenever an organism executes a successful divide.
 **/

void cPhenotype::DivideReset(const cGenome & _genome)
{
  assert(time_used > 0);
  assert(initialized == true);

  // Update these values as needed...
  int cur_merit_base = CalcSizeMerit();
  
  // If we are resetting the current merit, do it here
  // and it will also be propagated to the child
  const int merit_default_bonus = m_world->GetConfig().MERIT_DEFAULT_BONUS.Get();
  if (merit_default_bonus) {
    merit = cur_merit_base * m_world->GetConfig().DEFAULT_BONUS.Get();
  }
  else { // Default
    merit = cur_merit_base * cur_bonus;
  }
  
  genome_length   = _genome.GetSize();
  (void) copied_size;          // Unchanged
  (void) executed_size;        // Unchanged
  gestation_time  = time_used - gestation_start;
  gestation_start = time_used;
  fitness         = merit.GetDouble() / gestation_time;

  // Lock in cur values as last values.
  last_merit_base           = cur_merit_base;
  last_bonus                = cur_bonus;
  last_num_errors           = cur_num_errors;
  last_num_donates          = cur_num_donates;
  last_task_count           = cur_task_count;
  last_task_quality         = cur_task_quality;
  last_reaction_count       = cur_reaction_count;
  last_reaction_add_reward  = cur_reaction_add_reward;
  last_inst_count           = cur_inst_count;
  last_sense_count          = cur_sense_count;

  // Reset cur values.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_sense_count.SetAll(0);

  // Setup other miscellaneous values...
  num_divides++;
  (void) generation;
  (void) time_used;
  age             = 0;
  fault_desc      = "";
  (void) neutral_metric;
  life_fitness = fitness; 

  // Leave flags alone...
  (void) is_injected;
  (void) is_parasite;
  is_donor_last = is_donor_cur;
  is_donor_cur = false;
  is_receiver = false;
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
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    gestation_start = 0;
    cpu_cycles_used = 0;
    time_used = 0;
    neutral_metric += m_world->GetRandom().GetRandNormal();
    SetupPromoterWeights(_genome, true);
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

void cPhenotype::TestDivideReset(const cGenome & _genome)
{
  assert(time_used > 0);
  assert(initialized == true);

  // Update these values as needed...
  int cur_merit_base = CalcSizeMerit();
  merit           = cur_merit_base * cur_bonus;

  genome_length   = _genome.GetSize();
  (void) copied_size;                            // Unchanged
  (void) executed_size;                          // Unchanged
  gestation_time  = time_used - gestation_start;
  gestation_start = time_used;
  fitness         = merit.GetDouble() / gestation_time;
  (void) div_type; 				// Unchanged

  // Lock in cur values as last values.
  last_merit_base           = cur_merit_base;
  last_bonus                = cur_bonus;
  last_num_errors           = cur_num_errors;
  last_num_donates          = cur_num_donates;
  last_task_count           = cur_task_count;
  last_task_quality         = cur_task_quality;
  last_reaction_count       = cur_reaction_count;
  last_reaction_add_reward  = cur_reaction_add_reward;
  last_inst_count           = cur_inst_count;
  last_sense_count          = cur_sense_count;  

  // Reset cur values.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_task_quality.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_sense_count.SetAll(0); 
  sensed_resources.SetAll(-1.0);
  SetupPromoterWeights(_genome, true);
  
  // Setup other miscellaneous values...
  num_divides++;
  generation++;
  (void) time_used;
  (void) age;
  (void) fault_desc;
  (void) neutral_metric;
  life_fitness = fitness; 

  // Leave flags alone...
  (void) is_injected;
  (void) is_parasite;
  is_donor_last = is_donor_cur;
  is_donor_cur = false;
  is_receiver = false;
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
  genome_length   = clone_phenotype.genome_length;
  copied_size     = clone_phenotype.copied_size;
  // copied_size     = clone_phenotype.child_copied_size;
  executed_size   = clone_phenotype.executed_size;
  gestation_time  = clone_phenotype.gestation_time;
  gestation_start = 0;
  fitness         = clone_phenotype.fitness;
  div_type        = clone_phenotype.div_type;
  cur_promoter_weights = clone_phenotype.cur_promoter_weights; // @JEB Not correct if clone is not of fresh phenotype 
  base_promoter_weights = clone_phenotype.base_promoter_weights; // @JEB Not correct if clone is not of fresh phenotype 
  promoter_repression = clone_phenotype.promoter_repression; // @JEB Not correct if clone is not of fresh phenotype 
  promoter_activation = clone_phenotype.promoter_activation; // @JEB Not correct if clone is not of fresh phenotype 

  assert(genome_length > 0);
  assert(copied_size > 0);
  assert(executed_size > 0);
  assert(gestation_time > 0);
  assert(div_type > 0);

  // Initialize current values, as neeeded.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  eff_task_count.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_reaction_add_reward.SetAll(0);
  cur_inst_count.SetAll(0);
  cur_sense_count.SetAll(0);  
  for (int j = 0; j < sensed_resources.GetSize(); j++)
	      sensed_resources[j] =  clone_phenotype.sensed_resources[j];
  //SetupPromoterWeights(_genome); Do we reset here?

  // Copy last values from parent
  last_merit_base          = clone_phenotype.last_merit_base;
  last_bonus               = clone_phenotype.last_bonus;
  last_num_errors          = clone_phenotype.last_num_errors;
  last_num_donates         = clone_phenotype.last_num_donates;
  last_task_count          = clone_phenotype.last_task_count;
  last_reaction_count      = clone_phenotype.last_reaction_count;
  last_reaction_add_reward = clone_phenotype.last_reaction_add_reward;
  last_inst_count          = clone_phenotype.last_inst_count;
  last_sense_count         = clone_phenotype.last_sense_count;  
  last_fitness             = last_merit_base * last_bonus / gestation_time;

  // Setup other miscellaneous values...
  num_divides     = 0;
  generation      = clone_phenotype.generation;
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
  cpu_cycles_used = 0;
  time_used       = 0;
  age             = 0;
  fault_desc      = "";
  neutral_metric  = clone_phenotype.neutral_metric + m_world->GetRandom().GetRandNormal();
  life_fitness    = fitness; 

  // Setup flags...
  is_injected   = false;
  is_parasite   = false;
  is_donor_last = clone_phenotype.is_donor_last;
  is_donor_cur  = clone_phenotype.is_donor_cur;
  is_receiver = false;
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




bool cPhenotype::TestInput(tBuffer<int>& inputs, tBuffer<int>& outputs)
{
  assert(initialized == true);
  // For the moment, lets not worry about inputs...
  return false; // Nothing happened...
}

bool cPhenotype::TestOutput(cAvidaContext& ctx, cTaskContext& taskctx,
			    const tArray<double>& res_in, tArray<double>& res_change,
			    tArray<int>& insts_triggered)
{
  assert(initialized == true);
  taskctx.SetTaskStates(&m_task_states);

  const cEnvironment& env = m_world->GetEnvironment();
  const int num_resources = env.GetResourceLib().GetSize();
  const int num_tasks = env.GetNumTasks();
  const int num_reactions = env.GetReactionLib().GetSize();

  cReactionResult result(num_resources, num_tasks, num_reactions);
			
  // Run everything through the environment.
  bool found = env.TestOutput(ctx, result, taskctx, eff_task_count, cur_reaction_count, res_in);

  // If nothing was found, stop here.
  if (found == false) {
    res_change.SetAll(0.0);
    return false;  // Nothing happened.
  }

  // Update the phenotype with the results...
  // Start with updating task and reaction counters
  for (int i = 0; i < num_tasks; i++) {
    if (result.TaskDone(i) == true) 
    {
      cur_task_count[i]++;
      eff_task_count[i]++;
    }
    if (result.TaskQuality(i) > 0) cur_task_quality[i]+= result.TaskQuality(i);
  }
  for (int i = 0; i < num_reactions; i++) {
    if (result.ReactionTriggered(i) == true) cur_reaction_count[i]++;
    cur_reaction_add_reward[i] += result.GetReactionAddBonus(i);
  }

  // Update the merit bonus
  cur_bonus *= result.GetMultBonus();
  cur_bonus += result.GetAddBonus();

  // Denote consumed resources...
  for (int i = 0; i < res_in.GetSize(); i++) {
    res_change[i] = result.GetProduced(i) - result.GetConsumed(i);
  }

  // Save the instructions that should be triggered...
  insts_triggered = result.GetInstArray();

  //Put in detected resources
  for (int j = 0; j < res_in.GetSize(); j++)
	  if(result.GetDetected(j) != -1.0)
	      sensed_resources[j] = result.GetDetected(j);

  //Kill any cells that did lethal reactions
  to_die = result.GetLethal();
  
  return true;
}


///// For Loading and Saving State: /////


bool cPhenotype::SaveState(ofstream& fp)
{
  assert(fp.good());
  fp << "cPhenotype" << endl;

  fp << merit.GetDouble()   << " ";
  fp << genome_length       << " ";
  fp << copied_size         << " ";
  fp << executed_size       << " ";
  fp << gestation_time      << " ";
  fp << gestation_start     << " ";
  fp << fitness             << " ";
  fp << div_type            << " ";

  fp << cur_bonus           << " ";
  fp << cur_num_errors      << " ";
  fp << cur_num_donates      << " ";
  for (int i = 0; i < cur_task_count.GetSize(); i++) {
    fp << cur_task_count[i] << " ";
  }
  for (int i = 0; i < cur_reaction_count.GetSize(); i++) {
    fp << cur_reaction_count[i] << " ";
  }
  for (int i = 0; i < cur_inst_count.GetSize(); i++) {
    fp << cur_inst_count[i] << " ";
  }

  fp << last_merit_base     << " ";
  fp << last_bonus          << " ";
  fp << last_num_errors     << " ";
  fp << last_num_donates    << " ";
  for (int i = 0; i < last_task_count.GetSize(); i++) {
    fp << last_task_count[i] << " ";
  }
  for (int i = 0; i < last_reaction_count.GetSize(); i++) {
    fp << last_reaction_count[i] << " ";
  }
  for (int i = 0; i < last_inst_count.GetSize(); i++) {
    fp << last_inst_count[i] << " ";
  }

  fp << num_divides         << " ";
  fp << generation          << " ";
  fp << cpu_cycles_used     << " ";
  fp << time_used           << " ";
  fp << age                 << " ";
  fp << neutral_metric      << " ";
  fp << life_fitness        << " ";

  fp << is_injected         << " ";
  fp << is_parasite         << " ";
  fp << is_donor_last       << " ";
  fp << is_donor_cur        << " ";

  fp << is_receiver         << " ";
  fp << is_modifier         << " ";
  fp << is_modified         << " ";
  fp << is_fertile          << " ";
  fp << is_mutated          << " ";
  fp << parent_true         << " ";
  fp << parent_sex          << " ";
  fp << parent_cross_num    << " ";

  fp << copy_true           << " ";
  fp << divide_sex          << " ";
  fp << mate_select_id      << " ";
  fp << cross_num           << " ";
  fp << child_fertile       << " ";
  fp << last_child_fertile  << " ";

  fp << endl;
  return true;
}


bool cPhenotype::LoadState(ifstream & fp)
{
  double tmp_merit;
  assert(fp.good());
  if( !fp.good() ) return false;

  fp >> tmp_merit;  merit = tmp_merit;
  fp >> genome_length;
  fp >> copied_size;
  fp >> executed_size;
  fp >> gestation_time;
  fp >> gestation_start;
  fp >> fitness;
  fp >> div_type;

  fp >> cur_bonus;
  fp >> cur_num_errors;
  fp >> cur_num_donates;

  for (int i = 0; i < cur_task_count.GetSize(); i++) {
    fp >> cur_task_count[i];
  }
  for (int i = 0; i < cur_reaction_count.GetSize(); i++) {
    fp >> cur_reaction_count[i];
  }
  for (int i = 0; i < cur_inst_count.GetSize(); i++) {
    fp >> cur_inst_count[i];
  }

  fp >> last_merit_base;
  fp >> last_bonus;
  fp >> last_num_errors;
  fp >> last_num_donates;
  for (int i = 0; i < last_task_count.GetSize(); i++) {
    fp >> last_task_count[i];
  }
  for (int i = 0; i < last_reaction_count.GetSize(); i++) {
    fp >> last_reaction_count[i];
  }
  for (int i = 0; i < last_inst_count.GetSize(); i++) {
    fp >> last_inst_count[i];
  }

  fp >> num_divides;
  fp >> generation;
  fp >> cpu_cycles_used;
  fp >> time_used;
  fp >> age;
  fp >> neutral_metric;
  fp >> life_fitness;

  fp >> is_injected;
  fp >> is_parasite;
  fp >> is_donor_last;
  fp >> is_donor_cur;
  fp >> is_receiver;
  fp >> is_modifier;
  fp >> is_modified;
  fp >> is_fertile;
  fp >> is_mutated;
  fp >> parent_true;
  fp >> parent_sex;
  fp >> parent_cross_num;

  fp >> copy_true;
  fp >> divide_sex;
  fp >> mate_select_id;
  fp >> cross_num;
  fp >> child_fertile;
  fp >> last_child_fertile;

  return true;
}

void cPhenotype::PrintStatus(ostream& fp) const
{
  fp << "  MeritBase:"
     << CalcSizeMerit()
     << " Bonus: " << cur_bonus
     << " Errors:" << cur_num_errors
     << " Donates:" << cur_num_donates;
  fp << endl;
  
  fp << "  Task Count (Quality):";
  for (int i = 0; i < cur_task_count.GetSize(); i++)
    fp << " " << cur_task_count[i] << " (" << cur_task_quality[i] << ")";
  fp << endl;
  
  if (m_world->GetConfig().PROMOTERS_ENABLED.Get() == 1)
  {
    fp << "Promoters:     ";
    for (int i=0; i<cur_promoter_weights.GetSize(); i++)
    {
      if (cur_promoter_weights[i] != m_world->GetConfig().PROMOTER_BG_STRENGTH.Get()) fp << i << " (" << cur_promoter_weights[i] << ") ";
    }
    fp << endl;
  }

}

int cPhenotype::CalcSizeMerit() const
{
  assert(genome_length > 0);
  assert(executed_size > 0);
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
  case BASE_MERIT_CONST:
  default:
    out_size = m_world->GetConfig().BASE_CONST_MERIT.Get();
    break;
  }

  return out_size;
} 

void cPhenotype::SetupPromoterWeights(const cGenome & _genome, const bool clear)
{
  if (!m_world->GetConfig().PROMOTERS_ENABLED.Get()) return;

  // Ideally, this wouldn't be hard-coded
  static cInstruction promoter_inst = m_world->GetHardwareManager().GetInstSet().GetInst(cStringUtil::Stringf("promoter"));

  int old_size = base_promoter_weights.GetSize();
  cur_promoter_weights.Resize(_genome.GetSize());
  base_promoter_weights.Resize(_genome.GetSize());
  promoter_repression.Resize(_genome.GetSize());
  promoter_activation.Resize(_genome.GetSize());

  // Only change new regions of the genome (that might have been allocated since this was last called)
  for ( int i = (clear ? 0 : old_size); i<_genome.GetSize(); i++)
  {
    base_promoter_weights[i] = 1;
    promoter_repression[i] = 1;
    promoter_activation[i] = 1;

    // Now change the weights at instructions that are not promoters if called for
    if ( _genome[i] != promoter_inst)
    {
      base_promoter_weights[i] *= m_world->GetConfig().PROMOTER_BG_STRENGTH.Get(); 
    }
    cur_promoter_weights[i] = base_promoter_weights[i];
  }
}

void cPhenotype::DecayAllPromoterRegulation()
{
  for ( int i=0; i<cur_promoter_weights.GetSize(); i++)
  {
    promoter_activation[i] *= (1 - m_world->GetConfig().REGULATION_DECAY_FRAC.Get());
    promoter_repression[i] *= (1 - m_world->GetConfig().REGULATION_DECAY_FRAC.Get());
    cur_promoter_weights[i] = base_promoter_weights[i] * promoter_activation[i] / promoter_repression[i];
  }
}

void cPhenotype::RegulatePromoter(const int i, const bool up )
{
  // Make sure we were initialized
  assert ( (promoter_activation.GetSize() > 0) && (promoter_activation.GetSize() > 0) );
  
  if (up) {
    promoter_activation[i] += m_world->GetConfig().REGULATION_STRENGTH.Get(); 
    promoter_activation[i] *= (1 - m_world->GetConfig().REGULATION_DECAY_FRAC.Get());
  }
  else {
    promoter_repression[i] += m_world->GetConfig().REGULATION_STRENGTH.Get(); 
    promoter_repression[i] *= (1 - m_world->GetConfig().REGULATION_DECAY_FRAC.Get());
  }
  
 cur_promoter_weights[i] = base_promoter_weights[i] * promoter_activation[i] / promoter_repression[i];
}

