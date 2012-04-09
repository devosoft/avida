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


// StringProperty
// --------------------------------------------------------------------------------------------------------------  

Apto::String Avida::StringProperty::StringValue() const { return m_value; }
int Avida::StringProperty::IntValue() const { return Apto::StrAs(m_value); }
double Avida::StringProperty::DoubleValue() const { return Apto::StrAs(m_value); }

bool Avida::StringProperty::SetValue(const Apto::String& value) { m_value = value; return true; }
bool Avida::StringProperty::SetValue(const int value) { m_value = Apto::AsStr(value); return true; }
bool Avida::StringProperty::SetValue(const double value) { m_value = Apto::AsStr(value); return true; }

bool Avida::StringProperty::isEqual(const Property& rhs) const { return dynamic_cast<const StringProperty&>(rhs).m_value == m_value; }



// IntProperty
// --------------------------------------------------------------------------------------------------------------  

Apto::String Avida::IntProperty::StringValue() const { return Apto::AsStr(m_value); }
int Avida::IntProperty::IntValue() const { return m_value; }
double Avida::IntProperty::DoubleValue() const { return m_value; }

bool Avida::IntProperty::SetValue(const Apto::String& value) { m_value = Apto::StrAs(value); return true; }
bool Avida::IntProperty::SetValue(const int value) { m_value = value; return true; }
bool Avida::IntProperty::SetValue(const double value) { m_value = value; return true; }

bool Avida::IntProperty::isEqual(const Property& rhs) const { return dynamic_cast<const IntProperty&>(rhs).m_value == m_value; }



// DoubleProperty
// --------------------------------------------------------------------------------------------------------------  

Apto::String Avida::DoubleProperty::StringValue() const { return Apto::AsStr(m_value); }
int Avida::DoubleProperty::IntValue() const { return m_value; }
double Avida::DoubleProperty::DoubleValue() const { return m_value; }

bool Avida::DoubleProperty::SetValue(const Apto::String& value) { m_value = Apto::StrAs(value); return true; }
bool Avida::DoubleProperty::SetValue(const int value) { m_value = value; return true; }
bool Avida::DoubleProperty::SetValue(const double value) { m_value = value; return true; }

bool Avida::DoubleProperty::isEqual(const Property& rhs) const { return dynamic_cast<const DoubleProperty&>(rhs).m_value == m_value; }



const Avida::PropertyTypeID Avida::PropertyTraits<bool>::Type = "bool";
const Avida::PropertyTypeID Avida::PropertyTraits<int>::Type = "int";
const Avida::PropertyTypeID Avida::PropertyTraits<double>::Type = "float";
const Avida::PropertyTypeID Avida::PropertyTraits<const char*>::Type = "string";
const Avida::PropertyTypeID Avida::PropertyTraits<Apto::String>::Type = "string";


Avida::PropertyDescriptionMap Avida::PropertyMap::s_null_desc_map;
Avida::PropertyPtr Avida::PropertyMap::s_default_prop(new StringProperty("", Property::Null, s_null_desc_map, (const char*)""));


bool Avida::PropertyMap::SetValue(const PropertyID& p_id, const Apto::String& prop_value)
{
  PropertyPtr prop;
  if (m_prop_map.Get(p_id, prop)) {
    return prop->SetValue(prop_value);
  }
  return false;
}


bool Avida::PropertyMap::SetValue(const PropertyID& p_id, const int prop_value)
{
  PropertyPtr prop;
  if (m_prop_map.Get(p_id, prop)) {
    return prop->SetValue(prop_value);
  }
  return false;
}


bool Avida::PropertyMap::SetValue(const PropertyID& p_id, const double prop_value)
{
  PropertyPtr prop;
  if (m_prop_map.Get(p_id, prop)) {
    return prop->SetValue(prop_value);
  }
  return false;
}



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
    if (*m_prop_map.GetWithDefault(*it.Get(), s_default_prop) != p.Get(*it.Get())) return false;
  }

  return true;
}

bool Avida::PropertyMap::Serialize(ArchivePtr) const
{
  // @TODO
  assert(false);
  return false;
}
