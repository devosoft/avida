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

#include "avida/data/Package.h"
#include "avida/data/Provider.h"
#include "avida/data/Recorder.h"

#include <cassert>


Avida::Data::Manager::Manager(cWorld* world) : m_world(world)
{
  
}

Avida::Data::Manager::~Manager()
{
}


Avida::Data::ConstDataSetPtr Avida::Data::Manager::GetAvailable() const
{
  if (!m_available) {
    DataSetPtr available(new Apto::Set<Apto::String>);
    for (Apto::Map<Apto::String, ProviderActivateFunctor>::KeyIterator it = m_provider_map.Keys(); it.Next();) {
      available->Insert(*it.Get());
    }
    m_available = available;
  }
  return m_available;
}

bool Avida::Data::Manager::IsAvailable(const Apto::String& data_id) const
{
  return m_provider_map.Has(data_id);
}

bool Avida::Data::Manager::IsActive(const Apto::String& data_id) const
{
  return m_active_map.Has(data_id);
}


bool Avida::Data::Manager::AttachRecorder(RecorderPtr recorder)
{
  ConstDataSetPtr requested = recorder->GetRequested();
  
  // Make sure that all requested data values are available
  for (ConstDataSetIterator it = requested->Begin(); it.Next();) if (!m_provider_map.Has(*it.Get())) return false;
  
  // Make sure that all requested data values are active
  for (ConstDataSetIterator it = requested->Begin(); it.Next();) {
    if (!m_active_map.Has(*it.Get())) {
      // Request data not active, instantiate provider and register the values it provides as active
      ProviderPtr provider = (m_provider_map.Get(*it.Get()))(m_world);
      assert(provider);
      
      m_active_providers.Push(provider);

      ConstDataSetPtr provided = provider->Provides();
      for (ConstDataSetIterator pit = provided->Begin(); pit.Next();) {
        if (!m_active_map.Has(*pit.Get())) m_active_map[*pit.Get()] = provider;
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


bool Avida::Data::Manager::Register(const Apto::String& data_id, ProviderActivateFunctor functor)
{
  if (m_provider_map.Has(data_id)) return false;
  
  m_provider_map[data_id] = functor;
  return true;
}


void Avida::Data::Manager::UpdateState()
{
  // Update all of the active providers
  for (int i = 0; i < m_active_providers.GetSize(); i++) m_active_providers[i]->UpdateProvidedValues();
  
  // Notify recorders that new data is available
  DataRetrievalFunctor drf(this, &Manager::GetCurrentValue);
  for (Apto::Set<RecorderPtr>::Iterator it = m_recorders.Begin(); it.Next();) {
    (*it.Get())->NotifyData(drf);
  }
}

Avida::Data::PackagePtr Avida::Data::Manager::GetCurrentValue(const Apto::String& data_id) const
{
  ProviderPtr provider;
  if (m_active_map.Get(data_id, provider)) return provider->GetProvidedValue(data_id);
  
  return PackagePtr();
}

