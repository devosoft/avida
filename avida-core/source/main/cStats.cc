/*
 *  cStats.cc
 *  Avida
 *
 *  Called "stats.cc" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#include "cStats.h"

#include "avida/core/Feedback.h"
#include "avida/core/UniverseDriver.h"
#include "avida/data/Manager.h"
#include "avida/data/Package.h"
#include "avida/data/Util.h"
#include "avida/output/File.h"

#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStringUtil.h"
#include "cWorld.h"
#include "tDataEntry.h"
#include "cReaction.h"
#include "cEventList.h"

#include <algorithm>
#include <cfloat>
#include <numeric>
#include <cmath>
#include <sstream>

using namespace Avida;


cStats::cStats(cWorld* world)
: m_world(world)
, m_data_manager(this, "population_data")
, m_update(-1)
, avida_time(0)
, max_viable_fitness(0)
, max_fitness(0)
, max_merit(0)
, max_gestation_time(0)
, max_genome_length(0)
, min_fitness(FLT_MAX)
, min_merit(FLT_MAX)
, min_gestation_time(INT_MAX)
, min_genome_length(INT_MAX)
, num_births(0)
, cumulative_births(0)
, num_deaths(0)
, num_breed_in(0)
, num_breed_true(0)
, num_breed_true_creatures(0)
, num_creatures(0)
, num_executed(0)
, num_no_birth_creatures(0)
, num_single_thread_creatures(0)
, num_multi_thread_creatures(0)
, m_num_threads(0)
, num_modified(0)
, tot_organisms(0)
, tot_executed(0)
, num_resamplings(0)
, num_failedResamplings(0)
, last_update(0)
, sense_size(0)
, m_spec_total(0)
, m_spec_num(0)
, m_spec_waste(0)
{
  const cEnvironment& env = m_world->GetEnvironment();
  const int num_tasks = env.GetNumTasks();
  
  task_cur_count.Resize(num_tasks);
  task_last_count.Resize(num_tasks);
  task_test_count.Resize(num_tasks);
  m_collect_env_test_stats = false;
  
  task_cur_quality.Resize(num_tasks);
  task_last_quality.Resize(num_tasks);
  task_cur_max_quality.Resize(num_tasks);
  task_last_max_quality.Resize(num_tasks);
  task_exe_count.Resize(num_tasks);
  new_task_count.Resize(num_tasks);
  prev_task_count.Resize(num_tasks);
  cur_task_count.Resize(num_tasks);
  new_reaction_count.Resize(env.GetNumReactions());
  task_cur_count.SetAll(0);
  task_cur_quality.SetAll(0);
  task_cur_max_quality.SetAll(0);
  task_last_max_quality.SetAll(0);
  task_last_quality.SetAll(0);
  task_last_count.SetAll(0);
  task_test_count.SetAll(0);
  task_cur_max_quality.SetAll(0);
  task_last_max_quality.SetAll(0);
  task_exe_count.SetAll(0);
  new_task_count.SetAll(0);
  prev_task_count.SetAll(0);
  cur_task_count.SetAll(0);
  new_reaction_count.SetAll(0);
  
  
  ZeroFTInst();
  
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
  
  task_names.Resize(num_tasks);
  for (int i = 0; i < num_tasks; i++) task_names[i] = env.GetTask(i).GetDesc();
  
  reaction_names.Resize(num_reactions);
  for (int i = 0; i < num_reactions; i++) reaction_names[i] = env.GetReactionName(i);
  
  resource_names.Resize( m_world->GetNumResources() );
  
  
  
  setupProvidedData();
}


Data::ConstDataSetPtr cStats::Provides() const
{
  if (!m_provides) {
    Data::DataSetPtr provides(new Apto::Set<Apto::String>);
    for (Apto::Map<Apto::String, ProvidedData>::KeyIterator it = m_provided_data.Keys(); it.Next();) {
      provides->Insert(*it.Get());
    }
    m_provides = provides;
  }
  return m_provides;
}

void cStats::UpdateProvidedValues(Update)
{
  // Nothing for now, all handled by ProcessUpdate()
}

Data::PackagePtr cStats::GetProvidedValueForArgument(const Apto::String& data_id, const Data::Argument&) const
{
  Data::PackagePtr rtn;
  
  if (Data::IsStandardID(data_id)) {
    ProvidedData data_entry;
    if (data_id.GetSize() > 16 && data_id.Substring(0, 16) == "core.environment") {
      m_collect_env_test_stats = true;
    }
    if (m_provided_data.Get(data_id, data_entry)) {
      rtn = data_entry.GetData();
    }
    assert(rtn);
  } else if (Data::IsArgumentedID(data_id)) {
    // @TODO
    
  }
  
  return rtn;
}

Apto::String cStats::DescribeProvidedValue(const Apto::String& data_id) const
{
  ProvidedData data_entry;
  Apto::String rtn;
  if (m_provided_data.Get(data_id, data_entry)) {
    rtn = data_entry.description;
  }
  assert(rtn != "");
  return rtn;
}


void cStats::SetActiveArguments(const Data::DataID&, Data::ConstArgumentSetPtr)
{
  // @TODO
}


Data::ConstArgumentSetPtr cStats::GetValidArguments(const Data::DataID& data_id) const
{
  Data::ArgumentSetPtr arg_set;
  
  if (Data::IsStandardID(data_id)) return arg_set;
  
  // @TODO
  
  return arg_set;
}

bool cStats::IsValidArgument(const Data::DataID& data_id, Data::Argument) const
{
  if (Data::IsStandardID(data_id)) return false;
  
  // @TODO
  return false;
}


template <class T> Data::PackagePtr cStats::packageData(T (cStats::*func)() const) const
{
  return Data::PackagePtr(new Data::Wrap<T>((this->*func)()));
}

template <class T, class U> Data::PackagePtr cStats::packageArgData(T (cStats::*func)(U) const, U arg) const
{
  return Data::PackagePtr(new Data::Wrap<T>((this->*func)(arg)));
}


void cStats::setupProvidedData()
{
  // Load in all the keywords, descriptions, and associated functions for
  // data management.
  
  // Setup functors and references for use in the PROVIDE macro
  Data::ProviderActivateFunctor activate(m_world, &cWorld::GetStatsProvider);
  Data::ManagerPtr mgr = m_world->GetDataManager();
  Apto::Functor<Data::PackagePtr, Apto::TL::Create<int (cStats::*)() const> > intStat(this, &cStats::packageData<int>);
  Apto::Functor<Data::PackagePtr, Apto::TL::Create<double (cStats::*)() const> > doubleStat(this, &cStats::packageData<double>);
  
  // Define PROVIDE macro to simplify instantiating new provided data
#define PROVIDE(name, desc, type, func) { \
m_provided_data[name] = ProvidedData(desc, Apto::BindFirst(type ## Stat, &cStats::func));\
mgr->Register(name, activate); \
}
  
  // Time Stats
  m_data_manager.Add("update",      "Update",      &cStats::GetUpdate);
  m_data_manager.Add("generation",  "Generation",  &cStats::GetGeneration);
  
  PROVIDE("core.update",                   "Update",                               int,    GetUpdate);
  PROVIDE("core.world.ave_generation",     "Average Generation",                   double, GetGeneration);
  
  
  // Population Level Stats
  m_data_manager.Add("num_resamplings",  "Total Number of resamplings this time step", &cStats::GetResamplings);
  m_data_manager.Add("num_failedResamplings",  "Total Number of divide commands that reached the resampling hard-cap this time step", &cStats::GetFailedResamplings);
  
  // Current Counts...
  m_data_manager.Add("num_births",     "Count of Births in Population",          &cStats::GetNumBirths);
  m_data_manager.Add("cumulative_births", "Total Births over Time",              &cStats::GetCumulativeBirths);
  m_data_manager.Add("num_deaths",     "Count of Deaths in Population",          &cStats::GetNumDeaths);
  m_data_manager.Add("breed_in",       "Count of Non-Breed-True Births",         &cStats::GetBreedIn);
  m_data_manager.Add("breed_true",     "Count of Breed-True Births",             &cStats::GetBreedTrue);
  m_data_manager.Add("bred_true",      "Count of Organisms that have Bred True", &cStats::GetBreedTrueCreatures);
  m_data_manager.Add("num_cpus",       "Count of Organisms in Population",       &cStats::GetNumCreatures);
  m_data_manager.Add("threads",        "Count of Threads in Population",         &cStats::GetNumThreads);
  m_data_manager.Add("num_no_birth",   "Count of Childless Organisms",           &cStats::GetNumNoBirthCreatures);
  
  PROVIDE("core.world.organisms",          "Count of Organisms in the World",      int,    GetNumCreatures);
  
  
  // Total Counts...
  m_data_manager.Add("tot_cpus",      "Total Organisms ever in Population", &cStats::GetTotCreatures);
  
  
  // Some Average Data...
  m_data_manager.Add("ave_repro_rate", "Average Repro-Rate (1/Gestation)", &cStats::GetAveReproRate);
  m_data_manager.Add("ave_merit",      "Average Merit",                    &cStats::GetAveMerit);
  m_data_manager.Add("ave_age",        "Average Age",                      &cStats::GetAveCreatureAge);
  m_data_manager.Add("ave_memory",     "Average Memory Used",              &cStats::GetAveMemSize);
  m_data_manager.Add("ave_neutral",    "Average Neutral Metric",           &cStats::GetAveNeutralMetric);
  m_data_manager.Add("ave_gest",       "Average Gestation Time",           &cStats::GetAveGestation);
  m_data_manager.Add("ave_fitness",    "Average Fitness",                  &cStats::GetAveFitness);
  m_data_manager.Add("ave_copy_length","Average Copied Length",            &cStats::GetAveCopySize);
  m_data_manager.Add("ave_exe_length", "Average Executed Length",          &cStats::GetAveExeSize);
  
  m_data_manager.Add("ave_speculative","Averate Speculative Instructions", &cStats::GetAveSpeculative);
  m_data_manager.Add("speculative_waste", "Speculative Execution Waste",   &cStats::GetSpeculativeWaste);
  
  PROVIDE("core.world.ave_metabolic_rate", "Average Metabolic Rate",               double, GetAveMerit);
  PROVIDE("core.world.ave_age",            "Average Organism Age (in updates)",    double, GetAveCreatureAge);
  PROVIDE("core.world.ave_gestation_time", "Average Gestation Time",               double, GetAveGestation);
  PROVIDE("core.world.ave_fitness",        "Average Fitness",                      double, GetAveFitness);
  
  
  // Maximums
  m_data_manager.Add("max_fitness", "Maximum Fitness in Population", &cStats::GetMaxFitness);
  m_data_manager.Add("max_merit",   "Maximum Merit in Population",   &cStats::GetMaxMerit);
  
  
  // Minimums
  m_data_manager.Add("min_fitness", "Minimum Fitness in Population", &cStats::GetMinFitness);
  
  
  
  const cEnvironment& env = m_world->GetEnvironment();
  Apto::Functor<Data::PackagePtr, Apto::TL::Create<int> > taskLastCount(
                                                                        Apto::BindFirst(
                                                                                        Apto::Functor<Data::PackagePtr, Apto::TL::Create<int (cStats::*)(int) const, int> >(
                                                                                                                                                                            this, &cStats::packageArgData<int, int>
                                                                                                                                                                            ),
                                                                                        &cStats::GetTaskTestCount
                                                                                        )
                                                                        );
  for(int i = 0; i < task_names.GetSize(); i++) {
    Apto::String task_id(Apto::FormatStr("core.environment.triggers.%s.test_organisms", (const char*)env.GetTask(i).GetName()));
    Apto::String task_desc(task_names[i]);
    
    m_provided_data[task_id] = ProvidedData(task_desc, Apto::BindFirst(taskLastCount, i));
    mgr->Register(task_id, activate);
	}
  
  
#undef PROVIDE
}

void cStats::ZeroTasks()
{
  task_cur_count.SetAll(0);
  task_last_count.SetAll(0);
  task_test_count.SetAll(0);
  
  task_cur_quality.SetAll(0);
  task_last_quality.SetAll(0);
  task_last_max_quality.SetAll(0);
  task_cur_max_quality.SetAll(0);
}

void cStats::ZeroReactions()
{
  m_reaction_cur_count.SetAll(0);
  m_reaction_last_count.SetAll(0);
  m_reaction_cur_add_reward.SetAll(0);
  m_reaction_last_add_reward.SetAll(0);
}

void cStats::ZeroFTInst()
{
  for (Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > >::ValueIterator it = m_is_prey_exe_inst_map.Values(); it.Next();) {
    for (int i = 0; i < (*it.Get()).GetSize(); i++) (*it.Get())[i].Clear();
  }
  for (Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > >::ValueIterator it = m_is_pred_exe_inst_map.Values(); it.Next();) {
    for (int i = 0; i < (*it.Get()).GetSize(); i++) (*it.Get())[i].Clear();
  }
  for (Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > >::ValueIterator it = m_is_pred_fail_exe_inst_map.Values(); it.Next();) {
    for (int i = 0; i < (*it.Get()).GetSize(); i++) (*it.Get())[i].Clear();
  }
  for (Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > >::ValueIterator it = m_is_tpred_exe_inst_map.Values(); it.Next();) {
    for (int i = 0; i < (*it.Get()).GetSize(); i++) (*it.Get())[i].Clear();
  }
  for (Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > >::ValueIterator it = m_is_prey_from_sensor_inst_map.Values(); it.Next();) {
    for (int i = 0; i < (*it.Get()).GetSize(); i++) (*it.Get())[i].Clear();
  }
  for (Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > >::ValueIterator it = m_is_pred_from_sensor_inst_map.Values(); it.Next();) {
    for (int i = 0; i < (*it.Get()).GetSize(); i++) (*it.Get())[i].Clear();
  }
  for (Apto::Map<cString, Apto::Array<Apto::Stat::Accumulator<int> > >::ValueIterator it = m_is_tpred_from_sensor_inst_map.Values(); it.Next();) {
    for (int i = 0; i < (*it.Get()).GetSize(); i++) (*it.Get())[i].Clear();
  }
}


void cStats::RecordBirth(bool breed_true)
{
	if (m_world->GetEventsList()->CheckBirthInterruptQueue(tot_organisms) == true)
		m_world->GetEventsList()->ProcessInterrupt(m_world->GetDefaultContext());
  
  tot_organisms++;
  num_births++;
  cumulative_births++;
  
  if (breed_true) num_breed_true++;
  else num_breed_in++;
}

void cStats::ProcessUpdate()
{
  // Increment the "avida_time"
  if (sum_merit.Count() > 0 && sum_merit.Average() > 0) {
    double delta = ((double)(m_update-last_update))/sum_merit.Average();
    avida_time += delta;
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
  task_test_count.SetAll(0);
  task_cur_quality.SetAll(0);
  task_last_quality.SetAll(0);
  task_cur_max_quality.SetAll(0);
  task_last_max_quality.SetAll(0);
  task_exe_count.SetAll(0);
  
  sense_last_count.SetAll(0);
  sense_last_exe_count.SetAll(0);
  
  m_reaction_cur_count.SetAll(0);
  m_reaction_last_count.SetAll(0);
  m_reaction_cur_add_reward.SetAll(0.0);
  m_reaction_last_add_reward.SetAll(0.0);
  m_reaction_exe_count.SetAll(0);
  
  max_fitness = 0.0;
  
  num_resamplings = 0;
  num_failedResamplings = 0;
  
  m_spec_total = 0;
  m_spec_num = 0;
  m_spec_waste = 0;
}

int cStats::GetNumPreyCreatures() const
{
  return m_world->GetPopulation().GetNumPreyOrganisms();
}

int cStats::GetNumPredCreatures() const
{
  return m_world->GetPopulation().GetNumPredOrganisms();
}

int cStats::GetNumTopPredCreatures() const
{
  return m_world->GetPopulation().GetNumTopPredOrganisms();
}

int cStats::GetNumTotalPredCreatures() const
{
  return m_world->GetPopulation().GetNumTopPredOrganisms() + m_world->GetPopulation().GetNumPredOrganisms();
}

void cStats::PrintDataFile(const cString& filename, const cString& format, char sep)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  m_data_manager.PrintRow(*df, format, sep);
}


void cStats::PrintAverageData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida Average Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                      "Update");
  df->Write(sum_merit.Average(),           "Merit");
  df->Write(sum_gestation.Average(),       "Gestation Time");
  df->Write(sum_fitness.Average(),         "Fitness");
  df->Write(sum_repro_rate.Average(),      "Repro Rate?");
  df->Write(0,                             "(deprecated) Size");
  df->Write(sum_copy_size.Average(),       "Copied Size");
  df->Write(sum_exe_size.Average(),        "Executed Size");
  df->Write(0,                             "(deprecated) Abundance");
  
  // The following causes births and breed true to default to 0.0 when num_creatures is 0
  double ave_births = 0.0;
  double ave_breed_true = 0.0;
  if (num_creatures > 0) {
    const double d_num_creatures = static_cast<double>(num_creatures);
    ave_births = static_cast<double>(num_births) / d_num_creatures;
    ave_breed_true = static_cast<double>(num_breed_true) / d_num_creatures;
  }
  df->Write(ave_births,                    "Proportion of organisms that gave birth in this update");
  df->Write(ave_breed_true,                "Proportion of Breed True Organisms");
  
  df->Write(0,                             "(deprecated) Genotype Depth");
  df->Write(sum_generation.Average(),      "Generation");
  df->Write(sum_neutral_metric.Average(),  "Neutral Metric");
  df->Write(0, "(deprecated) True Replication Rate (based on births/update, time-averaged)");
  df->Endl();
}


void cStats::PrintErrorData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida Standard Error Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                 "Update");
  df->Write(sum_merit.StdError(),           "Merit");
  df->Write(sum_gestation.StdError(),       "Gestation Time");
  df->Write(sum_fitness.StdError(),         "Fitness");
  df->Write(sum_repro_rate.StdError(),      "Repro Rate?");
  df->Write(0,                              "Size");
  df->Write(sum_copy_size.StdError(),       "Copied Size");
  df->Write(sum_exe_size.StdError(),        "Executed Size");
  df->Write(0,                              "(deprecated) Abundance");
  df->Write(-1,                             "(deprecated)");
  df->Write(-1,                             "(deprecated)");
  df->Write(0,                              "(deprecated) Genotype Depth");
  df->Write(sum_generation.StdError(),      "Generation");
  df->Write(sum_neutral_metric.StdError(),  "Neutral Metric");
  df->Write(0, "(deprecated) True Replication Rate (based on births/update, time-averaged)");
  df->Endl();
}


void cStats::PrintVarianceData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida Variance Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                 "Update");
  df->Write(sum_merit.Variance(),           "Merit");
  df->Write(sum_gestation.Variance(),       "Gestation Time");
  df->Write(sum_fitness.Variance(),         "Fitness");
  df->Write(sum_repro_rate.Variance(),      "Repro Rate?");
  df->Write(0,                              "(deprecated) Size");
  df->Write(sum_copy_size.Variance(),       "Copied Size");
  df->Write(sum_exe_size.Variance(),        "Executed Size");
  df->Write(0,                              "(deprecated) Abundance");
  df->Write(-1,                             "(deprecated)");
  df->Write(-1,                             "(deprecated)");
  df->Write(0,                              "(deprecated) Genotype Depth");
  df->Write(sum_generation.Variance(),      "Generation");
  df->Write(sum_neutral_metric.Variance(),  "Neutral Metric");
  df->Write(0, "(deprecated) True Replication Rate (based on births/update, time-averaged)");
  df->Endl();
}



void cStats::PrintCountData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida count data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                "update");
  df->Write(num_executed,            "number of insts executed this update");
  df->Write(num_creatures,           "number of organisms");
  df->Write(0,                       "(deprecated) number of different genotypes");
  df->Write(0,                       "(deprecated) number of different threshold genotypes");
  df->Write(0,                       "(deprecated) number of different species");
  df->Write(0,                       "(deprecated) number of different threshold species");
  df->Write(0,                       "(deprecated) number of different lineages");
  df->Write(num_births,              "number of births in this update");
  df->Write(num_deaths,              "number of deaths in this update");
  df->Write(num_breed_true,          "number of breed true");
  df->Write(num_breed_true_creatures, "number of breed true organisms?");
  df->Write(num_no_birth_creatures,   "number of no-birth organisms");
  df->Write(num_single_thread_creatures, "number of single-threaded organisms");
  df->Write(num_multi_thread_creatures, "number of multi-threaded organisms");
  df->Write(num_modified, "number of modified organisms");
  df->Endl();
}


void cStats::PrintTotalsData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  df->Write(m_update, "Update");
  df->Write((tot_executed+num_executed), "Total Instructions Executed");
  df->Write(num_executed, "Instructions Executed This Update");
  df->Write(tot_organisms, "Total Organisms");
  df->Write(0, "(deprecated) Total Genotypes");
  df->Write(0, "(deprecated) Total Threshold");
  df->Write(0, "(deprecated) Total Species");
  df->Write(0, "(deprecated) Total Lineages");
  df->Endl();
}

void cStats::PrintThreadsData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  df->Write(m_update, "Update");
  df->Write(tot_organisms, "Total Organisms");
  df->Write(m_world->GetPopulation().GetLiveOrgList().GetSize(), "Total Living Organisms");
  df->Write(m_num_threads, "Total Living Org Threads");
  df->Endl();
}



void cStats::PrintTimeData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida time data");
  df->WriteTimeStamp();
  
  df->Write(m_update,              "update");
  df->Write(avida_time,               "avida time");
  df->Write(sum_generation.Average(), "average generation");
  df->Write(num_executed,             "num_executed?");
	df->Write(tot_organisms, "num_organisms");
  df->Endl();
}



void cStats::PrintMutationRateData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida copy mutation rate data");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  df->Write(sum_copy_mut_rate.Mean(), "Average copy mutation rate");
  df->Write(sum_copy_mut_rate.Variance(), "Variance in copy mutation rate");
  
  df->Write(sum_log_copy_mut_rate.Mean(), "Average log(copy mutation rate)");
  df->Write(sum_log_copy_mut_rate.Variance(), "Variance in log(copy mutation rate)");
  df->Endl();
  
}


void cStats::PrintDivideMutData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida divide mutation rate data");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  df->Write(sum_div_mut_rate.Mean(), "Average divide mutation rate");
  df->Write(sum_div_mut_rate.Variance(), "Variance in divide mutation rate");
  
  df->Write(sum_log_div_mut_rate.Mean(), "Average log(divide mutation rate)");
  df->Write(sum_log_div_mut_rate.Variance(), "Variance in log(divide mutation rate)");
  df->Endl();
}
