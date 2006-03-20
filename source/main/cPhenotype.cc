/*
 *  cPhenotype.cc
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cPhenotype.h"

#include "cEnvironment.h"
#include "cHardwareManager.h"
#include "cReactionResult.h"
#include "cTools.h"
#include "cWorld.h"

#include <fstream>

using namespace std;


cPhenotype::cPhenotype(cWorld* world)
  : m_world(world)
  , initialized(false)
  , cur_task_count(m_world->GetEnvironment().GetTaskLib().GetSize())
  , cur_reaction_count(m_world->GetEnvironment().GetReactionLib().GetSize())
  , cur_inst_count(world->GetHardwareManager().GetInstSet().GetSize())
  , sensed_resources(m_world->GetEnvironment().GetResourceLib().GetSize())
  , last_task_count(m_world->GetEnvironment().GetTaskLib().GetSize())
  , last_reaction_count(m_world->GetEnvironment().GetReactionLib().GetSize())
  , last_inst_count(world->GetHardwareManager().GetInstSet().GetSize())
{
}

cPhenotype::~cPhenotype()
{
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
				int _length)
{
  // Copy divide values from parent, which should already be setup.
  merit           = parent_phenotype.merit;
  genome_length   = _length;
  copied_size     = parent_phenotype.child_copied_size;
  executed_size   = parent_phenotype.executed_size;
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
  cur_reaction_count.SetAll(0);
  cur_inst_count.SetAll(0);
  for (int j = 0; j < sensed_resources.GetSize(); j++)
	      sensed_resources[j] =  parent_phenotype.sensed_resources[j];


  // Copy last values from parent
  last_merit_base     = parent_phenotype.last_merit_base;
  last_bonus          = parent_phenotype.last_bonus;
  last_num_errors     = parent_phenotype.last_num_errors;
  last_num_donates    = parent_phenotype.last_num_donates;
  last_task_count     = parent_phenotype.last_task_count;
  last_reaction_count = parent_phenotype.last_reaction_count;
  last_inst_count     = parent_phenotype.last_inst_count;
  last_fitness        = last_merit_base * last_bonus / gestation_time;

  // Setup other miscellaneous values...
  num_divides     = 0;
  generation      = parent_phenotype.generation;
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
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

void cPhenotype::SetupInject(int _length)
{
  // Setup reasonable initial values injected organism...
  merit           = _length;
  genome_length   = _length;
  copied_size     = _length;
  executed_size   = _length;
  gestation_time  = 0;
  gestation_start = 0;
  fitness         = 0;
  div_type	  = 1;

  // Initialize current values, as neeeded.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_inst_count.SetAll(0);
  sensed_resources.SetAll(0);

  // Copy last values from parent
  last_merit_base = _length;
  last_bonus      = 1;
  last_num_errors = 0;
  last_num_donates = 0;
  last_task_count.SetAll(0);
  last_reaction_count.SetAll(0);
  last_inst_count.SetAll(0);

  // Setup other miscellaneous values...
  num_divides     = 0;
  generation      = 0;
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

void cPhenotype::DivideReset(int _length)
{
  assert(time_used > 0);
  assert(initialized == true);

  // Update these values as needed...
  int cur_merit_base = CalcSizeMerit();
  merit = cur_merit_base * cur_bonus;

  genome_length   = _length;
  (void) copied_size;          // Unchanged
  (void) executed_size;        // Unchanged
  gestation_time  = time_used - gestation_start;
  gestation_start = time_used;
  fitness         = merit.GetDouble() / gestation_time;

  // Lock in cur values as last values.
  last_merit_base     = cur_merit_base;
  last_bonus          = cur_bonus;
  last_num_errors     = cur_num_errors;
  last_num_donates    = cur_num_donates;
  last_task_count     = cur_task_count;
  last_reaction_count = cur_reaction_count;
  last_inst_count     = cur_inst_count;

  // Reset cur values.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_inst_count.SetAll(0);

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
    time_used = 0;
    neutral_metric += m_world->GetRandom().GetRandNormal();
  }

  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() == GENERATION_INC_BOTH) generation++;
}


/**
 * This function runs whenever a *test* CPU divides. It processes much of
 * the information for that CPU in order to actively reflect its executed
 * and copied size in its merit.
 **/

void cPhenotype::TestDivideReset(int _length)
{
  assert(time_used > 0);
  assert(initialized == true);

  // Update these values as needed...
  int cur_merit_base = CalcSizeMerit();
  merit           = cur_merit_base * cur_bonus;

  genome_length   = _length;
  (void) copied_size;                            // Unchanged
  (void) executed_size;                          // Unchanged
  gestation_time  = time_used - gestation_start;
  gestation_start = time_used;
  fitness         = merit.GetDouble() / gestation_time;
  (void) div_type; 				// Unchanged

  // Lock in cur values as last values.
  last_merit_base     = cur_merit_base;
  last_bonus          = cur_bonus;
  last_num_errors     = cur_num_errors;
  last_num_donates    = cur_num_donates;
  last_task_count     = cur_task_count;
  last_reaction_count = cur_reaction_count;
  last_inst_count     = cur_inst_count;

  // Reset cur values.
  cur_bonus       = m_world->GetConfig().DEFAULT_BONUS.Get();
  cur_num_errors  = 0;
  cur_num_donates  = 0;
  cur_task_count.SetAll(0);
  cur_reaction_count.SetAll(0);
  cur_inst_count.SetAll(0);
  sensed_resources.SetAll(-1.0);

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
  cur_reaction_count.SetAll(0);
  cur_inst_count.SetAll(0);
  for (int j = 0; j < sensed_resources.GetSize(); j++)
	      sensed_resources[j] =  clone_phenotype.sensed_resources[j];


  // Copy last values from parent
  last_merit_base     = clone_phenotype.last_merit_base;
  last_bonus          = clone_phenotype.last_bonus;
  last_num_errors     = clone_phenotype.last_num_errors;
  last_num_donates    = clone_phenotype.last_num_donates;
  last_task_count     = clone_phenotype.last_task_count;
  last_reaction_count = clone_phenotype.last_reaction_count;
  last_inst_count     = clone_phenotype.last_inst_count;
  last_fitness        = last_merit_base * last_bonus / gestation_time;

  // Setup other miscellaneous values...
  num_divides     = 0;
  generation      = clone_phenotype.generation;
  if (m_world->GetConfig().GENERATION_INC_METHOD.Get() != GENERATION_INC_BOTH) generation++;
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




bool cPhenotype::TestInput(tBuffer<int> & inputs, tBuffer<int> & outputs)
{
  assert(initialized == true);
  // For the moment, lets not worry about inputs...
  return false; // Nothing happened...
}

bool cPhenotype::TestOutput(cAvidaContext& ctx, tBuffer<int> & input_buf, tBuffer<int> &output_buf,
			    tBuffer<int> & send_buf, tBuffer<int> &receive_buf,
			    const tArray<double> & res_in,
			    tArray<double> & res_change,
			    tArray<int> & insts_triggered,
			    tList<tBuffer<int> > & other_inputs,
			    tList<tBuffer<int> > & other_outputs)
{
  assert(initialized == true);

  const cEnvironment& env = m_world->GetEnvironment();
  const int num_resources = env.GetResourceLib().GetSize();
  const int num_tasks = env.GetTaskLib().GetSize();
  const int num_reactions = env.GetReactionLib().GetSize();

  cReactionResult result(num_resources, num_tasks, num_reactions);
			
  // Run everything through the environment.
  bool found = env.TestOutput(ctx, result, input_buf, output_buf, send_buf,
			      receive_buf, cur_task_count, cur_reaction_count,
			      res_in, other_inputs, other_outputs);

  // If nothing was found, stop here.
  if (found == false) {
    res_change.SetAll(0.0);
    return false;  // Nothing happened.
  }

  // Update the phenotype with the results...
  // Start with updating task and reaction counters
  for (int i = 0; i < num_tasks; i++) {
    if (result.TaskDone(i) == true) cur_task_count[i]++;
  }
  for (int i = 0; i < num_reactions; i++) {
    if (result.ReactionTriggered(i) == true) cur_reaction_count[i]++;
  }

  // Update the merit bonus
  cur_bonus *= result.GetMultBonus();
  cur_bonus += result.GetAddBonus();

  // Bonus should never go negative...
  // if (cur_bonus < 0.0) cur_bonus = 0.0;

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

void cPhenotype::PrintStatus(ostream& fp)
{
  fp << "  MeritBase:"
     << CalcSizeMerit()
     << " Bonus: " << cur_bonus
     << " Errors:" << cur_num_errors
     << " Donates:" << cur_num_donates
     << " Tasks:";

  for (int i = 0; i < cur_task_count.GetSize(); i++) {
    fp << " " << cur_task_count[i];
  }

  fp << endl;
}

int cPhenotype::CalcSizeMerit() const
{
  assert(genome_length > 0);
  assert(executed_size > 0);
  assert(copied_size > 0);

  int out_size;

  switch (m_world->GetConfig().SIZE_MERIT_METHOD.Get()) {
  case SIZE_MERIT_COPIED:
    out_size = copied_size;
    break;
  case SIZE_MERIT_EXECUTED:
    out_size = executed_size;
    break;
  case SIZE_MERIT_FULL:
    out_size = genome_length;
    break;
  case SIZE_MERIT_LEAST:
    out_size = genome_length;
    if (out_size > copied_size) out_size = copied_size;
    if (out_size > executed_size)    out_size = executed_size;
    break;
  case SIZE_MERIT_SQRT_LEAST:
    out_size = genome_length;
    if (out_size > copied_size) out_size = copied_size;
    if (out_size > executed_size)    out_size = executed_size;
    out_size = (int) sqrt((double) out_size);
    break;
  case SIZE_MERIT_OFF:
  default:
    out_size = m_world->GetConfig().BASE_SIZE_MERIT.Get();
    break;
  }

  return out_size;
}
