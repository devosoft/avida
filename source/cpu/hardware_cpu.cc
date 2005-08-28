//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "hardware_cpu.hh"

#include "config.hh"
#include "cpu_test_info.hh"
#include "functions.hh"
#include "genome_util.hh"
#include "genotype.hh"
#include "hardware_tracer.hh"
#include "hardware_tracer_cpu.hh"
#include "inst_lib_cpu.hh"
#include "inst_set.hh"
#include "mutation.hh"
#include "mutation_lib.hh"
#include "mutation_macros.hh"
#include "organism.hh"
#include "phenotype.hh"
#include "string_util.hh"
#include "test_cpu.hh"

#include <limits.h>
#include <fstream>

using namespace std;


///////////////
//  cHardwareCPU
///////////////

const cInstruction cInstLibCPU::inst_error(255);
const cInstruction cInstLibCPU::inst_default(0);
cInstLibCPU *cHardwareCPU::GetInstLib(){ return s_inst_slib; }

cInstLibCPU *cHardwareCPU::s_inst_slib = cHardwareCPU::initInstLib();
cInstLibCPU *cHardwareCPU::initInstLib(void){
  struct cNOPEntryCPU {
    cString name;
    int nop_mod;
    cNOPEntryCPU(const cString &name, int nop_mod)
      : name(name), nop_mod(nop_mod) {}
  };
  static const cNOPEntryCPU s_n_array[] = {
    cNOPEntryCPU("nop-A", REG_AX),
    cNOPEntryCPU("nop-B", REG_BX),
    cNOPEntryCPU("nop-C", REG_CX),
    cNOPEntryCPU("nop-D", REG_DX)
  };

  struct cInstEntryCPU { 
    const cString name;
    const tHardwareCPUMethod function;
    const bool is_default;
    const cString desc;

    cInstEntryCPU(const cString & _name, tHardwareCPUMethod _fun,
		  bool _def=false, const cString & _desc="")
      : name(_name), function(_fun), is_default(_def), desc(_desc) {}
  };
  static const cInstEntryCPU s_f_array[] = {
    /*
    Note: all entries of cNOPEntryCPU s_n_array must have corresponding
    in the same order in cInstEntryCPU s_f_array, and these entries must
    be the first elements of s_f_array.
    */
    cInstEntryCPU("nop-A",     &cHardwareCPU::Inst_Nop, true,
		  "No-operation instruction; modifies other instructions"),
    cInstEntryCPU("nop-B",     &cHardwareCPU::Inst_Nop, true,
		  "No-operation instruction; modifies other instructions"),
    cInstEntryCPU("nop-C",     &cHardwareCPU::Inst_Nop, true,
		  "No-operation instruction; modifies other instructions"),
    cInstEntryCPU("nop-D",     &cHardwareCPU::Inst_Nop, true,
		  "No-operation instruction; modifies other instructions"),

    cInstEntryCPU("NULL",      &cHardwareCPU::Inst_Nop, false,
		  "True no-operation instruction: does nothing"),
    cInstEntryCPU("nop-X",     &cHardwareCPU::Inst_Nop, false,
		  "True no-operation instruction: does nothing"),
    cInstEntryCPU("if-equ-0",  &cHardwareCPU::Inst_If0, false,
		  "Execute next instruction if ?BX?==0, else skip it"),
    cInstEntryCPU("if-not-0",  &cHardwareCPU::Inst_IfNot0, false,
		  "Execute next instruction if ?BX?!=0, else skip it"),
    cInstEntryCPU("if-n-equ",  &cHardwareCPU::Inst_IfNEqu, true,
		  "Execute next instruction if ?BX?!=?CX?, else skip it"),
    cInstEntryCPU("if-equ",    &cHardwareCPU::Inst_IfEqu, false,
		  "Execute next instruction if ?BX?==?CX?, else skip it"),
    cInstEntryCPU("if-grt-0",  &cHardwareCPU::Inst_IfGr0),
    cInstEntryCPU("if-grt",    &cHardwareCPU::Inst_IfGr),
    cInstEntryCPU("if->=-0",   &cHardwareCPU::Inst_IfGrEqu0),
    cInstEntryCPU("if->=",     &cHardwareCPU::Inst_IfGrEqu),
    cInstEntryCPU("if-les-0",  &cHardwareCPU::Inst_IfLess0),
    cInstEntryCPU("if-less",   &cHardwareCPU::Inst_IfLess, true,
		  "Execute next instruction if ?BX? < ?CX?, else skip it"),
    cInstEntryCPU("if-<=-0",   &cHardwareCPU::Inst_IfLsEqu0),
    cInstEntryCPU("if-<=",     &cHardwareCPU::Inst_IfLsEqu),
    cInstEntryCPU("if-A!=B",   &cHardwareCPU::Inst_IfANotEqB),
    cInstEntryCPU("if-B!=C",   &cHardwareCPU::Inst_IfBNotEqC),
    cInstEntryCPU("if-A!=C",   &cHardwareCPU::Inst_IfANotEqC),
    cInstEntryCPU("if-bit-1",  &cHardwareCPU::Inst_IfBit1),

    cInstEntryCPU("jump-f",    &cHardwareCPU::Inst_JumpF),
    cInstEntryCPU("jump-b",    &cHardwareCPU::Inst_JumpB),
    cInstEntryCPU("jump-p",    &cHardwareCPU::Inst_JumpP),
    cInstEntryCPU("jump-slf",  &cHardwareCPU::Inst_JumpSelf),
    cInstEntryCPU("call",      &cHardwareCPU::Inst_Call),
    cInstEntryCPU("return",    &cHardwareCPU::Inst_Return),

    cInstEntryCPU("pop",       &cHardwareCPU::Inst_Pop, true,
		  "Remove top number from stack and place into ?BX?"),
    cInstEntryCPU("push",      &cHardwareCPU::Inst_Push, true,
		  "Copy number from ?BX? and place it into the stack"),
    cInstEntryCPU("swap-stk",  &cHardwareCPU::Inst_SwitchStack, true,
		  "Toggle which stack is currently being used"),
    cInstEntryCPU("flip-stk",  &cHardwareCPU::Inst_FlipStack),
    cInstEntryCPU("swap",      &cHardwareCPU::Inst_Swap, true,
		  "Swap the contents of ?BX? with ?CX?"),
    cInstEntryCPU("swap-AB",   &cHardwareCPU::Inst_SwapAB),
    cInstEntryCPU("swap-BC",   &cHardwareCPU::Inst_SwapBC),
    cInstEntryCPU("swap-AC",   &cHardwareCPU::Inst_SwapAC),
    cInstEntryCPU("copy-reg",  &cHardwareCPU::Inst_CopyReg),
    cInstEntryCPU("set_A=B",   &cHardwareCPU::Inst_CopyRegAB),
    cInstEntryCPU("set_A=C",   &cHardwareCPU::Inst_CopyRegAC),
    cInstEntryCPU("set_B=A",   &cHardwareCPU::Inst_CopyRegBA),
    cInstEntryCPU("set_B=C",   &cHardwareCPU::Inst_CopyRegBC),
    cInstEntryCPU("set_C=A",   &cHardwareCPU::Inst_CopyRegCA),
    cInstEntryCPU("set_C=B",   &cHardwareCPU::Inst_CopyRegCB),
    cInstEntryCPU("reset",     &cHardwareCPU::Inst_Reset),

    cInstEntryCPU("pop-A",     &cHardwareCPU::Inst_PopA),
    cInstEntryCPU("pop-B",     &cHardwareCPU::Inst_PopB),
    cInstEntryCPU("pop-C",     &cHardwareCPU::Inst_PopC),
    cInstEntryCPU("push-A",    &cHardwareCPU::Inst_PushA),
    cInstEntryCPU("push-B",    &cHardwareCPU::Inst_PushB),
    cInstEntryCPU("push-C",    &cHardwareCPU::Inst_PushC),

    cInstEntryCPU("shift-r",   &cHardwareCPU::Inst_ShiftR, true,
		  "Shift bits in ?BX? right by one (divide by two)"),
    cInstEntryCPU("shift-l",   &cHardwareCPU::Inst_ShiftL, true,
		  "Shift bits in ?BX? left by one (multiply by two)"),
    cInstEntryCPU("bit-1",     &cHardwareCPU::Inst_Bit1),
    cInstEntryCPU("set-num",   &cHardwareCPU::Inst_SetNum),
    cInstEntryCPU("val-grey",  &cHardwareCPU::Inst_ValGrey),
    cInstEntryCPU("val-dir",   &cHardwareCPU::Inst_ValDir),
    cInstEntryCPU("val-add-p", &cHardwareCPU::Inst_ValAddP),
    cInstEntryCPU("val-fib",   &cHardwareCPU::Inst_ValFib),
    cInstEntryCPU("val-poly-c",&cHardwareCPU::Inst_ValPolyC),
    cInstEntryCPU("inc",       &cHardwareCPU::Inst_Inc, true,
		  "Increment ?BX? by one"),
    cInstEntryCPU("dec",       &cHardwareCPU::Inst_Dec, true,
		  "Decrement ?BX? by one"),
    cInstEntryCPU("zero",      &cHardwareCPU::Inst_Zero, false,
		  "Set ?BX? to zero"),
    cInstEntryCPU("neg",       &cHardwareCPU::Inst_Neg),
    cInstEntryCPU("square",    &cHardwareCPU::Inst_Square),
    cInstEntryCPU("sqrt",      &cHardwareCPU::Inst_Sqrt),
    cInstEntryCPU("not",       &cHardwareCPU::Inst_Not),
    cInstEntryCPU("minus-17",  &cHardwareCPU::Inst_Minus17),
    
    cInstEntryCPU("add",       &cHardwareCPU::Inst_Add, true,
		  "Add BX to CX and place the result in ?BX?"),
    cInstEntryCPU("sub",       &cHardwareCPU::Inst_Sub, true,
		  "Subtract CX from BX and place the result in ?BX?"),
    cInstEntryCPU("mult",      &cHardwareCPU::Inst_Mult, false,
		  "Multiple BX by CX and place the result in ?BX?"),
    cInstEntryCPU("div",       &cHardwareCPU::Inst_Div, false,
		  "Divide BX by CX and place the result in ?BX?"),
    cInstEntryCPU("mod",       &cHardwareCPU::Inst_Mod),
    cInstEntryCPU("nand",      &cHardwareCPU::Inst_Nand, true,
		  "Nand BX by CX and place the result in ?BX?"),
    cInstEntryCPU("nor",       &cHardwareCPU::Inst_Nor),
    cInstEntryCPU("and",       &cHardwareCPU::Inst_And),
    cInstEntryCPU("order",     &cHardwareCPU::Inst_Order),
    cInstEntryCPU("xor",       &cHardwareCPU::Inst_Xor),
    
    cInstEntryCPU("copy",      &cHardwareCPU::Inst_Copy),
    cInstEntryCPU("read",      &cHardwareCPU::Inst_ReadInst),
    cInstEntryCPU("write",     &cHardwareCPU::Inst_WriteInst),
    cInstEntryCPU("stk-read",  &cHardwareCPU::Inst_StackReadInst),
    cInstEntryCPU("stk-writ",  &cHardwareCPU::Inst_StackWriteInst),
    
    cInstEntryCPU("compare",   &cHardwareCPU::Inst_Compare),
    cInstEntryCPU("if-n-cpy",  &cHardwareCPU::Inst_IfNCpy),
    cInstEntryCPU("allocate",  &cHardwareCPU::Inst_Allocate),
    cInstEntryCPU("divide",    &cHardwareCPU::Inst_Divide),
    cInstEntryCPU("c-alloc",   &cHardwareCPU::Inst_CAlloc),
    cInstEntryCPU("c-divide",  &cHardwareCPU::Inst_CDivide),
    cInstEntryCPU("inject",    &cHardwareCPU::Inst_Inject),
    cInstEntryCPU("inject-r",  &cHardwareCPU::Inst_InjectRand),
    cInstEntryCPU("search-f",  &cHardwareCPU::Inst_SearchF),
    cInstEntryCPU("search-b",  &cHardwareCPU::Inst_SearchB),
    cInstEntryCPU("mem-size",  &cHardwareCPU::Inst_MemSize),

    cInstEntryCPU("get",       &cHardwareCPU::Inst_TaskGet),
    cInstEntryCPU("stk-get",   &cHardwareCPU::Inst_TaskStackGet),
    cInstEntryCPU("stk-load",  &cHardwareCPU::Inst_TaskStackLoad),
    cInstEntryCPU("put",       &cHardwareCPU::Inst_TaskPut),
    cInstEntryCPU("IO",        &cHardwareCPU::Inst_TaskIO, true,
		  "Output ?BX?, and input new number back into ?BX?"),

    cInstEntryCPU("send",      &cHardwareCPU::Inst_Send),
    cInstEntryCPU("receive",   &cHardwareCPU::Inst_Receive),
    cInstEntryCPU("sense",     &cHardwareCPU::Inst_Sense),

    cInstEntryCPU("donate-rnd",  &cHardwareCPU::Inst_DonateRandom),
    cInstEntryCPU("donate-kin",  &cHardwareCPU::Inst_DonateKin),
    cInstEntryCPU("donate-edt",  &cHardwareCPU::Inst_DonateEditDist),
    cInstEntryCPU("donate-NUL",  &cHardwareCPU::Inst_DonateNULL),

    cInstEntryCPU("rotate-l",  &cHardwareCPU::Inst_RotateL),
    cInstEntryCPU("rotate-r",  &cHardwareCPU::Inst_RotateR),

    cInstEntryCPU("set-cmut",  &cHardwareCPU::Inst_SetCopyMut),
    cInstEntryCPU("mod-cmut",  &cHardwareCPU::Inst_ModCopyMut),

    // Threading instructions
    cInstEntryCPU("fork-th",   &cHardwareCPU::Inst_ForkThread),
    cInstEntryCPU("kill-th",   &cHardwareCPU::Inst_KillThread),
    cInstEntryCPU("id-th",     &cHardwareCPU::Inst_ThreadID),

    // Head-based instructions
    cInstEntryCPU("h-alloc",   &cHardwareCPU::Inst_MaxAlloc, true,
		  "Allocate maximum allowed space"),
    cInstEntryCPU("h-divide",  &cHardwareCPU::Inst_HeadDivide, true,
		  "Divide code between read and write heads."),
    cInstEntryCPU("h-read",    &cHardwareCPU::Inst_HeadRead),
    cInstEntryCPU("h-write",   &cHardwareCPU::Inst_HeadWrite),
    cInstEntryCPU("h-copy",    &cHardwareCPU::Inst_HeadCopy, true,
		  "Copy from read-head to write-head; advance both"),
    cInstEntryCPU("h-search",  &cHardwareCPU::Inst_HeadSearch, true,
		  "Find complement template and make with flow head"),
    cInstEntryCPU("h-push",    &cHardwareCPU::Inst_HeadPush),
    cInstEntryCPU("h-pop",     &cHardwareCPU::Inst_HeadPop),
    cInstEntryCPU("set-head",  &cHardwareCPU::Inst_SetHead),
    cInstEntryCPU("adv-head",  &cHardwareCPU::Inst_AdvanceHead),
    cInstEntryCPU("mov-head",  &cHardwareCPU::Inst_MoveHead, true,
		  "Move head ?IP? to the flow head"),
    cInstEntryCPU("jmp-head",  &cHardwareCPU::Inst_JumpHead, true,
		  "Move head ?IP? by amount in CX register; CX = old pos."),
    cInstEntryCPU("get-head",  &cHardwareCPU::Inst_GetHead, true,
		  "Copy the position of the ?IP? head into CX"),
    cInstEntryCPU("if-label",  &cHardwareCPU::Inst_IfLabel, true,
		  "Execute next if we copied complement of attached label"),
    cInstEntryCPU("if-label2",  &cHardwareCPU::Inst_IfLabel2, true,
		  "If copied label compl., exec next inst; else SKIP W/NOPS"),
    cInstEntryCPU("set-flow",  &cHardwareCPU::Inst_SetFlow, true,
		  "Set flow-head to position in ?CX?"),

    cInstEntryCPU("h-copy2",    &cHardwareCPU::Inst_HeadCopy2),
    cInstEntryCPU("h-copy3",    &cHardwareCPU::Inst_HeadCopy3),
    cInstEntryCPU("h-copy4",    &cHardwareCPU::Inst_HeadCopy4),
    cInstEntryCPU("h-copy5",    &cHardwareCPU::Inst_HeadCopy5),
    cInstEntryCPU("h-copy6",    &cHardwareCPU::Inst_HeadCopy6),
    cInstEntryCPU("h-copy7",    &cHardwareCPU::Inst_HeadCopy7),
    cInstEntryCPU("h-copy8",    &cHardwareCPU::Inst_HeadCopy8),
    cInstEntryCPU("h-copy9",    &cHardwareCPU::Inst_HeadCopy9),
    cInstEntryCPU("h-copy10",   &cHardwareCPU::Inst_HeadCopy10),

    cInstEntryCPU("divide-sex",    &cHardwareCPU::Inst_HeadDivideSex),
    cInstEntryCPU("divide-asex",   &cHardwareCPU::Inst_HeadDivideAsex),

    cInstEntryCPU("div-sex",    &cHardwareCPU::Inst_HeadDivideSex),
    cInstEntryCPU("div-asex",   &cHardwareCPU::Inst_HeadDivideAsex),
    cInstEntryCPU("div-asex-w",   &cHardwareCPU::Inst_HeadDivideAsexWait),
    cInstEntryCPU("div-sex-MS",   &cHardwareCPU::Inst_HeadDivideMateSelect),

    cInstEntryCPU("h-divide1",      &cHardwareCPU::Inst_HeadDivide1),
    cInstEntryCPU("h-divide2",      &cHardwareCPU::Inst_HeadDivide2),
    cInstEntryCPU("h-divide3",      &cHardwareCPU::Inst_HeadDivide3),
    cInstEntryCPU("h-divide4",      &cHardwareCPU::Inst_HeadDivide4),
    cInstEntryCPU("h-divide5",      &cHardwareCPU::Inst_HeadDivide5),
    cInstEntryCPU("h-divide6",      &cHardwareCPU::Inst_HeadDivide6),
    cInstEntryCPU("h-divide7",      &cHardwareCPU::Inst_HeadDivide7),
    cInstEntryCPU("h-divide8",      &cHardwareCPU::Inst_HeadDivide8),
    cInstEntryCPU("h-divide9",      &cHardwareCPU::Inst_HeadDivide9),
    cInstEntryCPU("h-divide10",     &cHardwareCPU::Inst_HeadDivide10),
    cInstEntryCPU("h-divide16",     &cHardwareCPU::Inst_HeadDivide16),
    cInstEntryCPU("h-divide32",     &cHardwareCPU::Inst_HeadDivide32),
    cInstEntryCPU("h-divide50",     &cHardwareCPU::Inst_HeadDivide50),
    cInstEntryCPU("h-divide100",    &cHardwareCPU::Inst_HeadDivide100),
    cInstEntryCPU("h-divide500",    &cHardwareCPU::Inst_HeadDivide500),
    cInstEntryCPU("h-divide1000",   &cHardwareCPU::Inst_HeadDivide1000),
    cInstEntryCPU("h-divide5000",   &cHardwareCPU::Inst_HeadDivide5000),
    cInstEntryCPU("h-divide10000",  &cHardwareCPU::Inst_HeadDivide10000),
    cInstEntryCPU("h-divide50000",  &cHardwareCPU::Inst_HeadDivide50000),
    cInstEntryCPU("h-divide0.5",    &cHardwareCPU::Inst_HeadDivide0_5),
    cInstEntryCPU("h-divide0.1",    &cHardwareCPU::Inst_HeadDivide0_1),
    cInstEntryCPU("h-divide0.05",   &cHardwareCPU::Inst_HeadDivide0_05),
    cInstEntryCPU("h-divide0.01",   &cHardwareCPU::Inst_HeadDivide0_01),
    cInstEntryCPU("h-divide0.001",  &cHardwareCPU::Inst_HeadDivide0_001),

    // High-level instructions
    cInstEntryCPU("repro",      &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-A",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-B",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-C",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-D",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-E",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-F",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-G",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-H",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-I",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-J",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-K",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-L",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-M",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-N",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-O",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-P",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-Q",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-R",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-S",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-T",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-U",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-V",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-W",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-X",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-Y",    &cHardwareCPU::Inst_Repro),
    cInstEntryCPU("repro-Z",    &cHardwareCPU::Inst_Repro),

    // Suicide
    cInstEntryCPU("kazi",	&cHardwareCPU::Inst_Kazi),
    cInstEntryCPU("die",	&cHardwareCPU::Inst_Die),



    // Placebo instructions
    // nop-x (included with nops)
    cInstEntryCPU("skip",      &cHardwareCPU::Inst_Skip)
  };

  const int n_size = sizeof(s_n_array)/sizeof(cNOPEntryCPU);

  static cString n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size; i++){
    n_names[i] = s_n_array[i].name;
    nop_mods[i] = s_n_array[i].nop_mod;
  }

  const int f_size = sizeof(s_f_array)/sizeof(cInstEntryCPU);
  static cString f_names[f_size];
  static tHardwareCPUMethod functions[f_size];
  for (int i = 0; i < f_size; i++){
    f_names[i] = s_f_array[i].name;
    functions[i] = s_f_array[i].function;
  }

  cInstLibCPU *inst_lib = new cInstLibCPU(
    n_size,
    f_size,
    n_names,
    f_names,
    nop_mods,
    functions
  );

  return inst_lib;
}

cHardwareCPU::cHardwareCPU(cOrganism * in_organism, cInstSet * in_inst_set)
  : cHardwareBase(in_organism, in_inst_set)
{
  /* FIXME:  reorganize storage of m_functions.  -- kgn */
  m_functions = s_inst_slib->GetFunctions();
  /**/
  memory = in_organism->GetGenome();  // Initialize memory...
  Reset();                            // Setup the rest of the hardware...
}


cHardwareCPU::cHardwareCPU(const cHardwareCPU &hardware_cpu)
: cHardwareBase(hardware_cpu.organism, hardware_cpu.inst_set)
, m_functions(hardware_cpu.m_functions)
, memory(hardware_cpu.memory)
, global_stack(hardware_cpu.global_stack)
, thread_time_used(hardware_cpu.thread_time_used)
, threads(hardware_cpu.threads)
, thread_id_chart(hardware_cpu.thread_id_chart)
, cur_thread(hardware_cpu.cur_thread)
, mal_active(hardware_cpu.mal_active)
, advance_ip(hardware_cpu.advance_ip)
#ifdef INSTRUCTION_COSTS
, inst_cost(hardware_cpu.inst_cost)
, inst_ft_cost(hardware_cpu.inst_ft_cost)
#endif
{
}


cHardwareCPU::~cHardwareCPU()
{
}


void cHardwareCPU::Recycle(cOrganism * new_organism, cInstSet * in_inst_set)
{
  cHardwareBase::Recycle(new_organism, in_inst_set);
  memory = new_organism->GetGenome();
  Reset();
}


void cHardwareCPU::Reset()
{
  global_stack.Clear();
  thread_time_used = 0;

  // We want to reset to have a single thread.
  threads.Resize(1);

  // Reset that single thread.
  threads[0].Reset(this, 0);
  thread_id_chart = 1; // Mark only the first thread as taken...
  cur_thread = 0;

  mal_active = false;

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

void cHardwareCPU::SingleProcess()
{
  // Mark this organism as running...
  organism->SetRunning(true);

  cPhenotype & phenotype = organism->GetPhenotype();
  phenotype.IncTimeUsed();
  const int num_threads = GetNumThreads();
  if (num_threads > 1) thread_time_used++;

  // If we have threads turned on and we executed each thread in a single
  // timestep, adjust the number of instructions executed accordingly.
  const int num_inst_exec = (cConfig::GetThreadSlicingMethod() == 1) ?
    num_threads : 1;
  
  for (int i = 0; i < num_inst_exec; i++) {
    // Setup the hardware for the next instruction to be executed.
    NextThread();
    advance_ip = true;
    IP().Adjust();

#ifdef BREAKPOINTS
    if (IP().FlagBreakpoint() == true) {
      organism->DoBreakpoint();
    }
#endif
    
    // Print the status of this CPU at each step...
    if (m_tracer != NULL) {
      if (cHardwareTracer_CPU * tracer
          = dynamic_cast<cHardwareTracer_CPU *>(m_tracer)
      ){
        tracer->TraceHardware_CPU(*this);
      }
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
      if (advance_ip == true) IP().Advance();
    } // if exec
    
  } // Previous was executed once for each thread...

  // Kill creatures who have reached their max num of instructions executed
  const int max_executed = organism->GetMaxExecuted();
  if ((max_executed > 0 && phenotype.GetTimeUsed() >= max_executed)
      || phenotype.GetToDie() == true) {
    organism->Die();
  }

  organism->SetRunning(false);
}


// This method will test to see if all costs have been paid associated
// with executing an instruction and only return true when that instruction
// should proceed.
bool cHardwareCPU::SingleProcess_PayCosts(const cInstruction & cur_inst)
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
bool cHardwareCPU::SingleProcess_ExecuteInst(const cInstruction & cur_inst) 
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


void cHardwareCPU::ProcessBonusInst(const cInstruction & inst)
{
  // Mark this organism as running...
  bool prev_run_state = organism->GetIsRunning();
  organism->SetRunning(true);

  // @CAO FIX PRINTING TO INDICATE THIS IS A BONUS
  // Print the status of this CPU at each step...
  if (m_tracer != NULL) {
    if (cHardwareTracer_CPU * tracer
        = dynamic_cast<cHardwareTracer_CPU *>(m_tracer)
    ){
      tracer->TraceHardware_CPUBonus(*this);
    }
  }
    
  SingleProcess_ExecuteInst(inst);

  organism->SetRunning(prev_run_state);
}


void cHardwareCPU::LoadGenome(const cGenome & new_genome)
{
  GetMemory() = new_genome;
}


bool cHardwareCPU::OK()
{
  bool result = true;

  if (!memory.OK()) result = false;

  for (int i = 0; i < GetNumThreads(); i++) {
    if (threads[i].stack.OK() == false) result = false;
    if (threads[i].next_label.OK() == false) result = false;
  }

  return result;
}

void cHardwareCPU::PrintStatus(ostream & fp)
{
  fp << organism->GetPhenotype().GetTimeUsed() << " "
     << "IP:" << IP().GetPosition() << "    "

     << "AX:" << Register(REG_AX) << " "
     << setbase(16) << "[0x" << Register(REG_AX) << "]  " << setbase(10)

     << "BX:" << Register(REG_BX) << " "
     << setbase(16) << "[0x" << Register(REG_BX) << "]  " << setbase(10)

     << "CX:" << Register(REG_CX) << " "
     << setbase(16) << "[0x" << Register(REG_CX) << "]" << setbase(10)

     << endl;

  fp << "  R-Head:" << GetHead(HEAD_READ).GetPosition() << " "
     << "W-Head:" << GetHead(HEAD_WRITE).GetPosition()  << " "
     << "F-Head:" << GetHead(HEAD_FLOW).GetPosition()   << "  "
     << "RL:" << GetReadLabel().AsString() << "   "
     << endl;

  fp << "  Mem (" << GetMemory().GetSize() << "):"
		  << "  " << GetMemory().AsString()
		  << endl;
  fp.flush();
}





/////////////////////////////////////////////////////////////////////////
// Method: cHardwareCPU::FindLabel(direction)
//
// Search in 'direction' (+ or - 1) from the instruction pointer for the
// compliment of the label in 'next_label' and return a pointer to the
// results.  If direction is 0, search from the beginning of the genome.
//
/////////////////////////////////////////////////////////////////////////

cHeadCPU cHardwareCPU::FindLabel(int direction)
{
  cHeadCPU & inst_ptr = IP();

  // Start up a search head at the position of the instruction pointer.
  cHeadCPU search_head(inst_ptr);
  cCodeLabel & search_label = GetLabel();

  // Make sure the label is of size > 0.

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
  if (found_pos >= 0) search_head.Set(found_pos - 1);

  // Return the found position (still at start point if not found).
  return search_head;
}


// Search forwards for search_label from _after_ position pos in the
// memory.  Return the first line _after_ the the found label.  It is okay
// to find search label's match inside another label.

int cHardwareCPU::FindLabel_Forward(const cCodeLabel & search_label,
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

int cHardwareCPU::FindLabel_Backward(const cCodeLabel & search_label,
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
cHeadCPU cHardwareCPU::FindLabel(const cCodeLabel & in_label, int direction)
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
cHeadCPU cHardwareCPU::FindFullLabel(const cCodeLabel & in_label)
{
  // cout << "Running FindFullLabel with " << in_label.AsString() <<
  // endl;

  assert(in_label.GetSize() > 0); // Trying to find label of 0 size!

  cHeadCPU temp_head(this);

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


bool cHardwareCPU::InjectHost(const cCodeLabel & in_label, const cGenome & injection)
{
  // Make sure the genome will be below max size after injection.

  const int new_size = injection.GetSize() + GetMemory().GetSize();
  if (new_size > MAX_CREATURE_SIZE) return false; // (inject fails)

  const int inject_line = FindFullLabel(in_label).GetPosition();

  // Abort if no compliment is found.
  if (inject_line == -1) return false; // (inject fails)

  // Inject the code!
  InjectCode(injection, inject_line+1);

  return true; // (inject succeeds!)
}

int cHardwareCPU::InjectThread(const cCodeLabel & in_label, const cGenome & injection)
{
  // Make sure the genome will be below max size after injection.

  const int new_size = injection.GetSize() + GetMemory().GetSize();
  if (new_size > MAX_CREATURE_SIZE) return 1; // (inject fails)

  const int inject_line = FindFullLabel(in_label).GetPosition();

  // Abort if no compliment is found.
  if (inject_line == -1) return 2; // (inject fails)

  // Inject the code!
  InjectCodeThread(injection, inject_line+1);

  return 0; // (inject succeeds!)
}

void cHardwareCPU::InjectCode(const cGenome & inject_code, const int line_num)
{
  assert(line_num >= 0);
  assert(line_num <= memory.GetSize());
  assert(memory.GetSize() + inject_code.GetSize() < MAX_CREATURE_SIZE);

  // Inject the new code.
  const int inject_size = inject_code.GetSize();
  memory.Insert(line_num, inject_code);
  
  // Set instruction flags on the injected code
  for (int i = line_num; i < line_num + inject_size; i++) {
    memory.FlagInjected(i) = true;
  }
  organism->GetPhenotype().IsModified() = true;

  // Adjust all of the heads to take into account the new mem size.

  for (int i=0; i < NUM_HEADS; i++) {    
    if (!GetHead(i).TestParasite() &&
	GetHead(i).GetPosition() > line_num)
      GetHead(i).Jump(inject_size);
  }
}

void cHardwareCPU::InjectCodeThread(const cGenome & inject_code, const int line_num)
{
  assert(line_num >= 0);
  assert(line_num <= memory.GetSize());
  assert(memory.GetSize() + inject_code.GetSize() < MAX_CREATURE_SIZE);
  
  if(ForkThread())
    {
      // Inject the new code.
      const int inject_size = inject_code.GetSize();
      memory.Insert(line_num, inject_code);
      
      // Set instruction flags on the injected code
      for (int i = line_num; i < line_num + inject_size; i++) {
	memory.FlagInjected(i) = true;
      }
      organism->GetPhenotype().IsModified() = true;
      organism->GetPhenotype().IsMultiThread() = true;
      
      // Adjust all of the heads to take into account the new mem size.
      
      int currthread = GetCurThread();
      SetThread(0);
      for (int i=0; i<GetNumThreads()-2; i++)
	{
	  for (int j=0; j < NUM_HEADS; j++) 
	    {    
	      if (!GetHead(i).TestParasite() && GetHead(i).GetPosition() > line_num)
		GetHead(i).Jump(inject_size);
	    }
	  NextThread();
	}
      SetThread(currthread);
          
    }
  else
    {
      //Some kind of error message should go here...but what?
    }

}

void cHardwareCPU::Mutate(int mut_point)
{
  // Test if trying to mutate outside of genome...
  assert(mut_point >= 0 && mut_point < GetMemory().GetSize());

  GetMemory()[mut_point] = GetRandomInst();
  GetMemory().FlagMutated(mut_point) = true;
  GetMemory().FlagPointMut(mut_point) = true;
  //organism->GetPhenotype().IsMutated() = true;
  organism->CPUStats().mut_stats.point_mut_count++;
}

int cHardwareCPU::PointMutate(const double mut_rate)
{
  const int num_muts =
    g_random.GetRandBinomial(GetMemory().GetSize(), mut_rate);

  for (int i = 0; i < num_muts; i++) {
    const int pos = g_random.GetUInt(GetMemory().GetSize());
    Mutate(pos);
  }

  return num_muts;
}


// Trigger mutations of a specific type.  Outside triggers cannot specify
// a head since hardware types are not known.

bool cHardwareCPU::TriggerMutations(int trigger)
{
  // Only update triggers should happen from the outside!
  assert(trigger == MUTATION_TRIGGER_UPDATE);

  // Assume instruction pointer is the intended target (if one is even
  // needed!

  return TriggerMutations(trigger, IP());
}

bool cHardwareCPU::TriggerMutations(int trigger, cHeadCPU & cur_head)
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
    ? organism->ChildGenome() : GetMemory();

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

bool cHardwareCPU::TriggerMutations_ScopeGenome(const cMutation * cur_mut,
          cCPUMemory & target_memory, cHeadCPU & cur_head, const double rate)
{
  // The rate we have stored indicates the probability that a single
  // mutation will occur anywhere in the genome.
  
  if (g_random.P(rate) == true) {
    // We must create a temporary head and use it to randomly determine the
    // position in the genome to be mutated.
    cHeadCPU tmp_head(cur_head);
    tmp_head.AbsSet(g_random.GetUInt(target_memory.GetSize()));
    TriggerMutations_Body(cur_mut->GetType(), target_memory, tmp_head);
    return true;
  }
  return false;
}

bool cHardwareCPU::TriggerMutations_ScopeLocal(const cMutation * cur_mut,
          cCPUMemory & target_memory, cHeadCPU & cur_head, const double rate)
{
  // The rate we have stored is the probability for a mutation at this single
  // position in the genome.

  if (g_random.P(rate) == true) {
    TriggerMutations_Body(cur_mut->GetType(), target_memory, cur_head);
    return true;
  }
  return false;
}

int cHardwareCPU::TriggerMutations_ScopeGlobal(const cMutation * cur_mut,
          cCPUMemory & target_memory, cHeadCPU & cur_head, const double rate)
{
  // The probability we have stored is per-site, so we can pull a random
  // number from a binomial distribution to determine the number of mutations
  // that should occur.

  const int num_mut =
    g_random.GetRandBinomial(target_memory.GetSize(), rate);

  if (num_mut > 0) {
    for (int i = 0; i < num_mut; i++) {
      cHeadCPU tmp_head(cur_head);
      tmp_head.AbsSet(g_random.GetUInt(target_memory.GetSize()));
      TriggerMutations_Body(cur_mut->GetType(), target_memory, tmp_head);
    }
  }

  return num_mut;
}

void cHardwareCPU::TriggerMutations_Body(int type, cCPUMemory & target_memory,
					 cHeadCPU & cur_head)
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

void cHardwareCPU::ReadInst(const int in_inst)
{
  if (inst_set->IsNop( cInstruction(in_inst) )) {
    GetReadLabel().AddNop(in_inst);
  } else {
    GetReadLabel().Clear();
  }
}


void cHardwareCPU::AdjustHeads()
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

void cHardwareCPU::ReadLabel(int max_size)
{
  int count = 0;
  cHeadCPU * inst_ptr = &( IP() );

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


bool cHardwareCPU::ForkThread()
{
  const int num_threads = GetNumThreads();
  if (num_threads == cConfig::GetMaxCPUThreads()) return false;

  // Make room for the new thread.
  threads.Resize(num_threads + 1);

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


int cHardwareCPU::TestParasite() const
{
  return IP().TestParasite();
}


bool cHardwareCPU::KillThread()
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


void cHardwareCPU::SaveState(ostream & fp)
{
  // note, memory & child_memory handled by cpu (@CAO Not any more!)
  assert(fp.good());

  fp<<"cHardwareCPU"<<endl;

  // global_stack (in inverse order so load can just push)
  global_stack.SaveState(fp);

  fp << thread_time_used  << endl;
  fp << GetNumThreads()   << endl;
  fp << cur_thread        << endl;

  // Threads
  for( int i = 0; i < GetNumThreads(); i++ ) {
    threads[i].SaveState(fp);
  }
}


void cHardwareCPU::LoadState(istream & fp)
{
  // note, memory & child_memory handled by cpu (@CAO Not any more!)
  assert(fp.good());

  cString foo;
  fp>>foo;
  assert( foo == "cHardwareCPU" );

  // global_stack
  global_stack.LoadState(fp);

  int num_threads;
  fp >> thread_time_used;
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

inline int cHardwareCPU::FindModifiedRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.

  if (GetInstSet().IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_register = GetInstSet().GetNopMod(IP().GetInst());
    IP().FlagExecuted() = true;
  }
  return default_register;
}


inline int cHardwareCPU::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.

  if (GetInstSet().IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_head = GetInstSet().GetNopMod(IP().GetInst());
    IP().FlagExecuted() = true;
  }
  return default_head;
}


inline int cHardwareCPU::FindComplementRegister(int base_reg)
{
  const int comp_reg = base_reg + 1;
  return (comp_reg  == NUM_REGISTERS) ? 0 : comp_reg;
}


inline void cHardwareCPU::Fault(int fault_loc, int fault_type, cString fault_desc)
{
  organism->Fault(fault_loc, fault_type, fault_desc);
}


bool cHardwareCPU::Allocate_Necro(const int new_size)
{
  GetMemory().ResizeOld(new_size);
  return true;
}

bool cHardwareCPU::Allocate_Random(const int old_size, const int new_size)
{
  GetMemory().Resize(new_size);

  for (int i = old_size; i < new_size; i++) {
    GetMemory()[i] = GetInstSet().GetRandomInst();
  }
  return true;
}

bool cHardwareCPU::Allocate_Default(const int new_size)
{
  GetMemory().Resize(new_size);

  // New space already defaults to default instruction...

  return true;
}

bool cHardwareCPU::Allocate_Main(const int allocated_size)
{
  // must do divide before second allocate & must allocate positive amount...
  if (cConfig::GetRequireAllocate() && mal_active == true) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR, "Allocate already active");
    return false;
  }
  if (allocated_size < 1) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Allocate of %d too small", allocated_size));
    return false;
  }

  const int old_size = GetMemory().GetSize();
  const int new_size = old_size + allocated_size;

  // Make sure that the new size is in range.
  if (new_size > MAX_CREATURE_SIZE  ||  new_size < MIN_CREATURE_SIZE) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Invalid post-allocate size (%d)",
			       new_size));
    return false;
  }

  const int max_alloc_size = (int) (old_size * cConfig::GetChildSizeRange());
  if (allocated_size > max_alloc_size) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Allocate too large (%d > %d)",
			       allocated_size, max_alloc_size));
    return false;
  }

  const int max_old_size =
    (int) (allocated_size * cConfig::GetChildSizeRange());
  if (old_size > max_old_size) {
    Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Allocate too small (%d > %d)",
			       old_size, max_old_size));
    return false;
  }

  switch (cConfig::GetAllocMethod()) {
  case ALLOC_METHOD_NECRO:
    // Only break if this succeeds -- otherwise just do random.
    if (Allocate_Necro(new_size) == true) break;
  case ALLOC_METHOD_RANDOM:
    Allocate_Random(old_size, new_size);
    break;
  case ALLOC_METHOD_DEFAULT:
    Allocate_Default(new_size);
    break;
  }

  mal_active = true;

  return true;
}


bool cHardwareCPU::Divide_CheckViable(const int child_size,
				      const int parent_size)
{
  // Make sure the organism is okay with dividing now...
  if (organism->Divide_CheckViable() == false) return false; // (divide fails)

  // If required, make sure an allocate has occured.
  if (cConfig::GetRequireAllocate() && mal_active == false) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR, "Must allocate before divide");
    return false; //  (divide fails)
  }

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
  if (parent_size < min_size || parent_size > max_size) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Invalid post-divide length (%d)",parent_size));
    return false; // (divide fails)
  }

  // Count the number of lines executed in the parent, and make sure the
  // specified fraction has been reached.

  int executed_size = 0;
  for (int i = 0; i < parent_size; i++) {
    if (GetMemory().FlagExecuted(i)) executed_size++;
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
  for (int i = parent_size; i < parent_size + child_size; i++) {
    if (GetMemory().FlagCopied(i)) copied_size++;
  }

  const int min_copied =  (int) (child_size * cConfig::GetMinCopiedLines());
  if (copied_size < min_copied) {
    Fault(FAULT_LOC_DIVIDE, FAULT_TYPE_ERROR,
	  cStringUtil::Stringf("Too few copied commands (%d < %d)",
			       copied_size, min_copied));
    return false; // (divide fails)
  }

  // Save the information we collected here...
  cPhenotype & phenotype = organism->GetPhenotype();
  phenotype.SetLinesExecuted(executed_size);
  phenotype.SetLinesCopied(copied_size);

  // Determine the fitness of this organism as compared to its parent...
  if (cConfig::GetTestSterilize() == true &&
      phenotype.IsInjected() == false) {
    const int merit_base =
      cPhenotype::CalcSizeMerit(genome_size, copied_size, executed_size);
    const double cur_fitness =
      merit_base * phenotype.GetCurBonus() / phenotype.GetTimeUsed();
    const double fitness_ratio = cur_fitness / phenotype.GetLastFitness();


    //  const double neut_min = parent_fitness * FITNESS_NEUTRAL_MIN;
    //  const double neut_max = parent_fitness * FITNESS_NEUTRAL_MAX;
  
    bool sterilize = false;
  
    if (fitness_ratio < FITNESS_NEUTRAL_MIN) {
      if (g_random.P(organism->GetSterilizeNeg())) sterilize = true;
    } else if (fitness_ratio <= FITNESS_NEUTRAL_MAX) {
      if (g_random.P(organism->GetSterilizeNeut())) sterilize = true;
    } else {
      if (g_random.P(organism->GetSterilizePos())) sterilize = true;
    }
  
//     cout << "[ min(" << genome_size
// 	 << "," << copied_size
// 	 << "," << executed_size
// 	 << ") * " << phenotype.GetCurBonus()
// 	 << " / " << phenotype.GetTimeUsed()
// 	 << "] / " << phenotype.GetLastFitness()
// 	 << " == " << fitness_ratio;

    if (sterilize == true) {
      //Don't let this organism have this or any more children!
      phenotype.IsFertile() = false;
      return false;
    }    
  }

  return true; // (divide succeeds!)
}


void cHardwareCPU::Divide_DoMutations(double mut_multiplier)
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
    for (int i = 0; i < GetMemory().GetSize(); i++) {
      if (organism->TestParentMut()) {
	GetMemory()[i] = GetRandomInst();
	cpu_stats.mut_stats.parent_mut_line_count++;
      }
    }
  }


  // Count up mutated lines
  for(int i = 0; i < GetMemory().GetSize(); i++){
    if (GetMemory().FlagPointMut(i) == true) {
      cpu_stats.mut_stats.point_mut_line_count++;
    }
  }
  for(int i = 0; i < child_genome.GetSize(); i++){
    if( child_genome.FlagCopyMut(i) == true) {
      cpu_stats.mut_stats.copy_mut_line_count++;
    }
  }
}


// test whether the offspring creature contains an advantageous mutation.
void cHardwareCPU::Divide_TestFitnessMeasures()
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


bool cHardwareCPU::Divide_Main(const int div_point, const int extra_lines, double mut_multiplier)
{
  const int child_size = GetMemory().GetSize() - div_point - extra_lines;

  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(child_size, div_point);
  if (viable == false) return false;

  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  cGenome & child_genome = organism->ChildGenome();
  child_genome = cGenomeUtil::Crop(memory, div_point, div_point+child_size);

  // Cut off everything in this memory past the divide point.
  GetMemory().Resize(div_point);

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

  mal_active = false;
  if (cConfig::GetDivideMethod() == DIVIDE_METHOD_SPLIT) {
    advance_ip = false;
  }

  // Activate the child, and do more work if the parent lives through the
  // birth.
  bool parent_alive = organism->ActivateDivide();
  if (parent_alive) {
    if (cConfig::GetDivideMethod() == DIVIDE_METHOD_SPLIT) Reset();
  }

  return true;
}


//////////////////////////
// And the instructions...
//////////////////////////

bool cHardwareCPU::Inst_If0()          // Execute next if ?bx? ==0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) != 0)  IP().Advance();
  return true; 
}

bool cHardwareCPU::Inst_IfNot0()       // Execute next if ?bx? != 0.
{ 
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) == 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfEqu()      // Execute next if bx == ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) != Register(reg_used2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfNEqu()     // Execute next if bx != ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) == Register(reg_used2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGr0()       // Execute next if ?bx? ! < 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) <= 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGr()       // Execute next if bx > ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) <= Register(reg_used2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGrEqu0()       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) < 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGrEqu()       // Execute next if bx > ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) < Register(reg_used2)) IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLess0()       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) >= 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLess()       // Execute next if ?bx? < ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) >=  Register(reg_used2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLsEqu0()       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(reg_used) > 0) IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLsEqu()       // Execute next if bx > ?cx?
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int reg_used2 = FindComplementRegister(reg_used);
  if (Register(reg_used) >  Register(reg_used2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfBit1()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if ((Register(reg_used) & 1) == 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfANotEqB()     // Execute next if AX != BX
{
  if (Register(REG_AX) == Register(REG_BX) )  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfBNotEqC()     // Execute next if BX != CX
{
  if (Register(REG_BX) == Register(REG_CX) )  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfANotEqC()     // Execute next if AX != BX
{
  if (Register(REG_AX) == Register(REG_CX) )  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_JumpF()
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);

  // If there is no label, jump BX steps.
  if (GetLabel().GetSize() == 0) {
    GetActiveHead().Jump(Register(REG_BX));
    return true;
  }

  // Otherwise, try to jump to the complement label.
  const cHeadCPU jump_location(FindLabel(1));
  if ( jump_location.GetPosition() != -1 ) {
    GetActiveHead().Set(jump_location);
    return true;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "jump-f: No complement label");
  return false;
}


bool cHardwareCPU::Inst_JumpB()
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);

  // If there is no label, jump BX steps.
  if (GetLabel().GetSize() == 0) {
    GetActiveHead().Jump(-Register(REG_BX));
    return true;
  }

  // otherwise jump to the complement label.
  const cHeadCPU jump_location(FindLabel(-1));
  if ( jump_location.GetPosition() != -1 ) {
    GetActiveHead().Set(jump_location);
    return true;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "jump-b: No complement label");
  return false;
}

bool cHardwareCPU::Inst_JumpP()
{
  cOrganism * other_organism = organism->GetNeighbor();

  // Make sure the other organism was found and that its hardware is of the
  // same type, or else we won't be able to be parasitic on it.
  if (other_organism == NULL ||
      other_organism->GetHardware().GetType() != GetType()) {
    // Without another organism, its hard to determine if we're dealing
    // with a parasite.  For the moment, we'll assume it is and move on.
    // @CAO Do better!
    organism->GetPhenotype().IsParasite() = true;
    return true;
  }

  // Otherwise, grab the hardware from the neighbor, and use it!
  cHardwareCPU & other_hardware = (cHardwareCPU &) other_organism->GetHardware();

  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);

  // If there is no label, jump to line BX in creature.
  if (GetLabel().GetSize() == 0) {
    const int new_pos = Register(REG_BX);
    IP().Set(new_pos, &other_hardware);
    organism->GetPhenotype().IsParasite() = true;
    return true;
  }

  // otherwise jump to the complement label.
  const cHeadCPU jump_location(other_hardware.FindFullLabel(GetLabel()));
  if (jump_location.GetPosition() != -1) {
    IP().Set(jump_location);
    organism->GetPhenotype().IsParasite() = true;
    return true;
  }

  // If complement label was not found; record a warning (since the
  // actual neighbors are not under the organisms control, this is not
  // a full-scale error).
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_WARNING,
		  "jump-p: No complement label");
  return false;
}

bool cHardwareCPU::Inst_JumpSelf()
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);

  // If there is no label, jump to line BX in creature.
  if (GetLabel().GetSize() == 0) {
    IP().Set(Register(REG_BX), this);
    return true;
  }

  // otherwise jump to the complement label.
  const cHeadCPU jump_location( FindFullLabel(GetLabel()) );
  if ( jump_location.GetPosition() != -1 ) {
    IP().Set(jump_location);
    return true;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "jump-slf: no complement label");
  return false;
}

bool cHardwareCPU::Inst_Call()
{
  // Put the starting location onto the stack
  const int location = IP().GetPosition();
  StackPush(location);

  // Jump to the compliment label (or by the ammount in the bx register)
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);

  if (GetLabel().GetSize() == 0) {
    IP().Jump(Register(REG_BX));
    return true;
  }

  const cHeadCPU jump_location(FindLabel(1));
  if (jump_location.GetPosition() != -1) {
    IP().Set(jump_location);
    return true;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "call: no complement label");
  return false;
}

bool cHardwareCPU::Inst_Return()
{
  IP().Set(StackPop());
  return true;
}

bool cHardwareCPU::Inst_Pop()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = StackPop();
  return true;
}

bool cHardwareCPU::Inst_Push()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  StackPush(Register(reg_used));
  return true;
}

bool cHardwareCPU::Inst_HeadPop()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  GetHead(head_used).Set(StackPop(), this);
  return true;
}

bool cHardwareCPU::Inst_HeadPush()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  StackPush(GetHead(head_used).GetPosition());
  if (head_used == HEAD_IP) {
    GetHead(head_used).Set(GetHead(HEAD_FLOW));
    advance_ip = false;
  }
  return true;
}


bool cHardwareCPU::Inst_PopA() { Register(REG_AX) = StackPop(); return true;}
bool cHardwareCPU::Inst_PopB() { Register(REG_BX) = StackPop(); return true;}
bool cHardwareCPU::Inst_PopC() { Register(REG_CX) = StackPop(); return true;}

bool cHardwareCPU::Inst_PushA() { StackPush(Register(REG_AX)); return true;}
bool cHardwareCPU::Inst_PushB() { StackPush(Register(REG_BX)); return true;}
bool cHardwareCPU::Inst_PushC() { StackPush(Register(REG_CX)); return true;}

bool cHardwareCPU::Inst_SwitchStack() { SwitchStack(); return true;}
bool cHardwareCPU::Inst_FlipStack()   { StackFlip(); return true;}

bool cHardwareCPU::Inst_Swap()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int other_reg = FindComplementRegister(reg_used);
//  cout << endl; 
//  cout << "Regs 1, 2, 3 are: " << Register(1) << " " << Register(2) << " " << Register(3) << endl; 
//  cout << "Modified reg = " << reg_used << ", Complement reg = " << other_reg << endl; 
//  cout << "Calling Swap with " << Register(reg_used) << " and " << Register(other_reg) << endl; 
  nFunctions::Swap(Register(reg_used), Register(other_reg));
//  cout << "Current State is " << Register(reg_used) << " and " << Register(other_reg) << endl; 
//  cout << "Regs 1, 2, 3 are: " << Register(1) << " " << Register(2) << " " << Register(3) << endl; 
  return true;
}

bool cHardwareCPU::Inst_SwapAB() { nFunctions::Swap(Register(REG_AX), Register(REG_BX)); return true; }
bool cHardwareCPU::Inst_SwapBC() { nFunctions::Swap(Register(REG_BX), Register(REG_CX)); return true; }
bool cHardwareCPU::Inst_SwapAC() { nFunctions::Swap(Register(REG_AX), Register(REG_CX)); return true; }

bool cHardwareCPU::Inst_CopyReg()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int other_reg = FindComplementRegister(reg_used);
  Register(other_reg) = Register(reg_used);
  return true;
}

bool cHardwareCPU::Inst_CopyRegAB() { Register(REG_AX) = Register(REG_BX);   return true;
}
bool cHardwareCPU::Inst_CopyRegAC() { Register(REG_AX) = Register(REG_CX);   return true;
}
bool cHardwareCPU::Inst_CopyRegBA() { Register(REG_BX) = Register(REG_AX);   return true;
}
bool cHardwareCPU::Inst_CopyRegBC() { Register(REG_BX) = Register(REG_CX);   return true;
}
bool cHardwareCPU::Inst_CopyRegCA() { Register(REG_CX) = Register(REG_AX);   return true;
}
bool cHardwareCPU::Inst_CopyRegCB() { Register(REG_CX) = Register(REG_BX);   return true;
}

bool cHardwareCPU::Inst_Reset()
{
  Register(REG_AX) = 0;
  Register(REG_BX) = 0;
  Register(REG_CX) = 0;
  StackClear();
  return true;
}

bool cHardwareCPU::Inst_ShiftR()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) >>= 1;
  return true;
}

bool cHardwareCPU::Inst_ShiftL()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) <<= 1;
  return true;
}

bool cHardwareCPU::Inst_Bit1()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) |=  1;
  return true;
}

bool cHardwareCPU::Inst_SetNum()
{
  ReadLabel();
  Register(REG_BX) = GetLabel().AsInt(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValGrey(void) {
  ReadLabel();
  Register(REG_BX) = GetLabel().AsIntGreyCode(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValDir(void) {
  ReadLabel();
  Register(REG_BX) = GetLabel().AsIntDirect(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValAddP(void) {
  ReadLabel();
  Register(REG_BX) = GetLabel().AsIntAdditivePolynomial(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValFib(void) {
  ReadLabel();
  Register(REG_BX) = GetLabel().AsIntFib(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValPolyC(void) {
  ReadLabel();
  Register(REG_BX) = GetLabel().AsIntPolynomialCoefficent(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_Inc()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) += 1;
  return true;
}

bool cHardwareCPU::Inst_Dec()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) -= 1;
  return true;
}

bool cHardwareCPU::Inst_Zero()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = 0;
  return true;
}

bool cHardwareCPU::Inst_Neg()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = 0-Register(reg_used);
  return true;
}

bool cHardwareCPU::Inst_Square()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(reg_used) * Register(reg_used);
  return true;
}

bool cHardwareCPU::Inst_Sqrt()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = Register(reg_used);
  if (value > 1) Register(reg_used) = (int) sqrt((double) value);
  else if (value < 0) {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "sqrt: value is negative");
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Log()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = Register(reg_used);
  if (value >= 1) Register(reg_used) = (int) log((double) value);
  else if (value < 0) {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "log: value is negative");
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Log10()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = Register(reg_used);
  if (value >= 1) Register(reg_used) = (int) log10((double) value);
  else if (value < 0) {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "log10: value is negative");
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Minus17()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) -= 17;
  return true;
}

bool cHardwareCPU::Inst_Add()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(REG_BX) + Register(REG_CX);
  return true;
}

bool cHardwareCPU::Inst_Sub()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(REG_BX) - Register(REG_CX);
  return true;
}

bool cHardwareCPU::Inst_Mult()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(REG_BX) * Register(REG_CX);
  return true;
}

bool cHardwareCPU::Inst_Div()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(REG_CX) != 0) {
    if (0-INT_MAX > Register(REG_BX) && Register(REG_CX) == -1)
      Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: Float exception");
    else
      Register(reg_used) = Register(REG_BX) / Register(REG_CX);
  } else {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Mod()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (Register(REG_CX) != 0) {
    Register(reg_used) = Register(REG_BX) % Register(REG_CX);
  } else {
    Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
  return false;
  }
  return true;
}


bool cHardwareCPU::Inst_Nand()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = ~(Register(REG_BX) & Register(REG_CX));
  return true;
}

bool cHardwareCPU::Inst_Nor()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = ~(Register(REG_BX) | Register(REG_CX));
  return true;
}

bool cHardwareCPU::Inst_And()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = (Register(REG_BX) & Register(REG_CX));
  return true;
}

bool cHardwareCPU::Inst_Not()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = ~(Register(reg_used));
  return true;
}

bool cHardwareCPU::Inst_Order()
{
  if (Register(REG_BX) > Register(REG_CX)) {
    nFunctions::Swap(Register(REG_BX), Register(REG_CX));
  }
  return true;
}

bool cHardwareCPU::Inst_Xor()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = Register(REG_BX) ^ Register(REG_CX);
  return true;
}

bool cHardwareCPU::Inst_Copy()
{
  const cHeadCPU from(this, Register(REG_BX));
  cHeadCPU to(this, Register(REG_AX) + Register(REG_BX));
  sCPUStats & cpu_stats = organism->CPUStats();

  if (organism->TestCopyMut()) {
    to.SetInst(GetRandomInst());
    to.FlagMutated() = true;  // Mark this instruction as mutated...
    to.FlagCopyMut() = true;  // Mark this instruction as copy mut...
    //organism->GetPhenotype().IsMutated() = true;
    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(from.GetInst());
    to.FlagMutated() = false;  // UnMark
    to.FlagCopyMut() = false;  // UnMark
  }

  to.FlagCopied() = true;  // Set the copied flag.
  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareCPU::Inst_ReadInst()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  const cHeadCPU from(this, Register(REG_BX));

  // Dis-allowing mutations on read, for the moment (write only...)
  // @CAO This allows perfect error-correction...
  Register(reg_used) = from.GetInst().GetOp();
  return true;
}

bool cHardwareCPU::Inst_WriteInst()
{
  cHeadCPU to(this, Register(REG_AX) + Register(REG_BX));
  const int reg_used = FindModifiedRegister(REG_CX);
  const int value = Mod(Register(reg_used), GetNumInst());
  sCPUStats & cpu_stats = organism->CPUStats();

  // Change value on a mutation...
  if (organism->TestCopyMut()) {
    to.SetInst(GetRandomInst());
    to.FlagMutated() = true;      // Mark this instruction as mutated...
    to.FlagCopyMut() = true;      // Mark this instruction as copy mut...
    //organism->GetPhenotype().IsMutated() = true;
    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(cInstruction(value));
    to.FlagMutated() = false;     // UnMark
    to.FlagCopyMut() = false;     // UnMark
  }

  to.FlagCopied() = true;  // Set the copied flag.
  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareCPU::Inst_StackReadInst()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  cHeadCPU from(this, Register(reg_used));
  StackPush(from.GetInst().GetOp());
  return true;
}

bool cHardwareCPU::Inst_StackWriteInst()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  cHeadCPU to(this, Register(REG_AX) + Register(reg_used));
  const int value = Mod(StackPop(), GetNumInst());
  sCPUStats & cpu_stats = organism->CPUStats();

  // Change value on a mutation...
  if (organism->TestCopyMut()) {
    to.SetInst(GetRandomInst());
    to.FlagMutated() = true;      // Mark this instruction as mutated...
    to.FlagCopyMut() = true;      // Mark this instruction as copy mut...
    //organism->GetPhenotype().IsMutated() = true;
    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(cInstruction(value));
    to.FlagMutated() = false;     // UnMark
    to.FlagCopyMut() = false;     // UnMark
  }

  to.FlagCopied() = true;  // Set the copied flag.
  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareCPU::Inst_Compare()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  cHeadCPU from(this, Register(REG_BX));
  cHeadCPU to(this, Register(REG_AX) + Register(REG_BX));

  // Compare is dangerous -- it can cause mutations!
  if (organism->TestCopyMut()) {
    to.SetInst(GetRandomInst());
    to.FlagMutated() = true;      // Mark this instruction as mutated...
    to.FlagCopyMut() = true;      // Mark this instruction as copy mut...
    //organism->GetPhenotype().IsMutated() = true;
  }

  Register(reg_used) = from.GetInst().GetOp() - to.GetInst().GetOp();

  return true;
}

bool cHardwareCPU::Inst_IfNCpy()
{
  const cHeadCPU from(this, Register(REG_BX));
  const cHeadCPU to(this, Register(REG_AX) + Register(REG_BX));

  // Allow for errors in this test...
  if (organism->TestCopyMut()) {
    if (from.GetInst() != to.GetInst()) IP().Advance();
  } else {
    if (from.GetInst() == to.GetInst()) IP().Advance();
  }
  return true;
}

bool cHardwareCPU::Inst_Allocate()   // Allocate bx more space...
{
  const int size = GetMemory().GetSize();
  if( Allocate_Main(Register(REG_BX)) ) {
    Register(REG_AX) = size;
    return true;
  } else return false;
}

bool cHardwareCPU::Inst_Divide()  
{ 
  return Divide_Main(Register(REG_AX));    
}

bool cHardwareCPU::Inst_CDivide() 
{ 
  return Divide_Main(GetMemory().GetSize() / 2);   
}

bool cHardwareCPU::Inst_CAlloc()  
{ 
  return Allocate_Main(GetMemory().GetSize());   
}

bool cHardwareCPU::Inst_MaxAlloc()   // Allocate maximal more
{
  const int cur_size = GetMemory().GetSize();
  const int alloc_size = Min((int) (cConfig::GetChildSizeRange() * cur_size),
			     MAX_CREATURE_SIZE - cur_size);
  if( Allocate_Main(alloc_size) ) {
    Register(REG_AX) = cur_size;
    return true;
  } else return false;
}


bool cHardwareCPU::Inst_Repro()
{
  // Setup child
  cCPUMemory & child_genome = organism->ChildGenome();
  child_genome = GetMemory();
  organism->GetPhenotype().SetLinesCopied(GetMemory().GetSize());

  int lines_executed = 0;
  for ( int i = 0; i < GetMemory().GetSize(); i++ ) {
    if ( GetMemory().FlagExecuted(i) == true ) lines_executed++;
  }
  organism->GetPhenotype().SetLinesExecuted(lines_executed);

  // Perform Copy Mutations...
  if (organism->GetCopyMutProb() > 0) { // Skip this if no mutations....
    for (int i = 0; i < GetMemory().GetSize(); i++) {
      if (organism->TestCopyMut()) {
	child_genome[i]=GetRandomInst();
	//organism->GetPhenotype().IsMutated() = true;
      }
    }
  }
  Divide_DoMutations();

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

  if (cConfig::GetDivideMethod() == DIVIDE_METHOD_SPLIT) advance_ip = false;

  organism->ActivateDivide();

  return true;
}


bool cHardwareCPU::Inst_Kazi()
{
	const int reg_used = FindModifiedRegister(REG_AX);
	int percentProb = Register(reg_used) % 100;
	int random = abs(rand()) % 100;
	if (random >= percentProb)
	{
		return true;
	}
	else
	{
		organism->Kaboom();
		return true;
	}
}

bool cHardwareCPU::Inst_Die()
{
   const double die_prob = cConfig::GetDieProb();
   if(g_random.GetDouble() < die_prob) { organism->Die(); }
   return true; 
}

// The inject instruction can be used instead of a divide command, paired
// with an allocate.  Note that for an inject to work, one needs to have a
// broad range for sizes allowed to be allocated.
//
// This command will cut out from read-head to write-head.
// It will then look at the template that follows the command and inject it
// into the complement template found in a neighboring organism.

bool cHardwareCPU::Inst_Inject()
{
  AdjustHeads();
  const int start_pos = GetHead(HEAD_READ).GetPosition();
  const int end_pos = GetHead(HEAD_WRITE).GetPosition();
  const int inject_size = end_pos - start_pos;

  // Make sure the creature will still be above the minimum size,
  if (inject_size <= 0) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: no code to inject");
    return false; // (inject fails)
  }
  if (start_pos < MIN_CREATURE_SIZE) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: new size too small");
    return false; // (inject fails)
  }

  // Since its legal to cut out the injected piece, do so.
  cGenome inject_code( cGenomeUtil::Crop(GetMemory(), start_pos, end_pos) );
  GetMemory().Remove(start_pos, inject_size);

  // If we don't have a host, stop here.
  cOrganism * host_organism = organism->GetNeighbor();
  if (host_organism == NULL) return false;

  // Scan for the label to match...
  ReadLabel();

  // If there is no label, abort.
  if (GetLabel().GetSize() == 0) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: label required");
    return false; // (inject fails)
  }

  // Search for the label in the host...
  GetLabel().Rotate(1, NUM_NOPS);

  const bool inject_signal = host_organism->GetHardware().InjectHost(GetLabel(), inject_code);
  if (inject_signal) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_WARNING, "inject: host too large.");
    return false; // Inject failed.
  }

  // Set the relevent flags.
  organism->GetPhenotype().IsModifier() = true;

  return inject_signal;
}


bool cHardwareCPU::Inst_InjectRand()
{
  // Rotate to a random facing and then run the normal inject instruction
  const int num_neighbors = organism->GetNeighborhoodSize();
  organism->Rotate(g_random.GetUInt(num_neighbors));
  Inst_Inject();
  return true;
}

// The inject instruction can be used instead of a divide command, paired
// with an allocate.  Note that for an inject to work, one needs to have a
// broad range for sizes allowed to be allocated.
//
// This command will cut out from read-head to write-head.
// It will then look at the template that follows the command and inject it
// into the complement template found in a neighboring organism.

bool cHardwareCPU::Inst_InjectThread()
{
  AdjustHeads();
  const int start_pos = GetHead(HEAD_READ).GetPosition();
  const int end_pos = GetHead(HEAD_WRITE).GetPosition();
  const int inject_size = end_pos - start_pos;

  // Make sure the creature will still be above the minimum size,
  if (inject_size <= 0) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: no code to inject");
    return false; // (inject fails)
  }
  if (start_pos < MIN_CREATURE_SIZE) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: new size too small");
    return false; // (inject fails)
  }

  // Since its legal to cut out the injected piece, do so.
  cGenome inject_code( cGenomeUtil::Crop(GetMemory(), start_pos, end_pos) );
  GetMemory().Remove(start_pos, inject_size);

  // If we don't have a host, stop here.
  cOrganism * host_organism = organism->GetNeighbor();
  if (host_organism == NULL) return false;

  // Scan for the label to match...
  ReadLabel();

  // If there is no label, abort.
  if (GetLabel().GetSize() == 0) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: label required");
    return false; // (inject fails)
  }

  // Search for the label in the host...
  GetLabel().Rotate(1, NUM_NOPS);

  const int inject_signal =
    host_organism->GetHardware().InjectThread(GetLabel(), inject_code);
  if (inject_signal == 1) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_WARNING, "inject: host too large.");
    return false; // Inject failed.
  }
  if (inject_signal == 2) {
    Fault(FAULT_LOC_INJECT, FAULT_TYPE_WARNING, "inject: target not in host.");
    return false; // Inject failed.
  }

  // Set the relevent flags.
  organism->GetPhenotype().IsModifier() = true;

  return true;
}

bool cHardwareCPU::Inst_TaskGet()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  const int value = organism->GetNextInput();
  Register(reg_used) = value;
  organism->DoInput(value);
  return true;
}

bool cHardwareCPU::Inst_TaskStackGet()
{
  const int value = organism->GetNextInput();
  StackPush(value);
  organism->DoInput(value);
  return true;
}

bool cHardwareCPU::Inst_TaskStackLoad()
{
  for (int i = 0; i < IO_SIZE; i++) 
    StackPush( organism->GetNextInput() );
  return true;
}

bool cHardwareCPU::Inst_TaskPut()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = Register(reg_used);
  Register(reg_used) = 0;
  organism->DoOutput(value);
  return true;
}

bool cHardwareCPU::Inst_TaskIO()
{
  const int reg_used = FindModifiedRegister(REG_BX);

  // Do the "put" component
  const int value_out = Register(reg_used);
  organism->DoOutput(value_out);  // Check for tasks compleated.

  // Do the "get" component
  const int value_in = organism->GetNextInput();
  Register(reg_used) = value_in;
  organism->DoInput(value_in);
  return true;
}

bool cHardwareCPU::Inst_Send()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  organism->SendValue(Register(reg_used));
  Register(reg_used) = 0;
  return true;
}

bool cHardwareCPU::Inst_Receive()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = organism->ReceiveValue();
  return true;
}

bool cHardwareCPU::Inst_Sense()
{
  const tArray<double> & res_count = organism->PopInterface().GetResources();
  const int reg_used = FindModifiedRegister(REG_BX);

  // If there are no resources to measure, this instruction fails.
  if (res_count.GetSize() == 0) return false;

  // Always get the first resource, and convert it to and int.
  Register(reg_used) = (int) res_count[0];

  // @CAO Since resources are sometimes less than one, perhaps we should
  // multiply it by some constant?  Or perhaps taking the log would be more
  // useful so they can easily scan across orders of magnitude?

  return true;
}

void cHardwareCPU::DoDonate(cOrganism * to_org)
{
  assert(to_org != NULL);

  const double merit_given = cConfig::GetMeritGiven();
  const double merit_received = cConfig::GetMeritReceived();

  double cur_merit = organism->GetPhenotype().GetMerit().GetDouble();
  cur_merit -= merit_given; 

  // Plug the current merit back into this organism and notify the scheduler.
  organism->UpdateMerit(cur_merit);

  // Update the merit of the organism being donated to...
  double other_merit = to_org->GetPhenotype().GetMerit().GetDouble();
  other_merit += merit_received;
  to_org->UpdateMerit(other_merit);
}

bool cHardwareCPU::Inst_DonateRandom()
{
  organism->GetPhenotype().IncDonates();
  if (organism->GetPhenotype().GetCurNumDonates() > cConfig::GetMaxDonates()) {
    return false;
  }

  // Turn to a random neighbor, get it, and turn back...
  int neighbor_id = g_random.GetInt(organism->GetNeighborhoodSize());
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(-1);

  // Donate only if we have found a neighbor.
  if (neighbor != NULL) DoDonate(neighbor);
  
  return true;
}


bool cHardwareCPU::Inst_DonateKin()
{
  organism->GetPhenotype().IncDonates();
  if (organism->GetPhenotype().GetCurNumDonates() > cConfig::GetMaxDonates()) {
    return false;
  }

  // Find the target as the first Kin found in the neighborhood.
  const int num_neighbors = organism->GetNeighborhoodSize();

  // Turn to face a random neighbor
  int neighbor_id = g_random.GetInt(num_neighbors);
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();

  // If there is no max distance, just take the random neighbor we're facing.
  const int max_dist = cConfig::GetMaxDonateKinDistance();
  if (max_dist != -1) {
    int max_id = neighbor_id + num_neighbors;
    bool found = false;
    cGenotype * genotype = organism->GetGenotype();
    while (neighbor_id < max_id) {
      neighbor = organism->GetNeighbor();
      if (neighbor != NULL &&
	  genotype->GetPhyloDistance(neighbor->GetGenotype()) <= max_dist) {
	found = true;
	break;
      }
      organism->Rotate(1);
      neighbor_id++;
    }
    if (found == false) neighbor = NULL;
  }

  // Put the facing back where it was.
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(-1);

  // Donate only if we have found a close enough relative...
  if (neighbor != NULL)  DoDonate(neighbor);

  return true;
}

bool cHardwareCPU::Inst_DonateEditDist()
{
  organism->GetPhenotype().IncDonates();
  if (organism->GetPhenotype().GetCurNumDonates() > cConfig::GetMaxDonates()) {
    return false;
  }

  // Find the target as the first Kin found in the neighborhood.
  const int num_neighbors = organism->GetNeighborhoodSize();

  // Turn to face a random neighbor
  int neighbor_id = g_random.GetInt(num_neighbors);
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();

  // If there is no max edit distance, take the random neighbor we're facing.
  const int max_dist = cConfig::GetMaxDonateEditDistance();
  if (max_dist != -1) {
    int max_id = neighbor_id + num_neighbors;
    bool found = false;
    while (neighbor_id < max_id) {
      neighbor = organism->GetNeighbor();
      int edit_dist = max_dist + 1;
      if (neighbor != NULL) {
	edit_dist = cGenomeUtil::FindEditDistance(organism->GetGenome(),
						  neighbor->GetGenome());
      }
      if (edit_dist <= max_dist) {
	found = true;
	break;
      }
      organism->Rotate(1);
      neighbor_id++;
    }
    if (found == false) neighbor = NULL;
  }

  // Put the facing back where it was.
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(-1);

  // Donate only if we have found a close enough relative...
  if (neighbor != NULL)  DoDonate(neighbor);

  return true;
}


bool cHardwareCPU::Inst_DonateNULL()
{
  organism->GetPhenotype().IncDonates();
  if (organism->GetPhenotype().GetCurNumDonates() > cConfig::GetMaxDonates()) {
    return false;
  }

  // This is a fake donate command that causes the organism to lose merit,
  // but no one else to gain any.

  const double merit_given = cConfig::GetMeritGiven();
  double cur_merit = organism->GetPhenotype().GetMerit().GetDouble();
  cur_merit -= merit_given;

  // Plug the current merit back into this organism and notify the scheduler.
  organism->UpdateMerit(cur_merit);

  return true;
}


bool cHardwareCPU::Inst_SearchF()
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  const int search_size = FindLabel(1).GetPosition() - IP().GetPosition();
  Register(REG_BX) = search_size;
  Register(REG_CX) = GetLabel().GetSize();
  return true;
}

bool cHardwareCPU::Inst_SearchB()
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  const int search_size = IP().GetPosition() - FindLabel(-1).GetPosition();
  Register(REG_BX) = search_size;
  Register(REG_CX) = GetLabel().GetSize();
  return true;
}

bool cHardwareCPU::Inst_MemSize()
{
  Register(FindModifiedRegister(REG_BX)) = GetMemory().GetSize();
  return true;
}


bool cHardwareCPU::Inst_RotateL()
{
  const int num_neighbors = organism->GetNeighborhoodSize();

   // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;

  ReadLabel();

  // Always rotate at least once.
  organism->Rotate(-1);

  // If there is no label, then the one rotation was all we want.
  if (!GetLabel().GetSize()) return true;

  // Rotate until a complement label is found (or all have been checked).
  GetLabel().Rotate(1, NUM_NOPS);
  for (int i = 1; i < num_neighbors; i++) {
    cOrganism * neighbor = organism->GetNeighbor();

    // Assuming we have a neighbor and it is of the same hardware type,
    // search for the label in it.
    if (neighbor != NULL &&
	neighbor->GetHardware().GetType() == GetType()) {

      // If this facing has the full label, stop here.
      cHardwareCPU & cur_hardware = (cHardwareCPU &) neighbor->GetHardware();
      if (cur_hardware.FindFullLabel( GetLabel() ).InMemory()) return true;
    }

    // Otherwise keep rotating...
    organism->Rotate(-1);
  }
  return true;
}

bool cHardwareCPU::Inst_RotateR()
{
  const int num_neighbors = organism->GetNeighborhoodSize();

   // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;

  ReadLabel();

  // Always rotate at least once.
  organism->Rotate(-1);

  // If there is no label, then the one rotation was all we want.
  if (!GetLabel().GetSize()) return true;

  // Rotate until a complement label is found (or all have been checked).
  GetLabel().Rotate(1, NUM_NOPS);
  for (int i = 1; i < num_neighbors; i++) {
    cOrganism * neighbor = organism->GetNeighbor();

    // Assuming we have a neighbor and it is of the same hardware type,
    // search for the label in it.
    if (neighbor != NULL &&
	neighbor->GetHardware().GetType() == GetType()) {

      // If this facing has the full label, stop here.
      cHardwareCPU & cur_hardware = (cHardwareCPU &) neighbor->GetHardware();
      if (cur_hardware.FindFullLabel( GetLabel() ).InMemory()) return true;
    }

    // Otherwise keep rotating...
    organism->Rotate(1);
  }
  return true;
}

bool cHardwareCPU::Inst_SetCopyMut()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int new_mut_rate = Max( Register(reg_used), 1 );
  organism->SetCopyMutProb(((double) new_mut_rate) / 10000.0);
  return true;
}

bool cHardwareCPU::Inst_ModCopyMut()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const double new_mut_rate = organism->GetCopyMutProb() +
    ((double) Register(reg_used)) / 10000.0;
  if (new_mut_rate > 0.0) organism->SetCopyMutProb(new_mut_rate);
  return true;
}


// Multi-threading.

bool cHardwareCPU::Inst_ForkThread()
{
  IP().Advance();
  if (!ForkThread()) Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
  return true;
}

bool cHardwareCPU::Inst_KillThread()
{
  if (!KillThread()) Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  else advance_ip = false;
  return true;
}

bool cHardwareCPU::Inst_ThreadID()
{
  const int reg_used = FindModifiedRegister(REG_BX);
  Register(reg_used) = GetCurThreadID();
  return true;
}


// Head-based instructions

bool cHardwareCPU::Inst_SetHead()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  SetActiveHead(head_used);
  return true;
}

bool cHardwareCPU::Inst_AdvanceHead()
{
  const int head_used = FindModifiedHead(HEAD_WRITE);
  GetHead(head_used).Advance();
  return true;
}
 
bool cHardwareCPU::Inst_MoveHead()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  GetHead(head_used).Set(GetHead(HEAD_FLOW));
  if (head_used == HEAD_IP) advance_ip = false;
  return true;
}

bool cHardwareCPU::Inst_JumpHead()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  GetHead(head_used).Jump( Register(REG_CX) );
  return true;
}

bool cHardwareCPU::Inst_GetHead()
{
  const int head_used = FindModifiedHead(HEAD_IP);
  Register(REG_CX) = GetHead(head_used).GetPosition();
  return true;
}

bool cHardwareCPU::Inst_IfLabel()
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel())  IP().Advance();
  return true;
}

// This is a variation on IfLabel that will skip the next command if the "if"
// is false, but it will also skip all nops following that command.
bool cHardwareCPU::Inst_IfLabel2()
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel()) {
    IP().Advance();
    if (inst_set->IsNop( IP().GetNextInst() ))  IP().Advance();
  }
  return true;
}

bool cHardwareCPU::Inst_HeadDivideMut(double mut_multiplier)
{
  AdjustHeads();
  const int divide_pos = GetHead(HEAD_READ).GetPosition();
  int child_end =  GetHead(HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = GetMemory().GetSize();
  const int extra_lines = GetMemory().GetSize() - child_end;
  bool ret_val = Divide_Main(divide_pos, extra_lines, mut_multiplier);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val; 
}

bool cHardwareCPU::Inst_HeadDivide()
{
  return Inst_HeadDivideMut(1);
}

bool cHardwareCPU::Inst_HeadDivideSex()  
{ 
  organism->GetPhenotype().SetDivideSex(true);
  organism->GetPhenotype().SetCrossNum(1);
  return Inst_HeadDivide(); 
}

bool cHardwareCPU::Inst_HeadDivideAsex()  
{ 
  organism->GetPhenotype().SetDivideSex(false);
  organism->GetPhenotype().SetCrossNum(0);
  return Inst_HeadDivide(); 
}

bool cHardwareCPU::Inst_HeadDivideAsexWait()  
{ 
  organism->GetPhenotype().SetDivideSex(true);
  organism->GetPhenotype().SetCrossNum(0);
  return Inst_HeadDivide(); 
}

bool cHardwareCPU::Inst_HeadDivideMateSelect()  
{ 
  // Take the label that follows this divide and use it as the ID for which
  // other organisms this one is willing to mate with.
  ReadLabel();
  organism->GetPhenotype().SetMateSelectID( GetLabel().AsInt(NUM_NOPS) );

//   int mate_id = GetLabel().AsInt(NUM_NOPS);
//   if (mate_id > 0) cout << mate_id << " "
// 			<< GetLabel().AsString() << endl;

  // Proceed as normal with the rest of mate selection.
  organism->GetPhenotype().SetDivideSex(true);
  organism->GetPhenotype().SetCrossNum(1);
  return Inst_HeadDivide(); 
}

bool cHardwareCPU::Inst_HeadDivide1()  { return Inst_HeadDivideMut(1); }
bool cHardwareCPU::Inst_HeadDivide2()  { return Inst_HeadDivideMut(2); }
bool cHardwareCPU::Inst_HeadDivide3()  { return Inst_HeadDivideMut(3); }
bool cHardwareCPU::Inst_HeadDivide4()  { return Inst_HeadDivideMut(4); }
bool cHardwareCPU::Inst_HeadDivide5()  { return Inst_HeadDivideMut(5); }
bool cHardwareCPU::Inst_HeadDivide6()  { return Inst_HeadDivideMut(6); }
bool cHardwareCPU::Inst_HeadDivide7()  { return Inst_HeadDivideMut(7); }
bool cHardwareCPU::Inst_HeadDivide8()  { return Inst_HeadDivideMut(8); }
bool cHardwareCPU::Inst_HeadDivide9()  { return Inst_HeadDivideMut(9); }
bool cHardwareCPU::Inst_HeadDivide10()  { return Inst_HeadDivideMut(10); }
bool cHardwareCPU::Inst_HeadDivide16()  { return Inst_HeadDivideMut(16); }
bool cHardwareCPU::Inst_HeadDivide32()  { return Inst_HeadDivideMut(32); }
bool cHardwareCPU::Inst_HeadDivide50()  { return Inst_HeadDivideMut(50); }
bool cHardwareCPU::Inst_HeadDivide100()  { return Inst_HeadDivideMut(100); }
bool cHardwareCPU::Inst_HeadDivide500()  { return Inst_HeadDivideMut(500); }
bool cHardwareCPU::Inst_HeadDivide1000()  { return Inst_HeadDivideMut(1000); }
bool cHardwareCPU::Inst_HeadDivide5000()  { return Inst_HeadDivideMut(5000); }
bool cHardwareCPU::Inst_HeadDivide10000()  { return Inst_HeadDivideMut(10000); }
bool cHardwareCPU::Inst_HeadDivide50000()  { return Inst_HeadDivideMut(50000); }
bool cHardwareCPU::Inst_HeadDivide0_5()  { return Inst_HeadDivideMut(0.5); }
bool cHardwareCPU::Inst_HeadDivide0_1()  { return Inst_HeadDivideMut(0.1); }
bool cHardwareCPU::Inst_HeadDivide0_05()  { return Inst_HeadDivideMut(0.05); }
bool cHardwareCPU::Inst_HeadDivide0_01()  { return Inst_HeadDivideMut(0.01); }
bool cHardwareCPU::Inst_HeadDivide0_001()  { return Inst_HeadDivideMut(0.001); }

bool cHardwareCPU::Inst_HeadRead()
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
  Register(REG_BX) = read_inst;
  ReadInst(read_inst);

  cpu_stats.mut_stats.copies_exec++;  // @CAO, this too..
  GetHead(head_id).Advance();
  return true;
}

bool cHardwareCPU::Inst_HeadWrite()
{
  const int head_id = FindModifiedHead(HEAD_WRITE);
  cHeadCPU & active_head = GetHead(head_id);

  active_head.Adjust();

  int value = Register(REG_BX);
  if (value < 0 || value >= GetNumInst()) value = 0;

  active_head.SetInst(cInstruction(value));
  active_head.FlagCopied() = true;

  // Advance the head after write...
  active_head++;
  return true;
}

bool cHardwareCPU::Inst_HeadCopy()
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU & read_head = GetHead(HEAD_READ);
  cHeadCPU & write_head = GetHead(HEAD_WRITE);
  sCPUStats & cpu_stats = organism->CPUStats();

  read_head.Adjust();
  write_head.Adjust();

  // TriggerMutations(MUTATION_TRIGGER_READ, read_head);
  
  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  ReadInst(read_inst.GetOp());
  if (organism->TestCopyMut()) {
    read_inst = GetRandomInst();
    cpu_stats.mut_stats.copy_mut_count++; 
    write_head.FlagMutated() = true;
    write_head.FlagCopyMut() = true;
    //organism->GetPhenotype().IsMutated() = true;
  }

  cpu_stats.mut_stats.copies_exec++;

  write_head.SetInst(read_inst);
  write_head.FlagCopied() = true;  // Set the copied flag...

  // TriggerMutations(MUTATION_TRIGGER_WRITE, write_head);

  read_head.Advance();
  write_head.Advance();
  return true;
}

bool cHardwareCPU::HeadCopy_ErrorCorrect(double reduction)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU & read_head = GetHead(HEAD_READ);
  cHeadCPU & write_head = GetHead(HEAD_WRITE);
  sCPUStats & cpu_stats = organism->CPUStats();

  read_head.Adjust();
  write_head.Adjust();

  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  ReadInst(read_inst.GetOp());
  if ( g_random.P(organism->GetCopyMutProb() / reduction) ) {
    read_inst = GetRandomInst();
    cpu_stats.mut_stats.copy_mut_count++; 
    write_head.FlagMutated() = true;
    write_head.FlagCopyMut() = true;
    //organism->GetPhenotype().IsMutated() = true;
  }

  cpu_stats.mut_stats.copies_exec++;

  write_head.SetInst(read_inst);
  write_head.FlagCopied() = true;  // Set the copied flag...

  read_head.Advance();
  write_head.Advance();
  return true;
}

bool cHardwareCPU::Inst_HeadCopy2()  { return HeadCopy_ErrorCorrect(2); }
bool cHardwareCPU::Inst_HeadCopy3()  { return HeadCopy_ErrorCorrect(3); }
bool cHardwareCPU::Inst_HeadCopy4()  { return HeadCopy_ErrorCorrect(4); }
bool cHardwareCPU::Inst_HeadCopy5()  { return HeadCopy_ErrorCorrect(5); }
bool cHardwareCPU::Inst_HeadCopy6()  { return HeadCopy_ErrorCorrect(6); }
bool cHardwareCPU::Inst_HeadCopy7()  { return HeadCopy_ErrorCorrect(7); }
bool cHardwareCPU::Inst_HeadCopy8()  { return HeadCopy_ErrorCorrect(8); }
bool cHardwareCPU::Inst_HeadCopy9()  { return HeadCopy_ErrorCorrect(9); }
bool cHardwareCPU::Inst_HeadCopy10() { return HeadCopy_ErrorCorrect(10); }

bool cHardwareCPU::Inst_HeadSearch()
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabel(0);
  const int search_size = found_pos.GetPosition() - IP().GetPosition();
  Register(REG_BX) = search_size;
  Register(REG_CX) = GetLabel().GetSize();
  GetHead(HEAD_FLOW).Set(found_pos);
  GetHead(HEAD_FLOW).Advance();
  return true; 
}

bool cHardwareCPU::Inst_SetFlow()
{
  const int reg_used = FindModifiedRegister(REG_CX);
  GetHead(HEAD_FLOW).Set(Register(reg_used), this);
  return true; 
}

// Direct Matching Templates

bool cHardwareCPU::Inst_DMJumpF()
{
  ReadLabel();

  // If there is no label, jump BX steps.
  if (GetLabel().GetSize() == 0) {
    IP().Jump(Register(REG_BX));
    return true;
  }

  // Otherwise, jump to the label.
  cHeadCPU jump_location(FindLabel(1));
  if (jump_location.GetPosition() != -1) {
    IP().Set(jump_location);
    return true;
  }

  // If complement label was not found; record an error.
  organism->Fault(FAULT_LOC_JUMP, FAULT_TYPE_ERROR,
		  "dm-jump-f: no complement label");
  return false;
}


//// Placebo insts ////
bool cHardwareCPU::Inst_Skip()
{
  IP().Advance();
  return true;
}


