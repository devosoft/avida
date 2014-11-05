/*
 *  cEnvironment.cc
 *  Avida
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cEnvironment.h"

#include "avida/Avida.h"

#include "avida/core/Feedback.h"
#include "avida/environment/Manager.h"
#include "avida/environment/Product.h"

#include "cArgSchema.h"
#include "cAvidaContext.h"
#include "cContextPhenotype.h"
#include "cContextReactionRequisite.h"
#include "cEnvReqs.h"
#include "cInitFile.h"
#include "cOrganism.h"
#include "cPhenPlastUtil.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cReaction.h"
#include "nReaction.h"
#include "cReactionProcess.h"
#include "cReactionRequisite.h"
#include "cReactionResult.h"
#include "cResource.h"
#include "cStateGrid.h"
#include "cStringUtil.h"
#include "cTaskEntry.h"
#include "cWorld.h"

using namespace Avida;


cEnvironment::cEnvironment(cWorld* world) : m_world(world) , m_tasklib(world),
m_input_size(INPUT_SIZE_DEFAULT), m_output_size(OUTPUT_SIZE_DEFAULT), m_true_rand(false),
m_use_specific_inputs(false), m_specific_inputs(), m_mask(0), m_hammers(false), m_paths(false)
{
  mut_rates.Setup(world);
  if (m_world->GetConfig().DEFAULT_GROUP.Get() != -1) possible_group_ids.insert(m_world->GetConfig().DEFAULT_GROUP.Get());
  pp_fts.Resize(0);
}

cEnvironment::~cEnvironment()
{
  for (int i = 0; i < m_state_grids.GetSize(); i++) delete m_state_grids[i];
}


bool cEnvironment::ParseSetting(cString entry, cString& var_name, cString& var_value, const cString& var_type,
                                Feedback& feedback)
{
  // Make sure we have an actual entry to parse.
  if (entry.GetSize() == 0) {
    feedback.Error("empty setting to parse in %s", (const char*)var_type);
    return false;
  }

  // Collect the values...
  var_name = entry.Pop('=');
  var_value = entry;

  // Make sure we have both a name and a value...
  if (var_name.GetSize() == 0) {
    feedback.Error("no variable povided to set to '%s' in '%s'", (const char*)var_value, (const char*)var_type);
    return false;
  }

  if (var_value.GetSize() == 0) {
    feedback.Error("no value given for '%s' in %s", (const char*)var_name, (const char*)var_type);
    return false;
  }

  // Make the names case insensitive.
  var_name.ToLower();

  return true;
}

bool cEnvironment::AssertInputInt(const cString& input, const cString& name, const cString& type, Feedback& feedback)
{
  if (input.IsNumeric() == false) {
    feedback.Error("in %s, %s set to non-integer", (const char*)type, (const char*)name);
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputDouble(const cString& input, const cString& name, const cString& type, Feedback& feedback)
{
  if (input.IsNumber() == false) {
    feedback.Error("in %s, %s set to non-number", (const char*)type, (const char*)name);
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputBool(const cString& input, const cString& name, const cString& type, Feedback& feedback)
{
  if (input.IsNumber() == false) {
    feedback.Error("in %s, %s set to non-number", (const char*)type, (const char*)name);
    return false;
  }
  int value = input.AsInt();
  if ((value != 1) && (value != 0))  {
    feedback.Error("in %s, %s set to non-bool", (const char*)type, (const char*)name);
    return false;
  }
  return true;
}

bool cEnvironment::AssertInputValid(void* input, const cString& name, const cString& type, const cString& value,
                                    Feedback& feedback)
{
  if (input == NULL) {
    feedback.Error("in %s, '%s' setting of '%s' not found",
                                  (const char*)type, (const char*)name, (const char*)value);
    return false;
  }
  return true;
}

bool cEnvironment::LoadReactionProcess(cReaction* reaction, cString desc, Feedback& feedback)
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
    if (!ParseSetting(var_entry, var_name, var_value, var_type, feedback)) return false;

    // Now that we know we have a variable name and its value, set it!
    if (var_name == "resource") {
      cResource* test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "resource", var_type, var_value, feedback)) {
        return false;
      }
      new_process->SetResource(test_resource);
    }
    else if (var_name == "value") {
      if (!AssertInputDouble(var_value, "value", var_type, feedback)) return false;
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
        feedback.Error("unknown reaction process type '%s' found in '%s'",
                                      (const char*)var_value, (const char*)reaction->GetName());
        return false;
      }
    }
    else if (var_name == "max") {
      if (!AssertInputDouble(var_value, "max", var_type, feedback)) return false;
      new_process->SetMaxNumber(var_value.AsDouble());
    }
    else if (var_name == "min") {
      if (!AssertInputDouble(var_value, "min", var_type, feedback)) return false;
      new_process->SetMinNumber(var_value.AsDouble());
    }
    else if (var_name == "frac") {
      if (!AssertInputDouble(var_value, "frac", var_type, feedback)) return false;
      double in_frac = var_value.AsDouble();
      if (in_frac > 1.0) in_frac = 1.0;
      new_process->SetMaxFraction(in_frac);
    }
    else if (var_name == "ksubm") {
      if (!AssertInputDouble(var_value, "ksubm", var_type, feedback)) return false;
      double in_k_sub_m = var_value.AsDouble();
      new_process->SetKsubM(in_k_sub_m);
    }
    else if (var_name == "product") {
      cResource* test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "product", var_type, var_value, feedback)) {
        return false;
      }
      new_process->SetProduct(test_resource);
    }
    else if (var_name == "conversion") {
      if (!AssertInputDouble(var_value, "conversion", var_type, feedback)) return false;
      new_process->SetConversion(var_value.AsDouble());
    }
    else if (var_name == "inst") {
      new_process->SetInst(var_value);
    }
    else if (var_name == "lethal") {
      if (!AssertInputDouble(var_value, "lethal", var_type, feedback))
        return false;
      new_process->SetLethal(var_value.AsDouble());
    }
    else if (var_name == "sterilize") {
      if (!AssertInputBool(var_value, "sterilize", var_type, feedback))
        return false;
      new_process->SetSterile(var_value.AsInt());
    }
    else if (var_name == "deme") {
      if (!AssertInputDouble(var_value, "demefrac", var_type, feedback))
        return false;
      new_process->SetDemeFraction(var_value.AsDouble());
    }
    else if (var_name == "germline") {
      if (!AssertInputBool(var_value, "germline", var_type, feedback))
        return false;
      new_process->SetIsGermline(var_value.AsInt());
    }
    else if (var_name == "detect") {
      cResource* test_resource = resource_lib.GetResource(var_value);
      if (!AssertInputValid(test_resource, "product", var_type, var_value, feedback)) {
        return false;
      }
      new_process->SetDetect(test_resource);
    }
    else if (var_name == "threshold") {
      if (!AssertInputDouble(var_value, "threshold", var_type, feedback))
        return false;
      new_process->SetDetectionThreshold(var_value.AsDouble());
    }
    else if (var_name == "detectionerror") {
      if (!AssertInputDouble(var_value, "detectionerror", var_type, feedback))
        return false;
      new_process->SetDetectionError(var_value.AsDouble());
    }
    else if (var_name == "string") {
      new_process->SetMatchString(var_value);
    }
    else if (var_name == "depletable") {
      if (!AssertInputBool(var_value, "depletable", var_type, feedback))
        return false;
      new_process->SetDepletable(var_value.AsInt());
    }
    else if (var_name == "phenplastbonus") {
      if (var_value == "nobonus")
        new_process->SetPhenPlastBonusMethod(NO_BONUS);
      else if (var_value == "fracbonus")
        new_process->SetPhenPlastBonusMethod(FRAC_BONUS);
      else if (var_value == "fullbonus")
        new_process->SetPhenPlastBonusMethod(FULL_BONUS);
      else if (var_value == "default")
        new_process->SetPhenPlastBonusMethod(DEFAULT);
      else {
        feedback.Error("invalid setting for phenplastbonus in reaction '%s'", (const char*)reaction->GetName());
        return false;
      }
    }
    else if ( var_name == "internal") {
      if (!AssertInputBool(var_value, "internal", var_type, feedback))
        return false;
      new_process->SetInternal(var_value.AsInt());
    }
    else {
      feedback.Error("unknown process variable '%s' in reaction '%s'",
                                    (const char*)var_name, (const char*)reaction->GetName());
      return false;
    }
  }

  return true;
}

bool cEnvironment::LoadReactionRequisite(cReaction* reaction, cString desc, Feedback& feedback)
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
    if (!ParseSetting(var_entry, var_name, var_value, var_type, feedback)) return false;

    // Now that we know we have a variable name and its value, set it!
    if (var_name == "reaction") {
      cReaction* test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction, "reaction", var_type, var_value, feedback)) {
        return false;
      }
      new_requisite->AddReaction(test_reaction);
    }
    else if (var_name == "noreaction") {
      cReaction* test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction,"noreaction",var_type, var_value, feedback)) {
        return false;
      }
      new_requisite->AddNoReaction(test_reaction);
    }
    else if (var_name == "min_count") {
      if (!AssertInputInt(var_value, "min_count", var_type, feedback)) return false;
      new_requisite->SetMinTaskCount(var_value.AsInt());
    }
    else if (var_name == "max_count") {
      if (!AssertInputInt(var_value, "max_count", var_type, feedback)) return false;
      new_requisite->SetMaxTaskCount(var_value.AsInt());
    }
    else if (var_name == "reaction_min_count") {
      if (!AssertInputInt(var_value, "reaction_min_count", var_type, feedback)) return false;
      new_requisite->SetMinReactionCount(var_value.AsInt());
    }
    else if (var_name == "reaction_max_count") {
      if (!AssertInputInt(var_value, "reaction_max_count", var_type, feedback)) return false;
      new_requisite->SetMaxReactionCount(var_value.AsInt());
    }
    else if (var_name == "divide_only") {
      if (!AssertInputInt(var_value, "divide_only", var_type, feedback)) return false;
      new_requisite->SetDivideOnly(var_value.AsInt());
    }
    else if (var_name == "min_tot_count") {
      if (!AssertInputInt(var_value, "min_tot_count", var_type, feedback)) return false;
      new_requisite->SetMinTotReactionCount(var_value.AsInt());
    }
    else if (var_name == "max_tot_count") {
      if (!AssertInputInt(var_value, "max_tot_count", var_type, feedback)) return false;
      new_requisite->SetMaxTotReactionCount(var_value.AsInt());
    }
    else if (var_name == "parasite_only") {
      if (!AssertInputInt(var_value, "parasite_only", var_type, feedback)) return false;
      new_requisite->SetParasiteOnly(var_value.AsInt());
    }
    else {
      feedback.Error("unknown requisite variable '%s' in reaction '%s'",
                                    (const char*)var_name, (const char*)reaction->GetName());
      return false;
    }
  }

  return true;
}


bool cEnvironment::LoadContextReactionRequisite(cReaction* reaction, cString desc, Feedback& feedback)
{
  cContextReactionRequisite* new_requisite = reaction->AddContextRequisite();

  // Loop through all entries in description.
  while (desc.GetSize() > 0) {
    // Entries are divided by colons.
    cString var_entry = desc.Pop(':');
    cString var_name;
    cString var_value;
    const cString var_type = cStringUtil::Stringf("reaction '%s' requisite", static_cast<const char*>(reaction->GetName()));

    // Parse this entry.
    if (!ParseSetting(var_entry, var_name, var_value, var_type, feedback)) return false;

    // Now that we know we have a variable name and its value, set it!
    if (var_name == "reaction") {
      cReaction* test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction, "reaction", var_type, var_value, feedback)) {
        return false;
      }
      new_requisite->AddReaction(test_reaction);
    }
    else if (var_name == "noreaction") {
      cReaction* test_reaction = reaction_lib.GetReaction(var_value);
      if (!AssertInputValid(test_reaction,"noreaction",var_type, var_value, feedback)) {
        return false;
      }
      new_requisite->AddNoReaction(test_reaction);
    }
    else if (var_name == "min_count") {
      if (!AssertInputInt(var_value, "min_count", var_type, feedback)) return false;
      new_requisite->SetMinTaskCount(var_value.AsInt());
    }
    else if (var_name == "max_count") {
      if (!AssertInputInt(var_value, "max_count", var_type, feedback)) return false;
      new_requisite->SetMaxTaskCount(var_value.AsInt());
    }
    else if (var_name == "reaction_min_count") {
      if (!AssertInputInt(var_value, "reaction_min_count", var_type, feedback)) return false;
      new_requisite->SetMinReactionCount(var_value.AsInt());
    }
    else if (var_name == "reaction_max_count") {
      if (!AssertInputInt(var_value, "reaction_max_count", var_type, feedback)) return false;
      new_requisite->SetMaxReactionCount(var_value.AsInt());
    }
    else if (var_name == "divide_only") {
      if (!AssertInputInt(var_value, "divide_only", var_type, feedback)) return false;
      new_requisite->SetDivideOnly(var_value.AsInt());
    }
    else if (var_name == "min_tot_count") {
      if (!AssertInputInt(var_value, "min_tot_count", var_type, feedback)) return false;
      new_requisite->SetMinTotReactionCount(var_value.AsInt());
    }
    else if (var_name == "max_tot_count") {
      if (!AssertInputInt(var_value, "max_tot_count", var_type, feedback)) return false;
      new_requisite->SetMaxTotReactionCount(var_value.AsInt());
    }
    else if (var_name == "parasite_only") {
      if (!AssertInputInt(var_value, "parasite_only", var_type, feedback)) return false;
      new_requisite->SetParasiteOnly(var_value.AsInt());
    }
    else {
      feedback.Error("unknown requisite variable '%s' in reaction '%s'",
                                    (const char*)var_name, (const char*)reaction->GetName());
      return false;
    }
  }

  return true;
}



bool cEnvironment::LoadResource(cString desc, Feedback& feedback)
{
  if (desc.GetSize() == 0) {
    feedback.Warning("resource line with no resources listed");
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
      if (!ParseSetting(var_entry, var_name, var_value, var_type, feedback)) {
        return false;
      }

      if (var_name == "inflow") {
        if (!AssertInputDouble(var_value, "inflow", var_type, feedback)) return false;
        new_resource->SetInflow( var_value.AsDouble() );
      }
      else if (var_name == "outflow") {
        if (!AssertInputDouble(var_value, "outflow", var_type, feedback)) return false;
        new_resource->SetOutflow( var_value.AsDouble() );
      }
      else if (var_name == "initial") {
        if (!AssertInputDouble(var_value, "initial", var_type, feedback)) return false;
        new_resource->SetInitial( var_value.AsDouble() );
      }
      else if (var_name == "geometry") {
        if (!new_resource->SetGeometry( var_value )) {
          feedback.Error("in %s, %s unknown geometry", (const char*)var_type, (const char*)var_value);
          return false;
        }
      }
      else if (var_name == "cells")
      {
        Apto::Array<int> cell_list = cStringUtil::ReturnArray(var_value);
        new_resource->SetCellIdList(cell_list);
      }
      else if (var_name == "inflowx1" || var_name == "inflowx") {
        if (!AssertInputInt(var_value, "inflowX1", var_type, feedback)) return false;
        new_resource->SetInflowX1( var_value.AsInt() );
      }
      else if (var_name == "inflowx2") {
        if (!AssertInputInt(var_value, "inflowX2", var_type, feedback)) return false;
        new_resource->SetInflowX2( var_value.AsInt() );
      }
      else if (var_name == "inflowy1" || var_name == "inflowy") {
        if (!AssertInputInt(var_value, "inflowY1", var_type, feedback)) return false;
        new_resource->SetInflowY1( var_value.AsInt() );
      }
      else if (var_name == "inflowy2") {
        if (!AssertInputInt(var_value, "inflowY2", var_type, feedback)) return false;
        new_resource->SetInflowY2( var_value.AsInt() );
      }
      else if (var_name == "outflowx1" || var_name == "outflowx") {
        if (!AssertInputInt(var_value, "outflowX1", var_type, feedback)) return false;
        new_resource->SetOutflowX1( var_value.AsInt() );
      }
      else if (var_name == "outflowx2") {
        if (!AssertInputInt(var_value, "outflowX2", var_type, feedback)) return false;
        new_resource->SetOutflowX2( var_value.AsInt() );
      }
      else if (var_name == "outflowy1" || var_name == "outflowy") {
        if (!AssertInputInt(var_value, "outflowY1", var_type, feedback)) return false;
        new_resource->SetOutflowY1( var_value.AsInt() );
      }
      else if (var_name == "outflowy2") {
        if (!AssertInputInt(var_value, "outflowY2", var_type, feedback)) return false;
        new_resource->SetOutflowY2( var_value.AsInt() );
      }
      else if (var_name == "xdiffuse") {
        if (!AssertInputDouble(var_value, "xdiffuse", var_type, feedback)) return false;
        new_resource->SetXDiffuse( var_value.AsDouble() );
      }
      else if (var_name == "xgravity") {
        if (!AssertInputDouble(var_value, "xgravity", var_type, feedback)) return false;
        new_resource->SetXGravity( var_value.AsDouble() );
      }
      else if (var_name == "ydiffuse") {
        if (!AssertInputDouble(var_value, "ydiffuse", var_type, feedback)) return false;
        new_resource->SetYDiffuse( var_value.AsDouble() );
      }
      else if (var_name == "ygravity") {
        if (!AssertInputDouble(var_value, "ygravity", var_type, feedback)) return false;
        new_resource->SetYGravity( var_value.AsDouble() );
      }
      else if (var_name == "deme") {
        if (!new_resource->SetDemeResource( var_value )) {
          feedback.Error("in %s, %s must be true or false", (const char*)var_type, (const char*)var_value);
          return false;
        }
      }
      else if (var_name == "collectable") {
        if (!AssertInputBool(var_value, "collectable", var_type, feedback)) return false;
          new_resource->SetCollectable(var_value.AsInt());
      }
      else if (var_name == "energy") {
        if (!new_resource->SetEnergyResource( var_value )) {
          feedback.Error("in %s, %s must be true or false", (const char*)var_type, (const char*)var_value);
          return false;
        } else if (m_world->GetConfig().ENERGY_ENABLED.Get() == 0) {
          feedback.Error("energy resources can not be used without the energy model");
          return false;
        }
      }
      else if (var_name == "hgt") {
	// this resource is for HGT -- corresponds to genome fragments present in cells.
	if (!AssertInputBool(var_value, "hgt", var_type, feedback)) return false;
	new_resource->SetHGTMetabolize(var_value.AsInt());
      }
      else {
        feedback.Error("unknown variable '%s' in resource '%s'", (const char*)var_name, (const char*)name);
        return false;
      }
    }
    
    // Now that all geometry, etc. information is known, give the resource an index
    // within its own type
    resource_lib.SetResourceIndex(new_resource);
    
    // Prevent misconfiguration of HGT:
    
    if (new_resource->GetHGTMetabolize() &&
       ( (new_resource->GetGeometry() != nGeometry::GLOBAL)
	 || (new_resource->GetInitial() > 0.0)
	 || (new_resource->GetInflow() > 0.0)
	 || (new_resource->GetOutflow() > 0.0)
	 || (new_resource->GetInflowX1() != -99)
	 || (new_resource->GetInflowX2() != -99)
	 || (new_resource->GetInflowY1() != -99)
	 || (new_resource->GetInflowY2() != -99)
	 || (new_resource->GetXDiffuse() != 1.0)
	 || (new_resource->GetXGravity() != 0.0)
	 || (new_resource->GetYDiffuse() != 1.0)
	 || (new_resource->GetYGravity() != 0.0)
	 || (new_resource->GetDemeResource() != false))) {
      feedback.Error("misconfigured HGT resource: %s", (const char*)name);
      return false;
    }
    if (new_resource->GetHGTMetabolize() && !m_world->GetConfig().ENABLE_HGT.Get()) {
      feedback.Error("resource configured to use HGT, but HGT not enabled");
      return false;
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

bool cEnvironment::LoadCell(cString desc, Feedback& feedback)

/*****************************************************************************
 Routine to read in spatial resources loaded in one cell at a time. Syntax:

 CELL resource_name:cell_list[:options]

 where options are initial, inflow and outflow
 *****************************************************************************/

{
  if (desc.GetSize() == 0) {
    feedback.Warning("CELL line with no resources listed");
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
    Apto::Array<int> cell_list = cStringUtil::ReturnArray(cell_list_str);
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
      if (!ParseSetting(var_entry, var_name, var_value, var_type, feedback)) {
        return false;
      }

      if (var_name == "inflow") {
        if (!AssertInputDouble(var_value, "inflow", var_type, feedback)) return false;
        tmp_inflow = var_value.AsDouble();
      }
      else if (var_name == "outflow") {
        if (!AssertInputDouble(var_value, "outflow", var_type, feedback)) return false;
        tmp_outflow = var_value.AsDouble();
      }
      else if (var_name == "initial") {
        if (!AssertInputDouble(var_value, "initial", var_type, feedback)) return false;
        tmp_initial = var_value.AsDouble();
      }
      else {
        feedback.Error("unknown variable '%s' in resource '%s'", (const char*)var_name, (const char*)name);
        return false;
      }
    }
    for (int i = 0; i < cell_list.GetSize(); i++) {
      if (cCellResource *CellResourcePtr = this_resource->GetCellResourcePtr(cell_list[i])) {
        this_resource->UpdateCellResource(CellResourcePtr,tmp_initial, tmp_inflow, tmp_outflow);
      } else {
        cCellResource tmp_cell_resource(cell_list[i],tmp_initial, tmp_inflow, tmp_outflow);
        this_resource->AddCellResource(tmp_cell_resource);
      }
    }

  }

  return true;
}

bool cEnvironment::LoadReaction(cString desc, Feedback& feedback)
{
  // Make sure this reaction has a description...
  if (desc.GetSize() == 0) {
    feedback.Error("each reaction must include a name and trigger");
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
    feedback.Warning("re-defining reaction '%s'", (const char*)name);
  }

  // Finish loading in this reaction.
  cString trigger_info = desc.PopWord();
  cString trigger = trigger_info.Pop(':');

  // Load the task trigger
  cEnvReqs envreqs;
  cTaskEntry* cur_task = m_tasklib.AddTask(trigger, trigger_info, envreqs, feedback);
  if (cur_task == NULL) return false;
  new_reaction->SetTask(cur_task);      // Attack task to reaction.
  Environment::ManagerPtr env = Environment::Manager::Of(m_world->GetNewWorld());
  env->DefineActionTrigger((const char*)cur_task->GetName(), (const char*)cur_task->GetDesc(), Environment::ConstProductPtr(),
                           m_tasklib.GetSize() - 1);

  while (desc.GetSize()) {
    cString desc_entry = desc.PopWord();      // Get the next argument
    cString entry_type = desc_entry.Pop(':'); // Determine argument type
    entry_type.ToLower();                     // Make case insensitive.

    // Determine the type of each argument and process it.
    if (entry_type == "process") {
      if (LoadReactionProcess(new_reaction, desc_entry, feedback) == false) {
        feedback.Error("failed in loading reaction-process...");
        return false;
      }
    }
    else if (entry_type == "requisite") {
      if (LoadReactionRequisite(new_reaction, desc_entry, feedback) == false) {
        feedback.Error("failed in loading reaction-requisite...");
        return false;
      }
    }
    else if (entry_type == "context_requisite") {
      if (LoadContextReactionRequisite(new_reaction, desc_entry, feedback) == false) {
        feedback.Error("failed in loading reaction-requisite...");
        return false;
      }
    }
    else {
      feedback.Error("unknown entry type '%s' in reaction '%s'", (const char*)entry_type, (const char*)name);
      return false;
    }
  }

  // Process the environment requirements of this task
  if (envreqs.GetMinInputs() > m_input_size) m_input_size = envreqs.GetMinInputs();
  if (envreqs.GetMinOutputs() > m_output_size) m_output_size = envreqs.GetMinOutputs();
  if (envreqs.GetTrueRandInputs()) m_true_rand = true;

  return true;
}

bool cEnvironment::LoadGradientResource(cString desc, Feedback& feedback) 
{
  if (desc.GetSize() == 0) {
    feedback.Error("gradient resource line with no resources listed");
    return false;
  }
  
  while (desc.GetSize() > 0) {
    cString cur_resource = desc.PopWord();
    const cString name = cur_resource.Pop(':');
    
    /* If resource does not already exist create it, however if it already
     exists (for instance was created as a cell resource) return an error*/
    
    cResource* new_resource;
    if (!resource_lib.DoesResourceExist(name)) {
      new_resource = resource_lib.AddResource(name);
    } else {
      new_resource = resource_lib.GetResource(name);
    }
    
    new_resource->SetGeometry("grid");
    new_resource->SetInitial(0.0);
    new_resource->SetOutflow(1.0);
    new_resource->SetXDiffuse(0.0);
    new_resource->SetYDiffuse(0.0);
    new_resource->SetXGravity(0.0);
    new_resource->SetYGravity(0.0);
    new_resource->SetGradient(true);
    
    while (cur_resource.GetSize() != 0) {
      cString var_entry = cur_resource.Pop(':');
      cString var_name;
      cString var_value;
      const cString var_type = cStringUtil::Stringf("gradient resource '%s'", static_cast<const char*>(name));
      // Parse this entry.
      if (!ParseSetting(var_entry, var_name, var_value, var_type, feedback)) {
        return false;
      }
      
      if (var_name == "peakx") {
        if (!AssertInputInt(var_value, "peakx", var_type, feedback)) return false;
        new_resource->SetPeakX( var_value.AsInt() );
      }
      else if (var_name == "peaky") {
        if (!AssertInputInt(var_value, "peaky", var_type, feedback)) return false;
        new_resource->SetPeakY( var_value.AsInt() );
      }
      else if (var_name == "height") {
        if (!AssertInputInt(var_value, "height", var_type, feedback)) return false;
        new_resource->SetHeight( var_value.AsInt() );
      }
      else if (var_name == "spread") {
        if (!AssertInputInt(var_value, "spread", var_type, feedback)) return false;
        new_resource->SetSpread( var_value.AsInt() );
      }
      else if (var_name == "plateau") {
        if (!AssertInputDouble(var_value, "plateau", var_type, feedback)) return false;
        new_resource->SetPlateau( var_value.AsDouble() );
      }
      else if (var_name == "decay") {
        if (!AssertInputInt(var_value, "decay", var_type, feedback)) return false;
        new_resource->SetDecay( var_value.AsInt() );
      }
      else if (var_name == "max_x") {
        if (!AssertInputInt(var_value, "max_x", var_type, feedback)) return false;
        new_resource->SetMaxX( var_value.AsInt() );
      }
      else if (var_name == "max_y") {
        if (!AssertInputInt(var_value, "max_y", var_type, feedback)) return false;
        new_resource->SetMaxY( var_value.AsInt() );
      }
      else if (var_name == "min_x") {
        if (!AssertInputInt(var_value, "min_x", var_type, feedback)) return false;
        new_resource->SetMinX( var_value.AsInt() );
      }
      else if (var_name == "min_y") {
        if (!AssertInputInt(var_value, "min_y", var_type, feedback)) return false;
        new_resource->SetMinY( var_value.AsInt() );
      }
      else if (var_name == "move_a_scaler") {
        if (!AssertInputDouble(var_value, "move_a_scaler", var_type, feedback)) return false;
        new_resource->SetAscaler( var_value.AsDouble() );
      }
      else if (var_name == "updatestep") {
        if (!AssertInputInt(var_value, "updatestep", var_type, feedback)) return false;
        new_resource->SetUpdateStep( var_value.AsInt() );
      }
      else if (var_name == "halo") {
        if (!AssertInputInt(var_value, "halo", var_type, feedback)) return false;
        new_resource->SetHalo( var_value.AsInt() );
      }
      else if (var_name == "halo_inner_radius") {
        if (!AssertInputInt(var_value, "halo_inner_radius", var_type, feedback)) return false;
        new_resource->SetHaloInnerRadius( var_value.AsInt() );
      }
      else if (var_name == "halo_anchor_x") {
        if (!AssertInputInt(var_value, "halo_halo_anchor_x", var_type, feedback)) return false;
        new_resource->SetHaloAnchorX( var_value.AsInt() );
      }
      else if (var_name == "halo_anchor_y") {
        if (!AssertInputInt(var_value, "halo_halo_anchor_y", var_type, feedback)) return false;
        new_resource->SetHaloAnchorY( var_value.AsInt() );
      }
      else if (var_name == "move_speed") {
        if (!AssertInputInt(var_value, "move_speed", var_type, feedback)) return false;
        new_resource->SetMoveSpeed( var_value.AsInt() );
      }
      else if (var_name == "move_resistance") {
        if (!AssertInputInt(var_value, "move_resistance", var_type, feedback)) return false;
        new_resource->SetMoveResistance( var_value.AsInt() );
      }
      else if (var_name == "halo_width") {
        if (!AssertInputInt(var_value, "halo_width", var_type, feedback)) return false;
        new_resource->SetHaloWidth( var_value.AsInt() );
      }
      else if (var_name == "plateau_inflow") {
        if (!AssertInputDouble(var_value, "plateau_inflow", var_type, feedback)) return false;
        new_resource->SetPlateauInflow( var_value.AsDouble() );
      }      
      else if (var_name == "plateau_outflow") {
        if (!AssertInputDouble(var_value, "plateau_outflow", var_type, feedback)) return false;
        new_resource->SetPlateauOutflow( var_value.AsDouble() );
      } 
      else if (var_name == "cone_inflow") {
        if (!AssertInputDouble(var_value, "cone_inflow", var_type, feedback)) return false;
        new_resource->SetConeInflow( var_value.AsDouble() );
      }      
      else if (var_name == "cone_outflow") {
        if (!AssertInputDouble(var_value, "cone_outflow", var_type, feedback)) return false;
        new_resource->SetConeOutflow( var_value.AsDouble() );
      } 
      else if (var_name == "gradient_inflow") {
        if (!AssertInputDouble(var_value, "gradient_inflow", var_type, feedback)) return false;
        new_resource->SetGradientInflow( var_value.AsDouble() );
      } 
      else if (var_name == "initial") {
        if (!AssertInputDouble(var_value, "initial", var_type, feedback)) return false;
        new_resource->SetPlatInitial( var_value.AsDouble() );
      } 
      else if (var_name == "common") {
        if (!AssertInputInt(var_value, "common", var_type, feedback)) return false;
        new_resource->SetIsPlateauCommon( var_value.AsInt() );
      } 
      else if (var_name == "floor") {
        if (!AssertInputDouble(var_value, "floor", var_type, feedback)) return false;
        new_resource->SetFloor( var_value.AsDouble() );
      } 
      else if (var_name == "habitat") {
        if (!AssertInputInt(var_value, "habitat", var_type, feedback)) return false;
        new_resource->SetHabitat( var_value.AsInt() );
        AddHabitat(var_value.AsInt());
      } 
      else if (var_name == "min_size") {
        if (!AssertInputInt(var_value, "min_size", var_type, feedback)) return false;
        new_resource->SetMinSize( var_value.AsInt() );
      } 
      else if (var_name == "max_size") {
        if (!AssertInputInt(var_value, "max_size", var_type, feedback)) return false;
        new_resource->SetMaxSize( var_value.AsInt() );
      } 
      else if (var_name == "config") {
        if (!AssertInputInt(var_value, "config", var_type, feedback)) return false;
        new_resource->SetConfig( var_value.AsInt() );
      } 
      else if (var_name == "count") {
        if (!AssertInputInt(var_value, "count", var_type, feedback)) return false;
        new_resource->SetCount( var_value.AsInt() );
      } 
      else if (var_name == "resistance") {
        if (!AssertInputDouble(var_value, "resistance", var_type, feedback)) return false;
        new_resource->SetResistance( var_value.AsDouble() );
      } 
      else if (var_name == "damage") {
        if (!AssertInputDouble(var_value, "damage", var_type, feedback)) return false;
        new_resource->SetDamage( var_value.AsDouble() );
      } 
      else if (var_name == "deadly") {
        if (!AssertInputDouble(var_value, "deadly", var_type, feedback)) return false;
        new_resource->SetDeadly( var_value.AsDouble() );
      }
      else if (var_name == "path") {
        if (!AssertInputInt(var_value, "path", var_type, feedback)) return false;
        new_resource->SetPath( var_value.AsInt() );
        m_paths = true;
      }
      else if (var_name == "hammer") {
        if (!AssertInputInt(var_value, "hammer", var_type, feedback)) return false;
        new_resource->SetHammer( var_value.AsInt() );
        m_hammers = true;
      }
      else if (var_name == "threshold") {
        if (!AssertInputDouble(var_value, "threshold", var_type, feedback)) return false;
        new_resource->SetThreshold( var_value.AsDouble() );
      } 
      else if (var_name == "refuge") {
        if (!AssertInputInt(var_value, "refuge", var_type, feedback)) return false;
        new_resource->SetRefuge( var_value.AsInt() );
      } 
      else {
        feedback.Error("unknown variable '%s' in gradient resource '%s'",
                                      (const char*)var_name, (const char*)name);
        return false;
      }
    }
  }
  
  return true;  
}

bool cEnvironment::LoadStateGrid(cString desc, Feedback& feedback)
{
  // First component is the name
  cString name = desc.Pop(':');

  cArgSchema schema(':','=');

  // Integer Arguments
  schema.AddEntry("width", 0, 0, INT_MAX);
  schema.AddEntry("height", 1, 0, INT_MAX);
  schema.AddEntry("initx", 2, 0, INT_MAX);
  schema.AddEntry("inity", 3, 0, INT_MAX);
  schema.AddEntry("initfacing", 4, 0, 7);

  // String Arguments
  schema.AddEntry("states", 0, cArgSchema::SCHEMA_STRING);
  schema.AddEntry("grid", 1, cArgSchema::SCHEMA_STRING);

  // Load the Arguments
  Apto::SmartPtr<cArgContainer> args(cArgContainer::Load(desc, schema, feedback));

  // Check for errors loading the arguments
  if (!args) return false;

  // Extract and validate the arguments
  int width = args->GetInt(0);
  int height = args->GetInt(1);
  int initx = args->GetInt(2);
  int inity = args->GetInt(3);
  int initfacing = args->GetInt(4);
  
  if (initx >= width || inity >= height) {
    feedback.Error("initx and inity must not exceed (width - 1) and (height - 1)");
    return false;
  }


  // Load the states
  cString statename;
  cString statesensestr;

  Apto::Array<cString> states;
  Apto::Array<int> state_sense;
  cString statestr = args->GetString(0);
  statestr.Trim();
  while (statestr.GetSize()) {
    statesensestr = statestr.Pop(',');
    statename = statesensestr.Pop('=');
    statename.Trim();

    // Check for duplicate state definition
    for (int i = 0; i < states.GetSize(); i++) {
      if (statename == states[i]) {
        feedback.Error("duplicate state identifier for state grid %s", (const char*)name);
        return false;
      }
    }

    // Add state to the collection
    states.Push(statename);

    // Determing the value returned when sense operations are run on this state
    int state_sense_value = states.GetSize(); // Default value is the order in which the states are loaded
    if (statesensestr.GetSize()) state_sense_value = statesensestr.AsInt();
    state_sense.Push(state_sense_value);
  }
  if (states.GetSize() == 0) {
    feedback.Error("no states defined for state grid %s", (const char*)name);
    return false;
  }

  // Load the state grid itself
  Apto::Array<int> lgrid(width * height);
  cString gridstr = args->GetString(1);
  int cell = 0;
  while (gridstr.GetSize() && cell < lgrid.GetSize()) {
    statename = gridstr.Pop(',');
    statename.Trim();
    bool found = false;
    for (int i = 0; i < states.GetSize(); i++) {
      if (statename == states[i]) {
        lgrid[cell++] = i;
        found = true;
        break;
      }
    }
    if (!found) {
      feedback.Error("state identifier undefined for cell (%d, %d) in state grid %s",
                                    (cell / width), (cell % width), (const char*)name);
      return false;
    }
  }
  if (cell != lgrid.GetSize() || gridstr.GetSize() > 0) {
    feedback.Error("grid definition size mismatch for state grid %s", (const char*)name);
    return false;
  }

  // Invert row ordering so that it is interpreted as the highest indexed row comes first.  i.e. -
  // | a a |
  // | b a |
  // would be a,a,b,a
  Apto::Array<int> grid(lgrid.GetSize());
  for (int y = 0; y < height; y++) {
    int off = y * width;
    int loff = (height - y - 1) * width;
    for (int x = 0; x < width; x++) {
      grid[off + x] = lgrid[loff + x];
    }
  }
  
  m_state_grids.Push(new cStateGrid(name, width, height, initx, inity, initfacing, states, state_sense, grid));
  return true;
}

bool cEnvironment::LoadSetActive(cString desc, Feedback& feedback)
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
      feedback.Error("unknown REACTION: '%s'", (const char*)item_name);
      return false;
    }
    cur_reaction->SetActive(new_active);
  } else if (item_type == "") {
    feedback.Notify("format: SET_ACTIVE <type> <name> <new_status=true>");
  } else {
    feedback.Error("cannot deactivate items of type %s", (const char*)item_type);
    return false;
  }

  return true;
}

bool cEnvironment::LoadLine(cString line, Feedback& feedback)

/* Routine to read in a line from the enviroment file and hand that line
 line to the approprate routine to process it.                         */
{
  cString type = line.PopWord();      // Determine type of this entry.
  type.ToUpper();                     // Make type case insensitive.

  bool load_ok = true;
  if (type == "RESOURCE") load_ok = LoadResource(line, feedback);
  else if (type == "REACTION") load_ok = LoadReaction(line, feedback);
  else if (type == "SET_ACTIVE") load_ok = LoadSetActive(line, feedback);
  else if (type == "CELL") load_ok = LoadCell(line, feedback);
  else if (type == "GRID") load_ok = LoadStateGrid(line, feedback);
  else if (type == "GRADIENT_RESOURCE") load_ok = LoadGradientResource(line, feedback); 
  else {
    feedback.Error("unknown environment keyword '%s'", (const char*)type);
    return false;
  }

  if (load_ok == false) {
    feedback.Error("failed in loading '%s'", (const char*)type);
    return false;
  }

  return true;
}

bool cEnvironment::Load(const cString& filename, const cString& working_dir, Feedback& feedback, const Apto::Map<Apto::String, Apto::String>* defs)
{
  cInitFile infile(filename, working_dir, NULL, defs);
  if (!infile.WasOpened()) {
    for (int i = 0; i < infile.GetFeedback().GetNumMessages(); i++) {
      switch (infile.GetFeedback().GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:
          feedback.Error(infile.GetFeedback().GetMessage(i));
          break;
        case cUserFeedback::UF_WARNING:
          feedback.Warning(infile.GetFeedback().GetMessage(i));
          break;
        default:
          feedback.Notify(infile.GetFeedback().GetMessage(i));
      }
    }
    feedback.Error("failed to load environment '%s'", (const char*)filename);
    return false;
  }

  for (int line_id = 0; line_id < infile.GetNumLines(); line_id++) {
    // Load the next line from the file.
    bool load_ok = LoadLine(infile.GetLine(line_id), feedback);
    if (load_ok == false) return false;
  }

  // Make sure that all pre-declared reactions have been loaded correctly.
  for (int i = 0; i < reaction_lib.GetSize(); i++) {
    if (reaction_lib.GetReaction(i)->GetTask() == NULL) {
      feedback.Error("pre-declared reaction '%s' never defined",
                                    (const char*)reaction_lib.GetReaction(i)->GetName());
      return false;
    }
  }

  return true;
}


void cEnvironment::SetupInputs(cAvidaContext& ctx, Apto::Array<int>& input_array, bool random) const
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


void cEnvironment::SwapInputs(cAvidaContext&, Apto::Array<int>& src_input_array, Apto::Array<int>& dest_input_array) const
{
  Apto::Array<int> tmp_input_array = dest_input_array;

  dest_input_array = src_input_array;
  src_input_array = tmp_input_array;
}


bool cEnvironment::TestInput(cReactionResult&, const tBuffer<int>&, const tBuffer<int>&, const Apto::Array<double>&) const
{
  // @CAO nothing for the moment...
  return false;
}


bool cEnvironment::TestOutput(cAvidaContext& ctx, cReactionResult& result,
                              cTaskContext& taskctx, const Apto::Array<int>& task_count,
                              Apto::Array<int>& reaction_count,
                              const Apto::Array<double>& resource_count,
                              const Apto::Array<double>& rbins_count,
                              bool is_parasite, cContextPhenotype* context_phenotype) const
{
  //flag to skip processing of parasite tasks
  bool skipProcessing = false;
  
  if (is_parasite && m_world->GetConfig().PARASITE_SKIP_REACTIONS.Get())
    skipProcessing = true;

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
    if (TestRequisites(taskctx, cur_reaction, task_cnt, reaction_count, on_divide, is_parasite) == false) {
      if (!skipProcessing){
        continue;
      }
    }

    if (context_phenotype != 0) {
      Apto::Array<int> blank_tasks;
      Apto::Array<int> blank_reactions;
      blank_tasks.ResizeClear(task_count.GetSize());
      for(int count=0;count<task_count.GetSize();count++) {
        blank_tasks[count] = 0;
      }
      blank_reactions.ResizeClear(this->GetReactionLib().GetSize());
      for(int count=0;count<reaction_count.GetSize();count++) {
        blank_reactions[count] = 0;
      }
      context_phenotype->AddTaskCounts(blank_tasks.GetSize(), blank_tasks);
      context_phenotype->AddReactionCounts(blank_reactions.GetSize(), blank_reactions);
      int context_task_count = context_phenotype->GetTaskCounts()[task_id];
      if (TestContextRequisites(cur_reaction, context_task_count, context_phenotype->GetReactionCounts(), on_divide) == false) {
        if (!skipProcessing) {  // for those parasites again
          continue;
        }
      }
    }

    const double task_quality = m_tasklib.TestOutput(taskctx);
    assert(task_quality >= 0.0);

    // If this task wasn't performed, move on to the next one.

    // @MRR task_probability will be either the probability [0,1] for the task or it will
    // be -1.0 if the value is not needed for this reaction.
    bool force_mark_task = false; //@MRR Some phenplastbonus settings will force a task to be counted even if it isn't demonstrated.
    double task_probability = GetTaskProbability(ctx, taskctx, cur_reaction->GetProcesses(), force_mark_task);

    if (task_quality == 0.0 && !force_mark_task) continue;

    // Mark this task as performed...
    result.MarkTask(task_id, task_quality, taskctx.GetTaskValue());

    if (!skipProcessing) {
      // And let's process it!
      DoProcesses(ctx, cur_reaction->GetProcesses(), resource_count, rbins_count,
                  task_quality, task_probability, task_cnt, i, result, taskctx);
      
      if (result.ReactionTriggered(i) == true) {
        reaction_count[i]++;
        taskctx.GetOrganism()->GetPhenotype().SetFirstReactionCycle(i);
        taskctx.GetOrganism()->GetPhenotype().SetFirstReactionExec(i);
      }
      // Note: the reaction is actually marked as being performed inside DoProcesses.
    }
  }

  return result.GetActive();
}

bool cEnvironment::TestRequisites(cTaskContext& taskctx, const cReaction* cur_reaction,
                                  int task_count, const Apto::Array<int>& reaction_count, const bool on_divide, bool is_parasite) const
{
  const tList<cReactionRequisite>& req_list = cur_reaction->GetRequisites();
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
    
    if (taskctx.GetOrganism()) {
      // Have all reactions been met?
      const Apto::Array<int>& stolen_reactions = taskctx.GetOrganism()->GetPhenotype().GetStolenReactionCount();
      tLWConstListIterator<cReaction> reaction_it(cur_req->GetReactions());
      while (reaction_it.Next() != NULL) {
        int react_id = reaction_it.Get()->GetID();
        if (reaction_count[react_id] == 0 && stolen_reactions[react_id] == 0) {   
          satisfied = false;
          break;
        }
      }
    }
    // If being called as a deme reaction..
    else {
      tLWConstListIterator<cReaction> reaction_it(cur_req->GetReactions());
      while (reaction_it.Next() != NULL) {
        int react_id = reaction_it.Get()->GetID();
        if (reaction_count[react_id] == 0) {
          satisfied = false;
          break;
        }
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
    
    // Have all reaction counts been met?
    if (reaction_count[cur_reaction->GetID()] < cur_req->GetMinReactionCount()) continue;
    if (reaction_count[cur_reaction->GetID()] >= cur_req->GetMaxReactionCount()) continue;
    
    // Have all total reaction counts been met?
    int tot_reactions = 0;
    for (int i=0; i<reaction_count.GetSize(); i++) {
      tot_reactions += reaction_count[i];
    }
    if (tot_reactions < cur_req->GetMinTotReactionCount()) continue;
    if (tot_reactions >= cur_req->GetMaxTotReactionCount()) continue;
    

    // Have divide task reqs been met?
    // If div_type is 0 we only check on IO, if 1 we only check on divide,
    // if 2 we check always
    int div_type = cur_req->GetDivideOnly();
    if (div_type == 1 && !on_divide) continue;
    if (div_type == 0 && on_divide) continue;
    
    // If the reaction is parasite only, check to see if we are a parasite
    if (cur_req->GetParasiteOnly()){
      if (!is_parasite) continue;}

    return true;
  }

  return false;
}


bool cEnvironment::TestContextRequisites(const cReaction* cur_reaction,
					 int task_count, const Apto::Array<int>& reaction_count,
					 const bool on_divide) const
{
  const tList<cContextReactionRequisite>& req_list = cur_reaction->GetContextRequisites();
  const int num_reqs = req_list.GetSize();

  // If there are no requisites, there is nothing to meet!
  // (unless this is a check upon dividing, in which case we want the default to be to not check the task
  // and only if the requisite has been added to check it
  if (num_reqs == 0) {
    return !on_divide;
  }

  tLWConstListIterator<cContextReactionRequisite> req_it(req_list);
  for (int i = 0; i < num_reqs; i++) {
    // See if this requisite batch can be satisfied.
    const cContextReactionRequisite* cur_req = req_it.Next();
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
    
    // Have all reaction counts been met?
    if (reaction_count[cur_reaction->GetID()] < cur_req->GetMinReactionCount()) continue;
    if (reaction_count[cur_reaction->GetID()] >= cur_req->GetMaxReactionCount()) continue;
    
    // Have all total reaction counts been met?
    int tot_reactions = 0;
    for (int i=0; i<reaction_count.GetSize(); i++) {
      tot_reactions += reaction_count[i];
    }
    if (tot_reactions < cur_req->GetMinTotReactionCount()) continue;
    if (tot_reactions >= cur_req->GetMaxTotReactionCount()) continue;
    
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




double cEnvironment::GetTaskProbability(cAvidaContext& ctx, cTaskContext& taskctx,
                                        const tList<cReactionProcess>& req_proc, bool& force_mark_task) const
{
  force_mark_task = false;
  if (ctx.GetTestMode()) { //If we're in test-cpu mode, do not do this.
    return -1.0;
  }

  double task_prob = -1.0;
  tLWConstListIterator<cReactionProcess> proc_it(req_proc);
  cReactionProcess* cur_proc;
  bool test_plasticity = false;
  while ( (cur_proc = proc_it.Next()) != NULL){  //Determine whether or not we need to test for plastcity
    ePHENPLAST_BONUS_METHOD pp_meth = cur_proc->GetPhenPlastBonusMethod();
    if (pp_meth != DEFAULT){  //DEFAULT doesn't modify bonuses
      test_plasticity = true;
      if (pp_meth == FULL_BONUS || pp_meth == FRAC_BONUS)  //These will require us to force a task to be marked
        force_mark_task = true;
    }
  }
  if (test_plasticity){  //We have to test for plasticity, so try to get it
    int task_id = taskctx.GetTaskEntry()->GetID();
    task_prob = cPhenPlastUtil::GetTaskProbability(ctx, m_world, taskctx.GetOrganism()->SystematicsGroup("genotype"), task_id);
  }
  force_mark_task = force_mark_task && (task_prob > 0.0);  //If the task isn't demonstrated, we don't need to worry about marking it.
  return task_prob;
}



void cEnvironment::DoProcesses(cAvidaContext& ctx, const tList<cReactionProcess>& process_list,
                               const Apto::Array<double>& resource_count, const Apto::Array<double>& rbins_count,
                               const double task_quality, const double task_probability, const int task_count,
                               const int reaction_id, cReactionResult& result, cTaskContext& taskctx) const
{
  const int num_process = process_list.GetSize();
  
  tLWConstListIterator<cReactionProcess> process_it(process_list);
  for (int i = 0; i < num_process; i++) {
    // See if this requisite batch can be satisfied.
    const cReactionProcess* cur_process = process_it.Next();
    const double max_consumed = cur_process->GetMaxNumber();
    const double min_consumed = cur_process->GetMinNumber();

    ePHENPLAST_BONUS_METHOD pp_meth = cur_process->GetPhenPlastBonusMethod();
    const double task_plasticity_modifier =
    (pp_meth == NO_BONUS && task_probability != 1.0) ? 0.0 :
    (pp_meth == FRAC_BONUS) ? task_probability : 1.0;

    //Phenplast full bonus will use a 1.0 task quality
    const double local_task_quality =
    (pp_meth == FULL_BONUS || pp_meth == FRAC_BONUS) ? 1.0 : task_quality;

    // Determine resource consumption
    double consumed = 0.0;
    cResource* in_resource = cur_process->GetResource();

    if (in_resource == NULL) {
      // Test if infinite resource
      consumed = max_consumed * local_task_quality * task_plasticity_modifier;

    } else if (in_resource->GetHGTMetabolize()) {
      /* HGT Metabolism
       This bit of code is triggered when ENABLE_HGT=1 and a resource has hgt=1.
       Here's the idea: Each cell in the environment holds a buffer of genome fragments,
       where these fragments are drawn from the remains of organisms that have died.
       These remains are a potential source of energy to the current inhabitant of the
       cell.  This code metabolizes one of those fragments by pretending that it's just
       another resource.  Task quality can be used to control the conversion of fragments
       to bonus, but the amount of resource consumed is always equal to the length of the
       fragment.
       */
      int cellid = taskctx.GetOrganism()->GetCellID();
      if (cellid != -1) { // can't do this in the test cpu
        cPopulationCell& cell = m_world->GetPopulation().GetCell(cellid);
        if (cell.CountGenomeFragments() > 0) {
          InstructionSequence fragment = cell.PopGenomeFragment(ctx);
          consumed = local_task_quality * fragment.GetSize();
          result.Consume(in_resource->GetID(), fragment.GetSize(), true);
          m_world->GetStats().GenomeFragmentMetabolized(taskctx.GetOrganism(), fragment);
        }
      }
      // if we can't metabolize a fragment, stop here.
      if (consumed == 0.0) { continue; }
    } else {
      // Otherwise we're using a finite resource
      const int res_id = in_resource->GetID();

      // check to see if the value of this resource was set to 0 for this cell
      if (resource_count[res_id]==0) {
        consumed = 0;
      } else {
        assert(resource_count[res_id] >= 0);
        assert(result.GetConsumed(res_id) >= 0);
        consumed = resource_count[res_id] - result.GetConsumed(res_id);
        consumed *= cur_process->GetMaxFraction();
        assert(consumed >= 0.0);
      }

      bool may_use_rbins = m_world->GetConfig().USE_RESOURCE_BINS.Get();
      bool using_rbins = false;  //default: not using resource bins

      if (may_use_rbins) assert(rbins_count.GetSize() > res_id);

      if (cur_process->GetInternal())
      {
        consumed = rbins_count[res_id];
        using_rbins = true;
      }
      /* Check to see if we do want to use this resource from a bin instead of the environment:
       * - Can we use the resource bins?
       * - Is there anything in the bin for this resource?
       * - Is the usable fraction in the bin strictly greater than the threshold fraction
       *   of what we could consume from the outside environment?
       */
      else if (may_use_rbins && rbins_count[res_id] > 0 &&
          (m_world->GetConfig().USE_STORED_FRACTION.Get() * rbins_count[res_id]) >
          (m_world->GetConfig().ENV_FRACTION_THRESHOLD.Get() * consumed)
          ) {
        consumed = m_world->GetConfig().USE_STORED_FRACTION.Get() * rbins_count[res_id];
        using_rbins = true;
      }

      // Make sure we're not above the maximum consumption.
      if (consumed > max_consumed) consumed = max_consumed;

      // Multiply by task_quality
      assert((local_task_quality >= 0.0) && (local_task_quality <= 1.0));
      consumed = consumed * local_task_quality * task_plasticity_modifier;  // modify consumed based on task quality and plasticity

      // Test if we are below the minimum consumption.
      if (consumed < min_consumed) consumed = 0.0;

      // If we don't actually have any resource to consume stop here.
      if (consumed == 0.0) continue;

      // Can't consume more resource than what's available.
      if (!using_rbins) consumed = std::min(consumed, resource_count[res_id]);
      else consumed = std::min(consumed, rbins_count[res_id]);

      // Mark in the results the resource consumed.
			if (cur_process->GetDepletable()) {
      	result.Consume(res_id, consumed, !using_rbins);
      }
    }

    // Mark the reaction as having been performed if we get here.
    result.MarkReaction(reaction_id);

    double bonus = consumed * cur_process->GetValue();
    
    if (!cur_process->GetIsGermline())
    {
      // normal bonus
      double deme_bonus = 0;

      // How much of this bonus belongs to the deme, and how much belongs to the organism?
      if (cur_process->GetDemeFraction()) {
        deme_bonus = cur_process->GetDemeFraction() * bonus;
        bonus = (1-cur_process->GetDemeFraction()) * bonus;
      }

      // Take care of the organism's bonus:
      switch (cur_process->GetType()) {
        case nReaction::PROCTYPE_ADD:
          result.AddBonus(bonus, reaction_id);
          result.AddDemeBonus(deme_bonus);
          break;
        case nReaction::PROCTYPE_MULT:
          result.MultBonus(bonus);
          // @JEB: since deme_bonus is ZERO by default this will cause
          // a problem if we unintentionally multiply the deme's bonus
          // when we do not make a deme reaction, i.e. deme=0!
          // Other cases ADD zero, so they don't necessarily need this check.
          if (cur_process->GetDemeFraction()) result.MultDemeBonus(deme_bonus);
          break;
        case nReaction::PROCTYPE_POW:
          result.MultBonus(pow(2.0, bonus));
          result.MultDemeBonus(pow(2.0, deme_bonus));
          break;
        case nReaction::PROCTYPE_LIN:
          result.AddBonus(bonus * task_count, reaction_id);
          break;
        case nReaction::PROCTYPE_ENERGY:
          result.AddEnergy(bonus);
          assert(deme_bonus == 0.0);
          break;
        case nReaction::PROCTYPE_ENZYME: //@JEB -- experimental
        {
	  const int res_id = in_resource->GetID();
          assert(cur_process->GetMaxFraction() != 0);
          assert(resource_count[res_id] != 0);
          // double reward = cur_process->GetValue() * resource_count[res_id] / (resource_count[res_id] + cur_process->GetMaxFraction());
          double reward = cur_process->GetValue() * resource_count[res_id] / (resource_count[res_id] + cur_process->GetKsubM());
          result.AddBonus( reward , reaction_id);
          break;
        }
        case nReaction::PROCTYPE_EXP: //@JEB -- experimental
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
      }
    } else {  // if (cur_process->GetIsGermline())
      // @JEB -- this process changes germline propensities, not bonus
      switch (cur_process->GetType()) {
        case nReaction::PROCTYPE_ADD:
          result.AddGermline(bonus);
          break;
        case nReaction::PROCTYPE_MULT:
          result.MultGermline(bonus);
          break;
        case nReaction::PROCTYPE_POW:
          result.MultGermline(pow(2.0, bonus));
          break;

        default:
          assert(false);  // Should not get here!
          break;
      }
    }

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
      if (!cur_process->GetInternal())
        result.Produce(product_id, product_size, true);
      else
        result.Produce(product_id, product_size, false);
    }

    // Determine what instructions should be run...
    const cString& inst = cur_process->GetInst();
    if (inst != "") result.AddInst(inst);

    double prob_lethal = cur_process->GetLethal();
    bool lethal = false;

    if (prob_lethal != 0 && prob_lethal != 1) {
      // hjg
      double x = ctx.GetRandom().GetDouble();
      if (x < prob_lethal) {
        lethal = true;
      }
    } else {
      lethal = prob_lethal;
    }

    result.Lethal(lethal);
    result.Sterilize(cur_process->GetSterilize());
  }
}

const cString& cEnvironment::GetReactionName(int reaction_id) const
{
  return reaction_lib.GetReaction(reaction_id)->GetName();
}

double cEnvironment::GetReactionValue(const cString& name)
{
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return 0.0;
  return found_reaction->GetValue();
}

double cEnvironment::GetReactionValue(int reaction_id)
{
  cReaction* found_reaction = reaction_lib.GetReaction(reaction_id);
  if (found_reaction == NULL) return 0.0;
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
    Apto::Array<int> reaction_ids(num_set);
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
  found_reaction->ModifyInst(inst_name);
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

bool cEnvironment::SetReactionMinCount(const cString& name, int reaction_min_count)
{
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  return found_reaction->SetMinReactionCount( reaction_min_count );
}

bool cEnvironment::SetReactionMaxCount(const cString& name, int reaction_max_count)
{
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;
  return found_reaction->SetMaxReactionCount( reaction_max_count );
}

bool cEnvironment::SetReactionTask(const cString& name, const cString& task)
{
  cReaction* found_reaction = reaction_lib.GetReaction(name);
  if (found_reaction == NULL) return false;

  for (int i=0; i<m_tasklib.GetSize(); i++)
  {
    if (m_tasklib.GetTask(i).GetName() == task)
    {
      found_reaction->SetTask( m_tasklib.GetTaskReference(i) );
      return true;
    }
  }

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

bool cEnvironment::ChangeResource(cReaction* reaction, const cString& res, int process_num)
{
  cReactionProcess* process = reaction->GetProcess(process_num);
  process->SetResource(m_world->GetEnvironment().GetResourceLib().GetResource(res));
  return true;
}

/*
 helper function that checks if this is a valid group id. The ids are specified
 in the environment file as tasks.
 */
bool cEnvironment::IsGroupID(int test_id)
{
  bool val = false;
  if (possible_group_ids.find(test_id) != possible_group_ids.end()) {
    val = true;
  }
  return val;

}

/*
 helper function that checks if this is a valid target id. The ids are specified
 in the environment file as tasks.
 */
bool cEnvironment::IsTargetID(int test_id)
{
  bool val = false;
  if (possible_target_ids.find(test_id) != possible_target_ids.end()) {
    val = true;
  }
  return val;
}

bool cEnvironment::IsHabitat(int test_habitat)
{
  bool val = false;
  if (possible_habitats.find(test_habitat) != possible_habitats.end()) {
    val = true;
  }
  return val;
}

void cEnvironment::SetAttackPreyFTList()
{
  bool has_pred = false;
  int offset = 1;
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() == -2 || m_world->GetConfig().PRED_PREY_SWITCH.Get() > -1) {
    has_pred = true;
    offset = 3;
  }
  
  // ft's may not be sequentially numbered
  bool dec_prey = false;
  bool dec_pred = false;
  bool dec_tpred = false;
  int num_targets = 0;
  std::set<int> fts_avail = GetTargetIDs();
  set <int>::iterator itr;
  for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) {
    num_targets++;
    if (*itr == -1 && !dec_prey) {
      offset--;
      dec_prey = true;
    }
    if (*itr == -2 && !dec_pred) {
      offset--;
      dec_pred = true;
    }
    if (*itr == -3 && !dec_tpred) {
      offset--;
      dec_tpred = true;
    }
  }
  
  Apto::Array<int> raw_target_list;
  raw_target_list.Resize(num_targets);
  raw_target_list.SetAll(0);
  int this_index = 0;
  for (itr = fts_avail.begin(); itr!=fts_avail.end(); itr++) {
    raw_target_list[this_index] = *itr;
    this_index++;
  }
  
  Apto::Array<int> target_list;
  int tot_targets = num_targets + offset;
  target_list.Resize(tot_targets);
  target_list.SetAll(0);
  
  target_list[0] = -1;
  if (has_pred) {
    target_list[0] = -3;
    target_list[1] = -2;
    target_list[2] = -1;
  }
  
  for (int i = 0; i < raw_target_list.GetSize(); i++) {
    if (raw_target_list[i] >= 0) target_list[i + offset] = raw_target_list[i];
  }
  pp_fts = target_list;
}
