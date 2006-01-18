/*
 *  cInstSet.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#ifndef cInstSet_h
#define cInstSet_h

#include <iostream>

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef cInstruction_h
#include "cInstruction.h"
#endif
#ifndef cInstLibBase_h
#include "cInstLibBase.h"
#endif

using namespace std;

/**
 * This class is used to create a mapping from the command strings in
 * an organism's genome into real methods in one of the hardware objects.  This
 * object has been designed to allow easy manipulation of the instruction
 * sets, as well as multiple instruction sets within a single soup (just
 * attach different cInstSet objects to different hardware.
 **/

class cWorld;

class cInstSet
{
public:
  cWorld* m_world;
  cInstLibBase *m_inst_lib;
  class cInstEntry {
  public:
    int lib_fun_id;
    int redundancy;           // Weight in instruction set (not impl.)
    int cost;                 // additional time spent to exectute inst.
    int ft_cost;              // time spent first time exec (in add to cost)
    double prob_fail;         // probability of failing to execute inst
  };
  tArray<cInstEntry> m_lib_name_map;
  tArray<int> m_lib_nopmod_map;
  tArray<int> m_mutation_chart;     // ID's represented by redundancy values.
  // Static components...
  static cInstruction inst_error2;
  // static const cInstruction inst_none;
  static cInstruction inst_default2;

  // Static components...
  //static const cInstruction inst_error;
  // static const cInstruction inst_none;
  static const cInstruction inst_default;
  
  
  cInstSet(); // @not_implemented

public:
  cInstSet(cWorld* world);
  cInstSet(const cInstSet& in_inst_set);
  ~cInstSet();

  cInstSet& operator=(const cInstSet& _in);

  bool OK() const;

  // Accessors
  const cString & GetName(int id) const
  { 
    return m_inst_lib->GetName(m_lib_name_map[id].lib_fun_id);
  }
  const cString & GetName(const cInstruction & inst) const
  {
    return GetName(inst.GetOp());
  }
  int GetCost(const cInstruction & inst) const
  {
    return m_lib_name_map[inst.GetOp()].cost;
  }
  int GetFTCost(const cInstruction & inst) const
  {
    return m_lib_name_map[inst.GetOp()].ft_cost;
  }
  double GetProbFail(const cInstruction & inst) const
  {
    return m_lib_name_map[inst.GetOp()].prob_fail;
  }
  int GetRedundancy(const cInstruction & inst) const
  {
    return m_lib_name_map[inst.GetOp()].redundancy;
  }

  int GetLibFunctionIndex(const cInstruction & inst) const
  {
    return m_lib_name_map[inst.GetOp()].lib_fun_id;
  }

  int GetNopMod(const cInstruction & inst) const
  {
    return m_inst_lib->GetNopMod(m_lib_nopmod_map[inst.GetOp()]);
  }

  cInstruction GetRandomInst() const;
  int GetRandFunctionIndex() const
  {
    return m_lib_name_map[ GetRandomInst().GetOp() ].lib_fun_id;
  }

  int GetSize() const {
    return m_lib_name_map.GetSize();
  }
  int GetNumNops() const {
    return m_lib_nopmod_map.GetSize();
  }

  // Instruction Analysis.
  int IsNop(const cInstruction & inst) const
  {
    return (inst.GetOp() < m_lib_nopmod_map.GetSize());
  }

  // Insertion of new instructions...
  int AddInst(
    const int lib_fun_id,
    const int redundancy=1,
    const int ft_cost=0,
    const int cost=0,
    const double prob_fail=0.0
  );
  int AddNop(
    const int lib_nopmod_id,
    const int redundancy=1,
    const int ft_cost=0,
    const int cost=0,
    const double prob_fail=0.0
  );

  // accessors for instruction library
  cInstLibBase *GetInstLib(){ return m_inst_lib; }
  void SetInstLib(cInstLibBase *inst_lib){
    m_inst_lib = inst_lib;
    inst_error2 = inst_lib->GetInstError();
    inst_default2 = inst_lib->GetInstDefault();
  }

  inline cInstruction GetInst(const cString & in_name) const;
  cString FindBestMatch(const cString & in_name) const;

  // Static methods..
  static const cInstruction & GetInstDefault() {
    return inst_default2;
  }
  static const cInstruction & GetInstError()   {
    return inst_error2;
  }
  // static const cInstruction & GetInstNone()    { return inst_none; }
};


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
  /*
  FIXME:  this return value is supposed to be cInstSet::GetInstError
  which should be the same as m_inst_lib->GetInstError().
  -- kgn
  */
  return cInstruction(0);
}

#endif
