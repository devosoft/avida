/*
 *  cHardwareSMT.h
 *  Avida
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cHardwareSMT_h
#define cHardwareSMT_h

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
#ifndef nHardwareSMT_h
#include "nHardwareSMT.h"
#endif
#ifndef cHardwareSMT_Thread_h
#include "cHardwareSMT_Thread.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tManagedPointerArray_h
#include "tManagedPointerArray.h"
#endif
#ifndef tInstLib_h
#include "tInstLib.h"
#endif
#ifndef tHashTable_h
#include "tHashTable.h"
#endif


class cAvidaContext;
class cInstSet;
class cInstLibBase;
class cOrganism;
class cMutation;
class cInjectGenotype;

class cCodeLabel;
class cGenome;
class cInjectGenotype;
class cInstruction;
class cInstSet;
class cOrganism;

class cHardwareSMT : public cHardwareBase
{
public:
  typedef bool (cHardwareSMT::*tMethod)(cAvidaContext& ctx);

private:
  static tInstLib<cHardwareSMT::tMethod>* s_inst_slib;
  static tInstLib<cHardwareSMT::tMethod>* initInstLib(void);

  tMethod* m_functions;

  // Stacks
  cCPUStack m_global_stacks[nHardwareSMT::NUM_GLOBAL_STACKS];
	
  // Memory
  tManagedPointerArray<cCPUMemory> m_mem_array;
  tHashTable<int, int> m_mem_lbls;

  // Threads
  tManagedPointerArray<cHardwareSMT_Thread> m_threads;
  int thread_id_chart;
  int m_cur_thread;
	
  // Instruction costs...
#ifdef INSTRUCTION_COSTS
  tArray<int> inst_cost;
  tArray<int> inst_ft_cost;
#endif

  
  bool SingleProcess_PayCosts(cAvidaContext& ctx, const cInstruction& cur_inst);
  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst);
  	

  // --------  Stack Manipulation...  --------
  inline cCPUStack& Stack(int stack_id); 
  inline const cCPUStack& Stack(int stack_id) const;
  inline cCPUStack& Stack(int stack_id, int in_thread);
  inline const cCPUStack& Stack(int stack_id, int in_thread) const;

  
  // --------  Head Manipulation (including IP)  --------
  const bool& AdvanceIP() const { return m_threads[m_cur_thread].advance_ip; }
  bool& AdvanceIP() { return m_threads[m_cur_thread].advance_ip; }
  
  
  // --------  Label Manipulation  -------
  void ReadLabel(int max_size=nHardware::MAX_LABEL_SIZE);
  cHeadMultiMem FindLabel(int direction);
  int FindLabel_Forward(const cCodeLabel& search_label, const cGenome& search_genome, int pos);
  int FindLabel_Backward(const cCodeLabel& search_label, const cGenome& search_genome, int pos);
  cHeadMultiMem FindLabel(const cCodeLabel& in_label, int direction);
  cHeadMultiMem FindFullLabel(const cCodeLabel& in_label);
  const cCodeLabel& GetReadLabel() const { return m_threads[m_cur_thread].read_label; }
  cCodeLabel& GetReadLabel() { return m_threads[m_cur_thread].read_label; }

  
  bool TriggerMutations_ScopeGenome(cAvidaContext& ctx, const cMutation* cur_mut,
																		cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate);
  bool TriggerMutations_ScopeLocal(cAvidaContext& ctx, const cMutation* cur_mut,
																	 cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate);
  int TriggerMutations_ScopeGlobal(cAvidaContext& ctx, const cMutation* cur_mut,
																	 cCPUMemory& target_memory, cHeadCPU& cur_head, const double rate);
  void TriggerMutations_Body(cAvidaContext& ctx, int type, cCPUMemory& target_memory,
														 cHeadCPU& cur_head);
	
  // ---------- Instruction Helpers -----------
  int FindModifiedStack(int default_stack);
  int FindModifiedNextStack(int default_stack);
  int FindModifiedPreviousStack(int default_stack);
  int FindModifiedComplementStack(int default_stack);
  int FindModifiedHead(int default_head);
  int FindNextStack(int default_stack);
  int FindPreviousStack(int default_stack);
  int FindComplementStack(int base_stack);
  int FindMemorySpaceLabel(int mem_space);
	
  void Fault(int fault_loc, int fault_type, cString fault_desc=""); 
  bool Allocate_Necro(const int new_size);
  bool Allocate_Random(const int old_size, const int new_size);
  bool Allocate_Default(const int new_size);
  bool Allocate_Main(const int allocated_size);
	
  bool Divide_Main(cAvidaContext& ctx, const int mem_space_used, double mut_multiplier=1);
  bool Divide_CheckViable(cAvidaContext& ctx, const int parent_size, const int child_size, const int mem_space);
  void Divide_DoMutations(cAvidaContext& ctx, double mut_multiplier = 1);
  void Inject_DoMutations(cAvidaContext& ctx, double mut_multiplier, cCPUMemory& injected_code);
  void Divide_TestFitnessMeasures(cAvidaContext& ctx);
  void Mutate(cAvidaContext& ctx, const int mut_point);

  bool InjectParasite(cAvidaContext& ctx, double mut_multiplier);

  bool HeadCopy_ErrorCorrect(double reduction);
  
  void ReadInst(const int in_inst);
	
  inline int NormalizeMemSpace(int mem_space) const;

  cHardwareSMT(const cHardwareSMT&); // @not_implemented
  cHardwareSMT& operator=(const cHardwareSMT&); // @not_implemented
  
public:
  cHardwareSMT(cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  ~cHardwareSMT() { ; }
  static cInstLibBase* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "inst_lib.4stack"; }
	
  void Reset();
  void SingleProcess(cAvidaContext& ctx);
  void ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst);
	
  // --------  Helper methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_SMT; }
  bool OK();
  void PrintStatus(std::ostream& fp);
	
	
  // --------  Stack Manipulation...  --------
  inline int GetStack(int depth=0, int stack_id=-1, int in_thread=-1) const;
	

  // --------  Head Manipulation (including IP)  --------
  const cHeadMultiMem& GetHead(int head_id) const { return m_threads[m_cur_thread].heads[head_id]; }
  cHeadMultiMem& GetHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
  const cHeadMultiMem& GetHead(int head_id, int thread) const { return m_threads[thread].heads[head_id]; }
  cHeadMultiMem& GetHead(int head_id, int thread) { return m_threads[thread].heads[head_id];}
	
  const cHeadMultiMem& IP() const { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  cHeadMultiMem& IP() { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  const cHeadMultiMem& IP(int thread) const { return m_threads[thread].heads[nHardware::HEAD_IP]; }
  cHeadMultiMem& IP(int thread) { return m_threads[thread].heads[nHardware::HEAD_IP]; }
	  
  
  // --------  Label Manipulation  -------
  const cCodeLabel& GetLabel() const { return m_threads[m_cur_thread].next_label; }
  cCodeLabel& GetLabel() { return m_threads[m_cur_thread].next_label; }
	
  
  // --------  Memory Manipulation  --------
  cCPUMemory& GetMemory() { return m_mem_array[0]; }
  const cCPUMemory& GetMemory() const { return m_mem_array[0]; }
  cCPUMemory& GetMemory(int mem_space) { return m_mem_array[NormalizeMemSpace(mem_space)]; }
  const cCPUMemory& GetMemory(int mem_space) const { return m_mem_array[NormalizeMemSpace(mem_space)]; }
  
  
  // --------  Register Manipulation  --------
  const int GetRegister(int reg_id) const { return Stack(reg_id).Peek(); }
  int& GetRegister(int reg_id) { return Stack(reg_id).Peek(); }
  
  
  // --------  Thread Manipulation  --------
  bool ForkThread(); // Adds a new thread based off of m_cur_thread.
  bool KillThread(); // Kill the current thread!
  inline void PrevThread(); // Shift the current thread in use.
  inline void NextThread();
  inline void SetThread(int value);
  cInjectGenotype* GetCurThreadOwner() { return m_threads[m_cur_thread].owner; }
  cInjectGenotype* GetThreadOwner(int in_thread) { return m_threads[in_thread].owner; }
  void SetThreadOwner(cInjectGenotype* in_genotype) { m_threads[m_cur_thread].owner = in_genotype; }
	
  
  // --------  Parasite Stuff  --------
  int TestParasite() const;
  bool InjectHost(const cCodeLabel& in_label, const cGenome & injection);
  int InjectThread(const cCodeLabel&, const cGenome&) { return -1; }
	
  
  // --------  Accessors  --------
  int GetNumThreads() const     { return m_threads.GetSize(); }
  int GetCurThread() const      { return m_cur_thread; }
  int GetCurThreadID() const    { return m_threads[m_cur_thread].GetID(); }
  int GetThreadDist() const {
    if (GetNumThreads() == 1) return 0;
    return m_threads[0].heads[nHardware::HEAD_IP].GetPosition() - m_threads[1].heads[nHardware::HEAD_IP].GetPosition();
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
  bool Inst_Val_Mod(cAvidaContext& ctx);
  bool Inst_Val_Inc(cAvidaContext& ctx);
  bool Inst_Val_Dec(cAvidaContext& ctx);
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
  bool Inst_ThreadCreate(cAvidaContext& ctx);
  bool Inst_IfLabel(cAvidaContext& ctx);
  bool Inst_ThreadExit(cAvidaContext& ctx);
  bool Inst_IO(cAvidaContext& ctx);
  bool Inst_Inject(cAvidaContext& ctx);
  bool Inst_Apoptosis(cAvidaContext& ctx);
  bool Inst_NetGet(cAvidaContext& ctx);
  bool Inst_NetSend(cAvidaContext& ctx);
  bool Inst_NetReceive(cAvidaContext& ctx);
  bool Inst_NetLast(cAvidaContext& ctx);
};


inline void cHardwareSMT::NextThread()
{
  m_cur_thread++;
  if (m_cur_thread >= GetNumThreads()) m_cur_thread = 0;
}

inline void cHardwareSMT::PrevThread()
{
  if (m_cur_thread == 0) m_cur_thread = GetNumThreads() - 1;
  else m_cur_thread--;
}

inline void cHardwareSMT::SetThread(int value)
{
  if (value>=0 && value < GetNumThreads()) m_cur_thread = value;
}

inline int cHardwareSMT::GetStack(int depth, int stack_id, int in_thread) const
{
  if(stack_id<0 || stack_id > nHardwareSMT::NUM_STACKS) stack_id=0;
  
  if(in_thread==-1)
    in_thread=m_cur_thread;
  
  return Stack(stack_id, in_thread).Get(depth);
}

inline cCPUStack& cHardwareSMT::Stack(int stack_id)
{
  if(stack_id >= nHardwareSMT::NUM_STACKS) stack_id = 0;
  if(stack_id < nHardwareSMT::NUM_LOCAL_STACKS)
    return m_threads[m_cur_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % nHardwareSMT::NUM_LOCAL_STACKS];
}

inline const cCPUStack& cHardwareSMT::Stack(int stack_id) const 
{
  if(stack_id >= nHardwareSMT::NUM_STACKS) stack_id = 0;
  if(stack_id < nHardwareSMT::NUM_LOCAL_STACKS)
    return m_threads[m_cur_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % nHardwareSMT::NUM_LOCAL_STACKS];
}

inline cCPUStack& cHardwareSMT::Stack(int stack_id, int in_thread) 
{
  if(stack_id >= nHardwareSMT::NUM_STACKS) stack_id = 0;
  if(in_thread >= m_threads.GetSize()) in_thread = m_cur_thread;
	
  if(stack_id < nHardwareSMT::NUM_LOCAL_STACKS)
    return m_threads[in_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % nHardwareSMT::NUM_LOCAL_STACKS];
}

inline const cCPUStack& cHardwareSMT::Stack(int stack_id, int in_thread) const 
{
  if(stack_id >= nHardwareSMT::NUM_STACKS) stack_id = 0;
  if(in_thread >= m_threads.GetSize()) in_thread = m_cur_thread;
	
  if(stack_id < nHardwareSMT::NUM_LOCAL_STACKS)
    return m_threads[in_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % nHardwareSMT::NUM_LOCAL_STACKS];
}

inline int cHardwareSMT::NormalizeMemSpace(int mem_space) const
{
  if(mem_space >= m_mem_array.GetSize())
    mem_space %= m_mem_array.GetSize();
  return mem_space;
}

#endif
