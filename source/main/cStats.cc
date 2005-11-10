//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cStats.h"

#include "cDataFile.h"
#include "functions.h"
#include "cStringUtil.h"
#include "tDataEntry.h"
#include "cWorld.h"

#include <math.h>

using namespace std;

////////////
//  cStats
////////////

cStats::cStats(cWorld* world)
  : m_world(world)
  , current_update(-1)
  , sub_update(0)
  , avida_time(0)
  , data_manager(this, "population_data")
  , rave_true_replication_rate( 500 )
  , entropy(0)
  , species_entropy(0)
  , energy(0)
  , dom_fidelity(0)
  , ave_fidelity(0)
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
  , min_fitness(HUGE_VAL)
  , min_merit(HUGE_VAL)
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
  , num_genotypes_last(1)
  , tot_organisms(0)
  , tot_genotypes(0)
  , tot_threshold(0)
  , tot_species(0)
  , tot_thresh_species(0)
  , tot_lineages(0)
  , tot_executed(0)
{
  // Open log files.

  if (m_world->GetConfig().LOG_CREATURES.Get()) fp_creature_log.open("creature.log");
  else fp_creature_log.close();

  if (m_world->GetConfig().LOG_GENOTYPES.Get()) fp_genotype_log.open("genotype.log");
  else fp_genotype_log.close();

  if (m_world->GetConfig().LOG_THRESHOLD.Get()) fp_threshold_log.open("threshold.log");
  else fp_threshold_log.close();

  if (m_world->GetConfig().LOG_SPECIES.Get())   fp_species_log.open("species.log");
  else fp_species_log.close();

  if (m_world->GetConfig().LOG_LINEAGES.Get())  fp_lineage_log.open("lineage.log");
  else fp_lineage_log.close();

  task_cur_count.Resize( m_world->GetNumTasks() );
  task_last_count.Resize( m_world->GetNumTasks() );
  task_exe_count.Resize( m_world->GetNumTasks() );
  task_cur_count.SetAll(0);
  task_last_count.SetAll(0);
  task_exe_count.SetAll(0);

#ifdef INSTRUCTION_COUNT
  sum_exe_inst_array.Resize( m_world->GetNumInstructions() );
  ZeroInst();
#endif
  inst_names.Resize( m_world->GetNumInstructions() );



  reaction_count.Resize( m_world->GetNumReactions() );
  reaction_count.SetAll(0);

  resource_count.Resize( m_world->GetNumResources() );
  resource_count.SetAll(0);

  task_names.Resize( m_world->GetNumTasks() );
  reaction_names.Resize( m_world->GetNumReactions() );
  resource_names.Resize( m_world->GetNumResources() );

  genotype_map.Resize( m_world->GetConfig().WORLD_X.Get() * m_world->GetConfig().WORLD_Y.Get() );
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

cStats::~cStats()
{
  cout << "Closing stats object..."<<endl;

  // Close all the statistics files.
  fp_creature_log.close();
  fp_genotype_log.close();
  fp_threshold_log.close();
  fp_species_log.close();
  fp_lineage_log.close();
}

void cStats::ZeroTasks()
{
  for( int i=0; i < task_cur_count.GetSize(); i++ ){
    task_cur_count[i] = 0;
    task_last_count[i] = 0;
  }
}


#ifdef INSTRUCTION_COUNT
void cStats::ZeroInst()
{
  for( int i=0; i < sum_exe_inst_array.GetSize(); i++ ){
    sum_exe_inst_array[i].Clear();
  }
}
#endif

void cStats::CalcEnergy()
{
  assert(sum_fitness.Average() >= 0.0);
  assert(dom_fitness >= 0);

  if (sum_fitness.Average() == 0.0 || dom_fitness == 0.0) {
    energy = 0.0;
  } else  {
    energy = Log(dom_fitness / sum_fitness.Average());
  }
}

void cStats::CalcFidelity()
{
  // There is a (small) probability that when a random instruction is picked
  // after a mutation occurs, that it will be the original instruction again;
  // This needs to be adjusted for!

  double adj = ((double) (m_world->GetNumInstructions() - 1)) /
    (double) m_world->GetNumInstructions();

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
  if (fp_creature_log.good() == true) {
    fp_creature_log << GetUpdate()   << " "
		    << cell_id       << " "
		    << genotype_id   << endl;
  }

  if (breed_true == true) num_breed_true++;
  else num_breed_in++;
}


void cStats::RecordDeath(int cell_id, int genotype_id, int age)
{
  num_deaths++;
}

void cStats::AddGenotype(int id_num)
{
  id_num = -1;  // @CAO do we still need id_num here?
  tot_genotypes++;
}

void cStats::RemoveGenotype(int id_num, int parent_id,
   int parent_dist, int depth, int max_abundance, int parasite_abundance,
   int age, int length)
{
  if (fp_genotype_log.good() &&
      (!m_world->GetConfig().LOG_THRESHOLD.Get() || max_abundance > 2)) {
    const int update_born = cStats::GetUpdate() - age + 1;
    fp_genotype_log << id_num             << " "    //  1
		    << update_born        << " "    //  2
		    << parent_id          << " "    //  3
		    << parent_dist        << " "    //  4
		    << depth              << " "    //  5
		    << max_abundance      << " "    //  6
		    << age                << " "    //  7
		    << length             << endl;  //  8
  }

  (void) parasite_abundance; // Not used now, but maybe in future.
}

void cStats::AddThreshold(int id_num, const char * name, int species_num)
{
  num_threshold++;
  tot_threshold++;
  if( fp_threshold_log.good() )
    fp_threshold_log << cStats::GetUpdate() << " "   // 1
		     << id_num              << " "   // 2
		     << species_num         << " "   // 3
		     << name                << endl; // 4
}

void cStats::RemoveThreshold(int id_num)
{
  id_num = -1;  // @CAO do we still need id_num here?
  num_threshold--;
}

void cStats::AddSpecies(int id_num)
{
  id_num = -1; // @CAO do we still need id_num here?
  tot_species++;
  num_species++;
}

void cStats::RemoveSpecies(int id_num, int parent_id,
			   int max_gen_abundance, int max_abundance, int age)
{
  num_species--;
  if( fp_species_log.good() )
    fp_species_log << cStats::GetUpdate() << " "   // 1
		   << id_num              << " "   // 2
		   << parent_id           << " "   // 3
		   << max_gen_abundance   << " "   // 4
		   << max_abundance       << " "   // 5
		   << age                 << endl; // 6
}

void cStats::AddLineage()
{
  tot_lineages++;
  num_lineages++;
}

void cStats::ProcessUpdate()
{
  static int last_update = 0;

  // Increment the "avida_time"
  if (sum_merit.Count() > 0 && sum_merit.Average() > 0) {
    double delta = ((double)(current_update-last_update))/sum_merit.Average();
    avida_time += delta;

    // calculate the true replication rate in this update
    rave_true_replication_rate.Add( num_births/
	  (delta * m_world->GetConfig().AVE_TIME_SLICE.Get() * num_creatures) );
  }
  last_update = current_update;

  // Zero-out any variables which need to be cleared at end of update.

  num_births = 0;
  num_deaths = 0;
  num_breed_true = 0;

  tot_executed += num_executed;
  num_executed = 0;

  task_cur_count.SetAll(0);
  task_last_count.SetAll(0);
  task_exe_count.SetAll(0);

  dom_merit = 0;
  dom_gestation = 0.0;
  dom_fitness = 0.0;
  max_fitness = 0.0;
}

void cStats::RemoveLineage(int id_num, int parent_id, int update_born,
			   double generation_born,
			   int total_CPUs, int total_genotypes, double fitness,
			   double lineage_stat1, double lineage_stat2 )
{
  static bool msg_printed = false;

  if ( !msg_printed ){
    if( fp_lineage_log.good() )
      fp_lineage_log << "# (1) lineage id "
		     << "(2) parent lineage id "
		     << "(3) initial fitness "
		     << "(4) total number of creatures "
		     << "(5) total number of genotypes "
		     << "(6) update born "
		     << "(7) update extinct "
		     << "(8) generation born "
		     << "(9) generation extinct"
		     << "(10) lineage stat1"
		     << "(11) lineage stat2 [10, 11 depend on lineage creation method chosen]"
		     << endl;
    msg_printed = true;
  }

  num_lineages--;
  if( fp_lineage_log.good() ) {
    fp_lineage_log << id_num              << " "   // 1
		   << parent_id           << " "   // 2
		   << fitness             << " "   // 3
		   << total_CPUs          << " "   // 4
		   << total_genotypes     << " "   // 5
		   << update_born         << " "   // 6
		   << cStats::GetUpdate() << " "   // 7
		   << generation_born     << " "   // 8
		   << SumGeneration().Average() << " " // 9
		   << lineage_stat1 	  << " "   // 10
		   << lineage_stat2       << " " << endl;
  }
}


void cStats::PrintDataFile(const cString & filename, const cString & format,
			   char sep)
{
  cDataFile & data_file = m_world->GetDataFile(filename);
  data_manager.PrintRow(data_file, format, sep);
}


void cStats::PrintAverageData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida average data" );
  df.WriteTimeStamp();

  df.Write(GetUpdate(),                    "update" );
  df.Write(sum_merit.Average(),            "average merit" );
  df.Write(sum_gestation.Average(),        "average gestation time" );
  df.Write(sum_fitness.Average(),          "average fitness" );
  df.Write(sum_repro_rate.Average(),       "repro rate?" );
  df.Write(sum_size.Average(),             "average size" );
  df.Write(sum_copy_size.Average(),        "average copied size" );
  df.Write(sum_exe_size.Average(),         "average executed size"  );
  df.Write(sum_abundance.Average(),        "average abundance?" );
  df.Write((double)num_births/num_creatures,
	   "proportion of organisms that gave birth in this update" );
  df.Write((double)num_breed_true/num_creatures,
	   "proportion of breed true organisms" );
  df.Write(sum_genotype_depth.Average(),   "average genotype depth" );
  df.Write(sum_generation.Average(),       "average generation" );
  df.Write(sum_neutral_metric.Average(),   "average neutral metric" );
  df.Write(sum_lineage_label.Average(),    "average lineage label" );
  df.Write(rave_true_replication_rate.Average(),
	   "true replication rate (based on births/update, time-averaged)");
  df.Endl();
}


void cStats::PrintErrorData(const cString & filename)
{
  ofstream & fp = m_world->GetDataFileOFStream(filename);
  assert(fp.good());
  fp<< GetUpdate()                          << " "  // 1
    << sum_merit.StdError()                 << " "  // 2
    << sum_gestation.StdError()             << " "  // 3
    << sum_fitness.StdError()               << " "  // 4
    << sum_repro_rate.StdError()            << " "  // 5
    << sum_size.StdError()                  << " "  // 6
    << sum_copy_size.StdError()             << " "  // 7
    << sum_exe_size.StdError()              << " "  // 8
    << sum_abundance.StdError()             << " "  // 9
    << -1                                   << " "  // 10
    << -1                                   << " "  // 11
    << sum_genotype_depth.StdError()        << " "  // 12
    << sum_generation.StdError()            << " "  // 15
    << sum_neutral_metric.StdError()        << " "  // 16
    << sum_lineage_label.StdError()         << endl;// 17
}


void cStats::PrintVarianceData(const cString & filename)
{
  ofstream & fp = m_world->GetDataFileOFStream(filename);
  assert(fp.good());
  fp<<GetUpdate()                           << " "  // 1
    << sum_merit.Variance()                 << " "  // 2
    << sum_gestation.Variance()             << " "  // 3
    << sum_fitness.Variance()               << " "  // 4
    << sum_repro_rate.Variance()            << " "  // 5
    << sum_size.Variance()                  << " "  // 6
    << sum_copy_size.Variance()             << " "  // 7
    << sum_exe_size.Variance()              << " "  // 8
    << sum_abundance.Variance()             << " "  // 9
    << -1                                   << " "  // 10
    << -1                                   << " "  // 11
    << sum_genotype_depth.Variance()        << " "  // 12
    << sum_generation.Variance()            << " "  // 15
    << sum_neutral_metric.Variance()        << " "  // 16
    << sum_lineage_label.Variance()         << " "  // 17
    << rave_true_replication_rate.Variance()<< endl;// 18
}


void cStats::PrintDominantData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida dominant data" );
  df.WriteTimeStamp();

  df.Write( GetUpdate(),     "update" );
  df.Write( dom_merit,       "average merit of dominant genotype" );
  df.Write( dom_gestation,   "average gestation time of dominant" );
  df.Write( dom_fitness,     "average fitness of dominant genotype" );
  df.Write( dom_repro_rate,  "repro rate?" );
  df.Write( dom_size,        "size of dominant genotype" );
  df.Write( dom_copied_size, "copied size of dominant genotype" );
  df.Write( dom_exe_size,    "executed size of dominant genotype" );
  df.Write( dom_abundance,   "abundance of dominant genotype" );
  df.Write( dom_births,      "number of births" );
  df.Write( dom_breed_true,  "number of dominant breed true?" );
  df.Write( dom_gene_depth,  "dominant gene depth" );
  df.Write( dom_breed_in,    "dominant breed in");
  df.Write( max_fitness,     "max fitness?" );
  df.Write( dom_genotype_id, "genotype ID of dominant genotype" );
  df.Write( dom_name,        "name of dominant genotype" );
  df.Endl();
}

void cStats::PrintDominantParaData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida dominant parasite data" );
  df.WriteTimeStamp();

  df.Write( GetUpdate(),     "update" );
  df.Write( dom_inj_size,        "size of dominant genotype" );
  df.Write( dom_inj_abundance,   "abundance of dominant genotype" );
  df.Write( dom_inj_genotype_id, "genotype ID of dominant genotype" );
  df.Write( dom_inj_name,        "name of dominant genotype" );
  df.Endl();
}

void cStats::PrintStatsData(const cString & filename)
{
  const int genotype_change = num_genotypes - num_genotypes_last;
  const double log_ave_fid = (ave_fidelity > 0) ? -Log(ave_fidelity) : 0.0;
  const double log_dom_fid = (dom_fidelity > 0) ? -Log(dom_fidelity) : 0.0;

  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Generic Statistics Data" );
  df.WriteTimeStamp();

  df.Write( GetUpdate(),        "update" );
  df.Write( energy,             "average inferiority (energy)");
  df.Write( 1.0 - ave_fidelity, "ave probability of any mutations in genome" );
  df.Write( 1.0 - dom_fidelity, "probability of any mutations in dom genome" );
  df.Write( log_ave_fid,        "log(average fidelity)");
  df.Write( log_dom_fid,        "log(dominant fidelity)");
  df.Write( genotype_change,    "change in number of genotypes");
  df.Write( entropy,            "genotypic entropy");
  df.Write( species_entropy,    "species entropy");
  df.Write( coal_depth,         "depth of most reacent coalescence");
  df.Endl();
}


void cStats::PrintCountData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida count data" );
  df.WriteTimeStamp();

  df.Write( GetUpdate(),            "update");
  df.Write( num_executed,           "number of insts executed this update");
  df.Write( num_creatures,          "number of organisms");
  df.Write( num_genotypes,          "number of different genotypes");
  df.Write( num_threshold,          "number of different threshold genotypes");
  df.Write( num_species,            "number of different species");
  df.Write( num_thresh_species,     "number of different threshold species");
  df.Write( num_lineages,           "number of different lineages");
  df.Write( num_births,             "number of births in this update");
  df.Write( num_deaths,             "number of deaths in this update");
  df.Write( num_breed_true,         "number of breed true");
  df.Write( num_breed_true_creatures, "number of breed true organisms?");
  //df.Write( num_parasites,            "number of parasites");
  df.Write( num_no_birth_creatures,   "number of no-birth organisms");
  df.Write( num_single_thread_creatures, "number of single-threaded organisms");
  df.Write( num_multi_thread_creatures, "number of multi-threaded organisms");
  df.Write( num_modified, "number of modified organisms");
  df.Endl();
}


void cStats::PrintTotalsData(const cString & filename)
{
  ofstream & fp = m_world->GetDataFileOFStream(filename);
  assert(fp.good());
  fp << GetUpdate()                  << " " // 1
     << (tot_executed+num_executed)  << " " // 2
     << num_executed            << " " // 3
     << tot_organisms                << " " // 4
     << tot_genotypes                << " " // 5
     << tot_threshold                << " " // 6
     << tot_species                  << " " // 7
     << tot_lineages                 << endl; // 8
}


void cStats::PrintTasksData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida tasks data" );
  df.WriteTimeStamp();
  df.WriteComment( "First column gives the current update, all further columns give the number" );
  df.WriteComment( "of organisms that have the particular task as a component of the merit." );


  df.Write( GetUpdate(),   "Update");
  for(int i = 0; i < task_last_count.GetSize(); i++) {
    df.Write( task_last_count[i], task_names[i] );
  }
  df.Endl();
}


void cStats::PrintTasksExeData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida tasks data" );
  df.WriteTimeStamp();
  df.WriteComment( "First column gives the current update, all further columns give the number" );
  df.WriteComment( "of times the particular task has been executed this update." );

  df.Write( GetUpdate(),   "Update");
  for (int i = 0; i < task_exe_count.GetSize(); i++) {
    df.Write( task_exe_count[i], task_names[i] );
    task_exe_count[i] = 0;
  }
  df.Endl();
}


void cStats::PrintReactionData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida tasks data" );
  df.WriteTimeStamp();
  df.WriteComment( "First column gives the current update, all further columns give the number" );
  df.WriteComment( "of currently living organisms each reaction has affected." );

  df.Write( GetUpdate(),   "Update");
  for (int i = 0; i < reaction_count.GetSize(); i++) {
    df.Write( reaction_count[i], reaction_names[i] );
    task_exe_count[i] = 0;
  }
  df.Endl();
}

void cStats::PrintResourceData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida resource data" );
  df.WriteTimeStamp();
  df.WriteComment( "First column gives the current update, all further columns give the quantity" );
  df.WriteComment( "of the particular resource at that update." );

  df.Write( GetUpdate(),   "Update");
  for (int i = 0; i < resource_count.GetSize(); i++) {
    df.Write( resource_count[i], resource_names[i] );
    if (resource_geometry[i] != nGeometry::GLOBAL) {
      PrintSpatialResData(filename, i);
    }
  }
  df.Endl();
}

void cStats::PrintSpatialResData(const cString & filename, int i)
{

  // Write spatial data to a file that can easily be read into Matlab

  cString tmpfilename = "resource_";
  tmpfilename +=  resource_names[i] + ".m";
  cDataFile & df = m_world->GetDataFile(tmpfilename);
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
}


void cStats::PrintTimeData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida time data" );
  df.WriteTimeStamp();

  df.Write( GetUpdate(),              "update" );
  df.Write( avida_time,               "avida time" );
  df.Write( sum_generation.Average(), "average generation" );
  df.Write( num_executed,             "num_executed?" );
  df.Endl();
}


void cStats::PrintMutationData(const cString & filename)
{
  ofstream & fp = m_world->GetDataFileOFStream(filename);
  assert(fp.good());
  fp << GetUpdate()                              << " "   //  1
     << isum_parent_dist.Ave()                   << " "   //  2
     << isum_parent_size.Ave()                   << " "   //  3
     << isum_child_size.Ave()                    << " "   //  4
     << isum_copied_size.Ave()                   << " "   //  5
     << isum_executed_size.Ave()                 << " "   //  6
     << isum_copies_exec.Ave()                   << " "   //  7
     << isum_point_mut.Ave()                     << " "   //  8
     << isum_copy_mut.Ave()                      << " "   //  9
     << isum_insert_mut.Ave()                    << " "   // 10
     << isum_delete_mut.Ave()                    << " "   // 11
     << isum_point_mut_line.Ave()                << " "   // 12
     << isum_copy_mut_line.Ave()                 << " "   // 13
     << isum_divide_mut.Ave()                    << " "   // 14
     << isum_divide_insert_mut.Ave()             << " "   // 15
     << isum_divide_delete_mut.Ave()             << " "   // 16
     << dsum_copy_mut_by_copies_exec.Ave()       << " "   // 17
     << dsum_copied_size_by_copies_exec.Ave()    << " "   // 18
     << dsum_copy_mut_lines_by_copied_size.Ave() << " "   // 19
     << dsum_copy_mut_lines_by_copy_mut.Ave()    << endl; // 20

  isum_parent_dist.Clear();
  isum_parent_size.Clear();
  isum_child_size.Clear();
  isum_point_mut.Clear();
  isum_copy_mut.Clear();
  isum_insert_mut.Clear();
  isum_point_mut_line.Clear();
  isum_copy_mut_line.Clear();
  isum_delete_mut.Clear();
  isum_divide_mut.Clear();
  isum_divide_insert_mut.Clear();
  isum_divide_delete_mut.Clear();
  isum_copied_size.Clear();
  isum_executed_size.Clear();
  isum_copies_exec.Clear();
  dsum_copy_mut_by_copies_exec.Clear();
  dsum_copied_size_by_copies_exec.Clear();
  dsum_copy_mut_lines_by_copied_size.Clear();
  dsum_copy_mut_lines_by_copy_mut.Clear();
}

void cStats::PrintMutationRateData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida copy mutation rate data" );
  df.WriteTimeStamp();

  df.Write( GetUpdate(),              "Update" );
  df.Write( sum_copy_mut_rate.Ave(),  "Average copy mutation rate" );
  df.Write( sum_copy_mut_rate.Var(),  "Variance in copy mutation rate" );
  df.Write( sum_copy_mut_rate.StdDeviation(),  "Standard Deviation in copy mutation rate" );
  df.Write( sum_copy_mut_rate.Skw(),  "Skew in copy mutation rate" );
  df.Write( sum_copy_mut_rate.Kur(),  "Kurtosis in copy mutation rate" );

  df.Write( sum_log_copy_mut_rate.Ave(),  "Average log(copy mutation rate)" );
  df.Write( sum_log_copy_mut_rate.Var(),  "Variance in log(copy mutation rate)" );
  df.Write( sum_log_copy_mut_rate.StdDeviation(),  "Standard Deviation in log(copy mutation rate)" );
  df.Write( sum_log_copy_mut_rate.Skw(),  "Skew in log(copy mutation rate)" );
  df.Write( sum_log_copy_mut_rate.Kur(),  "Kurtosis in log(copy mutation rate)" );
  df.Endl();

}


void cStats::PrintDivideMutData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida divide mutation rate data" );
  df.WriteTimeStamp();

  df.Write( GetUpdate(),              "Update" );
  df.Write( sum_div_mut_rate.Ave(),  "Average divide mutation rate" );
  df.Write( sum_div_mut_rate.Var(),  "Variance in divide mutation rate" );
  df.Write( sum_div_mut_rate.StdDeviation(),  "Standard Deviation in divide mutation rate" );
  df.Write( sum_div_mut_rate.Skw(),  "Skew in divide mutation rate" );
  df.Write( sum_div_mut_rate.Kur(),  "Kurtosis in divide mutation rate" );

  df.Write( sum_log_div_mut_rate.Ave(),  "Average log(divide mutation rate)" );
  df.Write( sum_log_div_mut_rate.Var(),  "Variance in log(divide mutation rate)" );
  df.Write( sum_log_div_mut_rate.StdDeviation(),  "Standard Deviation in log(divide mutation rate)" );
  df.Write( sum_log_div_mut_rate.Skw(),  "Skew in log(divide mutation rate)" );
  df.Write( sum_log_div_mut_rate.Kur(),  "Kurtosis in log(divide mutation rate)" );
  df.Endl();

}

void cStats::PrintInstructionData(const cString & filename)
{
  cDataFile & df = m_world->GetDataFile(filename);

  df.WriteComment( "Avida instruction execution data" );
  df.WriteTimeStamp();

#ifdef INSTRUCTION_COUNT
  df.Write( GetUpdate(),              "Update" );
  for( int i=0; i < sum_exe_inst_array.GetSize(); i++ ){
    df.Write( (int) sum_exe_inst_array[i].Sum(), inst_names[i] );
  }
#else // INSTRUCTION_COUNT undefined
  cerr<<"Warning: Instruction Counts not compiled in"<<endl;
#endif // ifdef INSTRUCTION_COUNT

  df.Endl();
}

void cStats::PrintGenotypeMap(const cString & filename)
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
}
