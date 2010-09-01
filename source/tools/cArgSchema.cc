/*
 *  cArgSchema.cc
 *  Avida
 *
 *  Created by David Bryson on 9/14/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cArgSchema.h"


cArgSchema::~cArgSchema()
{
  for (int i = 0; i < m_ints.GetSize(); i++) delete m_ints[i];
  for (int i = 0; i < m_doubles.GetSize(); i++) delete m_doubles[i];
  for (int i = 0; i < m_strings.GetSize(); i++) delete m_strings[i];
}

bool cArgSchema::AddEntry(cString in_name, int in_idx, tType in_type)
{
  AdjustArgName(in_name);
  if (m_entries.HasEntry(in_name)) return false;
  
  sArgSchemaEntry* entry = new sArgSchemaEntry(in_name, in_idx, in_type);
  
  switch (in_type) {
    case SCHEMA_INT:
      if (m_ints.GetSize() <= in_idx) m_ints.Resize(in_idx + 1, NULL);
      m_ints[in_idx] = entry;
      break;
    case SCHEMA_DOUBLE:
      if (m_doubles.GetSize() <= in_idx) m_doubles.Resize(in_idx + 1, NULL);
      m_doubles[in_idx] = entry;
      break;
    case SCHEMA_STRING:
      if (m_strings.GetSize() <= in_idx) m_strings.Resize(in_idx + 1, NULL);
      m_strings[in_idx] = entry;
      break;
    default:
      delete entry;
      return false;
  }
  
  m_entries.Set(in_name, entry);
  
  return true;
}

bool cArgSchema::AddEntry(cString in_name, int in_idx, int def)
{
  AdjustArgName(in_name);
  if (m_entries.HasEntry(in_name)) return false;
  
  sArgSchemaEntry* entry = new sArgSchemaEntry(in_name, in_idx, def); 
  m_entries.Set(in_name, entry);

  if (m_ints.GetSize() <= in_idx) m_ints.Resize(in_idx + 1, NULL);
  m_ints[in_idx] = entry;
  
  return true;
}

bool cArgSchema::AddEntry(cString in_name, int in_idx, int lower, int upper)
{
  if (AddEntry(in_name, in_idx, SCHEMA_INT)) {
    m_ints[in_idx]->has_range_limits = true;
    m_ints[in_idx]->r_l_int = lower;
    m_ints[in_idx]->r_u_int = upper;
    return true;
  }
  
  return false;
}

bool cArgSchema::AddEntry(cString in_name, int in_idx, int lower, int upper, int def)
{
  if (AddEntry(in_name, in_idx, def)) {
    m_ints[in_idx]->has_range_limits = true;
    m_ints[in_idx]->r_l_int = lower;
    m_ints[in_idx]->r_u_int = upper;
    return true;
  }
  
  return false;
}

bool cArgSchema::AddEntry(cString in_name, int in_idx, double def)
{
  AdjustArgName(in_name);
  if (m_entries.HasEntry(in_name)) return false;
  
  sArgSchemaEntry* entry = new sArgSchemaEntry(in_name, in_idx, def); 
  m_entries.Set(in_name, entry);
  
  if (m_doubles.GetSize() <= in_idx) m_doubles.Resize(in_idx + 1, NULL);
  m_doubles[in_idx] = entry;
  
  return true;
}

bool cArgSchema::AddEntry(cString in_name, int in_idx, double lower, double upper)
{
  if (AddEntry(in_name, in_idx, SCHEMA_DOUBLE)) {
    m_ints[in_idx]->has_range_limits = true;
    m_ints[in_idx]->r_l_double = lower;
    m_ints[in_idx]->r_u_double = upper;
    return true;
  }
  
  return false;
}

bool cArgSchema::AddEntry(cString in_name, int in_idx, double lower, double upper, double def)
{
  if (AddEntry(in_name, in_idx, def)) {
    m_ints[in_idx]->has_range_limits = true;
    m_ints[in_idx]->r_l_double = lower;
    m_ints[in_idx]->r_u_double = upper;
    return true;
  }
  
  return false;
}

bool cArgSchema::AddEntry(cString in_name, int in_idx, const cString& def)
{
  AdjustArgName(in_name);
  if (m_entries.HasEntry(in_name)) return false;
  
  cString* str = new cString(def);
  sArgSchemaEntry* entry = new sArgSchemaEntry(in_name, in_idx, str); 
  m_entries.Set(in_name, entry);
  
  if (m_strings.GetSize() <= in_idx) m_strings.Resize(in_idx + 1, NULL);
  m_strings[in_idx] = entry;
  
  return true;
}


bool cArgSchema::FindEntry(const cString& in_name, tType& ret_type, int& ret_idx) const
{
  sArgSchemaEntry* entry;
  if (m_entries.Find(in_name, entry)) {
    ret_type = entry->type;
    ret_idx = entry->index;
    return true;
  }
  
  return false;
}
