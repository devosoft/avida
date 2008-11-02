/*
 *  cHardwareBase.h
 *  Avida
 *
 *  Called "hardware_base.hh" prior to 11/17/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#ifndef cHardwareBase_h
#define cHardwareBase_h

#include <cassert>
#include <iostream>

#ifndef cInstSet_h
#include "cInstSet.h"
#endif
#ifndef tBuffer_h
#include "tBuffer.h"
#endif

using namespace std;

class cAvidaContext;
class cCodeLabel;
class cCPUMemory;
class cGenome;
class cHardwareTracer;
class cHeadCPU;
class cInjectGenotype;
class cInstruction;
class cMutation;
class cOrganism;
class cString;
class cWorld;

class cHardwareBase
{
protected:
  cWorld* m_world;
  cOrganism* organism;       // Organism using this hardware.
  cInstSet* m_inst_set;      // Instruction set being used.
  cHardwareTracer* m_tracer; // Set this if you want execution traced.

  // Instruction costs...
  int m_inst_cost;
  tArray<int> inst_ft_cost;
  tArray<int> inst_energy_cost;
  bool m_has_any_costs;
  bool m_has_costs;
  bool m_has_ft_costs;
  bool m_has_energy_costs;
  

  virtual int GetExecutedSize(const int parent_size);
  virtual int GetCopiedSize(const int parent_size, const int child_size) = 0;  
  
  bool Divide_CheckViable(cAvidaContext& ctx, const int parent_size, const int child_size, bool using_repro = false);

protected:
  unsigned Divide_DoExactMutations(cAvidaContext& ctx, double mut_multiplier = 1.0, const int pointmut = INT_MAX);
  bool Divide_TestFitnessMeasures1(cAvidaContext& ctx);
  
  void TriggerMutations_Body(cAvidaContext& ctx, int type, cCPUMemory& target_memory, cHeadCPU& cur_head);
  bool TriggerMutations_ScopeGenome(cAvidaContext& ctx, const cMutation* cur_mut,
																		cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate);
  bool TriggerMutations_ScopeLocal(cAvidaContext& ctx, const cMutation* cur_mut,
																	 cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate);
  int TriggerMutations_ScopeGlobal(cAvidaContext& ctx, const cMutation* cur_mut,
																	 cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate);
  
  virtual bool SingleProcess_PayCosts(cAvidaContext& ctx, const cInstruction& cur_inst);
  void ResetInstructionCosts();
  
  
  cHardwareBase(); // @not_implemented
  cHardwareBase(const cHardwareBase&); // @not_implemented
  cHardwareBase& operator=(const cHardwareBase&); // @not_implemented

public:
  cHardwareBase(cWorld* world, cOrganism* in_organism, cInstSet* inst_set)
    : m_world(world), organism(in_organism), m_inst_set(inst_set), m_tracer(NULL)
    , m_has_costs(inst_set->HasCosts()), m_has_ft_costs(inst_set->HasFTCosts())
    , m_has_energy_costs(m_inst_set->HasEnergyCosts())
  {
    m_has_any_costs = (m_has_costs | m_has_ft_costs | m_has_energy_costs);
    assert(organism != NULL);
  }
  virtual ~cHardwareBase() { ; }

  // --------  Organism ---------
  cOrganism* GetOrganism() { return organism; }
  const cInstSet& GetInstSet() { return *m_inst_set; }

  
  // --------  Core Functionality  --------
  virtual void Reset() = 0;
  virtual bool SingleProcess(cAvidaContext& ctx, bool speculative = false) = 0;
  virtual void ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst) = 0;

  unsigned Divide_DoMutations(cAvidaContext& ctx, double mut_multiplier = 1.0, const int maxmut = INT_MAX);
  bool Divide_TestFitnessMeasures(cAvidaContext& ctx);
  
  // --------  Helper methods  --------
  virtual int GetType() const = 0;
  virtual bool OK() = 0;
  virtual void PrintStatus(std::ostream& fp) = 0;
  void SetTrace(cHardwareTracer* tracer) { m_tracer = tracer; }
  
  
  // --------  Stack Manipulation...  --------
  virtual int GetStack(int depth = 0, int stack_id = -1, int in_thread = -1) const = 0;
  virtual int GetNumStacks() const = 0;
  
  
  // --------  Head Manipulation (including IP)  --------
  virtual const cHeadCPU& GetHead(int head_id) const = 0;
  virtual cHeadCPU& GetHead(int head_id) = 0;
  virtual const cHeadCPU& GetHead(int head_id, int thread) const = 0;
  virtual cHeadCPU& GetHead(int head_id, int thread) = 0;
  virtual int GetNumHeads() const = 0;
  
  virtual const cHeadCPU& IP() const = 0;
  virtual cHeadCPU& IP() = 0;
  virtual const cHeadCPU& IP(int thread) const = 0;
  virtual cHeadCPU& IP(int thread) = 0;
  
  cHeadCPU FindLabelFull(const cCodeLabel& label);
  
  
  // --------  Memory Manipulation  --------
  virtual const cCPUMemory& GetMemory() const = 0;
  virtual cCPUMemory& GetMemory() = 0;
  virtual int GetMemSize() const = 0;
  virtual const cCPUMemory& GetMemory(int value) const = 0;
  virtual cCPUMemory& GetMemory(int value) = 0;
  virtual int GetMemSize(int value) const = 0;
  virtual int GetNumMemSpaces() const = 0;
  
  
  // --------  Register Manipulation  --------
  virtual int GetRegister(int reg_id) const = 0;
  virtual int GetNumRegisters() const = 0;
  
  
  // --------  Thread Manipulation  --------
  virtual bool ThreadSelect(const int thread_id) = 0;
  virtual bool ThreadSelect(const cCodeLabel& in_label) = 0;
  virtual void ThreadNext() = 0;
  virtual void ThreadPrev() = 0;
  virtual cInjectGenotype* ThreadGetOwner() = 0;
  virtual void ThreadSetOwner(cInjectGenotype* in_genotype) = 0;

  virtual int GetNumThreads() const = 0;
  virtual int GetCurThread() const = 0;
  virtual int GetCurThreadID() const = 0;
  
  
  // --------  Parasite Stuff  --------
  virtual bool InjectHost(const cCodeLabel& in_label, const cGenome& injection) = 0;
  
    
  // --------  Mutation  --------
  virtual int PointMutate(cAvidaContext& ctx, const double mut_rate);
  virtual bool TriggerMutations(cAvidaContext& ctx, int trigger);
  virtual bool TriggerMutations(cAvidaContext& ctx, int trigger, cHeadCPU& cur_head);

  // --------  Input/Output Buffers  --------
  virtual tBuffer<int>& GetInputBuf();
  virtual tBuffer<int>& GetOutputBuf();
  
  // --------  State Transfer  --------
  virtual void InheritState(cHardwareBase& in_hardware){ ; }
  
  //alarm
  virtual bool Jump_To_Alarm_Label(int jump_label) { return false; }
  
protected:
  // --------  No-Operation Instruction --------
  bool Inst_Nop(cAvidaContext& ctx);  // A no-operation instruction that does nothing! 
  
  // -------- Implicit Repro Check/Instruction -------- @JEB
  void CheckImplicitRepro(cAvidaContext& ctx, bool exec_last_inst = false);
  virtual bool Inst_Repro(cAvidaContext& ctx);

  // --------  Execution Speed Instruction --------
  bool Inst_DoubleEnergyUsage(cAvidaContext& ctx);
  bool Inst_HalfEnergyUsage(cAvidaContext& ctx);
  bool Inst_DefaultEnergyUsage(cAvidaContext& ctx);
	
	// -------- Synchronization --------
public:
  //! Called when the organism that owns this CPU has received a flash from a neighbor.
  virtual void ReceiveFlash();	
};


#ifdef ENABLE_UNIT_TESTS
namespace nHardwareBase {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
