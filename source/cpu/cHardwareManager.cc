/*
 *  cHardwareManager.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
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

#include "cHardwareManager.h"

#include "cArgContainer.h"
#include "cArgSchema.h"
#include "cDriverManager.h"
#include "cDriverStatusConduit.h"
#include "cHardwareCPU.h"
#include "cHardwareExperimental.h"
#include "cHardwareSMT.h"
#include "cHardwareTransSMT.h"
#include "cHardwareGX.h"
#include "cHardwareStatusPrinter.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "cMetaGenome.h"
#include "cStringList.h"
#include "cStringUtil.h"
#include "cWorld.h"


cHardwareManager::cHardwareManager(cWorld* world)
: m_world(world), m_cpu_count(0)
{
  cString filename = world->GetConfig().INST_SET.Get();

}

cHardwareManager::~cHardwareManager()
{
  for (int i = 0; i < m_inst_sets.GetSize(); i++) delete m_inst_sets[i];
}


bool cHardwareManager::LoadInstSets(tList<cString>* errors)
{
  const cStringList& cfg_list = m_world->GetConfig().INSTSETS.Get();
  cStringList* cur_list = NULL;
  cString name;
  int hw_type = -1;
  
  bool success = true;
  for (int line_id = 0; line_id < cfg_list.GetSize(); line_id++) {
    cString line_type = cfg_list.GetLine(line_id).PopWord();
    if (line_type == "INST") {
      if (cur_list) {
        cur_list->PushRear(cfg_list.GetLine(line_id));
      } else {
        if (errors) errors->PushRear(new cString("disassociated INST definition, no INSTSET defined"));
        success = false;
      }
    } else if (line_type == "INSTSET") {
      if (cur_list) {
        if (!loadInstSet(hw_type, name, *cur_list, errors)) success = false;
        delete cur_list;
        cur_list = NULL;
      }
      
      // Build up schema to process instruction set parameters
      cArgSchema schema(':');
      schema.AddEntry("hw_type", 0, cArgSchema::SCHEMA_INT);
      
      
      // Process the INSTSET line
      cString is_def_str = cfg_list.GetLine(line_id);
      is_def_str.PopWord(); // Pop off "INSTSET"
      name = is_def_str.Pop(':');
      name.Trim();
      if (!name.GetSize()) {
        if (errors) errors->PushRear(new cString("instruction set name not found"));
        success = false;
        continue;
      }
      
      // Process arguments on the INSTSET line
      cArgContainer* args = cArgContainer::Load(is_def_str, schema, errors);
      if (!args) {
        success = false;
        continue;
      }
      
      hw_type = args->GetInt(0);
      cur_list = new cStringList;
      delete args;
    }
  }
  
  if (cur_list) {
    if (!loadInstSet(hw_type, name, *cur_list, errors)) success = false;
    delete cur_list;
  }
  
  return success;
}

bool cHardwareManager::loadInstSet(int hw_type, const cString& name, cStringList& sl, tList<cString>* errors)
{
  // Current list in progress, create actual cInstSet instance and process it
  cInstSet* inst_set = NULL;
  switch (hw_type)
  {
    case HARDWARE_TYPE_CPU_ORIGINAL:
      inst_set = new cInstSet(m_world, name, hw_type, cHardwareCPU::GetInstLib());
      break;
    case HARDWARE_TYPE_CPU_SMT:
      inst_set = new cInstSet(m_world, name, hw_type, cHardwareSMT::GetInstLib());
      break;
    case HARDWARE_TYPE_CPU_TRANSSMT:
      inst_set = new cInstSet(m_world, name, hw_type, cHardwareTransSMT::GetInstLib());
      break;
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      inst_set = new cInstSet(m_world, name, hw_type, cHardwareExperimental::GetInstLib());
      break;
    case HARDWARE_TYPE_CPU_GX:
      inst_set = new cInstSet(m_world, name, hw_type, cHardwareGX::GetInstLib());
      break;      
    default:
      if (errors) {
        cString* errstr = new cString(cStringUtil::Stringf("unknown/unsupported hw_type specified for instset '%s'",
                                                           (const char*)name));
        errors->PushRear(errstr);
      }
      return false;
  }  
  inst_set->LoadWithStringList(sl);
  
  int inst_set_id = m_inst_sets.GetSize();
  m_inst_sets.Push(inst_set);
  m_is_name_map.Set(name, inst_set_id);
  
  return true;
}

bool cHardwareManager::ConvertLegacyInstSetFile(cString filename, cStringList& str_list, tList<cString>* errors)
{
  // Setup the instruction library and collect the default filename
  cString default_filename;
	switch (m_world->GetConfig().HARDWARE_TYPE.Get())
	{
		case HARDWARE_TYPE_CPU_ORIGINAL:
			default_filename = cHardwareCPU::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_SMT:
			default_filename = cHardwareSMT::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_TRANSSMT:
			default_filename = cHardwareTransSMT::GetDefaultInstFilename();
			break;
		case HARDWARE_TYPE_CPU_EXPERIMENTAL:
			default_filename = cHardwareExperimental::GetDefaultInstFilename();
			break;
    case HARDWARE_TYPE_CPU_GX:
			default_filename = cHardwareGX::GetDefaultInstFilename();
			break;      
		default:
      if (errors) errors->PushRear(new cString("Unknown/Unsupported HARDWARE_TYPE specified"));
      return false;
  }
  
  if (filename == "" || filename == "-") {
    filename = default_filename;
    cDriverManager::Status().NotifyComment(cString("Using default instruction set: ") + filename);
    // set INST_SET so that the proper name will show up in the text viewer
    m_world->GetConfig().INST_SET.Set(filename);
  }
  
  
  cInitFile file(filename, m_world->GetWorkingDir());
  
  if (!file.WasOpened()) {
    if (errors) errors->Append(file.GetErrors());
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
    
    str_list.PushRear(cStringUtil::Stringf("INST %s:redundancy=%d:cost=%d:initial_cost=%d:energy_cost=%d:prob_fail=%f:addl_time_cost=%d",
                                           (const char*)inst_name, redundancy, cost, ft_cost, energy_cost, prob_fail, addl_time_cost));
  }
  
  return true;
}


cHardwareBase* cHardwareManager::Create(cAvidaContext& ctx, cOrganism* org, const cMetaGenome& mg)
{
  assert(org != NULL);
	
  int inst_set_id = m_is_name_map.GetWithDefault(mg.GetInstSet(), -1);
  if (inst_set_id == -1) return NULL;
  
  cInstSet* inst_set = m_inst_sets[inst_set_id];
  
  cHardwareBase* hw = 0;	
  switch (mg.GetHardwareType()) {
    case HARDWARE_TYPE_CPU_ORIGINAL:
      hw = new cHardwareCPU(ctx, m_world, org, inst_set);
      break;
    case HARDWARE_TYPE_CPU_SMT:
      hw = new cHardwareSMT(ctx, m_world, org, inst_set);
      break;
    case HARDWARE_TYPE_CPU_TRANSSMT:
      hw = new cHardwareTransSMT(ctx, m_world, org, inst_set);
      break;
    case HARDWARE_TYPE_CPU_EXPERIMENTAL:
      hw = new cHardwareExperimental(ctx, m_world, org, inst_set);
      break;
    case HARDWARE_TYPE_CPU_GX:
      hw = new cHardwareGX(ctx, m_world, org, inst_set);
      break;
    default:
      cDriverManager::Status().SignalError("Unknown/Unsupported HARDWARE_TYPE specified", -1);
      break;
  }
  
  // Are we tracing the execution of this cpu?
  if (m_world->GetConfig().TRACE_EXECUTION.Get()) {
    cString filename =  cStringUtil::Stringf("trace-%d.trace", m_cpu_count++);
    hw->SetTrace(new cHardwareStatusPrinter(m_world->GetDataFileOFStream(filename)));
  }
  
  assert(hw != 0);
  return hw;
}
