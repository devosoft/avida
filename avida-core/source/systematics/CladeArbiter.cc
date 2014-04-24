/*
 *  private/systematics/CladeArbiter.cc
 *  Avida
 *
 *  Created by David on 11/11/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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

#include "avida/private/systematics/CladeArbiter.h"

#include "avida/data/Manager.h"
#include "avida/data/Package.h"

#include "avida/private/systematics/Clade.h"

#include "apto/stat/Accumulator.h"

#include <cmath>


Avida::Systematics::CladeArbiter::CladeArbiter(World* world, const RoleID& role)
: Arbiter(role)
, m_next_id(1)
, m_cur_update(-1)
{
  setupProvidedData(world);
}

Avida::Systematics::CladeArbiter::~CladeArbiter()
{
  // @TODO make sure this gets cleaned up last
  Apto::Map<Apto::String, CladePtr>::ValueIterator list_it(m_clades.Values());
  while (list_it.Next() != NULL) {
    assert((*list_it.Get())->ActiveReferenceCount() == 0);
    removeClade(*list_it.Get());
  }
  
}


Avida::Systematics::GroupPtr Avida::Systematics::CladeArbiter::ClassifyNewUnit(UnitPtr u, const ClassificationHints* hints)
{
  CladePtr grp;
  Apto::String group_name;

  if (hints) {
    if (hints->Get("name", group_name)) {
    
      if (m_clades.Get(group_name, grp)) {
        grp->NotifyNewUnit(u);
      }
    
      if (!grp) {
        grp = CladePtr(new Clade(thisPtr(), m_next_id++, group_name));
        m_clades.Set(group_name, grp);
        
        m_tot_clades++;
      }
    } else if (hints->Get("id", group_name)) {
      int group_id = Apto::StrAs(group_name);
      return Group(group_id);
    }
  }
  
  return grp;
}


void Avida::Systematics::CladeArbiter::PerformUpdate(Context&, Update current_update)
{
  m_cur_update = current_update + 1; // +1 since PerformUpdate happens at end of updates, but m_cur_update is used during
  
  Apto::Map<Apto::String, CladePtr>::ValueIterator list_it(m_clades.Values());
  while (list_it.Next() != NULL) (*list_it.Get())->UpdateReset();
}


bool Avida::Systematics::CladeArbiter::Serialize(ArchivePtr) const
{
  // @TOOD - serialize genotype arbiter
  assert(false);
  return false;
}

Avida::Systematics::GroupPtr Avida::Systematics::CladeArbiter::LegacyLoad(void* props)
{
  Apto::String group_name = (const char*)(*static_cast<Apto::SmartPtr<Apto::Map<Apto::String, Apto::String> >*>(props))->GetWithDefault("name", "");
  if (group_name.GetSize()) {
    
    CladePtr grp(m_clades.GetWithDefault(group_name, CladePtr(NULL)));
    if (!grp) {
      grp = CladePtr(new Clade(thisPtr(), m_next_id++, group_name, true));
      m_clades.Set(group_name, grp);
      m_tot_clades++;
    }
    return grp;
  }
  
  return GroupPtr(NULL);
}



Avida::Systematics::GroupPtr Avida::Systematics::CladeArbiter::GroupWithName(const Apto::String& name)
{
  return m_clades.GetWithDefault(name, CladePtr(NULL));
}


Avida::Systematics::GroupPtr Avida::Systematics::CladeArbiter::Group(GroupID g_id)
{
  Apto::Map<Apto::String, CladePtr>::ValueIterator list_it(m_clades.Values());
  while (list_it.Next() != NULL) if ((*list_it.Get())->ID() == g_id) return *list_it.Get();
  
  return GroupPtr(NULL);
}



Avida::Systematics::Arbiter::IteratorPtr Avida::Systematics::CladeArbiter::Begin()
{
  return IteratorPtr(new CladeIterator(thisPtr()));
}




Avida::Data::ConstDataSetPtr Avida::Systematics::CladeArbiter::Provides() const
{
  if (!m_provides) {
    Data::DataSetPtr provides(new Apto::Set<Apto::String>);
    for (Apto::Map<Data::DataID, ProvidedData>::KeyIterator it = m_provided_data.Keys(); it.Next();) {
      provides->Insert(*it.Get());
    }
    m_provides = provides;
  }
  return m_provides;
}

void Avida::Systematics::CladeArbiter::UpdateProvidedValues(Update current_update)
{
  (void)current_update;
  
  Apto::Stat::Accumulator<double> sum_abundance;
  
  // Pre-calculate the total number of units that are currently active (used in entropy calculation)
  Apto::Map<Apto::String, CladePtr>::ValueIterator list_it(m_clades.Values());
  while (list_it.Next() != NULL) {
    sum_abundance.Add((*list_it.Get())->NumUnits());
  }
    
  // Stash all stats so that the can be retrieved using the provider mechanisms
  m_ave_abundance = sum_abundance.Mean();
  m_stderr_abundance = sum_abundance.StdError();
  m_var_abundance = sum_abundance.Variance();
}


Avida::Data::PackagePtr Avida::Systematics::CladeArbiter::GetProvidedValue(const Data::DataID& data_id) const
{
  Data::PackagePtr rtn;
  ProvidedData data_entry;
  if (m_provided_data.Get(data_id, data_entry)) {
    rtn = data_entry.GetData();
  }
  assert(rtn);
  
  return rtn;
}


Apto::String Avida::Systematics::CladeArbiter::DescribeProvidedValue(const Data::DataID& data_id) const
{
  ProvidedData data_entry;
  Apto::String rtn;
  if (m_provided_data.Get(data_id, data_entry)) {
    rtn = data_entry.description;
  }
  assert(rtn != "");
  return rtn;
}




void Avida::Systematics::CladeArbiter::AdjustClade(CladePtr clade, int old_size, int new_size)
{
  // Handle defunct clades
  if (new_size == 0 && clade->ActiveReferenceCount() == 0) {
    removeClade(clade);
    return;
  }
}

template <class T> Avida::Data::PackagePtr Avida::Systematics::CladeArbiter::packageData(const T& val) const
{
  return Data::PackagePtr(new Data::Wrap<T>(val));
}

Avida::Data::ProviderPtr Avida::Systematics::CladeArbiter::activateProvider(World*)
{
  return thisPtr();
}


void Avida::Systematics::CladeArbiter::setupProvidedData(World* world)
{
  // Setup functors and references for use in the PROVIDE macro
  Data::ProviderActivateFunctor activate(this, &CladeArbiter::activateProvider);
  Data::ManagerPtr mgr = Data::Manager::Of(world);
  Apto::Functor<Data::PackagePtr, Apto::TL::Create<const int&> > intStat(this, &CladeArbiter::packageData<int>);
  Apto::Functor<Data::PackagePtr, Apto::TL::Create<const double&> > doubleStat(this, &CladeArbiter::packageData<double>);
  
  // Define PROVIDE macro to simplify instantiating new provided data
#define PROVIDE(name, desc, type, val) { \
m_provided_data[Apto::String("systematics.") + Role() + "." + name] = ProvidedData(desc, Apto::BindFirst(type ## Stat, val));\
mgr->Register(name, activate); \
}
  
  PROVIDE("total", "Total Number of Clades", int, m_tot_clades);
  PROVIDE("current", "Number of Current Clades", int, m_clades.GetSize());
  
  PROVIDE("ave_abundance", "Average Abundance", double, m_ave_abundance);
  PROVIDE("stderr_abundance", "Abundance Standard Error", double, m_stderr_abundance);
  PROVIDE("var_abundance", "Abundance Variance", double, m_var_abundance);
}


void Avida::Systematics::CladeArbiter::removeClade(CladePtr clade)
{
  if (clade->ActiveReferenceCount() || clade->PassiveReferenceCount()) return;
  
  m_clades.Remove(clade->Name());
}


inline Avida::Systematics::CladeArbiterPtr Avida::Systematics::CladeArbiter::thisPtr()
{
  AddReference(); // Explicitly add reference for newly created SmartPtr
  return CladeArbiterPtr(this);
}


Avida::Systematics::GroupPtr Avida::Systematics::CladeArbiter::CladeIterator::Get()
{
  return m_it.Get() ? (GroupPtr)*m_it.Get() : GroupPtr(NULL);
}


Avida::Systematics::GroupPtr Avida::Systematics::CladeArbiter::CladeIterator::Next()
{
  return m_it.Next() ? (GroupPtr)*m_it.Get() : GroupPtr(NULL);
}
