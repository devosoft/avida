//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////



#ifndef HARDWARE_4STACK_HH
#include "hardware_4stack.hh"
#endif

#ifndef CONFIG_HH
#include "config.hh"
#endif
#ifndef CPU_TEST_INFO_HH
#include "cpu_test_info.hh"
#endif
#ifndef FUNCTIONS_HH
#include "functions.hh"
#endif
#ifndef GENOME_UTIL_HH
#include "genome_util.hh"
#endif
#ifndef INST_LIB_BASE_HH
#include "inst_lib_base.hh"
#endif
#ifndef INST_SET_HH
#include "inst_set.hh"
#endif
#ifndef MUTATION_HH
#include "mutation.hh"
#endif
#ifndef MUTATION_LIB_HH
#include "mutation_lib.hh"
#endif
#ifndef MUTATION_MACROS_HH
#include "mutation_macros.hh"
#endif
#ifndef ORGANISM_HH
#include "organism.hh"
#endif
#ifndef PHENOTYPE_HH
#include "phenotype.hh"
#endif
#ifndef RANDOM_HH
#include "random.hh"
#endif
#ifndef STRING_UTIL_HH
#include "string_util.hh"
#endif
#ifndef TEST_CPU_HH
#include "test_cpu.hh"
#endif

#include <limits.h>

using namespace std;




///////////////
//  cInstLib4Stack
///////////////

class cInstLib4Stack : public cInstLibBase {
  const size_t m_nopmods_array_size;
  const size_t m_function_array_size;
  cString *m_nopmod_names;
  cString *m_function_names;
  const int *m_nopmods;
  cHardware4Stack::tHardware4StackMethod *m_functions;
  static const cInstruction inst_error;
  static const cInstruction inst_default;
public:
  cInstLib4Stack(
    size_t nopmod_array_size,
    size_t function_array_size,
    cString *nopmod_names,
    cString *function_names,
    const int *nopmods,
    cHardware4Stack::tHardware4StackMethod *functions
  ):m_nopmods_array_size(nopmod_array_size),
    m_function_array_size(function_array_size),
    m_nopmod_names(nopmod_names),
    m_function_names(function_names),
    m_nopmods(nopmods),
    m_functions(functions)
  {}
  cHardware4Stack::tHardware4StackMethod *GetFunctions(void){ return m_functions; } 
  const cString &GetName(const unsigned int id) {
    assert(id < m_function_array_size);
    return m_function_names[id];
  }
  const cString &GetNopName(const unsigned int id) {
    assert(id < m_nopmods_array_size);
    return m_nopmod_names[id];
  }
  int GetNopMod(const unsigned int id){
    assert(id < m_nopmods_array_size);
    return m_nopmods[id];
  }
  int GetNopMod(const cInstruction & inst){
    return GetNopMod(inst.GetOp());
  }
  int GetSize(){ return m_function_array_size; }
  int GetNumNops(){ return m_nopmods_array_size; }
  cInstruction GetInst(const cString & in_name){
    for (unsigned int i = 0; i < m_function_array_size; i++) {
      if (m_function_names[i] == in_name) return cInstruction(i);
    }
    return cInstLib4Stack::GetInstError();
  }
  const cInstruction & GetInstDefault(){ return inst_default; }
  const cInstruction & GetInstError(){ return inst_error; }
};

///////////////
//  cHardware4Stack
///////////////

const cInstruction cInstLib4Stack::inst_error(255);
const cInstruction cInstLib4Stack::inst_default(0);
cInstLibBase *cHardware4Stack::GetInstLib(){ return s_inst_slib; }

cInstLib4Stack *cHardware4Stack::s_inst_slib = cHardware4Stack::initInstLib();
cInstLib4Stack *cHardware4Stack::initInstLib(void){
  struct cNOPEntry4Stack {
    cNOPEntry4Stack(const cString &name, int nop_mod):name(name), nop_mod(nop_mod){}
    cString name;
    int nop_mod;
  };
  static const cNOPEntry4Stack s_n_array[] = {
    cNOPEntry4Stack("Nop-A", STACK_AX),
    cNOPEntry4Stack("Nop-B", STACK_BX),
    cNOPEntry4Stack("Nop-C", STACK_CX),
    cNOPEntry4Stack("Nop-D", STACK_DX),
    cNOPEntry4Stack("Nop-E", STACK_EX),
    cNOPEntry4Stack("Nop-F", STACK_FX)
  };

  struct cInstEntry4Stack {
    cInstEntry4Stack(const cString &name, tHardware4StackMethod function):name(name), function(function){}
    cString name;
    tHardware4StackMethod function;
  };
  static const cInstEntry4Stack s_f_array[] = {
    //1 
    cInstEntry4Stack("Nop-A",     &cHardware4Stack::Inst_Nop), 
    //2
    cInstEntry4Stack("Nop-B",     &cHardware4Stack::Inst_Nop), 
    //3
    cInstEntry4Stack("Nop-C",     &cHardware4Stack::Inst_Nop),   
    //4 
    cInstEntry4Stack("Nop-D",     &cHardware4Stack::Inst_Nop), 
    //38
    cInstEntry4Stack("Nop-E",     &cHardware4Stack::Inst_Nop),
    //39
    cInstEntry4Stack("Nop-F",     &cHardware4Stack::Inst_Nop),
    //5
    cInstEntry4Stack("Nop-X",     &cHardware4Stack::Inst_Nop),
    //6 
    cInstEntry4Stack("Val-Shift-R",   &cHardware4Stack::Inst_ShiftR),
    //7
    cInstEntry4Stack("Val-Shift-L",   &cHardware4Stack::Inst_ShiftL),
    //8
    cInstEntry4Stack("Val-Nand",      &cHardware4Stack::Inst_Val_Nand),
    //9
    cInstEntry4Stack("Val-Add",       &cHardware4Stack::Inst_Val_Add),
    //10
    cInstEntry4Stack("Val-Sub",       &cHardware4Stack::Inst_Val_Sub),
    //11
    cInstEntry4Stack("Val-Mult",      &cHardware4Stack::Inst_Val_Mult),
    //12
    cInstEntry4Stack("Val-Div",       &cHardware4Stack::Inst_Val_Div),
    //13
    cInstEntry4Stack("SetMemory",   &cHardware4Stack::Inst_SetMemory),
    //14
    cInstEntry4Stack("Divide",  &cHardware4Stack::Inst_Divide),
    //15
    cInstEntry4Stack("Inst-Read",    &cHardware4Stack::Inst_HeadRead),
    //16
    cInstEntry4Stack("Inst-Write",   &cHardware4Stack::Inst_HeadWrite),
    //keeping this one for the transition period
    //cInstEntry4Stack("Inst-Copy",    &cHardware4Stack::Inst_HeadCopy),
    //17
    cInstEntry4Stack("If-Equal",    &cHardware4Stack::Inst_IfEqual),
    //18
    cInstEntry4Stack("If-Not-Equal",  &cHardware4Stack::Inst_IfNotEqual),
    //19
    cInstEntry4Stack("If-Less",   &cHardware4Stack::Inst_IfLess),
    //20
    cInstEntry4Stack("If-Greater",    &cHardware4Stack::Inst_IfGreater),
    //21
    cInstEntry4Stack("Head-Push",    &cHardware4Stack::Inst_HeadPush),
    //22
    cInstEntry4Stack("Head-Pop",     &cHardware4Stack::Inst_HeadPop),
    //23
    cInstEntry4Stack("Head-Move",  &cHardware4Stack::Inst_HeadMove),
    //24
    cInstEntry4Stack("Search",  &cHardware4Stack::Inst_Search),
    //25
    cInstEntry4Stack("Push-Next",    &cHardware4Stack::Inst_PushNext),
    //26
    cInstEntry4Stack("Push-Prev",    &cHardware4Stack::Inst_PushPrevious),
    //27
    cInstEntry4Stack("Push-Comp",    &cHardware4Stack::Inst_PushComplement),
    //28
    cInstEntry4Stack("Val-Delete", &cHardware4Stack::Inst_ValDelete),
    //29
    cInstEntry4Stack("Val-Copy",  &cHardware4Stack::Inst_ValCopy),
    //30
    cInstEntry4Stack("ThreadFork",   &cHardware4Stack::Inst_ForkThread),
    //31
    //cInstEntry4Stack("if-label",  &cHardware4Stack::Inst_IfLabel),
    //32
    cInstEntry4Stack("Val-Inc",       &cHardware4Stack::Inst_Increment),
    //33
    cInstEntry4Stack("Val-Dec",       &cHardware4Stack::Inst_Decrement),
    //34
    cInstEntry4Stack("Val-Mod",       &cHardware4Stack::Inst_Mod),
    //35
    cInstEntry4Stack("ThreadKill",   &cHardware4Stack::Inst_KillThread),
    //36
    cInstEntry4Stack("IO", &cHardware4Stack::Inst_IO),
    //37
    cInstEntry4Stack("Inject", &cHardware4Stack::Inst_Inject)
  };

  const int n_size = sizeof(s_n_array)/sizeof(cNOPEntry4Stack);

  cout << "Instruction Library has " << n_size << " instructions." << endl;

  static cString n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size; i++){
    n_names[i] = s_n_array[i].name;
    nop_mods[i] = s_n_array[i].nop_mod;
  }

  const int f_size = sizeof(s_f_array)/sizeof(cInstEntry4Stack);
  static cString f_names[f_size];
  static tHardware4StackMethod functions[f_size];
  for (int i = 0; i < f_size; i++){
    f_names[i] = s_f_array[i].name;
    functions[i] = s_f_array[i].function;
  }

  cInstLib4Stack *inst_lib = new cInstLib4Stack(
    n_size,
    f_size,
    n_names,
    f_names,
    nop_mods,
    functions
  );

  cout <<
  "<cHardware4Stack::initInstLib> debug: important post-init values:" <<endl<<
  " --- GetSize(): " << inst_lib->GetSize() <<endl<<
  " --- GetNumNops(): " << inst_lib->GetNumNops() <<endl<<
  " --- GetName(last): " <<
  inst_lib->GetName(inst_lib->GetSize() - 1) <<endl<<
  endl;

  return inst_lib;
}

cHardware4Stack::cHardware4Stack(cOrganism * in_organism, cInstSet * in_inst_set)
  : cHardwareBase(in_organism, in_inst_set)
  , memory_array(NUM_MEMORY_SPACES)
{
  /* FIXME:  reorganize storage of m_functions.  -- kgn */
  m_functions = s_inst_slib->GetFunctions();
  /**/
  inst_remainder = 0;
 
  for(int x=1; x<=cConfig::GetMaxCPUThreads(); x++)
    {
      slice_array[x] = (x-1)*cConfig::GetThreadSlicingMethod()+1;
    }

  memory_array[0] = in_organism->GetGenome();  // Initialize memory...
  memory_array[0].Resize(GetMemory(0).GetSize()+1);
  memory_array[0][memory_array[0].GetSize()-1] = cInstruction();
  Reset();                            // Setup the rest of the hardware...
}


cHardware4Stack::~cHardware4Stack()
{
}


void cHardware4Stack::Recycle(cOrganism * new_organism, cInstSet * in_inst_set)
{
  cHardwareBase::Recycle(new_organism, in_inst_set);
  memory_array[0] = new_organism->GetGenome();
  memory_array[0].Resize(GetMemory(0).GetSize()+1);
  memory_array[0][memory_array[0].GetSize()-1] = cInstruction();
  Reset();
}


void cHardware4Stack::Reset()
{
  //global_stack.Clear();
  //thread_time_used = 0;

  // Setup the memory...
  for (int i = 1; i < NUM_MEMORY_SPACES; i++) {
      memory_array[i].Resize(1);
      //GetMemory(i).Replace(0, 1, cGenome(ConvertToInstruction(i)));
      GetMemory(i)=cGenome(ConvertToInstruction(i)); 
  }

  // We want to reset to have a single thread.
  threads.Resize(1);

  // Reset that single thread.
  threads[0].Reset(this, 0);
  thread_id_chart = 1; // Mark only the first thread as taken...
  cur_thread = 0;

  mal_active = false;

  // Reset all stacks (local and global)
  for(int i=0; i<NUM_STACKS; i++)
    {
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

void cHardware4Stack::SingleProcess()
{
  // Mark this organism as running...
  organism->SetRunning(true);

  cPhenotype & phenotype = organism->GetPhenotype();
  phenotype.IncTimeUsed();
  //if(organism->GetCellID()==46 && IP().GetMemSpace()==2)
  // int x=0;

  //if (GetNumThreads() > 1) thread_time_used++;
  //assert((GetHead(HEAD_WRITE).GetPosition() == Stack(STACK_BX).Top() ||
  // Stack(STACK_BX).Top()==GetMemory(IP().GetMemSpace()).GetSize()-1 || 
  // GetHead(HEAD_WRITE).GetPosition() == Stack(STACK_BX).Top()+1) &&
  // (GetHead(HEAD_WRITE).GetMemSpace() == IP().GetMemSpace() ||
  //  GetHead(HEAD_WRITE).GetMemSpace() == IP().GetMemSpace()+1));
  // If we have threads turned on and we executed each thread in a single
  // timestep, adjust the number of instructions executed accordingly.
  //const int num_inst_exec = (cConfig::GetThreadSlicingMethod() == 1) ?
  //  GetNumThreads() : 1;

  const int num_inst_exec = int(slice_array[GetNumThreads()]+ inst_remainder);
  inst_remainder = slice_array[GetNumThreads()] + inst_remainder - num_inst_exec;
  
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
    
    // Print the status of this CPU at each step...
    if (trace_fp != NULL) {
      const cString & next_name = inst_set->GetName(IP().GetInst())();
      organism->PrintStatus(*trace_fp, next_name);
    }
    
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
bool cHardware4Stack::SingleProcess_PayCosts(const cInstruction & cur_inst)
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
bool cHardware4Stack::SingleProcess_ExecuteInst(const cInstruction & cur_inst) 
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


void cHardware4Stack::ProcessBonusInst(const cInstruction & inst)
{
  // Mark this organism as running...
  bool prev_run_state = organism->GetIsRunning();
  organism->SetRunning(true);

  // @CAO FIX PRINTING TO INDICATE THIS IS A BONUS
  // Print the status of this CPU at each step...
  if (trace_fp != NULL) {
    cString next_name = cStringUtil::Stringf("%s (bonus instruction)",
					     inst_set->GetName(inst)());
    organism->PrintStatus(*trace_fp, next_name);
  }
    
  SingleProcess_ExecuteInst(inst);

  organism->SetRunning(prev_run_state);
}


void cHardware4Stack::LoadGenome(const cGenome & new_genome)
{
  GetMemory(0) = new_genome;
}


bool cHardware4Stack::OK()
{
  bool result = true;

  for(int i = 0 ; i < NUM_MEMORY_SPACES; i++) {
    if (!memory_array[i].OK()) result = false;
  }

  for (int i = 0; i < GetNumThreads(); i++) {
    for(int j=0; j<NUM_LOCAL_STACKS; j++)
    if (threads[i].local_stacks[j].OK() == false) result = false;
    if (threads[i].next_label.OK() == false) result = false;
  }

  return result;
}

void cHardware4Stack::PrintStatus(ostream & fp)
{
  fp << organism->GetPhenotype().GetTimeUsed() << " "
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
// Method: cHardware4Stack::FindLabel(direction)
//
// Search in 'direction' (+ or - 1) from the instruction pointer for the
// compliment of the label in 'next_label' and return a pointer to the
// results.  If direction is 0, search from the beginning of the genome.
//
/////////////////////////////////////////////////////////////////////////

c4StackHead cHardware4Stack::FindLabel(int direction)
{
  c4StackHead & inst_ptr = IP();

  // Start up a search head at the position of the instruction pointer.
  c4StackHead search_head(inst_ptr);
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
  //*** I THINK THIS MIGHT HAVE BEEN WRONG...CHANGED >= to >.  -law ***//
  
  // Return the found position (still at start point if not found).
  return search_head;
}


// Search forwards for search_label from _after_ position pos in the
// memory.  Return the first line _after_ the the found label.  It is okay
// to find search label's match inside another label.

int cHardware4Stack::FindLabel_Forward(const cCodeLabel & search_label,
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

int cHardware4Stack::FindLabel_Backward(const cCodeLabel & search_label,
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
c4StackHead cHardware4Stack::FindLabel(const cCodeLabel & in_label, int direction)
{
  assert (in_label.GetSize() > 0);

  // IDEALY:
  // Keep making jumps (in the proper direction) equal to the label
  // length.  If we are inside of a label, check its size, and see if
  // any of the sub-labels match properly.
  // FOR NOW:
  // Get something which works, no matter how inefficient!!!

  c4StackHead temp_head(this);

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
c4StackHead cHardware4Stack::FindFullLabel(const cCodeLabel & in_label)
{
  // cout << "Running FindFullLabel with " << in_label.AsString() <<
  // endl;

  assert(in_label.GetSize() > 0); // Trying to find label of 0 size!

  c4StackHead temp_head(this);

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
bool cHardware4Stack::InjectParasite(double mut_multiplier)
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

  for(int x=0; x<NUM_HEADS; x++)
    {
      GetHead(x).Reset(IP().GetMemSpace(), this);
    }

  for(int x=0; x<NUM_LOCAL_STACKS; x++)
    {
      Stack(x).Clear();
    }
  
  AdvanceIP() = false;
  
  return inject_signal;
}

//This is the code run by the TARGET of an injection.  This RECIEVES the infection.
bool cHardware4Stack::InjectHost(const cCodeLabel & in_label, const cGenome & inject_code)
{
  // Make sure the genome will be below max size after injection.

  // xxxTEMPORARYxxx - we should have this match injection templates.  For now it simply 
  
// FIND THE FIRST EMPTY MEMORY SPACE
  int target_mem_space;
  for (target_mem_space = 0; target_mem_space < NUM_MEMORY_SPACES; target_mem_space++)
    {
      if(isEmpty(target_mem_space))
	{
	  break;
	}
    }
  
  if (target_mem_space == NUM_MEMORY_SPACES)
    {
      return false;
    }

  assert(target_mem_space >=0 && target_mem_space < NUM_MEMORY_SPACES);
  
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
      memory_array[target_mem_space].FlagInjected(i) = true;
    }
    organism->GetPhenotype().IsModified() = true;
    
    // Adjust all of the heads to take into account the new mem size.
    
    cur_thread=GetNumThreads()-1;
    
    for(int i=0; i<cur_thread; i++) {
      for(int j=0; j<NUM_HEADS; j++) {
	if(threads[i].heads[j].GetMemSpace()==target_mem_space)
	  threads[i].heads[j].Jump(inject_code.GetSize());
      }
    }
    
    for (int i=0; i < NUM_HEADS; i++) {    
      GetHead(i).Reset(target_mem_space, this);
    }
    for (int i=0; i < NUM_LOCAL_STACKS; i++) {
      Stack(i).Clear();
    }
  }

  return true; // (inject succeeds!)
}

void cHardware4Stack::Mutate(int mut_point)
{
  // Test if trying to mutate outside of genome...
  assert(mut_point >= 0 && mut_point < GetMemory(0).GetSize());

  GetMemory(0)[mut_point] = GetRandomInst();
  GetMemory(0).FlagMutated(mut_point) = true;
  GetMemory(0).FlagPointMut(mut_point) = true;
  //organism->GetPhenotype().IsMutated() = true;
  organism->CPUStats().mut_stats.point_mut_count++;
}

int cHardware4Stack::PointMutate(const double mut_rate)
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

bool cHardware4Stack::TriggerMutations(int trigger)
{
  // Only update triggers should happen from the outside!
  assert(trigger == MUTATION_TRIGGER_UPDATE);

  // Assume instruction pointer is the intended target (if one is even
  // needed!

  return TriggerMutations(trigger, IP());
}

bool cHardware4Stack::TriggerMutations(int trigger, c4StackHead & cur_head)
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

bool cHardware4Stack::TriggerMutations_ScopeGenome(const cMutation * cur_mut,
          cCPUMemory & target_memory, c4StackHead & cur_head, const double rate)
{
  // The rate we have stored indicates the probability that a single
  // mutation will occur anywhere in the genome.
  
  if (g_random.P(rate) == true) {
    // We must create a temporary head and use it to randomly determine the
    // position in the genome to be mutated.
    c4StackHead tmp_head(cur_head);
    tmp_head.AbsSet(g_random.GetUInt(target_memory.GetSize()));
    TriggerMutations_Body(cur_mut->GetType(), target_memory, tmp_head);
    return true;
  }
  return false;
}

bool cHardware4Stack::TriggerMutations_ScopeLocal(const cMutation * cur_mut,
          cCPUMemory & target_memory, c4StackHead & cur_head, const double rate)
{
  // The rate we have stored is the probability for a mutation at this single
  // position in the genome.

  if (g_random.P(rate) == true) {
    TriggerMutations_Body(cur_mut->GetType(), target_memory, cur_head);
    return true;
  }
  return false;
}

int cHardware4Stack::TriggerMutations_ScopeGlobal(const cMutation * cur_mut,
          cCPUMemory & target_memory, c4StackHead & cur_head, const double rate)
{
  // The probability we have stored is per-site, so we can pull a random
  // number from a binomial distribution to determine the number of mutations
  // that should occur.

  const int num_mut =
    g_random.GetRandBinomial(target_memory.GetSize(), rate);

  if (num_mut > 0) {
    for (int i = 0; i < num_mut; i++) {
      c4StackHead tmp_head(cur_head);
      tmp_head.AbsSet(g_random.GetUInt(target_memory.GetSize()));
      TriggerMutations_Body(cur_mut->GetType(), target_memory, tmp_head);
    }
  }

  return num_mut;
}

void cHardware4Stack::TriggerMutations_Body(int type, cCPUMemory & target_memory,
					 c4StackHead & cur_head)
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

void cHardware4Stack::ReadInst(const int in_inst)
{
  if (inst_set->IsNop( cInstruction(in_inst) )) {
    GetReadLabel().AddNop(in_inst);
  } else {
    GetReadLabel().Clear();
  }
}


void cHardware4Stack::AdjustHeads()
{
  for (int i = 0; i < GetNumThreads(); i++) {
    for (int j = 0; j < NUM_HEADS; j++) {
      threads[i].heads[j].Adjust();
    }
  }
}



// This function looks at the current position in the info of a creature,
// and sets the next_label to be the sequence of nops which follows.  The
// instruction pointer is left on the last line of the label found.

void cHardware4Stack::ReadLabel(int max_size)
{
  int count = 0;
  c4StackHead * inst_ptr = &( IP() );

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


bool cHardware4Stack::ForkThread()
{
  const int num_threads = GetNumThreads();
  if (num_threads == cConfig::GetMaxCPUThreads()) return false;

  // Make room for the new thread.
  threads.Resize(num_threads + 1);

  //IP().Advance();

  // Initialize the new thread to the same values as the current one.
  threads[num_threads] = threads[cur_thread]; 

  // Find the first free bit in thread_id_chart to determine the new
  // thread id.
  int new_id = 0;
  while ( (thread_id_chart >> new_id) & 1 == 1) new_id++;
  threads[num_threads].SetID(new_id);
  thread_id_chart |= (1 << new_id);

  return true;
}


int cHardware4Stack::TestParasite() const
{
  return IP().TestParasite();
}


bool cHardware4Stack::KillThread()
{
  // Make sure that there is always at least one thread...
  if (GetNumThreads() == 1) return false;

  // Note the current thread and set the current back one.
  const int kill_thread = cur_thread;
  PrevThread();
  
  // Turn off this bit in the thread_id_chart...
  thread_id_chart ^= 1 << threads[kill_thread].GetID();

  // Copy the last thread into the kill position
  const int last_thread = GetNumThreads() - 1;
  if (last_thread != kill_thread) {
    threads[kill_thread] = threads[last_thread];
  }

  // Kill the thread!
  threads.Resize(GetNumThreads() - 1);

  if (cur_thread > kill_thread) cur_thread--;

  return true;
}


void cHardware4Stack::SaveState(ostream & fp)
{
  // note, memory & child_memory handled by cpu (@CAO Not any more!)
  assert(fp.good());

  fp<<"cHardware4Stack"<<endl;

  // global_stack (in inverse order so load can just push)
  for(int i=NUM_LOCAL_STACKS; i<NUM_STACKS; i++)
    Stack(i).SaveState(fp);

  //fp << thread_time_used  << endl;
  fp << GetNumThreads()   << endl;
  fp << cur_thread        << endl;

  // Threads
  for( int i = 0; i < GetNumThreads(); i++ ) {
    threads[i].SaveState(fp);
  }
}


void cHardware4Stack::LoadState(istream & fp)
{
  // note, memory & child_memory handled by cpu (@CAO Not any more!)
  assert(fp.good());

  cString foo;
  fp>>foo;
  assert( foo == "cHardware4Stack" );

  // global_stack
  for(int i=NUM_LOCAL_STACKS; i<NUM_STACKS; i++)
    Stack(i).LoadState(fp);

  int num_threads;
  //fp >> thread_time_used;
  fp >> num_threads;
  fp >> cur_thread;

  // Threads
  for( int i = 0; i < num_threads; i++ ){
    threads[i].LoadState(fp);
  }
}


////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int cHardware4Stack::FindModifiedStack(int default_stack)
{
  assert(default_stack < NUM_STACKS);  // Stack ID too high.

  if (GetInstSet().IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_stack = GetInstSet().GetNopMod(IP().GetInst());
    IP().FlagExecuted() = true;
  }
  return default_stack;
}

inline int cHardware4Stack::FindModifiedHead(int default_head)
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

inline int cHardware4Stack::FindComplementStack(int base_stack)
{
  const int comp_stack = base_stack + 2;
  return comp_stack%NUM_STACKS;
}

inline void cHardware4Stack::Fault(int fault_loc, int fault_type, cString fault_desc)
{
  organism->Fault(fault_loc, fault_type, fault_desc);
}

bool cHardware4Stack::Divide_CheckViable(const int parent_size,
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

void cHardware4Stack::Divide_DoMutations(double mut_multiplier)
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

void cHardware4Stack::Inject_DoMutations(double mut_multiplier, cCPUMemory & injected_code)
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
void cHardware4Stack::Divide_TestFitnessMeasures()
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


bool cHardware4Stack::Divide_Main(int mem_space_used, double mut_multiplier)
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
	  //not touch any other threads or memory spaces (ie: parasites)
	  else
	    {
	      for(int x=0; x<NUM_HEADS; x++)
		{
		  GetHead(x).Reset(0, this);
		}
	      for(int x=0; x<NUM_LOCAL_STACKS; x++)
		{
		  Stack(x).Clear();
		}	  
	    }
	}
      AdvanceIP()=false;
    }
     
  return true;
}

cString cHardware4Stack::ConvertToInstruction(int mem_space_used)
{
  char c = mem_space_used + 97;  // 97 - ASCII for 'a'
  cString ret;
  ret += c;
  return ret;
}

cString cHardware4Stack::GetActiveStackID(int stackID) const
{
  if(stackID==STACK_AX)
    return "AX";
  else if(stackID==STACK_BX)
    return "BX";
  else if(stackID==STACK_CX)
    return "CX";
  else if(stackID==STACK_DX)
    return "DX";
  else
    return "";
}
  

//////////////////////////
// And the instructions...
//////////////////////////

//6
bool cHardware4Stack::Inst_ShiftR()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  int value = Stack(stack_used).Pop();
  value >>= 1;
  Stack(stack_used).Push(value);
  return true;
}

//7
bool cHardware4Stack::Inst_ShiftL()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  int value = Stack(stack_used).Pop();
  value <<= 1;
  Stack(stack_used).Push(value);
  return true;
}

//8
bool cHardware4Stack::Inst_Val_Nand()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Push(~(Stack(STACK_BX).Top() & Stack(STACK_CX).Top()));
  return true;
}

//9
bool cHardware4Stack::Inst_Val_Add()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Push(Stack(STACK_BX).Top() + Stack(STACK_CX).Top());
  return true;
}

//10
bool cHardware4Stack::Inst_Val_Sub()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Push(Stack(STACK_BX).Top() - Stack(STACK_CX).Top());
  return true;
}

//11
bool cHardware4Stack::Inst_Val_Mult()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Push(Stack(STACK_BX).Top() * Stack(STACK_CX).Top());
  return true;
}

//12
bool cHardware4Stack::Inst_Val_Div()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  if (Stack(STACK_CX).Top() != 0) {
    if (0-INT_MAX > Stack(STACK_BX).Top() && Stack(STACK_CX).Top() == -1)
      Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: Float exception");
    else
      Stack(stack_used).Push(Stack(STACK_BX).Top() / Stack(STACK_CX).Top());
  } else {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
    return false;
  }
  return true;
}

//13 
bool cHardware4Stack::Inst_SetMemory()   // Allocate maximal more
{
  int mem_space_used = FindModifiedStack(-1);
  
  if(mem_space_used==-1) {
    mem_space_used = FindFirstEmpty();
    if(mem_space_used==-1)
      return false;
  }
  
  GetHead(HEAD_FLOW).Set(0, mem_space_used);
  return true;
  
  //const int cur_size = GetMemory(0).GetSize();
  //const int alloc_size = Min((int) (cConfig::GetChildSizeRange() * cur_size),
  //			     MAX_CREATURE_SIZE - cur_size);
  //if( Allocate_Main(alloc_size) ) {
  //  Stack(STACK_AX).Push(cur_size);
  //  return true;
  //} else return false;
}

//14
bool cHardware4Stack::Inst_Divide()
{
  int mem_space_used = GetHead(HEAD_WRITE).GetMemSpace();
  int mut_multiplier = 1;

  return Divide_Main(mem_space_used, mut_multiplier);
}

bool cHardware4Stack::Inst_HeadDivideMut(double mut_multiplier)
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
bool cHardware4Stack::Inst_HeadRead()
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
  Stack(STACK_AX).Push(read_inst);
  ReadInst(read_inst);

  cpu_stats.mut_stats.copies_exec++;  // @CAO, this too..
  GetHead(head_id).Advance();
  return true;
}

//16
bool cHardware4Stack::Inst_HeadWrite()
{
  const int head_id = FindModifiedHead(HEAD_WRITE);
  c4StackHead & active_head = GetHead(head_id);
  int mem_space_used = active_head.GetMemSpace();
  
  //commented out for right now...
  if(active_head.GetPosition()>=GetMemory(mem_space_used).GetSize()-1)
   {
     GetMemory(mem_space_used).Resize(GetMemory(mem_space_used).GetSize()+1);
     GetMemory(mem_space_used).Copy(GetMemory(mem_space_used).GetSize()-1, GetMemory(mem_space_used).GetSize()-2);
   }

  active_head.Adjust();

  int value = Stack(STACK_AX).Pop();
  if (value < 0 || value >= GetNumInst()) value = 0;

  active_head.SetInst(cInstruction(value));
  active_head.FlagCopied() = true;

  // Advance the head after write...
  active_head++;
  return true;
}

//??
bool cHardware4Stack::Inst_HeadCopy()
{
  // For the moment, this cannot be nop-modified.
  c4StackHead & read_head = GetHead(HEAD_READ);
  c4StackHead & write_head = GetHead(HEAD_WRITE);
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
bool cHardware4Stack::Inst_IfEqual()      // Execute next if bx == ?cx?
{
  const int stack_used = FindModifiedStack(STACK_AX);
  const int stack_used2 = (stack_used+1)%NUM_STACKS;
  if (Stack(stack_used).Top() != Stack(stack_used2).Top())  IP().Advance();
  return true;
}

//18
bool cHardware4Stack::Inst_IfNotEqual()     // Execute next if bx != ?cx?
{
  const int stack_used = FindModifiedStack(STACK_AX);
  const int stack_used2 = (stack_used+1)%NUM_STACKS;
  if (Stack(stack_used).Top() == Stack(stack_used2).Top())  IP().Advance();
  return true;
}

//19
bool cHardware4Stack::Inst_IfLess()       // Execute next if ?bx? < ?cx?
{
  const int stack_used = FindModifiedStack(STACK_AX);
  const int stack_used2 = (stack_used+1)%NUM_STACKS;
  if (Stack(stack_used).Top() >=  Stack(stack_used2).Top())  IP().Advance();
  return true;
}

//20
bool cHardware4Stack::Inst_IfGreater()       // Execute next if bx > ?cx?
{
  const int stack_used = FindModifiedStack(STACK_AX);
  const int stack_used2 = (stack_used+1)%NUM_STACKS;
  if (Stack(stack_used).Top() <= Stack(stack_used2).Top())  IP().Advance();
  return true;
}

//21
bool cHardware4Stack::Inst_HeadPush()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  Stack(STACK_BX).Push(GetHead(head_used).GetPosition());
  //if (head_used == HEAD_IP) {
  //  GetHead(head_used).Set(GetHead(HEAD_FLOW));
  //  AdvanceIP() = false;
  //}
  return true;
}

//22
bool cHardware4Stack::Inst_HeadPop()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  GetHead(head_used).Set(Stack(STACK_BX).Pop(), 
			 GetHead(head_used).GetMemSpace(), this);
  return true;
}

//23 
bool cHardware4Stack::Inst_HeadMove()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  if(head_used != HEAD_FLOW)
    {
      GetHead(head_used).Set(GetHead(HEAD_FLOW));
      if (head_used == HEAD_IP) AdvanceIP() = false;
    }
  else
    {
      threads[cur_thread].heads[HEAD_FLOW]++;
    }
  return true;
}

//24
bool cHardware4Stack::Inst_Search()
{
  ReadLabel();
  GetLabel().Rotate(2, NUM_NOPS_4STACK);
  c4StackHead found_pos = FindLabel(0);
  if(found_pos.GetPosition()-IP().GetPosition()==0)
    {
      GetHead(HEAD_FLOW).Set(IP().GetPosition()+1, IP().GetMemSpace(), this);
      // pushing zero into STACK_AX on a missed search makes it difficult to create
      // a self-replicating organism.  -law
      //Stack(STACK_AX).Push(0);
      Stack(STACK_BX).Push(0);
    }
  else
    {
      int search_size = found_pos.GetPosition() - IP().GetPosition() + GetLabel().GetSize() + 1;
      Stack(STACK_BX).Push(search_size);
      Stack(STACK_AX).Push(GetLabel().GetSize());
      GetHead(HEAD_FLOW).Set(found_pos);
    }  
  
  return true; 
}

//25
bool cHardware4Stack::Inst_PushNext() 
{
  int stack_used = FindModifiedStack(STACK_AX);
  int successor = (stack_used+1)%NUM_STACKS;
  Stack(successor).Push(Stack(stack_used).Pop());
  return true;
}

//26
bool cHardware4Stack::Inst_PushPrevious() 
{
  int stack_used = FindModifiedStack(STACK_BX);
  int predecessor = (stack_used+NUM_STACKS-1)%NUM_STACKS;
  Stack(predecessor).Push(Stack(stack_used).Pop());
  return true;
}

//27
bool cHardware4Stack::Inst_PushComplement() 
{
  int stack_used = FindModifiedStack(STACK_BX);
  int complement = FindComplementStack(stack_used);
  Stack(complement).Push(Stack(stack_used).Pop());
  return true;
}

//28
bool cHardware4Stack::Inst_ValDelete()
{
  int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Pop();
  return true;
}

//29
bool cHardware4Stack::Inst_ValCopy()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  Stack(stack_used).Push(Stack(stack_used).Top());
  return true;
}

//30
bool cHardware4Stack::Inst_ForkThread()
{
  if (!ForkThread()) 
    Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
  else
    IP().Advance();
  return true;
}

//31
bool cHardware4Stack::Inst_IfLabel()
{
  ReadLabel();
  GetLabel().Rotate(2, NUM_NOPS_4STACK);
  if (GetLabel() != GetReadLabel())  IP().Advance();
  return true;
}

//32
bool cHardware4Stack::Inst_Increment()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  int value = Stack(stack_used).Pop();
  Stack(stack_used).Push(++value);
  return true;
}

//33
bool cHardware4Stack::Inst_Decrement()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  int value = Stack(stack_used).Pop();
  Stack(stack_used).Push(--value);
  return true;
}

//34
bool cHardware4Stack::Inst_Mod()
{
  const int stack_used = FindModifiedStack(STACK_BX);
  if (Stack(STACK_CX).Top() != 0) {
    if(Stack(STACK_CX).Top() == -1)
      Stack(stack_used).Push(0);
    else
      Stack(stack_used).Push(Stack(STACK_BX).Top() % Stack(STACK_CX).Top());
  } else {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
  return false;
  }
  return true;
}

//35
bool cHardware4Stack::Inst_KillThread()
{
  if (!KillThread()) Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  else AdvanceIP() = false;
  return true;
}

//36
bool cHardware4Stack::Inst_IO()
{
  const int stack_used = FindModifiedStack(STACK_BX);

  // Do the "put" component
  const int value_out = Stack(stack_used).Top();
  organism->DoOutput(value_out);  // Check for tasks compleated.

  // Do the "get" component
  const int value_in = organism->GetNextInput();
  Stack(stack_used).Push(value_in);
  organism->DoInput(value_in);
  return true;
}

int cHardware4Stack::FindFirstEmpty()
{
  bool OK=true;
  const int current_mem_space = IP().GetMemSpace();

  for(int x=1; x<NUM_MEMORY_SPACES; x++)
    {
      OK=true;
      
      int index = (current_mem_space+x) % NUM_MEMORY_SPACES;

      for(int y=0; y<GetMemory(index).GetSize() && OK; y++)
	{
	  if(GetMemory(index)[y].GetOp() >= NUM_NOPS_4STACK)
	    OK=false; 
	}
      for(int y=0; y<GetNumThreads() && OK; y++)
	{
	  for(int z=0; z<NUM_HEADS; z++)
	    {
	      if(threads[y].heads[z].GetMemSpace() == index)
		OK=false;
	    }
	}
      if(OK)
	return index;
    }
  return -1;
}

bool cHardware4Stack::isEmpty(int mem_space_used)
{
  for(int x=0; x<GetMemory(mem_space_used).GetSize(); x++)
    {
      if(GetMemory(mem_space_used)[x].GetOp() >= NUM_NOPS_4STACK)
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

bool cHardware4Stack::Inst_Inject()
{
  double mut_multiplier = 1;

  return InjectParasite(mut_multiplier);
}



/*
bool cHardware4Stack::Inst_InjectRand()
{
  // Rotate to a random facing and then run the normal inject instruction
  const int num_neighbors = organism->GetNeighborhoodSize();
  organism->Rotate(g_random.GetUInt(num_neighbors));
  Inst_Inject();
  return true;
}

*/
