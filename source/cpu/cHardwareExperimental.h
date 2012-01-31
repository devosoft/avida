/*
 *  cHardwareExperimental.h
 *  Avida
 *
 *  Created by David on 2/10/07 based on cHardwareCPU.h
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

#ifndef cHardwareExperimental_h
#define cHardwareExperimental_h

#include "avida/Avida.h"

#include "cCodeLabel.h"
#include "nHardware.h"
#include "cHeadCPU.h"
#include "cCPUMemory.h"
#include "cHardwareBase.h"
#include "cStats.h"
#include "cString.h"
#include "tArray.h"
#include "tInstLib.h"
#include "tManagedPointerArray.h"
#include "cEnvReqs.h"
#include "cEnvironment.h"
#include "cCoords.h"

#include <cstring>
#include <iomanip>

/**
 * Each organism may have a cHardwareExperimental structure which keeps track of the
 * current status of all the components of the simulated hardware.
 *
 * @see cCPUMemory, cInstSet
 **/

class cInstLib;
class cInstSet;
class cMutation;
class cOrganism;

class cHardwareExperimental : public cHardwareBase
{
public:
  typedef bool (cHardwareExperimental::*tMethod)(cAvidaContext& ctx);

private:
  // --------  Structure Constants  --------
  static const int NUM_REGISTERS = 8;
  static const int NUM_HEADS = nHardware::NUM_HEADS >= NUM_REGISTERS ? nHardware::NUM_HEADS : NUM_REGISTERS;
  enum tRegisters { rAX = 0, rBX, rCX, rDX, rEX, rFX, rGX, rHX, rIX, rJX, rKX, rLX, rMX, rNX, rOX, rPX};
  static const int NUM_NOPS = NUM_REGISTERS;
  
  
  // --------  Static Variables  --------
  static tInstLib<cHardwareExperimental::tMethod>* s_inst_slib;
  static tInstLib<cHardwareExperimental::tMethod>* initInstLib(void);
  
  
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
  
  
  struct cLocalThread
  {
  private:
    int m_id;
    int m_promoter_inst_executed;
    unsigned int m_execurate;
    
  public:
    sInternalValue reg[NUM_REGISTERS];
    cHeadCPU heads[NUM_HEADS];
    cLocalStack stack;
    unsigned char cur_stack;              // 0 = local stack, 1 = global stack.
    unsigned char cur_head;
    
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
    
    inline cLocalThread() { ; }
    cLocalThread(cHardwareExperimental* in_hardware, int in_id = -1) { Reset(in_hardware, in_id); }
    ~cLocalThread() { ; }
    
    void Reset(cHardwareExperimental* in_hardware, int in_id);
    inline int GetID() const { return m_id; }
    inline void SetID(int in_id) { m_id = in_id; }
    
    inline unsigned int GetExecurate() const { return m_execurate; }
    inline void UpdateExecurate(int code_len, unsigned int inst_code) { m_execurate <<= code_len; m_execurate |= inst_code; }      
    
    inline int GetPromoterInstExecuted() const { return m_promoter_inst_executed; }
    inline void IncPromoterInstExecuted() { m_promoter_inst_executed++; }
    inline void ResetPromoterInstExecuted() { m_promoter_inst_executed = 0; }
  };
  
  
  struct cPromoter 
  {
  public:
    int pos;        // position within genome
    int bit_code;   // bit code of promoter
    int regulation;
    
    inline cPromoter(int p = 0, int bc = 0, int reg = 0) : pos(p), bit_code(bc), regulation(reg) { ; }
    inline int GetRegulatedBitCode() { return bit_code ^ regulation; }
    inline ~cPromoter() { ; }
  };
  
  
  // --------  Member Variables  --------
  const tMethod* m_functions;
  
  cCPUMemory m_memory;          // Memory...
  cLocalStack m_global_stack;     // A stack that all threads share.
  
  tArray<cLocalThread> m_threads;
  int m_thread_id_chart;
  int m_cur_thread;
  
  int m_avatar;
  
  struct {
    unsigned int m_cycle_count:16;
    unsigned int m_last_output:16;
  };
  
  // Flags
  struct {
    bool m_mal_active:1;         // Has an allocate occured since last divide?
    bool m_advance_ip:1;         // Should the IP advance after this instruction?
    bool m_executedmatchstrings:1;	// Have we already executed the match strings instruction?
    bool m_spec_die:1;
    
    bool m_thread_slicing_parallel:1;
    bool m_no_cpu_cycle_time:1;
    
    bool m_promoters_enabled:1;
    bool m_constitutive_regulation:1;
    bool m_no_active_promoter_halt:1;
    
    bool m_slip_read_head:1;
    
    bool m_io_expire:1;
    
    unsigned int m_waiting_threads:4;
  };
  
  
  // Promoter model
  int m_promoter_index;       // site to begin looking for the next active promoter from
  int m_promoter_offset;      // bit offset when testing whether a promoter is on
  tManagedPointerArray<cPromoter> m_promoters;
  
  
  cHardwareExperimental(const cHardwareExperimental&); // @not_implemented
  cHardwareExperimental& operator=(const cHardwareExperimental&); // @not_implemented
  
  
public:
  cHardwareExperimental(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  ~cHardwareExperimental() { ; }
  
  static tInstLib<cHardwareExperimental::tMethod>* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "instset-experimental.cfg"; }
  
  
  // --------  Core Execution Methods  --------
  bool SingleProcess(cAvidaContext& ctx, bool speculative = false);
  void ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst);
  
  
  // --------  Helper Methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_EXPERIMENTAL; }  
  bool SupportsSpeculative() const { return true; }
  void PrintStatus(std::ostream& fp);
  void SetupMiniTraceFileHeader(const cString& filename, cOrganism* in_organism, const int org_id, const cString& gen_id);
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
  
  const cHeadCPU& IP() const { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP() { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  const cHeadCPU& IP(int thread) const { return m_threads[thread].heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP(int thread) { return m_threads[thread].heads[nHardware::HEAD_IP]; }
  
  
  // --------  Memory Manipulation  --------
  const cCPUMemory& GetMemory() const { return m_memory; }
  cCPUMemory& GetMemory() { return m_memory; }
  int GetMemSize() const { return m_memory.GetSize(); }
  const cCPUMemory& GetMemory(int value) const { return m_memory; }
  cCPUMemory& GetMemory(int value) { return m_memory; }
  int GetMemSize(int value) const { return  m_memory.GetSize(); }
  int GetNumMemSpaces() const { return 1; }
  
  
  // --------  Register Manipulation  --------
  int GetRegister(int reg_id) const { return m_threads[m_cur_thread].reg[reg_id].value; }
  int GetNumRegisters() const { return NUM_REGISTERS; }
  
  
  // --------  Thread Manipulation  --------
  bool ThreadSelect(const int thread_num);
  bool ThreadSelect(const cCodeLabel& in_label) { return false; } // Labeled threads not supported
  inline void ThreadPrev(); // Shift the current thread in use.
  inline void ThreadNext();
  cBioUnit* ThreadGetOwner() { return m_organism; }
  
  int GetNumThreads() const     { return m_threads.GetSize(); }
  int GetCurThread() const      { return m_cur_thread; }
  int GetCurThreadID() const    { return m_threads[m_cur_thread].GetID(); }
  
  // interrupt current thread
  bool InterruptThread(int interruptType) { return false; }
  int GetThreadMessageTriggerType(int _index) { return -1; }
  
  // --------  Parasite Stuff  --------
  bool ParasiteInfectHost(cBioUnit* bu) { return false; }
  
  
  // --------  Non-Standard Methods  --------  
  int GetActiveStack() const { return m_threads[m_cur_thread].cur_stack; }
  bool GetMalActive() const   { return m_mal_active; }
  
  
private:
  
  // --------  Core Execution Methods  --------
  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst);
  void internalReset();
  void internalResetOnFailedDivide();
  
  
  // --------  Stack Manipulation  --------
  inline sInternalValue stackPop();
  inline cLocalStack& getStack(int stack_id);
  inline void switchStack();
  
  
  // --------  Head Manipulation (including IP)  --------
  cHeadCPU& GetActiveHead() { return m_threads[m_cur_thread].heads[m_threads[m_cur_thread].cur_head]; }
  void AdjustHeads();
  
  
  // --------  Label Manipulation  -------
  const cCodeLabel& GetLabel() const { return m_threads[m_cur_thread].next_label; }
  cCodeLabel& GetLabel() { return m_threads[m_cur_thread].next_label; }
  void ReadLabel(int max_size=nHardware::MAX_LABEL_SIZE);
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
  bool ForkThread(); // Adds a new thread based off of m_cur_thread.
  bool ExitThread(); // Kill the current thread!
  
  
  // ---------- Instruction Helpers -----------
  int FindModifiedRegister(int default_register);
  int FindModifiedNextRegister(int default_register);
  int FindModifiedPreviousRegister(int default_register);
  int FindModifiedHead(int default_head);
  int FindNextRegister(int base_reg);
  
  bool Allocate_Necro(const int new_size);
  bool Allocate_Random(cAvidaContext& ctx, const int old_size, const int new_size);
  bool Allocate_Default(const int new_size);
  bool Allocate_Main(cAvidaContext& ctx, const int allocated_size);
  
  int calcCopiedSize(const int parent_size, const int child_size);
  
  inline const cHeadCPU& getHead(int head_id) const { return m_threads[m_cur_thread].heads[head_id]; }
  inline cHeadCPU& getHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
  inline const cHeadCPU& getHead(int head_id, int thread) const { return m_threads[thread].heads[head_id]; }
  inline cHeadCPU& getHead(int head_id, int thread) { return m_threads[thread].heads[head_id];}
  
  inline const cHeadCPU& getIP() const { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  inline cHeadCPU& getIP() { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  inline const cHeadCPU& getIP(int thread) const { return m_threads[thread].heads[nHardware::HEAD_IP]; }
  inline cHeadCPU& getIP(int thread) { return m_threads[thread].heads[nHardware::HEAD_IP]; }

  // --------  Division Support  -------
  bool Divide_Main(cAvidaContext& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1);
  

  // ---------- Utility Functions -----------
  inline unsigned int BitCount(unsigned int value) const;
  inline void setInternalValue(int reg_num, int value, bool from_env = false);
  inline void setInternalValue(int reg_num, int value, const sInternalValue& src);
  inline void setInternalValue(int reg_num, int value, const sInternalValue& op1, const sInternalValue& op2);
  void checkWaitingThreads(int cur_thread, int reg_num);

  void ReadInst(cInstruction in_inst);
  
  
  // ---------- Promoter Helper Functions -----------
  void PromoterTerminate(cAvidaContext& ctx);
  int  Numberate(int _pos, int _dir, int _num_bits = 0);
  bool Do_Numberate(cAvidaContext& ctx, int num_bits = 0);
  
  
  // ---------- Instruction Library -----------
  // Multi-threading
  bool Inst_ForkThread(cAvidaContext& ctx);
  bool Inst_ExitThread(cAvidaContext& ctx);
  bool Inst_IdThread(cAvidaContext& ctx);
  
  // Flow Control
  bool Inst_IfNEqu(cAvidaContext& ctx);
  bool Inst_IfLess(cAvidaContext& ctx);
  bool Inst_IfNotZero(cAvidaContext& ctx);
  bool Inst_IfEqualZero(cAvidaContext& ctx);
  bool Inst_IfGreaterThanZero(cAvidaContext& ctx);
  bool Inst_IfLessThanZero(cAvidaContext& ctx);
  bool Inst_IfGtrX(cAvidaContext& ctx);
  bool Inst_IfEquX(cAvidaContext& ctx);
  bool Inst_IfConsensus(cAvidaContext& ctx);
  bool Inst_IfConsensus24(cAvidaContext& ctx);
  bool Inst_IfLessConsensus(cAvidaContext& ctx);
  bool Inst_IfLessConsensus24(cAvidaContext& ctx);
  bool Inst_IfStackGreater(cAvidaContext& ctx);
  bool Inst_Label(cAvidaContext& ctx);

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
  bool Inst_Mult100(cAvidaContext& ctx);
  
  // Double Argument Math
  bool Inst_Add(cAvidaContext& ctx);
  bool Inst_Sub(cAvidaContext& ctx);
  bool Inst_Mult(cAvidaContext& ctx);
  bool Inst_Div(cAvidaContext& ctx);
  bool Inst_Mod(cAvidaContext& ctx);
  bool Inst_Nand(cAvidaContext& ctx);

  // I/O and Sensory
  bool Inst_TaskIO(cAvidaContext& ctx);
  bool Inst_TaskIOExpire(cAvidaContext& ctx);
  bool Inst_TaskInput(cAvidaContext& ctx);
  bool Inst_TaskOutput(cAvidaContext& ctx);
  bool Inst_TaskOutputZero(cAvidaContext& ctx);
  bool Inst_TaskOutputExpire(cAvidaContext& ctx);

  // Head-based Instructions
  bool Inst_HeadAlloc(cAvidaContext& ctx);
  bool Inst_MoveHead(cAvidaContext& ctx);
  bool Inst_MoveHeadIfNEqu(cAvidaContext& ctx);
  bool Inst_MoveHeadIfLess(cAvidaContext& ctx);
  bool Inst_Goto(cAvidaContext& ctx);
  bool Inst_GotoIfNEqu(cAvidaContext& ctx);
  bool Inst_GotoIfLess(cAvidaContext& ctx);
  bool Inst_GotoConsensus(cAvidaContext& ctx);
  bool Inst_GotoConsensus24(cAvidaContext& ctx);
  bool Inst_JumpHead(cAvidaContext& ctx);
  bool Inst_GetHead(cAvidaContext& ctx);
  bool Inst_IfCopiedCompLabel(cAvidaContext& ctx);
  bool Inst_IfCopiedDirectLabel(cAvidaContext& ctx);
  bool Inst_IfCopiedCompSeq(cAvidaContext& ctx);
  bool Inst_IfCopiedDirectSeq(cAvidaContext& ctx);
  bool Inst_HeadDivide(cAvidaContext& ctx);
  bool Inst_HeadDivideSex(cAvidaContext& ctx);
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
  bool Inst_SetFlow(cAvidaContext& ctx);
  
  // Thread Execution Control
  bool Inst_WaitCondition_Equal(cAvidaContext& ctx);
  bool Inst_WaitCondition_Less(cAvidaContext& ctx);
  bool Inst_WaitCondition_Greater(cAvidaContext& ctx);
  
  // Promoter Model
  bool Inst_Promoter(cAvidaContext& ctx);
  bool Inst_Terminate(cAvidaContext& ctx);
  bool Inst_TerminateConsensus(cAvidaContext& ctx);
  bool Inst_TerminateConsensus24(cAvidaContext& ctx);
  bool Inst_Regulate(cAvidaContext& ctx);
  bool Inst_RegulateSpecificPromoters(cAvidaContext& ctx);
  bool Inst_SenseRegulate(cAvidaContext& ctx);
  bool Inst_Numberate(cAvidaContext& ctx) { return Do_Numberate(ctx); };
  bool Inst_Numberate24(cAvidaContext& ctx) { return Do_Numberate(ctx, 24); };
  bool Inst_Execurate(cAvidaContext& ctx);
  bool Inst_Execurate24(cAvidaContext& ctx);  

  // Bit Consensus
  bool Inst_BitConsensus(cAvidaContext& ctx);
  bool Inst_BitConsensus24(cAvidaContext& ctx);
  
  // Replication
  bool Inst_Repro(cAvidaContext& ctx);
  bool Inst_Die(cAvidaContext& ctx);
  
  // State Grid Navigation
  bool Inst_SGMove(cAvidaContext& ctx);
  bool Inst_SGRotateL(cAvidaContext& ctx);
  bool Inst_SGRotateR(cAvidaContext& ctx);
  bool Inst_SGSense(cAvidaContext& ctx);

  // Movement and Navigation 
  bool Inst_Move(cAvidaContext& ctx);
  bool Inst_RangeMove(cAvidaContext& ctx);
  bool Inst_RangePredMove(cAvidaContext& ctx);
  bool Inst_GetNorthOffset(cAvidaContext& ctx);  
  bool Inst_GetPositionOffset(cAvidaContext& ctx);  
  bool Inst_GetNortherly(cAvidaContext& ctx); 
  bool Inst_GetEasterly(cAvidaContext& ctx);
  bool Inst_ZeroEasterly(cAvidaContext& ctx);
  bool Inst_ZeroNortherly(cAvidaContext& ctx);
  bool Inst_ZeroPosOffset(cAvidaContext& ctx);
  
  // Rotation
  bool Inst_RotateLeftOne(cAvidaContext& ctx);
  bool Inst_RotateRightOne(cAvidaContext& ctx);
  bool Inst_RotateUphill(cAvidaContext& ctx);
  bool Inst_RotateUpFtHill(cAvidaContext& ctx);
  bool Inst_RotateHome(cAvidaContext& ctx);
  bool Inst_RotateUnoccupiedCell(cAvidaContext& ctx);
  bool Inst_RotateX(cAvidaContext& ctx);
  bool Inst_RotateOrgID(cAvidaContext& ctx);
  bool Inst_RotateAwayOrgID(cAvidaContext& ctx);
  
  // Resource and Topography Sensing
  bool Inst_SenseResourceID(cAvidaContext& ctx); 
  bool Inst_SenseResQuant(cAvidaContext& ctx); 
  bool Inst_SenseNest(cAvidaContext& ctx); 
  bool Inst_SenseResDiff(cAvidaContext& ctx); 
  bool Inst_SenseFacedHabitat(cAvidaContext& ctx);
  bool Inst_LookAhead(cAvidaContext& ctx);
  bool Inst_LookAround(cAvidaContext& ctx);
  bool Inst_LookFT(cAvidaContext& ctx);
  bool Inst_SetForageTarget(cAvidaContext& ctx);
  bool Inst_SetForageTargetOnce(cAvidaContext& ctx);
  bool Inst_GetForageTarget(cAvidaContext& ctx);
  bool Inst_SenseOpinionResQuant(cAvidaContext& ctx);
  bool Inst_SenseDiffFaced(cAvidaContext& ctx);
  bool Inst_GetLocOrgDensity(cAvidaContext& ctx);
  bool Inst_GetFacedOrgDensity(cAvidaContext& ctx);
  
  bool DoActualCollect(cAvidaContext& ctx, int bin_used, bool env_remove, bool internal_add, bool probabilistic, bool unit);
  bool Inst_CollectSpecific(cAvidaContext& ctx);

  // Groups 
  bool Inst_JoinGroup(cAvidaContext& ctx);
  bool Inst_ChangePredGroup(cAvidaContext& ctx); // @JJB
  bool Inst_MakePredGroup(cAvidaContext& ctx); // @JJB
  bool Inst_LeavePredGroup(cAvidaContext& ctx); // @JJB
  bool Inst_AdoptPredGroup(cAvidaContext& ctx); // @JJB
  bool Inst_GetGroupID(cAvidaContext& ctx);
  bool Inst_GetPredGroupID(cAvidaContext& ctx);
  bool Inst_IncPredTolerance(cAvidaContext& ctx);  // @JJB
  bool Inst_DecPredTolerance(cAvidaContext& ctx);  // @JJB
  bool Inst_GetPredTolerance(cAvidaContext& ctx);  // @JJB    
  bool Inst_GetPredGroupTolerance(cAvidaContext& ctx);  // @JJB
  void PushToleranceInstExe(int tol_inst, cAvidaContext& ctx);

  // Org Interactions
  bool Inst_GetFacedOrgID(cAvidaContext& ctx);
  bool Inst_AttackPrey(cAvidaContext& ctx); 
  bool Inst_AttackFTPrey(cAvidaContext& ctx); 
  bool Inst_FightMeritOrg(cAvidaContext& ctx); 
  bool Inst_GetMeritFightOdds(cAvidaContext& ctx); 
  bool Inst_FightOrg(cAvidaContext& ctx); 
  bool Inst_AttackPred(cAvidaContext& ctx); 
  bool Inst_KillPred(cAvidaContext& ctx); 
  bool Inst_FightPred(cAvidaContext& ctx); 
  bool Inst_MarkCell(cAvidaContext& ctx); 
  bool Inst_MarkPredCell(cAvidaContext& ctx); 
  bool Inst_ReadFacedCell(cAvidaContext& ctx); 
  bool Inst_ReadFacedPredCell(cAvidaContext& ctx); 
  bool Inst_TeachOffspring(cAvidaContext& ctx);
  bool Inst_LearnParent(cAvidaContext& ctx);
  bool Inst_CheckFacedKin(cAvidaContext& ctx);
  
  // Control-type Instructions
  bool Inst_ScrambleReg(cAvidaContext& ctx);
  
  // ---------- Some Instruction Helpers -----------
  struct searchInfo {
    double amountFound;
    int resource_id;
    bool has_edible;
  };
  struct lookRegAssign {
    int habitat;
    int distance;
    int search_type;
    int id_sought;
    int count;
    int value;
    int group;
    int ft;
  };
  struct lookOut {
    int report_type;
    int habitat;
    int distance;
    int search_type;
    int id_sought;
    int count;
    int value;
    int group;
    int forage;
  }; 
  struct bounds {
    int min_x;
    int min_y;
    int max_x;
    int max_y;
  };
  
  bool GoLook(cAvidaContext& ctx, const int look_dir, const int cell_id, bool use_ft = false);
  searchInfo TestCell(cAvidaContext& ctx, const cResourceLib& resource_lib, const int habitat_used, const int search_type, 
                      const cCoords target_cell_coords, const tSmartArray<int>& val_res, bool first_step);  
  lookOut SetLooking(cAvidaContext& ctx, lookRegAssign& lookin_defs, int facing, int cell_id, bool use_ft = false);
  lookOut WalkCells(cAvidaContext& ctx, const cResourceLib& resource_lib, const int habitat_used, const int search_type, const int distance_sought, const int id_sought, const int facing, const int cell_id);
  lookOut FindOrg(cOrganism* target_org, const int distance, const int facing);
  lookOut GlobalVal(cAvidaContext& ctx, const int habitat_used, const int id_sought, const int search_type);
  void LookResults(lookRegAssign& lookin_defs, lookOut& look_results);
  int TestResDist(const int dist_used, const int search_type, const int id_sought, const int facing, const int cell);
  int GetMinDist(cAvidaContext& ctx, const int worldx, bounds& bounds, const int cell_id, const int distance_sought, 
                 const int facing);
  int GetMaxDist(const int worldx, const int cell_id, const int distance_sought, bounds& res_bounds);
  bounds GetBounds(cAvidaContext& ctx, const cResourceLib& resource_lib, const int res_id, const int search_type);
  bool TestBounds(const cCoords cell_id, bounds& bounds_set);
  tSmartArray<int> BuildResArray(const int habitat_used, const int id_sought, const cResourceLib& resource_lib, bool single_bound);
};


inline cHardwareExperimental::sInternalValue& cHardwareExperimental::sInternalValue::operator=(const sInternalValue& i)
{
  value = i.value;
  originated = i.originated;
  from_env = i.from_env;
  oldest_component = i.oldest_component;
  env_component = i.env_component;
  return *this;
}

inline bool cHardwareExperimental::ThreadSelect(const int thread_num)
{
  if (thread_num >= 0 && thread_num < m_threads.GetSize()) {
    m_cur_thread = thread_num;
    return true;
  }
  
  return false;
}

inline void cHardwareExperimental::ThreadNext()
{
  m_cur_thread++;
  if (m_cur_thread >= m_threads.GetSize()) m_cur_thread = 0;
}

inline void cHardwareExperimental::ThreadPrev()
{
  if (m_cur_thread == 0) m_cur_thread = m_threads.GetSize() - 1;
  else m_cur_thread--;
}

inline cHardwareExperimental::sInternalValue cHardwareExperimental::stackPop()
{
  if (m_threads[m_cur_thread].cur_stack == 0) {
    return m_threads[m_cur_thread].stack.Pop();
  } else {
    return m_global_stack.Pop();
  }
}


inline cHardwareExperimental::cLocalStack& cHardwareExperimental::getStack(int stack_id)
{
  if (stack_id == 0) {
    return m_threads[m_cur_thread].stack;
  } else {
    return m_global_stack;
  }
}


inline void cHardwareExperimental::switchStack()
{
  m_threads[m_cur_thread].cur_stack++;
  if (m_threads[m_cur_thread].cur_stack > 1) m_threads[m_cur_thread].cur_stack = 0;
}


inline int cHardwareExperimental::GetStack(int depth, int stack_id, int in_thread) const
{
  sInternalValue value;

  if(in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;

  if (stack_id == -1) stack_id = m_threads[in_thread].cur_stack;

  if (stack_id == 0) value = m_threads[in_thread].stack.Get(depth);
  else if (stack_id == 1) value = m_global_stack.Get(depth);

  return value.value;
}

inline void cHardwareExperimental::setInternalValue(int reg_num, int value, bool from_env)
{
  sInternalValue& dest = m_threads[m_cur_thread].reg[reg_num];
  dest.value = value;
  dest.from_env = from_env;
  dest.originated = m_cycle_count;
  dest.oldest_component = m_cycle_count;
  dest.env_component = from_env;
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}


inline void cHardwareExperimental::setInternalValue(int reg_num, int value, const sInternalValue& src)
{
  sInternalValue& dest = m_threads[m_cur_thread].reg[reg_num];
  dest.value = value;
  dest.from_env = false;
  dest.originated = m_cycle_count;
  dest.oldest_component = src.oldest_component;
  dest.env_component = src.env_component;
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}


inline void cHardwareExperimental::setInternalValue(int reg_num, int value, const sInternalValue& op1, const sInternalValue& op2)
{
  sInternalValue& dest = m_threads[m_cur_thread].reg[reg_num];
  dest.value = value;
  dest.from_env = false;
  dest.originated = m_cycle_count;
  dest.oldest_component = (op1.oldest_component < op2.oldest_component) ? op1.oldest_component : op2.oldest_component;
  dest.env_component = (op1.env_component || op2.env_component);
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}


#endif
