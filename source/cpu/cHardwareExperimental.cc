/*
 *  cHardwareExperimental.cc
 *  Avida
 *
 *  Created by David on 2/10/07 based on cHardwareCPU.cc
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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


#include "cHardwareExperimental.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cHardwareManager.h"
#include "cHardwareTracer.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cStateGrid.h"
#include "cStringUtil.h"
#include "cTestCPU.h"
#include "cWorldDriver.h"
#include "cWorld.h"
#include "tInstLibEntry.h"

#include <climits>
#include <fstream>

using namespace std;
using namespace AvidaTools;


static const unsigned int CONSENSUS = (sizeof(int) * 8) / 2;
static const unsigned int CONSENSUS24 = 12;

inline unsigned int cHardwareExperimental::BitCount(unsigned int value) const
{
  const unsigned int w = value - ((value >> 1) & 0x55555555);
  const unsigned int x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
  const unsigned int bit_count = ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
  return bit_count;
}


tInstLib<cHardwareExperimental::tMethod>* cHardwareExperimental::s_inst_slib = cHardwareExperimental::initInstLib();

tInstLib<cHardwareExperimental::tMethod>* cHardwareExperimental::initInstLib(void)
{
  struct cNOPEntry {
    cString name;
    int nop_mod;
    cNOPEntry(const cString &name, int nop_mod)
      : name(name), nop_mod(nop_mod) {}
  };
  static const cNOPEntry s_n_array[] = {
    cNOPEntry("nop-A", REG_AX),
    cNOPEntry("nop-B", REG_BX),
    cNOPEntry("nop-C", REG_CX),
    cNOPEntry("nop-D", REG_DX),

    cNOPEntry("nop-E", REG_EX),
    cNOPEntry("nop-F", REG_FX),
};
  
  static const tInstLibEntry<tMethod> s_f_array[] = {
    /*
     Note: all entries of cNOPEntryCPU s_n_array must have corresponding
     in the same order in tInstLibEntry<tMethod> s_f_array, and these entries must
     be the first elements of s_f_array.
     */
    tInstLibEntry<tMethod>("nop-A", &cHardwareExperimental::Inst_Nop, (nInstFlag::DEFAULT | nInstFlag::NOP), "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-B", &cHardwareExperimental::Inst_Nop, (nInstFlag::DEFAULT | nInstFlag::NOP), "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-C", &cHardwareExperimental::Inst_Nop, (nInstFlag::DEFAULT | nInstFlag::NOP), "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-D", &cHardwareExperimental::Inst_Nop, (nInstFlag::DEFAULT | nInstFlag::NOP), "No-operation; modifies other instructions"),

    tInstLibEntry<tMethod>("nop-E", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-F", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),

    tInstLibEntry<tMethod>("NULL", &cHardwareExperimental::Inst_Nop, 0, "True no-operation instruction: does nothing"),
    tInstLibEntry<tMethod>("nop-X", &cHardwareExperimental::Inst_Nop, 0, "True no-operation instruction: does nothing"),

    
    // Standard Conditionals
    tInstLibEntry<tMethod>("if-n-equ", &cHardwareExperimental::Inst_IfNEqu, nInstFlag::DEFAULT, "Execute next instruction if ?BX?!=?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-less", &cHardwareExperimental::Inst_IfLess, nInstFlag::DEFAULT, "Execute next instruction if ?BX? < ?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-not-0", &cHardwareExperimental::Inst_IfNotZero, 0, "Execute next instruction if ?BX? != 0, else skip it"),
    tInstLibEntry<tMethod>("if-equ-0", &cHardwareExperimental::Inst_IfEqualZero, 0, "Execute next instruction if ?BX? == 0, else skip it"),
    tInstLibEntry<tMethod>("if-gtr-0", &cHardwareExperimental::Inst_IfGreaterThanZero, 0, "Execute next instruction if ?BX? > 0, else skip it"),
    tInstLibEntry<tMethod>("if-less-0", &cHardwareExperimental::Inst_IfLessThanZero, 0, "Execute next instruction if ?BX? < 0, else skip it"),
    tInstLibEntry<tMethod>("if-gtr-X", &cHardwareExperimental::Inst_IfGtrX),
    tInstLibEntry<tMethod>("if-equ-X", &cHardwareExperimental::Inst_IfEquX),

    tInstLibEntry<tMethod>("if-cons", &cHardwareExperimental::Inst_IfConsensus, 0, "Execute next instruction if ?BX? in consensus, else skip it"),
    tInstLibEntry<tMethod>("if-cons-24", &cHardwareExperimental::Inst_IfConsensus24, 0, "Execute next instruction if ?BX[0:23]? in consensus , else skip it"),
    tInstLibEntry<tMethod>("if-less-cons", &cHardwareExperimental::Inst_IfLessConsensus, 0, "Execute next instruction if Count(?BX?) < Count(?CX?), else skip it"),
    tInstLibEntry<tMethod>("if-less-cons-24", &cHardwareExperimental::Inst_IfLessConsensus24, 0, "Execute next instruction if Count(?BX[0:23]?) < Count(?CX[0:23]?), else skip it"),

    tInstLibEntry<tMethod>("if-stk-gtr", &cHardwareExperimental::Inst_IfStackGreater, nInstFlag::DEFAULT, "Execute next instruction if the top of the current stack > inactive stack, else skip it"),

    // Core ALU Operations
    tInstLibEntry<tMethod>("pop", &cHardwareExperimental::Inst_Pop, nInstFlag::DEFAULT, "Remove top number from stack and place into ?BX?"),
    tInstLibEntry<tMethod>("push", &cHardwareExperimental::Inst_Push, nInstFlag::DEFAULT, "Copy number from ?BX? and place it into the stack"),
    tInstLibEntry<tMethod>("swap-stk", &cHardwareExperimental::Inst_SwitchStack, nInstFlag::DEFAULT, "Toggle which stack is currently being used"),
    tInstLibEntry<tMethod>("swap-stk-top", &cHardwareExperimental::Inst_SwapStackTop, nInstFlag::DEFAULT, "Swap the values at the top of both stacks"),
    tInstLibEntry<tMethod>("swap", &cHardwareExperimental::Inst_Swap, nInstFlag::DEFAULT, "Swap the contents of ?BX? with ?CX?"),
    
    tInstLibEntry<tMethod>("shift-r", &cHardwareExperimental::Inst_ShiftR, nInstFlag::DEFAULT, "Shift bits in ?BX? right by one (divide by two)"),
    tInstLibEntry<tMethod>("shift-l", &cHardwareExperimental::Inst_ShiftL, nInstFlag::DEFAULT, "Shift bits in ?BX? left by one (multiply by two)"),
    tInstLibEntry<tMethod>("inc", &cHardwareExperimental::Inst_Inc, nInstFlag::DEFAULT, "Increment ?BX? by one"),
    tInstLibEntry<tMethod>("dec", &cHardwareExperimental::Inst_Dec, nInstFlag::DEFAULT, "Decrement ?BX? by one"),

    tInstLibEntry<tMethod>("add", &cHardwareExperimental::Inst_Add, nInstFlag::DEFAULT, "Add BX to CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("sub", &cHardwareExperimental::Inst_Sub, nInstFlag::DEFAULT, "Subtract CX from BX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("nand", &cHardwareExperimental::Inst_Nand, nInstFlag::DEFAULT, "Nand BX by CX and place the result in ?BX?"),
    
    tInstLibEntry<tMethod>("IO", &cHardwareExperimental::Inst_TaskIO, (nInstFlag::DEFAULT | nInstFlag::STALL), "Output ?BX?, and input new number back into ?BX?"),
    tInstLibEntry<tMethod>("IO-expire", &cHardwareExperimental::Inst_TaskIOExpire, (nInstFlag::DEFAULT | nInstFlag::STALL), "Output ?BX?, and input new number back into ?BX?, if the number has not yet expired"),
    tInstLibEntry<tMethod>("input", &cHardwareExperimental::Inst_TaskInput, nInstFlag::STALL, "Input new number into ?BX?"),
    tInstLibEntry<tMethod>("output", &cHardwareExperimental::Inst_TaskOutput, nInstFlag::STALL, "Output ?BX?"),
    tInstLibEntry<tMethod>("output-zero", &cHardwareExperimental::Inst_TaskOutputZero, nInstFlag::STALL, "Output ?BX?"),
    tInstLibEntry<tMethod>("output-expire", &cHardwareExperimental::Inst_TaskOutputExpire, nInstFlag::STALL, "Output ?BX?, as long as the output has not yet expired"),
    
    tInstLibEntry<tMethod>("mult", &cHardwareExperimental::Inst_Mult, 0, "Multiple BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("div", &cHardwareExperimental::Inst_Div, 0, "Divide BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("mod", &cHardwareExperimental::Inst_Mod),
    
    
    // Flow Control Instructions
    tInstLibEntry<tMethod>("label", &cHardwareExperimental::Inst_Label, (nInstFlag::DEFAULT | nInstFlag::LABEL)),
    
    tInstLibEntry<tMethod>("search-s", &cHardwareExperimental::Inst_SearchS, nInstFlag::DEFAULT, "Find complement template from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-f", &cHardwareExperimental::Inst_SearchF, 0, "Find complement template forward and move the flow head"),
    tInstLibEntry<tMethod>("search-b", &cHardwareExperimental::Inst_SearchB, 0, "Find complement template backward and move the flow head"),
    tInstLibEntry<tMethod>("search-s-direct", &cHardwareExperimental::Inst_SearchS_Direct, 0, "Find direct template from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-f-direct", &cHardwareExperimental::Inst_SearchF_Direct, 0, "Find direct template forward and move the flow head"),
    tInstLibEntry<tMethod>("search-b-direct", &cHardwareExperimental::Inst_SearchB_Direct, 0, "Find direct template backward and move the flow head"),

    tInstLibEntry<tMethod>("mov-head", &cHardwareExperimental::Inst_MoveHead, nInstFlag::DEFAULT, "Move head ?IP? to the flow head"),
    tInstLibEntry<tMethod>("mov-head-if-n-equ", &cHardwareExperimental::Inst_MoveHeadIfNEqu, nInstFlag::DEFAULT, "Move head ?IP? to the flow head if ?BX? != ?CX?"),
    tInstLibEntry<tMethod>("mov-head-if-less", &cHardwareExperimental::Inst_MoveHeadIfLess, nInstFlag::DEFAULT, "Move head ?IP? to the flow head if ?BX? != ?CX?"),

    tInstLibEntry<tMethod>("goto", &cHardwareExperimental::Inst_Goto, 0, "Move IP to labeled position matching the label that follows"),
    tInstLibEntry<tMethod>("goto-if-n-equ", &cHardwareExperimental::Inst_GotoIfNEqu, 0, "Move IP to labeled position if BX != CX"),
    tInstLibEntry<tMethod>("goto-if-less", &cHardwareExperimental::Inst_GotoIfLess, 0, "Move IP to labeled position if BX < CX"),
    tInstLibEntry<tMethod>("goto-if-cons", &cHardwareExperimental::Inst_GotoConsensus, 0, "Move IP to the labeled position if BX consensus"), 
    tInstLibEntry<tMethod>("goto-if-cons-24", &cHardwareExperimental::Inst_GotoConsensus24, 0, "Move IP to the labeled position if BX consensus"),
    
    tInstLibEntry<tMethod>("jmp-head", &cHardwareExperimental::Inst_JumpHead, nInstFlag::DEFAULT, "Move head ?Flow? by amount in ?CX? register"),
    tInstLibEntry<tMethod>("get-head", &cHardwareExperimental::Inst_GetHead, nInstFlag::DEFAULT, "Copy the position of the ?IP? head into ?CX?"),
    
    
    
    // Replication Instructions
    tInstLibEntry<tMethod>("h-alloc", &cHardwareExperimental::Inst_HeadAlloc, nInstFlag::DEFAULT, "Allocate maximum allowed space"),
    tInstLibEntry<tMethod>("h-divide", &cHardwareExperimental::Inst_HeadDivide, (nInstFlag::DEFAULT | nInstFlag::STALL), "Divide code between read and write heads."),
    tInstLibEntry<tMethod>("h-divide-sex", &cHardwareExperimental::Inst_HeadDivideSex, (nInstFlag::DEFAULT | nInstFlag::STALL), "Divide code between read and write heads."),
    tInstLibEntry<tMethod>("h-copy", &cHardwareExperimental::Inst_HeadCopy, nInstFlag::DEFAULT, "Copy from read-head to write-head; advance both"),
    tInstLibEntry<tMethod>("h-copy-nolabel", &cHardwareExperimental::Inst_HeadCopy_NoLabel, 0, "Copy from read-head to write-head; advance both"),
    tInstLibEntry<tMethod>("if-label", &cHardwareExperimental::Inst_IfLabel, nInstFlag::DEFAULT, "Execute next if we copied complement of attached label"),
    tInstLibEntry<tMethod>("if-label-direct", &cHardwareExperimental::Inst_IfLabel_Direct, 0, "Execute next if we copied direct match of the attached label"),

    tInstLibEntry<tMethod>("h-read", &cHardwareExperimental::Inst_HeadRead, 0, "Read from the read-head, place into ?BX?, advance read-head"),
    tInstLibEntry<tMethod>("h-write", &cHardwareExperimental::Inst_HeadWrite, 0, "Write from ?BX? to the write head, advance write-head"),
    
    tInstLibEntry<tMethod>("repro", &cHardwareExperimental::Inst_Repro, nInstFlag::STALL, "Instantly reproduces the organism"),

    tInstLibEntry<tMethod>("die", &cHardwareExperimental::Inst_Die, nInstFlag::STALL, "Instantly kills the organism"),

    
    
    // Promoter Model
    tInstLibEntry<tMethod>("promoter", &cHardwareExperimental::Inst_Promoter, nInstFlag::PROMOTER),
    tInstLibEntry<tMethod>("terminate", &cHardwareExperimental::Inst_Terminate),
    tInstLibEntry<tMethod>("term-cons", &cHardwareExperimental::Inst_TerminateConsensus),
    tInstLibEntry<tMethod>("term-cons-24", &cHardwareExperimental::Inst_TerminateConsensus24),
    tInstLibEntry<tMethod>("regulate", &cHardwareExperimental::Inst_Regulate),
    tInstLibEntry<tMethod>("regulate-sp", &cHardwareExperimental::Inst_RegulateSpecificPromoters),
    tInstLibEntry<tMethod>("s-regulate", &cHardwareExperimental::Inst_SenseRegulate),
    tInstLibEntry<tMethod>("numberate", &cHardwareExperimental::Inst_Numberate),
    tInstLibEntry<tMethod>("numberate-24", &cHardwareExperimental::Inst_Numberate24),
    tInstLibEntry<tMethod>("bit-cons", &cHardwareExperimental::Inst_BitConsensus),
    tInstLibEntry<tMethod>("bit-cons-24", &cHardwareExperimental::Inst_BitConsensus24),
    tInstLibEntry<tMethod>("execurate", &cHardwareExperimental::Inst_Execurate),
    tInstLibEntry<tMethod>("execurate-24", &cHardwareExperimental::Inst_Execurate24),

    
    // State Grid instructions
    tInstLibEntry<tMethod>("sg-move", &cHardwareExperimental::Inst_SGMove),
    tInstLibEntry<tMethod>("sg-rotate-l", &cHardwareExperimental::Inst_SGRotateL),
    tInstLibEntry<tMethod>("sg-rotate-r", &cHardwareExperimental::Inst_SGRotateR),
    tInstLibEntry<tMethod>("sg-sense", &cHardwareExperimental::Inst_SGSense),

    
    // DEPRECATED Instructions
    tInstLibEntry<tMethod>("set-flow", &cHardwareExperimental::Inst_SetFlow, 0, "Set flow-head to position in ?CX?")
  };
  
  
  const int n_size = sizeof(s_n_array)/sizeof(cNOPEntry);
  
  static cString n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size && i < NUM_REGISTERS; i++) {
    n_names[i] = s_n_array[i].name;
    nop_mods[i] = s_n_array[i].nop_mod;
  }
  
  const int f_size = sizeof(s_f_array)/sizeof(tInstLibEntry<tMethod>);
  static tMethod functions[f_size];
  for (int i = 0; i < f_size; i++) functions[i] = s_f_array[i].GetFunction();

	const int def = 0;
  const int null_inst = 6;
  
  return new tInstLib<tMethod>(f_size, s_f_array, n_names, nop_mods, functions, def, null_inst);
}

cHardwareExperimental::cHardwareExperimental(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set)
  : cHardwareBase(world, in_organism, in_inst_set)
{
  m_functions = s_inst_slib->GetFunctions();
  
  m_spec_die = false;

  m_thread_slicing_parallel = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1);
  m_no_cpu_cycle_time = m_world->GetConfig().NO_CPU_CYCLE_TIME.Get();
  
  m_promoters_enabled = m_world->GetConfig().PROMOTERS_ENABLED.Get();
  if (m_promoters_enabled) {
    m_constitutive_regulation = m_world->GetConfig().CONSTITUTIVE_REGULATION.Get();
    m_no_active_promoter_halt = (m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get() == 2);
  }
  
  m_slip_read_head = !m_world->GetConfig().SLIP_COPY_MODE.Get();
  
  m_memory = in_organism->GetGenome().GetSequence();  // Initialize memory...
  Reset(ctx);                            // Setup the rest of the hardware...
}


void cHardwareExperimental::internalReset()
{
  m_cycle_count = 0;
  m_last_output = 0;
  m_global_stack.Clear();
  
  // We want to reset to have a single thread.
  m_threads.Resize(1);
  
  // Reset that single thread.
  m_threads[0].Reset(this, 0);
  m_thread_id_chart = 1; // Mark only the first thread as taken...
  m_cur_thread = 0;
  
  m_mal_active = false;
  m_executedmatchstrings = false;
  
  // Promoter model
  if (m_promoters_enabled) {
    m_promoter_index = -1; // Meaning the last promoter was nothing
    m_promoter_offset = 0;
    
    m_promoters.Resize(0);

    for (int i=0; i < m_memory.GetSize(); i++) {
      if (m_inst_set->IsPromoter(m_memory[i])) {
        int code = Numberate(i - 1, -1, m_world->GetConfig().PROMOTER_CODE_SIZE.Get());
        m_promoters.Push(cPromoter(i, code));
      }
    }
  }
  
  m_io_expire = m_world->GetConfig().IO_EXPIRE.Get();
}


void cHardwareExperimental::internalResetOnFailedDivide()
{
	internalReset();
}

void cHardwareExperimental::cLocalThread::Reset(cHardwareExperimental* in_hardware, int in_id)
{
  m_id = in_id;
  
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i].Clear();
  for (int i = 0; i < NUM_HEADS; i++) heads[i].Reset(in_hardware);
  
  stack.Clear();
  cur_stack = 0;
  cur_head = nHardware::HEAD_IP;
  
  reading = false;
  read_label.Clear();
  next_label.Clear();

  // Promoter model
  m_execurate = 0;
  m_promoter_inst_executed = 0;
}


// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.

bool cHardwareExperimental::SingleProcess(cAvidaContext& ctx, bool speculative)
{
  assert(!speculative || (speculative && !m_thread_slicing_parallel));
  
  // Mark this organism as running...
  m_organism->SetRunning(true);
  
  if (!speculative && m_spec_die) {
    m_organism->Die();
    m_organism->SetRunning(false);
    return false;
  }
  
  cPhenotype& phenotype = m_organism->GetPhenotype();

  // First instruction - check whether we should be starting at a promoter, when enabled.
  if (phenotype.GetCPUCyclesUsed() == 0 && m_promoters_enabled) PromoterTerminate(ctx);
  
  m_cycle_count++;
  assert(m_cycle_count < 0x8000);
  phenotype.IncCPUCyclesUsed();
  if (!m_no_cpu_cycle_time) phenotype.IncTimeUsed();

  const int num_threads = m_threads.GetSize();
  
  // If we have threads turned on and we executed each thread in a single
  // timestep, adjust the number of instructions executed accordingly.
  const int num_inst_exec = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1) ? num_threads : 1;
  
  for (int i = 0; i < num_inst_exec; i++) {
    // Setup the hardware for the next instruction to be executed.
    int last_thread = m_cur_thread++;
    if (m_cur_thread >= num_threads) m_cur_thread = 0;
    
    m_advance_ip = true;
    cHeadCPU& ip = m_threads[m_cur_thread].heads[nHardware::HEAD_IP];
    ip.Adjust();
    
#if BREAKPOINTS
    if (ip.FlagBreakpoint()) {
      m_organism->DoBreakpoint();
    }
#endif
    
    // Print the status of this CPU at each step...    
    if (m_tracer != NULL) m_tracer->TraceHardware(*this);
    
    // Find the instruction to be executed
    const cInstruction& cur_inst = ip.GetInst();
    
    if (speculative && (m_spec_die || m_inst_set->ShouldStall(cur_inst))) {
      // Speculative instruction reject, flush and return
      m_cur_thread = last_thread;
      phenotype.DecCPUCyclesUsed();
      if (!m_no_cpu_cycle_time) phenotype.IncTimeUsed(-1);
      m_organism->SetRunning(false);
      return false;
    }
    
    // Test if costs have been paid and it is okay to execute this now...
    bool exec = true;
    if (m_has_any_costs) exec = SingleProcess_PayPreCosts(ctx, cur_inst);

    if (m_promoters_enabled) {
      // Constitutive regulation applied here
      if (m_constitutive_regulation) Inst_SenseRegulate(ctx); 

      // If there are no active promoters and a certain mode is set, then don't execute any further instructions
      if (m_no_active_promoter_halt && m_promoter_index == -1) exec = false;
    }
    
    // Now execute the instruction...
    if (exec == true) {
      // NOTE: This call based on the cur_inst must occur prior to instruction
      //       execution, because this instruction reference may be invalid after
      //       certain classes of instructions (namely divide instructions) @DMB
      const int addl_time_cost = m_inst_set->GetAddlTimeCost(cur_inst);

      // Prob of exec (moved from SingleProcess_PayCosts so that we advance IP after a fail)
      if ( m_inst_set->GetProbFail(cur_inst) > 0.0 ) 
      {
        exec = !( ctx.GetRandom().P(m_inst_set->GetProbFail(cur_inst)) );
      }
      
      //Add to the promoter inst executed count before executing the inst (in case it is a terminator)
      if (m_promoters_enabled) m_threads[m_cur_thread].IncPromoterInstExecuted();
      
      if (exec == true) if (SingleProcess_ExecuteInst(ctx, cur_inst)) SingleProcess_PayPostCosts(ctx, cur_inst);
      
      // Some instruction (such as jump) may turn m_advance_ip off.  Usually
      // we now want to move to the next instruction in the memory.
      if (m_advance_ip == true) ip.Advance();
      
      // Pay the additional death_cost of the instruction now
      phenotype.IncTimeUsed(addl_time_cost);

      // In the promoter model, we may force termination after a certain number of inst have been executed
      if (m_promoters_enabled) {
        const double processivity = m_world->GetConfig().PROMOTER_PROCESSIVITY.Get();
        if (ctx.GetRandom().P(1 - processivity)) PromoterTerminate(ctx);
        if (m_world->GetConfig().PROMOTER_INST_MAX.Get() &&
            (m_threads[m_cur_thread].GetPromoterInstExecuted() >= m_world->GetConfig().PROMOTER_INST_MAX.Get())) {
          PromoterTerminate(ctx);
        }
      }
    } // if exec
    
  } // Previous was executed once for each thread...
  
  // Kill creatures who have reached their max num of instructions executed
  const int max_executed = m_organism->GetMaxExecuted();
  if ((max_executed > 0 && phenotype.GetTimeUsed() >= max_executed) || phenotype.GetToDie() == true) {
    if (speculative) m_spec_die = true;
    else m_organism->Die();
  }
  if (!speculative && phenotype.GetToDelete()) m_spec_die = true;
  
  m_organism->SetRunning(false);
  CheckImplicitRepro(ctx);
        
  return !m_spec_die;
}

// This method will handle the actuall execution of an instruction
// within single process, once that function has been finalized.
bool cHardwareExperimental::SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst) 
{
  // Copy Instruction locally to handle stochastic effects
  cInstruction actual_inst = cur_inst;
  
#ifdef EXECUTION_ERRORS
  // If there is an execution error, execute a random instruction.
  if (m_organism->TestExeErr()) actual_inst = m_inst_set->GetRandomInst(ctx);
#endif /* EXECUTION_ERRORS */
  
  // Get a pointer to the corrisponding method...
  int inst_idx = m_inst_set->GetLibFunctionIndex(actual_inst);
  
  // Mark the instruction as executed
  getIP().SetFlagExecuted();
	
  
#if INSTRUCTION_COUNT
  // instruction execution count incremeneted
  m_organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
#endif
  
  // And execute it.
  const bool exec_success = (this->*(m_functions[inst_idx]))(ctx);

	if (exec_success) {
    int code_len = m_world->GetConfig().INST_CODE_LENGTH.Get();
    m_threads[m_cur_thread].UpdateExecurate(code_len, m_inst_set->GetInstructionCode(actual_inst));
  }

#if INSTRUCTION_COUNT
  // decremenet if the instruction was not executed successfully
  if (exec_success == false) {
    m_organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  }
#endif	
  
  return exec_success;
}


void cHardwareExperimental::ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = m_organism->IsRunning();
  m_organism->SetRunning(true);
  
  if (m_tracer != NULL) m_tracer->TraceHardware(*this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  m_organism->SetRunning(prev_run_state);
}


bool cHardwareExperimental::OK()
{
  bool result = true;
  
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (m_threads[i].next_label.OK() == false) result = false;
  }
  
  return result;
}

void cHardwareExperimental::PrintStatus(ostream& fp)
{
  fp << m_organism->GetPhenotype().GetCPUCyclesUsed() << " ";
  fp << "IP:" << getIP().GetPosition() << "    ";
  
  
  for (int i = 0; i < NUM_REGISTERS; i++) {
    sInternalValue& reg = m_threads[m_cur_thread].reg[i];
    fp << static_cast<char>('A' + i) << "X:" << GetRegister(i) << " ";
    fp << setbase(16) << "[0x" << reg.value <<  "] " << setbase(10);
    fp << "(" << reg.from_env << " " << reg.env_component << " " << reg.originated << " " << reg.oldest_component << ")  ";
  }
  
  // Add some extra information if additional time costs are used for instructions,
  // leave this out if there are no differences to keep it cleaner
  if ( m_organism->GetPhenotype().GetTimeUsed() != m_organism->GetPhenotype().GetCPUCyclesUsed() )
  {
    fp << "  EnergyUsed:" << m_organism->GetPhenotype().GetTimeUsed();
  }
  fp << endl;
  
  fp << "  R-Head:" << getHead(nHardware::HEAD_READ).GetPosition() << " "
    << "W-Head:" << getHead(nHardware::HEAD_WRITE).GetPosition()  << " "
    << "F-Head:" << getHead(nHardware::HEAD_FLOW).GetPosition()   << "  "
    << "RL:" << GetReadLabel().AsString() << "   "
    << "Ex:" << m_last_output
    << endl;
    
  int number_of_stacks = GetNumStacks();
  for (int stack_id = 0; stack_id < number_of_stacks; stack_id++) {
    fp << ((m_threads[m_cur_thread].cur_stack == stack_id) ? '*' : ' ') << " Stack " << stack_id << ":" << setbase(16) << setfill('0');
    for (int i = 0; i < nHardware::STACK_SIZE; i++) fp << " Ox" << setw(8) << GetStack(i, stack_id, 0);
    fp << setfill(' ') << setbase(10) << endl;
  }
  
  fp << "  Mem (" << m_memory.GetSize() << "):"
		  << "  " << m_memory.AsString()
		  << endl;
  
  
  if (m_world->GetConfig().PROMOTERS_ENABLED.Get())
  {
    fp << "Promoters: index=" << m_promoter_index << " offset=" << m_promoter_offset;
    fp << " exe_inst=" << m_threads[m_cur_thread].GetPromoterInstExecuted();
    for (int i=0; i<m_promoters.GetSize(); i++)
    {
      fp << setfill(' ') << setbase(10) << m_promoters[i].pos << ":";
      fp << "Ox" << setbase(16) << setfill('0') << setw(8) << (m_promoters[i].GetRegulatedBitCode()) << " "; 
    }
    fp << endl;    
    fp << setfill(' ') << setbase(10) << endl;
  }    

  fp.flush();
}





cHeadCPU cHardwareExperimental::FindLabelStart(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;

  cCPUMemory& memory = m_memory;
  int pos = 0;
  
  while (pos < memory.GetSize()) {
    if (m_inst_set->IsLabel(memory[pos])) { // starting label found
      pos++;
      
      // Check for direct matched label pattern, can be substring of 'label'ed target
      // - must match all NOPs in search_label
      // - extra NOPs in 'label'ed target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos < memory.GetSize()) {
        if (!m_inst_set->IsNop(memory[pos]) || search_label[size_matched] != m_inst_set->GetNopMod(memory[pos])) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops following the 'label' instruction
      if (size_matched == search_label.GetSize()) {
        // Return Head pointed at last NOP of label sequence
        if (mark_executed) {
          size_matched++; // Increment size matched so that it includes the label instruction
          const int start = pos - size_matched;
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get() + 1; // Max label + 1 for the label instruction itself
          for (int i = 0; i < size_matched && i < max; i++) memory.SetFlagExecuted(start + i);
        }
        return cHeadCPU(this, pos - 1, ip.GetMemSpace());
      }
      
      continue; 
    }
    pos++;
  }
         
  // Return start point if not found
  return ip;
}

cHeadCPU cHardwareExperimental::FindNopSequenceStart(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cCPUMemory& memory = m_memory;
  int pos = 0;
  
  while (pos < memory.GetSize()) {
    if (m_inst_set->IsNop(memory[pos])) { // start of sequence found
      
      // Check for direct matched label pattern, can be substring of 'label'ed target
      // - must match all NOPs in search_label
      // - extra NOPs in 'label'ed target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos < memory.GetSize()) {
        if (!m_inst_set->IsNop(memory[pos]) || search_label[size_matched] != m_inst_set->GetNopMod(memory[pos])) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops following the 'label' instruction
      if (size_matched == search_label.GetSize()) {
        // Return Head pointed at last NOP of label sequence
        if (mark_executed) {
          const int start = pos - size_matched;
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get();
          for (int i = 0; i < size_matched && i < max; i++) memory.SetFlagExecuted(start + i);
        }
        return cHeadCPU(this, pos - 1, ip.GetMemSpace());
      }
    }
    pos++;
  }
  
  // Return start point if not found
  return ip;
}


cHeadCPU cHardwareExperimental::FindLabelForward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cHeadCPU pos(ip);
  pos++;
  
  while (pos.GetPosition() != ip.GetPosition()) {
    if (m_inst_set->IsLabel(pos.GetInst())) { // starting label found
      const int label_start = pos.GetPosition();
      pos++;
      
      // Check for direct matched label pattern, can be substring of 'label'ed target
      // - must match all NOPs in search_label
      // - extra NOPs in 'label'ed target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.GetPosition() != ip.GetPosition()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops following the 'label' instruction
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.GetPosition();

        if (mark_executed) {
          pos.Set(label_start);
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get() + 1; // Max label + 1 for the label instruction itself
          for (int i = 0; i < size_matched && i < max; i++, pos++) pos.SetFlagExecuted();
        }

        // Return Head pointed at last NOP of label sequence
        return cHeadCPU(this, found_pos, ip.GetMemSpace());
      }
      
      continue; 
    }
    pos++;
  }
  
  // Return start point if not found
  return ip;
}

cHeadCPU cHardwareExperimental::FindLabelBackward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cHeadCPU lpos(ip);
  cHeadCPU pos(ip);
  lpos--;
  
  while (pos.GetPosition() != ip.GetPosition()) {
    if (m_inst_set->IsLabel(lpos.GetInst())) { // starting label found
      pos.Set(lpos.GetPosition());
      pos++;
      
      // Check for direct matched label pattern, can be substring of 'label'ed target
      // - must match all NOPs in search_label
      // - extra NOPs in 'label'ed target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.GetPosition() != ip.GetPosition()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops following the 'label' instruction
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.GetPosition();
        
        if (mark_executed) {
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get() + 1; // Max label + 1 for the label instruction itself
          for (int i = 0; i < size_matched && i < max; i++, lpos++) lpos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        return cHeadCPU(this, found_pos, ip.GetMemSpace());
      }
    }
    lpos--;
  }
  
  // Return start point if not found
  return ip;
}




cHeadCPU cHardwareExperimental::FindNopSequenceForward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cHeadCPU pos(ip);
  pos++;
  
  while (pos.GetPosition() != ip.GetPosition()) {
    if (m_inst_set->IsNop(pos.GetInst())) { // starting label found
      const int label_start = pos.GetPosition();
      
      // Check for direct matched nop sequence, can be substring of target
      // - must match all NOPs in search_label
      // - extra NOPs in target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.GetPosition() != ip.GetPosition()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.GetPosition();
        
        if (mark_executed) {
          pos.Set(label_start);
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get();
          for (int i = 0; i < size_matched && i < max; i++, pos++) pos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        return cHeadCPU(this, found_pos, ip.GetMemSpace());
      }
      
      continue; 
    }
    pos++;
  }
  
  // Return start point if not found
  return ip;
}


void cHardwareExperimental::ReadInst(const int in_inst)
{
  if (m_inst_set->IsLabel(cInstruction(in_inst))) {
    GetReadLabel().Clear();
    ReadingLabel() = true;
  } else if (ReadingLabel() && m_inst_set->IsNop(cInstruction(in_inst))) {
    GetReadLabel().AddNop(in_inst);
  } else {
    GetReadLabel().Clear();
    ReadingLabel() = false;
  }
}

void cHardwareExperimental::ReadInst_NoLabel(const int in_inst)
{
  if (!ReadingLabel() && m_inst_set->IsNop(cInstruction(in_inst))) {
    GetReadLabel().AddNop(in_inst);
    ReadingLabel() = true;
  } else if (ReadingLabel() && m_inst_set->IsNop(cInstruction(in_inst))) {
    GetReadLabel().AddNop(in_inst);
  } else {
    GetReadLabel().Clear();
    ReadingLabel() = false;
  }
}


void cHardwareExperimental::AdjustHeads()
{
  for (int i = 0; i < m_threads.GetSize(); i++) {
    for (int j = 0; j < NUM_HEADS; j++) {
      m_threads[i].heads[j].Adjust();
    }
  }
}



// This function looks at the current position in the info of a creature,
// and sets the next_label to be the sequence of nops which follows.  The
// instruction pointer is left on the last line of the label found.

void cHardwareExperimental::ReadLabel(int max_size)
{
  int count = 0;
  cHeadCPU * inst_ptr = &( getIP() );
  
  GetLabel().Clear();
  
  while (m_inst_set->IsNop(inst_ptr->GetNextInst()) &&
         (count < max_size)) {
    count++;
    inst_ptr->Advance();
    GetLabel().AddNop(m_inst_set->GetNopMod(inst_ptr->GetInst()));
    
    // If this is the first line of the template, mark it executed.
    if (GetLabel().GetSize() <=	m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get()) {
      inst_ptr->SetFlagExecuted();
    }
  }
}


bool cHardwareExperimental::ForkThread()
{
  const int num_threads = m_threads.GetSize();
  if (num_threads == m_world->GetConfig().MAX_CPU_THREADS.Get()) return false;
  
  // Make room for the new thread.
  m_threads.Resize(num_threads + 1);
  
  // Initialize the new thread to the same values as the current one.
  m_threads[num_threads] = m_threads[m_cur_thread];
  
  // Find the first free bit in m_thread_id_chart to determine the new
  // thread id.
  int new_id = 0;
  while ( (m_thread_id_chart >> new_id) & 1 == 1) new_id++;
  m_threads[num_threads].SetID(new_id);
  m_thread_id_chart |= (1 << new_id);
  
  return true;
}


bool cHardwareExperimental::KillThread()
{
  // Make sure that there is always at least one thread...
  if (m_threads.GetSize() == 1) return false;
  
  // Note the current thread and set the current back one.
  const int kill_thread = m_cur_thread;
  ThreadPrev();
  
  // Turn off this bit in the m_thread_id_chart...
  m_thread_id_chart ^= 1 << m_threads[kill_thread].GetID();
  
  // Copy the last thread into the kill position
  const int last_thread = m_threads.GetSize() - 1;
  if (last_thread != kill_thread) {
    m_threads[kill_thread] = m_threads[last_thread];
  }
  
  // Kill the thread!
  m_threads.Resize(m_threads.GetSize() - 1);
  
  if (m_cur_thread > kill_thread) m_cur_thread--;
  
  return true;
}

////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int cHardwareExperimental::FindModifiedRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_register = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_register;
}

inline int cHardwareExperimental::FindModifiedNextRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_register = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  } else {
    default_register = (default_register + 1) % NUM_REGISTERS;
  }
  return default_register;
}

inline int cHardwareExperimental::FindModifiedPreviousRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_register = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  } else {
    default_register = (default_register + NUM_REGISTERS - 1) % NUM_REGISTERS;
  }
  return default_register;
}


inline int cHardwareExperimental::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_head = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_head;
}


inline int cHardwareExperimental::FindNextRegister(int base_reg)
{
  return (base_reg + 1) % NUM_REGISTERS;
}


bool cHardwareExperimental::Allocate_Necro(const int new_size)
{
  m_memory.ResizeOld(new_size);
  return true;
}

bool cHardwareExperimental::Allocate_Random(cAvidaContext& ctx, const int old_size, const int new_size)
{
  m_memory.Resize(new_size);
  
  for (int i = old_size; i < new_size; i++) {
    m_memory[i] = m_inst_set->GetRandomInst(ctx);
  }
  return true;
}

bool cHardwareExperimental::Allocate_Default(const int new_size)
{
  m_memory.Resize(new_size);
  
  // New space already defaults to default instruction...
  
  return true;
}

bool cHardwareExperimental::Allocate_Main(cAvidaContext& ctx, const int allocated_size)
{
  // must do divide before second allocate & must allocate positive amount...
  if (m_world->GetConfig().REQUIRE_ALLOCATE.Get() && m_mal_active == true) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR, "Allocate already active");
    return false;
  }
  if (allocated_size < 1) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Allocate of %d too small", allocated_size));
    return false;
  }
  
  const int old_size = m_memory.GetSize();
  const int new_size = old_size + allocated_size;
  
  // Make sure that the new size is in range.
  if (new_size > MAX_GENOME_LENGTH  ||  new_size < MIN_GENOME_LENGTH) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Invalid post-allocate size (%d)",
                               new_size));
    return false;
  }
  
  const int max_alloc_size = (int) (old_size * m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get());
  if (allocated_size > max_alloc_size) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Allocate too large (%d > %d)",
                               allocated_size, max_alloc_size));
    return false;
  }
  
  const int max_old_size =
    (int) (allocated_size * m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get());
  if (old_size > max_old_size) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Allocate too small (%d > %d)",
                               old_size, max_old_size));
    return false;
  }
  
  switch (m_world->GetConfig().ALLOC_METHOD.Get()) {
    case ALLOC_METHOD_NECRO:
      // Only break if this succeeds -- otherwise just do random.
      if (Allocate_Necro(new_size) == true) break;
    case ALLOC_METHOD_RANDOM:
      Allocate_Random(ctx, old_size, new_size);
      break;
    case ALLOC_METHOD_DEFAULT:
      Allocate_Default(new_size);
      break;
  }
  
  m_mal_active = true;
  
  return true;
}

int cHardwareExperimental::calcCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  for (int i = parent_size; i < parent_size + child_size; i++) {
    if (m_memory.FlagCopied(i)) copied_size++;
  }
  return copied_size;
}  


bool cHardwareExperimental::Divide_Main(cAvidaContext& ctx, const int div_point, const int extra_lines, double mut_multiplier)
{
  const int child_size = m_memory.GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  m_organism->OffspringGenome().SetSequence(m_memory.Crop(div_point, div_point+child_size));
  m_organism->OffspringGenome().SetHardwareType(GetType());
  m_organism->OffspringGenome().SetInstSet(m_inst_set->GetInstSetName());
  
  // Cut off everything in this memory past the divide point.
  m_memory.Resize(div_point);
  
  // Handle Divide Mutations...
  Divide_DoMutations(ctx, mut_multiplier);
  
  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures(ctx);
  
#if INSTRUCTION_COSTS
  // reset first time instruction costs
  for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
    m_inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
  }
#endif
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child
  bool parent_alive = m_organism->ActivateDivide(ctx);

  // Do more work if the parent lives through the birth of the offspring
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset(ctx);
  }
  
  return true;
}



//////////////////////////
// And the instructions...
//////////////////////////

bool cHardwareExperimental::Inst_IfNEqu(cAvidaContext& ctx) // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) == GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLess(cAvidaContext& ctx) // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) >=  GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfNotZero(cAvidaContext& ctx)  // Execute next if ?bx? != 0
{
  const int op1 = FindModifiedRegister(REG_BX);
  if (GetRegister(op1) == 0)  getIP().Advance();
  return true;
}
bool cHardwareExperimental::Inst_IfEqualZero(cAvidaContext& ctx)  // Execute next if ?bx? == 0
{
  const int op1 = FindModifiedRegister(REG_BX);
  if (GetRegister(op1) != 0)  getIP().Advance();
  return true;
}
bool cHardwareExperimental::Inst_IfGreaterThanZero(cAvidaContext& ctx)  // Execute next if ?bx? > 0
{
  const int op1 = FindModifiedRegister(REG_BX);
  if (GetRegister(op1) <= 0)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLessThanZero(cAvidaContext& ctx)  // Execute next if ?bx? < 0
{
  const int op1 = FindModifiedRegister(REG_BX);
  if (GetRegister(op1) >= 0)  getIP().Advance();
  return true;
}


bool cHardwareExperimental::Inst_IfGtrX(cAvidaContext& ctx)       // Execute next if BX > X; X value set according to NOP label
{
  // Compares value in BX to a specific value.  The value to compare to is determined by the nop label as follows:
  //    no nop label (default): valueToCompare = 1;
  //    nop-A: toggles valueToCompare sign-bit 
  //    nop-B: valueToCompare left-shift by 1-bit
  //    nop-C: valueToCompare left-shift by 2-bits
  //    nop-D: valueToCompare left-shift by 3-bits, etc.
  
  int valueToCompare = 1;
  
  ReadLabel();
  const cCodeLabel& shift_label = GetLabel();
  for (int i = 0; i < shift_label.GetSize(); i++) {
    if (shift_label[i] == REG_AX) {
      valueToCompare *= -1;
    } else {
      valueToCompare <<= shift_label[i];
    }
  }
  
  if (GetRegister(REG_BX) <= valueToCompare)  getIP().Advance();
  
  return true;
}

bool cHardwareExperimental::Inst_IfEquX(cAvidaContext& ctx)       // Execute next if BX == X; X value set according to NOP label
{
  // Compares value in BX to a specific value.  The value to compare to is determined by the nop label as follows:
  //    no nop label (default): valueToCompare = 1;
  //    nop-A: toggles valueToCompare sign-bit 
  //    nop-B: valueToCompare left-shift by 1-bit
  //    nop-C: valueToCompare left-shift by 2-bits
  //    nop-D: valueToCompare left-shift by 3-bits, etc.
  
  int valueToCompare = 1;
  
  ReadLabel();
  const cCodeLabel& shift_label = GetLabel();
  for (int i = 0; i < shift_label.GetSize(); i++) {
    if (shift_label[i] == REG_AX) {
      valueToCompare *= -1;
    } else {
      valueToCompare <<= shift_label[i];
    }
  }
  
  if (GetRegister(REG_BX) != valueToCompare)  getIP().Advance();
  
  return true;
}



bool cHardwareExperimental::Inst_IfConsensus(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  if (BitCount(GetRegister(op1)) <  CONSENSUS)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfConsensus24(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  if (BitCount(GetRegister(op1) & MASK24) <  CONSENSUS24)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLessConsensus(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1)) >=  BitCount(GetRegister(op2)))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLessConsensus24(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1) & MASK24) >=  BitCount(GetRegister(op2) & MASK24))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfStackGreater(cAvidaContext& ctx)
{
  int cur_stack = m_threads[m_cur_thread].cur_stack;
  if (getStack(cur_stack).Peek().value <=  getStack(!cur_stack).Peek().value)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_Label(cAvidaContext& ctx)
{
  ReadLabel();
  return true;
};


bool cHardwareExperimental::Inst_Pop(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue pop = stackPop();
  setInternalValue(m_threads[m_cur_thread].reg[reg_used], pop.value, pop);
  return true;
}

bool cHardwareExperimental::Inst_Push(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  getStack(m_threads[m_cur_thread].cur_stack).Push(m_threads[m_cur_thread].reg[reg_used]);
  return true;
}


bool cHardwareExperimental::Inst_SwitchStack(cAvidaContext& ctx) { switchStack(); return true;}

bool cHardwareExperimental::Inst_SwapStackTop(cAvidaContext& ctx)
{
  sInternalValue v0 = getStack(0).Pop();
  sInternalValue v1 = getStack(1).Pop();
  getStack(0).Push(v1);
  getStack(1).Push(v0);
  return true;
}

bool cHardwareExperimental::Inst_Swap(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue v1 = m_threads[m_cur_thread].reg[op1];
  m_threads[m_cur_thread].reg[op1] = m_threads[m_cur_thread].reg[op2];
  m_threads[m_cur_thread].reg[op2] = v1;
  return true;
}

bool cHardwareExperimental::Inst_ShiftR(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  setInternalValue(reg, reg.value >> 1, reg);
  return true;
}

bool cHardwareExperimental::Inst_ShiftL(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  setInternalValue(reg, reg.value << 1, reg);
  return true;
}


bool cHardwareExperimental::Inst_Inc(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  setInternalValue(reg, reg.value + 1, reg);
  return true;
}

bool cHardwareExperimental::Inst_Dec(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  setInternalValue(reg, reg.value - 1, reg);
  return true;
}


bool cHardwareExperimental::Inst_Add(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& dreg = m_threads[m_cur_thread].reg[dst];
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dreg, r1.value + r2.value, r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_Sub(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& dreg = m_threads[m_cur_thread].reg[dst];
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dreg, r1.value - r2.value, r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_Mult(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& dreg = m_threads[m_cur_thread].reg[dst];
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dreg, r1.value * r2.value, r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_Div(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& dreg = m_threads[m_cur_thread].reg[dst];
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  if (r2.value != 0) {
    if (0 - INT_MAX > r1.value && r2.value == -1)
      m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: Float exception");
    else
      setInternalValue(dreg, r1.value / r2.value, r1, r2);
  } else {
    m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
    return false;
  }
  return true;
}

bool cHardwareExperimental::Inst_Mod(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& dreg = m_threads[m_cur_thread].reg[dst];
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  if (r2.value != 0) {
    setInternalValue(dreg, r1.value % r2.value, r1, r2);
  } else {
    m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
    return false;
  }
  return true;
}


bool cHardwareExperimental::Inst_Nand(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& dreg = m_threads[m_cur_thread].reg[dst];
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dreg, ~(r1.value & r2.value), r1, r2);
  return true;
}



bool cHardwareExperimental::Inst_HeadAlloc(cAvidaContext& ctx)   // Allocate maximal more
{
  const int dst = FindModifiedRegister(REG_AX);
  const int cur_size = m_memory.GetSize();
  const int alloc_size = Min((int) (m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get() * cur_size),
                             MAX_GENOME_LENGTH - cur_size);
  sInternalValue& reg = m_threads[m_cur_thread].reg[dst];
  if (Allocate_Main(ctx, alloc_size)) {
    setInternalValue(reg, cur_size);
    return true;
  } else return false;
}


bool cHardwareExperimental::Inst_TaskIO(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setInternalValue(reg, value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}


bool cHardwareExperimental::Inst_TaskIOExpire(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  if (m_io_expire && reg.env_component && reg.oldest_component < m_last_output) return false;
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;  
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setInternalValue(reg, value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}


bool cHardwareExperimental::Inst_TaskInput(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setInternalValue(m_threads[m_cur_thread].reg[reg_used], value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}


bool cHardwareExperimental::Inst_TaskOutput(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  return true;
}


bool cHardwareExperimental::Inst_TaskOutputZero(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  setInternalValue(reg, 0);
  
  return true;
}

bool cHardwareExperimental::Inst_TaskOutputExpire(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  if (m_io_expire && reg.env_component && reg.oldest_component < m_last_output) return false;
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;

  return true;
}


bool cHardwareExperimental::Inst_MoveHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = FindModifiedHead(nHardware::HEAD_FLOW);
  getHead(head_used).Set(getHead(target));
  if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  return true;
}

bool cHardwareExperimental::Inst_MoveHeadIfNEqu(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = FindModifiedHead(nHardware::HEAD_FLOW);
  if (m_threads[m_cur_thread].reg[op1].value != m_threads[m_cur_thread].reg[op2].value) {
    getHead(head_used).Set(getHead(target));
    if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  }
  return true;
}

bool cHardwareExperimental::Inst_MoveHeadIfLess(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = FindModifiedHead(nHardware::HEAD_FLOW);
  if (m_threads[m_cur_thread].reg[op1].value < m_threads[m_cur_thread].reg[op2].value) {
    getHead(head_used).Set(getHead(target));
    if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  }
  return true;
}


bool cHardwareExperimental::Inst_Goto(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}

bool cHardwareExperimental::Inst_GotoIfNEqu(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  ReadLabel();
  if (m_threads[m_cur_thread].reg[op1].value != m_threads[m_cur_thread].reg[op2].value) {
    cHeadCPU found_pos = FindLabelForward(true);
    getIP().Set(found_pos);
  }
  return true;
}

bool cHardwareExperimental::Inst_GotoIfLess(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  ReadLabel();
  if (m_threads[m_cur_thread].reg[op1].value < m_threads[m_cur_thread].reg[op2].value) {
    cHeadCPU found_pos = FindLabelForward(true);
    getIP().Set(found_pos);
  }
  return true;
}


bool cHardwareExperimental::Inst_GotoConsensus(cAvidaContext& ctx)
{
  if (BitCount(GetRegister(REG_BX)) < CONSENSUS) return true;
  
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}

bool cHardwareExperimental::Inst_GotoConsensus24(cAvidaContext& ctx)
{
  if (BitCount(GetRegister(REG_BX) & MASK24) < CONSENSUS24) return true;
  
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}


bool cHardwareExperimental::Inst_JumpHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int reg = FindModifiedRegister(REG_CX);
  getHead(head_used).Jump(m_threads[m_cur_thread].reg[reg].value);
  if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  return true;
}

bool cHardwareExperimental::Inst_GetHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int reg = FindModifiedRegister(REG_CX);
  setInternalValue(m_threads[m_cur_thread].reg[reg], getHead(head_used).GetPosition());
  return true;
}

bool cHardwareExperimental::Inst_IfLabel(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLabel_Direct(cAvidaContext& ctx)
{
  ReadLabel();
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}


bool cHardwareExperimental::Inst_HeadDivide(cAvidaContext& ctx)
{
  m_organism->GetPhenotype().SetDivideSex(false);
  m_organism->GetPhenotype().SetCrossNum(0);

  AdjustHeads();
  const int divide_pos = getHead(nHardware::HEAD_READ).GetPosition();
  int child_end =  getHead(nHardware::HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
  bool ret_val = Divide_Main(ctx, divide_pos, extra_lines, 1.0);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val; 
}


bool cHardwareExperimental::Inst_HeadDivideSex(cAvidaContext& ctx)  
{ 
  m_organism->GetPhenotype().SetDivideSex(true);
  m_organism->GetPhenotype().SetCrossNum(1);
  
  AdjustHeads();
  const int divide_pos = getHead(nHardware::HEAD_READ).GetPosition();
  int child_end =  getHead(nHardware::HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
  bool ret_val = Divide_Main(ctx, divide_pos, extra_lines, 1.0);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val; 
}


bool cHardwareExperimental::Inst_HeadRead(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  
  const int head_id = FindModifiedHead(nHardware::HEAD_READ);
  getHead(head_id).Adjust();
  
  // Mutations only occur on the read, for the moment.
  int read_inst = 0;
  if (m_organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx).GetOp();
  } else {
    read_inst = getHead(head_id).GetInst().GetOp();
  }
  setInternalValue(m_threads[m_cur_thread].reg[dst], read_inst);
  ReadInst(read_inst);
  
  if (m_slip_read_head && m_organism->TestCopySlip(ctx))
    getHead(head_id).Set(ctx.GetRandom().GetInt(m_memory.GetSize()));
  
  getHead(head_id).Advance();
  return true;
}

bool cHardwareExperimental::Inst_HeadWrite(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int head_id = FindModifiedHead(nHardware::HEAD_WRITE);
  cHeadCPU& active_head = getHead(head_id);
  
  active_head.Adjust();
  
  int value = m_threads[m_cur_thread].reg[src].value;
  if (value < 0 || value >= m_inst_set->GetSize()) value = 0;
  
  active_head.SetInst(cInstruction(value));
  active_head.SetFlagCopied();
  
  if (m_organism->TestCopyIns(ctx)) active_head.InsertInst(m_inst_set->GetRandomInst(ctx));
  if (m_organism->TestCopyDel(ctx)) active_head.RemoveInst();
  if (m_organism->TestCopyUniform(ctx)) doUniformCopyMutation(ctx, active_head);
  if (!m_slip_read_head && m_organism->TestCopySlip(ctx)) 
    doSlipMutation(ctx, m_memory, active_head.GetPosition());
  
  // Advance the head after write...
  active_head++;
  return true;
}

bool cHardwareExperimental::Inst_HeadCopy(cAvidaContext& ctx)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU& read_head = getHead(nHardware::HEAD_READ);
  cHeadCPU& write_head = getHead(nHardware::HEAD_WRITE);
  
  read_head.Adjust();
  write_head.Adjust();
  
  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  ReadInst(read_inst.GetOp());
  if (m_organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx);
    write_head.SetFlagMutated();
    write_head.SetFlagCopyMut();
  }
  
  write_head.SetInst(read_inst);
  write_head.SetFlagCopied();  // Set the copied flag...
  
  if (m_organism->TestCopyIns(ctx)) write_head.InsertInst(m_inst_set->GetRandomInst(ctx));
  if (m_organism->TestCopyDel(ctx)) write_head.RemoveInst();
  if (m_organism->TestCopyUniform(ctx)) doUniformCopyMutation(ctx, write_head);
  if (m_organism->TestCopySlip(ctx)) {
    if (m_slip_read_head) {
      read_head.Set(ctx.GetRandom().GetInt(m_memory.GetSize()));
    } else 
      doSlipMutation(ctx, m_memory, write_head.GetPosition());
  }
  
  read_head.Advance();
  write_head.Advance();
  return true;
}

bool cHardwareExperimental::Inst_HeadCopy_NoLabel(cAvidaContext& ctx)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU& read_head = getHead(nHardware::HEAD_READ);
  cHeadCPU& write_head = getHead(nHardware::HEAD_WRITE);
  
  read_head.Adjust();
  write_head.Adjust();
  
  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  ReadInst_NoLabel(read_inst.GetOp());
  if (m_organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx);
    write_head.SetFlagMutated();
    write_head.SetFlagCopyMut();
  }
  
  write_head.SetInst(read_inst);
  write_head.SetFlagCopied();  // Set the copied flag...
  
  if (m_organism->TestCopyIns(ctx)) write_head.InsertInst(m_inst_set->GetRandomInst(ctx));
  if (m_organism->TestCopyDel(ctx)) write_head.RemoveInst();
  if (m_organism->TestCopyUniform(ctx)) doUniformCopyMutation(ctx, write_head);
  if (m_organism->TestCopySlip(ctx)) {
    if (m_slip_read_head) {
      read_head.Set(ctx.GetRandom().GetInt(m_memory.GetSize()));
    } else 
      doSlipMutation(ctx, m_memory, write_head.GetPosition());
  }
  
  read_head.Advance();
  write_head.Advance();
  return true;
}

bool cHardwareExperimental::Inst_SearchS(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_SearchS_Direct(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}


bool cHardwareExperimental::Inst_SearchF(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_SearchF_Direct(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_SearchB(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_SearchB_Direct(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}



bool cHardwareExperimental::Inst_SetFlow(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_CX);
  getHead(nHardware::HEAD_FLOW).Set(GetRegister(reg_used));
  return true; 
}



// --------  Promoter Model  --------

bool cHardwareExperimental::Inst_Promoter(cAvidaContext& ctx)
{
  // Promoters don't do anything themselves
  return true;
}


// Move the instruction ptr to the next active promoter
bool cHardwareExperimental::Inst_Terminate(cAvidaContext& ctx)
{
  PromoterTerminate(ctx);
  return true;
}

// Move the instruction ptr to the next active promoter
bool cHardwareExperimental::Inst_TerminateConsensus(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_DX);
  if (BitCount(GetRegister(op1)) <  CONSENSUS)  PromoterTerminate(ctx);
  return true;
}


// Move the instruction ptr to the next active promoter
bool cHardwareExperimental::Inst_TerminateConsensus24(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_DX);
  if (BitCount(GetRegister(op1) & MASK24) <  CONSENSUS24)  PromoterTerminate(ctx);
  return true;
}


// Set a new regulation code (which is XOR'ed with ALL promoter codes).
bool cHardwareExperimental::Inst_Regulate(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  int regulation_code = GetRegister(reg_used);
  
  for (int i = 0; i < m_promoters.GetSize(); i++) m_promoters[i].regulation = regulation_code;

  return true;
}

// Set a new regulation code, but only on a subset of promoters.
bool cHardwareExperimental::Inst_RegulateSpecificPromoters(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_promoter = FindModifiedNextRegister(reg_used);

  int regulation_code = GetRegister(reg_used);
  int regulation_promoter = GetRegister(reg_promoter);
  
  for (int i = 0; i < m_promoters.GetSize(); i++) {
    
    // @DMB - should this always be using the low order bits?
    
    // Look for consensus bit matches over the length of the promoter code
    int test_p_code = m_promoters[i].bit_code;    
    int test_r_code = regulation_promoter;
    int bit_count = 0;
    for (int j = 0; j < m_world->GetConfig().PROMOTER_EXE_LENGTH.Get(); j++) {      
      if ((test_p_code & 1) == (test_r_code & 1)) bit_count++;
      test_p_code >>= 1;
      test_r_code >>= 1;
    }
    
    if (bit_count >= m_world->GetConfig().PROMOTER_EXE_LENGTH.Get() / 2) m_promoters[i].regulation = regulation_code;
  }
  
  return true;
}


bool cHardwareExperimental::Inst_SenseRegulate(cAvidaContext& ctx)
{
  unsigned int bits = 0;
  const tArray<double> & res_count = m_organism->GetOrgInterface().GetResources();
  assert (res_count.GetSize() != 0);
  for (int i=0; i<m_world->GetConfig().PROMOTER_CODE_SIZE.Get(); i++)
  {
    int b = i % res_count.GetSize();
    bits <<= 1;
    bits += (res_count[b] != 0);
  }  
  
  for (int i=0; i< m_promoters.GetSize();i++)
  {
    m_promoters[i].regulation = bits;
  }
  return true;
}

// Create a number from inst bit codes
bool cHardwareExperimental::Do_Numberate(cAvidaContext& ctx, int num_bits)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  
  // advance the IP now, so that it rests on the beginning of our number
  getIP().Advance();
  m_advance_ip = false;
  
  int num = Numberate(getIP().GetPosition(), +1, num_bits);
  setInternalValue(m_threads[m_cur_thread].reg[reg_used], num);
  return true;
}


void cHardwareExperimental::PromoterTerminate(cAvidaContext& ctx)
{
  // Optionally,
  // Reset the thread.
  if (m_world->GetConfig().TERMINATION_RESETS.Get())
  {
    m_threads[m_cur_thread].Reset(this, m_threads[m_cur_thread].GetID());
    
    //Setting this makes it harder to do things. You have to be modular.
    m_organism->GetOrgInterface().ResetInputs(ctx);   // Re-randomize the inputs this organism sees
    m_organism->ClearInput();                         // Also clear their input buffers, or they can still claim
    // rewards for numbers no longer in their environment!
  }
  
  // Reset our count
  m_threads[m_cur_thread].ResetPromoterInstExecuted();
  m_advance_ip = false;
  const int promoter_reg_used = REG_DX; // register to put chosen promoter code in, default to DX
  
  
  // @DMB - should the promoter index and offset be stored in cLocalThread to allow multiple threads?
  
  // Search for an active promoter  
  int start_offset = m_promoter_offset;
  int start_index  = m_promoter_index;
  
  
  bool no_promoter_found = true;
  if (m_promoters.GetSize() > 0) {
    const int promoter_exe_length = m_world->GetConfig().PROMOTER_EXE_LENGTH.Get();
    const int promoter_code_size = m_world->GetConfig().PROMOTER_CODE_SIZE.Get();
    const int promoter_exe_threshold = m_world->GetConfig().PROMOTER_EXE_THRESHOLD.Get();
    
    while (true) {
      // If the next promoter is active, then break out
      
      // Move promoter index, rolling over if necessary
      m_promoter_index++;
      
      if (m_promoter_index == m_promoters.GetSize()) {
        m_promoter_index = 0;
        
        // Move offset, rolling over when there are not enough bits before we would have to wrap around left
        m_promoter_offset += promoter_exe_length;
        if (m_promoter_offset + promoter_exe_length > promoter_code_size) m_promoter_offset = 0;
      }
      
      
      int count = 0;
      unsigned int code = m_promoters[m_promoter_index].GetRegulatedBitCode();
      for (int i = 0; i < promoter_exe_length; i++) {
        // @DMB - changed the following to avoid integer division. We should only ever need to circle around once
        //int offset = (m_promoter_offset + i) % promoter_code_size;
        int offset = m_promoter_offset + i;
        if (offset >= promoter_code_size) offset -= promoter_code_size;
        int state = code >> offset;
        count += (state & 1);
      }
      
      if (count >= promoter_exe_threshold) {
        no_promoter_found = false;
        break;
      }
      
      // If we just checked the promoter that we were originally on, then there
      // are no active promoters.
      if ((start_offset == m_promoter_offset) && (start_index == m_promoter_index)) break;
      
      // If we originally were not on a promoter, then stop once we check the
      // first promoter and an offset of zero
      if (start_index == -1) start_index = 0;
    } 
  }
  
  if (no_promoter_found) {
    switch (m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get()) {
      case 0:
      case 2:
        // Set defaults for when no active promoter is found
        m_promoter_index = -1;
        getIP().Set(0);
        setInternalValue(m_threads[m_cur_thread].reg[promoter_reg_used], 0);
        break;
        
      case 1: // Death to organisms that refuse to use promoters!
        m_organism->Die();
        break;
        
      default:
        cout << "Unrecognized NO_ACTIVE_PROMOTER_EFFECT setting: " << m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get() << endl;
        exit(1);
        break;
    }
  } else {
    // We found an active match, offset to just after it.
    // cHeadCPU will do the mod genome size for us
    getIP().Set(m_promoters[m_promoter_index].pos + 1);
    
    // Put its bit code in BX for the organism to have if option is set
    if (m_world->GetConfig().PROMOTER_TO_REGISTER.Get())
      setInternalValue(m_threads[m_cur_thread].reg[promoter_reg_used], m_promoters[m_promoter_index].bit_code);
  }  
}

// Construct a promoter bit code from instruction bit codes
int cHardwareExperimental::Numberate(int _pos, int _dir, int _num_bits)
{  
  int code_size = 0;
  unsigned int code = 0;
  unsigned int max_bits = sizeof(code) * 8;
  assert(_num_bits <= (int)max_bits);
  if (_num_bits == 0) _num_bits = max_bits;
  
  // Enforce a boundary, sometimes -1 can be passed for _pos
  int j = _pos + m_memory.GetSize();
  j %= m_memory.GetSize();
  assert(j >=0);
  assert(j < m_memory.GetSize());
  while (code_size < _num_bits)
  {
    unsigned int inst_code = (unsigned int) GetInstSet().GetInstructionCode(m_memory[j]);
    // shift bits in, one by one ... excuse the counter variable pun
    for (int code_on = 0; (code_size < _num_bits) && (code_on < m_world->GetConfig().INST_CODE_LENGTH.Get()); code_on++)
    {
      if (_dir < 0)
      {
        code >>= 1; // shift first so we don't go one too far at the end
        code += (1 << (_num_bits - 1)) * (inst_code & 1);
        inst_code >>= 1; 
      }
      else
      {
        code <<= 1; // shift first so we don't go one too far at the end;        
        code += (inst_code >> (m_world->GetConfig().INST_CODE_LENGTH.Get() - 1)) & 1;
        inst_code <<= 1; 
      }
      code_size++;
    }
    
    // move back one inst
    j += m_memory.GetSize() + _dir;
    j %= m_memory.GetSize();
    
  }
  
  return code;
}

/*! 
 Sets BX to 1 if >=50% of the bits in the specified register places
 are 1's and zero otherwise.
 */

bool cHardwareExperimental::Inst_BitConsensus(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedNextRegister(reg_used);
  sInternalValue& val = m_threads[m_cur_thread].reg[op1];

  setInternalValue(m_threads[m_cur_thread].reg[reg_used], (BitCount(val.value) >= CONSENSUS) ? 1 : 0, val); 
  return true; 
}

// Looks at only the lower 24 bits
bool cHardwareExperimental::Inst_BitConsensus24(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedNextRegister(reg_used);
  sInternalValue& val = m_threads[m_cur_thread].reg[op1];
  
  setInternalValue(m_threads[m_cur_thread].reg[reg_used], (BitCount(val.value & MASK24) >= CONSENSUS24) ? 1 : 0, val); 
  return true; 
}

// Create a number from inst bit codes of the previously executed instructions
bool cHardwareExperimental::Inst_Execurate(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  setInternalValue(m_threads[m_cur_thread].reg[reg_used], m_threads[m_cur_thread].GetExecurate());
  return true;
}

// Create a number from inst bit codes of the previously executed instructions, truncated to 24 bits
bool cHardwareExperimental::Inst_Execurate24(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  setInternalValue(m_threads[m_cur_thread].reg[reg_used], (MASK24 & m_threads[m_cur_thread].GetExecurate()));
  return true;
}


bool cHardwareExperimental::Inst_Repro(cAvidaContext& ctx)
{
  // const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  // these checks should be done, but currently they make some assumptions
  // that crash when evaluating this kind of organism -- JEB

  
  if (m_organism->GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  m_organism->OffspringGenome().SetSequence(m_memory);
  m_organism->OffspringGenome().SetHardwareType(GetType());
  m_organism->OffspringGenome().SetInstSet(m_inst_set->GetInstSetName());
  m_organism->GetPhenotype().SetLinesCopied(m_memory.GetSize());

  int lines_executed = 0;
  for (int i = 0; i < m_memory.GetSize(); i++) if (m_memory.FlagExecuted(i)) lines_executed++;
  m_organism->GetPhenotype().SetLinesExecuted(lines_executed);
  
  
  // Perform Copy Mutations...
  if (m_organism->GetCopyMutProb() > 0) { // Skip this if no mutations....
    for (int i = 0; i < m_memory.GetSize(); i++) {
      if (m_organism->TestCopyMut(ctx)) m_organism->OffspringGenome().GetSequence()[i] = m_inst_set->GetRandomInst(ctx);
    }
  }
  
  // Handle Divide Mutations...
  Divide_DoMutations(ctx);
  
  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures(ctx);
  
#if INSTRUCTION_COSTS
  // reset first time instruction costs
  for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
    m_inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
  }
#endif
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child
  bool parent_alive = m_organism->ActivateDivide(ctx);
  
  // Do more work if the parent lives through the birth of the offspring
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset(ctx);
  }
  
  return true;
}


bool cHardwareExperimental::Inst_Die(cAvidaContext& ctx)
{
  m_organism->Die();
  
  return true;
}


bool cHardwareExperimental::Inst_SGMove(cAvidaContext& ctx)
{
  assert(m_ext_mem.GetSize() > 3);
  
  const cStateGrid& sg = m_organism->GetStateGrid();
  
  int& x = m_ext_mem[0];
  int& y = m_ext_mem[1];
  
  const int facing = m_ext_mem[2];
  
  // State grid is treated as a 2-dimensional toroidal grid with size [0, width) and [0, height)
  switch (facing) {
    case 0: // N
      if (++y == sg.GetHeight()) y = 0;
      break;
      
    case 1: // NE
      if (++x == sg.GetWidth()) x = 0;
      if (++y == sg.GetHeight()) y = 0;
      break;
      
    case 2: // E
      if (++x == sg.GetWidth()) x = 0;
      break;
      
    case 3: // SE
      if (++x == sg.GetWidth()) x = 0;
      if (--y == -1) y = sg.GetHeight() - 1;
      break;
      
    case 4: // S
      if (--y == -1) y = sg.GetHeight() - 1;
      break;
      
    case 5: // SW
      if (--x == -1) x = sg.GetWidth() - 1;
      if (--y == -1) y = sg.GetHeight() - 1;
      break;
      
    case 6: // W
      if (--x == -1) x = sg.GetWidth() - 1;
      break;
      
    case 7: // NW
      if (--x == -1) x = sg.GetWidth() - 1;
      if (++y == sg.GetHeight()) y = 0;
      break;
      
    default:
      assert(facing >= 0 && facing <= 7);
  }
  
  // Increment state observed count
  m_ext_mem[3 + sg.GetStateAt(x, y)]++;
  
  // Save this location in the movement history
  m_ext_mem.Push(sg.GetIDFor(x, y));
  return true;
}

bool cHardwareExperimental::Inst_SGRotateL(cAvidaContext& ctx)
{
  assert(m_ext_mem.GetSize() > 3);
  if (--m_ext_mem[2] < 0) m_ext_mem[2] = 7;
  return true;
}

bool cHardwareExperimental::Inst_SGRotateR(cAvidaContext& ctx)
{
  assert(m_ext_mem.GetSize() > 3);
  if (++m_ext_mem[2] > 7) m_ext_mem[2] = 0;
  return true;
}

bool cHardwareExperimental::Inst_SGSense(cAvidaContext& ctx)
{
  const cStateGrid& sg = m_organism->GetStateGrid();
  const int reg_used = FindModifiedRegister(REG_BX);
  setInternalValue(m_threads[m_cur_thread].reg[reg_used], sg.SenseStateAt(m_ext_mem[0], m_ext_mem[1]));
  return true; 
}



