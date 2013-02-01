/*
 *  util/ArgParser.cc
 *  avida-core
 *
 *  Created by David Bryson on 9/14/06.
 *  Copyright 2006-2013 Michigan State University. All rights reserved.
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

#include "avida/util/ArgParser.h"

#include "avida/core/Feedback.h"


Avida::Util::ArgSchema::~ArgSchema()
{
  for (int i = 0; i < m_ints.GetSize(); i++) delete m_ints[i];
  for (int i = 0; i < m_doubles.GetSize(); i++) delete m_doubles[i];
  for (int i = 0; i < m_strings.GetSize(); i++) delete m_strings[i];
}


int Avida::Util::ArgSchema::Define(Apto::String in_name, ArgumentType in_type)
{
  AdjustArgName(in_name);
  if (m_entries.Has(in_name)) return false;
  
  Entry* entry = NULL;
  int rtn_idx = -1;
  
  switch (in_type) {
    case INT:
      rtn_idx = m_ints.GetSize();
      entry = new Entry(in_name, rtn_idx, in_type);
      m_ints.Push(entry);
      break;
    case DOUBLE:
      rtn_idx = m_doubles.GetSize();
      entry = new Entry(in_name, rtn_idx, in_type);
      m_doubles.Push(entry);
      break;
    case STRING:
      rtn_idx = m_strings.GetSize();
      entry = new Entry(in_name, rtn_idx, in_type);
      m_strings.Push(entry);
      break;
    default:
      return rtn_idx;
  }
  
  m_entries.Set(in_name, entry);
  
  return rtn_idx;;
}


int Avida::Util::ArgSchema::Define(Apto::String in_name, int def)
{
  AdjustArgName(in_name);
  if (m_entries.Has(in_name)) return -1;
  
  int new_idx = m_ints.GetSize();
  Entry* entry = new Entry(in_name, new_idx, def);
  m_entries.Set(in_name, entry);
  m_ints.Push(entry);
  
  return new_idx;
}

int Avida::Util::ArgSchema::Define(Apto::String in_name, int lower, int upper)
{
  int new_idx = Define(in_name, INT);
  if (new_idx >= 0) {
    m_ints[new_idx]->has_range_limits = true;
    m_ints[new_idx]->r_l_int = lower;
    m_ints[new_idx]->r_u_int = upper;
  }
  
  return new_idx;
}


int Avida::Util::ArgSchema::Define(Apto::String in_name, int lower, int upper, int def)
{
  int new_idx = Define(in_name, def);
  if (new_idx >= 0) {
    m_ints[new_idx]->has_range_limits = true;
    m_ints[new_idx]->r_l_int = lower;
    m_ints[new_idx]->r_u_int = upper;
  }
  
  return new_idx;
}


int Avida::Util::ArgSchema::Define(Apto::String in_name, double def)
{  
  AdjustArgName(in_name);
  if (m_entries.Has(in_name)) return -1;
  
  int new_idx = m_doubles.GetSize();
  Entry* entry = new Entry(in_name, new_idx, def);
  m_entries.Set(in_name, entry);
  m_doubles.Push(entry);
  
  return new_idx;
}


int Avida::Util::ArgSchema::Define(Apto::String in_name, double lower, double upper)
{
  int new_idx = Define(in_name, DOUBLE);
  if (new_idx >= 0) {
    m_doubles[new_idx]->has_range_limits = true;
    m_doubles[new_idx]->r_l_double = lower;
    m_doubles[new_idx]->r_u_double = upper;
  }
  
  return new_idx;
}


int Avida::Util::ArgSchema::Define(Apto::String in_name, double lower, double upper, double def)
{
  int new_idx = Define(in_name, def);
  if (new_idx >= 0) {
    m_doubles[new_idx]->has_range_limits = true;
    m_doubles[new_idx]->r_l_double = lower;
    m_doubles[new_idx]->r_u_double = upper;
  }
  
  return new_idx;
}


int Avida::Util::ArgSchema::Define(Apto::String in_name, const Apto::String& def, Apto::Set<Apto::String>* vocab)
{
  AdjustArgName(in_name);
  if (m_entries.Has(in_name)) return -1;
  
  int new_idx = m_strings.GetSize();
  Apto::String* str = new Apto::String(def);
  Entry* entry = new Entry(in_name, new_idx, str);
  entry->vocab = vocab;
  m_entries.Set(in_name, entry);
  m_strings.Push(entry);
  
  return new_idx;
}


bool Avida::Util::ArgSchema::FindEntry(const Apto::String& in_name, ArgumentType& ret_type, int& ret_idx) const
{
  Entry* entry;
  if (m_entries.Get(in_name, entry)) {
    ret_type = entry->type;
    ret_idx = entry->index;
    return true;
  }
  
  return false;
}


Avida::Util::Args* Avida::Util::Args::Load(Apto::String args, const ArgSchema& schema, char e_sep, char v_sep,
                                           Feedback* feedback)
{
  Apto::Array<bool> set_ints;
  Apto::Array<bool> set_doubles;
  Apto::Array<bool> set_strings;
  
  Args* ret = new Args(schema);
  
  set_ints.Resize(schema.NumIntArgs(), false);
  set_doubles.Resize(schema.NumDoubleArgs(), false);
  set_strings.Resize(schema.NumStringArgs(), false);
  
  Apto::String arg_ent;
  Apto::String arg_name;
  bool success = true;
  
  arg_ent = args.Pop(e_sep);
  while (arg_ent.GetSize() > 0) {
    arg_name = arg_ent.Pop(v_sep);
    schema.AdjustArgName(arg_name);
    
    ArgumentType type;
    int index;
    if (schema.FindEntry(arg_name, type, index)) {
      switch (type) {
        case INT:
          set_ints[index] = true;
          ret->m_ints[index] = Apto::StrAs(arg_ent);
          if (!schema.ValidateInt(index, ret->m_ints[index])) {
            Apto::String name;
            if (schema.IntName(index, name)) {
              if (feedback) feedback->Error("value of '%s' exceeds its defined range", static_cast<const char*>(name));
            } else {
              if (feedback) feedback->Error("invalid int schema entry at index %d", index);
            }
          }
          break;
        case DOUBLE:
          set_doubles[index] = true;
          ret->m_doubles[index] = Apto::StrAs(arg_ent);
          if (!schema.ValidateDouble(index, ret->m_doubles[index])) {
            Apto::String name;
            if (schema.DoubleName(index, name)) {
              if (feedback) feedback->Error("value of '%s' exceeds its defined range", static_cast<const char*>(name));
            } else {
              if (feedback) feedback->Error("invalid double schema entry at index %d", index);
            }
          }
          break;
        case STRING:
          set_strings[index] = true;
          arg_ent.Trim();
          ret->m_strings[index] = arg_ent;
          if (!schema.ValidateString(index, ret->m_strings[index])) {
            Apto::String name;
            if (schema.StringName(index, name)) {
              if (feedback) feedback->Error("value of '%s' unrecognized", static_cast<const char*>(name));
            } else {
              if (feedback) feedback->Error("invalid string schema entry at index %d", index);
            }
          }
          break;
        default:
          success = false;
          if (feedback) feedback->Error("invalid schema argument type!");
      }
    } else {
      success = false;
      if (feedback) feedback->Error("unrecognized argument: '%s'", static_cast<const char*>(arg_name));
    }
    arg_ent = args.Pop(e_sep);
  }
  
  for (int i = 0; i < set_ints.GetSize(); i++) {
    if (set_ints[i]) continue;
    if (schema.IsOptionalInt(i)) schema.SetDefaultInt(i, ret->m_ints[i]);
    else {
      success = false; // doc err here
      Apto::String name;
      if (schema.IntName(i, name)) {
        if (feedback) feedback->Error("required argument '%s' was not found", static_cast<const char*>(name));
      } else {
        if (feedback) feedback->Error("invalid int schema entry at index %d", i);
      }
    }
  }
  for (int i = 0; i < set_doubles.GetSize(); i++) {
    if (set_doubles[i]) continue;
    if (schema.IsOptionalDouble(i)) schema.SetDefaultDouble(i, ret->m_doubles[i]);
    else {
      success = false; // doc err here
      Apto::String name;
      if (schema.DoubleName(i, name)) {
        if (feedback) feedback->Error("required argument '%s' was not found", static_cast<const char*>(name));
      } else {
        if (feedback) feedback->Error("invalid double schema entry at index %d", i);
      }
    }
  }
  for (int i = 0; i < set_strings.GetSize(); i++) {
    if (set_strings[i]) continue;
    if (schema.IsOptionalString(i)) {
      Apto::String def_string;
      schema.SetDefaultString(i, def_string);
      ret->m_strings[i] = def_string;
    }
    else {
      success = false; // doc err here
      Apto::String name;
      if (schema.StringName(i, name)) {
        if (feedback) feedback->Error("required argument '%s' was not found", static_cast<const char*>(name));
      } else {
        if (feedback) feedback->Error("invalid string schema entry at index %d", i);
      }
    }
  }
  
  if (!success) {
    delete ret;
    ret = NULL;
  }
  
  return ret;
}
