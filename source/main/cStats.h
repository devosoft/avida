/*
 *  cStats.h
 *  Avida
 *
 *  Called "stats.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2002 California Institute of Technology.
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

#ifndef cStats_h
#define cStats_h

#include "avida/core/InstructionSequence.h"
#include "avida/data/Provider.h"

#include "apto/stat/Accumulator.h"

#include "cBirthEntry.h"
#include "cOrganism.h"
#include "cRunningAverage.h"
#include "tDataManager.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <utility>

class cWorld;
class cOrganism;

using namespace Avida;


struct s_inst_circumstances {
  int update;
  int inst;
  int gr_id;
  int gr_size;
  double res_level;
  double odds_immigrants;
  double odds_offspring_own;
  double odds_offspring_others;
  int tol_immigrants;
  int tol_own;
  int tol_others;
  int tol_max;
};

class cStats : public Data::ArgumentedProvider
{
private:
  cWorld* m_world;

  tDataManager<cStats> m_data_manager;
  
  
  // --------  Data Provider Support  ---------
  struct ProvidedData
  {
    Apto::String description;
    Apto::Functor<Data::PackagePtr, Apto::NullType> GetData;
    
    ProvidedData() { ; }
    ProvidedData(const Apto::String& desc, Apto::Functor<Data::PackagePtr, Apto::NullType> func)
      : description(desc), GetData(func) { ; } 
  };
  Apto::Map<Apto::String, ProvidedData> m_provided_data;
  mutable Data::ConstDataSetPtr m_provides;


  // --------  Time scales  ---------
  int m_update;
  double avida_time;



  // --------  Organism Sums  ---------  (Cleared and resummed by population each update)
  Apto::Stat::Accumulator<double> sum_merit;
  Apto::Stat::Accumulator<double> sum_mem_size;
  Apto::Stat::Accumulator<double> sum_creature_age;
  Apto::Stat::Accumulator<double> sum_generation;

  Apto::Stat::Accumulator<double> sum_neutral_metric;

  Apto::Stat::Accumulator<double> sum_copy_mut_rate;
  Apto::Stat::Accumulator<double> sum_log_copy_mut_rate;

  Apto::Stat::Accumulator<double> sum_div_mut_rate;
  Apto::Stat::Accumulator<double> sum_log_div_mut_rate;


  // --------  Genotype Sums  ---------  (Cleared and resummed by population each update)
  Apto::Stat::Accumulator<double> sum_gestation;
  Apto::Stat::Accumulator<double> sum_fitness;
  Apto::Stat::Accumulator<double> sum_repro_rate;


  Apto::Stat::Accumulator<double> sum_copy_size;
  Apto::Stat::Accumulator<double> sum_exe_size;



  // --------  Instruction Counts  ---------
  Apto::Map<cString, Apto::Array<cString> > m_is_inst_names_map;

  Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > > m_is_prey_exe_inst_map;
  Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > > m_is_pred_exe_inst_map;
  Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > > m_is_prey_fail_exe_inst_map;
  Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > > m_is_pred_fail_exe_inst_map;
  Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > > m_is_tpred_exe_inst_map;
  Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > > m_is_prey_from_sensor_inst_map;
  Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > > m_is_pred_from_sensor_inst_map;
  Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > > m_is_tpred_from_sensor_inst_map;

  
  // --------  Calculated Stats  ---------

  // For tracking of advantageous mutations
  double max_viable_fitness;

  // --------  Dominant Genotype  ---------
  double max_fitness;
  double max_merit;
  int max_gestation_time;
  int max_genome_length;
  double min_fitness;
  double min_merit;
  int min_gestation_time;
  int min_genome_length;


  // --------  Population Stats  ---------
  int num_births;
  int cumulative_births;
  int num_deaths;
  int num_breed_in;
  int num_breed_true;
  int num_breed_true_creatures;
  int num_creatures;
  int num_executed;
  int num_no_birth_creatures;
  int num_single_thread_creatures;
  int num_multi_thread_creatures;
  int m_num_threads;
  int num_modified;

  int tot_organisms;
  int tot_executed;


  // --------  Organism Task Stats  ---------
  mutable bool m_collect_env_test_stats;
  Apto::Array<int> task_cur_count;
  Apto::Array<int> task_last_count;
  Apto::Array<int> task_test_count;
  Apto::Array<double> task_cur_quality;
  Apto::Array<double> task_last_quality;
  Apto::Array<double> task_cur_max_quality;
  Apto::Array<double> task_last_max_quality;
  Apto::Array<int> task_exe_count;
  Apto::Array<int> new_task_count;
  Apto::Array<int> prev_task_count;
  Apto::Array<int> cur_task_count;
  Apto::Array<int> new_reaction_count;

  Apto::Array<int> m_reaction_cur_count;
  Apto::Array<int> m_reaction_last_count;
  Apto::Array<double> m_reaction_cur_add_reward;
  Apto::Array<double> m_reaction_last_add_reward;
  Apto::Array<int> m_reaction_exe_count;

  Apto::Array<double> resource_count;
  Apto::Array<int> resource_geometry;
  Apto::Array< Apto::Array<double> > spatial_res_count;

  Apto::Array<cString> task_names;
  Apto::Array<cString> reaction_names;
  Apto::Array<cString> resource_names;

  // --------  Resampling Stats  ---------
  int num_resamplings;
  int num_failedResamplings;


  // --------  State Variables  ---------
  int last_update;


  // --------  Sense Instruction Stats  ---------
  int sense_size;
  Apto::Array<int> sense_last_count;
  Apto::Array<int> sense_last_exe_count;
  Apto::Array<cString> sense_names;


  // --------  Speculative Execution Stats  ---------
  int m_spec_total;
  int m_spec_num;
  int m_spec_waste;


  // --------  Organism Kill Stats  ---------
  Apto::Stat::Accumulator<int> sum_orgs_killed;
  Apto::Stat::Accumulator<int> sum_unoccupied_cell_kill_attempts;
  Apto::Stat::Accumulator<int> sum_cells_scanned_at_kill;


  // --------  Pred-prey Stats  ---------
  Apto::Stat::Accumulator<double> sum_prey_fitness;
  Apto::Stat::Accumulator<double> sum_prey_gestation;
  Apto::Stat::Accumulator<double> sum_prey_merit;
  Apto::Stat::Accumulator<double> sum_prey_creature_age;
  Apto::Stat::Accumulator<double> sum_prey_generation;  
  Apto::Stat::Accumulator<double> sum_prey_size;
  
  Apto::Stat::Accumulator<double> sum_pred_fitness;
  Apto::Stat::Accumulator<double> sum_pred_gestation;
  Apto::Stat::Accumulator<double> sum_pred_merit;
  Apto::Stat::Accumulator<double> sum_pred_creature_age;
  Apto::Stat::Accumulator<double> sum_pred_generation;  
  Apto::Stat::Accumulator<double> sum_pred_size;

  Apto::Stat::Accumulator<double> sum_tpred_fitness;
  Apto::Stat::Accumulator<double> sum_tpred_gestation;
  Apto::Stat::Accumulator<double> sum_tpred_merit;
  Apto::Stat::Accumulator<double> sum_tpred_creature_age;
  Apto::Stat::Accumulator<double> sum_tpred_generation;
  Apto::Stat::Accumulator<double> sum_tpred_size;

  double prey_entropy;
  double pred_entropy;
  double tpred_entropy;

  
  // --------  TopNavTrace Stats  ---------
  Apto::Array<char> toptrace;
  Apto::Array<int> topnavtraceupdate;
  Apto::Array<int> topnavtraceloc;
  Apto::Array<int> topnavtracefacing;
  Apto::Array<int> topreactions;
  Apto::Array<int> topreactioncycles;
  Apto::Array<int> topreactionexecs;
  int topreac;
  int topcycle;
  int topid;
  int topgenid;
  int toptarget;
  int topgroup;
  int topbirthud;
  int topstart;
  int toprepro;
  bool firstnavtrace;
  Genome topgenome;
    
public:
  cStats(cWorld* world);
  ~cStats() { ; }

  
  // Data::Provider
  Data::ConstDataSetPtr Provides() const;
  void UpdateProvidedValues(Update current_update);
  Apto::String DescribeProvidedValue(const Apto::String& data_id) const;

  // Data::ArgumentedProvider
  void SetActiveArguments(const Data::DataID& data_id, Data::ConstArgumentSetPtr args);
  Data::ConstArgumentSetPtr GetValidArguments(const Data::DataID& data_id) const;
  bool IsValidArgument(const Data::DataID& data_id, Data::Argument arg) const;
  
  Data::PackagePtr GetProvidedValueForArgument(const Data::DataID& data_id, const Data::Argument& arg) const;
  
  // cStats
  void ProcessUpdate();

  inline void SetCurrentUpdate(int new_update) { m_update = new_update; }
  inline void IncCurrentUpdate() { m_update++; }

  // Accessors...
  int GetUpdate() const { return m_update; }
  double GetGeneration() const { return SumGeneration().Average(); }

  int GetSenseSize() const { return sense_size; }

  // Settings...

  inline void SetNumGenotypes(int new_genotypes, int num_historic);
  inline void SetNumCreatures(int new_creatures) { num_creatures = new_creatures; }
  inline void SetBreedTrueCreatures(int in_num_breed_true_creatures);
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

  void SetPreyEntropy(double in_prey_entropy) { prey_entropy = in_prey_entropy; }
  void SetPredEntropy(double in_pred_entropy) { pred_entropy = in_pred_entropy; }
  void SetTopPredEntropy(double in_tpred_entropy) { tpred_entropy = in_tpred_entropy; }
  
  Apto::Stat::Accumulator<double>& SumFitness()       { return sum_fitness; }
  Apto::Stat::Accumulator<double>& SumGestation()     { return sum_gestation; }
  Apto::Stat::Accumulator<double>& SumMerit()         { return sum_merit; }
  Apto::Stat::Accumulator<double>& SumReproRate()     { return sum_repro_rate; }

  Apto::Stat::Accumulator<double>& SumCreatureAge()   { return sum_creature_age; }
  Apto::Stat::Accumulator<double>& SumGeneration()    { return sum_generation; }

  Apto::Stat::Accumulator<double>& SumNeutralMetric() { return sum_neutral_metric; }
  Apto::Stat::Accumulator<double>& SumCopyMutRate()   { return sum_copy_mut_rate; }
  Apto::Stat::Accumulator<double>& SumLogCopyMutRate()   { return sum_log_copy_mut_rate; }
  Apto::Stat::Accumulator<double>& SumDivMutRate()   { return sum_div_mut_rate; }
  Apto::Stat::Accumulator<double>& SumLogDivMutRate()   { return sum_log_div_mut_rate; }

  Apto::Stat::Accumulator<double>& SumCopySize()      { return sum_copy_size; }
  Apto::Stat::Accumulator<double>& SumExeSize()       { return sum_exe_size; }
  Apto::Stat::Accumulator<double>& SumMemSize()       { return sum_mem_size; }


  //pred-prey
  Apto::Stat::Accumulator<double>& SumPreyFitness()       { return sum_prey_fitness; }
  Apto::Stat::Accumulator<double>& SumPreyGestation()     { return sum_prey_gestation; }
  Apto::Stat::Accumulator<double>& SumPreyMerit()         { return sum_prey_merit; }
  Apto::Stat::Accumulator<double>& SumPreyCreatureAge()   { return sum_prey_creature_age; }
  Apto::Stat::Accumulator<double>& SumPreyGeneration()    { return sum_prey_generation; }  
  Apto::Stat::Accumulator<double>& SumPreySize()          { return sum_prey_size; }
  Apto::Array<Apto::Stat::Accumulator<int> >& InstPreyExeCountsForInstSet(const cString& inst_set) { return m_is_prey_exe_inst_map[inst_set]; }
  Apto::Array<Apto::Stat::Accumulator<int> >& InstPreyFromSensorExeCountsForInstSet(const cString& inst_set) { return m_is_prey_from_sensor_inst_map[inst_set]; }

  Apto::Stat::Accumulator<double>& SumPredFitness()       { return sum_pred_fitness; }
  Apto::Stat::Accumulator<double>& SumPredGestation()     { return sum_pred_gestation; }
  Apto::Stat::Accumulator<double>& SumPredMerit()         { return sum_pred_merit; }
  Apto::Stat::Accumulator<double>& SumPredCreatureAge()   { return sum_pred_creature_age; }
  Apto::Stat::Accumulator<double>& SumPredGeneration()    { return sum_pred_generation; }  
  Apto::Stat::Accumulator<double>& SumPredSize()          { return sum_pred_size; }
  Apto::Array<Apto::Stat::Accumulator<int> >& InstPredExeCountsForInstSet(const cString& inst_set) { return m_is_pred_exe_inst_map[inst_set]; }
  Apto::Array<Apto::Stat::Accumulator<int> >& InstPredFromSensorExeCountsForInstSet(const cString& inst_set) { return m_is_pred_from_sensor_inst_map[inst_set]; }

  Apto::Stat::Accumulator<double>& SumTopPredFitness()       { return sum_tpred_fitness; }
  Apto::Stat::Accumulator<double>& SumTopPredGestation()     { return sum_tpred_gestation; }
  Apto::Stat::Accumulator<double>& SumTopPredMerit()         { return sum_tpred_merit; }
  Apto::Stat::Accumulator<double>& SumTopPredCreatureAge()   { return sum_tpred_creature_age; }
  Apto::Stat::Accumulator<double>& SumTopPredGeneration()    { return sum_tpred_generation; }
  Apto::Stat::Accumulator<double>& SumTopPredSize()          { return sum_tpred_size; }
  Apto::Array<Apto::Stat::Accumulator<int> >& InstTopPredExeCountsForInstSet(const cString& inst_set) { return m_is_tpred_exe_inst_map[inst_set]; }
  Apto::Array<Apto::Stat::Accumulator<int> >&  InstTopPredFromSensorExeCountsForInstSet(const cString& inst_set) { return m_is_tpred_from_sensor_inst_map[inst_set]; }

  void ZeroFTInst();
  
  
 

  // And constant versions of the above...
  const Apto::Stat::Accumulator<double>& SumFitness() const       { return sum_fitness; }
  const Apto::Stat::Accumulator<double>& SumGestation() const     { return sum_gestation; }
  const Apto::Stat::Accumulator<double>& SumMerit() const         { return sum_merit; }
  const Apto::Stat::Accumulator<double>& SumReproRate() const     { return sum_repro_rate; }

  const Apto::Stat::Accumulator<double>& SumCreatureAge() const   { return sum_creature_age; }
  const Apto::Stat::Accumulator<double>& SumGeneration() const    { return sum_generation; }

  const Apto::Stat::Accumulator<double>& SumNeutralMetric() const { return sum_neutral_metric; }
  const Apto::Stat::Accumulator<double>& SumCopyMutRate() const   { return sum_copy_mut_rate; }
  const Apto::Stat::Accumulator<double>& SumLogCopyMutRate() const{ return sum_log_copy_mut_rate; }
  const Apto::Stat::Accumulator<double>& SumDivMutRate() const   { return sum_div_mut_rate; }
  const Apto::Stat::Accumulator<double>& SumLogDivMutRate() const{ return sum_log_div_mut_rate; }

  const Apto::Stat::Accumulator<double>& SumCopySize() const      { return sum_copy_size; }
  const Apto::Stat::Accumulator<double>& SumExeSize() const       { return sum_exe_size; }
  const Apto::Stat::Accumulator<double>& SumMemSize() const       { return sum_mem_size; }


  //pred-prey
  const Apto::Stat::Accumulator<double>& SumPreyFitness() const       { return sum_prey_fitness; }
  const Apto::Stat::Accumulator<double>& SumPreyGestation() const     { return sum_prey_gestation; }
  const Apto::Stat::Accumulator<double>& SumPreyMerit() const         { return sum_prey_merit; }
  const Apto::Stat::Accumulator<double>& SumPreyCreatureAge() const   { return sum_prey_creature_age; }
  const Apto::Stat::Accumulator<double>& SumPreyGeneration() const    { return sum_prey_generation; }  
  const Apto::Stat::Accumulator<double>& SumPreySize() const          { return sum_prey_size; }
  
  const Apto::Stat::Accumulator<double>& SumPredFitness() const       { return sum_pred_fitness; }
  const Apto::Stat::Accumulator<double>& SumPredGestation() const     { return sum_pred_gestation; }
  const Apto::Stat::Accumulator<double>& SumPredMerit() const         { return sum_pred_merit; }
  const Apto::Stat::Accumulator<double>& SumPredCreatureAge() const   { return sum_pred_creature_age; }
  const Apto::Stat::Accumulator<double>& SumPredGeneration() const    { return sum_pred_generation; }  
  const Apto::Stat::Accumulator<double>& SumPredSize() const          { return sum_pred_size; }

  const Apto::Stat::Accumulator<double>& SumTopPredFitness() const       { return sum_tpred_fitness; }
  const Apto::Stat::Accumulator<double>& SumTopPredGestation() const     { return sum_tpred_gestation; }
  const Apto::Stat::Accumulator<double>& SumTopPredMerit() const         { return sum_tpred_merit; }
  const Apto::Stat::Accumulator<double>& SumTopPredCreatureAge() const   { return sum_tpred_creature_age; }
  const Apto::Stat::Accumulator<double>& SumTopPredGeneration() const    { return sum_tpred_generation; }
  const Apto::Stat::Accumulator<double>& SumTopPredSize() const          { return sum_tpred_size; }

  
  void IncResamplings() { ++num_resamplings; }
  void IncFailedResamplings() { ++num_failedResamplings; }

  void RecordBirth(bool breed_true);
  void RecordDeath() { num_deaths++; }

  void IncExecuted() { num_executed++; }

  void AddNumOrgsKilled(long num) { sum_orgs_killed.Add(num); }
	void AddNumUnoccupiedCellAttemptedToKill(long num) { sum_unoccupied_cell_kill_attempts.Add(num); }
  void AddNumCellsScannedAtKill(long num) { sum_cells_scanned_at_kill.Add(num); }
  void IncNumMigrations() { num_migrations++; }

  void AddCurTask(int task_num) { task_cur_count[task_num]++; }

  void AddCurTaskQuality(int task_num, double quality)
  {
	  task_cur_quality[task_num] += quality;
	  if (quality > task_cur_max_quality[task_num]) task_cur_max_quality[task_num] = quality;
  }
  void AddLastTask(int task_num) { task_last_count[task_num]++; }
  void AddTestTask(int task_num) { task_test_count[task_num]++; }
  
  bool ShouldCollectEnvTestStats() const { return m_collect_env_test_stats; }

  void AddLastTaskQuality(int task_num, double quality)
  {
	  task_last_quality[task_num] += quality;
	  if (quality > task_last_max_quality[task_num]) task_last_max_quality[task_num] = quality;
  }
  void AddNewTaskCount(int task_num) {new_task_count[task_num]++; }
  void AddOtherTaskCounts(int task_num, int prev_tasks, int cur_tasks) {
	  prev_task_count[task_num] += prev_tasks;
	  cur_task_count[task_num] += cur_tasks;
  }
  void AddNewReactionCount(int reaction_num) {new_reaction_count[reaction_num]++; }
  void IncTaskExeCount(int task_num, int task_count) { task_exe_count[task_num] += task_count; }
  void ZeroTasks();


  void AddCurReaction(int reaction) { m_reaction_cur_count[reaction]++; }
  void AddLastReaction(int reaction) { m_reaction_last_count[reaction]++; }
  void AddCurReactionAddReward(int reaction, double reward) { m_reaction_cur_add_reward[reaction] += reward; }
  void AddLastReactionAddReward(int reaction, double reward) { m_reaction_last_add_reward[reaction] += reward; }
  void IncReactionExeCount(int reaction, int count) { m_reaction_exe_count[reaction] += count; }
  void ZeroReactions();

  void SetResources(const Apto::Array<double> &_in) { resource_count = _in; }
  void SetResourceGeometries(const Apto::Array<int> &_in) { resource_geometry = _in;}
  void SetSpatialRes(const Apto::Array< Apto::Array<double> > &_in) { spatial_res_count = _in; }

  void SetInstNames(const cString& inst_set, const Apto::Array<cString>& names) { m_is_inst_names_map[inst_set] = names; }
  void SetReactionName(int id, const cString & name) { reaction_names[id] = name; }
  void SetResourceName(int id, const cString & name) { resource_names[id] = name; }


  void AddSpeculative(int spec) { m_spec_total += spec; m_spec_num++; }
  void AddSpeculativeWaste(int waste) { m_spec_waste += waste; }

  // Information retrieval section...

  int GetNumBirths() const          { return num_births; }
  int GetCumulativeBirths() const   { return cumulative_births; }
  int GetNumDeaths() const          { return num_deaths; }
  int GetBreedIn() const            { return num_breed_in; }
  int GetBreedTrue() const          { return num_breed_true; }
  int GetBreedTrueCreatures() const { return num_breed_true_creatures; }
  int GetNumCreatures() const       { return num_creatures; }
  int GetNumNoBirthCreatures() const{ return num_no_birth_creatures; }
  int GetNumSingleThreadCreatures() const { return num_single_thread_creatures; }
  int GetNumMultiThreadCreatures() const { return num_multi_thread_creatures; }
  int GetNumThreads() const { return m_num_threads; }
  int GetNumModified() const { return num_modified;}

  int GetTotCreatures() const       { return tot_organisms; }

  int GetTaskCurCount(int task_num) const { return task_cur_count[task_num]; }
  double GetTaskCurQuality(int task_num) const { return task_cur_quality[task_num]/(double)task_cur_count[task_num]; }

  int GetTaskTestCount(int task_num) const {return task_test_count[task_num];}
  
  int GetTaskLastCount(int task_num) const {return task_last_count[task_num];}
  double GetTaskLastQuality(int task_num) const {return task_last_quality[task_num]/(double)task_last_count[task_num];}

  double GetTaskMaxCurQuality(int task_num) const { return task_cur_max_quality[task_num];}
  double GetTaskMaxLastQuality(int task_num) const { return task_last_max_quality[task_num];}
  int GetTaskExeCount(int task_num) const { return task_exe_count[task_num]; }

  const Apto::Array<int>& GetReactions() const { return m_reaction_last_count; }
  const Apto::Array<double> & GetResources() const { return resource_count; }

  double GetAveReproRate() const  { return sum_repro_rate.Average(); }

  double GetAveMerit() const      { return sum_merit.Average(); }
  double GetAveCreatureAge() const{ return sum_creature_age.Average(); }
  double GetAveMemSize() const    { return sum_mem_size.Average(); }

  double GetAveNeutralMetric() const { return sum_neutral_metric.Average(); }
  double GetAveCopyMutRate() const   { return sum_copy_mut_rate.Mean(); }
  double GetAveLogCopyMutRate() const{ return sum_log_copy_mut_rate.Mean();}
  double GetAveDivMutRate() const   { return sum_div_mut_rate.Mean(); }
  double GetAveLogDivMutRate() const{ return sum_log_div_mut_rate.Mean();}

  double GetAveGestation() const { return sum_gestation.Average(); }
  double GetAveFitness() const   { return sum_fitness.Average(); }

  double GetAveCopySize() const   { return sum_copy_size.Average(); }
  double GetAveExeSize() const    { return sum_exe_size.Average(); }

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

  double GetAveSpeculative() const { return (m_spec_num) ? ((double)m_spec_total / (double)m_spec_num) : 0.0; }
  int GetSpeculativeWaste() const { return m_spec_waste; }

  double GetAvgNumOrgsKilled() const { return sum_orgs_killed.Mean(); }
  double GetAvgNumCellsScannedAtKill() const { return sum_cells_scanned_at_kill.Mean(); }
  int GetNumMigrations() const { return num_migrations; }
  
  // Pred-Prey
  int GetNumPreyCreatures() const;
  int GetNumPredCreatures() const;
  int GetNumTopPredCreatures() const;
  int GetNumTotalPredCreatures() const;
  
  // this value gets recorded when a creature with the particular
  // fitness value gets born. It will never change to a smaller value,
  // i.e., when the maximum fitness in the population drops, this value will
  // still stay up.
  double GetMaxViableFitness() const { return max_viable_fitness; }

  // User-defined datafile...
  void PrintDataFile(const cString& filename, const cString& format, char sep=' ');

  // Public calls to output data files (for events)
  void PrintAverageData(const cString& filename);
  void PrintErrorData(const cString& filename);
  void PrintVarianceData(const cString& filename);
  
  void PrintPreyAverageData(const cString& filename);
  void PrintPredatorAverageData(const cString& filename);
  void PrintTopPredatorAverageData(const cString& filename);
  void PrintPreyErrorData(const cString& filename);
  void PrintPredatorErrorData(const cString& filename);
  void PrintTopPredatorErrorData(const cString& filename);
  void PrintPreyVarianceData(const cString& filename);
  void PrintPredatorVarianceData(const cString& filename);
  void PrintTopPredatorVarianceData(const cString& filename);
  void PrintPreyInstructionData(const cString& filename, const cString& inst_set);
  void PrintPredatorInstructionData(const cString& filename, const cString& inst_set);
  void PrintTopPredatorInstructionData(const cString& filename, const cString& inst_set);
  void PrintPreyFromSensorInstructionData(const cString& filename, const cString& inst_set);
  void PrintPredatorFromSensorInstructionData(const cString& filename, const cString& inst_set);
  void PrintTopPredatorFromSensorInstructionData(const cString& filename, const cString& inst_set);
  void PrintKilledPreyFTData(const cString& filename);
  void PrintBirthLocData(int org_idx);
  void PrintLookData(cString& string);
  void PrintLookDataOutput(cString& string);
  void PrintLookEXDataOutput(cString& string);

  void PrintCountData(const cString& filename);
  void PrintThreadsData(const cString& filename);
  void PrintTotalsData(const cString& filename);
  void PrintTasksData(const cString& filename);
  void PrintSoloTaskSnapshot(const cString& filename, cAvidaContext& ctx);
  void PrintHostTasksData(const cString& filename);
  void PrintTasksExeData(const cString& filename);
  void PrintTasksQualData(const cString& filename);
  void PrintDynamicMaxMinData(const cString& filename);
  void PrintNewTasksData(const cString& filename);
  void PrintNewReactionData(const cString& filename);
  void PrintNewTasksDataPlus(const cString& filename);
  void PrintReactionData(const cString& filename);
  void PrintReactionExeData(const cString& filename);
  void PrintCurrentReactionData(const cString& filename);
  void PrintReactionRewardData(const cString& filename);
  void PrintCurrentReactionRewardData(const cString& filename);
  void PrintResourceData(const cString& filename);
  void PrintResourceLocData(const cString& filename, cAvidaContext& ctx);
  void PrintResWallLocData(const cString& filename, cAvidaContext& ctx);
  void PrintTimeData(const cString& filename);
  void PrintDivideMutData(const cString& filename);
  void PrintMutationRateData(const cString& filename);
  void PrintSenseData(const cString& filename);
  void PrintSenseExeData(const cString& filename);
  void PrintExtendedTimeData(const cString& filename);
  void PrintNumOrgsKilledData(const cString& filename);
  void PrintTargets(const cString& filename);
  void PrintMimicDisplays(const cString& filename);
  void PrintTopPredTargets(const cString& filename);

  void PrintMiniTraceReactions(cOrganism* org);
  void PrintMicroTraces(Apto::Array<char, Apto::Smart>& exec_trace, int birth_update, int org_id, int ft, int gen_id);
  void UpdateTopNavTrace(cOrganism* org, bool force_update = false);
  void SetNavTrace(bool use_first) { firstnavtrace = use_first; }
  void PrintTopNavTrace(bool flush = false);
  void PrintReproData(cOrganism* org);
    

	// -------- Support for organism locations --------
public:
	//! Print organism locations.
	void PrintOrganismLocation(const cString& filename);
  
private:
  // Initialization
  void setupProvidedData();
  
  // Helper Methods
  template <class T> Data::PackagePtr packageData(T (cStats::*)() const) const;
  template <class T, class U> Data::PackagePtr packageArgData(T (cStats::*)(U arg) const, U arg) const;
};


inline void cStats::SetBreedTrueCreatures(int in_num_breed_true_creatures)
{
  num_breed_true_creatures = in_num_breed_true_creatures;
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
