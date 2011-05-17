/*
 *  data/cManager.cc
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

#include "avida/data/cManager.h"


Avida::Data::cManager::cManager(cWorld* world) : m_world(world)
{
  
  
}

Avida::Data::cManager::AvailableSetPtr Avida::Data::cManager::GetAvailable() const
{
  if (!m_available) {
    Apto::SmartPtr<Apto::Set<Apto::String>, Apto::ThreadSafeRefCount> available(new Apto::Set<Apto::String>);
    for (Apto::Map<Apto::String, ProviderActivateFunctor>::KeyIterator it = m_provider_map.Keys(); it.Next();) {
      available->Insert(*it.Get());
    }
    m_available = available;
  }
  return m_available;
}
