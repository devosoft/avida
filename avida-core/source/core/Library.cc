/*
 *  core/Library.cc
 *  avida-core
 *
 *  Created by David on 4/16/14.
 *  Copyright 2014 Michigan State University. All rights reserved.
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

#include "avida/core/Library.h"


bool Avida::Core::Library::RegisterEpigeneticObjectType(const Apto::String& type_name, EpigeneticObjectCreateFunctor create)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  if (m_epi_types.Has(type_name)) return false;
  
  EpigeneticObjectType* epi_type = new EpigeneticObjectType(create);
  m_epi_types.Set(type_name, epi_type);
  
  return true;
}


bool Avida::Core::Library::RegisterEventType(const Apto::String& type_name, const Util::ArgSchema& schema, EventCreateFunctor create)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  if (m_event_types.Has(type_name)) return false;
  
  EventType* event_type = new EventType(schema, create);
  m_event_types.Set(type_name, event_type);
  
  return true;
}


bool Avida::Core::Library::RegisterEventTriggerType(const Apto::String& type_name, EventTriggerFunctor create)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  if (m_event_trigger_types.Has(type_name)) return false;
  
  EventTriggerType* trigger_type = new EventTriggerType(create);
  m_event_trigger_types.Set(type_name, trigger_type);
  
  return true;
}


Avida::Core::Library::Library()
{
  ;
}


Avida::Core::Library::~Library()
{
  for (Apto::Map<Apto::String, EpigeneticObjectType*>::ValueIterator it = m_epi_types.Values(); it.Next();) {
    delete (*it.Get());
  }
  for (Apto::Map<Apto::String, EventType*>::ValueIterator it = m_event_types.Values(); it.Next();) {
    delete (*it.Get());
  }
  for (Apto::Map<Apto::String, EventTriggerType*>::ValueIterator it = m_event_trigger_types.Values(); it.Next();) {
    delete (*it.Get());
  }
}

