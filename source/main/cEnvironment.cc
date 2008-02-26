/*
 *  cEnvironment.cc
 *  Avida
 *
 *  Called "environment.cc" prior to 12/2/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

/*!  *  Routines to read the environment files that contains information
about resources and reactions (which allow rewards or punishments
to organisms doing certain tasks).  */

#include "cEnvironment.h"

#include "cAvidaContext.h"
#include "cEnvReqs.h"
#include "cHardwareManager.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "nMutation.h"
#include "cRandom.h"
#include "cReaction.h"
#include "nReaction.h"
#include "cReactionProcess.h"
#include "cReactionRequisite.h"
#include "cReactionResult.h"
#include "cResource.h"
#include "cStringUtil.h"
#include "cTaskEntry.h"
#include "cTools.h"
#include "cWorld.h"
#include <iostream>
#include <algorithm>

#ifndef tArray_h
#include "tArray.h"
#endif

using namespace std;


bool cEnvironment::ParseSetting(cString entry, cString& var_name, cString& var_value, const cString& var_type)
{
  // Make sure we have an actual entry to parse.
  if (entry.GetSize() == 0) {
    cerr << "Error: Empty setting to parse in " << var_type << endl;
    return false;
  }
  
  // Collect the values...
  var_name = entry.Pop('=');
  var_value = entry;
  
  // Make sure we have both a name and a value...
  if (var_name.GetSize() == 0) {
    cerr << "Error: No variable povided to set to '" << var_value << "' in " << var_type << endl;
    return false;
  }
  
  if (var_value.GetSize() == 0) {
    cerr << "Error: No value given for '" << var_name << "' in " << var_type << endl;
    return false;
  }
  
  // Make the names case insensitive.
  var_name.ToLower();
  
  return true;
}

bool cEnvironment::AssertInputInt(const cString& input, const cString& name, const cString& type)
{
  if (input.IsNumeric() == false) {
    cerr << "Error: In " << type << "," << name << " set to non-integer." << endl;
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputDouble(const cString& input, const cString& name, const cString& type)
{
  if (input.IsNumber() == false) {
    cerr << "Error: In " << type << "," << name << " set to non-number." << endl;
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputBool(const cString& input, const cString& name, const cString& type)
{
  if (input.IsNumber() == false) {
    cerr << "Error: In " << type << "," << name << " set to non-number." << endl;
    return false;
  }
  int value = input.AsInt();
  if ((value != 1) && (value != 0))  {
    cerr << "Error: In " << type << "," << name << " set to non-bool." << endl;
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputValid(void* input, const cString& name, const cString& type, const cString& value)
{
  if (input == NULL) {
    cerr << "Error: In " << type << ", '" << name << "' setting of '" << value << "' not found." << endl;
    return false;
  }
  return true;
}



bool cEnvironment::LoadReactionProcess(cReaction* reaction, cString desc)
{
  cReactionProcess* new_process = reaction->AddProcess();
  
  // Loop through all entries in description.
  while (desc.GetSize() > 0) {
    // Entries are divided by colons.
    cString var_entry = desc.Pop(':');
    cString var_name;
    cString var_value;
    const cString var_type = cStringUtil::Stringf("reaction '%s' process", static_cast<const char*>(reaction->GetName()));
    
    // Parse this entry.
    if (!ParseSetting(var_entry, var_name, var_value, var_type)) return false;
    
    // Now that we know we have a variable name and its value, set it!
    if (var_name == "resource") {
      cResource* test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "resource", var_type, var_value)) {
        return false;
      }
      new_process->SetResource(test_resource);
    }
    else if (var_name == "value") {
      if (!AssertInputDouble(var_value, "value", var_type)) return false;
      new_process->SetValue(var_value.AsDouble());
    }
    else if (var_name == "type") {
      if (var_value=="add") new_process->SetType(nReaction::PROCTYPE_ADD);
      else if (var_value=="mult") new_process->SetType(nReaction::PROCTYPE_MULT);
      else if (var_value=="pow") new_process->SetType(nReaction::PROCTYPE_POW);
      else if (var_value=="lin") new_process->SetType(nReaction::PROCTYPE_LIN);
      else if (var_value=="energy") new_process->SetType(nReaction::PROCTYPE_ENERGY);
      else if (var_value=="enzyme") new_process->SetType(nReaction::PROCTYPE_ENZYME);
      else if (var_value=="exp") new_process->SetType(nReaction::PROCTYPE_EXP);
      else {
        cerr << "Unknown reaction process type '" << var_value
        << "' found in '" << reaction->GetName() << "'." << endl;
        return false;
      }
    }
    else if (var_name == "max") {
      if (!AssertInputDouble(var_value, "max", var_type)) return false;
      new_process->SetMaxNumber(var_value.AsDouble());
    }
    else if (var_name == "min") {
      if (!AssertInputDouble(var_value, "min", var_type)) return false;
      new_process->SetMinNumber(var_value.AsDouble());
    }
    else if (var_name == "frac") {
      if (!AssertInputDouble(var_value, "frac", var_type)) return false;
      double in_frac = var_value.AsDouble();
      if (in_frac > 1.0) in_frac = 1.0;
      new_process->SetMaxFraction(in_frac);
    }
    else if (var_name == "product") {
      cResource* test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "product", var_type, var_value)) {
        return false;
      }
      new_process->SetProduct(test_resource);
    }
    else if (var_name == "conversion") {
      if (!AssertInputDouble(var_value, "conversion", var_type)) return false;
      new_process->SetConversion(var_value.AsDouble());
    }
    else if (var_name == "inst") {
      new_process->SetInstID( m_world->GetHardwareManager().GetInstSet().GetInst(var_value).GetOp() );
    }
    else if (var_name == "lethal") {
      if (!AssertInputBool(var_value, "lethal", var_type)) 
        return false;
      new_process->SetLethal(var_value.AsInt());
    }
    else if (var_name == "sterilize") {
      if (!AssertInputBool(var_value, "sterilize", var_type))
        return false;
      new_process->SetSterile(var_value.AsInt());
    }
    else if (var_name == "detect") {
      cResource* test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "product", var_type, var_value)) {
        return false;
      }
      new_process->SetDetect(test_resource);
    }
    else if (var_name == "threshold") {
      if (!AssertInputDouble(var_value, "threshold", var_type))
        return false;
      new_process->SetDetectionThreshold(var_value.AsDouble());
    }
    else if (var_name == "detectionerror") {
      if (!AssertInputDouble(var_value, "detectionerror", var_type)) 
        return false;
      new_process->SetDetectionError(var_value.AsDouble());
    }
    else if (var_name == "string") {
      new_process->SetMatchString(var_value);
    }
    else if (var_name == "depletable") {
    if (!AssertInputBool(var_value, "depletable", var_type))
        return false;
    new_process->SetDepletable(var_value.AsInt());  
    }

    else {
      cerr << "Error: Unknown process variable '" << var_name
      << "' in reaction '" << reaction->GetName() << "'" << endl;
      return false;
    }
  }
  
  return true;
}

bool cEnvironment::LoadReactionRequisite(cReaction* reaction, cString desc)
{
  cReactionRequisite* new_requisite = reaction->AddRequisite();
  
  // Loop through all entries in description.
  while (desc.GetSize() > 0) {
    // Entries are divided by colons.
    cString var_entry = desc.Pop(':');
    cString var_name;
    cString var_value;
    const cString var_type = cStringUtil::Stringf("reaction '%s' requisite", static_cast<const char*>(reaction->GetName()));
    
    // Parse this entry.
    if (!ParseSetting(var_entry, var_name, var_value, var_type)) return false;
    
    // Now that we know we have a variable name and its value, set it!
    if (var_name == "reaction") {
      cReaction* test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction, "reaction", var_type, var_value)) {
        return false;
      }
      new_requisite->AddReaction(test_reaction);
    }
    else if (var_name == "noreaction") {
      cReaction* test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction,"noreaction",var_type, var_value)) {
        return false;
      }
      new_requisite->AddNoReaction(test_reaction);
    }
    else if (var_name == "min_count") {
      if (!AssertInputInt(var_value, "min_count", var_type)) return false;
      new_requisite->SetMinTaskCount(var_value.AsInt());
    }
    else if (var_name == "max_count") {
      if (!AssertInputInt(var_value, "max_count", var_type)) return false;
      new_requisite->SetMaxTaskCount(var_value.AsInt());
    }
	else if (var_name == "divide_only") {
		if (!AssertInputInt(var_value, "divide_only", var_type)) return false;
		new_requisite->SetDivideOnly(var_value.AsInt());
	}
    else {
      cerr << "Error: Unknown requisite variable '" << var_name
      << "' in reaction '" << reaction->GetName() << "'" << endl;
      return false;
    }
  }
  
  return true;
}


bool cEnvironment::LoadResource(cString desc)
{
  if (desc.GetSize() == 0) {
    cerr << "Warning: Resource line with no resources listed." << endl;
    return false;
  }
  
  while (desc.GetSize() > 0) {
    cString cur_resource = desc.PopWord();
    const cString name = cur_resource.Pop(':');

    /* If resource does not already exist create it, however if it already
       exists (for instance was created as a cell resource) pull it out of
       the library and modify the existing values */

    cResource* new_resource;
    if (! resource_lib.DoesResourceExist(name)) {
      new_resource = resource_lib.AddResource(name);
    } else {
      new_resource = resource_lib.GetResource(name);
    }
    
    while (cur_resource.GetSize() != 0) {
      cString var_entry = cur_resource.Pop(':');
      cString var_name;
      cString var_value;
      const cString var_type = cStringUtil::Stringf("resource '%s'", static_cast<const char*>(name));
      
      // Parse this entry.
      if (!ParseSetting(var_entry, var_name, var_value, var_type)) {
        return false;
      }
      
      if (var_name == "inflow") {
        if (!AssertInputDouble(var_value, "inflow", var_type)) return false;
        new_resource->SetInflow( var_value.AsDouble() );
      }
      else if (var_name == "outflow") {
        if (!AssertInputDouble(var_value, "outflow", var_type)) return false;
        new_resource->SetOutflow( var_value.AsDouble() );
      }
      else if (var_name == "initial") {
        if (!AssertInputDouble(var_value, "initial", var_type)) return false;
        new_resource->SetInitial( var_value.AsDouble() );
      }
      else if (var_name == "geometry") {
        if (!new_resource->SetGeometry( var_value )) {
          cerr << "Error: In " << var_type << "," << var_value << 
          " unknown geometry" << endl;
          return false;
        }
      }
      else if (var_name == "inflowx1" || var_name == "inflowx") {
        if (!AssertInputInt(var_value, "inflowX1", var_type)) return false;
        new_resource->SetInflowX1( var_value.AsInt() );
      }
      else if (var_name == "inflowx2") {
        if (!AssertInputInt(var_value, "inflowX2", var_type)) return false;
        new_resource->SetInflowX2( var_value.AsInt() );
      }
      else if (var_name == "inflowy1" || var_name == "inflowy") {
        if (!AssertInputInt(var_value, "inflowY1", var_type)) return false;
        new_resource->SetInflowY1( var_value.AsInt() );
      }
      else if (var_name == "inflowy2") {
        if (!AssertInputInt(var_value, "inflowY2", var_type)) return false;
        new_resource->SetInflowY2( var_value.AsInt() );
      }
      else if (var_name == "outflowx1" || var_name == "outflowx") {
        if (!AssertInputInt(var_value, "outflowX1", var_type)) return false;
        new_resource->SetOutflowX1( var_value.AsInt() );
      }
      else if (var_name == "outflowx2") {
        if (!AssertInputInt(var_value, "outflowX2", var_type)) return false;
        new_resource->SetOutflowX2( var_value.AsInt() );
      }
      else if (var_name == "outflowy1" || var_name == "outflowy") {
        if (!AssertInputInt(var_value, "outflowY1", var_type)) return false;
        new_resource->SetOutflowY1( var_value.AsInt() );
      }
      else if (var_name == "outflowy2") {
        if (!AssertInputInt(var_value, "outflowY2", var_type)) return false;
        new_resource->SetOutflowY2( var_value.AsInt() );
      }
      else if (var_name == "xdiffuse") {
        if (!AssertInputDouble(var_value, "xdiffuse", var_type)) return false;
        new_resource->SetXDiffuse( var_value.AsDouble() );
      }
      else if (var_name == "xgravity") {
        if (!AssertInputDouble(var_value, "xgravity", var_type)) return false;
        new_resource->SetXGravity( var_value.AsDouble() );
      }
      else if (var_name == "ydiffuse") {
        if (!AssertInputDouble(var_value, "ydiffuse", var_type)) return false;
        new_resource->SetYDiffuse( var_value.AsDouble() );
      }
      else if (var_name == "ygravity") {
        if (!AssertInputDouble(var_value, "ygravity", var_type)) return false;
        new_resource->SetYGravity( var_value.AsDouble() );
      }
      else if (var_name == "deme") {
        if (!new_resource->SetDemeResource( var_value )) {
          cerr << "Error: In " << var_type << "," << var_value <<
          " must be true or false" << endl;
          return false;
        }
      }
      else if (var_name == "energy") {
        if (!new_resource->SetEnergyResource( var_value )) {
          cerr << "Error: In " << var_type << "," << var_value <<
          " must be true or false" << endl;
          return false;
        } else if(m_world->GetConfig().ENERGY_ENABLED.Get() == 0) {
          cerr <<"Error: Energy resources can not be used without the energy model.\n";
        }
      }
      else {
        cerr << "Error: Unknown variable '" << var_name
        << "' in resource '" << name << "'" << endl;
        return false;
      }
    }
    
    // If there are valid values for X/Y1's but not for X/Y2's assume that 
    // the user is interested only in one point and set the X/Y2's to the
    // same value as X/Y1's
    
    if (new_resource->GetInflowX1()>-99 && new_resource->GetInflowX2()==-99){
      new_resource->SetInflowX2(new_resource->GetInflowX1());
    }
    if (new_resource->GetInflowY1()>-99 && new_resource->GetInflowY2()==-99){
      new_resource->SetInflowY2(new_resource->GetInflowY1());
    }
    if (new_resource->GetOutflowX1()>-99 && new_resource->GetOutflowX2()==-99) {
      new_resource->SetOutflowX2(new_resource->GetOutflowX1());
    }
    if (new_resource->GetOutflowY1()>-99 && new_resource->GetOutflowY2()==-99) {
      new_resource->SetOutflowY2(new_resource->GetOutflowY1());
    }
  }
  
  return true;
}

bool cEnvironment::LoadCell(cString desc)

/*****************************************************************************
  Routine to read in spatial resources loaded in one cell at a time. Syntax:

   CELL resource_name:cell_list[:options]

   where options are initial, inflow and outflow
*****************************************************************************/

{
  if (desc.GetSize() == 0) {
    cerr << "Warning: CELL line with no resources listed." << endl;
    return false;
  }
  
  cResource* this_resource;
  while (desc.GetSize() > 0) {
    cString cur_resource = desc.PopWord();
    const cString name = cur_resource.Pop(':');

    /* if this resource has not been already created go ahead and create it and
       set some default global values */

    if (! resource_lib.DoesResourceExist(name)) {
      this_resource = resource_lib.AddResource(name);
      this_resource->SetInitial(0.0);
      this_resource->SetInflow(0.0);
      this_resource->SetOutflow(0.0);
      this_resource->SetGeometry("GRID");
      this_resource->SetInflowX1(-99);
      this_resource->SetInflowX2(-99);
      this_resource->SetInflowY1(-99);
      this_resource->SetInflowY2(-99);
      this_resource->SetOutflowX1(-99);
      this_resource->SetOutflowX2(-99);
      this_resource->SetOutflowY1(-99);
      this_resource->SetOutflowY2(-99);
      this_resource->SetXDiffuse(0.0);
      this_resource->SetXGravity(0.0);
      this_resource->SetYDiffuse(0.0);
      this_resource->SetYGravity(0.0);
      this_resource->SetDemeResource("false");
    } else {
      this_resource = resource_lib.GetResource(name);
    }
    cString cell_list_str = cur_resource.Pop(':'); 
    tArray<int> cell_list = cStringUtil::ReturnArray(cell_list_str);
    double tmp_initial = 0.0;
    double tmp_inflow = 0.0;
    double tmp_outflow = 0.0;
    while (cur_resource.GetSize() != 0) {
      cString var_entry = cur_resource.Pop(':');
      cString var_name;
      cString var_value;
      const cString var_type = 
        cStringUtil::Stringf("resource '%s'", static_cast<const char*>(name));
      
      // Parse this entry.
      if (!ParseSetting(var_entry, var_name, var_value, var_type)) {
        return false;
      }
      
      if (var_name == "inflow") {
        if (!AssertInputDouble(var_value, "inflow", var_type)) return false;
        tmp_inflow = var_value.AsDouble();
      }
      else if (var_name == "outflow") {
        if (!AssertInputDouble(var_value, "outflow", var_type)) return false;
        tmp_outflow = var_value.AsDouble();
      }
      else if (var_name == "initial") {
        if (!AssertInputDouble(var_value, "initial", var_type)) return false;
        tmp_initial = var_value.AsDouble();
      }
      else {
        cerr << "Error: Unknown variable '" << var_name
        << "' in resource '" << name << "'" << endl;
        return false;
      }
    }
    for (int i=0; i < cell_list.GetSize(); i++) {
      cCellResource tmp_cell_resource(cell_list[i],tmp_initial,
                                      tmp_inflow, tmp_outflow);
      this_resource->AddCellResource(tmp_cell_resource);
    }
    
  }
  
  return true;
}

bool cEnvironment::LoadReaction(cString desc)
{
  // Make sure this reaction has a description...
  if (desc.GetSize() == 0) {
    cerr << "Error: Each reaction must include a name and trigger." << endl;
    return false;
  }
  
  // Load in the reaction name
  const cString name = desc.PopWord();
  cReaction* new_reaction = reaction_lib.AddReaction(name);
  
  // If only a name was present, assume this reaction is a pre-declaration.
  if (desc.GetSize() == 0) {
    return true;
  }
  
  // Make sure this reaction hasn't already been loaded with a different
  // definition.
  if (new_reaction->GetTask() != NULL) {
    cerr << "Error: Re-defining reaction '" << name << "'." << endl;
    return false;
  }
  
  // Finish loading in this reaction.
  cString trigger_info = desc.PopWord();
	cString trigger = trigger_info.Pop(':');
  
  // Load the task trigger
  cEnvReqs envreqs;
  tList<cString> errors;
  cTaskEntry* cur_task = m_tasklib.AddTask(trigger, trigger_info, envreqs, &errors);
  if (cur_task == NULL || errors.GetSize() > 0) {
    cString* err_str;
    while ((err_str = errors.Pop()) != NULL) {
      cerr << *err_str << endl;
      delete err_str;
    }
    return false;
  }
  new_reaction->SetTask(cur_task);      // Attack task to reaction.
  
  while (desc.GetSize()) {
    cString desc_entry = desc.PopWord();      // Get the next argument
    cString entry_type = desc_entry.Pop(':'); // Determine argument type
    entry_type.ToLower();                     // Make case insensitive.
    
    // Determine the type of each argument and process it.
    if (entry_type == "process") {
      if (LoadReactionProcess(new_reaction, desc_entry) == false) {
        cerr << "...failed in loading reaction-process..." << endl;
        return false;
      }
    }
    else if (entry_type == "requisite") {
      if (LoadReactionRequisite(new_reaction, desc_entry) == false) {
        cerr << "...failed in loading reaction-requisite..." << endl;
        return false;
      }
    }
    else {
      cerr << "Unknown entry type '" << entry_type
  	   << "' in reaction '" << name << "'"
  	   << endl;
      return false;
    }
  }
  
  // Process the environment requirements of this task
  if (envreqs.GetMinInputs() > m_input_size) m_input_size = envreqs.GetMinInputs();
  if (envreqs.GetMinOutputs() > m_output_size) m_output_size = envreqs.GetMinOutputs();
  if (envreqs.GetTrueRandInputs()) m_true_rand = true;
  
  return true;
}

bool cEnvironment::LoadMutation(cString desc)
{
  // Make sure this mutation has a description...
  if (desc.CountNumWords() < 5) {
    cerr << "Error: Each mutation must include a name, trigger, scope, type, and rate." << endl;
    return false;
  }
  
  // Load in the mutation info
  const cString name = desc.PopWord().ToLower();
  const cString trigger = desc.PopWord().ToLower();
  const cString scope = desc.PopWord().ToLower();
  const cString type = desc.PopWord().ToLower();
  const double rate = desc.PopWord().AsDouble();
  
  int trig_id = -1;
  int scope_id = -1;
  int type_id = -1;
  
  if (trigger == "none") trig_id = nMutation::TRIGGER_NONE;
  else if (trigger == "update") trig_id = nMutation::TRIGGER_UPDATE;
  else if (trigger == "divide") trig_id = nMutation::TRIGGER_DIVIDE;
  else if (trigger == "parent") trig_id = nMutation::TRIGGER_PARENT;
  else if (trigger == "write") trig_id = nMutation::TRIGGER_WRITE;
  else if (trigger == "read") trig_id = nMutation::TRIGGER_READ;
  else if (trigger == "exec") trig_id = nMutation::TRIGGER_EXEC;
  else {
    cerr << "Error: Unknown mutation trigger '" << trigger << "'." << endl;
    return false;
  }
  
  if (scope == "genome") scope_id = nMutation::SCOPE_GENOME;
  else if (scope == "local") scope_id = nMutation::SCOPE_LOCAL;
  else if (scope == "prop") scope_id = nMutation::SCOPE_PROP;
  else if (scope == "global") scope_id = nMutation::SCOPE_GLOBAL;
  else if (scope == "spread") scope_id = nMutation::SCOPE_SPREAD;
  else {
    cerr << "Error: Unknown mutation scope '" << scope << "'." << endl;
    return false;
  }
  
  if (type == "point") type_id = nMutation::TYPE_POINT;
  else if (type == "insert") type_id = nMutation::TYPE_INSERT;
  else if (type == "delete") type_id = nMutation::TYPE_DELETE;
  else if (type == "head_inc") type_id = nMutation::TYPE_HEAD_INC;
  else if (type == "head_dec") type_id = nMutation::TYPE_HEAD_DEC;
  else if (type == "temp") type_id = nMutation::TYPE_TEMP;
  else if (type == "kill") type_id = nMutation::TYPE_KILL;
  else {
    cerr << "Error: Unknown mutation type '" << type << "'." << endl;
    return false;
  }
  
  // Lets do a few checks for legal combinations...
  if (trig_id == nMutation::TRIGGER_NONE) {
    cerr << "Warning: Mutations with trigger 'none' will never occur." << endl;
  }
  
  if (scope_id == nMutation::SCOPE_LOCAL || scope_id == nMutation::SCOPE_PROP) {
    if (trig_id == nMutation::TRIGGER_DIVIDE) {
      cerr << "Error: Offspring after divide have no " << scope
      << " for mutations." << endl;
      return false;
    }
    if (trig_id == nMutation::TRIGGER_UPDATE ||
        trig_id == nMutation::TRIGGER_PARENT) {
      cerr << "Warning: Mutation trigger " << trigger
      << "has no natural positions; IP used." << endl;
    }
  }
  else {  // Genome-wide scope
    if (type_id == nMutation::TYPE_HEAD_INC ||
        type_id == nMutation::TYPE_HEAD_DEC ||
        type_id == nMutation::TYPE_TEMP) {
      cerr << "Error: " << scope << " scope not compatible with type "
      << type << "." << endl;
      return false;
    }
  }
  
  if (type_id == nMutation::TYPE_TEMP) {
    if (trig_id == nMutation::TRIGGER_UPDATE ||
        trig_id == nMutation::TRIGGER_DIVIDE ||
        trig_id == nMutation::TRIGGER_PARENT ||
        trig_id == nMutation::TRIGGER_WRITE) {
      cerr << "Error: " << trigger << " trigger not meaningful with type "
      << type << "." << endl;
      return false;
    }
  }
  
  // If we made it this far, it should be safe to build the mutation.
  mutation_lib.AddMutation(name, trig_id, scope_id, type_id, rate);
  
  return true;
}

bool cEnvironment::LoadSetActive(cString desc)
{
  cString item_type = desc.PopWord(); 
  item_type.ToUpper();
  
  cString item_name = desc.PopWord();
  
  cString item_active = desc.PopWord();
  item_active.ToUpper();
  
  bool new_active = true;
  if (item_active == "0" || item_active == "FALSE") new_active = false;
  
  if (item_type == "REACTION") {
    cReaction* cur_reaction = reaction_lib.GetReaction(item_name);
    if (cur_reaction == NULL) {
      cerr << "Unknown REACTION: '" << item_name << "'" << endl;
      return false;
    }
    cur_reaction->SetActive(new_active);
  } else if (item_type == "") {
    cerr << "Format: SET_ACTIVE <type> <name> <new_status=true>" << endl;
  } else {
    cerr << "Error: Cannot deactivate items of type " << item_type << endl;
    return false;
  }
  
  return true;
}

bool cEnvironment::LoadLine(cString line) 

/* Routine to read in a line from the enviroment file and hand that line
   line to the approprate routine to process it.                         */ 
{
  cString type = line.PopWord();      // Determine type of this entry.
  type.ToUpper();                     // Make type case insensitive.
  
  bool load_ok = true;
  if (type == "RESOURCE") load_ok = LoadResource(line);
  else if (type == "REACTION") load_ok = LoadReaction(line);
  else if (type == "MUTATION") load_ok = LoadMutation(line);
  else if (type == "SET_ACTIVE") load_ok = LoadSetActive(line);
  else if (type == "CELL") load_ok = LoadCell(line);
  else {
    cerr << "Error: Unknown environment keyword '" << type << "." << endl;
    return false;
  }
  
  if (load_ok == false) {
    cerr << "...failed in loading '" << type << "'..." << endl;
    return false;
  }
  
  return true;
}

bool cEnvironment::Load(const cString& filename)
{
  cInitFile infile(filename);
  if (!infile.WasOpened()) {
    tConstListIterator<cString> err_it(infile.GetErrors());
    const cString* errstr = NULL;
    while ((errstr = err_it.Next())) cerr << "Error: " << *errstr << endl;
    cerr << "Error: Failed to load environment '" << filename << "'." << endl;
    return false;
  }
  
  for (int line_id = 0; line_id < infile.GetNumLines(); line_id++) {
    // Load the next line from the file.
    bool load_ok = LoadLine(infile.GetLine(line_id));
    if (load_ok == false) return false;
  }
  
  // Make sure that all pre-declared reactions have been loaded correctly.
  for (int i = 0; i < reaction_lib.GetSize(); i++) {
    if (reaction_lib.GetReaction(i)->GetTask() == NULL) {
      cerr << "Error: Pre-declared reaction '"
      << reaction_lib.GetReaction(i)->GetName() << "' never defined." << endl;
      return false;
    }
  }
  
  return true;
}


void cEnvironment::SetupInputs(cAvidaContext& ctx, tArray<int>& input_array, bool random) const
{
  input_array.Resize(m_input_size);
  
  if (m_use_specific_inputs)
  {
    // Specific inputs trump everything
    input_array = m_specific_inputs;
    
    // If a mask has been set, process the inputs with it
    if (m_mask) {
      for (int i = 0; i < m_input_size; i++) {
        input_array[i] = (input_array[i] & ~m_mask) | (m_mask & ctx.GetRandom().GetUInt(1 << 24));
      }
    }
  } else if (random) {
    if (m_true_rand) {
      for (int i = 0; i < m_input_size; i++) {
        input_array[i] = ctx.GetRandom().GetUInt((unsigned int) 1 << 31);
      }
    } else {
      // Set the top 8 bits of the input buffer...
      input_array[0] = (15 << 24) + ctx.GetRandom().GetUInt(1 << 24);  // 00001111
      input_array[1] = (51 << 24) + ctx.GetRandom().GetUInt(1 << 24);  // 00110011
      input_array[2] = (85 << 24) + ctx.GetRandom().GetUInt(1 << 24);  // 01010101

      // And randomize the rest...
      for (int i = 3; i < m_input_size; i++) {
        input_array[i] = ctx.GetRandom().GetUInt(1 << 24);
      }
    }
  } else {
    // We make sure that all combinations of inputs are present.  This is
    // done explicitly in the key columns... (0f, 33, and 55)
    input_array[0] = 0x0f13149f;  // 00001111 00010011 00010100 10011111
    input_array[1] = 0x3308e53e;  // 00110011 00001000 11100101 00111110
    input_array[2] = 0x556241eb;  // 01010101 01100010 01000001 11101011
    
    // Fill out the rest with deterministically bit-shifted versions of the default 3
    for (int i = 3; i < m_input_size; i++) {
      input_array[i] = input_array[i % 3] << (i / 3);
    }
  }
}


void cEnvironment::SwapInputs(cAvidaContext& ctx, tArray<int>& src_input_array, tArray<int>& dest_input_array) const
{
  tArray<int> tmp_input_array = dest_input_array;

  dest_input_array = src_input_array;
  src_input_array = tmp_input_array;  
}


bool cEnvironment::TestInput(cReactionResult& result, const tBuffer<int>& inputs,
                             const tBuffer<int>& outputs, const tArray<double>& resource_count) const
{
  // @CAO nothing for the moment...
  return false;
}


bool cEnvironment::TestOutput(cAvidaContext& ctx, cReactionResult& result,
                              cTaskContext& taskctx, const tArray<int>& task_count,
                              const tArray<int>& reaction_count, const tArray<double>& resource_count) const
{
  // Do setup for reaction tests...
  m_tasklib.SetupTests(taskctx);
  
  // Loop through all reactions to see if any have been triggered...
  const int num_reactions = reaction_lib.GetSize();
  for (int i = 0; i < num_reactions; i++) {
    cReaction* cur_reaction = reaction_lib.GetReaction(i);
    assert(cur_reaction != NULL);
    
    // Only use active reactions...
    if (cur_reaction->GetActive() == false) continue;
    
    // Examine the task trigger associated with this reaction
    cTaskEntry* cur_task = cur_reaction->GetTask();
    assert(cur_task != NULL);
    
    taskctx.SetTaskEntry(cur_task); // Set task entry in the context, so that tasks can reference task settings
    const int task_id = cur_task->GetID();
    const int task_cnt = task_count[task_id];
    const bool on_divide = taskctx.GetOnDivide();
    
    // Examine requisites on this reaction
    if (TestRequisites(cur_reaction->GetRequisites(), task_cnt, reaction_count, on_divide) == false) {
      continue;
    }

    const double task_quality = m_tasklib.TestOutput(taskctx);
    assert(task_quality >= 0.0);
	

    // If this task wasn't performed, move on to the next one.
    if (task_quality == 0.0) continue;
    
    // Mark this task as performed...
    result.MarkTask(task_id, task_quality, taskctx.GetTaskValue());

    // And lets process it!
    DoProcesses(ctx, cur_reaction->GetProcesses(), resource_count, task_quality, task_cnt, i, result);

    // Note: the reaction is actually marked as being performed inside DoProcesses.
  }  
  
  return result.GetActive();
}



bool cEnvironment::TestRequisites(const tList<cReactionRequisite>& req_list,
                                  int task_count, const tArray<int>& reaction_count, const bool on_divide) const
{
  const int num_reqs = req_list.GetSize();

  // If there are no requisites, there is nothing to meet!
  // (unless this is a check upon dividing, in which case we want the default to be to not check the task
  // and only if the requisite has been added to check it
  if (num_reqs == 0) {
	  return !on_divide;
  }
  
  tLWConstListIterator<cReactionRequisite> req_it(req_list);
  for (int i = 0; i < num_reqs; i++) {
    // See if this requisite batch can be satisfied.
    const cReactionRequisite* cur_req = req_it.Next();
    bool satisfied = true;
    
    // Have all reactions been met?
    tLWConstListIterator<cReaction> reaction_it(cur_req->GetReactions());
    while (reaction_it.Next() != NULL) {
      int react_id = reaction_it.Get()->GetID();
      if (reaction_count[react_id] == 0) {
        satisfied = false;
        break;
      }
    }
    if (satisfied == false) continue;
    
    // Have all no-reactions been met?
    tLWConstListIterator<cReaction> noreaction_it(cur_req->GetNoReactions());
    while (noreaction_it.Next() != NULL) {
      int react_id = noreaction_it.Get()->GetID();
      if (reaction_count[react_id] != 0) {
        satisfied = false;
        break;
      }
    }
    if (satisfied == false) continue;
    
    // Have all task counts been met?
    if (task_count < cur_req->GetMinTaskCount()) continue;
    if (task_count >= cur_req->GetMaxTaskCount()) continue;

	// Have divide task reqs been met?
    // If div_type is 0 we only check on IO, if 1 we only check on divide,
    // if 2 we check always
    int div_type = cur_req->GetDivideOnly();
    if (div_type == 1 && !on_divide) continue;
    if (div_type == 0 && on_divide) continue;
    
    return true;
  }
  
  return false;
}


void cEnvironment::DoProcesses(cAvidaContext& ctx, const tList<cReactionProcess>& process_list,
                               const tArray<double>& resource_count, const double task_quality,
                               const int task_count, const int reaction_id, cReactionResult& result) const
{
  const int num_process = process_list.GetSize();
  
  tLWConstListIterator<cReactionProcess> process_it(process_list);
  for (int i = 0; i < num_process; i++) {
    // See if this requisite batch can be satisfied.
    const cReactionProcess* cur_process = process_it.Next();
    const double max_consumed = cur_process->GetMaxNumber();
    const double min_consumed = cur_process->GetMinNumber();
    
    // Determine resource consumption
    double consumed = 0.0;
    cResource* in_resource = cur_process->GetResource();
    
    if (in_resource == NULL) {
      // Test if infinite resource
      consumed = max_consumed * task_quality;
    } else {
      // Otherwise we're using a finite resource      
      const int res_id = in_resource->GetID();
      
      assert(resource_count[res_id] >= 0);
      assert(result.GetConsumed(res_id) >= 0);
      consumed = resource_count[res_id] - result.GetConsumed(res_id);
      consumed *= cur_process->GetMaxFraction();
      assert(consumed >= 0.0);
      
      // Make sure we're not above the maximum consumption.
      if (consumed > max_consumed) consumed = max_consumed;

      assert((task_quality >= 0.0) && (task_quality <= 1.0));
      consumed *= task_quality;  // modify consumed based on task quality
      
      // Test if we are below the minimum consumption.
      if (consumed < min_consumed) consumed = 0.0;
      
      // If we don't actually have any resource to consume stop here.
      if (consumed == 0.0) continue;
      
      // Can't consume more resource than what's available.
      consumed = std::min(consumed, resource_count[res_id]);
      
      // Mark in the results the resource consumed.
      if (cur_process->GetDepletable()) result.Consume(res_id, consumed);
    }
    
    // Mark the reaction as having been performed if we get here.
    result.MarkReaction(reaction_id);
    
    // Calculate the bonus
    double bonus = consumed * cur_process->GetValue();
    
    switch (cur_process->GetType()) {
      case nReaction::PROCTYPE_ADD:
        result.AddBonus(bonus, reaction_id);
        break;
      case nReaction::PROCTYPE_MULT:
        result.MultBonus(bonus);
        break;
      case nReaction::PROCTYPE_POW:
        result.MultBonus( pow(2.0, bonus) );
        break;
      case nReaction::PROCTYPE_LIN:
        result.AddBonus( bonus * task_count, reaction_id);
        break;
      case nReaction::PROCTYPE_ENERGY:
        result.AddEnergy(bonus);
        break;
      case nReaction::PROCTYPE_ENZYME: //@JEB
      {
        const int res_id = in_resource->GetID();
        assert(cur_process->GetMaxFraction() != 0);
        assert(resource_count[res_id] != 0);
        double reward = cur_process->GetValue() * resource_count[res_id] / (resource_count[res_id] + cur_process->GetMaxFraction());
        result.AddBonus( reward , reaction_id);
        break;
      }
      case nReaction::PROCTYPE_EXP: //@JEB
      {
        // Cumulative rewards are Value * integral (exp (-MaxFraction * TaskCount))
        // Evaluate to get stepwise amount to add per task executed.
        assert(task_count >= 1);
        const double decay = cur_process->GetMaxFraction();
        const double value = cur_process->GetValue();
        result.AddBonus( value * (1.0 / decay) * ( exp((task_count-1) * decay) - exp(task_count * decay)), reaction_id );
        break;
      }
          
      default:
        assert(false);  // Should not get here!
        break;
    };
    
    // Determine detection events
    cResource* detected = cur_process->GetDetect();
    if (detected != NULL) {
      const int detected_id = detected->GetID();
      const double real_amount = resource_count[detected_id];
      double estimated_amount = ctx.GetRandom().GetRandNormal(real_amount, cur_process->GetDetectionError() * real_amount);
      if (estimated_amount < cur_process->GetDetectionThreshold()) {
        result.Detect(detected_id, 0.0);		
      } else {
        result.Detect(detected_id, estimated_amount);
      }
    }
    
    // Determine byproducts
    cResource* product = cur_process->GetProduct();
    if (product != NULL) {
      int product_id = product->GetID();
      double product_size = consumed * cur_process->GetConversion();
      result.Produce(product_id, product_size);
    }
    
    // Determine what instructions should be run...
    const int inst_id = cur_process->GetInstID();
    if (inst_id >= 0) {
      result.AddInst(inst_id);
    }
    
    result.Lethal(cur_process->GetLethal());
    result.Sterilize(cur_process->GetSterilize());
    }
}

double cEnvironment::GetReactionValue(int& reaction_id)
{
  cReaction* found_reaction = reaction_lib.GetReaction(reaction_id);
  if (found_reaction == NULL) return false;
  return found_reaction->GetValue();
}

bool cEnvironment::SetReactionValue(cAvidaContext& ctx, const cString& name, double value)
{
  const int num_reactions = reaction_lib.GetSize();

  // See if this should be applied to all reactions.
  if (name == "ALL") {
    // Loop through all reactions to update their values.
    for (int i = 0; i < num_reactions; i++) {
      cReaction* cur_reaction = reaction_lib.GetReaction(i);
      assert(cur_reaction != NULL);
      cur_reaction->ModifyValue(value);
    }

    return true;
  }

  // See if this should be applied to random reactions.
  if (name.IsSubstring("RANDOM:", 0)) {
    // Determine how many reactions to set.
    const int num_set = name.Substring(7, name.GetSize()-7).AsInt();
    if (num_set > num_reactions) return false;

    // Choose the reactions.
    tArray<int> reaction_ids(num_set);
    ctx.GetRandom().Choose(num_reactions, reaction_ids);

    // And set them...
    for (int i = 0; i < num_set; i++) {
      cReaction* cur_reaction = reaction_lib.GetReaction(reaction_ids[i]);
      assert(cur_reaction != NULL);
      cur_reaction->ModifyValue(value);      
    }
    return true;
  }
  
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  found_reaction->ModifyValue(value);
  return true;
}

bool cEnvironment::SetReactionValueMult(const cString& name, double value_mult)
{
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  found_reaction->MultiplyValue(value_mult);
  return true;
}

bool cEnvironment::SetReactionInst(const cString& name, cString inst_name)
{
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  found_reaction->ModifyInst( m_world->GetHardwareManager().GetInstSet().GetInst(inst_name).GetOp() );
  return true;
}

bool cEnvironment::SetReactionMinTaskCount(const cString& name, int min_count)
{
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  return found_reaction->SetMinTaskCount( min_count );
}

bool cEnvironment::SetReactionMaxTaskCount(const cString& name, int max_count)
{
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  return found_reaction->SetMaxTaskCount( max_count );
}

bool cEnvironment::SetReactionTask(const cString& name, const cString& task)
{
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;

  for(int i=0; i<m_tasklib.GetSize(); i++)
  {
    if (m_tasklib.GetTask(i).GetName() == task) 
    {
      found_reaction->SetTask( m_tasklib.GetTaskReference(i) ); 
      return true;
    }
  }
  
  // If we didn't find the task, then we need to make a new one
  // @JEB currently, this messes up stat tracking to add a task
  // in the middle of a run.
/*  
  // Finish loading in this reaction.
  cString trigger_info = task;
	cString trigger = trigger_info.Pop(':');
  
  // Load the task trigger
  cEnvReqs envreqs;
  tList<cString> errors;
  
  cTaskEntry* cur_task = m_tasklib.AddTask(trigger, trigger_info, envreqs, &errors);
  if (cur_task == NULL || errors.GetSize() > 0) {
    cString* err_str;
    while ((err_str = errors.Pop()) != NULL) {
      cerr << *err_str << endl;
      delete err_str;
    }
    return false;
  }
  
  found_reaction->SetTask(cur_task);      // Attack task to reaction.  
  return true;
*/

  return false;
}

bool cEnvironment::SetResourceInflow(const cString& name, double _inflow )
{
  cResource* found_resource = resource_lib.GetResource(name);
  if (found_resource == NULL) return false;
  found_resource->SetInflow( _inflow );
  return true;
}

bool cEnvironment::SetResourceOutflow(const cString& name, double _outflow )
{
  cResource* found_resource = resource_lib.GetResource(name);
  if (found_resource == NULL) return false;
  found_resource->SetOutflow( _outflow );
  return true;
}

