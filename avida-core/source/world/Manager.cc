/*
 *  world/Manager.cc
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

#include "avida/world/Manager.h"

#include "avida/data/Manager.h"
#include "avida/data/Package.h"
#include "avida/data/Util.h"
#include "avida/world/Container.h"


Avida::World::Manager::Manager(Universe* universe, Structure::Controller* structure) : m_universe(universe)
{
  m_top_level = ContainerPtr(new Container(m_universe, structure));
}

Avida::World::Manager::~Manager()
{

}


Avida::World::ManagerPtr Avida::World::Manager::CreateWithStructure(Universe* universe, Structure::Controller* structure)
{
  ManagerPtr mgr(new Manager(universe, structure));
  
  if (universe->AttachFacet(Reserved::WorldManagerFacetID, mgr)) return mgr;

  return ManagerPtr(NULL);
}


Avida::World::ManagerPtr Avida::World::Manager::Of(Universe* universe)
{
  ManagerPtr manager;
  manager.DynamicCastFrom(universe->WorldManager());
  return manager;
}


bool Avida::World::Manager::Serialize(ArchivePtr) const
{
  // @TODO
  return false;
}


Avida::UniverseFacetID Avida::World::Manager::UpdateBefore() const
{
  return Reserved::EnvironmentFacetID;
}

Avida::UniverseFacetID Avida::World::Manager::UpdateAfter() const
{
  return "";
}



Avida::Data::ConstDataSetPtr Avida::World::Manager::Provides() const
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

void Avida::World::Manager::UpdateProvidedValues(Update)
{
  // Nothing for now, all handled by ProcessUpdate()
}

Avida::Data::PackagePtr Avida::World::Manager::GetProvidedValueForArgument(const Apto::String& data_id, const Data::Argument&) const
{
  Data::PackagePtr rtn;
  
  if (Data::IsStandardID(data_id)) {
    ProvidedData data_entry;
    if (m_provided_data.Get(data_id, data_entry)) {
      rtn = data_entry.GetData();
    }
    assert(rtn);
  } else if (Data::IsArgumentedID(data_id)) {
    // @TODO
    
  }
  
  return rtn;
}

Apto::String Avida::World::Manager::DescribeProvidedValue(const Apto::String& data_id) const
{
  ProvidedData data_entry;
  Apto::String rtn;
  if (m_provided_data.Get(data_id, data_entry)) {
    rtn = data_entry.description;
  }
  assert(rtn != "");
  return rtn;
}


void Avida::World::Manager::SetActiveArguments(const Data::DataID&, Data::ConstArgumentSetPtr)
{
  // @TODO
}


Avida::Data::ConstArgumentSetPtr Avida::World::Manager::GetValidArguments(const Data::DataID& data_id) const
{
  Data::ArgumentSetPtr arg_set;
  
  if (Data::IsStandardID(data_id)) return arg_set;
  
  // @TODO
  
  return arg_set;
}

bool Avida::World::Manager::IsValidArgument(const Data::DataID& data_id, Data::Argument) const
{
  if (Data::IsStandardID(data_id)) return false;
  
  // @TODO
  return false;
}


template <class T> Avida::Data::PackagePtr packageDataFun(Apto::Functor<T, Apto::NullType> fun)
{
  return Avida::Data::PackagePtr(new Avida::Data::Wrap<T>(fun()));
}

template <class T> Avida::Data::PackagePtr packageDataVal(const T& val)
{
  return Avida::Data::PackagePtr(new Avida::Data::Wrap<T>(val));
}


void Avida::World::Manager::setupProvidedData()
{
  // Load in all the keywords, descriptions, and associated functions for
  // data management.
  
  // Setup functors and references for use in the PROVIDE macro
  Data::ProviderActivateFunctor activate(&Of);
  Data::ManagerPtr mgr = Data::Manager::Of(m_universe);
  
  // Define PROVIDE macro to simplify instantiating new provided data
#define PROVIDE_FUN(name, desc, type, obj, meth) { \
m_provided_data[name] = ProvidedData(desc, Apto::BindFirst(Apto::Functor<Data::PackagePtr, Apto::TL::Create<Apto::Functor<type, Apto::NullType> > >(&packageDataFun<type>), Apto::Functor<type, Apto::NullType>(obj, meth)));\
mgr->Register(name, activate); \
}
#define PROVIDE_VAL(name, desc, type, val) { \
m_provided_data[name] = ProvidedData(desc, Apto::BindFirst(Apto::Functor<Data::PackagePtr, Apto::TL::Create<const type&> >(&packageDataVal<type>), val));\
mgr->Register(name, activate); \
}
  
  PROVIDE_FUN("core.world.ave_generation", "Average Generation", double, &m_top_level->m_stat_org_fitness, &Apto::Stat::Accumulator<double>::Mean);
  PROVIDE_FUN("core.world.organisms", "Count of Organisms in the World", int, m_top_level, &Container::NumOrganisms);
  PROVIDE_FUN("core.world.ave_metabolic_rate", "Average Metabolic Rate", double, &m_top_level->m_stat_org_metabolic_rate, &Apto::Stat::Accumulator<double>::Mean);
  PROVIDE_FUN("core.world.ave_age", "Average Organism Age (in updates)", double, &m_top_level->m_stat_org_age, &Apto::Stat::Accumulator<double>::Mean);
  PROVIDE_FUN("core.world.ave_gestation_time", "Average Gestation Time", double, &m_top_level->m_stat_org_gestation, &Apto::Stat::Accumulator<double>::Mean);
  PROVIDE_FUN("core.world.ave_fitness", "Average Fitness", double, &m_top_level->m_stat_org_fitness, &Apto::Stat::Accumulator<double>::Mean);
  PROVIDE_VAL("core.world.num_births", "Number of Births", int, m_top_level->m_stat_num_births);
  PROVIDE_VAL("core.world.num_deaths", "Number of Deaths", int, m_top_level->m_stat_num_deaths);
  PROVIDE_VAL("core.world.num_breed_true", "Number of Breed True", int, m_top_level->m_stat_num_breed_true);
  PROVIDE_VAL("core.world.num_breed_in", "Number of Breed In", int, m_top_level->m_stat_num_breed_in);
  PROVIDE_VAL("core.world.total_organisms", "Total Number of Organisms", int, m_top_level->m_stat_tot_organisms);
  PROVIDE_VAL("core.world.total_births", "Total Number of Births", int, m_top_level->m_stat_tot_births);
  
#undef PROVIDE
}

