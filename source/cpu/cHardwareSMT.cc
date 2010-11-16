/*
 *  cHardwareSMT.cc
 *  Avida
 *
 *  Created by David on 6/4/05.
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

#include "cHardwareSMT.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cInstLib.h"
#include "cInstSet.h"
#include "cHardwareTracer.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cRandom.h"
#include "cTestCPU.h"
#include "cWorld.h"
#include "tArrayUtils.h"
#include "tInstLibEntry.h"

#include "AvidaTools.h"

#include <iomanip>

using namespace std;
using namespace AvidaTools;

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
	
  static const tInstLibEntry<tMethod> s_f_array[] = {
    tInstLibEntry<tMethod>("Nop-A", &cHardwareSMT::Inst_Nop, nInstFlag::NOP),
    tInstLibEntry<tMethod>("Nop-B", &cHardwareSMT::Inst_Nop, nInstFlag::NOP),
    tInstLibEntry<tMethod>("Nop-C", &cHardwareSMT::Inst_Nop, nInstFlag::NOP),
    tInstLibEntry<tMethod>("Nop-D", &cHardwareSMT::Inst_Nop, nInstFlag::NOP),
    tInstLibEntry<tMethod>("Alt", &cHardwareSMT::Inst_Alt),
    tInstLibEntry<tMethod>("Val-Shift", &cHardwareSMT::Inst_ValShift),
    tInstLibEntry<tMethod>("Val-Nand", &cHardwareSMT::Inst_ValNand),
    tInstLibEntry<tMethod>("Val-Add", &cHardwareSMT::Inst_ValAdd),
    tInstLibEntry<tMethod>("Val-Sub", &cHardwareSMT::Inst_ValSub),
    tInstLibEntry<tMethod>("Val-Negate", &cHardwareSMT::Inst_ValNegate),
    tInstLibEntry<tMethod>("Val-Mult", &cHardwareSMT::Inst_ValMult),
    tInstLibEntry<tMethod>("Val-Div", &cHardwareSMT::Inst_ValDiv),
    tInstLibEntry<tMethod>("Val-Mod", &cHardwareSMT::Inst_ValMod),
    tInstLibEntry<tMethod>("Val-Inc", &cHardwareSMT::Inst_ValInc),
    tInstLibEntry<tMethod>("Val-Dec", &cHardwareSMT::Inst_ValDec),
    tInstLibEntry<tMethod>("Val-Copy", &cHardwareSMT::Inst_ValCopy),
    tInstLibEntry<tMethod>("Val-Delete", &cHardwareSMT::Inst_ValDelete),
    tInstLibEntry<tMethod>("Stack-Delete", &cHardwareSMT::Inst_StackDelete),
    tInstLibEntry<tMethod>("Push-Next", &cHardwareSMT::Inst_PushNext),
    tInstLibEntry<tMethod>("Push-Prev", &cHardwareSMT::Inst_PushPrev),
    tInstLibEntry<tMethod>("Push-Comp", &cHardwareSMT::Inst_PushComp),
    tInstLibEntry<tMethod>("Mem-Set", &cHardwareSMT::Inst_MemSet),
    tInstLibEntry<tMethod>("Mem-Mark", &cHardwareSMT::Inst_MemMark),
    tInstLibEntry<tMethod>("Mem-Split", &cHardwareSMT::Inst_MemSplit),
    tInstLibEntry<tMethod>("Mem-Merge", &cHardwareSMT::Inst_MemMerge),
    tInstLibEntry<tMethod>("Divide", &cHardwareSMT::Inst_Divide),
    tInstLibEntry<tMethod>("Inject", &cHardwareSMT::Inst_Inject),
    tInstLibEntry<tMethod>("Inst-Read", &cHardwareSMT::Inst_InstRead),
    tInstLibEntry<tMethod>("Inst-Write", &cHardwareSMT::Inst_InstWrite),
    tInstLibEntry<tMethod>("Block-Read", &cHardwareSMT::Inst_BlockRead),
    tInstLibEntry<tMethod>("Block-Write", &cHardwareSMT::Inst_BlockWrite),
    tInstLibEntry<tMethod>("If-Equal", &cHardwareSMT::Inst_IfEqual),
    tInstLibEntry<tMethod>("If-Not-Equal", &cHardwareSMT::Inst_IfNotEqual),
    tInstLibEntry<tMethod>("If-Less", &cHardwareSMT::Inst_IfLess),
    tInstLibEntry<tMethod>("If-Greater", &cHardwareSMT::Inst_IfGreater),
    tInstLibEntry<tMethod>("Head-Push", &cHardwareSMT::Inst_HeadPush),
    tInstLibEntry<tMethod>("Head-Pop", &cHardwareSMT::Inst_HeadPop),
    tInstLibEntry<tMethod>("Head-Move", &cHardwareSMT::Inst_HeadMove),
    tInstLibEntry<tMethod>("Head-Set", &cHardwareSMT::Inst_HeadSet),
    tInstLibEntry<tMethod>("Call", &cHardwareSMT::Inst_Call),
    tInstLibEntry<tMethod>("Return", &cHardwareSMT::Inst_Return),
    tInstLibEntry<tMethod>("Search", &cHardwareSMT::Inst_Search),
    tInstLibEntry<tMethod>("Search-Mem", &cHardwareSMT::Inst_SearchMem),
    tInstLibEntry<tMethod>("IO", &cHardwareSMT::Inst_IO),
    tInstLibEntry<tMethod>("Thread-Set", &cHardwareSMT::Inst_ThreadSet),
    tInstLibEntry<tMethod>("Thread-Get", &cHardwareSMT::Inst_ThreadGet),
    
    tInstLibEntry<tMethod>("Apoptosis", &cHardwareSMT::Inst_Apoptosis),
    
    tInstLibEntry<tMethod>("NULL", &cHardwareSMT::Inst_Nop) // Last Instruction Always NULL
  };
	
  const int n_size = sizeof(s_n_array)/sizeof(cNOPEntry);
	
  static cString n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size; i++){
    n_names[i] = s_n_array[i].name;
    nop_mods[i] = s_n_array[i].nop_mod;
  }
	
  const int f_size = sizeof(s_f_array)/sizeof(tInstLibEntry<tMethod>);
  static tMethod functions[f_size];
  for (int i = 0; i < f_size; i++) functions[i] = s_f_array[i].GetFunction();
	
	const int def = 0;
  const int null_inst = f_size - 1;
  
  return new tInstLib<tMethod>(f_size, s_f_array, n_names, nop_mods, functions, def, null_inst);
}

cHardwareSMT::cHardwareSMT(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set)
: cHardwareBase(world, in_organism, in_inst_set), m_mem_array(1), m_mem_marks(1)
, m_mem_lbls(Pow(NUM_NOPS, MAX_MEMSPACE_LABEL) / MEM_LBLS_HASH_FACTOR)
, m_thread_lbls(Pow(NUM_NOPS, MAX_THREAD_LABEL) / THREAD_LBLS_HASH_FACTOR)
{
  m_functions = s_inst_slib->GetFunctions();
	
  m_mem_array[0] = in_organism->GetGenome().GetSequence();  // Initialize memory...
  m_mem_array[0].Resize(m_mem_array[0].GetSize() + 1);
  m_mem_array[0][m_mem_array[0].GetSize() - 1] = cInstruction();
  Reset(ctx);                            // Setup the rest of the hardware...
}

void cHardwareSMT::internalReset()
{
  // Setup the memory...
  m_mem_array.Resize(1);
  m_mem_marks.Resize(1);
  m_mem_marks[0] = false;
  m_mem_lbls.ClearAll();
  
  // We want to reset to have a single thread.
  m_threads.Resize(1);
  m_thread_lbls.ClearAll();
	
  // Reset that single thread.
  m_threads[0].Reset(this, 0);
  m_cur_thread = 0;
		
  // Reset all stacks (local and global)
  for(int i = 0; i < NUM_STACKS; i++) {
		Stack(i).Clear();
	}
  
  m_organism->ClearParasites();
}

void cHardwareSMT::internalResetOnFailedDivide()
{
	internalReset();
}

void cHardwareSMT::cLocalThread::Reset(cHardwareBase* in_hardware, int mem_space)
{
  for (int i = 0; i < NUM_LOCAL_STACKS; i++) local_stacks[i].Clear();
  for (int i = 0; i < NUM_EXTENDED_HEADS; i++) heads[i].Reset(in_hardware, mem_space);
	
  cur_head = nHardware::HEAD_IP;
  read_label.Clear();
  next_label.Clear();
  running = true;
  owner = NULL;  
}

cBioUnit* cHardwareSMT::ThreadGetOwner()
{
  return (m_threads[m_cur_thread].owner) ? m_threads[m_cur_thread].owner : m_organism;
}



// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.
bool cHardwareSMT::SingleProcess(cAvidaContext& ctx, bool speculative)
{
  if (speculative) return false;

  // Mark this organism as running...
  m_organism->SetRunning(true);
	
  cPhenotype& phenotype = m_organism->GetPhenotype();
  phenotype.IncTimeUsed();
	
  const int num_inst_exec = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1) ? m_threads.GetSize() : 1;
  
  for (int i = 0; i < num_inst_exec; i++) {
    // Setup the hardware for the next instruction to be executed.
    m_cur_thread++;
    if (m_cur_thread >= m_threads.GetSize()) m_cur_thread = 0;

    if (!ThreadIsRunning()) continue;
    
    AdvanceIP() = true;
    IP().Adjust();
		
#if BREAKPOINTS
    if (IP().FlagBreakpoint()) m_organism->DoBreakpoint();
#endif
    
    // Print the status of this CPU at each step...
    if (m_tracer) m_tracer->TraceHardware(*this);
    
    // Find the instruction to be executed
    const cInstruction& cur_inst = IP().GetInst();
		
    // Test if costs have been paid and it is okay to execute this now...
    bool exec = SingleProcess_PayPreCosts(ctx, cur_inst);
		
    // Now execute the instruction...
    if (exec == true) {
      
      // Prob of exec (moved from SingleProcess_PayCosts so that we advance IP after a fail)
      if ( m_inst_set->GetProbFail(cur_inst) > 0.0 ) {
        exec = !( ctx.GetRandom().P(m_inst_set->GetProbFail(cur_inst)) );
      }
      
      if (exec == true) if (SingleProcess_ExecuteInst(ctx, cur_inst)) SingleProcess_PayPostCosts(ctx, cur_inst);
      			
      // Some instruction (such as jump) may turn advance_ip off.  Ususally
      // we now want to move to the next instruction in the memory.
      if (AdvanceIP() == true) IP().Advance();
    }
  }
  
  // Kill creatures who have reached their max num of instructions executed
  const int max_executed = m_organism->GetMaxExecuted();
  if ((max_executed > 0 && phenotype.GetTimeUsed() >= max_executed)
      || phenotype.GetToDie()) {
    m_organism->Die();
  }
  
  m_organism->SetRunning(false);
  CheckImplicitRepro(ctx);
  
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
  if (m_organism->TestExeErr()) actual_inst = m_inst_set->GetRandomInst(ctx);
#endif /* EXECUTION_ERRORS */
	
  // Get a pointer to the corrisponding method...
  int inst_idx = m_inst_set->GetLibFunctionIndex(actual_inst);
  
  // Mark the instruction as executed
  IP().SetFlagExecuted();
	
	
#if INSTRUCTION_COUNT
  // instruction execution count incremeneted
  m_organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
#endif
	
  // And execute it.
  const bool exec_success = (this->*(m_functions[inst_idx]))(ctx);
	
#if INSTRUCTION_COUNT
  // decremenet if the instruction was not executed successfully
  if (exec_success == false) {
    m_organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  }
#endif	
	
  return exec_success;
}


void cHardwareSMT::ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = m_organism->IsRunning();
  m_organism->SetRunning(true);
	
  // Print the status of this CPU at each step...
  if (m_tracer != NULL) m_tracer->TraceHardware(*this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  m_organism->SetRunning(prev_run_state);
}

bool cHardwareSMT::OK()
{	
  for (int i = 0; i < m_threads.GetSize(); i++) {
    for(int j=0; j < NUM_LOCAL_STACKS; j++)
			if (m_threads[i].local_stacks[j].OK() == false) return false;
    if (m_threads[i].next_label.OK() == false) return false;
  }
	
  return true;
}

void cHardwareSMT::PrintStatus(ostream& fp)
{
  fp << m_organism->GetPhenotype().GetTimeUsed() << " "
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
    fp << "  Mem " << i << " (" << mem.GetSize() << ")[" << (m_mem_marks[i] ? '*':' ') << "]: " << mem.AsString() << endl;
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
    m_mem_marks.Resize(mem_space + 1);
    m_mem_marks[mem_space] = false;
    m_mem_lbls.Set(hash_key, mem_space);
  }
  
  return mem_space;
}


// Search in 'direction' (+ or - 1) from the instruction pointer for the
// compliment of the label in 'next_label' and return a pointer to the
// results.  If direction is 0, search from the beginning of the genome.
cHeadCPU cHardwareSMT::FindLabel(int direction)
{
  cHeadCPU& inst_ptr = IP();
	
  // Start up a search head at the position of the instruction pointer.
  cHeadCPU search_head(inst_ptr);
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
int cHardwareSMT::FindLabel_Forward(const cCodeLabel& search_label, const cSequence& search_genome, int pos)
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
int cHardwareSMT::FindLabel_Backward(const cCodeLabel& search_label, const cSequence& search_genome, int pos)
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
cHeadCPU cHardwareSMT::FindLabel(const cCodeLabel& in_label, int direction)
{
  assert (in_label.GetSize() > 0);
	
  // IDEALY:
  // Keep making jumps (in the proper direction) equal to the label
  // length.  If we are inside of a label, check its size, and see if
  // any of the sub-labels match properly.
  // FOR NOW:
  // Get something which works, no matter how inefficient!!!
	
  cHeadCPU temp_head(this);
	
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

// This is the code run by the INFECTED organism.  Its function is to SPREAD infection.
bool cHardwareSMT::InjectParasite(cAvidaContext& ctx, double mut_multiplier)
{
  const int end_pos = GetHead(nHardware::HEAD_WRITE).GetPosition();
  const int mem_space_used = GetHead(nHardware::HEAD_WRITE).GetMemSpace();
  
  // Make sure the creature will still be above the minimum size
  if (end_pos <= 0) {
    m_organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: no code to inject");
    return false; // (inject fails)
  }
  if (end_pos < MIN_INJECT_SIZE) {
    m_mem_array[mem_space_used] = cSequence("a"); 
    m_organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: new size too small");
    return false; // (inject fails)
  }  
  
  m_mem_array[mem_space_used].Resize(end_pos);
  cCPUMemory injected_code = m_mem_array[mem_space_used];
	
  Inject_DoMutations(ctx, mut_multiplier, injected_code);
	
  bool inject_signal = false;
  if (injected_code.GetSize() > 0) {
    cBioUnit* parent = (m_threads[m_cur_thread].owner) ? m_threads[m_cur_thread].owner : m_organism;
    inject_signal = m_organism->InjectParasite(parent, GetLabel().AsString(), injected_code);
  }
	
  // reset the memory space that was injected
  m_mem_array[mem_space_used] = cSequence("a"); 
	
  for (int x = 0; x < NUM_EXTENDED_HEADS; x++) GetHead(x).Reset(this, IP().GetMemSpace());
  for (int x = 0; x < NUM_LOCAL_STACKS; x++) Stack(x).Clear();
  
  AdvanceIP() = false;
  
  return inject_signal;
}

bool cHardwareSMT::ParasiteInfectHost(cBioUnit* bu)
{
  assert(bu->GetGenome().GetHardwareType() == GetType() && bu->GetGenome().GetInstSet() == m_inst_set->GetInstSetName());
  
  cCodeLabel label;
  label.ReadString(bu->GetUnitSourceArgs());
  
  // Inject fails if the memory space is already in use
  if (label.GetSize() == 0 || MemorySpaceExists(label)) return false;
  
  int thread_id = m_threads.GetSize();
  
  // Check for existing thread
  int hash_key = label.AsInt(NUM_NOPS);
  if (m_thread_lbls.Find(hash_key, thread_id)) {
    if (m_threads[thread_id].running) return false;  // Thread exists, and is running... call fails
  } else {
    // Check for thread cap
    if (thread_id == m_world->GetConfig().MAX_CPU_THREADS.Get()) return false;
    
    // Add new thread entry
    m_threads.Resize(thread_id + 1);
    m_thread_lbls.Set(hash_key, thread_id);
  }
  
  // Create the memory space and copy in the parasite
  int mem_space = FindMemorySpaceLabel(label, -1);
  assert(mem_space != -1);
  m_mem_array[mem_space] = bu->GetGenome().GetSequence();
  
  // Setup the thread
  m_threads[thread_id].Reset(this, mem_space);
  m_threads[thread_id].owner = bu;
  
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
  cHeadCPU& inst_ptr = IP();
	
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
  if (thread_id == m_world->GetConfig().MAX_CPU_THREADS.Get() || label.GetSize() == 0) return 0;	
  
  // Check for existing thread
  int hash_key = label.AsInt(NUM_NOPS);
  if (m_thread_lbls.Find(hash_key, thread_id)) {
    if (m_threads[thread_id].running) {
      return 0;  // Thread exists, and is running... call fails
    } else {
      m_threads[thread_id].Reset(this, mem_space);
      return (thread_id + 1);
    }
  }
  
  // Add new thread entry
  m_threads.Resize(thread_id + 1);
  m_thread_lbls.Set(hash_key, thread_id);
    
  // Setup this thread into the current selected memory space (Flow Head)
  m_threads[thread_id].Reset(this, mem_space);
  m_threads[thread_id].owner = m_threads[m_cur_thread].owner;
	
  return (thread_id + 1);
}

bool cHardwareSMT::ThreadKill(int thread_id)
{
  if (thread_id < 1 || thread_id > m_threads.GetSize()) return false;

  // Adjust thread id to thread array index
  thread_id--;
  
  // Increment thread, if killing current thread
  if (m_cur_thread == thread_id) ThreadNext();
  
  // Set this thread to not running
  m_threads[thread_id].running = false;
	
  return true;
}



// --------  Instruction Helpers  --------

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
  assert(default_head < NUM_EXTENDED_HEADS); // Head ID too high.
	
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();    
    int nop_head = m_inst_set->GetNopMod(IP().GetInst());
    if (nop_head < NUM_EXTENDED_HEADS) default_head = nop_head;
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


int cHardwareSMT::calcCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  const cCPUMemory& memory = m_mem_array[m_cur_child];
  for (int i = 0; i < memory.GetSize(); i++) {
    if (memory.FlagCopied(i)) copied_size++;
	}
  return copied_size;
}

void cHardwareSMT::Inject_DoMutations(cAvidaContext& ctx, double mut_multiplier, cCPUMemory& injected_code)
{
  m_organism->GetPhenotype().SetDivType(mut_multiplier);
	
  // Divide Mutations (per site)
  if(m_organism->GetDivMutProb() > 0){
    int num_mut = ctx.GetRandom().GetRandBinomial(injected_code.GetSize(), 
																					 m_organism->GetInjectMutProb() / mut_multiplier);
    // If we have lines to mutate...
    if( num_mut > 0 ){
      for (int i = 0; i < num_mut; i++) {
				int site = ctx.GetRandom().GetUInt(injected_code.GetSize());
				injected_code[site] = m_inst_set->GetRandomInst(ctx);
      }
    }
  }
	
	
  // Insert Mutations (per site)
  if(m_organism->GetDivInsProb() > 0){
    int num_mut = ctx.GetRandom().GetRandBinomial(injected_code.GetSize(),
																					 m_organism->GetInjectInsProb());
    // If would make creature to big, insert up to MAX_GENOME_LENGTH
    if( num_mut + injected_code.GetSize() > MAX_GENOME_LENGTH ){
      num_mut = MAX_GENOME_LENGTH - injected_code.GetSize();
    }
    // If we have lines to insert...
    if( num_mut > 0 ){
      // Build a list of the sites where mutations occured
      tArray<int> mut_sites(num_mut);
      for (int i = 0; i < num_mut; i++) mut_sites[i] = ctx.GetRandom().GetUInt(injected_code.GetSize() + 1);
      tArrayUtils::QSort(mut_sites);
      
      // Actually do the mutations (in reverse sort order)
      for(int i = num_mut-1; i >= 0; i--) {
				injected_code.Insert(mut_sites[i], m_inst_set->GetRandomInst(ctx));
      }
    }
  }
	
	
  // Delete Mutations (per site)
  if( m_organism->GetDivDelProb() > 0 ){
    int num_mut = ctx.GetRandom().GetRandBinomial(injected_code.GetSize(),
																					 m_organism->GetInjectDelProb());
    // If would make creature too small, delete down to MIN_GENOME_LENGTH
    if (injected_code.GetSize() - num_mut < MIN_GENOME_LENGTH) {
      num_mut = injected_code.GetSize() - MIN_GENOME_LENGTH;
    }
		
    // If we have lines to delete...
    for (int i = 0; i < num_mut; i++) {
      int site = ctx.GetRandom().GetUInt(injected_code.GetSize());
      injected_code.Remove(site);
    }
  }
	
  // Mutations in the parent's genome
  if (m_organism->GetParentMutProb() > 0) {
    for (int i = 0; i < m_mem_array[0].GetSize(); i++) {
      if (m_organism->TestParentMut(ctx)) {
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
  m_cur_child = mem_space_used; // save current child memory space for use by dependent functions (e.g. calcCopiedSize())
  if (!Divide_CheckViable(ctx, m_mem_array[0].GetSize(), write_head_pos)) return false;
  
  // Since the divide will now succeed, set up the information to be sent to the new organism
  m_mem_array[mem_space_used].Resize(write_head_pos);
  m_organism->OffspringGenome().SetSequence(m_mem_array[mem_space_used]);
  m_organism->OffspringGenome().SetHardwareType(GetType());
  m_organism->OffspringGenome().SetInstSet(m_inst_set->GetInstSetName());
	
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
	
  bool parent_alive = m_organism->ActivateDivide(ctx);
	
  //reset the memory of the memory space that has been divided off
  m_mem_array[mem_space_used] = cSequence("a"); 
	
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
			Reset(ctx);
			
		}
		else if (div_method == DIVIDE_METHOD_BIRTH)
		{
			if((!m_organism->GetPhenotype().IsModified() && m_threads.GetSize() > 1) || m_threads.GetSize() > 2) {
	      ThreadKill(m_cur_thread + 1);
	    } else {
        //this will reset the current thread's heads and stacks.  It will 
        //not touch any other threads or memory spaces (ie: parasites)
	      for(int x = 0; x < NUM_EXTENDED_HEADS; x++) {
					GetHead(x).Reset(this, 0);
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

bool cHardwareSMT::Inst_Alt(cAvidaContext& ctx)
{
  // @DMB - todo
  return true;
}

bool cHardwareSMT::Inst_ValShift(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(dst);
  const int op2 = FindModifiedNextStack(op1);
  int value = Stack(op1).Top();
  int shift = Stack(op2).Top();
  
  const int bits = sizeof(int) * 8;
  if (shift >= 0 && shift < bits) {           // Positive shift == right shift
    value >>= shift;
  } else if (shift < 0 && (-shift) < bits) {  // Negative shift == left shift
    value <<= -shift;    
  } else {                                    // if shift > num_bits then flush to 0
    value = 0;
  }
  Stack(dst).Push(value);
  return true;
}

bool cHardwareSMT::Inst_ValNand(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  Stack(dst).Push(~(Stack(op1).Top() & Stack(op2).Top()));
  return true;
}

bool cHardwareSMT::Inst_ValAdd(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  Stack(dst).Push(Stack(op1).Top() + Stack(op2).Top());
  return true;
}

bool cHardwareSMT::Inst_ValNegate(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  Stack(dst).Push(-Stack(op1).Top());
  return true;
}

bool cHardwareSMT::Inst_ValSub(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  Stack(dst).Push(Stack(op1).Top() - Stack(op2).Top());
  return true;
}

bool cHardwareSMT::Inst_ValMult(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  Stack(dst).Push(Stack(op1).Top() * Stack(op2).Top());
  return true;
}

bool cHardwareSMT::Inst_ValDiv(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op2).Top() != 0) {
    if (-INT_MAX > Stack(op1).Top() && Stack(op2).Top() == -1) {
      m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: overflow exception");
      return false;
    } else {
      Stack(dst).Push(Stack(op1).Top() / Stack(op2).Top());
    }
  } else {
    m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
    return false;
  }
  return true;
}

bool cHardwareSMT::Inst_ValMod(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op2).Top() != 0) {
    Stack(dst).Push(Stack(op1).Top() % Stack(op2).Top());
  } else {
    m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
		return false;
  }
  return true;
}

bool cHardwareSMT::Inst_ValInc(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
  int value = Stack(src).Top();
  Stack(dst).Push(++value);
  return true;
}

bool cHardwareSMT::Inst_ValDec(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
  int value = Stack(src).Top();
  Stack(dst).Push(--value);
  return true;
}

bool cHardwareSMT::Inst_ValCopy(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
  Stack(dst).Push(Stack(src).Top());
  return true;
}

bool cHardwareSMT::Inst_ValDelete(cAvidaContext& ctx)
{
  int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Pop();
  return true;
}

bool cHardwareSMT::Inst_StackDelete(cAvidaContext& ctx)
{
  int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Clear();
  return true;
}


bool cHardwareSMT::Inst_PushNext(cAvidaContext& ctx) 
{
  const int src = FindModifiedStack(STACK_AX);
  const int dst = FindModifiedNextStack(src);
  Stack(dst).Push(Stack(src).Pop());
  return true;
}

bool cHardwareSMT::Inst_PushPrev(cAvidaContext& ctx) 
{
  const int src = FindModifiedStack(STACK_BX);
  const int dst = FindModifiedPreviousStack(src);
  Stack(dst).Push(Stack(src).Pop());
  return true;
}

bool cHardwareSMT::Inst_PushComp(cAvidaContext& ctx) 
{
  int src = FindModifiedStack(STACK_BX);
  const int dst = FindModifiedComplementStack(src);
  Stack(dst).Push(Stack(src).Pop());
  return true;
}

bool cHardwareSMT::Inst_MemSet(cAvidaContext& ctx) 
{
  ReadLabel(MAX_MEMSPACE_LABEL);
  
  int mem_space_used = FindMemorySpaceLabel(GetLabel(), -1);
  if (mem_space_used == -1) return false;
  GetHead(nHardware::HEAD_FLOW).Set(0, mem_space_used);
  
  return true;
}

bool cHardwareSMT::Inst_MemMark(cAvidaContext& ctx) 
{
  ReadLabel(MAX_MEMSPACE_LABEL);
  
  int mem_space_used = FindMemorySpaceLabel(GetLabel(), -1);
  if (mem_space_used == -1) return false;
  m_mem_marks[mem_space_used] = !m_mem_marks[mem_space_used];
  
  return true;
}

bool cHardwareSMT::Inst_MemSplit(cAvidaContext& ctx) 
{
  // @DMB - todo
  return true;
}

bool cHardwareSMT::Inst_MemMerge(cAvidaContext& ctx) 
{
  // @DMB - todo
  return true;
}

bool cHardwareSMT::Inst_Divide(cAvidaContext& ctx)
{
  return Divide_Main(ctx);
}

bool cHardwareSMT::Inst_Inject(cAvidaContext& ctx)
{
  ReadLabel(MAX_MEMSPACE_LABEL);
  
  double mut_multiplier = 1.0;	
  return InjectParasite(ctx, mut_multiplier);
}

bool cHardwareSMT::Inst_InstRead(cAvidaContext& ctx)
{
  const int head_id = FindModifiedHead(nHardware::HEAD_READ);
  const int dst = FindModifiedStack(STACK_AX);
  
  GetHead(head_id).Adjust();
	
  // Mutations only occur on the read, for the moment.
  int read_inst = 0;
  if (m_organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx).GetOp();
  } else {
    read_inst = GetHead(head_id).GetInst().GetOp();
  }
  Stack(dst).Push(read_inst);
  ReadInst(read_inst);
	
  GetHead(head_id).Advance();
  return true;
}

bool cHardwareSMT::Inst_InstWrite(cAvidaContext& ctx)
{
  const int head_id = FindModifiedHead(nHardware::HEAD_WRITE);
  const int src = FindModifiedStack(STACK_AX);

  cHeadCPU & active_head = GetHead(head_id);
  int mem_space_used = active_head.GetMemSpace();
  
  if(active_head.GetPosition() >= m_mem_array[mem_space_used].GetSize() - 1)
	{
		m_mem_array[mem_space_used].Resize(m_mem_array[mem_space_used].GetSize() + 1);
		m_mem_array[mem_space_used].Copy(m_mem_array[mem_space_used].GetSize() - 1, m_mem_array[mem_space_used].GetSize() - 2);
	}
	
  active_head.Adjust();
	
  int value = Stack(src).Pop();
  if (value < 0 || value >= m_inst_set->GetSize()) value = 0;
	
  active_head.SetInst(cInstruction(value));
  active_head.SetFlagCopied();
	
  // Advance the head after write...
  active_head++;
  return true;
}

bool cHardwareSMT::Inst_BlockRead(cAvidaContext& ctx)
{
  // @DMB - todo
  return true;
}

bool cHardwareSMT::Inst_BlockWrite(cAvidaContext& ctx)
{
  // @DMB - todo
  return true;
}

bool cHardwareSMT::Inst_IfEqual(cAvidaContext& ctx) // Execute next if op1 == op2
{
  const int op1 = FindModifiedStack(STACK_AX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op1).Top() != Stack(op2).Top()) IP().Advance();
  return true;
}

bool cHardwareSMT::Inst_IfNotEqual(cAvidaContext& ctx) // Execute next if op1 != op2
{
  const int op1 = FindModifiedStack(STACK_AX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op1).Top() == Stack(op2).Top()) IP().Advance();
  return true;
}

bool cHardwareSMT::Inst_IfLess(cAvidaContext& ctx) // Execute next if op1 < op2
{
  const int op1 = FindModifiedStack(STACK_AX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op1).Top() >= Stack(op2).Top()) IP().Advance();
  return true;
}

bool cHardwareSMT::Inst_IfGreater(cAvidaContext& ctx) // Execute next if op1 > op2
{
  const int op1 = FindModifiedStack(STACK_AX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op1).Top() <= Stack(op2).Top()) IP().Advance();
  return true;
}

bool cHardwareSMT::Inst_HeadPush(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int dst = FindModifiedStack(STACK_BX);
  Stack(dst).Push(GetHead(head_used).GetFullLocation());
  return true;
}

bool cHardwareSMT::Inst_HeadPop(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int src = FindModifiedStack(STACK_BX);
  GetHead(head_used).SetFullLocation(Stack(src).Pop());
  return true;
}

bool cHardwareSMT::Inst_HeadMove(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = FindModifiedHead(nHardware::HEAD_FLOW);

  if (head_used != target) {
		GetHead(head_used).Set(GetHead(target));
		if (head_used == nHardware::HEAD_IP) AdvanceIP() = false;
	} else {
		m_threads[m_cur_thread].heads[head_used]++;
	}
  return true;
}

bool cHardwareSMT::Inst_HeadSet(cAvidaContext& ctx)
{
  // @DMB - todo
  return true;
}

bool cHardwareSMT::Inst_Call(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_AX);
  
  Stack(dst).Push(IP().GetFullLocation());
  
  ReadLabel(MAX_MEMSPACE_LABEL);
  
  // If a label was specified, call target label
  if (GetLabel().GetSize() != 0) {
    int mem_space_used = FindMemorySpaceLabel(GetLabel(), -1);
    if (mem_space_used > -1) {
      // Jump to beginning of memory space
      IP().Set(0, mem_space_used);
      return true;
    }
    return false;
  }
  
  // Default to calling the flow head
  IP().Set(GetHead(nHardware::HEAD_FLOW));
  
  return true;
}

bool cHardwareSMT::Inst_Return(cAvidaContext& ctx)
{
  const int src = FindModifiedStack(STACK_AX);
  IP().SetFullLocation(Stack(src).Pop());
  return true;
}

bool cHardwareSMT::Inst_Search(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(2, NUM_NOPS);
  cHeadCPU found_pos = FindLabel(0);
  if(found_pos.GetPosition() - IP().GetPosition() == 0)
	{
		GetHead(nHardware::HEAD_FLOW).Set(IP().GetPosition() + 1, IP().GetMemSpace());
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

bool cHardwareSMT::Inst_SearchMem(cAvidaContext& ctx)
{
  // @DMB - todo
  return true; 
}

bool cHardwareSMT::Inst_IO(cAvidaContext& ctx)
{
  // @DMB - todo : exchange full stack...
  
  
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
	
  // Do the "put" component
  const int value_out = Stack(src).Top();
  m_organism->DoOutput(ctx, value_out);  // Check for tasks compleated.
	
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  Stack(dst).Push(value_in);
  m_organism->DoInput(value_in);
  return true;
}

bool cHardwareSMT::Inst_ThreadSet(cAvidaContext& ctx)
{
  // @DMB - todo : add actual thread priority handling
  
  ReadLabel(MAX_THREAD_LABEL);
  bool success = false;
  
  const int priority = Stack(STACK_BX).Top();
  
  if (GetLabel().GetSize()) { // working with a labeled thread
    if (priority) {
      int thread_id = ThreadCreate(GetLabel(), GetHead(nHardware::HEAD_FLOW).GetMemSpace());
      Stack(STACK_CX).Push(thread_id);
      if (!thread_id) {
        success = false;
        m_organism->Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
      }
    } else {
      success = ThreadKill(GetLabel());
      if (!success) m_organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
    }
  } else { // working with an absolute thread ID
    int thread_id = Stack(STACK_CX).Top();
    if (!thread_id) thread_id = m_cur_thread + 1;
    
    if (!priority) {
      success = ThreadKill(thread_id);
      if (!success) m_organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
    }
  }

  return success;
}

bool cHardwareSMT::Inst_ThreadGet(cAvidaContext& ctx)
{
  // @DMB - todo : add actual thread priority handling

  ReadLabel(MAX_THREAD_LABEL);
  int thread_id = 0;

  
  if (GetLabel().GetSize()) {
    int hash_key = GetLabel().AsInt(NUM_NOPS);
    if (m_thread_lbls.Find(hash_key, thread_id)) {
      Stack(STACK_BX).Push(m_threads[thread_id].running);
      Stack(STACK_CX).Push(thread_id + 1);
    } else {
      Stack(STACK_BX).Push(0);
      Stack(STACK_CX).Push(0);
    }
  } else {
    thread_id = Stack(STACK_CX).Top();
    if (thread_id == 0) {                                           // Current Thread
      Stack(STACK_BX).Push(1); // should be priority
      Stack(STACK_CX).Push(m_cur_thread + 1);
    } else if (thread_id < 1 || thread_id > m_threads.GetSize()) {  // Thread out of range
      Stack(STACK_BX).Push(0);
      Stack(STACK_CX).Push(0);
    } else {                                                        // Lookup valid thread
      Stack(STACK_BX).Push(m_threads[thread_id - 1].running);
      Stack(STACK_CX).Push(thread_id);
    }
  }

  return true;
}


bool cHardwareSMT::Inst_Apoptosis(cAvidaContext& ctx)
{
  m_organism->Die();
  return true;
}
