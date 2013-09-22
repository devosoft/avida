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

#include "cArgContainer.h"
#include "cArgSchema.h"
#include "cHardwareBCR.h"
#include "cHardwareCPU.h"
#include "cHardwareExperimental.h"
#include "cHardwareGP8.h"
#include "cHardwareTransSMT.h"
#include "cHardwareStatusPrinter.h"
#include "cInitFile.h"
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
  cArgSchema is_schema(':');
  is_schema.AddEntry("hw_type", 0, cArgSchema::SCHEMA_INT);
  is_schema.AddEntry("stack_size", 1, 10);
  is_schema.AddEntry("uops_per_cycle", 2, 20);
  
  cArgContainer* args = NULL;

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
        if (!loadInstSet(args->GetInt(0), (const char*)name, args->GetInt(1), args->GetInt(2), *cur_list, feedback)) success = false;
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
      args = cArgContainer::Load(is_def_str, is_schema, *feedback);
      if (!args) {
        success = false;
        continue;
      }
      
      cur_list = new cStringList;
    }
  }
  
  if (cur_list) {
    if (!loadInstSet(args->GetInt(0), (const char*)name, args->GetInt(1), args->GetInt(2), *cur_list, feedback)) success = false;
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
      inst_set = new cInstSet(m_world, (const char*)name, hw_type, cHardwareCPU::GetInstLib(), stack_size, uops_per_cycle);
      break;
    case HARDWARE_TYPE_CPU_TRANSSMT:
      inst_set = new cInstSet(m_world, (const char*)name, hw_type, cHardwareTransSMT::GetInstLib(), stack_size, uops_per_cycle);
      break;
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      inst_set = new cInstSet(m_world, (const char*)name, hw_type, cHardwareExperimental::GetInstLib(), stack_size, uops_per_cycle);
      break;
    case HARDWARE_TYPE_CPU_GP8:
      inst_set = new cInstSet(m_world, (const char*)name, hw_type, cHardwareGP8::GetInstLib(), stack_size, uops_per_cycle);
      break;
    case HARDWARE_TYPE_CPU_BCR:
      inst_set = new cInstSet(m_world, (const char*)name, hw_type, cHardwareBCR::GetInstLib(), stack_size, uops_per_cycle);
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
  m_world->GetStats().SetGroupAttackInstNames(inst_set->GetInstSetName());

  m_world->GetStats().InstPreyExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstPredExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstTopPredExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstPreyFromSensorExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstPredFromSensorExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstTopPredFromSensorExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstFromMessageExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());

  m_world->GetStats().ExecCountsForGroupAttackInstSet(inst_set->GetInstSetName()).Clear();
  Apto::Array<cString> att_inst = m_world->GetStats().GetGroupAttackInsts(inst_set->GetInstSetName());
  for (int i = 0; i < att_inst.GetSize(); i++) {
    m_world->GetStats().ExecCountsForGroupAttackInst(inst_set->GetInstSetName(), att_inst[i]).Resize(20);
    for (int j = 0; j < 20; j++) {
      m_world->GetStats().ExecCountsForGroupAttackInst(inst_set->GetInstSetName(), att_inst[i])[j].Clear();
    }
  }
  
  m_world->GetStats().InstMaleExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  m_world->GetStats().InstFemaleExeCountsForInstSet(inst_set->GetInstSetName()).Resize(inst_set->GetSize());
  
  return true;
}

bool cHardwareManager::ConvertLegacyInstSetFile(cString filename, cStringList& str_list, cUserFeedback* feedback)
{
  // Setup the instruction library and collect the default filename
  cString default_filename;
	switch (m_world->GetConfig().HARDWARE_TYPE.Get())
	{
		case HARDWARE_TYPE_CPU_ORIGINAL:
			default_filename = cHardwareCPU::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_TRANSSMT:
			default_filename = cHardwareTransSMT::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_EXPERIMENTAL:
			default_filename = cHardwareExperimental::GetDefaultInstFilename();
			break;
		default:
      if (feedback) feedback->Error("unknown/unsupported HARDWARE_TYPE specified");
      return false;
  }
  
  if (filename == "" || filename == "-") {
    filename = default_filename;
    if (feedback) feedback->Notify("using default instruction set: %s", (const char*)filename);
    // set INST_SET so that the proper name will show up in the text viewer
    m_world->GetConfig().INST_SET.Set(filename);
  }
  
  
  cInitFile file(filename, m_world->GetWorkingDir());
  
  if (!file.WasOpened()) {
    if (feedback) feedback->Append(file.GetFeedback());
    return false;
  }

  str_list.PushRear(cStringUtil::Stringf("INSTSET %s:hw_type=%d", (const char*)filename, m_world->GetConfig().HARDWARE_TYPE.Get()));
  for (int line_id = 0; line_id < file.GetNumLines(); line_id++) {
    cString cur_line = file.GetLine(line_id);
    cString inst_name = cur_line.PopWord();
    int redundancy = cur_line.PopWord().AsInt();
    int cost = cur_line.PopWord().AsInt();
    int ft_cost = cur_line.PopWord().AsInt();
    int energy_cost = cur_line.PopWord().AsInt();
    double prob_fail = cur_line.PopWord().AsDouble();
    int addl_time_cost = cur_line.PopWord().AsInt();
    double res_cost = cur_line.PopWord().AsDouble();
    double fem_res_cost = cur_line.PopWord().AsDouble();
    int post_cost = cur_line.PopWord().AsDouble();

    str_list.PushRear(cStringUtil::Stringf("INST %s:redundancy=%d:cost=%d:initial_cost=%d:energy_cost=%d:prob_fail=%f:addl_time_cost=%d:res_cost=%f:fem_res_cost=%f:post_cost=%d",
                                           (const char*)inst_name, redundancy, cost, ft_cost, energy_cost, prob_fail, addl_time_cost, res_cost, fem_res_cost, post_cost)); 
  }  
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
    case HARDWARE_TYPE_CPU_BCR:
      hw = new cHardwareBCR(ctx, m_world, org, inst_set);
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
