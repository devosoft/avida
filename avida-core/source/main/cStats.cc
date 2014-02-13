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
#include "cWorld.h"
#include "tDataEntry.h"
#include "cReaction.h"
#include "cEventList.h"


using namespace Avida;


cStats::cStats(cWorld* world)
: m_world(world)
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
, last_update(0)
{
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
  
  PROVIDE("core.update",                   "Update",                               int,    GetUpdate);
  PROVIDE("core.world.ave_generation",     "Average Generation",                   double, GetGeneration);
  PROVIDE("core.world.organisms",          "Count of Organisms in the World",      int,    GetNumCreatures);
  PROVIDE("core.world.ave_metabolic_rate", "Average Metabolic Rate",               double, GetAveMerit);
  PROVIDE("core.world.ave_age",            "Average Organism Age (in updates)",    double, GetAveCreatureAge);
  PROVIDE("core.world.ave_gestation_time", "Average Gestation Time",               double, GetAveGestation);
  PROVIDE("core.world.ave_fitness",        "Average Fitness",                      double, GetAveFitness);
  
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



void cStats::RecordBirth(bool breed_true)
{
  tot_organisms++;
  num_births++;
  cumulative_births++;
  
  if (breed_true) num_breed_true++;
  else num_breed_in++;
}

void cStats::ProcessUpdate()
{
  // Increment the "avida_time"
  if (sum_merit.Count() > 0 && sum_merit.Mean() > 0) {
    double delta = ((double)(m_update-last_update))/sum_merit.Mean();
    avida_time += delta;
  }
  last_update = m_update;
  
  // Zero-out any variables which need to be cleared at end of update.
  
  num_births = 0;
  num_deaths = 0;
  num_breed_true = 0;
  
  tot_executed += num_executed;
  num_executed = 0;
  
  
  max_fitness = 0.0;
}

