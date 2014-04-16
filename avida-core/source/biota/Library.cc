/*
 *  biota/Library.cc
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

#include "avida/biota/Library.h"


bool Avida::Biota::Library::RegisterTraitType(const Apto::String& type_name, TraitCreateFunctor create)
{
  Apto::MutexAutoLock lock(m_mutex);
  
  if (m_trait_types.Has(type_name)) return false;
  
  TraitType* hw_type = new TraitType(create);
  m_trait_types.Set(type_name, hw_type);
  
  return true;
}


Avida::Biota::Library::Library()
{
  ;
}


Avida::Biota::Library::~Library()
{
  for (Apto::Map<Apto::String, TraitType*>::ValueIterator it = m_trait_types.Values(); it.Next();) {
    delete (*it.Get());
  }
}

