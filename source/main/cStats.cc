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
#include "cReaction.h"

#include "functions.h"

#include <cfloat>
#include <cmath>


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

#if INSTRUCTION_COUNT
  sum_exe_inst_array.Resize(m_world->GetNumInstructions());
  ZeroInst();
#endif
  inst_names.Resize(m_world->GetNumInstructions());

  reaction_count.Resize( m_world->GetNumReactions() );
  reaction_count.SetAll(0);
  
  reaction_add_reward.Resize( m_world->GetNumReactions() );
  reaction_add_reward.SetAll(0);

  resource_count.Resize( m_world->GetNumResources() );
  resource_count.SetAll(0);

  task_names.Resize(num_tasks);
  for (int i = 0; i < num_tasks; i++)
    task_names[i] = env.GetTask(i).GetDesc();
  
  reaction_names.Resize( m_world->GetNumReactions() );
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

  numAsleep.Resize(m_world->GetConfig().NUM_DEMES.Get());
  numAsleep.SetAll(0);

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
  for (int i = 0; i < task_cur_count.GetSize(); i++) {
    task_cur_count[i] = 0;
    task_last_count[i] = 0;
    task_cur_quality[i] = 0;
    task_last_quality[i] = 0;
    task_last_max_quality[i] = 0;
    task_cur_max_quality[i] = 0;
  }
}

void cStats::ZeroRewards()
{
  reaction_add_reward.SetAll(0);
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

  sense_last_count.SetAll(0);
  sense_last_exe_count.SetAll(0);

  reaction_add_reward.SetAll(0);

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

  df.Write(m_update,                        "Update");
  df.Write(sum_deme_age.Average(),          "Age");
  df.Write(sum_deme_birth_count.Average(),  "Births");
  df.Write(sum_deme_org_count.Average(),    "Organisms");
  df.Endl();
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
    task_exe_count[i] = 0;
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

void cStats::PrintReactionData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment("Avida reaction data");
  df.WriteTimeStamp();
  df.WriteComment("First column gives the current update, all further columns give the number");
  df.WriteComment("of currently living organisms each reaction has affected.");

  df.Write(m_update,   "Update");
  
  const int num_reactions=m_world->GetEnvironment().GetReactionLib().GetSize();
  tArray<int> reactions(num_reactions);
  reactions.SetAll(0);
  
  for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
    cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
    if(cell.IsOccupied()) {
      const tArray<int>& org_rx = cell.GetOrganism()->GetPhenotype().GetLastReactionCount();
      for(int j=0; j<num_reactions; ++j) {
        reactions[j] += org_rx[j];
      }
    }
  }
    
  for(int i=0; i<num_reactions; ++i) {
    df.Write(reactions[i], m_world->GetEnvironment().GetReactionLib().GetReaction(i)->GetName());
  }
  
//    df.Write( 0.0, 
//    df.Write(reaction_count[i], reaction_names[i] );
//    task_exe_count[i] = 0;
//  }
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
  
  const int num_reactions=m_world->GetEnvironment().GetReactionLib().GetSize();
  tArray<int> reactions(num_reactions);
  reactions.SetAll(0);
  
  for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
    cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
    if(cell.IsOccupied()) {
      const tArray<int>& org_rx = cell.GetOrganism()->GetPhenotype().GetCurReactionCount();
      for(int j=0; j<num_reactions; ++j) {
        reactions[j] += org_rx[j];
      }
    }
  }
    
  for(int i=0; i<num_reactions; ++i) {
    df.Write(reactions[i], m_world->GetEnvironment().GetReactionLib().GetReaction(i)->GetName());
  }
  
//    df.Write( 0.0, 
//    df.Write(reaction_count[i], reaction_names[i] );
//    task_exe_count[i] = 0;
//  }
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
  for (int i = 0; i < reaction_count.GetSize(); i++) {
    df.Write(reaction_add_reward[i], reaction_names[i] );
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
    if (resource_geometry[i] != nGeometry::GLOBAL) {
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

void cStats::PrintSleepData(const cString& filename){
  cDataFile& df = m_world->GetDataFile(filename);

  df.WriteComment( "Number of organisms sleeping\n" );
  df.WriteComment("total number of organisms sleeping" );
  
  df.Write( GetUpdate(), "update" );
    
  for( int i=0; i < numAsleep.GetSize(); i++ ){
    df.Write(numAsleep[i], cStringUtil::Stringf("DemeID %d", i));
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
  for(message_pred_ptr_list::iterator i=m_message_predicates.begin(); 
      i!=m_message_predicates.end(); ++i) {
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


/*! This method prints information contained within all active message predicates.

about  specific messages that are being tracked.
The messages that are being tracked are setup by the AddTrackedMessage method below.

The format of this log file is:
<update> \
<predicate>:{<cell_id>,...}...
*/
void cStats::PrintPredicatedMessages(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  df.WriteColumnDesc("update [update]");
  df.WriteColumnDesc("predicate:{p_info,...}...");
  df.FlushComments();
  
  df.WriteAnonymous(m_update);
  std::ofstream& out = df.GetOFStream();
  for(message_pred_ptr_list::iterator i=m_message_predicates.begin();
      i!=m_message_predicates.end(); ++i) {
    (*i)->Print(out);
    (*i)->Reset();
    out << " ";
  }
  df.Endl();  
}


void cStats::DemePreReplication(cDeme& source_deme, cDeme& target_deme)
{
  ++m_deme_num_repls;
  m_deme_gestation_time.Add(source_deme.GetAge());
  m_deme_births.Add(source_deme.GetBirthCount());                      
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
  df.Write(m_update, "Update [update]");
  df.Write(m_deme_num_repls, "Number of deme replications [numrepl]");
  df.Write(m_deme_gestation_time.Average(), "Mean deme gestation time [gesttime]");
  df.Write(m_deme_births.Average(), "Mean number of births [numbirths]");
  
  df.Endl();
  
  m_deme_num_repls = 0;
  m_deme_gestation_time.Clear();
  m_deme_births.Clear();
}


void cStats::PrintGermlineData(const cString& filename)
{
  cDataFile& df = m_world->GetDataFile(filename);
  
  df.WriteComment("Avida germline data");
  df.WriteTimeStamp();
  df.Write(m_update, "Update [update]");
  df.Write(m_germline_generation.Average(), "Mean germline generation of replicated germlines [replgen]");
  df.Endl();
    
  m_germline_generation.Clear();
}
