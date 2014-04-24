/*
 *  hardware/types/GP8.cc
 *  avida-core
 *
 *  Created by David on 7/24/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/private/hardware/types/GP8.h"

#include "avida/core/Feedback.h"
#include "avida/core/UniverseDriver.h"
#include "avida/output/File.h"

#include <climits>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace Avida;
using namespace Avida::Hardware::InstructionFlags;
using namespace Avida::Util;


Hardware::Types::GP8::GP8InstLib* Hardware::Types::GP8::s_inst_lib = Hardware::Types::GP8::initInstLib();

Hardware::Types::GP8::GP8InstLib* Hardware::Types::GP8::initInstLib(void)
{
  struct NOPEntry {
    Apto::String name;
    int nop_mod;
    NOPEntry(const Apto::String &name, int nop_mod) : name(name), nop_mod(nop_mod) {}
  };
  static const NOPEntry s_n_array[] = {
    NOPEntry("nop-A", rAX),
    NOPEntry("nop-B", rBX),
    NOPEntry("nop-C", rCX),
    NOPEntry("nop-D", rDX),
    NOPEntry("nop-E", rEX),
    NOPEntry("nop-F", rFX),
    NOPEntry("nop-G", rGX),
    NOPEntry("nop-H", rHX),
  };
  
  static const GP8Inst s_f_array[] = {
    /*
     Note: all entries of cNOPEntryCPU s_n_array must have corresponding in the same order in
     InstLib Entries s_f_array, and these entries must be the first elements of s_f_array.
     */
#define INST(NAME, FUNC, CLS, FLAGS, UNITS, DESC) GP8Inst(NAME, &Hardware::Types::GP8::FUNC, INST_CLASS_ ## CLS, FLAGS, DESC, UNITS)
#define INSTI(NAME, FUNC, VAL, CLS, FLAGS, UNITS, DESC) GP8Inst(NAME, &Hardware::Types::GP8::FUNC, INST_CLASS_ ## CLS, FLAGS, DESC, UNITS, &Hardware::Types::GP8::VAL)
    INST("nop-A", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    INST("nop-B", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    INST("nop-C", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    INST("nop-D", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    INST("nop-E", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    INST("nop-F", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    INST("nop-G", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    INST("nop-H", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    
    INST("NULL", Inst_Nop, NOP, 0, 0, "True no-operation instruction: does nothing"),
    INST("nop-X", Inst_Nop, NOP, 0, 0, "True no-operation instruction: does nothing"),
    
    // Genes
    INST("promoter", Inst_Nop, FLOW_CONTROL, PROMOTER, 0, "True no-operation instruction: does nothing"),
    INST("terminator", Inst_Nop, FLOW_CONTROL, TERMINATOR, 0, "True no-operation instruction: does nothing"),
    
    // Multi-Threading
    INST("regulate-pause", Inst_RegulatePause, OTHER, 0, 0, ""),
    INST("regulate-resume", Inst_RegulateResume, OTHER, 0, 0, ""),
    INST("regulate-reset", Inst_RegulateReset, OTHER, 0, 0, ""),
    INST("wait-cond-equ", Inst_WaitCondition_Equal, OTHER, 0, 0, ""),
    INST("wait-cond-less", Inst_WaitCondition_Less, OTHER, 0, 0, ""),
    INST("wait-cond-gtr", Inst_WaitCondition_Greater, OTHER, 0, 0, ""),
    INST("yield", Inst_Yield, OTHER, 0, 0, ""),
    
    // Flow Control Instructions
    INST("set-memory", Inst_SetMemory, FLOW_CONTROL, 0, 0, "Set ?mem_space_label? of the ?Flow? head."),
    INST("mov-head", Inst_MoveHead, FLOW_CONTROL, 0, 0, "Move head ?IP? to the flow head"),
    INST("jmp-head", Inst_JumpHead, FLOW_CONTROL, 0, 0, "Move head ?Flow? by amount in ?CX? register"),
    INST("get-head", Inst_GetHead, FLOW_CONTROL, 0, 0, "Copy the position of the ?IP? head into ?CX?"),
    INST("label", Inst_Label, FLOW_CONTROL, LABEL, 0, ""),
    INST("search-lbl-s", Inst_Search_Label_S, FLOW_CONTROL, 0, 0, "Find direct label from genome start and move the flow head"),
    INST("search-lbl-d", Inst_Search_Label_D, FLOW_CONTROL, 0, 0, "Find direct label backward and move the flow head"),
    INST("search-seq-d", Inst_Search_Seq_D, FLOW_CONTROL, 0, 0, "Find complement template backward and move the flow head"),
    
    // Standard Conditionals
    INST("if-n-equ", Inst_IfNEqu, CONDITIONAL, 0, 0, "Execute next instruction if ?BX?!=?CX?, else skip it"),
    INST("if-less", Inst_IfLess, CONDITIONAL, 0, 0, "Execute next instruction if ?BX? < ?CX?, else skip it"),
    INST("if-not-0", Inst_IfNotZero, CONDITIONAL, 0, 0, "Execute next instruction if ?BX? != 0, else skip it"),
    INST("if-equ-0", Inst_IfEqualZero, CONDITIONAL, 0, 0, "Execute next instruction if ?BX? == 0, else skip it"),
    INST("if-gtr-0", Inst_IfGreaterThanZero, CONDITIONAL, 0, 0, "Execute next instruction if ?BX? > 0, else skip it"),
    INST("if-less-0", Inst_IfLessThanZero, CONDITIONAL, 0, 0, "Execute next instruction if ?BX? < 0, else skip it"),
    
    // Core ALU Operations
    INST("shift-r", Inst_ShiftR, ARITHMETIC_LOGIC, 0, 0, "Shift bits in ?BX? right by one (divide by two)"),
    INST("shift-l", Inst_ShiftL, ARITHMETIC_LOGIC, 0, 0, "Shift bits in ?BX? left by one (multiply by two)"),
    INST("inc", Inst_Inc, ARITHMETIC_LOGIC, 0, 0, "Increment ?BX? by one"),
    INST("dec", Inst_Dec, ARITHMETIC_LOGIC, 0, 0, "Decrement ?BX? by one"),
    
    INST("add", Inst_Add, ARITHMETIC_LOGIC, 0, 0, "Add BX to CX and place the result in ?BX?"),
    INST("sub", Inst_Sub, ARITHMETIC_LOGIC, 0, 0, "Subtract CX from BX and place the result in ?BX?"),
    INST("nand", Inst_Nand, ARITHMETIC_LOGIC, 0, 0, "Nand BX by CX and place the result in ?BX?"),
    
    INST("mult", Inst_Mult, ARITHMETIC_LOGIC, 0, 0, "Multiple BX by CX and place the result in ?BX?"),
    INST("div", Inst_Div, ARITHMETIC_LOGIC, 0, 0, "Divide BX by CX and place the result in ?BX?"),
    INST("mod", Inst_Mod, ARITHMETIC_LOGIC, 0, 0, ""),
    
    INSTI("zero", Inst_Zero, Val_Zero, ARITHMETIC_LOGIC, IMMEDIATE_VALUE, 0, "Set ?BX? to 0"),
    INSTI("one", Inst_One, Val_One, ARITHMETIC_LOGIC, IMMEDIATE_VALUE, 0, "Set ?BX? to 1"),
    INSTI("maxint", Inst_MaxInt, Val_MaxInt, ARITHMETIC_LOGIC, IMMEDIATE_VALUE, 0, "Set ?BX? to MAX_INT"),
    INSTI("rand", Inst_Rand, Val_Rand, ARITHMETIC_LOGIC, IMMEDIATE_VALUE, 0, "Set ?BX? to rand number"),
    
    INST("pop", Inst_Pop, DATA, 0, 0, "Remove top number from stack and place into ?BX?"),
    INST("push", Inst_Push, DATA, 0, 0, "Copy number from ?BX? and place it into the stack"),
    INST("pop-all", Inst_PopAll, DATA, 0, 0, "Remove top numbers from stack and place into ?BX?"),
    INST("push-all", Inst_PushAll, DATA, 0, 0, "Copy number from all registers and place into the stack"),
    INST("swap-stk", Inst_SwitchStack, DATA, 0, 0, "Toggle which stack is currently being used"),
    INST("swap", Inst_Swap, DATA, 0, 0, "Swap the contents of ?BX? with ?CX?"),
    
    INST("input", Inst_TaskInput, ENVIRONMENT, STALL, 0, "Input new number into ?BX?"),
    INST("output", Inst_TaskOutput, ENVIRONMENT, STALL, 0, "Output ?BX?"),
    
    // Replication Instructions
    INST("h-read", Inst_HeadRead, LIFECYCLE, 0, uREAD, "Read instruction from ?read-head? to ?AX?; advance the head."),
    INST("h-write", Inst_HeadWrite, LIFECYCLE, 0, uWRITE, "Write to ?write-head? instruction from ?AX?; advance the head."),
    INST("h-copy", Inst_HeadCopy, LIFECYCLE, 0, (uREAD & uWRITE), "Copy from read-head to write-head; advance both"),
    INST("divide-memory", Inst_DivideMemory, LIFECYCLE, STALL, 0, "Divide memory space."),
    INST("did-copy-lbl", Inst_DidCopyLabel, OTHER, 0, 0, "Execute next if we copied direct match of the attached label"),
    
    INST("repro", Inst_Repro, LIFECYCLE, STALL, 0, "Instantly reproduces the organism"),
    
    INST("die", Inst_Die, LIFECYCLE, STALL, 0, "Instantly kills the organism"),
    
    // Movement and Navigation instructions
    INST("move", Inst_Move, ENVIRONMENT, STALL, 0, ""),
    INST("get-north-offset", Inst_GetNorthOffset, ENVIRONMENT, 0, 0, ""),
    
    // Rotation
    INST("rotate-x", Inst_RotateX, ENVIRONMENT, STALL, 0, ""),
    INST("rotate-org-id", Inst_RotateOrgID, ENVIRONMENT, STALL, 0, ""),
    INST("rotate-away-org-id", Inst_RotateAwayOrgID, ENVIRONMENT, STALL, 0, ""),
    
    // Resource and Topography Sensing
    INST("set-forage-target", Inst_SetForageTarget, ENVIRONMENT, STALL, 0, ""),
    INST("set-ft-once", Inst_SetForageTargetOnce, ENVIRONMENT, STALL, 0, ""),
    INST("set-rand-ft-once", Inst_SetRandForageTargetOnce, ENVIRONMENT, STALL, 0, ""),
    INST("get-forage-target", Inst_GetForageTarget, ENVIRONMENT, 0, 0, ""),
    
    INST("sense-resource-id", Inst_SenseResourceID, ENVIRONMENT, STALL, 0, ""),
    INST("sense-nest", Inst_SenseNest, ENVIRONMENT, STALL, 0, ""),
    INST("sense-faced-habitat", Inst_SenseFacedHabitat, ENVIRONMENT, STALL, 0, ""),
    INST("look-ahead-ex", Inst_LookAheadEX, ENVIRONMENT, STALL, 0, ""),
    INST("look-again-ex", Inst_LookAgainEX, ENVIRONMENT, STALL, 0, ""),
    
    INST("eat", Inst_Eat, ENVIRONMENT, STALL, 0, ""),
    
    
    INST("collect-specific", Inst_CollectSpecific, ENVIRONMENT, STALL, 0, ""),
    INST("get-res-stored", Inst_GetResStored, ENVIRONMENT, STALL, 0, ""),
    
    // Org Interaction instructions
    INST("get-faced-org-id", Inst_GetFacedOrgID, ENVIRONMENT, STALL, 0, ""),
    
    INST("teach-offspring", Inst_TeachOffspring, ENVIRONMENT, 0, 0, ""),
    INST("learn-parent", Inst_LearnParent, ENVIRONMENT, STALL, 0, ""),
    
    INST("attack-prey", Inst_AttackPrey, ENVIRONMENT, STALL, uATTACK, ""),
    
    // Control-type Instructions
    INST("scramble-registers", Inst_ScrambleReg, DATA, 0, 0, ""),
#undef INST
  };
  
  
  const int n_size = sizeof(s_n_array)/sizeof(NOPEntry);
  
  static Apto::String n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size && i < NUM_REGISTERS; i++) {
    n_names[i] = s_n_array[i].name;
    nop_mods[i] = s_n_array[i].nop_mod;
  }
  
  const int f_size = sizeof(s_f_array)/sizeof(GP8Inst);
  static InstMethod functions[f_size];
  static unsigned int hw_units[f_size];
  static ImmMethod imm_methods[f_size];
  for (int i = 0; i < f_size; i++) {
    functions[i] = s_f_array[i].Function();
    hw_units[i] = s_f_array[i].HWUnits();
    imm_methods[i] = s_f_array[i].ImmediateMethod();
  }
  
  const int def = 0;
  const int null_inst = 8;
  
  return new GP8InstLib(f_size, s_f_array, n_names, nop_mods, functions, hw_units, imm_methods, def, null_inst);
}

Hardware::Types::GP8::GP8(Context& ctx, ConfigPtr cfg, Biota::OrganismPtr owner)
: InstArchCPU(ctx, cfg, owner), m_genes(0), m_mem_array(1), m_sensor_sessions(NUM_NOPS)
{
  m_functions = s_inst_lib->Functions();
  m_hw_units = s_inst_lib->HWUnits();
  m_imm_methods = s_inst_lib->ImmediateMethods();
  
  m_spec_die = false;
  
  const Genome& in_genome = owner->UnitGenome();
  ConstInstructionSequencePtr in_seq_p;
  in_seq_p.DynamicCastFrom(in_genome.Representation());
  const InstructionSequence& in_seq = *in_seq_p;
  
  m_mem_array[0] = in_seq;  // Initialize memory...
  Reset(ctx); // Setup the rest of the hardware...
}

Hardware::Types::GP8::~GP8()
{
  
}



void Hardware::Types::GP8::internalReset()
{
  m_spec_stall = false;
  m_hw_reset = true;
  
  m_cycle_count = 0;
  m_last_output = 0;
  
  for (int i = 0; i < m_sensor_sessions.GetSize(); i++) m_sensor_sessions[i].Clear();
  
  // Stack
  m_global_stack.Clear(m_inst_set->GetStackSize());
  
  
  // Threads
  m_threads.Resize(0);
  m_waiting_threads = 0;
  m_running_threads = 0;
  
  // Memory
  m_mem_array.Resize(1);
  for (int i = 1; i < MAX_MEM_SPACES; i++) m_mem_ids[i] = -1;
  m_mem_ids[0] = 0;
  
  // Genes
  m_genes.Resize(0);
  setupGenes();
  
  m_action_side_effect_queue = NULL;
}


void Hardware::Types::GP8::internalResetOnFailedDivide()
{
	internalReset();
}


void Hardware::Types::GP8::setupGenes()
{
  Head cur_promoter(this, 0, 0, false);
  
  do {
    if (m_instset->IsPromoter(cur_promoter.GetInst())) {
      // Flag the promoter as executed
      cur_promoter.SetFlagExecuted();
      
      // Create the gene data structure
      int gene_id = m_genes.GetSize();
      m_genes.Resize(gene_id + 1);
      
      // Start reading gene content, including the label if specified
      Head gene_content_start(cur_promoter);
      readLabel(gene_content_start, m_genes[gene_id].label);
      
      
      // Copy the specified genome segment
      Head seghead(gene_content_start);
      seghead.Advance();
      
      int gene_idx = 0;
      InstMemSpace& gene = m_genes[gene_id].memory;
      
      while (!m_instset->IsTerminator(seghead.GetInst()) && seghead != gene_content_start) {
        if (gene.GetSize() <= gene_idx) gene.Resize(gene.GetSize() + 1);
        gene[gene_idx] = seghead.GetInst();
        seghead.SetFlagExecuted();
        gene_idx++;
        seghead.Advance();
      }
      
      // Ignore zero length genes
      if (gene.GetSize() == 0) {
        m_genes.Resize(gene_id);
        continue;
      }
      
      // Create the gene thread
      Head thread_start(this, 0, gene_id, true);
      threadCreate(m_genes[gene_id].label, thread_start);
    }
    
    cur_promoter.Advance();
  } while (!cur_promoter.AtEnd());
  
  
  // If no valid genes where identified, create default gene from the whole genome
  if (m_genes.GetSize() == 0) {
    m_genes.Resize(1);
    m_genes[0].memory = m_mem_array[0];
    for (int i = 0; i < m_mem_array[0].GetSize(); i++) m_mem_array[0].SetFlagExecuted(i);
    Head thread_start(this, 0, 0, true);
    threadCreate(m_genes[0].label, thread_start);
  }
}


void Hardware::Types::GP8::Thread::Reset(GP8* in_hardware, const Head& start_pos)
{
  // Clear registers
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i].Clear();
  
  // Reset the heads (read/write in genome, others at start_pos)
  heads[hREAD].Reset(in_hardware, 0, 0, false);
  heads[hWRITE].Reset(in_hardware, 0, 0, false);
  heads[hIP] = start_pos;
  for (int i = hFLOW; i < NUM_HEADS; i++) heads[i] = start_pos;
  
  // Clear the stack
  stack.Clear(in_hardware->GetInstSet().GetStackSize());
  cur_stack = 0;
  
  // Clear other flags and values
  reading_label = false;
  reading_seq = false;
  running = true;
  active = true;
  read_label.Clear();
  next_label.Clear();
  
  sensor_session.Clear();
}

bool Hardware::Types::GP8::ProcessCycleStep(Context& ctx, Update current_update, bool speculative)
{
  (void)ctx;
  (void)current_update;
  (void)speculative;
  
  return false;
}

void Hardware::Types::GP8::ProcessTimeStep(Context& ctx, Update current_update)
{
  m_hw_reset = false;
  
  // Update cycle counts
  m_cycle_count++;
  
  // Wake any stalled threads
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (!m_threads[i].active && m_threads[i].wait_reg == -1) m_threads[i].active = true;
  }
  
  // Reset hardware units
  m_hw_busy = 0;
  m_hw_queue_eat = false;
  m_hw_queue_move = false;
  m_hw_queue_rotate = false;
  
  m_hw_queued = 0;
  
  m_hw_queue_eat_threads.Resize(0);
  
  // Reset execution state
  m_cur_uop = 0;
  m_cur_thread = 0;
  
  // Execute specified number of micro ops per cpu cycle on each thread in a round robin fashion
  const int uop_ratio = m_inst_set->GetUOpsPerCycle();
  for (; m_cur_uop < uop_ratio; m_cur_uop++) {
    for (m_cur_thread = (m_cur_thread < m_threads.GetSize()) ? m_cur_thread : 0; m_cur_thread < m_threads.GetSize(); m_cur_thread++) {
      // Setup the hardware for the next instruction to be executed.
      
      // If the currently selected thread is inactive, proceed to the next thread
      if (!m_threads[m_cur_thread].running || !m_threads[m_cur_thread].active) continue;
      
      m_advance_ip = true;
      Head& ip = m_threads[m_cur_thread].heads[hIP];
      ip.Adjust();
      
      // Find the instruction to be executed
      const Instruction cur_inst = ip.GetInst();
      
      
      bool exec = true;
      int exec_success = 0;
      
      unsigned int inst_hw_units = m_hw_units[m_instset->LibIDOf(ip.GetInst())];
      
      // Check if this instruction needs hardware units that are busy
      if ((inst_hw_units & m_hw_busy)) {
        m_threads[m_cur_thread].active = false;
        m_threads[m_cur_thread].wait_reg = -1;
        continue;
      }
      
      // Test if costs have been paid and it is okay to execute this now...
      
      // record any failure due to costs being paid
      // before we try to execute the instruction, is this org currently paying precosts for it
      bool on_pause = IsPayingActiveCost(ctx, m_cur_thread);
      if (m_has_any_costs) exec = SingleProcess_PayPreCosts(ctx, cur_inst, m_cur_thread);
      if (!exec) exec_success = -1;
      
      // Now execute the instruction...
      bool rand_fail = false;
      if (exec == true) {
        // NOTE: This call based on the cur_inst must occur prior to instruction
        //       execution, because this instruction reference may be invalid after
        //       certain classes of instructions (namely divide instructions) @DMB
        const int addl_time_cost = m_inst_set->GetAddlTimeCost(cur_inst);
        
        // Prob of exec (moved from SingleProcess_PayCosts so that we advance IP after a fail)
        if ( m_inst_set->GetProbFail(cur_inst) > 0.0 ) {
          exec = !( ctx.GetRandom().P(m_inst_set->GetProbFail(cur_inst)) );
          rand_fail = !exec;
        }
        
        if (exec == true) {
          if (SingleProcess_ExecuteInst(ctx, cur_inst)) {
            SingleProcess_PayPostResCosts(ctx, cur_inst);
            SingleProcess_SetPostCPUCosts(ctx, cur_inst, m_cur_thread);
            // record execution success
            exec_success = 1;
            m_hw_busy |= inst_hw_units;
          }
        }
        
        // Check if the instruction just executed caused premature death, break out of execution if so
        if (phenotype.GetToDelete()) {
          if (m_tracer) m_tracer->TraceHardware(ctx, *this, false, true, exec_success);
          break;
        }
        
        // Some instruction (such as jump) may turn m_advance_ip off.  Usually
        // we now want to move to the next instruction in the memory.
        if (m_advance_ip == true) ip.Advance();
        
        // Pay the additional death_cost of the instruction now
        phenotype.IncTimeUsed(addl_time_cost);
      }
      
      // if using mini traces, report success or failure of execution
      if (m_tracer) m_tracer->TraceHardware(ctx, *this, false, true, exec_success);
      
      bool do_record = false;
      // record exec failed if the org just now started paying precosts
      if (exec_success == -1 && !on_pause) do_record = true;
      // if exec succeeded but was on pause before this execution, we already recorded it
      // otherwise we record what the org did
      else if (exec_success == 1 && !on_pause) do_record = true;
      // if random failure, we record 'what the org was trying to do'
      else if (rand_fail) do_record = true;
      // if exec failed because of something inside the instruction itself, record the attempt
      else if (exec_success == 0) do_record = true;
      if (do_record) {
        // this will differ from time used
        phenotype.IncNumExecs();
      }
      
      if (phenotype.GetToDelete()) {
        break;
      }
      
      if (m_hw_reset) break;
    }
    
    if (phenotype.GetToDelete() || m_hw_reset) break;
  }
  
  for (int hw_action = 0; hw_action < m_hw_queued && !phenotype.GetToDelete() && !m_hw_reset; hw_action++) {
    switch (m_hw_queue[hw_action]) {
      case aEAT:
        m_action_side_effect_queue = &m_hw_queue_eat_threads;
        m_organism->DoOutput(ctx, 0);
        m_hw_queue_eat_threads.Resize(0);
        m_action_side_effect_queue = NULL;
        break;
        
      case aMOVE:
        if (m_use_avatar) m_organism->MoveAV(ctx);
        else m_organism->Move(ctx);
        break;
        
      case aROTATE:
        for (int i = 0; i < m_hw_queue_rotate_num; i++) m_organism->Rotate(ctx, m_hw_queue_rotate_reverse ? -1 : 1);
        break;
        
      default:
        break;
    }
  }
  
  // Kill creatures who have reached their max num of instructions executed
  const int max_executed = m_organism->GetMaxExecuted();
  if ((max_executed > 0 && phenotype.GetTimeUsed() >= max_executed) || phenotype.GetToDie() == true) {
    if (speculative) m_spec_die = true;
    else m_organism->Die(ctx);
  }
  if (!speculative && phenotype.GetToDelete()) m_spec_die = true;
  
  m_organism->SetRunning(false);
  CheckImplicitRepro(ctx);
  
  return true;
}


bool Hardware::Types::GP8::SingleProcess_ExecuteInst(Context& ctx, const Instruction& cur_inst)
{
  // Copy Instruction locally to handle stochastic effects
  Instruction actual_inst = cur_inst;
  
  // Get a pointer to the corresponding method...
  int inst_idx = m_inst_set->GetLibFunctionIndex(actual_inst);
  
  // Mark the instruction as executed
  getIP().SetFlagExecuted();
	
  // instruction execution count incremeneted
  m_organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
  
  // And execute it.
  const bool exec_success = (this->*(m_functions[inst_idx]))(ctx);
  
  // decremenet if the instruction was not executed successfully
  if (exec_success == false) {
    m_organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  }
  return exec_success;
}


void Hardware::Types::GP8::PrintStatus(ostream& fp)
{
  fp << "CPU CYCLE:" << m_organism->GetPhenotype().GetCPUCyclesUsed() << "."  << m_cur_uop << " ";
  fp << "THREAD:" << m_cur_thread << "  ";
  fp << "IP:" << getIP().Position() << " (" << GetInstSet().GetName(getIP().GetInst()) << ")" << endl;
  
  
  for (int i = 0; i < NUM_REGISTERS; i++) {
    DataValue& reg = m_threads[m_cur_thread].reg[i];
    fp << static_cast<char>('A' + i) << "X:" << reg.value << " ";
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
  
  fp << "  R-Head:" << getHead(hREAD).Position() << " "
  << "W-Head:" << getHead(hWRITE).Position()  << " "
  << "F-Head:" << getHead(hFLOW).Position()   << "  "
  << "RL:" << GetReadLabel().AsString() << "   "
  << "Ex:" << m_last_output
  << endl;
  
  int number_of_stacks = GetNumStacks();
  for (int stack_id = 0; stack_id < number_of_stacks; stack_id++) {
    fp << ((m_threads[m_cur_thread].cur_stack == stack_id) ? '*' : ' ') << " Stack " << stack_id << ":" << setbase(16) << setfill('0');
    for (int i = 0; i < m_inst_set->GetStackSize(); i++) fp << " Ox" << setw(8) << GetStack(i, stack_id, 0);
    fp << setfill(' ') << setbase(10) << endl;
  }
  
  for (int i = 0; i < m_mem_array.GetSize(); i++) {
    const InstMemSpace& mem = m_mem_array[i];
    fp << "  Mem " << i << " (" << mem.GetSize() << "): " << mem.AsString() << endl;
  }
  
  for (int i = 0; i < m_genes.GetSize(); i++) {
    const InstMemSpace& mem = m_genes[i].memory;
    fp << "  Gene " << i << " (" << mem.GetSize() << "): " << mem.AsString() << endl;
  }
  
  fp.flush();
}



void Hardware::Types::GP8::FindLabelStart(Head& head, Head& default_pos, bool mark_executed)
{
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) {
    head.Set(default_pos);
    return;
  }
  
  InstMemSpace& memory = head.GetMemory();
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
        head.SetPosition(pos - 1);
        return;
      }
      
      continue;
    }
    pos++;
  }
  
  // Return start point if not found
  head.Set(default_pos);
}

void Hardware::Types::GP8::FindNopSequenceStart(Head& head, Head& default_pos, bool mark_executed)
{
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) {
    head.Set(default_pos);
    return;
  }
  
  InstMemSpace& memory = head.GetMemory();
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
        head.SetPosition(pos - 1);
        return;
      }
    }
    pos++;
  }
  
  // Return start point if not found
  head.Set(default_pos);
}


void Hardware::Types::GP8::FindLabelForward(Head& head, Head& default_pos, bool mark_executed)
{
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) {
    head.Set(default_pos);
    return;
  }
  
  head.Adjust();
  
  Head pos(head);
  pos++;
  
  while (pos.Position() != head.Position()) {
    if (m_inst_set->IsLabel(pos.GetInst())) { // starting label found
      const int label_start = pos.Position();
      pos++;
      
      // Check for direct matched label pattern, can be substring of 'label'ed target
      // - must match all NOPs in search_label
      // - extra NOPs in 'label'ed target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.Position() != head.Position()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops following the 'label' instruction
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.Position();
        
        if (mark_executed) {
          pos.SetPosition(label_start);
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get() + 1; // Max label + 1 for the label instruction itself
          for (int i = 0; i < size_matched && i < max; i++, pos++) pos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        head.SetPosition(found_pos);
        return;
      }
      
      continue;
    }
    pos++;
  }
  
  // Return start point if not found
  head.Set(default_pos);
}

void Hardware::Types::GP8::FindLabelBackward(Head& head, Head& default_pos, bool mark_executed)
{
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) {
    head.Set(default_pos);
    return;
  }
  
  head.Adjust();
  
  Head lpos(head);
  Head pos(head);
  lpos--;
  
  while (pos.Position() != head.Position()) {
    if (m_inst_set->IsLabel(lpos.GetInst())) { // starting label found
      pos.SetPosition(lpos.Position());
      pos++;
      
      // Check for direct matched label pattern, can be substring of 'label'ed target
      // - must match all NOPs in search_label
      // - extra NOPs in 'label'ed target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.Position() != head.Position()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops following the 'label' instruction
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.Position();
        
        if (mark_executed) {
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get() + 1; // Max label + 1 for the label instruction itself
          for (int i = 0; i < size_matched && i < max; i++, lpos++) lpos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        head.SetPosition(found_pos);
        return;
      }
    }
    lpos--;
  }
  
  // Return start point if not found
  head.Set(default_pos);
}




void Hardware::Types::GP8::FindNopSequenceForward(Head& head, Head& default_pos, bool mark_executed)
{
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) {
    head.Set(default_pos);
    return;
  }
  
  head.Adjust();
  
  Head pos(head);
  pos++;
  
  while (pos.Position() != head.Position()) {
    if (m_inst_set->IsNop(pos.GetInst())) { // starting label found
      const int label_start = pos.Position();
      
      // Check for direct matched nop sequence, can be substring of target
      // - must match all NOPs in search_label
      // - extra NOPs in target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.Position() != head.Position()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.Position();
        
        if (mark_executed) {
          pos.SetPosition(label_start);
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get();
          for (int i = 0; i < size_matched && i < max; i++, pos++) pos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        head.SetPosition(found_pos);
        return;
      }
    }
    
    if (pos.Position() == head.Position()) break;
    pos++;
  }
  
  // Return start point if not found
  head.Set(default_pos);
}


void Hardware::Types::GP8::FindNopSequenceBackward(Head& head, Head& default_pos, bool mark_executed)
{
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) {
    head.Set(default_pos);
    return;
  }
  
  head.Adjust();
  
  Head lpos(head);
  Head pos(head);
  lpos--;
  
  while (pos.Position() != head.Position()) {
    if (m_inst_set->IsNop(pos.GetInst())) { // starting label found
      pos.SetPosition(lpos.Position());
      
      // Check for direct matched nop sequence, can be substring of target
      // - must match all NOPs in search_label
      // - extra NOPs in target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.Position() != head.Position()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.Position();
        
        if (mark_executed) {
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get();
          for (int i = 0; i < size_matched && i < max; i++, lpos++) lpos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        head.SetPosition(found_pos);
        return;
      }
      
      continue;
    }
    lpos--;
  }
  
  // Return start point if not found
  head.Set(default_pos);
}

void Hardware::Types::GP8::ReadInst(Instruction in_inst)
{
  bool is_nop = m_inst_set->IsNop(in_inst);
  
  if (m_inst_set->IsLabel(in_inst)) {
    GetReadLabel().Clear();
    m_threads[m_cur_thread].reading_label = true;
  } else if (m_threads[m_cur_thread].reading_label && is_nop) {
    GetReadLabel().AddNop(in_inst.GetOp());
  } else {
    GetReadLabel().Clear();
    m_threads[m_cur_thread].reading_label = false;
  }
  
  if (!m_threads[m_cur_thread].reading_seq && is_nop) {
    GetReadSequence().AddNop(in_inst.GetOp());
    m_threads[m_cur_thread].reading_seq = true;
  } else if (m_threads[m_cur_thread].reading_seq && is_nop) {
    GetReadSequence().AddNop(in_inst.GetOp());
  } else {
    GetReadSequence().Clear();
    m_threads[m_cur_thread].reading_seq = false;
  }
}


// This function looks at the current position in the info of the organism and sets the next_label to be the sequence of nops
// which follows.  The instruction pointer is left on the last line of the label found.

void Hardware::Types::GP8::readLabel(Head& head, NopSequence& label)
{
  int count = 0;
  
  label.Clear();
  
  while (m_inst_set->IsNop(head.NextInst()) && (count < label.MaxSize())) {
    count++;
    head.Advance();
    label.AddNop(m_inst_set->GetNopMod(head.GetInst()));
    
    // If this is the first line of the template, mark it executed.
    if (label.GetSize() <=	m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get()) head.SetFlagExecuted();
  }
}

void Hardware::Types::GP8::threadCreate(const NopSequence& thread_label, const Head& start_pos)
{
  // Check for existing thread
  if (thread_label.GetSize() > 0) {
    for (int thread_idx = 0; thread_idx < m_threads.GetSize(); thread_idx++) {
      if (m_threads[thread_idx].thread_label == thread_label) {
        if (!m_threads[thread_idx].running) {
          m_threads[thread_idx].Reset(this, start_pos);
          m_running_threads++;
        }
        return;
      }
    }
  }
  
  
  // Create new thread
  int thread_id = m_threads.GetSize();
  m_threads.Resize(thread_id + 1);
  m_threads[thread_id].thread_label = thread_label;
  m_threads[thread_id].Reset(this, start_pos);
  m_running_threads++;
  
  m_organism->GetPhenotype().SetIsMultiThread();
}


// Instruction Helpers
// --------------------------------------------------------------------------------------------------------------

inline int Hardware::Types::GP8::FindModifiedRegister(int default_register, bool accept_immediate)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  Instruction inst = getIP().NextInst();
  if (m_inst_set->IsNop(inst)) {
    getIP().Advance();
    default_register = m_inst_set->GetNopMod(inst);
    getIP().SetFlagExecuted();
  } else if (accept_immediate && m_inst_set->IsImmediateValue(inst)) {
    getIP().Advance();
    default_register = m_inst_set->GetLibFunctionIndex(inst);
    getIP().SetFlagExecuted();
  }
  return default_register;
}

inline int Hardware::Types::GP8::FindModifiedNextRegister(int default_register, bool accept_immediate)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  Instruction inst = getIP().NextInst();
  if (m_inst_set->IsNop(inst)) {
    getIP().Advance();
    default_register = m_inst_set->GetNopMod(inst);
    getIP().SetFlagExecuted();
  } else if (accept_immediate && m_inst_set->IsImmediateValue(inst)) {
    getIP().Advance();
    default_register = m_inst_set->GetLibFunctionIndex(inst);
    getIP().SetFlagExecuted();
  } else {
    default_register = (default_register + 1) % NUM_REGISTERS;
  }
  return default_register;
}

inline int Hardware::Types::GP8::FindModifiedPreviousRegister(int default_register, bool accept_immediate)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  Instruction inst = getIP().NextInst();
  if (m_inst_set->IsNop(inst)) {
    getIP().Advance();
    default_register = m_inst_set->GetNopMod(inst);
    getIP().SetFlagExecuted();
  } else if (accept_immediate && m_inst_set->IsImmediateValue(inst)) {
    getIP().Advance();
    default_register = m_inst_set->GetLibFunctionIndex(inst);
    getIP().SetFlagExecuted();
  } else {
    default_register = (default_register + NUM_REGISTERS - 1) % NUM_REGISTERS;
  }
  return default_register;
}


inline int Hardware::Types::GP8::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.
  
  if (m_inst_set->IsNop(getIP().NextInst())) {
    getIP().Advance();
    default_head = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_head;
}


inline int Hardware::Types::GP8::FindNextRegister(int base_reg)
{
  return (base_reg + 1) % NUM_REGISTERS;
}

inline int Hardware::Types::GP8::FindUpstreamModifiedRegister(int offset, int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  assert(offset >= 0);
  
  Head location = getIP();
  location.Jump(-offset - 1);
  
  if (m_inst_set->IsNop(location.GetInst())) {
    default_register = m_inst_set->GetNopMod(location.GetInst());
    location.SetFlagExecuted();
  }
  return default_register;
}


int Hardware::Types::GP8::calcCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  const InstMemSpace& memory = m_mem_array[m_cur_offspring];
  for (int i = 0; i < memory.GetSize(); i++) {
    if (memory.FlagCopied(i)) copied_size++;
	}
  return copied_size;
}


bool Hardware::Types::GP8::Divide_Main(Context& ctx, int mem_space_used, int write_head_pos, double mut_multiplier)
{
  // Make sure the memory space we're using exists
  if (m_mem_array.GetSize() <= mem_space_used) return false;
  
  // Make sure this divide will produce a viable offspring.
  m_cur_offspring = mem_space_used; // save current child memory space for use by dependent functions (e.g. calcCopiedSize())
  if (!Divide_CheckViable(ctx, m_mem_array[0].GetSize(), write_head_pos)) return false;
  
  // Since the divide will now succeed, set up the information to be sent to the new organism
  m_mem_array[mem_space_used].Resize(write_head_pos);
  
  InstructionSequencePtr offspring_seq(new InstructionSequence(m_mem_array[mem_space_used]));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)m_inst_set->GetInstSetName());
  Genome offspring(GetType(), props, offspring_seq);
  
  m_organism->OffspringGenome() = offspring;
	
  // Handle Divide Mutations...
  Divide_DoMutations(ctx, mut_multiplier);
	
  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures(ctx);
	
  // reset first time instruction costs
  for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
    m_inst_ft_cost[i] = m_inst_set->GetFTCost(Instruction(i));
  }
	
  //bool parent_alive = m_organism->ActivateDivide(ctx);
  bool parent_alive = m_organism->ActivateDivide(ctx);
  //reset the memory of the memory space that has been divided off
  m_mem_array[mem_space_used] = InstructionSequence("a");
	
  // Division Methods:
  // 0 - DIVIDE_METHOD_OFFSPRING - Create a child, leave parent state untouched.
  // 1 - DIVIDE_METHOD_SPLIT - Create a child, completely reset state of parent.
  // 2 - DIVIDE_METHOD_BIRTH - Create a child, reset state of parent's current thread.
  
  if (parent_alive) { // If the parent is no longer alive, all of this is moot
    switch (m_world->GetConfig().DIVIDE_METHOD.Get()) {
      case DIVIDE_METHOD_SPLIT:
        Reset(ctx);
        break;
        
      case DIVIDE_METHOD_BIRTH:
        // Reset only the calling thread's state
        for(int x = 0; x < NUM_HEADS; x++) getHead(x).Reset(this, 0, 0, false);
        for(int x = 0; x < NUM_REGISTERS; x++) setRegister(x, 0, false);
        if (m_world->GetConfig().INHERIT_MERIT.Get() == 0) m_organism->GetPhenotype().ResetMerit();
        break;
        
      case DIVIDE_METHOD_OFFSPRING:
      default:
        break;
		}
		m_advance_ip = false;
	}
	
  return true;
}


void Hardware::Types::GP8::checkWaitingThreads(int cur_thread, int reg_num)
{
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (i != cur_thread && !m_threads[i].active && int(m_threads[i].wait_reg) == reg_num) {
      int wait_value = m_threads[i].wait_value;
      int check_value = m_threads[cur_thread].reg[reg_num].value;
      if ((m_threads[i].wait_greater && check_value > wait_value) ||
          (m_threads[i].wait_equal && check_value == wait_value) ||
          (m_threads[i].wait_less && check_value < wait_value)) {
        
        // Wake up the thread with matched condition
        m_threads[i].active = true;
        m_waiting_threads--;
        
        // Set destination register to be the check value
        DataValue& dest = m_threads[i].reg[m_threads[i].wait_dst];
        dest.value = check_value;
        dest.from_env = false;
        dest.originated = m_cycle_count;
        dest.oldest_component = m_threads[cur_thread].reg[reg_num].oldest_component;
        dest.env_component = m_threads[cur_thread].reg[reg_num].env_component;
        
        // Cascade check
        if (m_waiting_threads) checkWaitingThreads(i, m_threads[i].wait_dst);
      }
    }
  }
}


// Instructions
// --------------------------------------------------------------------------------------------------------------

bool Hardware::Types::GP8::Inst_Nop(Context& ctx)
{
  (void)ctx;
  return true;
}

// Multi-threading.
bool Hardware::Types::GP8::Inst_Yield(Context&)
{
  m_threads[m_cur_thread].active = false;
  m_threads[m_cur_thread].wait_reg = -1;
  return true;
}


bool Hardware::Types::GP8::Inst_RegulatePause(Context&)
{
  readLabel(getIP(), m_threads[m_cur_thread].next_label);
  if (m_threads[m_cur_thread].next_label.GetSize() == 0) return false;
  for (ThreadLabelIterator it(this, m_threads[m_cur_thread].next_label, false); it.Next() >= 0;) {
    m_threads[it.Get()].running = false;
  }
  return true;
}


bool Hardware::Types::GP8::Inst_RegulateResume(Context&)
{
  readLabel(getIP(), m_threads[m_cur_thread].next_label);
  if (m_threads[m_cur_thread].next_label.GetSize() == 0) return false;
  for (ThreadLabelIterator it(this, m_threads[m_cur_thread].next_label, false); it.Next() >= 0;) {
    m_threads[it.Get()].running = true;
  }
  return true;
}


bool Hardware::Types::GP8::Inst_RegulateReset(Context&)
{
  readLabel(getIP(), m_threads[m_cur_thread].next_label);
  if (m_threads[m_cur_thread].next_label.GetSize() == 0) return false;
  for (ThreadLabelIterator it(this, m_threads[m_cur_thread].next_label, false); it.Next() >= 0;) {
    Head& thread_hIP = m_threads[it.Get()].heads[hIP];
    Head thread_start(this, 0, thread_hIP.MemSpaceIndex(), thread_hIP.MemSpaceIsGene());
    m_threads[it.Get()].Reset(this, thread_start);
  }
  return true;
}



bool Hardware::Types::GP8::Inst_Label(Context&)
{
  readLabel(getIP(), GetLabel());
  return true;
}

bool Hardware::Types::GP8::Inst_IfNEqu(Context& ctx) // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedRegister(rBX, true);
  const int op2 = FindModifiedNextRegister((op1 < NUM_REGISTERS) ? op1 : rBX, true);
  if (getRegister(ctx, op1) == getRegister(ctx, op2))  getIP().Advance();
  return true;
}

bool Hardware::Types::GP8::Inst_IfLess(Context& ctx) // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedRegister(rBX, true);
  const int op2 = FindModifiedNextRegister((op1 < NUM_REGISTERS) ? op1 : rBX, true);
  if (getRegister(ctx, op1) >=  getRegister(ctx, op2))  getIP().Advance();
  return true;
}

bool Hardware::Types::GP8::Inst_IfNotZero(Context& ctx)  // Execute next if ?bx? != 0
{
  const int op1 = FindModifiedRegister(rBX, true);
  if (getRegister(ctx, op1) == 0)  getIP().Advance();
  return true;
}
bool Hardware::Types::GP8::Inst_IfEqualZero(Context& ctx)  // Execute next if ?bx? == 0
{
  const int op1 = FindModifiedRegister(rBX, true);
  if (getRegister(ctx, op1) != 0)  getIP().Advance();
  return true;
}
bool Hardware::Types::GP8::Inst_IfGreaterThanZero(Context& ctx)  // Execute next if ?bx? > 0
{
  const int op1 = FindModifiedRegister(rBX, true);
  if (getRegister(ctx, op1) <= 0)  getIP().Advance();
  return true;
}

bool Hardware::Types::GP8::Inst_IfLessThanZero(Context& ctx)  // Execute next if ?bx? < 0
{
  const int op1 = FindModifiedRegister(rBX, true);
  if (getRegister(ctx, op1) >= 0)  getIP().Advance();
  return true;
}


bool Hardware::Types::GP8::Inst_Pop(Context&)
{
  const int reg_used = FindModifiedRegister(rBX, true);
  DataValue pop = stackPop();
  setRegister(reg_used, pop.value, pop);
  return true;
}

bool Hardware::Types::GP8::Inst_Push(Context& ctx)
{
  const int reg_used = FindModifiedRegister(rBX, true);
  getStack(m_threads[m_cur_thread].cur_stack).Push(getRegisterData(ctx, reg_used));
  return true;
}

bool Hardware::Types::GP8::Inst_PopAll(Context&)
{
  int reg_used = FindModifiedRegister(rBX);
  for (int i = 0; i < NUM_REGISTERS; i++) {
    DataValue pop = stackPop();
    setRegister(reg_used, pop.value, pop);
    reg_used++;
    if (reg_used == NUM_REGISTERS) reg_used = 0;
  }
  return true;
}

bool Hardware::Types::GP8::Inst_PushAll(Context&)
{
  int reg_used = FindModifiedRegister(rBX);
  for (int i = 0; i < NUM_REGISTERS; i++) {
    getStack(m_threads[m_cur_thread].cur_stack).Push(m_threads[m_cur_thread].reg[reg_used]);
    reg_used++;
    if (reg_used == NUM_REGISTERS) reg_used = 0;
  }
  return true;
}

bool Hardware::Types::GP8::Inst_SwitchStack(Context&) { switchStack(); return true; }

bool Hardware::Types::GP8::Inst_Swap(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  DataValue v1 = m_threads[m_cur_thread].reg[op1];
  m_threads[m_cur_thread].reg[op1] = m_threads[m_cur_thread].reg[op2];
  m_threads[m_cur_thread].reg[op2] = v1;
  return true;
}

bool Hardware::Types::GP8::Inst_ShiftR(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setRegister(reg_used, m_threads[m_cur_thread].reg[reg_used].value >> 1,
              m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool Hardware::Types::GP8::Inst_ShiftL(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setRegister(reg_used, m_threads[m_cur_thread].reg[reg_used].value << 1,
              m_threads[m_cur_thread].reg[reg_used]);
  return true;
}


bool Hardware::Types::GP8::Inst_Inc(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setRegister(reg_used, m_threads[m_cur_thread].reg[reg_used].value + 1,
              m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool Hardware::Types::GP8::Inst_Dec(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setRegister(reg_used, m_threads[m_cur_thread].reg[reg_used].value - 1,
              m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool Hardware::Types::GP8::Inst_Zero(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setRegister(reg_used, 0, false);
  return true;
}

bool Hardware::Types::GP8::Inst_One(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setRegister(reg_used, 1, false);
  return true;
}

bool Hardware::Types::GP8::Inst_MaxInt(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setRegister(reg_used, std::numeric_limits<int>::max(), false);
  return true;
}

bool Hardware::Types::GP8::Inst_Rand(Context& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  int randsign = ctx.GetRandom().GetUInt(0,2) ? -1 : 1;
  setRegister(reg_used, ctx.GetRandom().GetInt(std::numeric_limits<int>::max()) * randsign, false);
  return true;
}



int Hardware::Types::GP8::Val_Zero(Context&)
{
  return 0;
}

int Hardware::Types::GP8::Val_One(Context&)
{
  return 1;
}

int Hardware::Types::GP8::Val_MaxInt(Context&)
{
  return std::numeric_limits<int>::max();
}

int Hardware::Types::GP8::Val_Rand(Context& ctx)
{
  int randsign = ctx.GetRandom().GetUInt(0,2) ? -1 : 1;
  return ctx.GetRandom().GetInt(std::numeric_limits<int>::max()) * randsign;
}


bool Hardware::Types::GP8::Inst_Add(Context& ctx)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst, true);
  const int op2 = FindModifiedNextRegister((op1 < NUM_REGISTERS) ? op1 : dst, true);
  DataValue r1 = getRegisterData(ctx, op1);
  DataValue r2 = getRegisterData(ctx, op2);
  setRegister(dst, r1.value + r2.value, r1, r2);
  return true;
}

bool Hardware::Types::GP8::Inst_Sub(Context& ctx)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst, true);
  const int op2 = FindModifiedNextRegister((op1 < NUM_REGISTERS) ? op1 : dst, true);
  DataValue r1 = getRegisterData(ctx, op1);
  DataValue r2 = getRegisterData(ctx, op2);
  setRegister(dst, r1.value - r2.value, r1, r2);
  return true;
}

bool Hardware::Types::GP8::Inst_Mult(Context& ctx)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst, true);
  const int op2 = FindModifiedNextRegister((op1 < NUM_REGISTERS) ? op1 : dst, true);
  DataValue r1 = getRegisterData(ctx, op1);
  DataValue r2 = getRegisterData(ctx, op2);
  setRegister(dst, r1.value * r2.value, r1, r2);
  return true;
}

bool Hardware::Types::GP8::Inst_Div(Context& ctx)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst, true);
  const int op2 = FindModifiedNextRegister((op1 < NUM_REGISTERS) ? op1 : dst, true);
  DataValue r1 = getRegisterData(ctx, op1);
  DataValue r2 = getRegisterData(ctx, op2);
  if (r2.value != 0) {
    if (!(0 - INT_MAX > r1.value && r2.value == -1)) setRegister(dst, r1.value / r2.value, r1, r2);
  } else {
    return false;
  }
  return true;
}

bool Hardware::Types::GP8::Inst_Mod(Context& ctx)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst, true);
  const int op2 = FindModifiedNextRegister((op1 < NUM_REGISTERS) ? op1 : dst, true);
  DataValue r1 = getRegisterData(ctx, op1);
  DataValue r2 = getRegisterData(ctx, op2);
  if (r2.value != 0) {
    setRegister(dst, r1.value % r2.value, r1, r2);
  } else {
    return false;
  }
  return true;
}


bool Hardware::Types::GP8::Inst_Nand(Context& ctx)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst, true);
  const int op2 = FindModifiedNextRegister((op1 < NUM_REGISTERS) ? op1 : dst, true);
  DataValue r1 = getRegisterData(ctx, op1);
  DataValue r2 = getRegisterData(ctx, op2);
  setRegister(dst, ~(r1.value & r2.value), r1, r2);
  return true;
}

bool Hardware::Types::GP8::Inst_SetMemory(Context& ctx)
{
  int mem_label = FindModifiedRegister(rBX);
  
  int mem_id = m_mem_ids[mem_label];
  
  // Check for existing mem_space
  if (mem_id < 0) {
    mem_id = m_mem_array.GetSize();
    m_mem_array.Resize(mem_id + 1);
    m_mem_ids[mem_label] = mem_id;
  }
  
  m_threads[m_cur_thread].heads[hWRITE].Set(0, mem_id, false);
  return true;
}


bool Hardware::Types::GP8::Inst_TaskInput(Context&)
{
  const int reg_used = FindModifiedRegister(rBX, true);
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setRegister(reg_used, value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}

bool Hardware::Types::GP8::Inst_TaskOutput(Context& ctx)
{
  const int reg_used = FindModifiedRegister(rBX, true);
  DataValue reg = getRegisterData(ctx, reg_used);
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  return true;
}



bool Hardware::Types::GP8::Inst_MoveHead(Context&)
{
  const int head_used = FindModifiedHead(hIP);
  int target = FindModifiedHead(hFLOW);
  
  // Cannot move to the read/write heads, acts as move to flow head instead
  if (target == hWRITE && head_used != hREAD) target = hFLOW;
  if (target == hREAD && head_used != hWRITE) target = hFLOW;
  
  getHead(head_used).Set(getHead(target));
  if (head_used == hIP) m_advance_ip = false;
  return true;
}

bool Hardware::Types::GP8::Inst_JumpHead(Context& ctx)
{
  const int head_used = FindModifiedHead(hIP);
  const int reg = FindModifiedRegister(rCX, true);
  getHead(head_used).Jump(getRegister(ctx, reg));
  if (head_used == hIP) m_advance_ip = false;
  return true;
}

bool Hardware::Types::GP8::Inst_GetHead(Context&)
{
  const int head_used = FindModifiedHead(hIP);
  const int reg = FindModifiedRegister(rCX);
  setRegister(reg, getHead(head_used).Position());
  return true;
}

bool Hardware::Types::GP8::Inst_DidCopyLabel(Context&)
{
  readLabel(getIP(), GetLabel());
  setRegister(rBX, (GetLabel() == GetReadLabel()), false);
  return true;
}

bool Hardware::Types::GP8::Inst_DivideMemory(Context& ctx)
{
  int mem_space_used = FindModifiedRegister(rBX);
  
  if (mem_space_used < rBX || m_mem_ids[mem_space_used]  < 0) return false;
  
  mem_space_used = m_mem_ids[mem_space_used];
  int end_of_memory = m_mem_array[mem_space_used].GetSize() - 1;
  
  return Divide_Main(ctx, mem_space_used, end_of_memory, 1.0);
}

bool Hardware::Types::GP8::Inst_HeadRead(Context& ctx)
{
  const int head_id = FindModifiedHead(hREAD);
  const int dst = FindModifiedRegister(rAX);
  getHead(head_id).Adjust();
  
  // Mutations only occur on the read, for the moment.
  Instruction read_inst;
  if (m_organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx);
  } else {
    read_inst = getHead(head_id).GetInst();
  }
  setRegister(dst, read_inst.GetOp());
  ReadInst(read_inst);
  
  if (m_slip_read_head && m_organism->TestCopySlip(ctx))
    getHead(head_id).SetPosition(ctx.GetRandom().GetInt(getHead(head_id).GetMemory().GetSize()));
  
  getHead(head_id).Advance();
  return true;
}

bool Hardware::Types::GP8::Inst_HeadWrite(Context& ctx)
{
  const int head_id = FindModifiedHead(hWRITE);
  const int src = FindModifiedRegister(rAX);
  Head& active_head = getHead(head_id);
  
  InstMemSpace& memory = active_head.GetMemory();
  
  if (active_head.Position() >= memory.GetSize() - 1) {
		memory.Resize(memory.GetSize() + 1);
		memory.Copy(memory.GetSize() - 1, memory.GetSize() - 2);
	}
  
  active_head.Adjust();
  
  int value = m_threads[m_cur_thread].reg[src].value;
  if (value < 0 || value >= m_inst_set->GetSize()) value = 0;
  
  active_head.SetInst(Instruction(value));
  active_head.SetFlagCopied();
  
  if (m_organism->TestCopyIns(ctx)) active_head.InsertInst(m_inst_set->GetRandomInst(ctx));
  if (m_organism->TestCopyDel(ctx)) active_head.RemoveInst();
  //  if (m_organism->TestCopyUniform(ctx)) doUniformCopyMutation(ctx, active_head);
  if (!m_slip_read_head && m_organism->TestCopySlip(ctx))
    doSlipMutation(ctx, active_head.GetMemory(), active_head.Position());
  
  // Advance the head after write...
  active_head++;
  return true;
}

bool Hardware::Types::GP8::Inst_HeadCopy(Context& ctx)
{
  // For the moment, this cannot be nop-modified.
  Head& read_head = getHead(hREAD);
  Head& write_head = getHead(hWRITE);
  
  InstMemSpace& memory = write_head.GetMemory();
  
  if (write_head.Position() >= memory.GetSize() - 1) {
		memory.Resize(memory.GetSize() + 1);
		memory.Copy(memory.GetSize() - 1, memory.GetSize() - 2);
	}
  
  
  read_head.Adjust();
  write_head.Adjust();
  
  // Do mutations.
  Instruction read_inst = read_head.GetInst();
  ReadInst(read_inst);
  if (m_organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx);
    write_head.SetFlagMutated();
    write_head.SetFlagCopyMut();
  }
  
  write_head.SetInst(read_inst);
  write_head.SetFlagCopied();  // Set the copied flag...
  
  if (m_organism->TestCopyIns(ctx)) write_head.InsertInst(m_inst_set->GetRandomInst(ctx));
  if (m_organism->TestCopyDel(ctx)) write_head.RemoveInst();
  //  if (m_organism->TestCopyUniform(ctx)) doUniformCopyMutation(ctx, write_head);
  if (m_organism->TestCopySlip(ctx)) {
    if (m_slip_read_head) {
      read_head.SetPosition(ctx.GetRandom().GetInt(read_head.GetMemory().GetSize()));
    } else
      doSlipMutation(ctx, write_head.GetMemory(), write_head.Position());
  }
  
  read_head.Advance();
  write_head.Advance();
  return true;
}

bool Hardware::Types::GP8::Inst_Search_Label_S(Context&)
{
  readLabel(getIP(), GetLabel());
  FindLabelStart(getHead(hFLOW), getIP(), true);
  getHead(hFLOW).Advance();
  return true;
}

bool Hardware::Types::GP8::Inst_Search_Label_D(Context& ctx)
{
  readLabel(getIP(), GetLabel());
  int direction = getRegister(ctx, rBX);
  if (direction == 0) {
    FindLabelStart(getHead(hFLOW), getIP(), true);
  } else if (direction < 0) {
    FindLabelBackward(getHead(hFLOW), getIP(), true);
  } else {
    FindLabelForward(getHead(hFLOW), getIP(), true);
  }
  getHead(hFLOW).Advance();
  return true;
}

bool Hardware::Types::GP8::Inst_Search_Seq_D(Context& ctx)
{
  readLabel(getIP(), GetLabel());
  GetLabel().Rotate(1, NUM_NOPS);
  int direction = getRegister(ctx, rBX);
  if (direction == 0) {
    FindNopSequenceStart(getHead(hFLOW), getIP(), true);
  } else if (direction < 0) {
    FindNopSequenceBackward(getHead(hFLOW), getIP(), true);
  } else {
    FindNopSequenceForward(getHead(hFLOW), getIP(), true);
  }
  getHead(hFLOW).Advance();
  return true;
}


bool Hardware::Types::GP8::Inst_WaitCondition_Equal(Context& ctx)
{
  const int wait_value_reg = FindModifiedRegister(rBX, true);
  const int check_reg = FindModifiedRegister(rHX);
  const int wait_dst = FindModifiedRegister((wait_value_reg < NUM_REGISTERS) ? wait_value_reg : rBX);
  
  const int wait_value = getRegister(ctx, wait_value_reg);
  
  // Check if condition has already been met
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (i != m_cur_thread && m_threads[i].reg[check_reg].value == wait_value) {
      setRegister(wait_dst, m_threads[i].reg[check_reg].value, m_threads[i].reg[check_reg]);
      return true;
    }
  }
  
  // Fail to sleep if this is the last thread awake
  if (m_waiting_threads == m_running_threads) return false;
  
  // Put thread to sleep with appropriate wait condition
  m_threads[m_cur_thread].active = false;
  m_waiting_threads++;
  m_threads[m_cur_thread].wait_equal = true;
  m_threads[m_cur_thread].wait_less = false;
  m_threads[m_cur_thread].wait_greater = false;
  m_threads[m_cur_thread].wait_reg = check_reg;
  m_threads[m_cur_thread].wait_value = wait_value;
  m_threads[m_cur_thread].wait_dst = wait_dst;
  
  return true;
}

bool Hardware::Types::GP8::Inst_WaitCondition_Less(Context& ctx)
{
  const int wait_value_reg = FindModifiedRegister(rBX, true);
  const int check_reg = FindModifiedRegister(rHX);
  const int wait_dst = FindModifiedRegister((wait_value_reg < NUM_REGISTERS) ? wait_value_reg : rBX);
  
  const int wait_value = getRegister(ctx, wait_value_reg);
  
  // Check if condition has already been met
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (i != m_cur_thread && m_threads[i].reg[check_reg].value < wait_value) {
      setRegister(wait_dst, m_threads[i].reg[check_reg].value, m_threads[i].reg[check_reg]);
      return true;
    }
  }
  
  // Fail to sleep if this is the last thread awake
  if (m_waiting_threads == m_running_threads) return false;
  
  // Put thread to sleep with appropriate wait condition
  m_threads[m_cur_thread].active = false;
  m_waiting_threads++;
  m_threads[m_cur_thread].wait_equal = false;
  m_threads[m_cur_thread].wait_less = true;
  m_threads[m_cur_thread].wait_greater = false;
  m_threads[m_cur_thread].wait_reg = check_reg;
  m_threads[m_cur_thread].wait_value = wait_value;
  m_threads[m_cur_thread].wait_dst = wait_dst;
  
  return true;
}

bool Hardware::Types::GP8::Inst_WaitCondition_Greater(Context& ctx)
{
  const int wait_value_reg = FindModifiedRegister(rBX, true);
  const int check_reg = FindModifiedRegister(rHX);
  const int wait_dst = FindModifiedRegister((wait_value_reg < NUM_REGISTERS) ? wait_value_reg : rBX);
  
  const int wait_value = getRegister(ctx, wait_value_reg);
  
  // Check if condition has already been met
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (i != m_cur_thread && m_threads[i].reg[check_reg].value > wait_value) {
      setRegister(wait_dst, m_threads[i].reg[check_reg].value, m_threads[i].reg[check_reg]);
      return true;
    }
  }
  
  // Fail to sleep if this is the last thread awake
  if (m_waiting_threads == m_running_threads) return false;
  
  // Put thread to sleep with appropriate wait condition
  m_threads[m_cur_thread].active = false;
  m_waiting_threads++;
  m_threads[m_cur_thread].wait_equal = false;
  m_threads[m_cur_thread].wait_less = false;
  m_threads[m_cur_thread].wait_greater = true;
  m_threads[m_cur_thread].wait_reg = check_reg;
  m_threads[m_cur_thread].wait_value = wait_value;
  m_threads[m_cur_thread].wait_dst = wait_dst;
  
  return true;
}

bool Hardware::Types::GP8::Inst_Repro(Context& ctx)
{
  // these checks should be done, but currently they make some assumptions
  // that crash when evaluating this kind of organism -- JEB
  
  InstMemSpace& memory = m_mem_array[0];
  
  if (m_organism->GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  InstructionSequencePtr offspring_seq(new InstructionSequence(memory));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)m_inst_set->GetInstSetName());
  Genome offspring(GetType(), props, offspring_seq);
  
  m_organism->OffspringGenome() = offspring;
  m_organism->GetPhenotype().SetLinesCopied(memory.GetSize());
  
  int lines_executed = 0;
  for (int i = 0; i < memory.GetSize(); i++) if (memory.FlagExecuted(i)) lines_executed++;
  m_organism->GetPhenotype().SetLinesExecuted(lines_executed);
  
  const Genome& org = m_organism->GetGenome();
  ConstInstructionSequencePtr org_seq_p;
  org_seq_p.DynamicCastFrom(org.Representation());
  const InstructionSequence& org_genome = *org_seq_p;
  
  Genome& child = m_organism->OffspringGenome();
  InstructionSequencePtr child_seq_p;
  child_seq_p.DynamicCastFrom(child.Representation());
  InstructionSequence& child_seq = *child_seq_p;
  
  // Perform Copy Mutations...
  if (m_organism->GetCopyMutProb() > 0) { // Skip this if no mutations....
    for (int i = 0; i < child_seq.GetSize(); i++) {
      //    for (int i = 0; i < m_memory.GetSize(); i++) {
      if (m_organism->TestCopyMut(ctx)) child_seq[i] = m_inst_set->GetRandomInst(ctx);
    }
  }
  
  // Handle Divide Mutations...
  Divide_DoMutations(ctx);
  
  const bool viable = Divide_CheckViable(ctx, org_genome.GetSize(), child_seq.GetSize(), 1);
  if (viable == false) return false;
  
  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures(ctx);
  
  // reset first time instruction costs
  for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
    m_inst_ft_cost[i] = m_inst_set->GetFTCost(Instruction(i));
  }
  
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

bool Hardware::Types::GP8::Inst_Die(Context& ctx)
{
  m_organism->Die(ctx);
  
  return true;
}

bool Hardware::Types::GP8::Inst_Move(Context& ctx)
{
  // In TestCPU, movement fails...
  if (m_organism->GetOrgInterface().GetCellID() == -1) return false;
  
  if (m_juv_enabled && m_organism->GetPhenotype().GetTimeUsed() < m_world->GetConfig().JUV_PERIOD.Get()) return false;
  
  if (!m_hw_queue_move) {
    m_hw_queue[m_hw_queued++] = aMOVE;
    m_hw_queue_move = true;
  }
  return true;
}

bool Hardware::Types::GP8::Inst_GetNorthOffset(Context& ctx) {
  const int out_reg = FindModifiedRegister(rBX);
  int compass_dir = m_organism->GetOrgInterface().GetFacedDir();
  if (m_use_avatar) compass_dir = m_organism->GetOrgInterface().GetAVFacing();
  setRegister(out_reg, compass_dir, true);
  return true;
}


bool Hardware::Types::GP8::Inst_Eat(Context& ctx)
{
  if (!m_hw_queue_eat) {
    m_hw_queue[m_hw_queued++] = aEAT;
    m_hw_queue_eat = true;
  }
  
  // Queue up to find out the result of the eat action, then yield until that executes
  m_hw_queue_eat_threads.Push(m_cur_thread);
  m_threads[m_cur_thread].active = false;
  m_threads[m_cur_thread].wait_reg = -1;
  
  return true;
}

bool Hardware::Types::GP8::Inst_RotateX(Context& ctx)
{
  int num_neighbors = m_organism->GetNeighborhoodSize();
  if (m_use_avatar) num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
  
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  const int reg_used = FindModifiedRegister(rBX, true);
  int rot_num = getRegister(ctx, reg_used);
  
  // rotate the nop number of times in the appropriate direction
  m_hw_queue_rotate_reverse = (rot_num < 0) ? 1 : 0;
  rot_num = abs(rot_num);
  if (rot_num > 7) rot_num = rot_num % 8;
  m_hw_queue_rotate_num = rot_num;
  
  if (!m_hw_queue_rotate) {
    m_hw_queue[m_hw_queued++] = aROTATE;
    m_hw_queue_rotate = true;
  }
  
  return true;
}

// Will rotate organism to face a specified other org
bool Hardware::Types::GP8::Inst_RotateOrgID(Context& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  // Will rotate organism to face a specificied other org
  const int id_sought_reg = FindModifiedRegister(rBX, true);
  const int id_sought = getRegister(ctx, id_sought_reg);
  const int worldx = m_world->GetPopulation().GetWorldX();
  const int worldy = m_world->GetPopulation().GetWorldY();
  int max_dist = 0;
  const int long_axis = (int) (max(worldx, worldy) * 0.5 + 0.5);
  m_world->GetConfig().LOOK_DIST.Get() != -1 ? max_dist = m_world->GetConfig().LOOK_DIST.Get() : max_dist = long_axis;
  bool have_org2use = false;
  
  // return false if invalid number or self
  if (id_sought < 0 || id_sought == m_organism->GetID()) return false;
  
  // if valid number, does the value represent a living organism?
  cOrganism* target_org  = NULL;
  const Apto::Array<cOrganism*, Apto::Smart>& live_orgs = m_organism->GetOrgInterface().GetLiveOrgList();
  for (int i = 0; i < live_orgs.GetSize(); i++) {
    cOrganism* org = live_orgs[i];
    if (id_sought == org->GetID()) {
      target_org = org;
      have_org2use = true;
      break;
    }
  }
  if (!have_org2use) return false;
  else {
    int target_org_cell = target_org->GetOrgInterface().GetCellID();
    int searching_org_cell = m_organism->GetOrgInterface().GetCellID();
    if (m_use_avatar == 2) {
      target_org_cell = target_org->GetOrgInterface().GetAVCellID();
      searching_org_cell = m_organism->GetOrgInterface().GetAVCellID();
      if (target_org_cell == searching_org_cell) return true; // avatars in same cell
    }
    const int target_x = target_org_cell % worldx;
    const int target_y = target_org_cell / worldx;
    const int searching_x = searching_org_cell % worldx;
    const int searching_y = searching_org_cell / worldx;
    const int x_dist = target_x - searching_x;
    const int y_dist = target_y - searching_y;
    
    const int travel_dist = max(abs(x_dist), abs(y_dist));
    if (travel_dist > max_dist) return false;
    
    int correct_facing = 0;
    if (y_dist < 0 && x_dist == 0) correct_facing = 0; // rotate N
    else if (y_dist < 0 && x_dist > 0) correct_facing = 1; // rotate NE
    else if (y_dist == 0 && x_dist > 0) correct_facing = 2; // rotate E
    else if (y_dist > 0 && x_dist > 0) correct_facing = 3; // rotate SE
    else if (y_dist > 0 && x_dist == 0) correct_facing = 4; // rotate S
    else if (y_dist > 0 && x_dist < 0) correct_facing = 5; // rotate SW
    else if (y_dist == 0 && x_dist < 0) correct_facing = 6; // rotate W
    else if (y_dist < 0 && x_dist < 0) correct_facing = 7; // rotate NW
    
    bool found_org = false;
    if (m_use_avatar == 2) {
      m_organism->GetOrgInterface().SetAVFacing(ctx, correct_facing);
      found_org = true;
    }
    else {
      int rotates = m_organism->GetNeighborhoodSize();
      for (int i = 0; i < rotates; i++) {
        m_organism->Rotate(ctx, -1);
        if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedDir() == correct_facing) {
          found_org = true;
          break;
        }
      }
    }
    // return some data as in look sensor
    if (found_org) {
      int dist_reg = FindModifiedNextRegister(id_sought_reg);
      int dir_reg = FindModifiedNextRegister(dist_reg);
      int fat_reg = FindModifiedNextRegister(dir_reg);
      int ft_reg = FindModifiedNextRegister(fat_reg);
      int group_reg = FindModifiedNextRegister(ft_reg);
      
      setRegister(dist_reg, -2, true);
      setRegister(dir_reg, Features::VisualSensor::Of(this).ReturnRelativeFacing(target_org), true);
      setRegister(fat_reg, (int) target_org->GetPhenotype().GetCurBonus(), true);
      setRegister(ft_reg, target_org->GetForageTarget(), true);
      if ((target_org->IsDisplaying() || m_world->GetConfig().USE_DISPLAY.Get()) && target_org->GetOrgDisplayData() != NULL) Features::VisualSensor::Of(this).SetLastSeenDisplay(target_org->GetOrgDisplayData());
    }
    return true;
  }
}

// Will rotate organism to face away from a specificied other org
bool Hardware::Types::GP8::Inst_RotateAwayOrgID(Context& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  // Will rotate organism to face a specificied other org
  const int id_sought_reg = FindModifiedRegister(rBX, true);
  const int id_sought = getRegister(ctx, id_sought_reg);
  const int worldx = m_world->GetPopulation().GetWorldX();
  const int worldy = m_world->GetPopulation().GetWorldY();
  int max_dist = 0;
  const int long_axis = (int) (max(worldx, worldy) * 0.5 + 0.5);
  m_world->GetConfig().LOOK_DIST.Get() != -1 ? max_dist = m_world->GetConfig().LOOK_DIST.Get() : max_dist = long_axis;
  bool have_org2use = false;
  
  // return false if invalid number or self
  if (id_sought < 0 || id_sought == m_organism->GetID()) return false;
  
  // if valid number, does the value represent a living organism?
  cOrganism* target_org = NULL;
  const Apto::Array<cOrganism*, Apto::Smart>& live_orgs = m_organism->GetOrgInterface().GetLiveOrgList();
  for (int i = 0; i < live_orgs.GetSize(); i++) {
    cOrganism* org = live_orgs[i];
    if (id_sought == org->GetID()) {
      target_org = org;
      have_org2use = true;
      break;
    }
  }
  if (!have_org2use) return false;
  else {
    int target_org_cell = target_org->GetOrgInterface().GetCellID();
    int searching_org_cell = m_organism->GetOrgInterface().GetCellID();
    if (m_use_avatar == 2) {
      target_org_cell = target_org->GetOrgInterface().GetAVCellID();
      searching_org_cell = m_organism->GetOrgInterface().GetAVCellID();
      if (target_org_cell == searching_org_cell) return true; // avatars in same cell
    }
    const int target_x = target_org_cell % worldx;
    const int target_y = target_org_cell / worldx;
    const int searching_x = searching_org_cell % worldx;
    const int searching_y = searching_org_cell / worldx;
    const int x_dist =  target_x - searching_x;
    const int y_dist = target_y - searching_y;
    
    const int travel_dist = max(abs(x_dist), abs(y_dist));
    if (travel_dist > max_dist) return false;
    
    int correct_facing = 0;
    if (y_dist < 0 && x_dist == 0) correct_facing = 4; // rotate away from N
    else if (y_dist < 0 && x_dist > 0) correct_facing = 5; // rotate away from NE
    else if (y_dist == 0 && x_dist > 0) correct_facing = 6; // rotate away from E
    else if (y_dist > 0 && x_dist > 0) correct_facing = 7; // rotate away from SE
    else if (y_dist > 0 && x_dist == 0) correct_facing = 0; // rotate away from S
    else if (y_dist > 0 && x_dist < 0) correct_facing = 1; // rotate away from SW
    else if (y_dist == 0 && x_dist < 0) correct_facing = 2; // rotate away from W
    else if (y_dist < 0 && x_dist < 0) correct_facing = 3; // rotate away from NW
    
    bool found_org = false;
    if (m_use_avatar == 2) {
      m_organism->GetOrgInterface().SetAVFacing(ctx, correct_facing);
      found_org = true;
    }
    else {
      int rotates = m_organism->GetNeighborhoodSize();
      for (int i = 0; i < rotates; i++) {
        m_organism->Rotate(ctx, -1);
        if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedDir() == correct_facing) {
          found_org = true;
          break;
        }
      }
    }
    // return some data as in look sensor
    if (found_org) {
      int dist_reg = FindModifiedNextRegister(id_sought_reg);
      int dir_reg = FindModifiedNextRegister(dist_reg);
      int fat_reg = FindModifiedNextRegister(dir_reg);
      int ft_reg = FindModifiedNextRegister(fat_reg);
      int group_reg = FindModifiedNextRegister(ft_reg);
      
      setRegister(dist_reg, -2, true);
      setRegister(dir_reg, Features::VisualSensor::Of(this).ReturnRelativeFacing(target_org), true);
      setRegister(fat_reg, (int) target_org->GetPhenotype().GetCurBonus(), true);
      setRegister(ft_reg, target_org->GetForageTarget(), true);
      if ((target_org->IsDisplaying() || m_world->GetConfig().USE_DISPLAY.Get()) && target_org->GetOrgDisplayData() != NULL) Features::VisualSensor::Of(this).SetLastSeenDisplay(target_org->GetOrgDisplayData());
    }
    return true;
  }
}

bool Hardware::Types::GP8::Inst_SenseResourceID(Context& ctx)
{
  Apto::Array<double> cell_res;
  if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx);
  int reg_to_set = FindModifiedRegister(rBX);
  double max_resource = 0.0;
  // if more than one resource is available, return the resource ID with the most available in this spot (note that, with global resources, the GLOBAL total will evaluated)
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (cell_res[i] > max_resource) {
      max_resource = cell_res[i];
      setRegister(reg_to_set, i, true);
    }
  }
  return true;
}

bool Hardware::Types::GP8::Inst_SenseNest(Context& ctx)
{
  Apto::Array<double> cell_res;
  if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx);
  
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  const int reg_used = FindModifiedRegister(rBX);
  
  int nest_id = m_threads[m_cur_thread].reg[reg_used].value;
  int nest_val = 0;
  
  // if invalid nop value, return the id of the first nest in the cell with val >= 1
  if (nest_id < 0 || nest_id >= resource_lib.GetSize() || !resource_lib.GetResource(nest_id)->IsNest()) {
    for (int i = 0; i < cell_res.GetSize(); i++) {
      if (resource_lib.GetResource(i)->IsNest() && cell_res[i] >= 1) {
        nest_id = i;
        nest_val = (int) cell_res[i];
        break;
      }
    }
  }
  else nest_val = (int) cell_res[nest_id];
  
  setRegister(reg_used, nest_id, true);
  const int val_reg = FindModifiedNextRegister(reg_used);
  setRegister(val_reg, nest_val, true);
  return true;
}

bool Hardware::Types::GP8::Inst_LookAheadEX(Context& ctx)
{
  return DoLookAheadEX(ctx);
}

bool Hardware::Types::GP8::Inst_LookAgainEX(Context& ctx)
{
  return DoLookAgainEX(ctx);
}


bool Hardware::Types::GP8::DoLookAheadEX(Context& ctx, bool use_ft)
{
  int cell_id = m_organism->GetOrgInterface().GetCellID();
  int facing = m_organism->GetOrgInterface().GetFacedDir();
  
  if (m_use_avatar) {
    cell_id = m_organism->GetOrgInterface().GetAVCellID();
    facing = m_organism->GetOrgInterface().GetAVFacing();
  }
  
  // temp check on world geometry until code can handle other geometries
  if (m_world->GetConfig().WORLD_GEOMETRY.Get() != 1) {
    // Instruction sense-diff-ahead only written to work in bounded grids
    assert(false);
    return false;
  }
  
  if (!m_use_avatar && m_organism->GetNeighborhoodSize() == 0) return false;
  else if (m_use_avatar && m_organism->GetOrgInterface().GetAVNumNeighbors() == 0) return false;
  
  const int reg_session = FindUpstreamModifiedRegister(0, -1);
  
  
  const int reg_habitat = FindModifiedRegister(rBX);                        // ?rBX?
  const int reg_id_sought = FindModifiedNextRegister(reg_habitat);          // ?rCX?
  const int reg_travel_distance = FindModifiedNextRegister(reg_id_sought);  // ?rDX?
  const int reg_deviance = FindNextRegister(reg_travel_distance);           // rDX + 1 = rEX
  const int reg_cv = FindNextRegister(reg_deviance);                        // rDX + 2 = rFX
  
  const int reg_search_distance = FindModifiedRegister(-1, true);                 // ?r?X?
  const int reg_search_type = FindModifiedRegister(-1, true);                     // ?r?X?
  
  const int reg_id_found = FindModifiedRegister(-1, true);                        // ?r?X?
  
  Features::VisualSensor::LookSettings look_init;
  look_init.habitat = m_threads[m_cur_thread].reg[reg_habitat].value;
  look_init.distance = (reg_search_distance == -1) ? std::numeric_limits<int>::max() : getRegister(ctx, reg_search_distance);
  look_init.search_type = (reg_search_type == -1) ? 0 : getRegister(ctx, reg_search_type);
  look_init.id_sought = m_threads[m_cur_thread].reg[reg_id_sought].value;
  
  Features::VisualSensor::LookResults look_results;
  look_results.value = 0;
  
  look_results = Features::VisualSensor::Of(this).PeformLook(ctx, look_init, facing, cell_id, use_ft);
  
  // Update Sessions
  m_threads[m_cur_thread].sensor_session.habitat = look_results.habitat;
  m_threads[m_cur_thread].sensor_session.distance = look_init.distance;
  m_threads[m_cur_thread].sensor_session.search_type = look_results.search_type;
  m_threads[m_cur_thread].sensor_session.id_sought = look_results.id_sought;
  
  if (reg_session != -1) {
    const int session_idx = Apto::Abs(m_threads[m_cur_thread].reg[reg_session].value % m_sensor_sessions.GetSize());
    m_sensor_sessions[session_idx].habitat = look_results.habitat;
    m_sensor_sessions[session_idx].distance = look_init.distance;
    m_sensor_sessions[session_idx].search_type = look_results.search_type;
    m_sensor_sessions[session_idx].id_sought = look_results.id_sought;
  }
  
  if (look_results.report_type == 0) {
    setRegister(reg_habitat, look_results.habitat, true);
    setRegister(reg_id_sought, look_results.id_sought, true);
    setRegister(reg_travel_distance, -1, true);
    setRegister(reg_deviance, 0, true);
    setRegister(reg_cv, 0, true);
    
    if (reg_search_type != -1) setRegister(reg_search_type, look_results.search_type, true);
    if (reg_id_found != -1) setRegister(reg_id_found, -9, true);
  } else if (look_results.report_type == 1) {
    setRegister(reg_habitat, look_results.habitat, true);
    setRegister(reg_id_sought, look_results.id_sought, true);
    setRegister(reg_travel_distance, look_results.distance, true);
    setRegister(reg_deviance, look_results.deviance, true);
    setRegister(reg_cv, (look_results.count <= 1) ? look_results.value : look_results.count, true);
    
    if (reg_search_type != -1) setRegister(reg_search_type, look_results.search_type, true);
    if (reg_id_found != -1) setRegister(reg_id_found, (look_results.forage == -9) ? look_results.group : look_results.forage, true);
  }
  
  if (m_world->GetConfig().LOOK_DISABLE.Get() > 5) {
    int org_type = m_world->GetConfig().LOOK_DISABLE_TYPE.Get();
    bool is_target_type = false;
    if (org_type == 0 && !m_organism->IsPreyFT()) is_target_type = true;
    else if (org_type == 1 && m_organism->IsPreyFT()) is_target_type = true;
    else if (org_type == 2) is_target_type = true;
    
    if (is_target_type) {
      int randsign = ctx.GetRandom().GetUInt(0,2) ? -1 : 1;
      int rand = ctx.GetRandom().GetInt(INT_MAX) * randsign;
      int target_reg = m_world->GetConfig().LOOK_DISABLE.Get();
      
      if (target_reg == 6) setRegister(reg_habitat, rand, true);
      else if (target_reg == 7) setRegister(reg_travel_distance, rand, true);
      else if (target_reg == 8 && reg_search_type != -1) setRegister(reg_search_type, rand, true);
      else if (target_reg == 9) setRegister(reg_id_sought, rand, true);
      else if (target_reg == 10 || target_reg == 11) setRegister(reg_cv, rand, true);
      else if ((target_reg == 12 || target_reg == 13) && reg_id_found != -1) setRegister(reg_id_found, rand, true);
    }
  }
  if (m_world->GetConfig().TRACK_LOOK_OUTPUT.Get()) {
    cString look_string = "";
    look_string += cStringUtil::Stringf("%d", m_organism->GetForageTarget());
    look_string += cStringUtil::Stringf(",%d", look_results.report_type);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[reg_habitat].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[reg_id_sought].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[reg_travel_distance].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[reg_deviance].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[reg_cv].value);
    m_organism->GetOrgInterface().TryWriteLookEXOutput(look_string);
  }
  
  return true;
}

bool Hardware::Types::GP8::DoLookAgainEX(Context& ctx, bool use_ft)
{
  int cell_id = m_organism->GetOrgInterface().GetCellID();
  int facing = m_organism->GetOrgInterface().GetFacedDir();
  
  if (m_use_avatar) {
    cell_id = m_organism->GetOrgInterface().GetAVCellID();
    facing = m_organism->GetOrgInterface().GetAVFacing();
  }
  
  // temp check on world geometry until code can handle other geometries
  if (m_world->GetConfig().WORLD_GEOMETRY.Get() != 1) {
    // Instruction sense-diff-ahead only written to work in bounded grids
    assert(false);
    return false;
  }
  
  if (!m_use_avatar && m_organism->GetNeighborhoodSize() == 0) return false;
  else if (m_use_avatar && m_organism->GetOrgInterface().GetAVNumNeighbors() == 0) return false;
  
  const int reg_session = FindUpstreamModifiedRegister(0, -1);
  
  const int reg_travel_distance = FindModifiedNextRegister(rDX);  // ?rDX?
  const int reg_deviance = FindNextRegister(reg_travel_distance);           // rDX + 1 = rEX
  const int reg_cv = FindNextRegister(reg_deviance);                        // rDX + 2 = rFX
  const int reg_id_found = FindModifiedRegister(-1);                        // ?r?X?
  
  Features::VisualSensor::LookSettings look_init = m_threads[m_cur_thread].sensor_session;
  if (reg_session != -1) {
    const int session_idx = Apto::Abs(m_threads[m_cur_thread].reg[reg_session].value % m_sensor_sessions.GetSize());
    look_init = m_sensor_sessions[session_idx];
  }
  
  Features::VisualSensor::LookResults look_results;
  look_results.value = 0;
  
  look_results = Features::VisualSensor::Of(this).PerformLook(ctx, look_init, facing, cell_id, use_ft);
  
  if (m_world->GetConfig().TRACK_LOOK_SETTINGS.Get()) {
    cString look_string = "";
    look_string += cStringUtil::Stringf("%d", m_organism->GetForageTarget());
    look_string += cStringUtil::Stringf(",%d", look_results.report_type);
    look_string += cStringUtil::Stringf(",%d", look_results.habitat);
    look_string += cStringUtil::Stringf(",%d", look_results.distance);
    look_string += cStringUtil::Stringf(",%d", look_results.search_type);
    look_string += cStringUtil::Stringf(",%d", look_results.id_sought);
    m_organism->GetOrgInterface().TryWriteLookData(look_string);
  }
  
  if (look_results.report_type == 0) {
    setRegister(reg_travel_distance, -1, true);
    setRegister(reg_deviance, 0, true);
    setRegister(reg_cv, 0, true);
    
    if (reg_id_found != -1) setRegister(reg_id_found, -9, true);
  } else if (look_results.report_type == 1) {
    setRegister(reg_travel_distance, look_results.distance, true);
    setRegister(reg_deviance, look_results.deviance, true);
    setRegister(reg_cv, (look_results.count <= 1) ? look_results.value : look_results.count, true);
    
    if (reg_id_found != -1) setRegister(reg_id_found, (look_results.forage == -9) ? look_results.group : look_results.forage, true);
  }
  
  if (m_world->GetConfig().LOOK_DISABLE.Get() > 5) {
    int org_type = m_world->GetConfig().LOOK_DISABLE_TYPE.Get();
    bool is_target_type = false;
    if (org_type == 0 && !m_organism->IsPreyFT()) is_target_type = true;
    else if (org_type == 1 && m_organism->IsPreyFT()) is_target_type = true;
    else if (org_type == 2) is_target_type = true;
    
    if (is_target_type) {
      int randsign = ctx.GetRandom().GetUInt(0,2) ? -1 : 1;
      int rand = ctx.GetRandom().GetInt(INT_MAX) * randsign;
      int target_reg = m_world->GetConfig().LOOK_DISABLE.Get();
      
      if (target_reg == 7) setRegister(reg_travel_distance, rand, true);
      else if (target_reg == 10 || target_reg == 11) setRegister(reg_cv, rand, true);
      else if ((target_reg == 12 || target_reg == 13) && reg_id_found != -1) setRegister(reg_id_found, rand, true);
    }
  }
  if (m_world->GetConfig().TRACK_LOOK_OUTPUT.Get()) {
    cString look_string = "";
    look_string += cStringUtil::Stringf("%d", m_organism->GetForageTarget());
    look_string += cStringUtil::Stringf(",%d", look_results.report_type);
    look_string += cStringUtil::Stringf(",%d", look_results.habitat);
    look_string += cStringUtil::Stringf(",%d", look_results.id_sought);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[reg_travel_distance].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[reg_deviance].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[reg_cv].value);
    m_organism->GetOrgInterface().TryWriteLookEXOutput(look_string);
  }
  return true;
}





bool Hardware::Types::GP8::Inst_SenseFacedHabitat(Context& ctx)
{
  int reg_to_set = FindModifiedRegister(rBX);
  
  // get the resource library
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  
  // get the destination cell resource levels
  Apto::Array<double> cell_res;
  if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx);
  
  // check for any habitats ahead that affect movement, returning the most 'severe' habitat type
  // simulated predator ahead
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 5 && cell_res[i] > 0) {
      setRegister(reg_to_set, 3, true);
      return true;
    }
  }
  // are there any barrier resources in the faced cell
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 2 && cell_res[i] > 0) {
      setRegister(reg_to_set, 2, true);
      return true;
    }
  }
  // if no barriers, are there any hills in the faced cell
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 1 && cell_res[i] > 0) {
      setRegister(reg_to_set, 1, true);
      return true;
    }
  }
  // if no barriers or hills, we return a 0 to indicate clear sailing
  setRegister(reg_to_set, 0, true);
  return true;
}

bool Hardware::Types::GP8::Inst_SetForageTarget(Context& ctx)
{
  assert(m_organism != 0);
  int prop_target = getRegister(ctx, FindModifiedRegister(rBX, true));
  
  //return false if org setting target to current one (avoid paying costs for not switching)
  const int old_target = m_organism->GetForageTarget();
  if (old_target == prop_target) return false;
  
  // return false if predator trying to become prey and this has been disallowed
  if (old_target <= -2 && prop_target > -2 && (m_world->GetConfig().PRED_PREY_SWITCH.Get() == 0 || m_world->GetConfig().PRED_PREY_SWITCH.Get() == 2)) return false;
  
  // return false if trying to become predator and there are none in the experiment
  if (prop_target <= -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0) return false;
  
  // return false if trying to become predator this has been disallowed via setforagetarget
  if (prop_target <= -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() == 2) return false;
  
  // a little mod help...can't set to -1, that's for juevniles only...so only exception to mod help is -2
  if (!m_world->GetEnvironment().IsTargetID(prop_target) && prop_target != -2 && prop_target != -3) {
    int num_fts = 0;
    std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
    set <int>::iterator itr;
    for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) if (*itr != -1 && *itr != -2 && *itr != -3) num_fts++;
    if (m_world->GetEnvironment().IsTargetID(-1) && num_fts == 0) prop_target = -1;
    else {
      // ft's may not be sequentially numbered
      int ft_num = abs(prop_target) % num_fts;
      itr = fts_avail.begin();
      for (int i = 0; i < ft_num; i++) itr++;
      prop_target = *itr;
    }
  }
  
  // make sure we use a valid (resource) target
  // -2 target means setting to predator
  // if (!m_world->GetEnvironment().IsTargetID(prop_target) && (prop_target != -2)) return false;
  
  // switching between predator and prey means having to switch avatar list...don't run this for orgs with AVCell == -1 (avatars off or test cpu)
  if (m_use_avatar && (((prop_target == -2 || prop_target == -3) && old_target > -2) || (prop_target > -2 && (old_target == -2 || old_target == -3))) &&
      (m_organism->GetOrgInterface().GetAVCellID() != -1)) {
    m_organism->GetOrgInterface().SwitchPredPrey(ctx);
    m_organism->SetForageTarget(ctx, prop_target);
  }
  else m_organism->SetForageTarget(ctx, prop_target);
  
  // Set the new target and return the value
  m_organism->RecordFTSet();
  setRegister(FindModifiedRegister(rBX, true), prop_target, false);
  return true;
}

bool Hardware::Types::GP8::Inst_SetForageTargetOnce(Context& ctx)
{
  assert(m_organism != 0);
  if (m_organism->HasSetFT()) return false;
  else return Inst_SetForageTarget(ctx);
}

bool Hardware::Types::GP8::Inst_SetRandForageTargetOnce(Context& ctx)
{
  assert(m_organism != 0);
  int cap = 0;
  if (m_world->GetConfig().POPULATION_CAP.Get()) cap = m_world->GetConfig().POPULATION_CAP.Get();
  else if (m_world->GetConfig().POP_CAP_ELDEST.Get()) cap = m_world->GetConfig().POP_CAP_ELDEST.Get();
  if (cap && (m_organism->GetOrgInterface().GetLiveOrgList().GetSize() >= (((double)(cap)) * 0.5)) && ctx.GetRandom().P(0.5)) {
    if (m_organism->HasSetFT()) return false;
    else {
      int num_fts = 0;
      std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
      set <int>::iterator itr;
      for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) if (*itr != -1 && *itr != -2 && *itr != -3) num_fts++;
      int prop_target = ctx.GetRandom().GetUInt(num_fts);
      if (m_world->GetEnvironment().IsTargetID(-1) && num_fts == 0) prop_target = -1;
      else {
        // ft's may not be sequentially numbered
        int ft_num = abs(prop_target) % num_fts;
        itr = fts_avail.begin();
        for (int i = 0; i < ft_num; i++) itr++;
        prop_target = *itr;
      }
      // Set the new target and return the value
      m_organism->SetForageTarget(ctx, prop_target);
      m_organism->RecordFTSet();
      setRegister(FindModifiedRegister(rBX, true), prop_target, false);
      return true;
    }
  }
  else return Inst_SetForageTargetOnce(ctx);
}

bool Hardware::Types::GP8::Inst_GetForageTarget(Context& ctx)
{
  assert(m_organism != 0);
  const int target_reg = FindModifiedRegister(rBX);
  setRegister(target_reg, m_organism->GetForageTarget(), false);
  return true;
}

bool Hardware::Types::GP8::Inst_CollectSpecific(Context& ctx)
{
  const int resource = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  double res_before = m_organism->GetRBin(resource);
  bool success = DoActualCollect(ctx, resource, false);
  double res_after = m_organism->GetRBin(resource);
  int out_reg = FindModifiedRegister(rBX);
  setRegister(out_reg, (int)(res_after - res_before), true);
  setRegister(FindModifiedNextRegister(out_reg, true), (int)(res_after), true);
  return success;
}

bool Hardware::Types::GP8::Inst_GetResStored(Context& ctx)
{
  int resource_id = abs(getRegister(ctx, FindModifiedRegister(rBX, true)));
  Apto::Array<double> bins = m_organism->GetRBins();
  resource_id %= bins.GetSize();
  int out_reg = FindModifiedRegister(rBX);
  setRegister(out_reg, (int)(bins[resource_id]), true);
  return true;
}


bool Hardware::Types::GP8::Inst_GetFacedOrgID(Context& ctx)
//Get ID of organism faced by this one, if there is an organism in front.
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  cOrganism* neighbor = NULL;
  if (!m_use_avatar && !m_organism->IsNeighborCellOccupied()) return false;
  else if (m_use_avatar == 2 && !m_organism->GetOrgInterface().FacedHasAV()) return false;
  
  if (!m_use_avatar) neighbor = m_organism->GetOrgInterface().GetNeighbor();
  else if (m_use_avatar == 2) neighbor = m_organism->GetOrgInterface().GetRandFacedAV(ctx);
  if (neighbor->IsDead())  return false;
  
  const int out_reg = FindModifiedRegister(rBX);
  setRegister(out_reg, neighbor->GetID(), true);
  return true;
}

//Teach offspring learned targeting/foraging behavior
bool Hardware::Types::GP8::Inst_TeachOffspring(Context&)
{
  assert(m_organism != 0);
  m_organism->Teach(true);
  return true;
}

bool Hardware::Types::GP8::Inst_LearnParent(Context& ctx)
{
  assert(m_organism != 0);
  bool halt = false;
  if (m_organism->HadParentTeacher()) {
    int old_target = m_organism->GetForageTarget();
    int prop_target = -1;
    prop_target = m_organism->GetParentFT();
    
    halt = (prop_target <= -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0);
    if (!halt) {
      if (m_use_avatar && m_organism->GetOrgInterface().GetAVCellID() != -1 &&
          (((prop_target == -2 || prop_target == -3) && old_target > -2) || (prop_target > -2 && (old_target == -2 || prop_target == -3)))) {
        m_organism->GetOrgInterface().SwitchPredPrey(ctx);
        m_organism->CopyParentFT(ctx);
      }
      else m_organism->CopyParentFT(ctx);
    }
  }
  return !halt;
}


//Attack organism faced by this one, if there is non-predator target in front, and steal it's merit, current bonus, and reactions.
bool Hardware::Types::GP8::Inst_AttackPrey(Context& ctx)
{
  if (!testAttack(ctx)) return false;
  cOrganism* target = getPreyTarget(ctx);
  if (!testPreyTarget(target)) return false;
  
  AttackRegisters reg;
  setAttackReg(reg);
  
  if (!executeAttack(ctx, target, reg)) return false;
  
  return true;
}


bool Hardware::Types::GP8::Inst_ScrambleReg(Context& ctx)
{
  for (int i = 0; i < NUM_REGISTERS; i++) setRegister(rAX + i, ctx.GetRandom().GetInt(), true);
  return true;
}

