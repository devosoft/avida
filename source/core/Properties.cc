/*
 *  core/Properties.cc
 *  avida-core
 *
 *  Created by David on 8/11/11.
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

#include "avida/core/Properties.h"


Avida::PropertyTypeID Avida::Property::Null = "null";

Avida::Property::~Property() { ; }

Apto::String Avida::Property::Value() const { return ""; }
bool Avida::Property::SetValue(const Apto::String&) { return false; }

Apto::String Avida::StringProperty::Value() const { return m_value; }
bool Avida::StringProperty::SetValue(const Apto::String& value) { m_value = value; return true; }


const Avida::PropertyTypeID Avida::PropertyTraits<bool>::Type = "bool";
const Avida::PropertyTypeID Avida::PropertyTraits<int>::Type = "int";
const Avida::PropertyTypeID Avida::PropertyTraits<double>::Type = "float";
const Avida::PropertyTypeID Avida::PropertyTraits<const char*>::Type = "string";
const Avida::PropertyTypeID Avida::PropertyTraits<Apto::String>::Type = "string";

bool Avida::PropertyMap::operator==(const PropertyMap& p) const
{
  // Build distinct key sets
  Apto::Set<PropertyID> pm1pids, pm2pids;
  PropertyIDIterator it = m_prop_map.Keys();
  while (it.Next()) pm1pids.Insert(*it.Get());
  it = p.PropertyIDs();
  while (it.Next()) pm2pids.Insert(*it.Get());
  
  // Compare key sets
  if (pm1pids != pm2pids) return false;

  // Compare values
  it = m_prop_map.Keys();
  while (it.Next()) {
    if (m_prop_map.GetWithDefault(*it.Get(), m_default)->Value() != p.Get(*it.Get()).Value()) return false;
  }

  return true;
}

bool Avida::PropertyMap::Serialize(ArchivePtr) const
{
  // @TODO
  assert(false);
  return false;
}
