/*
 *  cHardwareCPU.cc
 *  Avida
 *
 *  Called "hardware_cpu.cc" prior to 11/17/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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


#include "cHardwareCPU.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "functions.h"
#include "cGenomeUtil.h"
#include "cGenotype.h"
#include "cHardwareManager.h"
#include "cHardwareTracer.h"
#include "cInstSet.h"
#include "cMutation.h"
#include "cMutationLib.h"
#include "nMutation.h"
#include "cOrganism.h"
#include "cOrgMessage.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStringUtil.h"
#include "cTestCPU.h"
#include "cWorldDriver.h"
#include "cWorld.h"
#include "tInstLibEntry.h"

#include <climits>
#include <fstream>

using namespace std;


tInstLib<cHardwareCPU::tMethod>* cHardwareCPU::s_inst_slib = cHardwareCPU::initInstLib();

tInstLib<cHardwareCPU::tMethod>* cHardwareCPU::initInstLib(void)
{
  struct cNOPEntryCPU {
    cString name;
    int nop_mod;
    cNOPEntryCPU(const cString &name, int nop_mod)
      : name(name), nop_mod(nop_mod) {}
  };
  static const cNOPEntryCPU s_n_array[] = {
    cNOPEntryCPU("nop-A", REG_AX),
    cNOPEntryCPU("nop-B", REG_BX),
    cNOPEntryCPU("nop-C", REG_CX)
  };
  
  static const tInstLibEntry<tMethod> s_f_array[] = {
    /*
     Note: all entries of cNOPEntryCPU s_n_array must have corresponding
     in the same order in tInstLibEntry<tMethod> s_f_array, and these entries must
     be the first elements of s_f_array.
     */
    tInstLibEntry<tMethod>("nop-A", &cHardwareCPU::Inst_Nop, (nInstFlag::DEFAULT | nInstFlag::NOP), "No-operation instruction; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-B", &cHardwareCPU::Inst_Nop, (nInstFlag::DEFAULT | nInstFlag::NOP), "No-operation instruction; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-C", &cHardwareCPU::Inst_Nop, (nInstFlag::DEFAULT | nInstFlag::NOP), "No-operation instruction; modifies other instructions"),
    
    tInstLibEntry<tMethod>("nop-X", &cHardwareCPU::Inst_Nop, 0, "True no-operation instruction: does nothing"),
    tInstLibEntry<tMethod>("if-equ-0", &cHardwareCPU::Inst_If0, 0, "Execute next instruction if ?BX?==0, else skip it"),
    tInstLibEntry<tMethod>("if-not-0", &cHardwareCPU::Inst_IfNot0, 0, "Execute next instruction if ?BX?!=0, else skip it"),
    tInstLibEntry<tMethod>("if-n-equ", &cHardwareCPU::Inst_IfNEqu, nInstFlag::DEFAULT, "Execute next instruction if ?BX?!=?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-equ", &cHardwareCPU::Inst_IfEqu, 0, "Execute next instruction if ?BX?==?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-grt-0", &cHardwareCPU::Inst_IfGr0),
    tInstLibEntry<tMethod>("if-grt", &cHardwareCPU::Inst_IfGr),
    tInstLibEntry<tMethod>("if->=-0", &cHardwareCPU::Inst_IfGrEqu0),
    tInstLibEntry<tMethod>("if->=", &cHardwareCPU::Inst_IfGrEqu),
    tInstLibEntry<tMethod>("if-les-0", &cHardwareCPU::Inst_IfLess0),
    tInstLibEntry<tMethod>("if-less", &cHardwareCPU::Inst_IfLess, nInstFlag::DEFAULT, "Execute next instruction if ?BX? < ?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-<=-0", &cHardwareCPU::Inst_IfLsEqu0),
    tInstLibEntry<tMethod>("if-<=", &cHardwareCPU::Inst_IfLsEqu),
    tInstLibEntry<tMethod>("if-A!=B", &cHardwareCPU::Inst_IfANotEqB),
    tInstLibEntry<tMethod>("if-B!=C", &cHardwareCPU::Inst_IfBNotEqC),
    tInstLibEntry<tMethod>("if-A!=C", &cHardwareCPU::Inst_IfANotEqC),
    tInstLibEntry<tMethod>("if-bit-1", &cHardwareCPU::Inst_IfBit1),
    
    tInstLibEntry<tMethod>("jump-f", &cHardwareCPU::Inst_JumpF),
    tInstLibEntry<tMethod>("jump-b", &cHardwareCPU::Inst_JumpB),
    tInstLibEntry<tMethod>("call", &cHardwareCPU::Inst_Call),
    tInstLibEntry<tMethod>("return", &cHardwareCPU::Inst_Return),

    tInstLibEntry<tMethod>("throw", &cHardwareCPU::Inst_Throw),
    tInstLibEntry<tMethod>("throwif=0", &cHardwareCPU::Inst_ThrowIf0),    
    tInstLibEntry<tMethod>("throwif!=0", &cHardwareCPU::Inst_ThrowIfNot0),
    tInstLibEntry<tMethod>("catch", &cHardwareCPU::Inst_Catch),
    
    tInstLibEntry<tMethod>("goto", &cHardwareCPU::Inst_Goto),
    tInstLibEntry<tMethod>("goto-if=0", &cHardwareCPU::Inst_GotoIf0),    
    tInstLibEntry<tMethod>("goto-if!=0", &cHardwareCPU::Inst_GotoIfNot0),
    tInstLibEntry<tMethod>("label", &cHardwareCPU::Inst_Label),
    
    tInstLibEntry<tMethod>("pop", &cHardwareCPU::Inst_Pop, nInstFlag::DEFAULT, "Remove top number from stack and place into ?BX?"),
    tInstLibEntry<tMethod>("push", &cHardwareCPU::Inst_Push, nInstFlag::DEFAULT, "Copy number from ?BX? and place it into the stack"),
    tInstLibEntry<tMethod>("swap-stk", &cHardwareCPU::Inst_SwitchStack, nInstFlag::DEFAULT, "Toggle which stack is currently being used"),
    tInstLibEntry<tMethod>("flip-stk", &cHardwareCPU::Inst_FlipStack),
    tInstLibEntry<tMethod>("swap", &cHardwareCPU::Inst_Swap, nInstFlag::DEFAULT, "Swap the contents of ?BX? with ?CX?"),
    tInstLibEntry<tMethod>("swap-AB", &cHardwareCPU::Inst_SwapAB),
    tInstLibEntry<tMethod>("swap-BC", &cHardwareCPU::Inst_SwapBC),
    tInstLibEntry<tMethod>("swap-AC", &cHardwareCPU::Inst_SwapAC),
    tInstLibEntry<tMethod>("copy-reg", &cHardwareCPU::Inst_CopyReg),
    tInstLibEntry<tMethod>("set_A=B", &cHardwareCPU::Inst_CopyRegAB),
    tInstLibEntry<tMethod>("set_A=C", &cHardwareCPU::Inst_CopyRegAC),
    tInstLibEntry<tMethod>("set_B=A", &cHardwareCPU::Inst_CopyRegBA),
    tInstLibEntry<tMethod>("set_B=C", &cHardwareCPU::Inst_CopyRegBC),
    tInstLibEntry<tMethod>("set_C=A", &cHardwareCPU::Inst_CopyRegCA),
    tInstLibEntry<tMethod>("set_C=B", &cHardwareCPU::Inst_CopyRegCB),
    tInstLibEntry<tMethod>("reset", &cHardwareCPU::Inst_Reset),
    
    tInstLibEntry<tMethod>("pop-A", &cHardwareCPU::Inst_PopA),
    tInstLibEntry<tMethod>("pop-B", &cHardwareCPU::Inst_PopB),
    tInstLibEntry<tMethod>("pop-C", &cHardwareCPU::Inst_PopC),
    tInstLibEntry<tMethod>("push-A", &cHardwareCPU::Inst_PushA),
    tInstLibEntry<tMethod>("push-B", &cHardwareCPU::Inst_PushB),
    tInstLibEntry<tMethod>("push-C", &cHardwareCPU::Inst_PushC),
    
    tInstLibEntry<tMethod>("shift-r", &cHardwareCPU::Inst_ShiftR, nInstFlag::DEFAULT, "Shift bits in ?BX? right by one (divide by two)"),
    tInstLibEntry<tMethod>("shift-l", &cHardwareCPU::Inst_ShiftL, nInstFlag::DEFAULT, "Shift bits in ?BX? left by one (multiply by two)"),
    tInstLibEntry<tMethod>("bit-1", &cHardwareCPU::Inst_Bit1),
    tInstLibEntry<tMethod>("set-num", &cHardwareCPU::Inst_SetNum),
    tInstLibEntry<tMethod>("val-grey", &cHardwareCPU::Inst_ValGrey),
    tInstLibEntry<tMethod>("val-dir", &cHardwareCPU::Inst_ValDir),
    tInstLibEntry<tMethod>("val-add-p", &cHardwareCPU::Inst_ValAddP),
    tInstLibEntry<tMethod>("val-fib", &cHardwareCPU::Inst_ValFib),
    tInstLibEntry<tMethod>("val-poly-c", &cHardwareCPU::Inst_ValPolyC),
    tInstLibEntry<tMethod>("inc", &cHardwareCPU::Inst_Inc, nInstFlag::DEFAULT, "Increment ?BX? by one"),
    tInstLibEntry<tMethod>("dec", &cHardwareCPU::Inst_Dec, nInstFlag::DEFAULT, "Decrement ?BX? by one"),
    tInstLibEntry<tMethod>("zero", &cHardwareCPU::Inst_Zero, 0, "Set ?BX? to zero"),
    tInstLibEntry<tMethod>("neg", &cHardwareCPU::Inst_Neg),
    tInstLibEntry<tMethod>("square", &cHardwareCPU::Inst_Square),
    tInstLibEntry<tMethod>("sqrt", &cHardwareCPU::Inst_Sqrt),
    tInstLibEntry<tMethod>("not", &cHardwareCPU::Inst_Not),
    
    tInstLibEntry<tMethod>("add", &cHardwareCPU::Inst_Add, nInstFlag::DEFAULT, "Add BX to CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("sub", &cHardwareCPU::Inst_Sub, nInstFlag::DEFAULT, "Subtract CX from BX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("mult", &cHardwareCPU::Inst_Mult, 0, "Multiple BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("div", &cHardwareCPU::Inst_Div, 0, "Divide BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("mod", &cHardwareCPU::Inst_Mod),
    tInstLibEntry<tMethod>("nand", &cHardwareCPU::Inst_Nand, nInstFlag::DEFAULT, "Nand BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("nor", &cHardwareCPU::Inst_Nor),
    tInstLibEntry<tMethod>("and", &cHardwareCPU::Inst_And),
    tInstLibEntry<tMethod>("order", &cHardwareCPU::Inst_Order),
    tInstLibEntry<tMethod>("xor", &cHardwareCPU::Inst_Xor),
    
    tInstLibEntry<tMethod>("copy", &cHardwareCPU::Inst_Copy),
    tInstLibEntry<tMethod>("read", &cHardwareCPU::Inst_ReadInst),
    tInstLibEntry<tMethod>("write", &cHardwareCPU::Inst_WriteInst),
    tInstLibEntry<tMethod>("stk-read", &cHardwareCPU::Inst_StackReadInst),
    tInstLibEntry<tMethod>("stk-writ", &cHardwareCPU::Inst_StackWriteInst),
    
    tInstLibEntry<tMethod>("compare", &cHardwareCPU::Inst_Compare),
    tInstLibEntry<tMethod>("if-n-cpy", &cHardwareCPU::Inst_IfNCpy),
    tInstLibEntry<tMethod>("allocate", &cHardwareCPU::Inst_Allocate),
    tInstLibEntry<tMethod>("divide", &cHardwareCPU::Inst_Divide),
    tInstLibEntry<tMethod>("divideRS", &cHardwareCPU::Inst_DivideRS),
    tInstLibEntry<tMethod>("c-alloc", &cHardwareCPU::Inst_CAlloc),
    tInstLibEntry<tMethod>("c-divide", &cHardwareCPU::Inst_CDivide),
    tInstLibEntry<tMethod>("inject", &cHardwareCPU::Inst_Inject),
    tInstLibEntry<tMethod>("inject-r", &cHardwareCPU::Inst_InjectRand),
    tInstLibEntry<tMethod>("transposon", &cHardwareCPU::Inst_Transposon),
    tInstLibEntry<tMethod>("search-f", &cHardwareCPU::Inst_SearchF),
    tInstLibEntry<tMethod>("search-b", &cHardwareCPU::Inst_SearchB),
    tInstLibEntry<tMethod>("mem-size", &cHardwareCPU::Inst_MemSize),
    
    tInstLibEntry<tMethod>("get", &cHardwareCPU::Inst_TaskGet),
    tInstLibEntry<tMethod>("get-2", &cHardwareCPU::Inst_TaskGet2),
    tInstLibEntry<tMethod>("stk-get", &cHardwareCPU::Inst_TaskStackGet),
    tInstLibEntry<tMethod>("stk-load", &cHardwareCPU::Inst_TaskStackLoad),
    tInstLibEntry<tMethod>("put", &cHardwareCPU::Inst_TaskPut),
    tInstLibEntry<tMethod>("put-reset", &cHardwareCPU::Inst_TaskPutResetInputs),
    tInstLibEntry<tMethod>("IO", &cHardwareCPU::Inst_TaskIO, nInstFlag::DEFAULT, "Output ?BX?, and input new number back into ?BX?"),
    tInstLibEntry<tMethod>("IO-Feedback", &cHardwareCPU::Inst_TaskIO_Feedback, 0, "Output ?BX?, and input new number back into ?BX?,  and push 1,0,  or -1 onto stack1 if merit increased, stayed the same, or decreased"),
    tInstLibEntry<tMethod>("IO-bc-0.001", &cHardwareCPU::Inst_TaskIO_BonusCost_0_001),
    tInstLibEntry<tMethod>("match-strings", &cHardwareCPU::Inst_MatchStrings),
    tInstLibEntry<tMethod>("sell", &cHardwareCPU::Inst_Sell),
    tInstLibEntry<tMethod>("buy", &cHardwareCPU::Inst_Buy),
    tInstLibEntry<tMethod>("send", &cHardwareCPU::Inst_Send),
    tInstLibEntry<tMethod>("receive", &cHardwareCPU::Inst_Receive),
    tInstLibEntry<tMethod>("sense", &cHardwareCPU::Inst_SenseLog2),           // If you add more sense instructions
    tInstLibEntry<tMethod>("sense-unit", &cHardwareCPU::Inst_SenseUnit),      // and want to keep stats, also add
    tInstLibEntry<tMethod>("sense-m100", &cHardwareCPU::Inst_SenseMult100),   // the names to cStats::cStats() @JEB
    
    tInstLibEntry<tMethod>("donate-rnd", &cHardwareCPU::Inst_DonateRandom),
    tInstLibEntry<tMethod>("donate-kin", &cHardwareCPU::Inst_DonateKin),
    tInstLibEntry<tMethod>("donate-edt", &cHardwareCPU::Inst_DonateEditDist),
    tInstLibEntry<tMethod>("donate-gbg",  &cHardwareCPU::Inst_DonateGreenBeardGene),
    tInstLibEntry<tMethod>("donate-tgb",  &cHardwareCPU::Inst_DonateTrueGreenBeard),
    tInstLibEntry<tMethod>("donate-threshgb",  &cHardwareCPU::Inst_DonateThreshGreenBeard),
    tInstLibEntry<tMethod>("donate-quantagb",  &cHardwareCPU::Inst_DonateQuantaThreshGreenBeard),
    tInstLibEntry<tMethod>("donate-NUL", &cHardwareCPU::Inst_DonateNULL),
    tInstLibEntry<tMethod>("donate-facing", &cHardwareCPU::Inst_DonateFacing),
    
    tInstLibEntry<tMethod>("IObuf-add1", &cHardwareCPU::Inst_IOBufAdd1),
    tInstLibEntry<tMethod>("IObuf-add0", &cHardwareCPU::Inst_IOBufAdd0),

    tInstLibEntry<tMethod>("rotate-l", &cHardwareCPU::Inst_RotateL),
    tInstLibEntry<tMethod>("rotate-r", &cHardwareCPU::Inst_RotateR),
    tInstLibEntry<tMethod>("rotate-left-one", &cHardwareCPU::Inst_RotateLeftOne),
    tInstLibEntry<tMethod>("rotate-right-one", &cHardwareCPU::Inst_RotateRightOne),

    tInstLibEntry<tMethod>("rotate-label", &cHardwareCPU::Inst_RotateLabel),
    
    tInstLibEntry<tMethod>("set-cmut", &cHardwareCPU::Inst_SetCopyMut),
    tInstLibEntry<tMethod>("mod-cmut", &cHardwareCPU::Inst_ModCopyMut),
    // @WRE additions for movement
    tInstLibEntry<tMethod>("tumble", &cHardwareCPU::Inst_Tumble),
    tInstLibEntry<tMethod>("move", &cHardwareCPU::Inst_Move),
    
    // Threading instructions
    tInstLibEntry<tMethod>("fork-th", &cHardwareCPU::Inst_ForkThread),
    tInstLibEntry<tMethod>("forkl", &cHardwareCPU::Inst_ForkThreadLabel),
    tInstLibEntry<tMethod>("forkl!=0", &cHardwareCPU::Inst_ForkThreadLabelIfNot0),
    tInstLibEntry<tMethod>("forkl=0", &cHardwareCPU::Inst_ForkThreadLabelIf0),
    tInstLibEntry<tMethod>("kill-th", &cHardwareCPU::Inst_KillThread),
    tInstLibEntry<tMethod>("id-th", &cHardwareCPU::Inst_ThreadID),
    
    // Head-based instructions
    tInstLibEntry<tMethod>("h-alloc", &cHardwareCPU::Inst_MaxAlloc, nInstFlag::DEFAULT, "Allocate maximum allowed space"),
    tInstLibEntry<tMethod>("h-alloc-mw", &cHardwareCPU::Inst_MaxAllocMoveWriteHead),
    tInstLibEntry<tMethod>("h-divide", &cHardwareCPU::Inst_HeadDivide, nInstFlag::DEFAULT, "Divide code between read and write heads."),
    tInstLibEntry<tMethod>("h-divide1RS", &cHardwareCPU::Inst_HeadDivide1RS, 0, "Divide code between read and write heads, at most one mutation on divide, resample if reverted."),
    tInstLibEntry<tMethod>("h-divide2RS", &cHardwareCPU::Inst_HeadDivide2RS, 0, "Divide code between read and write heads, at most two mutations on divide, resample if reverted."),
    tInstLibEntry<tMethod>("h-divideRS", &cHardwareCPU::Inst_HeadDivideRS, 0, "Divide code between read and write heads, resample if reverted."),
    tInstLibEntry<tMethod>("h-read", &cHardwareCPU::Inst_HeadRead),
    tInstLibEntry<tMethod>("h-write", &cHardwareCPU::Inst_HeadWrite),
    tInstLibEntry<tMethod>("h-copy", &cHardwareCPU::Inst_HeadCopy, nInstFlag::DEFAULT, "Copy from read-head to write-head; advance both"),
    tInstLibEntry<tMethod>("h-search", &cHardwareCPU::Inst_HeadSearch, nInstFlag::DEFAULT, "Find complement template and make with flow head"),
    tInstLibEntry<tMethod>("h-push", &cHardwareCPU::Inst_HeadPush),
    tInstLibEntry<tMethod>("h-pop", &cHardwareCPU::Inst_HeadPop),
    tInstLibEntry<tMethod>("set-head", &cHardwareCPU::Inst_SetHead),
    tInstLibEntry<tMethod>("adv-head", &cHardwareCPU::Inst_AdvanceHead),
    tInstLibEntry<tMethod>("mov-head", &cHardwareCPU::Inst_MoveHead, nInstFlag::DEFAULT, "Move head ?IP? to the flow head"),
    tInstLibEntry<tMethod>("jmp-head", &cHardwareCPU::Inst_JumpHead, nInstFlag::DEFAULT, "Move head ?IP? by amount in CX register; CX = old pos."),
    tInstLibEntry<tMethod>("get-head", &cHardwareCPU::Inst_GetHead, nInstFlag::DEFAULT, "Copy the position of the ?IP? head into CX"),
    tInstLibEntry<tMethod>("if-label", &cHardwareCPU::Inst_IfLabel, nInstFlag::DEFAULT, "Execute next if we copied complement of attached label"),
    tInstLibEntry<tMethod>("if-label2", &cHardwareCPU::Inst_IfLabel2, 0, "If copied label compl., exec next inst; else SKIP W/NOPS"),
    tInstLibEntry<tMethod>("set-flow", &cHardwareCPU::Inst_SetFlow, nInstFlag::DEFAULT, "Set flow-head to position in ?CX?"),
    
    tInstLibEntry<tMethod>("h-copy2", &cHardwareCPU::Inst_HeadCopy2),
    tInstLibEntry<tMethod>("h-copy3", &cHardwareCPU::Inst_HeadCopy3),
    tInstLibEntry<tMethod>("h-copy4", &cHardwareCPU::Inst_HeadCopy4),
    tInstLibEntry<tMethod>("h-copy5", &cHardwareCPU::Inst_HeadCopy5),
    tInstLibEntry<tMethod>("h-copy6", &cHardwareCPU::Inst_HeadCopy6),
    tInstLibEntry<tMethod>("h-copy7", &cHardwareCPU::Inst_HeadCopy7),
    tInstLibEntry<tMethod>("h-copy8", &cHardwareCPU::Inst_HeadCopy8),
    tInstLibEntry<tMethod>("h-copy9", &cHardwareCPU::Inst_HeadCopy9),
    tInstLibEntry<tMethod>("h-copy10", &cHardwareCPU::Inst_HeadCopy10),
    
    tInstLibEntry<tMethod>("divide-sex", &cHardwareCPU::Inst_HeadDivideSex),
    tInstLibEntry<tMethod>("divide-asex", &cHardwareCPU::Inst_HeadDivideAsex),
    
    tInstLibEntry<tMethod>("div-sex", &cHardwareCPU::Inst_HeadDivideSex),
    tInstLibEntry<tMethod>("div-asex", &cHardwareCPU::Inst_HeadDivideAsex),
    tInstLibEntry<tMethod>("div-asex-w", &cHardwareCPU::Inst_HeadDivideAsexWait),
    tInstLibEntry<tMethod>("div-sex-MS", &cHardwareCPU::Inst_HeadDivideMateSelect),
    
    tInstLibEntry<tMethod>("h-divide1", &cHardwareCPU::Inst_HeadDivide1),
    tInstLibEntry<tMethod>("h-divide2", &cHardwareCPU::Inst_HeadDivide2),
    tInstLibEntry<tMethod>("h-divide3", &cHardwareCPU::Inst_HeadDivide3),
    tInstLibEntry<tMethod>("h-divide4", &cHardwareCPU::Inst_HeadDivide4),
    tInstLibEntry<tMethod>("h-divide5", &cHardwareCPU::Inst_HeadDivide5),
    tInstLibEntry<tMethod>("h-divide6", &cHardwareCPU::Inst_HeadDivide6),
    tInstLibEntry<tMethod>("h-divide7", &cHardwareCPU::Inst_HeadDivide7),
    tInstLibEntry<tMethod>("h-divide8", &cHardwareCPU::Inst_HeadDivide8),
    tInstLibEntry<tMethod>("h-divide9", &cHardwareCPU::Inst_HeadDivide9),
    tInstLibEntry<tMethod>("h-divide10", &cHardwareCPU::Inst_HeadDivide10),
    tInstLibEntry<tMethod>("h-divide16", &cHardwareCPU::Inst_HeadDivide16),
    tInstLibEntry<tMethod>("h-divide32", &cHardwareCPU::Inst_HeadDivide32),
    tInstLibEntry<tMethod>("h-divide50", &cHardwareCPU::Inst_HeadDivide50),
    tInstLibEntry<tMethod>("h-divide100", &cHardwareCPU::Inst_HeadDivide100),
    tInstLibEntry<tMethod>("h-divide500", &cHardwareCPU::Inst_HeadDivide500),
    tInstLibEntry<tMethod>("h-divide1000", &cHardwareCPU::Inst_HeadDivide1000),
    tInstLibEntry<tMethod>("h-divide5000", &cHardwareCPU::Inst_HeadDivide5000),
    tInstLibEntry<tMethod>("h-divide10000", &cHardwareCPU::Inst_HeadDivide10000),
    tInstLibEntry<tMethod>("h-divide50000", &cHardwareCPU::Inst_HeadDivide50000),
    tInstLibEntry<tMethod>("h-divide0.5", &cHardwareCPU::Inst_HeadDivide0_5),
    tInstLibEntry<tMethod>("h-divide0.1", &cHardwareCPU::Inst_HeadDivide0_1),
    tInstLibEntry<tMethod>("h-divide0.05", &cHardwareCPU::Inst_HeadDivide0_05),
    tInstLibEntry<tMethod>("h-divide0.01", &cHardwareCPU::Inst_HeadDivide0_01),
    tInstLibEntry<tMethod>("h-divide0.001", &cHardwareCPU::Inst_HeadDivide0_001),
    
    // High-level instructions
    tInstLibEntry<tMethod>("repro", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-A", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-B", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-C", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-D", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-E", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-F", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-G", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-H", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-I", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-J", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-K", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-L", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-M", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-N", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-O", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-P", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-Q", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-R", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-S", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-T", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-U", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-V", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-W", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-X", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-Y", &cHardwareCPU::Inst_Repro),
    tInstLibEntry<tMethod>("repro-Z", &cHardwareCPU::Inst_Repro),

    tInstLibEntry<tMethod>("put-repro", &cHardwareCPU::Inst_TaskPutRepro),
    tInstLibEntry<tMethod>("metabolize", &cHardwareCPU::Inst_TaskPutResetInputsRepro),        

    tInstLibEntry<tMethod>("sterilize", &cHardwareCPU::Inst_Sterilize),
    
    tInstLibEntry<tMethod>("spawn-deme", &cHardwareCPU::Inst_SpawnDeme),
    
    // Suicide
    tInstLibEntry<tMethod>("kazi",	&cHardwareCPU::Inst_Kazi),
    tInstLibEntry<tMethod>("kazi5", &cHardwareCPU::Inst_Kazi5),
    tInstLibEntry<tMethod>("die", &cHardwareCPU::Inst_Die),

    // Sleep and time
    tInstLibEntry<tMethod>("sleep", &cHardwareCPU::Inst_Sleep),
    tInstLibEntry<tMethod>("sleep1", &cHardwareCPU::Inst_Sleep),
    tInstLibEntry<tMethod>("sleep2", &cHardwareCPU::Inst_Sleep),
    tInstLibEntry<tMethod>("sleep3", &cHardwareCPU::Inst_Sleep),
    tInstLibEntry<tMethod>("sleep4", &cHardwareCPU::Inst_Sleep),
    tInstLibEntry<tMethod>("time", &cHardwareCPU::Inst_GetUpdate),
    
    // Promoter Model
    tInstLibEntry<tMethod>("terminate", &cHardwareCPU::Inst_Terminate),
    tInstLibEntry<tMethod>("promoter", &cHardwareCPU::Inst_Promoter),
    tInstLibEntry<tMethod>("regulate", &cHardwareCPU::Inst_Regulate),
    
    // Energy usage
    tInstLibEntry<tMethod>("double-energy-usage", &cHardwareCPU::Inst_DoubleEnergyUsage),
    tInstLibEntry<tMethod>("half-energy-usage", &cHardwareCPU::Inst_HalfEnergyUsage),

    // Messaging
    tInstLibEntry<tMethod>("send-msg", &cHardwareCPU::Inst_SendMessage),
    tInstLibEntry<tMethod>("retrieve-msg", &cHardwareCPU::Inst_RetrieveMessage),
    
    // Placebo instructions
    tInstLibEntry<tMethod>("skip", &cHardwareCPU::Inst_Skip),

    // Must always be the last instruction in the array
    tInstLibEntry<tMethod>("NULL", &cHardwareCPU::Inst_Nop, 0, "True no-operation instruction: does nothing"),
  };
  
  const int n_size = sizeof(s_n_array)/sizeof(cNOPEntryCPU);
  
  static cString n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size && i < NUM_REGISTERS; i++) {
    n_names[i] = s_n_array[i].name;
    nop_mods[i] = s_n_array[i].nop_mod;
  }
  
  const int f_size = sizeof(s_f_array)/sizeof(tInstLibEntry<tMethod>);
  static tMethod functions[f_size];
  for (int i = 0; i < f_size; i++) functions[i] = s_f_array[i].GetFunction();

	const cInstruction error(255);
	const cInstruction def(0);
  const cInstruction null_inst(f_size - 1);
  
  return new tInstLib<tMethod>(f_size, s_f_array, n_names, nop_mods, functions, error, def, null_inst);
}

cHardwareCPU::cHardwareCPU(cWorld* world, cOrganism* in_organism, cInstSet* in_m_inst_set)
: cHardwareBase(world, in_organism, in_m_inst_set)
{
  /* FIXME:  reorganize storage of m_functions.  -- kgn */
  m_functions = s_inst_slib->GetFunctions();
  /**/
  m_memory = in_organism->GetGenome();  // Initialize memory...
  Reset();                            // Setup the rest of the hardware...
}


cHardwareCPU::cHardwareCPU(const cHardwareCPU &hardware_cpu)
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
{
#if INSTRUCTION_COSTS
  inst_cost = hardware_cpu.inst_cost;
  inst_ft_cost = hardware_cpu.inst_ft_cost;
  inst_energy_cost = hardware_cpu.inst_energy_cost;
  m_has_costs = hardware_cpu.m_has_costs;
  m_has_ft_costs = hardware_cpu.m_has_ft_costs;
  m_has_energy_costs = hardware_cpu.m_has_energy_costs;
#endif

}


void cHardwareCPU::Reset()
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
  inst_energy_cost.Resize(num_inst_cost);
  m_has_costs = false;
  m_has_ft_costs = false;
  m_has_energy_costs = false;
  
  for (int i = 0; i < num_inst_cost; i++) {
    inst_cost[i] = m_inst_set->GetCost(cInstruction(i));
    if (!m_has_costs && inst_cost[i]) m_has_costs = true;
    
    inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
    if (!m_has_ft_costs && inst_ft_cost[i]) m_has_ft_costs = true;

    inst_energy_cost[i] = m_inst_set->GetEnergyCost(cInstruction(i));    
    if(!m_has_energy_costs && inst_energy_cost[i]) m_has_energy_costs = true;
  }
#endif   

  if (m_world->GetConfig().PROMOTERS_ENABLED.Get())
  {
    // Ideally, this shouldn't be hard-coded
    cInstruction promoter_inst = m_world->GetHardwareManager().GetInstSet().GetInst(cStringUtil::Stringf("promoter"));
    promoter_search_pos = 0;
    promoter_inst_executed = 0;
    promoter_pos.Resize(0);
    promoter_active.Resize(0);
    for (int i=0; i<GetMemory().GetSize(); i++)
    {
      if ( (GetMemory())[i] == promoter_inst)
      {
        promoter_pos.Push(i);
        promoter_active.Push(true);
      }
    }
  }
}

void cHardwareCPU::cLocalThread::operator=(const cLocalThread& in_thread)
{
  m_id = in_thread.m_id;
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i] = in_thread.reg[i];
  for (int i = 0; i < NUM_HEADS; i++) heads[i] = in_thread.heads[i];
  stack = in_thread.stack;
}

void cHardwareCPU::cLocalThread::Reset(cHardwareBase* in_hardware, int in_id)
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

void cHardwareCPU::SingleProcess(cAvidaContext& ctx)
{
  int last_IP_pos = IP().GetPosition();

  // Mark this organism as running...
  organism->SetRunning(true);
  
  cPhenotype & phenotype = organism->GetPhenotype();
  
  if (m_world->GetConfig().PROMOTERS_ENABLED.Get() == 1) {
    //First instruction - check whether we should be starting at a promoter.
    if (phenotype.GetCPUCyclesUsed() == 0) Inst_Terminate(m_world->GetDefaultContext());
  }
  
  phenotype.IncCPUCyclesUsed();
  if (!m_world->GetConfig().NO_CPU_CYCLE_TIME.Get()) phenotype.IncTimeUsed();

  const int num_threads = GetNumThreads();
  
  // If we have threads turned on and we executed each thread in a single
  // timestep, adjust the number of instructions executed accordingly.
  const int num_inst_exec = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1) ? num_threads : 1;
  
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
      const int time_cost = m_inst_set->GetAddlTimeCost(cur_inst);

      // Prob of exec (moved from SingleProcess_PayCosts so that we advance IP after a fail)
      if ( m_inst_set->GetProbFail(cur_inst) > 0.0 ) {
        exec = !( ctx.GetRandom().P(m_inst_set->GetProbFail(cur_inst)) );
      }
      
      //Add to the promoter inst executed count before executing the inst (in case it is a terminator)
      if (m_world->GetConfig().PROMOTERS_ENABLED.Get() == 1) {
        promoter_inst_executed++;
      }

      if (exec == true) SingleProcess_ExecuteInst(ctx, cur_inst);
      
      // Some instruction (such as jump) may turn m_advance_ip off.  Usually
      // we now want to move to the next instruction in the memory.
      if (m_advance_ip == true) IP().Advance();
      
      // Pay the time cost of the instruction now
      phenotype.IncTimeUsed(time_cost);
      
      // In the promoter model, there may be a chance of termination
      // that causes execution to start at a new instruction (per instruction executed)
      if ( m_world->GetConfig().PROMOTERS_ENABLED.Get() == 1 ) {
        const double processivity = m_world->GetConfig().PROMOTER_PROCESSIVITY_INST.Get();
        if ( ctx.GetRandom().P(1-processivity) ) Inst_Terminate(ctx);
      }
      
    } // if exec
    
    // In the promoter model, there may be a chance of termination
    // that causes execution to start at a new instruction (per cpu cycle executed)
    // @JEB - since processivities usually v. close to 1 it doesn't
    // improve speed much to combine with "per instruction" block
    if ( m_world->GetConfig().PROMOTERS_ENABLED.Get() == 1 )
    {
      const double processivity = m_world->GetConfig().PROMOTER_PROCESSIVITY.Get();
      if ( ctx.GetRandom().P(1-processivity) ) 
        Inst_Terminate(ctx);
      if ( m_world->GetConfig().PROMOTER_MAX_INST.Get() && (promoter_inst_executed >= m_world->GetConfig().PROMOTER_MAX_INST.Get()) ) 
        Inst_Terminate(ctx);
    }
    
  } // Previous was executed once for each thread...
  
  // Kill creatures who have reached their max num of instructions executed
  const int max_executed = organism->GetMaxExecuted();
  if ((max_executed > 0 && phenotype.GetTimeUsed() >= max_executed)
      || phenotype.GetToDie() == true) {
    organism->Die();
  }
  
  organism->SetRunning(false);
  CheckImplicitRepro(ctx, last_IP_pos > IP().GetPosition());
}

// This method will handle the actual execution of an instruction
// within a single process, once that function has been finalized.
bool cHardwareCPU::SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst) 
{
  // Copy Instruction locally to handle stochastic effects
  cInstruction actual_inst = cur_inst;
  
#ifdef EXECUTION_ERRORS
  // If there is an execution error, execute a random instruction.
  if (organism->TestExeErr()) actual_inst = m_inst_set->GetRandomInst(ctx);
#endif /* EXECUTION_ERRORS */
    
  // Get a pointer to the corresponding method...
  int inst_idx = m_inst_set->GetLibFunctionIndex(actual_inst);
  
  // Mark the instruction as executed
  IP().SetFlagExecuted();
	
  
#if INSTRUCTION_COUNT
  // instruction execution count incremented
  organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
#endif
	
  // And execute it.
  const bool exec_success = (this->*(m_functions[inst_idx]))(ctx);

  // NOTE: Organism may be dead now if instruction executed killed it (such as some divides, "die", or "kazi")

#if INSTRUCTION_COUNT
  // Decrement if the instruction was not executed successfully.
  if (exec_success == false) {
    organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  }
#endif	
  
  return exec_success;
}


void cHardwareCPU::ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = organism->IsRunning();
  organism->SetRunning(true);
  
  if (m_tracer != NULL) m_tracer->TraceHardware(*this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  organism->SetRunning(prev_run_state);
}


bool cHardwareCPU::OK()
{
  bool result = true;
  
  if (!m_memory.OK()) result = false;
  
  for (int i = 0; i < GetNumThreads(); i++) {
    if (m_threads[i].stack.OK() == false) result = false;
    if (m_threads[i].next_label.OK() == false) result = false;
  }
  
  return result;
}

void cHardwareCPU::PrintStatus(ostream& fp)
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
    fp << "  EnergyUsed:" << organism->GetPhenotype().GetTimeUsed();
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
      
  if (m_world->GetConfig().PROMOTERS_ENABLED.Get())
  {
    fp << "Promoters:";
    for (int i=0; i<promoter_pos.GetSize(); i++)
    {
      fp << " " << promoter_pos[i] << "-" << (promoter_active[i] ? "on" : "off"); 
    }
    fp << endl;
    fp << "Instructions executed past promoter: " << promoter_inst_executed << endl;
  }    
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


bool cHardwareCPU::InjectHost(const cCodeLabel & in_label, const cGenome & injection)
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

void cHardwareCPU::InjectCode(const cGenome & inject_code, const int line_num)
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


void cHardwareCPU::ReadInst(const int in_inst)
{
  if (m_inst_set->IsNop( cInstruction(in_inst) )) {
    GetReadLabel().AddNop(in_inst);
  } else {
    GetReadLabel().Clear();
  }
}


void cHardwareCPU::AdjustHeads()
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

void cHardwareCPU::ReadLabel(int max_size)
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


bool cHardwareCPU::ForkThread()
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


bool cHardwareCPU::KillThread()
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

inline int cHardwareCPU::FindModifiedRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_register = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  }
  return default_register;
}

inline int cHardwareCPU::FindModifiedNextRegister(int default_register)
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

inline int cHardwareCPU::FindModifiedPreviousRegister(int default_register)
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


inline int cHardwareCPU::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.
  
  if (m_inst_set->IsNop(IP().GetNextInst())) {
    IP().Advance();
    default_head = m_inst_set->GetNopMod(IP().GetInst());
    IP().SetFlagExecuted();
  }
  return default_head;
}


inline int cHardwareCPU::FindNextRegister(int base_reg)
{
  return (base_reg + 1) % NUM_REGISTERS;
}


bool cHardwareCPU::Allocate_Necro(const int new_size)
{
  GetMemory().ResizeOld(new_size);
  return true;
}

bool cHardwareCPU::Allocate_Random(cAvidaContext& ctx, const int old_size, const int new_size)
{
  GetMemory().Resize(new_size);

  for (int i = old_size; i < new_size; i++) {
    GetMemory()[i] = m_inst_set->GetRandomInst(ctx);
  }
  return true;
}

bool cHardwareCPU::Allocate_Default(const int new_size)
{
  GetMemory().Resize(new_size);
  
  // New space already defaults to default instruction...
  
  return true;
}

bool cHardwareCPU::Allocate_Main(cAvidaContext& ctx, const int allocated_size)
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

int cHardwareCPU::GetCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  const cCPUMemory& memory = GetMemory();
  for (int i = parent_size; i < parent_size + child_size; i++) {
    if (memory.FlagCopied(i)) copied_size++;
  }
  return copied_size;
}  


bool cHardwareCPU::Divide_Main(cAvidaContext& ctx, const int div_point,
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
bool cHardwareCPU::Divide_MainRS(cAvidaContext& ctx, const int div_point,
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
bool cHardwareCPU::Divide_Main1RS(cAvidaContext& ctx, const int div_point,
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
bool cHardwareCPU::Divide_Main2RS(cAvidaContext& ctx, const int div_point,
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

bool cHardwareCPU::Inst_If0(cAvidaContext& ctx)          // Execute next if ?bx? ==0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) != 0)  IP().Advance();
  return true; 
}

bool cHardwareCPU::Inst_IfNot0(cAvidaContext& ctx)       // Execute next if ?bx? != 0.
{ 
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) == 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfEqu(cAvidaContext& ctx)      // Execute next if bx == ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) != GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfNEqu(cAvidaContext& ctx)     // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) == GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGr0(cAvidaContext& ctx)       // Execute next if ?bx? ! < 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) <= 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGr(cAvidaContext& ctx)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) <= GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGrEqu0(cAvidaContext& ctx)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) < 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGrEqu(cAvidaContext& ctx)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) < GetRegister(op2)) IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLess0(cAvidaContext& ctx)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) >= 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLess(cAvidaContext& ctx)       // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) >=  GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLsEqu0(cAvidaContext& ctx)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) > 0) IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLsEqu(cAvidaContext& ctx)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) >  GetRegister(op2))  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfBit1(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if ((GetRegister(reg_used) & 1) == 0)  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfANotEqB(cAvidaContext& ctx)     // Execute next if AX != BX
{
  if (GetRegister(REG_AX) == GetRegister(REG_BX) )  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfBNotEqC(cAvidaContext& ctx)     // Execute next if BX != CX
{
  if (GetRegister(REG_BX) == GetRegister(REG_CX) )  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfANotEqC(cAvidaContext& ctx)     // Execute next if AX != BX
{
  if (GetRegister(REG_AX) == GetRegister(REG_CX) )  IP().Advance();
  return true;
}

bool cHardwareCPU::Inst_JumpF(cAvidaContext& ctx)
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


bool cHardwareCPU::Inst_JumpB(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_Call(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_Return(cAvidaContext& ctx)
{
  IP().Set(StackPop());
  return true;
}

bool cHardwareCPU::Inst_Throw(cAvidaContext& ctx)
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


bool cHardwareCPU::Inst_ThrowIfNot0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) == 0) return false;
  return Inst_Throw(ctx);
}

bool cHardwareCPU::Inst_ThrowIf0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) != 0) return false;
  return Inst_Throw(ctx);
}

bool cHardwareCPU::Inst_Goto(cAvidaContext& ctx)
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


bool cHardwareCPU::Inst_GotoIfNot0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) == 0) return false;
  return Inst_Goto(ctx);
}

bool cHardwareCPU::Inst_GotoIf0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) != 0) return false;
  return Inst_Goto(ctx);
}


bool cHardwareCPU::Inst_Pop(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = StackPop();
  return true;
}

bool cHardwareCPU::Inst_Push(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  StackPush(GetRegister(reg_used));
  return true;
}

bool cHardwareCPU::Inst_HeadPop(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  GetHead(head_used).Set(StackPop());
  return true;
}

bool cHardwareCPU::Inst_HeadPush(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  StackPush(GetHead(head_used).GetPosition());
  if (head_used == nHardware::HEAD_IP) {
    GetHead(head_used).Set(GetHead(nHardware::HEAD_FLOW));
    m_advance_ip = false;
  }
  return true;
}


bool cHardwareCPU::Inst_PopA(cAvidaContext& ctx) { GetRegister(REG_AX) = StackPop(); return true;}
bool cHardwareCPU::Inst_PopB(cAvidaContext& ctx) { GetRegister(REG_BX) = StackPop(); return true;}
bool cHardwareCPU::Inst_PopC(cAvidaContext& ctx) { GetRegister(REG_CX) = StackPop(); return true;}

bool cHardwareCPU::Inst_PushA(cAvidaContext& ctx) { StackPush(GetRegister(REG_AX)); return true;}
bool cHardwareCPU::Inst_PushB(cAvidaContext& ctx) { StackPush(GetRegister(REG_BX)); return true;}
bool cHardwareCPU::Inst_PushC(cAvidaContext& ctx) { StackPush(GetRegister(REG_CX)); return true;}

bool cHardwareCPU::Inst_SwitchStack(cAvidaContext& ctx) { SwitchStack(); return true;}
bool cHardwareCPU::Inst_FlipStack(cAvidaContext& ctx)   { StackFlip(); return true;}

bool cHardwareCPU::Inst_Swap(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  nFunctions::Swap(GetRegister(op1), GetRegister(op2));
  return true;
}

bool cHardwareCPU::Inst_SwapAB(cAvidaContext& ctx)\
{
  nFunctions::Swap(GetRegister(REG_AX), GetRegister(REG_BX)); return true;
}
bool cHardwareCPU::Inst_SwapBC(cAvidaContext& ctx)
{
  nFunctions::Swap(GetRegister(REG_BX), GetRegister(REG_CX)); return true;
}
bool cHardwareCPU::Inst_SwapAC(cAvidaContext& ctx)
{
  nFunctions::Swap(GetRegister(REG_AX), GetRegister(REG_CX)); return true;
}

bool cHardwareCPU::Inst_CopyReg(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindNextRegister(src);
  GetRegister(dst) = GetRegister(src);
  return true;
}

bool cHardwareCPU::Inst_CopyRegAB(cAvidaContext& ctx)
{
  GetRegister(REG_AX) = GetRegister(REG_BX);   return true;
}
bool cHardwareCPU::Inst_CopyRegAC(cAvidaContext& ctx)
{
  GetRegister(REG_AX) = GetRegister(REG_CX);   return true;
}
bool cHardwareCPU::Inst_CopyRegBA(cAvidaContext& ctx)
{
  GetRegister(REG_BX) = GetRegister(REG_AX);   return true;
}
bool cHardwareCPU::Inst_CopyRegBC(cAvidaContext& ctx)
{
  GetRegister(REG_BX) = GetRegister(REG_CX);   return true;
}
bool cHardwareCPU::Inst_CopyRegCA(cAvidaContext& ctx)
{
  GetRegister(REG_CX) = GetRegister(REG_AX);   return true;
}
bool cHardwareCPU::Inst_CopyRegCB(cAvidaContext& ctx)
{
  GetRegister(REG_CX) = GetRegister(REG_BX);   return true;
}

bool cHardwareCPU::Inst_Reset(cAvidaContext& ctx)
{
  GetRegister(REG_AX) = 0;
  GetRegister(REG_BX) = 0;
  GetRegister(REG_CX) = 0;
  StackClear();
  return true;
}

bool cHardwareCPU::Inst_ShiftR(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) >>= 1;
  return true;
}

bool cHardwareCPU::Inst_ShiftL(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) <<= 1;
  return true;
}

bool cHardwareCPU::Inst_Bit1(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) |=  1;
  return true;
}

bool cHardwareCPU::Inst_SetNum(cAvidaContext& ctx)
{
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsInt(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValGrey(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntGreyCode(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValDir(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntDirect(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValAddP(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntAdditivePolynomial(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValFib(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntFib(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValPolyC(cAvidaContext& ctx) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntPolynomialCoefficent(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_Inc(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) += 1;
  return true;
}

bool cHardwareCPU::Inst_Dec(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) -= 1;
  return true;
}

bool cHardwareCPU::Inst_Zero(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = 0;
  return true;
}

bool cHardwareCPU::Inst_Neg(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  GetRegister(dst) = -GetRegister(src);
  return true;
}

bool cHardwareCPU::Inst_Square(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  GetRegister(dst) = GetRegister(src) * GetRegister(src);
  return true;
}

bool cHardwareCPU::Inst_Sqrt(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  const int value = GetRegister(src);
  if (value > 1) GetRegister(dst) = static_cast<int>(sqrt(static_cast<double>(value)));
  else if (value < 0) {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "sqrt: value is negative");
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Log(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  const int value = GetRegister(src);
  if (value >= 1) GetRegister(dst) = static_cast<int>(log(static_cast<double>(value)));
  else if (value < 0) {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "log: value is negative");
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Log10(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  const int value = GetRegister(src);
  if (value >= 1) GetRegister(dst) = static_cast<int>(log10(static_cast<double>(value)));
  else if (value < 0) {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "log10: value is negative");
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Add(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = GetRegister(op1) + GetRegister(op2);
  return true;
}

bool cHardwareCPU::Inst_Sub(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = GetRegister(op1) - GetRegister(op2);
  return true;
}

bool cHardwareCPU::Inst_Mult(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = GetRegister(op1) * GetRegister(op2);
  return true;
}

bool cHardwareCPU::Inst_Div(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
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

bool cHardwareCPU::Inst_Mod(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  if (GetRegister(op2) != 0) {
    GetRegister(dst) = GetRegister(op1) % GetRegister(op2);
  } else {
    organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
    return false;
  }
  return true;
}


bool cHardwareCPU::Inst_Nand(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = ~(GetRegister(op1) & GetRegister(op2));
  return true;
}

bool cHardwareCPU::Inst_Nor(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = ~(GetRegister(op1) | GetRegister(op2));
  return true;
}

bool cHardwareCPU::Inst_And(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = (GetRegister(op1) & GetRegister(op2));
  return true;
}

bool cHardwareCPU::Inst_Not(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  GetRegister(dst) = ~(GetRegister(src));
  return true;
}

bool cHardwareCPU::Inst_Order(cAvidaContext& ctx)
{
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  if (GetRegister(op1) > GetRegister(op2)) {
    nFunctions::Swap(GetRegister(op1), GetRegister(op2));
  }
  return true;
}

bool cHardwareCPU::Inst_Xor(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = GetRegister(op1) ^ GetRegister(op2);
  return true;
}

bool cHardwareCPU::Inst_Copy(cAvidaContext& ctx)
{
  const int op1 = REG_BX;
  const int op2 = REG_AX;

  const cHeadCPU from(this, GetRegister(op1));
  cHeadCPU to(this, GetRegister(op2) + GetRegister(op1));
//  sCPUStats& cpu_stats = organism->CPUStats();
  
  if (organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();  // Mark this instruction as mutated...
    to.SetFlagCopyMut();  // Mark this instruction as copy mut...
                              //organism->GetPhenotype().IsMutated() = true;
//    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(from.GetInst());
    to.ClearFlagMutated();  // UnMark
    to.ClearFlagCopyMut();  // UnMark
  }
  
  to.SetFlagCopied();  // Set the copied flag.
//  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareCPU::Inst_ReadInst(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_CX);
  const int src = REG_BX;

  const cHeadCPU from(this, GetRegister(src));
  
  // Dis-allowing mutations on read, for the moment (write only...)
  // @CAO This allows perfect error-correction...
  GetRegister(dst) = from.GetInst().GetOp();
  return true;
}

bool cHardwareCPU::Inst_WriteInst(cAvidaContext& ctx)
{
  const int src = FindModifiedRegister(REG_CX);
  const int op1 = REG_BX;
  const int op2 = REG_AX;

  cHeadCPU to(this, GetRegister(op2) + GetRegister(op1));
  const int value = Mod(GetRegister(src), m_inst_set->GetSize());
//  sCPUStats& cpu_stats = organism->CPUStats();

  // Change value on a mutation...
  if (organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();      // Mark this instruction as mutated...
    to.SetFlagCopyMut();      // Mark this instruction as copy mut...
                                  //organism->GetPhenotype().IsMutated() = true;
//    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(cInstruction(value));
    to.ClearFlagMutated();     // UnMark
    to.ClearFlagCopyMut();     // UnMark
  }

  to.SetFlagCopied();  // Set the copied flag.
//  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareCPU::Inst_StackReadInst(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_CX);
  cHeadCPU from(this, GetRegister(reg_used));
  StackPush(from.GetInst().GetOp());
  return true;
}

bool cHardwareCPU::Inst_StackWriteInst(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_AX;
  cHeadCPU to(this, GetRegister(op1) + GetRegister(dst));
  const int value = Mod(StackPop(), m_inst_set->GetSize());
//  sCPUStats& cpu_stats = organism->CPUStats();
  
  // Change value on a mutation...
  if (organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();      // Mark this instruction as mutated...
    to.SetFlagCopyMut();      // Mark this instruction as copy mut...
                                  //organism->GetPhenotype().IsMutated() = true;
//    cpu_stats.mut_stats.copy_mut_count++;
  } else {
    to.SetInst(cInstruction(value));
    to.ClearFlagMutated();     // UnMark
    to.ClearFlagCopyMut();     // UnMark
  }
  
  to.SetFlagCopied();  // Set the copied flag.
//  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareCPU::Inst_Compare(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(REG_CX);
  const int op1 = REG_BX;
  const int op2 = REG_AX;

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

bool cHardwareCPU::Inst_IfNCpy(cAvidaContext& ctx)
{
  const int op1 = REG_BX;
  const int op2 = REG_AX;

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

bool cHardwareCPU::Inst_Allocate(cAvidaContext& ctx)   // Allocate bx more space...
{
  const int src = REG_BX;
  const int dst = REG_AX;
  const int size = GetMemory().GetSize();
  if (Allocate_Main(ctx, GetRegister(src))) {
    GetRegister(dst) = size;
    return true;
  } else return false;
}

bool cHardwareCPU::Inst_Divide(cAvidaContext& ctx)  
{ 
  const int src = REG_AX;
  return Divide_Main(ctx, GetRegister(src));    
}

/*
  Divide with resampling -- Same as regular divide but on reversions will be 
  resampled after they are reverted.

  AWC 06/29/06

 */

bool cHardwareCPU::Inst_DivideRS(cAvidaContext& ctx)  
{ 
  const int src = REG_AX;
  return Divide_MainRS(ctx, GetRegister(src));    
}


bool cHardwareCPU::Inst_CDivide(cAvidaContext& ctx) 
{ 
  return Divide_Main(ctx, GetMemory().GetSize() / 2);   
}

bool cHardwareCPU::Inst_CAlloc(cAvidaContext& ctx)  
{ 
  return Allocate_Main(ctx, GetMemory().GetSize());   
}

bool cHardwareCPU::Inst_MaxAlloc(cAvidaContext& ctx)   // Allocate maximal more
{
  const int dst = REG_AX;
  const int cur_size = GetMemory().GetSize();
  const int alloc_size = Min((int) (m_world->GetConfig().CHILD_SIZE_RANGE.Get() * cur_size),
                             MAX_CREATURE_SIZE - cur_size);
  if (Allocate_Main(ctx, alloc_size)) {
    GetRegister(dst) = cur_size;
    return true;
  } else return false;
}

// Alloc and move write head if we're successful
bool cHardwareCPU::Inst_MaxAllocMoveWriteHead(cAvidaContext& ctx)   // Allocate maximal more
{
  const int dst = REG_AX;
  const int cur_size = GetMemory().GetSize();
  const int alloc_size = Min((int) (m_world->GetConfig().CHILD_SIZE_RANGE.Get() * cur_size),
                             MAX_CREATURE_SIZE - cur_size);
  if (Allocate_Main(ctx, alloc_size)) {
    GetRegister(dst) = cur_size;
    GetHead(nHardware::HEAD_WRITE).Set(cur_size);
    return true;
  } else return false;
}

bool cHardwareCPU::Inst_Transposon(cAvidaContext& ctx)
{
  ReadLabel();
  //organism->GetPhenotype().ActivateTransposon(GetLabel());
  return true;
}

void cHardwareCPU::Divide_DoTransposons(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_Repro(cAvidaContext& ctx)
{
  // const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  // these checks should be done, but currently they make some assumptions
  // that crash when evaluating this kind of organism -- JEB

  if (organism->GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
  // Setup child
  cCPUMemory& child_genome = organism->ChildGenome();
  child_genome = GetMemory();
  organism->GetPhenotype().SetLinesCopied(GetMemory().GetSize());

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
  
  const bool parent_alive = organism->ActivateDivide(ctx);
  
  //Reset the parent
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset();
  }
  return true;
}

bool cHardwareCPU::Inst_TaskPutRepro(cAvidaContext& ctx)
{
  // Do normal IO, but don't zero register
  //Inst_TaskPut(ctx);
  
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = GetRegister(reg_used);
 // GetRegister(reg_used) = 0;
  organism->DoOutput(ctx, value);
  
  // Immediately attempt a repro
  return Inst_Repro(ctx);
}

bool cHardwareCPU::Inst_TaskPutResetInputsRepro(cAvidaContext& ctx)
{
  // Do normal IO
  bool return_value = Inst_TaskPutResetInputs(ctx);
  
  // Immediately attempt a repro
  Inst_Repro(ctx);

  // return value of put since successful repro would wipe state anyway
  return return_value; 
}

bool cHardwareCPU::Inst_SpawnDeme(cAvidaContext& ctx)
{
  organism->SpawnDeme();
  return true;
}

bool cHardwareCPU::Inst_Kazi(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_AX);
  double percentProb = ((double) (GetRegister(reg_used) % 100)) / 100.0;
  if ( ctx.GetRandom().P(percentProb) ) organism->Kaboom(0);
  return true;
}

                   
bool cHardwareCPU::Inst_Sterilize(cAvidaContext& ctx)
{
  organism->GetPhenotype().IsFertile() = false;
  return true;
}

bool cHardwareCPU::Inst_Kazi5(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_AX);
  double percentProb = ((double) (GetRegister(reg_used) % 100)) / 100.0;
  if ( ctx.GetRandom().P(percentProb) ) organism->Kaboom(5);
  return true;
}

bool cHardwareCPU::Inst_Die(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_Inject(cAvidaContext& ctx)
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


bool cHardwareCPU::Inst_InjectRand(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_InjectThread(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_TaskGet(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_CX);
  const int value = organism->GetNextInput();
  GetRegister(reg_used) = value;
  organism->DoInput(value);
  return true;
}


// @JEB - this instruction does more than two "gets" together, it also resets the inputs
bool cHardwareCPU::Inst_TaskGet2(cAvidaContext& ctx)
{
  // Randomize the inputs so they can't save numbers
  organism->GetOrgInterface().ResetInputs(ctx);   // Now re-randomize the inputs this organism sees
  organism->ClearInput();                         // Also clear their input buffers, or they can still claim
                                                  // rewards for numbers no longer in their environment!

  const int reg_used_1 = FindModifiedRegister(REG_BX);
  const int reg_used_2 = FindNextRegister(reg_used_1);
  
  const int value1 = organism->GetNextInput();
  GetRegister(reg_used_1) = value1;
  organism->DoInput(value1);
  
  const int value2 = organism->GetNextInput();
  GetRegister(reg_used_2) = value2;
  organism->DoInput(value2);
  
  // Clear the task number
  //organism->GetPhenotype().ClearEffTaskCount();
  
  return true;
}

bool cHardwareCPU::Inst_TaskStackGet(cAvidaContext& ctx)
{
  const int value = organism->GetNextInput();
  StackPush(value);
  organism->DoInput(value);
  return true;
}

bool cHardwareCPU::Inst_TaskStackLoad(cAvidaContext& ctx)
{
  // @DMB - TODO: this should look at the input_size...
  for (int i = 0; i < 3; i++) 
    StackPush( organism->GetNextInput() );
  return true;
}

bool cHardwareCPU::Inst_TaskPut(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = GetRegister(reg_used);
  GetRegister(reg_used) = 0;
  organism->DoOutput(ctx, value);
  return true;
}

bool cHardwareCPU::Inst_TaskPutResetInputs(cAvidaContext& ctx)
{
  bool return_value = Inst_TaskPut(ctx);          // Do a normal put
  organism->GetOrgInterface().ResetInputs(ctx);   // Now re-randomize the inputs this organism sees
  organism->ClearInput();                         // Also clear their input buffers, or they can still claim
                                                  // rewards for numbers no longer in their environment!
  return return_value;
}

bool cHardwareCPU::Inst_TaskIO(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_TaskIO_BonusCost(cAvidaContext& ctx, double bonus_cost)
{
  // Levy the cost
  double new_bonus = organism->GetPhenotype().GetCurBonus() * (1 - bonus_cost);
  if (new_bonus < 0) new_bonus = 0;
  //keep the bonus positive or zero
  organism->GetPhenotype().SetCurBonus(new_bonus);
  
  return Inst_TaskIO(ctx);
}

bool cHardwareCPU::Inst_TaskIO_Feedback(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_MatchStrings(cAvidaContext& ctx)
{
	if (m_executedmatchstrings)
		return false;
	organism->DoOutput(ctx, 357913941);
	m_executedmatchstrings = true;
	return true;
}

bool cHardwareCPU::Inst_Sell(cAvidaContext& ctx)
{
	int search_label = GetLabel().AsInt(3) % MARKET_SIZE;
	int send_value = GetRegister(REG_BX);
	int sell_price = m_world->GetConfig().SELL_PRICE.Get();
	organism->SellValue(send_value, search_label, sell_price);
	return true;
}

bool cHardwareCPU::Inst_Buy(cAvidaContext& ctx)
{
	int search_label = GetLabel().AsInt(3) % MARKET_SIZE;
	int buy_price = m_world->GetConfig().BUY_PRICE.Get();
	GetRegister(REG_BX) = organism->BuyValue(search_label, buy_price);
	return true;
}

bool cHardwareCPU::Inst_Send(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  organism->SendValue(GetRegister(reg_used));
  GetRegister(reg_used) = 0;
  return true;
}

bool cHardwareCPU::Inst_Receive(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = organism->ReceiveValue();
  return true;
}

bool cHardwareCPU::Inst_SenseLog2(cAvidaContext& ctx)
{
  return DoSense(ctx, 0, 2);
}

bool cHardwareCPU::Inst_SenseUnit(cAvidaContext& ctx)
{
  return DoSense(ctx, 1, 1);
}

bool cHardwareCPU::Inst_SenseMult100(cAvidaContext& ctx)
{
  return DoSense(ctx, 1, 100);
}

bool cHardwareCPU::DoSense(cAvidaContext& ctx, int conversion_method, double base)
{
  // Returns the amount of a resource or resources 
  // specified by modifying NOPs into register BX
  const tArray<double> & res_count = organism->GetOrgInterface().GetResources();

  // Arbitrarily set to BX since the conditional instructions use this directly.
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
  ReadLabel(max_label_length);
  
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
  double dresource_result = 0;
  for (int i = start_index; i <= end_index; i++)
  {
    // if it's a valid resource
    if (i < res_count.GetSize())
    {
      if (conversion_method == 0) // Log
      {
        // for log, add together and then take log
        dresource_result += (double) res_count[i];
      }
      else if (conversion_method == 1) // Addition of multiplied resource amount
      {
        int add_amount = (int) (res_count[i] * base);
        // Do some range checking to make sure we don't overflow
        resource_result = (INT_MAX - resource_result <= add_amount) ? INT_MAX : resource_result + add_amount;
      }
    } 
  }
  
  // Take the log after adding resource amounts together!
  // Otherwise 
  if (conversion_method == 0) // Log2
  {
    // You really shouldn't be using the log method if you can get to zero resources
    assert(dresource_result != 0);
    resource_result = (int)(log(dresource_result)/log(base));
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

void cHardwareCPU::DoDonate(cOrganism* to_org)
{
  assert(to_org != NULL);

  const double merit_given = m_world->GetConfig().MERIT_GIVEN.Get();
  const double merit_received = m_world->GetConfig().MERIT_RECEIVED.Get();

  double cur_merit = organism->GetPhenotype().GetMerit().GetDouble();
  cur_merit -= merit_given;
  if(cur_merit < 0) cur_merit=0; 

  // Plug the current merit back into this organism and notify the scheduler.
  organism->UpdateMerit(cur_merit);
  
  // Update the merit of the organism being donated to...
  double other_merit = to_org->GetPhenotype().GetMerit().GetDouble();
  other_merit += merit_received;
  to_org->UpdateMerit(other_merit);
}

void cHardwareCPU::DoEnergyDonate(cOrganism* to_org)
{
  assert(to_org != NULL);

  const double frac_energy_given = m_world->GetConfig().MERIT_GIVEN.Get();

  double cur_energy = organism->GetPhenotype().GetStoredEnergy();
  double energy_given = cur_energy * frac_energy_given;
  
  //update energy store and merit of donor
  organism->GetPhenotype().ReduceEnergy(energy_given);
  double senderMerit = cMerit::EnergyToMerit(organism->GetPhenotype().GetStoredEnergy(), m_world) * organism->GetPhenotype().GetExecutionRatio();
  organism->UpdateMerit(senderMerit);
  
  // update energy store and merit of donee
  to_org->GetPhenotype().ReduceEnergy(-1.0*energy_given);
  double receiverMerit = cMerit::EnergyToMerit(to_org->GetPhenotype().GetStoredEnergy(), m_world) * to_org->GetPhenotype().GetExecutionRatio();
  to_org->UpdateMerit(receiverMerit);
}

bool cHardwareCPU::Inst_DonateFacing(cAvidaContext& ctx) {
  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }
  organism->GetPhenotype().IncDonates();
  organism->GetPhenotype().SetIsDonorRand();

  // Get faced neighbor
  cOrganism * neighbor = organism->GetNeighbor();
  
  // Donate only if we have found a neighbor.
  if (neighbor != NULL) {
    DoEnergyDonate(neighbor);
    
    neighbor->GetPhenotype().SetIsReceiver();
  }
  return true;
}

bool cHardwareCPU::Inst_DonateRandom(cAvidaContext& ctx)
{
  
  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }

  organism->GetPhenotype().IncDonates();
  organism->GetPhenotype().SetIsDonorRand();

  // Turn to a random neighbor, get it, and turn back...
  int neighbor_id = ctx.GetRandom().GetInt(organism->GetNeighborhoodSize());
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(-1);
  
  // Donate only if we have found a neighbor.
  if (neighbor != NULL) {
    DoDonate(neighbor);
    
    //print out how often random donations go to kin
    /*
    static ofstream kinDistanceFile("kinDistance.dat");
    kinDistanceFile << (genotype->GetPhyloDistance(neighbor->GetGenotype())<=1) << " ";
    kinDistanceFile << (genotype->GetPhyloDistance(neighbor->GetGenotype())<=2) << " ";
    kinDistanceFile << (genotype->GetPhyloDistance(neighbor->GetGenotype())<=3) << " ";
    kinDistanceFile << genotype->GetPhyloDistance(neighbor->GetGenotype());
    kinDistanceFile << endl; 
    */
    neighbor->GetPhenotype().SetIsReceiverRand();
  }

  return true;
}


bool cHardwareCPU::Inst_DonateKin(cAvidaContext& ctx)
{
  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }
  
  organism->GetPhenotype().IncDonates();
  organism->GetPhenotype().SetIsDonorKin();


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
  if (neighbor != NULL){
    DoDonate(neighbor);
    neighbor->GetPhenotype().SetIsReceiverKin();
  }
  return true;
}

bool cHardwareCPU::Inst_DonateEditDist(cAvidaContext& ctx)
{
  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }

  organism->GetPhenotype().IncDonates();
  organism->GetPhenotype().SetIsDonorEdit();
  
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
  if (neighbor != NULL){
    DoDonate(neighbor);
    neighbor->GetPhenotype().SetIsReceiverEdit();
  }
  return true;
}

bool cHardwareCPU::Inst_DonateGreenBeardGene(cAvidaContext& ctx)
{
  //this donates to organisms that have this instruction anywhere
  //in their genome (see Dawkins 1976, The Selfish Gene, for 
  //the history of the theory and the name 'green beard'
  cPhenotype & phenotype = organism->GetPhenotype();

  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }

  phenotype.IncDonates();
  phenotype.SetIsDonorGbg();

  // Find the target as the first match found in the neighborhood.

  //get the neighborhood size
  const int num_neighbors = organism->GetNeighborhoodSize();

  // Turn to face a random neighbor
  int neighbor_id = ctx.GetRandom().GetInt(num_neighbors);
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();

  int max_id = neighbor_id + num_neighbors;
 
  //we have not found a match yet
  bool found = false;

  // rotate through orgs in neighborhood  
  while (neighbor_id < max_id) {
      neighbor = organism->GetNeighbor();

      //if neighbor exists, do they have the green beard gene?
      if (neighbor != NULL) {
          const cGenome & neighbor_genome = neighbor->GetGenome();

          // for each instruction in the genome...
          for(int i=0;i<neighbor_genome.GetSize();i++){

            // ...see if it is donate-gbg
            if (neighbor_genome[i] == IP().GetInst()) {
              found = true;
              break;
            }
	    
          }
      }
      
      // stop searching through the neighbors if we already found one
      if (found == true);{
    	break;
      }
  
      organism->Rotate(1);
      neighbor_id++;
  }

    if (found == false) neighbor = NULL;

  // Put the facing back where it was.
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(-1);

  // Donate only if we have found a close enough relative...
  if (neighbor != NULL) {
    DoDonate(neighbor);
    neighbor->GetPhenotype().SetIsReceiverGbg();
  }
  
  return true;
  
}

bool cHardwareCPU::Inst_DonateTrueGreenBeard(cAvidaContext& ctx)
{
  //this donates to organisms that have this instruction anywhere
  //in their genome AND their parents excuted it
  //(see Dawkins 1976, The Selfish Gene, for 
  //the history of the theory and the name 'green beard'
  //  cout << "i am about to donate to a green beard" << endl;
  cPhenotype & phenotype = organism->GetPhenotype();

  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }

  phenotype.IncDonates();
  phenotype.SetIsDonorTrueGb();

  // Find the target as the first match found in the neighborhood.

  //get the neighborhood size
  const int num_neighbors = organism->GetNeighborhoodSize();

  // Turn to face a random neighbor
  int neighbor_id = ctx.GetRandom().GetInt(num_neighbors);
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();

  int max_id = neighbor_id + num_neighbors;
 
  //we have not found a match yet
  bool found = false;

  // rotate through orgs in neighborhood  
  while (neighbor_id < max_id) {
      neighbor = organism->GetNeighbor();
      //if neighbor exists, AND if their parent attempted to donate,
      if (neighbor != NULL && neighbor->GetPhenotype().IsDonorTrueGbLast()) {
          const cGenome & neighbor_genome = neighbor->GetGenome();

          // for each instruction in the genome...
          for(int i=0;i<neighbor_genome.GetSize();i++){

            // ...see if it is donate-tgb, if so, we found a target
            if (neighbor_genome[i] == IP().GetInst()) {
              found = true;
              break;
            }
	    
          }
      }
      
      // stop searching through the neighbors if we already found one
      if (found == true);{
    	break;
      }
  
      organism->Rotate(1);
      neighbor_id++;
  }

    if (found == false) neighbor = NULL;

  // Put the facing back where it was.
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(-1);

  // Donate only if we have found a close enough relative...
  if (neighbor != NULL) {
    DoDonate(neighbor);
    neighbor->GetPhenotype().SetIsReceiverTrueGb();
  }

  
  return true;
  
}

bool cHardwareCPU::Inst_DonateThreshGreenBeard(cAvidaContext& ctx)
{
  //this donates to organisms that have this instruction anywhere
  //in their genome AND their parents excuted it >=THRESHOLD number of times
  //(see Dawkins 1976, The Selfish Gene, for 
  //the history of the theory and the name 'green beard'
  //  cout << "i am about to donate to a green beard" << endl;
  cPhenotype & phenotype = organism->GetPhenotype();

  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }


  phenotype.IncDonates();
  phenotype.SetIsDonorThreshGb();
  phenotype.IncNumThreshGbDonations();

  // Find the target as the first match found in the neighborhood.

  //get the neighborhood size
  const int num_neighbors = organism->GetNeighborhoodSize();

  // Turn to face a random neighbor
  int neighbor_id = ctx.GetRandom().GetInt(num_neighbors);
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();

  int max_id = neighbor_id + num_neighbors;
 
  //we have not found a match yet
  bool found = false;

  // rotate through orgs in neighborhood  
  while (neighbor_id < max_id) {
      neighbor = organism->GetNeighbor();
      //if neighbor exists, AND if their parent attempted to donate >= threshhold,
      if (neighbor != NULL && neighbor->GetPhenotype().GetNumThreshGbDonationsLast()>= m_world->GetConfig().MIN_GB_DONATE_THRESHOLD.Get() ) {
          const cGenome & neighbor_genome = neighbor->GetGenome();

          // for each instruction in the genome...
          for(int i=0;i<neighbor_genome.GetSize();i++){

	         // ...see if it is donate-threshgb, if so, we found a target
            if (neighbor_genome[i] == IP().GetInst()) {
              found = true;
              break;
            }
	    
          }
      }
      
      // stop searching through the neighbors if we already found one
      if (found == true);{
    	break;
      }
  
      organism->Rotate(1);
      neighbor_id++;
  }

    if (found == false) neighbor = NULL;

  // Put the facing back where it was.
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(-1);

  // Donate only if we have found a close enough relative...
  if (neighbor != NULL) {
    DoDonate(neighbor);
    neighbor->GetPhenotype().SetIsReceiverThreshGb();
    // cout << "************ neighbor->GetPhenotype().GetNumThreshGbDonationsLast() is " << neighbor->GetPhenotype().GetNumThreshGbDonationsLast() << endl;
    
  }

  return true;
  
}


bool cHardwareCPU::Inst_DonateQuantaThreshGreenBeard(cAvidaContext& ctx)
{
  // this donates to organisms that have this instruction anywhere
  // in their genome AND their parents excuted it more than a
  // THRESHOLD number of times where that threshold depend on the
  // number of times the individual's parents attempted to donate
  // using this instruction.  The threshold levels are multiples of
  // the quanta value set in genesis, and the highest level that
  // the donor qualifies for is the one used.

  // (see Dawkins 1976, The Selfish Gene, for 
  // the history of the theory and the name 'green beard'
  //  cout << "i am about to donate to a green beard" << endl;
  cPhenotype & phenotype = organism->GetPhenotype();

  if (phenotype.GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }

  phenotype.IncDonates();
  phenotype.SetIsDonorQuantaThreshGb();
  phenotype.IncNumQuantaThreshGbDonations();
  //cout << endl << "quanta_threshgb attempt.. " ;


  // Find the target as the first match found in the neighborhood.

  //get the neighborhood size
  const int num_neighbors = organism->GetNeighborhoodSize();

  // Turn to face a random neighbor
  int neighbor_id = ctx.GetRandom().GetInt(num_neighbors);
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(1);
  cOrganism * neighbor = organism->GetNeighbor();

  int max_id = neighbor_id + num_neighbors;
 
  //we have not found a match yet
  bool found = false;

  // Get the quanta (step size) between threshold levels.
  const int donate_quanta = m_world->GetConfig().DONATE_THRESH_QUANTA.Get();
  
  // Calculate what quanta level we should be at for this individual.  We do a
  // math trick to make sure its the next lowest event multiple of donate_quanta.
  const int quanta_donate_thresh =
    (phenotype.GetNumQuantaThreshGbDonationsLast() / donate_quanta) * donate_quanta;
  //cout << " phenotype.GetNumQuantaThreshGbDonationsLast() is " << phenotype.GetNumQuantaThreshGbDonationsLast();
  //cout << " quanta thresh=  " << quanta_donate_thresh;
  // rotate through orgs in neighborhood  
  while (neighbor_id < max_id) {
      neighbor = organism->GetNeighbor();
      //if neighbor exists, AND if their parent attempted to donate >= threshhold,
      if (neighbor != NULL &&
	  neighbor->GetPhenotype().GetNumQuantaThreshGbDonationsLast() >= quanta_donate_thresh) {

          const cGenome & neighbor_genome = neighbor->GetGenome();

          // for each instruction in the genome...
          for(int i=0;i<neighbor_genome.GetSize();i++){

	         // ...see if it is donate-quantagb, if so, we found a target
            if (neighbor_genome[i] == IP().GetInst()) {
              found = true;
              break;
            }
	    
          }
      }
      
      // stop searching through the neighbors if we already found one
      if (found == true);{
    	break;
      }
  
      organism->Rotate(1);
      neighbor_id++;
  }

    if (found == false) neighbor = NULL;

  // Put the facing back where it was.
  for (int i = 0; i < neighbor_id; i++) organism->Rotate(-1);

  // Donate only if we have found a close enough relative...
  if (neighbor != NULL) {
    DoDonate(neighbor);
    neighbor->GetPhenotype().SetIsReceiverQuantaThreshGb();
    //cout << " ************ neighbor->GetPhenotype().GetNumQuantaThreshGbDonationsLast() is " << neighbor->GetPhenotype().GetNumQuantaThreshGbDonationsLast();
    
  }

  return true;
  
}


bool cHardwareCPU::Inst_DonateNULL(cAvidaContext& ctx)
{
  if (organism->GetPhenotype().GetCurNumDonates() > m_world->GetConfig().MAX_DONATES.Get()) {
    return false;
  }

  organism->GetPhenotype().IncDonates();
  organism->GetPhenotype().SetIsDonorNull();
  
  // This is a fake donate command that causes the organism to lose merit,
  // but no one else to gain any.
  
  const double merit_given = m_world->GetConfig().MERIT_GIVEN.Get();
  double cur_merit = organism->GetPhenotype().GetMerit().GetDouble();
  cur_merit -= merit_given;
  
  // Plug the current merit back into this organism and notify the scheduler.
  organism->UpdateMerit(cur_merit);
  
  return true;
}


bool cHardwareCPU::Inst_SearchF(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  const int search_size = FindLabel(1).GetPosition() - IP().GetPosition();
  GetRegister(REG_BX) = search_size;
  GetRegister(REG_CX) = GetLabel().GetSize();
  return true;
}

bool cHardwareCPU::Inst_SearchB(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  const int search_size = IP().GetPosition() - FindLabel(-1).GetPosition();
  GetRegister(REG_BX) = search_size;
  GetRegister(REG_CX) = GetLabel().GetSize();
  return true;
}

bool cHardwareCPU::Inst_MemSize(cAvidaContext& ctx)
{
  GetRegister(FindModifiedRegister(REG_BX)) = GetMemory().GetSize();
  return true;
}

bool cHardwareCPU::Inst_IOBufAdd1(cAvidaContext& ctx)
{ 
	organism->AddOutput(1);
	return true;
}
bool cHardwareCPU::Inst_IOBufAdd0(cAvidaContext& ctx)
{ 
	organism->AddOutput(0);
	return true; 
}

bool cHardwareCPU::Inst_RotateL(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_RotateR(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_RotateLeftOne(cAvidaContext& ctx)
{
  organism->Rotate(-1);
  return true;
}

bool cHardwareCPU::Inst_RotateRightOne(cAvidaContext& ctx)
{
  organism->Rotate(1);
  return true;
}

/**
  Rotate to facing specified by following label
*/
bool cHardwareCPU::Inst_RotateLabel(cAvidaContext& ctx)
{
  int standardNeighborhoodSize, actualNeighborhoodSize, newFacing, currentFacing;
  actualNeighborhoodSize = organism->GetNeighborhoodSize();
  
  ReadLabel();
  if(m_world->GetConfig().WORLD_GEOMETRY.Get() == nGeometry::TORUS || m_world->GetConfig().WORLD_GEOMETRY.Get() == nGeometry::GRID)
    standardNeighborhoodSize = 8;
  else {
    exit(-1);
  }
  newFacing = GetLabel().AsIntGreyCode(NUM_NOPS) % standardNeighborhoodSize;
  
  for(int i = 0; i < actualNeighborhoodSize; i++) {
    currentFacing = organism->GetFacing();
    if(newFacing == currentFacing)
      break;
    organism->Rotate(1);
  }
  return true;
}

bool cHardwareCPU::Inst_SetCopyMut(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int new_mut_rate = Max(GetRegister(reg_used), 1 );
  organism->SetCopyMutProb(static_cast<double>(new_mut_rate) / 10000.0);
  return true;
}

bool cHardwareCPU::Inst_ModCopyMut(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const double new_mut_rate = organism->GetCopyMutProb() + static_cast<double>(GetRegister(reg_used)) / 10000.0;
  if (new_mut_rate > 0.0) organism->SetCopyMutProb(new_mut_rate);
  return true;
}

// @WRE addition for movement
// Tumble sets the organism and cell to a new random facing
// 
bool cHardwareCPU::Inst_Tumble(cAvidaContext& ctx)
{
  // Code taken from Inst_Inject
  //cout << "Tumble: start" << endl;
  const int num_neighbors = organism->GetNeighborhoodSize();
  //cout << "Tumble: size = " << num_neighbors << endl;
  organism->Rotate(ctx.GetRandom().GetUInt(num_neighbors));
  return true;
}

// @WRE addition for movement
// Move uses the cPopulation::SwapCells method to move an organism to a different cell
// and the cPopulation::MoveOrganisms helper function to clean up after a move
// The cell selected as a destination is the one faced
bool cHardwareCPU::Inst_Move(cAvidaContext& ctx)
{
  // Declarations
  int fromcellID, destcellID; //, actualNeighborhoodSize, fromFacing, destFacing, currentFacing;

  // Get population
  cPopulation& pop = m_world->GetPopulation();

  // Get stepsize. Currently, all moves are one cell regardless of stepsize.
  // This could be changed in the future.
  const int stepsize = m_world->GetConfig().BIOMIMETIC_MOVEMENT_STEP.Get();
  
  // Code
  if (0 < stepsize) {
    // Current cell
    fromcellID = organism->GetCellID();
    // With sanity check
    if (-1  == fromcellID) return false;
    // Destination cell
    destcellID = pop.GetCell(fromcellID).GetCellFaced().GetID();
    // Actually perform the move using SwapCells
    //cout << "SwapCells: " << fromcellID << " to " << destcellID << endl;
    pop.SwapCells(pop.GetCell(fromcellID),pop.GetCell(destcellID));
    // Swap inputs and facings between cells using helper function
    pop.MoveOrganisms(ctx, pop.GetCell(fromcellID), pop.GetCell(destcellID));

    return true;
  } else {
    return false;
  }
}

// Multi-threading.

bool cHardwareCPU::Inst_ForkThread(cAvidaContext& ctx)
{
  IP().Advance();
  if (!ForkThread()) organism->Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
  return true;
}

bool cHardwareCPU::Inst_ForkThreadLabel(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  
  // If there is no label, then do normal fork behavior
  if (GetLabel().GetSize() == 0)
  {
    return Inst_ForkThread(ctx);
  }
  
  cHeadCPU searchHead = FindLabel(+1);
  if ( searchHead.GetPosition() != IP().GetPosition() )
  {
    int save_pos = IP().GetPosition();
    IP().Set(searchHead.GetPosition() + 1);
    if (!ForkThread()) organism->Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
    IP().Set( save_pos );
  }
  
  return true;
}

bool cHardwareCPU::Inst_ForkThreadLabelIfNot0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) == 0) 
  {
    ReadLabel();
    return false;
  }
  return Inst_ForkThreadLabel(ctx);
}

bool cHardwareCPU::Inst_ForkThreadLabelIf0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) != 0)
  {
    ReadLabel();
    return false;
  }
  return Inst_ForkThreadLabel(ctx);
}

bool cHardwareCPU::Inst_KillThread(cAvidaContext& ctx)
{
  if (!KillThread()) organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  else m_advance_ip = false;
  return true;
}

bool cHardwareCPU::Inst_ThreadID(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = GetCurThreadID();
  return true;
}


// Head-based instructions

bool cHardwareCPU::Inst_SetHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  m_threads[m_cur_thread].cur_head = static_cast<unsigned char>(head_used);
  return true;
}

bool cHardwareCPU::Inst_AdvanceHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_WRITE);
  GetHead(head_used).Advance();
  return true;
}

bool cHardwareCPU::Inst_MoveHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = nHardware::HEAD_FLOW;
  GetHead(head_used).Set(GetHead(target));
  if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  return true;
}

bool cHardwareCPU::Inst_JumpHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  GetHead(head_used).Jump(GetRegister(REG_CX) );
  // @JEB - probably shouldn't advance IP after jumping here?
  // Any negative number jumps to the beginning of the genome (pos 0)
  // and then we immediately advance past that first instruction.
  return true;
}

bool cHardwareCPU::Inst_GetHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  GetRegister(REG_CX) = GetHead(head_used).GetPosition();
  return true;
}

bool cHardwareCPU::Inst_IfLabel(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel())  IP().Advance();
  return true;
}

// This is a variation on IfLabel that will skip the next command if the "if"
// is false, but it will also skip all nops following that command.
bool cHardwareCPU::Inst_IfLabel2(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel()) {
    IP().Advance();
    if (m_inst_set->IsNop( IP().GetNextInst() ))  IP().Advance();
  }
  return true;
}

bool cHardwareCPU::Inst_HeadDivideMut(cAvidaContext& ctx, double mut_multiplier)
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

bool cHardwareCPU::Inst_HeadDivide(cAvidaContext& ctx)
{
  return Inst_HeadDivideMut(ctx, 1);
  
}

/*
  Resample Divide -- AWC 06/29/06
*/

bool cHardwareCPU::Inst_HeadDivideRS(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_HeadDivide1RS(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_HeadDivide2RS(cAvidaContext& ctx)
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


bool cHardwareCPU::Inst_HeadDivideSex(cAvidaContext& ctx)  
{ 
  organism->GetPhenotype().SetDivideSex(true);
  organism->GetPhenotype().SetCrossNum(1);
  return Inst_HeadDivide(ctx); 
}

bool cHardwareCPU::Inst_HeadDivideAsex(cAvidaContext& ctx)  
{ 
  organism->GetPhenotype().SetDivideSex(false);
  organism->GetPhenotype().SetCrossNum(0);
  return Inst_HeadDivide(ctx); 
}

bool cHardwareCPU::Inst_HeadDivideAsexWait(cAvidaContext& ctx)  
{ 
  organism->GetPhenotype().SetDivideSex(true);
  organism->GetPhenotype().SetCrossNum(0);
  return Inst_HeadDivide(ctx); 
}

bool cHardwareCPU::Inst_HeadDivideMateSelect(cAvidaContext& ctx)  
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

bool cHardwareCPU::Inst_HeadDivide1(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 1); }
bool cHardwareCPU::Inst_HeadDivide2(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 2); }
bool cHardwareCPU::Inst_HeadDivide3(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 3); }
bool cHardwareCPU::Inst_HeadDivide4(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 4); }
bool cHardwareCPU::Inst_HeadDivide5(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 5); }
bool cHardwareCPU::Inst_HeadDivide6(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 6); }
bool cHardwareCPU::Inst_HeadDivide7(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 7); }
bool cHardwareCPU::Inst_HeadDivide8(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 8); }
bool cHardwareCPU::Inst_HeadDivide9(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 9); }
bool cHardwareCPU::Inst_HeadDivide10(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 10); }
bool cHardwareCPU::Inst_HeadDivide16(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 16); }
bool cHardwareCPU::Inst_HeadDivide32(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 32); }
bool cHardwareCPU::Inst_HeadDivide50(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 50); }
bool cHardwareCPU::Inst_HeadDivide100(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 100); }
bool cHardwareCPU::Inst_HeadDivide500(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 500); }
bool cHardwareCPU::Inst_HeadDivide1000(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 1000); }
bool cHardwareCPU::Inst_HeadDivide5000(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 5000); }
bool cHardwareCPU::Inst_HeadDivide10000(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 10000); }
bool cHardwareCPU::Inst_HeadDivide50000(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 50000); }
bool cHardwareCPU::Inst_HeadDivide0_5(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.5); }
bool cHardwareCPU::Inst_HeadDivide0_1(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.1); }
bool cHardwareCPU::Inst_HeadDivide0_05(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.05); }
bool cHardwareCPU::Inst_HeadDivide0_01(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.01); }
bool cHardwareCPU::Inst_HeadDivide0_001(cAvidaContext& ctx)  { return Inst_HeadDivideMut(ctx, 0.001); }

bool cHardwareCPU::Inst_HeadRead(cAvidaContext& ctx)
{
  const int dst = REG_BX;
  
  const int head_id = FindModifiedHead(nHardware::HEAD_READ);
  GetHead(head_id).Adjust();
//  sCPUStats & cpu_stats = organism->CPUStats();
  
  // Mutations only occur on the read, for the moment.
  int read_inst = 0;
  if (organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx).GetOp();
//    cpu_stats.mut_stats.copy_mut_count++;  // @CAO, hope this is good!
  } else {
    read_inst = GetHead(head_id).GetInst().GetOp();
  }
  GetRegister(dst) = read_inst;
  ReadInst(read_inst);
  
//  cpu_stats.mut_stats.copies_exec++;  // @CAO, this too..
  GetHead(head_id).Advance();
  return true;
}

bool cHardwareCPU::Inst_HeadWrite(cAvidaContext& ctx)
{
  const int src = REG_BX;
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

bool cHardwareCPU::Inst_HeadCopy(cAvidaContext& ctx)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU& read_head = GetHead(nHardware::HEAD_READ);
  cHeadCPU& write_head = GetHead(nHardware::HEAD_WRITE);
//  sCPUStats& cpu_stats = organism->CPUStats();
  
  read_head.Adjust();
  write_head.Adjust();
  
  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  ReadInst(read_inst.GetOp());
  if (organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx);
//    cpu_stats.mut_stats.copy_mut_count++; 
    write_head.SetFlagMutated();
    write_head.SetFlagCopyMut();
  }
  
//  cpu_stats.mut_stats.copies_exec++;
  write_head.SetInst(read_inst);
  write_head.SetFlagCopied();  // Set the copied flag...
  
  read_head.Advance();
  write_head.Advance();
  
  //Slip mutations
   if (organism->TestCopySlip(ctx)) {
    read_head.Set(ctx.GetRandom().GetInt(organism->GetGenome().GetSize()));
  }
  
  return true;
}

bool cHardwareCPU::HeadCopy_ErrorCorrect(cAvidaContext& ctx, double reduction)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU & read_head = GetHead(nHardware::HEAD_READ);
  cHeadCPU & write_head = GetHead(nHardware::HEAD_WRITE);
//  sCPUStats & cpu_stats = organism->CPUStats();
  
  read_head.Adjust();
  write_head.Adjust();
  
  // Do mutations.
  cInstruction read_inst = read_head.GetInst();
  ReadInst(read_inst.GetOp());
  if ( ctx.GetRandom().P(organism->GetCopyMutProb() / reduction) ) {
    read_inst = m_inst_set->GetRandomInst(ctx);
//    cpu_stats.mut_stats.copy_mut_count++; 
    write_head.SetFlagMutated();
    write_head.SetFlagCopyMut();
    //organism->GetPhenotype().IsMutated() = true;
  }
  
//  cpu_stats.mut_stats.copies_exec++;
  
  write_head.SetInst(read_inst);
  write_head.SetFlagCopied();  // Set the copied flag...
  
  read_head.Advance();
  write_head.Advance();
  return true;
}

bool cHardwareCPU::Inst_HeadCopy2(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 2); }
bool cHardwareCPU::Inst_HeadCopy3(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 3); }
bool cHardwareCPU::Inst_HeadCopy4(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 4); }
bool cHardwareCPU::Inst_HeadCopy5(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 5); }
bool cHardwareCPU::Inst_HeadCopy6(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 6); }
bool cHardwareCPU::Inst_HeadCopy7(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 7); }
bool cHardwareCPU::Inst_HeadCopy8(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 8); }
bool cHardwareCPU::Inst_HeadCopy9(cAvidaContext& ctx)  { return HeadCopy_ErrorCorrect(ctx, 9); }
bool cHardwareCPU::Inst_HeadCopy10(cAvidaContext& ctx) { return HeadCopy_ErrorCorrect(ctx, 10); }

bool cHardwareCPU::Inst_HeadSearch(cAvidaContext& ctx)
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

bool cHardwareCPU::Inst_SetFlow(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_CX);
  GetHead(nHardware::HEAD_FLOW).Set(GetRegister(reg_used));
  return true; 
}

bool cHardwareCPU::Inst_Sleep(cAvidaContext& ctx) {
  cPopulation& pop = m_world->GetPopulation();
  if(m_world->GetConfig().LOG_SLEEP_TIMES.Get() == 1) {
    pop.AddEndSleep(organism->GetCellID(), m_world->GetStats().GetUpdate());
  }
  int cellID = organism->GetCellID();
  organism->SetSleeping(false);  //this instruction get executed at the end of a sleep cycle
  m_world->GetStats().decNumAsleep(pop.GetCell(cellID).GetDemeID());
  if(m_world->GetConfig().APPLY_ENERGY_METHOD.Get() == 2) {
    organism->GetPhenotype().RefreshEnergy();
    organism->GetPhenotype().ApplyToEnergyStore();
    double newMerit = cMerit::EnergyToMerit(organism->GetPhenotype().GetStoredEnergy(), m_world) * organism->GetPhenotype().GetExecutionRatio();
    pop.UpdateMerit(organism->GetCellID(), newMerit);
  }
  return true;
}

bool cHardwareCPU::Inst_GetUpdate(cAvidaContext& ctx) {
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = m_world->GetStats().GetUpdate();
  return true;
}


//// Promoter Model ////

bool cHardwareCPU::Inst_Promoter(cAvidaContext& ctx)
{
  // Promoters don't do anything themselves
  return true;
}

// Move execution to a new promoter
bool cHardwareCPU::Inst_Terminate(cAvidaContext& ctx)
{
  // Reset the CPU, clearing everything except R/W head positions.
  const int write_head_pos = GetHead(nHardware::HEAD_WRITE).GetPosition();
  const int read_head_pos = GetHead(nHardware::HEAD_READ).GetPosition();
  m_threads[m_cur_thread].Reset(this, m_threads[m_cur_thread].GetID());
  GetHead(nHardware::HEAD_WRITE).Set(write_head_pos);
  GetHead(nHardware::HEAD_READ).Set(read_head_pos);

  // We want to execute the promoter that we land on.
  promoter_inst_executed = 0;
  m_advance_ip = false;
  
  //Setting this makes it harder to do things. You have to be modular.
  organism->GetOrgInterface().ResetInputs(ctx);   // Re-randomize the inputs this organism sees
  organism->ClearInput();                         // Also clear their input buffers, or they can still claim
                                                  // rewards for numbers no longer in their environment!
  
  // Find the next active promoter
  int started_search_pos = promoter_search_pos;
  
  while (promoter_pos.GetSize() > 0) // conditional infinite loop, breaks out
  {
    promoter_search_pos++;
    promoter_search_pos %= promoter_active.GetSize();
    if (promoter_active[promoter_search_pos]) break;
    if (started_search_pos == promoter_search_pos) break;
  } 
  
  //Can't find a promoter -- start at the beginning of the genome (or die! or sleep!)
  if ((promoter_pos.GetSize() == 0) || ((promoter_search_pos == started_search_pos) && (!promoter_active[promoter_search_pos])))
  {
    IP().Set(0);
  }
  else
  {
    IP().Set(promoter_pos[promoter_search_pos]);
  }
  
  return true;
}

// To be implemented...
bool cHardwareCPU::Inst_Regulate(cAvidaContext& ctx)
{
  return true;
}


/*! Send a message to the organism that is currently faced by this cell,
where the label field of sent message is from register ?BX?, and the data field
is from register ~?BX?.
*/
bool cHardwareCPU::Inst_SendMessage(cAvidaContext& ctx)
{
  const int label_reg = FindModifiedRegister(REG_BX);
  const int data_reg = FindNextRegister(label_reg);
  
  cOrgMessage msg = cOrgMessage(organism);
  msg.SetLabel(GetRegister(label_reg));
  msg.SetData(GetRegister(data_reg));
  return organism->SendMessage(ctx, msg);
}


/*! This method /attempts/ to retrieve a message -- It may not be possible, as in
the case of an empty receive buffer.

If a message is available, ?BX? is set to the message's label, and ~?BX? is set
to its data.
*/
bool cHardwareCPU::Inst_RetrieveMessage(cAvidaContext& ctx) 
{
  const cOrgMessage* msg = organism->RetrieveMessage();
  if(msg == 0)
    return false;
  
  const int label_reg = FindModifiedRegister(REG_BX);
  const int data_reg = FindNextRegister(label_reg);
  
  GetRegister(label_reg) = msg->GetLabel();
  GetRegister(data_reg) = msg->GetData();
  return true;
}


//// Placebo insts ////
bool cHardwareCPU::Inst_Skip(cAvidaContext& ctx)
{
  IP().Advance();
  return true;
}
