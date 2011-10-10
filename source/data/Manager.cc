/*
 *  data/Manager.cc
 *  avida-core
 *
 *  Created by David on 5/17/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
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

#include "avida/data/Manager.h"

#include "avida/core/Archive.h"
#include "avida/data/Package.h"
#include "avida/data/Provider.h"
#include "avida/data/Recorder.h"

#include <cassert>


static Avida::WorldFacetPtr DeserializeDataManager(Avida::ArchivePtr ar)
{
  // @TODO
  return Avida::WorldFacetPtr();
}
bool Avida::Data::Manager::s_registered_with_facet_factory =
  Avida::WorldFacet::RegisterFacetType(Avida::Reserved::DataManagerFacetID, DeserializeDataManager);


Avida::Data::Manager::Manager() : m_world(NULL)
{
  
}

Avida::Data::Manager::~Manager()
{
}


Avida::Data::ConstDataSetPtr Avida::Data::Manager::GetAvailable() const
{
  if (!m_available) {
    DataSetPtr available(new DataSet);
    for (Apto::Map<DataID, ProviderActivateFunctor>::KeyIterator it = m_provider_map.Keys(); it.Next();) {
      available->Insert(*it.Get());
    }
    m_available = available;
  }
  return m_available;
}

bool Avida::Data::Manager::IsAvailable(const DataID& data_id) const
{
  return m_provider_map.Has(data_id);
}

bool Avida::Data::Manager::IsActive(const DataID& data_id) const
{
  if (data_id[data_id.GetSize() - 1] == ']') return m_active_arg_provider_map.Has(data_id);
  return m_active_provider_map.Has(data_id);
}


bool Avida::Data::Manager::AttachRecorder(RecorderPtr recorder)
{
  ConstDataSetPtr requested = recorder->GetRequested();
  
  // Make sure that all requested data values are available
  for (ConstDataSetIterator it = requested->Begin(); it.Next();) {
    DataID data_id = *it.Get();
    
    // Check for invalid data id
    if (!data_id.GetSize()) return false;
    
    if (data_id[data_id.GetSize() - 1] == ']') {
      // Handle argumented data value
      
      // Find start of argument
      int start_idx = -1;
      for (int i = 0; i < data_id.GetSize(); i++) {
        if (data_id[i] == '[') {
          start_idx = i + 1;
          break;
        }
      }
      if (start_idx == -1) return false;  // argument start not found
      
      // Separate argument from incoming requested data id
      Apto::String argument = data_id.Substring(start_idx, data_id.GetSize() - start_idx - 1);
      DataID raw_id = data_id.Substring(0, start_idx) + "]";
      
      // Check if argumented provider exists for requested data
      if (!m_arg_provider_map.Has(raw_id)) return false;
      
      // Check and activate provider if active not currently active
      if (!m_active_arg_provider_map.Has(raw_id)) {
        ArgumentedProviderPtr arg_provider = (m_arg_provider_map.Get(raw_id))(m_world);
        if (!arg_provider) return false;
        
        m_active_arg_providers.Push(arg_provider);
        
        ConstDataSetPtr provided = arg_provider->Provides();
        for (ConstDataSetIterator pit = provided->Begin(); pit.Next();) {
          const DataID& pdid = *pit.Get();
          if (pdid[pdid.GetSize() - 1] == ']') {
            if (!m_active_arg_provider_map.Has(pdid)) m_active_arg_provider_map[pdid] = arg_provider;
          } else {
            if (!m_active_provider_map.Has(pdid)) m_active_provider_map[pdid] = arg_provider;
          }
        }
      }
      
      if (!m_active_arg_provider_map[raw_id]->IsValidArgument(raw_id, argument)) return false;
    } else {
      // Check for standard data value availability
      if (!m_provider_map.Has(*it.Get())) return false;
    }
  }
  
  // Make sure that all requested data values are active
  for (ConstDataSetIterator it = requested->Begin(); it.Next();) {
    const DataID& rdid = *it.Get();
    if (rdid[rdid.GetSize() - 1] == ']') {
      // Find start of argument
      int start_idx = -1;
      for (int i = 0; i < rdid.GetSize(); i++) {
        if (rdid[i] == '[') {
          start_idx = i + 1;
          break;
        }
      }
      if (start_idx == -1) return false;  // argument start not found
      
      // Separate argument from incoming requested data id
      Apto::String argument = rdid.Substring(start_idx, rdid.GetSize() - start_idx - 1);
      DataID raw_id = rdid.Substring(0, start_idx) + "]";

      if (!m_active_arg_provider_map.Has(raw_id)) return false; // Argumented providers should be activated above, whaa??
    
      ArgMultiSetPtr arg_set = m_active_args[raw_id];
      if (!arg_set) {
        arg_set = ArgMultiSetPtr(new ArgMultiSet);
        m_active_args[raw_id] = arg_set;
      }
      arg_set->Insert(argument);
      ArgumentSetPtr basic_arg_set(new ArgumentSet);
      *basic_arg_set = *arg_set;
      m_active_arg_provider_map[raw_id]->SetActiveArguments(raw_id, basic_arg_set);
    } else {
      // Request data not active, instantiate provider and register the values it provides as active
      ProviderPtr provider = (m_provider_map.Get(*it.Get()))(m_world);
      if (!provider) return false;
      
      ArgumentedProviderPtr arg_provider;
      arg_provider.DynamicCastFrom(provider);
      
      if (arg_provider) {
        m_active_arg_providers.Push(arg_provider);
        ConstDataSetPtr provided = arg_provider->Provides();
        for (ConstDataSetIterator pit = provided->Begin(); pit.Next();) {
          const DataID& pdid = *pit.Get();
          if (pdid[pdid.GetSize() - 1] == ']') {
            if (!m_active_arg_provider_map.Has(pdid)) m_active_arg_provider_map[pdid] = arg_provider;
          } else {
            if (!m_active_provider_map.Has(pdid)) m_active_provider_map[pdid] = arg_provider;
          }
        }        
      } else {
        m_active_providers.Push(provider);

        ConstDataSetPtr provided = provider->Provides();
        for (ConstDataSetIterator pit = provided->Begin(); pit.Next();) {
          const DataID& pdid = *pit.Get();
          if (pdid[pdid.GetSize() - 1] == ']') {
            // Argumented data from a non-argumented provider, whaa??
            continue;
          } else {
            if (!m_active_provider_map.Has(pdid)) m_active_provider_map[pdid] = arg_provider;
          }
        }
      }
    }
  }
  
  // Store the recorder
  m_recorders.Insert(recorder);
  return true;
}

bool Avida::Data::Manager::DetachRecorder(RecorderPtr recorder)
{
  // @TODO - this should probably deactivate data providers that are no longer needed, or at least adjust schedule
  return m_recorders.Remove(recorder);
}


bool Avida::Data::Manager::Register(const DataID& data_id, ProviderActivateFunctor functor)
{
  if (data_id.GetSize() == 0 || data_id[data_id.GetSize() - 1] == ']' || m_provider_map.Has(data_id)) return false;
  
  m_provider_map[data_id] = functor;
  return true;
}

Avida::Data::ProviderPtr ConvertArgumented(Avida::Data::ArgumentedProviderActivateFunctor functor, Avida::World* world)
{
  Avida::Data::ProviderPtr ptr;
  Avida::Data::ArgumentedProviderPtr aptr(functor(world));
  ptr = aptr;
  return ptr;
}

bool Avida::Data::Manager::Register(const DataID& data_id, ArgumentedProviderActivateFunctor functor)
{
  const int id_size = data_id.GetSize();
  if (id_size > 0 && data_id[id_size - 1] != ']' && m_provider_map.Has(data_id)) {
    Apto::Functor<ProviderPtr, Apto::TL::Create<ArgumentedProviderActivateFunctor, World*> > conv_func(ConvertArgumented);
    m_provider_map[data_id] = Apto::BindFirst(conv_func, functor);
    return true;
  } else if (id_size > 2 && data_id[id_size - 2] == '[' && data_id[id_size - 1] == ']' && m_arg_provider_map.Has(data_id)) {
    m_arg_provider_map[data_id] = functor;
    return true;
  }
  return false;
}

bool Avida::Data::Manager::AttachTo(World* world)
{
  if (m_world) return false;
  
  WorldFacetPtr ptr(this);
  AddReference();  // explictly add reference, since this is internally creating a smart pointer to itself
  
  if (world->AttachFacet(Reserved::DataManagerFacetID, ptr)) {
    m_world = world;
    return true;
  }
  return false;
}
                                    
                                    
Avida::Data::ManagerPtr Avida::Data::Manager::Of(World* world)
{
  ManagerPtr manager;
  WorldFacetPtr facet(world->DataManager());
  manager.DynamicCastFrom(facet);
  return manager;
}


bool Avida::Data::Manager::Serialize(ArchivePtr ar) const
{
  // @TODO
  return false;
}


Avida::WorldFacetID Avida::Data::Manager::UpdateBefore() const
{
  return "";
}

Avida::WorldFacetID Avida::Data::Manager::UpdateAfter() const
{
  return "";
}


void Avida::Data::Manager::PerformUpdate(Context& ctx, Update current_update)
{
  m_current_values.Clear();
  
  // Update all of the active providers
  for (int i = 0; i < m_active_providers.GetSize(); i++) m_active_providers[i]->UpdateProvidedValues(current_update);
  
  // Notify recorders that new data is available
  DataRetrievalFunctor drf(this, &Manager::GetCurrentValue);
  for (Apto::Set<RecorderPtr>::Iterator it = m_recorders.Begin(); it.Next();) {
    (*it.Get())->NotifyData(current_update, drf);
  }
}

Avida::Data::PackagePtr Avida::Data::Manager::GetCurrentValue(const DataID& data_id) const
{
  PackagePtr rtn;
  if (m_current_values.Get(data_id, rtn)) return rtn;
  
  if (data_id[data_id.GetSize() - 1] == ']') {
    // Find start of argument
    int start_idx = -1;
    for (int i = 0; i < data_id.GetSize(); i++) {
      if (data_id[i] == '[') {
        start_idx = i + 1;
        break;
      }
    }
    if (start_idx == -1) return rtn;  // argument start not found
    
    // Separate argument from incoming requested data id
    Apto::String argument = data_id.Substring(start_idx, data_id.GetSize() - start_idx - 1);
    DataID raw_id = data_id.Substring(0, start_idx) + "]";
    
    ArgumentedProviderPtr arg_provider;
    if (m_active_arg_provider_map.Get(raw_id, arg_provider)) {
      rtn = arg_provider->GetProvidedValueForArgument(raw_id, argument);
      if (rtn) m_current_values[data_id] = rtn;
    }
  } else {
    ProviderPtr provider;
    if (m_active_provider_map.Get(data_id, provider)) {
      rtn = provider->GetProvidedValue(data_id);
      if (rtn) m_current_values[data_id] = rtn;
    }
  }
  
  return rtn;
}

