/*
 *  cInstSet.h
 *  Avida
 *
 *  Called "inst_set.hh" prior to 12/5/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#ifndef cInstSet_h
#define cInstSet_h

#include <iostream>

#ifndef cString_h
#include "cString.h"
#endif
#ifndef cInstLib_h
#include "cInstLib.h"
#endif
#ifndef cInstruction_h
#include "cInstruction.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tSmartArray_h
#include "tSmartArray.h"
#endif

using namespace std;

/**
 * This class is used to create a mapping from the command strings in
 * an organism's genome into real methods in one of the hardware objects.  This
 * object has been designed to allow easy manipulation of the instruction
 * sets, as well as multiple instruction sets within a single soup (just
 * attach different cInstSet objects to different hardware.
 **/

class cAvidaContext;
class cStringList;
class cWorld;

class cInstSet
{
public:
  cWorld* m_world;
  cInstLib* m_inst_lib;
  
  struct sInstEntry {
    int lib_fun_id;
    int redundancy;           // Weight in instruction set (not impl.)
    int cost;                 // additional time spent to exectute inst.
    int ft_cost;              // time spent first time exec (in add to cost)
    int energy_cost;          // energy required to execute.
    double prob_fail;         // probability of failing to execute inst
    int addl_time_cost;       // additional time added to age for executing instruction
    int inst_code;            // instruction binary code
  };
  tSmartArray<sInstEntry> m_lib_name_map;
  
  tArray<int> m_lib_nopmod_map;
  tArray<int> m_mutation_chart;     // ID's represented by redundancy values.
  
  bool m_has_costs;
  bool m_has_ft_costs;
  bool m_has_energy_costs;
  
  
  void LoadWithStringList(const cStringList& sl);

  cInstSet(); // @not_implemented

public:
  inline cInstSet(cWorld* world, cInstLib* inst_lib)
    : m_world(world), m_inst_lib(inst_lib), m_has_costs(false), m_has_ft_costs(false), m_has_energy_costs(false) { ; }
  inline cInstSet(const cInstSet& is);
  inline ~cInstSet() { ; }

  inline cInstSet& operator=(const cInstSet& _in);

  bool OK() const;

  // Accessors
  const cString& GetName(int id) const { return m_inst_lib->GetName(m_lib_name_map[id].lib_fun_id); }
  const cString& GetName(const cInstruction& inst) const { return GetName(inst.GetOp()); }
  
  int GetRedundancy(const cInstruction& inst) const { return m_lib_name_map[inst.GetOp()].redundancy; }
  int GetCost(const cInstruction& inst) const { return m_lib_name_map[inst.GetOp()].cost; }
  int GetFTCost(const cInstruction& inst) const { return m_lib_name_map[inst.GetOp()].ft_cost; }
  int GetEnergyCost(const cInstruction& inst) const { return m_lib_name_map[inst.GetOp()].energy_cost; }
  double GetProbFail(const cInstruction& inst) const { return m_lib_name_map[inst.GetOp()].prob_fail; }
  int GetAddlTimeCost(const cInstruction& inst) const { return m_lib_name_map[inst.GetOp()].addl_time_cost; }
  int GetInstructionCode(const cInstruction& inst) const { return m_lib_name_map[inst.GetOp()].inst_code; }
  
  int GetLibFunctionIndex(const cInstruction& inst) const { return m_lib_name_map[inst.GetOp()].lib_fun_id; }

  int GetNopMod(const cInstruction& inst) const
  {
    int nopmod = m_lib_nopmod_map[inst.GetOp()];
    return m_inst_lib->GetNopMod(nopmod);
  }

  cInstruction GetRandomInst(cAvidaContext& ctx) const;
  int GetRandFunctionIndex(cAvidaContext& ctx) const { return m_lib_name_map[ GetRandomInst(ctx).GetOp() ].lib_fun_id; }

  int GetSize() const { return m_lib_name_map.GetSize(); }
  int GetNumNops() const { return m_lib_nopmod_map.GetSize(); }
  
  bool HasCosts() const { return m_has_costs; }
  bool HasFTCosts() const { return m_has_ft_costs; }
  bool HasEnergyCosts() const { return m_has_energy_costs; }

  // Instruction Analysis.
  int IsNop(const cInstruction& inst) const { return (inst.GetOp() < m_lib_nopmod_map.GetSize()); }
  bool IsLabel(const cInstruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).IsLabel(); }
  bool IsPromoter(const cInstruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).IsPromoter(); }
  bool ShouldStall(const cInstruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).ShouldStall(); }
  
  unsigned int GetFlags(const cInstruction& inst) const { return m_inst_lib->Get(GetLibFunctionIndex(inst)).GetFlags(); }

  // Insertion of new instructions...
  cInstruction ActivateNullInst();
  
  // Modification of instructions during run.
  void SetProbFail(const cInstruction& inst, double _prob_fail) { m_lib_name_map[inst.GetOp()].prob_fail = _prob_fail; }

  // accessors for instruction library
  cInstLib* GetInstLib() { return m_inst_lib; }

  inline cInstruction GetInst(const cString& in_name) const;
  cString FindBestMatch(const cString& in_name) const;
  bool InstInSet(const cString& in_name) const;

  cInstruction GetInstDefault() const { return cInstruction(m_inst_lib->GetInstDefault()); }
  cInstruction GetInstError() const { return cInstruction(255); }
  
  void LoadFromConfig();
  void LoadFromFile(const cString& filename);
  void LoadFromLegacyFile(const cString& filename);
};


#ifdef ENABLE_UNIT_TESTS
namespace nInstSet {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  


inline cInstSet::cInstSet(const cInstSet& is)
: m_world(is.m_world), m_inst_lib(is.m_inst_lib), m_lib_name_map(is.m_lib_name_map)
, m_lib_nopmod_map(is.m_lib_nopmod_map), m_mutation_chart(is.m_mutation_chart)
, m_has_costs(is.m_has_costs), m_has_ft_costs(is.m_has_ft_costs), m_has_energy_costs(is.m_has_energy_costs)
{
}

inline cInstSet& cInstSet::operator=(const cInstSet& _in)
{
  m_inst_lib = _in.m_inst_lib;
  m_lib_name_map = _in.m_lib_name_map;
  m_lib_nopmod_map = _in.m_lib_nopmod_map;
  m_mutation_chart = _in.m_mutation_chart;
  m_has_costs = _in.m_has_costs;
  m_has_ft_costs = _in.m_has_ft_costs;
  m_has_energy_costs = _in.m_has_energy_costs;
  return *this;
}

inline cInstruction cInstSet::GetInst(const cString & in_name) const
{
  for (int i = 0; i < m_lib_name_map.GetSize(); i++) {
    if (m_inst_lib->GetName(m_lib_name_map[i].lib_fun_id) == in_name) {
      return cInstruction(i);
    }
  }

  // @CAO Hacking this to make sure we don't have defaults...
  cerr << "Error: Unknown instruction '" << in_name << "'.  Exiting..." << endl;
  exit(1);


  // Adding default answer if nothing is found...
  return cInstruction(255);
}

#endif
