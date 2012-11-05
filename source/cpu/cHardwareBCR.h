/*
 *  cHardwareBCR.h
 *  Avida
 *
 *  Created by David on 11/2/2012 based on cHardwareMBE.h
 *  Copyright 2012 Michigan State University. All rights reserved.
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
 *  Authors: David M. Bryson <david@programerror.com>, Aaron P. Wagner <apwagner@msu.edu>
 *
 */

#ifndef cHardwareBCR_h
#define cHardwareBCR_h

#include "avida/Avida.h"

#include "cCodeLabel.h"
#include "cCPUMemory.h"
#include "cEnvReqs.h"
#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHeadCPU.h"
#include "cOrgSensor.h"
#include "cStats.h"
#include "cString.h"

#include "tInstLib.h"
#include "cEnvReqs.h"
#include "cEnvironment.h"

#include <cstring>
#include <iomanip>

/**
 * Each organism may have a cHardwareBCR structure which keeps track of the
 * current status of all the components of the simulated hardware.
 *
 * @see cCPUMemory, cInstSet
 **/

class cInstLib;
class cInstSet;
class cMutation;
class cOrganism;

class cHardwareBCR : public cHardwareBase
{
public:
  typedef bool (cHardwareBCR::*tMethod)(cAvidaContext& ctx);

private:
  // --------  Structure Constants  --------
  static const int NUM_REGISTERS = 16;
  static const int NUM_BEHAVIORS = 3; // num inst types capable of storing their own data
  static const int NUM_HEADS = NUM_REGISTERS;
  enum { rAX = 0, rBX, rCX, rDX, rEX, rFX, rGX, rHX, rIX, rJX, rKX, rLX, rMX, rNX, rOX, rPX };
  enum { hIP, hR, hW, hF, hF2, hF3, hF4, hF5 };
  static const int NUM_NOPS = NUM_REGISTERS;
  static const int STACK_SIZE = 10;
  static const int MAX_THREADS = NUM_NOPS;
  static const int MAX_MEM_SPACES = NUM_NOPS;
  
  
  // --------  Static Variables  --------
  static tInstLib<cHardwareBCR::tMethod>* s_inst_slib;
  
  
  // --------  Define Internal Data Structures  --------
  struct DataValue
  {
    int value;
    
    // Actual age of this value
    unsigned int originated:15;
    unsigned int from_env:1;
    
    // Age of the oldest component used to create this value
    unsigned int oldest_component:15;
    unsigned int env_component:1;
    
    inline DataValue() : value(0) { ; }
    inline void Clear() { value = 0; originated = 0; from_env = 0, oldest_component = 0; env_component = 0; }
    inline DataValue& operator=(const DataValue& i);
  };
  
  
  class Stack
  {
  private:
    DataValue m_stack[STACK_SIZE];
    char m_sp;
    
  public:
    Stack() : m_sp(0) { Clear(); }
    inline Stack(const Stack& is) : m_sp(is.m_sp) { for (int i = 0; i < STACK_SIZE; i++) m_stack[i] = is.m_stack[i]; }
    ~Stack() { ; }
    
    inline void operator=(const Stack& is) { m_sp = is.m_sp; for (int i = 0; i < STACK_SIZE; i++) m_stack[i] = is.m_stack[i]; }
    
    inline void Push(const DataValue& value) { if (--m_sp < 0) m_sp = STACK_SIZE - 1; m_stack[(int)m_sp] = value; }
    inline DataValue Pop() { DataValue v = m_stack[(int)m_sp]; m_stack[(int)m_sp].Clear(); if (++m_sp == STACK_SIZE) m_sp = 0; return v; }
    inline DataValue& Peek() { return m_stack[(int)m_sp]; }
    inline const DataValue& Peek() const { return m_stack[(int)m_sp]; }
    inline const DataValue& Get(int d = 0) const { assert(d >= 0); int p = d + m_sp; return m_stack[(p >= STACK_SIZE) ? (p - STACK_SIZE) : p]; }
    inline void Clear() { for (int i = 0; i < STACK_SIZE; i++) m_stack[i].Clear(); }
  };
  

  struct Thread
  {
  public:
    int thread_id;

    DataValue reg[NUM_REGISTERS];
    cHeadCPU heads[NUM_HEADS];
    Stack stack;
    
    struct {
      unsigned int cur_stack:1;
      unsigned int cur_head:3;
      bool reading_label:1;
      bool reading_seq:1;
      bool running:1;
      bool active:1;
      bool wait_greater:1;
      bool wait_equal:1;
      bool wait_less:1;
      int wait_reg:4;
      unsigned int wait_dst:4;
    };
    int wait_value;
    
    cCodeLabel read_label;
    cCodeLabel read_seq;
    cCodeLabel next_label;
    
    inline Thread() { ; }
    ~Thread() { ; }
    
    void operator=(const Thread& in_thread);
    void Reset(cHardwareBCR* in_hardware, int in_id);
  };
  
  
  // --------  Member Variables  --------
  const tMethod* m_functions;

  // Memory
  Apto::Array<cCPUMemory, Apto::ManagedPointer> m_mem_array;
  char m_mem_ids[MAX_MEM_SPACES];
  
  // Stacks
  Stack m_global_stack;     // A stack that all threads share.
  
  // Threads
  Apto::Array<Thread, Apto::ManagedPointer> m_threads;
  char m_thread_ids[MAX_THREADS];
  int m_cur_thread;
  
  int m_cur_offspring;
  
  int m_use_avatar;
  cOrgSensor m_sensor;
  
  struct {
    unsigned int m_cycle_count:16;
    unsigned int m_last_output:16;
  };
  
  // Flags
  struct {
    bool m_mal_active:1;         // Has an allocate occured since last divide?
    bool m_advance_ip:1;         // Should the IP advance after this instruction?
    bool m_spec_die:1;
    
    bool m_thread_slicing_parallel:1;
    bool m_no_cpu_cycle_time:1;
    
    bool m_slip_read_head:1;
    
    unsigned int m_waiting_threads:4;
  };
  
  cHardwareBCR(const cHardwareBCR&); // @not_implemented
  cHardwareBCR& operator=(const cHardwareBCR&); // @not_implemented
  
  
public:
  cHardwareBCR(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  ~cHardwareBCR() { ; }
  
  static tInstLib<cHardwareBCR::tMethod>* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "instset-BCR.cfg"; }
  
  
  // --------  Core Execution Methods  --------
  bool SingleProcess(cAvidaContext& ctx, bool speculative = false);
  void ProcessBonusInst(cAvidaContext& ctx, const Instruction& inst);

  
  // --------  Helper Methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_BCR; }
  bool SupportsSpeculative() const { return true; }
  void PrintStatus(std::ostream& fp);
  void SetupMiniTraceFileHeader(const cString& filename, const int gen_id, const cString& genotype);
  void PrintMiniTraceStatus(cAvidaContext& ctx, std::ostream& fp, const cString& next_name);
  void PrintMiniTraceSuccess(std::ostream& fp, const int exec_success);
  
  // --------  Stack Manipulation  --------
  inline int GetStack(int depth=0, int stack_id = -1, int in_thread = -1) const;
  inline int GetNumStacks() const { return 2; }
  
  
  // --------  Head Manipulation (including IP)  --------
  const cHeadCPU& GetHead(int head_id) const { return m_threads[m_cur_thread].heads[head_id]; }
  cHeadCPU& GetHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
  const cHeadCPU& GetHead(int head_id, int thread) const { return m_threads[thread].heads[head_id]; }
  cHeadCPU& GetHead(int head_id, int thread) { return m_threads[thread].heads[head_id];}
  int GetNumHeads() const { return NUM_HEADS; }
  
  const cHeadCPU& IP() const { return m_threads[m_cur_thread].heads[hIP]; }
  cHeadCPU& IP() { return m_threads[m_cur_thread].heads[hIP]; }
  const cHeadCPU& IP(int thread) const { return m_threads[thread].heads[hIP]; }
  cHeadCPU& IP(int thread) { return m_threads[thread].heads[hIP]; }
  
  
  // --------  Memory Manipulation  --------
  const cCPUMemory& GetMemory() const { return m_mem_array[0]; }
  cCPUMemory& GetMemory() { return m_mem_array[0]; }
  int GetMemSize() const { return m_mem_array[0].GetSize(); }
  const cCPUMemory& GetMemory(int idx) const { return m_mem_array[idx]; }
  cCPUMemory& GetMemory(int idx) { return m_mem_array[idx]; }
  int GetMemSize(int idx) const { return  m_mem_array[idx].GetSize(); }
  int GetNumMemSpaces() const { return m_mem_array.GetSize(); }
  
  
  // --------  Register Manipulation  --------
  int GetRegister(int reg_id) const { return m_threads[m_cur_thread].reg[reg_id].value; }
  int GetNumRegisters() const { return NUM_REGISTERS; }
  
  
  // --------  Thread Manipulation  --------
  bool ThreadSelect(const int thread_num);
  bool ThreadSelect(const cCodeLabel&) { return false; } // Labeled threads not supported
  inline void ThreadPrev(); // Shift the current thread in use.
  inline void ThreadNext();
  Systematics::UnitPtr ThreadGetOwner() { m_organism->AddReference(); return Systematics::UnitPtr(m_organism); }
  
  int GetNumThreads() const     { return m_threads.GetSize(); }
  int GetCurThread() const      { return m_cur_thread; }
  int GetCurThreadID() const    { return m_threads[m_cur_thread].thread_id; }
  
  // --------  Non-Standard Methods  --------
  int GetActiveStack() const { return m_threads[m_cur_thread].cur_stack; }
  bool GetMalActive() const   { return m_mal_active; }


  // interrupt current thread @ not implemented
  bool InterruptThread(int interruptType) { return false; }
  int GetThreadMessageTriggerType(int _index) { return -1; }
  
  // --------  Parasite Stuff  -------- @ not implemented
  bool ParasiteInfectHost(Systematics::UnitPtr) { return false; }

  
private:
  // --------  Core Execution Methods  --------
  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const Instruction& cur_inst);
  void internalReset();
  void internalResetOnFailedDivide();
  
  
  // --------  Stack Manipulation  --------
  inline DataValue stackPop();
  inline Stack& getStack(int stack_id);
  inline void switchStack();
    
  // --------  Head Manipulation (including IP)  --------
  cHeadCPU& GetActiveHead() { return m_threads[m_cur_thread].heads[m_threads[m_cur_thread].cur_head]; }
  void AdjustHeads();
    
  // --------  Label Manipulation  -------
  const cCodeLabel& GetLabel() const { return m_threads[m_cur_thread].next_label; }
  cCodeLabel& GetLabel() { return m_threads[m_cur_thread].next_label; }
  void ReadLabel(int max_size = cCodeLabel::MAX_LENGTH);
  cHeadCPU FindLabelStart(bool mark_executed);
  cHeadCPU FindLabelForward(bool mark_executed);
  cHeadCPU FindLabelBackward(bool mark_executed);
  cHeadCPU FindNopSequenceStart(bool mark_executed);
  cHeadCPU FindNopSequenceForward(bool mark_executed);
  cHeadCPU FindNopSequenceBackward(bool mark_executed);
  inline const cCodeLabel& GetReadLabel() const { return m_threads[m_cur_thread].read_label; }
  inline const cCodeLabel& GetReadSequence() const { return m_threads[m_cur_thread].read_seq; }
  inline cCodeLabel& GetReadLabel() { return m_threads[m_cur_thread].read_label; }
  inline cCodeLabel& GetReadSequence() { return m_threads[m_cur_thread].read_seq; }
  
  
  // --------  Thread Manipulation  -------
  bool ThreadCreate(int thread_label, const cHeadCPU& start_pos);
  
  
  // ---------- Instruction Helpers -----------
  int FindModifiedRegister(int default_register);
  int FindModifiedNextRegister(int default_register);
  int FindModifiedPreviousRegister(int default_register);
  int FindModifiedHead(int default_head);
  int FindNextRegister(int base_reg);
  
  int calcCopiedSize(const int parent_size, const int child_size);
  
  inline const cHeadCPU& getHead(int head_id) const { return m_threads[m_cur_thread].heads[head_id]; }
  inline cHeadCPU& getHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
  inline const cHeadCPU& getHead(int head_id, int thread) const { return m_threads[thread].heads[head_id]; }
  inline cHeadCPU& getHead(int head_id, int thread) { return m_threads[thread].heads[head_id];}
  
  inline const cHeadCPU& getIP() const { return m_threads[m_cur_thread].heads[hIP]; }
  inline cHeadCPU& getIP() { return m_threads[m_cur_thread].heads[hIP]; }
  inline const cHeadCPU& getIP(int thread) const { return m_threads[thread].heads[hIP]; }
  inline cHeadCPU& getIP(int thread) { return m_threads[thread].heads[hIP]; }

  // --------  Division Support  -------
  bool Divide_Main(cAvidaContext& ctx, double mut_multiplier=1);
  

  // ---------- Utility Functions -----------
  inline void setInternalValue(int reg_num, int value, bool from_env = false);
  inline void setInternalValue(int reg_num, int value, const DataValue& src);
  inline void setInternalValue(int reg_num, int value, const DataValue& op1, const DataValue& op2);
  void checkWaitingThreads(int cur_thread, int reg_num);

  void ReadInst(Instruction in_inst);
  
  
  // ---------- Instruction Library -----------
  // Multi-threading
  bool Inst_ThreadCreate(cAvidaContext& ctx);
  bool Inst_ThreadCancel(cAvidaContext& ctx);
  bool Inst_ThreadID(cAvidaContext& ctx);
  bool Inst_Yield(cAvidaContext& ctx);
  
  // Flow Control
  bool Inst_Label(cAvidaContext& ctx);
  bool Inst_IfNEqu(cAvidaContext& ctx);
  bool Inst_IfLess(cAvidaContext& ctx);
  bool Inst_IfNotZero(cAvidaContext& ctx);
  bool Inst_IfEqualZero(cAvidaContext& ctx);
  bool Inst_IfGreaterThanZero(cAvidaContext& ctx);
  bool Inst_IfLessThanZero(cAvidaContext& ctx);
  bool Inst_IfGtrX(cAvidaContext& ctx);
  bool Inst_IfEquX(cAvidaContext& ctx);

  // Stack and Register Operations
  bool Inst_Pop(cAvidaContext& ctx);
  bool Inst_Push(cAvidaContext& ctx);
  bool Inst_PopAll(cAvidaContext& ctx);
  bool Inst_PushAll(cAvidaContext& ctx);
  bool Inst_SwitchStack(cAvidaContext& ctx);
  bool Inst_SwapStackTop(cAvidaContext& ctx);
  bool Inst_Swap(cAvidaContext& ctx);

  // Single-Argument Math
  bool Inst_ShiftR(cAvidaContext& ctx);
  bool Inst_ShiftL(cAvidaContext& ctx);
  bool Inst_Inc(cAvidaContext& ctx);
  bool Inst_Dec(cAvidaContext& ctx);
  bool Inst_Zero(cAvidaContext& ctx);
  bool Inst_One(cAvidaContext& ctx);
  bool Inst_Rand(cAvidaContext& ctx);
  
  // Double Argument Math
  bool Inst_Add(cAvidaContext& ctx);
  bool Inst_Sub(cAvidaContext& ctx);
  bool Inst_Mult(cAvidaContext& ctx);
  bool Inst_Div(cAvidaContext& ctx);
  bool Inst_Mod(cAvidaContext& ctx);
  bool Inst_Nand(cAvidaContext& ctx);

  // I/O and Sensory
  bool Inst_TaskIO(cAvidaContext& ctx);
  bool Inst_TaskInput(cAvidaContext& ctx);
  bool Inst_TaskOutput(cAvidaContext& ctx);

  // Head-based Instructions
  bool Inst_SetMemory(cAvidaContext& ctx);
  bool Inst_MoveHead(cAvidaContext& ctx);
  bool Inst_JumpHead(cAvidaContext& ctx);
  bool Inst_GetHead(cAvidaContext& ctx);
  bool Inst_Divide(cAvidaContext& ctx);
  bool Inst_HeadRead(cAvidaContext& ctx);
  bool Inst_HeadWrite(cAvidaContext& ctx);
  bool Inst_HeadCopy(cAvidaContext& ctx);
  
  bool Inst_Search_Label_Comp_S(cAvidaContext& ctx);
  bool Inst_Search_Label_Comp_F(cAvidaContext& ctx);
  bool Inst_Search_Label_Comp_B(cAvidaContext& ctx);
  bool Inst_Search_Label_Direct_S(cAvidaContext& ctx);
  bool Inst_Search_Label_Direct_F(cAvidaContext& ctx);
  bool Inst_Search_Label_Direct_B(cAvidaContext& ctx);
  bool Inst_Search_Seq_Comp_S(cAvidaContext& ctx);
  bool Inst_Search_Seq_Comp_F(cAvidaContext& ctx);
  bool Inst_Search_Seq_Comp_B(cAvidaContext& ctx);
  bool Inst_Search_Seq_Direct_S(cAvidaContext& ctx);
  bool Inst_Search_Seq_Direct_F(cAvidaContext& ctx);
  bool Inst_Search_Seq_Direct_B(cAvidaContext& ctx);

  // Thread Execution Control
  bool Inst_WaitCondition_Equal(cAvidaContext& ctx);
  bool Inst_WaitCondition_Less(cAvidaContext& ctx);
  bool Inst_WaitCondition_Greater(cAvidaContext& ctx);
  
  // Replication
  bool Inst_IfCopiedCompLabel(cAvidaContext& ctx);
  bool Inst_IfCopiedDirectLabel(cAvidaContext& ctx);
  bool Inst_IfCopiedCompSeq(cAvidaContext& ctx);
  bool Inst_IfCopiedDirectSeq(cAvidaContext& ctx);
  bool Inst_Repro(cAvidaContext& ctx);
  bool Inst_Die(cAvidaContext& ctx);
  
  // Movement and Navigation
  bool Inst_Move(cAvidaContext& ctx);
  bool Inst_GetNorthOffset(cAvidaContext& ctx);
  bool Inst_GetPositionOffset(cAvidaContext& ctx);  
  bool Inst_GetNortherly(cAvidaContext& ctx); 
  bool Inst_GetEasterly(cAvidaContext& ctx);
  bool Inst_ZeroEasterly(cAvidaContext& ctx);
  bool Inst_ZeroNortherly(cAvidaContext& ctx);
  bool Inst_ZeroPosOffset(cAvidaContext& ctx);
  
  // Rotation
  bool Inst_RotateHome(cAvidaContext& ctx);
  bool Inst_RotateUnoccupiedCell(cAvidaContext& ctx);
  bool Inst_RotateX(cAvidaContext& ctx);

  // Resource and Topography Sensing
  bool Inst_SenseResourceID(cAvidaContext& ctx); 
  bool Inst_SenseNest(cAvidaContext& ctx);
  bool Inst_SenseFacedHabitat(cAvidaContext& ctx);
  bool Inst_LookAhead(cAvidaContext& ctx);
  bool Inst_LookAheadIntercept(cAvidaContext& ctx);
  bool Inst_LookAround(cAvidaContext& ctx);
  bool Inst_LookAroundIntercept(cAvidaContext& ctx);
  bool Inst_LookFT(cAvidaContext& ctx);
  bool Inst_LookAroundFT(cAvidaContext& ctx);

  // Foraging
  bool Inst_SetForageTarget(cAvidaContext& ctx);
  bool Inst_SetForageTargetOnce(cAvidaContext& ctx);
  bool Inst_SetRandForageTargetOnce(cAvidaContext& ctx);
  bool Inst_GetForageTarget(cAvidaContext& ctx);
  
  // Collection
  bool DoActualCollect(cAvidaContext& ctx, int bin_used, bool unit);
  bool Inst_CollectSpecific(cAvidaContext& ctx);
  bool Inst_GetResStored(cAvidaContext& ctx);

  // Groups 
  bool Inst_SetOpinion(cAvidaContext& ctx);
  bool Inst_GetOpinion(cAvidaContext& ctx);
  bool Inst_JoinGroup(cAvidaContext& ctx);
  bool Inst_GetGroupID(cAvidaContext& ctx);

  // Org Interactions
  bool Inst_GetFacedOrgID(cAvidaContext& ctx);

  bool Inst_TeachOffspring(cAvidaContext& ctx);
  bool Inst_LearnParent(cAvidaContext& ctx);
  
  // Control-type Instructions
  bool Inst_ScrambleReg(cAvidaContext& ctx);


  // ---------- Some Instruction Helpers -----------
  struct sLookRegAssign {
    int habitat;
    int distance;
    int search_type;
    int id_sought;
    int count;
    int value;
    int group;
    int ft;
  };
  
  bool GoLook(cAvidaContext& ctx, const int look_dir, const int cell_id, bool use_ft = false);
  cOrgSensor::sLookOut InitLooking(cAvidaContext& ctx, sLookRegAssign& lookin_defs, int facing, int cell_id, bool use_ft = false);
  void LookResults(sLookRegAssign& lookin_defs, cOrgSensor::sLookOut& look_results);
  
private:
  static tInstLib<cHardwareBCR::tMethod>* initInstLib();
};


inline cHardwareBCR::DataValue& cHardwareBCR::DataValue::operator=(const DataValue& i)
{
  value = i.value;
  originated = i.originated;
  from_env = i.from_env;
  oldest_component = i.oldest_component;
  env_component = i.env_component;
  return *this;
}

inline bool cHardwareBCR::ThreadSelect(const int thread_num)
{
  if (thread_num >= 0 && thread_num < m_threads.GetSize()) {
    m_cur_thread = thread_num;
    return true;
  }
  
  return false;
}

inline void cHardwareBCR::ThreadNext()
{
  m_cur_thread++;
  if (m_cur_thread >= m_threads.GetSize()) m_cur_thread = 0;
}

inline void cHardwareBCR::ThreadPrev()
{
  if (m_cur_thread == 0) m_cur_thread = m_threads.GetSize() - 1;
  else m_cur_thread--;
}

inline cHardwareBCR::DataValue cHardwareBCR::stackPop()
{
  if (m_threads[m_cur_thread].cur_stack == 0) {
    return m_threads[m_cur_thread].stack.Pop();
  } else {
    return m_global_stack.Pop();
  }
}

inline cHardwareBCR::Stack& cHardwareBCR::getStack(int stack_id)
{
  if (stack_id == 0) {
    return m_threads[m_cur_thread].stack;
  } else {
    return m_global_stack;
  }
}

inline void cHardwareBCR::switchStack()
{
  m_threads[m_cur_thread].cur_stack++;
  if (m_threads[m_cur_thread].cur_stack > 1) m_threads[m_cur_thread].cur_stack = 0;
}


inline int cHardwareBCR::GetStack(int depth, int stack_id, int in_thread) const
{
  DataValue value;

  if(in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;

  if (stack_id == -1) stack_id = m_threads[in_thread].cur_stack;

  if (stack_id == 0) value = m_threads[in_thread].stack.Get(depth);
  else if (stack_id == 1) value = m_global_stack.Get(depth);

  return value.value;
}

inline void cHardwareBCR::setInternalValue(int reg_num, int value, bool from_env)
{
  DataValue& dest = m_threads[m_cur_thread].reg[reg_num];
  dest.value = value;
  dest.from_env = from_env;
  dest.originated = m_cycle_count;
  dest.oldest_component = m_cycle_count;
  dest.env_component = from_env;
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}

inline void cHardwareBCR::setInternalValue(int reg_num, int value, const DataValue& src)
{
  DataValue& dest = m_threads[m_cur_thread].reg[reg_num];
  dest.value = value;
  dest.from_env = false;
  dest.originated = m_cycle_count;
  dest.oldest_component = src.oldest_component;
  dest.env_component = src.env_component;
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}

inline void cHardwareBCR::setInternalValue(int reg_num, int value, const DataValue& op1, const DataValue& op2)
{
  DataValue& dest = m_threads[m_cur_thread].reg[reg_num];
  dest.value = value;
  dest.from_env = false;
  dest.originated = m_cycle_count;
  dest.oldest_component = (op1.oldest_component < op2.oldest_component) ? op1.oldest_component : op2.oldest_component;
  dest.env_component = (op1.env_component || op2.env_component);
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}


#endif
