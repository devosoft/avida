/*
 *  cStats.cc
 *  Avida
 *
 *  Called "stats.cc" prior to 12/5/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#include "cStats.h"

#include "cDataFile.h"
#include "cEnvironment.h"
#include "cGenotype.h"
#include "cHardwareManager.h"
#include "cInstSet.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStringUtil.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "tDataEntry.h"
#include "cOrgMessage.h"
#include "cOrgMessagePredicate.h"
#include "cOrgMovementPredicate.h"
#include "cReaction.h"
#include "cEventList.h"

#include "functions.h"

#include <cfloat>
#include <numeric>
#include <cmath>
#include <sstream>


cStats::cStats(cWorld* world)
  : m_world(world)
  , m_update(-1)
  , sub_update(0)
  , avida_time(0)
  , data_manager(this, "population_data")
  , rave_true_replication_rate( 500 )
  , entropy(0.0)
  , species_entropy(0.0)
  , energy(0.0)
  , dom_fidelity(0.0)
  , ave_fidelity(0.0)
  , max_viable_fitness(0)
  , dom_genotype(NULL)
  , dom_merit(0)
  , dom_gestation(0)
  , dom_repro_rate(0)
  , dom_fitness(0)
  , dom_size(0)
  , dom_copied_size(0)
  , dom_exe_size(0)
  , max_fitness(0)
  , max_merit(0)
  , max_gestation_time(0)
  , max_genome_length(0)
  , min_fitness(FLT_MAX)
  , min_merit(FLT_MAX)
  , min_gestation_time(INT_MAX)
  , min_genome_length(INT_MAX)
  , dom_genotype_id(-1)
  , dom_name("(none)")
  , dom_births(0)
  , dom_breed_true(0)
  , dom_breed_in(0)
  , dom_breed_out(0)
  , dom_abundance(0)
  , dom_gene_depth(-1)
  , dom_sequence("")
  , coal_depth(0)
  , num_births(0)
  , num_deaths(0)
  , num_breed_in(0)
  , num_breed_true(0)
  , num_breed_true_creatures(0)
  , num_creatures(0)
  , num_genotypes(0)
  , num_threshold(0)
  , num_species(0)
  , num_thresh_species(0)
  , num_lineages(0)
  , num_executed(0)
  , num_parasites(0)
  , num_no_birth_creatures(0)
  , num_single_thread_creatures(0)
  , num_multi_thread_creatures(0)
  , m_num_threads(0)
  , num_modified(0)
  , num_genotypes_last(1)
  , tot_organisms(0)
  , tot_genotypes(0)
  , tot_threshold(0)
  , tot_species(0)
  , tot_thresh_species(0)
  , tot_lineages(0)
  , tot_executed(0)
  , num_resamplings(0)
  , num_failedResamplings(0)
  , last_update(0)
  , num_bought(0)
  , num_sold(0)
  , num_used(0)
  , num_own_used(0)
  , sense_size(0)
  , avg_competition_fitness(0)
  , min_competition_fitness(0)
  , max_competition_fitness(0)
  , avg_competition_copied_fitness(0)
  , min_competition_copied_fitness(0)
  , max_competition_copied_fitness(0)
  , num_orgs_replicated(0)
  , m_spec_total(0)
  , m_spec_num(0)
  , m_spec_waste(0)
  , m_deme_num_repls(0)
{
  const cEnvironment& env = m_world->GetEnvironment();
  const int num_tasks = env.GetNumTasks();
    
  task_cur_count.Resize(num_tasks);
  task_last_count.Resize(num_tasks);
  task_cur_quality.Resize(num_tasks);
  task_last_quality.Resize(num_tasks);
  task_cur_max_quality.Resize(num_tasks);
  task_last_max_quality.Resize(num_tasks);
  task_exe_count.Resize(num_tasks);
  task_cur_count.SetAll(0);
  task_cur_quality.SetAll(0);
  task_cur_max_quality.SetAll(0);
  task_last_max_quality.SetAll(0);
  task_last_quality.SetAll(0);
  task_last_count.SetAll(0);
  task_cur_max_quality.SetAll(0);
  task_last_max_quality.SetAll(0);
  task_exe_count.SetAll(0);
  
  // Stats for internal resource use
  task_internal_cur_count.Resize(num_tasks);
  task_internal_last_count.Resize(num_tasks);
  task_internal_cur_quality.Resize(num_tasks);
  task_internal_last_quality.Resize(num_tasks);
  task_internal_cur_max_quality.Resize(num_tasks);
  task_internal_last_max_quality.Resize(num_tasks);
  task_internal_cur_count.SetAll(0);
  task_internal_last_count.SetAll(0);
  task_internal_cur_quality.SetAll(0.0);
  task_internal_last_quality.SetAll(0.0);
  task_internal_cur_max_quality.SetAll(0.0);
  task_internal_last_max_quality.SetAll(0.0);
  

#if INSTRUCTION_COUNT
  sum_exe_inst_array.Resize(m_world->GetNumInstructions());
  ZeroInst();
#endif
  inst_names.Resize(m_world->GetNumInstructions());

  const int num_reactions = env.GetNumReactions();
  m_reaction_cur_count.Resize(num_reactions);
  m_reaction_last_count.Resize(num_reactions);
  m_reaction_cur_add_reward.Resize(num_reactions);
  m_reaction_last_add_reward.Resize(num_reactions);
  m_reaction_exe_count.Resize(num_reactions);
  m_reaction_cur_count.SetAll(0);
  m_reaction_last_count.SetAll(0);
  m_reaction_cur_add_reward.SetAll(0.0);
  m_reaction_last_add_reward.SetAll(0.0);
  m_reaction_exe_count.SetAll(0);
  

  resource_count.Resize( m_world->GetNumResources() );
  resource_count.SetAll(0);

  resource_geometry.Resize( m_world->GetNumResources() );
  resource_geometry.SetAll(nGeometry::GLOBAL);

  task_names.Resize(num_tasks);
  for (int i = 0; i < num_tasks; i++) task_names[i] = env.GetTask(i).GetDesc();
  
  reaction_names.Resize(num_reactions);
  for (int i = 0; i < num_reactions; i++) reaction_names[i] = env.GetReactionName(i);
  
  resource_names.Resize( m_world->GetNumResources() );

  // This block calculates how many slots we need to
  // make for paying attention to different label combinations 
  // Require sense instruction to be present then die if not at least 2 NOPs
  
  bool sense_used = m_world->GetHardwareManager().GetInstSet().InstInSet( cStringUtil::Stringf("sense") )
                ||  m_world->GetHardwareManager().GetInstSet().InstInSet( cStringUtil::Stringf("sense-unit") )
                ||  m_world->GetHardwareManager().GetInstSet().InstInSet( cStringUtil::Stringf("sense-m100") );
  if (sense_used)
  {
    if (m_world->GetHardwareManager().GetInstSet().GetNumNops() < 2)
    {
      cerr << "Error: If you have a sense instruction in your instruction set, then";
      cerr << "you MUST also include at least two NOPs in your instruction set. " << endl; exit(1);
    }
  
    int on = 1;
    int max_sense_label_length = 0;
    while (on < m_world->GetNumResources())
    {
      max_sense_label_length++;
      sense_size += on;
      on *= m_world->GetHardwareManager().GetInstSet().GetNumNops();
    }
    sense_size += on;
    
    sense_last_count.Resize( sense_size );
    sense_last_count.SetAll(0);
      
    sense_last_exe_count.Resize( sense_size );
    sense_last_exe_count.SetAll(0);
    
    sense_names.Resize( sense_size );
    int assign_index = 0;
    int num_per = 1;
    for (int i=0; i<= max_sense_label_length; i++)
    {
      for (int j=0; j< num_per; j++)
      {
        sense_names[assign_index] = (on > 1) ? 
          cStringUtil::Stringf("sense_res.%i-%i", j*on, (j+1)*on-1) :
          cStringUtil::Stringf("sense_res.%i", j);
    
        assign_index++;
      }
      on /= m_world->GetHardwareManager().GetInstSet().GetNumNops();
      num_per *= m_world->GetHardwareManager().GetInstSet().GetNumNops();
    }
  }
  // End sense tracking initialization

  genotype_map.Resize( m_world->GetConfig().WORLD_X.Get() * m_world->GetConfig().WORLD_Y.Get() );

  if(m_world->GetConfig().NUM_DEMES.Get() == 0) {
    relative_pos_event_count.ResizeClear(m_world->GetConfig().WORLD_X.Get(), m_world->GetConfig().WORLD_Y.Get()); 
    relative_pos_pred_sat.ResizeClear(m_world->GetConfig().WORLD_X.Get(), m_world->GetConfig().WORLD_Y.Get());
  } else {
    relative_pos_event_count.ResizeClear(m_world->GetConfig().WORLD_X.Get(), m_world->GetConfig().WORLD_Y.Get() / m_world->GetConfig().NUM_DEMES.Get());
    relative_pos_pred_sat.ResizeClear(m_world->GetConfig().WORLD_X.Get(), m_world->GetConfig().WORLD_Y.Get() / m_world->GetConfig().NUM_DEMES.Get());
  }
  
  relative_pos_event_count.SetAll(0);
  relative_pos_pred_sat.SetAll(0);

  SetupPrintDatabase();
}

void cStats::SetupPrintDatabase()
{
  // Load in all the keywords, descriptions, and associated functions for
  // data management.

  // Time Stats
  data_manager.Add("update",      "Update",      &cStats::GetUpdate);
  data_manager.Add("sub_update",  "Sub-Update",  &cStats::GetSubUpdate);
  data_manager.Add("generation",  "Generation",  &cStats::GetGeneration);

  // Population Level Stats
  data_manager.Add("entropy",         "Genotype Entropy (Diversity)", &cStats::GetEntropy);
  data_manager.Add("species_entropy", "Species Entropy (Diversity)",  &cStats::GetEntropy);
  data_manager.Add("energy",          "Average Inferiority (Energy)", &cStats::GetEnergy);
  data_manager.Add("richness",        "Number of Different Genotypes (Richness)", &cStats::GetNumGenotypes);
  data_manager.Add("eveness",         "Equitability of Genotype Distribution (Evenness)", &cStats::GetEvenness);
  data_manager.Add("coal_depth",      "Depth of Coalescent Genotype", &cStats::GetCoalescentDepth);
  data_manager.Add("num_resamplings",  "Total Number of resamplings this time step", &cStats::GetResamplings);
  data_manager.Add("num_failedResamplings",  "Total Number of divide commands that reached the resampling hard-cap this time step", &cStats::GetFailedResamplings);


  // Dominant Genotype Stats
  data_manager.Add("dom_merit",      "Ave Merit of Dominant Genotype",          &cStats::GetDomMerit);
  data_manager.Add("dom_gest",       "Ave Gestation Time of Dominant Genotype", &cStats::GetDomGestation);
  data_manager.Add("dom_fitness",    "Ave Fitness of Dominant Genotype",        &cStats::GetDomFitness);
  data_manager.Add("dom_repro",      "Ave Repro-Rate of Dominant Genotype",     &cStats::GetDomReproRate);
  data_manager.Add("dom_length",     "Genome Length of Dominant Genotype",      &cStats::GetDomSize);
  data_manager.Add("dom_copy_length","Copied Length of Dominant Genotype",      &cStats::GetDomCopySize);
  data_manager.Add("dom_exe_length", "Executed Length of Dominant Genotype",    &cStats::GetDomExeSize);
  data_manager.Add("dom_id",         "ID of Dominant Genotype",                 &cStats::GetDomID);
  data_manager.Add("dom_name",       "Name of Dominant Genotype",               &cStats::GetDomName);
  data_manager.Add("dom_births",     "Birth Count of Dominant Genotype",        &cStats::GetDomBirths);
  data_manager.Add("dom_breed_true", "Breed-True Count  of Dominant Genotype",  &cStats::GetDomBreedTrue);
  data_manager.Add("dom_breed_in",   "Breed-In Count of Dominant Genotype",     &cStats::GetDomBreedIn);
  data_manager.Add("dom_breed_out",  "Breed-Out Count of Dominant Genotype",    &cStats::GetDomBreedOut);
  data_manager.Add("dom_num_cpus",   "Abundance of Dominant Genotype",          &cStats::GetDomAbundance);
  data_manager.Add("dom_depth",      "Tree Depth of Dominant Genotype",         &cStats::GetDomGeneDepth);
  data_manager.Add("dom_sequence",   "Sequence of Dominant Genotype",           &cStats::GetDomSequence);

  // Dominant Inject Genotype Stats
  data_manager.Add("dom_inj_size",      "Genome Length of Dominant Parasite",   &cStats::GetDomInjSize);
  data_manager.Add("dom_inj_ID",        "ID of Dominant Parasite",              &cStats::GetDomInjID);
  data_manager.Add("dom_inj_name",      "Nameof Dominant Parasite",             &cStats::GetDomInjName);
  data_manager.Add("dom_inj_births",    "Birth Count of Dominant Parasite",     &cStats::GetDomInjBirths);
  data_manager.Add("dom_inj_abundance", "Abundance of Dominant Parasite",       &cStats::GetDomInjAbundance);
  data_manager.Add("dom_inj_sequence",  "Sequence of Dominant Parasite",        &cStats::GetDomInjSequence);
  
  // Current Counts...
  data_manager.Add("num_births",     "Count of Births in Population",          &cStats::GetNumBirths);
  data_manager.Add("num_deaths",     "Count of Deaths in Population",          &cStats::GetNumDeaths);
  data_manager.Add("breed_in",       "Count of Non-Breed-True Births",         &cStats::GetBreedIn);
  data_manager.Add("breed_true",     "Count of Breed-True Births",             &cStats::GetBreedTrue);
  data_manager.Add("bred_true",      "Count of Organisms that have Bred True", &cStats::GetBreedTrueCreatures);
  data_manager.Add("num_cpus",       "Count of Organisms in Population",       &cStats::GetNumCreatures);
  data_manager.Add("num_genotypes",  "Count of Genotypes in Population",       &cStats::GetNumGenotypes);
  data_manager.Add("num_threshold",  "Count of Threshold Genotypes",           &cStats::GetNumThreshold);
  data_manager.Add("num_species",    "Count of Species in Population",         &cStats::GetNumSpecies);
  data_manager.Add("thresh_species", "Count of Threshold Species",             &cStats::GetNumThreshSpecies);
  data_manager.Add("num_lineages",   "Count of Lineages in Population",        &cStats::GetNumLineages);
  data_manager.Add("num_parasites",  "Count of Parasites in Population",       &cStats::GetNumParasites);
  data_manager.Add("num_no_birth",   "Count of Childless Organisms",           &cStats::GetNumNoBirthCreatures);

  // Total Counts...
  data_manager.Add("tot_cpus",      "Total Organisms ever in Population", &cStats::GetTotCreatures);
  data_manager.Add("tot_genotypes", "Total Genotypes ever in Population", &cStats::GetTotGenotypes);
  data_manager.Add("tot_threshold", "Total Threshold Genotypes Ever",     &cStats::GetTotThreshold);
  data_manager.Add("tot_species",   "Total Species ever in Population",   &cStats::GetTotSpecies);
  data_manager.Add("tot_lineages",  "Total Lineages ever in Population",  &cStats::GetTotLineages);

  // Some Average Data...
  data_manager.Add("ave_repro_rate", "Average Repro-Rate (1/Gestation)", &cStats::GetAveReproRate);
  data_manager.Add("ave_merit",      "Average Merit",                    &cStats::GetAveMerit);
  data_manager.Add("ave_age",        "Average Age",                      &cStats::GetAveCreatureAge);
  data_manager.Add("ave_memory",     "Average Memory Used",              &cStats::GetAveMemSize);
  data_manager.Add("ave_neutral",    "Average Neutral Metric",           &cStats::GetAveNeutralMetric);
  data_manager.Add("ave_lineage",    "Average Lineage Label",            &cStats::GetAveLineageLabel);
  data_manager.Add("ave_gest",       "Average Gestation Time",           &cStats::GetAveGestation);
  data_manager.Add("ave_fitness",    "Average Fitness",                  &cStats::GetAveFitness);
  data_manager.Add("ave_gen_age",    "Average Genotype Age",             &cStats::GetAveGenotypeAge);
  data_manager.Add("ave_length",     "Average Genome Length",            &cStats::GetAveSize);
  data_manager.Add("ave_copy_length","Average Copied Length",            &cStats::GetAveCopySize);
  data_manager.Add("ave_exe_length", "Average Executed Length",          &cStats::GetAveExeSize);
  data_manager.Add("ave_thresh_age", "Average Threshold Genotype Age",   &cStats::GetAveThresholdAge);
  data_manager.Add("ave_species_age","Average Species Age",              &cStats::GetAveSpeciesAge);

  // And a couple of Maximums
  data_manager.Add("max_fitness", "Maximum Fitness in Population", &cStats::GetMaxFitness);
  data_manager.Add("max_merit",   "Maximum Merit in Population",   &cStats::GetMaxMerit);
}

void cStats::ZeroTasks()
{
  task_cur_count.SetAll(0);
  task_last_count.SetAll(0);
  task_cur_quality.SetAll(0);
  task_last_quality.SetAll(0);
  task_last_max_quality.SetAll(0);
  task_cur_max_quality.SetAll(0);
  task_internal_cur_count.SetAll(0);
  task_internal_cur_quality.SetAll(0);
  task_internal_cur_max_quality.SetAll(0);
  task_internal_last_count.SetAll(0);
  task_internal_last_quality.SetAll(0);
  task_internal_last_max_quality.SetAll(0);
}

void cStats::ZeroReactions()
{
  m_reaction_cur_count.SetAll(0);
  m_reaction_last_count.SetAll(0);
  m_reaction_cur_add_reward.SetAll(0);
  m_reaction_last_add_reward.SetAll(0);
}


#if INSTRUCTION_COUNT
void cStats::ZeroInst()
{
  for (int i = 0; i < sum_exe_inst_array.GetSize(); i++) {
    sum_exe_inst_array[i].Clear();
  }
}
#endif

void cStats::CalcEnergy()
{
  assert(sum_fitness.Average() >= 0.0);
  assert(dom_fitness >= 0);

  
  // Note: When average fitness and dominant fitness are close in value (i.e. should be identical)
  //       floating point rounding error can cause output variances.  To mitigate this, threshold
  //       caps off values that differ by less than it, flushing the effective output value to zero.
  const double ave_fitness = sum_fitness.Average();
  const double threshold = 1.0e-14;
  if (ave_fitness == 0.0 || dom_fitness == 0.0 || fabs(ave_fitness - dom_fitness) < threshold) {
    energy = 0.0;
  } else  {
    energy = Log(dom_fitness / ave_fitness);
  }
}

void cStats::CalcFidelity()
{
  // There is a (small) probability that when a random instruction is picked
  // after a mutation occurs, that it will be the original instruction again;
  // This needs to be adjusted for!

  double adj = static_cast<double>(m_world->GetNumInstructions() - 1) /
    static_cast<double>(m_world->GetNumInstructions());

  double base_fidelity = (1.0 - adj * m_world->GetConfig().DIVIDE_MUT_PROB.Get()) *
    (1.0 - m_world->GetConfig().DIVIDE_INS_PROB.Get()) * (1.0 - m_world->GetConfig().DIVIDE_DEL_PROB.Get());

  double true_cm_rate = adj * m_world->GetConfig().COPY_MUT_PROB.Get();
  ave_fidelity = base_fidelity * pow(1.0 - true_cm_rate, sum_size.Average());
  dom_fidelity = base_fidelity * pow(1.0 - true_cm_rate, dom_size);
}

void cStats::RecordBirth(int cell_id, int genotype_id, bool breed_true)
{

	
	if (m_world->GetEventsList()->CheckBirthInterruptQueue(tot_organisms) == true)
		m_world->GetEventsList()->ProcessInterrupt(m_world->GetDefaultContext());
		
  tot_organisms++;
  num_births++;

  if (m_world->GetConfig().LOG_CREATURES.Get()) {
    cDataFile& df = m_world->GetDataFile("creature.log");
    df.Write(m_update, "Update");
    df.Write(cell_id, "Cell ID");
    df.Write(genotype_id, "Genotype ID");
    df.Endl();
  }

  if (breed_true) num_breed_true++;
  else num_breed_in++;
}


void cStats::RemoveGenotype(int id_num, int parent_id,
   int parent_dist, int depth, int max_abundance, int parasite_abundance,
   int age, int length)
{
  if (m_world->GetConfig().LOG_GENOTYPES.Get() &&
      (m_world->GetConfig().LOG_GENOTYPES.Get() != 2 || max_abundance > 2)) {
    const int update_born = cStats::GetUpdate() - age + 1;
    cDataFile& df = m_world->GetDataFile("genotype.log");
    df.Write(id_num, "Genotype ID");
    df.Write(update_born, "Update Born");
    df.Write(parent_id, "Parent ID");
    df.Write(parent_dist, "Parent Distance");
    df.Write(depth, "Depth");
    df.Write(max_abundance, "Maximum Abundance");
    df.Write(age, "Age");
    df.Write(length, "Length");
    df.Endl();
  }

  (void) parasite_abundance; // Not used now, but maybe in future.
}

void cStats::AddThreshold(int id_num, const char* name, int species_num)
{
  num_threshold++;
  tot_threshold++;
  if (m_world->GetConfig().LOG_THRESHOLD.Get()) {
    cDataFile& df = m_world->GetDataFile("threshold.log");
    df.Write(m_update, "Update");
    df.Write(id_num, "ID");
    df.Write(species_num, "Species Num");
    df.Write(name, "Name");
    df.Endl();
  }
}


void cStats::RemoveSpecies(int id_num, int parent_id, int max_gen_abundance, int max_abundance, int age)
{
  num_species--;
  if (m_world->GetConfig().LOG_SPECIES.Get()) {
    cDataFile& df = m_world->GetDataFile("species.log");
    df.Write(m_update, "Update");
    df.Write(id_num, "Species ID");
    df.Write(parent_id, "Parent ID");
    df.Write(max_gen_abundance, "Maximum Gen Abundance");
    df.Write(max_abundance, "Maximum Abundance");
    df.Write(age, "Age");
    df.Endl();
  }
}

void cStats::ProcessUpdate()
{
  // Increment the "avida_time"
  if (sum_merit.Count() > 0 && sum_merit.Average() > 0) {
    double delta = ((double)(m_update-last_update))/sum_merit.Average();
    avida_time += delta;

    // calculate the true replication rate in this update
    rave_true_replication_rate.Add( num_births/
	  (delta * m_world->GetConfig().AVE_TIME_SLICE.Get() * num_creatures) );
  }
  last_update = m_update;

  // Zero-out any variables which need to be cleared at end of update.

  num_births = 0;
  num_deaths = 0;
  num_breed_true = 0;

  tot_executed += num_executed;
  num_executed = 0;

  task_cur_count.SetAll(0);
  task_last_count.SetAll(0);
  task_cur_quality.SetAll(0);
  task_last_quality.SetAll(0);
  task_cur_max_quality.SetAll(0);
  task_last_max_quality.SetAll(0);
  task_exe_count.SetAll(0);
  
  task_internal_cur_count.SetAll(0);
  task_internal_last_count.SetAll(0);
  task_internal_cur_quality.SetAll(0);
  task_internal_last_quality.SetAll(0);
  task_internal_cur_max_quality.SetAll(0);
  task_internal_last_max_quality.SetAll(0);
  
  sense_last_count.SetAll(0);
  sense_last_exe_count.SetAll(0);

  m_reaction_cur_count.SetAll(0);
  m_reaction_last_count.SetAll(0);
  m_reaction_cur_add_reward.SetAll(0.0);
  m_reaction_last_add_reward.SetAll(0.0);
  m_reaction_exe_count.SetAll(0);

  dom_merit = 0;
  dom_gestation = 0.0;
  dom_fitness = 0.0;
  max_fitness = 0.0;

  num_resamplings = 0;
  num_failedResamplings = 0;

  m_spec_total = 0;
  m_spec_num = 0;
  m_spec_waste = 0;
}

void cStats::RemoveLineage(int id_num, int parent_id, int update_born, double generation_born, int total_CPUs,
                           int total_genotypes, double fitness, double lineage_stat1, double lineage_stat2 )
{
  num_lineages--;
  if (m_world->GetConfig().LOG_LINEAGES.Get()) {
    cDataFile& lineage_log = m_world->GetDataFile("lineage.log");

    lineage_log.WriteComment("Columns 10, 11 depend on lineage creation method chosen.");
    
    lineage_log.Write(id_num, "lineage id");
    lineage_log.Write(parent_id, "parent lineage id");
    lineage_log.Write(fitness, "initial fitness");
    lineage_log.Write(total_CPUs, "total number of creatures");
    lineage_log.Write(total_genotypes, "total number of genotypes");
    lineage_log.Write(update_born, "update born");
    lineage_log.Write(cStats::GetUpdate(), "update extinct");
    lineage_log.Write(generation_born, "generation born");
    lineage_log.Write(SumGeneration().Average(), "generation extinct");
    lineage_log.Write(lineage_stat1, "lineage stat1");
    lineage_log.Write(lineage_stat2, "lineage stat2");
    lineage_log.Endl();
  }
}


void cStats::PrintDataFile(const cString& filename, const cString& format, char sep)
{
  cDataFile& data_file = m_world->GetDataFile(filename);
  data_manager.PrintRow(data_file, format, sep);
}


void cStats::PrintAverageData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida Average Data");
  df.WriteTimeStamp();

  df.Write(m_update,                "Update");
  df.Write(sum_merit.Average(),           "Merit");
  df.Write(sum_gestation.Average(),       "Gestation Time");
  df.Write(sum_fitness.Average(),         "Fitness");
  df.Write(sum_repro_rate.Average(),      "Repro Rate?");
  df.Write(sum_size.Average(),            "Size");
  df.Write(sum_copy_size.Average(),       "Copied Size");
  df.Write(sum_exe_size.Average(),        "Executed Size");
  df.Write(sum_abundance.Average(),       "Abundance");
  
  // The following causes births and breed true to default to 0.0 when num_creatures is 0
  double ave_births = 0.0;
  double ave_breed_true = 0.0;
  if (num_creatures > 0) {
    const double d_num_creatures = static_cast<double>(num_creatures);
    ave_births = static_cast<double>(num_births) / d_num_creatures;
    ave_breed_true = static_cast<double>(num_breed_true) / d_num_creatures;
  }
  df.Write(ave_births,                    "Proportion of organisms that gave birth in this update");
  df.Write(ave_breed_true,                "Proportion of Breed True Organisms");
  
  df.Write(sum_genotype_depth.Average(),  "Genotype Depth");
  df.Write(sum_generation.Average(),      "Generation");
  df.Write(sum_neutral_metric.Average(),  "Neutral Metric");
  df.Write(sum_lineage_label.Average(),   "Lineage Label");
  df.Write(rave_true_replication_rate.Average(), "True Replication Rate (based on births/update, time-averaged)");
  df.Endl();
}

void cStats::PrintDemeAverageData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida Average Deme Data");
  df.WriteTimeStamp();

  df.Write(m_update,                                        "Update");
  df.Write(m_num_occupied_demes,                            "Count");
  df.Write(sum_deme_age.Average(),                          "Age");
  df.Write(sum_deme_birth_count.Average(),                  "Births");
  df.Write(sum_deme_org_count.Average(),                    "Organisms");
  df.Write(sum_deme_generation.Average(),                   "Generation");
  df.Write(sum_deme_last_birth_count.Average(),                  "Births (at last replication)");
  df.Write(sum_deme_last_org_count.Average(),                    "Organisms (at last replication)");
  df.Write(sum_deme_merit.Average(),                        "Merit");
  df.Write(sum_deme_gestation_time.Average(),               "Gestation Time");
  df.Write(sum_deme_normalized_time_used.Average(),         "Time Used (normalized by org fitness)");
  df.Write(sum_deme_generations_per_lifetime.Average(),     "Generations between current and last founders");
  df.Write(sum_deme_events_killed.Average(),                "Events killed");
  df.Write(sum_deme_events_kill_attempts.Average(),         "Attempts to kill event");
  
  df.Endl();
}

void cStats::PrintFlowRateTuples(const cString& filename) {
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Flow Rate Tuples");
  df.WriteTimeStamp();

  df.Write(m_update,                                        "Update");
  // write each tuple
  for(map<int, flow_rate_tuple>::iterator iter = flow_rate_tuples.begin(); iter != flow_rate_tuples.end(); iter++) {
    ostringstream oss;
    oss << "flow rate " << (*iter).first;
    string flow_rate_str(oss.str());
    string flow_rate_pop_size_str(flow_rate_str+" deme pop size");
    string flow_rate_events_killed_str(flow_rate_str+" events killed");
    string flow_rate_events_attempted_to_kill_str(flow_rate_str+" events attempted to kill");
    string flow_rate_exe_ratio_str(flow_rate_str+" exe ratio");
    string flow_rate_total_births_str(flow_rate_str+" total births");
    string flow_rate_total_sleeping_str(flow_rate_str+" total sleeping");
    
    df.Write((*iter).first, flow_rate_str.c_str());
    df.Write((*iter).second.orgCount.Average(), flow_rate_pop_size_str.c_str());
    df.Write((*iter).second.eventsKilled.Average(), flow_rate_events_killed_str.c_str());
    df.Write((*iter).second.attemptsToKillEvents.Average(), flow_rate_events_attempted_to_kill_str.c_str());
    df.Write((*iter).second.AvgEnergyUsageRatio.Average(), flow_rate_exe_ratio_str.c_str());
    df.Write((*iter).second.totalBirths.Average(), flow_rate_total_births_str.c_str());
    df.Write((*iter).second.currentSleeping.Average(), flow_rate_total_sleeping_str.c_str());
    
  }
  df.Endl();

  // reset all tuples
  for(map<int, flow_rate_tuple >::iterator iter = flow_rate_tuples.begin(); iter != flow_rate_tuples.end(); iter++) {
    (*iter).second.orgCount.Clear();
    (*iter).second.eventsKilled.Clear();
    (*iter).second.attemptsToKillEvents.Clear();
    (*iter).second.AvgEnergyUsageRatio.Clear();
    (*iter).second.totalBirths.Clear();
    (*iter).second.currentSleeping.Clear();
  }
}

void cStats::PrintErrorData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Avida Standard Error Data");
  df.WriteTimeStamp();

  df.Write(m_update,                 "Update");
  df.Write(sum_merit.StdError(),           "Merit");
  df.Write(sum_gestation.StdError(),       "Gestation Time");
  df.Write(sum_fitness.StdError(),         "Fitness");
  df.Write(sum_repro_rate.StdError(),      "Repro Rate?");
  df.Write(sum_size.StdError(),            "Size");
  df.Write(sum_copy_size.StdError(),       "Copied Size");
  df.Write(sum_exe_size.StdError(),        "Executed Size");
  df.Write(sum_abundance.StdError(),       "Abundance");
  df.Write(-1,                             "(No Data)");
  df.Write(-1,                             "(No Data)");
  df.Write(sum_genotype_depth.StdError(),  "Genotype Depth");
  df.Write(sum_generation.StdError(),      "Generation");
  df.Write(sum_neutral_metric.StdError(),  "Neutral Metric");
  df.Write(sum_lineage_label.StdError(),   "Lineage Label");
  df.Write(rave_true_replication_rate.StdError(), "True Replication Rate (based on births/update, time-averaged)");
  df.Endl();
}


void cStats::PrintVarianceData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida Variance Data");
  df.WriteTimeStamp();

  df.Write(m_update,                 "Update");
  df.Write(sum_merit.Variance(),           "Merit");
  df.Write(sum_gestation.Variance(),       "Gestation Time");
  df.Write(sum_fitness.Variance(),         "Fitness");
  df.Write(sum_repro_rate.Variance(),      "Repro Rate?");
  df.Write(sum_size.Variance(),            "Size");
  df.Write(sum_copy_size.Variance(),       "Copied Size");
  df.Write(sum_exe_size.Variance(),        "Executed Size");
  df.Write(sum_abundance.Variance(),       "Abundance");
  df.Write(-1,                             "(No Data)");
  df.Write(-1,                             "(No Data)");
  df.Write(sum_genotype_depth.Variance(),  "Genotype Depth");
  df.Write(sum_generation.Variance(),      "Generation");
  df.Write(sum_neutral_metric.Variance(),  "Neutral Metric");
  df.Write(sum_lineage_label.Variance(),   "Lineage Label");
  df.Write(rave_true_replication_rate.Variance(), "True Replication Rate (based on births/update, time-averaged)");
  df.Endl();
}


void cStats::PrintDominantData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida Dominant Data");
  df.WriteTimeStamp();

  df.Write(m_update,     "Update");
  df.Write(dom_merit,       "Average Merit of the Dominant Genotype");
  df.Write(dom_gestation,   "Average Gestation Time of the Dominant Genotype");
  df.Write(dom_fitness,     "Average Fitness of the Dominant Genotype");
  df.Write(dom_repro_rate,  "Repro Rate?");
  df.Write(dom_size,        "Size of Dominant Genotype");
  df.Write(dom_copied_size, "Copied Size of Dominant Genotype");
  df.Write(dom_exe_size,    "Executed Size of Dominant Genotype");
  df.Write(dom_abundance,   "Abundance of Dominant Genotype");
  df.Write(dom_births,      "Number of Births");
  df.Write(dom_breed_true,  "Number of Dominant Breed True?");
  df.Write(dom_gene_depth,  "Dominant Gene Depth");
  df.Write(dom_breed_in,    "Dominant Breed In");
  df.Write(max_fitness,     "Max Fitness?");
  df.Write(dom_genotype_id, "Genotype ID of Dominant Genotype");
  df.Write(dom_name,        "Name of the Dominant Genotype");
  df.Endl();
}

void cStats::PrintParasiteData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida Dominant Parasite Data");
  df.WriteTimeStamp();

  df.Write(m_update, "Update");
  df.Write(num_parasites, "Number of Extant Parasites");
  df.Write(dom_inj_size, "Size of Dominant Parasite Genotype");
  df.Write(dom_inj_abundance, "Abundance of Dominant Parasite Genotype");
  df.Write(dom_inj_genotype_id, "Genotype ID of Dominant Parasite Genotype");
  df.Write(dom_inj_name, "Name of the Dominant Parasite Genotype");
  df.Endl();
}

void cStats::PrintStatsData(const cString& filename)
{
  const int genotype_change = num_genotypes - num_genotypes_last;
  const double log_ave_fid = (ave_fidelity > 0) ? -Log(ave_fidelity) : 0.0;
  const double log_dom_fid = (dom_fidelity > 0) ? -Log(dom_fidelity) : 0.0;

  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Generic Statistics Data");
  df.WriteTimeStamp();

  df.Write(m_update,          "update");
  df.Write(energy,               "average inferiority (energy)");
  df.Write(1.0 - ave_fidelity,   "ave probability of any mutations in genome");
  df.Write(1.0 - dom_fidelity,   "probability of any mutations in dom genome");
  df.Write(log_ave_fid,          "log(average fidelity)");
  df.Write(log_dom_fid,          "log(dominant fidelity)");
  df.Write(genotype_change,      "change in number of genotypes");
  df.Write(entropy,              "genotypic entropy");
  df.Write(species_entropy,      "species entropy");
  df.Write(coal_depth,           "depth of most reacent coalescence");
  df.Write(num_resamplings,      "Total number of resamplings this generation");
  df.Write(num_failedResamplings, "Total number of organisms that failed to resample this generation"); 

  df.Endl();
}


void cStats::PrintCountData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida count data");
  df.WriteTimeStamp();

  df.Write(m_update,         "update");
  df.Write(num_executed,           "number of insts executed this update");
  df.Write(num_creatures,          "number of organisms");
  df.Write(num_genotypes,          "number of different genotypes");
  df.Write(num_threshold,          "number of different threshold genotypes");
  df.Write(num_species,            "number of different species");
  df.Write(num_thresh_species,     "number of different threshold species");
  df.Write(num_lineages,           "number of different lineages");
  df.Write(num_births,             "number of births in this update");
  df.Write(num_deaths,             "number of deaths in this update");
  df.Write(num_breed_true,         "number of breed true");
  df.Write(num_breed_true_creatures, "number of breed true organisms?");
  df.Write(num_no_birth_creatures,   "number of no-birth organisms");
  df.Write(num_single_thread_creatures, "number of single-threaded organisms");
  df.Write(num_multi_thread_creatures, "number of multi-threaded organisms");
  df.Write(num_modified, "number of modified organisms");
  df.Endl();
}

void cStats::PrintMessageData(const cString& filename) {
	cDataFile& df = m_world->GetDataFile(filename);
	
  df.WriteComment( "Number of organsism to organisms messages\n" );
  
  df.Write( GetUpdate(), "update" );
  
  cPopulation& pop = m_world->GetPopulation();
  int numDemes = pop.GetNumDemes();
  
	unsigned int totalMessagesSent(0);
	unsigned int totalMessagesSuccessfullySent(0);
	unsigned int totalMessagesDropped(0);
	unsigned int totalMessagesFailed(0);
	
	for( int i=0; i < numDemes; i++ ){
		totalMessagesSent += pop.GetDeme(i).GetMessagesSent();
		totalMessagesSuccessfullySent += pop.GetDeme(i).GetMessageSuccessfullySent();
		totalMessagesDropped += pop.GetDeme(i).GetMessageDropped();
		totalMessagesFailed  += pop.GetDeme(i).GetMessageSendFailed();
	}
	
	df.Write(totalMessagesSent, "Totlal messages sent");
	df.Write(totalMessagesSuccessfullySent, "Sent successfully");
	df.Write(totalMessagesDropped, "Dropped");
	df.Write(totalMessagesFailed, "Failed");
	
  df.Endl();
}

void cStats::PrintTotalsData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  df.Write(m_update, "Update");
  df.Write((tot_executed+num_executed), "Total Instructions Executed");
  df.Write(num_executed, "Instructions Executed This Update");
  df.Write(tot_organisms, "Total Organisms");
  df.Write(tot_genotypes, "Total Genotypes");
  df.Write(tot_threshold, "Total Threshold");
  df.Write(tot_species, "Total Species");
  df.Write(tot_lineages, "Total Lineages");
  df.Endl();
}


void cStats::PrintTasksData(const cString& filename)
{
	cString file = filename;

	// flag to print both tasks.dat and taskquality.dat
	if (filename == "tasksq.dat")
	{
		file = "tasks.dat";
		PrintTasksQualData("taskquality.dat");
	}

	// print tasks.dat
	cDataFile& df = m_world->GetDataFile(file);
	df.WriteComment("Avida tasks data");
	df.WriteTimeStamp();
	df.WriteComment("First column gives the current update, next columns give the number");
	df.WriteComment("of organisms that have the particular task as a component of their merit");

	df.Write(m_update,   "Update");
	for(int i = 0; i < task_last_count.GetSize(); i++) {
		df.Write(task_last_count[i], task_names[i] );
	}
	df.Endl();
}


void cStats::PrintTasksExeData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida tasks execution data");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the number");
  df.WriteComment("of times the particular task has been executed this update.");

  df.Write(m_update,   "Update");
  for (int i = 0; i < task_exe_count.GetSize(); i++) {
    df.Write(task_exe_count[i], task_names[i] );
  }
  df.Endl();
}

void cStats::PrintTasksQualData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida tasks quality data");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, rest give average and max task quality");
  df.Write(m_update, "Update");
  for(int i = 0; i < task_last_count.GetSize(); i++) {
    double qual = 0.0;
    if (task_last_count[i] > 0) 
      qual = task_last_quality[i] / static_cast<double>(task_last_count[i]);
    df.Write(qual, task_names[i] + " Average");
    df.Write(task_last_max_quality[i], task_names[i] + " Max");
  }
  df.Endl();
}

void cStats::PrintDynamicMaxMinData(const cString& filename)
{
	cDataFile& df = m_world->GetDataFile(filename);

	df.WriteComment("Avida dynamic max min data");
	df.WriteTimeStamp();
	df.WriteComment("First column gives the current update, 2nd and 3rd give max and min Fx");
	df.Write(m_update, "Update");
	for(int i = 0; i < task_last_count.GetSize(); i++) {
		double max = m_world->GetEnvironment().GetTask(i).GetArguments().GetDouble(1);
		double min = m_world->GetEnvironment().GetTask(i).GetArguments().GetDouble(2);
		df.Write(max, task_names[i] + " Max");
		df.Write(min, task_names[i] + " Min");
	}
	df.Endl();
}

void cStats::PrintReactionData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida reaction data");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the number");
  df.WriteComment("of currently living organisms each reaction has affected.");

	df.Write(m_update,   "Update");
	for(int i = 0; i < m_reaction_last_count.GetSize(); i++) {
		df.Write(m_reaction_last_count[i], reaction_names[i]);
	}
	df.Endl();
}

void cStats::PrintCurrentReactionData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida reaction data");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the number");
  df.WriteComment("of currently living organisms each reaction has affected.");

	df.Write(m_update,   "Update");
	for(int i = 0; i < m_reaction_cur_count.GetSize(); i++) {
		df.Write(m_reaction_cur_count[i], reaction_names[i]);
	}
	df.Endl();
}


void cStats::PrintReactionRewardData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida reaction data");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the add bonus reward");
  df.WriteComment("currently living organisms have garnered from each reaction.");

  df.Write(m_update,   "Update");
  for (int i = 0; i < m_reaction_last_add_reward.GetSize(); i++) {
    df.Write(m_reaction_last_add_reward[i], reaction_names[i]);
  }
  df.Endl();
}


void cStats::PrintCurrentReactionRewardData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Avida reaction data");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the add bonus reward");
  df.WriteComment("currently living organisms have garnered from each reaction.");
  
  df.Write(m_update,   "Update");
  for (int i = 0; i < m_reaction_cur_add_reward.GetSize(); i++) {
    df.Write(m_reaction_cur_add_reward[i], reaction_names[i]);
  }
  df.Endl();
}


void cStats::PrintReactionExeData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Avida reaction execution data");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the number");
  df.WriteComment("of times the particular reaction has been triggered this update.");
  
  df.Write(m_update,   "Update");
  for (int i = 0; i < m_reaction_exe_count.GetSize(); i++) {
    df.Write(m_reaction_exe_count[i], reaction_names[i]);
  }
  df.Endl();
}


void cStats::PrintResourceData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida resource data");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the quantity");
  df.WriteComment("of the particular resource at that update.");

  df.Write(m_update,   "Update");
  
  // Check for spatial resources if they exist total up the resource in each 
  // cell and print that total.  Also call the routine to print the individual
  // maps for each spatial resource 
  
  for (int i = 0; i < resource_count.GetSize(); i++) {
   if (resource_geometry[i] != nGeometry::GLOBAL && resource_geometry[i] != nGeometry::PARTIAL) {
         double sum_spa_resource = 0;
      for (int j = 0; j < spatial_res_count[i].GetSize(); j++) {
         sum_spa_resource += spatial_res_count[i][j];
      }
      df.Write(sum_spa_resource, resource_names[i] );
      PrintSpatialResData(filename, i);
    } else {
      df.Write(resource_count[i], resource_names[i] );
    }
  }
  df.Endl();
}

void cStats::PrintSpatialResData(const cString& filename, int i)
{

  // Write spatial resource data to a file that can easily be read into Matlab

  cString tmpfilename = "resource_";
  tmpfilename +=  resource_names[i] + ".m";
  cDataFile& df = m_world->GetDataFile(tmpfilename);
  cString UpdateStr = resource_names[i] + 
                      cStringUtil::Stringf( "%07i", GetUpdate() ) + " = [ ...";

  df.WriteRaw(UpdateStr);

  int gridsize = spatial_res_count[i].GetSize();
  int xsize = m_world->GetConfig().WORLD_X.Get();

  // write grid to file

  for (int j = 0; j < gridsize; j++) {
    df.WriteBlockElement(spatial_res_count[i][j], j, xsize);
  }
  df.WriteRaw("];");
  df.Flush();
}

// @WRE: Added method for printing out visit data
void cStats::PrintCellVisitsData(const cString& filename)
{
  // Write cell visits data to a file that can easily be read into Matlab

  cString tmpfilename = "visits.m";
  cDataFile& df = m_world->GetDataFile(tmpfilename);
  cString UpdateStr = cStringUtil::Stringf( "visits%07i", GetUpdate() ) + " = [ ...";

  df.WriteRaw(UpdateStr);

  int xsize = m_world->GetConfig().WORLD_X.Get();
  
  for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
    df.WriteBlockElement(m_world->GetPopulation().GetCell(i).GetVisits(), i, xsize);
  }

  df.WriteRaw("];");
  df.Flush();
}


void cStats::PrintTimeData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida time data");
  df.WriteTimeStamp();

  df.Write(m_update,              "update");
  df.Write(avida_time,               "avida time");
  df.Write(sum_generation.Average(), "average generation");
  df.Write(num_executed,             "num_executed?");
  df.Endl();
}


//@MRR Add additional time information
void cStats::PrintExtendedTimeData(const cString& filename)
{
	cDataFile& df = m_world->GetDataFile(filename);
	df.WriteTimeStamp();
	df.Write(m_update, "update");
	df.Write(avida_time, "avida time");
	df.Write(num_executed, "num_executed");
	df.Write(tot_organisms, "num_organisms");
	df.Endl();
}

void cStats::PrintMutationRateData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida copy mutation rate data");
  df.WriteTimeStamp();

  df.Write(m_update, "Update");
  df.Write(sum_copy_mut_rate.Ave(), "Average copy mutation rate");
  df.Write(sum_copy_mut_rate.Var(), "Variance in copy mutation rate");
  df.Write(sum_copy_mut_rate.StdDeviation(), "Standard Deviation in copy mutation rate");
  df.Write(sum_copy_mut_rate.Skw(), "Skew in copy mutation rate");
  df.Write(sum_copy_mut_rate.Kur(), "Kurtosis in copy mutation rate");

  df.Write(sum_log_copy_mut_rate.Ave(), "Average log(copy mutation rate)");
  df.Write(sum_log_copy_mut_rate.Var(), "Variance in log(copy mutation rate)");
  df.Write(sum_log_copy_mut_rate.StdDeviation(), "Standard Deviation in log(copy mutation rate)");
  df.Write(sum_log_copy_mut_rate.Skw(), "Skew in log(copy mutation rate)");
  df.Write(sum_log_copy_mut_rate.Kur(), "Kurtosis in log(copy mutation rate)");
  df.Endl();

}


void cStats::PrintDivideMutData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida divide mutation rate data");
  df.WriteTimeStamp();

  df.Write(m_update, "Update");
  df.Write(sum_div_mut_rate.Ave(), "Average divide mutation rate");
  df.Write(sum_div_mut_rate.Var(), "Variance in divide mutation rate");
  df.Write(sum_div_mut_rate.StdDeviation(), "Standard Deviation in divide mutation rate");
  df.Write(sum_div_mut_rate.Skw(), "Skew in divide mutation rate");
  df.Write(sum_div_mut_rate.Kur(), "Kurtosis in divide mutation rate");

  df.Write(sum_log_div_mut_rate.Ave(), "Average log(divide mutation rate)");
  df.Write(sum_log_div_mut_rate.Var(), "Variance in log(divide mutation rate)");
  df.Write(sum_log_div_mut_rate.StdDeviation(), "Standard Deviation in log(divide mutation rate)");
  df.Write(sum_log_div_mut_rate.Skw(), "Skew in log(divide mutation rate)");
  df.Write(sum_log_div_mut_rate.Kur(), "Kurtosis in log(divide mutation rate)");
  df.Endl();
}

void cStats::PrintInstructionData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida instruction execution data");
  df.WriteTimeStamp();

  df.Write(m_update, "Update");

#if INSTRUCTION_COUNT
  for (int i = 0; i < sum_exe_inst_array.GetSize(); i++) {
    df.Write(sum_exe_inst_array[i].Sum(), inst_names[i]);
  }
#else // INSTRUCTION_COUNT undefined
  m_world->GetDriver().RaiseException("Warning: Instruction Counts not compiled in");
#endif // ifdef INSTRUCTION_COUNT

  df.Endl();
}

void cStats::PrintGenotypeMap(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  cString UpdateStr =
    cStringUtil::Stringf( "GenoID%07i", GetUpdate() ) + " = [ ...";
  df.WriteRaw(UpdateStr);

  int gridsize = genotype_map.GetSize();
  int xsize = m_world->GetConfig().WORLD_X.Get();

  // write grid to file                                        

  for (int i = 0; i < gridsize; i++) {
    df.WriteBlockElement(genotype_map[i],i,xsize);
  }
  df.WriteRaw("];");
  df.Flush();
}

void cStats::PrintMarketData(const cString& filename)
{
	cDataFile& df = m_world->GetDataFile(filename);

	df.WriteComment( "Avida market data\n" );
	df.WriteComment("cumulative totals since the last update data was printed" );
	df.WriteTimeStamp();
	df.Write( GetUpdate(), "update" );
	df.Write( num_bought, "num bought" );
	df.Write( num_sold, "num sold" );
	df.Write(num_used, "num used" );
	df.Write(num_own_used, "num own used" );
	num_bought = num_sold = num_used = num_own_used = 0;
  df.Endl();
}

void cStats::PrintSenseData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida sense instruction usage\n" );
  df.WriteComment("total number of organisms whose parents executed sense instructions with given labels" );

  df.Write( GetUpdate(), "update" );

  for( int i=0; i < sense_last_count.GetSize(); i++ ){
    df.Write(sense_last_count[i], sense_names[i]);
  }
  df.Endl();
}

void cStats::PrintSenseExeData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida sense instruction usage\n" );
  df.WriteComment("total number of sense instructions executed by the parents of current organisms with given labels" );
  
  df.Write( GetUpdate(), "update" );
    
  for( int i=0; i < sense_last_exe_count.GetSize(); i++ ){
    df.Write(sense_last_exe_count[i], sense_names[i]);
  }
  df.Endl();
}

void cStats::PrintInternalTasksData(const cString& filename)
{
	cString file = filename;

	// flag to print both in_tasks.dat and in_taskquality.dat
	if (filename == "in_tasksq.dat")
	{
		file = "in_tasks.dat";
		PrintInternalTasksQualData("in_taskquality.dat");
	}

	// print in_tasks.dat
	cDataFile& df = m_world->GetDataFile(file);
	df.WriteComment("Avida tasks data: tasks performed with internal resources");
	df.WriteTimeStamp();
	df.WriteComment("First column gives the current update, next columns give the number");
	df.WriteComment("of organisms that have the particular task, performed with internal resources, ");
	df.WriteComment("as a component of their merit");

	df.Write(m_update,   "Update");
	for(int i = 0; i < task_internal_last_count.GetSize(); i++) {
		df.Write(task_internal_last_count[i], task_names[i] );
	}
	df.Endl();
}

void cStats::PrintInternalTasksQualData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida tasks quality data: tasks performed using internal resources");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, rest give average and max task quality ");
  df.WriteComment("for those tasks performed using internal resources");
  df.Write(m_update, "Update");
  for(int i = 0; i < task_internal_last_count.GetSize(); i++) {
    double qual = 0.0;
    if (task_internal_last_count[i] > 0) 
      qual = task_internal_last_quality[i] / static_cast<double>(task_internal_last_count[i]);
    df.Write(qual, task_names[i] + " Average");
    df.Write(task_internal_last_max_quality[i], task_names[i] + " Max");
  }
  df.Endl();
}

void cStats::PrintSleepData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment( "Number of organisms sleeping\n" );
  df.WriteComment("total number of organisms sleeping" );
  
  df.Write( GetUpdate(), "update" );
  
  cPopulation& pop = m_world->GetPopulation();
  int numDemes = pop.GetNumDemes();
  
  for( int i=0; i < numDemes; i++ ){
    df.Write(pop.GetDeme(i).GetSleepingCount(), cStringUtil::Stringf("DemeID %d", i));
  }
  df.Endl();
}

void cStats::PrintCompetitionData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment( "Competition results\n" );
  df.WriteComment( "results of the current competitions" );
  
  df.Write( GetUpdate(), "update" );
  df.Write( avg_competition_fitness, "average competition fitness" );
  df.Write( min_competition_fitness, "min competition fitness" );
  df.Write( max_competition_fitness, "max competition fitness" );
  df.Write( avg_competition_copied_fitness, "average copied fitness" );
  df.Write( min_competition_copied_fitness, "min copied fitness" );
  df.Write( max_competition_copied_fitness, "max copied fitness" );
  df.Write( num_orgs_replicated, "number of organisms copied" );

  // Only print trial info if there were multiple trials.
  if (avg_trial_fitnesses.GetSize() > 1)
  {
    for( int i=0; i < avg_trial_fitnesses.GetSize(); i++ ){
      df.Write(avg_trial_fitnesses[i], cStringUtil::Stringf("trial.%d fitness", i));
    }
  }
  df.Endl();
}


/*! This method is called whenever an organism successfully sends a message.  Success,
in this case, means that the message has been delivered to the receive buffer of
the organism that this message was sent to. */
void cStats::SentMessage(const cOrgMessage& msg)
{
  // Check to see if this message matches any of our predicates.
  for(message_pred_ptr_list::iterator i=m_message_predicates.begin(); i!=m_message_predicates.end(); ++i) {
    (**i)(msg); // Predicate is responsible for tracking info about messages.
  }  
}


/*! This method adds a message predicate to the list of all predicates.  Each predicate
in the list is evaluated for every sent message.

NOTE: cStats does NOT own the predicate pointer!  (It DOES NOT delete them!)
*/
void cStats::AddMessagePredicate(cOrgMessagePredicate* predicate)
{
  m_message_predicates.push_back(predicate);
}

void cStats::RemoveMessagePredicate(cOrgMessagePredicate* predicate)
{
  for(message_pred_ptr_list::iterator iter = m_message_predicates.begin(); iter != m_message_predicates.end(); iter++) {
    if((*iter) == predicate) {
      m_message_predicates.erase(iter);
      return;
    }
  }
}


/*! This method adds a movement predicate to the list of all movement predicates.  Each predicate
 * in the list is evaluated for every organism movement.
 *
 * NOTE: cStats does NOT own the predicate pointer!  (It DOES NOT delete them!)
 * */
void cStats::AddMovementPredicate(cOrgMovementPredicate* predicate)
{
  m_movement_predicates.push_back(predicate);
}

/*! This method is called whenever an organism moves.*/
void cStats::Move(cOrganism& org) {
  // Check to see if this message matches any of our predicates.
  for(movement_pred_ptr_list::iterator i=m_movement_predicates.begin();
      i!=m_movement_predicates.end(); ++i) {
    (**i)(org); // Predicate is responsible for tracking info about movement.
  }
}

// deme predicate stats
void cStats::IncEventCount(int x, int y) {
  relative_pos_event_count.ElementAt(x,y)++;
}

void cStats::IncPredSat(int cell_id) {
  cPopulation& pop = m_world->GetPopulation();
  int deme_id = pop.GetCell(cell_id).GetDemeID();
  std::pair<int, int> pos = pop.GetDeme(deme_id).GetCellPosition(cell_id);
  relative_pos_pred_sat.ElementAt(pos.first, pos.second)++;
}



/*! This method prints information contained within all active message predicates.

 Each row of the data file has the following format: 
   update predicate_name predicate_data...
 */
void cStats::PrintPredicatedMessages(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  df.WriteColumnDesc("update [update]");
	df.WriteColumnDesc("predicate name: [pname]");
  df.WriteColumnDesc("predicate data: [pdata]");
  df.FlushComments();
  
  std::ofstream& out = df.GetOFStream();
  for(message_pred_ptr_list::iterator i=m_message_predicates.begin();
      i!=m_message_predicates.end(); ++i) {
    (*i)->Print(GetUpdate(), out);
    (*i)->Reset();
  }
//  df.Endl();  
}

void cStats::PrintPredSatFracDump(const cString& filename) {
  cDataFile& df = m_world->GetDataFile(filename);
  df.WriteComment( "Displays the fraction of events detected in cell since last print.\n" );
  df.FlushComments();
  cString UpdateStr = cStringUtil::Stringf( "%07i", GetUpdate() ) + " = [ ...";
  df.WriteRaw(UpdateStr);

  int rows = relative_pos_pred_sat.GetNumRows();
  int cols = relative_pos_pred_sat.GetNumCols();
  for (int x = 0; x < rows; x++) {
    for (int y = 0; y < cols; y++) {
      double data;
      if(relative_pos_event_count.ElementAt(x,y) == 0) {
        data = 0.0;
      } else {
        data = (double) relative_pos_pred_sat.ElementAt(x,y) / (double) relative_pos_event_count.ElementAt(x,y);
      }
      df.WriteBlockElement(data, x*cols+y, cols);
    }
  }
  df.WriteRaw("];");
  df.Endl();
  
  relative_pos_pred_sat.SetAll(0);
  relative_pos_event_count.SetAll(0);
}

void cStats::DemePreReplication(cDeme& source_deme, cDeme& target_deme)
{
  ++m_deme_num_repls;
  m_deme_gestation_time.Add(source_deme.GetAge());
  m_deme_births.Add(source_deme.GetBirthCount());
  m_deme_merit.Add(source_deme.GetHeritableDemeMerit().GetDouble());
  m_deme_generation.Add(source_deme.GetGeneration());
}


/*! This method is a generic hook for post-deme-replication stat tracking.  We 
currently only track the genotype ids of all the founders of each deme in the population.
Note that we capture genotype ids at the time of deme replication, so we unfortunately
lose the ancestral deme founders.
*/
void cStats::DemePostReplication(cDeme& source_deme, cDeme& target_deme)
{
  std::vector<int> genotype_ids;
  for(int i=0; i<target_deme.GetSize(); ++i) {
    cPopulationCell& cell = target_deme.GetCell(i);
    if(cell.IsOccupied()) {
      genotype_ids.push_back(cell.GetOrganism()->GetGenotype()->GetID());  
    }
  }
  //assert(genotype_ids.size()>0); // How did we get to replication otherwise?
  //@JEB some germline methods can result in empty source demes if they didn't produce a germ)
  m_deme_founders[target_deme.GetID()] = genotype_ids;
}


/*! Called immediately prior to germline replacement.
*/
void cStats::GermlineReplication(cGermline& source_germline, cGermline& target_germline)
{
  m_germline_generation.Add(source_germline.Size());
}


/*! Print statistics related to deme replication.  Currently only prints the
number of deme replications since the last time PrintDemeReplicationData was
invoked.
*/
void cStats::PrintDemeReplicationData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Avida deme replication data");
  df.WriteTimeStamp();
  df.Write(GetUpdate(), "Update [update]");
  df.Write(m_deme_num_repls, "Number of deme replications [numrepl]");
  df.Write(m_deme_gestation_time.Average(), "Mean deme gestation time [gesttime]");
  df.Write(m_deme_births.Average(), "Mean number of births within replicated demes [numbirths]");
  df.Write(m_deme_merit.Average(), "Mean heritable merit of replicated demes [merit]");
  df.Write(m_deme_generation.Average(), "Mean generation of replicated demes [generation]");
  
  df.Endl();
  
  m_deme_num_repls = 0;
  m_deme_gestation_time.Clear();
  m_deme_births.Clear();
  m_deme_merit.Clear();
  m_deme_generation.Clear();
}


void cStats::PrintGermlineData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Avida germline data");
  df.WriteTimeStamp();
  df.Write(GetUpdate(), "Update [update]");
  df.Write(m_germline_generation.Average(), "Mean germline generation of replicated germlines [replgen]");
  df.Endl();
    
  m_germline_generation.Clear();
}


/*! Print the genotype IDs of the founders of recently born demes.

Prints only the most recent set of founding genotype ids for each deme.  If a deme was replaced multiple
times since the last time this method ran, only the most recent is printed.  Only deme "births" (i.e., due
to deme replication) are tracked; the ancestral deme founders are lost.  The update column is the update 
at which this method executes, not the time at which the given deme was born.
*/
void cStats::PrintDemeFoundersData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Avida deme founder data.");
  df.WriteTimeStamp();
  df.WriteColumnDesc("Update [update]");
  df.WriteColumnDesc("Deme ID [demeid]");
  df.WriteColumnDesc("Number of founders [size]");
  df.WriteColumnDesc("{Genotype ID of founder 0, ...}");
  df.FlushComments();
  
  std::ofstream& out = df.GetOFStream();
  for(t_founder_map::iterator i=m_deme_founders.begin(); i!=m_deme_founders.end(); ++i) {
    out << GetUpdate() << " " << i->first << " " << i->second.size();    
    for(std::vector<int>::iterator j=i->second.begin(); j!=i->second.end(); ++j) {
      out << " " << *j;
    }
    df.Endl();
  }
  m_deme_founders.clear();
}

void cStats::PrintPerDemeTasksData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme tasks data");
	df.WriteTimeStamp();
	df.WriteComment("First column gives the current update, next columns give the number");
	df.WriteComment("of organisms that have the particular task as a component of their merit");
	df.WriteComment("in a particular deme");

  const int num_tasks = m_world->GetEnvironment().GetNumTasks();

	df.Write(m_update,   "Update");
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    for(int j = 0; j < num_tasks; j++) {
      df.Write( (deme.GetLastTaskExeCount()[j] > 0), cStringUtil::Stringf("%i.", i) + task_names[j] );
    }
	}
  df.Endl();
}

void cStats::PrintPerDemeTasksExeData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme tasks exe data");
	df.WriteTimeStamp();
	df.WriteComment("First column gives the current update, next columns give the number");
	df.WriteComment("of times a task has contributed to the merit of all organisms");
	df.WriteComment("in a particular deme");

  const int num_tasks = m_world->GetEnvironment().GetNumTasks();

	df.Write(m_update,   "Update");
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    for(int j = 0; j < num_tasks; j++) {
      df.Write( deme.GetLastTaskExeCount()[j], cStringUtil::Stringf("%i.", i) + task_names[j] );
    }
	}
  df.Endl();
}

void cStats::PrintPerDemeReactionData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme reactions data");
	df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the number");
  df.WriteComment("of currently living organisms each reaction has affected.");

  const int num_reactions = m_world->GetEnvironment().GetReactionLib().GetSize();

	df.Write(m_update,   "Update");
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    for(int j = 0; j < num_reactions; j++) {
      df.Write( deme.GetLastReactionCount()[j], cStringUtil::Stringf("%i.", i) + m_world->GetEnvironment().GetReactionLib().GetReaction(j)->GetName()  );
    }
  }
  df.Endl();
}

void cStats::PrintDemeTasksData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme tasks data");
	df.WriteTimeStamp();
	df.WriteComment("First column gives the current update, next columns give the number");
	df.WriteComment("of organisms per deme that had the given task as a component of their merit");
	df.WriteComment("during the lifetime of the deme");

  const int num_tasks = m_world->GetEnvironment().GetNumTasks();

  tArray<int> deme_tasks;
  deme_tasks.ResizeClear(num_tasks);
  deme_tasks.SetAll(num_tasks);
  int occupied_demes = 0;
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    if (!deme.IsEmpty()) {
      occupied_demes++;
      for(int j = 0; j < num_tasks; j++) { 
        deme_tasks[j] += static_cast<int>(deme.GetLastTaskExeCount()[j] > 0);
      }
    }
  }
  
	df.Write(m_update,   "Update");
  for(int j = 0; j < num_tasks; j++) {    
    df.Write( static_cast<double>(deme_tasks[j]) / static_cast<double>(occupied_demes), task_names[j] );
	}
  df.Endl();
}

void cStats::PrintDemeTasksExeData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme tasks exe data");
	df.WriteTimeStamp();
	df.WriteComment("First column gives the current update, next columns give the number");
	df.WriteComment("of times per deme that a given task counted as a component of an");
	df.WriteComment("organisms's merit during the lifetime of the deme");

  const int num_tasks = m_world->GetEnvironment().GetNumTasks();

  tArray<int> deme_tasks;
  deme_tasks.ResizeClear(num_tasks);
  deme_tasks.SetAll(num_tasks);
  int occupied_demes = 0;
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    if (!deme.IsEmpty()) {
      occupied_demes++;
      for(int j = 0; j < num_tasks; j++) { 
        deme_tasks[j] += deme.GetLastTaskExeCount()[j];
      }
    }
  }
  
	df.Write(m_update,   "Update");
  for(int j = 0; j < num_tasks; j++) {    
    df.Write( static_cast<double>(deme_tasks[j]) / static_cast<double>(occupied_demes), task_names[j] );
	}
  df.Endl();
}

void cStats::PrintDemeReactionData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme reactions data");
	df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the number");
  df.WriteComment("of times each reaction has affected a deme.");

  const int num_reactions = m_world->GetEnvironment().GetReactionLib().GetSize();

  tArray<int> deme_reactions;
  deme_reactions.ResizeClear(num_reactions);
  deme_reactions.SetAll(0);
  int occupied_demes = 0;
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    if (!deme.IsEmpty()) {
      occupied_demes++;
      for(int j = 0; j < num_reactions; j++) { 
        deme_reactions[j] += deme.GetLastReactionCount()[j];
      }
    }
  }
  
	df.Write(m_update,   "Update");
  for(int j = 0; j < num_reactions; j++) {    
    df.Write( static_cast<double>(deme_reactions[j]) / static_cast<double>(occupied_demes), m_world->GetEnvironment().GetReactionLib().GetReaction(j)->GetName() );
	}
  df.Endl();
}

void cStats::PrintDemeOrgTasksData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme org tasks data");
	df.WriteTimeStamp();
	df.WriteComment("First column gives the current update, next columns give the number");
	df.WriteComment("of organisms that have the particular task as a component of their merit");
	df.WriteComment("in a particular deme when the deme last divided.");

  const int num_tasks = m_world->GetEnvironment().GetNumTasks();

	df.Write(m_update,   "Update");
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    for(int j = 0; j < num_tasks; j++) {
      df.Write( deme.GetLastOrgTaskCount()[j], cStringUtil::Stringf("%i.", i) + task_names[j] );
    }
  }
  df.Endl();
}

void cStats::PrintDemeOrgTasksExeData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme org tasks exe data");
	df.WriteTimeStamp();
	df.WriteComment("First column gives the current update, next columns give the number");
	df.WriteComment("of times a task has contributed to the merit of all organisms");
	df.WriteComment("in a particular deme when the deme last divided.");

  const int num_tasks = m_world->GetEnvironment().GetNumTasks();

	df.Write(m_update,   "Update");
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    for(int j = 0; j < num_tasks; j++) {
      df.Write( deme.GetLastOrgTaskExeCount()[j], cStringUtil::Stringf("%i.", i) + task_names[j] );
    }
  }

  df.Endl();
}

void cStats::PrintDemeCurrentTaskExeData(const cString& filename) {
	cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme current task exe data");
	df.WriteTimeStamp();
	df.WriteComment("First column gives update number, next columns give the number");
	df.WriteComment("of times a given task has been executed in a given deme by");
	df.WriteComment("some organism in that deme.");

	const int num_tasks = m_world->GetEnvironment().GetNumTasks();
	df.Write(m_update, "Update");
	for (int deme_num=0; deme_num < m_world->GetPopulation().GetNumDemes(); ++deme_num) {
		cDeme& deme = m_world->GetPopulation().GetDeme(deme_num);
		for (int task_num=0; task_num < num_tasks; task_num++) {
			df.Write(	deme.GetCurTaskExeCount()[task_num], 
						cStringUtil::Stringf("%i.", deme_num)+task_names[task_num]);
		}
	}

	df.Endl();
}

void cStats::PrintCurrentTaskCounts(const cString& filename) {
  ofstream& fp = m_world->GetDataFileOFStream(filename);
  fp << "Update " << m_world->GetStats().GetUpdate() << ":" << endl;
  for (int y = 0; y < m_world->GetPopulation().GetWorldY(); y++) {
    for (int x = 0; x < m_world->GetPopulation().GetWorldX(); x++) {
      cPopulationCell& cell = m_world->GetPopulation().GetCell(y * m_world->GetPopulation().GetWorldX() + x);
      if (cell.IsOccupied()) {
        fp << cell.GetOrganism()->GetPhenotype().GetCurTaskCount()[0] << "\t";
      } else {
        fp << "---\t";
      }
    }
    fp << endl;
  }
  fp << endl;
}

void cStats::PrintDemeOrgReactionData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida deme org reactions data");
	df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the number");
  df.WriteComment("of currently living organisms each reaction has affected");
	df.WriteComment("in a particular deme when the deme last divided.");

  const int num_reactions = m_world->GetEnvironment().GetReactionLib().GetSize();

	df.Write(m_update,   "Update");
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    for(int j = 0; j < num_reactions; j++) {
      df.Write( deme.GetLastOrgReactionCount()[j], cStringUtil::Stringf("%i.", i) + m_world->GetEnvironment().GetReactionLib().GetReaction(j)->GetName()  );
    }
  }
  df.Endl();
}


void cStats::PrintPerDemeGenPerFounderData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Avida org generations between deme founders");
	df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the number");
  df.WriteComment("number of generations that passed between the parent and current deme's founders");

	df.Write(m_update,   "Update");
  for(int i=0; i<m_world->GetPopulation().GetNumDemes(); ++i) {
    cDeme& deme = m_world->GetPopulation().GetDeme(i);
    double val = deme.GetGenerationsPerLifetime();
    if ( deme.IsEmpty() ) val = -1;
    df.Write( val, cStringUtil::Stringf("deme.%i", i)  );
  }
  df.Endl();
}


void cStats::CompeteDemes(const std::vector<double>& fitness) {
  m_deme_fitness = fitness;
}


void cStats::PrintDemeCompetitionData(const cString& filename) {
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Avida compete demes data");
  df.WriteTimeStamp();
  df.Write(m_update, "Update [update]");
  
  double avg = std::accumulate(m_deme_fitness.begin(), m_deme_fitness.end(), 0.0);
  if(avg > 0.0) {
    avg /= m_deme_fitness.size();
  }
  df.Write(avg, "Avg. deme fitness [avgfit]");
  df.Endl();
  
  m_deme_fitness.clear();
}


/*! Prints the cell data from every cell, including the deme for that cell. */
void cStats::PrintCellData(const cString& filename) {
	cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Cell data per udpate.");
	df.WriteTimeStamp();
	
	for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
		const cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
		df.Write(GetUpdate(), "Update [update]");
		df.Write(cell.GetID(), "Global cell ID [globalid]");
		df.Write(cell.GetDemeID(), "Deme ID for cell [demeid]");
		df.Write(cell.GetCellData(), "Cell data [data]");
		df.Endl();
	}
}


void cStats::PrintCurrentOpinions(const cString& filename) {
	cDataFile& df = m_world->GetDataFile(filename);
	df.WriteComment("Current opinions of each organism.");
	df.WriteTimeStamp();
	df.WriteComment("1: Update [update]");
	df.WriteComment("2: Global cell ID [globalid]");
	df.WriteComment("3: Current opinion [opinion]");
	df.FlushComments();

	for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
		const cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
		df.WriteAnonymous(GetUpdate());
		df.WriteAnonymous(cell.GetID());
		if(cell.IsOccupied() && cell.GetOrganism()->HasOpinion()) {
			df.WriteAnonymous(cell.GetOrganism()->GetOpinion().first);
		} else {
			df.WriteAnonymous(0);
		}
		df.Endl();
	}	
}

/*! Called when an organism issues a flash instruction.
 
 We do some pretty detailed tracking here in order to support the use of flash
 messages in deme competition.  All flashes are tracked per deme.
 */
void cStats::SentFlash(cOrganism& organism) {
  ++m_flash_count;	
	if(organism.GetOrgInterface().GetDeme() != 0) {
		const cDeme* deme = organism.GetOrgInterface().GetDeme();
		m_flash_times[GetUpdate()][deme->GetID()].push_back(deme->GetRelativeCellID(organism.GetCellID()));
	}
}


/*! Print statistics about synchronization flashes. */
void cStats::PrintSynchronizationData(const cString& filename) {
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Avida synchronization data");
  df.WriteTimeStamp();
  df.Write(m_update, "Update [update]");
  df.Write(m_flash_count, "Flash count [fcount]");
  df.Endl();
  
  m_flash_count = 0;
	m_flash_times.clear();
}


/*! Print detailed synchronization data. */
void cStats::PrintDetailedSynchronizationData(const cString& filename) {
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Detailed Avida synchronization data");
  df.WriteComment("Rows are (update, demeid, cellid) tuples, representing the update at which that cell flashed.");
  df.WriteTimeStamp();
	
	for(PopulationFlashes::iterator i=m_flash_times.begin(); i!=m_flash_times.end(); ++i) {
		for(DemeFlashes::iterator j=i->second.begin(); j!=i->second.end(); ++j) {
			for(CellFlashes::iterator k=j->second.begin(); k!=j->second.end(); ++k) {
				df.Write(i->first, "Update [update]");
				df.Write(j->first, "Deme ID [demeid]");
				df.Write(*k, "Deme-relative cell ID that issued a flash at this update [relcellid]");
				df.Endl();
			}
		}
	}
	
	m_flash_times.clear();
}
