/*
 *  cHardwareSMT.h
 *  Avida
 *
 *  Created by David on 6/4/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cHardwareSMT_h
#define cHardwareSMT_h

#include "Avida.h"

#include "cCodeLabel.h"
#include "cCPUMemory.h"
#include "cCPUStack.h"
#include "cHeadCPU.h"
#include "cHardwareBase.h"
#include "cString.h"
#include "tHashMap.h"
#include "tInstLib.h"
#include "tManagedPointerArray.h"


class cHardwareSMT : public cHardwareBase
{
public:
  typedef bool (cHardwareSMT::*tMethod)(cAvidaContext& ctx);

protected:
  // --------  Structure Constants  --------
  static const int NUM_LOCAL_STACKS = 3;
  static const int NUM_GLOBAL_STACKS = 1;
  static const int NUM_STACKS = NUM_LOCAL_STACKS + NUM_GLOBAL_STACKS;
  static const int NUM_NOPS = 4;
  static const int MAX_MEMSPACE_LABEL = 3;
  static const int MAX_THREAD_LABEL = 3;
  static const int NUM_EXTENDED_HEADS = nHardware::NUM_HEADS + NUM_NOPS;

  enum tStacks { STACK_AX = 0, STACK_BX, STACK_CX, STACK_DX };
  
  // --------  Performance Constants --------
  static const int MEM_LBLS_HASH_FACTOR = 4; // Sets hast table size to (NUM_NOPS^MAX_MEMSPACE_LABEL) / FACTOR
  static const int THREAD_LBLS_HASH_FACTOR = 4; // Sets hast table size to (NUM_NOPS^MAX_THREAD_LABEL) / FACTOR

  // --------  Data Structures  --------
  class cLocalThread
  {
  public:
    cHeadCPU heads[NUM_EXTENDED_HEADS];
    unsigned char cur_head;
    cCPUStack local_stacks[NUM_LOCAL_STACKS];
    
    bool advance_ip;         // Should the IP advance after this instruction?
    cCodeLabel read_label;
    cCodeLabel next_label;
    bool running;
    
    // If this thread was spawned by Inject, this will point to the biounit of the parasite running the thread.
    // Otherwise, it will be NULL.
    cBioUnit* owner;
    
    cLocalThread(cHardwareBase* hw = NULL) { Reset(hw); }
    ~cLocalThread() { ; }
    
    void Reset(cHardwareBase* in_hardware, int mem_space = 0);
  };
  
  // --------  Static Variables  --------
  static tInstLib<cHardwareSMT::tMethod>* s_inst_slib;
  static tInstLib<cHardwareSMT::tMethod>* initInstLib(void);
    

  // --------  Member Variables  --------
  const tMethod* m_functions;

  // Stacks
  cCPUStack m_global_stacks[NUM_GLOBAL_STACKS];
	
  // Memory
  tManagedPointerArray<cCPUMemory> m_mem_array;
  tArray<bool> m_mem_marks;
  tHashMap<int, int> m_mem_lbls;

  // Threads
  tManagedPointerArray<cLocalThread> m_threads;
  tHashMap<int, int> m_thread_lbls;
  int m_cur_thread;  
  int m_cur_child;

  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst);
  	

  // --------  Stack Manipulation...  --------
  inline cCPUStack& Stack(int stack_id); 
  inline const cCPUStack& Stack(int stack_id) const;
  inline cCPUStack& Stack(int stack_id, int in_thread);
  inline const cCPUStack& Stack(int stack_id, int in_thread) const;

  int FindModifiedStack(int default_stack);
  int FindModifiedNextStack(int default_stack);
  int FindModifiedPreviousStack(int default_stack);
  int FindModifiedComplementStack(int default_stack);
  int FindModifiedHead(int default_head);
  int FindNextStack(int default_stack);
  int FindPreviousStack(int default_stack);
  int FindComplementStack(int base_stack);
  
  
  // --------  Head Manipulation (including IP)  --------
  const bool& AdvanceIP() const { return m_threads[m_cur_thread].advance_ip; }
  bool& AdvanceIP() { return m_threads[m_cur_thread].advance_ip; }
  
  
  // --------  Label Manipulation  -------
  const cCodeLabel& GetLabel() const { return m_threads[m_cur_thread].next_label; }
  cCodeLabel& GetLabel() { return m_threads[m_cur_thread].next_label; }
  void ReadLabel(int max_size=nHardware::MAX_LABEL_SIZE);
  cHeadCPU FindLabel(int direction);
  int FindLabel_Forward(const cCodeLabel& search_label, const cSequence& search_genome, int pos);
  int FindLabel_Backward(const cCodeLabel& search_label, const cSequence& search_genome, int pos);
  cHeadCPU FindLabel(const cCodeLabel& in_label, int direction);
  const cCodeLabel& GetReadLabel() const { return m_threads[m_cur_thread].read_label; }
  cCodeLabel& GetReadLabel() { return m_threads[m_cur_thread].read_label; }


  // ---------- Memory Manipulation -----------
  int FindMemorySpaceLabel(const cCodeLabel& label, int mem_space);
  inline bool MemorySpaceExists(const cCodeLabel& label);

  
  // ---------- Thread Manipulation -----------
  int ThreadCreate(const cCodeLabel& label, int mem_space);
  bool ThreadKill(const int thread_id);
  inline bool ThreadKill(const cCodeLabel& in_label);
  inline int FindThreadLabel(const cCodeLabel& label);
  bool ThreadIsRunning() { return m_threads[m_cur_thread].running; }
  
  	
  // ---------- Instruction Helpers -----------
  bool Allocate_Necro(const int new_size);
  bool Allocate_Random(const int old_size, const int new_size);
  bool Allocate_Default(const int new_size);
  bool Allocate_Main(const int allocated_size);


  void internalReset();
  void internalResetOnFailedDivide();
  
	int calcCopiedSize(const int parent_size, const int child_size);
  
  bool Divide_Main(cAvidaContext& ctx, double mut_multiplier = 1.0);
  void Inject_DoMutations(cAvidaContext& ctx, double mut_multiplier, cCPUMemory& injected_code);

  bool InjectParasite(cAvidaContext& ctx, double mut_multiplier);

  bool HeadCopy_ErrorCorrect(double reduction);
  
  void ReadInst(const int in_inst);
	
  inline int NormalizeMemSpace(int mem_space) const;

  cHardwareSMT(const cHardwareSMT&); // @not_implemented
  cHardwareSMT& operator=(const cHardwareSMT&); // @not_implemented
  
public:
  cHardwareSMT(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  ~cHardwareSMT() { ; }

  static cInstLib* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "instset-smt.cfg"; }
	
  bool SingleProcess(cAvidaContext& ctx, bool speculative = false);
  void ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst);
	
  // --------  Helper methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_SMT; }
  bool SupportsSpeculative() const { return false; }
  bool OK();
  void PrintStatus(std::ostream& fp);
	
	
  // --------  Stack Manipulation...  --------
  inline int GetStack(int depth=0, int stack_id=-1, int in_thread=-1) const;
  inline int GetNumStacks() const { return NUM_STACKS; }
	

  // --------  Head Manipulation (including IP)  --------
  const cHeadCPU& GetHead(int head_id) const { return m_threads[m_cur_thread].heads[head_id]; }
  cHeadCPU& GetHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
  const cHeadCPU& GetHead(int head_id, int thread) const { return m_threads[thread].heads[head_id]; }
  cHeadCPU& GetHead(int head_id, int thread) { return m_threads[thread].heads[head_id];}
  int GetNumHeads() const { return NUM_EXTENDED_HEADS; }
	
  const cHeadCPU& IP() const { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP() { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  const cHeadCPU& IP(int thread) const { return m_threads[thread].heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP(int thread) { return m_threads[thread].heads[nHardware::HEAD_IP]; }
	  
  
  // --------  Memory Manipulation  --------
  cCPUMemory& GetMemory() { return m_mem_array[0]; }
  const cCPUMemory& GetMemory() const { return m_mem_array[0]; }
  int GetMemSize() const { return m_mem_array[0].GetSize(); }
  cCPUMemory& GetMemory(int mem_space) { return m_mem_array[NormalizeMemSpace(mem_space)]; }
  const cCPUMemory& GetMemory(int mem_space) const { return m_mem_array[NormalizeMemSpace(mem_space)]; }
  int GetMemSize(int mem_space) const { return m_mem_array[NormalizeMemSpace(mem_space)].GetSize(); }
  int GetNumMemSpaces() const { return m_mem_array.GetSize(); }
  
  
  // --------  Register Manipulation  --------
  int GetRegister(int reg_id) const { return Stack(reg_id).Peek(); }
  int& GetRegister(int reg_id) { return Stack(reg_id).Peek(); }
  int GetNumRegisters() const { return NUM_STACKS; }
  
  
  // --------  Thread Manipulation  --------
  inline bool ThreadSelect(const int thread_id);
  inline bool ThreadSelect(const cCodeLabel& in_label);
  inline void ThreadPrev(); // Shift the current thread in use.
  inline void ThreadNext();
  cBioUnit* ThreadGetOwner();

  int GetNumThreads() const { return m_threads.GetSize(); }
  int GetCurThread() const { return m_cur_thread; }
  int GetCurThreadID() const { return m_cur_thread; }
  
  // interrupt current thread
  bool InterruptThread(int interruptType) { return false; }
  int GetThreadMessageTriggerType(int _index) { return -1; }
  
  // --------  Parasite Stuff  --------
  bool ParasiteInfectHost(cBioUnit* bu);
	
  
private:
  // ---------- Instruction Library -----------

  // Core Instuction Set
  bool Inst_Alt(cAvidaContext& ctx);
  bool Inst_ValShift(cAvidaContext& ctx);
  bool Inst_ValNand(cAvidaContext& ctx);
  bool Inst_ValAdd(cAvidaContext& ctx);
  bool Inst_ValSub(cAvidaContext& ctx);
  bool Inst_ValNegate(cAvidaContext& ctx);
  bool Inst_ValMult(cAvidaContext& ctx);
  bool Inst_ValDiv(cAvidaContext& ctx);
  bool Inst_ValMod(cAvidaContext& ctx);
  bool Inst_ValInc(cAvidaContext& ctx);
  bool Inst_ValDec(cAvidaContext& ctx);
  bool Inst_ValCopy(cAvidaContext& ctx);
  bool Inst_ValDelete(cAvidaContext& ctx);
  bool Inst_StackDelete(cAvidaContext& ctx);
  bool Inst_PushNext(cAvidaContext& ctx);
  bool Inst_PushPrev(cAvidaContext& ctx);
  bool Inst_PushComp(cAvidaContext& ctx);
  bool Inst_MemSet(cAvidaContext& ctx);
  bool Inst_MemMark(cAvidaContext& ctx);
  bool Inst_MemSplit(cAvidaContext& ctx);
  bool Inst_MemMerge(cAvidaContext& ctx);
  bool Inst_Divide(cAvidaContext& ctx);
  bool Inst_Inject(cAvidaContext& ctx);
  bool Inst_InstRead(cAvidaContext& ctx);
  bool Inst_InstWrite(cAvidaContext& ctx);
  bool Inst_BlockRead(cAvidaContext& ctx);
  bool Inst_BlockWrite(cAvidaContext& ctx);
  bool Inst_IfEqual(cAvidaContext& ctx);
  bool Inst_IfNotEqual(cAvidaContext& ctx);
  bool Inst_IfLess(cAvidaContext& ctx);
  bool Inst_IfGreater(cAvidaContext& ctx);
  bool Inst_HeadPush(cAvidaContext& ctx);
  bool Inst_HeadPop(cAvidaContext& ctx);
  bool Inst_HeadMove(cAvidaContext& ctx);
  bool Inst_HeadSet(cAvidaContext& ctx);
  bool Inst_Call(cAvidaContext& ctx);
  bool Inst_Return(cAvidaContext& ctx);
  bool Inst_Search(cAvidaContext& ctx);
  bool Inst_SearchMem(cAvidaContext& ctx);
  bool Inst_IO(cAvidaContext& ctx);
  bool Inst_ThreadSet(cAvidaContext& ctx);
  bool Inst_ThreadGet(cAvidaContext& ctx);
  
  bool Inst_Apoptosis(cAvidaContext& ctx);
};


inline bool cHardwareSMT::ThreadKill(const cCodeLabel& in_label)
{
  return ThreadKill(FindThreadLabel(in_label));
}

inline bool cHardwareSMT::ThreadSelect(const int thread_id)
{
  if (thread_id >= 0 && thread_id < m_threads.GetSize()) {
    m_cur_thread = thread_id;
    return true;
  }
  
  return false;
}

inline bool cHardwareSMT::ThreadSelect(const cCodeLabel& in_label)
{
  return ThreadSelect(FindThreadLabel(in_label));
}

inline void cHardwareSMT::ThreadNext()
{
  m_cur_thread++;
  if (m_cur_thread >= GetNumThreads()) m_cur_thread = 0;
}

inline void cHardwareSMT::ThreadPrev()
{
  if (m_cur_thread == 0) m_cur_thread = GetNumThreads() - 1;
  else m_cur_thread--;
}


inline int cHardwareSMT::GetStack(int depth, int stack_id, int in_thread) const
{
  if (stack_id<0 || stack_id > NUM_STACKS) stack_id=0;
  
  if (in_thread==-1)
    in_thread=m_cur_thread;
  
  return Stack(stack_id, in_thread).Get(depth);
}

inline cCPUStack& cHardwareSMT::Stack(int stack_id)
{
  if (stack_id >= NUM_STACKS) stack_id = 0;
  if (stack_id < NUM_LOCAL_STACKS)
    return m_threads[m_cur_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % NUM_LOCAL_STACKS];
}

inline const cCPUStack& cHardwareSMT::Stack(int stack_id) const 
{
  if (stack_id >= NUM_STACKS) stack_id = 0;
  if (stack_id < NUM_LOCAL_STACKS)
    return m_threads[m_cur_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % NUM_LOCAL_STACKS];
}

inline cCPUStack& cHardwareSMT::Stack(int stack_id, int in_thread) 
{
  if (stack_id >= NUM_STACKS || stack_id < 0) stack_id = 0;
  if (in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;
	
  if (stack_id < NUM_LOCAL_STACKS)
    return m_threads[in_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % NUM_LOCAL_STACKS];
}

inline const cCPUStack& cHardwareSMT::Stack(int stack_id, int in_thread) const 
{
  if (stack_id >= NUM_STACKS || stack_id < 0) stack_id = 0;
  if (in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;
	
  if (stack_id < NUM_LOCAL_STACKS)
    return m_threads[in_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % NUM_LOCAL_STACKS];
}

inline int cHardwareSMT::NormalizeMemSpace(int mem_space) const
{
  assert(mem_space >= 0);
  if (mem_space >= m_mem_array.GetSize()) mem_space %= m_mem_array.GetSize();
  return mem_space;
}

inline bool cHardwareSMT::MemorySpaceExists(const cCodeLabel& label)
{
  int null;
  if (label.GetSize() == 0 || m_mem_lbls.Find(label.AsInt(NUM_NOPS), null)) return true;
  return false;
}

inline int cHardwareSMT::FindThreadLabel(const cCodeLabel& label)
{
  int thread_id = -1;
  if (label.GetSize() == 0) return 0;
  m_thread_lbls.Find(label.AsInt(NUM_NOPS), thread_id);
  return thread_id;
}


#endif
