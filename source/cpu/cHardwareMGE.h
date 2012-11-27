/*
 *  cHardwareMGE.h
 *  Avida
 *
 *  Created by APWagner on 10/26/2012 based on cHardwareMGE.h
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#ifndef cHardwareMGE_h
#define cHardwareMGE_h

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

#include "nHardware.h"

#include "tInstLib.h"
#include "cEnvReqs.h"
#include "cEnvironment.h"

#include <cstring>
#include <iomanip>

/**
 * Each organism may have a cHardwareMGE structure which keeps track of the
 * current status of all the components of the simulated hardware.
 *
 * @see cCPUMemory, cInstSet
 **/

class cInstLib;
class cInstSet;
class cMutation;
class cOrganism;

class cHardwareMGE : public cHardwareBase
{
public:
  typedef bool (cHardwareMGE::*tMethod)(cAvidaContext& ctx);

private:
  // --------  Structure Constants  --------
  static const int NUM_REGISTERS = 8;
  static const int NUM_BEHAVIORS = 3; // num inst types capable of storing their own data
  enum tRegisters { rAX = 0, rBX, rCX, rDX, rEX, rFX, rGX, rHX, rIX, rJX, rKX, rLX, rMX, rNX, rOX, rPX };

  enum mHEADS { mIP = 0, mFH, mRH, mWH };
  enum thHEADS { thIP = 0, thFH };
  static const int NUM_M_HEADS = 4;
  static const int NUM_TH_HEADS = 2;

  static const int NUM_NOPS = NUM_REGISTERS;
  
  // --------  Static Variables  --------
  static tInstLib<cHardwareMGE::tMethod>* s_inst_slib;
  static tInstLib<cHardwareMGE::tMethod>* initInstLib(void);
  
  // --------  Define Internal Data Structures  --------
  struct sInternalValue
  {
    int value;
    
    // Actual age of this value
    unsigned int originated:15;
    unsigned int from_env:1;
    
    // Age of the oldest component used to create this value
    unsigned int oldest_component:15;
    unsigned int env_component:1;
    
    inline sInternalValue() : value(0) { ; }
    inline void Clear() { value = 0; originated = 0; from_env = 0, oldest_component = 0; env_component = 0; }
    inline sInternalValue& operator=(const sInternalValue& i);
  };
  
  
  class cLocalStack
  {
#define SIZE nHardware::STACK_SIZE
  private:
    sInternalValue m_stack[SIZE];
    char m_sp;
    
  public:
    cLocalStack() : m_sp(0) { Clear(); }
    inline cLocalStack(const cLocalStack& is) : m_sp(is.m_sp) { for (int i = 0; i < SIZE; i++) m_stack[i] = is.m_stack[i]; }
    ~cLocalStack() { ; }
    
    inline void operator=(const cLocalStack& is) { m_sp = is.m_sp; for (int i = 0; i < SIZE; i++) m_stack[i] = is.m_stack[i]; }
    
    inline void Push(const sInternalValue& value) { if (--m_sp < 0) m_sp = SIZE - 1; m_stack[(int)m_sp] = value; }
    inline sInternalValue Pop() { sInternalValue v = m_stack[(int)m_sp]; m_stack[(int)m_sp].Clear(); if (++m_sp == SIZE) m_sp = 0; return v; }
    inline sInternalValue& Peek() { return m_stack[(int)m_sp]; }
    inline const sInternalValue& Peek() const { return m_stack[(int)m_sp]; }
    inline const sInternalValue& Get(int d = 0) const { assert(d >= 0); int p = d + m_sp; return m_stack[(p >= SIZE) ? (p - SIZE) : p]; }
    inline void Clear() { for (int i = 0; i < SIZE; i++) m_stack[i].Clear(); }
#undef SIZE
  };
  
  struct cBehavThread
  {
  private:
  
  public:
    cHeadCPU thHeads[NUM_TH_HEADS];
    cCPUMemory thread_mem;
    int mem_id;
    int thread_class;
    int start;
    int end;
    
    struct {
      bool reading_label:1;
      bool reading_seq:1;
      bool active:1;
      bool wait_greater:1;
      bool wait_equal:1;
      bool wait_less:1;
      unsigned int wait_reg:4;
      unsigned int wait_dst:4;
    };
    int wait_value;
    
    cCodeLabel read_label;
    cCodeLabel read_seq;
    cCodeLabel next_label;
  
    inline cBehavThread() { ; }
    cBehavThread(cHardwareMGE* in_hardware, int in_id) { Reset(in_hardware, in_id); }
    ~cBehavThread() { ; }
    
    void operator=(const cBehavThread& in_proc);
    void Reset(cHardwareMGE* in_hardware, int in_id);
  };

 struct cBehavProc
  {
    sInternalValue reg[NUM_REGISTERS];
    cLocalStack stack;
    unsigned char cur_stack;          // 0 = local stack, 1 = global stack.
    Apto::Array<int> bp_thread_ids;
    int bp_cur_thread;
  };

  // --------  Member Variables  --------
  const tMethod* m_functions;
  cHeadCPU mHeads[NUM_M_HEADS];               // Keep a set of heads to walk through parent's genome
  cCPUMemory main_memory;                     // Memory...
  cCPUMemory child_memory;                    // Spot for the offspring
  cLocalStack m_global_stack;                 // A stack that all behavioral processes share.
  
  Apto::Array<cBehavThread> m_threads;          // The hardware is a collection of threads, each with a behavioral class type
  Apto::Array<cBehavProc> m_bps;                // The 3 behavioral proceses keep the registers and stacks.
  unsigned int m_waiting_threads;
  unsigned int m_cur_thread;
  unsigned int m_cur_behavior;
  
  int m_use_avatar;
  cOrgSensor m_sensor;
  
  struct {
    unsigned int m_cycle_count:16;
    unsigned int m_last_output:16;
  };
  
  // Flags
  struct {
    bool m_has_alloc:1;         // Has an allocate occured since last divide?
    bool m_advance_ip:1;         // Should the IP advance after this instruction?
    bool m_executedmatchstrings:1;	// Have we already executed the match strings instruction?
    bool m_spec_die:1;
    
    int m_cur_offspring:5;

    bool m_thread_slicing_parallel:1;
    bool m_no_cpu_cycle_time:1;
    
    bool m_slip_read_head:1;    
  };
  
  cHardwareMGE(const cHardwareMGE&); // @not_implemented
  cHardwareMGE& operator=(const cHardwareMGE&); // @not_implemented  
  
public:
  cHardwareMGE(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  ~cHardwareMGE() { ; }
  
  static tInstLib<cHardwareMGE::tMethod>* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "instset-MGE.cfg"; }
  
  // --------  Behavioral Processes
  inline int GetCurrBehav() const { return m_threads[m_cur_thread].thread_class; }
  int PreclassNewGeneBehavior(int cur_class, int pos);
  int GetNextGeneClass(int position, int seq_size, int cur_class);
  BehavClass GetBehavClass(int classid);
  inline void IncBehavior() {  m_cur_behavior = (m_cur_behavior + 1) % NUM_BEHAVIORS; }
  inline void IncThread() {
    m_bps[m_cur_behavior].bp_cur_thread++;
    if ((int) m_bps[m_cur_behavior].bp_cur_thread >= m_bps[m_cur_behavior].bp_thread_ids.GetSize()) m_bps[m_cur_behavior].bp_cur_thread = 0;
  }
  inline bool AllUsed(Apto::Array <int>& bp_exec_count, Apto::Array <int>& gene_count, int max_exec_count) {
    int num_used = 0;
    for (int i = 0; i < NUM_BEHAVIORS; i ++) {
      if (bp_exec_count[i] >= max_exec_count || gene_count[i] >= max_exec_count) num_used++;
    }
    return num_used >= NUM_BEHAVIORS;
  }

  // --------  Core Execution Methods  --------
  bool SingleProcess(cAvidaContext& ctx, bool speculative = false);
  void ProcessBonusInst(cAvidaContext& ctx, const Instruction& inst);
  
  // --------  Helper Methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_MGE; }
  bool SupportsSpeculative() const { return true; }
  void PrintStatus(std::ostream& fp);                                                                 // not implemented for this hardware
  void SetupMiniTraceFileHeader(const cString& filename, const int gen_id, const cString& genotype);
  void PrintMiniTraceStatus(cAvidaContext& ctx, std::ostream& fp, const cString& next_name);
  void PrintMiniTraceSuccess(std::ostream& fp, const int exec_success);
  
  // --------  Stack Manipulation  --------
  inline int GetStack(int depth=0, int stack_id = -1, int in_thread = -1) const;
  inline int GetNumStacks() const { return 2; }
  
  // --------  Head Manipulation (including IP)  --------
  // main memory
  const cHeadCPU& IP() const { return mHeads[mIP]; }
  cHeadCPU& IP() { return mHeads[mIP]; }
  const cHeadCPU& GetHead(int head_id) const { return mHeads[head_id];  }
  cHeadCPU& GetHead(int head_id) { return mHeads[head_id];  }

  // gene threads
  const cHeadCPU& IP(int thread) const { return m_threads[thread].thHeads[thIP]; }    // NOT to be used for getting mHEADS!
  cHeadCPU& IP(int thread) { return m_threads[thread].thHeads[thIP]; }
  const cHeadCPU& GetHead(int head_id, int thread) const { return m_threads[thread].thHeads[head_id]; }
  cHeadCPU& GetHead(int head_id, int thread) { return m_threads[thread].thHeads[head_id]; }

  int GetNumHeads() const { return NUM_M_HEADS; }
  
  // --------  Memory Manipulation  --------
  const cCPUMemory& GetMemory() const { return main_memory; }
  cCPUMemory& GetMemory() { return main_memory; }
  int GetMemSize() const { return main_memory.GetSize(); }
  
  const cCPUMemory& GetMemory(int mem_id) const {
    if (mem_id == 0) return main_memory;
    else if (mem_id == 1) return child_memory;
    return m_threads[mem_id - 2].thread_mem;
  }
  cCPUMemory& GetMemory(int mem_id) {
    if (mem_id == 0) return main_memory;
    else if (mem_id == 1) return child_memory;
    return m_threads[mem_id - 2].thread_mem;
  }
  int GetMemSize(int mem_id) const {
    if (mem_id == 0) return main_memory.GetSize();
    else if (mem_id == 1) return child_memory.GetSize();
    return m_threads[mem_id - 2].thread_mem.GetSize();
  }

  int GetNumMemSpaces() const { return m_threads.GetSize() + 2; }
  
  // --------  Register Manipulation  --------
  int GetRegVal(int reg_id) const { return GetRegister(reg_id); }
  int GetRegister(int reg_id) const { return m_bps[GetCurrBehav()].reg[reg_id].value; }
  int GetNumRegisters() const { return NUM_REGISTERS; }
  
  // --------  Thread Manipulation  --------
  bool ThreadSelect(const int thread_num);
  bool ThreadSelect(const cCodeLabel&) { return false; } // Labeled threads not supported
  inline void ThreadPrev(); // Shift the current thread in use.
  inline void ThreadNext();
  Systematics::UnitPtr ThreadGetOwner() { m_organism->AddReference(); return Systematics::UnitPtr(m_organism); }
  
  int GetNumThreads() const     { return GetNumMemSpaces(); }
  int GetCurThread() const      { return GetCurThreadID(); }
  int GetCurThreadID() const    { return m_cur_thread; }
  
  // --------  Non-Standard Methods  --------
  int GetActiveStack() const { return m_bps[GetCurrBehav()].cur_stack; }
  bool GetMalActive() const   { return m_has_alloc; }


  // --------  Parasite Stuff  -------- @ not implemented
  bool ParasiteInfectHost(Systematics::UnitPtr) { return false; }

private:
  
  // --------  Core Execution Methods  --------
  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const Instruction& cur_inst);
  void internalReset();
  void internalResetOnFailedDivide();  
  
  // --------  Stack Manipulation  --------
  inline sInternalValue stackPop();
  inline cLocalStack& getStack(int stack_id);
  inline void switchStack();
    
  // --------  Head Manipulation (including IP)  --------
  // for tracking reasons, movement of main flow and ip heads to mirror that of thread heads
  inline void Adjust(cHeadCPU& head, int head_id) {
    head.Adjust();
    MirrorHeads(head, head_id);
  }
  inline void Advance(cHeadCPU& head, int head_id) {
    head.Advance();
    MirrorHeads(head, head_id);
  }
  // move the main memory head to the correct position in this mem_space to match the location in the thread mem space
  inline void MirrorHeads(cHeadCPU& head, int head_id) { mHeads[head_id].Set(head.GetPosition() + m_threads[m_cur_thread].start); }
  
  // --------  Label Manipulation  -------
  const cCodeLabel& GetLabel() const { return m_threads[m_cur_thread].next_label; }
  cCodeLabel& GetLabel() { return m_threads[m_cur_thread].next_label; }
  void ReadLabel(int max_size = cCodeLabel::MAX_LENGTH);
  cHeadCPU FindLabelStart(bool mark_executed);
  cHeadCPU FindLabelForward(bool mark_executed);
  cHeadCPU FindLabelBackward(bool mark_executed);
  int FindNopSequenceOrgStart(bool mark_executed);
  cHeadCPU FindNopSequenceStart(bool mark_executed);
  cHeadCPU FindNopSequenceForward(bool mark_executed);
  cHeadCPU FindNopSequenceBackward(bool mark_executed);
  inline const cCodeLabel& GetReadLabel() const { return m_threads[m_cur_thread].read_label; }
  inline const cCodeLabel& GetReadSequence() const { return m_threads[m_cur_thread].read_seq; }
  inline cCodeLabel& GetReadLabel() { return m_threads[m_cur_thread].read_label; }
  inline cCodeLabel& GetReadSequence() { return m_threads[m_cur_thread].read_seq; }  
  
  // --------  Thread Manipulation  -------
  void CreateBPThreads();   // Create a new thread for this behavioral process.
  
  // ---------- Instruction Helpers -----------
  int FindModifiedRegister(int default_register);
  int FindModifiedNextRegister(int default_register);
  int FindModifiedPreviousRegister(int default_register);
  int FindModifiedHead(int default_head);
  int FindNextRegister(int base_reg);
  
  int calcCopiedSize(const int parent_size, const int child_size);
  
  // main memory
  inline cHeadCPU& getRH() { return mHeads[mRH];  }
  inline cHeadCPU& getWH() { return mHeads[mWH];  }  
  inline cHeadCPU& getFH() { return mHeads[mFH];  }
  
  inline const cHeadCPU& getIP() const { return mHeads[mIP]; }
  inline cHeadCPU& getIP() { return mHeads[mIP]; }
  inline const cHeadCPU& getHead(int head_id) const { return mHeads[head_id]; }
  inline cHeadCPU& getHead(int head_id) { return mHeads[head_id];  }

  // thread memory...not to be used for mHeads!!!
  inline const cHeadCPU& getThIP() const { return getHead(thIP, GetCurThreadID()); }
  inline cHeadCPU& getThIP() { return getHead(thIP, GetCurThreadID()); }
  inline const cHeadCPU& getThHead(int head_id) const { return getHead(head_id, GetCurThreadID()); }
  inline cHeadCPU& getThHead(int head_id) { return getHead(head_id, GetCurThreadID());  }

  inline const cHeadCPU& getIP(int thread) const { return m_threads[thread].thHeads[thIP]; }
  inline cHeadCPU& getIP(int thread) { return m_threads[thread].thHeads[thIP]; }  
  inline const cHeadCPU& getHead(int head_id, int thread) const { return m_threads[thread].thHeads[head_id]; }
  inline cHeadCPU& getHead(int head_id, int thread) { return m_threads[thread].thHeads[head_id]; }
  
  // --------  Division Support  -------
  bool Divide_Main(cAvidaContext& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1);  

  // ---------- Utility Functions -----------
  inline unsigned int BitCount(unsigned int value) const;
  inline void setInternalValue(int reg_num, int value, bool from_env = false);
  inline void setInternalValue(int reg_num, int value, const sInternalValue& src);
  inline void setInternalValue(int reg_num, int value, const sInternalValue& op1, const sInternalValue& op2);
  void checkWaitingThreads(int cur_thread, int reg_num);

  void ReadInst(Instruction in_inst);
  
  // ---------- Instruction Library -----------
  bool Inst_IdThread(cAvidaContext& ctx);
  
  // --------  Behavior Execution  --------
  bool Inst_StartGene(cAvidaContext& ctx);
  bool Inst_EndGene(cAvidaContext& ctx);
  
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
  bool Inst_Alloc(cAvidaContext& ctx);
  bool Inst_Copy(cAvidaContext& ctx);
  bool Inst_Divide(cAvidaContext& ctx);
  bool Inst_Repro(cAvidaContext& ctx);
  bool Inst_Die(cAvidaContext& ctx);
  
  bool Inst_MoveHead(cAvidaContext& ctx);
  bool Inst_JumpHead(cAvidaContext& ctx);
  bool Inst_GetHead(cAvidaContext& ctx);
  bool Inst_JumpGene(cAvidaContext& ctx);
  bool Inst_JumpBehavior(cAvidaContext& ctx);
  
  bool Inst_Search_Seq_Comp_S(cAvidaContext& ctx);
  bool Inst_Search_Seq_Comp_F(cAvidaContext& ctx);
  bool Inst_Search_Seq_Comp_B(cAvidaContext& ctx);
  bool Inst_Search_Seq_Direct_S(cAvidaContext& ctx);
  bool Inst_Search_Seq_Direct_F(cAvidaContext& ctx);
  bool Inst_Search_Seq_Direct_B(cAvidaContext& ctx);
  bool Inst_Search_Label_Direct_S(cAvidaContext& ctx);

  // Thread Execution Control
  bool Inst_WaitCondition_Equal(cAvidaContext& ctx);
  bool Inst_WaitCondition_Less(cAvidaContext& ctx);
  bool Inst_WaitCondition_Greater(cAvidaContext& ctx);
  
  // Replication
  bool Inst_IfCopiedCompLabel(cAvidaContext& ctx);
  bool Inst_IfCopiedDirectLabel(cAvidaContext& ctx);
  bool Inst_IfCopiedCompSeq(cAvidaContext& ctx);
  bool Inst_IfCopiedDirectSeq(cAvidaContext& ctx);

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
  

public:
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
};


inline cHardwareMGE::sInternalValue& cHardwareMGE::sInternalValue::operator=(const sInternalValue& i)
{
  value = i.value;
  originated = i.originated;
  from_env = i.from_env;
  oldest_component = i.oldest_component;
  env_component = i.env_component;
  return *this;
}

inline bool cHardwareMGE::ThreadSelect(const int thread_num)
{
  if (thread_num >= 0 && thread_num < m_threads.GetSize()) {
    m_cur_thread = thread_num;
    return true;
  }
  return false;
}

inline void cHardwareMGE::ThreadNext()
{
  m_cur_thread++;
  if ((int) m_cur_thread >= m_threads.GetSize()) m_cur_thread = 0;
}

inline void cHardwareMGE::ThreadPrev()
{
  if (m_cur_thread == 0) m_cur_thread = m_threads.GetSize() - 1;
  else m_cur_thread--;
}

inline cHardwareMGE::sInternalValue cHardwareMGE::stackPop()
{
  if (m_bps[GetCurrBehav()].cur_stack == 0) {
    return m_bps[GetCurrBehav()].stack.Pop();
  } else {
    return m_global_stack.Pop();
  }
}

inline cHardwareMGE::cLocalStack& cHardwareMGE::getStack(int stack_id)
{
  if (stack_id == 0) {
    return m_bps[GetCurrBehav()].stack;
  } else {
    return m_global_stack;
  }
}

inline void cHardwareMGE::switchStack()
{
  m_bps[GetCurrBehav()].cur_stack++;
  if (m_bps[GetCurrBehav()].cur_stack > 1) m_bps[GetCurrBehav()].cur_stack = 0;
}


inline int cHardwareMGE::GetStack(int depth, int stack_id, int in_thread) const
{
  sInternalValue value;

  if (stack_id == -1) stack_id = m_bps[GetCurrBehav()].cur_stack;

  if (stack_id == 0) value = m_bps[GetCurrBehav()].stack.Get(depth);
  else if (stack_id == 1) value = m_global_stack.Get(depth);

  return value.value;
}

inline void cHardwareMGE::setInternalValue(int reg_num, int value, bool from_env)
{
  sInternalValue& dest = m_bps[GetCurrBehav()].reg[reg_num];
  dest.value = value;
  dest.from_env = from_env;
  dest.originated = m_cycle_count;
  dest.oldest_component = m_cycle_count;
  dest.env_component = from_env;
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}

inline void cHardwareMGE::setInternalValue(int reg_num, int value, const sInternalValue& src)
{
  sInternalValue& dest = m_bps[GetCurrBehav()].reg[reg_num];
  dest.value = value;
  dest.from_env = false;
  dest.originated = m_cycle_count;
  dest.oldest_component = src.oldest_component;
  dest.env_component = src.env_component;
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}

inline void cHardwareMGE::setInternalValue(int reg_num, int value, const sInternalValue& op1, const sInternalValue& op2)
{
  sInternalValue& dest = m_bps[GetCurrBehav()].reg[reg_num];
  dest.value = value;
  dest.from_env = false;
  dest.originated = m_cycle_count;
  dest.oldest_component = (op1.oldest_component < op2.oldest_component) ? op1.oldest_component : op2.oldest_component;
  dest.env_component = (op1.env_component || op2.env_component);
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}


#endif
