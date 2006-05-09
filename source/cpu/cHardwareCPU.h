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

class cInjectGenotype;
class cInstLibBase;
class cInstLibCPU;
class cInstSet;
class cMutation;
class cOrganism;

class cHardwareCPU : public cHardwareBase
{
public:
  typedef bool (cHardwareCPU::*tHardwareCPUMethod)(cAvidaContext& ctx);

protected:
  static cInstLibCPU* s_inst_slib;
  static cInstLibCPU* initInstLib(void);

  tHardwareCPUMethod* m_functions;

  cCPUMemory memory;          // Memory...
  cCPUStack global_stack;     // A stack that all threads share.
  int thread_time_used;

  tArray<cHardwareCPU_Thread> threads;
  int thread_id_chart;
  int cur_thread;

  // Flags...
  bool mal_active;         // Has an allocate occured since last divide?
  bool advance_ip;         // Should the IP advance after this instruction?
  bool executedmatchstrings;	// Have we already executed the match strings instruction?

  // Instruction costs...
#if INSTRUCTION_COSTS
  tArray<int> inst_cost;
  tArray<int> inst_ft_cost;
#endif
  
  
  bool SingleProcess_PayCosts(cAvidaContext& ctx, const cInstruction& cur_inst);
  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst);
  
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
  
  
  // --------  Thread Manipulation  -------
  bool ForkThread(); // Adds a new thread based off of cur_thread.
  bool KillThread(); // Kill the current thread!
  
  
  // ---------- Instruction Helpers -----------
  int FindModifiedRegister(int default_register);
  int FindModifiedHead(int default_head);
  int FindComplementRegister(int base_reg);
  
  bool Allocate_Necro(const int new_size);
  bool Allocate_Random(cAvidaContext& ctx, const int old_size, const int new_size);
  bool Allocate_Default(const int new_size);
  bool Allocate_Main(cAvidaContext& ctx, const int allocated_size);
  
  int GetCopiedSize(const int parent_size, const int child_size);
  
  bool Divide_Main(cAvidaContext& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1);
  
  void InjectCode(const cGenome& injection, const int line_num);
  
  bool HeadCopy_ErrorCorrect(cAvidaContext& ctx, double reduction);
  bool Inst_HeadDivideMut(cAvidaContext& ctx, double mut_multiplier = 1);
  
  void ReadInst(const int in_inst);

  
  cHardwareCPU& operator=(const cHardwareCPU&); // @not_implemented

public:
  cHardwareCPU(cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  explicit cHardwareCPU(const cHardwareCPU&);
  ~cHardwareCPU();
  static cInstLibCPU* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "inst_lib.default"; }

  void Reset();
  void SingleProcess(cAvidaContext& ctx);
  void ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst);

  
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
  bool ThreadSelect(const int thread_num);
  bool ThreadSelect(const cCodeLabel& in_label) { return false; } // Labeled threads not supported
  inline void ThreadPrev(); // Shift the current thread in use.
  inline void ThreadNext();
  cInjectGenotype* ThreadGetOwner() { return NULL; } // @DMB - cHardwareCPU does not really implement cInjectGenotype yet
  void ThreadSetOwner(cInjectGenotype* in_genotype) { return; }
  
  int GetNumThreads() const     { return threads.GetSize(); }
  int GetCurThread() const      { return cur_thread; }
  int GetCurThreadID() const    { return threads[cur_thread].GetID(); }
  
  
  // --------  Parasite Stuff  --------
  int TestParasite() const;
  bool InjectHost(const cCodeLabel& in_label, const cGenome& injection);

  
  // Non-Standard Methods
  
  int GetActiveStack() const { return threads[cur_thread].cur_stack; }
  bool GetMalActive() const   { return mal_active; }
  
private:
  // ---------- Instruction Library -----------

  // Flow Control
  bool Inst_If0(cAvidaContext& ctx);
  bool Inst_IfEqu(cAvidaContext& ctx);
  bool Inst_IfNot0(cAvidaContext& ctx);
  bool Inst_IfNEqu(cAvidaContext& ctx);
  bool Inst_IfGr0(cAvidaContext& ctx);
  bool Inst_IfGr(cAvidaContext& ctx);
  bool Inst_IfGrEqu0(cAvidaContext& ctx);
  bool Inst_IfGrEqu(cAvidaContext& ctx);
  bool Inst_IfLess0(cAvidaContext& ctx);
  bool Inst_IfLess(cAvidaContext& ctx);
  bool Inst_IfLsEqu0(cAvidaContext& ctx);
  bool Inst_IfLsEqu(cAvidaContext& ctx);
  bool Inst_IfBit1(cAvidaContext& ctx);
  bool Inst_IfANotEqB(cAvidaContext& ctx);
  bool Inst_IfBNotEqC(cAvidaContext& ctx);
  bool Inst_IfANotEqC(cAvidaContext& ctx);

  bool Inst_JumpF(cAvidaContext& ctx);
  bool Inst_JumpB(cAvidaContext& ctx);
  bool Inst_JumpP(cAvidaContext& ctx);
  bool Inst_JumpSelf(cAvidaContext& ctx);
  bool Inst_Call(cAvidaContext& ctx);
  bool Inst_Return(cAvidaContext& ctx);

  // Stack and Register Operations
  bool Inst_Pop(cAvidaContext& ctx);
  bool Inst_Push(cAvidaContext& ctx);
  bool Inst_HeadPop(cAvidaContext& ctx);
  bool Inst_HeadPush(cAvidaContext& ctx);

  bool Inst_PopA(cAvidaContext& ctx);
  bool Inst_PopB(cAvidaContext& ctx);
  bool Inst_PopC(cAvidaContext& ctx);
  bool Inst_PushA(cAvidaContext& ctx);
  bool Inst_PushB(cAvidaContext& ctx);
  bool Inst_PushC(cAvidaContext& ctx);

  bool Inst_SwitchStack(cAvidaContext& ctx);
  bool Inst_FlipStack(cAvidaContext& ctx);
  bool Inst_Swap(cAvidaContext& ctx);
  bool Inst_SwapAB(cAvidaContext& ctx);
  bool Inst_SwapBC(cAvidaContext& ctx);
  bool Inst_SwapAC(cAvidaContext& ctx);
  bool Inst_CopyReg(cAvidaContext& ctx);
  bool Inst_CopyRegAB(cAvidaContext& ctx);
  bool Inst_CopyRegAC(cAvidaContext& ctx);
  bool Inst_CopyRegBA(cAvidaContext& ctx);
  bool Inst_CopyRegBC(cAvidaContext& ctx);
  bool Inst_CopyRegCA(cAvidaContext& ctx);
  bool Inst_CopyRegCB(cAvidaContext& ctx);
  bool Inst_Reset(cAvidaContext& ctx);

  // Single-Argument Math
  bool Inst_ShiftR(cAvidaContext& ctx);
  bool Inst_ShiftL(cAvidaContext& ctx);
  bool Inst_Bit1(cAvidaContext& ctx);
  bool Inst_SetNum(cAvidaContext& ctx);
  bool Inst_ValGrey(cAvidaContext& ctx);
  bool Inst_ValDir(cAvidaContext& ctx);
  bool Inst_ValAddP(cAvidaContext& ctx);
  bool Inst_ValFib(cAvidaContext& ctx);
  bool Inst_ValPolyC(cAvidaContext& ctx);
  bool Inst_Inc(cAvidaContext& ctx);
  bool Inst_Dec(cAvidaContext& ctx);
  bool Inst_Zero(cAvidaContext& ctx);
  bool Inst_Not(cAvidaContext& ctx);
  bool Inst_Neg(cAvidaContext& ctx);
  bool Inst_Square(cAvidaContext& ctx);
  bool Inst_Sqrt(cAvidaContext& ctx);
  bool Inst_Log(cAvidaContext& ctx);
  bool Inst_Log10(cAvidaContext& ctx);
  bool Inst_Minus17(cAvidaContext& ctx);

  // Double Argument Math
  bool Inst_Add(cAvidaContext& ctx);
  bool Inst_Sub(cAvidaContext& ctx);
  bool Inst_Mult(cAvidaContext& ctx);
  bool Inst_Div(cAvidaContext& ctx);
  bool Inst_Mod(cAvidaContext& ctx);
  bool Inst_Nand(cAvidaContext& ctx);
  bool Inst_Nor(cAvidaContext& ctx);
  bool Inst_And(cAvidaContext& ctx);
  bool Inst_Order(cAvidaContext& ctx);
  bool Inst_Xor(cAvidaContext& ctx);

  // Biological
  bool Inst_Copy(cAvidaContext& ctx);
  bool Inst_ReadInst(cAvidaContext& ctx);
  bool Inst_WriteInst(cAvidaContext& ctx);
  bool Inst_StackReadInst(cAvidaContext& ctx);
  bool Inst_StackWriteInst(cAvidaContext& ctx);
  bool Inst_Compare(cAvidaContext& ctx);
  bool Inst_IfNCpy(cAvidaContext& ctx);
  bool Inst_Allocate(cAvidaContext& ctx);
  bool Inst_Divide(cAvidaContext& ctx);
  bool Inst_CAlloc(cAvidaContext& ctx);
  bool Inst_CDivide(cAvidaContext& ctx);
  bool Inst_MaxAlloc(cAvidaContext& ctx);
  bool Inst_Inject(cAvidaContext& ctx);
  bool Inst_InjectRand(cAvidaContext& ctx);
  bool Inst_InjectThread(cAvidaContext& ctx);
  bool Inst_Repro(cAvidaContext& ctx);
  bool Inst_Kazi(cAvidaContext& ctx);
  bool Inst_Die(cAvidaContext& ctx);

  // I/O and Sensory
  bool Inst_TaskGet(cAvidaContext& ctx);
  bool Inst_TaskStackGet(cAvidaContext& ctx);
  bool Inst_TaskStackLoad(cAvidaContext& ctx);
  bool Inst_TaskPut(cAvidaContext& ctx);
  bool Inst_TaskIO(cAvidaContext& ctx);
  bool Inst_MatchStrings(cAvidaContext& ctx);
  bool Inst_Send(cAvidaContext& ctx);
  bool Inst_Receive(cAvidaContext& ctx);
  bool Inst_Sense(cAvidaContext& ctx);

  void DoDonate(cOrganism * to_org);
  bool Inst_DonateRandom(cAvidaContext& ctx);
  bool Inst_DonateKin(cAvidaContext& ctx);
  bool Inst_DonateEditDist(cAvidaContext& ctx);
  bool Inst_DonateNULL(cAvidaContext& ctx);

  bool Inst_SearchF(cAvidaContext& ctx);
  bool Inst_SearchB(cAvidaContext& ctx);
  bool Inst_MemSize(cAvidaContext& ctx);

  // Environment

  bool Inst_RotateL(cAvidaContext& ctx);
  bool Inst_RotateR(cAvidaContext& ctx);
  bool Inst_SetCopyMut(cAvidaContext& ctx);
  bool Inst_ModCopyMut(cAvidaContext& ctx);

  // Multi-threading...

  bool Inst_ForkThread(cAvidaContext& ctx);
  bool Inst_KillThread(cAvidaContext& ctx);
  bool Inst_ThreadID(cAvidaContext& ctx);

  // Head-based instructions...

  bool Inst_SetHead(cAvidaContext& ctx);
  bool Inst_AdvanceHead(cAvidaContext& ctx);
  bool Inst_MoveHead(cAvidaContext& ctx);
  bool Inst_JumpHead(cAvidaContext& ctx);
  bool Inst_GetHead(cAvidaContext& ctx);
  bool Inst_IfLabel(cAvidaContext& ctx);
  bool Inst_IfLabel2(cAvidaContext& ctx);
  bool Inst_HeadDivide(cAvidaContext& ctx);
  bool Inst_HeadRead(cAvidaContext& ctx);
  bool Inst_HeadWrite(cAvidaContext& ctx);
  bool Inst_HeadCopy(cAvidaContext& ctx);
  bool Inst_HeadSearch(cAvidaContext& ctx);
  bool Inst_SetFlow(cAvidaContext& ctx);

  bool Inst_HeadCopy2(cAvidaContext& ctx);
  bool Inst_HeadCopy3(cAvidaContext& ctx);
  bool Inst_HeadCopy4(cAvidaContext& ctx);
  bool Inst_HeadCopy5(cAvidaContext& ctx);
  bool Inst_HeadCopy6(cAvidaContext& ctx);
  bool Inst_HeadCopy7(cAvidaContext& ctx);
  bool Inst_HeadCopy8(cAvidaContext& ctx);
  bool Inst_HeadCopy9(cAvidaContext& ctx);
  bool Inst_HeadCopy10(cAvidaContext& ctx);

  bool Inst_HeadDivideSex(cAvidaContext& ctx);
  bool Inst_HeadDivideAsex(cAvidaContext& ctx);
  bool Inst_HeadDivideAsexWait(cAvidaContext& ctx);
  bool Inst_HeadDivideMateSelect(cAvidaContext& ctx);

  bool Inst_HeadDivide1(cAvidaContext& ctx);
  bool Inst_HeadDivide2(cAvidaContext& ctx);
  bool Inst_HeadDivide3(cAvidaContext& ctx);
  bool Inst_HeadDivide4(cAvidaContext& ctx);
  bool Inst_HeadDivide5(cAvidaContext& ctx);
  bool Inst_HeadDivide6(cAvidaContext& ctx);
  bool Inst_HeadDivide7(cAvidaContext& ctx);
  bool Inst_HeadDivide8(cAvidaContext& ctx);
  bool Inst_HeadDivide9(cAvidaContext& ctx);
  bool Inst_HeadDivide10(cAvidaContext& ctx);
  bool Inst_HeadDivide16(cAvidaContext& ctx);
  bool Inst_HeadDivide32(cAvidaContext& ctx);
  bool Inst_HeadDivide50(cAvidaContext& ctx);
  bool Inst_HeadDivide100(cAvidaContext& ctx);
  bool Inst_HeadDivide500(cAvidaContext& ctx);
  bool Inst_HeadDivide1000(cAvidaContext& ctx);
  bool Inst_HeadDivide5000(cAvidaContext& ctx);
  bool Inst_HeadDivide10000(cAvidaContext& ctx);
  bool Inst_HeadDivide50000(cAvidaContext& ctx);
  bool Inst_HeadDivide0_5(cAvidaContext& ctx);
  bool Inst_HeadDivide0_1(cAvidaContext& ctx);
  bool Inst_HeadDivide0_05(cAvidaContext& ctx);
  bool Inst_HeadDivide0_01(cAvidaContext& ctx);
  bool Inst_HeadDivide0_001(cAvidaContext& ctx);

  //// Placebo ////
  bool Inst_Skip(cAvidaContext& ctx);
};


#ifdef ENABLE_UNIT_TESTS
namespace nHardwareCPU {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  


inline bool cHardwareCPU::ThreadSelect(const int thread_num)
{
  if (thread_num >= 0 && thread_num < threads.GetSize()) {
    cur_thread = thread_num;
    return true;
  }
  
  return false;
}

inline void cHardwareCPU::ThreadNext()
{
  cur_thread++;
  if (cur_thread >= GetNumThreads()) cur_thread = 0;
}

inline void cHardwareCPU::ThreadPrev()
{
  if (cur_thread == 0) cur_thread = GetNumThreads() - 1;
  else cur_thread--;
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
