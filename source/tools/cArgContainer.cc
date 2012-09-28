/*
 *  cArgContainer.cc
 *  Avida
 *
 *  Created by David Bryson on 9/12/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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
 */

#include "cArgContainer.h"

#include "avida/core/Feedback.h"

#include "cArgSchema.h"

using namespace Avida;


cArgContainer* cArgContainer::Load(cString args, const cArgSchema& schema, Feedback& feedback)
{
  Apto::Array<bool> set_ints;
  Apto::Array<bool> set_doubles;
  Apto::Array<bool> set_strings;
  
  cArgContainer* ret = new cArgContainer();

  set_ints.Resize(schema.GetNumIntArgs(), false);
  ret->m_ints.Resize(schema.GetNumIntArgs());
  set_doubles.Resize(schema.GetNumDoubleArgs(), false);
  ret->m_doubles.Resize(schema.GetNumDoubleArgs());
  set_strings.Resize(schema.GetNumStringArgs(), false);
  ret->m_strings.Resize(schema.GetNumStringArgs());

  cString arg_ent;
  Apto::String arg_name;
  bool success = true;

  arg_ent = args.Pop(schema.GetEntrySeparator());
  while (arg_ent.GetSize() > 0) {
    arg_name = arg_ent.Pop(schema.GetValueSeparator());
    schema.AdjustArgName(arg_name);
    
    cArgSchema::tType type;
    int index;
    if (schema.FindEntry(arg_name, type, index)) {
      switch (type) {
        case cArgSchema::SCHEMA_INT:
          set_ints[index] = true;
          ret->m_ints[index] = arg_ent.AsInt();
          if (!schema.ValidateInt(index, ret->m_ints[index])) {
            Apto::String name;
            if (schema.GetIntName(index, name)) {
              feedback.Error("value of '%s' exceeds its defined range", static_cast<const char*>(name));
            } else {
              feedback.Error("invalid int schema entry at index %d", index);
            }
          }
          break;
        case cArgSchema::SCHEMA_DOUBLE:
          set_doubles[index] = true;
          ret->m_doubles[index] = arg_ent.AsDouble();
          if (!schema.ValidateDouble(index, ret->m_doubles[index])) {
            Apto::String name;
            if (schema.GetDoubleName(index, name)) {
              feedback.Error("value of '%s' exceeds its defined range", static_cast<const char*>(name));
            } else {
              feedback.Error("invalid double schema entry at index %d", index);
            }
          }
          break;
        case cArgSchema::SCHEMA_STRING:
          set_strings[index] = true;
          arg_ent.Trim();
          ret->m_strings[index] = arg_ent;
          break;
        default:
          success = false;
          feedback.Error("invalid schema argument type!");
      }
    } else {
      success = false;
      feedback.Error("unrecognized argument: '%s'", static_cast<const char*>(arg_name));
    }
    arg_ent = args.Pop(schema.GetEntrySeparator());
  }
  
  for (int i = 0; i < set_ints.GetSize(); i++) {
    if (set_ints[i]) continue;
    if (schema.IsOptionalInt(i)) schema.SetDefaultInt(i, ret->m_ints[i]);
    else {
      success = false; // doc err here
      Apto::String name;
      if (schema.GetIntName(i, name)) {
        feedback.Error("required argument '%s' was not found", static_cast<const char*>(name));
      } else {
        feedback.Error("invalid int schema entry at index %d", i);
      }
    }
  }
  for (int i = 0; i < set_doubles.GetSize(); i++) {
    if (set_doubles[i]) continue;
    if (schema.IsOptionalDouble(i)) schema.SetDefaultDouble(i, ret->m_doubles[i]);
    else {
      success = false; // doc err here
      Apto::String name;
      if (schema.GetDoubleName(i, name)) {
        feedback.Error("required argument '%s' was not found", static_cast<const char*>(name));
      } else {
        feedback.Error("invalid double schema entry at index %d", i);
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
      if (schema.GetStringName(i, name)) {
        feedback.Error("required argument '%s' was not found", static_cast<const char*>(name));
      } else {
        feedback.Error("invalid string schema entry at index %d", i);
      }
    }
  }
  
  if (!success) {
    delete ret;
    ret = NULL;
  }
  
  return ret;
}
