/*
 *  cInstSet.cc
 *  Avida
 *
 *  Called "inst_set.cc" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#include "cInstSet.h"

#include "cArgContainer.h"
#include "cArgSchema.h"
#include "cAvidaContext.h"
#include "cStringUtil.h"
#include "cUserFeedback.h"
#include "cWorld.h"
#include "cWorldDriver.h"

using namespace std;


bool cInstSet::OK() const
{
  assert(m_lib_name_map.GetSize() < 255);
  assert(m_lib_nopmod_map.GetSize() < m_lib_name_map.GetSize());

  // Make sure that all of the redundancies are represented the appropriate
  // number of times.
  tArray<int> test_redundancy2(m_lib_name_map.GetSize());
  test_redundancy2.SetAll(0);
  for (int i = 0; i < m_mutation_chart.GetSize(); i++) {
    int test_id = m_mutation_chart[i];
    test_redundancy2[test_id]++;
  }
  for (int i = 0; i < m_lib_name_map.GetSize(); i++) {
    assert(m_lib_name_map[i].redundancy == test_redundancy2[i]);
  }

  return true;
}


cInstruction cInstSet::GetRandomInst(cAvidaContext& ctx) const
{
  return cInstruction(m_mutation_chart[ctx.GetRandom().GetUInt(m_mutation_chart.GetSize())]);
}



cInstruction cInstSet::ActivateNullInst()
{  
  const int inst_id = m_lib_name_map.GetSize();
  const int null_fun_id = m_inst_lib->GetInstNull();
  
  assert(inst_id < 255);
  
  // Make sure not to activate again if NULL is already active
  for (int i = 0; i < inst_id; i++) if (m_lib_name_map[i].lib_fun_id == null_fun_id) return cInstruction(i);
  
  
  // Increase the size of the array...
  m_lib_name_map.Resize(inst_id + 1);
  
  // Setup the new function...
  m_lib_name_map[inst_id].lib_fun_id = null_fun_id;
  m_lib_name_map[inst_id].redundancy = 0;
  m_lib_name_map[inst_id].cost = 0;
  m_lib_name_map[inst_id].ft_cost = 0;
  m_lib_name_map[inst_id].energy_cost = 0;
  m_lib_name_map[inst_id].prob_fail = 0.0;
  m_lib_name_map[inst_id].addl_time_cost = 0;
  
  return cInstruction(inst_id);
}


cString cInstSet::FindBestMatch(const cString& in_name) const
{
  int best_dist = 1024;
  cString best_name("");
  
  for (int i = 0; i < m_lib_name_map.GetSize(); i++) {
    const cString & cur_name = m_inst_lib->GetName(m_lib_name_map[i].lib_fun_id);
    const int cur_dist = cStringUtil::EditDistance(cur_name, in_name);
    if (cur_dist < best_dist) {
      best_dist = cur_dist;
      best_name = cur_name;
    }
    if (cur_dist == 0) break;
  }

  return best_name;
}

bool cInstSet::InstInSet(const cString& in_name) const
{
  cString best_name("");
  
  for (int i = 0; i < m_lib_name_map.GetSize(); i++) {
    const cString & cur_name = m_inst_lib->GetName(m_lib_name_map[i].lib_fun_id);
    if (cur_name == in_name) return true;
  }
  return false;
}

bool cInstSet::LoadWithStringList(const cStringList& sl, cUserFeedback* feedback)
{
  cArgSchema schema(':');
  
  // Integer
  schema.AddEntry("redundancy", 0, 1);
  schema.AddEntry("cost", 1, 0);
  schema.AddEntry("initial_cost", 2, 0);
  schema.AddEntry("energy_cost", 3, 0);
  schema.AddEntry("addl_time_cost", 4, 0);
  
  // Double
  schema.AddEntry("prob_fail", 0, 0.0);
  
  // String  
  schema.AddEntry("inst_code", 0, "");
  
  
  // Ensure that the instruction code length is in the range of bits supported by the int type
  int inst_code_len = m_world->GetConfig().INST_CODE_LENGTH.Get();
  if ((unsigned)inst_code_len > (sizeof(int) * 8)) inst_code_len = sizeof(int) * 8;
  else if (inst_code_len <= 0) inst_code_len = 1;
  
  bool success = true;
  for (int line_id = 0; line_id < sl.GetSize(); line_id++) {
    cString cur_line = sl.GetLine(line_id);
    
    // Look for the INST keyword at the beginning of each line, and ignore if not found.
    cString inst_name = cur_line.PopWord();
    if (inst_name != "INST") continue;
    
    // Lookup the instruction name in the library
    inst_name = cur_line.Pop(':');
    int fun_id = m_inst_lib->GetIndex(inst_name);
    if (fun_id == -1) {
      // Oh oh!  Didn't find an instruction!
      if (feedback) feedback->Error("unknown instruction '%s' (Best match = '%s')",
                                    (const char*)inst_name, (const char*)m_inst_lib->GetNearMatch(inst_name));
      success = false;
      continue;
    }
    
    // Load the arguments for this instruction
    cArgContainer* args = cArgContainer::Load(cur_line, schema, feedback);
    if (!args) {
      success = false;
      continue;
    }
    
    // Check to make sure we are not inserting the special NULL instruction
    if (fun_id == m_inst_lib->GetInstNull()) {
      if (feedback) feedback->Error("invalid use of NULL instruction");
      success = false;
      continue;
    }
    
    int redundancy = args->GetInt(0);
    if (redundancy < 0) {
      if (feedback) feedback->Warning("instruction '%s' has negative redundancy, ignoring...", (const char*)inst_name);
      continue;
    }
    if (redundancy > 256) {
      if (feedback) feedback->Warning("max redundancy is 256, resetting redundancy of '%s' from %d to 256",
                                      (const char*) inst_name, redundancy);
      redundancy = 256;
    }
    
    
    
    // Get the ID of the new Instruction
    const int inst_id = m_lib_name_map.GetSize();
    assert(inst_id < 255);
    
    // Increase the size of the array...
    m_lib_name_map.Resize(inst_id + 1);
    
    // Setup the new function...
    m_lib_name_map[inst_id].lib_fun_id = fun_id;
    m_lib_name_map[inst_id].redundancy = redundancy;
    m_lib_name_map[inst_id].cost = args->GetInt(1);
    m_lib_name_map[inst_id].ft_cost = args->GetInt(2);
    m_lib_name_map[inst_id].energy_cost = args->GetInt(3);
    m_lib_name_map[inst_id].prob_fail = args->GetDouble(0);
    m_lib_name_map[inst_id].addl_time_cost = args->GetInt(4);
    
    if (m_lib_name_map[inst_id].cost > 1) m_has_costs = true;
    if (m_lib_name_map[inst_id].ft_cost) m_has_ft_costs = true;
    if (m_lib_name_map[inst_id].energy_cost) m_has_energy_costs = true;
    
    
    // Parse the instruction code
    cString inst_code = args->GetString(0);
    if (inst_code == "") {
      switch (m_world->GetConfig().INST_CODE_DEFAULT_TYPE.Get()) {
        case INST_CODE_ZEROS:
          m_lib_name_map[inst_id].inst_code = 0;
          break;
        case INST_CODE_INSTNUM:
          m_lib_name_map[inst_id].inst_code = ((~0) >> ((sizeof(int) * 8) - inst_code_len)) & inst_id;
          break;
        default:
          if (feedback) feedback->Error("invalid default instruction code type");
          success = false;
          break;
      }
    } else {
      int inst_code_val = 0;
      for (int i = 0; i < inst_code_len && i < inst_code.GetSize(); i++) {
        inst_code_val <<= 1;
        if (inst_code[i] == '1') inst_code_val |= 1;
        else if (inst_code[i] != '0') {
          if (feedback) feedback->Error("invalid character in instruction code, must be 0 or 1");
          success = false;
          break;
        }
      }
      
      m_lib_name_map[inst_id].inst_code = inst_code_val;
    }
    

    // If this is a nop, add it to the proper mappings
    if ((*m_inst_lib)[fun_id].IsNop()) {
      // Assert nops are at the _beginning_ of an inst_set.
      if (m_lib_name_map.GetSize() != (m_lib_nopmod_map.GetSize() + 1)) {
        if (feedback) feedback->Error("invalid NOP placement, all NOPs must be listed first");
        success = false;
      }

      m_lib_nopmod_map.Resize(inst_id + 1);
      m_lib_nopmod_map[inst_id] = fun_id;
    }
    

    const int total_redundancy = m_mutation_chart.GetSize();
    m_mutation_chart.Resize(total_redundancy + redundancy);
    for (int i = 0; i < redundancy; i++) {
      m_mutation_chart[total_redundancy + i] = inst_id;
    }
    
    // Clean up the argument container for this instruction
    delete args;
  }
  
  return success;
}
