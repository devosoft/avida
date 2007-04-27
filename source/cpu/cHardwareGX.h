/*
 *  cHardwareGX.h
 *  Avida
 *
 * cHardwareGX enables gene expression as follows:
 * 1) Unlike cHardware{CPU,SMT,TransSMT}, the genome is not directly 
 *    executed by this organism.  Instead, cHardwareGX enables portions of the
 *    genome to be transcribed into "programids," which are able to execute
 *    independently.
 * 2) The interaction between programids within cHardwareGX is based on
 *    pattern-matching different genome fragments.  Each programid is able to
 *    specify a "match" that will be probabilistically compared against other
 *    programids.  When (if) a match is found, those two programids "bind"
 *    together.  Different actions may be taken on bind, depending on the 
 *    type of match performed.
 *
 * \todo cHardwareGX is really not a new CPU architecture, but rather a way for
 *  CPUs to interact.  It's much easier, however, to start off by implementing a 
 *  new CPU, so that's what we're doing.  Eventually we'll need to revisit this.
 *
 * \todo There should be better ways for promoter regions to work.  Right now,
 *  we stop at the first one encountered.
 *
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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
#ifndef cHardwareGX_h
#define cHardwareGX_h

#include <iomanip>
#include <vector>
#include <utility>
#include "cCodeLabel.h"
#include "cHeadCPU.h"
#include "cCPUMemory.h"
#include "cCPUStack.h"
#include "cHardwareBase.h"
#include "cString.h"
#include "cStats.h"
#include "tArray.h"
#include "tInstLib.h"
#include "defs.h"
#include "nHardware.h"


class cInjectGenotype;
class cInstLib;
class cInstSet;
class cMutation;
class cOrganism;


/*! Each organism may have a cHardwareGX structure that keeps track of the 
* current status of simulated hardware.  This particular CPU architecture is
* designed to explore the evolution of gene expression.
*/
class cHardwareGX : public cHardwareBase
{
public:
  typedef bool (cHardwareGX::*tMethod)(cAvidaContext& ctx); //!< Instruction type.

  static const int NUM_REGISTERS = 3; //!< Number of registers each cProgramid has.
  //! Number of heads each cProgramid has.
  static const int NUM_HEADS = nHardware::NUM_HEADS >= NUM_REGISTERS ? nHardware::NUM_HEADS : NUM_REGISTERS;
  static const int NUM_NOPS = 3; //!< Number of NOPS that cHardwareGX supports.

  //! Enums for the different supported registers.
  enum tRegisters { REG_AX=0, REG_BX, REG_CX };
  
  struct cProgramid; // pre-declaration.
  typedef cProgramid* programid_ptr; //!< It would be nice to change this to boost::shared_ptr.
  typedef std::vector<programid_ptr> programid_list; //!< Type for the list of cProgramids.
  
  //! cMatchSite holds a couple useful pointers for cProgramid::Match, Bind.  
  struct cMatchSite {
    cMatchSite() : m_programid(0), m_site(0) { }
    cProgramid* m_programid; //!< The programid matched against; 0 if not matched.
    cInstruction* m_site; //!< Location in the cProgramid where a match occurred; 0 if not matched.
    cCodeLabel m_label; //!< The label that was matched against.
  };
  
  /*! cProgramid is the "heart" of the gene expression hardware.  It encapsulates
    the genome fragment that is used by both active and passive elements within
    this organism, and enables these fragments to match against, and bind to, each
    other depending on their particular instruction sequence.
    
    It is similar in spirit to a thread, but has certain operational characteristics
    that make it unique (e.g., matching, disassociation, and a self-contained genome
    fragment).
    
    \todo Need to rework cHeadCPU to not need a pointer to cHardwareBase.
    */
  struct cProgramid {
    //! Constructs a cProgramid from a genome and CPU.
    cProgramid(const cGenome& genome, cHardwareBase* cpu);
    //! Returns whether and where this cProgramid matches the passed-in label.
    std::pair<bool, cMatchSite> Matches(const cCodeLabel& label);
    //! Binds one of this cProgramid's heads to the passed-in match site.
    void Bind(nHardware::tHeads head, cMatchSite& site);
    //! Called when this cProgramid "falls off" the cProgramid it is bound to.
    void Disassociate();
    
    programid_ptr m_offspring; //!< An offspring of this cProgramid; may be null.
    cCodeLabel m_terminator; //!< The label that this cProgramid must traverse to disassociate.
    cCodeLabel m_readLabel; //!< ?
    cCodeLabel m_nextLabel; //!< ?
    cCPUMemory m_memory; //!< This cProgramid's genome fragment.
    cCPUStack m_stack; //!< This cProgramid's stack (no global stack).
    cHeadCPU m_heads[NUM_HEADS]; //!< This cProgramid's heads.
    int m_regs[NUM_REGISTERS]; //!< This cProgramid's registers.
  };
  

  
protected:
  static tInstLib<tMethod>* initInstLib(void); //!< Initialize the instruction library.
  static tInstLib<tMethod>* s_inst_slib; //!< Instruction library (method pointers for all instructions).

  programid_list m_programids; //!< The list of cProgramids.
  programid_ptr m_current; //!< The currently-executing cProgramid.

  
  // --------  Member Variables  --------
  const tMethod* m_functions;

  // Flags...
  bool m_mal_active;         // Has an allocate occured since last divide?
  bool m_advance_ip;         // Should the IP advance after this instruction?
  bool m_executedmatchstrings;	// Have we already executed the match strings instruction?

  // Instruction costs...
#if INSTRUCTION_COSTS
  tArray<int> inst_cost;
  tArray<int> inst_ft_cost;
#endif
  
  
  bool SingleProcess_PayCosts(cAvidaContext& ctx, const cInstruction& cur_inst);
  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst);
  
  // --------  Stack Manipulation...  --------
  inline void StackPush(int value) { m_current->m_stack.Push(value); }
  inline int StackPop() { return m_current->m_stack.Pop(); }
  inline void StackFlip() { m_current->m_stack.Flip(); }
  inline void StackClear() { m_current->m_stack.Clear(); }
  inline void SwitchStack() { }
  
  // --------  Head Manipulation (including IP)  --------
  void AdjustHeads();
  
  // --------  Label Manipulation  -------
  const cCodeLabel& GetLabel() const { return m_current->m_nextLabel; }
  cCodeLabel& GetLabel() { return m_current->m_nextLabel; }
  void ReadLabel(int max_size=nHardware::MAX_LABEL_SIZE);
  cHeadCPU FindLabel(int direction);
  int FindLabel_Forward(const cCodeLabel & search_label, const cGenome& search_genome, int pos);
  int FindLabel_Backward(const cCodeLabel & search_label, const cGenome& search_genome, int pos);
  cHeadCPU FindLabel(const cCodeLabel & in_label, int direction);
  const cCodeLabel& GetReadLabel() const { return m_current->m_readLabel; }
  cCodeLabel& GetReadLabel() { return m_current->m_readLabel; }

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
  
  int GetCopiedSize(const int parent_size, const int child_size);
  bool Divide_Main(cAvidaContext& ctx);
  void InjectCode(const cGenome& injection, const int line_num);
  bool HeadCopy_ErrorCorrect(cAvidaContext& ctx, double reduction);
  void ReadInst(const int in_inst);

public:
  //! Main constructor for cHardwareGX; called from cHardwareManager for every(?) organism.
  cHardwareGX(cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  virtual ~cHardwareGX(); //!< Destructor; removes all cProgramids.
    
  static tInstLib<tMethod>* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "instset-gx.cfg"; }

  void Reset();
  void SingleProcess(cAvidaContext& ctx);
  void ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst);

  
  // --------  Helper methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_GX; }  
  bool OK();
  void PrintStatus(std::ostream& fp);


  // --------  Stack Manipulation...  --------
  inline int GetStack(int depth=0, int stack_id=-1, int in_thread=-1) const { return m_current->m_stack.Get(depth); }
  inline int GetNumStacks() const { return 2; }
  
  // --------  Head Manipulation (including IP)  --------
  const cHeadCPU& GetHead(int head_id) const { return m_current->m_heads[head_id]; }
  cHeadCPU& GetHead(int head_id) { return m_current->m_heads[head_id];}
  const cHeadCPU& GetHead(int head_id, int thread) const { return m_current->m_heads[head_id]; }
  cHeadCPU& GetHead(int head_id, int thread) { return m_current->m_heads[head_id];}
  int GetNumHeads() const { return NUM_HEADS; }
  
  const cHeadCPU& IP() const { return m_current->m_heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP() { return m_current->m_heads[nHardware::HEAD_IP]; }
  const cHeadCPU& IP(int thread) const { return m_current->m_heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP(int thread) { return m_current->m_heads[nHardware::HEAD_IP]; }
  
  
  // --------  Memory Manipulation  --------
  const cCPUMemory& GetMemory() const { return m_current->m_memory; }
  cCPUMemory& GetMemory() { return m_current->m_memory; }
  const cCPUMemory& GetMemory(int value) const { return m_current->m_memory; }
  cCPUMemory& GetMemory(int value) { return m_current->m_memory; }
  int GetNumMemSpaces() const { return 1; }
  
  
  // --------  Register Manipulation  --------
  const int GetRegister(int reg_id) const { return m_current->m_regs[reg_id]; }
  int& GetRegister(int reg_id) { return m_current->m_regs[reg_id]; }
  int GetNumRegisters() const { return NUM_REGISTERS; }  
  
  // --------  Thread Manipuluation --------
  /* cHardwareGX does not support threads (at least, not as in other CPUs). */
  virtual bool ThreadSelect(const int thread_id) { return false; }
  virtual bool ThreadSelect(const cCodeLabel& in_label) { return false; }
  virtual void ThreadPrev() { }
  virtual void ThreadNext() { }
  virtual cInjectGenotype* ThreadGetOwner() { return 0; }
  virtual void ThreadSetOwner(cInjectGenotype* in_genotype) { }
  
  virtual int GetNumThreads() const { return -1; }
  virtual int GetCurThread() const { return -1; }
  virtual int GetCurThreadID() const { return -1; }
  
  bool InjectHost(const cCodeLabel& in_label, const cGenome& injection);
  
private:
  cHardwareGX& operator=(const cHardwareGX&); //!< Not implemented.
  cHardwareGX(const cHardwareGX&); //!< Not implemented.
  
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

//  bool Inst_JumpF(cAvidaContext& ctx);
//  bool Inst_JumpB(cAvidaContext& ctx);
  bool Inst_Call(cAvidaContext& ctx);
  bool Inst_Return(cAvidaContext& ctx);
  
  bool Inst_Throw(cAvidaContext& ctx);
  bool Inst_ThrowIf0(cAvidaContext& ctx);
  bool Inst_ThrowIfNot0(cAvidaContext& ctx);  
  bool Inst_Catch(cAvidaContext& ctx) { ReadLabel(); return true; };
 
  bool Inst_Goto(cAvidaContext& ctx);
  bool Inst_GotoIf0(cAvidaContext& ctx);
  bool Inst_GotoIfNot0(cAvidaContext& ctx);  
  bool Inst_Label(cAvidaContext& ctx) { ReadLabel(); return true; };
    
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
  bool Inst_CAlloc(cAvidaContext& ctx);
  bool Inst_MaxAlloc(cAvidaContext& ctx);
  bool Inst_Inject(cAvidaContext& ctx);
  bool Inst_InjectRand(cAvidaContext& ctx);

  bool Inst_SpawnDeme(cAvidaContext& ctx);
  bool Inst_Kazi(cAvidaContext& ctx);
  bool Inst_Kazi5(cAvidaContext& ctx);
  bool Inst_Die(cAvidaContext& ctx);

  // I/O and Sensory
  bool Inst_TaskGet(cAvidaContext& ctx);
  bool Inst_TaskGet2(cAvidaContext& ctx);
  bool Inst_TaskStackGet(cAvidaContext& ctx);
  bool Inst_TaskStackLoad(cAvidaContext& ctx);
  bool Inst_TaskPut(cAvidaContext& ctx);
  bool Inst_TaskPutResetInputs(cAvidaContext& ctx);
  bool Inst_TaskIO(cAvidaContext& ctx);
  bool Inst_TaskIO_Feedback(cAvidaContext& ctx);
  bool Inst_MatchStrings(cAvidaContext& ctx);
  bool Inst_Sell(cAvidaContext& ctx);
  bool Inst_Buy(cAvidaContext& ctx);
  bool Inst_Send(cAvidaContext& ctx);
  bool Inst_Receive(cAvidaContext& ctx);
  bool Inst_SenseLog2(cAvidaContext& ctx);
  bool Inst_SenseUnit(cAvidaContext& ctx);
  bool Inst_SenseMult100(cAvidaContext& ctx);
  bool DoSense(cAvidaContext& ctx, int conversion_method, double base);

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

  // Energy use
  bool Inst_ZeroEnergyUsed(cAvidaContext& ctx); 

  // Head-based instructions...
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

  //// Placebo ////
  bool Inst_Skip(cAvidaContext& ctx);
  
  // -= Gene expression instructions =-
  bool Inst_Match(cAvidaContext& ctx); //!< Attempt to match the currently executing cProgramid against other cProgramids.
  bool Inst_OnDisassociate(cAvidaContext& ctx); //!< Called automatically when a cProgramid disassociates.
};


#ifdef ENABLE_UNIT_TESTS
namespace nHardwareGX {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif

#endif
