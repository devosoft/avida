//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cAnalyzeGenotype.h"

#include "cCPUTestInfo.h"
#include "cInstSet.h"
#include "cLandscape.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cTestCPU.h"
#include "cEnvironment.h"
#include "cHardwareManager.h"
#include "cWorld.h"

using namespace std;

//////////////////////
//  cAnalyzeGenotype
//////////////////////

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
  , landscape_stats(NULL)
{
  // Make sure that the sequences jive with the inst_set
  for (int i = 0; i < genome.GetSize(); i++) {
    if (genome[i].GetOp() >= inst_set.GetSize()) {
      cerr << "Error: Trying to load instruction " << genome[i].GetOp()
	   << ".  Max in set is" << inst_set.GetSize() - 1
	   << endl;
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
  , landscape_stats(NULL)
{
}

cAnalyzeGenotype::cAnalyzeGenotype(const cAnalyzeGenotype & _gen)
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
  , landscape_stats(NULL)
{
  if (_gen.landscape_stats != NULL) {
    landscape_stats = new cAnalyzeLandscape;
    *landscape_stats = *(_gen.landscape_stats);
  }
}

cAnalyzeGenotype::~cAnalyzeGenotype()
{
  if (landscape_stats != NULL) delete landscape_stats;
}


int cAnalyzeGenotype::CalcMaxGestation() const
{
  return m_world->GetConfig().TEST_CPU_TIME_MOD.Get() * genome.GetSize();
}

void cAnalyzeGenotype::CalcLandscape() const
{
  if (landscape_stats != NULL) return;

  landscape_stats = new cAnalyzeLandscape;
  cLandscape landscape(m_world, genome, inst_set);
  landscape.Process(1);
  landscape_stats->frac_dead = landscape.GetProbDead();
  landscape_stats->frac_neg  = landscape.GetProbNeg();
  landscape_stats->frac_neut = landscape.GetProbNeut();
  landscape_stats->frac_pos  = landscape.GetProbPos();
  landscape_stats->complexity = landscape.GetComplexity();
}

void cAnalyzeGenotype::Recalculate(cAnalyzeGenotype * parent_genotype)
{
    // Build the test info for printing.
  cCPUTestInfo test_info;
  test_info.TestThreads();
  // test_info.TraceTaskOrder();

  // DDD - This does some 'interesting' things with the instruction set
  
  // Use the inst lib for this genotype... and syncrhonize environment
  
  // Backup old instruction set, update with new
  cInstSet env_inst_set_backup = m_world->GetHardwareManager().GetInstSet();
  m_world->GetHardwareManager().GetInstSet() = inst_set;

  m_world->GetTestCPU().TestGenome(test_info, genome);
  
  // Restore the instruction set
  m_world->GetHardwareManager().GetInstSet() = env_inst_set_backup;

  viable = test_info.IsViable();

  cOrganism * test_organism = test_info.GetTestOrganism();
  cPhenotype & test_phenotype = test_organism->GetPhenotype();

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


double cAnalyzeGenotype::GetFracDead() const
{
  CalcLandscape();  // Make sure the landscape is calculated...
  return landscape_stats->frac_dead;
}

double cAnalyzeGenotype::GetFracNeg() const
{
  CalcLandscape();  // Make sure the landscape is calculated...
  return landscape_stats->frac_neg;
}

double cAnalyzeGenotype::GetFracNeut() const
{
  CalcLandscape();  // Make sure the landscape is calculated...
  return landscape_stats->frac_neut;
}

double cAnalyzeGenotype::GetFracPos() const
{
  CalcLandscape();  // Make sure the landscape is calculated...
  return landscape_stats->frac_pos;
}

double cAnalyzeGenotype::GetComplexity() const
{
  CalcLandscape();  // Make sure the landscape is calculated...
  return landscape_stats->complexity;
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
      symbol_string.Set("<b><font color=\"%s\">%c</font></b>",
			color(), symbol);
      html_code += symbol_string;
    }
  }

  html_code += "</tt>";

  return html_code;
}
