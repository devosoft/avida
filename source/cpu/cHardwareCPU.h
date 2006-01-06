/*
 *  cHardwareCPU.h
 *  Avida
 *
 *  Created by David on 11/17/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cHardwareCPU_h
#define cHardwareCPU_h

#include <iomanip>
#include <vector>

#ifndef defs_h
#include "defs.h"
#endif
#ifndef cCodeLabel_h
#include "cCodeLabel.h"
#endif
#ifndef nHardware_h
#include "nHardware.h"
#endif
#ifndef nHardwareCPU_h
#include "nHardwareCPU.h"
#endif
#ifndef cHeadCPU_h
#include "cHeadCPU.h"
#endif
#ifndef cCPUMemory_h
#include "cCPUMemory.h"
#endif
#ifndef cCPUStack_h
#include "cCPUStack.h"
#endif
#ifndef cHardwareBase_h
#include "cHardwareBase.h"
#endif
#ifndef cHardwareCPU_Thread_h
#include "cHardwareCPU_Thread.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

/**
 * Each organism may have a cHardwareCPU structure which keeps track of the
 * current status of all the components of the simulated hardware.
 *
 * @see cHardwareCPU_Thread, cCPUStack, cCPUMemory, cInstSet
 **/

class cCodeLabel; // access
class cGenome;
class cInjectGenotype;
class cInstLibBase;
class cInstLibCPU;
class cInstruction;
class cInstSet;
class cMutation;
class cOrganism;
template <class T> class tBuffer;
template <class T> class tArray; // aggregate

class cHardwareCPU : public cHardwareBase {
public:
  typedef bool (cHardwareCPU::*tHardwareCPUMethod)();

private:
  static cInstLibCPU *s_inst_slib;
  static cInstLibCPU *initInstLib(void);

  tHardwareCPUMethod *m_functions;

  cCPUMemory memory;          // Memory...
  cCPUStack global_stack;     // A stack that all threads share.
  int thread_time_used;

  tArray<cHardwareCPU_Thread> threads;
  int thread_id_chart;
  int cur_thread;

  // Flags...
  bool mal_active;         // Has an allocate occured since last divide?
  bool advance_ip;         // Should the IP advance after this instruction?

  // Instruction costs...
#ifdef INSTRUCTION_COSTS
  tArray<int> inst_cost;
  tArray<int> inst_ft_cost;
#endif

public:
  cHardwareCPU(cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  explicit cHardwareCPU(const cHardwareCPU&);
  ~cHardwareCPU();
  static cInstLibCPU* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "inst_lib.default"; }

  void Reset();
  void SingleProcess();
  void ProcessBonusInst(const cInstruction& inst);

  
  // --------  Helper methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_ORIGINAL; }  
  bool OK();
  void PrintStatus(std::ostream& fp);


  // --------  Stack Manipulation...  --------
  inline int GetStack(int depth=0, int stack_id=-1, int in_thread=-1) const;


  // --------  Head Manipulation (including IP)  --------
  const cHeadCPU& GetHead(int head_id) const { return threads[cur_thread].heads[head_id]; }
  cHeadCPU& GetHead(int head_id) { return threads[cur_thread].heads[head_id];}
  const cHeadCPU& GetHead(int head_id, int thread) const { return threads[thread].heads[head_id]; }
  cHeadCPU& GetHead(int head_id, int thread) { return threads[thread].heads[head_id];}
  
  const cHeadCPU& IP() const { return threads[cur_thread].heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP() { return threads[cur_thread].heads[nHardware::HEAD_IP]; }
  const cHeadCPU& IP(int thread) const { return threads[thread].heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP(int thread) { return threads[thread].heads[nHardware::HEAD_IP]; }
  
  
  // --------  Label Manipulation  -------
  const cCodeLabel& GetLabel() const { return threads[cur_thread].next_label; }
  cCodeLabel& GetLabel() { return threads[cur_thread].next_label; }
  
  
  // --------  Memory Manipulation  --------
  const cCPUMemory& GetMemory() const { return memory; }
  cCPUMemory& GetMemory() { return memory; }
  const cCPUMemory& GetMemory(int value) const { return memory;}
  cCPUMemory& GetMemory(int value) { return memory; }
  
  
  // --------  Register Manipulation  --------
  const int GetRegister(int reg_id) const { return threads[cur_thread].reg[reg_id]; }
  int& GetRegister(int reg_id) { return threads[cur_thread].reg[reg_id]; }

  
  // --------  Thread Manipulation  --------
  bool ForkThread(); // Adds a new thread based off of cur_thread.
  bool KillThread(); // Kill the current thread!
  inline void PrevThread(); // Shift the current thread in use.
  inline void NextThread();
  inline void SetThread(int value);
  cInjectGenotype* GetCurThreadOwner() { return NULL; } // @DMB - cHardwareCPU does not really implement cInjectGenotype yet
  cInjectGenotype* GetThreadOwner(int in_thread) { return NULL; }
  void SetThreadOwner(cInjectGenotype* in_genotype) { return; }
  
  
  // --------  Parasite Stuff  --------
  int TestParasite() const;
  bool InjectHost(const cCodeLabel& in_label, const cGenome& injection);
  int InjectThread(const cCodeLabel& in_label, const cGenome& injection);

  
  // --------  Accessors  --------
  int GetNumThreads() const     { return threads.GetSize(); }
  int GetCurThread() const      { return cur_thread; }
  int GetCurThreadID() const    { return threads[cur_thread].GetID(); }
  int GetThreadDist() const {
    if (GetNumThreads() == 1) return 0;
    return threads[0].heads[nHardware::HEAD_IP].GetPosition() - threads[1].heads[nHardware::HEAD_IP].GetPosition();
  }


  // --------  Mutation  --------
  int PointMutate(const double mut_rate);  
  bool TriggerMutations(int trigger);
  bool TriggerMutations(int trigger, cHeadCPU& cur_head);
  
  
  // Non-Standard Methods
  
  int GetActiveStack() const { return threads[cur_thread].cur_stack; }
  bool GetMalActive() const   { return mal_active; }
  
  
  /*
  FIXME: Breakage of interface. These functions return data that is
  supposed to be private.

  I need to make complete snapshots of hardware state from Python.
  If I have the time, I'll come back and fix the breakage.

  @kgn
  */
  cCPUStack pyGetGlobalStack() { return global_stack; }
  int pyGetThreadTimeUsed() { return thread_time_used; }
  const tArray<cHardwareCPU_Thread>& pyGetThreads();
  bool pyGetAdvanceIP() { return advance_ip; }


private:
  // ---------- Instruction Library -----------

  // Flow Control
  bool Inst_If0();
  bool Inst_IfEqu();
  bool Inst_IfNot0();
  bool Inst_IfNEqu();
  bool Inst_IfGr0();
  bool Inst_IfGr();
  bool Inst_IfGrEqu0();
  bool Inst_IfGrEqu();
  bool Inst_IfLess0();
  bool Inst_IfLess();
  bool Inst_IfLsEqu0();
  bool Inst_IfLsEqu();
  bool Inst_IfBit1();
  bool Inst_IfANotEqB();
  bool Inst_IfBNotEqC();
  bool Inst_IfANotEqC();

  bool Inst_JumpF();
  bool Inst_JumpB();
  bool Inst_JumpP();
  bool Inst_JumpSelf();
  bool Inst_Call();
  bool Inst_Return();

  // Stack and Register Operations
  bool Inst_Pop();
  bool Inst_Push();
  bool Inst_HeadPop();
  bool Inst_HeadPush();

  bool Inst_PopA();
  bool Inst_PopB();
  bool Inst_PopC();
  bool Inst_PushA();
  bool Inst_PushB();
  bool Inst_PushC();

  bool Inst_SwitchStack();
  bool Inst_FlipStack();
  bool Inst_Swap();
  bool Inst_SwapAB();
  bool Inst_SwapBC();
  bool Inst_SwapAC();
  bool Inst_CopyReg();
  bool Inst_CopyRegAB();
  bool Inst_CopyRegAC();
  bool Inst_CopyRegBA();
  bool Inst_CopyRegBC();
  bool Inst_CopyRegCA();
  bool Inst_CopyRegCB();
  bool Inst_Reset();

  // Single-Argument Math
  bool Inst_ShiftR();
  bool Inst_ShiftL();
  bool Inst_Bit1();
  bool Inst_SetNum();
  bool Inst_ValGrey();
  bool Inst_ValDir();
  bool Inst_ValAddP();
  bool Inst_ValFib();
  bool Inst_ValPolyC();
  bool Inst_Inc();
  bool Inst_Dec();
  bool Inst_Zero();
  bool Inst_Not();
  bool Inst_Neg();
  bool Inst_Square();
  bool Inst_Sqrt();
  bool Inst_Log();
  bool Inst_Log10();
  bool Inst_Minus17();

  // Double Argument Math
  bool Inst_Add();
  bool Inst_Sub();
  bool Inst_Mult();
  bool Inst_Div();
  bool Inst_Mod();
  bool Inst_Nand();
  bool Inst_Nor();
  bool Inst_And();
  bool Inst_Order();
  bool Inst_Xor();

  // Biological
  bool Inst_Copy();
  bool Inst_ReadInst();
  bool Inst_WriteInst();
  bool Inst_StackReadInst();
  bool Inst_StackWriteInst();
  bool Inst_Compare();
  bool Inst_IfNCpy();
  bool Inst_Allocate();
  bool Inst_Divide();
  bool Inst_CAlloc();
  bool Inst_CDivide();
  bool Inst_MaxAlloc();
  bool Inst_Inject();
  bool Inst_InjectRand();
  bool Inst_InjectThread();
  bool Inst_Repro();
  bool Inst_Kazi();
  bool Inst_Die();

  // I/O and Sensory
  bool Inst_TaskGet();
  bool Inst_TaskStackGet();
  bool Inst_TaskStackLoad();
  bool Inst_TaskPut();
  bool Inst_TaskIO();
  bool Inst_Send();
  bool Inst_Receive();
  bool Inst_Sense();

  void DoDonate(cOrganism * to_org);
  bool Inst_DonateRandom();
  bool Inst_DonateKin();
  bool Inst_DonateEditDist();
  bool Inst_DonateNULL();

  bool Inst_SearchF();
  bool Inst_SearchB();
  bool Inst_MemSize();

  // Environment

  bool Inst_RotateL();
  bool Inst_RotateR();
  bool Inst_SetCopyMut();
  bool Inst_ModCopyMut();

  // Multi-threading...

  bool Inst_ForkThread();
  bool Inst_KillThread();
  bool Inst_ThreadID();

  // Head-based instructions...

  bool Inst_SetHead();
  bool Inst_AdvanceHead();
  bool Inst_MoveHead();
  bool Inst_JumpHead();
  bool Inst_GetHead();
  bool Inst_IfLabel();
  bool Inst_IfLabel2();
  bool Inst_HeadDivide();
  bool Inst_HeadRead();
  bool Inst_HeadWrite();
  bool Inst_HeadCopy();
  bool Inst_HeadSearch();
  bool Inst_SetFlow();

  bool Inst_HeadCopy2();
  bool Inst_HeadCopy3();
  bool Inst_HeadCopy4();
  bool Inst_HeadCopy5();
  bool Inst_HeadCopy6();
  bool Inst_HeadCopy7();
  bool Inst_HeadCopy8();
  bool Inst_HeadCopy9();
  bool Inst_HeadCopy10();

  bool Inst_HeadDivideSex();
  bool Inst_HeadDivideAsex();
  bool Inst_HeadDivideAsexWait();
  bool Inst_HeadDivideMateSelect();

  bool Inst_HeadDivide1();
  bool Inst_HeadDivide2();
  bool Inst_HeadDivide3();
  bool Inst_HeadDivide4();
  bool Inst_HeadDivide5();
  bool Inst_HeadDivide6();
  bool Inst_HeadDivide7();
  bool Inst_HeadDivide8();
  bool Inst_HeadDivide9();
  bool Inst_HeadDivide10();
  bool Inst_HeadDivide16();
  bool Inst_HeadDivide32();
  bool Inst_HeadDivide50();
  bool Inst_HeadDivide100();
  bool Inst_HeadDivide500();
  bool Inst_HeadDivide1000();
  bool Inst_HeadDivide5000();
  bool Inst_HeadDivide10000();
  bool Inst_HeadDivide50000();
  bool Inst_HeadDivide0_5();
  bool Inst_HeadDivide0_1();
  bool Inst_HeadDivide0_05();
  bool Inst_HeadDivide0_01();
  bool Inst_HeadDivide0_001();

  //// Placebo ////
  bool Inst_Skip();


  // Internal Implementation

  bool SingleProcess_PayCosts(const cInstruction & cur_inst);
  bool SingleProcess_ExecuteInst(const cInstruction & cur_inst);
  
  // --------  Stack Manipulation...  --------
  inline void StackPush(int value);
  inline int StackPop();
  inline void StackFlip();
  inline void StackClear();
  inline void SwitchStack();
  
  
  // --------  Head Manipulation (including IP)  --------
  cHeadCPU& GetActiveHead() { return threads[cur_thread].heads[threads[cur_thread].cur_head]; }
  void AdjustHeads();
  
  
  // --------  Label Manipulation  -------
  void ReadLabel(int max_size=nHardware::MAX_LABEL_SIZE);
  cHeadCPU FindLabel(int direction);
  int FindLabel_Forward(const cCodeLabel & search_label, const cGenome& search_genome, int pos);
  int FindLabel_Backward(const cCodeLabel & search_label, const cGenome& search_genome, int pos);
  cHeadCPU FindLabel(const cCodeLabel & in_label, int direction);
  cHeadCPU FindFullLabel(const cCodeLabel & in_label);
  const cCodeLabel& GetReadLabel() const { return threads[cur_thread].read_label; }
  cCodeLabel& GetReadLabel() { return threads[cur_thread].read_label; }
  
  
  bool TriggerMutations_ScopeGenome(const cMutation* cur_mut,
                                    cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate);
  bool TriggerMutations_ScopeLocal(const cMutation* cur_mut,
                                   cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate);
  int TriggerMutations_ScopeGlobal(const cMutation* cur_mut, 
                                   cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate);
  void TriggerMutations_Body(int type, cCPUMemory& target_memory, cHeadCPU& cur_head);
  
  
  // ---------- Instruction Helpers -----------
  int FindModifiedRegister(int default_register);
  int FindModifiedHead(int default_head);
  int FindComplementRegister(int base_reg);
  
  void Fault(int fault_loc, int fault_type, cString fault_desc="");
  
  bool Allocate_Necro(const int new_size);
  bool Allocate_Random(const int old_size, const int new_size);
  bool Allocate_Default(const int new_size);
  bool Allocate_Main(const int allocated_size);
  
  bool Divide_Main(const int divide_point, const int extra_lines=0, double mut_multiplier=1);
  bool Divide_CheckViable(const int child_size, const int parent_size);
  void Divide_DoMutations(double mut_multiplier=1);
  void Divide_TestFitnessMeasures();
  void Mutate(const int mut_point);
  
  void InjectCode(const cGenome& injection, const int line_num);
  void InjectCodeThread(const cGenome& injection, const int line_num);
  
  bool HeadCopy_ErrorCorrect(double reduction);
  bool Inst_HeadDivideMut(double mut_multiplier=1);
  
  void ReadInst(const int in_inst);
};


inline void cHardwareCPU::NextThread()
{
  cur_thread++;
  if (cur_thread >= GetNumThreads()) cur_thread = 0;
}

inline void cHardwareCPU::PrevThread()
{
  if (cur_thread == 0) cur_thread = GetNumThreads() - 1;
  else cur_thread--;
}

inline void cHardwareCPU::SetThread(int value)
{
     if (value>=0 && value < GetNumThreads())
          cur_thread=value;
}

inline void cHardwareCPU::StackPush(int value)
{
  if (threads[cur_thread].cur_stack == 0) {
    threads[cur_thread].stack.Push(value);
  } else {
    global_stack.Push(value);
  }
}

inline int cHardwareCPU::StackPop()
{
  int pop_value;

  if (threads[cur_thread].cur_stack == 0) {
    pop_value = threads[cur_thread].stack.Pop();
  } else {
    pop_value = global_stack.Pop();
  }

  return pop_value;
}

inline void cHardwareCPU::StackFlip()
{
  if (threads[cur_thread].cur_stack == 0) {
    threads[cur_thread].stack.Flip();
  } else {
    global_stack.Flip();
  }
}

inline int cHardwareCPU::GetStack(int depth, int stack_id, int in_thread) const
{
  // @DMB - warning: cHardwareCPU::GetStack ignores in_thread
  int value = 0;

  if (stack_id == -1) stack_id = threads[cur_thread].cur_stack;

  if (stack_id == 0) value = threads[cur_thread].stack.Get(depth);
  else if (stack_id == 1) value = global_stack.Get(depth);

  return value;
}

inline void cHardwareCPU::StackClear()
{
  if (threads[cur_thread].cur_stack == 0) {
    threads[cur_thread].stack.Clear();
  } else {
    global_stack.Clear();
  }
}

inline void cHardwareCPU::SwitchStack()
{
  threads[cur_thread].cur_stack++;
  if (threads[cur_thread].cur_stack > 1) threads[cur_thread].cur_stack = 0;
}

#endif
