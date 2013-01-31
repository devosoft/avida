/*
 *  environment/Library.cc
 *  avida-core
 *
 *  Created by David on 1/30/13.
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

#include "avida/environment/Library.h"


bool Avida::Environment::Library::RegisterResourceType(const Apto::String& res_type_name, Util::ArgSchema& arg_schema,
                                                       ResourceCreateFunctor res_create)
{
  Apto::MutexAutoLock lock(m_mutex);

  if (m_resource_types.Has(res_type_name)) return false;
  
  ResourceType* res_type = new ResourceType(arg_schema, res_create);
  m_resource_types.Set(res_type_name, res_type);
  
  return true;
}


Avida::Environment::Library::Library()
{
  ;
}


Avida::Environment::Library::~Library()
{
  for (Apto::Map<Apto::String, ResourceType*>::ValueIterator it = m_resource_types.Values(); it.Next();) {
    delete (*it.Get());
  }
}

