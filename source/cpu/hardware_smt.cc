/*
 *  hardware_smt.cc
 *  Avida2
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#include "hardware_smt.h"

#include "config.hh"
#include "cpu_test_info.hh"
#include "functions.hh"
#include "genome_util.hh"
#include "inst_lib_base.hh"
#include "inst_set.hh"
#include "hardware_tracer.hh"
//#include "hardware_tracer_4stack.hh"
#include "mutation.hh"
#include "mutation_lib.hh"
#include "mutation_macros.hh"
#include "organism.hh"
#include "phenotype.hh"
#include "random.hh"
#include "string_util.hh"
#include "test_cpu.hh"

#include <limits.h>

using namespace std;



///////////////
//  cHardwareSMT
///////////////

cInstLibBase* cHardwareSMT::GetInstLib(){ return s_inst_slib; }

tInstLib<cHardwareSMT::tMethod> *cHardwareSMT::s_inst_slib = cHardwareSMT::initInstLib();
tInstLib<cHardwareSMT::tMethod> *cHardwareSMT::initInstLib(void){
  struct cNOPEntry4Stack {
    cNOPEntry4Stack(const cString &name, int nop_mod):name(name), nop_mod(nop_mod){}
    cString name;
    int nop_mod;
  };
  static const cNOPEntry4Stack s_n_array[] = {
    cNOPEntry4Stack("Nop-A", nHardwareSMT::STACK_AX),
    cNOPEntry4Stack("Nop-B", nHardwareSMT::STACK_BX),
    cNOPEntry4Stack("Nop-C", nHardwareSMT::STACK_CX),
    cNOPEntry4Stack("Nop-D", nHardwareSMT::STACK_DX),
  };
	
  struct cInstEntry4Stack {
    cInstEntry4Stack(const cString &name, tMethod function):name(name), function(function){}
    cString name;
    tMethod function;
  };
  static const cInstEntry4Stack s_f_array[] = {
    //1 
    cInstEntry4Stack("Nop-A",     &cHardwareSMT::Inst_Nop), 
    //2
    cInstEntry4Stack("Nop-B",     &cHardwareSMT::Inst_Nop), 
    //3
    cInstEntry4Stack("Nop-C",     &cHardwareSMT::Inst_Nop),   
    //4 
    cInstEntry4Stack("Nop-D",     &cHardwareSMT::Inst_Nop), 
    //38
    cInstEntry4Stack("Nop-E",     &cHardwareSMT::Inst_Nop),
    //39
    cInstEntry4Stack("Nop-F",     &cHardwareSMT::Inst_Nop),
    //5
    cInstEntry4Stack("Nop-X",     &cHardwareSMT::Inst_Nop),
    //6 
    cInstEntry4Stack("Val-Shift-R",   &cHardwareSMT::Inst_ShiftR),
    //7
    cInstEntry4Stack("Val-Shift-L",   &cHardwareSMT::Inst_ShiftL),
    //8
    cInstEntry4Stack("Val-Nand",      &cHardwareSMT::Inst_Val_Nand),
    //9
    cInstEntry4Stack("Val-Add",       &cHardwareSMT::Inst_Val_Add),
    //10
    cInstEntry4Stack("Val-Sub",       &cHardwareSMT::Inst_Val_Sub),
    //11
    cInstEntry4Stack("Val-Mult",      &cHardwareSMT::Inst_Val_Mult),
    //12
    cInstEntry4Stack("Val-Div",       &cHardwareSMT::Inst_Val_Div),
    //13
    cInstEntry4Stack("SetMemory",   &cHardwareSMT::Inst_SetMemory),
    //14
    cInstEntry4Stack("Divide",  &cHardwareSMT::Inst_Divide),
    //15
    cInstEntry4Stack("Inst-Read",    &cHardwareSMT::Inst_HeadRead),
    //16
    cInstEntry4Stack("Inst-Write",   &cHardwareSMT::Inst_HeadWrite),
    //keeping this one for the transition period
    //cInstEntry4Stack("Inst-Copy",    &cHardwareSMT::Inst_HeadCopy),
    //17
    cInstEntry4Stack("If-Equal",    &cHardwareSMT::Inst_IfEqual),
    //18
    cInstEntry4Stack("If-Not-Equal",  &cHardwareSMT::Inst_IfNotEqual),
    //19
    cInstEntry4Stack("If-Less",   &cHardwareSMT::Inst_IfLess),
    //20
    cInstEntry4Stack("If-Greater",    &cHardwareSMT::Inst_IfGreater),
    //21
    cInstEntry4Stack("Head-Push",    &cHardwareSMT::Inst_HeadPush),
    //22
    cInstEntry4Stack("Head-Pop",     &cHardwareSMT::Inst_HeadPop),
    //23
    cInstEntry4Stack("Head-Move",  &cHardwareSMT::Inst_HeadMove),
    //24
    cInstEntry4Stack("Search",  &cHardwareSMT::Inst_Search),
    //25
    cInstEntry4Stack("Push-Next",    &cHardwareSMT::Inst_PushNext),
    //26
    cInstEntry4Stack("Push-Prev",    &cHardwareSMT::Inst_PushPrevious),
    //27
    cInstEntry4Stack("Push-Comp",    &cHardwareSMT::Inst_PushComplement),
    //28
    cInstEntry4Stack("Val-Delete", &cHardwareSMT::Inst_ValDelete),
    //29
    cInstEntry4Stack("Val-Copy",  &cHardwareSMT::Inst_ValCopy),
    //30
    cInstEntry4Stack("ThreadFork",   &cHardwareSMT::Inst_ForkThread),
    //31
    cInstEntry4Stack("Val-Inc",       &cHardwareSMT::Inst_Increment),
    //32
    cInstEntry4Stack("Val-Dec",       &cHardwareSMT::Inst_Decrement),
    //33
    cInstEntry4Stack("Val-Mod",       &cHardwareSMT::Inst_Mod),
    //34
    cInstEntry4Stack("ThreadKill",   &cHardwareSMT::Inst_KillThread),
    //35
    cInstEntry4Stack("IO", &cHardwareSMT::Inst_IO),
    //36
    cInstEntry4Stack("Inject", &cHardwareSMT::Inst_Inject)
  };
	
  const int n_size = sizeof(s_n_array)/sizeof(cNOPEntry4Stack);
	
  static cString n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size; i++){
    n_names[i] = s_n_array[i].name;
    nop_mods[i] = s_n_array[i].nop_mod;
  }
	
  const int f_size = sizeof(s_f_array)/sizeof(cInstEntry4Stack);
  static cString f_names[f_size];
  static tMethod functions[f_size];
  for (int i = 0; i < f_size; i++){
    f_names[i] = s_f_array[i].name;
    functions[i] = s_f_array[i].function;
  }
	
	const cInstruction error(255);
	const cInstruction def(0);
	
  tInstLib<cHardwareSMT::tMethod> *inst_lib =
    new tInstLib<cHardwareSMT::tMethod>(n_size, f_size, n_names, f_names, nop_mods, functions, error, def);
	
  return inst_lib;
}

cHardwareSMT::cHardwareSMT(cOrganism* in_organism, cInstSet* in_inst_set)
  : cHardwareBase(in_organism, in_inst_set), m_mem_array(1)
{
  m_functions = s_inst_slib->GetFunctions();
	
  m_mem_array[0] = in_organism->GetGenome();  // Initialize memory...
  m_mem_array[0].Resize(GetMemory(0).GetSize() + 1);
  m_mem_array[0][m_mem_array[0].GetSize() - 1] = cInstruction();
  Reset();                            // Setup the rest of the hardware...
}


cHardwareSMT::cHardwareSMT(const cHardwareSMT& hardware)
: cHardwareBase(hardware.organism, hardware.inst_set)
, m_functions(hardware.m_functions)
, m_mem_array(hardware.m_mem_array)
, m_threads(hardware.m_threads)
, thread_id_chart(hardware.thread_id_chart)
, m_cur_thread(hardware.m_cur_thread)
, inst_cost(hardware.inst_cost)
#ifdef INSTRUCTION_COSTS
, inst_ft_cost(hardware.inst_ft_cost)
#endif
{
  for(int i = 0; i < nHardwareSMT::NUM_GLOBAL_STACKS; i++) {
    m_global_stacks[i] = hardware.m_global_stacks[i];
  }
}


void cHardwareSMT::Recycle(cOrganism * new_organism, cInstSet * in_inst_set)
{
  cHardwareBase::Recycle(new_organism, in_inst_set);
  m_mem_array[0] = new_organism->GetGenome();
  m_mem_array[0].Resize(GetMemory(0).GetSize() + 1);
  m_mem_array[0][m_mem_array[0].GetSize()-1] = cInstruction();
  Reset();
}


void cHardwareSMT::Reset()
{
  // Setup the memory...
  m_mem_array.Resize(1);
  
  // We want to reset to have a single thread.
  m_threads.Resize(1);
	
  // Reset that single thread.
  m_threads[0].Reset(this, 0);
  thread_id_chart = 1; // Mark only the first thread as taken...
  m_cur_thread = 0;
		
  // Reset all stacks (local and global)
  for(int i = 0; i < nHardwareSMT::NUM_STACKS; i++) {
		Stack(i).Clear();
	}
	
#ifdef INSTRUCTION_COSTS
  // instruction cost arrays
  const int num_inst_cost = GetNumInst();
  inst_cost.Resize(num_inst_cost);
  inst_ft_cost.Resize(num_inst_cost);
	
  for (int i = 0; i < num_inst_cost; i++) {
    inst_cost[i] = GetInstSet().GetCost(cInstruction(i));
    inst_ft_cost[i] = GetInstSet().GetFTCost(cInstruction(i));
  }
#endif	
}

// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.

void cHardwareSMT::SingleProcess()
{
  // Mark this organism as running...
  organism->SetRunning(true);
	
  cPhenotype & phenotype = organism->GetPhenotype();
  phenotype.IncTimeUsed();
	
  const int num_inst_exec = (cConfig::GetThreadSlicingMethod() == 1) ? GetNumThreads() : 1;

  for (int i = 0; i < num_inst_exec; i++) {
    // Setup the hardware for the next instruction to be executed.
    NextThread();
    AdvanceIP() = true;
    IP().Adjust();
		
#ifdef BREAKPOINTS
    if (IP().FlagBreakpoint() == true) {
      organism->DoBreakpoint();
    }
#endif
    
    // DDD - there is no cHardwareTracer_SMT -- Print the status of this CPU at each step...
    //if (m_tracer != NULL) {
    //  if (cHardwareTracer_SMT* tracer = dynamic_cast<cHardwareTracer_SMT *>(m_tracer)) {
    //    tracer->TraceHardware_SMT(*this);
    //  }
    //}

    // Find the instruction to be executed
    const cInstruction & cur_inst = IP().GetInst();
		
    // Test if costs have been paid and it is okay to execute this now...
    const bool exec = SingleProcess_PayCosts(cur_inst);
		
    // Now execute the instruction...
    if (exec == true) {
      SingleProcess_ExecuteInst(cur_inst);
			
      // Some instruction (such as jump) may turn advance_ip off.  Ususally
      // we now want to move to the next instruction in the memory.
      if (AdvanceIP() == true) IP().Advance();
    } // if exec
  } // Previous was executed once for each thread...

  // Kill creatures who have reached their max num of instructions executed
  const int max_executed = organism->GetMaxExecuted();
  if ((max_executed > 0 && phenotype.GetTimeUsed() >= max_executed)
      || phenotype.GetToDie()) {
    organism->Die();
  }

  organism->SetRunning(false);
}

// This method will test to see if all costs have been paid associated
// with executing an instruction and only return true when that instruction
// should proceed.
bool cHardwareSMT::SingleProcess_PayCosts(const cInstruction & cur_inst)
{
#ifdef INSTRUCTION_COSTS
  assert(cur_inst.GetOp() < inst_cost.GetSize());
	
  // If first time cost hasn't been paid off...
  if ( inst_ft_cost[cur_inst.GetOp()] > 0 ) {
    inst_ft_cost[cur_inst.GetOp()]--;       // dec cost
    return false;
  }
	
  // Next, look at the per use cost
  if ( GetInstSet().GetCost(cur_inst) > 0 ) {
    if ( inst_cost[cur_inst.GetOp()] > 1 ){  // if isn't paid off (>1)
      inst_cost[cur_inst.GetOp()]--;         // dec cost
      return false;
    } else {                                 // else, reset cost array
      inst_cost[cur_inst.GetOp()] = GetInstSet().GetCost(cur_inst);
    }
  }
	
  // Prob of exec
  if ( GetInstSet().GetProbFail(cur_inst) > 0.0 ){
    return !( g_random.P(GetInstSet().GetProbFail(cur_inst)) );
  }
#endif
  return true;
}

// This method will handle the actuall execution of an instruction
// within single process, once that function has been finalized.
bool cHardwareSMT::SingleProcess_ExecuteInst(const cInstruction & cur_inst) 
{
  // Copy Instruction locally to handle stochastic effects
  cInstruction actual_inst = cur_inst;
  
#ifdef EXECUTION_ERRORS
  // If there is an execution error, execute a random instruction.
  if (organism->TestExeErr()) actual_inst = GetInstSet().GetRandomInst();
#endif /* EXECUTION_ERRORS */
	
  // Get a pointer to the corrisponding method...
  int inst_idx = GetInstSet().GetLibFunctionIndex(actual_inst);
  
  // Mark the instruction as executed
  IP().FlagExecuted() = true;
	
	
#ifdef INSTRUCTION_COUNT
  // instruction execution count incremeneted
  organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
#endif
	
  // And execute it.
  const bool exec_success = (this->*(m_functions[inst_idx]))();
	
#ifdef INSTRUCTION_COUNT
  // decremenet if the instruction was not executed successfully
  if (exec_success == false) {
    organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  }
#endif	
	
  return exec_success;
}


void cHardwareSMT::ProcessBonusInst(const cInstruction & inst)
{
  // Mark this organism as running...
  bool prev_run_state = organism->GetIsRunning();
  organism->SetRunning(true);
	
  // DDD - there is no cHardwareTracer_SMT -- Print the status of this CPU at each step...
  //if (m_tracer != NULL) {
  //  if (cHardwareTracer_SMT* tracer = dynamic_cast<cHardwareTracer_SMT *>(m_tracer)) {
  //    tracer->TraceHardware_SMTBonus(*this);
  //  }
  //}

  SingleProcess_ExecuteInst(inst);

  organism->SetRunning(prev_run_state);
}

bool cHardwareSMT::OK()
{
  bool result = true;
	
  for(int i = 0 ; i < m_mem_array.GetSize(); i++) {
    if (!m_mem_array[i].OK()) result = false;
  }
	
  for (int i = 0; i < GetNumThreads(); i++) {
    for(int j=0; j < nHardwareSMT::NUM_LOCAL_STACKS; j++)
			if (m_threads[i].local_stacks[j].OK() == false) result = false;
    if (m_threads[i].next_label.OK() == false) result = false;
  }
	
  return result;
}

void cHardwareSMT::PrintStatus(ostream & fp)
{
  fp << organism->GetPhenotype().GetTimeUsed() << " "
	<< "IP:(" << IP().GetMemSpace() << ", " << IP().GetPosition() << ")    "
	
	<< "AX:" << Stack(nHardwareSMT::STACK_AX).Top() << " "
	<< setbase(16) << "[0x" << Stack(nHardwareSMT::STACK_AX).Top() << "]  " << setbase(10)
	
	<< "BX:" << Stack(nHardwareSMT::STACK_BX).Top() << " "
	<< setbase(16) << "[0x" << Stack(nHardwareSMT::STACK_BX).Top() << "]  " << setbase(10)
	
	<< "CX:" << Stack(nHardwareSMT::STACK_CX).Top() << " "
	<< setbase(16) << "[0x" << Stack(nHardwareSMT::STACK_CX).Top() << "]  " << setbase(10)
	
	<< "DX:" << Stack(nHardwareSMT::STACK_DX).Top() << " "
	<< setbase(16) << "[0x" << Stack(nHardwareSMT::STACK_DX).Top() << "]  " << setbase(10)
	
	<< endl;
	
  fp << "  R-Head:(" << GetHead(HEAD_READ).GetMemSpace() << ", " 
		<< GetHead(HEAD_READ).GetPosition() << ")  " 
		<< "W-Head:(" << GetHead(HEAD_WRITE).GetMemSpace()  << ", "
		<< GetHead(HEAD_WRITE).GetPosition() << ")  "
		<< "F-Head:(" << GetHead(HEAD_FLOW).GetMemSpace()   << ",  "
		<< GetHead(HEAD_FLOW).GetPosition() << ")  "
		<< "RL:" << GetReadLabel().AsString() << "   "
		<< endl;
	
  fp << "  Mem (" << GetMemory(0).GetSize() << "):"
		  << "  " << GetMemory(0).AsString()
		  << endl;
  fp << "       " << GetMemory(1).GetSize() << "):"
		  << "  " << GetMemory(1).AsString()
		  << endl;
  fp << "       " << GetMemory(2).GetSize() << "):"
		  << "  " << GetMemory(2).AsString()
		  << endl;
  fp << "       " << GetMemory(3).GetSize() << "):"
		  << "  " << GetMemory(3).AsString()
		  << endl;
  
  
  fp.flush();
}




/////////////////////////////////////////////////////////////////////////
// Method: cHardwareSMT::FindLabel(direction)
//
// Search in 'direction' (+ or - 1) from the instruction pointer for the
// compliment of the label in 'next_label' and return a pointer to the
// results.  If direction is 0, search from the beginning of the genome.
//
/////////////////////////////////////////////////////////////////////////

cHeadMultiMem cHardwareSMT::FindLabel(int direction)
{
  cHeadMultiMem & inst_ptr = IP();
	
  // Start up a search head at the position of the instruction pointer.
  cHeadMultiMem search_head(inst_ptr);
  cCodeLabel & search_label = GetLabel();
	
  // Make sure the label is of size  > 0.
	
  if (search_label.GetSize() == 0) {
    return inst_ptr;
  }
	
  // Call special functions depending on if jump is forwards or backwards.
  int found_pos = 0;
  if( direction < 0 ) {
    found_pos = FindLabel_Backward(search_label, inst_ptr.GetMemory(),
																	 inst_ptr.GetPosition() - search_label.GetSize());
  }
	
  // Jump forward.
  else if (direction > 0) {
    found_pos = FindLabel_Forward(search_label, inst_ptr.GetMemory(),
																	inst_ptr.GetPosition());
  }
	
  // Jump forward from the very beginning.
  else {
    found_pos = FindLabel_Forward(search_label, inst_ptr.GetMemory(), 0);
  }
  
  // Return the last line of the found label, if it was found.
  if (found_pos > 0) search_head.Set(found_pos - 1, IP().GetMemSpace());
  
  // Return the found position (still at start point if not found).
  return search_head;
}


// Search forwards for search_label from _after_ position pos in the
// memory.  Return the first line _after_ the the found label.  It is okay
// to find search label's match inside another label.

int cHardwareSMT::FindLabel_Forward(const cCodeLabel & search_label,
                                    const cGenome & search_genome, int pos)
{
  assert (pos < search_genome.GetSize() && pos >= 0);
	
  int search_start = pos;
  int label_size = search_label.GetSize();
  bool found_label = false;
	
  // Move off the template we are on.
  pos += label_size;
	
  // Search until we find the complement or exit the memory.
  while (pos < search_genome.GetSize()) {
		
    // If we are within a label, rewind to the beginning of it and see if
    // it has the proper sub-label that we're looking for.
		
    if (inst_set->IsNop(search_genome[pos])) {
      // Find the start and end of the label we're in the middle of.
			
      int start_pos = pos;
      int end_pos = pos + 1;
      while (start_pos > search_start &&
						 inst_set->IsNop( search_genome[start_pos - 1] )) {
				start_pos--;
      }
      while (end_pos < search_genome.GetSize() &&
						 inst_set->IsNop( search_genome[end_pos] )) {
				end_pos++;
      }
      int test_size = end_pos - start_pos;
			
      // See if this label has the proper sub-label within it.
      int max_offset = test_size - label_size + 1;
      int offset = start_pos;
      for (offset = start_pos; offset < start_pos + max_offset; offset++) {
				
				// Test the number of matches for this offset.
				int matches;
				for (matches = 0; matches < label_size; matches++) {
					if (search_label[matches] !=
							inst_set->GetNopMod( search_genome[offset + matches] )) {
						break;
					}
				}
				
				// If we have found it, break out of this loop!
				if (matches == label_size) {
					found_label = true;
					break;
				}
      }
			
      // If we've found the complement label, set the position to the end of
      // the label we found it in, and break out.
			
      if (found_label == true) {
				// pos = end_pos;
				pos = label_size + offset;
				break;
      }
			
      // We haven't found it; jump pos to just after the current label being
      // checked.
      pos = end_pos;
    }
		
    // Jump up a block to the next possible point to find a label,
    pos += label_size;
  }
	
  // If the label was not found return a -1.
  if (found_label == false) pos = -1;
	
  return pos;
}

// Search backwards for search_label from _before_ position pos in the
// memory.  Return the first line _after_ the the found label.  It is okay
// to find search label's match inside another label.

int cHardwareSMT::FindLabel_Backward(const cCodeLabel & search_label,
                                     const cGenome & search_genome, int pos)
{
  assert (pos < search_genome.GetSize());
	
  int search_start = pos;
  int label_size = search_label.GetSize();
  bool found_label = false;
	
  // Move off the template we are on.
  pos -= label_size;
	
  // Search until we find the complement or exit the memory.
  while (pos >= 0) {
    // If we are within a label, rewind to the beginning of it and see if
    // it has the proper sub-label that we're looking for.
		
    if (inst_set->IsNop( search_genome[pos] )) {
      // Find the start and end of the label we're in the middle of.
			
      int start_pos = pos;
      int end_pos = pos + 1;
      while (start_pos > 0 && inst_set->IsNop(search_genome[start_pos - 1])) {
				start_pos--;
      }
      while (end_pos < search_start &&
						 inst_set->IsNop(search_genome[end_pos])) {
				end_pos++;
      }
      int test_size = end_pos - start_pos;
			
      // See if this label has the proper sub-label within it.
      int max_offset = test_size - label_size + 1;
      for (int offset = start_pos; offset < start_pos + max_offset; offset++) {
				
				// Test the number of matches for this offset.
				int matches;
				for (matches = 0; matches < label_size; matches++) {
					if (search_label[matches] !=
							inst_set->GetNopMod(search_genome[offset + matches])) {
						break;
					}
				}
				
				// If we have found it, break out of this loop!
				if (matches == label_size) {
					found_label = true;
					break;
				}
      }
			
      // If we've found the complement label, set the position to the end of
      // the label we found it in, and break out.
			
      if (found_label == true) {
				pos = end_pos;
				break;
      }
			
      // We haven't found it; jump pos to just before the current label
      // being checked.
      pos = start_pos - 1;
    }
		
    // Jump up a block to the next possible point to find a label,
    pos -= label_size;
  }
	
  // If the label was not found return a -1.
  if (found_label == false) pos = -1;
	
  return pos;
}

// Search for 'in_label' anywhere in the hardware.
cHeadMultiMem cHardwareSMT::FindLabel(const cCodeLabel & in_label, int direction)
{
  assert (in_label.GetSize() > 0);
	
  // IDEALY:
  // Keep making jumps (in the proper direction) equal to the label
  // length.  If we are inside of a label, check its size, and see if
  // any of the sub-labels match properly.
  // FOR NOW:
  // Get something which works, no matter how inefficient!!!
	
  cHeadMultiMem temp_head(this);
	
  while (temp_head.InMemory()) {
    // IDEALY: Analyze the label we are in; see if the one we are looking
    // for could be a sub-label of it.  Skip past it if not.
		
    int i;
    for (i = 0; i < in_label.GetSize(); i++) {
      if (!inst_set->IsNop(temp_head.GetInst()) ||
					in_label[i] != inst_set->GetNopMod(temp_head.GetInst())) {
				break;
      }
    }
    if (i == GetLabel().GetSize()) {
      temp_head.AbsJump(i - 1);
      return temp_head;
    }
		
    temp_head.AbsJump(direction);     // IDEALY: MAKE LARGER JUMPS
  }
	
  temp_head.AbsSet(-1);
  return temp_head;
}

// @CAO: direction is not currently used; should be used to indicate the
// direction which the heads[HEAD_IP] should progress through a creature.
cHeadMultiMem cHardwareSMT::FindFullLabel(const cCodeLabel & in_label)
{
  // cout << "Running FindFullLabel with " << in_label.AsString() <<
  // endl;
	
  assert(in_label.GetSize() > 0); // Trying to find label of 0 size!
	
  cHeadMultiMem temp_head(this);
	
  while (temp_head.InMemory()) {
    // If we are not in a label, jump to the next checkpoint...
    if (inst_set->IsNop(temp_head.GetInst())) {
      temp_head.AbsJump(in_label.GetSize());
      continue;
    }
		
    // Otherwise, rewind to the begining of this label...
		
    while (!(temp_head.AtFront()) && inst_set->IsNop(temp_head.GetInst(-1)))
      temp_head.AbsJump(-1);
		
    // Calculate the size of the label being checked, and make sure they
    // are equal.
		
    int checked_size = 0;
    while (inst_set->IsNop(temp_head.GetInst(checked_size))) {
      checked_size++;
    }
    if (checked_size != in_label.GetSize()) {
      temp_head.AbsJump(checked_size + 1);
      continue;
    }
		
    // cout << "Testing label at line " << temp_head.GetPosition() <<
    // endl;
		
    // ...and do the comparison...
		
    int j;
    bool label_match = true;
    for (j = 0; j < in_label.GetSize(); j++) {
      if (!inst_set->IsNop(temp_head.GetInst(j)) ||
					in_label[j] != inst_set->GetNopMod(temp_head.GetInst(j))) {
				temp_head.AbsJump(in_label.GetSize() + 1);
				label_match = false;
				break;
      }
    }
		
    if (label_match) {
      // If we have found the label, return the position after it.
      temp_head.AbsJump(j - 1);
      return temp_head;
    }
		
    // We have not found the label... increment i.
		
    temp_head.AbsJump(in_label.GetSize() + 1);
  }
	
  // The label does not exist in this creature.
	
  temp_head.AbsSet(-1);
  return temp_head;
}

// This is the code run by the INFECTED organism.  Its function is to SPREAD infection.
bool cHardwareSMT::InjectParasite(double mut_multiplier)
{
  const int end_pos = GetHead(HEAD_WRITE).GetPosition();
  const int mem_space_used = GetHead(HEAD_WRITE).GetMemSpace();
  
  // Make sure the creature will still be above the minimum size,
  // TEMPORARY!  INJECTED CODE CAN 
  if (end_pos <= 0) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: no code to inject");
    return false; // (inject fails)
  }
  
  if (end_pos < MIN_INJECT_SIZE) {
    GetMemory(mem_space_used)=cGenome(ConvertToInstruction(mem_space_used)); 
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: new size too small");
    return false; // (inject fails)
  }
	
  GetMemory(mem_space_used).Resize(end_pos);
	
  cCPUMemory injected_code = GetMemory(mem_space_used);
	
  Inject_DoMutations(mut_multiplier, injected_code);
	
  int inject_signal = false;
	
  if(injected_code.GetSize()>0)
    inject_signal = organism->InjectParasite(injected_code);
  
  //************* CALL GOES HERE ******************//
  // spin around randomly (caution: possible organism dizziness)
  //const int num_neighbors = organism->GetNeighborhoodSize();
  //for(unsigned int i=0; i<g_random.GetUInt(num_neighbors); i++)
  //  organism->Rotate(1);
	
  // If we don't have a host, stop here.
  //cOrganism * host_organism = organism->GetNeighbor();
  
	
  //if(host_organism!=NULL)
  //  {
  //    
  //  }
	
  //************** CALL ENDS HERE ******************//
	
  //reset the memory space which was injected
  GetMemory(mem_space_used)=cGenome(ConvertToInstruction(mem_space_used)); 
	
  for(int x=0; x<NUM_HEADS; x++) {
		GetHead(x).Reset(IP().GetMemSpace(), this);
	}
	
  for(int x=0; x < nHardwareSMT::NUM_LOCAL_STACKS; x++) {
		Stack(x).Clear();
	}
  
  AdvanceIP() = false;
  
  return inject_signal;
}

//This is the code run by the TARGET of an injection.  This RECIEVES the infection.
bool cHardwareSMT::InjectHost(const cCodeLabel & in_label, const cGenome & inject_code)
{
  // Make sure the genome will be below max size after injection.
	
  // xxxTEMPORARYxxx - we should have this match injection templates.  For now it simply 
  
	// FIND THE FIRST EMPTY MEMORY SPACE
  int target_mem_space;
  for (target_mem_space = 0; target_mem_space < m_mem_array.GetSize(); target_mem_space++)
	{
		if(isEmpty(target_mem_space))
		{
			break;
		}
	}
  
  if (target_mem_space == m_mem_array.GetSize())
	{
		return false;
	}
	
  assert(target_mem_space >=0 && target_mem_space < m_mem_array.GetSize());
  
  if(ForkThread()) {
    // Inject the new code
    cCPUMemory oldcode = GetMemory(target_mem_space);
    GetMemory(target_mem_space) = inject_code;
    GetMemory(target_mem_space).Resize(inject_code.GetSize() + oldcode.GetSize());
		
    // Copies previous instructions to the end of the injected code.
    // Is there a faster way to do this?? -law
    for(int x=0; x<oldcode.GetSize(); x++)
      GetMemory(target_mem_space)[inject_code.GetSize()+x] = oldcode[x];
		
    // Set instruction flags on the injected code
    for (int i = 0; i < inject_code.GetSize(); i++) {
      m_mem_array[target_mem_space].FlagInjected(i) = true;
    }
    organism->GetPhenotype().IsModified() = true;
    
    // Adjust all of the heads to take into account the new mem size.
    
    m_cur_thread=GetNumThreads()-1;
    
    for(int i=0; i<m_cur_thread; i++) {
      for(int j=0; j < NUM_HEADS; j++) {
				if(m_threads[i].heads[j].GetMemSpace()==target_mem_space)
					m_threads[i].heads[j].Jump(inject_code.GetSize());
      }
    }
    
    for (int i=0; i < NUM_HEADS; i++) {    
      GetHead(i).Reset(target_mem_space, this);
    }
    for (int i=0; i < nHardwareSMT::NUM_LOCAL_STACKS; i++) {
      Stack(i).Clear();
    }
  }
	
  return true; // (inject succeeds!)
}

void cHardwareSMT::Mutate(int mut_point)
{
  // Test if trying to mutate outside of genome...
  assert(mut_point >= 0 && mut_point < GetMemory(0).GetSize());
	
  GetMemory(0)[mut_point] = GetRandomInst();
  GetMemory(0).FlagMutated(mut_point) = true;
  GetMemory(0).FlagPointMut(mut_point) = true;
  //organism->GetPhenotype().IsMutated() = true;
  organism->CPUStats().mut_stats.point_mut_count++;
}

int cHardwareSMT::PointMutate(const double mut_rate)
{
  const int num_muts =
	g_random.GetRandBinomial(GetMemory(0).GetSize(), mut_rate);
	
  for (int i = 0; i < num_muts; i++) {
    const int pos = g_random.GetUInt(GetMemory(0).GetSize());
    Mutate(pos);
  }
	
  return num_muts;
}


// Trigger mutations of a specific type.  Outside triggers cannot specify
// a head since hardware types are not known.

bool cHardwareSMT::TriggerMutations(int trigger)
{
  // Only update triggers should happen from the outside!
  assert(trigger == MUTATION_TRIGGER_UPDATE);
	
  // Assume instruction pointer is the intended target (if one is even
  // needed!
	
  return TriggerMutations(trigger, IP());
}

bool cHardwareSMT::TriggerMutations(int trigger, cHeadMultiMem & cur_head)
{
  // Collect information about mutations from the organism.
  cLocalMutations & mut_info = organism->GetLocalMutations();
  const tList<cMutation> & mut_list =
    mut_info.GetMutationLib().GetMutationList(trigger);
	
  // If we have no mutations for this trigger, stop here.
  if (mut_list.GetSize() == 0) return false;
  bool has_mutation = false;
	
  // Determine what memory this mutation will be affecting.
  cCPUMemory & target_mem = (trigger == MUTATION_TRIGGER_DIVIDE) 
    ? organism->ChildGenome() : GetMemory(0);
	
  // Loop through all mutations associated with this trigger and test them.
  tConstListIterator<cMutation> mut_it(mut_list);
	
  while (mut_it.Next() != NULL) {
    const cMutation * cur_mut = mut_it.Get();
    const int mut_id = cur_mut->GetID();
    const int scope = cur_mut->GetScope();
    const double rate = mut_info.GetRate(mut_id);
    switch (scope) {
			case MUTATION_SCOPE_GENOME:
				if (TriggerMutations_ScopeGenome(cur_mut, target_mem, cur_head, rate)) {
					has_mutation = true;
					mut_info.IncCount(mut_id);
				}
				break;
			case MUTATION_SCOPE_LOCAL:
			case MUTATION_SCOPE_PROP:
				if (TriggerMutations_ScopeLocal(cur_mut, target_mem, cur_head, rate)) {
					has_mutation = true;
					mut_info.IncCount(mut_id);
				}
				break;
			case MUTATION_SCOPE_GLOBAL:
			case MUTATION_SCOPE_SPREAD:
				int num_muts =
				TriggerMutations_ScopeGlobal(cur_mut, target_mem, cur_head, rate);
				if (num_muts > 0) {
					has_mutation = true;
					mut_info.IncCount(mut_id, num_muts);
				}
					break;
    }
  }
	
  return has_mutation;
}

bool cHardwareSMT::TriggerMutations_ScopeGenome(const cMutation * cur_mut,
                                                cCPUMemory & target_memory, cHeadMultiMem & cur_head, const double rate)
{
  // The rate we have stored indicates the probability that a single
  // mutation will occur anywhere in the genome.
  
  if (g_random.P(rate) == true) {
    // We must create a temporary head and use it to randomly determine the
    // position in the genome to be mutated.
    cHeadMultiMem tmp_head(cur_head);
    tmp_head.AbsSet(g_random.GetUInt(target_memory.GetSize()));
    TriggerMutations_Body(cur_mut->GetType(), target_memory, tmp_head);
    return true;
  }
  return false;
}

bool cHardwareSMT::TriggerMutations_ScopeLocal(const cMutation * cur_mut,
                                               cCPUMemory & target_memory, cHeadMultiMem & cur_head, const double rate)
{
  // The rate we have stored is the probability for a mutation at this single
  // position in the genome.
	
  if (g_random.P(rate) == true) {
    TriggerMutations_Body(cur_mut->GetType(), target_memory, cur_head);
    return true;
  }
  return false;
}

int cHardwareSMT::TriggerMutations_ScopeGlobal(const cMutation * cur_mut,
                                               cCPUMemory & target_memory, cHeadMultiMem & cur_head, const double rate)
{
  // The probability we have stored is per-site, so we can pull a random
  // number from a binomial distribution to determine the number of mutations
  // that should occur.
	
  const int num_mut =
	g_random.GetRandBinomial(target_memory.GetSize(), rate);
	
  if (num_mut > 0) {
    for (int i = 0; i < num_mut; i++) {
      cHeadMultiMem tmp_head(cur_head);
      tmp_head.AbsSet(g_random.GetUInt(target_memory.GetSize()));
      TriggerMutations_Body(cur_mut->GetType(), target_memory, tmp_head);
    }
  }
	
  return num_mut;
}

void cHardwareSMT::TriggerMutations_Body(int type, cCPUMemory & target_memory,
                                         cHeadMultiMem & cur_head)
{
  const int pos = cur_head.GetPosition();
	
  switch (type) {
		case MUTATION_TYPE_POINT:
			target_memory[pos] = GetRandomInst();
			target_memory.FlagMutated(pos) = true;
			break;
		case MUTATION_TYPE_INSERT:
		case MUTATION_TYPE_DELETE:
		case MUTATION_TYPE_HEAD_INC:
		case MUTATION_TYPE_HEAD_DEC:
		case MUTATION_TYPE_TEMP:
		case MUTATION_TYPE_KILL:
		default:
			cout << "Error: Mutation type not implemented!" << endl;
			break;
  };
}

void cHardwareSMT::ReadInst(const int in_inst)
{
  if (inst_set->IsNop( cInstruction(in_inst) )) {
    GetReadLabel().AddNop(in_inst);
  } else {
    GetReadLabel().Clear();
  }
}


void cHardwareSMT::AdjustHeads()
{
  for (int i = 0; i < GetNumThreads(); i++) {
    for (int j = 0; j < NUM_HEADS; j++) {
      m_threads[i].heads[j].Adjust();
    }
  }
}



// This function looks at the current position in the info of a creature,
// and sets the next_label to be the sequence of nops which follows.  The
// instruction pointer is left on the last line of the label found.

void cHardwareSMT::ReadLabel(int max_size)
{
  int count = 0;
  cHeadMultiMem* inst_ptr = &( IP() );
	
  GetLabel().Clear();
	
  while (inst_set->IsNop(inst_ptr->GetNextInst()) &&
				 (count < max_size)) {
    count++;
    inst_ptr->Advance();
    GetLabel().AddNop(inst_set->GetNopMod(inst_ptr->GetInst()));
		
    // If this is the first line of the template, mark it executed.
    if (GetLabel().GetSize() <=	cConfig::GetMaxLabelExeSize()) {
      inst_ptr->FlagExecuted() = true;
    }
  }
}


bool cHardwareSMT::ForkThread()
{
  const int num_threads = GetNumThreads();
  if (num_threads == cConfig::GetMaxCPUThreads()) return false;
	
  // Make room for the new thread.
  m_threads.Resize(num_threads + 1);
	
  //IP().Advance();
	
  // Initialize the new thread to the same values as the current one.
  m_threads[num_threads] = m_threads[m_cur_thread]; 
	
  // Find the first free bit in thread_id_chart to determine the new
  // thread id.
  int new_id = 0;
  while ( (thread_id_chart >> new_id) & 1 == 1) new_id++;
  m_threads[num_threads].SetID(new_id);
  thread_id_chart |= (1 << new_id);
	
  return true;
}


int cHardwareSMT::TestParasite() const
{
  return IP().TestParasite();
}


bool cHardwareSMT::KillThread()
{
  // Make sure that there is always at least one thread...
  if (GetNumThreads() == 1) return false;
	
  // Note the current thread and set the current back one.
  const int kill_thread = m_cur_thread;
  PrevThread();
  
  // Turn off this bit in the thread_id_chart...
  thread_id_chart ^= 1 << m_threads[kill_thread].GetID();
	
  // Copy the last thread into the kill position
  const int last_thread = GetNumThreads() - 1;
  if (last_thread != kill_thread) {
    m_threads[kill_thread] = m_threads[last_thread];
  }
	
  // Kill the thread!
  m_threads.Resize(GetNumThreads() - 1);
	
  if (m_cur_thread > kill_thread) m_cur_thread--;
	
  return true;
}


void cHardwareSMT::SaveState(ostream & fp)
{
  // note, memory & child_memory handled by cpu (@CAO Not any more!)
  assert(fp.good());
	
  fp<<"cHardwareSMT"<<endl;
	
  // global_stack (in inverse order so load can just push)
  for(int i = nHardwareSMT::NUM_LOCAL_STACKS; i < nHardwareSMT::NUM_STACKS; i++)
    Stack(i).SaveState(fp);
	
  //fp << thread_time_used  << endl;
  fp << GetNumThreads()   << endl;
  fp << m_cur_thread        << endl;
	
  // Threads
  for( int i = 0; i < GetNumThreads(); i++ ) {
    m_threads[i].SaveState(fp);
  }
}


void cHardwareSMT::LoadState(istream & fp)
{
  // note, memory & child_memory handled by cpu (@CAO Not any more!)
  assert(fp.good());
	
  cString foo;
  fp>>foo;
  assert( foo == "cHardwareSMT" );
	
  // global_stack
  for(int i = nHardwareSMT::NUM_LOCAL_STACKS; i < nHardwareSMT::NUM_STACKS; i++)
    Stack(i).LoadState(fp);
	
  int num_threads;
  //fp >> thread_time_used;
  fp >> num_threads;
  fp >> m_cur_thread;
	
  // Threads
  for( int i = 0; i < num_threads; i++ ){
    m_threads[i].LoadState(fp);
  }
}


////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int cHardwareSMT::FindModifiedStack(int default_stack)
{
  assert(default_stack < nHardwareSMT::NUM_STACKS);  // Stack ID too high.
	
  if (GetInstSet().IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_stack = GetInstSet().GetNopMod(IP().GetInst());
    IP().FlagExecuted() = true;
  }
  return default_stack;
}

inline int cHardwareSMT::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.
	
  if (GetInstSet().IsNop(IP().GetNextInst())) {
    IP().Advance();    
    int nop_head = GetInstSet().GetNopMod(IP().GetInst());
    if (nop_head < NUM_HEADS) default_head = nop_head;
    IP().FlagExecuted() = true;
  }
  return default_head;
}

inline int cHardwareSMT::FindComplementStack(int base_stack)
{
  const int comp_stack = base_stack + 2;
  return comp_stack % nHardwareSMT::NUM_STACKS;
}

inline void cHardwareSMT::Fault(int fault_loc, int fault_type, cString fault_desc)
{
  organism->Fault(fault_loc, fault_type, fault_desc);
}

bool cHardwareSMT::Divide_CheckViable(const int parent_size,
                                      const int child_size, const int mem_space)
{
  // Make sure the organism is okay with dividing now...
  if (organism->Divide_CheckViable() == false) return false; // (divide fails)
	
  // Make sure that neither parent nor child will be below the minimum size.
	
  const int genome_size = organism->GetGenome().GetSize();
  const double size_range = cConfig::GetChildSizeRange();
  const int min_size = Max(MIN_CREATURE_SIZE, (int) (genome_size/size_range));
  const int max_size = Min(MAX_CREATURE_SIZE, (int) (genome_size*size_range));
  
  if (child_size < min_size || child_size > max_size) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
					cStringUtil::Stringf("Invalid offspring length (%d)", child_size));
    return false; // (divide fails)
  }
	
  // Count the number of lines executed in the parent, and make sure the
  // specified fraction has been reached.
	
  int executed_size = 0;
  for (int i = 0; i < parent_size; i++) {
    if (GetMemory(0).FlagExecuted(i)) executed_size++;
  }
	
  const int min_exe_lines = (int) (parent_size * cConfig::GetMinExeLines());
  if (executed_size < min_exe_lines) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
					cStringUtil::Stringf("Too few executed lines (%d < %d)",
															 executed_size, min_exe_lines));
    return false; // (divide fails)
  }
	
  // Count the number of lines which were copied into the child, and make
  // sure the specified fraction has been reached.
	
  int copied_size = 0;
  for (int i = 0; i < GetMemory(mem_space).GetSize(); i++) {
    if (GetMemory(mem_space).FlagCopied(i)) copied_size++;
	}
	
  const int min_copied =  (int) (child_size * cConfig::GetMinCopiedLines());
  if (copied_size < min_copied) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
					cStringUtil::Stringf("Too few copied commands (%d < %d)",
															 copied_size, min_copied));
    return false; // (divide fails)
  }
	
  // Save the information we collected here...
  organism->GetPhenotype().SetLinesExecuted(executed_size);
  organism->GetPhenotype().SetLinesCopied(copied_size);
	
  return true; // (divide succeeds!)
}

void cHardwareSMT::Divide_DoMutations(double mut_multiplier)
{
  sCPUStats & cpu_stats = organism->CPUStats();
  cCPUMemory & child_genome = organism->ChildGenome();
  
  organism->GetPhenotype().SetDivType(mut_multiplier);
	
  // Divide Mutations
  if (organism->TestDivideMut()) {
    const UINT mut_line = g_random.GetUInt(child_genome.GetSize());
    child_genome[mut_line] = GetRandomInst();
    cpu_stats.mut_stats.divide_mut_count++;
  }
	
  // Divide Insertions
  if (organism->TestDivideIns() && child_genome.GetSize() < MAX_CREATURE_SIZE){
    const UINT mut_line = g_random.GetUInt(child_genome.GetSize() + 1);
    child_genome.Insert(mut_line, GetRandomInst());
    cpu_stats.mut_stats.divide_insert_mut_count++;
  }
	
  // Divide Deletions
  if (organism->TestDivideDel() && child_genome.GetSize() > MIN_CREATURE_SIZE){
    const UINT mut_line = g_random.GetUInt(child_genome.GetSize());
    // if( child_genome.FlagCopied(mut_line) == true) copied_size_change--;
    child_genome.Remove(mut_line);
    cpu_stats.mut_stats.divide_delete_mut_count++;
  }
	
  // Divide Mutations (per site)
  if(organism->GetDivMutProb() > 0){
    int num_mut = g_random.GetRandBinomial(child_genome.GetSize(), 
																					 organism->GetDivMutProb() / mut_multiplier);
    // If we have lines to mutate...
    if( num_mut > 0 ){
      for (int i = 0; i < num_mut; i++) {
				int site = g_random.GetUInt(child_genome.GetSize());
				child_genome[site]=GetRandomInst();
				cpu_stats.mut_stats.div_mut_count++;
      }
    }
  }
	
	
  // Insert Mutations (per site)
  if(organism->GetInsMutProb() > 0){
    int num_mut = g_random.GetRandBinomial(child_genome.GetSize(),
																					 organism->GetInsMutProb());
    // If would make creature to big, insert up to MAX_CREATURE_SIZE
    if( num_mut + child_genome.GetSize() > MAX_CREATURE_SIZE ){
      num_mut = MAX_CREATURE_SIZE - child_genome.GetSize();
    }
    // If we have lines to insert...
    if( num_mut > 0 ){
      // Build a list of the sites where mutations occured
      static int mut_sites[MAX_CREATURE_SIZE];
      for (int i = 0; i < num_mut; i++) {
				mut_sites[i] = g_random.GetUInt(child_genome.GetSize() + 1);
      }
      // Sort the list
      qsort( (void*)mut_sites, num_mut, sizeof(int), &IntCompareFunction );
      // Actually do the mutations (in reverse sort order)
      for(int i = num_mut-1; i >= 0; i--) {
				child_genome.Insert(mut_sites[i], GetRandomInst());
				cpu_stats.mut_stats.insert_mut_count++;
      }
    }
  }
	
	
  // Delete Mutations (per site)
  if( organism->GetDelMutProb() > 0 ){
    int num_mut = g_random.GetRandBinomial(child_genome.GetSize(),
																					 organism->GetDelMutProb());
    // If would make creature too small, delete down to MIN_CREATURE_SIZE
    if (child_genome.GetSize() - num_mut < MIN_CREATURE_SIZE) {
      num_mut = child_genome.GetSize() - MIN_CREATURE_SIZE;
    }
		
    // If we have lines to delete...
    for (int i = 0; i < num_mut; i++) {
      int site = g_random.GetUInt(child_genome.GetSize());
      // if (child_genome.FlagCopied(site) == true) copied_size_change--;
      child_genome.Remove(site);
      cpu_stats.mut_stats.delete_mut_count++;
    }
  }
	
  // Mutations in the parent's genome
  if (organism->GetParentMutProb() > 0) {
    for (int i = 0; i < GetMemory(0).GetSize(); i++) {
      if (organism->TestParentMut()) {
				GetMemory(0)[i] = GetRandomInst();
				cpu_stats.mut_stats.parent_mut_line_count++;
      }
    }
  }
	
	
  // Count up mutated lines
  for(int i = 0; i < GetMemory(0).GetSize(); i++){
    if (GetMemory(0).FlagPointMut(i) == true) {
      cpu_stats.mut_stats.point_mut_line_count++;
    }
  }
  for(int i = 0; i < child_genome.GetSize(); i++){
    if( child_genome.FlagCopyMut(i) == true) {
      cpu_stats.mut_stats.copy_mut_line_count++;
    }
  }
}

void cHardwareSMT::Inject_DoMutations(double mut_multiplier, cCPUMemory & injected_code)
{
  //sCPUStats & cpu_stats = organism->CPUStats();
  //cCPUMemory & child_genome = organism->ChildGenome();
  
  organism->GetPhenotype().SetDivType(mut_multiplier);
	
  // Divide Mutations
  if (organism->TestDivideMut()) {
    const UINT mut_line = g_random.GetUInt(injected_code.GetSize());
    injected_code[mut_line] = GetRandomInst();
    //cpu_stats.mut_stats.divide_mut_count++;
  }
	
  // Divide Insertions
  if (organism->TestDivideIns() && injected_code.GetSize() < MAX_CREATURE_SIZE){
    const UINT mut_line = g_random.GetUInt(injected_code.GetSize() + 1);
    injected_code.Insert(mut_line, GetRandomInst());
    //cpu_stats.mut_stats.divide_insert_mut_count++;
  }
	
  // Divide Deletions
  if (organism->TestDivideDel() && injected_code.GetSize() > MIN_CREATURE_SIZE){
    const UINT mut_line = g_random.GetUInt(injected_code.GetSize());
    // if( injected_code.FlagCopied(mut_line) == true) copied_size_change--;
    injected_code.Remove(mut_line);
    //cpu_stats.mut_stats.divide_delete_mut_count++;
  }
	
  // Divide Mutations (per site)
  if(organism->GetDivMutProb() > 0){
    int num_mut = g_random.GetRandBinomial(injected_code.GetSize(), 
																					 organism->GetDivMutProb() / mut_multiplier);
    // If we have lines to mutate...
    if( num_mut > 0 ){
      for (int i = 0; i < num_mut; i++) {
				int site = g_random.GetUInt(injected_code.GetSize());
				injected_code[site]=GetRandomInst();
				//cpu_stats.mut_stats.div_mut_count++;
      }
    }
  }
	
	
  // Insert Mutations (per site)
  if(organism->GetInsMutProb() > 0){
    int num_mut = g_random.GetRandBinomial(injected_code.GetSize(),
																					 organism->GetInsMutProb());
    // If would make creature to big, insert up to MAX_CREATURE_SIZE
    if( num_mut + injected_code.GetSize() > MAX_CREATURE_SIZE ){
      num_mut = MAX_CREATURE_SIZE - injected_code.GetSize();
    }
    // If we have lines to insert...
    if( num_mut > 0 ){
      // Build a list of the sites where mutations occured
      static int mut_sites[MAX_CREATURE_SIZE];
      for (int i = 0; i < num_mut; i++) {
				mut_sites[i] = g_random.GetUInt(injected_code.GetSize() + 1);
      }
      // Sort the list
      qsort( (void*)mut_sites, num_mut, sizeof(int), &IntCompareFunction );
      // Actually do the mutations (in reverse sort order)
      for(int i = num_mut-1; i >= 0; i--) {
				injected_code.Insert(mut_sites[i], GetRandomInst());
				//cpu_stats.mut_stats.insert_mut_count++;
      }
    }
  }
	
	
  // Delete Mutations (per site)
  if( organism->GetDelMutProb() > 0 ){
    int num_mut = g_random.GetRandBinomial(injected_code.GetSize(),
																					 organism->GetDelMutProb());
    // If would make creature too small, delete down to MIN_CREATURE_SIZE
    if (injected_code.GetSize() - num_mut < MIN_CREATURE_SIZE) {
      num_mut = injected_code.GetSize() - MIN_CREATURE_SIZE;
    }
		
    // If we have lines to delete...
    for (int i = 0; i < num_mut; i++) {
      int site = g_random.GetUInt(injected_code.GetSize());
      // if (injected_code.FlagCopied(site) == true) copied_size_change--;
      injected_code.Remove(site);
      //cpu_stats.mut_stats.delete_mut_count++;
    }
  }
	
  // Mutations in the parent's genome
  if (organism->GetParentMutProb() > 0) {
    for (int i = 0; i < GetMemory(0).GetSize(); i++) {
      if (organism->TestParentMut()) {
				GetMemory(0)[i] = GetRandomInst();
				//cpu_stats.mut_stats.parent_mut_line_count++;
      }
    }
  }
	
  /*
	 // Count up mutated lines
	 for(int i = 0; i < GetMemory(0).GetSize(); i++){
		 if (GetMemory(0).FlagPointMut(i) == true) {
			 cpu_stats.mut_stats.point_mut_line_count++;
		 }
	 }
	 for(int i = 0; i < injected_code.GetSize(); i++){
		 if( injected_code.FlagCopyMut(i) == true) {
			 cpu_stats.mut_stats.copy_mut_line_count++;
		 }
	 }*/
}


// test whether the offspring creature contains an advantageous mutation.
void cHardwareSMT::Divide_TestFitnessMeasures()
{
  cPhenotype & phenotype = organism->GetPhenotype();
  phenotype.CopyTrue() = ( organism->ChildGenome() == organism->GetGenome() );
  phenotype.ChildFertile() = true;
	
  // Only continue if we're supposed to do a fitness test on divide...
  if (organism->GetTestOnDivide() == false) return;
	
  // If this was a perfect copy, then we don't need to worry about any other
  // tests...  Theoretically, we need to worry about the parent changing,
  // but as long as the child is always compared to the original genotype,
  // this won't be an issue.
  if (phenotype.CopyTrue() == true) return;
	
  const double parent_fitness = organism->GetTestFitness();
  const double neut_min = parent_fitness * FITNESS_NEUTRAL_MIN;
  const double neut_max = parent_fitness * FITNESS_NEUTRAL_MAX;
  
  cCPUTestInfo test_info;
  test_info.UseRandomInputs();
  cTestCPU::TestGenome(test_info, organism->ChildGenome());
  const double child_fitness = test_info.GetGenotypeFitness();
  
  bool revert = false;
  bool sterilize = false;
  
  // If implicit mutations are turned off, make sure this won't spawn one.
  if (organism->GetFailImplicit() == true) {
    if (test_info.GetMaxDepth() > 0) sterilize = true;
  }
  
  if (child_fitness == 0.0) {
    // Fatal mutation... test for reversion.
    if (g_random.P(organism->GetRevertFatal())) revert = true;
    if (g_random.P(organism->GetSterilizeFatal())) sterilize = true;
  } else if (child_fitness < neut_min) {
    if (g_random.P(organism->GetRevertNeg())) revert = true;
    if (g_random.P(organism->GetSterilizeNeg())) sterilize = true;
  } else if (child_fitness <= neut_max) {
    if (g_random.P(organism->GetRevertNeut())) revert = true;
    if (g_random.P(organism->GetSterilizeNeut())) sterilize = true;
  } else {
    if (g_random.P(organism->GetRevertPos())) revert = true;
    if (g_random.P(organism->GetSterilizePos())) sterilize = true;
  }
  
  // Ideally, we won't have reversions and sterilizations turned on at the
  // same time, but if we do, give revert the priority.
  if (revert == true) {
    organism->ChildGenome() = organism->GetGenome();
  }
	
  if (sterilize == true) {
    organism->GetPhenotype().ChildFertile() = false;
  }
}


bool cHardwareSMT::Divide_Main(int mem_space_used, double mut_multiplier)
{
  int write_head_pos = GetHead(HEAD_WRITE).GetPosition();
  
  // We're going to disallow division calls from memory spaces other than zero 
  // for right now -law
  if(IP().GetMemSpace()!=0)
    return false;
	
  // Make sure this divide will produce a viable offspring.
  if(!Divide_CheckViable(GetMemory(IP().GetMemSpace()).GetSize(), 
												 write_head_pos, mem_space_used)) 
    return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  cGenome & child_genome = organism->ChildGenome();
  GetMemory(mem_space_used).Resize(write_head_pos);
  child_genome = GetMemory(mem_space_used);
	
  // Handle Divide Mutations...
  Divide_DoMutations(mut_multiplier);
	
  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures();
	
#ifdef INSTRUCTION_COSTS
  // reset first time instruction costs
  for (int i = 0; i < inst_ft_cost.GetSize(); i++) {
    inst_ft_cost[i] = GetInstSet().GetFTCost(cInstruction(i));
  }
#endif
	
  bool parent_alive = organism->ActivateDivide();
	
  //reset the memory of the memory space that has been divided off
  GetMemory(mem_space_used)=cGenome(ConvertToInstruction(mem_space_used)); 
	
  // 3 Division Methods:
  // 1) DIVIDE_METHOD_OFFSPRING - Create a child, leave parent state untouched.
  // 2) DIVIDE_METHOD_SPLIT - Create a child, completely reset state of parent.
  // 3) DIVIDE_METHOD_BIRTH - Create a child, reset state of parent's current thread.
  if(parent_alive && !(cConfig::GetDivideMethod() == DIVIDE_METHOD_OFFSPRING))
	{
		
		if(cConfig::GetDivideMethod() == DIVIDE_METHOD_SPLIT)
		{
			//this will wipe out all parasites on a divide.
			Reset();
			
		}
		else if(cConfig::GetDivideMethod() == DIVIDE_METHOD_BIRTH)
		{
			//if this isn't the only thread, get rid of it!
			// ***this can cause a concurrency problem if we have 
			// multiprocessor support for single organisms...don't 
			// think that's happening anytime soon though -law ***
			if(!organism->GetPhenotype().IsModified() && GetNumThreads()>1 || 
				 GetNumThreads()>2)
	    {
	      KillThread();
	    }
			
			//this will reset the current thread's heads and stacks.  It will 
			//not touch any other m_threads or memory spaces (ie: parasites)
			else
	    {
	      for(int x = 0; x < NUM_HEADS; x++)
				{
					GetHead(x).Reset(0, this);
				}
	      for(int x = 0; x < nHardwareSMT::NUM_LOCAL_STACKS; x++)
				{
					Stack(x).Clear();
				}	  
	    }
		}
		AdvanceIP()=false;
	}
	
  return true;
}

cString cHardwareSMT::ConvertToInstruction(int mem_space_used)
{
  char c = mem_space_used + 97;  // 97 - ASCII for 'a'
  cString ret;
  ret += c;
  return ret;
}

cString cHardwareSMT::GetActiveStackID(int stackID) const
{
  if(stackID == nHardwareSMT::STACK_AX)
    return "AX";
  else if(stackID == nHardwareSMT::STACK_BX)
    return "BX";
  else if(stackID == nHardwareSMT::STACK_CX)
    return "CX";
  else if(stackID == nHardwareSMT::STACK_DX)
    return "DX";
  else
    return "";
}


//////////////////////////
// And the instructions...
//////////////////////////

//6
bool cHardwareSMT::Inst_ShiftR()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  int value = Stack(stack_used).Pop();
  value >>= 1;
  Stack(stack_used).Push(value);
  return true;
}

//7
bool cHardwareSMT::Inst_ShiftL()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  int value = Stack(stack_used).Pop();
  value <<= 1;
  Stack(stack_used).Push(value);
  return true;
}

//8
bool cHardwareSMT::Inst_Val_Nand()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  Stack(stack_used).Push(~(Stack(nHardwareSMT::STACK_BX).Top() & Stack(nHardwareSMT::STACK_CX).Top()));
  return true;
}

//9
bool cHardwareSMT::Inst_Val_Add()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  Stack(stack_used).Push(Stack(nHardwareSMT::STACK_BX).Top() + Stack(nHardwareSMT::STACK_CX).Top());
  return true;
}

//10
bool cHardwareSMT::Inst_Val_Sub()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  Stack(stack_used).Push(Stack(nHardwareSMT::STACK_BX).Top() - Stack(nHardwareSMT::STACK_CX).Top());
  return true;
}

//11
bool cHardwareSMT::Inst_Val_Mult()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  Stack(stack_used).Push(Stack(nHardwareSMT::STACK_BX).Top() * Stack(nHardwareSMT::STACK_CX).Top());
  return true;
}

//12
bool cHardwareSMT::Inst_Val_Div()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  if (Stack(nHardwareSMT::STACK_CX).Top() != 0) {
    if (0-INT_MAX > Stack(nHardwareSMT::STACK_BX).Top() && Stack(nHardwareSMT::STACK_CX).Top() == -1)
      Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: Float exception");
    else
      Stack(stack_used).Push(Stack(nHardwareSMT::STACK_BX).Top() / Stack(nHardwareSMT::STACK_CX).Top());
  } else {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
    return false;
  }
  return true;
}

//13 
bool cHardwareSMT::Inst_SetMemory()   // Allocate maximal more
{
  ReadLabel();
  int mem_space_used = FindModifiedStack(-1);
  
  if (GetLabel().GetSize() == 0) {
    GetHead(HEAD_FLOW).Set(0, 0);
  } else {
    int mem_space_used = FindMemorySpaceLabel(-1);
    
    if (mem_space_used != -1) {
      
    } else {
      
    }
    
    mem_space_used = FindFirstEmpty();
    if(mem_space_used==-1)
      return false;
    GetHead(HEAD_FLOW).Set(0, mem_space_used);
  }
  
  return true;
}

//14
bool cHardwareSMT::Inst_Divide()
{
  int mem_space_used = GetHead(HEAD_WRITE).GetMemSpace();
  int mut_multiplier = 1;
	
  return Divide_Main(mem_space_used, mut_multiplier);
}

bool cHardwareSMT::Inst_HeadDivideMut(double mut_multiplier)
{
  // Unused for the moment...
  return true;
  //AdjustHeads();
  //const int divide_pos = GetHead(HEAD_READ).GetPosition();
  //int child_end =  GetHead(HEAD_WRITE).GetPosition();
  //if (child_end == 0) child_end = GetMemory(0).GetSize();
  //const int extra_lines = GetMemory(0).GetSize() - child_end;
  //bool ret_val = Divide_Main(divide_pos, extra_lines, mut_multiplier);
  //// Re-adjust heads.
  //AdjustHeads();
  //return ret_val; 
}

//15
bool cHardwareSMT::Inst_HeadRead()
{
  const int head_id = FindModifiedHead(HEAD_READ);
  GetHead(head_id).Adjust();
  sCPUStats & cpu_stats = organism->CPUStats();
	
  // Mutations only occur on the read, for the moment.
  int read_inst = 0;
  if (organism->TestCopyMut()) {
    read_inst = GetRandomInst().GetOp();
    cpu_stats.mut_stats.copy_mut_count++;  // @CAO, hope this is good!
  } else {
    read_inst = GetHead(head_id).GetInst().GetOp();
  }
  Stack(nHardwareSMT::STACK_AX).Push(read_inst);
  ReadInst(read_inst);
	
  cpu_stats.mut_stats.copies_exec++;  // @CAO, this too..
  GetHead(head_id).Advance();
  return true;
}

//16
bool cHardwareSMT::Inst_HeadWrite()
{
  const int head_id = FindModifiedHead(HEAD_WRITE);
  cHeadMultiMem & active_head = GetHead(head_id);
  int mem_space_used = active_head.GetMemSpace();
  
  //commented out for right now...
  if(active_head.GetPosition()>=GetMemory(mem_space_used).GetSize()-1)
	{
		GetMemory(mem_space_used).Resize(GetMemory(mem_space_used).GetSize()+1);
		GetMemory(mem_space_used).Copy(GetMemory(mem_space_used).GetSize()-1, GetMemory(mem_space_used).GetSize()-2);
	}
	
  active_head.Adjust();
	
  int value = Stack(nHardwareSMT::STACK_AX).Pop();
  if (value < 0 || value >= GetNumInst()) value = 0;
	
  active_head.SetInst(cInstruction(value));
  active_head.FlagCopied() = true;
	
  // Advance the head after write...
  active_head++;
  return true;
}

//??
bool cHardwareSMT::Inst_HeadCopy()
{
  // For the moment, this cannot be nop-modified.
  cHeadMultiMem & read_head = GetHead(HEAD_READ);
  cHeadMultiMem & write_head = GetHead(HEAD_WRITE);
  sCPUStats & cpu_stats = organism->CPUStats();
	
  read_head.Adjust();
  write_head.Adjust();
	
  // TriggerMutations(MUTATION_TRIGGER_READ, read_head);
  
  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  if (organism->TestCopyMut()) {
    read_inst = GetRandomInst();
    cpu_stats.mut_stats.copy_mut_count++; 
    write_head.FlagMutated() = true;
    write_head.FlagCopyMut() = true;
    //organism->GetPhenotype().IsMutated() = true;
  }
  ReadInst(read_inst.GetOp());
	
  cpu_stats.mut_stats.copies_exec++;
	
  write_head.SetInst(read_inst);
  write_head.FlagCopied() = true;  // Set the copied flag...
	
  // TriggerMutations(MUTATION_TRIGGER_WRITE, write_head);
	
  read_head.Advance();
  write_head.Advance();
  return true;
}

//17
bool cHardwareSMT::Inst_IfEqual()      // Execute next if bx == ?cx?
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_AX);
  const int stack_used2 = (stack_used+1) % nHardwareSMT::NUM_STACKS;
  if (Stack(stack_used).Top() != Stack(stack_used2).Top())  IP().Advance();
  return true;
}

//18
bool cHardwareSMT::Inst_IfNotEqual()     // Execute next if bx != ?cx?
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_AX);
  const int stack_used2 = (stack_used+1) % nHardwareSMT::NUM_STACKS;
  if (Stack(stack_used).Top() == Stack(stack_used2).Top())  IP().Advance();
  return true;
}

//19
bool cHardwareSMT::Inst_IfLess()       // Execute next if ?bx? < ?cx?
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_AX);
  const int stack_used2 = (stack_used+1) % nHardwareSMT::NUM_STACKS;
  if (Stack(stack_used).Top() >=  Stack(stack_used2).Top())  IP().Advance();
  return true;
}

//20
bool cHardwareSMT::Inst_IfGreater()       // Execute next if bx > ?cx?
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_AX);
  const int stack_used2 = (stack_used+1) % nHardwareSMT::NUM_STACKS;
  if (Stack(stack_used).Top() <= Stack(stack_used2).Top())  IP().Advance();
  return true;
}

//21
bool cHardwareSMT::Inst_HeadPush()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  Stack(nHardwareSMT::STACK_BX).Push(GetHead(head_used).GetPosition());
  //if (head_used == HEAD_IP) {
  //  GetHead(head_used).Set(GetHead(HEAD_FLOW));
  //  AdvanceIP() = false;
  //}
  return true;
}

//22
bool cHardwareSMT::Inst_HeadPop()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  GetHead(head_used).Set(Stack(nHardwareSMT::STACK_BX).Pop(), 
												 GetHead(head_used).GetMemSpace(), this);
  return true;
}

//23 
bool cHardwareSMT::Inst_HeadMove()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  if(head_used != HEAD_FLOW)
	{
		GetHead(head_used).Set(GetHead(HEAD_FLOW));
		if (head_used == HEAD_IP) AdvanceIP() = false;
	}
  else
	{
		m_threads[m_cur_thread].heads[HEAD_FLOW]++;
	}
  return true;
}

//24
bool cHardwareSMT::Inst_Search()
{
  ReadLabel();
  GetLabel().Rotate(2, nHardwareSMT::NUM_NOPS);
  cHeadMultiMem found_pos = FindLabel(0);
  if(found_pos.GetPosition()-IP().GetPosition()==0)
	{
		GetHead(HEAD_FLOW).Set(IP().GetPosition()+1, IP().GetMemSpace(), this);
		// pushing zero into STACK_AX on a missed search makes it difficult to create
		// a self-replicating organism.  -law
		//Stack(STACK_AX).Push(0);
		Stack(nHardwareSMT::STACK_BX).Push(0);
	}
  else
	{
		int search_size = found_pos.GetPosition() - IP().GetPosition() + GetLabel().GetSize() + 1;
		Stack(nHardwareSMT::STACK_BX).Push(search_size);
		Stack(nHardwareSMT::STACK_AX).Push(GetLabel().GetSize());
		GetHead(HEAD_FLOW).Set(found_pos);
	}  
  
  return true; 
}

//25
bool cHardwareSMT::Inst_PushNext() 
{
  int stack_used = FindModifiedStack(nHardwareSMT::STACK_AX);
  int successor = (stack_used+1) % nHardwareSMT::NUM_STACKS;
  Stack(successor).Push(Stack(stack_used).Pop());
  return true;
}

//26
bool cHardwareSMT::Inst_PushPrevious() 
{
  int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  int predecessor = (stack_used + nHardwareSMT::NUM_STACKS - 1) % nHardwareSMT::NUM_STACKS;
  Stack(predecessor).Push(Stack(stack_used).Pop());
  return true;
}

//27
bool cHardwareSMT::Inst_PushComplement() 
{
  int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  int complement = FindComplementStack(stack_used);
  Stack(complement).Push(Stack(stack_used).Pop());
  return true;
}

//28
bool cHardwareSMT::Inst_ValDelete()
{
  int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  Stack(stack_used).Pop();
  return true;
}

//29
bool cHardwareSMT::Inst_ValCopy()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  Stack(stack_used).Push(Stack(stack_used).Top());
  return true;
}

//30
bool cHardwareSMT::Inst_ForkThread()
{
  if (!ForkThread()) 
    Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
  else
    IP().Advance();
  return true;
}

//31
bool cHardwareSMT::Inst_IfLabel()
{
  ReadLabel();
  GetLabel().Rotate(2, nHardwareSMT::NUM_NOPS);
  if (GetLabel() != GetReadLabel())  IP().Advance();
  return true;
}

//32
bool cHardwareSMT::Inst_Increment()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  int value = Stack(stack_used).Pop();
  Stack(stack_used).Push(++value);
  return true;
}

//33
bool cHardwareSMT::Inst_Decrement()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  int value = Stack(stack_used).Pop();
  Stack(stack_used).Push(--value);
  return true;
}

//34
bool cHardwareSMT::Inst_Mod()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
  if (Stack(nHardwareSMT::STACK_CX).Top() != 0) {
    if(Stack(nHardwareSMT::STACK_CX).Top() == -1)
      Stack(stack_used).Push(0);
    else
      Stack(stack_used).Push(Stack(nHardwareSMT::STACK_BX).Top() % Stack(nHardwareSMT::STACK_CX).Top());
  } else {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
		return false;
  }
  return true;
}

//35
bool cHardwareSMT::Inst_KillThread()
{
  if (!KillThread()) Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  else AdvanceIP() = false;
  return true;
}

//36
bool cHardwareSMT::Inst_IO()
{
  const int stack_used = FindModifiedStack(nHardwareSMT::STACK_BX);
	
  // Do the "put" component
  const int value_out = Stack(stack_used).Top();
  organism->DoOutput(value_out);  // Check for tasks compleated.
	
  // Do the "get" component
  const int value_in = organism->GetNextInput();
  Stack(stack_used).Push(value_in);
  organism->DoInput(value_in);
  return true;
}

int cHardwareSMT::FindFirstEmpty()
{
  bool OK=true;
  const int current_mem_space = IP().GetMemSpace();
	
  for(int x = 1; x < m_mem_array.GetSize(); x++)
	{
		OK=true;
		
		int index = (current_mem_space + x) % m_mem_array.GetSize();
		
		for(int y=0; y<GetMemory(index).GetSize() && OK; y++)
		{
			if(GetMemory(index)[y].GetOp() >= nHardwareSMT::NUM_NOPS)
				OK=false; 
		}
		for(int y=0; y<GetNumThreads() && OK; y++)
		{
			for(int z=0; z<NUM_HEADS; z++)
	    {
	      if(m_threads[y].heads[z].GetMemSpace() == index)
					OK=false;
	    }
		}
		if(OK)
			return index;
	}
  return -1;
}

bool cHardwareSMT::isEmpty(int mem_space_used)
{
  for(int x=0; x<GetMemory(mem_space_used).GetSize(); x++)
	{
		if(GetMemory(mem_space_used)[x].GetOp() >= nHardwareSMT::NUM_NOPS)
			return false;
	}
  return true;
}

// The inject instruction can be used instead of a divide command, paired
// with an allocate.  Note that for an inject to work, one needs to have a
// broad range for sizes allowed to be allocated.
//
// This command will cut out from read-head to write-head.
// It will then look at the template that follows the command and inject it
// into the complement template found in a neighboring organism.

bool cHardwareSMT::Inst_Inject()
{
  double mut_multiplier = 1;
	
  return InjectParasite(mut_multiplier);
}



/*
 bool cHardwareSMT::Inst_InjectRand()
 {
	 // Rotate to a random facing and then run the normal inject instruction
	 const int num_neighbors = organism->GetNeighborhoodSize();
	 organism->Rotate(g_random.GetUInt(num_neighbors));
	 Inst_Inject();
	 return true;
 }
 
 */
