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

#include "tDataManager.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <utility>

using namespace Avida;


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




  // --------  State Variables  ---------
  int last_update;


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


  
  void RecordBirth(bool breed_true);
  void RecordDeath() { num_deaths++; }

  void IncExecuted() { num_executed++; }

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
  

  void PrintCountData(const cString& filename);
  void PrintThreadsData(const cString& filename);
  void PrintTotalsData(const cString& filename);


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
