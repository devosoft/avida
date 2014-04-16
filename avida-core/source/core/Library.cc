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


bool Avida::Library::RegisterEpigeneticObjectType(const Apto::String& type_name, EpigeneticObjectCreateFunctor create)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  if (m_epi_types.Has(type_name)) return false;
  
  EpigeneticObjectType* epi_type = new EpigeneticObjectType(create);
  m_epi_types.Set(type_name, epi_type);
  
  return true;
}


Avida::Library::Library()
{
  ;
}


Avida::Library::~Library()
{
  for (Apto::Map<Apto::String, EpigeneticObjectType*>::ValueIterator it = m_epi_types.Values(); it.Next();) {
    delete (*it.Get());
  }
}

