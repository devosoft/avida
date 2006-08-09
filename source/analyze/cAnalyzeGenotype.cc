/*
 *  cAnalyzeGenotype.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cAnalyzeGenotype.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cHardwareManager.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cTestCPU.h"
#include "cEnvironment.h"
#include "cHardwareManager.h"
#include "cWorld.h"
#include "cWorldDriver.h"

using namespace std;

cAnalyzeGenotype::cAnalyzeGenotype(cWorld* world, cString symbol_string, cInstSet& in_inst_set)
  : m_world(world)
  , genome(symbol_string)
  , inst_set(in_inst_set)
  , name("")
  , aligned_sequence("")
  , tag("")
  , viable(false)
  , id_num(-1)
  , parent_id(-1)
  , parent2_id(-1)
  , num_cpus(0)
  , total_cpus(0)
  , update_born(0)
  , update_dead(0)
  , depth(0)
  , length(0)
  , copy_length(0)
  , exe_length(0)
  , merit(0.0)
  , gest_time(INT_MAX)
  , fitness(0.0)
  , errors(0)
  , task_counts(0)
  , fitness_ratio(0.0)
  , efficiency_ratio(0.0)
  , comp_merit_ratio(0.0)
  , parent_dist(0)
  , ancestor_dist(0)
  , parent_muts("")
  , knockout_stats(NULL)
  , m_land(NULL)
{
  // Make sure that the sequences jive with the inst_set
  for (int i = 0; i < genome.GetSize(); i++) {
    if (genome[i].GetOp() >= inst_set.GetSize()) {
      cString msg("Trying to load instruction ");
      msg += genome[i].GetOp();
      msg += ".  Max in set is";
      msg += (inst_set.GetSize() - 1);
      m_world->GetDriver().RaiseException(msg);
    }
  }
}

cAnalyzeGenotype::cAnalyzeGenotype(cWorld* world, const cGenome& _genome, cInstSet& in_inst_set)
  : m_world(world)
  , genome(_genome)
  , inst_set(in_inst_set)
  , name("")
  , aligned_sequence("")
  , tag("")
  , viable(false)
  , id_num(-1)
  , parent_id(-1)
  , parent2_id(-1)
  , num_cpus(0)
  , total_cpus(0)
  , update_born(0)
  , update_dead(0)
  , depth(0)
  , length(0)
  , copy_length(0)
  , exe_length(0)
  , merit(0.0)
  , gest_time(INT_MAX)
  , fitness(0.0)
  , errors(0)
  , task_counts(0)
  , fitness_ratio(0.0)
  , efficiency_ratio(0.0)
  , comp_merit_ratio(0.0)
  , parent_dist(0)
  , ancestor_dist(0)
  , parent_muts("")
  , knockout_stats(NULL)
  , m_land(NULL)
{
}

cAnalyzeGenotype::cAnalyzeGenotype(const cAnalyzeGenotype& _gen)
  : m_world(_gen.m_world)
  , genome(_gen.genome)
  , inst_set(_gen.inst_set)
  , name(_gen.name)
  , aligned_sequence(_gen.aligned_sequence)
  , tag(_gen.tag)
  , viable(_gen.viable)
  , id_num(_gen.id_num)
  , parent_id(_gen.parent_id)
  , parent2_id(_gen.parent2_id)
  , num_cpus(_gen.num_cpus)
  , total_cpus(_gen.total_cpus)
  , update_born(_gen.update_born)
  , update_dead(_gen.update_dead)
  , depth(_gen.depth)
  , length(_gen.length)
  , copy_length(_gen.copy_length)
  , exe_length(_gen.exe_length)
  , merit(_gen.merit)
  , gest_time(_gen.gest_time)
  , fitness(_gen.fitness)
  , errors(_gen.errors)
  , task_counts(_gen.task_counts)
  , fitness_ratio(_gen.fitness_ratio)
  , efficiency_ratio(_gen.efficiency_ratio)
  , comp_merit_ratio(_gen.comp_merit_ratio)
  , parent_dist(_gen.parent_dist)
  , ancestor_dist(_gen.ancestor_dist)
  , parent_muts(_gen.parent_muts)
  , knockout_stats(NULL)
  , m_land(NULL)
{
  if (_gen.knockout_stats != NULL) {
    knockout_stats = new cAnalyzeKnockouts;
    *knockout_stats = *(_gen.knockout_stats);
  }
}

cAnalyzeGenotype::~cAnalyzeGenotype()
{
  if (knockout_stats != NULL) delete knockout_stats;
}


int cAnalyzeGenotype::CalcMaxGestation() const
{
  return m_world->GetConfig().TEST_CPU_TIME_MOD.Get() * genome.GetSize();
}

void cAnalyzeGenotype::CalcKnockouts(bool check_pairs, bool check_chart) const
{
  if (knockout_stats == NULL) {
    // We've never called this before -- setup the stats.
    knockout_stats = new cAnalyzeKnockouts;
  }
  else if (check_pairs == true && knockout_stats->has_pair_info == false) {
    // We don't have the pair stats we need -- keep going.
    knockout_stats->Reset();
  }
  else if (check_chart == true && knockout_stats->has_chart_info == false) {
    // We don't have the phyenotype chart we need -- keep going.
    knockout_stats->Reset();
  }
  else {
    // We already have all the info we need -- just quit.
    return;
  }
  
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();

  cAvidaContext& ctx = m_world->GetDefaultContext();
  
  // Calculate the base fitness for the genotype we're working with...
  // (This may not have been run already, and cost negligiably more time
  // considering the number of knockouts we need to do.
  cAnalyzeGenotype base_genotype(m_world, genome, inst_set);
  base_genotype.Recalculate(ctx, testcpu);
  double base_fitness = base_genotype.GetFitness();
  const tArray<int> base_task_counts( base_genotype.GetTaskCounts() );
  
  // If the base fitness is 0, the organism is dead and has no complexity.
  if (base_fitness == 0.0) {
    knockout_stats->neut_count = length;
    delete testcpu;
    return;
  }
  
  cGenome mod_genome(genome);
  
  // Setup a NULL instruction in a special inst set.
  cInstSet ko_inst_set(inst_set);
  // Locate the instruction corresponding to "NULL" in the instruction library.
  {
    cInstruction lib_null_inst = ko_inst_set.GetInstLib()->GetInst("NULL");
    if (lib_null_inst == ko_inst_set.GetInstLib()->GetInstError()) {
      m_world->GetDriver().RaiseFatalException(1, "instruction 'NULL' not in current hardware type");
    }
    // Add mapping to located instruction. 
    ko_inst_set.AddInst(lib_null_inst.GetOp());
  }
  const cInstruction null_inst = ko_inst_set.GetInst("NULL");
  
  // If we are keeping track of the specific effects on tasks from the
  // knockouts, setup the matrix.
  if (check_chart == true) {
    knockout_stats->task_counts.Resize(length);
    knockout_stats->has_chart_info = true;
  }

  // Loop through all the lines of code, testing the removal of each.
  // -2=lethal, -1=detrimental, 0=neutral, 1=beneficial
  tArray<int> ko_effect(length);
  for (int line_num = 0; line_num < length; line_num++) {
    // Save a copy of the current instruction and replace it with "NULL"
    int cur_inst = mod_genome[line_num].GetOp();
    mod_genome[line_num] = null_inst;
    cAnalyzeGenotype ko_genotype(m_world, mod_genome, ko_inst_set);
    ko_genotype.Recalculate(ctx, testcpu);
    if (check_chart == true) {
      const tArray<int> ko_task_counts( ko_genotype.GetTaskCounts() );
      knockout_stats->task_counts[line_num] = ko_task_counts;
    }
    
    double ko_fitness = ko_genotype.GetFitness();
    if (ko_fitness == 0.0) {
      knockout_stats->dead_count++;
      ko_effect[line_num] = -2;
    } else if (ko_fitness < base_fitness) {
      knockout_stats->neg_count++;
      ko_effect[line_num] = -1;
    } else if (ko_fitness == base_fitness) {
      knockout_stats->neut_count++;
      ko_effect[line_num] = 0;
    } else if (ko_fitness > base_fitness) {
      knockout_stats->pos_count++;
      ko_effect[line_num] = 1;
    } else {
      m_world->GetDriver().RaiseException("internal: illegal state in CalcKnockouts()");
    }
    
    // Reset the mod_genome back to the original sequence.
    mod_genome[line_num].SetOp(cur_inst);
  }
  
  // Only continue from here if we are looking at all pairs of knockouts
  // as well.
  if (check_pairs == false) {
    delete testcpu;
    return;
  }
  
  tArray<int> ko_pair_effect(ko_effect);
  for (int line1 = 0; line1 < length; line1++) {
    // If this line has already been changed, keep going...
    if (ko_effect[line1] != ko_pair_effect[line1]) continue;
    
    // Loop through all possibilities for the next line.
    for (int line2 = line1+1; line2 < length; line2++) {
      // If this line has already been changed, keep going...
      if (ko_effect[line2] != ko_pair_effect[line2]) continue;
      
      // If the two lines are of different types (one is information and the
      // other is not) then we're not interested in testing this combination
      // since any possible result is reasonable.
      if ((ko_effect[line1] < 0 && ko_effect[line2] >= 0) ||
          (ko_effect[line1] >= 0 && ko_effect[line2] < 0)) {
        continue;
      }
      
      // Calculate the fitness for this pair of knockouts to determine if its
      // something other than what we expected.
      
      int cur_inst1 = mod_genome[line1].GetOp();
      int cur_inst2 = mod_genome[line2].GetOp();
      mod_genome[line1] = null_inst;
      mod_genome[line2] = null_inst;
      cAnalyzeGenotype ko_genotype(m_world, mod_genome, ko_inst_set);
      ko_genotype.Recalculate(ctx, testcpu);
      
      double ko_fitness = ko_genotype.GetFitness();
      
      // If the individual knockouts are both harmful, but in combination
      // they are neutral or even beneficial, they should not count as 
      // information.
      if (ko_fitness >= base_fitness &&
          ko_effect[line1] < 0 && ko_effect[line2] < 0) {
        ko_pair_effect[line1] = 0;
        ko_pair_effect[line2] = 0;
      }
      
      // If the individual knockouts are both neutral (or beneficial?),
      // but in combination they are harmful, they are likely redundant
      // to each other.  For now, count them both as information.
      if (ko_fitness < base_fitness &&
          ko_effect[line1] >= 0 && ko_effect[line2] >= 0) {
        ko_pair_effect[line1] = -1;
        ko_pair_effect[line2] = -1;
      }	
      
      // Reset the mod_genome back to the original sequence.
      mod_genome[line1].SetOp(cur_inst1);
      mod_genome[line2].SetOp(cur_inst2);
    }
  }
  
  for (int i = 0; i < length; i++) {
    if (ko_pair_effect[i] == -2) knockout_stats->pair_dead_count++;
    else if (ko_pair_effect[i] == -1) knockout_stats->pair_neg_count++;
    else if (ko_pair_effect[i] == 0) knockout_stats->pair_neut_count++;
    else if (ko_pair_effect[i] == 1) knockout_stats->pair_pos_count++;
  }
  
  knockout_stats->has_pair_info = true;
  delete testcpu;
}

void cAnalyzeGenotype::CheckLand() const
{
  if (m_land == NULL) {
    m_land = new cLandscape(m_world, genome, inst_set);
    m_land->SetDistance(1);
    m_land->Process(m_world->GetDefaultContext());
  }
}


void cAnalyzeGenotype::CalcLandscape(cAvidaContext& ctx)
{
  if (m_land == NULL) m_land = new cLandscape(m_world, genome, inst_set);
  m_land->SetDistance(1);
  m_land->Process(ctx);
}

void cAnalyzeGenotype::Recalculate(cAvidaContext& ctx, cTestCPU* testcpu, cAnalyzeGenotype* parent_genotype)
{
    // Build the test info for printing.
  cCPUTestInfo test_info;
  // test_info.TraceTaskOrder();

  // @DMB - This does some 'interesting' things with the instruction set
  
  // Use the inst lib for this genotype... and syncrhonize environment
  
  // Backup old instruction set, update with new
  cInstSet env_inst_set_backup = m_world->GetHardwareManager().GetInstSet();
  m_world->GetHardwareManager().GetInstSet() = inst_set;

  testcpu->TestGenome(ctx, test_info, genome);
  
  // Restore the instruction set
  m_world->GetHardwareManager().GetInstSet() = env_inst_set_backup;

  viable = test_info.IsViable();

  cOrganism* test_organism = test_info.GetTestOrganism();
  cPhenotype& test_phenotype = test_organism->GetPhenotype();

  length = test_organism->GetGenome().GetSize();
  copy_length = test_phenotype.GetCopiedSize();
  exe_length = test_phenotype.GetExecutedSize();
  merit = test_phenotype.GetMerit().GetDouble();
  gest_time = test_phenotype.GetGestationTime();
  fitness = test_phenotype.GetFitness();
  errors = test_phenotype.GetLastNumErrors();
  div_type = test_phenotype.GetDivType();
  mate_id = test_phenotype.MateSelectID();
  task_counts = test_phenotype.GetLastTaskCount();

  // Setup a new parent stats if we have a parent to work with.
  if (parent_genotype != NULL) {
    fitness_ratio = GetFitness() / parent_genotype->GetFitness();
    efficiency_ratio = GetEfficiency() / parent_genotype->GetEfficiency();
    comp_merit_ratio = GetCompMerit() / parent_genotype->GetCompMerit();
    parent_dist = cStringUtil::EditDistance(genome.AsString(),
		    parent_genotype->GetGenome().AsString(), parent_muts);
    ancestor_dist = parent_genotype->GetAncestorDist() + parent_dist;
  }
}


void cAnalyzeGenotype::PrintTasks(ofstream& fp, int min_task, int max_task)
{
  if (max_task == -1) max_task = task_counts.GetSize();

  for (int i = min_task; i < max_task; i++) {
    fp << task_counts[i] << " ";
  }
}

void cAnalyzeGenotype::SetSequence(cString _sequence)
{
  cGenome new_genome(_sequence);
  genome = new_genome;
}

int cAnalyzeGenotype::GetKO_DeadCount() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->dead_count;
}

int cAnalyzeGenotype::GetKO_NegCount() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->neg_count;
}

int cAnalyzeGenotype::GetKO_NeutCount() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->neut_count;
}

int cAnalyzeGenotype::GetKO_PosCount() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->pos_count;
}

int cAnalyzeGenotype::GetKO_Complexity() const
{
  CalcKnockouts(false);  // Make sure knockouts are calculated
  return knockout_stats->dead_count + knockout_stats->neg_count;
}

int cAnalyzeGenotype::GetKOPair_DeadCount() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_dead_count;
}

int cAnalyzeGenotype::GetKOPair_NegCount() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_neg_count;
}

int cAnalyzeGenotype::GetKOPair_NeutCount() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_neut_count;
}

int cAnalyzeGenotype::GetKOPair_PosCount() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_pos_count;
}

int cAnalyzeGenotype::GetKOPair_Complexity() const
{
  CalcKnockouts(true);  // Make sure knockouts are calculated
  return knockout_stats->pair_dead_count + knockout_stats->pair_neg_count;
}

const tArray< tArray<int> > & cAnalyzeGenotype::GetKO_TaskCounts() const
{
  CalcKnockouts(false, true);  // Make sure knockouts are calculated
  return knockout_stats->task_counts;
}

cString cAnalyzeGenotype::GetTaskList() const
{
  const int num_tasks = task_counts.GetSize();
  cString out_string(num_tasks);
  
  for (int i = 0; i < num_tasks; i++) {
    const int cur_count = task_counts[i];
    if (cur_count < 10) {
      out_string[i] = '0' + cur_count;
    }
    else if (cur_count < 30) {
      out_string[i] = 'X';
    }
    else if (cur_count < 80) {
      out_string[i] = 'L';
    }
    else if (cur_count < 300) {
      out_string[i] = 'C';
    }
    else if (cur_count < 800) {
      out_string[i] = 'D';
    }
    else if (cur_count < 3000) {
      out_string[i] = 'M';
    }
    else {
      out_string[i] = '+';
    }
  }

  return out_string;
}


cString cAnalyzeGenotype::GetHTMLSequence() const
{
  cString text_genome = genome.AsString();
  cString html_code("<tt>");

  cString diff_info = parent_muts;
  char mut_type = 'N';
  int mut_pos = -1;

  cString cur_mut = diff_info.Pop(',');
  if (cur_mut != "") {
    mut_type = cur_mut[0];
    cur_mut.ClipFront(1); cur_mut.ClipEnd(1);
    mut_pos = cur_mut.AsInt();
  }

  int ins_count = 0;
  for (int i = 0; i < genome.GetSize(); i++) {
    char symbol = text_genome[i];
    if (i != mut_pos) html_code += symbol;
    else {
      // Figure out the information for the type of mutation we had...
      cString color;
      if (mut_type == 'M') {
	color = "#FF0000";	
      } else if (mut_type == 'I') {
	color = "#00FF00";
	ins_count++;
      } else { // if (mut_type == 'D') {
	color = "#0000FF";
	symbol = '*';
	i--;  // Rewind - we didn't read the handle character yet!
      }

      // Move on to the next mutation...
      cur_mut = diff_info.Pop(',');
      if (cur_mut != "") {
	mut_type = cur_mut[0];
	cur_mut.ClipFront(1); cur_mut.ClipEnd(1);
	mut_pos = cur_mut.AsInt();
	if (mut_type == 'D') mut_pos += ins_count;
      } else mut_pos = -1;

      // Tack on the current symbol...
      cString symbol_string;
      symbol_string.Set("<b><font color=\"%s\">%c</font></b>", static_cast<const char*>(color), symbol);
      html_code += symbol_string;
    }
  }

  html_code += "</tt>";

  return html_code;
}
