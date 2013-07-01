/*
 *  cInstSet.h
 *  Avida
 *
 *  Called "inst_set.hh" prior to 12/5/05.
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

#ifndef cInstSet_h
#define cInstSet_h

#include <iostream>

#include "avida/core/InstructionSequence.h"

#include "cString.h"
#include "cInstLib.h"
#include "cOrderedWeightedIndex.h"

using namespace std;
using namespace Avida;

/**
 * This class is used to create a mapping from the command strings in
 * an organism's genome into real methods in one of the hardware objects.  This
 * object has been designed to allow easy manipulation of the instruction
 * sets, as well as multiple instruction sets within a single soup (just
 * attach different cInstSet objects to different hardware.
 **/

class cAvidaContext;
class cStringList;
class cUserFeedback;
class cWorld;

const int MAX_INSTSET_SIZE = 255;

class cInstSet
{
public:
  cWorld* m_world;
  cString m_name;
  int m_hw_type;
  cInstLib* m_inst_lib;
  
  struct sInstEntry {
    int lib_fun_id;
    int redundancy;           // Weight in instruction set (not impl.)
    int cost;                 // additional time spent to exectute inst within the thread that executed the instruction
    int ft_cost;              // time spent first time exec (in add to cost)
    int energy_cost;          // energy required to execute.
    double prob_fail;         // probability of failing to execute inst
    int addl_time_cost;       // additional time added to age for executing instruction
    int inst_code;            // instruction binary code
    double res_cost;          // resources (from bins) required to execute inst
    double fem_res_cost;      
    int female_cost;          // additional cost paid by females to execute the instruction @CHC
    int choosy_female_cost;   // additional cost paid by females to execute the instruction (on top of female_cost) @CHC
    int post_cost;             // cpu cost to be paid AFTER instruction executed the first time (e.g. post-kill handling time in predators)
    double bonus_cost;          // current bonus required to execute inst
  };
  Apto::Array<sInstEntry, Apto::Smart> m_lib_name_map;
  
  Apto::Array<int> m_lib_nopmod_map;
  
  cOrderedWeightedIndex* m_mutation_index;     // Weighted index for instructions 
  
  bool m_has_costs;
  bool m_has_ft_costs;
  bool m_has_energy_costs;
  bool m_has_res_costs;
  bool m_has_fem_res_costs;
  bool m_has_female_costs;
  bool m_has_choosy_female_costs;
  bool m_has_post_costs;
  bool m_has_bonus_costs;
  
  int m_stack_size;
  int m_uops_per_cycle;
  
  cInstSet(); // @not_implemented

public:
  inline cInstSet(cWorld* world, const cString& name, int hw_type, cInstLib* inst_lib, int stack_size, int uops_per_cycle)
    : m_world(world), m_name(name), m_hw_type(hw_type), m_inst_lib(inst_lib), m_mutation_index(NULL)
    , m_has_costs(false), m_has_ft_costs(false), m_has_energy_costs(false), m_has_res_costs(false), m_has_fem_res_costs(false)
    , m_has_female_costs(false), m_has_choosy_female_costs(false), m_has_post_costs(false), m_has_bonus_costs(false), m_stack_size(stack_size)
    , m_uops_per_cycle(uops_per_cycle) { ; }
  cInstSet(const cInstSet&); 
  cInstSet& operator=(const cInstSet&); 
  inline ~cInstSet() { if (m_mutation_index != NULL) delete m_mutation_index; }
  
  const cString& GetInstSetName() const { return m_name; }
  int GetHardwareType() const { return m_hw_type; }

  // Accessors
  const cString& GetName(int id) const { return m_inst_lib->GetName(m_lib_name_map[id].lib_fun_id); }
  const cString& GetName(const Instruction& inst) const { return GetName(inst.GetOp()); }
  
  int GetRedundancy(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].redundancy; }
  int GetCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].cost; }
  int GetFTCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].ft_cost; }
  int GetEnergyCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].energy_cost; }
  double GetProbFail(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].prob_fail; }
  int GetAddlTimeCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].addl_time_cost; }
  int GetInstructionCode(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].inst_code; }
  double GetResCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].res_cost; }
  double GetFemResCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].fem_res_cost; }
  int GetFemaleCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].female_cost; } //@CHC
  int GetChoosyFemaleCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].choosy_female_cost; } //@CHC
  int GetPostCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].post_cost; }
  double GetBonusCost(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].bonus_cost; }
  
  int GetLibFunctionIndex(const Instruction& inst) const { return m_lib_name_map[inst.GetOp()].lib_fun_id; }

  int GetNopMod(const Instruction& inst) const
  {
    int nopmod = m_lib_nopmod_map[inst.GetOp()];
    return m_inst_lib->GetNopMod(nopmod);
  }

  Instruction GetRandomInst(cAvidaContext& ctx) const;
  int GetRandFunctionIndex(cAvidaContext& ctx) const { return m_lib_name_map[ GetRandomInst(ctx).GetOp() ].lib_fun_id; }

  int GetSize() const { return m_lib_name_map.GetSize(); }
  int GetNumNops() const { return m_lib_nopmod_map.GetSize(); }
  
  bool HasCosts() const { return m_has_costs; }
  bool HasFTCosts() const { return m_has_ft_costs; }
  bool HasEnergyCosts() const { return m_has_energy_costs; }
  bool HasResCosts() const { return m_has_res_costs; }
  bool HasFemResCosts() const { return m_has_fem_res_costs; }
  bool HasFemaleCosts() const { return m_has_female_costs; }
  bool HasChoosyFemaleCosts() const { return m_has_choosy_female_costs; }
  bool HasPostCosts() const { return m_has_post_costs; }
  bool HasBonusCosts() const { return m_has_bonus_costs; }
  
  int GetStackSize() const { return m_stack_size; }
  int GetUOpsPerCycle() const { return m_uops_per_cycle; }
  
  // Instruction Analysis.
  int IsNop(const Instruction& inst) const { return (inst.GetOp() < m_lib_nopmod_map.GetSize()); }
  bool IsLabel(const Instruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).IsLabel(); }
  bool IsPromoter(const Instruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).IsPromoter(); }
  bool IsTerminator(const Instruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).IsTerminator(); }
  bool ShouldStall(const Instruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).ShouldStall(); }
  bool ShouldSleep(const Instruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).ShouldSleep(); }
  bool IsImmediateValue(const Instruction& inst) const { return (inst != GetInstError() && m_inst_lib->Get(GetLibFunctionIndex(inst)).IsImmediateValue()); }
  
  unsigned int GetFlags(const Instruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).GetFlags(); }
  

  // Insertion of new instructions...
  Instruction ActivateNullInst();
  
  // Modification of instructions during run.
  void SetProbFail(const Instruction& inst, double _prob_fail) { m_lib_name_map[inst.GetOp()].prob_fail = _prob_fail; }
  void SetRedundancy(const Instruction& inst, int _redundancy) { m_lib_name_map[inst.GetOp()].redundancy = _redundancy; m_mutation_index->SetWeight(inst.GetOp(), _redundancy);}

  // accessors for instruction library
  cInstLib* GetInstLib() { return m_inst_lib; }
  const cInstLib* GetInstLib() const { return m_inst_lib; }

  inline Instruction GetInst(const cString& in_name) const;
  cString FindBestMatch(const cString& in_name) const;
  bool InstInSet(const cString& in_name) const;

  Instruction GetInstDefault() const { return Instruction(m_inst_lib->GetInstDefault()); }
  Instruction GetInstError() const { return Instruction(255); }
  
  bool LoadWithStringList(const cStringList& sl, cUserFeedback* errors = NULL);
  
  void SaveInstructionSequence(ofstream& of, const InstructionSequence& seq) const;
};


inline Instruction cInstSet::GetInst(const cString & in_name) const
{
  for (int i = 0; i < m_lib_name_map.GetSize(); i++) {
    if (m_inst_lib->GetName(m_lib_name_map[i].lib_fun_id) == in_name) {
      return Instruction(i);
    }
  }

  // @CAO Hacking this to make sure we don't have defaults...
  cerr << "Error: Unknown instruction '" << in_name << "'.  Exiting..." << endl;
  exit(1);


  // Adding default answer if nothing is found...
  return Instruction(255);
}

#endif
