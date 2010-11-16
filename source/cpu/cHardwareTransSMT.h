/*
 *  cHardwareTransSMT.h
 *  Avida
 *
 *  Created by David on 7/13/06.
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

#ifndef cHardwareTransSMT_h
#define cHardwareTransSMT_h

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


class cHardwareTransSMT : public cHardwareBase
{
public:
  typedef bool (cHardwareTransSMT::*tMethod)(cAvidaContext& ctx);

protected:
  // --------  Structure Constants  --------
  static const int NUM_LOCAL_STACKS = 3;
  static const int NUM_GLOBAL_STACKS = 1;
  static const int NUM_STACKS = NUM_LOCAL_STACKS + NUM_GLOBAL_STACKS;
  static const int NUM_NOPS = 4;
  static const int MAX_MEMSPACE_LABEL = 3;
  static const int MAX_THREAD_LABEL = 3;

  enum tStacks { STACK_AX = 0, STACK_BX, STACK_CX, STACK_DX };
  
  // --------  Performance Constants --------
  static const int MEM_LBLS_HASH_FACTOR = 4; // Sets hast table size to (NUM_NOPS^MAX_MEMSPACE_LABEL) / FACTOR
  static const int THREAD_LBLS_HASH_FACTOR = 4; // Sets hast table size to (NUM_NOPS^MAX_THREAD_LABEL) / FACTOR

  // --------  Data Structures  --------
  class cLocalThread
  {
  public:
    cHeadCPU heads[nHardware::NUM_HEADS];
    unsigned char cur_head;
    cCPUStack local_stacks[NUM_LOCAL_STACKS];
    
    bool advance_ip;         // Should the IP advance after this instruction?
	bool skipExecution;
    cCodeLabel read_label;
    cCodeLabel next_label;
    bool running;
    
    // If this thread was spawned by Inject, this will point to the biounit of the parasite running the thread.
    // Otherwise, it will be NULL.
    cBioUnit* owner;
    
    cLocalThread(cHardwareBase* in_hardware = NULL) { Reset(in_hardware); }
    ~cLocalThread() { ; }
    
    void Reset(cHardwareBase* in_hardware, int mem_space = 0);
  };
  
  // --------  Static Variables  --------
  static tInstLib<cHardwareTransSMT::tMethod>* s_inst_slib;
  static tInstLib<cHardwareTransSMT::tMethod>* initInstLib(void);
    

  // --------  Member Variables  --------
  const tMethod* m_functions;

  // Stacks
  cCPUStack m_global_stacks[NUM_GLOBAL_STACKS];
	
  // Memory
  tManagedPointerArray<cCPUMemory> m_mem_array;
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

  cHardwareTransSMT(const cHardwareTransSMT&); // @not_implemented
  cHardwareTransSMT& operator=(const cHardwareTransSMT&); // @not_implemented
  
public:
  cHardwareTransSMT(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  ~cHardwareTransSMT() { ; }

  static cInstLib* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "instset-transsmt.cfg"; }
	
  bool SingleProcess(cAvidaContext& ctx, bool speculative = false);
  void ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst);
	
  // --------  Helper methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_TRANSSMT; }
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
  int GetNumHeads() const { return nHardware::NUM_HEADS; }
	
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
  bool Inst_ShiftR(cAvidaContext& ctx);         // 6
  bool Inst_ShiftL(cAvidaContext& ctx);         // 7
  bool Inst_Val_Nand(cAvidaContext& ctx);       // 8
  bool Inst_Val_Add(cAvidaContext& ctx);        // 9
  bool Inst_Val_Sub(cAvidaContext& ctx);        // 10
  bool Inst_Val_Mult(cAvidaContext& ctx);       // 11
  bool Inst_Val_Div(cAvidaContext& ctx);        // 12
  bool Inst_Val_Mod(cAvidaContext& ctx);        // 13
  bool Inst_Val_Inc(cAvidaContext& ctx);        // 14
  bool Inst_Val_Dec(cAvidaContext& ctx);        // 15
  bool Inst_SetMemory(cAvidaContext& ctx);      // 16
  bool Inst_Divide(cAvidaContext& ctx);         // 17
  bool Inst_HeadRead(cAvidaContext& ctx);       // 18
  bool Inst_HeadWrite(cAvidaContext& ctx);      // 19
  bool Inst_IfEqual(cAvidaContext& ctx);        // 20
  bool Inst_IfNotEqual(cAvidaContext& ctx);     // 21
  bool Inst_IfLess(cAvidaContext& ctx);         // 22
  bool Inst_IfGreater(cAvidaContext& ctx);      // 23
  bool Inst_HeadPush(cAvidaContext& ctx);       // 24
  bool Inst_HeadPop(cAvidaContext& ctx);        // 25
  bool Inst_HeadMove(cAvidaContext& ctx);       // 26
  bool Inst_Search(cAvidaContext& ctx);         // 27
  bool Inst_PushNext(cAvidaContext& ctx);       // 28
  bool Inst_PushPrevious(cAvidaContext& ctx);   // 29
  bool Inst_PushComplement(cAvidaContext& ctx); // 30
  bool Inst_ValDelete(cAvidaContext& ctx);      // 31
  bool Inst_ValCopy(cAvidaContext& ctx);        // 32
  bool Inst_IO(cAvidaContext& ctx);             // 33
  
  // Additional Instructions
  bool Inst_ThreadCreate(cAvidaContext& ctx);   // 34
  bool Inst_ThreadCancel(cAvidaContext& ctx);   // 35
  bool Inst_ThreadKill(cAvidaContext& ctx);     // 36
  bool Inst_Inject(cAvidaContext& ctx);         // 37
  bool Inst_Apoptosis(cAvidaContext& ctx);      // 38
  bool Inst_NetGet(cAvidaContext& ctx);         // 39
  bool Inst_NetSend(cAvidaContext& ctx);        // 40
  bool Inst_NetReceive(cAvidaContext& ctx);     // 41
  bool Inst_NetLast(cAvidaContext& ctx);        // 42
  bool Inst_RotateLeft(cAvidaContext& ctx);     // 43
  bool Inst_RotateRight(cAvidaContext& ctx);    // 44
  bool Inst_CallFlow(cAvidaContext& ctx);       // 45
  bool Inst_CallLabel(cAvidaContext& ctx);      // 46
  bool Inst_Return(cAvidaContext& ctx);         // 47
  bool Inst_IfGreaterEqual(cAvidaContext& ctx); //48
  bool Inst_Divide_Erase(cAvidaContext& ctx); //49
  
};


inline bool cHardwareTransSMT::ThreadKill(const cCodeLabel& in_label)
{
  return ThreadKill(FindThreadLabel(in_label));
}

inline bool cHardwareTransSMT::ThreadSelect(const int thread_id)
{
  if (thread_id >= 0 && thread_id < m_threads.GetSize()) {
    m_cur_thread = thread_id;
    return true;
  }
  
  return false;
}

inline bool cHardwareTransSMT::ThreadSelect(const cCodeLabel& in_label)
{
  return ThreadSelect(FindThreadLabel(in_label));
}

inline void cHardwareTransSMT::ThreadNext()
{
  m_cur_thread++;
  if (m_cur_thread >= GetNumThreads()) m_cur_thread = 0;
}

inline void cHardwareTransSMT::ThreadPrev()
{
  if (m_cur_thread == 0) m_cur_thread = GetNumThreads() - 1;
  else m_cur_thread--;
}


inline int cHardwareTransSMT::GetStack(int depth, int stack_id, int in_thread) const
{
  if (stack_id<0 || stack_id > NUM_STACKS) stack_id=0;
  
  if (in_thread==-1)
    in_thread=m_cur_thread;
  
  return Stack(stack_id, in_thread).Get(depth);
}

inline cCPUStack& cHardwareTransSMT::Stack(int stack_id)
{
  if (stack_id >= NUM_STACKS) stack_id = 0;
  if (stack_id < NUM_LOCAL_STACKS)
    return m_threads[m_cur_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % NUM_LOCAL_STACKS];
}

inline const cCPUStack& cHardwareTransSMT::Stack(int stack_id) const 
{
  if (stack_id >= NUM_STACKS) stack_id = 0;
  if (stack_id < NUM_LOCAL_STACKS)
    return m_threads[m_cur_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % NUM_LOCAL_STACKS];
}

inline cCPUStack& cHardwareTransSMT::Stack(int stack_id, int in_thread) 
{
  if (stack_id >= NUM_STACKS || stack_id < 0) stack_id = 0;
  if (in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;
	
  if (stack_id < NUM_LOCAL_STACKS)
    return m_threads[in_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % NUM_LOCAL_STACKS];
}

inline const cCPUStack& cHardwareTransSMT::Stack(int stack_id, int in_thread) const 
{
  if (stack_id >= NUM_STACKS || stack_id < 0) stack_id = 0;
  if (in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;
	
  if (stack_id < NUM_LOCAL_STACKS)
    return m_threads[in_thread].local_stacks[stack_id];
  else
    return m_global_stacks[stack_id % NUM_LOCAL_STACKS];
}

inline int cHardwareTransSMT::NormalizeMemSpace(int mem_space) const
{
  assert(mem_space >= 0);
  if (mem_space >= m_mem_array.GetSize()) mem_space %= m_mem_array.GetSize();
  return mem_space;
}

inline bool cHardwareTransSMT::MemorySpaceExists(const cCodeLabel& label)
{
  int null;
  if (label.GetSize() == 0 || m_mem_lbls.Find(label.AsInt(NUM_NOPS), null)) return true;
  return false;
}

inline int cHardwareTransSMT::FindThreadLabel(const cCodeLabel& label)
{
  int thread_id = -1;
  if (label.GetSize() == 0) return 0;
  m_thread_lbls.Find(label.AsInt(NUM_NOPS), thread_id);
  return thread_id;
}


#endif
