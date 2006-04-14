/*
 *  cHardware4Stack.h
 *  Avida
 *
 *  Created by David on 11/17/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cHardware4Stack_h
#define cHardware4Stack_h

#include <iomanip>

#ifndef cCPUMemory_h
#include "cCPUMemory.h"
#endif
#ifndef cCPUStack_h
#include "cCPUStack.h"
#endif
#ifndef defs_h
#include "defs.h"
#endif
#ifndef cHeadMultiMem_h
#include "cHeadMultiMem.h"
#endif
#ifndef cHardwareBase_h
#include "cHardwareBase.h"
#endif
#ifndef nHardware4Stack_h
#include "nHardware4Stack.h"
#endif
#ifndef cHardware4Stack_Thread_h
#include "cHardware4Stack_Thread.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

class cAvidaContext;
class cOrganism;
class cMutation;
class cInjectGenotype;

/**
 * Each organism may have a cHardware4Stack structure which keeps track of the
 * current status of all the components of the simulated hardware.
 *
 * @see cHardware4Stack_Thread, cCPUStack, cCPUMemory, cInstSet
 **/

class cCodeLabel;
class cCPUMemory;
class cGenome;
class cInjectGenotype;
class cInstLib4Stack;
class cInstLibBase;
class cInstruction;
class cInstSet;
class cOrganism;

class cHardware4Stack : public cHardwareBase
{
public:
  typedef bool (cHardware4Stack::*tHardware4StackMethod)(cAvidaContext& ctx);
private:
  static cInstLib4Stack* s_inst_slib;
  static cInstLib4Stack* initInstLib(void);
  
  tHardware4StackMethod* m_functions;

  tArray<cCPUMemory> memory_array;          // Memory...
  //cCPUStack global_stack;     // A stack that all threads share.
  cCPUStack global_stacks[nHardware4Stack::NUM_GLOBAL_STACKS];
  //int thread_time_used;

  tArray<cHardware4Stack_Thread> threads;
  int thread_id_chart;
  int cur_thread;

  // Instruction costs...
#ifdef INSTRUCTION_COSTS
  tArray<int> inst_cost;
  tArray<int> inst_ft_cost;
#endif

  // Thread slicing...

    // Keeps track of the base thread slicing number for each possible number of threads
  float slice_array[10]; //***HACK!  How do I do this right? -law

  // Keeps track of fractional instructions that carry over into next update
  float inst_remainder; 

  
  bool SingleProcess_PayCosts(cAvidaContext& ctx, const cInstruction & cur_inst);
  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction & cur_inst);
  
  
  // --------  Stack Manipulation...  --------
  inline cCPUStack& Stack(int stack_id); 
  inline const cCPUStack& Stack(int stack_id) const;
  inline cCPUStack& Stack(int stack_id, int in_thread);
  inline const cCPUStack& Stack(int stack_id, int in_thread) const;
  
  
  // --------  Head Manipulation (including IP)  --------
  const bool& AdvanceIP() const { return threads[cur_thread].advance_ip; }
  bool& AdvanceIP() { return threads[cur_thread].advance_ip; }
  
  
  // --------  Label Manipulation  -------
  void ReadLabel(int max_size=nHardware::MAX_LABEL_SIZE);
  cHeadMultiMem FindLabel(int direction);
  int FindLabel_Forward(const cCodeLabel& search_label, const cGenome& search_genome, int pos);
  int FindLabel_Backward(const cCodeLabel& search_label, const cGenome& search_genome, int pos);
  cHeadMultiMem FindLabel(const cCodeLabel& in_label, int direction);
  cHeadMultiMem FindFullLabel(const cCodeLabel& in_label);
  const cCodeLabel & GetReadLabel() const { return threads[cur_thread].read_label; }
  cCodeLabel & GetReadLabel() { return threads[cur_thread].read_label; }
  

  bool TriggerMutations_ScopeGenome(cAvidaContext& ctx, const cMutation* cur_mut, cCPUMemory& target_memory,
                                    cHeadCPU& cur_head, const double rate);
  bool TriggerMutations_ScopeLocal(cAvidaContext& ctx, const cMutation* cur_mut, cCPUMemory& target_memory,
                                   cHeadCPU& cur_head, const double rate);
  int TriggerMutations_ScopeGlobal(cAvidaContext& ctx, const cMutation* cur_mut, cCPUMemory& target_memory,
                                   cHeadCPU& cur_head, const double rate);
  void TriggerMutations_Body(cAvidaContext& ctx, int type, cCPUMemory & target_memory, cHeadCPU& cur_head);

  // ---------- Instruction Helpers -----------
  int FindModifiedStack(int default_stack);
  int FindModifiedHead(int default_head);
  int FindComplementStack(int base_stack);
  
  void Fault(int fault_loc, int fault_type, cString fault_desc=""); 
  bool Allocate_Necro(const int new_size);
  bool Allocate_Random(const int old_size, const int new_size);
  bool Allocate_Default(const int new_size);
  bool Allocate_Main(const int allocated_size);
  
  bool Divide_Main(cAvidaContext& ctx, const int mem_space_used, double mut_multiplier=1);
  bool Divide_CheckViable(cAvidaContext& ctx, const int parent_size, const int child_size, const int mem_space);
  void Divide_DoMutations(cAvidaContext& ctx, double mut_multiplier = 1);
  void Inject_DoMutations(cAvidaContext& ctx, double mut_multiplier, cCPUMemory & injected_code);
  void Divide_TestFitnessMeasures(cAvidaContext& ctx);
  void Mutate(cAvidaContext& ctx, const int mut_point);
  
  bool HeadCopy_ErrorCorrect(double reduction);

  bool InjectParasite(cAvidaContext& ctx, double mut_multiplier);

  void ReadInst(const int in_inst);
  
  cString ConvertToInstruction(int mem_space_used);
  
  int FindFirstEmpty();
  bool isEmpty(int mem_space_used);
  inline int NormalizeMemSpace(int mem_space) const;
  
  cHardware4Stack& operator=(const cHardware4Stack&); // @not_implemented
  
public:
  cHardware4Stack(cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  explicit cHardware4Stack(const cHardware4Stack&);
  ~cHardware4Stack() { ; }
  static cInstLibBase* GetInstLib();
  static cString GetDefaultInstFilename() { return "inst_lib.4stack"; }

  void Reset();
  void SingleProcess(cAvidaContext& ctx);
  void ProcessBonusInst(cAvidaContext& ctx, const cInstruction & inst);

  // --------  Helper methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_4STACK; }
  bool OK();
  void PrintStatus(std::ostream& fp);


  // --------  Stack Manipulation...  --------
  inline int GetStack(int depth=0, int stack_id=-1, int in_thread=-1) const;

  
  // --------  Head Manipulation (including IP)  --------
  const cHeadMultiMem& GetHead(int head_id) const { return threads[cur_thread].heads[head_id]; }
  cHeadMultiMem& GetHead(int head_id) { return threads[cur_thread].heads[head_id];}
  const cHeadMultiMem& GetHead(int head_id, int thread) const { return threads[thread].heads[head_id]; }
  cHeadMultiMem& GetHead(int head_id, int thread) { return threads[thread].heads[head_id];}

  const cHeadMultiMem& IP() const { return threads[cur_thread].heads[nHardware::HEAD_IP]; }
  cHeadMultiMem& IP() { return threads[cur_thread].heads[nHardware::HEAD_IP]; }
  const cHeadMultiMem& IP(int thread) const { return threads[thread].heads[nHardware::HEAD_IP]; }
  cHeadMultiMem& IP(int thread) { return threads[thread].heads[nHardware::HEAD_IP]; }

  
  // --------  Label Manipulation  -------
  const cCodeLabel & GetLabel() const { return threads[cur_thread].next_label; }
  cCodeLabel & GetLabel() { return threads[cur_thread].next_label; }


  // --------  Memory Manipulation  --------
  cCPUMemory& GetMemory() { return memory_array[0]; }
  const cCPUMemory& GetMemory() const { return memory_array[0]; }
  cCPUMemory& GetMemory(int mem_space) { return memory_array[NormalizeMemSpace(mem_space)]; }
  const cCPUMemory& GetMemory(int mem_space) const { return memory_array[NormalizeMemSpace(mem_space)]; }
  
  
  // --------  Register Manipulation  --------
  const int GetRegister(int reg_id) const { return Stack(reg_id).Peek(); }
  int& GetRegister(int reg_id) { return Stack(reg_id).Peek(); }

  
  // --------  Thread Manipulation  --------
  bool ForkThread(); // Adds a new thread based off of cur_thread.
  bool KillThread(); // Kill the current thread!
  inline void PrevThread(); // Shift the current thread in use.
  inline void NextThread();
  inline void SetThread(int value);
  cInjectGenotype* GetCurThreadOwner() { return threads[cur_thread].owner; }
  cInjectGenotype* GetThreadOwner(int in_thread) { return threads[in_thread].owner; }
  void SetThreadOwner(cInjectGenotype* in_genotype) { threads[cur_thread].owner = in_genotype; }
	
  
  // --------  Parasite Stuff  --------
  int TestParasite() const;
  bool InjectHost(const cCodeLabel& in_label, const cGenome& injection);
  bool InjectThread(const cCodeLabel& in_label) { return false; }

  
  // --------  Accessors  --------
  int GetNumThreads() const     { return threads.GetSize(); }
  int GetCurThread() const      { return cur_thread; }
  int GetCurThreadID() const    { return threads[cur_thread].GetID(); }
  int GetThreadDist() const {
    if (GetNumThreads() == 1) return 0;
    return threads[0].heads[nHardware::HEAD_IP].GetPosition() - threads[1].heads[nHardware::HEAD_IP].GetPosition();
  }
	
  
  // --------  Mutation  --------
  int PointMutate(cAvidaContext& ctx, const double mut_rate);  
  bool TriggerMutations(cAvidaContext& ctx, int trigger);
  bool TriggerMutations(cAvidaContext& ctx, int trigger, cHeadCPU& cur_head);
  

private:
  // ---------- Instruction Library -----------
  bool Inst_ShiftR(cAvidaContext& ctx);
  bool Inst_ShiftL(cAvidaContext& ctx);
  bool Inst_Val_Nand(cAvidaContext& ctx);
  bool Inst_Val_Add(cAvidaContext& ctx);
  bool Inst_Val_Sub(cAvidaContext& ctx);
  bool Inst_Val_Mult(cAvidaContext& ctx);
  bool Inst_Val_Div(cAvidaContext& ctx);
  bool Inst_SetMemory(cAvidaContext& ctx);
  bool Inst_Divide(cAvidaContext& ctx);
  bool Inst_HeadRead(cAvidaContext& ctx);
  bool Inst_HeadWrite(cAvidaContext& ctx);
  bool Inst_HeadCopy(cAvidaContext& ctx);
  bool Inst_IfEqual(cAvidaContext& ctx);
  bool Inst_IfNotEqual(cAvidaContext& ctx);
  bool Inst_IfLess(cAvidaContext& ctx);
  bool Inst_IfGreater(cAvidaContext& ctx);
  bool Inst_HeadPush(cAvidaContext& ctx);
  bool Inst_HeadPop(cAvidaContext& ctx);
  bool Inst_HeadMove(cAvidaContext& ctx);
  bool Inst_Search(cAvidaContext& ctx);
  bool Inst_PushNext(cAvidaContext& ctx);
  bool Inst_PushPrevious(cAvidaContext& ctx);
  bool Inst_PushComplement(cAvidaContext& ctx);
  bool Inst_ValDelete(cAvidaContext& ctx);
  bool Inst_ValCopy(cAvidaContext& ctx);
  bool Inst_ForkThread(cAvidaContext& ctx);
  bool Inst_IfLabel(cAvidaContext& ctx);
  bool Inst_Increment(cAvidaContext& ctx);
  bool Inst_Decrement(cAvidaContext& ctx);
  bool Inst_Mod(cAvidaContext& ctx);
  bool Inst_KillThread(cAvidaContext& ctx);
  bool Inst_IO(cAvidaContext& ctx);
  bool Inst_Inject(cAvidaContext& ctx);  
};


inline void cHardware4Stack::NextThread()
{
  cur_thread++;
  if (cur_thread >= GetNumThreads()) cur_thread = 0;
}

inline void cHardware4Stack::PrevThread()
{
  if (cur_thread == 0) cur_thread = GetNumThreads() - 1;
  else cur_thread--;
}

inline void cHardware4Stack::SetThread(int value)
{
  if (value>=0 && value < GetNumThreads())
    cur_thread=value;
}

inline int cHardware4Stack::GetStack(int depth, int stack_id, int in_thread) const
{
  if(stack_id<0 || stack_id>nHardware4Stack::NUM_STACKS) stack_id=0;
  
  if(in_thread==-1)
    in_thread=cur_thread;
  
  return Stack(stack_id, in_thread).Get(depth);
}

inline cCPUStack& cHardware4Stack::Stack(int stack_id)
{
  if(stack_id >= nHardware4Stack::NUM_STACKS)
    {
      stack_id=0;
    }
  if(stack_id < nHardware4Stack::NUM_LOCAL_STACKS)
    return threads[cur_thread].local_stacks[stack_id];
  else
    return global_stacks[stack_id % nHardware4Stack::NUM_LOCAL_STACKS];
}

inline const cCPUStack& cHardware4Stack::Stack(int stack_id) const 
{
  if(stack_id >= nHardware4Stack::NUM_STACKS)
    {
      stack_id=0;
    }
  if(stack_id < nHardware4Stack::NUM_LOCAL_STACKS)
    return threads[cur_thread].local_stacks[stack_id];
  else
    return global_stacks[stack_id % nHardware4Stack::NUM_LOCAL_STACKS];
}

inline cCPUStack& cHardware4Stack::Stack(int stack_id, int in_thread) 
{
  if(stack_id >= nHardware4Stack::NUM_STACKS)
      stack_id=0;
  if(in_thread >= threads.GetSize())
      in_thread=cur_thread;

  if(stack_id < nHardware4Stack::NUM_LOCAL_STACKS)
    return threads[in_thread].local_stacks[stack_id];
  else
    return global_stacks[stack_id % nHardware4Stack::NUM_LOCAL_STACKS];
}

inline const cCPUStack& cHardware4Stack::Stack(int stack_id, int in_thread) const 
{
  if(stack_id >= nHardware4Stack::NUM_STACKS)
      stack_id=0;
  if(in_thread >= threads.GetSize())
      in_thread=cur_thread;

  if(stack_id < nHardware4Stack::NUM_LOCAL_STACKS)
    return threads[in_thread].local_stacks[stack_id];
  else
    return global_stacks[stack_id % nHardware4Stack::NUM_LOCAL_STACKS];
}

inline int cHardware4Stack::NormalizeMemSpace(int mem_space) const
{
  if(mem_space >= nHardware4Stack::NUM_MEMORY_SPACES)
    mem_space %= nHardware4Stack::NUM_MEMORY_SPACES;
  return mem_space;
}

#endif
