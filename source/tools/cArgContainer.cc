/*
 *  cArgContainer.cc
 *  Avida
 *
 *  Created by David Bryson on 9/12/06.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include "cArgContainer.h"

#include "cArgSchema.h"
#include "tList.h"


cArgContainer* cArgContainer::Load(cString args, const cArgSchema& schema, tList<cString>* errors)
{
  tArray<bool> set_ints;
  tArray<bool> set_doubles;
  tArray<bool> set_strings;
  
  cArgContainer* ret = new cArgContainer();

  set_ints.Resize(schema.GetNumIntArgs(), false);
  ret->m_ints.Resize(schema.GetNumIntArgs());
  set_doubles.Resize(schema.GetNumDoubleArgs(), false);
  ret->m_doubles.Resize(schema.GetNumDoubleArgs());
  set_strings.Resize(schema.GetNumStringArgs(), false);
  ret->m_strings.Resize(schema.GetNumStringArgs());

  cString arg_ent;
  cString arg_name;
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
          break;
        case cArgSchema::SCHEMA_DOUBLE:
          set_doubles[index] = true;
          ret->m_doubles[index] = arg_ent.AsDouble();
          break;
        case cArgSchema::SCHEMA_STRING:
          set_strings[index] = true;
          arg_ent.Trim();
          ret->m_strings[index] = arg_ent;
          break;
        default:
          success = false;
          if (errors) errors->PushRear(new cString("Invalid schema argument type!"));
      }
    } else {
      success = false;
      if (errors) {
        cString* err_str = new cString();
        err_str->Set("Unrecognized argument: '%s'", static_cast<const char*>(arg_name));
        errors->PushRear(err_str);
      }
    }
    arg_ent = args.Pop(schema.GetEntrySeparator());
  }
  
  for (int i = 0; i < set_ints.GetSize(); i++) {
    if (set_ints[i]) continue;
    if (schema.IsOptionalInt(i)) schema.SetDefaultInt(i, ret->m_ints[i]);
    else {
      success = false; // doc err here
      if (errors) {
        cString* err_str = new cString();
        cString name;
        if (schema.GetIntName(i, name)) {
          err_str->Set("Required argument '%s' was not found.", static_cast<const char*>(name));
        } else {
          err_str->Set("Invalid int schema entry at index %d.", i);
        }
        errors->PushRear(err_str);
      }      
    }
  }
  for (int i = 0; i < set_doubles.GetSize(); i++) {
    if (set_doubles[i]) continue;
    if (schema.IsOptionalDouble(i)) schema.SetDefaultDouble(i, ret->m_doubles[i]);
    else {
      success = false; // doc err here
      if (errors) {
        cString* err_str = new cString();
        cString name;
        if (schema.GetDoubleName(i, name)) {
          err_str->Set("Required argument '%s' was not found.", static_cast<const char*>(name));
        } else {
          err_str->Set("Invalid double schema entry at index %d.", i);
        }
        errors->PushRear(err_str);
      }      
    }
  }
  for (int i = 0; i < set_strings.GetSize(); i++) {
    if (set_strings[i]) continue;
    if (schema.IsOptionalString(i)) schema.SetDefaultString(i, ret->m_strings[i]);
    else {
      success = false; // doc err here
      if (errors) {
        cString* err_str = new cString();
        cString name;
        if (schema.GetStringName(i, name)) {
          err_str->Set("Required argument '%s' was not found.", static_cast<const char*>(name));
        } else {
          err_str->Set("Invalid string schema entry at index %d.", i);
        }
        errors->PushRear(err_str);
      }      
    }
  }
  
  if (!success) {
    delete ret;
    ret = NULL;
  }
  
  return ret;
}
