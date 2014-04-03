/*
 *  cHardwareBCR.h
 *  Avida
 *
 *  Created by David on 11/2/2012 based on cHardwareMBE.h
 *  Copyright 1999-2013 Michigan State University. All rights reserved.
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


class cInstLib;
class cInstSet;
class cOrganism;


class cHardwareBCR : public cHardwareBase
{
public:
  typedef bool (cHardwareBCR::*tMethod)(cAvidaContext& ctx);

private:
  // --------  Structure Constants  --------
  static const int NUM_REGISTERS = 12;
  static const int NUM_BEHAVIORS = 3; // num inst types capable of storing their own data
  static const int NUM_HEADS = NUM_REGISTERS;
  enum { rAX = 0, rBX, rCX, rDX, rEX, rFX, rGX, rHX, rIX, rJX, rKX, rLX };
  enum { hIP, hREAD, hWRITE, hFLOW, hFLOW2, hFLOW3, hFLOW4, hFLOW5, hFLOW6, hFLOW7, hFLOW8, hFLOW9 };
  static const int NUM_NOPS = NUM_REGISTERS;
  static const int MAX_THREADS = NUM_NOPS;
  static const int MAX_MEM_SPACES = NUM_NOPS;
  
  
  // --------  Static Variables  --------
  static tInstLib<cHardwareBCR::tMethod>* s_inst_slib;
  

private:
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
    
    inline DataValue() { Clear(); }
    inline void Clear() { value = 0; originated = 0; from_env = 0, oldest_component = 0; env_component = 0; }
    inline DataValue& operator=(const DataValue& i);
  };
  
  class Head
  {
  protected:
    cHardwareBCR* m_hw;
    int m_pos;
    unsigned int m_ms:31;
    bool m_is_gene:1;
    
    void fullAdjust(int mem_size = -1);
    
  public:
    inline Head(cHardwareBCR* hw = NULL, int pos = 0, unsigned int ms = 0, bool is_gene = false)
      : m_hw(hw), m_pos(pos), m_ms(ms), m_is_gene(is_gene) { ; }
    
    inline void Reset(cHardwareBCR* hw, int pos, unsigned int ms, bool is_gene)
      { m_hw = hw; m_pos = pos; m_ms = ms; m_is_gene = is_gene; }
    
    inline cCPUMemory& GetMemory() { return (m_is_gene) ? m_hw->m_genes[m_ms].memory : m_hw->m_mem_array[m_ms]; }
    
    inline void Adjust();
    
    inline unsigned int MemSpaceIndex() const { return m_ms; }
    inline bool MemSpaceIsGene() const { return m_is_gene; }
    
    inline int Position() const { return m_pos; }
    
    inline void SetPosition(int pos) { m_pos = pos; Adjust(); }
    inline void Set(int pos, unsigned int ms, bool is_gene) { m_pos = pos; m_ms = ms; m_is_gene = is_gene; Adjust(); }
    inline void Set(const Head& head) { m_pos = head.m_pos; m_ms = head.m_ms; m_is_gene = head.m_is_gene; }
    inline void SetAbsPosition(int new_pos) { m_pos = new_pos; }
    
    inline void Jump(int jump) { m_pos += jump; Adjust(); }
    inline void AbsJump(int jump) { m_pos += jump; }
    
    inline void Advance() { m_pos++; Adjust(); }
    
    inline const Instruction& GetInst() { return GetMemory()[m_pos]; }
    inline const Instruction& GetInst(int offset) { return GetMemory()[m_pos + offset]; }
    inline Instruction NextInst();
    inline Instruction PrevInst();
    
    inline void SetInst(const Instruction& value) { GetMemory()[m_pos] = value; }
    inline void InsertInst(const Instruction& inst) { GetMemory().Insert(m_pos, inst); }
    inline void RemoveInst() { GetMemory().Remove(m_pos); }
    
    inline void SetFlagCopied() { return GetMemory().SetFlagCopied(m_pos); }
    inline void SetFlagMutated() { return GetMemory().SetFlagMutated(m_pos); }
    inline void SetFlagExecuted() { return GetMemory().SetFlagExecuted(m_pos); }
    inline void SetFlagPointMut() { return GetMemory().SetFlagPointMut(m_pos); }
    inline void SetFlagCopyMut() { return GetMemory().SetFlagCopyMut(m_pos); }
    
    inline void ClearFlagCopied() { return GetMemory().ClearFlagCopied(m_pos); }
    inline void ClearFlagMutated() { return GetMemory().ClearFlagMutated(m_pos); }
    inline void ClearFlagExecuted() { return GetMemory().ClearFlagExecuted(m_pos); }
    inline void ClearFlagPointMut() { return GetMemory().ClearFlagPointMut(m_pos); }
    inline void ClearFlagCopyMut() { return GetMemory().ClearFlagCopyMut(m_pos); }
    
    // Operator Overloading...
    inline Head& operator++() { m_pos++; Adjust(); return *this; }
    inline Head& operator--() { m_pos--; Adjust(); return *this; }
    inline Head& operator++(int) { return operator++(); }
    inline Head& operator--(int) { return operator--(); }
    inline int operator-(const Head& rhs) { return m_pos - rhs.m_pos; }
    inline bool operator==(const Head& rhs) const;
    inline bool operator!=(const Head& rhs) const { return !operator==(rhs); }
    
    // Bool Tests...
    inline bool AtFront() { return (m_pos == 0); }
    inline bool AtEnd() { return (m_pos + 1 == GetMemory().GetSize()); }
    inline bool InMemory() { return (m_pos >= 0 && m_pos < GetMemory().GetSize()); }
  };
  
  
  class Stack
  {
  private:
    int m_sz;
    DataValue* m_stack;
    int m_sp;
    
  public:
    Stack() : m_sz(0), m_stack(NULL), m_sp(0) { ; }
    inline Stack(const Stack& is) : m_sp(is.m_sp) { Clear(is.m_sz); for (int i = 0; i < m_sz; i++) m_stack[i] = is.m_stack[i]; }
    ~Stack() { delete [] m_stack; }
    
    inline void operator=(const Stack& is) { m_sp = is.m_sp; Clear(is.m_sz); for (int i = 0; i < m_sz; i++) m_stack[i] = is.m_stack[i]; }
    
    inline void Push(const DataValue& value) { if (--m_sp < 0) m_sp = m_sz - 1; m_stack[(int)m_sp] = value; }
    inline DataValue Pop() { DataValue v = m_stack[(int)m_sp]; m_stack[(int)m_sp].Clear(); if (++m_sp == m_sz) m_sp = 0; return v; }
    inline DataValue& Peek() { return m_stack[(int)m_sp]; }
    inline const DataValue& Peek() const { return m_stack[(int)m_sp]; }
    inline const DataValue& Get(int d = 0) const { assert(d >= 0); int p = d + m_sp; return m_stack[(p >= m_sz) ? (p - m_sz) : p]; }
    inline void Clear(int sz) { delete [] m_stack; m_sz = sz; m_stack = new DataValue[sz]; }
  };
  
  
  

  struct Thread
  {
  public:
    cCodeLabel thread_label;

    DataValue reg[NUM_REGISTERS];
    Head heads[NUM_HEADS];
    Stack stack;
    
    struct {
      unsigned int cur_stack:1;
      bool reading_label:1;
      bool reading_seq:1;
      bool running:1;
      bool active:1;
      bool wait_greater:1;
      bool wait_equal:1;
      bool wait_less:1;
      int wait_reg:5;
      unsigned int wait_dst:4;
    };
    int wait_value;
    
    cCodeLabel read_label;
    cCodeLabel read_seq;
    cCodeLabel next_label;
    
    cOrgSensor::sLookInit sensor_session;
    
    inline Thread() { ; }
    inline ~Thread() { ; }
    
    void Reset(cHardwareBCR* in_hardware, const Head& start_pos);
    
  private:
    Thread(const Thread& thread);
    void operator=(const Thread& thread);
  };
  
  struct Gene
  {
    cCPUMemory memory;
    cCodeLabel label;
    int thread_id;
  };
  
  
private:
  // --------  Member Variables  --------
  const tMethod* m_functions;

  // Genes
  Apto::Array<Gene> m_genes;

  // Memory
  Apto::Array<cCPUMemory, Apto::ManagedPointer> m_mem_array;
  char m_mem_ids[MAX_MEM_SPACES];
  
  // Stacks
  Stack m_global_stack;     // A stack that all threads share.
  
  // Threads
  Apto::Array<Thread, Apto::ManagedPointer> m_threads;
  int m_cur_thread;
  

  cOrgSensor m_sensor;
  Apto::Array<cOrgSensor::sLookInit> m_sensor_sessions;
  
  // Flags
  struct {
    unsigned int m_cycle_count:16;
    unsigned int m_last_output:16;

    unsigned int m_cur_uop:8;

    int m_cur_offspring:5;
    
    int m_use_avatar:3;
    
    bool m_advance_ip:1;         // Should the IP advance after this instruction?
    bool m_spec_stall:1;
    bool m_spec_die:1;
    
    bool m_no_cpu_cycle_time:1;
    
    bool m_slip_read_head:1;
    
    unsigned int m_waiting_threads:4;
    unsigned int m_running_threads:4;
  };
  bool m_behav_class_used[3];
  
  cHeadCPU m_placeholder_head;
  
  
private:
  cHardwareBCR(const cHardwareBCR&); // @not_implemented
  cHardwareBCR& operator=(const cHardwareBCR&); // @not_implemented
  
  
public:
  cHardwareBCR(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  ~cHardwareBCR() { ; }
  
  static tInstLib<cHardwareBCR::tMethod>* GetInstLib() { return s_inst_slib; }
  
  
  // --------  Core Execution Methods  --------
  bool SingleProcess(cAvidaContext& ctx, bool speculative = false);
  void ProcessBonusInst(cAvidaContext& ctx, const Instruction& inst);

  
  // --------  Helper Methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_BCR; }
  bool SupportsSpeculative() const { return true; }
  void PrintStatus(std::ostream& fp);
  void SetupMiniTraceFileHeader(Avida::Output::File& df, const int gen_id, const Apto::String& genotype);
  void PrintMiniTraceStatus(cAvidaContext& ctx, std::ostream& fp);
  void PrintMiniTraceSuccess(std::ostream& fp, const int exec_success);
  
  // --------  Stack Manipulation  --------
  inline int GetStack(int depth=0, int stack_id = -1, int in_thread = -1) const;
  inline int GetNumStacks() const { return 2; }
  
  
  // --------  Head Manipulation (including IP)  --------
  const cHeadCPU& GetHead(int head_id) const { return m_placeholder_head; }
  cHeadCPU& GetHead(int head_id) { return m_placeholder_head; }
  const cHeadCPU& GetHead(int head_id, int thread) const { return m_placeholder_head; }
  cHeadCPU& GetHead(int head_id, int thread) { return m_placeholder_head; }
  int GetNumHeads() const { return 0; }
  
  const cHeadCPU& IP() const { return m_placeholder_head; }
  cHeadCPU& IP() { return m_placeholder_head; }
  const cHeadCPU& IP(int thread) const { return m_placeholder_head; }
  cHeadCPU& IP(int thread) { return m_placeholder_head; }
  
  
  // --------  Memory Manipulation  --------
  const cCPUMemory& GetMemory() const { return m_mem_array[0]; }
  cCPUMemory& GetMemory() { return m_mem_array[0]; }
  int GetMemSize() const { return m_mem_array[0].GetSize(); }
  const cCPUMemory& GetMemory(int idx) const { return m_mem_array[idx]; }
  cCPUMemory& GetMemory(int idx) { return m_mem_array[idx]; }
  int GetMemSize(int idx) const { return  m_mem_array[idx].GetSize(); }
  int GetNumMemSpaces() const { return -1; }
  
  
  // --------  Register Manipulation  --------
  int GetRegister(int reg_id) const { return m_threads[m_cur_thread].reg[reg_id].value; }
  int GetNumRegisters() const { return NUM_REGISTERS; }
  
  
  // --------  Thread Manipulation  --------
  Systematics::UnitPtr ThreadGetOwner() { m_organism->AddReference(); return Systematics::UnitPtr(m_organism); }
  
  int GetNumThreads() const     { return m_threads.GetSize(); }
  int GetCurThread() const      { return m_cur_thread; }
  
  
  // --------  Parasite Stuff  -------- @ not implemented
  bool ParasiteInfectHost(Systematics::UnitPtr) { return false; }

  
private:
  // --------  Core Execution Methods  --------
  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const Instruction& cur_inst);
  void internalReset();
  void internalResetOnFailedDivide();
  void setupGenes();
  
  
  // --------  Stack Manipulation  --------
  inline DataValue stackPop();
  inline Stack& getStack(int stack_id);
  inline void switchStack();

  
  // --------  Label Manipulation  -------
  inline const cCodeLabel& GetLabel() const { return m_threads[m_cur_thread].next_label; }
  inline cCodeLabel& GetLabel() { return m_threads[m_cur_thread].next_label; }
  void readLabel(Head& head, cCodeLabel& label, int max_size = cCodeLabel::MAX_LENGTH);
  
  void FindLabelStart(Head& head, Head& default_pos, bool mark_executed);
  void FindLabelForward(Head& head, Head& default_pos, bool mark_executed);
  void FindLabelBackward(Head& head, Head& default_pos, bool mark_executed);
  void FindNopSequenceStart(Head& head, Head& default_pos, bool mark_executed);
  void FindNopSequenceForward(Head& head, Head& default_pos, bool mark_executed);
  void FindNopSequenceBackward(Head& head, Head& default_pos, bool mark_executed);
  inline const cCodeLabel& GetReadLabel() const { return m_threads[m_cur_thread].read_label; }
  inline const cCodeLabel& GetReadSequence() const { return m_threads[m_cur_thread].read_seq; }
  inline cCodeLabel& GetReadLabel() { return m_threads[m_cur_thread].read_label; }
  inline cCodeLabel& GetReadSequence() { return m_threads[m_cur_thread].read_seq; }
  
  
  // --------  Thread Manipulation  -------
  void threadCreate(const cCodeLabel& thread_label, const Head& start_pos);
  
  
  // ---------- Instruction Helpers -----------
  int FindModifiedRegister(int default_register);
  int FindModifiedNextRegister(int default_register);
  int FindModifiedPreviousRegister(int default_register);
  int FindModifiedHead(int default_head);
  int FindNextRegister(int base_reg);
  int FindUpstreamModifiedRegister(int offset, int default_register);
  
  int calcCopiedSize(const int parent_size, const int child_size);
  
  inline Head& getHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
  inline Head& getIP() { return m_threads[m_cur_thread].heads[hIP]; }

  int getRegister(int reg_id) const { return m_threads[m_cur_thread].reg[reg_id].value; }

  
  // --------  Division Support  -------
  bool Divide_Main(cAvidaContext& ctx, int mem_space, int position, double mut_multiplier=1);
  

  // ---------- Utility Functions -----------
  inline void setRegister(int reg_num, int value, bool from_env = false);
  inline void setRegister(int reg_num, int value, const DataValue& src);
  inline void setRegister(int reg_num, int value, const DataValue& op1, const DataValue& op2);
  void checkWaitingThreads(int cur_thread, int reg_num);

  void ReadInst(Instruction in_inst);
  
  
  // ---------- Predator-Prey Support Functions -----------
  struct sAttackReg {
    int success_reg;
    int bonus_reg;
    int bin_reg;
  };
  
  void injureOrg(cAvidaContext& ctx, cOrganism* target);
  void makePred(cAvidaContext& ctx);
  void makeTopPred(cAvidaContext& ctx);
  bool testAttack(cAvidaContext& ctx);
  bool testAttackPred(cAvidaContext& ctx);
  cOrganism* getPreyTarget(cAvidaContext& ctx);
  bool testPreyTarget(cOrganism* target);
  void setAttackReg(sAttackReg& reg);
  bool executeAttack(cAvidaContext& ctx, cOrganism* target, sAttackReg& reg, double odds = -1);
  
  bool testAttackChance(cAvidaContext& ctx, cOrganism* target, sAttackReg& reg, double odds = -1);
  void applyKilledPreyMerit(cAvidaContext& ctx, cOrganism* target, double effic);
  void applyKilledPreyReactions(cOrganism* target);
  void applyKilledPreyBonus(cOrganism* target, sAttackReg& reg, double effic);
  void applyKilledPreyResBins(cOrganism* target, sAttackReg& reg, double effic);

  void tryPreyClone(cAvidaContext& ctx);  
  
  // ---------- Instruction Library -----------
  // Multi-threading
  bool Inst_ThreadCreate(cAvidaContext& ctx);
  bool Inst_ThreadCancel(cAvidaContext& ctx);
  bool Inst_ThreadID(cAvidaContext& ctx);
  bool Inst_Yield(cAvidaContext& ctx);
  bool Inst_RegulatePause(cAvidaContext& ctx);
  bool Inst_RegulatePauseSP(cAvidaContext& ctx);
  bool Inst_RegulateResume(cAvidaContext& ctx);
  bool Inst_RegulateResumeSP(cAvidaContext& ctx);
  bool Inst_RegulateReset(cAvidaContext& ctx);
  bool Inst_RegulateResetSP(cAvidaContext& ctx);
  
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
  bool Inst_CopyVal(cAvidaContext& ctx);

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
  bool Inst_MoveHeadIfNEqu(cAvidaContext& ctx);
  bool Inst_MoveHeadIfLess(cAvidaContext& ctx);
  bool Inst_JumpHead(cAvidaContext& ctx);
  bool Inst_GetHead(cAvidaContext& ctx);
  bool Inst_Divide(cAvidaContext& ctx);
  bool Inst_DivideMemory(cAvidaContext& ctx);
  bool Inst_HeadRead(cAvidaContext& ctx);
  bool Inst_HeadWrite(cAvidaContext& ctx);
  bool Inst_HeadCopy(cAvidaContext& ctx);

  bool Inst_Search_Label_Direct_S(cAvidaContext& ctx);
  bool Inst_Search_Label_Direct_F(cAvidaContext& ctx);
  bool Inst_Search_Label_Direct_B(cAvidaContext& ctx);
  bool Inst_Search_Label_Direct_D(cAvidaContext& ctx);
  bool Inst_Search_Seq_Comp_S(cAvidaContext& ctx);
  bool Inst_Search_Seq_Comp_F(cAvidaContext& ctx);
  bool Inst_Search_Seq_Comp_B(cAvidaContext& ctx);
  bool Inst_Search_Seq_Comp_D(cAvidaContext& ctx);

  // Thread Execution Control
  bool Inst_WaitCondition_Equal(cAvidaContext& ctx);
  bool Inst_WaitCondition_Less(cAvidaContext& ctx);
  bool Inst_WaitCondition_Greater(cAvidaContext& ctx);
  
  // Replication
  bool Inst_IfCopiedCompLabel(cAvidaContext& ctx);
  bool Inst_IfCopiedDirectLabel(cAvidaContext& ctx);
  bool Inst_IfCopiedCompSeq(cAvidaContext& ctx);
  bool Inst_IfCopiedDirectSeq(cAvidaContext& ctx);
  bool Inst_DidCopyCompLabel(cAvidaContext& ctx);
  bool Inst_DidCopyDirectLabel(cAvidaContext& ctx);
  bool Inst_DidCopyCompSeq(cAvidaContext& ctx);
  bool Inst_DidCopyDirectSeq(cAvidaContext& ctx);
  bool Inst_Repro(cAvidaContext& ctx);
  bool Inst_Die(cAvidaContext& ctx);
  
  // State Grid Navigation
  bool Inst_SGMove(cAvidaContext& ctx);
  bool Inst_SGRotateL(cAvidaContext& ctx);
  bool Inst_SGRotateR(cAvidaContext& ctx);
  bool Inst_SGSense(cAvidaContext& ctx);
  
  // Movement and Navigation
  bool Inst_Move(cAvidaContext& ctx);
  bool Inst_JuvMove(cAvidaContext& ctx);
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
  bool Inst_RotateOrgID(cAvidaContext& ctx);
  bool Inst_RotateAwayOrgID(cAvidaContext& ctx);

  // Resource and Topography Sensing
  bool Inst_SenseResourceID(cAvidaContext& ctx); 
  bool Inst_SenseNest(cAvidaContext& ctx);
  bool Inst_SenseFacedHabitat(cAvidaContext& ctx);
  bool Inst_LookAhead(cAvidaContext& ctx);
  bool Inst_LookAheadIntercept(cAvidaContext& ctx);
  bool Inst_LookAheadEX(cAvidaContext& ctx);
  bool Inst_LookAgainEX(cAvidaContext& ctx);
  bool Inst_LookAheadFTX(cAvidaContext& ctx);
  bool Inst_LookAgainFTX(cAvidaContext& ctx);
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
  
  bool Inst_ModifySimpDisplay(cAvidaContext& ctx);
  bool Inst_ReadLastSimpDisplay(cAvidaContext& ctx);
  bool Inst_KillDisplay(cAvidaContext& ctx);
  
  // Predator-Prey Instructions
  bool Inst_AttackPrey(cAvidaContext& ctx);
  bool Inst_AttackFTPrey(cAvidaContext& ctx);
  
  // Control-type Instructions
  bool Inst_ScrambleReg(cAvidaContext& ctx);

private:
  static tInstLib<cHardwareBCR::tMethod>* initInstLib();
  
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
  void LookResults(cAvidaContext& ctx, sLookRegAssign& lookin_defs, cOrgSensor::sLookOut& look_results);

  
  bool DoLookAheadEX(cAvidaContext& ctx, bool use_ft = false);
  bool DoLookAgainEX(cAvidaContext& ctx, bool use_ft = false);
  
private:
  class ThreadLabelIterator
  {
  private:
    cHardwareBCR* m_hw;
    const cCodeLabel m_label;
    bool m_is_specific;
    int m_next_idx;
    int m_cur_id;
    
  public:
    ThreadLabelIterator(cHardwareBCR* hw, const cCodeLabel& label, bool specific = true)
      : m_hw(hw), m_label(label), m_is_specific(specific), m_next_idx(0), m_cur_id(-1) { ; }
    
    inline int Next()
    {
      for (; m_next_idx < m_hw->m_threads.GetSize(); m_next_idx++) {
        const cCodeLabel& thread_label = m_hw->m_threads[m_next_idx].next_label;
        if ((m_is_specific && m_label == thread_label) || thread_label.Contains(m_label)) {
          m_cur_id = m_next_idx;
          m_next_idx++;
          return m_cur_id;
        }
      }
      m_cur_id = -1;
      return m_cur_id;
    }
    
    inline int Get() const { return m_cur_id; }
    inline void Reset() { m_next_idx = 0; m_cur_id = -1; }
  };

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




inline void cHardwareBCR::Head::Adjust()
{
  const int mem_size = GetMemory().GetSize();
  
  // If we are still in range, stop here!
  if (m_pos >= 0 && m_pos < mem_size) return;
  
  // If the memory is gone, just stick it at the begining of its parent.
  if (mem_size == 0 || m_pos < 0) {
    m_pos = 0;
    return;
  }
  
  // position back at the begining of the memory as necessary.
  if (m_pos < (2 * mem_size)) m_pos -= mem_size;
  else m_pos %= mem_size;
}


inline bool cHardwareBCR::Head::operator==(const Head& rhs) const
{
  return m_hw == rhs.m_hw && m_pos == rhs.m_pos && m_ms == rhs.m_ms && m_is_gene == rhs.m_is_gene;
}

inline Instruction cHardwareBCR::Head::PrevInst()
{
  return (AtFront()) ? GetMemory()[GetMemory().GetSize() - 1] : GetMemory()[m_pos - 1];
}

inline Instruction cHardwareBCR::Head::NextInst()
{
  return (AtEnd()) ? m_hw->GetInstSet().GetInstError() : GetMemory()[m_pos + 1];
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

inline void cHardwareBCR::setRegister(int reg_num, int value, bool from_env)
{
  DataValue& dest = m_threads[m_cur_thread].reg[reg_num];
  dest.value = value;
  dest.from_env = from_env;
  dest.originated = m_cycle_count;
  dest.oldest_component = m_cycle_count;
  dest.env_component = from_env;
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}

inline void cHardwareBCR::setRegister(int reg_num, int value, const DataValue& src)
{
  DataValue& dest = m_threads[m_cur_thread].reg[reg_num];
  dest.value = value;
  dest.from_env = false;
  dest.originated = m_cycle_count;
  dest.oldest_component = src.oldest_component;
  dest.env_component = src.env_component;
  if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
}

inline void cHardwareBCR::setRegister(int reg_num, int value, const DataValue& op1, const DataValue& op2)
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
