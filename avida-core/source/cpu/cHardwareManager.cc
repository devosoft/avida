/*
 *  cHardwareManager.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
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

#include "cHardwareManager.h"

#include "avida/core/Genome.h"
#include "avida/core/GlobalObject.h"

#include "cHardwareCPU.h"
#include "cHardwareExperimental.h"
#include "cHardwareGP8.h"
#include "cHardwareTransSMT.h"
#include "cInstSet.h"
#include "cStringList.h"
#include "cStringUtil.h"
#include "cWorld.h"

using namespace Avida;

static const Apto::BasicString<Apto::ThreadSafe> s_prop_id_instset("instset");

cHardwareManager::cHardwareManager(cWorld* world)
: m_world(world)
{
  cString filename = world->GetConfig().INST_SET.Get();
  m_is_name_map.Set("(default)", 0);

}

cHardwareManager::~cHardwareManager()
{
  for (int i = 0; i < m_inst_sets.GetSize(); i++) delete m_inst_sets[i];
}


bool cHardwareManager::LoadInstSets(cUserFeedback* feedback)
{
  const cStringList& cfg_list = m_world->GetConfig().INSTSETS.Get();
  cStringList* cur_list = NULL;
  cString name;
  
  // Build up schema to process instruction set parameters
  Util::ArgSchema is_schema(':');
  is_schema.Define("hw_type", Util::INT);
  is_schema.Define("stack_size", 10);
  is_schema.Define("uops_per_cycle", 20);
  
  Util::Args* args = NULL;

  bool success = true;
  for (int line_id = 0; line_id < cfg_list.GetSize(); line_id++) {
    cString line_type = cfg_list.GetLine(line_id).PopWord();
    if (line_type == "INST") {
      if (cur_list) {
        cur_list->PushRear(cfg_list.GetLine(line_id));
      } else {
        if (feedback) feedback->Error("disassociated INST definition, no INSTSET defined");
        success = false;
      }
    } else if (line_type == "INSTSET") {
      if (cur_list) {
        if (!loadInstSet(args->Int(0), (const char*)name, args->Int(1), args->Int(2), *cur_list, feedback)) success = false;
        delete cur_list;
        delete args;
        cur_list = NULL;
        args = NULL;
      }
      
      
      // Process the INSTSET line
      cString is_def_str = cfg_list.GetLine(line_id);
      is_def_str.PopWord(); // Pop off "INSTSET"
      name = is_def_str.Pop(':');
      name.Trim();
      if (!name.GetSize()) {
        if (feedback) feedback->Error("instruction set name not found");
        success = false;
        continue;
      }
      
      // Process arguments on the INSTSET line
      args = Util::Args::Load((const char*)is_def_str, is_schema, ':', '=', feedback);
      if (!args) {
        success = false;
        continue;
      }
      
      cur_list = new cStringList;
    }
  }
  
  if (cur_list) {
    if (!loadInstSet(args->Int(0), (const char*)name, args->Int(1), args->Int(2), *cur_list, feedback)) success = false;
    delete cur_list;
    delete args;
  }
  
  return success;
}

bool cHardwareManager::loadInstSet(int hw_type, const Apto::String& name, int stack_size, int uops_per_cycle, cStringList& sl, cUserFeedback* feedback)
{
  // Current list in progress, create actual cInstSet instance and process it
  cInstSet* inst_set = NULL;
  switch (hw_type)
  {
    case HARDWARE_TYPE_CPU_ORIGINAL:
      inst_set = new cInstSet(m_world, (const char*)name, hw_type, cHardwareCPU::InstructionLibrary(), stack_size, uops_per_cycle);
      break;
    case HARDWARE_TYPE_CPU_TRANSSMT:
      inst_set = new cInstSet(m_world, (const char*)name, hw_type, cHardwareTransSMT::InstructionLibrary(), stack_size, uops_per_cycle);
      break;
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      inst_set = new cInstSet(m_world, (const char*)name, hw_type, cHardwareExperimental::InstructionLibrary(), stack_size, uops_per_cycle);
      break;
    case HARDWARE_TYPE_CPU_GP8:
      inst_set = new cInstSet(m_world, (const char*)name, hw_type, cHardwareGP8::InstructionLibrary(), stack_size, uops_per_cycle);
      break;
    default:
      if (feedback) feedback->Error("unknown/unsupported hw_type specified for instset '%s'", (const char*)name);
      return false;
  }  
  if (!inst_set->LoadWithStringList(sl, feedback)) return false;
  
  int inst_set_id = m_inst_sets.GetSize();
  m_inst_sets.Push(inst_set);
  m_is_name_map.Set(name, inst_set_id);
  
  Apto::Array<cString> names(inst_set->GetSize());
  for (int i = 0; i < inst_set->GetSize(); i++) names[i] = inst_set->GetName(i);
  m_world->GetStats().SetInstNames(inst_set->GetInstSetName(), names);
  Apto::String is((const char*)inst_set->GetInstSetName());

  m_world->GetStats().InstPreyExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstPredExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstTopPredExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstPreyFromSensorExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstPredFromSensorExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstTopPredFromSensorExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());

  return true;
}


cHardwareBase* cHardwareManager::Create(cAvidaContext& ctx, cOrganism* org, const Genome& mg)
{
  assert(org != NULL);
	
  Apto::String inst_set_name = mg.Properties().Get(s_prop_id_instset).StringValue();
  assert(inst_set_name.GetSize());
  int inst_set_id = m_is_name_map.GetWithDefault(inst_set_name, -1);
  if (inst_set_id == -1) {
    assert(false);
    return NULL; // No valid instruction set found
  }
  
  cInstSet* inst_set = m_inst_sets[inst_set_id];
  if (inst_set->GetHardwareType() != mg.HardwareType()) {
    assert(false);
    return NULL; // inst_set/hw_type mismatch
  }
  
  cHardwareBase* hw = 0;
  switch (inst_set->GetHardwareType()) {
    case HARDWARE_TYPE_CPU_ORIGINAL:
      hw = new cHardwareCPU(ctx, m_world, org, inst_set);
      break;
    case HARDWARE_TYPE_CPU_TRANSSMT:
      hw = new cHardwareTransSMT(ctx, m_world, org, inst_set);
      break;
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      hw = new cHardwareExperimental(ctx, m_world, org, inst_set);
      break;
    case HARDWARE_TYPE_CPU_GP8:
      hw = new cHardwareGP8(ctx, m_world, org, inst_set);
      break;
    default:
      assert(false);
      return NULL;
      break;
  }
  
  assert(hw != 0);
  return hw;
}

bool cHardwareManager::RegisterInstSet(const Apto::String& name, cInstSet* inst_set)
{
  if (m_is_name_map.Has(name)) return false;
  
  int inst_set_id = m_inst_sets.GetSize();
  m_inst_sets.Push(inst_set);
  m_is_name_map.Set(name, inst_set_id);  
  
  return true;
}
