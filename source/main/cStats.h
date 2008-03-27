/*
 *  cStats.h
 *  Avida
 *
 *  Called "stats.hh" prior to 12/5/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
 *  Copyright 1993-2002 California Institute of Technology.
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

#ifndef cStats_h
#define cStats_h

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <set>

#ifndef defs_h
#include "defs.h"
#endif
#ifndef cDoubleSum_h
#include "cDoubleSum.h"
#endif
#ifndef functions_h
#include "functions.h"
#endif
#ifndef cIntSum_h
#include "cIntSum.h"
#endif
#ifndef cRunningAverage_h
#include "cRunningAverage.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tDataManager_h
#include "tDataManager.h"
#endif
#ifndef nGeometry_h
#include "nGeometry.h"
#endif

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


class cGenotype;
class cInjectGenotype;
class cWorld;
class cOrgMessage;
class cOrgMessagePredicate;
class cDeme;
class cGermline;

class cStats
{
#if USE_tMemTrack
  tMemTrack<cStats> mt;
#endif
private:
  cWorld* m_world;
  
  // Time scales...
  int m_update;
  int sub_update;
  double avida_time;

  // The data manager handles printing user-formated output files.
  tDataManager<cStats> data_manager;

  //// By Creature Sums ////  (Cleared and resummed by population each update)
  cDoubleSum sum_merit;
  cDoubleSum sum_mem_size;
  cDoubleSum sum_creature_age;
  cDoubleSum sum_generation;

  cDoubleSum sum_neutral_metric;
  cDoubleSum sum_lineage_label;

  cDoubleSum sum_copy_mut_rate;
  cDoubleSum sum_log_copy_mut_rate;

  cDoubleSum sum_div_mut_rate;
  cDoubleSum sum_log_div_mut_rate;

  //// By Genotype Sums ////  (Cleared and resummed by population each update)

  cDoubleSum sum_gestation;
  cDoubleSum sum_fitness;
  cDoubleSum sum_repro_rate;

  // calculates a running average over the actual replication rate
  // given by the number of births per update
  cRunningAverage rave_true_replication_rate;

  cDoubleSum sum_size;
  cDoubleSum sum_copy_size;
  cDoubleSum sum_exe_size;

  cDoubleSum sum_genotype_age;

  // breed/geneological stats
  cDoubleSum sum_abundance;
  cDoubleSum sum_genotype_depth;


  //// By Threshold Sums ////  (Cleared and resummed by population each update)

  cDoubleSum sum_threshold_age;


  //// By Species Sums ////  (Cleared and resummed by population each update)

  cDoubleSum sum_species_age;


  // Instruction Counts (DM)
  tArray<cIntSum> sum_exe_inst_array;


  // Calculated stats
  double entropy;
  double species_entropy;
  double energy;
  double dom_fidelity;
  double ave_fidelity;

  // For tracking of advantageous mutations
  double max_viable_fitness;

  // Dominant Genotype
  cGenotype * dom_genotype;
  double dom_merit;
  double dom_gestation;
  double dom_repro_rate;
  double dom_fitness;
  int dom_size;
  double dom_copied_size;
  double dom_exe_size;
  double max_fitness;
  double max_merit;
  int max_gestation_time;
  int max_genome_length;
  double min_fitness;
  double min_merit;
  int min_gestation_time;
  int min_genome_length;
  int dom_genotype_id;
  cString dom_name;
  int dom_births;
  int dom_breed_true;
  int dom_breed_in;
  int dom_breed_out;
  int dom_abundance;
  int dom_gene_depth;
  cString dom_sequence;
  int coal_depth;

  // Dominant Parasite
  cInjectGenotype * dom_inj_genotype;
  int dom_inj_size;
  int dom_inj_genotype_id;
  cString dom_inj_name;
  int dom_inj_births;
  int dom_inj_abundance;
  cString dom_inj_sequence;

  int num_births;
  int num_deaths;
  int num_breed_in;
  int num_breed_true;
  int num_breed_true_creatures;
  int num_creatures;
  int num_genotypes;
  int num_threshold;
  int num_species;
  int num_thresh_species;
  int num_lineages;
  int num_executed;
  int num_parasites;
  int num_no_birth_creatures;
  int num_single_thread_creatures;
  int num_multi_thread_creatures;
  int m_num_threads;
  int num_modified;

  int num_genotypes_last;

  int tot_organisms;
  int tot_genotypes;
  int tot_threshold;
  int tot_species;
  int tot_thresh_species;
  int tot_lineages;
  int tot_executed;
  
  tArray<int> genotype_map;

  tArray<int> task_cur_count;
  tArray<int> task_last_count;
  tArray<double> task_cur_quality;
  tArray<double> task_last_quality;
  tArray<double> task_cur_max_quality;
  tArray<double> task_last_max_quality;
  tArray<int> task_exe_count;

  tArray<double> reaction_count;
  tArray<double> reaction_add_reward;
  tArray<double> resource_count;
  tArray<int> resource_geometry;
  tArray< tArray<double> > spatial_res_count;

  tArray<cString> task_names;
  tArray<cString> inst_names;
  tArray<cString> reaction_names;
  tArray<cString> resource_names;

  // Resampling Statistics @AWC - 06/29/06
  int num_resamplings;
  int num_failedResamplings;
  
  // State variables
  int last_update;

  // Stats for market econ
  int num_bought;
  int num_sold;
  int num_used;
  int num_own_used;
  
  // Stats for how sense instruction is being used
  int sense_size;
  tArray<int> sense_last_count;
  tArray<int> sense_last_exe_count;
  tArray<cString> sense_names;

  // Stats for competitions
  tArray<double> avg_trial_fitnesses;
  double avg_competition_fitness;
  double min_competition_fitness;
  double max_competition_fitness;
  double avg_competition_copied_fitness;
  double min_competition_copied_fitness;
  double max_competition_copied_fitness;  
  int num_orgs_replicated;

  tArray<int> numAsleep;
  
  // simple deme stats
  cIntSum sum_deme_age;
  cIntSum sum_deme_birth_count;
  cIntSum sum_deme_org_count;
  cIntSum sum_deme_generation;
  cIntSum sum_deme_gestation_time;
  cDoubleSum sum_deme_normalized_time_used;
  cDoubleSum sum_deme_merit;

  // Speculative Execution Stats
  int m_spec_total;
  int m_spec_num;
  int m_spec_waste;
  

  cStats(); // @not_implemented
  cStats(const cStats&); // @not_implemented
  cStats& operator=(const cStats&); // @not_implemented

public:
  cStats(cWorld* world);
  ~cStats() { ; }

  void SetupPrintDatabase();
  void ProcessUpdate();

  inline void SetCurrentUpdate(int new_update) { m_update = new_update; sub_update = 0; }
  inline void IncCurrentUpdate() { m_update++; sub_update = 0; }
  inline void IncSubUpdate() { sub_update++; }

  // Accessors...
  int GetUpdate() const { return m_update; }
  int GetSubUpdate() const { return sub_update; }
  double GetGeneration() const { return SumGeneration().Average(); }

  cGenotype* GetDomGenotype() const { return dom_genotype; }
  double GetDomMerit() const { return dom_merit; }
  double GetDomGestation() const { return dom_gestation; }
  double GetDomReproRate() const { return dom_repro_rate; }
  double GetDomFitness() const { return dom_fitness; }
  double GetDomCopySize() const { return dom_copied_size; }
  double GetDomExeSize() const { return dom_exe_size; }

  int GetDomSize() const { return dom_size; }
  int GetDomID() const { return dom_genotype_id; }
  const cString & GetDomName() const { return dom_name; }
  int GetDomBirths() const { return dom_births; }
  int GetDomBreedTrue() const { return dom_breed_true; }
  int GetDomBreedIn() const { return dom_breed_in; }
  int GetDomBreedOut() const { return dom_breed_out; }
  int GetDomAbundance() const { return dom_abundance; }
  int GetDomGeneDepth() const { return dom_gene_depth; }
  const cString & GetDomSequence() const { return dom_sequence; }

  cInjectGenotype * GetDomInjGenotype() const { return dom_inj_genotype; }
  int GetDomInjSize() const { return dom_inj_size; }
  int GetDomInjID() const { return dom_inj_genotype_id; }
  const cString & GetDomInjName() const { return dom_inj_name; }
  int GetDomInjBirths() const { return dom_inj_births; }
  int GetDomInjAbundance() const { return dom_inj_abundance; }
  const cString & GetDomInjSequence() const { return dom_inj_sequence; }
  
  int GetSenseSize() const { return sense_size; }

  // Settings...
  void SetDomGenotype(cGenotype * in_gen) { dom_genotype = in_gen; }
  void SetDomMerit(double in_merit) { dom_merit = in_merit; }
  void SetDomGestation(double in_gest) { dom_gestation = in_gest; }
  void SetDomReproRate(double in_rate) { dom_repro_rate = in_rate; }
  void SetDomFitness(double in_fit) { dom_fitness = in_fit; }
  void SetDomCopiedSize(double in_size) { dom_copied_size = in_size; }
  void SetDomExeSize(double in_size) { dom_exe_size = in_size; }

  void SetDomSize(int in_size) { dom_size = in_size; }
  void SetDomID(int in_id) { dom_genotype_id = in_id; }
  void SetDomName(const cString & in_name) { dom_name = in_name; }
  void SetDomBirths(int in_births) { dom_births = in_births; }
  void SetDomBreedTrue(int in_bt) { dom_breed_true = in_bt; }
  void SetDomBreedIn(int in_bi) { dom_breed_in = in_bi; }
  void SetDomBreedOut(int in_bo) { dom_breed_out = in_bo; }
  void SetDomAbundance(int in_abund) { dom_abundance = in_abund; }
  void SetDomGeneDepth(int in_depth) { dom_gene_depth = in_depth; }
  void SetDomSequence(const cString & in_seq) { dom_sequence = in_seq; }

  void SetDomInjGenotype(cInjectGenotype * in_inj_genotype) { dom_inj_genotype = in_inj_genotype; }
  void SetDomInjSize(int in_inj_size) { dom_inj_size = in_inj_size; }
  void SetDomInjID(int in_inj_ID) { dom_inj_genotype_id = in_inj_ID; }
  void SetDomInjName(const cString & in_name) { dom_inj_name = in_name; }
  void SetDomInjBirths(int in_births) { dom_inj_births = in_births; }
  void SetDomInjAbundance(int in_inj_abundance) { dom_inj_abundance = in_inj_abundance; }
  void SetDomInjSequence(const cString & in_inj_sequence) { dom_inj_sequence = in_inj_sequence; }

  void SetGenoMapElement(int i, int in_geno) { genotype_map[i] = in_geno; }
  void SetCoalescentGenotypeDepth(int in_depth) {coal_depth = in_depth;}

  inline void SetNumGenotypes(int new_genotypes);
  inline void SetNumCreatures(int new_creatures);
  inline void SetNumThreshSpecies(int new_thresh_species);
  inline void SetBreedTrueCreatures(int in_num_breed_true_creatures);
  inline void SetNumParasites(int in_num_parasites);
  inline void SetNumNoBirthCreatures(int in_num_no_birth_creatures);
  inline void SetNumSingleThreadCreatures(int in_num_single_thread_creatures);
  inline void SetNumMultiThreadCreatures(int in_num_multi_thread_creatures);
  inline void SetNumThreads(int in_num_threads) { m_num_threads = in_num_threads; }
  inline void SetNumModified(int in_num_modified);

  void SetMaxFitness(double in_max_fitness) { max_fitness = in_max_fitness; }
  void SetMaxMerit(double in_max_merit) { max_merit = in_max_merit; }
  void SetMaxGestationTime(int in_max_gestation_time) { max_gestation_time = in_max_gestation_time; }
  void SetMaxGenomeLength(int in_max_genome_length) { max_genome_length = in_max_genome_length; }

  void SetMinFitness(double in_min_fitness) { min_fitness = in_min_fitness; }
  void SetMinMerit(double in_min_merit) { min_merit = in_min_merit; }
  void SetMinGestationTime(int in_min_gestation_time) { min_gestation_time = in_min_gestation_time; }
  void SetMinGenomeLength(int in_min_genome_length) { min_genome_length = in_min_genome_length; }

  void SetEntropy(double in_entropy) { entropy = in_entropy; }
  void SetSpeciesEntropy(double in_ent) { species_entropy = in_ent; }

  cDoubleSum& SumFitness()       { return sum_fitness; }
  cDoubleSum& SumGestation()     { return sum_gestation; }
  cDoubleSum& SumMerit()         { return sum_merit; }
  cDoubleSum& SumReproRate()     { return sum_repro_rate; }

  cDoubleSum& SumCreatureAge()   { return sum_creature_age; }
  cDoubleSum& SumGenotypeAge()   { return sum_genotype_age; }
  cDoubleSum& SumGeneration()    { return sum_generation; }
  cDoubleSum& SumAbundance()     { return sum_abundance; }
  cDoubleSum& SumGenotypeDepth() { return sum_genotype_depth; }
  cDoubleSum& SumThresholdAge()  { return sum_threshold_age; }
  cDoubleSum& SumSpeciesAge()    { return sum_species_age; }

  cDoubleSum& SumNeutralMetric() { return sum_neutral_metric; }
  cDoubleSum& SumLineageLabel()  { return sum_lineage_label; }
  cDoubleSum& SumCopyMutRate()   { return sum_copy_mut_rate; }
  cDoubleSum& SumLogCopyMutRate()   { return sum_log_copy_mut_rate; }
  cDoubleSum& SumDivMutRate()   { return sum_div_mut_rate; }
  cDoubleSum& SumLogDivMutRate()   { return sum_log_div_mut_rate; }

  cDoubleSum& SumSize()          { return sum_size; }
  cDoubleSum& SumCopySize()      { return sum_copy_size; }
  cDoubleSum& SumExeSize()       { return sum_exe_size; }
  cDoubleSum& SumMemSize()       { return sum_mem_size; }

  //deme
  cIntSum& SumDemeAge()          { return sum_deme_age; }
  cIntSum& SumDemeBirthCount()   { return sum_deme_birth_count; }
  cIntSum& SumDemeOrgCount()     { return sum_deme_org_count; }
  cIntSum& SumDemeGeneration()   { return sum_deme_generation; }
  cIntSum& SumDemeGestationTime()   { return sum_deme_gestation_time; }
  cDoubleSum& SumDemeNormalizedTimeUsed()   { return sum_deme_normalized_time_used; }
  cDoubleSum& SumDemeMerit()   { return sum_deme_merit; }

#if INSTRUCTION_COUNT
  void ZeroInst();
#endif
  tArray<cIntSum>& SumExeInst() { return sum_exe_inst_array; }

  // And constant versions of the above...
  const cDoubleSum& SumFitness() const       { return sum_fitness; }
  const cDoubleSum& SumGestation() const     { return sum_gestation; }
  const cDoubleSum& SumMerit() const         { return sum_merit; }
  const cDoubleSum& SumReproRate() const     { return sum_repro_rate; }

  const cDoubleSum& SumCreatureAge() const   { return sum_creature_age; }
  const cDoubleSum& SumGenotypeAge() const   { return sum_genotype_age; }
  const cDoubleSum& SumGeneration() const    { return sum_generation; }
  const cDoubleSum& SumAbundance() const     { return sum_abundance; }
  const cDoubleSum& SumGenotypeDepth() const { return sum_genotype_depth; }
  const cDoubleSum& SumThresholdAge() const  { return sum_threshold_age; }
  const cDoubleSum& SumSpeciesAge() const    { return sum_species_age; }

  const cDoubleSum& SumNeutralMetric() const { return sum_neutral_metric; }
  const cDoubleSum& SumLineageLabel() const  { return sum_lineage_label; }
  const cDoubleSum& SumCopyMutRate() const   { return sum_copy_mut_rate; }
  const cDoubleSum& SumLogCopyMutRate() const{ return sum_log_copy_mut_rate; }
  const cDoubleSum& SumDivMutRate() const   { return sum_div_mut_rate; }
  const cDoubleSum& SumLogDivMutRate() const{ return sum_log_div_mut_rate; }

  const cDoubleSum& SumSize() const          { return sum_size; }
  const cDoubleSum& SumCopySize() const      { return sum_copy_size; }
  const cDoubleSum& SumExeSize() const       { return sum_exe_size; }
  const cDoubleSum& SumMemSize() const       { return sum_mem_size; }

  //deme
  const cIntSum& SumDemeAge() const          { return sum_deme_age; }
  const cIntSum& SumDemeBirthCount() const   { return sum_deme_birth_count; }
  const cIntSum& SumDemeOrgCount() const     { return sum_deme_org_count; }
  const cIntSum& SumDemeGeneration() const   { return sum_deme_generation; }
  const cIntSum& SumDemeGestationTime() const  { return sum_deme_generation; }
  const cDoubleSum& SumDemeNormalizedTimeUsed() const  { return sum_deme_normalized_time_used; }
  const cDoubleSum& SumDemeMerit()  const  { return sum_deme_merit; }


  void IncResamplings() { ++num_resamplings; }  // @AWC 06/29/06
  void IncFailedResamplings() { ++num_failedResamplings; }  // @AWC 06/29/06

  void CalcEnergy();
  void CalcFidelity();

  void RecordBirth(int cell_id, int genotype_id, bool breed_true);
  void RecordDeath() { num_deaths++; }
  void AddGenotype() { tot_genotypes++; }
  void RemoveGenotype(int id_num, int parent_id,
			     int parent_distance, int depth, int max_abundance,
			     int parasite_abundance, int age, int length);
  void AddThreshold(int id_num, const char * name,
				  int species_num=-1);
  void RemoveThreshold() { num_threshold--; }
  void AddSpecies() { tot_species++; num_species++; }
  void RemoveSpecies(int id_num, int parent_id,
			 int max_gen_abundance, int max_abundance, int age);
  void AddLineage() { tot_lineages++; num_lineages++; }
  void RemoveLineage(int id_num, int parent_id, int update_born,
		     double generation_born, int total_CPUs,
		     int total_genotypes, double fitness, 
		     double lineage_stat1, double lineage_stat2 );
				
  void IncExecuted() { num_executed++; }

  void AddCurTask(int task_num) { task_cur_count[task_num]++; }
  void AddCurTaskQuality(int task_num, double quality) 
  {  
	  task_cur_quality[task_num] += quality;
	  if (quality > task_cur_max_quality[task_num])
		  task_cur_max_quality[task_num] = quality;
  }
  void AddLastTask(int task_num) { task_last_count[task_num]++; }
  void AddLastTaskQuality(int task_num, double quality) 
  { 
	  task_last_quality[task_num] += quality; 
	  if (quality > task_last_max_quality[task_num])
		  task_last_max_quality[task_num] = quality;
  }
  void IncTaskExeCount(int task_num, int task_count) 
    { task_exe_count[task_num] += task_count; }
  void ZeroTasks();
  
  void AddLastSense(int res_comb_index) { sense_last_count[res_comb_index]++; }
  void IncLastSenseExeCount(int res_comb_index, int count) 
    { sense_last_exe_count[res_comb_index]+= count; }
    
  void SetReactions(const tArray<double> &_in) { reaction_count = _in; }
  void AddLastReactionAddReward(int _id, double _reward) { reaction_add_reward[_id] += _reward; }
  void ZeroRewards();
  
  void SetResources(const tArray<double> &_in) { resource_count = _in; }
  void SetResourcesGeometry(const tArray<int> &_in) { resource_geometry = _in;}
  void SetSpatialRes(const tArray< tArray<double> > &_in) { 
    spatial_res_count = _in;
  }

  void SetInstName(int id, const cString & name) {
    assert(id < inst_names.GetSize());
    inst_names[id] = name;
  }
  void SetReactionName(int id, const cString & name) { reaction_names[id] = name; }
  void SetResourceName(int id, const cString & name) { resource_names[id] = name; }

  void SetCompetitionTrialFitnesses(tArray<double> _in) { avg_trial_fitnesses = _in; }
  void SetCompetitionFitnesses(double _in_avg, double _in_min, double _in_max, double _in_cp_avg, double _in_cp_min, double _in_cp_max) 
    { avg_competition_fitness = _in_avg; min_competition_fitness = _in_min; max_competition_fitness = _in_max; 
      avg_competition_copied_fitness = _in_cp_avg; min_competition_copied_fitness = _in_cp_min; max_competition_copied_fitness = _in_cp_max; }
  void SetCompetitionOrgsReplicated(int _in) { num_orgs_replicated = _in; }

  //market info
  void AddMarketItemBought() { num_bought++;}
  void AddMarketItemSold() { num_sold++; }
  void AddMarketItemUsed() { num_used++; }
  void AddMarketOwnItemUsed() { num_own_used++; }
  
  
  void AddSpeculative(int spec) { m_spec_total += spec; m_spec_num++; }
  void AddSpeculativeWaste(int waste) { m_spec_waste += waste; }

  // Information retrieval section...

  int GetNumBirths() const          { return num_births; }
  int GetNumDeaths() const          { return num_deaths; }
  int GetBreedIn() const            { return num_breed_in; }
  int GetBreedTrue() const          { return num_breed_true; }
  int GetBreedTrueCreatures() const { return num_breed_true_creatures; }
  int GetNumCreatures() const       { return num_creatures; }
  int GetNumGenotypes() const       { return num_genotypes; }
  int GetNumThreshold() const       { return num_threshold; }
  int GetNumSpecies() const         { return num_species; }
  int GetNumThreshSpecies() const   { return num_thresh_species; }
  int GetNumLineages() const        { return num_lineages; }
  int GetNumParasites() const       { return num_parasites; }
  int GetNumNoBirthCreatures() const{ return num_no_birth_creatures; }
  int GetNumSingleThreadCreatures() const { return num_single_thread_creatures; }
  int GetNumMultiThreadCreatures() const { return num_multi_thread_creatures; }
  int GetNumThreads() const { return m_num_threads; }
  int GetNumModified() const { return num_modified;}

  int GetTotCreatures() const       { return tot_organisms; }
  int GetTotGenotypes() const       { return tot_genotypes; }
  int GetTotThreshold() const       { return tot_threshold; }
  int GetTotSpecies() const         { return tot_species; }
  int GetTotThreshSpecies() const   { return tot_thresh_species; }
  int GetTotLineages() const        { return tot_lineages; }

  int GetTaskCurCount(int task_num) const { return task_cur_count[task_num]; }
  double GetTaskCurQuality(int task_num) const { return task_cur_quality[task_num]/(double)task_cur_count[task_num]; }  
  int GetTaskLastCount(int task_num) const {return task_last_count[task_num];}
  double GetTaskLastQuality(int task_num) const {return task_last_quality[task_num]/(double)task_last_count[task_num];}
  double GetTaskMaxCurQuality(int task_num) const { return task_cur_max_quality[task_num];}
  double GetTaskMaxLastQuality(int task_num) const { return task_last_max_quality[task_num];}
  int GetTaskExeCount(int task_num) const { return task_exe_count[task_num]; }

  const tArray<double> & GetReactions() const { return reaction_count; }
  const tArray<double> & GetResources() const { return resource_count; }

  // market info
  int GetMarketNumBought() const { return num_bought; }
  int GetMarketNumSold() const { return num_sold; }
  int GetMarketNumUsed() const { return num_used; }
  int GetMarketNumOwnUsed() const { return num_own_used; }

  double GetAveReproRate() const  { return sum_repro_rate.Average(); }

  double GetAveMerit() const      { return sum_merit.Average(); }
  double GetAveCreatureAge() const{ return sum_creature_age.Average(); }
  double GetAveMemSize() const    { return sum_mem_size.Average(); }

  double GetAveNeutralMetric() const { return sum_neutral_metric.Average(); }
  double GetAveLineageLabel() const  { return sum_lineage_label.Average(); }
  double GetAveCopyMutRate() const   { return sum_copy_mut_rate.Average(); }
  double GetAveLogCopyMutRate() const{ return sum_log_copy_mut_rate.Average();}
  double GetAveDivMutRate() const   { return sum_div_mut_rate.Average(); }
  double GetAveLogDivMutRate() const{ return sum_log_div_mut_rate.Average();}

  double GetAveGestation() const { return sum_gestation.Average(); }
  double GetAveFitness() const   { return sum_fitness.Average(); }

  double GetAveGenotypeAge() const { return sum_genotype_age.Average();}

  double GetAveSize() const       { return sum_size.Average(); }
  double GetAveCopySize() const   { return sum_copy_size.Average(); }
  double GetAveExeSize() const    { return sum_exe_size.Average(); }

  double GetEntropy() const        { return entropy; }
  double GetSpeciesEntropy() const { return species_entropy; }
  double GetEnergy() const         { return energy; }
  double GetEvenness() const       { return entropy / Log(num_genotypes); }
  int GetCoalescentDepth() const   { return coal_depth; }

  double GetAveThresholdAge() const { return sum_threshold_age.Average(); }
  double GetAveSpeciesAge() const { return sum_species_age.Average(); }

  double GetMaxFitness() const { return max_fitness; }
  double GetMaxMerit() const { return max_merit; }
  int GetMaxGestationTime() const { return max_gestation_time; }
  int GetMaxGenomeLength() const { return max_genome_length; }

  double GetMinFitness() const { return min_fitness; }
  double GetMinMerit() const { return min_merit; }
  int GetMinGestationTime() const { return min_gestation_time; }
  int GetMinGenomeLength() const { return min_genome_length; }


  int GetResamplings() const { return num_resamplings;}  //AWC 06/29/06
  int GetFailedResamplings() const { return num_failedResamplings;}  //AWC 06/29/06

  int GetNumSenseSlots();

  int getNumAsleep(int demeID) { return numAsleep[demeID]; }
  void incNumAsleep(int demeID) { numAsleep[demeID]++; }
  void decNumAsleep(int demeID) { numAsleep[demeID]--; }

  double GetAveSpeculative() const { return (m_spec_num) ? ((double)m_spec_total / (double)m_spec_num) : 0.0; }
  int GetSpeculativeWaste() const { return m_spec_waste; }

  // this value gets recorded when a creature with the particular
  // fitness value gets born. It will never change to a smaller value,
  // i.e., when the maximum fitness in the population drops, this value will
  // still stay up.
  double GetMaxViableFitness() const { return max_viable_fitness; }

  // User-defined datafile...
  void PrintDataFile(const cString& filename, const cString& format, char sep=' ');

  // Public calls to output data files (for events)
  void PrintAverageData(const cString& filename);
  void PrintDemeAverageData(const cString& filename);
  void PrintErrorData(const cString& filename);
  void PrintVarianceData(const cString& filename);
  void PrintDominantData(const cString& filename);
  void PrintParasiteData(const cString& filename);
  void PrintStatsData(const cString& filename);
  void PrintCountData(const cString& filename);
  void PrintTotalsData(const cString& filename);
  void PrintTasksData(const cString& filename);
  void PrintTasksExeData(const cString& filename);
  void PrintTasksQualData(const cString& filename);
  void PrintReactionData(const cString& filename);
  void PrintCurrentReactionData(const cString& filename);
  void PrintReactionRewardData(const cString& filename);
  void PrintResourceData(const cString& filename);
  void PrintSpatialResData(const cString& filename, int i);
  void PrintTimeData(const cString& filename);
  void PrintDivideMutData(const cString& filename);
  void PrintMutationRateData(const cString& filename);
  void PrintInstructionData(const cString& filename);
  void PrintGenotypeMap(const cString& filename);
  void PrintMarketData(const cString& filename);
  void PrintSenseData(const cString& filename);
  void PrintSenseExeData(const cString& filename);
  void PrintSleepData(const cString& filename);
  void PrintCompetitionData(const cString& filename);
  // @WRE: Added event for printing visit counts
  void PrintCellVisitsData(const cString& filename);

  // -------- Messaging support --------
public:
  //! Type for a list of pointers to message predicates.
  typedef std::vector<cOrgMessagePredicate*> message_pred_ptr_list;
  
  //! Called for every message successfully sent anywhere in the population.
  void SentMessage(const cOrgMessage& msg);
  //! Adds a predicate that will be evaluated for each message.
  void AddMessagePredicate(cOrgMessagePredicate* predicate);
  //! Prints information regarding messages that "passed" their predicate.
  void PrintPredicatedMessages(const cString& filename);

protected:
  /*! List of all active message predicates.  The idea here is that the predicates,
  rather than cStats / cOrgMessage / etc., do the tracking of particular messages
  of interest. */
  message_pred_ptr_list m_message_predicates;
  // -------- End messaging support --------
  
  
  // -------- Deme replication support --------
public:
  //! Called immediately prior to deme replacement.
  void DemePreReplication(cDeme& source_deme, cDeme& target_deme);
  //! Called immediately after deme replacement.
  void DemePostReplication(cDeme& source_deme, cDeme& target_deme);
  //! Called immediately prior to germline replacement.
  void GermlineReplication(cGermline& source_germline, cGermline& target_germline);
  //! Print statistics about deme replication.
  void PrintDemeReplicationData(const cString& filename);
  //! Print statistics about germlines.
  void PrintGermlineData(const cString& filename);
  //! Accessor for average "generation" of germlines.
  double GetAveGermlineGeneration() const { return m_germline_generation.Average(); }
  /*! Typedef of a data structure to track deme founders.
    * Map of deme id -> {founder genotype id_0, id_1,... id_{deme propagule size}} */
  typedef std::map<int, std::set<int> > t_founder_map;
  //! Print the genotype IDs for the founders of demes that have recently been "born."
  void PrintDemeFounders(const cString& filename);

protected:
  int m_deme_num_repls; //!< Number of deme replications since last PrintDemeReplicationData.
  cDoubleSum m_deme_gestation_time; //!< Gestation time for demes - mean age at deme replication.
  cDoubleSum m_deme_births; //!< Mean number of births in replicated demes.
  cDoubleSum m_deme_merit; //!< Mean merit of replicated demes.
  cDoubleSum m_deme_generation; //!< Mean generation of replicated demes.
  cDoubleSum m_germline_generation; //!< "Generation" accumulator of replicated germlines.
  t_founder_map m_deme_founders; //!< Data structure to track the founders of demes.
};


#ifdef ENABLE_UNIT_TESTS
namespace nStats {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  


inline void cStats::SetNumGenotypes(int new_genotypes)
{
  num_genotypes_last = num_genotypes;
  num_genotypes = new_genotypes;
}

inline void cStats::SetNumCreatures(int new_creatures)
{
  num_creatures = new_creatures;
}

inline void cStats::SetNumThreshSpecies(int new_thresh_species)
{
  num_thresh_species = new_thresh_species;
}

inline void cStats::SetBreedTrueCreatures(int in_num_breed_true_creatures)
{
  num_breed_true_creatures = in_num_breed_true_creatures;
}

inline void cStats::SetNumParasites(int in_num_parasites)
{
  num_parasites = in_num_parasites;
}

inline void cStats::SetNumNoBirthCreatures(int in_num_no_birth_creatures)
{
  num_no_birth_creatures = in_num_no_birth_creatures;
}

inline void cStats::SetNumSingleThreadCreatures(int in_num_single_thread_creatures)
{
  num_single_thread_creatures = in_num_single_thread_creatures;
}

inline void cStats::SetNumMultiThreadCreatures(int in_num_multi_thread_creatures)
{
  num_multi_thread_creatures = in_num_multi_thread_creatures;
}

inline void cStats::SetNumModified(int in_num_modified)
{
  num_modified = in_num_modified;
}

#endif
