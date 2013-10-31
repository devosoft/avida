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
, num_parasites(0)
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
, prey_entropy(0.0)
, pred_entropy(0.0)
, topreac(-1)
, topcycle(-1)
, firstnavtrace(false)
{
  const cEnvironment& env = m_world->GetEnvironment();
  const int num_tasks = env.GetNumTasks();
  
  task_cur_count.Resize(num_tasks);
  task_last_count.Resize(num_tasks);
  task_test_count.Resize(num_tasks);
  m_collect_env_test_stats = false;
  
  tasks_host_current.Resize(num_tasks);
  tasks_host_last.Resize(num_tasks);
  tasks_parasite_current.Resize(num_tasks);
  tasks_parasite_last.Resize(num_tasks);
  
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
  m_data_manager.Add("num_parasites",  "Count of Parasites in Population",       &cStats::GetNumParasites);
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
  
  tasks_host_current.SetAll(0);
  tasks_host_last.SetAll(0);
  tasks_parasite_current.SetAll(0);
  tasks_parasite_last.SetAll(0);
  
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


void cStats::PrintParasiteData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida Dominant Parasite Data");
  df->WriteTimeStamp();
  df->Write(m_update, "Update");
  df->Write(num_parasites, "Number of Extant Parasites");
  df->Endl();
}

void cStats::PrintPreyAverageData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Prey Average Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                          "Update");
  df->Write(sum_prey_fitness.Average(),        "Fitness");
  df->Write(sum_prey_gestation.Average(),      "Gestation Time");
  df->Write(sum_prey_merit.Average(),          "Merit");
  df->Write(sum_prey_creature_age.Average(),   "Creature Age");
  df->Write(sum_prey_generation.Average(),     "Generation");
  df->Write(sum_prey_size.Average(),           "Genome Length");
  df->Write(prey_entropy,                      "Total Prey Genotypic Entropy");
  
  df->Endl();
}

void cStats::PrintPredatorAverageData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Predator Average Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                          "Update");
  df->Write(sum_pred_fitness.Average(),        "Fitness");
  df->Write(sum_pred_gestation.Average(),      "Gestation Time");
  df->Write(sum_pred_merit.Average(),          "Merit");
  df->Write(sum_pred_creature_age.Average(),   "Creature Age");
  df->Write(sum_pred_generation.Average(),     "Generation");
  df->Write(sum_pred_size.Average(),           "Genome Length");
  df->Write(pred_entropy,                      "Total Predator Genotypic Entropy");
  
  df->Endl();
}

void cStats::PrintTopPredatorAverageData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Top predator Average Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                          "Update");
  df->Write(sum_tpred_fitness.Average(),        "Fitness");
  df->Write(sum_tpred_gestation.Average(),      "Gestation Time");
  df->Write(sum_tpred_merit.Average(),          "Merit");
  df->Write(sum_tpred_creature_age.Average(),   "Creature Age");
  df->Write(sum_tpred_generation.Average(),     "Generation");
  df->Write(sum_tpred_size.Average(),           "Genome Length");
  df->Write(tpred_entropy,                      "Total Top Predator Genotypic Entropy");
  
  df->Endl();
}

void cStats::PrintPreyErrorData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Prey Standard Error Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                            "Update");
  df->Write(sum_prey_fitness.StdError(),         "Fitness");
  df->Write(sum_prey_gestation.StdError(),       "Gestation Time");
  df->Write(sum_prey_merit.StdError(),           "Merit");
  df->Write(sum_prey_creature_age.StdError(),    "Creature Age");
  df->Write(sum_prey_generation.StdError(),      "Generation");
  df->Write(sum_prey_size.StdError(),            "Genome Length");
  
  df->Endl();
}

void cStats::PrintPredatorErrorData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Predator Standard Error Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                            "Update");
  df->Write(sum_pred_fitness.StdError(),         "Fitness");
  df->Write(sum_pred_gestation.StdError(),       "Gestation Time");
  df->Write(sum_pred_merit.StdError(),           "Merit");
  df->Write(sum_pred_creature_age.StdError(),    "Creature Age");
  df->Write(sum_pred_generation.StdError(),      "Generation");
  df->Write(sum_pred_size.StdError(),            "Genome Length");
  
  df->Endl();
}

void cStats::PrintTopPredatorErrorData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Top predator Standard Error Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                            "Update");
  df->Write(sum_tpred_fitness.StdError(),         "Fitness");
  df->Write(sum_tpred_gestation.StdError(),       "Gestation Time");
  df->Write(sum_tpred_merit.StdError(),           "Merit");
  df->Write(sum_tpred_creature_age.StdError(),    "Creature Age");
  df->Write(sum_tpred_generation.StdError(),      "Generation");
  df->Write(sum_tpred_size.StdError(),            "Genome Length");
  
  df->Endl();
}

void cStats::PrintPreyVarianceData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Prey Variance Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                            "Update");
  df->Write(sum_prey_fitness.Variance(),         "Fitness");
  df->Write(sum_prey_gestation.Variance(),       "Gestation Time");
  df->Write(sum_prey_merit.Variance(),           "Merit");
  df->Write(sum_prey_creature_age.Variance(),    "Creature Age");
  df->Write(sum_prey_generation.Variance(),      "Generation");
  df->Write(sum_prey_size.Variance(),            "Genome Length");
  
  df->Endl();
}

void cStats::PrintPredatorVarianceData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Predator Variance Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                            "Update");
  df->Write(sum_pred_fitness.Variance(),         "Fitness");
  df->Write(sum_pred_gestation.Variance(),       "Gestation Time");
  df->Write(sum_pred_merit.Variance(),           "Merit");
  df->Write(sum_pred_creature_age.Variance(),    "Creature Age");
  df->Write(sum_pred_generation.Variance(),      "Generation");
  df->Write(sum_pred_size.Variance(),            "Genome Length");
  
  df->Endl();
}

void cStats::PrintTopPredatorVarianceData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Top predator Variance Data");
  df->WriteTimeStamp();
  
  df->Write(m_update,                            "Update");
  df->Write(sum_tpred_fitness.Variance(),         "Fitness");
  df->Write(sum_tpred_gestation.Variance(),       "Gestation Time");
  df->Write(sum_tpred_merit.Variance(),           "Merit");
  df->Write(sum_tpred_creature_age.Variance(),    "Creature Age");
  df->Write(sum_tpred_generation.Variance(),      "Generation");
  df->Write(sum_tpred_size.Variance(),            "Genome Length");
  
  df->Endl();
}

void cStats::PrintPreyInstructionData(const cString& filename, const cString& inst_set)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Prey org instruction execution data");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  for (int i = 0; i < m_is_prey_exe_inst_map[inst_set].GetSize(); i++) {
    df->Write(m_is_prey_exe_inst_map[inst_set][i].Sum(), m_is_inst_names_map[inst_set][i]);
  }
  df->Endl();
}

void cStats::PrintPredatorInstructionData(const cString& filename, const cString& inst_set)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Predator org instruction execution data");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  for (int i = 0; i < m_is_pred_exe_inst_map[inst_set].GetSize(); i++) {
    df->Write(m_is_pred_exe_inst_map[inst_set][i].Sum(), m_is_inst_names_map[inst_set][i]);
  }
  df->Endl();
}

void cStats::PrintTopPredatorInstructionData(const cString& filename, const cString& inst_set)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Top predator org instruction execution data");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  for (int i = 0; i < m_is_tpred_exe_inst_map[inst_set].GetSize(); i++) {
    df->Write(m_is_tpred_exe_inst_map[inst_set][i].Sum(), m_is_inst_names_map[inst_set][i]);
  }
  df->Endl();
}

void cStats::PrintPreyFromSensorInstructionData(const cString& filename, const cString& inst_set)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Prey org instruction execution data using values originating from sensory input");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  for (int i = 0; i < m_is_prey_from_sensor_inst_map[inst_set].GetSize(); i++) {
    df->Write(m_is_prey_from_sensor_inst_map[inst_set][i].Sum(), m_is_inst_names_map[inst_set][i]);
  }
  df->Endl();
}

void cStats::PrintPredatorFromSensorInstructionData(const cString& filename, const cString& inst_set)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Predator org instruction execution data using values originating from sensory input");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  for (int i = 0; i < m_is_pred_from_sensor_inst_map[inst_set].GetSize(); i++) {
    df->Write(m_is_pred_from_sensor_inst_map[inst_set][i].Sum(), m_is_inst_names_map[inst_set][i]);
  }
  df->Endl();
}

void cStats::PrintTopPredatorFromSensorInstructionData(const cString& filename, const cString& inst_set)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Top predator org instruction execution data using values originating from sensory input");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  for (int i = 0; i < m_is_tpred_from_sensor_inst_map[inst_set].GetSize(); i++) {
    df->Write(m_is_tpred_from_sensor_inst_map[inst_set][i].Sum(), m_is_inst_names_map[inst_set][i]);
  }
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
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)file);
	df->WriteComment("Avida tasks data");
	df->WriteTimeStamp();
	df->WriteComment("First column gives the current update, next columns give the number");
	df->WriteComment("of organisms that have the particular task as a component of their merit");
  
	df->Write(m_update,   "Update");
	for(int i = 0; i < task_last_count.GetSize(); i++) {
		df->Write(task_last_count[i], task_names[i] );
	}
	df->Endl();
}

void cStats::PrintSoloTaskSnapshot(const cString& filename, cAvidaContext& ctx)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Pop potential tasks (test cpu).");
	df->WriteTimeStamp();
	df->WriteComment("First set of columns gives number of organisms that can perform that particular reaction at least once. ");
	df->WriteComment("Second set of columns gives total number of times that reaction can be performed. ");
	df->WriteComment("Third set of columns gives number of organisms that can perform that particular task at least once. ");
	df->WriteComment("Fourth set of columns gives total number of times that task can be performed. ");
  df->WriteComment("Orgs are tested for each resource in the environment with res level of 1 for the current test resource and levels of 0 for other resources.");
	df->Write(m_update,   "Update");
	df->Write(sum_generation.Average(),      "Generation");
  
  Apto::Array<int> reac_list;
  reac_list.Resize(m_world->GetEnvironment().GetNumTasks());
  reac_list.SetAll(0);
  
  Apto::Array<int> total_reacs;
  total_reacs.Resize(m_world->GetEnvironment().GetNumTasks());
  total_reacs.SetAll(0);
  
  Apto::Array<int> task_list;
  task_list.Resize(m_world->GetEnvironment().GetNumTasks());
  task_list.SetAll(0);
  
  Apto::Array<int> totals_list;
  totals_list.Resize(m_world->GetEnvironment().GetNumTasks());
  totals_list.SetAll(0);
  
  const Apto::Array <cOrganism*, Apto::Smart> pop = m_world->GetPopulation().GetLiveOrgList();
	df->Write(pop.GetSize(),   "PopSize");
  
  for (int i = 0; i < pop.GetSize(); i++) {
    cOrganism* organism = pop[i];
    
    // create a test-cpu for the current creature and then test the creature using res level of 1 for each resource in the environment file
    const cResourceLib& resLib = m_world->GetEnvironment().GetResourceLib();
    for (int k = 0; k < resLib.GetSize(); k++) {
      cCPUTestInfo test_info;
      cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
      testcpu->SetSoloRes(k, 1.0);
      testcpu->TestGenome(ctx, test_info, organism->GetGenome());
      cPhenotype& test_phenotype = test_info.GetTestPhenotype();
      
      for (int j = 0; j < task_list.GetSize(); j++) {
        // inc once if the task was ever performed
        task_list[j] += ( test_phenotype.GetLastTaskCount()[j] == 0 ) ? 0 : 1;
        // for totals, inc by actual number of times the task was performed
        totals_list[j] += test_phenotype.GetLastTaskCount()[j];
        // inc once if the reaction was ever performed
        reac_list[j] += ( test_phenotype.GetLastReactionCount()[j] == 0 ) ? 0 : 1;
        // for totals, inc by actual number of times the reaction was performed
        total_reacs[j] += test_phenotype.GetLastReactionCount()[j];
      }
      delete testcpu;
    }
  }
  for(int j = 0; j < reac_list.GetSize(); j++) {
    df->Write(reac_list[j], task_names[j] );
  }
  for(int j = 0; j < total_reacs.GetSize(); j++) {
    Apto::String tot_str(Apto::FormatStr("%s_reac_totals", (const char*)task_names[j]));
    df->Write(total_reacs[j], tot_str);
  }
  for(int j = 0; j < task_list.GetSize(); j++) {
    df->Write(task_list[j], task_names[j] );
  }
  for(int j = 0; j < task_list.GetSize(); j++) {
    Apto::String tot_str(Apto::FormatStr("%s_task_totals", (const char*)task_names[j]));
    df->Write(totals_list[j], tot_str);
  }
  df->Write(sum_generation.Average(),      "Generation");
  df->Endl();    
};

void cStats::PrintHostTasksData(const cString& filename)
{
	cString file = filename;
  
	// flag to print both tasks.dat and taskquality.dat
	if (filename == "tasksq.dat")
	{
		file = "host_tasks.dat";
	}
  
	// print tasks.dat
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)file);
	df->WriteComment("Avida Host Tasks data");
	df->WriteTimeStamp();
	df->WriteComment("First column gives the current update, next columns give the number");
	df->WriteComment("of Hosts that have the particular task");
  
	df->Write(m_update,   "Update");
	for(int i = 0; i < tasks_host_last.GetSize(); i++) {
		df->Write(tasks_host_last[i], task_names[i] );
	}
	df->Endl();
}

void cStats::PrintParasiteTasksData(const cString& filename)
{
	cString file = filename;
  
	// flag to print both tasks.dat and taskquality.dat
	if (filename == "tasksq.dat")
	{
		file = "parasite_tasks.dat";
	}
  
	// print tasks.dat
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)file);
	df->WriteComment("Avida tasks data");
	df->WriteTimeStamp();
	df->WriteComment("First column gives the current update, next columns give the number");
	df->WriteComment("of Parasites that have the particular task");
  
	df->Write(m_update,   "Update");
	for(int i = 0; i < tasks_parasite_last.GetSize(); i++) {
		df->Write(tasks_parasite_last[i], task_names[i] );
	}
	df->Endl();
}


void cStats::PrintTasksExeData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida tasks execution data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the number");
  df->WriteComment("of times the particular task has been executed this update.");
  
  df->Write(m_update,   "Update");
  for (int i = 0; i < task_exe_count.GetSize(); i++) {
    df->Write(task_exe_count[i], task_names[i] );
  }
  df->Endl();
}

void cStats::PrintTasksQualData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida tasks quality data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, rest give average and max task quality");
  df->Write(m_update, "Update");
  for(int i = 0; i < task_last_count.GetSize(); i++) {
    double qual = 0.0;
    if (task_last_count[i] > 0)
      qual = task_last_quality[i] / static_cast<double>(task_last_count[i]);
    df->Write(qual, task_names[i] + " Average");
    df->Write(task_last_max_quality[i], task_names[i] + " Max");
  }
  df->Endl();
}

void cStats::PrintNewTasksData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida new tasks data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the number");
  df->WriteComment("of times the particular task has newly evolved since the last time printed.");
  
  df->Write(m_update,   "Update");
  for (int i = 0; i < new_task_count.GetSize(); i++) {
    df->Write(new_task_count[i], task_names[i]);
  }
  df->Endl();
  new_task_count.SetAll(0);
}

void cStats::PrintNewTasksDataPlus(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida new tasks data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns are in sets of 3, giving the number");
  df->WriteComment("of times the particular task has newly evolved since the last time printed, then the average");
  df->WriteComment("number of tasks the parent of the organism evolving the new task performed, then the average");
  df->WriteComment("number of tasks the organism evolving the new task performed.  One set of 3 for each task");
  
  df->Write(m_update,   "Update");
  for (int i = 0; i < new_task_count.GetSize(); i++) {
    df->Write(new_task_count[i], task_names[i] + " - num times newly evolved");
    double prev_ave = -1;
    double cur_ave = -1;
    if (new_task_count[i]>0) {
      prev_ave = prev_task_count[i]/double(new_task_count[i]);
      cur_ave = cur_task_count[i]/double(new_task_count[i]);
    }
    df->Write(prev_ave, "ave num tasks parent performed");
    df->Write(cur_ave, "ave num tasks cur org performed");
    
  }
  df->Endl();
  new_task_count.SetAll(0);
  prev_task_count.SetAll(0);
  cur_task_count.SetAll(0);
}

void cStats::PrintNewReactionData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida new reactions data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the number");
  df->WriteComment("of times the particular reaction has newly evolved since the last time printed.");
  
  df->Write(m_update,   "Update");
  for (int i = 0; i < new_reaction_count.GetSize(); i++) {
    df->Write(new_reaction_count[i], reaction_names[i]);
  }
  df->Endl();
  new_reaction_count.SetAll(0);
}

void cStats::PrintDynamicMaxMinData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
	df->WriteComment("Avida dynamic max min data");
	df->WriteTimeStamp();
	df->WriteComment("First column gives the current update, 2nd and 3rd give max and min Fx");
	df->Write(m_update, "Update");
	for(int i = 0; i < task_last_count.GetSize(); i++) {
		double max = m_world->GetEnvironment().GetTask(i).GetArguments().Double(1);
		double min = m_world->GetEnvironment().GetTask(i).GetArguments().Double(2);
		df->Write(max, task_names[i] + " Max");
		df->Write(min, task_names[i] + " Min");
	}
	df->Endl();
}

void cStats::PrintReactionData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida reaction data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the number");
  df->WriteComment("of currently living organisms each reaction has affected.");
  
	df->Write(m_update,   "Update");
	for(int i = 0; i < m_reaction_last_count.GetSize(); i++) {
		df->Write(m_reaction_last_count[i], reaction_names[i]);
	}
	df->Endl();
}

void cStats::PrintCurrentReactionData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida reaction data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the number");
  df->WriteComment("of currently living organisms each reaction has affected.");
  
	df->Write(m_update,   "Update");
	for(int i = 0; i < m_reaction_cur_count.GetSize(); i++) {
		df->Write(m_reaction_cur_count[i], reaction_names[i]);
	}
	df->Endl();
}


void cStats::PrintReactionRewardData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida reaction data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the add bonus reward");
  df->WriteComment("currently living organisms have garnered from each reaction.");
  
  df->Write(m_update,   "Update");
  for (int i = 0; i < m_reaction_last_add_reward.GetSize(); i++) {
    df->Write(m_reaction_last_add_reward[i], reaction_names[i]);
  }
  df->Endl();
}


void cStats::PrintCurrentReactionRewardData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida reaction data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the add bonus reward");
  df->WriteComment("currently living organisms have garnered from each reaction.");
  
  df->Write(m_update,   "Update");
  for (int i = 0; i < m_reaction_cur_add_reward.GetSize(); i++) {
    df->Write(m_reaction_cur_add_reward[i], reaction_names[i]);
  }
  df->Endl();
}


void cStats::PrintReactionExeData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida reaction execution data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the number");
  df->WriteComment("of times the particular reaction has been triggered this update.");
  
  df->Write(m_update,   "Update");
  for (int i = 0; i < m_reaction_exe_count.GetSize(); i++) {
    df->Write(m_reaction_exe_count[i], reaction_names[i]);
  }
  df->Endl();
}


void cStats::PrintResourceData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida resource data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the quantity");
  df->WriteComment("of the particular resource at that update.");
  
  df->Write(m_update,   "Update");
  
  for (int i = 0; i < resource_count.GetSize(); i++) {
    df->Write(resource_count[i], resource_names[i] );
  }
  df->Endl();
}

void cStats::PrintResourceLocData(const cString& filename, cAvidaContext& ctx)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida resource location data");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, all further columns give the cell id");
  df->WriteComment("for center of gradient resources at that update.");
  
  df->Write(m_update,   "Update");
  
  const cResourceDefLib& resLib = m_world->GetEnvironment().GetResDefLib();
  for (int i = 0; i < resLib.GetSize(); i++) {
    if (resLib.GetResDef(i)->IsDynamic()) {
      df->Write(m_world->GetPopulation().GetResources().GetCurrPeakX(ctx, i) + (m_world->GetPopulation().GetResources().GetCurrPeakY(ctx, i) * m_world->GetConfig().WORLD_X.Get()), "CellID");
    }
  }
  df->Endl();
}

void cStats::PrintResWallLocData(const cString& filename, cAvidaContext& ctx)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  if (!df->HeaderDone()) {
    df->WriteComment("Avida wall resource filled cells data");
    df->WriteTimeStamp();
    df->WriteComment("First column gives the current update, all further columns give filled cell ids for each wall res");
    df->Endl();
  }
  
  std::ofstream& fp = df->OFStream();
  fp << m_update << " ";

  const cResourceDefLib& resLib = m_world->GetEnvironment().GetResDefLib();
  for (int i = 0; i < resLib.GetSize(); i++) {
    if (resLib.GetResDef(i)->IsDynamic() && resLib.GetResDef(i)->GetHabitat() == 2) {
      Apto::Array<int>& cells = *(m_world->GetPopulation().GetResources().GetWallCells(i));
      for (int i = 0; i < cells.GetSize() - 1; i++) {
        fp << cells[i] << ",";
      }
      fp << cells[cells.GetSize() - 1] << " ";
    }
  }
  fp << endl;
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
  df->Endl();
}


//@MRR Add additional time information
void cStats::PrintExtendedTimeData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
	df->WriteTimeStamp();
	df->Write(m_update, "update");
	df->Write(avida_time, "avida time");
	df->Write(num_executed, "num_executed");
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
  df->Write(sum_copy_mut_rate.StdDeviation(), "Standard Deviation in copy mutation rate");
  df->Write(sum_copy_mut_rate.Skewness(), "Skew in copy mutation rate");
  df->Write(sum_copy_mut_rate.Kurtosis(), "Kurtosis in copy mutation rate");
  
  df->Write(sum_log_copy_mut_rate.Mean(), "Average log(copy mutation rate)");
  df->Write(sum_log_copy_mut_rate.Variance(), "Variance in log(copy mutation rate)");
  df->Write(sum_log_copy_mut_rate.StdDeviation(), "Standard Deviation in log(copy mutation rate)");
  df->Write(sum_log_copy_mut_rate.Skewness(), "Skew in log(copy mutation rate)");
  df->Write(sum_log_copy_mut_rate.Kurtosis(), "Kurtosis in log(copy mutation rate)");
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
  df->Write(sum_div_mut_rate.StdDeviation(), "Standard Deviation in divide mutation rate");
  df->Write(sum_div_mut_rate.Skewness(), "Skew in divide mutation rate");
  df->Write(sum_div_mut_rate.Kurtosis(), "Kurtosis in divide mutation rate");
  
  df->Write(sum_log_div_mut_rate.Mean(), "Average log(divide mutation rate)");
  df->Write(sum_log_div_mut_rate.Variance(), "Variance in log(divide mutation rate)");
  df->Write(sum_log_div_mut_rate.StdDeviation(), "Standard Deviation in log(divide mutation rate)");
  df->Write(sum_log_div_mut_rate.Skewness(), "Skew in log(divide mutation rate)");
  df->Write(sum_log_div_mut_rate.Kurtosis(), "Kurtosis in log(divide mutation rate)");
  df->Endl();
}


void cStats::PrintSenseData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment( "Avida sense instruction usage\n" );
  df->WriteComment("total number of organisms whose parents executed sense instructions with given labels" );
  
  df->Write( GetUpdate(), "update" );
  
  for( int i=0; i < sense_last_count.GetSize(); i++ ){
    df->Write(sense_last_count[i], sense_names[i]);
  }
  df->Endl();
}

void cStats::PrintSenseExeData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment( "Avida sense instruction usage\n" );
  df->WriteComment("total number of sense instructions executed by the parents of current organisms with given labels" );
  
  df->Write( GetUpdate(), "update" );
  
  for( int i=0; i < sense_last_exe_count.GetSize(); i++ ){
    df->Write(sense_last_exe_count[i], sense_names[i]);
  }
  df->Endl();
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
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)file);
	df->WriteComment("Avida tasks data: tasks performed with internal resources");
	df->WriteTimeStamp();
	df->WriteComment("First column gives the current update, next columns give the number");
	df->WriteComment("of organisms that have the particular task, performed with internal resources, ");
	df->WriteComment("as a component of their merit");
  
	df->Write(m_update,   "Update");
	for(int i = 0; i < task_internal_last_count.GetSize(); i++) {
		df->Write(task_internal_last_count[i], task_names[i] );
	}
	df->Endl();
}

void cStats::PrintInternalTasksQualData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida tasks quality data: tasks performed using internal resources");
  df->WriteTimeStamp();
  df->WriteComment("First column gives the current update, rest give average and max task quality ");
  df->WriteComment("for those tasks performed using internal resources");
  df->Write(m_update, "Update");
  for(int i = 0; i < task_internal_last_count.GetSize(); i++) {
    double qual = 0.0;
    if (task_internal_last_count[i] > 0)
      qual = task_internal_last_quality[i] / static_cast<double>(task_internal_last_count[i]);
    df->Write(qual, task_names[i] + " Average");
    df->Write(task_internal_last_max_quality[i], task_names[i] + " Max");
  }
  df->Endl();
}






void cStats::PrintKilledPreyFTData(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Record of orgs killed by predators by victim forage type.");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  Apto::Array<int> poss_targets = m_world->GetEnvironment().GetAttackPreyFTList();
  Apto::Array<int> org_targets;
  org_targets.Resize(poss_targets.GetSize());
  org_targets.SetAll(0);
  
  const Apto::Array <cOrganism*, Apto::Smart> live_orgs = m_world->GetPopulation().GetLiveOrgList();
  for (int i = 0; i < live_orgs.GetSize(); i++) {
    Apto::Array<int> killed_list = live_orgs[i]->GetPhenotype().GetKilledPreyFTData();
    if (killed_list.GetSize() > 0) {
      assert(killed_list.GetSize() == org_targets.GetSize());
      for (int i = 0; i < killed_list.GetSize(); i++) org_targets[i] += killed_list[i];
    }
  }

  for (int target = 0; target < org_targets.GetSize(); target++) {
    df->Write(poss_targets[target], "Killed FT ID");
    df->Write(org_targets[target], "Num Orgs Killed");
  }
  df->Endl();
}

void cStats::PrintBirthLocData(int org_idx)
{
  cString file = "birthlocs";
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)file);
  
  if (!df->HeaderDone()) {
    df->WriteComment("Update,OrgID,ParentFT,BirthCellX,BirthCellY");
    df->FlushComments();
    df->Endl();
  }
  
  bool use_av = m_world->GetConfig().USE_AVATARS.Get();
  cOrganism* org = m_world->GetPopulation().GetLiveOrgList()[org_idx];
  const int worldx = m_world->GetConfig().WORLD_X.Get();

  int loc = org->GetPhenotype().GetBirthCell();
  if (use_av) loc = org->GetPhenotype().GetAVBirthCell();
  const int locx = loc % worldx;
  const int locy = loc / worldx;
  const int ft = org->GetParentFT();
  
  std::ofstream& fp = df->OFStream();
  fp << GetUpdate() << "," << org->GetID() << "," << ft << "," << locx << "," << locy;
  fp << endl;
}

void cStats::PrintLookData(cString& string)
{
  cString file = "looksettings.dat";
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)file);
  
  if (!df->HeaderDone()) {
    df->WriteComment("Final (used) Look Settings");
    df->WriteTimeStamp();
    
    df->WriteComment("Update");
    df->WriteComment("ThisOrgForageTarget");
    df->WriteComment("AnyFound");
    df->WriteComment("Habitat");
    df->WriteComment("Distance");
    df->WriteComment("SearchType");
    df->WriteComment("SoughtID");
    df->FlushComments();
    df->Endl();
  }
  
  std::ofstream& fp = df->OFStream();
  fp << GetUpdate() << "," << string << endl;
}

void cStats::PrintLookDataOutput(cString& string)
{
  cString file = "lookoutput.dat";
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)file);
  
  if (!df->HeaderDone()) {
    df->WriteComment("Final (used) Look Output to Registers");
    df->WriteTimeStamp();
    
    df->WriteComment("Update");
    df->WriteComment("ThisOrgForageTarget");
    df->WriteComment("AnyFound");
    df->WriteComment("Habitat");
    df->WriteComment("Distance");
    df->WriteComment("SearchType");
    df->WriteComment("SoughtID");
    df->WriteComment("Count");
    df->WriteComment("Value");
    df->WriteComment("Group");
    df->WriteComment("FT");
    df->FlushComments();
    df->Endl();
  }
  
  std::ofstream& fp = df->OFStream();
  fp << GetUpdate() << "," << string << endl;
}

void cStats::PrintLookEXDataOutput(cString& string)
{
  cString file = "lookoutput.dat";
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)file);
  
  if (!df->HeaderDone()) {
    df->WriteComment("Final (used) Look Output to Registers");
    df->WriteTimeStamp();
    
    df->WriteComment("Update");
    df->WriteComment("ThisOrgForageTarget");
    df->WriteComment("AnyFound");
    df->WriteComment("Habitat");
    df->WriteComment("SoughtID");
    df->WriteComment("TravelDistance");
    df->WriteComment("Deviance");
    df->WriteComment("CountValue");
    df->FlushComments();
    df->Endl();
  }
  
  std::ofstream& fp = df->OFStream();
  fp << GetUpdate() << "," << string << endl;
}






/*
 Print data regarding the living org targets.
 */
void cStats::PrintTargets(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
	df->WriteComment("Targets in use on update boundary.");
  df->WriteComment("-2: is predator, -1: no targets(default), >=0: id of environmental resource targeted).");
  df->WriteComment("Format is update + target0 + count0 + target1 + count1 ...");
	df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  bool has_pred = false;
  int offset = 1;
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
    has_pred = true;
    offset = 2;
  }
  
  // ft's may not be sequentially numbered
  bool dec_prey = false;
  bool dec_pred = false;
  int num_targets = 0;
  std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
  set <int>::iterator itr;
  for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) {
    num_targets++;
    if (*itr == -1 && !dec_prey) {
      offset--;
      dec_prey = true;
    }
    if (*itr == -2 && !dec_pred) {
      offset--;
      dec_pred = true;
    }
  }
  
  Apto::Array<int> raw_target_list;
  raw_target_list.Resize(num_targets);
  raw_target_list.SetAll(0);
  int this_index = 0;
  for (itr = fts_avail.begin(); itr!=fts_avail.end(); itr++) {
    raw_target_list[this_index] = *itr;
    this_index++;
  }
  
  Apto::Array<int> target_list;
  int tot_targets = num_targets + offset;
  target_list.Resize(tot_targets);
  target_list.SetAll(0);
  
  target_list[0] = -1;
  if (has_pred) {
    target_list[0] = -2;
    target_list[1] = -1;
  }
  
  for (int i = 0; i < raw_target_list.GetSize(); i++) {
    if (raw_target_list[i] >= 0) target_list[i + offset] = raw_target_list[i];
  }
  
  Apto::Array<int> org_targets;
  org_targets.Resize(tot_targets);
  org_targets.SetAll(0);
  
  const Apto::Array<cOrganism*, Apto::Smart>& live_orgs = m_world->GetPopulation().GetLiveOrgList();
  for (int i = 0; i < live_orgs.GetSize(); i++) {
    cOrganism* org = live_orgs[i];
    int this_target = org->GetForageTarget();
    if (this_target < -2) this_target = -2;
    int this_index = this_target;
    for (int i = 0; i < target_list.GetSize(); i++) {
      if (target_list[i] == this_target) {
        this_index = i;
        break;
      }
    }
    org_targets[this_index]++;
  }
  for (int target = 0; target < org_targets.GetSize(); target++) {
    df->Write(target_list[target], "Target ID");
    df->Write(org_targets[target], "Num Orgs Targeting ID");
  }
  df->Endl();
}

void cStats::PrintMimicDisplays(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  df->WriteComment("FTs being displayed by mimics on update boundary (true mimic ft == 1).");
  df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
  
  Apto::Array<int> poss_fts;
  poss_fts.Resize(0);
  
  set <int>::iterator itr;
  for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) {
    if (*itr >= 0) {
      poss_fts.Resize(poss_fts.GetSize() + 1);
      poss_fts[poss_fts.GetSize() - 1] = *itr;
    }
  }
  
  Apto::Array<int> displayed_fts;
  displayed_fts.Resize(poss_fts.GetSize());
  displayed_fts.SetAll(0);
  
  const Apto::Array<cOrganism*, Apto::Smart>& live_orgs = m_world->GetPopulation().GetLiveOrgList();
  for (int i = 0; i < live_orgs.GetSize(); i++) {
    cOrganism* org = live_orgs[i];
    if (org->GetForageTarget() == 1) {
      int shown_ft = org->GetShowForageTarget();
      // fts may not be sequentially numbered
      for (int j = 0; j < displayed_fts.GetSize(); j++) {
        if (poss_fts[j] == shown_ft) {
          displayed_fts[j]++;
          break;
        }
      }
    }
  }
      
  for (int target = 0; target < poss_fts.GetSize(); target++) {
    df->Write(poss_fts[target], "Displayed FT");
    df->Write(displayed_fts[target], "Num Orgs Displaying this FT");
  }
  df->Endl();
}

/*
 Print data regarding the living org targets.
 */
void cStats::PrintTopPredTargets(const cString& filename)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
	df->WriteComment("Targets in use on update boundary.");
  df->WriteComment("-3: is top predator, -2: is predator, -1: no targets(default), >=0: id of environmental resource targeted).");
  df->WriteComment("Format is update + target0 + count0 + target1 + count1 ...");
	df->WriteTimeStamp();
  
  df->Write(m_update, "Update");
  
  bool has_pred = false;
  int offset = 1;
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
    has_pred = true;
    offset = 3;
  }
  
  // ft's may not be sequentially numbered
  bool dec_prey = false;
  bool dec_pred = false;
  bool dec_tpred = false;
  int num_targets = 0;
  std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
  set <int>::iterator itr;
  for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) {
    num_targets++;
    if (*itr == -1 && !dec_prey) {
      offset--;
      dec_prey = true;
    }
    if (*itr == -2 && !dec_pred) {
      offset--;
      dec_pred = true;
    }
    if (*itr == -3 && !dec_tpred) {
      offset--;
      dec_tpred = true;
    }
  }
  
  Apto::Array<int> raw_target_list;
  raw_target_list.Resize(num_targets);
  raw_target_list.SetAll(0);
  int this_index = 0;
  for (itr = fts_avail.begin(); itr!=fts_avail.end(); itr++) {
    raw_target_list[this_index] = *itr;
    this_index++;
  }
  
  Apto::Array<int> target_list;
  int tot_targets = num_targets + offset;
  target_list.Resize(tot_targets);
  target_list.SetAll(0);
  
  target_list[0] = -1;
  if (has_pred) {
    target_list[0] = -3;
    target_list[1] = -2;
    target_list[2] = -1;
  }
  
  for (int i = 0; i < raw_target_list.GetSize(); i++) {
    if (raw_target_list[i] >= 0) target_list[i + offset] = raw_target_list[i];
  }
  
  Apto::Array<int> org_targets;
  org_targets.Resize(tot_targets);
  org_targets.SetAll(0);
  
  const Apto::Array <cOrganism*, Apto::Smart> live_orgs = m_world->GetPopulation().GetLiveOrgList();
  for (int i = 0; i < live_orgs.GetSize(); i++) {
    cOrganism* org = live_orgs[i];
    int this_target = org->GetForageTarget();
    if (this_target < -3) this_target = -3;
    int this_index = this_target;
    for (int i = 0; i < target_list.GetSize(); i++) {
      if (target_list[i] == this_target) {
        this_index = i;
        break;
      }
    }
    org_targets[this_index]++;
  }
  for (int target = 0; target < org_targets.GetSize(); target++) {
    df->Write(target_list[target], "Target ID");
    df->Write(org_targets[target], "Num Orgs Targeting ID");
  }
  df->Endl();
}






/*! Prints logged retrieved messages.
 */
void cStats::PrintRetMessageLog(const cString& filename) {
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
	df->WriteComment("Log of all messages sent in population.");
  df->WriteTimeStamp();
  
	for(message_log_t::iterator i=m_retmessage_log.begin(); i!=m_retmessage_log.end(); ++i) {
		df->Write(i->update, "Update [update]");
		df->Write(i->deme, "Deme ID [deme]");
		df->Write(i->src_cell, "Source [src]");
		df->Write(i->dst_cell, "Destination [dst]");
    df->Write(i->transmit_cell, "Transmission_cell [trs]");
		df->Write(i->msg_data, "Message data [data]");
		df->Write(i->msg_label, "Message label [label]");
		df->Endl();
	}
  
	m_retmessage_log.clear();
}



/* Add the time between two tasks */
void cStats::AddTaskSwitchTime(int t1, int t2, int time) {
  intrinsic_task_switch_time[make_pair(t1, t2)].Add(time);
}


void cStats::PrintDenData(const cString& filename) {
  if (m_world->GetConfig().USE_AVATARS.Get() <= 0) return;
  
  int juv_age = m_world->GetConfig().JUV_PERIOD.Get();
  
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  
  int num_juvs = 0;
  int num_adults = 0;
  int num_guards_nest = 0;
  int num_foragers = 0;
  int num_guards_off = 0;
    
  int num_loiterers = 0;
  int active_dens = 0;
  
  
  for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
    cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
    if (!cell.HasAV()) continue;
    
    Apto::Array<double> cell_res;
    cell_res = m_world->GetPopulation().GetResources().GetCellResources(i, m_world->GetDefaultContext());
    
    bool is_active = false;
    for (int j = 0; j < cell_res.GetSize(); j++) {
      if ((resource_lib.GetResDef(j)->GetHabitat() == 4 || resource_lib.GetResDef(j)->GetHabitat() == 3) && cell_res[j] > 0) {
        Apto::Array<cOrganism*> cell_avs = cell.GetCellAVs(); 
        for (int k = 0; k < cell_avs.GetSize(); k++) {
          if (cell_avs[k]->GetPhenotype().GetTimeUsed() < juv_age) {
            num_juvs++;
            is_active = true;
          }
          else
          {
            num_adults++;
            if (cell_avs[k]->IsGuard()) num_guards_nest++;
            else num_loiterers++;
          }
        }
        active_dens += (int)is_active;
        break;  // only do this once if two dens overlap
      } 
      else {
	Apto::Array<cOrganism*> cell_avs = cell.GetCellAVs();
        for (int k = 0; k < cell_avs.GetSize(); k++) {
	      num_adults++;
	      if (cell_avs[k]->IsGuard()) num_guards_off++;
	      else num_foragers++;
	}
      }
    }
  }

  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  df->WriteComment("Number of juveniles and adults in dens");
  df->WriteTimeStamp();
	df->WriteColumnDesc("Update [update]");
  df->WriteColumnDesc("ActiveDens [active_dens]");
  df->WriteColumnDesc("Juveniles [juveniles]");
	df->WriteColumnDesc("Adults [adults]");
  df->WriteColumnDesc("Guards On Nest[guards nest]");
	df->WriteColumnDesc("Loiterers [loiterers]");
  
  df->WriteColumnDesc("Juveniles Killed [juveniles killed]");
  df->WriteColumnDesc("Foragers [foragers]");
  df->WriteColumnDesc("Guards Off Nest [guards off]");
  df->WriteColumnDesc("Guard Fails [guard fails]");

  df->FlushComments();
  
  df->Write(m_update,   "Update");
  df->Write(active_dens,      "ActiveDens");
  df->Write(num_juvs,      "Juveniles");
	df->Write(num_adults,    "Adults");
	df->Write(num_guards_nest,    "GuardsOnNest");
	df->Write(num_loiterers, "Loiterers");
  df->Write(juv_killed, "JuvenilesKilled");
  df->Write(num_foragers, "Foragers");
  df->Write(num_guards_off, "GuardsOffNest");
  df->Write(num_guard_fail, "Guard Failures");
  
  df->Endl();
  num_guard_fail=0; 
  
}





/*! Record information about an organism migrating from this population.
 */
void cStats::OutgoingMigrant(const cOrganism*) {
	m_outgoing.Add(1);
}

/*! Record information about an organism migrating into this population.
 */
void cStats::IncomingMigrant(const cOrganism*) {
	m_incoming.Add(1);
}

/*! Print multiprocess data.
 */
void cStats::PrintMultiProcessData(const cString& filename) {
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Avida-MP data");
  df->WriteTimeStamp();
  df->Write(GetUpdate(), "Update [update]");
	df->Write(m_outgoing.Count(), "Outgoing migrants [outgoing]");
	df->Write(m_incoming.Count(), "Incoming migrants [incoming]");
	df->Endl();
  
	m_outgoing.Clear();
	m_incoming.Clear();
}

/*! Track profiling data.
 */
void cStats::ProfilingData(const profiling_stats_t& pf) {
	for(profiling_stats_t::const_iterator i=pf.begin(); i!=pf.end(); ++i) {
		m_profiling[i->first].Add(i->second);
	}
}

/*! Print profiling data.
 */
void cStats::PrintProfilingData(const cString& filename) {
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  df->WriteComment("Profiling statistics");
  df->WriteTimeStamp();
	df->Write(GetUpdate(), "Update [update]");
  
	for(avg_profiling_stats_t::iterator i=m_profiling.begin(); i!=m_profiling.end(); ++i) {
		df->Write(i->second.Average(), i->first.c_str());
	}
	df->Endl();
  
	m_profiling.clear();
}

/*! Print organism location.
 */
void cStats::PrintOrganismLocation(const cString& filename) {
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)filename);
	
  df->WriteComment("Organism location data");
  df->WriteTimeStamp();
	
	for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
		cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
		if(cell.IsOccupied()) {
			df->Write(GetUpdate(), "Update [update]");
			df->Write(i, "Cell ID [cellid]");
			df->Write(cell.GetOrganism()->GetID(), "Organism ID [orgid]");
			df->Endl();
		}
	}
}



void cStats::PrintMiniTraceReactions(cOrganism* org)
{
  cString filename("");
  filename.Set("minitraces/trace_reactions/org%d-ud%d_ft%d-gt%d.trcreac", org->GetID(), org->GetPhenotype().GetUpdateBorn(), org->GetForageTarget(), org->SystematicsGroup("genotype")->ID());
  
  // Open the file...
  Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)filename);
  
  if (!df->HeaderDone()) {
    df->WriteTimeStamp();
    df->WriteComment("Reaction Data for Traced Org to Date (death or end)");
    df->WriteComment("OrgID");
    df->WriteComment("Update Born");
    df->WriteComment("Reaction Counts");
    df->WriteComment("CPU Cycle at First Trigger of Each Reaction");
    df->WriteComment("Exec Count at First Trigger (== index into execution trace and nav traces)");
    df->FlushComments();
    df->Endl();
  }
  
  std::ofstream& fp = df->OFStream();
  
  Apto::Array<int> reaction_count = org->GetPhenotype().GetCurReactionCount();
  Apto::Array<int> reaction_cycles = org->GetPhenotype().GetFirstReactionCycles();
  Apto::Array<int> reaction_execs = org->GetPhenotype().GetFirstReactionExecs();
  
  fp << org->GetID() << " " << org->GetPhenotype().GetUpdateBorn() << " ";
  for (int i = 0; i < reaction_count.GetSize() - 1; i++) {
    fp << reaction_count[i] << ",";
  }
  fp << reaction_count[reaction_count.GetSize() - 1] << " ";
  
  for (int i = 0; i < reaction_cycles.GetSize() - 1; i++) {
    fp << reaction_cycles[i] << ",";
  }
  fp << reaction_cycles[reaction_cycles.GetSize() - 1] << " ";
  
  for (int i = 0; i < reaction_execs.GetSize() - 1; i++) {
    fp << reaction_execs[i] << ",";
  }
  fp << reaction_execs[reaction_execs.GetSize() - 1];
  fp << endl;  
}

void cStats::PrintMicroTraces(Apto::Array<char, Apto::Smart>& exec_trace, int birth_update, int org_id, int ft, int gen_id)
{
  int death_update = GetUpdate();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "microtrace.dat");
  
  if (!df->HeaderDone()) {
    df->WriteComment("Trace Execution Data");
    df->WriteTimeStamp();
    df->WriteComment("DeathUpdate");
    df->WriteComment("BirthUpdate");
    df->WriteComment("OrgID");
    df->WriteComment("GenotypeID");
    df->WriteComment("ForageTarget");
    df->Endl();
  }
  
  std::ofstream& fp = df->OFStream();
  fp << death_update << "," << birth_update << "," << org_id << "," << gen_id << "," << ft << ",";
  for (int i = 0; i < exec_trace.GetSize(); i++) {
    fp << exec_trace[i];
  }
  fp << endl;
}

void cStats::UpdateTopNavTrace(cOrganism* org, bool force_update)
{
  int best_reac = -1;
  Apto::Array<int> reaction_count = org->GetPhenotype().GetCurReactionCount();
  if (!firstnavtrace) {
    // 'best' org is the one among the orgs with the highest reaction achieved that reproduced in the least number of cycles
    // using cycles, so any inst executions in parallel multi-threads are only counted as one exec
    for (int i = reaction_count.GetSize() -1; i >= 0; i--) {
      if (reaction_count[i] > 0) {
        best_reac = i;
        break;
      }
    }
  }
  int cycle = org->GetPhenotype().GetTimeUsed();
  bool new_winner = false;
  if (best_reac >= topreac && !firstnavtrace) {
    if (best_reac == topreac && cycle < topcycle) new_winner = true;
    else if (best_reac > topreac) new_winner = true;
  }
  else if (cycle < topcycle) new_winner = true;
  if (new_winner || force_update) {
    topreac = best_reac;
    topcycle = cycle;
    topgenid = org->SystematicsGroup("genotype")->ID();
    topid = org->GetID();
    topbirthud = org->GetPhenotype().GetUpdateBorn();
    toprepro = org->GetPhenotype().GetNumExecs();
    topgenome = Genome(org->SystematicsGroup("genotype")->Properties().Get("genome"));
    
    Apto::Array<char, Apto::Smart> trace = org->GetHardware().GetMicroTrace();
    Apto::Array<int, Apto::Smart> traceloc = org->GetHardware().GetNavTraceLoc();
    Apto::Array<int, Apto::Smart> tracefacing = org->GetHardware().GetNavTraceFacing();
    Apto::Array<int, Apto::Smart> traceupdate = org->GetHardware().GetNavTraceUpdate();
    
    toptrace.Resize(trace.GetSize());
    topnavtraceloc.Resize(traceloc.GetSize());
    topnavtraceloc.SetAll(-1);
    topnavtracefacing.Resize(tracefacing.GetSize());
    topnavtracefacing.SetAll(-1);
    topnavtraceupdate.Resize(traceupdate.GetSize());
    topnavtraceupdate.SetAll(-1);
    
    assert(toptrace.GetSize() == topnavtraceloc.GetSize());
    assert(topnavtraceloc.GetSize() == topnavtracefacing.GetSize());
    assert(topnavtracefacing.GetSize() == topnavtraceupdate.GetSize());
    for (int i = 0; i < toptrace.GetSize(); i++) {
      toptrace[i] = trace[i];
      topnavtraceloc[i] = traceloc[i];
      topnavtracefacing[i] = tracefacing[i];
      topnavtraceupdate[i] = traceupdate[i];
    }
    
    topstart = org->GetPhenotype().GetNumExecs() - toptrace.GetSize();

    Apto::Array<int> reaction_cycles = org->GetPhenotype().GetFirstReactionCycles();
    Apto::Array<int> reaction_execs = org->GetPhenotype().GetFirstReactionExecs();
    
    topreactioncycles.Resize(reaction_cycles.GetSize());
    topreactioncycles.SetAll(-1);
    topreactionexecs.Resize(reaction_execs.GetSize());
    topreactionexecs.SetAll(-1);
    topreactions.Resize(reaction_count.GetSize());
    topreactions.SetAll(0);
    
    assert(topreactions.GetSize() == topreactioncycles.GetSize());
    assert(topreactioncycles.GetSize() == topreactionexecs.GetSize());
    for (int i = 0; i < topreactions.GetSize(); i++) {
      topreactions[i] = reaction_count[i];
      topreactioncycles[i] = reaction_cycles[i];
      topreactionexecs[i] = reaction_execs[i];
    }
    toptarget = org->GetParentFT();
  }
  if (m_world->GetPopulation().GetTopNavQ().GetSize() <= 1) PrintTopNavTrace(true);
}

void cStats::PrintTopNavTrace(bool flush)
{
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "navtrace.dat");
  
  df->WriteComment("Org That Reproduced the Fastest (fewest cycles) Among Orgs with the Highest Reaction ID");
  df->WriteTimeStamp();
  df->WriteComment("GenotypeID");
  df->WriteComment("OrgID");
  df->WriteComment("Cycle at First Reproduction (parallel multithread execs = 1 cycle)");
  df->WriteComment("Exec Count at Trace Start");
  df->WriteComment("Exec Count at First Reproduction");

  df->WriteComment("Reaction Counts at First Reproduction");
  df->WriteComment("CPU Cycle at First Trigger of Each Reaction");
  df->WriteComment("Exec Count at First Trigger (== index into execution trace and nav traces)");
  df->WriteComment("");
  df->WriteComment("Updates for each entry in each following trace (to match with res data)");
  df->WriteComment("CellIDs to First Reproduction");
  df->WriteComment("OrgFacings to First Reproduction");
  df->WriteComment("Execution Trace to First Reproduction");
  df->Endl();
  
  std::ofstream& fp = df->OFStream();
  
  // in case nobody has reproduced (e.g. in single org trial) print what we know to date
  if (!topreactions.GetSize()) {
    const Apto::Array <cOrganism*, Apto::Smart> live_orgs = m_world->GetPopulation().GetLiveOrgList();
    for (int i = 0; i < live_orgs.GetSize(); i++) {
      UpdateTopNavTrace(live_orgs[i], true);
      topcycle = -1;
    }
  }
  
  if (topreactions.GetSize()) {
    fp << topgenid << " " << topid << " " << topcycle << " " << topstart << " " << toprepro << " ";
    // reaction related
    for (int i = 0; i < topreactions.GetSize() - 1; i++) {
      fp << topreactions[i] << ",";
    }
    fp << topreactions[topreactions.GetSize() - 1] << " ";
    
    for (int i = 0; i < topreactioncycles.GetSize() - 1; i++) {
      fp << topreactioncycles[i] << ",";
    }
    fp << topreactioncycles[topreactioncycles.GetSize() - 1] << " ";
    
    for (int i = 0; i < topreactionexecs.GetSize() - 1; i++) {
      fp << topreactionexecs[i] << ",";
    }
    fp << topreactionexecs[topreactionexecs.GetSize() - 1] << " ";
    
    // instruction exec sequence related
    for (int i = 0; i < topnavtraceupdate.GetSize() - 1; i++) {
      fp << topnavtraceupdate[i] << ",";
    }
    fp << topnavtraceupdate[topnavtraceupdate.GetSize() - 1] << " ";
    
    for (int i = 0; i < topnavtraceloc.GetSize() - 1; i++) {
      fp << topnavtraceloc[i] << ",";
    }
    fp << topnavtraceloc[topnavtraceloc.GetSize() - 1] << " ";
    
    for (int i = 0; i < topnavtracefacing.GetSize() - 1; i++) {
      fp << topnavtracefacing[i] << ",";
    }
    fp << topnavtracefacing[topnavtracefacing.GetSize() - 1] << " ";
    
    for (int i = 0; i < toptrace.GetSize(); i++) {
      fp << toptrace[i];
    }
    fp << endl;
    
    // print the winning genome
    cString genfile =  cStringUtil::Stringf("topnav_genome/org%d-ud%d-grp%d_ft%d-gt%d.navgeno", topid, topbirthud, topgroup, toptarget, topgenid);
    // need a random number generator to pass to testcpu that does not affect any other random number pulls (since this is just for printing the genome)
    Apto::RNG::AvidaRNG rng(0);
    cAvidaContext ctx2(&m_world->GetDriver(), rng);
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx2);
    testcpu->PrintGenome(ctx2, topgenome, genfile, m_world->GetStats().GetUpdate());
    delete testcpu;
  }
  if (flush) {
    topreac = -1;
    topcycle = -1;
    m_world->GetPopulation().GetTopNavQ().Resize(0);
  }
}

void cStats::PrintReproData(cOrganism* org)
{
  int update = GetUpdate();
  Avida::Output::FilePtr df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), "repro_data.dat");
  
  if (!df->HeaderDone()) {
    df->WriteComment("Org Data up to First Reproduction");
    df->WriteTimeStamp();
    df->WriteComment("ReproUpdate");
    df->WriteComment("GenotypeID");
    df->WriteComment("OrgID");
    df->WriteComment("Age (updates)");
    df->WriteComment("TimeUsed (cycles)");
    df->WriteComment("NumExecutions (attempted executions)");
    df->WriteComment("ReactionCounts");
    df->Endl();
  }
  
  std::ofstream& fp = df->OFStream();
  fp << update << " " << org->SystematicsGroup("genotype")->ID()<< " " << org->GetID() << " " << org->GetPhenotype().GetAge() << " " << org->GetPhenotype().GetTimeUsed()
  << " " << org->GetPhenotype().GetNumExecs() << " ";
  Apto::Array<int> reaction_count = org->GetPhenotype().GetCurReactionCount();
  for (int i = 0; i < reaction_count.GetSize() - 1; i++) {
    fp << reaction_count[i] << ",";
  }
  fp << reaction_count[reaction_count.GetSize() - 1] << endl;
}
