/*
 *  cHardwareExperimental.cc
 *  Avida
 *
 *  Created by David on 2/10/07 based on cHardwareCPU.cc
 *  Copyright 2005-2007 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */


#include "cHardwareExperimental.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "functions.h"
#include "cGenomeUtil.h"
#include "cGenotype.h"
#include "cHardwareManager.h"
#include "cHardwareTracer.h"
#include "cInstLibCPU.h"
#include "cInstSet.h"
#include "cMutation.h"
#include "cMutationLib.h"
#include "nMutation.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cStringUtil.h"
#include "cTestCPU.h"
#include "cWorldDriver.h"
#include "cWorld.h"

#include <limits.h>
#include <fstream>

using namespace std;


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
    cNOPEntry("nop-F", REG_FX)
  };
  
  struct cInstEntry { 
    const cString name;
    const tMethod function;
    const bool is_default;
    const cString desc;
    
    cInstEntry(const cString & _name, tMethod _fun,
                  bool _def=false, const cString & _desc="")
      : name(_name), function(_fun), is_default(_def), desc(_desc) {}
  };
  static const cInstEntry s_f_array[] = {
    /*
     Note: all entries of cNOPEntryCPU s_n_array must have corresponding
     in the same order in cInstEntry s_f_array, and these entries must
     be the first elements of s_f_array.
     */
    cInstEntry("nop-A",     &cHardwareExperimental::Inst_Nop, true,
                  "No-operation instruction; modifies other instructions"),
    cInstEntry("nop-B",     &cHardwareExperimental::Inst_Nop, true,
                  "No-operation instruction; modifies other instructions"),
    cInstEntry("nop-C",     &cHardwareExperimental::Inst_Nop, true,
                  "No-operation instruction; modifies other instructions"),
    cInstEntry("nop-D",     &cHardwareExperimental::Inst_Nop, true,
                  "No-operation instruction; modifies other instructions"),
    cInstEntry("nop-E",     &cHardwareExperimental::Inst_Nop, true,
                  "No-operation instruction; modifies other instructions"),
    cInstEntry("nop-F",     &cHardwareExperimental::Inst_Nop, true,
                  "No-operation instruction; modifies other instructions"),
    
    cInstEntry("NULL",      &cHardwareExperimental::Inst_Nop, false,
                  "True no-operation instruction: does nothing"),
    cInstEntry("nop-X",     &cHardwareExperimental::Inst_Nop, false,
                  "True no-operation instruction: does nothing"),
    cInstEntry("if-equ-0",  &cHardwareExperimental::Inst_If0, false,
                  "Execute next instruction if ?BX?==0, else skip it"),
    cInstEntry("if-not-0",  &cHardwareExperimental::Inst_IfNot0, false,
                  "Execute next instruction if ?BX?!=0, else skip it"),
    cInstEntry("if-n-equ",  &cHardwareExperimental::Inst_IfNEqu, true,
                  "Execute next instruction if ?BX?!=?CX?, else skip it"),
    cInstEntry("if-equ",    &cHardwareExperimental::Inst_IfEqu, false,
                  "Execute next instruction if ?BX?==?CX?, else skip it"),
    cInstEntry("if-grt-0",  &cHardwareExperimental::Inst_IfGr0),
    cInstEntry("if-grt",    &cHardwareExperimental::Inst_IfGr),
    cInstEntry("if->=-0",   &cHardwareExperimental::Inst_IfGrEqu0),
    cInstEntry("if->=",     &cHardwareExperimental::Inst_IfGrEqu),
    cInstEntry("if-les-0",  &cHardwareExperimental::Inst_IfLess0),
    cInstEntry("if-less",   &cHardwareExperimental::Inst_IfLess, true,
                  "Execute next instruction if ?BX? < ?CX?, else skip it"),
    cInstEntry("if-<=-0",   &cHardwareExperimental::Inst_IfLsEqu0),
    cInstEntry("if-<=",     &cHardwareExperimental::Inst_IfLsEqu),
    cInstEntry("if-A!=B",   &cHardwareExperimental::Inst_IfANotEqB),
    cInstEntry("if-B!=C",   &cHardwareExperimental::Inst_IfBNotEqC),
    cInstEntry("if-A!=C",   &cHardwareExperimental::Inst_IfANotEqC),
    cInstEntry("if-bit-1",  &cHardwareExperimental::Inst_IfBit1),
    
    cInstEntry("jump-f",    &cHardwareExperimental::Inst_JumpF),
    cInstEntry("jump-b",    &cHardwareExperimental::Inst_JumpB),
    cInstEntry("call",      &cHardwareExperimental::Inst_Call),
    cInstEntry("return",    &cHardwareExperimental::Inst_Return),

    cInstEntry("throw",     &cHardwareExperimental::Inst_Throw),
    cInstEntry("throwif=0", &cHardwareExperimental::Inst_ThrowIf0),    
    cInstEntry("throwif!=0",&cHardwareExperimental::Inst_ThrowIfNot0),
    cInstEntry("catch",     &cHardwareExperimental::Inst_Catch),
    
    cInstEntry("goto",      &cHardwareExperimental::Inst_Goto),
    cInstEntry("goto-if=0", &cHardwareExperimental::Inst_GotoIf0),    
    cInstEntry("goto-if!=0",&cHardwareExperimental::Inst_GotoIfNot0),
    cInstEntry("label",     &cHardwareExperimental::Inst_Label),
    
    cInstEntry("pop",       &cHardwareExperimental::Inst_Pop, true,
                  "Remove top number from stack and place into ?BX?"),
    cInstEntry("push",      &cHardwareExperimental::Inst_Push, true,
                  "Copy number from ?BX? and place it into the stack"),
    cInstEntry("swap-stk",  &cHardwareExperimental::Inst_SwitchStack, true,
                  "Toggle which stack is currently being used"),
    cInstEntry("flip-stk",  &cHardwareExperimental::Inst_FlipStack),
    cInstEntry("swap",      &cHardwareExperimental::Inst_Swap, true,
                  "Swap the contents of ?BX? with ?CX?"),
    cInstEntry("swap-AB",   &cHardwareExperimental::Inst_SwapAB),
    cInstEntry("swap-BC",   &cHardwareExperimental::Inst_SwapBC),
    cInstEntry("swap-AC",   &cHardwareExperimental::Inst_SwapAC),
    cInstEntry("copy-reg",  &cHardwareExperimental::Inst_CopyReg),
    cInstEntry("set_A=B",   &cHardwareExperimental::Inst_CopyRegAB),
    cInstEntry("set_A=C",   &cHardwareExperimental::Inst_CopyRegAC),
    cInstEntry("set_B=A",   &cHardwareExperimental::Inst_CopyRegBA),
    cInstEntry("set_B=C",   &cHardwareExperimental::Inst_CopyRegBC),
    cInstEntry("set_C=A",   &cHardwareExperimental::Inst_CopyRegCA),
    cInstEntry("set_C=B",   &cHardwareExperimental::Inst_CopyRegCB),
    cInstEntry("reset",     &cHardwareExperimental::Inst_Reset),
    
    cInstEntry("pop-A",     &cHardwareExperimental::Inst_PopA),
    cInstEntry("pop-B",     &cHardwareExperimental::Inst_PopB),
    cInstEntry("pop-C",     &cHardwareExperimental::Inst_PopC),
    cInstEntry("push-A",    &cHardwareExperimental::Inst_PushA),
    cInstEntry("push-B",    &cHardwareExperimental::Inst_PushB),
    cInstEntry("push-C",    &cHardwareExperimental::Inst_PushC),
    
    cInstEntry("shift-r",   &cHardwareExperimental::Inst_ShiftR, true,
                  "Shift bits in ?BX? right by one (divide by two)"),
    cInstEntry("shift-l",   &cHardwareExperimental::Inst_ShiftL, true,
                  "Shift bits in ?BX? left by one (multiply by two)"),
    cInstEntry("bit-1",     &cHardwareExperimental::Inst_Bit1),
    cInstEntry("set-num",   &cHardwareExperimental::Inst_SetNum),
    cInstEntry("val-grey",  &cHardwareExperimental::Inst_ValGrey),
    cInstEntry("val-dir",   &cHardwareExperimental::Inst_ValDir),
    cInstEntry("val-add-p", &cHardwareExperimental::Inst_ValAddP),
    cInstEntry("val-fib",   &cHardwareExperimental::Inst_ValFib),
    cInstEntry("val-poly-c",&cHardwareExperimental::Inst_ValPolyC),
    cInstEntry("inc",       &cHardwareExperimental::Inst_Inc, true,
                  "Increment ?BX? by one"),
    cInstEntry("dec",       &cHardwareExperimental::Inst_Dec, true,
                  "Decrement ?BX? by one"),
    cInstEntry("zero",      &cHardwareExperimental::Inst_Zero, false,
                  "Set ?BX? to zero"),
    cInstEntry("neg",       &cHardwareExperimental::Inst_Neg),
    cInstEntry("square",    &cHardwareExperimental::Inst_Square),
    cInstEntry("sqrt",      &cHardwareExperimental::Inst_Sqrt),
    cInstEntry("not",       &cHardwareExperimental::Inst_Not),
    
    cInstEntry("add",       &cHardwareExperimental::Inst_Add, true,
                  "Add BX to CX and place the result in ?BX?"),
    cInstEntry("sub",       &cHardwareExperimental::Inst_Sub, true,
                  "Subtract CX from BX and place the result in ?BX?"),
    cInstEntry("mult",      &cHardwareExperimental::Inst_Mult, false,
                  "Multiple BX by CX and place the result in ?BX?"),
    cInstEntry("div",       &cHardwareExperimental::Inst_Div, false,
                  "Divide BX by CX and place the result in ?BX?"),
    cInstEntry("mod",       &cHardwareExperimental::Inst_Mod),
    cInstEntry("nand",      &cHardwareExperimental::Inst_Nand, true,
                  "Nand BX by CX and place the result in ?BX?"),
    cInstEntry("nor",       &cHardwareExperimental::Inst_Nor),
    cInstEntry("and",       &cHardwareExperimental::Inst_And),
    cInstEntry("order",     &cHardwareExperimental::Inst_Order),
    cInstEntry("xor",       &cHardwareExperimental::Inst_Xor),
    
    cInstEntry("copy",      &cHardwareExperimental::Inst_Copy),
    cInstEntry("read",      &cHardwareExperimental::Inst_ReadInst),
    cInstEntry("write",     &cHardwareExperimental::Inst_WriteInst),
    cInstEntry("stk-read",  &cHardwareExperimental::Inst_StackReadInst),
    cInstEntry("stk-writ",  &cHardwareExperimental::Inst_StackWriteInst),
    
    cInstEntry("compare",   &cHardwareExperimental::Inst_Compare),
    cInstEntry("if-n-cpy",  &cHardwareExperimental::Inst_IfNCpy),
    cInstEntry("allocate",  &cHardwareExperimental::Inst_Allocate),
    cInstEntry("divide",    &cHardwareExperimental::Inst_Divide),
    cInstEntry("divideRS",  &cHardwareExperimental::Inst_DivideRS),
    cInstEntry("c-alloc",   &cHardwareExperimental::Inst_CAlloc),
    cInstEntry("c-divide",  &cHardwareExperimental::Inst_CDivide),
    cInstEntry("inject",    &cHardwareExperimental::Inst_Inject),
    cInstEntry("inject-r",  &cHardwareExperimental::Inst_InjectRand),
    cInstEntry("transposon",&cHardwareExperimental::Inst_Transposon),
    cInstEntry("search-f",  &cHardwareExperimental::Inst_SearchF),
    cInstEntry("search-b",  &cHardwareExperimental::Inst_SearchB),
    cInstEntry("mem-size",  &cHardwareExperimental::Inst_MemSize),
    
    cInstEntry("get",       &cHardwareExperimental::Inst_TaskGet),
    cInstEntry("stk-get",   &cHardwareExperimental::Inst_TaskStackGet),
    cInstEntry("stk-load",  &cHardwareExperimental::Inst_TaskStackLoad),
    cInstEntry("put",       &cHardwareExperimental::Inst_TaskPut),
    cInstEntry("put-clear", &cHardwareExperimental::Inst_TaskPutClearInput),    
    cInstEntry("put-bcost2", &cHardwareExperimental::Inst_TaskPutBonusCost2),
    cInstEntry("put-mcost2", &cHardwareExperimental::Inst_TaskPutMeritCost2),
    cInstEntry("IO",        &cHardwareExperimental::Inst_TaskIO, true,
                  "Output ?BX?, and input new number back into ?BX?"),
    cInstEntry("IO-Feedback",        &cHardwareExperimental::Inst_TaskIO_Feedback, true,\
                  "Output ?BX?, and input new number back into ?BX?,  and push 1,0,\
                  or -1 onto stack1 if merit increased, stayed the same, or decreased"),
    cInstEntry("match-strings", &cHardwareExperimental::Inst_MatchStrings),
    cInstEntry("sell", &cHardwareExperimental::Inst_Sell),
    cInstEntry("buy", &cHardwareExperimental::Inst_Buy),
    cInstEntry("send",      &cHardwareExperimental::Inst_Send),
    cInstEntry("receive",   &cHardwareExperimental::Inst_Receive),
    cInstEntry("sense",     &cHardwareExperimental::Inst_SenseLog2),
    cInstEntry("sense-unit",     &cHardwareExperimental::Inst_SenseUnit),
    cInstEntry("sense-m100",     &cHardwareExperimental::Inst_SenseMult100),
    
    cInstEntry("donate-rnd",  &cHardwareExperimental::Inst_DonateRandom),
    cInstEntry("donate-kin",  &cHardwareExperimental::Inst_DonateKin),
    cInstEntry("donate-edt",  &cHardwareExperimental::Inst_DonateEditDist),
    cInstEntry("donate-NUL",  &cHardwareExperimental::Inst_DonateNULL),
    
    cInstEntry("rotate-l",  &cHardwareExperimental::Inst_RotateL),
    cInstEntry("rotate-r",  &cHardwareExperimental::Inst_RotateR),
    
    cInstEntry("set-cmut",  &cHardwareExperimental::Inst_SetCopyMut),
    cInstEntry("mod-cmut",  &cHardwareExperimental::Inst_ModCopyMut),
    
    // Threading instructions
    cInstEntry("fork-th",   &cHardwareExperimental::Inst_ForkThread),
    cInstEntry("kill-th",   &cHardwareExperimental::Inst_KillThread),
    cInstEntry("id-th",     &cHardwareExperimental::Inst_ThreadID),
    
    // Head-based instructions
    cInstEntry("h-alloc",   &cHardwareExperimental::Inst_MaxAlloc, true,
                  "Allocate maximum allowed space"),
    cInstEntry("h-divide",  &cHardwareExperimental::Inst_HeadDivide, true,
                  "Divide code between read and write heads."),
    cInstEntry("h-divide1RS",  &cHardwareExperimental::Inst_HeadDivide1RS, true,
		  "Divide code between read and write heads, at most one mutation on divide, resample if reverted."),
    cInstEntry("h-divide2RS",  &cHardwareExperimental::Inst_HeadDivide2RS, true,
                  "Divide code between read and write heads, at most two mutations on divide, resample if reverted."),
    cInstEntry("h-divideRS",  &cHardwareExperimental::Inst_HeadDivideRS, true,
                  "Divide code between read and write heads, resample if reverted."),
    cInstEntry("h-read",    &cHardwareExperimental::Inst_HeadRead),
    cInstEntry("h-write",   &cHardwareExperimental::Inst_HeadWrite),
    cInstEntry("h-copy",    &cHardwareExperimental::Inst_HeadCopy, true,
                  "Copy from read-head to write-head; advance both"),
    cInstEntry("h-search",  &cHardwareExperimental::Inst_HeadSearch, true,
                  "Find complement template and make with flow head"),
    cInstEntry("h-push",    &cHardwareExperimental::Inst_HeadPush),
    cInstEntry("h-pop",     &cHardwareExperimental::Inst_HeadPop),
    cInstEntry("set-head",  &cHardwareExperimental::Inst_SetHead),
    cInstEntry("adv-head",  &cHardwareExperimental::Inst_AdvanceHead),
    cInstEntry("mov-head",  &cHardwareExperimental::Inst_MoveHead, true,
                  "Move head ?IP? to the flow head"),
    cInstEntry("jmp-head",  &cHardwareExperimental::Inst_JumpHead, true,
                  "Move head ?IP? by amount in CX register; CX = old pos."),
    cInstEntry("get-head",  &cHardwareExperimental::Inst_GetHead, true,
                  "Copy the position of the ?IP? head into CX"),
    cInstEntry("if-label",  &cHardwareExperimental::Inst_IfLabel, true,
                  "Execute next if we copied complement of attached label"),
    cInstEntry("if-label2",  &cHardwareExperimental::Inst_IfLabel2, true,
                  "If copied label compl., exec next inst; else SKIP W/NOPS"),
    cInstEntry("set-flow",  &cHardwareExperimental::Inst_SetFlow, true,
                  "Set flow-head to position in ?CX?"),
    
    cInstEntry("h-copy2",    &cHardwareExperimental::Inst_HeadCopy2),
    cInstEntry("h-copy3",    &cHardwareExperimental::Inst_HeadCopy3),
    cInstEntry("h-copy4",    &cHardwareExperimental::Inst_HeadCopy4),
    cInstEntry("h-copy5",    &cHardwareExperimental::Inst_HeadCopy5),
    cInstEntry("h-copy6",    &cHardwareExperimental::Inst_HeadCopy6),
    cInstEntry("h-copy7",    &cHardwareExperimental::Inst_HeadCopy7),
    cInstEntry("h-copy8",    &cHardwareExperimental::Inst_HeadCopy8),
    cInstEntry("h-copy9",    &cHardwareExperimental::Inst_HeadCopy9),
    cInstEntry("h-copy10",   &cHardwareExperimental::Inst_HeadCopy10),
    
    cInstEntry("divide-sex",    &cHardwareExperimental::Inst_HeadDivideSex),
    cInstEntry("divide-asex",   &cHardwareExperimental::Inst_HeadDivideAsex),
    
    cInstEntry("div-sex",    &cHardwareExperimental::Inst_HeadDivideSex),
    cInstEntry("div-asex",   &cHardwareExperimental::Inst_HeadDivideAsex),
    cInstEntry("div-asex-w",   &cHardwareExperimental::Inst_HeadDivideAsexWait),
    cInstEntry("div-sex-MS",   &cHardwareExperimental::Inst_HeadDivideMateSelect),
    
    cInstEntry("h-divide1",      &cHardwareExperimental::Inst_HeadDivide1),
    cInstEntry("h-divide2",      &cHardwareExperimental::Inst_HeadDivide2),
    cInstEntry("h-divide3",      &cHardwareExperimental::Inst_HeadDivide3),
    cInstEntry("h-divide4",      &cHardwareExperimental::Inst_HeadDivide4),
    cInstEntry("h-divide5",      &cHardwareExperimental::Inst_HeadDivide5),
    cInstEntry("h-divide6",      &cHardwareExperimental::Inst_HeadDivide6),
    cInstEntry("h-divide7",      &cHardwareExperimental::Inst_HeadDivide7),
    cInstEntry("h-divide8",      &cHardwareExperimental::Inst_HeadDivide8),
    cInstEntry("h-divide9",      &cHardwareExperimental::Inst_HeadDivide9),
    cInstEntry("h-divide10",     &cHardwareExperimental::Inst_HeadDivide10),
    cInstEntry("h-divide16",     &cHardwareExperimental::Inst_HeadDivide16),
    cInstEntry("h-divide32",     &cHardwareExperimental::Inst_HeadDivide32),
    cInstEntry("h-divide50",     &cHardwareExperimental::Inst_HeadDivide50),
    cInstEntry("h-divide100",    &cHardwareExperimental::Inst_HeadDivide100),
    cInstEntry("h-divide500",    &cHardwareExperimental::Inst_HeadDivide500),
    cInstEntry("h-divide1000",   &cHardwareExperimental::Inst_HeadDivide1000),
    cInstEntry("h-divide5000",   &cHardwareExperimental::Inst_HeadDivide5000),
    cInstEntry("h-divide10000",  &cHardwareExperimental::Inst_HeadDivide10000),
    cInstEntry("h-divide50000",  &cHardwareExperimental::Inst_HeadDivide50000),
    cInstEntry("h-divide0.5",    &cHardwareExperimental::Inst_HeadDivide0_5),
    cInstEntry("h-divide0.1",    &cHardwareExperimental::Inst_HeadDivide0_1),
    cInstEntry("h-divide0.05",   &cHardwareExperimental::Inst_HeadDivide0_05),
    cInstEntry("h-divide0.01",   &cHardwareExperimental::Inst_HeadDivide0_01),
    cInstEntry("h-divide0.001",  &cHardwareExperimental::Inst_HeadDivide0_001),
    
    // High-level instructions
    cInstEntry("repro",      &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-A",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-B",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-C",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-D",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-E",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-F",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-G",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-H",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-I",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-J",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-K",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-L",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-M",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-N",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-O",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-P",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-Q",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-R",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-S",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-T",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-U",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-V",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-W",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-X",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-Y",    &cHardwareExperimental::Inst_Repro),
    cInstEntry("repro-Z",    &cHardwareExperimental::Inst_Repro),

    cInstEntry("IO-repro",   &cHardwareExperimental::Inst_IORepro),
    cInstEntry("put-repro",  &cHardwareExperimental::Inst_PutRepro),
    cInstEntry("putc-repro", &cHardwareExperimental::Inst_PutClearRepro),

    cInstEntry("spawn-deme", &cHardwareExperimental::Inst_SpawnDeme),
    
    // Suicide
    cInstEntry("kazi",	&cHardwareExperimental::Inst_Kazi),
    cInstEntry("kazi5",	&cHardwareExperimental::Inst_Kazi5),
    cInstEntry("die",	&cHardwareExperimental::Inst_Die),
	    
    // Placebo instructions
    // nop-x (included with nops)
    cInstEntry("skip",      &cHardwareExperimental::Inst_Skip)
  };
  
  const int n_size = sizeof(s_n_array)/sizeof(cNOPEntry);
  
  static cString n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size && i < NUM_REGISTERS; i++) {
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
  
  return new tInstLib<cHardwareExperimental::tMethod>(n_size, f_size, n_names, f_names, nop_mods, functions, error, def);
}

cHardwareExperimental::cHardwareExperimental(cWorld* world, cOrganism* in_organism, cInstSet* in_m_inst_set)
: cHardwareBase(world, in_organism, in_m_inst_set)
{
  /* FIXME:  reorganize storage of m_functions.  -- kgn */
  m_functions = s_inst_slib->GetFunctions();
  /**/
  m_memory = in_organism->GetGenome();  // Initialize memory...
  Reset();                            // Setup the rest of the hardware...
}


cHardwareExperimental::cHardwareExperimental(const cHardwareExperimental &hardware_cpu)
: cHardwareBase(hardware_cpu.m_world, hardware_cpu.organism, hardware_cpu.m_inst_set)
, m_functions(hardware_cpu.m_functions)
, m_memory(hardware_cpu.m_memory)
, m_global_stack(hardware_cpu.m_global_stack)
, m_threads(hardware_cpu.m_threads)
, m_thread_id_chart(hardware_cpu.m_thread_id_chart)
, m_cur_thread(hardware_cpu.m_cur_thread)
, m_mal_active(hardware_cpu.m_mal_active)
, m_advance_ip(hardware_cpu.m_advance_ip)
, m_executedmatchstrings(hardware_cpu.m_executedmatchstrings)
#if INSTRUCTION_COSTS
, inst_cost(hardware_cpu.inst_cost)
, inst_ft_cost(hardware_cpu.inst_ft_cost)
#endif
{
}


void cHardwareExperimental::Reset()
{
  m_global_stack.Clear();
  
  // We want to reset to have a single thread.
  m_threads.Resize(1);
  
  // Reset that single thread.
  m_threads[0].Reset(this, 0);
  m_thread_id_chart = 1; // Mark only the first thread as taken...
  m_cur_thread = 0;
  
  m_mal_active = false;
  m_executedmatchstrings = false;
  
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
  
}

void cHardwareExperimental::cLocalThread::operator=(const cLocalThread& in_thread)
{
  m_id = in_thread.m_id;
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i] = in_thread.reg[i];
  for (int i = 0; i < NUM_HEADS; i++) heads[i] = in_thread.heads[i];
  stack = in_thread.stack;
}

void cHardwareExperimental::cLocalThread::Reset(cHardwareBase* in_hardware, int in_id)
{
  m_id = in_id;
  
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i] = 0;
  for (int i = 0; i < NUM_HEADS; i++) heads[i].Reset(in_hardware);
  
  stack.Clear();
  cur_stack = 0;
  cur_head = nHardware::HEAD_IP;
  read_label.Clear();
  next_label.Clear();
}



// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.

void cHardwareExperimental::SingleProcess(cAvidaContext& ctx)
{
  // Mark this organism as running...
  organism->SetRunning(true);
  
  cPhenotype & phenotype = organism->GetPhenotype();
  phenotype.IncTimeUsed();
  phenotype.IncCPUCyclesUsed();

  const int num_threads = GetNumThreads();
  
  // If we have threads turned on and we executed each thread in a single
  // timestep, adjust the number of instructions executed accordingly.
  const int num_inst_exec = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1) ?
num_threads : 1;
  
  for (int i = 0; i < num_inst_exec; i++) {
    // Setup the hardware for the next instruction to be executed.
    ThreadNext();
    m_advance_ip = true;
    IP().Adjust();
    
#if BREAKPOINTS
    if (IP().FlagBreakpoint()) {
      organism->DoBreakpoint();
    }
#endif
    
    // Print the status of this CPU at each step...
    if (m_tracer != NULL) m_tracer->TraceHardware(*this);
    
    // Find the instruction to be executed
    const cInstruction& cur_inst = IP().GetInst();
    
    // Test if costs have been paid and it is okay to execute this now...
    bool exec = SingleProcess_PayCosts(ctx, cur_inst);

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
      
      if (exec == true) SingleProcess_ExecuteInst(ctx, cur_inst);
      
      // Some instruction (such as jump) may turn m_advance_ip off.  Usually
      // we now want to move to the next instruction in the memory.
      if (m_advance_ip == true) IP().Advance();
      
      // Pay the additional death_cost of the instruction now
      phenotype.IncTimeUsed(addl_time_cost);
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
bool cHardwareExperimental::SingleProcess_PayCosts(cAvidaContext& ctx, const cInstruction& cur_inst)
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
  
#endif
  return true;
}

// This method will handle the actuall execution of an instruction
// within single process, once that function has been finalized.
bool cHardwareExperimental::SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst) 
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


void cHardwareExperimental::ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = organism->IsRunning();
  organism->SetRunning(true);
  
  if (m_tracer != NULL) m_tracer->TraceHardware(*this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  organism->SetRunning(prev_run_state);
}


bool cHardwareExperimental::OK()
{
  bool result = true;
  
  if (!m_memory.OK()) result = false;
  
  for (int i = 0; i < GetNumThreads(); i++) {
    if (m_threads[i].stack.OK() == false) result = false;
    if (m_threads[i].next_label.OK() == false) result = false;
  }
  
  return result;
}

void cHardwareExperimental::PrintStatus(ostream& fp)
{
  fp << organism->GetPhenotype().GetCPUCyclesUsed() << " ";
  fp << "IP:" << IP().GetPosition() << "    ";
  
  for (int i = 0; i < NUM_REGISTERS; i++) {
    fp << static_cast<char>('A' + i) << "X:" << GetRegister(i) << " ";
    fp << setbase(16) << "[0x" << GetRegister(i) << "]  " << setbase(10);
  }
  
  // Add some extra information if additional time costs are used for instructions,
  // leave this out if there are no differences to keep it cleaner
  if ( organism->GetPhenotype().GetTimeUsed() != organism->GetPhenotype().GetCPUCyclesUsed() )
  {
    fp << "  AgedTime:" << organism->GetPhenotype().GetTimeUsed();
  }
  fp << endl;
  
  fp << "  R-Head:" << GetHead(nHardware::HEAD_READ).GetPosition() << " "
    << "W-Head:" << GetHead(nHardware::HEAD_WRITE).GetPosition()  << " "
    << "F-Head:" << GetHead(nHardware::HEAD_FLOW).GetPosition()   << "  "
    << "RL:" << GetReadLabel().AsString() << "   "
    << endl;
    
  int number_of_stacks = GetNumStacks();
  for (int stack_id = 0; stack_id < number_of_stacks; stack_id++) {
    fp << ((m_threads[m_cur_thread].cur_stack == stack_id) ? '*' : ' ') << " Stack " << stack_id << ":" << setbase(16) << setfill('0');
    for (int i = 0; i < nHardware::STACK_SIZE; i++) fp << " Ox" << setw(8) << GetStack(i, stack_id, 0);
    fp << setfill(' ') << setbase(10) << endl;
  }
  
  fp << "  Mem (" << GetMemory().GetSize() << "):"
		  << "  " << GetMemory().AsString()
		  << endl;
  fp.flush();
}





/////////////////////////////////////////////////////////////////////////
// Method: cHardwareExperimental::FindLabel(direction)
//
// Search in 'direction' (+ or - 1) from the instruction pointer for the
// compliment of the label in 'next_label' and return a pointer to the
// results.  If direction is 0, search from the beginning of the genome.
//
/////////////////////////////////////////////////////////////////////////

cHeadCPU cHardwareExperimental::FindLabel(int direction)
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

int cHardwareExperimental::FindLabel_Forward(const cCodeLabel & search_label,
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

int cHardwareExperimental::FindLabel_Backward(const cCodeLabel & search_label,
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
cHeadCPU cHardwareExperimental::FindLabel(const cCodeLabel & in_label, int direction)
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


bool cHardwareExperimental::InjectHost(const cCodeLabel & in_label, const cGenome & injection)
{
  // Make sure the genome will be below max size after injection.
  
  const int new_size = injection.GetSize() + GetMemory().GetSize();
  if (new_size > MAX_CREATURE_SIZE) return false; // (inject fails)
  
  const int inject_line = FindLabelFull(in_label).GetPosition();
  
  // Abort if no compliment is found.
  if (inject_line == -1) return false; // (inject fails)
  
  // Inject the code!
  InjectCode(injection, inject_line+1);
  
  return true; // (inject succeeds!)
}

void cHardwareExperimental::InjectCode(const cGenome & inject_code, const int line_num)
{
  assert(line_num >= 0);
  assert(line_num <= m_memory.GetSize());
  assert(m_memory.GetSize() + inject_code.GetSize() < MAX_CREATURE_SIZE);
  
  // Inject the new code.
  const int inject_size = inject_code.GetSize();
  m_memory.Insert(line_num, inject_code);
  
  // Set instruction flags on the injected code
  for (int i = line_num; i < line_num + inject_size; i++) {
    m_memory.SetFlagInjected(i);
  }
  organism->GetPhenotype().IsModified() = true;
  
  // Adjust all of the heads to take into account the new mem size.  
  for (int i = 0; i < NUM_HEADS; i++) {    
    if (GetHead(i).GetPosition() > line_num) GetHead(i).Jump(inject_size);
  }
}


void cHardwareExperimental::ReadInst(const int in_inst)
{
  if (m_inst_set->IsNop( cInstruction(in_inst) )) {
    GetReadLabel().AddNop(in_inst);
  } else {
    GetReadLabel().Clear();
  }
}


void cHardwareExperimental::AdjustHeads()
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

void cHardwareExperimental::ReadLabel(int max_size)
{
  int count = 0;
  cHeadCPU * inst_ptr = &( IP() );
  
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
  const int num_threads = GetNumThreads();
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
  if (GetNumThreads() == 1) return false;
  
  // Note the current thread and set the current back one.
  const int kill_thread = m_cur_thread;
  ThreadPrev();
  
  // Turn off this bit in the m_thread_id_chart...
  m_thread_id_chart ^= 1 << m_threads[kill_thread].GetID();
  
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

////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int cHardwareExperimental::FindModifiedRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_register = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  }
  return default_register;
}

inline int cHardwareExperimental::FindModifiedNextRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_register = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  } else {
    default_register = (default_register + 1) % NUM_REGISTERS;
  }
  return default_register;
}

inline int cHardwareExperimental::FindModifiedPreviousRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_register = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  } else {
    default_register = (default_register + NUM_REGISTERS - 1) % NUM_REGISTERS;
  }
  return default_register;
}


inline int cHardwareExperimental::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.
  
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_head = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  }
  return default_head;
}


inline int cHardwareExperimental::FindNextRegister(int base_reg)
{
  return (base_reg + 1) % NUM_REGISTERS;
}


bool cHardwareExperimental::Allocate_Necro(const int new_size)
{
  GetMemory().ResizeOld(new_size);
  return true;
}

bool cHardwareExperimental::Allocate_Random(cAvidaContext& ctx, const int old_size, const int new_size)
{
  GetMemory().Resize(new_size);
  
  for (int i = old_size; i < new_size; i++) {
    GetMemory()[i] = m_inst_set->GetRandomInst(ctx);
  }
  return true;
}

bool cHardwareExperimental::Allocate_Default(const int new_size)
{
  GetMemory().Resize(new_size);
  
  // New space already defaults to default instruction...
  
  return true;
}

bool cHardwareExperimental::Allocate_Main(cAvidaContext& ctx, const int allocated_size)
{
  // must do divide before second allocate & must allocate positive amount...
  if (m_world->GetConfig().REQUIRE_ALLOCATE.Get() && m_mal_active == true) {
    organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR, "Allocate already active");
    return false;
  }
  if (allocated_size < 1) {
    organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Allocate of %d too small", allocated_size));
    return false;
  }
  
  const int old_size = GetMemory().GetSize();
  const int new_size = old_size + allocated_size;
  
  // Make sure that the new size is in range.
  if (new_size > MAX_CREATURE_SIZE  ||  new_size < MIN_CREATURE_SIZE) {
    organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Invalid post-allocate size (%d)",
                               new_size));
    return false;
  }
  
  const int max_alloc_size = (int) (old_size * m_world->GetConfig().CHILD_SIZE_RANGE.Get());
  if (allocated_size > max_alloc_size) {
    organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
          cStringUtil::Stringf("Allocate too large (%d > %d)",
                               allocated_size, max_alloc_size));
    return false;
  }
  
  const int max_old_size =
    (int) (allocated_size * m_world->GetConfig().CHILD_SIZE_RANGE.Get());
  if (old_size > max_old_size) {
    organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
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

int cHardwareExperimental::GetCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  const cCPUMemory& memory = GetMemory();
  for (int i = parent_size; i < parent_size + child_size; i++) {
    if (memory.FlagCopied(i)) copied_size++;
  }
  return copied_size;
}  


bool cHardwareExperimental::Divide_Main(cAvidaContext& ctx, const int div_point,
                               const int extra_lines, double mut_multiplier)
{
  const int child_size = GetMemory().GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  cGenome & child_genome = organism->ChildGenome();
  child_genome = cGenomeUtil::Crop(m_memory, div_point, div_point+child_size);
  
  // Cut off everything in this memory past the divide point.
  GetMemory().Resize(div_point);
  
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
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child
  bool parent_alive = organism->ActivateDivide(ctx);

  // Do more work if the parent lives through the birth of the offspring
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset();
  }
  
  return true;
}

/*
  Almost the same as Divide_Main, but resamples reverted offspring.

  RESAMPLING ONLY WORKS CORRECTLY WHEN ALL MUTIONS OCCUR ON DIVIDE!!

  AWC - 06/29/06
*/
bool cHardwareExperimental::Divide_MainRS(cAvidaContext& ctx, const int div_point,
                               const int extra_lines, double mut_multiplier)
{

  //cStats stats = m_world->GetStats();
  const int child_size = GetMemory().GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  cGenome & child_genome = organism->ChildGenome();
  child_genome = cGenomeUtil::Crop(m_memory, div_point, div_point+child_size);
  
  // Cut off everything in this memory past the divide point.
  GetMemory().Resize(div_point);
  
  unsigned 
    totalMutations = 0,
    mutations = 0;
    //RScount = 0;


  bool
    fitTest = false;

  // Handle Divide Mutations...
  /*
    Do mutations until one of these conditions are satisified:
     we have resampled X times
     we have an offspring with the same number of muations as the first offspring
      that is not reverted
     the parent is steralized (usually means an implicit mutation)
  */
  for(unsigned i = 0; i <= 100; i++){
    if(i == 0){
      mutations = totalMutations = Divide_DoMutations(ctx, mut_multiplier);
    }
    else{
      mutations = Divide_DoMutations(ctx, mut_multiplier);
      m_world->GetStats().IncResamplings();
    }

    fitTest = Divide_TestFitnessMeasures(ctx);
    
    if(!fitTest && mutations >= totalMutations) break;

  } 
  // think about making this mutations == totalMuations - though this may be too hard...
  /*
  if(RScount > 2)
    cerr << "Resampled " << RScount << endl;
  */
  //org could not be resampled beneath the hard cap -- it is then steraalized
  if(fitTest/*RScount == 11*/) {
    organism->GetPhenotype().ChildFertile() = false;
    m_world->GetStats().IncFailedResamplings();
  }

#if INSTRUCTION_COSTS
  // reset first time instruction costs
  for (int i = 0; i < inst_ft_cost.GetSize(); i++) {
    inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
  }
#endif
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child, and do more work if the parent lives through the
  // birth.
  bool parent_alive = organism->ActivateDivide(ctx);
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset();
  }
  
  return true;
}

/*
  Almost the same as Divide_Main, but only allows for one mutation 
    on divde and resamples reverted offspring.

  RESAMPLING ONLY WORKS CORRECTLY WHEN ALL MUTIONS OCCUR ON DIVIDE!!

  AWC - 07/28/06
*/
bool cHardwareExperimental::Divide_Main1RS(cAvidaContext& ctx, const int div_point,
                               const int extra_lines, double mut_multiplier)
{

  //cStats stats = m_world->GetStats();
  const int child_size = GetMemory().GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  cGenome & child_genome = organism->ChildGenome();
  child_genome = cGenomeUtil::Crop(m_memory, div_point, div_point+child_size);
  
  // Cut off everything in this memory past the divide point.
  GetMemory().Resize(div_point);
  
  unsigned 
    totalMutations = 0,
    mutations = 0;
  //    RScount = 0;

  bool
    fitTest = false;


  // Handle Divide Mutations...
  /*
    Do mutations until one of these conditions are satisified:
     we have resampled X times
     we have an offspring with the same number of muations as the first offspring
      that is not reverted
     the parent is steralized (usually means an implicit mutation)
  */
  for(unsigned i = 0; i < 100; i++){
    if(!i){
      mutations = totalMutations = Divide_DoMutations(ctx, mut_multiplier,1);
    }
    else{
      mutations = Divide_DoExactMutations(ctx, mut_multiplier,1);
      m_world->GetStats().IncResamplings();
    }

    fitTest = Divide_TestFitnessMeasures(ctx);
    //if(mutations > 1 ) cerr << "Too Many mutations!!!!!!!!!!!!!!!" << endl;
    if(!fitTest && mutations >= totalMutations) break;

  } 
  // think about making this mutations == totalMuations - though this may be too hard...
  /*
  if(RScount > 2)
    cerr << "Resampled " << RScount << endl;
  */
  //org could not be resampled beneath the hard cap -- it is then steraalized
  if(fitTest/*RScount == 11*/) {
    organism->GetPhenotype().ChildFertile() = false;
    m_world->GetStats().IncFailedResamplings();
  }

#if INSTRUCTION_COSTS
  // reset first time instruction costs
  for (int i = 0; i < inst_ft_cost.GetSize(); i++) {
    inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
  }
#endif
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child, and do more work if the parent lives through the
  // birth.
  bool parent_alive = organism->ActivateDivide(ctx);
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset();
  }
  
  return true;
}

/*
  Almost the same as Divide_Main, but only allows for one mutation 
    on divde and resamples reverted offspring.

  RESAMPLING ONLY WORKS CORRECTLY WHEN ALL MUTIONS OCCUR ON DIVIDE!!

  AWC - 07/28/06
*/
bool cHardwareExperimental::Divide_Main2RS(cAvidaContext& ctx, const int div_point,
                               const int extra_lines, double mut_multiplier)
{

  //cStats stats = m_world->GetStats();
  const int child_size = GetMemory().GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  cGenome & child_genome = organism->ChildGenome();
  child_genome = cGenomeUtil::Crop(m_memory, div_point, div_point+child_size);
  
  // Cut off everything in this memory past the divide point.
  GetMemory().Resize(div_point);
  
  unsigned 
    totalMutations = 0,
    mutations = 0;
  //    RScount = 0;

  bool
    fitTest = false;


  // Handle Divide Mutations...
  /*
    Do mutations until one of these conditions are satisified:
     we have resampled X times
     we have an offspring with the same number of muations as the first offspring
      that is not reverted
     the parent is steralized (usually means an implicit mutation)
  */
  for(unsigned i = 0; i < 100; i++){
    if(!i){
      mutations = totalMutations = Divide_DoMutations(ctx, mut_multiplier,2);
    }
    else{
      Divide_DoExactMutations(ctx, mut_multiplier,mutations);
      m_world->GetStats().IncResamplings();
    }

    fitTest = Divide_TestFitnessMeasures(ctx);
    //if(mutations > 1 ) cerr << "Too Many mutations!!!!!!!!!!!!!!!" << endl;
    if(!fitTest && mutations >= totalMutations) break;

  } 
  // think about making this mutations == totalMuations - though this may be too hard...
  /*
  if(RScount > 2)
    cerr << "Resampled " << RScount << endl;
  */
  //org could not be resampled beneath the hard cap -- it is then steraalized
  if(fitTest/*RScount == 11*/) {
    organism->GetPhenotype().ChildFertile() = false;
    m_world->GetStats().IncFailedResamplings();
  }

#if INSTRUCTION_COSTS
  // reset first time instruction costs
  for (int i = 0; i < inst_ft_cost.GetSize(); i++) {
    inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
  }
#endif
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child, and do more work if the parent lives through the
  // birth.
  bool parent_alive = organism->ActivateDivide(ctx);
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset();
  }
  
  return true;
}


//////////////////////////
// And the instructions...
//////////////////////////

bool cHardwareExperimental::Inst_If0(cAvidaContext& ctx)          // Execute next if ?bx? ==0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) != 0)  IP().Advance();
  return true; 
}

bool cHardwareExperimental::Inst_IfNot0(cAvidaContext& ctx)       // Execute next if ?bx? != 0.
{ 
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) == 0)  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfEqu(cAvidaContext& ctx)      // Execute next if bx == ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) != GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfNEqu(cAvidaContext& ctx)     // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) == GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfGr0(cAvidaContext& ctx)       // Execute next if ?bx? ! < 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) <= 0)  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfGr(cAvidaContext& ctx)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) <= GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfGrEqu0(cAvidaContext& ctx)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) < 0)  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfGrEqu(cAvidaContext& ctx)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) < GetRegister(op2)) IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLess0(cAvidaContext& ctx)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) >= 0)  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLess(cAvidaContext& ctx)       // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) >=  GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLsEqu0(cAvidaContext& ctx)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) > 0) IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLsEqu(cAvidaContext& ctx)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) >  GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfBit1(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if ((GetRegister(reg_used) & 1) == 0)  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfANotEqB(cAvidaContext& ctx)     // Execute next if AX != BX
{
  if (GetRegister(REG_AX) == GetRegister(REG_BX) )  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfBNotEqC(cAvidaContext& ctx)     // Execute next if BX != CX
{
  if (GetRegister(REG_BX) == GetRegister(REG_CX) )  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfANotEqC(cAvidaContext& ctx)     // Execute next if AX != BX
{
  if (GetRegister(REG_AX) == GetRegister(REG_CX) )  IP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_JumpF(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  
  // If there is no label, jump BX steps.
  if (GetLabel().GetSize() == 0) {
    GetActiveHead().Jump(GetRegister(REG_BX));
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


bool cHardwareExperimental::Inst_JumpB(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  
  // If there is no label, jump BX steps.
  if (GetLabel().GetSize() == 0) {
    GetActiveHead().Jump(GetRegister(REG_BX));
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

bool cHardwareExperimental::Inst_Call(cAvidaContext& ctx)
{
  // Put the starting location onto the stack
  const int location = IP().GetPosition();
  StackPush(location);
  
  // Jump to the compliment label (or by the ammount in the bx register)
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  
  if (GetLabel().GetSize() == 0) {
    IP().Jump(GetRegister(REG_BX));
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

bool cHardwareExperimental::Inst_Return(cAvidaContext& ctx)
{
  IP().Set(StackPop());
  return true;
}

bool cHardwareExperimental::Inst_Throw(cAvidaContext& ctx)
{
  // Only initialize this once to save some time...
  static cInstruction catch_inst = GetInstSet().GetInst(cStringUtil::Stringf("catch"));

  //Look for the label directly (no complement)
  ReadLabel();
    
  cHeadCPU search_head(IP());
  int start_pos = search_head.GetPosition();
  search_head++;
  
  while (start_pos != search_head.GetPosition()) 
  {
    // If we find a catch instruction, compare the NOPs following it
    if (search_head.GetInst() == catch_inst)
    {
      int catch_pos = search_head.GetPosition();
      search_head++;

      // Continue to examine the label after the catch
      //  (1) It ends (=> use the catch!)
      //  (2) It becomes longer than the throw label (=> use the catch!)
      //  (3) We find a NOP that doesnt match the throw (=> DON'T use the catch...)
      
      bool match = true;
      int size_matched = 0;      
      while ( match && m_inst_set->IsNop(search_head.GetInst()) && (size_matched < GetLabel().GetSize()) )
      {
        if ( GetLabel()[size_matched] != m_inst_set->GetNopMod( search_head.GetInst()) ) match = false;
        search_head++;
        size_matched++;
      }
      
      // We found a matching catch instruction
      if (match)
      {
        IP().Set(catch_pos);
        m_advance_ip = false; // Don't automatically move the IP
                              // so we mark the catch as executed.
        return true;
      }
      
      //If we advanced past NOPs during testing, retreat
      if ( !m_inst_set->IsNop(search_head.GetInst()) ) search_head--;
    }
    search_head.Advance();
  }

  return false;
}


bool cHardwareExperimental::Inst_ThrowIfNot0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) == 0) return false;
  return Inst_Throw(ctx);
}

bool cHardwareExperimental::Inst_ThrowIf0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) != 0) return false;
  return Inst_Throw(ctx);
}

bool cHardwareExperimental::Inst_Goto(cAvidaContext& ctx)
{
  // Only initialize this once to save some time...
  static cInstruction label_inst = GetInstSet().GetInst(cStringUtil::Stringf("label"));

  //Look for an EXACT label match after a 'label' instruction
  ReadLabel();
  
  cHeadCPU search_head(IP());
  int start_pos = search_head.GetPosition();
  search_head++;
  
  while (start_pos != search_head.GetPosition()) 
  {
    if (search_head.GetInst() == label_inst)
    {
      int label_pos = search_head.GetPosition();
      search_head++;
      int size_matched = 0;
      bool match = true;
      while ( size_matched < GetLabel().GetSize() )
      {
        if ( !m_inst_set->IsNop(search_head.GetInst()) ) break;
        if ( GetLabel()[size_matched] != m_inst_set->GetNopMod( search_head.GetInst()) ) break;
        if ( !m_inst_set->IsNop(search_head.GetInst()) ) break;

        size_matched++;
        search_head++;
      }
      
      // We found a matching 'label' instruction only if the next 
      // instruction (at the search head now) is also not a NOP
      if ( (size_matched == GetLabel().GetSize()) && !m_inst_set->IsNop(search_head.GetInst()) )
      {
        IP().Set(label_pos);
        m_advance_ip = false; // Don't automatically move the IP
                              // so we mark the catch as executed.
        return true;
      }

      //If we advanced past NOPs during testing, retreat
      if ( !m_inst_set->IsNop(search_head.GetInst()) ) search_head--;
    }
    search_head++;
  }

  return false;
}


bool cHardwareExperimental::Inst_GotoIfNot0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) == 0) return false;
  return Inst_Goto(ctx);
}

bool cHardwareExperimental::Inst_GotoIf0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) != 0) return false;
  return Inst_Goto(ctx);
}


bool cHardwareExperimental::Inst_Pop(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = StackPop();
  return true;
}

bool cHardwareExperimental::Inst_Push(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  StackPush(GetRegister(reg_used));
  return true;
}

bool cHardwareExperimental::Inst_HeadPop(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  GetHead(head_used).Set(StackPop());
  return true;
}

bool cHardwareExperimental::Inst_HeadPush(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  StackPush(GetHead(head_used).GetPosition());
  if (head_used == nHardware::HEAD_IP) {
    GetHead(head_used).Set(GetHead(nHardware::HEAD_FLOW));
    m_advance_ip = false;
  }
  return true;
}


bool cHardwareExperimental::Inst_PopA(cAvidaContext& ctx) { GetRegister(REG_AX) = StackPop(); return true;}
bool cHardwareExperimental::Inst_PopB(cAvidaContext& ctx) { GetRegister(REG_BX) = StackPop(); return true;}
bool cHardwareExperimental::Inst_PopC(cAvidaContext& ctx) { GetRegister(REG_CX) = StackPop(); return true;}

bool cHardwareExperimental::Inst_PushA(cAvidaContext& ctx) { StackPush(GetRegister(REG_AX)); return true;}
bool cHardwareExperimental::Inst_PushB(cAvidaContext& ctx) { StackPush(GetRegister(REG_BX)); return true;}
bool cHardwareExperimental::Inst_PushC(cAvidaContext& ctx) { StackPush(GetRegister(REG_CX)); return true;}

bool cHardwareExperimental::Inst_SwitchStack(cAvidaContext& ctx) { SwitchStack(); return true;}
bool cHardwareExperimental::Inst_FlipStack(cAvidaContext& ctx)   { StackFlip(); return true;}

bool cHardwareExperimental::Inst_Swap(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  nFunctions::Swap(GetRegister(op1), GetRegister(op2));
  return true;
}

bool cHardwareExperimental::Inst_SwapAB(cAvidaContext& ctx)\
{
  nFunctions::Swap(GetRegister(REG_AX), GetRegister(REG_BX)); return true;
}
bool cHardwareExperimental::Inst_SwapBC(cAvidaContext& ctx)
{
  nFunctions::Swap(GetRegister(REG_BX), GetRegister(REG_CX)); return true;
}
bool cHardwareExperimental::Inst_SwapAC(cAvidaContext& ctx)
{
  nFunctions::Swap(GetRegister(REG_AX), GetRegister(REG_CX)); return true;
}

bool cHardwareExperimental::Inst_CopyReg(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindModifiedNextRegister(src);
  GetRegister(dst) = GetRegister(src);
  return true;
}

bool cHardwareExperimental::Inst_CopyRegAB(cAvidaContext& ctx)
{
  GetRegister(REG_AX) = GetRegister(REG_BX);   return true;
}
bool cHardwareExperimental::Inst_CopyRegAC(cAvidaContext& ctx)
{
  GetRegister(REG_AX) = GetRegister(REG_CX);   return true;
}
bool cHardwareExperimental::Inst_CopyRegBA(cAvidaContext& ctx)
{
  GetRegister(REG_BX) = GetRegister(REG_AX);   return true;
}
bool cHardwareExperimental::Inst_CopyRegBC(cAvidaContext& ctx)
{
  GetRegister(REG_BX) = GetRegister(REG_CX);   return true;
}
bool cHardwareExperimental::Inst_CopyRegCA(cAvidaContext& ctx)
{
  GetRegister(REG_CX) = GetRegister(REG_AX);   return true;
}
bool cHardwareExperimental::Inst_CopyRegCB(cAvidaContext& ctx)
{
  GetRegister(REG_CX) = GetRegister(REG_BX);   return true;
}

bool cHardwareExperimental::Inst_Reset(cAvidaContext& ctx)
{
  GetRegister(REG_AX) = 0;
  GetRegister(REG_BX) = 0;
  GetRegister(REG_CX) = 0;
  StackClear();
  return true;
}

bool cHardwareExperimental::Inst_ShiftR(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) >>= 1;
  return true;
}

bool cHardwareExperimental::Inst_ShiftL(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) <<= 1;
  return true;
}

bool cHardwareExperimental::Inst_Bit1(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) |=  1;
  return true;
}

bool cHardwareExperimental::Inst_SetNum(cAvidaContext& ctx)
{
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsInt(NUM_NOPS);
  return true;
}

bool cHardwareExperimental::Inst_ValGrey(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntGreyCode(NUM_NOPS);
  return true;
}

bool cHardwareExperimental::Inst_ValDir(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntDirect(NUM_NOPS);
  return true;
}

bool cHardwareExperimental::Inst_ValAddP(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntAdditivePolynomial(NUM_NOPS);
  return true;
}

bool cHardwareExperimental::Inst_ValFib(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntFib(NUM_NOPS);
  return true;
}

bool cHardwareExperimental::Inst_ValPolyC(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntPolynomialCoefficent(NUM_NOPS);
  return true;
}

bool cHardwareExperimental::Inst_Inc(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) += 1;
  return true;
}

bool cHardwareExperimental::Inst_Dec(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) -= 1;
  return true;
}

bool cHardwareExperimental::Inst_Zero(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = 0;
  return true;
}

bool cHardwareExperimental::Inst_Neg(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindModifiedRegister(src);
  GetRegister(dst) = -GetRegister(src);
  return true;
}

bool cHardwareExperimental::Inst_Square(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindModifiedRegister(src);
  GetRegister(dst) = GetRegister(src) * GetRegister(src);
  return true;
}

bool cHardwareExperimental::Inst_Sqrt(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindModifiedRegister(src);
  const int value = GetRegister(src);
  if (value > 1) GetRegister(dst) = static_cast<int>(sqrt(static_cast<double>(value)));
  else if (value < 0) {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "sqrt: value is negative");
    return false;
  }
  return true;
}

bool cHardwareExperimental::Inst_Log(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindModifiedRegister(src);
  const int value = GetRegister(src);
  if (value >= 1) GetRegister(dst) = static_cast<int>(log(static_cast<double>(value)));
  else if (value < 0) {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "log: value is negative");
    return false;
  }
  return true;
}

bool cHardwareExperimental::Inst_Log10(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindModifiedRegister(src);
  const int value = GetRegister(src);
  if (value >= 1) GetRegister(dst) = static_cast<int>(log10(static_cast<double>(value)));
  else if (value < 0) {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "log10: value is negative");
    return false;
  }
  return true;
}

bool cHardwareExperimental::Inst_Add(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  GetRegister(dst) = GetRegister(op1) + GetRegister(op2);
  return true;
}

bool cHardwareExperimental::Inst_Sub(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  GetRegister(dst) = GetRegister(op1) - GetRegister(op2);
  return true;
}

bool cHardwareExperimental::Inst_Mult(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  GetRegister(dst) = GetRegister(op1) * GetRegister(op2);
  return true;
}

bool cHardwareExperimental::Inst_Div(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op2) != 0) {
    if (0-INT_MAX > GetRegister(op1) && GetRegister(op2) == -1)
      organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: Float exception");
    else
      GetRegister(dst) = GetRegister(op1) / GetRegister(op2);
  } else {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
    return false;
  }
  return true;
}

bool cHardwareExperimental::Inst_Mod(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op2) != 0) {
    GetRegister(dst) = GetRegister(op1) % GetRegister(op2);
  } else {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
    return false;
  }
  return true;
}


bool cHardwareExperimental::Inst_Nand(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  GetRegister(dst) = ~(GetRegister(op1) & GetRegister(op2));
  return true;
}

bool cHardwareExperimental::Inst_Nor(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  GetRegister(dst) = ~(GetRegister(op1) | GetRegister(op2));
  return true;
}

bool cHardwareExperimental::Inst_And(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  GetRegister(dst) = (GetRegister(op1) & GetRegister(op2));
  return true;
}

bool cHardwareExperimental::Inst_Not(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindModifiedRegister(src);
  GetRegister(dst) = ~(GetRegister(src));
  return true;
}

bool cHardwareExperimental::Inst_Order(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) > GetRegister(op2)) {
    nFunctions::Swap(GetRegister(op1), GetRegister(op2));
  }
  return true;
}

bool cHardwareExperimental::Inst_Xor(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  GetRegister(dst) = GetRegister(op1) ^ GetRegister(op2);
  return true;
}

bool cHardwareExperimental::Inst_Copy(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedRegister(REG_AX);

  const cHeadCPU from(this, GetRegister(op1));
  cHeadCPU to(this, GetRegister(op2) + GetRegister(op1));
  sCPUStats& cpu_stats = organism->CPUStats();
  
  if (organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();  // Mark this instruction as mutated...
    to.SetFlagCopyMut();  // Mark this instruction as copy mut...
                              //organism->GetPhenotype().IsMutated() = true;
    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(from.GetInst());
    to.ClearFlagMutated();  // UnMark
    to.ClearFlagCopyMut();  // UnMark
  }
  
  to.SetFlagCopied();  // Set the copied flag.
  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareExperimental::Inst_ReadInst(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_CX);
  const int src = FindModifiedRegister(REG_BX);

  const cHeadCPU from(this, GetRegister(src));
  
  // Dis-allowing mutations on read, for the moment (write only...)
  // @CAO This allows perfect error-correction...
  GetRegister(dst) = from.GetInst().GetOp();
  return true;
}

bool cHardwareExperimental::Inst_WriteInst(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_CX);
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedRegister(REG_AX);

  cHeadCPU to(this, GetRegister(op2) + GetRegister(op1));
  const int value = Mod(GetRegister(src), m_inst_set->GetSize());
  sCPUStats& cpu_stats = organism->CPUStats();

  // Change value on a mutation...
  if (organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();      // Mark this instruction as mutated...
    to.SetFlagCopyMut();      // Mark this instruction as copy mut...
                                  //organism->GetPhenotype().IsMutated() = true;
    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(cInstruction(value));
    to.ClearFlagMutated();     // UnMark
    to.ClearFlagCopyMut();     // UnMark
  }

  to.SetFlagCopied();  // Set the copied flag.
  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareExperimental::Inst_StackReadInst(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_CX);
  cHeadCPU from(this, GetRegister(reg_used));
  StackPush(from.GetInst().GetOp());
  return true;
}

bool cHardwareExperimental::Inst_StackWriteInst(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedRegister(REG_AX);
  cHeadCPU to(this, GetRegister(op1) + GetRegister(dst));
  const int value = Mod(StackPop(), m_inst_set->GetSize());
  sCPUStats& cpu_stats = organism->CPUStats();
  
  // Change value on a mutation...
  if (organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();      // Mark this instruction as mutated...
    to.SetFlagCopyMut();      // Mark this instruction as copy mut...
                                  //organism->GetPhenotype().IsMutated() = true;
    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(cInstruction(value));
    to.ClearFlagMutated();     // UnMark
    to.ClearFlagCopyMut();     // UnMark
  }
  
  to.SetFlagCopied();  // Set the copied flag.
  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareExperimental::Inst_Compare(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_CX);
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedRegister(REG_AX);

  cHeadCPU from(this, GetRegister(op1));
  cHeadCPU to(this, GetRegister(op2) + GetRegister(op1));
  
  // Compare is dangerous -- it can cause mutations!
  if (organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();      // Mark this instruction as mutated...
    to.SetFlagCopyMut();      // Mark this instruction as copy mut...
                                  //organism->GetPhenotype().IsMutated() = true;
  }
  
  GetRegister(dst) = from.GetInst().GetOp() - to.GetInst().GetOp();
  
  return true;
}

bool cHardwareExperimental::Inst_IfNCpy(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedRegister(REG_AX);

  const cHeadCPU from(this, GetRegister(op1));
  const cHeadCPU to(this, GetRegister(op2) + GetRegister(op1));
  
  // Allow for errors in this test...
  if (organism->TestCopyMut(ctx)) {
    if (from.GetInst() != to.GetInst()) IP().Advance();
  } else {
    if (from.GetInst() == to.GetInst()) IP().Advance();
  }
  return true;
}

bool cHardwareExperimental::Inst_Allocate(cAvidaContext& ctx)   // Allocate bx more space...
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindModifiedRegister(REG_AX);
  const int size = GetMemory().GetSize();
  if (Allocate_Main(ctx, GetRegister(src))) {
    GetRegister(dst) = size;
    return true;
  } else return false;
}

bool cHardwareExperimental::Inst_Divide(cAvidaContext& ctx)  
{ 
  const int src = FindModifiedRegister(REG_AX);
  return Divide_Main(ctx, GetRegister(src));    
}

/*
  Divide with resampling -- Same as regular divide but on reversions will be 
  resampled after they are reverted.

  AWC 06/29/06

 */

bool cHardwareExperimental::Inst_DivideRS(cAvidaContext& ctx)  
{ 
  const int src = FindModifiedRegister(REG_AX);
  return Divide_MainRS(ctx, GetRegister(src));    
}


bool cHardwareExperimental::Inst_CDivide(cAvidaContext& ctx) 
{ 
  return Divide_Main(ctx, GetMemory().GetSize() / 2);   
}

bool cHardwareExperimental::Inst_CAlloc(cAvidaContext& ctx)  
{ 
  return Allocate_Main(ctx, GetMemory().GetSize());   
}

bool cHardwareExperimental::Inst_MaxAlloc(cAvidaContext& ctx)   // Allocate maximal more
{
  const int dst = FindModifiedRegister(REG_AX);
  const int cur_size = GetMemory().GetSize();
  const int alloc_size = Min((int) (m_world->GetConfig().CHILD_SIZE_RANGE.Get() * cur_size),
                             MAX_CREATURE_SIZE - cur_size);
  if (Allocate_Main(ctx, alloc_size)) {
    GetRegister(dst) = cur_size;
    return true;
  } else return false;
}

bool cHardwareExperimental::Inst_Transposon(cAvidaContext& ctx)
{
  ReadLabel();
  //organism->GetPhenotype().ActivateTransposon(GetLabel());
  return true;
}

void cHardwareExperimental::Divide_DoTransposons(cAvidaContext& ctx)
{
  // This only works if 'transposon' is in the current instruction set
  static bool transposon_in_use = GetInstSet().InstInSet(cStringUtil::Stringf("transposon"));
  if (!transposon_in_use) return;
  
  static cInstruction transposon_inst = GetInstSet().GetInst(cStringUtil::Stringf("transposon"));
  cCPUMemory& child_genome = organism->ChildGenome();

  // Count the number of transposons that are marked as executed
  int tr_count = 0;
  for (int i=0; i < child_genome.GetSize(); i++) 
  {
    if (child_genome.FlagExecuted(i) && (child_genome[i] == transposon_inst)) tr_count++;
  }
  
  for (int i=0; i < tr_count; i++) 
  {
    if (ctx.GetRandom().P(0.01))
    {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize() + 1);
      child_genome.Insert(mut_line, transposon_inst);
    }
  }
  
  
/*
  const tArray<cCodeLabel> tr = organism->GetPhenotype().GetActiveTransposons();
  cCPUMemory& child_genome = organism->ChildGenome();
  
  for (int i=0; i < tr.GetSize(); i++) 
  {
    if (ctx.GetRandom().P(0.1))
    {
      const unsigned int mut_line = ctx.GetRandom().GetUInt(child_genome.GetSize() + 1);
      child_genome.Insert(mut_line, transposon_inst);
    }
  }
*/  
}

bool cHardwareExperimental::Inst_Repro(cAvidaContext& ctx)
{
  // const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  // these checks should be done, but currently they make some assumptions
  // that crash when evaluating this kind of organism -- JEB

  // Setup child
  cCPUMemory& child_genome = organism->ChildGenome();
  child_genome = GetMemory();
  organism->GetPhenotype().SetLinesCopied(GetMemory().GetSize());

  // JEB Hack
  // Make sure that an organism has accumulated any required bonus
  const int bonus_required = m_world->GetConfig().REQUIRED_BONUS.Get();
  if (organism->GetPhenotype().GetCurBonus() < bonus_required) {
    return false; //  (divide fails)
  }
  
  int lines_executed = 0;
  for ( int i = 0; i < GetMemory().GetSize(); i++ ) {
    if ( GetMemory().FlagExecuted(i)) lines_executed++;
  }
  organism->GetPhenotype().SetLinesExecuted(lines_executed);
  
  // Do transposon movement and copying before other mutations
  Divide_DoTransposons(ctx);
  
  // Perform Copy Mutations...
  if (organism->GetCopyMutProb() > 0) { // Skip this if no mutations....
    for (int i = 0; i < GetMemory().GetSize(); i++) {
      if (organism->TestCopyMut(ctx)) {
        child_genome[i] = m_inst_set->GetRandomInst(ctx);
        //organism->GetPhenotype().IsMutated() = true;
      }
    }
  }
  Divide_DoMutations(ctx);
  
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
  
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) m_advance_ip = false;
  
  organism->ActivateDivide(ctx);
  
  //Reset the parent
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset();

  return true;
}

bool cHardwareExperimental::Inst_IORepro(cAvidaContext& ctx)
{
  // Do normal IO
  Inst_TaskIO(ctx);
  
  // Immediately attempt a repro
  return Inst_Repro(ctx);
}

bool cHardwareExperimental::Inst_PutRepro(cAvidaContext& ctx)
{
  // Do normal IO
  Inst_TaskPut(ctx);
  
  // Immediately attempt a repro
  return Inst_Repro(ctx);
}

bool cHardwareExperimental::Inst_PutClearRepro(cAvidaContext& ctx)
{
  // Do normal IO
  Inst_TaskPutClearInput(ctx);
  
  // Immediately attempt a repro
  return Inst_Repro(ctx);
}


bool cHardwareExperimental::Inst_SpawnDeme(cAvidaContext& ctx)
{
  organism->SpawnDeme();
  return true;
}

bool cHardwareExperimental::Inst_Kazi(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_AX);
  double percentProb = ((double) (GetRegister(reg_used) % 100)) / 100.0;
  if ( ctx.GetRandom().P(percentProb) ) organism->Kaboom(0);
  return true;
}

bool cHardwareExperimental::Inst_Kazi5(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_AX);
  double percentProb = ((double) (GetRegister(reg_used) % 100)) / 100.0;
  if ( ctx.GetRandom().P(percentProb) ) organism->Kaboom(5);
  return true;
}

bool cHardwareExperimental::Inst_Die(cAvidaContext& ctx)
{
  organism->Die();
  return true; 
}

// The inject instruction can be used instead of a divide command, paired
// with an allocate.  Note that for an inject to work, one needs to have a
// broad range for sizes allowed to be allocated.
//
// This command will cut out from read-head to write-head.
// It will then look at the template that follows the command and inject it
// into the complement template found in a neighboring organism.

bool cHardwareExperimental::Inst_Inject(cAvidaContext& ctx)
{
  AdjustHeads();
  const int start_pos = GetHead(nHardware::HEAD_READ).GetPosition();
  const int end_pos = GetHead(nHardware::HEAD_WRITE).GetPosition();
  const int inject_size = end_pos - start_pos;
  
  // Make sure the creature will still be above the minimum size,
  if (inject_size <= 0) {
    organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: no code to inject");
    return false; // (inject fails)
  }
  if (start_pos < MIN_CREATURE_SIZE) {
    organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: new size too small");
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
    organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: label required");
    return false; // (inject fails)
  }
  
  // Search for the label in the host...
  GetLabel().Rotate(1, NUM_NOPS);
  
  const bool inject_signal = host_organism->GetHardware().InjectHost(GetLabel(), inject_code);
  if (inject_signal) {
    organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_WARNING, "inject: host too large.");
    return false; // Inject failed.
  }
  
  // Set the relevent flags.
  organism->GetPhenotype().IsModifier() = true;
  
  return inject_signal;
}


bool cHardwareExperimental::Inst_InjectRand(cAvidaContext& ctx)
{
  // Rotate to a random facing and then run the normal inject instruction
  const int num_neighbors = organism->GetNeighborhoodSize();
  organism->Rotate(ctx.GetRandom().GetUInt(num_neighbors));
  Inst_Inject(ctx);
  return true;
}

// The inject instruction can be used instead of a divide command, paired
// with an allocate.  Note that for an inject to work, one needs to have a
// broad range for sizes allowed to be allocated.
//
// This command will cut out from read-head to write-head.
// It will then look at the template that follows the command and inject it
// into the complement template found in a neighboring organism.

bool cHardwareExperimental::Inst_InjectThread(cAvidaContext& ctx)
{
  AdjustHeads();
  const int start_pos = GetHead(nHardware::HEAD_READ).GetPosition();
  const int end_pos = GetHead(nHardware::HEAD_WRITE).GetPosition();
  const int inject_size = end_pos - start_pos;
  
  // Make sure the creature will still be above the minimum size,
  if (inject_size <= 0) {
    organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: no code to inject");
    return false; // (inject fails)
  }
  if (start_pos < MIN_CREATURE_SIZE) {
    organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: new size too small");
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
    organism->Fault(FAULT_LOC_INJECT, FAULT_TYPE_ERROR, "inject: label required");
    return false; // (inject fails)
  }
  
  // Search for the label in the host...
  GetLabel().Rotate(1, NUM_NOPS);
  
  if (host_organism->GetHardware().InjectHost(GetLabel(), inject_code)) {
    if (ForkThread()) organism->GetPhenotype().IsMultiThread() = true;
  }
  
  // Set the relevent flags.
  organism->GetPhenotype().IsModifier() = true;
  
  return true;
}

bool cHardwareExperimental::Inst_TaskGet(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_CX);
  const int value = organism->GetNextInput();
  GetRegister(reg_used) = value;
  organism->DoInput(value);
  return true;
}

bool cHardwareExperimental::Inst_TaskStackGet(cAvidaContext& ctx)
{
  const int value = organism->GetNextInput();
  StackPush(value);
  organism->DoInput(value);
  return true;
}

bool cHardwareExperimental::Inst_TaskStackLoad(cAvidaContext& ctx)
{
  // @DMB - TODO: this should look at the input_size...
  for (int i = 0; i < 3; i++) 
    StackPush( organism->GetNextInput() );
  return true;
}

bool cHardwareExperimental::Inst_TaskPut(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = GetRegister(reg_used);
  GetRegister(reg_used) = 0;
  organism->DoOutput(ctx, value);
  return true;
}

bool cHardwareExperimental::Inst_TaskPutClearInput(cAvidaContext& ctx)
{
  bool return_value = Inst_TaskPut(ctx);
  organism->ClearInput();
  return return_value;
}

bool cHardwareExperimental::Inst_TaskPutBonusCost2(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = GetRegister(reg_used);
  GetRegister(reg_used) = 0;
  organism->DoOutput(ctx, value);
  double new_bonus = organism->GetPhenotype().GetCurBonus();
  new_bonus *= 0.5;
//if (new_bonus < 1) new_bonus = 1;
  organism->GetPhenotype().SetCurBonus(new_bonus);
  return true;
}

bool cHardwareExperimental::Inst_TaskPutMeritCost2(cAvidaContext& ctx)
{
  // Normal put code
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = GetRegister(reg_used);
  GetRegister(reg_used) = 0;
  organism->DoOutput(ctx, value);
  
  // Immediately half the merit of the current organism, never going below 1
  double new_merit = organism->GetPhenotype().GetMerit().GetDouble();
  new_merit /= 2;
  if (new_merit < 1) new_merit = 1;
  
  // Immediately re-initialize the time-slice for this organism.  
  organism->UpdateMerit(new_merit);
  
  return true;
}

bool cHardwareExperimental::Inst_TaskIO(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  
  // Do the "put" component
  const int value_out = GetRegister(reg_used);
  organism->DoOutput(ctx, value_out);  // Check for tasks completed.
  
  // Do the "get" component
  const int value_in = organism->GetNextInput();
  GetRegister(reg_used) = value_in;
  organism->DoInput(value_in);
  return true;
}

bool cHardwareExperimental::Inst_TaskIO_Feedback(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);

  //check cur_bonus before the output
  double preOutputBonus = organism->GetPhenotype().GetCurBonus();
  
  // Do the "put" component
  const int value_out = GetRegister(reg_used);
  organism->DoOutput(ctx, value_out);  // Check for tasks completed.

  //check cur_merit after the output
  double postOutputBonus = organism->GetPhenotype().GetCurBonus(); 
  
  
  //push the effect of the IO on merit (+,0,-) to the active stack

  if (preOutputBonus > postOutputBonus){
    StackPush(-1);
    }
  else if (preOutputBonus == postOutputBonus){
    StackPush(0);
    }
  else if (preOutputBonus < postOutputBonus){
    StackPush(1);
    }
  else {
    assert(0);
    //Bollocks. There was an error.
    }


  


  
  // Do the "get" component
  const int value_in = organism->GetNextInput();
  GetRegister(reg_used) = value_in;
  organism->DoInput(value_in);
  return true;
}

bool cHardwareExperimental::Inst_MatchStrings(cAvidaContext& ctx)
{
	if (m_executedmatchstrings)
		return false;
	organism->DoOutput(ctx, 357913941);
	m_executedmatchstrings = true;
	return true;
}

bool cHardwareExperimental::Inst_Sell(cAvidaContext& ctx)
{
	int search_label = GetLabel().AsInt(3) % MARKET_SIZE;
	int send_value = GetRegister(REG_BX);
	int sell_price = m_world->GetConfig().SELL_PRICE.Get();
	organism->SellValue(send_value, search_label, sell_price);
	return true;
}

bool cHardwareExperimental::Inst_Buy(cAvidaContext& ctx)
{
	int search_label = GetLabel().AsInt(3) % MARKET_SIZE;
	int buy_price = m_world->GetConfig().BUY_PRICE.Get();
	GetRegister(REG_BX) = organism->BuyValue(search_label, buy_price);
	return true;
}

bool cHardwareExperimental::Inst_Send(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  organism->SendValue(GetRegister(reg_used));
  GetRegister(reg_used) = 0;
  return true;
}

bool cHardwareExperimental::Inst_Receive(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = organism->ReceiveValue();
  return true;
}

bool cHardwareExperimental::Inst_SenseLog2(cAvidaContext& ctx)
{
  return DoSense(ctx, 0, 2);
}

bool cHardwareExperimental::Inst_SenseUnit(cAvidaContext& ctx)
{
  return DoSense(ctx, 1, 1);
}

bool cHardwareExperimental::Inst_SenseMult100(cAvidaContext& ctx)
{
  return DoSense(ctx, 1, 100);
}

bool cHardwareExperimental::DoSense(cAvidaContext& ctx, int conversion_method, double base)
{
  // Returns the log2 amount of a resource or resources 
  // specified by modifying NOPs into register BX
  const tArray<double> & res_count = organism->GetOrgInterface().GetResources();

  // Arbitrarily set to BX since the conditionals use this directly.
  int reg_to_set = REG_BX;

  // There are no resources, return
  if (res_count.GetSize() == 0) return false;

  // Only recalculate logs if these values have changed
  static int last_num_resources = 0;
  static int max_label_length = 0;
  int num_nops = GetInstSet().GetNumNops();
  
  if ((last_num_resources != res_count.GetSize()))
  {
      max_label_length = (int) ceil(log((double)res_count.GetSize())/log((double)num_nops));
      last_num_resources = res_count.GetSize();
  }

  // Convert modifying NOPs to the index of the resource.
  // If there are fewer than the number of NOPs required
  // to uniquely specify a resource, then add together
  // a subset of resources (motivation: regulation can evolve
  // to be more specific if there is an advantage)
   
  // Find the maximum number of NOPs needed to specify this number of resources
  // Note: It's a bit wasteful to recalculate this every time and organisms will
  // definitely be confused if the number of resources changes during a run
  // because their mapping to resources will be disrupted
  
  // Attempt to read a label with this maximum length
  cHardwareExperimental::ReadLabel(max_label_length);
  
  // Find the length of the label that we actually obtained (max is max_reg_needed)
  int real_label_length = GetLabel().GetSize();
  
  // Start and end labels to define the start and end indices of  
  // resources that we need to add together
  cCodeLabel start_label = cCodeLabel(GetLabel());
  cCodeLabel   end_label = cCodeLabel(GetLabel());
  
  for (int i = 0; i < max_label_length - real_label_length; i++)
  {
    start_label.AddNop(0);
    end_label.AddNop(num_nops-1);
  }
  
  int start_index = start_label.AsInt(num_nops);
  int   end_index =   end_label.AsInt(num_nops);

  // If the label refers to ONLY resources that 
  // do not exist, then the operation fails
  if (start_index >= res_count.GetSize()) return false;

  // Otherwise sum all valid resources that it might refer to
  // (this will only be ONE if the label was of the maximum length).
  int resource_result = 0;
  for (int i = start_index; i <= end_index; i++)
  {
    // if it's a valid resource
    if (i < res_count.GetSize())
    {
      if (conversion_method == 0) // Log2
      {
        // (alternately you could assign min_int for zero resources, but
        // that would cause wierdness when adding sense values together)
        if (res_count[i] > 0) resource_result += (int)(log(res_count[i])/log(base));
      }
      else if (conversion_method == 1) // Addition of multiplied resource amount
      {
        int add_amount = (int) (res_count[i] * base);
        // Do some range checking to make sure we don't overflow
        resource_result = (INT_MAX - resource_result <= add_amount) ? INT_MAX : resource_result + add_amount;
      }
    } 
  }
    
  //Dump this value into an arbitrary register: BX
  GetRegister(reg_to_set) = resource_result;
  
  //We have to convert this to a different index that includes all degenerate labels possible: shortest to longest
  int sensed_index = 0;
  int on = 1;
  for (int i = 0; i < real_label_length; i++)
  {
    sensed_index += on;
    on *= num_nops;
  }
  sensed_index+= GetLabel().AsInt(num_nops);
  organism->GetPhenotype().IncSenseCount(sensed_index);
  
  return true; 

  // Note that we are converting <double> resources to <int> register values
}

void cHardwareExperimental::DoDonate(cOrganism* to_org)
{
  assert(to_org != NULL);
  
  const double merit_given = m_world->GetConfig().DONATE_SIZE.Get();
  const double merit_received =
    merit_given * m_world->GetConfig().DONATE_MULT.Get();
  
  double cur_merit = organism->GetPhenotype().GetMerit().GetDouble();
  cur_merit -= merit_given; 
  
  // Plug the current merit back into this organism and notify the scheduler.
  organism->UpdateMerit(cur_merit);
  
  // Update the merit of the organism being donated to...
  double other_merit = to_org->GetPhenotype().GetMerit().GetDouble();
  other_merit += merit_received;
  to_org->UpdateMerit(other_merit);
}

bool cHardwareExperimental::Inst_DonateRandom(cAvidaContext& ctx)
{
  organism->GetPhenotype().IncDonates();
  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }
  
  // Turn to a random neighbor, get it, and turn back...
  int neighbor_id = ctx.GetRandom().GetInt(organism->GetNeighborhoodSize());
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(-1);
  
  // Donate only if we have found a neighbor.
  if (neighbor != NULL) DoDonate(neighbor);
  
  return true;
}


bool cHardwareExperimental::Inst_DonateKin(cAvidaContext& ctx)
{
  organism->GetPhenotype().IncDonates();
  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }
  
  // Find the target as the first Kin found in the neighborhood.
  const int num_neighbors = organism->GetNeighborhoodSize();
  
  // Turn to face a random neighbor
  int neighbor_id = ctx.GetRandom().GetInt(num_neighbors);
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();
  
  // If there is no max distance, just take the random neighbor we're facing.
  const int max_dist = m_world->GetConfig().MAX_DONATE_KIN_DIST.Get();
  if (max_dist != -1) {
    int max_id = neighbor_id + num_neighbors;
    bool found = false;
    cGenotype* genotype = organism->GetGenotype();
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

bool cHardwareExperimental::Inst_DonateEditDist(cAvidaContext& ctx)
{
  organism->GetPhenotype().IncDonates();
  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }
  
  // Find the target as the first Kin found in the neighborhood.
  const int num_neighbors = organism->GetNeighborhoodSize();
  
  // Turn to face a random neighbor
  int neighbor_id = ctx.GetRandom().GetInt(num_neighbors);
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism* neighbor = organism->GetNeighbor();
  
  // If there is no max edit distance, take the random neighbor we're facing.
  const int max_dist = m_world->GetConfig().MAX_DONATE_EDIT_DIST.Get();
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


bool cHardwareExperimental::Inst_DonateNULL(cAvidaContext& ctx)
{
  organism->GetPhenotype().IncDonates();
  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }
  
  // This is a fake donate command that causes the organism to lose merit,
  // but no one else to gain any.
  
  const double merit_given = m_world->GetConfig().DONATE_SIZE.Get();
  double cur_merit = organism->GetPhenotype().GetMerit().GetDouble();
  cur_merit -= merit_given;
  
  // Plug the current merit back into this organism and notify the scheduler.
  organism->UpdateMerit(cur_merit);
  
  return true;
}


bool cHardwareExperimental::Inst_SearchF(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  const int search_size = FindLabel(1).GetPosition() - IP().GetPosition();
  GetRegister(REG_BX) = search_size;
  GetRegister(REG_CX) = GetLabel().GetSize();
  return true;
}

bool cHardwareExperimental::Inst_SearchB(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  const int search_size = IP().GetPosition() - FindLabel(-1).GetPosition();
  GetRegister(REG_BX) = search_size;
  GetRegister(REG_CX) = GetLabel().GetSize();
  return true;
}

bool cHardwareExperimental::Inst_MemSize(cAvidaContext& ctx)
{
  GetRegister(FindModifiedRegister(REG_BX)) = GetMemory().GetSize();
  return true;
}


bool cHardwareExperimental::Inst_RotateL(cAvidaContext& ctx)
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
    cOrganism* neighbor = organism->GetNeighbor();
    
    if (neighbor != NULL && neighbor->GetHardware().FindLabelFull(GetLabel()).InMemory()) return true;
    
    // Otherwise keep rotating...
    organism->Rotate(-1);
  }
  return true;
}

bool cHardwareExperimental::Inst_RotateR(cAvidaContext& ctx)
{
  const int num_neighbors = organism->GetNeighborhoodSize();
  
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  ReadLabel();
  
  // Always rotate at least once.
  organism->Rotate(1);
  
  // If there is no label, then the one rotation was all we want.
  if (!GetLabel().GetSize()) return true;
  
  // Rotate until a complement label is found (or all have been checked).
  GetLabel().Rotate(1, NUM_NOPS);
  for (int i = 1; i < num_neighbors; i++) {
    cOrganism* neighbor = organism->GetNeighbor();
    
    if (neighbor != NULL && neighbor->GetHardware().FindLabelFull(GetLabel()).InMemory()) return true;
    
    // Otherwise keep rotating...
    organism->Rotate(1);
  }
  return true;
}

bool cHardwareExperimental::Inst_SetCopyMut(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int new_mut_rate = Max(GetRegister(reg_used), 1 );
  organism->SetCopyMutProb(static_cast<double>(new_mut_rate) / 10000.0);
  return true;
}

bool cHardwareExperimental::Inst_ModCopyMut(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const double new_mut_rate = organism->GetCopyMutProb() + static_cast<double>(GetRegister(reg_used)) / 10000.0;
  if (new_mut_rate > 0.0) organism->SetCopyMutProb(new_mut_rate);
  return true;
}


// Multi-threading.

bool cHardwareExperimental::Inst_ForkThread(cAvidaContext& ctx)
{
  IP().Advance();
  if (!ForkThread()) organism->Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
  return true;
}

bool cHardwareExperimental::Inst_KillThread(cAvidaContext& ctx)
{
  if (!KillThread()) organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  else m_advance_ip = false;
  return true;
}

bool cHardwareExperimental::Inst_ThreadID(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = GetCurThreadID();
  return true;
}


// Head-based instructions

bool cHardwareExperimental::Inst_SetHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  m_threads[m_cur_thread].cur_head = static_cast<unsigned char>(head_used);
  return true;
}

bool cHardwareExperimental::Inst_AdvanceHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_WRITE);
  GetHead(head_used).Advance();
  return true;
}

bool cHardwareExperimental::Inst_MoveHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = FindModifiedHead(nHardware::HEAD_FLOW);
  GetHead(head_used).Set(GetHead(target));
  if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  return true;
}

bool cHardwareExperimental::Inst_JumpHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  GetHead(head_used).Jump(GetRegister(REG_CX) );
  // JEB - probably shouldn't advance inst ptr after jumping here?
  // Any negative number jumps to the beginning of the genome (pos 0)
  // and then we immediately advance past that first instruction.
  return true;
}

bool cHardwareExperimental::Inst_GetHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  GetRegister(REG_CX) = GetHead(head_used).GetPosition();
  return true;
}

bool cHardwareExperimental::Inst_IfLabel(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel())  IP().Advance();
  return true;
}

// This is a variation on IfLabel that will skip the next command if the "if"
// is false, but it will also skip all nops following that command.
bool cHardwareExperimental::Inst_IfLabel2(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel()) {
    IP().Advance();
    if (m_inst_set->IsNop( IP().GetNextInst() ))  IP().Advance();
  }
  return true;
}

bool cHardwareExperimental::Inst_HeadDivideMut(cAvidaContext& ctx, double mut_multiplier)
{
  AdjustHeads();
  const int divide_pos = GetHead(nHardware::HEAD_READ).GetPosition();
  int child_end =  GetHead(nHardware::HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = GetMemory().GetSize();
  const int extra_lines = GetMemory().GetSize() - child_end;
  bool ret_val = Divide_Main(ctx, divide_pos, extra_lines, mut_multiplier);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val; 
}

bool cHardwareExperimental::Inst_HeadDivide(cAvidaContext& ctx)
{
  return Inst_HeadDivideMut(ctx, 1);
  
}

/*
  Resample Divide -- AWC 06/29/06
*/

bool cHardwareExperimental::Inst_HeadDivideRS(cAvidaContext& ctx)
{
  AdjustHeads();
  const int divide_pos = GetHead(nHardware::HEAD_READ).GetPosition();
  int child_end =  GetHead(nHardware::HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = GetMemory().GetSize();
  const int extra_lines = GetMemory().GetSize() - child_end;
  bool ret_val = Divide_MainRS(ctx, divide_pos, extra_lines, 1);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val; 
}

/*
  Resample Divide -- single mut on divide-- AWC 07/28/06
*/

bool cHardwareExperimental::Inst_HeadDivide1RS(cAvidaContext& ctx)
{
  AdjustHeads();
  const int divide_pos = GetHead(nHardware::HEAD_READ).GetPosition();
  int child_end =  GetHead(nHardware::HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = GetMemory().GetSize();
  const int extra_lines = GetMemory().GetSize() - child_end;
  bool ret_val = Divide_Main1RS(ctx, divide_pos, extra_lines, 1);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val; 
}

/*
  Resample Divide -- double mut on divide-- AWC 08/29/06
*/

bool cHardwareExperimental::Inst_HeadDivide2RS(cAvidaContext& ctx)
{
  AdjustHeads();
  const int divide_pos = GetHead(nHardware::HEAD_READ).GetPosition();
  int child_end =  GetHead(nHardware::HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = GetMemory().GetSize();
  const int extra_lines = GetMemory().GetSize() - child_end;
  bool ret_val = Divide_Main2RS(ctx, divide_pos, extra_lines, 1);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val; 
}


bool cHardwareExperimental::Inst_HeadDivideSex(cAvidaContext& ctx)  
{ 
  organism->GetPhenotype().SetDivideSex(true);
  organism->GetPhenotype().SetCrossNum(1);
  return Inst_HeadDivide(ctx); 
}

bool cHardwareExperimental::Inst_HeadDivideAsex(cAvidaContext& ctx)  
{ 
  organism->GetPhenotype().SetDivideSex(false);
  organism->GetPhenotype().SetCrossNum(0);
  return Inst_HeadDivide(ctx); 
}

bool cHardwareExperimental::Inst_HeadDivideAsexWait(cAvidaContext& ctx)  
{ 
  organism->GetPhenotype().SetDivideSex(true);
  organism->GetPhenotype().SetCrossNum(0);
  return Inst_HeadDivide(ctx); 
}

bool cHardwareExperimental::Inst_HeadDivideMateSelect(cAvidaContext& ctx)  
{ 
  // Take the label that follows this divide and use it as the ID for which
  // other organisms this one is willing to mate with.
  ReadLabel();
  organism->GetPhenotype().SetMateSelectID( GetLabel().AsInt(NUM_NOPS) );
  
  // Proceed as normal with the rest of mate selection.
  organism->GetPhenotype().SetDivideSex(true);
  organism->GetPhenotype().SetCrossNum(1);
  return Inst_HeadDivide(ctx); 
}

bool cHardwareExperimental::Inst_HeadDivide1(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 1); }
bool cHardwareExperimental::Inst_HeadDivide2(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 2); }
bool cHardwareExperimental::Inst_HeadDivide3(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 3); }
bool cHardwareExperimental::Inst_HeadDivide4(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 4); }
bool cHardwareExperimental::Inst_HeadDivide5(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 5); }
bool cHardwareExperimental::Inst_HeadDivide6(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 6); }
bool cHardwareExperimental::Inst_HeadDivide7(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 7); }
bool cHardwareExperimental::Inst_HeadDivide8(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 8); }
bool cHardwareExperimental::Inst_HeadDivide9(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 9); }
bool cHardwareExperimental::Inst_HeadDivide10(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 10); }
bool cHardwareExperimental::Inst_HeadDivide16(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 16); }
bool cHardwareExperimental::Inst_HeadDivide32(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 32); }
bool cHardwareExperimental::Inst_HeadDivide50(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 50); }
bool cHardwareExperimental::Inst_HeadDivide100(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 100); }
bool cHardwareExperimental::Inst_HeadDivide500(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 500); }
bool cHardwareExperimental::Inst_HeadDivide1000(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 1000); }
bool cHardwareExperimental::Inst_HeadDivide5000(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 5000); }
bool cHardwareExperimental::Inst_HeadDivide10000(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 10000); }
bool cHardwareExperimental::Inst_HeadDivide50000(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 50000); }
bool cHardwareExperimental::Inst_HeadDivide0_5(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.5); }
bool cHardwareExperimental::Inst_HeadDivide0_1(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.1); }
bool cHardwareExperimental::Inst_HeadDivide0_05(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.05); }
bool cHardwareExperimental::Inst_HeadDivide0_01(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.01); }
bool cHardwareExperimental::Inst_HeadDivide0_001(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.001); }

bool cHardwareExperimental::Inst_HeadRead(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  
  const int head_id = FindModifiedHead(nHardware::HEAD_READ);
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
  GetRegister(dst) = read_inst;
  ReadInst(read_inst);
  
  cpu_stats.mut_stats.copies_exec++;  // @CAO, this too..
  GetHead(head_id).Advance();
  return true;
}

bool cHardwareExperimental::Inst_HeadWrite(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int head_id = FindModifiedHead(nHardware::HEAD_WRITE);
  cHeadCPU& active_head = GetHead(head_id);
  
  active_head.Adjust();
  
  int value = GetRegister(src);
  if (value < 0 || value >= m_inst_set->GetSize()) value = 0;
  
  active_head.SetInst(cInstruction(value));
  active_head.SetFlagCopied();
  
  // Advance the head after write...
  active_head++;
  return true;
}

bool cHardwareExperimental::Inst_HeadCopy(cAvidaContext& ctx)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU& read_head = GetHead(nHardware::HEAD_READ);
  cHeadCPU& write_head = GetHead(nHardware::HEAD_WRITE);
  sCPUStats& cpu_stats = organism->CPUStats();
  
  read_head.Adjust();
  write_head.Adjust();
  
  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  ReadInst(read_inst.GetOp());
  if (organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx);
    cpu_stats.mut_stats.copy_mut_count++; 
    write_head.SetFlagMutated();
    write_head.SetFlagCopyMut();
  }
  
  cpu_stats.mut_stats.copies_exec++;
  
  write_head.SetInst(read_inst);
  write_head.SetFlagCopied();  // Set the copied flag...
  
  read_head.Advance();
  write_head.Advance();
  return true;
}

bool cHardwareExperimental::HeadCopy_ErrorCorrect(cAvidaContext& ctx, double reduction)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU & read_head = GetHead(nHardware::HEAD_READ);
  cHeadCPU & write_head = GetHead(nHardware::HEAD_WRITE);
  sCPUStats & cpu_stats = organism->CPUStats();
  
  read_head.Adjust();
  write_head.Adjust();
  
  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  ReadInst(read_inst.GetOp());
  if ( ctx.GetRandom().P(organism->GetCopyMutProb() / reduction) ) {
    read_inst = m_inst_set->GetRandomInst(ctx);
    cpu_stats.mut_stats.copy_mut_count++; 
    write_head.SetFlagMutated();
    write_head.SetFlagCopyMut();
    //organism->GetPhenotype().IsMutated() = true;
  }
  
  cpu_stats.mut_stats.copies_exec++;
  
  write_head.SetInst(read_inst);
  write_head.SetFlagCopied();  // Set the copied flag...
  
  read_head.Advance();
  write_head.Advance();
  return true;
}

bool cHardwareExperimental::Inst_HeadCopy2(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 2); }
bool cHardwareExperimental::Inst_HeadCopy3(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 3); }
bool cHardwareExperimental::Inst_HeadCopy4(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 4); }
bool cHardwareExperimental::Inst_HeadCopy5(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 5); }
bool cHardwareExperimental::Inst_HeadCopy6(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 6); }
bool cHardwareExperimental::Inst_HeadCopy7(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 7); }
bool cHardwareExperimental::Inst_HeadCopy8(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 8); }
bool cHardwareExperimental::Inst_HeadCopy9(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 9); }
bool cHardwareExperimental::Inst_HeadCopy10(cAvidaContext& ctx) { return HeadCopy_ErrorCorrect(ctx, 10); }

bool cHardwareExperimental::Inst_HeadSearch(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabel(0);
  const int search_size = found_pos.GetPosition() - IP().GetPosition();
  GetRegister(REG_BX) = search_size;
  GetRegister(REG_CX) = GetLabel().GetSize();
  GetHead(nHardware::HEAD_FLOW).Set(found_pos);
  GetHead(nHardware::HEAD_FLOW).Advance();
  return true; 
}

bool cHardwareExperimental::Inst_SetFlow(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_CX);
  GetHead(nHardware::HEAD_FLOW).Set(GetRegister(reg_used));
return true; 
}

//// Placebo insts ////
bool cHardwareExperimental::Inst_Skip(cAvidaContext& ctx)
{
  IP().Advance();
  return true;
}

