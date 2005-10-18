/*
 *  cHardwareSMT.h
 *  Avida2
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef HARDWARE_SMT_H
#define HARDWARE_SMT_H

#include <iomanip>

#ifndef CPU_MEMORY_HH
#include "cCPUMemory.h"
#endif
#ifndef CPU_STACK_HH
#include "cCPUStack.h"
#endif
#ifndef DEFS_HH
#include "defs.h"
#endif
#ifndef HEAD_MULTI_MEM_HH
#include "cHeadMultiMem.h"
#endif
#ifndef HARDWARE_BASE_HH
#include "cHardwareBase.h"
#endif
#ifndef HARDWARE_SMT_CONSTANTS_H
#include "nHardwareSMT.h"
#endif
#ifndef HARDWARE_SMT_THREAD_H
#include "cHardwareSMT_Thread.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef TARRAY_HH
#include "tArray.h"
#endif
#ifndef TINSTLIB_H
#include "tInstLib.h"
#endif
#ifndef THASH_TABLE_HH
#include "tHashTable.h"
#endif

/**
* Each organism may have a cHardwareSMT structure which keeps track of the
 * current status of all the components of the simulated hardware.
 *
 * @see cHardwareSMT_Thread, cCPUStack, cCPUMemory, cInstSet
 **/

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

class cHardwareSMT : public cHardwareBase {
public:
  typedef bool (cHardwareSMT::*tMethod)();
private:
  static tInstLib<cHardwareSMT::tMethod>* s_inst_slib;
  static tInstLib<cHardwareSMT::tMethod>* initInstLib(void);
  tMethod* m_functions;
private:
  // Stacks
  cCPUStack m_global_stacks[nHardwareSMT::NUM_GLOBAL_STACKS];
	
  // Memory
  tArray<cCPUMemory> m_mem_array;
  tHashTable<int, int> m_mem_lbls;

  // Threads
  tArray<cHardwareSMT_Thread> m_threads;
  int thread_id_chart;
  int m_cur_thread;
	
  // Instruction costs...
#ifdef INSTRUCTION_COSTS
  tArray<int> inst_cost;
  tArray<int> inst_ft_cost;
#endif

  cHardwareSMT(const cHardwareSMT &); // disabled...  can't (easily) copy m_mem_lbls @dmb

public:
  cHardwareSMT(cOrganism * in_organism, cInstSet * in_inst_set);
  ~cHardwareSMT() { ; }
  void Recycle(cOrganism* new_organism, cInstSet * in_inst_set);
  static cInstLibBase* GetInstLib();
  static cString GetDefaultInstFilename() { return "inst_lib.4stack"; }
  static void WriteDefaultInstSet() { ; }
	
  void Reset();
  void SingleProcess();
  bool SingleProcess_PayCosts(const cInstruction & cur_inst);
  bool SingleProcess_ExecuteInst(const cInstruction & cur_inst);
  void ProcessBonusInst(const cInstruction & inst);
  void LoadGenome(const cGenome& new_genome) { m_mem_array[0] = new_genome; }
	
  // --------  Helper methods  --------
  bool OK();
  void PrintStatus(std::ostream & fp);
	
	
  // --------  Stack Manipulation...  --------
  inline int GetStack(int depth=0, int stack_id=-1, int in_thread=-1) const;
  cString GetActiveStackID(int stackID) const;
  
  // retrieves appropriate stack
  inline cCPUStack & Stack(int stack_id); 
  inline const cCPUStack & Stack(int stack_id) const;
  inline cCPUStack & Stack(int stack_id, int in_thread);
  inline const cCPUStack & Stack(int stack_id, int in_thread) const;
	
  // --------  Head Manipulation (including IP)  --------
  inline void SetActiveHead(const int new_head)
  { m_threads[m_cur_thread].cur_head = (UCHAR) new_head; }
	
  int GetCurHead() const { return m_threads[m_cur_thread].cur_head; }
  
  const cHeadMultiMem & GetHead(int head_id) const
  { return m_threads[m_cur_thread].heads[head_id]; }
  cHeadMultiMem & GetHead(int head_id) 
  { return m_threads[m_cur_thread].heads[head_id];}
  
  const cHeadMultiMem & GetHead(int head_id, int thread) const
  { return m_threads[thread].heads[head_id]; }
  cHeadMultiMem & GetHead(int head_id, int thread) 
  { return m_threads[thread].heads[head_id];}
	
  const cHeadMultiMem & GetActiveHead() const { return GetHead(GetCurHead()); }
  cHeadMultiMem & GetActiveHead() { return GetHead(GetCurHead()); }
	
  void AdjustHeads();
	
  const cHeadMultiMem & IP() const { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  cHeadMultiMem & IP() { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
	
  const cHeadMultiMem & IP(int thread) const { return m_threads[thread].heads[nHardware::HEAD_IP]; }
  cHeadMultiMem & IP(int thread) { return m_threads[thread].heads[nHardware::HEAD_IP]; }
	
	
  const bool & AdvanceIP() const { return m_threads[m_cur_thread].advance_ip; }
  bool & AdvanceIP() { return m_threads[m_cur_thread].advance_ip; }
	
  // --------  Label Manipulation  -------
  void ReadLabel(int max_size=nHardware::MAX_LABEL_SIZE);
  const cCodeLabel & GetLabel() const { return m_threads[m_cur_thread].next_label; }
  cCodeLabel & GetLabel() { return m_threads[m_cur_thread].next_label; }
  const cCodeLabel & GetReadLabel() const { return m_threads[m_cur_thread].read_label; }
  cCodeLabel & GetReadLabel() { return m_threads[m_cur_thread].read_label; }
	
  // --------  Memory Manipulation  --------}
  cCPUMemory& GetMemory() { return m_mem_array[0]; }
  const cCPUMemory& GetMemory() const { return m_mem_array[0]; }
  cCPUMemory& GetMemory(int mem_space)
  {
    if(mem_space >= m_mem_array.GetSize())
      mem_space %= m_mem_array.GetSize();
    return m_mem_array[mem_space];
  }
  const cCPUMemory& GetMemory(int mem_space) const
  {
    if(mem_space >= m_mem_array.GetSize())
      mem_space %= m_mem_array.GetSize();
    return m_mem_array[mem_space];
  }
  
  // --------  Thread Manipulation  --------
  bool ForkThread(); // Adds a new thread based off of m_cur_thread.
  bool KillThread(); // Kill the current thread!
  inline void PrevThread(); // Shift the current thread in use.
  inline void NextThread();
  inline void SetThread(int value);
  inline cInjectGenotype * GetCurThreadOwner(); 
  inline cInjectGenotype * GetThreadOwner(int in_thread);
  inline void SetThreadOwner(cInjectGenotype * in_genotype);
	
  // --------  Tests  --------
	
  int TestParasite() const;
	
  // --------  Accessors  --------
  int GetNumThreads() const     { return m_threads.GetSize(); }
  int GetCurThread() const      { return m_cur_thread; }
  int GetCurThreadID() const    { return m_threads[m_cur_thread].GetID(); }
	
  int GetThreadDist() const {
    if (GetNumThreads() == 1) return 0;
    return m_threads[0].heads[nHardware::HEAD_IP].GetPosition() - m_threads[1].heads[nHardware::HEAD_IP].GetPosition();
  }
	
  // Complex label manipulation...
  cHeadMultiMem FindLabel(int direction);
  int FindLabel_Forward(const cCodeLabel & search_label,
												const cGenome & search_genome, int pos);
  int FindLabel_Backward(const cCodeLabel & search_label,
												 const cGenome & search_genome, int pos);
  cHeadMultiMem FindLabel(const cCodeLabel & in_label, int direction);
  cHeadMultiMem FindFullLabel(const cCodeLabel & in_label);
	
  int GetType() const { return HARDWARE_TYPE_CPU_SMT; }
  bool InjectParasite(double mut_multiplier);
  bool InjectHost(const cCodeLabel & in_label, const cGenome & injection);
  int InjectThread(const cCodeLabel &, const cGenome &) { return -1; }
  void Mutate(const int mut_point);
  int PointMutate(const double mut_rate);
  int FindFirstEmpty();
  bool isEmpty(int mem_space_used);
	
  bool TriggerMutations(int trigger);
  bool TriggerMutations(int trigger, cHeadMultiMem & cur_head);
  bool TriggerMutations_ScopeGenome(const cMutation * cur_mut,
																		cCPUMemory & target_memory, cHeadMultiMem & cur_head, const double rate);
  bool TriggerMutations_ScopeLocal(const cMutation * cur_mut,
																	 cCPUMemory & target_memory, cHeadMultiMem & cur_head, const double rate);
  int TriggerMutations_ScopeGlobal(const cMutation * cur_mut,
																	 cCPUMemory & target_memory, cHeadMultiMem & cur_head, const double rate);
  void TriggerMutations_Body(int type, cCPUMemory & target_memory,
														 cHeadMultiMem & cur_head);
	
  void ReadInst(const int in_inst);
	
  void SaveState(std::ostream & fp);
  void LoadState(std::istream & fp);
		
private:
  /////////---------- Instruction Helpers ------------//////////
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
	
  bool Divide_Main(const int mem_space_used, double mut_multiplier=1);
  bool Divide_CheckViable(const int parent_size, const int child_size, const int mem_space);
  void Divide_DoMutations(double mut_multiplier=1);
  void Inject_DoMutations(double mut_multiplier, cCPUMemory & injected_code);
  void Divide_TestFitnessMeasures();
	
  bool HeadCopy_ErrorCorrect(double reduction);
	
public:
  /////////---------- Instruction Library ------------//////////
  bool Inst_ShiftR();
  bool Inst_ShiftL();
  bool Inst_Val_Nand();
  bool Inst_Val_Add();
  bool Inst_Val_Sub();
  bool Inst_Val_Mult();
  bool Inst_Val_Div();
  bool Inst_Val_Mod();
  bool Inst_Val_Inc();
  bool Inst_Val_Dec();
  bool Inst_SetMemory();
  bool Inst_Divide();
  bool Inst_HeadRead();
  bool Inst_HeadWrite();
  bool Inst_HeadCopy();
  bool Inst_IfEqual();
  bool Inst_IfNotEqual();
  bool Inst_IfLess();
  bool Inst_IfGreater();
  bool Inst_HeadPush();
  bool Inst_HeadPop();
  bool Inst_HeadMove();
  bool Inst_Search();
  bool Inst_PushNext();
  bool Inst_PushPrevious();
  bool Inst_PushComplement();
  bool Inst_ValDelete();
  bool Inst_ValCopy();
  bool Inst_ForkThread();
  bool Inst_IfLabel();
  bool Inst_KillThread();
  bool Inst_IO();
  bool Inst_Inject();
};


//////////////////
//  cHardwareSMT
//////////////////

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

inline cInjectGenotype * cHardwareSMT::GetCurThreadOwner() 
{ 
  return m_threads[m_cur_thread].owner; 
}

inline cInjectGenotype * cHardwareSMT::GetThreadOwner(int thread) 
{ 
  return m_threads[thread].owner; 
}

inline void cHardwareSMT::SetThreadOwner(cInjectGenotype * in_genotype)
{ 
  m_threads[m_cur_thread].owner = in_genotype; 
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

#endif
