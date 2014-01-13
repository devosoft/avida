/*
 *  cInstSet.cc
 *  Avida
 *
 *  Called "inst_set.cc" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#include "cInstSet.h"

#include "avida/core/UniverseDriver.h"
#include "avida/util/ArgParser.h"

#include "cStringUtil.h"
#include "cUserFeedback.h"
#include "cWorld.h"

#include <iostream>

using namespace std;
using namespace Avida;


cInstSet::cInstSet(const cInstSet& _in)
  : m_world(_in.m_world)
  , m_name(_in.m_name)
  , m_hw_type(_in.m_hw_type)
  , m_inst_lib(_in.m_inst_lib)
  , m_lib_name_map(_in.m_lib_name_map)
  , m_mutation_index(NULL)
  , m_has_costs(_in.m_has_costs)
  , m_has_ft_costs(_in.m_has_ft_costs)
  , m_has_res_costs(_in.m_has_res_costs)
  , m_has_fem_res_costs(_in.m_has_fem_res_costs)
  , m_has_female_costs(_in.m_has_female_costs)
  , m_has_choosy_female_costs(_in.m_has_choosy_female_costs)
  , m_has_post_costs(_in.m_has_post_costs)
  , m_has_bonus_costs(_in.m_has_bonus_costs)
{
  m_mutation_index = new cOrderedWeightedIndex(*_in.m_mutation_index);
}

cInstSet& cInstSet::operator=(const cInstSet& _in)
{
  m_world = _in.m_world;
  m_name = _in.m_name;
  m_hw_type = _in.m_hw_type;
  m_inst_lib = _in.m_inst_lib;
  m_lib_name_map = _in.m_lib_name_map;
  m_mutation_index = NULL;
  m_has_costs = _in.m_has_costs;
  m_has_ft_costs = _in.m_has_ft_costs;
  m_has_res_costs = _in.m_has_res_costs;
  m_has_fem_res_costs = _in.m_has_fem_res_costs;
  m_has_female_costs = _in.m_has_female_costs;
  m_has_choosy_female_costs = _in.m_has_choosy_female_costs;
  m_has_post_costs = _in.m_has_post_costs;
  m_has_bonus_costs = _in.m_has_bonus_costs;

  m_mutation_index = new cOrderedWeightedIndex(*_in.m_mutation_index);
  return *this;
}


Instruction cInstSet::GetRandomInst(cAvidaContext& ctx) const
{
  double weight = ctx.GetRandom().GetDouble(m_mutation_index->GetTotalWeight());
  unsigned inst_ndx = m_mutation_index->FindPosition(weight);
  return Instruction(inst_ndx);
}



Instruction cInstSet::ActivateNullInst()
{  
  const int inst_id = m_lib_name_map.GetSize();
  const int null_fun_id = m_inst_lib->InstNull();
  
  assert(inst_id < MAX_INSTSET_SIZE);
  
  // Make sure not to activate again if NULL is already active
  for (int i = 0; i < inst_id; i++) if (m_lib_name_map[i].lib_fun_id == null_fun_id) return Instruction(i);
  
  
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
  m_lib_name_map[inst_id].res_cost = 0.0; 
  m_lib_name_map[inst_id].fem_res_cost = 0.0; 
  m_lib_name_map[inst_id].post_cost = 0;
  m_lib_name_map[inst_id].bonus_cost = 0.0;
  
  return Instruction(inst_id);
}


cString cInstSet::FindBestMatch(const cString& in_name) const
{
  int best_dist = 1024;
  cString best_name("");
  
  for (int i = 0; i < m_lib_name_map.GetSize(); i++) {
    const Apto::String& cur_name = m_inst_lib->NameOf(m_lib_name_map[i].lib_fun_id);
    const int cur_dist = cStringUtil::EditDistance((const char*)cur_name, in_name);
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
    const Apto::String& cur_name = m_inst_lib->NameOf(m_lib_name_map[i].lib_fun_id);
    if (cur_name == in_name) return true;
  }
  return false;
}

bool cInstSet::LoadWithStringList(const cStringList& sl, cUserFeedback* feedback)
{
  Util::ArgSchema schema;
  
  // Integer
  schema.Define("cost", 0);
  schema.Define("initial_cost", 0);
  schema.Define("energy_cost", 0);
  schema.Define("addl_time_cost", 0);
  schema.Define("female_cost", 0); //@CHC
  schema.Define("choosy_female_cost", 0); //@CHC
  schema.Define("post_cost", 0);

  // Double
  schema.Define("prob_fail", 0.0);
  schema.Define("res_cost", 0.0);
  schema.Define("redundancy", 1.0);
  schema.Define("fem_res_cost", 0.0);  
  schema.Define("bonus_cost", 0.0);
  
  bool success = true;
  for (int line_id = 0; line_id < sl.GetSize(); line_id++) {
    cString cur_line = sl.GetLine(line_id);

    // Look for the INST keyword at the beginning of each line, and ignore if not found.
    cString inst_name = cur_line.PopWord();
    if (inst_name != "INST") continue;
    
    // Lookup the instruction name in the library
    inst_name = cur_line.Pop(':');
    int fun_id = m_inst_lib->IndexOf((const char*)inst_name);
    if (fun_id == -1) {
      // Oh oh!  Didn't find an instruction!
      Apto::String a_inst_name((const char*)inst_name);
      if (feedback) feedback->Error("unknown instruction '%s' (Best match = '%s')",
                                    (const char*)inst_name, (const char*)m_inst_lib->NearMatch(a_inst_name));
      success = false;
      continue;
    }
    
    // Load the arguments for this instruction
    Util::Args* args = Util::Args::Load((const char*)cur_line, schema, ':', '=', feedback);
    if (!args) {
      success = false;
      continue;
    }
    
    // Check to make sure we are not inserting the special NULL instruction
    if (fun_id == m_inst_lib->InstNull()) {
      if (feedback) feedback->Error("invalid use of NULL instruction");
      success = false;
      continue;
    }
    
    double redundancy = args->Double(2);
    if (redundancy < 0.0) {
      if (feedback) feedback->Warning("instruction '%s' has negative redundancy, ignoring...", (const char*)inst_name);
      continue;
    }
    
    
    // Get the ID of the new Instruction
    const int inst_id = m_lib_name_map.GetSize();
    assert(inst_id < MAX_INSTSET_SIZE);
    
    // Increase the size of the array...
    m_lib_name_map.Resize(inst_id + 1);
    
    // Setup the new function...
    m_lib_name_map[inst_id].lib_fun_id = fun_id;
    m_lib_name_map[inst_id].redundancy = redundancy;
    m_lib_name_map[inst_id].cost = args->Int(0);
    m_lib_name_map[inst_id].ft_cost = args->Int(1);
    m_lib_name_map[inst_id].energy_cost = args->Int(2);
    m_lib_name_map[inst_id].prob_fail = args->Double(0);
    m_lib_name_map[inst_id].addl_time_cost = args->Int(3);
    m_lib_name_map[inst_id].res_cost = args->Double(1);
    m_lib_name_map[inst_id].fem_res_cost = args->Double(3);
    m_lib_name_map[inst_id].female_cost = args->Int(4);
    m_lib_name_map[inst_id].choosy_female_cost = args->Int(5);
    m_lib_name_map[inst_id].post_cost = args->Int(6);
    m_lib_name_map[inst_id].bonus_cost = args->Double(4);
    
    if (m_lib_name_map[inst_id].cost > 1) m_has_costs = true;
    if (m_lib_name_map[inst_id].ft_cost) m_has_ft_costs = true;
    if (m_lib_name_map[inst_id].res_cost) m_has_res_costs = true;
    if (m_lib_name_map[inst_id].fem_res_cost) m_has_fem_res_costs = true;   
    if (m_lib_name_map[inst_id].female_cost) m_has_female_costs = true;
    if (m_lib_name_map[inst_id].choosy_female_cost) m_has_choosy_female_costs = true;
    if (m_lib_name_map[inst_id].post_cost > 1) m_has_post_costs = true;
    if (m_lib_name_map[inst_id].bonus_cost) m_has_bonus_costs = true;
    

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
    
    // Clean up the argument container for this instruction
    delete args;
  }

  //Setup mutation indexing based on redundancies
  m_mutation_index = new cOrderedWeightedIndex();
  for (int id=0; id < m_lib_name_map.GetSize(); id++)
  {
     double red = m_lib_name_map[id].redundancy;
     if (red == 0.0)
     {
       continue;
     }
     m_mutation_index->SetWeight(id, m_lib_name_map[id].redundancy);
  }
  return success;
}


void cInstSet::SaveInstructionSequence(ofstream& of, const InstructionSequence& seq) const
{
  for (int i = 0; i < seq.GetSize(); i++) of << GetName(seq[i]) << endl;  
}
