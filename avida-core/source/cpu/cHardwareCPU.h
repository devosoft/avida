/*
*  cHardwareCPU.h
*  Avida
*
*  Called "hardware_cpu.hh" prior to 11/17/05.
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

#ifndef cHardwareCPU_h
#define cHardwareCPU_h

#include "avida/Avida.h"

#include "cCodeLabel.h"
#include "cHeadCPU.h"
#include "cCPUMemory.h"
#include "cCPUStack.h"
#include "cHardwareBase.h"
#include "cString.h"
#include "cStats.h"
#include "tInstLib.h"

#include "nHardware.h"

#include <iomanip>
#include <vector>

/**
* Each organism may have a cHardwareCPU structure which keeps track of the
* current status of all the components of the simulated hardware.
*
* @see cHardwareCPU_Thread, cCPUStack, cCPUMemory, cInstSet
**/

class cInstLib;
class cInstSet;
class cOrganism;


class cHardwareCPU : public cHardwareBase
{
public:
  typedef bool (cHardwareCPU::*tMethod)(cAvidaContext& ctx);

protected:
  // --------  Structure Constants  --------
  static const int NUM_REGISTERS = 3;
  static const int NUM_HEADS = nHardware::NUM_HEADS >= NUM_REGISTERS ? nHardware::NUM_HEADS : NUM_REGISTERS;
  enum tRegisters { REG_AX = 0, REG_BX, REG_CX, REG_DX, REG_EX, REG_FX };
  static const int NUM_NOPS = 3;

  // --------  Data Structures  --------
  struct cLocalThread
  {
  private:
    int m_id;
    int m_promoter_inst_executed;
    int m_messageTriggerType;
  public:
    int reg[NUM_REGISTERS];
    cHeadCPU heads[NUM_HEADS];
    cCPUStack stack;
    unsigned char cur_stack;              // 0 = local stack, 1 = global stack.
    unsigned char cur_head;

    cCodeLabel read_label;
    cCodeLabel next_label;


    cLocalThread(cHardwareBase* in_hardware = NULL, int in_id = -1) { Reset(in_hardware, in_id); }
    ~cLocalThread() { ; }

    void operator=(const cLocalThread& in_thread);

    void Reset(cHardwareBase* in_hardware, int in_id);
    int GetID() const { return m_id; }
    void SetID(int in_id) { m_id = in_id; }
    int GetPromoterInstExecuted() { return m_promoter_inst_executed; }
    void IncPromoterInstExecuted() { m_promoter_inst_executed++; }
    void ResetPromoterInstExecuted() { m_promoter_inst_executed = 0; }
    void setMessageTriggerType(int value) { m_messageTriggerType = value; }
    int getMessageTriggerType() { return m_messageTriggerType; }
  };


  // --------  Static Variables  --------
  static tInstLib<tMethod>* s_inst_slib;
  static tInstLib<tMethod>* initInstLib(void);


  // --------  Member Variables  --------
  const tMethod* m_functions;

  cCPUMemory m_memory;          // Memory...
  cCPUStack m_global_stack;     // A stack that all threads share.

  Apto::Array<cLocalThread> m_threads;
  int m_thread_id_chart;
  int m_cur_thread;

  // Flags...
  struct {
    bool m_mal_active:1;         // Has an allocate occured since last divide?
    bool m_advance_ip:1;         // Should the IP advance after this instruction?
    bool m_executedmatchstrings:1;	// Have we already executed the match strings instruction?
    bool m_spec_die:1;

    bool m_thread_slicing_parallel:1;
    bool m_no_cpu_cycle_time:1;

    bool m_promoters_enabled:1;
    bool m_constitutive_regulation:1;

    bool m_slip_read_head:1;
  };

  // <-- Promoter model
  int m_promoter_index;       //site to begin looking for the next active promoter from
  int m_promoter_offset;      //bit offset when testing whether a promoter is on

  struct cPromoter 
  {
  public:
    int m_pos;      //position within genome
    int m_bit_code; //bit code of promoter
    int m_regulation; //bit code of promoter
  public:
    cPromoter(int _pos = 0, int _bit_code = 0, int _regulation = 0) { m_pos = _pos; m_bit_code = _bit_code; m_regulation = _regulation; }
    int GetRegulatedBitCode() { return m_bit_code ^ m_regulation; }
    ~cPromoter() { ; }
  };
  Apto::Array<cPromoter> m_promoters;
  // Promoter Model -->

  // <-- Epigenetic State
  bool m_epigenetic_state;
  int m_epigenetic_saved_reg[NUM_REGISTERS];
  cCPUStack m_epigenetic_saved_stack;
  // Epigenetic State -->


  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const Instruction& cur_inst);
  
  // --------  Stack Manipulation...  --------
  inline void StackPush(int value);
  inline int StackPop();
  inline void StackFlip();
  inline void StackClear();
  inline void SwitchStack();


  // --------  Head Manipulation (including IP)  --------
  cHeadCPU& GetActiveHead() { return m_threads[m_cur_thread].heads[m_threads[m_cur_thread].cur_head]; }
  void AdjustHeads();


  // --------  Label Manipulation  -------
  const cCodeLabel& GetLabel() const { return m_threads[m_cur_thread].next_label; }
  cCodeLabel& GetLabel() { return m_threads[m_cur_thread].next_label; }
  void ReadLabel(int max_size=cCodeLabel::MAX_LENGTH);
  cHeadCPU FindLabel(int direction);
  int FindLabel_Forward(const cCodeLabel & search_label, const InstructionSequence& search_genome, int pos);
  int FindLabel_Backward(const cCodeLabel & search_label, const InstructionSequence& search_genome, int pos);
  cHeadCPU FindLabel(const cCodeLabel & in_label, int direction);
  void FindLabelInMemory(const cCodeLabel& label, cHeadCPU& search_head);

  const cCodeLabel& GetReadLabel() const { return m_threads[m_cur_thread].read_label; }
  cCodeLabel& GetReadLabel() { return m_threads[m_cur_thread].read_label; }


  // --------  Thread Manipulation  -------
  bool ForkThread(); // Adds a new thread based off of m_cur_thread.
  bool InterruptThread(int interruptType); // Create a new thread that interrupts the current thread
  bool KillThread(); // Kill the current thread!

  // ---------- Instruction Helpers -----------
  int FindModifiedRegister(int default_register);
  int FindModifiedNextRegister(int default_register);
  int FindModifiedPreviousRegister(int default_register);
  int FindModifiedHead(int default_head);
  int FindNextRegister(int base_reg);

  inline const cHeadCPU& getHead(int head_id) const { return m_threads[m_cur_thread].heads[head_id]; }
  inline cHeadCPU& getHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
  inline const cHeadCPU& getHead(int head_id, int thread) const { return m_threads[thread].heads[head_id]; }
  inline cHeadCPU& getHead(int head_id, int thread) { return m_threads[thread].heads[head_id];}

  inline const cHeadCPU& getIP() const { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  inline cHeadCPU& getIP() { return m_threads[m_cur_thread].heads[nHardware::HEAD_IP]; }
  inline const cHeadCPU& getIP(int thread) const { return m_threads[thread].heads[nHardware::HEAD_IP]; }
  inline cHeadCPU& getIP(int thread) { return m_threads[thread].heads[nHardware::HEAD_IP]; }


  bool Allocate_Necro(const int new_size);
  bool Allocate_Random(cAvidaContext& ctx, const int old_size, const int new_size);
  bool Allocate_Default(const int new_size);
  bool Allocate_Main(cAvidaContext& ctx, const int allocated_size);


  void internalReset();

  void internalResetOnFailedDivide();


  int calcCopiedSize(const int parent_size, const int child_size);

  bool Divide_Main(cAvidaContext& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1);
  bool Divide_MainRS(cAvidaContext& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1); //AWC 06/29/06
  bool Divide_Main1RS(cAvidaContext& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1); //AWC 07/28/06
  bool Divide_Main2RS(cAvidaContext& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1); //AWC 07/28/06

  void Divide_DoTransposons(cAvidaContext& ctx);
  void InheritState(cHardwareBase& in_hardware);

  bool HeadCopy_ErrorCorrect(cAvidaContext& ctx, double reduction);
  bool Inst_HeadDivideMut(cAvidaContext& ctx, double mut_multiplier = 1);

  void ReadInst(const int in_inst);


  cHardwareCPU& operator=(const cHardwareCPU&); // @not_implemented

public:
  cHardwareCPU(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  ~cHardwareCPU() { ; }

  static tInstLib<tMethod>* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "instset-heads.cfg"; }

  bool SingleProcess(cAvidaContext& ctx, bool speculative = false);
  void ProcessBonusInst(cAvidaContext& ctx, const Instruction& inst);


  // --------  Helper methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_ORIGINAL; }  
  bool SupportsSpeculative() const { return true; }
  void PrintStatus(std::ostream& fp);
  void SetupMiniTraceFileHeader(Avida::Output::File& df, const int gen_id, const Apto::String& genotype);
  void PrintMiniTraceStatus(cAvidaContext& ctx, std::ostream& fp) { (void)ctx, (void)fp; }
  void PrintMiniTraceSuccess(std::ostream& fp, const int exec_success) { (void)fp, (void)exec_success; }

  // --------  Stack Manipulation...  --------
  inline int GetStack(int depth=0, int stack_id=-1, int in_thread=-1) const;
  inline int GetCurStack(int in_thread = -1) const;
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
  const cCPUMemory& GetMemory(int) const { return m_memory; }
  cCPUMemory& GetMemory(int) { return m_memory; }
  int GetMemSize(int) const { return  m_memory.GetSize(); }
  int GetNumMemSpaces() const { return 1; }


  // --------  Register Manipulation  --------
  int GetRegister(int reg_id) const { return m_threads[m_cur_thread].reg[reg_id]; }
  int& GetRegister(int reg_id) { return m_threads[m_cur_thread].reg[reg_id]; }
  int GetNumRegisters() const { return NUM_REGISTERS; }


  // --------  Thread Manipulation  --------
  inline void ThreadPrev(); // Shift the current thread in use.
  Systematics::UnitPtr ThreadGetOwner() { m_organism->AddReference(); return Systematics::UnitPtr(m_organism); }
  

  int GetNumThreads() const     { return m_threads.GetSize(); }
  int GetCurThread() const      { return m_cur_thread; }
  const cLocalThread& GetThread(int _index) const { return m_threads[_index]; }
  int GetThreadMessageTriggerType(int _index) { return m_threads[_index].getMessageTriggerType(); }

  // --------  Parasite Stuff  --------
  bool ParasiteInfectHost(Systematics::UnitPtr) { return false; }
    
  // -------- Kaboom Stuff ------------
  bool checkNoMutList(cHeadCPU to);


  // Non-Standard Methods

  int GetActiveStack() const { return m_threads[m_cur_thread].cur_stack; }


private:
  // ---------- Instruction Library -----------

  // Flow Control
  bool Inst_If0(cAvidaContext& ctx);
  bool Inst_IfEqu(cAvidaContext& ctx);
  bool Inst_IfNot0(cAvidaContext& ctx);
  bool Inst_If0_defaultAX(cAvidaContext& ctx);
  bool Inst_IfNot0_defaultAX(cAvidaContext& ctx);
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
  bool Inst_IfGrX(cAvidaContext& ctx);
  bool Inst_IfEquX(cAvidaContext& ctx);

  bool Inst_IfAboveResLevel(cAvidaContext& ctx);
  bool Inst_IfAboveResLevelEnd(cAvidaContext& ctx);
  bool Inst_IfNotAboveResLevel(cAvidaContext& ctx);
  bool Inst_IfNotAboveResLevelEnd(cAvidaContext& ctx);
  
  bool Inst_IfGerm(cAvidaContext& ctx);
  bool Inst_IfSoma(cAvidaContext& ctx);  

  // Probabilistic ifs.
  bool Inst_IfP0p125(cAvidaContext& ctx);
  bool Inst_IfP0p25(cAvidaContext& ctx);
  bool Inst_IfP0p50(cAvidaContext& ctx);
  bool Inst_IfP0p75(cAvidaContext& ctx);

  // If-less-else-if, else, endif
  cHeadCPU Find(const char* instr);
  void Else_TopHalf();
  bool Inst_IfLessEnd(cAvidaContext& ctx);
  bool Inst_IfNotEqualEnd(cAvidaContext& ctx);
  bool Inst_IfGrtEquEnd(cAvidaContext& ctx);
  bool Inst_Else(cAvidaContext& ctx);
  bool Inst_EndIf(cAvidaContext& ctx);	

  bool Inst_JumpF(cAvidaContext& ctx);
  bool Inst_JumpB(cAvidaContext& ctx);
  bool Inst_Call(cAvidaContext& ctx);
  bool Inst_Return(cAvidaContext& ctx);

  bool Inst_Throw(cAvidaContext& ctx);
  bool Inst_ThrowIf0(cAvidaContext& ctx);
  bool Inst_ThrowIfNot0(cAvidaContext& ctx);  
  bool Inst_Catch(cAvidaContext&) { ReadLabel(); return true; };
 
  bool Inst_Goto(cAvidaContext& ctx);
  bool Inst_GotoIf0(cAvidaContext& ctx);
  bool Inst_GotoIfNot0(cAvidaContext& ctx);  
  bool Inst_Label(cAvidaContext&) { ReadLabel(); return true; };
    
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
  bool Inst_All1s(cAvidaContext& ctx);
  bool Inst_Zero(cAvidaContext& ctx);
  bool Inst_One(cAvidaContext& ctx);
  bool Inst_Not(cAvidaContext& ctx);
  bool Inst_Neg(cAvidaContext& ctx);
  bool Inst_Square(cAvidaContext& ctx);
  bool Inst_Sqrt(cAvidaContext& ctx);
  bool Inst_Log(cAvidaContext& ctx);
  bool Inst_Log10(cAvidaContext& ctx);

  // Double Argument Math
  bool Inst_Add(cAvidaContext& ctx);
  bool Inst_Sub(cAvidaContext& ctx);
  bool Inst_Mult(cAvidaContext& ctx);
  bool Inst_Div(cAvidaContext& ctx);
  bool Inst_Mod(cAvidaContext& ctx);
  bool Inst_Nand(cAvidaContext& ctx);
  bool Inst_Or(cAvidaContext& ctx);
  bool Inst_Nor(cAvidaContext& ctx);
  bool Inst_And(cAvidaContext& ctx);
  bool Inst_Order(cAvidaContext& ctx);
  bool Inst_Xor(cAvidaContext& ctx);

  // Bit-setting instructions
  bool Inst_Setbit(cAvidaContext& ctx);
  bool Inst_Clearbit(cAvidaContext& ctx);

  // Double Argument Math that are treatable
  bool Inst_NandTreatable(cAvidaContext& ctx);

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
  bool Inst_DivideRS(cAvidaContext& ctx); // AWC 06/29/06
  bool Inst_CAlloc(cAvidaContext& ctx);
  bool Inst_CDivide(cAvidaContext& ctx);
  bool Inst_MaxAlloc(cAvidaContext& ctx);
  bool Inst_MaxAllocMoveWriteHead(cAvidaContext& ctx);
  bool Inst_Transposon(cAvidaContext& ctx);
  bool Inst_ReproDeme(cAvidaContext& ctx);
  bool Inst_Repro(cAvidaContext& ctx);
  bool Inst_ReproSex(cAvidaContext& ctx);
  bool Inst_ReproGermFlag(cAvidaContext& ctx);
  bool Inst_TaskPutRepro(cAvidaContext& ctx);
  bool Inst_TaskPutResetInputsRepro(cAvidaContext& ctx);
  bool Inst_ConditionalRepro(cAvidaContext& ctx);
  bool Inst_Sterilize(cAvidaContext& ctx);

  bool Inst_SpawnDeme(cAvidaContext& ctx);
  bool Inst_Lyse(cAvidaContext& ctx);
  bool Inst_Lyse_PreDivide(cAvidaContext& ctx);
  bool Inst_Lyse_PostDivide(cAvidaContext& ctx);
  bool Inst_NopPre(cAvidaContext& ctx);
  bool Inst_NopPost(cAvidaContext& ctx);
  bool Inst_Kazi(cAvidaContext& ctx);
  bool Inst_Kazi1(cAvidaContext& ctx);
  bool Inst_Kazi2(cAvidaContext& ctx);
  bool Inst_Kazi3(cAvidaContext& ctx);
  bool Inst_Kazi4(cAvidaContext& ctx);
  bool Inst_Kazi5(cAvidaContext& ctx);
  bool Inst_SenseQuorum(cAvidaContext& ctx);
  bool Inst_NoisyQuorum(cAvidaContext& ctx);
  bool Inst_SmartExplode(cAvidaContext& ctx);
  bool Inst_Die(cAvidaContext& ctx);
  bool Inst_Poison(cAvidaContext& ctx);
  bool Inst_Suicide(cAvidaContext& ctx);
  bool Inst_RelinquishEnergyToFutureDeme(cAvidaContext& ctx);
  bool Inst_RelinquishEnergyToNeighborOrganisms(cAvidaContext& ctx);
  bool Inst_RelinquishEnergyToOrganismsInDeme(cAvidaContext& ctx);

  // I/O and Sensory
  bool Inst_TaskGet(cAvidaContext& ctx);
  bool Inst_TaskGet2(cAvidaContext& ctx);
  bool Inst_TaskStackGet(cAvidaContext& ctx);
  bool Inst_TaskStackLoad(cAvidaContext& ctx);
  bool Inst_TaskPut(cAvidaContext& ctx);
  bool Inst_TaskPutResetInputs(cAvidaContext& ctx);
  bool Inst_TaskIO(cAvidaContext& ctx);
  bool Inst_TaskIO_Feedback(cAvidaContext& ctx);
  bool Inst_TaskIO_BonusCost(cAvidaContext& ctx, double bonus_cost);
  bool Inst_TaskIO_BonusCost_0_001(cAvidaContext& ctx) { return Inst_TaskIO_BonusCost(ctx, 0.001); };
  bool Inst_MatchStrings(cAvidaContext& ctx);
  bool Inst_Send(cAvidaContext& ctx);
  bool Inst_Receive(cAvidaContext& ctx);
  bool Inst_SenseLog2(cAvidaContext& ctx);
  bool Inst_SenseUnit(cAvidaContext& ctx);
  bool Inst_SenseMult100(cAvidaContext& ctx);
  bool DoSense(cAvidaContext& ctx, int conversion_method, double base);
  bool DoSenseResourceX(int reg_to_set, int cell_id, int resid, cAvidaContext& ctx); 
  bool Inst_SenseResource0(cAvidaContext& ctx);
  bool Inst_SenseResource1(cAvidaContext& ctx);
  bool Inst_SenseResource2(cAvidaContext& ctx);
  bool Inst_SenseFacedResource0(cAvidaContext& ctx);
  bool Inst_SenseFacedResource1(cAvidaContext& ctx);
  bool Inst_SenseFacedResource2(cAvidaContext& ctx);
  bool Inst_SenseResourceID(cAvidaContext& ctx);
  // Resources of next group +1 or -1, based on positive or negative value in the nop register,
  // wrapping from the top group back to group 1 (skipping 0). 
  bool Inst_SenseNextResLevel(cAvidaContext& ctx);
  bool Inst_SenseOpinionResourceQuantity(cAvidaContext& ctx);
  bool Inst_SenseDiffFaced(cAvidaContext& ctx);
  bool Inst_SenseFacedHabitat(cAvidaContext& ctx);

  // Resources
  int FindModifiedResource(cAvidaContext& ctx, int& spec_id);
  bool DoCollect(cAvidaContext& ctx, bool env_remove, bool internal_add, bool probabilistic, bool unit, float nUnits);
  bool DoActualCollect(cAvidaContext& ctx, int bin_used, bool env_remove, bool internal_add, bool probabilistic, bool unit, float nUnits);
  bool Inst_Collect(cAvidaContext& ctx);
  bool Inst_CollectNoEnvRemove(cAvidaContext& ctx);
  bool Inst_Destroy(cAvidaContext& ctx);
  bool Inst_NopCollect(cAvidaContext& ctx);
  bool Inst_CollectUnitProbabilistic(cAvidaContext& ctx);
  bool Inst_CollectSpecific(cAvidaContext& ctx);
  bool Inst_CollectSpecificNeeded(cAvidaContext& ctx);
  bool Inst_CollectSpecificRatio(cAvidaContext& ctx);
  bool Inst_IfResources(cAvidaContext& ctx);  //! Execute the following instruction if all resources are above their min level.

  // Donation
  void DoDonate(cAvidaContext& ctx, cOrganism * to_org);
  void DoEnergyDonate(cAvidaContext& ctx, cOrganism* to_org);
  void DoEnergyDonatePercent(cAvidaContext& ctx, cOrganism* to_org, const double frac_energy_given);
  void DoEnergyDonateAmount(cAvidaContext& ctx, cOrganism* to_org, const double amount);
  bool Inst_DonateRandom(cAvidaContext& ctx);
  bool Inst_DonateKin(cAvidaContext& ctx);
  bool Inst_DonateEditDist(cAvidaContext& ctx);
  bool Inst_GetFacedEditDistance(cAvidaContext& ctx);
  bool Inst_DonateGreenBeardGene(cAvidaContext& ctx);
  bool Inst_DonateTrueGreenBeard(cAvidaContext& ctx);
  bool Inst_DonateShadedGreenBeard(cAvidaContext& ctx);
  bool Inst_DonateThreshGreenBeard(cAvidaContext& ctx);
  bool Inst_DonateQuantaThreshGreenBeard(cAvidaContext& ctx);
  bool Inst_DonateGreenBeardSameLocus(cAvidaContext& ctx);
  bool Inst_DonateNULL(cAvidaContext& ctx);
  bool Inst_DonateFacing(cAvidaContext& ctx);
  bool Inst_ReceiveDonatedEnergy(cAvidaContext& ctx);
  bool Inst_DonateEnergy(cAvidaContext& ctx);
  bool Inst_UpdateMetabolicRate(cAvidaContext& ctx);
  bool Inst_DonateEnergyFaced(cAvidaContext& ctx);
  bool Inst_DonateEnergyFaced1(cAvidaContext& ctx);
  bool Inst_DonateEnergyFaced2(cAvidaContext& ctx);
  bool Inst_DonateEnergyFaced5(cAvidaContext& ctx);
  bool Inst_DonateEnergyFaced10(cAvidaContext& ctx);
  bool Inst_DonateEnergyFaced20(cAvidaContext& ctx);
  bool Inst_DonateEnergyFaced50(cAvidaContext& ctx);
  bool Inst_DonateEnergyFaced100(cAvidaContext& ctx);
  bool Inst_RotateToMostNeedy(cAvidaContext& ctx);
  bool Inst_RequestEnergy(cAvidaContext& ctx);
  bool Inst_RequestEnergyFlagOn(cAvidaContext& ctx);
  bool Inst_RequestEnergyFlagOff(cAvidaContext& ctx);
  bool Inst_IncreaseEnergyDonation(cAvidaContext& ctx);
  bool Inst_DecreaseEnergyDonation(cAvidaContext& ctx);

  void DoResourceDonatePercent(cAvidaContext& ctx, const int to_cell, const int resource_id, const double frac_resource_given); 
  void DoResourceDonateAmount(cAvidaContext& ctx, const int to_cell, const int resource_id, const double amount); 
  bool DonateResourceX(cAvidaContext& ctx, const int res_id);
  bool Inst_DonateResource0(cAvidaContext& ctx);
  bool Inst_DonateResource1(cAvidaContext& ctx);
  bool Inst_DonateResource2(cAvidaContext& ctx);
  bool Inst_DonateSpecific(cAvidaContext& ctx);


  bool Inst_SearchF(cAvidaContext& ctx);
  bool Inst_SearchB(cAvidaContext& ctx);
  bool Inst_MemSize(cAvidaContext& ctx);

  bool Inst_IOBufAdd1(cAvidaContext& ctx);
  bool Inst_IOBufAdd0(cAvidaContext& ctx);

  // Environment

  bool Inst_RotateL(cAvidaContext& ctx);
  bool Inst_RotateR(cAvidaContext& ctx);
  bool Inst_RotateLeftOne(cAvidaContext& ctx);
  bool Inst_RotateRightOne(cAvidaContext& ctx);
  bool Inst_RotateLabel(cAvidaContext& ctx);
  bool Inst_RotateOccupiedCell(cAvidaContext& ctx);
  bool Inst_RotateNextOccupiedCell(cAvidaContext& ctx);
  bool Inst_RotateUnoccupiedCell(cAvidaContext& ctx);
  bool Inst_RotateNextUnoccupiedCell(cAvidaContext& ctx);
  bool Inst_RotateEventCell(cAvidaContext& ctx);
  bool Inst_RotateUphill(cAvidaContext& ctx);
  bool Inst_RotateHome(cAvidaContext& ctx);
  bool Inst_SetCopyMut(cAvidaContext& ctx);
  bool Inst_ModCopyMut(cAvidaContext& ctx);
  bool Inst_GetCellPosition(cAvidaContext& ctx);
  bool Inst_GetCellPositionX(cAvidaContext& ctx);
  bool Inst_GetCellPositionY(cAvidaContext& ctx);
  bool Inst_GetDirectionOffNorth(cAvidaContext& ctx);  
  bool Inst_GetNortherly(cAvidaContext& ctx); 
  bool Inst_GetEasterly(cAvidaContext& ctx);
  bool Inst_ZeroEasterly(cAvidaContext& ctx);
  bool Inst_ZeroNortherly(cAvidaContext& ctx);

  // State Grid Sensory/Movement
  bool Inst_SGMove(cAvidaContext& ctx);
  bool Inst_SGRotateL(cAvidaContext& ctx);
  bool Inst_SGRotateR(cAvidaContext& ctx);
  bool Inst_SGSense(cAvidaContext& ctx);

  bool Inst_GetDistanceFromDiagonal(cAvidaContext& ctx);
  bool Inst_Tumble(cAvidaContext& ctx);
  bool Inst_Move(cAvidaContext& ctx);
  bool Inst_MoveToEvent(cAvidaContext& ctx);
  bool Inst_IfNeighborEventInUnoccupiedCell(cAvidaContext& ctx);
  bool Inst_IfFacingEventCell(cAvidaContext& ctx);
  bool Inst_IfEventInCell(cAvidaContext& ctx);

  // Multi-threading...

  bool Inst_ForkThread(cAvidaContext& ctx);
  bool Inst_ForkThreadLabel(cAvidaContext& ctx);
  bool Inst_ForkThreadLabelIf0(cAvidaContext& ctx);
  bool Inst_ForkThreadLabelIfNot0(cAvidaContext& ctx);
  bool Inst_KillThread(cAvidaContext& ctx);
  bool Inst_ThreadID(cAvidaContext& ctx);

  // Head-based instructions...

  bool Inst_SetHead(cAvidaContext& ctx);
  bool Inst_AdvanceHead(cAvidaContext& ctx);
  bool Inst_MoveHead(cAvidaContext& ctx);
  bool Inst_ResMoveHead(cAvidaContext& ctx);
  bool Inst_JumpHead(cAvidaContext& ctx);
  bool Inst_ResJumpHead(cAvidaContext& ctx);
  bool Inst_GetHead(cAvidaContext& ctx);
  bool Inst_IfLabel(cAvidaContext& ctx);
  bool Inst_IfLabelDirect(cAvidaContext& ctx);
  bool Inst_IfLabel2(cAvidaContext& ctx);
  bool Inst_HeadDivide(cAvidaContext& ctx);
  bool Inst_HeadDivideRS(cAvidaContext& ctx); //AWC 06/29/06
  bool Inst_HeadDivide1RS(cAvidaContext& ctx); //AWC 07/28/06
  bool Inst_HeadDivide2RS(cAvidaContext& ctx); //AWC 08/29/06
  bool Inst_HeadRead(cAvidaContext& ctx);
  bool Inst_HeadWrite(cAvidaContext& ctx);
  bool Inst_HeadCopy(cAvidaContext& ctx);
  bool Inst_HeadSearch(cAvidaContext& ctx);
  bool Inst_HeadSearchDirect(cAvidaContext& ctx);
  bool Inst_SetFlow(cAvidaContext& ctx);

  bool Inst_HeadCopy_ifResource(cAvidaContext& ctx);
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

  bool Inst_IfEnergyLow(cAvidaContext& ctx);
  bool Inst_IfEnergyNotLow(cAvidaContext& ctx);
  bool Inst_IfFacedEnergyLow(cAvidaContext& ctx);
  bool Inst_IfFacedEnergyNotLow(cAvidaContext& ctx);
  bool Inst_IfEnergyHigh(cAvidaContext& ctx);
  bool Inst_IfEnergyNotHigh(cAvidaContext& ctx);
  bool Inst_IfFacedEnergyHigh(cAvidaContext& ctx);
  bool Inst_IfFacedEnergyNotHigh(cAvidaContext& ctx);
  bool Inst_IfEnergyMed(cAvidaContext& ctx);
  bool Inst_IfFacedEnergyMed(cAvidaContext& ctx);
  bool Inst_IfFacedEnergyMore(cAvidaContext& ctx);
  bool Inst_IfFacedEnergyLess(cAvidaContext& ctx);
  bool Inst_IfEnergyInBuffer(cAvidaContext& ctx);
  bool Inst_IfEnergyNotInBuffer(cAvidaContext& ctx);
  bool Inst_GetEnergyLevel(cAvidaContext& ctx);
  bool Inst_GetFacedEnergyLevel(cAvidaContext& ctx);
  bool Inst_IfFacedEnergyRequestOn(cAvidaContext& ctx);
  bool Inst_IfFacedEnergyRequestOff(cAvidaContext& ctx);
  bool Inst_GetEnergyRequestStatus(cAvidaContext& ctx);
  bool Inst_GetFacedEnergyRequestStatus(cAvidaContext& ctx);



  bool Inst_Sleep(cAvidaContext& ctx);
  bool Inst_GetUpdate(cAvidaContext& ctx);

  //// Promoter Model ////
  bool Inst_Promoter(cAvidaContext& ctx);
  bool Inst_Terminate(cAvidaContext& ctx);
  bool Inst_Regulate(cAvidaContext& ctx);
  bool Inst_RegulateSpecificPromoters(cAvidaContext& ctx);
  bool Inst_SenseRegulate(cAvidaContext& ctx);
  bool Inst_Numberate(cAvidaContext& ctx) { return Do_Numberate(ctx); };
  bool Inst_Numberate24(cAvidaContext& ctx) { return Do_Numberate(ctx, 24); };
  bool Do_Numberate(cAvidaContext& ctx, int num_bits=0);

  // Helper functions //
  bool IsActivePromoter();
  void NextPromoter();
  int  Numberate(int _pos, int _dir, int _num_bits = 0);

  //// Bit consensus functions ////
  inline unsigned int BitCount(unsigned int value) const;
  bool Inst_BitConsensus(cAvidaContext& ctx);
  bool Inst_BitConsensus24(cAvidaContext& ctx);
  bool Inst_IfConsensus(cAvidaContext& ctx);
  bool Inst_IfConsensus24(cAvidaContext& ctx);  
  bool Inst_IfLessConsensus(cAvidaContext& ctx);
  bool Inst_IfLessConsensus24(cAvidaContext& ctx);

  // Bit masking instructions
  bool Inst_MaskSignBit(cAvidaContext& ctx);
  bool Inst_MaskOffLower16Bits(cAvidaContext& ctx);
  bool Inst_MaskOffLower16Bits_defaultAX(cAvidaContext& ctx);
  bool Inst_MaskOffLower15Bits(cAvidaContext& ctx);
  bool Inst_MaskOffLower15Bits_defaultAX(cAvidaContext& ctx);
  bool Inst_MaskOffLower14Bits(cAvidaContext& ctx);
  bool Inst_MaskOffLower14Bits_defaultAX(cAvidaContext& ctx);
  bool Inst_MaskOffLower13Bits(cAvidaContext& ctx);
  bool Inst_MaskOffLower13Bits_defaultAX(cAvidaContext& ctx);
  bool Inst_MaskOffLower12Bits(cAvidaContext& ctx);
  bool Inst_MaskOffLower12Bits_defaultAX(cAvidaContext& ctx);
  bool Inst_MaskOffLower8Bits(cAvidaContext& ctx);
  bool Inst_MaskOffLower8Bits_defaultAX(cAvidaContext& ctx);
  bool Inst_MaskOffLower4Bits(cAvidaContext& ctx);
  bool Inst_MaskOffLower4Bits_defaultAX(cAvidaContext& ctx);

  //// Messaging ////
  bool Inst_SendMessage(cAvidaContext& ctx);
  bool SendMessage(cAvidaContext& ctx, int messageType = 0);
  bool Inst_RetrieveMessage(cAvidaContext& ctx);
  bool BroadcastX(cAvidaContext& ctx, int depth);
  bool Inst_Broadcast1(cAvidaContext& ctx);
  bool Inst_Broadcast2(cAvidaContext& ctx);
  bool Inst_Broadcast4(cAvidaContext& ctx);
  bool Inst_Broadcast8(cAvidaContext& ctx);

  // Active messaging //
  bool Inst_SendMessageInterruptType0(cAvidaContext& ctx);
  bool Inst_SendMessageInterruptType1(cAvidaContext& ctx);
  bool Inst_SendMessageInterruptType2(cAvidaContext& ctx);
  bool Inst_SendMessageInterruptType3(cAvidaContext& ctx);
  bool Inst_SendMessageInterruptType4(cAvidaContext& ctx);
  bool Inst_SendMessageInterruptType5(cAvidaContext& ctx);
  bool Inst_START_Handler(cAvidaContext& ctx);
  bool Inst_End_Handler(cAvidaContext& ctx);

  //// Alarm ////
  bool Inst_Alarm_MSG_local(cAvidaContext& ctx);
  bool Inst_Alarm_MSG_multihop(cAvidaContext& ctx);
  bool Inst_Alarm_MSG_Bit_Cons24_local(cAvidaContext& ctx);
  bool Inst_Alarm_MSG_Bit_Cons24_multihop(cAvidaContext& ctx);
  bool Inst_Alarm_Label(cAvidaContext& ctx);
  bool Jump_To_Alarm_Label(int jump_label);


  // -------- Reputation support --------
  /* These instructions interact with the "reputation" support in cOrganism.h.  They 
  are  based on the donate instructions. However, these instructions donate
  "raw materials" rather than merit and will eventually be used to support 
  reputation based cooperation.
  */ 
  // Donate a raw material to the neighbor
  bool Inst_DonateFacingRawMaterials(cAvidaContext& ctx);
  // Donate a raw material to the neighbor if it is another species
  bool Inst_DonateFacingRawMaterialsOtherSpecies(cAvidaContext& ctx);
  // Donate a raw material to the neighbor if it was a prior donor
  bool Inst_DonateIfDonor(cAvidaContext& ctx);	
  // Donate a string to the neighbor, if it's reputation is > 0
  bool Inst_DonateStringIfDonorRep(cAvidaContext& ctx);		
  // Donate a string to a neighbor
  bool Inst_DonateFacingString(cAvidaContext& ctx);

  // Rotate to the organims with the greatest reputation
  bool Inst_RotateToGreatestReputation(cAvidaContext& ctx);	
  // Rotate to the organims with the greatest reputation that has a different tag
  bool Inst_RotateToGreatestReputationWithDifferentTag(cAvidaContext& ctx);	
  // Rotate to the organims with the greatest reputation that has a different tag
  bool Inst_RotateToGreatestReputationWithDifferentLineage(cAvidaContext& ctx);		
  // Rotate to an organim with a different tag
  bool Inst_RotateToDifferentTag(cAvidaContext& ctx);	
  // Rotate to the organims with the greatest reputation and donate
  bool Inst_RotateToGreatestReputationAndDonate(cAvidaContext& ctx);	
  // Get a neighbor's reputation
  bool Inst_GetNeighborsReputation(cAvidaContext& ctx);
  // Get the organism's reputation
  bool Inst_GetReputation(cAvidaContext& ctx);	
  // Execute the following instruction if the facing neighbor was a donor
  bool Inst_IfDonor(cAvidaContext& ctx);
  // Produce string
  bool Inst_ProduceString(cAvidaContext& ctx);
  // Get the organism's raw material level
  bool Inst_GetAmountOfRawMaterials(cAvidaContext& ctx);
  // Get the number of raw materials the organism 
  // has gotten from others
  bool Inst_GetAmountOfOtherRawMaterials(cAvidaContext& ctx);	
  // Pretend to donate
  bool Inst_Pose(cAvidaContext& ctx);

  // Reputation
  void ComputeReputation();


  //// Placebo ////
  bool Inst_Skip(cAvidaContext& ctx);

  // @BDC Additions for pheromones
  bool Inst_PheroOn(cAvidaContext& ctx);
  bool Inst_PheroOff(cAvidaContext& ctx);
  bool Inst_PheroToggle(cAvidaContext& ctx);
  bool DoSenseFacing(cAvidaContext& ctx, int conversion_method, double base);
  bool Inst_SenseLog2Facing(cAvidaContext& ctx);
  bool Inst_SenseUnitFacing(cAvidaContext& ctx);
  bool Inst_SenseMult100Facing(cAvidaContext& ctx);
  bool Inst_SenseTarget(cAvidaContext& ctx);
  bool Inst_SenseTargetFaced(cAvidaContext& ctx);
  bool DoSensePheromone(cAvidaContext& ctx, int cellid);
  bool DoSensePheromoneInDemeGlobal(cAvidaContext& ctx, tRegisters REG_DEFAULT);
  bool DoSensePheromoneGlobal(cAvidaContext& ctx, tRegisters REG_DEFAULT);
  bool Inst_SensePheromone(cAvidaContext& ctx);
  bool Inst_SensePheromoneFaced(cAvidaContext& ctx);
  bool Inst_SensePheromoneInDemeGlobal(cAvidaContext& ctx);
  bool Inst_SensePheromoneGlobal(cAvidaContext& ctx);
  bool Inst_SensePheromoneGlobal_defaultAX(cAvidaContext& ctx);
  bool Inst_Exploit(cAvidaContext& ctx);
  bool Inst_ExploitForward5(cAvidaContext& ctx);
  bool Inst_ExploitForward3(cAvidaContext& ctx);
  bool Inst_Explore(cAvidaContext& ctx);
  bool Inst_MoveTarget(cAvidaContext& ctx);
  bool Inst_MoveTargetForward5(cAvidaContext& ctx);
  bool Inst_MoveTargetForward3(cAvidaContext& ctx);
  bool Inst_SuperMove(cAvidaContext& ctx);
  bool Inst_IfTarget(cAvidaContext& ctx);
  bool Inst_IfNotTarget(cAvidaContext& ctx);
  bool Inst_IfPheromone(cAvidaContext& ctx);
  bool Inst_IfNotPheromone(cAvidaContext& ctx);
  bool Inst_DropPheromone(cAvidaContext& ctx);

  // -------- Opinion support --------
public:
  /* These instructions interact with the "opinion" support in cOrganism.h.  The
  idea is that we're enabling organisms to express an opinion about *something*,
  where that something is defined by the particular tasks and/or (deme) fitness function
  in use.  This may have to be extended in the future to support different kinds of
  opinions that can be expressed during the same experiment, and possibly augmented
  with a "strength" of that opinion (but not right now).
  */
  bool Inst_SetOpinion(cAvidaContext& ctx);
  bool Inst_GetOpinion(cAvidaContext& ctx);
  //! Only get opinion.  If none then reg is set to zero
  bool Inst_GetOpinionOnly_ZeroIfNone(cAvidaContext& ctx);
  //! Clear this organism's current opinion.
  bool Inst_ClearOpinion(cAvidaContext& ctx);
  //! Execute next instruction is org has an opinion, otherwise skip
  bool Inst_IfOpinionSet(cAvidaContext& ctx);
  bool Inst_IfOpinionNotSet(cAvidaContext& ctx);
  bool Inst_SetOpinionToZero(cAvidaContext& ctx);
  bool Inst_SetOpinionToOne(cAvidaContext& ctx);
  bool Inst_SetOpinionToTwo(cAvidaContext& ctx);
  

  // -------- Cell Data Support --------
public:
  //! Collect this cell's data, and place it in a register.
  bool Inst_CollectCellData(cAvidaContext& ctx);
  //! Detect if this cell's data has changed since the last collection.
  bool Inst_IfCellDataChanged(cAvidaContext& ctx);
  bool Inst_KillCellEvent(cAvidaContext& ctx);
  bool Inst_KillFacedCellEvent(cAvidaContext& ctx);
  bool Inst_CollectCellDataAndKillEvent(cAvidaContext& ctx);
  bool Inst_ReadCellData(cAvidaContext& ctx);
  bool Inst_ReadFacedCellData(cAvidaContext& ctx);
  bool Inst_ReadFacedCellDataOrgID(cAvidaContext& ctx);
  bool Inst_ReadFacedCellDataFreshness(cAvidaContext& ctx);
  bool Inst_MarkCellWithID(cAvidaContext& ctx);
  bool Inst_MarkCellWithVitality(cAvidaContext& ctx);
  bool Inst_GetResStored(cAvidaContext& ctx);
  bool Inst_GetID(cAvidaContext& ctx);
  bool Inst_GetFacedVitalityDiff(cAvidaContext& ctx); 
  bool Inst_GetFacedOrgID(cAvidaContext& ctx);  
  bool Inst_AttackFacedOrg(cAvidaContext& ctx); 
  bool Inst_GetAttackOdds(cAvidaContext& ctx);

private:
  std::pair<bool, int> m_last_cell_data; //<! If cell data has been previously collected, and it's value.

  // -------- Synchronization primitives --------
public:
  //! Called when the owning organism receives a flash from a neighbor.
  virtual void ReceiveFlash();
  //! Sends a "flash" to all neighboring organisms.
  bool Inst_Flash(cAvidaContext& ctx);
  //! Test if this organism has ever received a flash.
  bool Inst_IfRecvdFlash(cAvidaContext& ctx);
  //! Get if & when this organism last received a flash.
  bool Inst_FlashInfo(cAvidaContext& ctx);
  //! Get if (but not when) this organism last received a flash.
  bool Inst_FlashInfoB(cAvidaContext& ctx);  
  //! Reset the information this organism has regarding receiving a flash.
  bool Inst_ResetFlashInfo(cAvidaContext& ctx);  
  //! Reset the entire CPU.
  bool Inst_HardReset(cAvidaContext& ctx);
  //! Current "time": the number of cycles this CPU has been "alive."
  bool Inst_GetCycles(cAvidaContext& ctx);

private:
  /*! Used to track the last flash received; first=whether we've received a flash, 
  second= #cycles since we've received a flash, or 0 if we haven't. */
  std::pair<unsigned int, unsigned int> m_flash_info;
  //! Cycle timer; counts the number of cycles this virtual CPU has executed.
  unsigned int m_cycle_counter;	

  // -------- Neighborhood-sensing support --------
public:	
  //! Loads the current neighborhood into the organism's memory.
  bool Inst_GetNeighborhood(cAvidaContext& ctx);
  //! Test if the current neighborhood has changed from that in the organism's memory.
  bool Inst_IfNeighborhoodChanged(cAvidaContext& ctx);


  // -------- Group Formation Support --------
public:
  //! An organism joins a group by setting it opinion to the group id. 
  bool Inst_JoinGroup(cAvidaContext& ctx);
  bool Inst_JoinMTGroup(cAvidaContext& ctx);
  // Organism joins group +1 or -1 wrapping from the top group back to group 1 (skipping 0)
  // based on whether the nop register is positive or negative.
  bool Inst_JoinNextGroup(cAvidaContext& ctx);
  bool Inst_JoinNextMTGroup(cAvidaContext& ctx);
  //! Returns the number of organisms in the current organism's group
  bool Inst_NumberOrgsInMyGroup(cAvidaContext& ctx);
  bool Inst_NumberMTInMyGroup(cAvidaContext& ctx);
  //! Returns the number of organisms in the current organism's group
  bool Inst_NumberOrgsInGroup(cAvidaContext& ctx);
  bool Inst_NumberMTInGroup(cAvidaContext& ctx);
  // Places in BX register, the number of organisms in the group +1 or -1, wrapping from the top back to group 1
  // skipping 0, based on whether the nop register is positive or negative.
  bool Inst_NumberNextGroup(cAvidaContext& ctx);
  bool Inst_NumberMTNextGroup(cAvidaContext& ctx);
  bool Inst_KillGroupMember(cAvidaContext& ctx);

  bool Inst_IncTolerance(cAvidaContext& ctx);
  bool Inst_DecTolerance(cAvidaContext& ctx);
  bool Inst_GetTolerance(cAvidaContext& ctx);
  bool Inst_GetGroupTolerance(cAvidaContext& ctx);

  // -------- Network creation support --------
public:
  //! Create a link to the currently-faced cell.
  bool Inst_CreateLinkByFacing(cAvidaContext& ctx);
  //! Create a link to the cell specified by xy-coordinates.
  bool Inst_CreateLinkByXY(cAvidaContext& ctx);
  //! Create a link to the cell specified by index.
  bool Inst_CreateLinkByIndex(cAvidaContext& ctx);
  //! Broadcast a message in the communication network.
  bool Inst_NetworkBroadcast1(cAvidaContext& ctx);
  //! Unicast a message in the communication network.
  bool Inst_NetworkUnicast(cAvidaContext& ctx);
  //! Rotate the current active link by the contents of register ?BX?.
  bool Inst_NetworkRotate(cAvidaContext& ctx);
  //! Select the current active link from the contents of register ?BX?.
  bool Inst_NetworkSelect(cAvidaContext& ctx);


  // -------- Division of labor support --------
  bool Inst_GetTimeUsed(cAvidaContext& ctx);
  bool Inst_DonateResToDeme(cAvidaContext& ctx);
  // If there is a penalty for switching tasks, call this function and 
  // the additional cycle cost will be added.
  void IncrementTaskSwitchingCost(int cost);
  int GetTaskSwitchingCost() { return m_task_switching_cost; }
  // Apply point mutations to a genome.
  bool Inst_ApplyPointMutations(cAvidaContext& ctx);
  // Apply point mutations to a genome, where the mutation rate
  // depends on the task last performed
  bool Inst_ApplyVaryingPointMutations(cAvidaContext& ctx);
  // Apply point mutations to a genome in the deme with the same 
  // germ/soma status
  bool Inst_ApplyPointMutationsGroupGS(cAvidaContext& ctx);
  // Apply point mutations to a genome in the deme at random
  bool Inst_ApplyPointMutationsGroupRandom(cAvidaContext& ctx);

  bool Inst_JoinGermline(cAvidaContext& ctx);
  bool Inst_ExitGermline(cAvidaContext& ctx);
  bool Inst_RepairPointMutOn(cAvidaContext& ctx);
  bool Inst_RepairPointMutOff(cAvidaContext& ctx);


  // -------- Mating types support support --------
public:
  bool Inst_SetMatingTypeMale(cAvidaContext& ctx);
  bool Inst_SetMatingTypeFemale(cAvidaContext& ctx);
  bool Inst_SetMatingTypeJuvenile(cAvidaContext& ctx);
  bool Inst_DivideSexMatingType(cAvidaContext& ctx);
  bool Inst_IfMatingTypeMale(cAvidaContext& ctx);
  bool Inst_IfMatingTypeFemale(cAvidaContext& ctx);
  bool Inst_IfMatingTypeJuvenile(cAvidaContext& ctx);
  bool Inst_IncrementMatingDisplayA(cAvidaContext& ctx);
  bool Inst_IncrementMatingDisplayB(cAvidaContext& ctx);
  bool Inst_SetMatingDisplayA(cAvidaContext& ctx);
  bool Inst_SetMatingDisplayB(cAvidaContext& ctx);
  bool Inst_SetMatePreference(cAvidaContext& ctx, int mate_pref);
  bool Inst_SetMatePreferenceRandom(cAvidaContext& ctx);
  bool Inst_SetMatePreferenceHighestDisplayA(cAvidaContext& ctx);
  bool Inst_SetMatePreferenceHighestDisplayB(cAvidaContext& ctx);
  bool Inst_SetMatePreferenceHighestMerit(cAvidaContext& ctx);
  bool Inst_SetMatePreferenceLowestDisplayA(cAvidaContext& ctx);
  bool Inst_SetMatePreferenceLowestDisplayB(cAvidaContext& ctx);
  bool Inst_SetMatePreferenceLowestMerit(cAvidaContext& ctx);
};


inline void cHardwareCPU::ThreadPrev()
{
  if (m_cur_thread == 0) m_cur_thread = m_threads.GetSize() - 1;
  else m_cur_thread--;
}

inline void cHardwareCPU::StackPush(int value)
{
  if (m_threads[m_cur_thread].cur_stack == 0) {
    m_threads[m_cur_thread].stack.Push(value);
  } else {
    m_global_stack.Push(value);
  }
}

inline int cHardwareCPU::StackPop()
{
  int pop_value;

  if (m_threads[m_cur_thread].cur_stack == 0) {
    pop_value = m_threads[m_cur_thread].stack.Pop();
  } else {
    pop_value = m_global_stack.Pop();
  }

  return pop_value;
}

inline void cHardwareCPU::StackFlip()
{
  if (m_threads[m_cur_thread].cur_stack == 0) {
    m_threads[m_cur_thread].stack.Flip();
  } else {
    m_global_stack.Flip();
  }
}

inline int cHardwareCPU::GetStack(int depth, int stack_id, int in_thread) const
{
  int value = 0;

  if(in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;

  if (stack_id == -1) stack_id = m_threads[in_thread].cur_stack;

  if (stack_id == 0) value = m_threads[in_thread].stack.Get(depth);
  else if (stack_id == 1) value = m_global_stack.Get(depth);

  return value;
}

inline int cHardwareCPU::GetCurStack(int in_thread) const
{
  if(in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;
  
  return m_threads[in_thread].cur_stack;  
}


inline void cHardwareCPU::StackClear()
{
  if (m_threads[m_cur_thread].cur_stack == 0) {
    m_threads[m_cur_thread].stack.Clear();
  } else {
    m_global_stack.Clear();
  }
}

inline void cHardwareCPU::SwitchStack()
{
  m_threads[m_cur_thread].cur_stack++;
  if (m_threads[m_cur_thread].cur_stack > 1) m_threads[m_cur_thread].cur_stack = 0;
}

#endif


