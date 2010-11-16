/*
 *  cHardwareTransSMT.cc
 *  Avida
 *
 *  Created by David on 7/13/06.
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

#include "cHardwareTransSMT.h"

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

tInstLib<cHardwareTransSMT::tMethod>* cHardwareTransSMT::s_inst_slib = cHardwareTransSMT::initInstLib();

tInstLib<cHardwareTransSMT::tMethod>* cHardwareTransSMT::initInstLib(void)
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
    tInstLibEntry<tMethod>("Nop-A", &cHardwareTransSMT::Inst_Nop, nInstFlag::NOP), // 1
    tInstLibEntry<tMethod>("Nop-B", &cHardwareTransSMT::Inst_Nop, nInstFlag::NOP), // 2
    tInstLibEntry<tMethod>("Nop-C", &cHardwareTransSMT::Inst_Nop, nInstFlag::NOP), // 3
    tInstLibEntry<tMethod>("Nop-D", &cHardwareTransSMT::Inst_Nop, nInstFlag::NOP), // 4
    tInstLibEntry<tMethod>("Nop-X", &cHardwareTransSMT::Inst_Nop), // 5
    tInstLibEntry<tMethod>("Val-Shift-R", &cHardwareTransSMT::Inst_ShiftR), // 6
    tInstLibEntry<tMethod>("Val-Shift-L", &cHardwareTransSMT::Inst_ShiftL), // 7
    tInstLibEntry<tMethod>("Val-Nand", &cHardwareTransSMT::Inst_Val_Nand), // 8
    tInstLibEntry<tMethod>("Val-Add", &cHardwareTransSMT::Inst_Val_Add), // 9
    tInstLibEntry<tMethod>("Val-Sub", &cHardwareTransSMT::Inst_Val_Sub), // 10
    tInstLibEntry<tMethod>("Val-Mult", &cHardwareTransSMT::Inst_Val_Mult), // 11
    tInstLibEntry<tMethod>("Val-Div", &cHardwareTransSMT::Inst_Val_Div), // 12
    tInstLibEntry<tMethod>("Val-Mod", &cHardwareTransSMT::Inst_Val_Mod), // 13
    tInstLibEntry<tMethod>("Val-Inc", &cHardwareTransSMT::Inst_Val_Inc), // 14
    tInstLibEntry<tMethod>("Val-Dec", &cHardwareTransSMT::Inst_Val_Dec), // 15
    tInstLibEntry<tMethod>("SetMemory", &cHardwareTransSMT::Inst_SetMemory), // 16
    tInstLibEntry<tMethod>("Divide", &cHardwareTransSMT::Inst_Divide), // 17
    tInstLibEntry<tMethod>("Inst-Read", &cHardwareTransSMT::Inst_HeadRead), // 18
    tInstLibEntry<tMethod>("Inst-Write", &cHardwareTransSMT::Inst_HeadWrite), // 19
    tInstLibEntry<tMethod>("If-Equal", &cHardwareTransSMT::Inst_IfEqual), // 20
    tInstLibEntry<tMethod>("If-Not-Equal", &cHardwareTransSMT::Inst_IfNotEqual), // 21
    tInstLibEntry<tMethod>("If-Less", &cHardwareTransSMT::Inst_IfLess), // 22
    tInstLibEntry<tMethod>("If-Greater", &cHardwareTransSMT::Inst_IfGreater), // 23
    tInstLibEntry<tMethod>("Head-Push", &cHardwareTransSMT::Inst_HeadPush), // 24
    tInstLibEntry<tMethod>("Head-Pop", &cHardwareTransSMT::Inst_HeadPop), // 25
    tInstLibEntry<tMethod>("Head-Move", &cHardwareTransSMT::Inst_HeadMove), // 26
    tInstLibEntry<tMethod>("Search", &cHardwareTransSMT::Inst_Search), // 27
    tInstLibEntry<tMethod>("Push-Next", &cHardwareTransSMT::Inst_PushNext), // 28
    tInstLibEntry<tMethod>("Push-Prev", &cHardwareTransSMT::Inst_PushPrevious), // 29
    tInstLibEntry<tMethod>("Push-Comp", &cHardwareTransSMT::Inst_PushComplement), // 30
    tInstLibEntry<tMethod>("Val-Delete", &cHardwareTransSMT::Inst_ValDelete), // 31
    tInstLibEntry<tMethod>("Val-Copy", &cHardwareTransSMT::Inst_ValCopy), // 32
    tInstLibEntry<tMethod>("IO", &cHardwareTransSMT::Inst_IO), // 33
    tInstLibEntry<tMethod>("Thread-Create", &cHardwareTransSMT::Inst_ThreadCreate), // 34
    tInstLibEntry<tMethod>("Thread-Cancel", &cHardwareTransSMT::Inst_ThreadCancel), // 35
    tInstLibEntry<tMethod>("Thread-Kill", &cHardwareTransSMT::Inst_ThreadKill), // 36
    tInstLibEntry<tMethod>("Inject", &cHardwareTransSMT::Inst_Inject), // 37
    tInstLibEntry<tMethod>("Apoptosis", &cHardwareTransSMT::Inst_Apoptosis), // 38
    tInstLibEntry<tMethod>("Net-Get", &cHardwareTransSMT::Inst_NetGet), // 39
    tInstLibEntry<tMethod>("Net-Send", &cHardwareTransSMT::Inst_NetSend), // 40
    tInstLibEntry<tMethod>("Net-Receive", &cHardwareTransSMT::Inst_NetReceive), // 41
    tInstLibEntry<tMethod>("Net-Last", &cHardwareTransSMT::Inst_NetLast), // 42
    tInstLibEntry<tMethod>("Rotate-Left", &cHardwareTransSMT::Inst_RotateLeft), // 43
    tInstLibEntry<tMethod>("Rotate-Right", &cHardwareTransSMT::Inst_RotateRight), // 44
    tInstLibEntry<tMethod>("Call-Flow", &cHardwareTransSMT::Inst_CallFlow), // 44
    tInstLibEntry<tMethod>("Call-Label", &cHardwareTransSMT::Inst_CallLabel), // 44
    tInstLibEntry<tMethod>("Return", &cHardwareTransSMT::Inst_Return), // 44
    tInstLibEntry<tMethod>("If-Greater-Equal", &cHardwareTransSMT::Inst_IfGreaterEqual), // 23
    tInstLibEntry<tMethod>("Divide-Erase", &cHardwareTransSMT::Inst_Divide_Erase), // 23
    
    tInstLibEntry<tMethod>("NULL", &cHardwareTransSMT::Inst_Nop) // Last Instruction Always NULL
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

cHardwareTransSMT::cHardwareTransSMT(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set)
: cHardwareBase(world, in_organism, in_inst_set), m_mem_array(1)
, m_mem_lbls(Pow(NUM_NOPS, MAX_MEMSPACE_LABEL) / MEM_LBLS_HASH_FACTOR)
, m_thread_lbls(Pow(NUM_NOPS, MAX_THREAD_LABEL) / THREAD_LBLS_HASH_FACTOR)
{
  m_functions = s_inst_slib->GetFunctions();
	
  m_mem_array[0] = in_organism->GetGenome().GetSequence();  // Initialize memory...
  m_mem_array[0].Resize(m_mem_array[0].GetSize() + 1);
  m_mem_array[0][m_mem_array[0].GetSize() - 1] = cInstruction();
  Reset(ctx);                            // Setup the rest of the hardware...
}

void cHardwareTransSMT::internalReset()
{
  // Setup the memory...
  m_mem_array.Resize(1);
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


//TODO: Need to handle different divide methods here, for example not resetting parasites when divide method
//also doesn't reset parasites. - LZ
void cHardwareTransSMT::internalResetOnFailedDivide()
{
	internalReset();
  AdvanceIP() = false;
}

void cHardwareTransSMT::cLocalThread::Reset(cHardwareBase* in_hardware, int mem_space)
{
  for (int i = 0; i < NUM_LOCAL_STACKS; i++) local_stacks[i].Clear();
  for (int i = 0; i < nHardware::NUM_HEADS; i++) heads[i].Reset(in_hardware, mem_space);
	
  cur_head = nHardware::HEAD_IP;
  read_label.Clear();
  next_label.Clear();
  running = true;
  owner = NULL;
  skipExecution = false;
}

cBioUnit* cHardwareTransSMT::ThreadGetOwner()
{
  return (m_threads[m_cur_thread].owner) ? m_threads[m_cur_thread].owner : m_organism;
}


// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.
bool cHardwareTransSMT::SingleProcess(cAvidaContext& ctx, bool speculative)
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
		//Ignore incremeting the thread, set it to be the parasite, unless we draw something lower thean 0.8
		//Then set it to the parasite
		double parasiteVirulence = m_world->GetConfig().PARASITE_VIRULENCE.Get();
		if (parasiteVirulence != -1) {
      
			double probThread = ctx.GetRandom().GetDouble();
      
      //Default to the first thread
			m_cur_thread = 0;
			if (probThread < parasiteVirulence)
			{
        //LZ- this only works for MAX_THREADS 2 right now
				m_cur_thread = 1;
			}
		};
		
		//If we don't have a parasite, this will fix it. 
    if (m_cur_thread >= m_threads.GetSize())
		{
			m_cur_thread = 0;			
		}    
    
    if(m_threads[m_cur_thread].skipExecution)
      m_cur_thread++;
    
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
bool cHardwareTransSMT::SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst) 
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


void cHardwareTransSMT::ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = m_organism->IsRunning();
  m_organism->SetRunning(true);
	
  // Print the status of this CPU at each step...
  if (m_tracer != NULL) m_tracer->TraceHardware(*this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  m_organism->SetRunning(prev_run_state);
}

bool cHardwareTransSMT::OK()
{	
  for (int i = 0; i < m_threads.GetSize(); i++) {
    for(int j=0; j < NUM_LOCAL_STACKS; j++)
			if (m_threads[i].local_stacks[j].OK() == false) return false;
    if (m_threads[i].next_label.OK() == false) return false;
  }
	
  return true;
}

void cHardwareTransSMT::PrintStatus(ostream& fp)
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
    fp << "  Mem " << i << " (" << mem.GetSize() << "): " << mem.AsString() << endl;
  }
  
  fp.flush();
}


int cHardwareTransSMT::FindMemorySpaceLabel(const cCodeLabel& label, int mem_space)
{
	if (label.GetSize() == 0) return 0;
  
  int hash_key = label.AsInt(NUM_NOPS);
  if (!m_mem_lbls.Find(hash_key, mem_space)) {
    mem_space = m_mem_array.GetSize();
    m_mem_array.Resize(mem_space + 1);
    m_mem_lbls.Set(hash_key, mem_space);
  }
  
  return mem_space;
}


/////////////////////////////////////////////////////////////////////////
// Method: cHardwareTransSMT::FindLabel(direction)
//
// Search in 'direction' (+ or - 1) from the instruction pointer for the
// compliment of the label in 'next_label' and return a pointer to the
// results.  If direction is 0, search from the beginning of the genome.
//
/////////////////////////////////////////////////////////////////////////
cHeadCPU cHardwareTransSMT::FindLabel(int direction)
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
int cHardwareTransSMT::FindLabel_Forward(const cCodeLabel& search_label,
                                         const cSequence& search_genome, int pos)
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
int cHardwareTransSMT::FindLabel_Backward(const cCodeLabel & search_label,
                                          const cSequence & search_genome, int pos)
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
cHeadCPU cHardwareTransSMT::FindLabel(const cCodeLabel& in_label, int direction)
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
bool cHardwareTransSMT::InjectParasite(cAvidaContext& ctx, double mut_multiplier)
{
  const int end_pos = GetHead(nHardware::HEAD_WRITE).GetPosition();
  const int mem_space_used = GetHead(nHardware::HEAD_WRITE).GetMemSpace();
  
  // Make sure the creature will still be above the minimum size
  if (end_pos <= 0) {
    m_organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: no code to inject");
    return false; // (inject fails)
  }
  if (end_pos < MIN_INJECT_SIZE) {
    m_organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: new size too small");
    return false; // (inject fails)
  }  
  
  //update the parasites tasks
	m_organism->GetPhenotype().UpdateParasiteTasks();
  
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
	
  if (m_world->GetConfig().INJECT_METHOD.Get() == INJECT_METHOD_SPLIT) {
    for (int x = 0; x < nHardware::NUM_HEADS; x++) GetHead(x).Reset(this, IP().GetMemSpace());
    for (int x = 0; x < NUM_LOCAL_STACKS; x++) Stack(x).Clear();
  }
  
  AdvanceIP() = false;
  
  return inject_signal;
}


bool cHardwareTransSMT::ParasiteInfectHost(cBioUnit* bu)
{
  assert(bu->GetGenome().GetHardwareType() == GetType() && bu->GetGenome().GetInstSet() == m_inst_set->GetInstSetName());
  
  cCodeLabel label;
  label.ReadString(bu->GetUnitSourceArgs());
  
  // Inject fails if the memory space is already in use
  if (label.GetSize() == 0 || MemorySpaceExists(label)) { return false; }
  
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
  
  if(m_world->GetConfig().INJECT_IS_VIRULENT.Get())
    m_threads[0].skipExecution = true;
  
  return true;
}


void cHardwareTransSMT::ReadInst(const int in_inst)
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

void cHardwareTransSMT::ReadLabel(int max_size)
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


int cHardwareTransSMT::ThreadCreate(const cCodeLabel& label, int mem_space)
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
  m_thread_lbls.Set(hash_key, thread_id);
  
  // Setup this thread into the current selected memory space (Flow Head)
  m_threads[thread_id].Reset(this, mem_space);
  m_threads[thread_id].owner = m_threads[m_cur_thread].owner;
	
  return thread_id;
}

bool cHardwareTransSMT::ThreadKill(const int thread_id)
{
  if (thread_id < 1 || thread_id >= m_threads.GetSize()) return false;
  
  // Increment thread, if killing current thread
  if (m_cur_thread == thread_id) ThreadNext();
  
  // Set this thread to not running
  m_threads[thread_id].running = false;
	
  return true;
}



////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int cHardwareTransSMT::FindModifiedStack(int default_stack)
{
  assert(default_stack < NUM_STACKS);  // Stack ID too high.
	
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_stack = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  }
  return default_stack;
}

inline int cHardwareTransSMT::FindModifiedNextStack(int default_stack)
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

inline int cHardwareTransSMT::FindModifiedPreviousStack(int default_stack)
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

inline int cHardwareTransSMT::FindModifiedComplementStack(int default_stack)
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

inline int cHardwareTransSMT::FindModifiedHead(int default_head)
{
  assert(default_head < nHardware::NUM_HEADS); // Head ID too high.
	
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();    
    int nop_head = m_inst_set->GetNopMod(IP().GetInst());
    if (nop_head < nHardware::NUM_HEADS) default_head = nop_head;
    IP().SetFlagExecuted();
  }
  
  if (default_head > 240)
    cout << "uh oh!" << endl;
  
  return default_head;
  
}

inline int cHardwareTransSMT::FindNextStack(int default_stack)
{
  return (default_stack + 1) % NUM_STACKS;
}

inline int cHardwareTransSMT::FindPreviousStack(int default_stack)
{
  return (default_stack + NUM_STACKS - 1) % NUM_STACKS;
}

inline int cHardwareTransSMT::FindComplementStack(int base_stack)
{
  return (base_stack + 2) % NUM_STACKS;
}


int cHardwareTransSMT::calcCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  const cCPUMemory& memory = m_mem_array[m_cur_child];
  for (int i = 0; i < memory.GetSize(); i++) {
    if (memory.FlagCopied(i)) copied_size++;
	}
  return copied_size;
}

void cHardwareTransSMT::Inject_DoMutations(cAvidaContext& ctx, double mut_multiplier, cCPUMemory& injected_code)
{
  m_organism->GetPhenotype().SetDivType(mut_multiplier);
	
  // Divide Mutations (per site)
  int num_mut = ctx.GetRandom().GetRandBinomial(injected_code.GetSize(), 
                                                m_organism->GetInjectMutProb() / mut_multiplier);
  // If we have lines to mutate...
  if( num_mut > 0 ){
    for (int i = 0; i < num_mut; i++) {
      int site = ctx.GetRandom().GetUInt(injected_code.GetSize());
      injected_code[site] = m_inst_set->GetRandomInst(ctx);
    }
  }
	
  // Insert Mutations (per site)
  num_mut = ctx.GetRandom().GetRandBinomial(injected_code.GetSize(),
                                            m_organism->GetInjectInsProb());
  // If would make creature to big, insert up to MAX_GENOME_LENGTH
  if( num_mut + injected_code.GetSize() > MAX_GENOME_LENGTH )
    num_mut = MAX_GENOME_LENGTH - injected_code.GetSize();
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
	
  // Delete Mutations (per site)
  num_mut = ctx.GetRandom().GetRandBinomial(injected_code.GetSize(),
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
	
  // Mutations in the parent's genome
  if (m_organism->GetParentMutProb() > 0) {
    for (int i = 0; i < m_mem_array[0].GetSize(); i++) {
      if (m_organism->TestParentMut(ctx)) {
				m_mem_array[0][i] = m_inst_set->GetRandomInst(ctx);
      }
    }
  }
	
}



bool cHardwareTransSMT::Divide_Main(cAvidaContext& ctx, double mut_multiplier)
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
	
  // Division Methods:
  // 0 - DIVIDE_METHOD_OFFSPRING - Create a child, leave parent state untouched.
  // 1 - DIVIDE_METHOD_SPLIT - Create a child, completely reset state of parent.
  // 2 - DIVIDE_METHOD_BIRTH - Create a child, reset state of parent's current thread.
  
  if (parent_alive) { // If the parent is no longer alive, all of this is moot
    switch (m_world->GetConfig().DIVIDE_METHOD.Get()) {
      case DIVIDE_METHOD_SPLIT:
        Reset(ctx);  // This will wipe out all parasites on a divide.
        break;
        
      case DIVIDE_METHOD_BIRTH:
        // Reset only the calling thread's state
        for(int x = 0; x < nHardware::NUM_HEADS; x++) GetHead(x).Reset(this, 0);
        for(int x = 0; x < NUM_LOCAL_STACKS; x++) Stack(x).Clear();
        if(m_world->GetConfig().INHERIT_MERIT.Get() == 0)
          m_organism->GetPhenotype().ResetMerit(m_organism->GetGenome().GetSequence());
        break;
        
      case DIVIDE_METHOD_OFFSPRING:
      default:
        break;
		}
		AdvanceIP() = false;
	}
	
  return true;
}


// --------  Begin Core Instruction Set --------


//6
bool cHardwareTransSMT::Inst_ShiftR(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
  int value = Stack(src).Pop();
  value >>= 1;
  Stack(dst).Push(value);
  return true;
}

//7
bool cHardwareTransSMT::Inst_ShiftL(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
  int value = Stack(src).Pop();
  value <<= 1;
  Stack(dst).Push(value);
  return true;
}

//8
bool cHardwareTransSMT::Inst_Val_Nand(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  Stack(dst).Push(~(Stack(op1).Top() & Stack(op2).Top()));
  return true;
}

//9
bool cHardwareTransSMT::Inst_Val_Add(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  Stack(dst).Push(Stack(op1).Top() + Stack(op2).Top());
  return true;
}

//10
bool cHardwareTransSMT::Inst_Val_Sub(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  Stack(dst).Push(Stack(op1).Top() - Stack(op2).Top());
  return true;
}

//11
bool cHardwareTransSMT::Inst_Val_Mult(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  Stack(dst).Push(Stack(op1).Top() * Stack(op2).Top());
  return true;
}

//12
bool cHardwareTransSMT::Inst_Val_Div(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op2).Top() != 0) {
    if (0-INT_MAX > Stack(op1).Top() && Stack(op2).Top() == -1)
      m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: Float exception");
    else
      Stack(dst).Push(Stack(op1).Top() / Stack(op2).Top());
  } else {
    m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
    return false;
  }
  return true;
}

//13
bool cHardwareTransSMT::Inst_Val_Mod(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int op1 = FindModifiedStack(STACK_BX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op2).Top() != 0) {
    if(Stack(op2).Top() == -1)
      Stack(dst).Push(0);
    else
      Stack(dst).Push(Stack(op1).Top() % Stack(op2).Top());
  } else {
    m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
		return false;
  }
  return true;
}

//14
bool cHardwareTransSMT::Inst_Val_Inc(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
  int value = Stack(src).Pop();
  Stack(dst).Push(++value);
  return true;
}

//15
bool cHardwareTransSMT::Inst_Val_Dec(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
  int value = Stack(src).Pop();
  Stack(dst).Push(--value);
  return true;
}

//16
bool cHardwareTransSMT::Inst_SetMemory(cAvidaContext& ctx) 
{
  ReadLabel(MAX_MEMSPACE_LABEL);
  
  if(ThreadGetOwner()->IsParasite())
  {
		if(m_world->GetConfig().PARASITE_MEM_SPACES.Get())
		{
      //If parasites get their own memory spaces, just prefix them with 4 nops (1 more than the hosts can possibly access) - LZ
			GetLabel().AddNop(1);
			GetLabel().AddNop(1);
			GetLabel().AddNop(1);
			GetLabel().AddNop(1);
		}
  }
	
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
bool cHardwareTransSMT::Inst_Divide(cAvidaContext& ctx)
{
  return Divide_Main(ctx);
}


//18
bool cHardwareTransSMT::Inst_HeadRead(cAvidaContext& ctx)
{
  const int head_id = FindModifiedHead(nHardware::HEAD_READ);
  const int dst = FindModifiedStack(STACK_AX);
  
  GetHead(head_id).Adjust();
	
  // Mutations only occur on the read, for the moment.
  int read_inst = 0;
  bool toMutate = true;
  
  if(m_world->GetConfig().PARASITE_NO_COPY_MUT.Get())
  {
    //Parasites should not have any copy mutations;
    if(ThreadGetOwner()->IsParasite()) toMutate = false;
  }
  
  if (m_organism->TestCopyMut(ctx) && toMutate) {
    read_inst = m_inst_set->GetRandomInst(ctx).GetOp();
  } else {
    read_inst = GetHead(head_id).GetInst().GetOp();
  }
  
  //read_inst = GetHead(head_id).GetInst().GetOp();
  Stack(dst).Push(read_inst);
  ReadInst(read_inst);
	
  GetHead(head_id).Advance();
  return true;
}

//19
bool cHardwareTransSMT::Inst_HeadWrite(cAvidaContext& ctx)
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

//20
bool cHardwareTransSMT::Inst_IfEqual(cAvidaContext& ctx)      // Execute next if bx == ?cx?
{
  const int op1 = FindModifiedStack(STACK_AX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op1).Top() != Stack(op2).Top())  IP().Advance();
  return true;
}

//21
bool cHardwareTransSMT::Inst_IfNotEqual(cAvidaContext& ctx)     // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedStack(STACK_AX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op1).Top() == Stack(op2).Top())  IP().Advance();
  return true;
}

//22
bool cHardwareTransSMT::Inst_IfLess(cAvidaContext& ctx)       // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedStack(STACK_AX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op1).Top() >=  Stack(op2).Top())  IP().Advance();
  return true;
}

//23
bool cHardwareTransSMT::Inst_IfGreater(cAvidaContext& ctx)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedStack(STACK_AX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op1).Top() <= Stack(op2).Top())  IP().Advance();
  return true;
}

//24
bool cHardwareTransSMT::Inst_HeadPush(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int dst = FindModifiedStack(STACK_BX);
  Stack(dst).Push(GetHead(head_used).GetPosition());
  return true;
}

//25
bool cHardwareTransSMT::Inst_HeadPop(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int src = FindModifiedStack(STACK_BX);
  GetHead(head_used).Set(Stack(src).Pop(), GetHead(head_used).GetMemSpace());
  return true;
}

//26
bool cHardwareTransSMT::Inst_HeadMove(cAvidaContext& ctx)
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
bool cHardwareTransSMT::Inst_Search(cAvidaContext& ctx)
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

//28
bool cHardwareTransSMT::Inst_PushNext(cAvidaContext& ctx) 
{
  const int src = FindModifiedStack(STACK_AX);
  const int dst = FindModifiedNextStack(src);
  Stack(dst).Push(Stack(src).Pop());
  return true;
}

//29
bool cHardwareTransSMT::Inst_PushPrevious(cAvidaContext& ctx) 
{
  const int src = FindModifiedStack(STACK_BX);
  const int dst = FindModifiedPreviousStack(src);
  Stack(dst).Push(Stack(src).Pop());
  return true;
}

//30
bool cHardwareTransSMT::Inst_PushComplement(cAvidaContext& ctx) 
{
  int src = FindModifiedStack(STACK_BX);
  const int dst = FindModifiedComplementStack(src);
  Stack(dst).Push(Stack(src).Pop());
  return true;
}

//31
bool cHardwareTransSMT::Inst_ValDelete(cAvidaContext& ctx)
{
  int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Pop();
  return true;
}

//32
bool cHardwareTransSMT::Inst_ValCopy(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
  Stack(dst).Push(Stack(src).Top());
  return true;
}

//33
bool cHardwareTransSMT::Inst_IO(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int src = FindModifiedStack(dst);
	
  // Do the "put" component
  const int value_out = Stack(src).Top();
  
  m_organism->DoOutput(ctx, value_out, ThreadGetOwner()->IsParasite());  // Check for tasks compleated.
	
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  Stack(dst).Push(value_in);
  m_organism->DoInput(value_in);
  return true;
}


// --------  End Core Instruction Set --------


// --------  Additional Instructions --------


//34
bool cHardwareTransSMT::Inst_ThreadCreate(cAvidaContext& ctx)
{
  ReadLabel(MAX_THREAD_LABEL);
  bool success = ThreadCreate(GetLabel(), GetHead(nHardware::HEAD_FLOW).GetMemSpace());
  if (!success) m_organism->Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
  
  if (success) m_organism->GetPhenotype().SetIsMultiThread();
  
  return success;
}

//35
bool cHardwareTransSMT::Inst_ThreadCancel(cAvidaContext& ctx)
{
  bool success = ThreadKill(m_cur_thread);
  if (!success) m_organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  
  if(m_threads.GetSize() == 1) m_organism->GetPhenotype().ClearIsMultiThread();
  
  return success;
}

//36
bool cHardwareTransSMT::Inst_ThreadKill(cAvidaContext& ctx)
{
  ReadLabel(MAX_THREAD_LABEL);
  bool success = ThreadKill(GetLabel());
  if (!success) m_organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  
  if(m_threads.GetSize() == 1) m_organism->GetPhenotype().ClearIsMultiThread();
  
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
bool cHardwareTransSMT::Inst_Inject(cAvidaContext& ctx)
{
  ReadLabel(MAX_MEMSPACE_LABEL);
  
  if(ThreadGetOwner()->IsParasite())
  {
		if(m_world->GetConfig().PARASITE_MEM_SPACES.Get())
		{
			GetLabel().AddNop(1);
			GetLabel().AddNop(1);
			GetLabel().AddNop(1);
			GetLabel().AddNop(1);
		}
  }
  
  double mut_multiplier = 1.0;	
  return InjectParasite(ctx, mut_multiplier);
}

//38
bool cHardwareTransSMT::Inst_Apoptosis(cAvidaContext& ctx)
{
  m_organism->Die();
  
  return true;
}

//39
bool cHardwareTransSMT::Inst_NetGet(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  const int seq_dst = FindModifiedNextStack(dst);
  int val, seq;
  m_organism->NetGet(ctx, val, seq);
  Stack(dst).Push(val);
  Stack(seq_dst).Push(seq);
  
  return true;
}

//40
bool cHardwareTransSMT::Inst_NetSend(cAvidaContext& ctx)
{
  const int src = FindModifiedStack(STACK_BX);
  m_organism->NetSend(ctx, Stack(src).Pop());
  return true;
}

//41
bool cHardwareTransSMT::Inst_NetReceive(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_BX);
  int val;
  bool success = m_organism->NetReceive(val);
  Stack(dst).Push(val);
  return success;
}

//42
bool cHardwareTransSMT::Inst_NetLast(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_CX);
  Stack(dst).Push(m_organism->NetLast());
  return true;
}

//43
bool cHardwareTransSMT::Inst_RotateLeft(cAvidaContext& ctx)
{
  const int num_neighbors = m_organism->GetNeighborhoodSize();
  
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  // Always rotate at least once.
  m_organism->Rotate(-1);
  
  return true;
}

//44
bool cHardwareTransSMT::Inst_RotateRight(cAvidaContext& ctx)
{
  const int num_neighbors = m_organism->GetNeighborhoodSize();
  
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  // Always rotate at least once.
  m_organism->Rotate(1);
  
  return true;
}

//45
bool cHardwareTransSMT::Inst_CallFlow(cAvidaContext& ctx)
{
  const int dst = FindModifiedStack(STACK_AX);
  
  const int location = IP().GetPosition() & 0xFFFF;
  const int mem_space = IP().GetMemSpace() << 8;
  const int ra  = location | mem_space;
  
  Stack(dst).Push(ra);
  
  cHeadCPU& flow = GetHead(nHardware::HEAD_FLOW);
  IP().Set(flow.GetPosition(), flow.GetMemSpace());
  
  return true;
}

//46
bool cHardwareTransSMT::Inst_CallLabel(cAvidaContext& ctx)
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
bool cHardwareTransSMT::Inst_Return(cAvidaContext& ctx)
{
  const int src = FindModifiedStack(STACK_AX);
  const unsigned int ra = Stack(src).Pop();
  
  const int location = ra & 0xFFFF;
  const int mem_space = NormalizeMemSpace(ra >> 8);
  
  IP().Set(location, mem_space);
  
  return true;
}

//48
bool cHardwareTransSMT::Inst_IfGreaterEqual(cAvidaContext& ctx)      // Execute next if bx >= ?cx?
{
  const int op1 = FindModifiedStack(STACK_AX);
  const int op2 = FindModifiedNextStack(op1);
  if (Stack(op1).Top() > Stack(op2).Top())  IP().Advance();
  return true;
}

bool cHardwareTransSMT::Inst_Divide_Erase(cAvidaContext& ctx)
{
  bool toReturn =  Divide_Main(ctx);
  if(toReturn)
    return toReturn;
  
	//internalReset();
  
  m_organism->GetPhenotype().DivideFailed();
  
  const int mem_space_used = GetHead(nHardware::HEAD_WRITE).GetMemSpace();
  
  if (m_mem_array.GetSize() <= mem_space_used) return false;
  
  m_mem_array[mem_space_used] = cSequence("a"); 
  
  for(int x = 0; x < nHardware::NUM_HEADS; x++) GetHead(x).Reset(this, 0);
  //for(int x = 0; x < NUM_LOCAL_STACKS; x++) Stack(x).Clear();
  
  return toReturn;
  
}
