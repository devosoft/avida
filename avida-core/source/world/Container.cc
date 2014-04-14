/*
 *  world/Container.cc
 *  avida-core
 *
 *  Created by David on 1/24/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/world/Container.h"

#include "avida/biota/Organism.h"
#include "avida/environment/Manager.h"
#include "avida/environment/ResourceManager.h"
#include "avida/structure/Controller.h"
#include "avida/systematics/Group.h"


Avida::World::Container::Container(Universe* universe, Structure::Controller* structure)
: m_universe(universe), m_structure(structure)
{
  Environment::ManagerPtr env(Environment::Manager::Of(universe));
  assert(env);
  
  m_resources = env->CreateResourceManagerForStructure(structure);
  
}


Avida::World::Container::~Container()
{
  delete m_structure;
  delete m_resources;
}


void Avida::World::Container::PerformUpdate(Context& ctx, Update current_update)
{
  
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


bool cPopulation::SupportsConcurrentUpdate() const
{
  return true;
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



void Avida::World::Container::recordBirth(bool breed_true)
{
  m_stat_tot_organisms++;
  m_stat_num_births++;
  m_stat_tot_births++;
  
  if (breed_true) m_stat_num_breed_true++;
  else m_stat_num_breed_in++;
}

