/*
 *  cHardwareGX.h
 *  Avida
 *
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
 * look for exact matches only and they have equal probabilities @JEB
 *
 * \todo We need to abstract cOrganism and derive a new class
 * that can accommodate multiple genome fragments.
 * 
 */
#ifndef cHardwareGX_h
#define cHardwareGX_h

#include "Avida.h"

#include "cCodeLabel.h"
#include "cHeadCPU.h"
#include "cCPUMemory.h"
#include "cCPUStack.h"
#include "cHardwareBase.h"
#include "cString.h"
#include "cStats.h"
#include "tArray.h"
#include "tInstLib.h"
#include "nHardware.h"
#include "tBuffer.h"

#include <iomanip>
#include <vector>
#include <list>
#include <utility>

class cInstLib;
class cInstSet;
class cOrganism;

/*! Each organism may have a cHardwareGX structure that keeps track of the 
* current status of simulated hardware.  This particular CPU architecture is
* designed to explore the evolution of gene expression and also the effect
* of having persistent "protein-like" pieces of code that continually execute in parallel.
*/
class cHardwareGX : public cHardwareBase
{
public:
  typedef bool (cHardwareGX::*tMethod)(cAvidaContext& ctx); //!< Instruction type.

  static const int NUM_REGISTERS = 3; //!< Number of registers each cProgramid has.
  //! Number of heads each cProgramid has.
  static const int NUM_HEADS = nHardware::NUM_HEADS >= NUM_REGISTERS ? nHardware::NUM_HEADS : NUM_REGISTERS;
  static const int NUM_NOPS = 3; //!< Number of NOPS that cHardwareGX supports.
    
  // GX "options" that haven't graduated to the Config File
  
  // "Normal" Model

  static const int PROGRAMID_REPLACEMENT_METHOD = 0;
   //!< Controls what happens when we try to allocate a new cProgramid, but are up against the limit
    // 0 = Fail if no programids available
    // 1 = Replace the programid that has used the most cpu cycles
  static const double EXECUTABLE_COPY_PROCESSIVITY; // Set to 1.0 by default
  static const double READABLE_COPY_PROCESSIVITY;   // Set to 1.0 by default
  
  unsigned int m_last_unique_id_assigned; // Counter: so programids can be assigned unique IDs for tracking

  //! Enums for the different supported registers.
  enum tRegisters { REG_AX=0, REG_BX, REG_CX };
  
  class cProgramid; // pre-declaration.
  typedef cProgramid* programid_ptr; //!< It would be nice to change this to boost::shared_ptr.
  typedef std::vector<programid_ptr> programid_list; //!< Type for the list of cProgramids.
  
  //! cMatchSite holds a couple useful pointers for cProgramid::Match, Bind.  
  struct cMatchSite {
    cMatchSite() : m_programid(0), m_site(0) { }
    cProgramid* m_programid; //!< The programid matched against; 0 if not matched.
    int m_site; //!< Location in the cProgramid where a match occurred; 0 if not matched.
    cCodeLabel m_label; //!< The label that was matched against.
  };

  /*! cHeadProgramid is just cHeadCPU with a link back to the programid
  so that we can tell when a head is on the programid that owns it.
  */
  class cHeadProgramid : public cHeadCPU 
  {
  private:
    cProgramid* m_programid;
  public:
    cHeadProgramid(cHardwareBase* hw = NULL, int pos = 0, int ms = 0) : cHeadCPU(hw, pos, ms) , m_programid(NULL) {  };
    ~cHeadProgramid() { ; }
      
    void SetProgramid(cProgramid* _programid) { m_programid = _programid; }
    cProgramid* GetProgramid() { return m_programid; }
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
  class cProgramid {
  public:
    //! Constructs a cProgramid from a genome and CPU.
    cProgramid(const cSequence& genome, cHardwareGX* hardware);
    ~cProgramid() {}
    
    //! Returns whether and where this cProgramid matches the passed-in label.
    std::vector<cHardwareGX::cMatchSite> Sites(const cCodeLabel& label);
    //! Binds one of this cProgramid's heads to the passed-in match site.
    void Bind(nHardware::tHeads head, cMatchSite& site);
    //! Detaches this cProgramid's heads from bound cProgramids.
    void Detach();
    
    //! Removes regulation in implicit GX mode
    void RemoveRegulation();
    
    // Programids keep a count of the total number
    // of READ + WRITE heads of other programids that 
    // have been placed on them. They only execute
    // if free of other heads and also initialized as executable.
    void RemoveContactingHead(cHeadProgramid& head) {
      if(head.GetProgramid()->GetID() == m_id) return;
      assert(m_contacting_heads > 0);
      m_contacting_heads--; 
    }
    
    void AddContactingHead(cHeadProgramid& head) { 
      if(head.GetProgramid()->GetID() == m_id) return; 
      m_contacting_heads++; 
    }
    
    void ResetHeads();    
    void Reset();
    
    // Accessors
    bool GetExecute() { return m_executable && (m_contacting_heads == 0); }
    bool GetExecutable() { return m_executable; }
    bool GetBindable() { return m_bindable; }
    bool GetReadable() { return m_readable; }
    int  GetID() { return m_id; }
    tBuffer<int>& GetInputBuf() { return m_input_buf; }
    tBuffer<int>& GetOutputBuf() { return m_output_buf; }
    int  GetCPUCyclesUsed() { return m_cpu_cycles_used; }
    void ResetCPUCyclesUsed() { m_cpu_cycles_used = 0; }
    cInstruction GetInst(cString inst) { assert(m_gx_hardware); return m_gx_hardware->GetInstSet().GetInst(inst); }

    const cCPUMemory& GetMemory() const { return m_memory; }
    
    //! Append this programid's genome to the passed-in genome in linear format (includes tags).
    void AppendLinearGenome(cSequence& genome);

    //! Print this programid's genome, in linear format.
    void PrintGenome(std::ostream& out);
    
    cHeadProgramid& GetHead(int head_id) { return m_heads[head_id]; }
    
    // Assignment
    void SetExecutable(bool _executable) { m_executable = _executable; }
    void SetBindable(bool _bindable) { m_bindable = _bindable; }
    void SetReadable(bool _readable) { m_readable = _readable; }

    void IncCPUCyclesUsed() { m_cpu_cycles_used++; }
    
    cHardwareGX* m_gx_hardware;  //!< Back reference
    int m_id; //!< Each programid is cross-referenced to a memory space. 
              // The index in cHardwareGX::m_programids and cHeadCPU::GetMemSpace() must match up.
              // A programid also needs to be kept aware of its current index.
    int m_unique_id; // ID unique to this programid (per hardware)          
              
    int m_contacting_heads; //!< The number of read/write heads on this programid from other programids. 
    bool m_executable;  //!< Is this programid ever executable? Currently, a programid with head from another cProgramid on it is also stopped. 
    bool m_bindable; //!< Is this programid bindable, i.e. can other programids put their read heads on it?
    bool m_readable; //!< Is this programid readable?
    bool m_marked_for_death; //!< Is this programid marked for deletion?
    int m_cpu_cycles_used; //!< Number of cpu cycles this programid has used.

    bool m_copying_site; //! Are we in the middle of copying a "site" (which could cause termination)
    cCodeLabel m_copying_label; //! The current site label that we are copying
    cCodeLabel m_terminator_label; //!< The label that this cProgramid must traverse to disassociate.

    // Core variables maintained from previous incarnation as a thread
    cCodeLabel m_read_label; //!< ?
    cCodeLabel m_next_label; //!< ?
    cCPUMemory m_memory; //!< This cProgramid's genome fragment.
    cCPUStack m_stack; //!< This cProgramid's stack (no global stack).
    cHeadProgramid m_heads[NUM_HEADS]; //!< This cProgramid's heads.
    int m_regs[NUM_REGISTERS]; //!< This cProgramid's registers.
    
    int m_input_pointer;
    tBuffer<int> m_input_buf; //!< This programid's input buffer.
    tBuffer<int> m_output_buf; //!< This programid's output buffer.

  };
  
protected:
  static tInstLib<tMethod>* initInstLib(void); //!< Initialize the instruction library.
  static tInstLib<tMethod>* s_inst_slib; //!< Instruction library (method pointers for all instructions).

  programid_list m_programids; //!< The list of cProgramids.
  programid_ptr m_current; //!< The currently-executing cProgramid.
  
  // Implicit RNAP Model only
  cHeadProgramid m_promoter_update_head; //Promoter position that last executable programid was created from.
  tArray<double> m_promoter_states;
  tArray<double> m_promoter_rates; // CURRENT promoter rates. Regulation on top of default.
  tArray<double> m_promoter_default_rates; // Rates sans regulation
  tArray<int> m_promoter_occupied_sites; // Whether the site is blocked by a currently bound regulatory protein.
  double m_recycle_state;
  double m_promoter_sum;
  
  // --------  Member Variables  --------
  const tMethod* m_functions;

  // Flags...
  bool m_mal_active;         // Has an allocate occured since last divide?
  bool m_advance_ip;         // Should the IP advance after this instruction?
  bool m_executedmatchstrings;	// Have we already executed the match strings instruction?
  bool m_just_divided; // Did we just divide (in which case end execution of programids until next cycle).
  bool m_reset_inputs; // Flag to make it easy for instructions to reset all inputs (force task modularity).
  bool m_reset_heads;  // Flas to make it easy for instructions to reset heads back (force task modularity).

  bool SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst);
  
  // --------  Stack Manipulation...  --------
  inline void StackPush(int value) { assert(m_current); m_current->m_stack.Push(value); }
  inline int StackPop() { assert(m_current); return m_current->m_stack.Pop(); }
  inline void StackFlip() { assert(m_current); m_current->m_stack.Flip(); }
  inline void StackClear() { assert(m_current); m_current->m_stack.Clear(); }
  inline void SwitchStack() { }
  
  // --------  Head Manipulation (including IP)  --------
  void AdjustHeads();
  
  // --------  Label Manipulation  -------
  const cCodeLabel& GetLabel() const { assert(m_current); return m_current->m_next_label; }
  cCodeLabel& GetLabel() { assert(m_current); return m_current->m_next_label; }
  void ReadLabel(int max_size=nHardware::MAX_LABEL_SIZE);
  cHeadCPU FindLabel(int direction);
  int FindLabel_Forward(const cCodeLabel & search_label, const cSequence& search_genome, int pos);
  int FindLabel_Backward(const cCodeLabel & search_label, const cSequence& search_genome, int pos);
  cHeadCPU FindLabel(const cCodeLabel & in_label, int direction);
  const cCodeLabel& GetReadLabel() const { assert(m_current); return m_current->m_read_label; }
  cCodeLabel& GetReadLabel() { assert(m_current); return m_current->m_read_label; }

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
  

  void internalReset();
	void internalResetOnFailedDivide();
  
    
  int calcExecutedSize(const int parent_size);
  int calcCopiedSize(const int parent_size, const int child_size);
  bool Divide_Main(cAvidaContext& ctx);
  bool HeadCopy_ErrorCorrect(cAvidaContext& ctx, double reduction);
  void ReadInst(const int in_inst);

public:
  //! Main constructor for cHardwareGX; called from cHardwareManager for every organism.
  cHardwareGX(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set);
  virtual ~cHardwareGX(); //!< Destructor; removes all cProgramids.
    
  static tInstLib<tMethod>* GetInstLib() { return s_inst_slib; }
  static cString GetDefaultInstFilename() { return "instset-gx.cfg"; }

  bool SingleProcess(cAvidaContext& ctx, bool speculative = false);
  void ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst);

  
  // --------  Helper methods  --------
  int GetType() const { return HARDWARE_TYPE_CPU_GX; }  
  bool SupportsSpeculative() const { return false; }
  bool OK();
  void PrintStatus(std::ostream& fp);

  // --------  Stack Manipulation...  --------
  inline int GetStack(int depth=0, int stack_id=-1, int in_thread=-1) const { assert(m_current); return m_current->m_stack.Get(depth); }
  inline int GetNumStacks() const { return 2; }
  
  // --------  Head Manipulation (including IP)  --------
  const cHeadProgramid& GetHead(int head_id) const { assert(m_current); return m_current->m_heads[head_id]; }
  cHeadProgramid& GetHead(int head_id) { assert(m_current); return m_current->m_heads[head_id];}
  const cHeadProgramid& GetHead(int head_id, int thread) const { assert(m_current); return m_current->m_heads[head_id]; }
  cHeadProgramid& GetHead(int head_id, int thread) { assert(m_current); return m_current->m_heads[head_id];}
  int GetNumHeads() const { return NUM_HEADS; }
  
  const cHeadCPU& IP() const { assert(m_current); return m_current->m_heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP() { assert(m_current); return m_current->m_heads[nHardware::HEAD_IP]; }
  const cHeadCPU& IP(int thread) const { assert(m_current); return m_current->m_heads[nHardware::HEAD_IP]; }
  cHeadCPU& IP(int thread) { assert(m_current); return m_current->m_heads[nHardware::HEAD_IP]; }
  
  
  // --------  Memory Manipulation  --------
  //<! Each programid counts as a memory space.
  // Heads from one programid can end up on another,
  // so be careful to fix these when changing the programid list.
  const cCPUMemory& GetMemory() const { assert(m_current); return m_current->m_memory; }
  cCPUMemory& GetMemory() { assert(m_current); return m_current->m_memory; }
  int GetMemSize() const { assert(m_current); return m_current->m_memory.GetSize(); }
  const cCPUMemory& GetMemory(int value) const { return m_programids[value]->m_memory; }
  cCPUMemory& GetMemory(int value) { return m_programids[value]->m_memory; }
  int GetMemSize(int value) const { return m_programids[value]->m_memory.GetSize(); }
  int GetNumMemSpaces() const { return m_programids.size(); }
  
  
  // --------  Register Manipulation  --------
  int GetRegister(int reg_id) const { assert(m_current); return m_current->m_regs[reg_id]; }
  int& GetRegister(int reg_id) { assert(m_current); return m_current->m_regs[reg_id]; }
  int GetNumRegisters() const { return NUM_REGISTERS; }  
  
  // --------  Thread Manipuluation --------
  /* cHardwareGX does not support threads (at least, not as in other CPUs). */
  virtual bool ThreadSelect(const int thread_id) { return false; }
  virtual bool ThreadSelect(const cCodeLabel& in_label) { return false; }
  virtual void ThreadPrev() { ; }
  virtual void ThreadNext() { ; }
  virtual cBioUnit* ThreadGetOwner() { return m_organism; }
  
  virtual int GetNumThreads() const { return 1; }
  virtual int GetCurThread() const { return -1; }
  virtual int GetCurThreadID() const { return -1; }
 
  // interrupt current thread
  bool InterruptThread(int interruptType) { return false; }
  int GetThreadMessageTriggerType(int _index) { return -1; }

   // --------  Parasite Stuff  --------
  bool ParasiteInfectHost(cBioUnit* bu) { return false; }


  // --------  Input/Output Buffers  --------
  virtual tBuffer<int>& GetInputBuf() { return m_current->GetInputBuf(); }
  virtual tBuffer<int>& GetOutputBuf() { return m_current->GetOutputBuf(); }

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
  
  bool Inst_Repro(cAvidaContext& ctx);

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
  bool Inst_TaskIO_DecayBonus(cAvidaContext& ctx);
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
  bool Inst_DonateEditDist(cAvidaContext& ctx);
  bool Inst_DonateGreenBeardGene(cAvidaContext& ctx);
  bool Inst_DonateTrueGreenBeard(cAvidaContext& ctx);
  bool Inst_DonateThreshGreenBeard(cAvidaContext& ctx);
  bool Inst_DonateQuantaThreshGreenBeard(cAvidaContext& ctx);
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
  bool Inst_NewProgramid(cAvidaContext& ctx, bool executable, bool bindable, bool readable); //!< Allocate a new programid and place the write head there.
  bool Inst_NewExecutableProgramid(cAvidaContext& ctx) { return Inst_NewProgramid(ctx, true, false, false); } //!< Allocate a "protein". Cannot be bound or read.
  bool Inst_NewGenomeProgramid(cAvidaContext& ctx) { return Inst_NewProgramid(ctx, false, true, true); } //!< Allocate a "genomic" fragment. Cannot execute.
  
  bool Inst_Site(cAvidaContext& ctx); //!< A binding site (execution simply advances past label)
  bool Inst_Bind(cAvidaContext& ctx); //!< Attempt to match the currently executing cProgramid against other cProgramids.
  bool Inst_Bind2(cAvidaContext& ctx); //!< Attempt to locate two programids with the same site.
  bool Inst_IfBind(cAvidaContext& ctx); //!< Attempt to match the currently executing cProgramid against other cProgramids. Execute next inst if successful.
  bool Inst_IfBind2(cAvidaContext& ctx); //!< Attempt to match and bind two programids.
  bool Inst_NumSites(cAvidaContext& ctx); //!< Count the number of corresponding binding sites
  bool Inst_ProgramidCopy(cAvidaContext& ctx); //!< Like h-copy, but fails if read/write heads not on other programids and will not write over
  bool Inst_ProgramidDivide(cAvidaContext& ctx); //!< Like h-divide, 
  bool Inst_ProgramidImplicitAllocate(cAvidaContext& ctx);
  bool Inst_ProgramidImplicitDivide(cAvidaContext& ctx);
  bool Inst_EndProgramidExecution(cAvidaContext& ctx);
  
  bool Inst_Promoter(cAvidaContext& ctx);
  bool Inst_Terminator(cAvidaContext& ctx);
  bool Inst_HeadActivate(cAvidaContext& ctx);
  bool Inst_HeadRepress(cAvidaContext& ctx);

  //!< Add/Remove a new programid to/from the list and give it the proper index within the list so we keep track of memory spaces...
  void AddProgramid(programid_ptr programid);
  void RemoveProgramid(unsigned int remove_index);
  
  // Create executable programids in the implicit GX model
  void ProcessImplicitGeneExpression(int in_limit = -1);  
  void AdjustPromoterRates(); //Call after a change to occupied array to correctly update rates.
  int FindRegulatoryMatch(const cCodeLabel& label);
};

#endif
