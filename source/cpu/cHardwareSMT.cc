/*
 *  cHardwareSMT.cc
 *  Avida
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#include "cHardwareSMT.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cInstLibBase.h"
#include "cInstSet.h"
#include "cHardwareTracer.h"
#include "cHardwareTracer_SMT.h"
#include "cMutation.h"
#include "cMutationLib.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cRandom.h"
#include "cTestCPU.h"
#include "cWorld.h"

#include "functions.h"
#include "nMutation.h"

#include <iomanip>

using namespace std;

tInstLib<cHardwareSMT::tMethod>* cHardwareSMT::s_inst_slib = cHardwareSMT::initInstLib();

tInstLib<cHardwareSMT::tMethod>* cHardwareSMT::initInstLib(void)
{
  struct cNOPEntry {
    cNOPEntry(const cString &name, int nop_mod):name(name), nop_mod(nop_mod){}
    cString name;
    int nop_mod;
  };
  static const cNOPEntry s_n_array[] = {
    cNOPEntry("Nop-A", STACK_AX),
    cNOPEntry("Nop-B", STACK_BX),
    cNOPEntry("Nop-C", STACK_CX),
    cNOPEntry("Nop-D", STACK_DX),
  };
	
  struct cInstEntry {
    cInstEntry(const cString &name, tMethod function):name(name), function(function){}
    cString name;
    tMethod function;
  };
  static const cInstEntry s_f_array[] = {
    cInstEntry("Nop-A", &cHardwareSMT::Inst_Nop), // 1
    cInstEntry("Nop-B", &cHardwareSMT::Inst_Nop), // 2
    cInstEntry("Nop-C", &cHardwareSMT::Inst_Nop), // 3
    cInstEntry("Nop-D", &cHardwareSMT::Inst_Nop), // 4
    cInstEntry("Nop-X", &cHardwareSMT::Inst_Nop), // 5
    cInstEntry("Val-Shift-R", &cHardwareSMT::Inst_ShiftR), // 6
    cInstEntry("Val-Shift-L", &cHardwareSMT::Inst_ShiftL), // 7
    cInstEntry("Val-Nand", &cHardwareSMT::Inst_Val_Nand), // 8
    cInstEntry("Val-Add", &cHardwareSMT::Inst_Val_Add), // 9
    cInstEntry("Val-Sub", &cHardwareSMT::Inst_Val_Sub), // 10
    cInstEntry("Val-Mult", &cHardwareSMT::Inst_Val_Mult), // 11
    cInstEntry("Val-Div", &cHardwareSMT::Inst_Val_Div), // 12
    cInstEntry("Val-Mod", &cHardwareSMT::Inst_Val_Mod), // 13
    cInstEntry("Val-Inc", &cHardwareSMT::Inst_Val_Inc), // 14
    cInstEntry("Val-Dec", &cHardwareSMT::Inst_Val_Dec), // 15
    cInstEntry("SetMemory", &cHardwareSMT::Inst_SetMemory), // 16
    cInstEntry("Divide", &cHardwareSMT::Inst_Divide), // 17
    cInstEntry("Inst-Read", &cHardwareSMT::Inst_HeadRead), // 18
    cInstEntry("Inst-Write", &cHardwareSMT::Inst_HeadWrite), // 19
    cInstEntry("If-Equal", &cHardwareSMT::Inst_IfEqual), // 20
    cInstEntry("If-Not-Equal", &cHardwareSMT::Inst_IfNotEqual), // 21
    cInstEntry("If-Less", &cHardwareSMT::Inst_IfLess), // 22
    cInstEntry("If-Greater", &cHardwareSMT::Inst_IfGreater), // 23
    cInstEntry("Head-Push", &cHardwareSMT::Inst_HeadPush), // 24
    cInstEntry("Head-Pop", &cHardwareSMT::Inst_HeadPop), // 25
    cInstEntry("Head-Move", &cHardwareSMT::Inst_HeadMove), // 26
    cInstEntry("Search", &cHardwareSMT::Inst_Search), // 27
    cInstEntry("Push-Next", &cHardwareSMT::Inst_PushNext), // 28
    cInstEntry("Push-Prev", &cHardwareSMT::Inst_PushPrevious), // 29
    cInstEntry("Push-Comp", &cHardwareSMT::Inst_PushComplement), // 30
    cInstEntry("Val-Delete", &cHardwareSMT::Inst_ValDelete), // 31
    cInstEntry("Val-Copy", &cHardwareSMT::Inst_ValCopy), // 32
    cInstEntry("IO", &cHardwareSMT::Inst_IO), // 33
    cInstEntry("Thread-Create", &cHardwareSMT::Inst_ThreadCreate), // 34
    cInstEntry("Thread-Cancel", &cHardwareSMT::Inst_ThreadCancel), // 35
    cInstEntry("Thread-Kill", &cHardwareSMT::Inst_ThreadKill), // 36
    cInstEntry("Inject", &cHardwareSMT::Inst_Inject), // 37
    cInstEntry("Apoptosis", &cHardwareSMT::Inst_Apoptosis), // 38
    cInstEntry("Net-Get", &cHardwareSMT::Inst_NetGet), // 39
    cInstEntry("Net-Send", &cHardwareSMT::Inst_NetSend), // 40
    cInstEntry("Net-Receive", &cHardwareSMT::Inst_NetReceive), // 41
    cInstEntry("Net-Last", &cHardwareSMT::Inst_NetLast), // 42
    cInstEntry("Rotate-Left", &cHardwareSMT::Inst_RotateLeft), // 43
    cInstEntry("Rotate-Right", &cHardwareSMT::Inst_RotateRight), // 44
    cInstEntry("Call-Flow", &cHardwareSMT::Inst_CallFlow), // 44
    cInstEntry("Call-Label", &cHardwareSMT::Inst_CallLabel), // 44
    cInstEntry("Return", &cHardwareSMT::Inst_Return), // 44
    
    cInstEntry("NULL", &cHardwareSMT::Inst_Nop) // Last Instruction Always NULL
  };
	
  const int n_size = sizeof(s_n_array)/sizeof(cNOPEntry);
	
  static cString n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size; i++){
    n_names[i] = s_n_array[i].name;
    nop_mods[i] = s_n_array[i].nop_mod;
  }
	
  const int f_size = sizeof(s_f_array)/sizeof(cInstEntry);
  static cString f_names[f_size];
  static tMethod functions[f_size];
  for (int i = 0; i < f_size; i++){
    f_names[i] = s_f_array[i].name;
    functions[i] = s_f_array[i].function;
  }
	
	const cInstruction error(255);
	const cInstruction def(0);
	
  tInstLib<cHardwareSMT::tMethod>* inst_lib =
    new tInstLib<cHardwareSMT::tMethod>(n_size, f_size, n_names, f_names, nop_mods, functions, error, def);
	
  return inst_lib;
}

cHardwareSMT::cHardwareSMT(cWorld* world, cOrganism* in_organism, cInstSet* in_m_inst_set)
: cHardwareBase(world, in_organism, in_m_inst_set), m_mem_array(1)
, m_mem_lbls(Pow(NUM_NOPS, MAX_MEMSPACE_LABEL) / MEM_LBLS_HASH_FACTOR)
, m_thread_lbls(Pow(NUM_NOPS, MAX_THREAD_LABEL) / THREAD_LBLS_HASH_FACTOR)
{
  m_functions = s_inst_slib->GetFunctions();
	
  m_mem_array[0] = in_organism->GetGenome();  // Initialize memory...
  m_mem_array[0].Resize(m_mem_array[0].GetSize() + 1);
  m_mem_array[0][m_mem_array[0].GetSize() - 1] = cInstruction();
  Reset();                            // Setup the rest of the hardware...
}

void cHardwareSMT::Reset()
{
  // Setup the memory...
  m_mem_array.Resize(1);
  m_mem_lbls.ClearAll();
  
  // We want to reset to have a single thread.
  m_threads.Resize(1);
	
  // Reset that single thread.
  m_threads[0].Reset(this, 0);
  m_cur_thread = 0;
		
  // Reset all stacks (local and global)
  for(int i = 0; i < NUM_STACKS; i++) {
		Stack(i).Clear();
	}
	
#if INSTRUCTION_COSTS
  // instruction cost arrays
  const int num_inst_cost = m_inst_set->GetSize();
  inst_cost.Resize(num_inst_cost);
  inst_ft_cost.Resize(num_inst_cost);
	
  for (int i = 0; i < num_inst_cost; i++) {
    inst_cost[i] = m_inst_set->GetCost(cInstruction(i));
    inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
  }
#endif	
  
  organism->NetReset();
}

void cHardwareSMT::cLocalThread::Reset(cHardwareBase* in_hardware, int mem_space)
{
  for (int i = 0; i < NUM_LOCAL_STACKS; i++) local_stacks[i].Clear();
  for (int i = 0; i < nHardware::NUM_HEADS; i++) heads[i].Reset(mem_space, in_hardware);
	
  cur_head = nHardware::HEAD_IP;
  read_label.Clear();
  next_label.Clear();
  running = true;
  owner = NULL;  
}



// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.
void cHardwareSMT::SingleProcess(cAvidaContext& ctx)
{
  // Mark this organism as running...
  organism->SetRunning(true);
	
  cPhenotype& phenotype = organism->GetPhenotype();
  phenotype.IncTimeUsed();
	
  const int num_inst_exec = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1) ? GetNumThreads() : 1;
  
  for (int i = 0; i < num_inst_exec; i++) {
    // Setup the hardware for the next instruction to be executed.
    ThreadNext();
    if (!ThreadIsRunning()) continue;
    
    AdvanceIP() = true;
    IP().Adjust();
		
#if BREAKPOINTS
    if (IP().FlagBreakpoint()) organism->DoBreakpoint();
#endif
    
    // Print the status of this CPU at each step...
    if (m_tracer)
    {
      cHardwareTracer_SMT* tracer = dynamic_cast<cHardwareTracer_SMT *>(m_tracer);
      if (tracer) tracer->TraceHardware_SMT(*this);
    }
    
    // Find the instruction to be executed
    const cInstruction& cur_inst = IP().GetInst();
		
    // Test if costs have been paid and it is okay to execute this now...
    const bool exec = SingleProcess_PayCosts(ctx, cur_inst);
		
    // Now execute the instruction...
    if (exec == true) {
      SingleProcess_ExecuteInst(ctx, cur_inst);
			
      // Some instruction (such as jump) may turn advance_ip off.  Ususally
      // we now want to move to the next instruction in the memory.
      if (AdvanceIP() == true) IP().Advance();
    }
  }
  
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
bool cHardwareSMT::SingleProcess_PayCosts(cAvidaContext& ctx, const cInstruction& cur_inst)
{
#if INSTRUCTION_COSTS
  assert(cur_inst.GetOp() < inst_cost.GetSize());
	
  // If first time cost hasn't been paid off...
  if ( inst_ft_cost[cur_inst.GetOp()] > 0 ) {
    inst_ft_cost[cur_inst.GetOp()]--;       // dec cost
    return false;
  }
	
  // Next, look at the per use cost
  if ( m_inst_set->GetCost(cur_inst) > 0 ) {
    if ( inst_cost[cur_inst.GetOp()] > 1 ){  // if isn't paid off (>1)
      inst_cost[cur_inst.GetOp()]--;         // dec cost
      return false;
    } else {                                 // else, reset cost array
      inst_cost[cur_inst.GetOp()] = m_inst_set->GetCost(cur_inst);
    }
  }
	
  // Prob of exec
  if ( m_inst_set->GetProbFail(cur_inst) > 0.0 ){
    return !( ctx.GetRandom().P(m_inst_set->GetProbFail(cur_inst)) );
  }
#endif
  return true;
}

// This method will handle the actual execution of an instruction
// within single process, once that function has been finalized.
bool cHardwareSMT::SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst) 
{
  // Copy Instruction locally to handle stochastic effects
  cInstruction actual_inst = cur_inst;
  
#ifdef EXECUTION_ERRORS
  // If there is an execution error, execute a random instruction.
  if (organism->TestExeErr()) actual_inst = m_inst_set->GetRandomInst(ctx);
#endif /* EXECUTION_ERRORS */
	
  // Get a pointer to the corrisponding method...
  int inst_idx = m_inst_set->GetLibFunctionIndex(actual_inst);
  
  // Mark the instruction as executed
  IP().SetFlagExecuted();
	
	
#if INSTRUCTION_COUNT
  // instruction execution count incremeneted
  organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
#endif
	
  // And execute it.
  const bool exec_success = (this->*(m_functions[inst_idx]))(ctx);
	
#if INSTRUCTION_COUNT
  // decremenet if the instruction was not executed successfully
  if (exec_success == false) {
    organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  }
#endif	
	
  return exec_success;
}


void cHardwareSMT::ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = organism->GetIsRunning();
  organism->SetRunning(true);
	
  // Print the status of this CPU at each step...
  if (m_tracer != NULL) {
    if (cHardwareTracer_SMT* tracer = dynamic_cast<cHardwareTracer_SMT *>(m_tracer)) {
      tracer->TraceHardware_SMTBonus(*this);
    }
  }
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  organism->SetRunning(prev_run_state);
}

bool cHardwareSMT::OK()
{
  for(int i = 0 ; i < m_mem_array.GetSize(); i++) {
    if (!m_mem_array[i].OK()) return false;
  }
	
  for (int i = 0; i < GetNumThreads(); i++) {
    for(int j=0; j < NUM_LOCAL_STACKS; j++)
			if (m_threads[i].local_stacks[j].OK() == false) return false;
    if (m_threads[i].next_label.OK() == false) return false;
  }
	
  return true;
}

void cHardwareSMT::PrintStatus(ostream& fp)
{
  fp << organism->GetPhenotype().GetTimeUsed() << " "
  << "THREAD: " << m_cur_thread << ", " << m_threads.GetSize() << "   "
	<< "IP:(" << IP().GetMemSpace() << ", " << IP().GetPosition() << ")    "
	
	<< "AX:" << Stack(STACK_AX).Top() << " "
	<< setbase(16) << "[0x" << Stack(STACK_AX).Top() << "]  " << setbase(10)
	
	<< "BX:" << Stack(STACK_BX).Top() << " "
	<< setbase(16) << "[0x" << Stack(STACK_BX).Top() << "]  " << setbase(10)
	
	<< "CX:" << Stack(STACK_CX).Top() << " "
	<< setbase(16) << "[0x" << Stack(STACK_CX).Top() << "]  " << setbase(10)
	
	<< "DX:" << Stack(STACK_DX).Top() << " "
	<< setbase(16) << "[0x" << Stack(STACK_DX).Top() << "]  " << setbase(10)
	
	<< endl;
	
  fp << "  R-Head:(" << GetHead(nHardware::HEAD_READ).GetMemSpace() << ", " 
		<< GetHead(nHardware::HEAD_READ).GetPosition() << ")  " 
		<< "W-Head:(" << GetHead(nHardware::HEAD_WRITE).GetMemSpace()  << ", "
		<< GetHead(nHardware::HEAD_WRITE).GetPosition() << ")  "
		<< "F-Head:(" << GetHead(nHardware::HEAD_FLOW).GetMemSpace()   << ",  "
		<< GetHead(nHardware::HEAD_FLOW).GetPosition() << ")  "
		<< "RL:" << GetReadLabel().AsString() << "   "
    << endl;
	
  for (int i = 0; i < m_mem_array.GetSize(); i++) {
    const cCPUMemory& mem = m_mem_array[i];
    fp << "  Mem " << i << " (" << mem.GetSize() << "): " << mem.AsString() << endl;
  }
  
  fp.flush();
}


int cHardwareSMT::FindMemorySpaceLabel(const cCodeLabel& label, int mem_space)
{
	if (label.GetSize() == 0) return 0;
  
  int hash_key = label.AsInt(NUM_NOPS);
  if (!m_mem_lbls.Find(hash_key, mem_space)) {
    mem_space = m_mem_array.GetSize();
    m_mem_array.Resize(mem_space + 1);
    m_mem_lbls.Add(hash_key, mem_space);
  }
  
  return mem_space;
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
  cHeadMultiMem& inst_ptr = IP();
	
  // Start up a search head at the position of the instruction pointer.
  cHeadMultiMem search_head(inst_ptr);
  cCodeLabel& search_label = GetLabel();
	
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
int cHardwareSMT::FindLabel_Forward(const cCodeLabel& search_label,
                                    const cGenome& search_genome, int pos)
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
		
    if (m_inst_set->IsNop(search_genome[pos])) {
      // Find the start and end of the label we're in the middle of.
			
      int start_pos = pos;
      int end_pos = pos + 1;
      while (start_pos > search_start &&
						 m_inst_set->IsNop( search_genome[start_pos - 1] )) {
				start_pos--;
      }
      while (end_pos < search_genome.GetSize() &&
						 m_inst_set->IsNop( search_genome[end_pos] )) {
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
							m_inst_set->GetNopMod( search_genome[offset + matches] )) {
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
		
    if (m_inst_set->IsNop( search_genome[pos] )) {
      // Find the start and end of the label we're in the middle of.
			
      int start_pos = pos;
      int end_pos = pos + 1;
      while (start_pos > 0 && m_inst_set->IsNop(search_genome[start_pos - 1])) {
				start_pos--;
      }
      while (end_pos < search_start &&
						 m_inst_set->IsNop(search_genome[end_pos])) {
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
							m_inst_set->GetNopMod(search_genome[offset + matches])) {
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
cHeadMultiMem cHardwareSMT::FindLabel(const cCodeLabel& in_label, int direction)
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
      if (!m_inst_set->IsNop(temp_head.GetInst()) ||
					in_label[i] != m_inst_set->GetNopMod(temp_head.GetInst())) {
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
// direction which the heads[nHardware::HEAD_IP] should progress through a creature.
cHeadMultiMem cHardwareSMT::FindFullLabel(const cCodeLabel & in_label)
{
  assert(in_label.GetSize() > 0); // Trying to find label of 0 size!
	
  cHeadMultiMem temp_head(this);
	
  while (temp_head.InMemory()) {
    // If we are not in a label, jump to the next checkpoint...
    if (m_inst_set->IsNop(temp_head.GetInst())) {
      temp_head.AbsJump(in_label.GetSize());
      continue;
    }
		
    // Otherwise, rewind to the begining of this label...
    while (!(temp_head.AtFront()) && m_inst_set->IsNop(temp_head.GetInst(-1)))
      temp_head.AbsJump(-1);
		
    // Calculate the size of the label being checked, and make sure they
    // are equal.		
    int checked_size = 0;
    while (m_inst_set->IsNop(temp_head.GetInst(checked_size))) {
      checked_size++;
    }
    if (checked_size != in_label.GetSize()) {
      temp_head.AbsJump(checked_size + 1);
      continue;
    }

    // ...and do the comparison...
    int j;
    bool label_match = true;
    for (j = 0; j < in_label.GetSize(); j++) {
      if (!m_inst_set->IsNop(temp_head.GetInst(j)) ||
					in_label[j] != m_inst_set->GetNopMod(temp_head.GetInst(j))) {
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
bool cHardwareSMT::InjectParasite(cAvidaContext& ctx, double mut_multiplier)
{
  const int end_pos = GetHead(nHardware::HEAD_WRITE).GetPosition();
  const int mem_space_used = GetHead(nHardware::HEAD_WRITE).GetMemSpace();
  
  // Make sure the creature will still be above the minimum size
  if (end_pos <= 0) {
    organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: no code to inject");
    return false; // (inject fails)
  }
  if (end_pos < MIN_INJECT_SIZE) {
    m_mem_array[mem_space_used] = cGenome("a"); 
    organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: new size too small");
    return false; // (inject fails)
  }  
  
  m_mem_array[mem_space_used].Resize(end_pos);
  cCPUMemory injected_code = m_mem_array[mem_space_used];
	
  Inject_DoMutations(ctx, mut_multiplier, injected_code);
	
  bool inject_signal = false;
  if (injected_code.GetSize() > 0) inject_signal = organism->InjectParasite(injected_code);
	
  // reset the memory space that was injected
  m_mem_array[mem_space_used] = cGenome("a"); 
	
  for (int x = 0; x < nHardware::NUM_HEADS; x++) GetHead(x).Reset(IP().GetMemSpace(), this);
  for (int x = 0; x < NUM_LOCAL_STACKS; x++) Stack(x).Clear();
  
  AdvanceIP() = false;
  
  return inject_signal;
}

//This is the code run by the TARGET of an injection.  This RECIEVES the infection.
bool cHardwareSMT::InjectHost(const cCodeLabel& in_label, const cGenome& inject_code)
{
  // Inject fails if the memory space is already in use or thread exists
  if (MemorySpaceExists(in_label) || FindThreadLabel(in_label) == -1) return false;

  // Otherwise create the memory space and copy in the genome
  int mem_space_used = FindMemorySpaceLabel(in_label, -1);
  assert(mem_space_used == -1);
  m_mem_array[mem_space_used] = inject_code;
  
  // Create a thread for this parasite
  if (!ThreadCreate(in_label, mem_space_used)) return false; // Partially failed injection, could not create thread

  return true;
}


void cHardwareSMT::ReadInst(const int in_inst)
{
  if (m_inst_set->IsNop( cInstruction(in_inst) )) {
    GetReadLabel().AddNop(in_inst);
  } else {
    GetReadLabel().Clear();
  }
}

// This function looks at the current position in the info of a creature,
// and sets the next_label to be the sequence of nops which follows.  The
// instruction pointer is left on the last line of the label found.

void cHardwareSMT::ReadLabel(int max_size)
{
  int count = 0;
  cHeadMultiMem& inst_ptr = IP();
	
  GetLabel().Clear();
	
  while (m_inst_set->IsNop(inst_ptr.GetNextInst()) && (count < max_size)) {
    count++;
    inst_ptr.Advance();
    GetLabel().AddNop(m_inst_set->GetNopMod(inst_ptr.GetInst()));
		
    // If this is the first line of the template, mark it executed.
    if (GetLabel().GetSize() <=	m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get()) {
      inst_ptr.SetFlagExecuted();
    }
  }
}


int cHardwareSMT::ThreadCreate(const cCodeLabel& label, int mem_space)
{
  int thread_id = m_threads.GetSize();
  
  // Check for thread cap, base thread label (i.e. no label)
  if (thread_id == m_world->GetConfig().MAX_CPU_THREADS.Get() || label.GetSize() == 0) return -1;	
  
  // Check for existing thread
  int hash_key = label.AsInt(NUM_NOPS);
  if (m_thread_lbls.Find(hash_key, thread_id)) {
    if (m_threads[thread_id].running) {
      return -1;  // Thread exists, and is running... call fails
    } else {
      m_threads[thread_id].Reset(this, mem_space);
      return thread_id;
    }
  }
  
  // Add new thread entry
  m_threads.Resize(thread_id + 1);
  m_thread_lbls.Add(hash_key, thread_id);
    
  // Setup this thread into the current selected memory space (Flow Head)
  m_threads[thread_id].Reset(this, mem_space);
	
  return thread_id;
}

bool cHardwareSMT::ThreadKill(const int thread_id)
{
  if (thread_id < 1 || thread_id >= m_threads.GetSize()) return false;

  // Increment thread, if killing current thread
  if (m_cur_thread == thread_id) ThreadNext();
  
  // Set this thread to not running
  m_threads[thread_id].running = false;
	
  return true;
}


int cHardwareSMT::TestParasite() const
{
  return IP().TestParasite();
}



////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int cHardwareSMT::FindModifiedStack(int default_stack)
{
  assert(default_stack < NUM_STACKS);  // Stack ID too high.
	
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_stack = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  }
  return default_stack;
}

inline int cHardwareSMT::FindModifiedNextStack(int default_stack)
{
  assert(default_stack < NUM_STACKS);  // Stack ID too high.
	
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_stack = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  } else {
    default_stack = FindNextStack(default_stack);
  }
  return default_stack;
}

inline int cHardwareSMT::FindModifiedPreviousStack(int default_stack)
{
  assert(default_stack < NUM_STACKS);  // Stack ID too high.
	
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_stack = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  } else {
    default_stack = FindPreviousStack(default_stack);
  }
  return default_stack;
}

inline int cHardwareSMT::FindModifiedComplementStack(int default_stack)
{
  assert(default_stack < NUM_STACKS);  // Stack ID too high.
	
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_stack = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  } else {
    default_stack = FindPreviousStack(default_stack);
  }
  return default_stack;
}

inline int cHardwareSMT::FindModifiedHead(int default_head)
{
  assert(default_head < nHardware::NUM_HEADS); // Head ID too high.
	
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();    
    int nop_head = m_inst_set->GetNopMod(IP().GetInst());
    if (nop_head < nHardware::NUM_HEADS) default_head = nop_head;
    IP().SetFlagExecuted();
  }
  return default_head;
}

inline int cHardwareSMT::FindNextStack(int default_stack)
{
  return (default_stack + 1) % NUM_STACKS;
}

inline int cHardwareSMT::FindPreviousStack(int default_stack)
{
  return (default_stack + NUM_STACKS - 1) % NUM_STACKS;
}

inline int cHardwareSMT::FindComplementStack(int base_stack)
{
  return (base_stack + 2) % NUM_STACKS;
}


int cHardwareSMT::GetCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  const cCPUMemory& memory = m_mem_array[m_cur_child];
  for (int i = 0; i < memory.GetSize(); i++) {
    if (memory.FlagCopied(i)) copied_size++;
	}
  return copied_size;
}

void cHardwareSMT::Inject_DoMutations(cAvidaContext& ctx, double mut_multiplier, cCPUMemory & injected_code)
{
  organism->GetPhenotype().SetDivType(mut_multiplier);
	
  // Divide Mutations
  if (organism->TestDivideMut(ctx)) {
    const unsigned int mut_line = ctx.GetRandom().GetUInt(injected_code.GetSize());
    injected_code[mut_line] = m_inst_set->GetRandomInst(ctx);
  }
	
  // Divide Insertions
  if (organism->TestDivideIns(ctx) && injected_code.GetSize() < MAX_CREATURE_SIZE){
    const unsigned int mut_line = ctx.GetRandom().GetUInt(injected_code.GetSize() + 1);
    injected_code.Insert(mut_line, m_inst_set->GetRandomInst(ctx));
  }
	
  // Divide Deletions
  if (organism->TestDivideDel(ctx) && injected_code.GetSize() > MIN_CREATURE_SIZE){
    const unsigned int mut_line = ctx.GetRandom().GetUInt(injected_code.GetSize());
    injected_code.Remove(mut_line);
  }
	
  // Divide Mutations (per site)
  if(organism->GetDivMutProb() > 0){
    int num_mut = ctx.GetRandom().GetRandBinomial(injected_code.GetSize(), 
																					 organism->GetDivMutProb() / mut_multiplier);
    // If we have lines to mutate...
    if( num_mut > 0 ){
      for (int i = 0; i < num_mut; i++) {
				int site = ctx.GetRandom().GetUInt(injected_code.GetSize());
				injected_code[site] = m_inst_set->GetRandomInst(ctx);
      }
    }
  }
	
	
  // Insert Mutations (per site)
  if(organism->GetInsMutProb() > 0){
    int num_mut = ctx.GetRandom().GetRandBinomial(injected_code.GetSize(),
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
				mut_sites[i] = ctx.GetRandom().GetUInt(injected_code.GetSize() + 1);
      }
      // Sort the list
      qsort( (void*)mut_sites, num_mut, sizeof(int), &IntCompareFunction );
      // Actually do the mutations (in reverse sort order)
      for(int i = num_mut-1; i >= 0; i--) {
				injected_code.Insert(mut_sites[i], m_inst_set->GetRandomInst(ctx));
      }
    }
  }
	
	
  // Delete Mutations (per site)
  if( organism->GetDelMutProb() > 0 ){
    int num_mut = ctx.GetRandom().GetRandBinomial(injected_code.GetSize(),
																					 organism->GetDelMutProb());
    // If would make creature too small, delete down to MIN_CREATURE_SIZE
    if (injected_code.GetSize() - num_mut < MIN_CREATURE_SIZE) {
      num_mut = injected_code.GetSize() - MIN_CREATURE_SIZE;
    }
		
    // If we have lines to delete...
    for (int i = 0; i < num_mut; i++) {
      int site = ctx.GetRandom().GetUInt(injected_code.GetSize());
      injected_code.Remove(site);
    }
  }
	
  // Mutations in the parent's genome
  if (organism->GetParentMutProb() > 0) {
    for (int i = 0; i < m_mem_array[0].GetSize(); i++) {
      if (organism->TestParentMut(ctx)) {
				m_mem_array[0][i] = m_inst_set->GetRandomInst(ctx);
      }
    }
  }
	
}


bool cHardwareSMT::Divide_Main(cAvidaContext& ctx, double mut_multiplier)
{
  const int mem_space_used = GetHead(nHardware::HEAD_WRITE).GetMemSpace();
  const int write_head_pos = GetHead(nHardware::HEAD_WRITE).GetPosition();
  
  // Make sure the memory space we're using exists
  if (m_mem_array.GetSize() <= mem_space_used) return false;
  	
  // Make sure this divide will produce a viable offspring.
  m_cur_child = mem_space_used; // save current child memory space for use by dependent functions (e.g. GetCopiedSize())
  if (!Divide_CheckViable(ctx, m_mem_array[0].GetSize(), write_head_pos)) return false;
  
  // Since the divide will now succeed, set up the information to be sent to the new organism
  m_mem_array[mem_space_used].Resize(write_head_pos);
  organism->ChildGenome() = m_mem_array[mem_space_used];
	
  // Handle Divide Mutations...
  Divide_DoMutations(ctx, mut_multiplier);
	
  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures(ctx);
	
#if INSTRUCTION_COSTS
  // reset first time instruction costs
  for (int i = 0; i < inst_ft_cost.GetSize(); i++) {
    inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
  }
#endif
	
  bool parent_alive = organism->ActivateDivide(ctx);
	
  //reset the memory of the memory space that has been divided off
  m_mem_array[mem_space_used] = cGenome("a"); 
	
  // 3 Division Methods:
  // 1) DIVIDE_METHOD_OFFSPRING - Create a child, leave parent state untouched.
  // 2) DIVIDE_METHOD_SPLIT - Create a child, completely reset state of parent.
  // 3) DIVIDE_METHOD_BIRTH - Create a child, reset state of parent's current thread.
  const int div_method = m_world->GetConfig().DIVIDE_METHOD.Get();
  if (parent_alive && !(div_method == DIVIDE_METHOD_OFFSPRING))
	{
		
		if (div_method == DIVIDE_METHOD_SPLIT)
		{
			//this will wipe out all parasites on a divide.
			Reset();
			
		}
		else if (div_method == DIVIDE_METHOD_BIRTH)
		{
			if((!organism->GetPhenotype().IsModified() && GetNumThreads() > 1) || GetNumThreads() > 2) {
	      ThreadKill(m_cur_thread);
	    } else {
        //this will reset the current thread's heads and stacks.  It will 
        //not touch any other threads or memory spaces (ie: parasites)
	      for(int x = 0; x < nHardware::NUM_HEADS; x++) {
					GetHead(x).Reset(0, this);
				}
	      for(int x = 0; x < NUM_LOCAL_STACKS; x++) {
					Stack(x).Clear();
				}
	    }
		}
		AdvanceIP() = false;
	}
	
  return true;
}


// --------  Begin Core Instruction Set --------


//6
bool cHardwareSMT::Inst_ShiftR(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int src = FindModifiedStack(dst);
#else
  const int src = dst;
#endif
  int value = Stack(src).Pop();
  value >>= 1;
  Stack(dst).Push(value);
  return true;
}

//7
bool cHardwareSMT::Inst_ShiftL(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int src = FindModifiedStack(dst);
#else
  const int src = dst;
#endif
  int value = Stack(src).Pop();
  value <<= 1;
  Stack(dst).Push(value);
  return true;
}

//8
bool cHardwareSMT::Inst_Val_Nand(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op1 = STACK_BX;
  const int op2 = STACK_CX;
#endif
  Stack(dst).Push(~(Stack(op1).Top() & Stack(op2).Top()));
  return true;
}

//9
bool cHardwareSMT::Inst_Val_Add(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op1 = STACK_BX;
  const int op2 = STACK_CX;
#endif
  Stack(dst).Push(Stack(op1).Top() + Stack(op2).Top());
  return true;
}

//10
bool cHardwareSMT::Inst_Val_Sub(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op1 = STACK_BX;
  const int op2 = STACK_CX;
#endif
  Stack(dst).Push(Stack(op1).Top() - Stack(op2).Top());
  return true;
}

//11
bool cHardwareSMT::Inst_Val_Mult(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op1 = STACK_BX;
  const int op2 = STACK_CX;
#endif
  Stack(dst).Push(Stack(op1).Top() * Stack(op2).Top());
  return true;
}

//12
bool cHardwareSMT::Inst_Val_Div(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op1 = STACK_BX;
  const int op2 = STACK_CX;
#endif
  if (Stack(op2).Top() != 0) {
    if (0-INT_MAX > Stack(op1).Top() && Stack(op2).Top() == -1)
      organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: Float exception");
    else
      Stack(dst).Push(Stack(op1).Top() / Stack(op2).Top());
  } else {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
    return false;
  }
  return true;
}

//13
bool cHardwareSMT::Inst_Val_Mod(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op1 = STACK_BX;
  const int op2 = STACK_CX;
#endif
  if (Stack(op2).Top() != 0) {
    if(Stack(op2).Top() == -1)
      Stack(dst).Push(0);
    else
      Stack(dst).Push(Stack(op1).Top() % Stack(op2).Top());
  } else {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
		return false;
  }
  return true;
}

//14
bool cHardwareSMT::Inst_Val_Inc(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int src = FindModifiedStack(dst);
#else
  const int src = dst;
#endif
  int value = Stack(src).Pop();
  Stack(dst).Push(++value);
  return true;
}

//15
bool cHardwareSMT::Inst_Val_Dec(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int src = FindModifiedStack(dst);
#else
  const int src = dst;
#endif
  int value = Stack(src).Pop();
  Stack(dst).Push(--value);
  return true;
}

//16
bool cHardwareSMT::Inst_SetMemory(cAvidaContext& ctx) 
{
  ReadLabel(MAX_MEMSPACE_LABEL);
  
  if (GetLabel().GetSize() == 0) {
    GetHead(nHardware::HEAD_FLOW).Set(0, 0);
  } else {
    int mem_space_used = FindMemorySpaceLabel(GetLabel(), -1);
    if (mem_space_used == -1) return false;
    GetHead(nHardware::HEAD_FLOW).Set(0, mem_space_used);
  }
  
  return true;
}

//17
bool cHardwareSMT::Inst_Divide(cAvidaContext& ctx)
{
  return Divide_Main(ctx);
}

//18
bool cHardwareSMT::Inst_HeadRead(cAvidaContext& ctx)
{
  const int head_id = FindModifiedHead(nHardware::HEAD_READ);
#if SMT_FULLY_ASSOCIATIVE
  const int dst = FindModifiedStack(STACK_AX);
#else
  const int dst = STACK_AX;
#endif
  
  GetHead(head_id).Adjust();
  sCPUStats & cpu_stats = organism->CPUStats();
	
  // Mutations only occur on the read, for the moment.
  int read_inst = 0;
  if (organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx).GetOp();
    cpu_stats.mut_stats.copy_mut_count++;  // @CAO, hope this is good!
  } else {
    read_inst = GetHead(head_id).GetInst().GetOp();
  }
  Stack(dst).Push(read_inst);
  ReadInst(read_inst);
	
  cpu_stats.mut_stats.copies_exec++;  // @CAO, this too..
  GetHead(head_id).Advance();
  return true;
}

//19
bool cHardwareSMT::Inst_HeadWrite(cAvidaContext& ctx)
{
  const int head_id = FindModifiedHead(nHardware::HEAD_WRITE);
#if SMT_FULLY_ASSOCIATIVE
  const int src = FindModifiedStack(STACK_AX);
#else
  const int src = STACK_AX;
#endif

  cHeadMultiMem & active_head = GetHead(head_id);
  int mem_space_used = active_head.GetMemSpace();
  
  if(active_head.GetPosition() >= m_mem_array[mem_space_used].GetSize() - 1)
	{
		m_mem_array[mem_space_used].Resize(m_mem_array[mem_space_used].GetSize() + 1);
		m_mem_array[mem_space_used].Copy(m_mem_array[mem_space_used].GetSize() - 1, m_mem_array[mem_space_used].GetSize() - 2);
	}
	
  active_head.Adjust();
	
  int value = Stack(src).Pop();
  if (value < 0 || value >= m_inst_set->GetSize()) value = NOPX;
	
  active_head.SetInst(cInstruction(value));
  active_head.SetFlagCopied();
	
  // Advance the head after write...
  active_head++;
  return true;
}

//20
bool cHardwareSMT::Inst_IfEqual(cAvidaContext& ctx)      // Execute next if bx == ?cx?
{
  const int op1 = FindModifiedStack(STACK_AX);
#if SMT_FULLY_ASSOCIATIVE
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op2 = FindNextStack(op1);
#endif
  if (Stack(op1).Top() != Stack(op2).Top())  IP().Advance();
  return true;
}

//21
bool cHardwareSMT::Inst_IfNotEqual(cAvidaContext& ctx)     // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedStack(STACK_AX);
#if SMT_FULLY_ASSOCIATIVE
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op2 = FindNextStack(op1);
#endif
  if (Stack(op1).Top() == Stack(op2).Top())  IP().Advance();
  return true;
}

//22
bool cHardwareSMT::Inst_IfLess(cAvidaContext& ctx)       // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedStack(STACK_AX);
#if SMT_FULLY_ASSOCIATIVE
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op2 = FindNextStack(op1);
#endif
  if (Stack(op1).Top() >=  Stack(op2).Top())  IP().Advance();
  return true;
}

//23
bool cHardwareSMT::Inst_IfGreater(cAvidaContext& ctx)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedStack(STACK_AX);
#if SMT_FULLY_ASSOCIATIVE
  const int op2 = FindModifiedNextStack(op1);
#else
  const int op2 = FindNextStack(op1);
#endif
  if (Stack(op1).Top() <= Stack(op2).Top())  IP().Advance();
  return true;
}

//24
bool cHardwareSMT::Inst_HeadPush(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
#if SMT_FULLY_ASSOCIATIVE
  const int dst = FindModifiedStack(STACK_BX);
#else
  const int dst = STACK_BX;
#endif
  Stack(dst).Push(GetHead(head_used).GetPosition());
  return true;
}

//25
bool cHardwareSMT::Inst_HeadPop(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
#if SMT_FULLY_ASSOCIATIVE
  const int src = FindModifiedStack(STACK_BX);
#else
  const int src = STACK_BX;
#endif
  GetHead(head_used).Set(Stack(src).Pop(), GetHead(head_used).GetMemSpace(), this);
  return true;
}

//26
bool cHardwareSMT::Inst_HeadMove(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  if(head_used != nHardware::HEAD_FLOW)
	{
		GetHead(head_used).Set(GetHead(nHardware::HEAD_FLOW));
		if (head_used == nHardware::HEAD_IP) AdvanceIP() = false;
	}
  else
	{
		m_threads[m_cur_thread].heads[nHardware::HEAD_FLOW]++;
	}
  return true;
}

//27
bool cHardwareSMT::Inst_Search(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(2, NUM_NOPS);
  cHeadMultiMem found_pos = FindLabel(0);
  if(found_pos.GetPosition() - IP().GetPosition() == 0)
	{
		GetHead(nHardware::HEAD_FLOW).Set(IP().GetPosition() + 1, IP().GetMemSpace(), this);
		// pushing zero into STACK_AX on a missed search makes it difficult to create
		// a self-replicating organism.  @law
		Stack(STACK_BX).Push(0);
	}
  else
	{
		int search_size = found_pos.GetPosition() - IP().GetPosition() + GetLabel().GetSize() + 1;
		Stack(STACK_BX).Push(search_size);
		Stack(STACK_AX).Push(GetLabel().GetSize());
		GetHead(nHardware::HEAD_FLOW).Set(found_pos);
	}  
  
  return true; 
}

//28
bool cHardwareSMT::Inst_PushNext(cAvidaContext& ctx) 
{
  const int src = FindModifiedStack(STACK_AX);
#if SMT_FULLY_ASSOCIATIVE
  const int dst = FindModifiedNextStack(src);
#else
  const int dst = FindNextStack(src);
#endif
  Stack(dst).Push(Stack(src).Pop());
  return true;
}

//29
bool cHardwareSMT::Inst_PushPrevious(cAvidaContext& ctx) 
{
  const int src = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int dst = FindModifiedPreviousStack(src);
#else
  const int dst = FindPreviousStack(src);
#endif
  Stack(dst).Push(Stack(src).Pop());
  return true;
}

//30
bool cHardwareSMT::Inst_PushComplement(cAvidaContext& ctx) 
{
  int src = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int dst = FindModifiedComplementStack(src);
#else
  const int dst = FindComplementStack(src);
#endif
  Stack(dst).Push(Stack(src).Pop());
  return true;
}

//31
bool cHardwareSMT::Inst_ValDelete(cAvidaContext& ctx)
{
  int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Pop();
  return true;
}

//32
bool cHardwareSMT::Inst_ValCopy(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int src = FindModifiedStack(dst);
#else
  const int src = dst;
#endif
  Stack(dst).Push(Stack(src).Top());
  return true;
}

//33
bool cHardwareSMT::Inst_IO(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int src = FindModifiedStack(dst);
#else
  const int src = dst;
#endif
	
  // Do the "put" component
  const int value_out = Stack(src).Top();
  organism->DoOutput(ctx, value_out);  // Check for tasks compleated.
	
  // Do the "get" component
  const int value_in = organism->GetNextInput();
  Stack(dst).Push(value_in);
  organism->DoInput(value_in);
  return true;
}


// --------  End Core Instruction Set --------


// --------  Additional Instructions --------


//34
bool cHardwareSMT::Inst_ThreadCreate(cAvidaContext& ctx)
{
  ReadLabel(MAX_THREAD_LABEL);
  bool success = ThreadCreate(GetLabel(), GetHead(nHardware::HEAD_FLOW).GetMemSpace());
  if (!success) organism->Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
  return success;
}

//35
bool cHardwareSMT::Inst_ThreadCancel(cAvidaContext& ctx)
{
  bool success = ThreadKill(m_cur_thread);
  if (!success) organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  return success;
}

//36
bool cHardwareSMT::Inst_ThreadKill(cAvidaContext& ctx)
{
  ReadLabel(MAX_THREAD_LABEL);
  bool success = ThreadKill(GetLabel());
  if (!success) organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  return success;
}



//37
//
// The inject instruction can be used instead of a divide command, paired
// with an allocate.  Note that for an inject to work, one needs to have a
// broad range for sizes allowed to be allocated.
//
// This command will cut out from read-head to write-head.
// It will then look at the template that follows the command and inject it
// into the complement template found in a neighboring organism.
bool cHardwareSMT::Inst_Inject(cAvidaContext& ctx)
{
  ReadLabel(MAX_MEMSPACE_LABEL);
  
  double mut_multiplier = 1.0;	
  return InjectParasite(ctx, mut_multiplier);
}

//38
bool cHardwareSMT::Inst_Apoptosis(cAvidaContext& ctx)
{
  organism->Die();
  
  return true;
}

//39
bool cHardwareSMT::Inst_NetGet(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
#if SMT_FULLY_ASSOCIATIVE
  const int seq_dst = FindModifiedNextStack(dst);
#else
  const int seq_dst = FindNextStack(dst);
#endif
  int val, seq;
  organism->NetGet(ctx, val, seq);
  Stack(dst).Push(val);
  Stack(seq_dst).Push(seq);
  
  return true;
}

//40
bool cHardwareSMT::Inst_NetSend(cAvidaContext& ctx)
{
  const int src = FindModifiedStack(STACK_BX);
  organism->NetSend(ctx, Stack(src).Pop());
  return true;
}

//41
bool cHardwareSMT::Inst_NetReceive(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  int val;
  bool success = organism->NetReceive(val);
  Stack(dst).Push(val);
  return success;
}

//42
bool cHardwareSMT::Inst_NetLast(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_CX);
  Stack(dst).Push(organism->NetLast());
  return true;
}

//43
bool cHardwareSMT::Inst_RotateLeft(cAvidaContext& ctx)
{
  const int num_neighbors = organism->GetNeighborhoodSize();
  
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  // Always rotate at least once.
  organism->Rotate(-1);
  
  return true;
}

//44
bool cHardwareSMT::Inst_RotateRight(cAvidaContext& ctx)
{
  const int num_neighbors = organism->GetNeighborhoodSize();
  
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  // Always rotate at least once.
  organism->Rotate(1);
  
  return true;
}

//45
bool cHardwareSMT::Inst_CallFlow(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_AX);

  const int location = IP().GetPosition() & 0xFFFF;
  const int mem_space = IP().GetMemSpace() << 8;
  const int ra  = location | mem_space;
  
  Stack(dst).Push(ra);
  
  cHeadMultiMem& flow = GetHead(nHardware::HEAD_FLOW);
  IP().Set(flow.GetPosition(), flow.GetMemSpace());
  
  return true;
}

//46
bool cHardwareSMT::Inst_CallLabel(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_AX);
  
  const unsigned int location = IP().GetPosition() & 0xFFFF;
  const unsigned int mem_space = IP().GetMemSpace() << 8;
  const unsigned int ra  = location | mem_space;
  
  Stack(dst).Push(ra);
  
  ReadLabel(MAX_MEMSPACE_LABEL);
  
  if (GetLabel().GetSize() != 0) {
    int mem_space_used = FindMemorySpaceLabel(GetLabel(), -1);
    if (mem_space_used > -1) {
      // Jump to beginning of memory space
      IP().Set(0, mem_space_used);
      return true;
    }
  }
  
  return false;
}

//47
bool cHardwareSMT::Inst_Return(cAvidaContext& ctx)
{
  const int src = FindModifiedStack(STACK_AX);
  const unsigned int ra = Stack(src).Pop();
  
  const int location = ra & 0xFFFF;
  const int mem_space = NormalizeMemSpace(ra >> 8);
  
  IP().Set(location, mem_space);
  
  return true;
}
