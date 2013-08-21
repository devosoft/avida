/*
 *  hardware/types/Heads.cc
 *  avida-core
 *
 *  Created by David on 8/21/13.
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

#include "avida/private/hardware/types/Heads.h"

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


Hardware::Types::Heads::HeadsInstLib* Hardware::Types::Heads::s_inst_slib = Hardware::Types::Heads::initInstLib();

Hardware::Types::Heads::HeadsInstLib* Hardware::Types::Heads::initInstLib(void)
{
  struct NOPEntry {
    Apto::String name;
    int nop_mod;
    NOPEntry(const Apto::String &name, int nop_mod) : name(name), nop_mod(nop_mod) {}
  };
  static const NOPEntry s_n_array[] = {
    NOPEntry("nop-A", REG_AX),
    NOPEntry("nop-B", REG_BX),
    NOPEntry("nop-C", REG_CX),
  };
  
  static const HeadsInst s_f_array[] = {
    /*
     Note: all entries of cNOPEntryCPU s_n_array must have corresponding in the same order in
     InstLib Entries s_f_array, and these entries must be the first elements of s_f_array.
     */
#define INST(NAME, FUNC, CLS, FLAGS, UNITS, DESC) HeadsInst(NAME, &Hardware::Types::Heads::FUNC, INST_CLASS_ ## CLS, FLAGS, DESC, UNITS)
    INST("nop-A", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    INST("nop-B", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
    INST("nop-C", Inst_Nop, NOP, NOP, 0, "No-operation; modifies other instructions"),
  };
  
}








StaticTableInstLib<tMethod>::MethodEntry("nop-X", &cHardwareCPU::Inst_Nop, INST_CLASS_NOP, 0, "True no-operation instruction: does nothing"),
StaticTableInstLib<tMethod>::MethodEntry("if-equ-0", &cHardwareCPU::Inst_If0, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX?==0, else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-not-0", &cHardwareCPU::Inst_IfNot0, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX?!=0, else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-equ-0-defaultAX", &cHardwareCPU::Inst_If0_defaultAX, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?AX?==0, else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-not-0-defaultAX", &cHardwareCPU::Inst_IfNot0_defaultAX, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?AX?!=0, else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-n-equ", &cHardwareCPU::Inst_IfNEqu, INST_CLASS_CONDITIONAL, DEFAULT, "Execute next instruction if ?BX?!=?CX?, else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-equ", &cHardwareCPU::Inst_IfEqu, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX?==?CX?, else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-grt-0", &cHardwareCPU::Inst_IfGr0, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-grt", &cHardwareCPU::Inst_IfGr, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if->=-0", &cHardwareCPU::Inst_IfGrEqu0, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if->=", &cHardwareCPU::Inst_IfGrEqu, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-les-0", &cHardwareCPU::Inst_IfLess0, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-less", &cHardwareCPU::Inst_IfLess, INST_CLASS_CONDITIONAL, DEFAULT, "Execute next instruction if ?BX? < ?CX?, else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-<=-0", &cHardwareCPU::Inst_IfLsEqu0, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-<=", &cHardwareCPU::Inst_IfLsEqu, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-A!=B", &cHardwareCPU::Inst_IfANotEqB, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-B!=C", &cHardwareCPU::Inst_IfBNotEqC, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-A!=C", &cHardwareCPU::Inst_IfANotEqC, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-bit-1", &cHardwareCPU::Inst_IfBit1, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-grt-X", &cHardwareCPU::Inst_IfGrX, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-equ-X", &cHardwareCPU::Inst_IfEquX, INST_CLASS_CONDITIONAL),

// Probabilistic ifs.
StaticTableInstLib<tMethod>::MethodEntry("if-p-0.125", &cHardwareCPU::Inst_IfP0p125, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-p-0.25", &cHardwareCPU::Inst_IfP0p25, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-p-0.50", &cHardwareCPU::Inst_IfP0p50, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-p-0.75", &cHardwareCPU::Inst_IfP0p75, INST_CLASS_CONDITIONAL),

// The below series of conditionals extend the traditional Avida single-instruction-skip
// to a block, or series of instructions.
StaticTableInstLib<tMethod>::MethodEntry("if-less.end", &cHardwareCPU::Inst_IfLessEnd, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if-n-equ.end", &cHardwareCPU::Inst_IfNotEqualEnd, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("if->=.end", &cHardwareCPU::Inst_IfGrtEquEnd, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("else", &cHardwareCPU::Inst_Else, INST_CLASS_CONDITIONAL),
StaticTableInstLib<tMethod>::MethodEntry("end-if", &cHardwareCPU::Inst_EndIf, INST_CLASS_CONDITIONAL),

StaticTableInstLib<tMethod>::MethodEntry("jump-f", &cHardwareCPU::Inst_JumpF, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("jump-b", &cHardwareCPU::Inst_JumpB, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("call", &cHardwareCPU::Inst_Call, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("return", &cHardwareCPU::Inst_Return, INST_CLASS_FLOW_CONTROL),

StaticTableInstLib<tMethod>::MethodEntry("throw", &cHardwareCPU::Inst_Throw, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("throwif=0", &cHardwareCPU::Inst_ThrowIf0, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("throwif!=0", &cHardwareCPU::Inst_ThrowIfNot0, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("catch", &cHardwareCPU::Inst_Catch, INST_CLASS_FLOW_CONTROL),

StaticTableInstLib<tMethod>::MethodEntry("goto", &cHardwareCPU::Inst_Goto, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("goto-if=0", &cHardwareCPU::Inst_GotoIf0, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("goto-if!=0", &cHardwareCPU::Inst_GotoIfNot0, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("label", &cHardwareCPU::Inst_Label, INST_CLASS_FLOW_CONTROL),

StaticTableInstLib<tMethod>::MethodEntry("pop", &cHardwareCPU::Inst_Pop, INST_CLASS_DATA, DEFAULT, "Remove top number from stack and place into ?BX?"),
StaticTableInstLib<tMethod>::MethodEntry("push", &cHardwareCPU::Inst_Push, INST_CLASS_DATA, DEFAULT, "Copy number from ?BX? and place it into the stack"),
StaticTableInstLib<tMethod>::MethodEntry("swap-stk", &cHardwareCPU::Inst_SwitchStack, INST_CLASS_DATA, DEFAULT, "Toggle which stack is currently being used"),
StaticTableInstLib<tMethod>::MethodEntry("swap", &cHardwareCPU::Inst_Swap, INST_CLASS_DATA, DEFAULT, "Swap the contents of ?BX? with ?CX?"),
StaticTableInstLib<tMethod>::MethodEntry("swap-AB", &cHardwareCPU::Inst_SwapAB, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("swap-BC", &cHardwareCPU::Inst_SwapBC, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("swap-AC", &cHardwareCPU::Inst_SwapAC, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("copy-reg", &cHardwareCPU::Inst_CopyReg, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("set_A=B", &cHardwareCPU::Inst_CopyRegAB, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("set_A=C", &cHardwareCPU::Inst_CopyRegAC, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("set_B=A", &cHardwareCPU::Inst_CopyRegBA, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("set_B=C", &cHardwareCPU::Inst_CopyRegBC, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("set_C=A", &cHardwareCPU::Inst_CopyRegCA, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("set_C=B", &cHardwareCPU::Inst_CopyRegCB, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("reset", &cHardwareCPU::Inst_Reset, INST_CLASS_DATA),

StaticTableInstLib<tMethod>::MethodEntry("pop-A", &cHardwareCPU::Inst_PopA, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("pop-B", &cHardwareCPU::Inst_PopB, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("pop-C", &cHardwareCPU::Inst_PopC, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("push-A", &cHardwareCPU::Inst_PushA, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("push-B", &cHardwareCPU::Inst_PushB, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("push-C", &cHardwareCPU::Inst_PushC, INST_CLASS_DATA),

StaticTableInstLib<tMethod>::MethodEntry("shift-r", &cHardwareCPU::Inst_ShiftR, INST_CLASS_ARITHMETIC_LOGIC, DEFAULT, "Shift bits in ?BX? right by one (divide by two)"),
StaticTableInstLib<tMethod>::MethodEntry("shift-l", &cHardwareCPU::Inst_ShiftL, INST_CLASS_ARITHMETIC_LOGIC, DEFAULT, "Shift bits in ?BX? left by one (multiply by two)"),
StaticTableInstLib<tMethod>::MethodEntry("bit-1", &cHardwareCPU::Inst_Bit1, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("set-num", &cHardwareCPU::Inst_SetNum, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("val-grey", &cHardwareCPU::Inst_ValGrey, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("val-dir", &cHardwareCPU::Inst_ValDir, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("val-add-p", &cHardwareCPU::Inst_ValAddP, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("val-fib", &cHardwareCPU::Inst_ValFib, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("val-poly-c", &cHardwareCPU::Inst_ValPolyC, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("inc", &cHardwareCPU::Inst_Inc, INST_CLASS_ARITHMETIC_LOGIC, DEFAULT, "Increment ?BX? by one"),
StaticTableInstLib<tMethod>::MethodEntry("dec", &cHardwareCPU::Inst_Dec, INST_CLASS_ARITHMETIC_LOGIC, DEFAULT, "Decrement ?BX? by one"),
StaticTableInstLib<tMethod>::MethodEntry("zero", &cHardwareCPU::Inst_Zero, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to zero"),
StaticTableInstLib<tMethod>::MethodEntry("one", &cHardwareCPU::Inst_One, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to one"),
StaticTableInstLib<tMethod>::MethodEntry("all1s", &cHardwareCPU::Inst_All1s, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to all 1s in bitstring"),
StaticTableInstLib<tMethod>::MethodEntry("neg", &cHardwareCPU::Inst_Neg, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("square", &cHardwareCPU::Inst_Square, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("sqrt", &cHardwareCPU::Inst_Sqrt, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("not", &cHardwareCPU::Inst_Not, INST_CLASS_ARITHMETIC_LOGIC),

StaticTableInstLib<tMethod>::MethodEntry("add", &cHardwareCPU::Inst_Add, INST_CLASS_ARITHMETIC_LOGIC, DEFAULT, "Add BX to CX and place the result in ?BX?"),
StaticTableInstLib<tMethod>::MethodEntry("sub", &cHardwareCPU::Inst_Sub, INST_CLASS_ARITHMETIC_LOGIC, DEFAULT, "Subtract CX from BX and place the result in ?BX?"),
StaticTableInstLib<tMethod>::MethodEntry("mult", &cHardwareCPU::Inst_Mult, INST_CLASS_ARITHMETIC_LOGIC, 0, "Multiple BX by CX and place the result in ?BX?"),
StaticTableInstLib<tMethod>::MethodEntry("div", &cHardwareCPU::Inst_Div, INST_CLASS_ARITHMETIC_LOGIC, 0, "Divide BX by CX and place the result in ?BX?"),
StaticTableInstLib<tMethod>::MethodEntry("mod", &cHardwareCPU::Inst_Mod, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("nand", &cHardwareCPU::Inst_Nand, INST_CLASS_ARITHMETIC_LOGIC, DEFAULT, "Nand BX by CX and place the result in ?BX?"),
StaticTableInstLib<tMethod>::MethodEntry("or", &cHardwareCPU::Inst_Or, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("nor", &cHardwareCPU::Inst_Nor, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("and", &cHardwareCPU::Inst_And, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("order", &cHardwareCPU::Inst_Order, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("xor", &cHardwareCPU::Inst_Xor, INST_CLASS_ARITHMETIC_LOGIC),

// Instructions that modify specific bits in the register values
StaticTableInstLib<tMethod>::MethodEntry("setbit", &cHardwareCPU::Inst_Setbit, INST_CLASS_ARITHMETIC_LOGIC, DEFAULT, "Set the bit in ?BX? specified by ?BX?'s complement"),
StaticTableInstLib<tMethod>::MethodEntry("clearbit", &cHardwareCPU::Inst_Clearbit, INST_CLASS_ARITHMETIC_LOGIC, DEFAULT, "Clear the bit in ?BX? specified by ?BX?'s complement"),

StaticTableInstLib<tMethod>::MethodEntry("copy", &cHardwareCPU::Inst_Copy, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("read", &cHardwareCPU::Inst_ReadInst, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("write", &cHardwareCPU::Inst_WriteInst, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("stk-read", &cHardwareCPU::Inst_StackReadInst, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("stk-writ", &cHardwareCPU::Inst_StackWriteInst, INST_CLASS_LIFECYCLE),

StaticTableInstLib<tMethod>::MethodEntry("compare", &cHardwareCPU::Inst_Compare),
StaticTableInstLib<tMethod>::MethodEntry("if-n-cpy", &cHardwareCPU::Inst_IfNCpy),
StaticTableInstLib<tMethod>::MethodEntry("allocate", &cHardwareCPU::Inst_Allocate, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("divide", &cHardwareCPU::Inst_Divide, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("divideRS", &cHardwareCPU::Inst_DivideRS, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("c-alloc", &cHardwareCPU::Inst_CAlloc, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("c-divide", &cHardwareCPU::Inst_CDivide, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("search-f", &cHardwareCPU::Inst_SearchF, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("search-b", &cHardwareCPU::Inst_SearchB, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("mem-size", &cHardwareCPU::Inst_MemSize),

StaticTableInstLib<tMethod>::MethodEntry("get", &cHardwareCPU::Inst_TaskGet, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("get-2", &cHardwareCPU::Inst_TaskGet2, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("stk-get", &cHardwareCPU::Inst_TaskStackGet, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("stk-load", &cHardwareCPU::Inst_TaskStackLoad, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("put", &cHardwareCPU::Inst_TaskPut, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("put-reset", &cHardwareCPU::Inst_TaskPutResetInputs, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("IO", &cHardwareCPU::Inst_TaskIO, INST_CLASS_ENVIRONMENT, DEFAULT | STALL, "Output ?BX?, and input new number back into ?BX?"),
StaticTableInstLib<tMethod>::MethodEntry("IO-Feedback", &cHardwareCPU::Inst_TaskIO_Feedback, INST_CLASS_ENVIRONMENT, STALL, "Output ?BX?, and input new number back into ?BX?,  and push 1,0,  or -1 onto stack1 if merit increased, stayed the same, or decreased"),
StaticTableInstLib<tMethod>::MethodEntry("IO-bc-0.001", &cHardwareCPU::Inst_TaskIO_BonusCost_0_001, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("match-strings", &cHardwareCPU::Inst_MatchStrings, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("send", &cHardwareCPU::Inst_Send, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("receive", &cHardwareCPU::Inst_Receive, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("sense", &cHardwareCPU::Inst_SenseLog2, INST_CLASS_ENVIRONMENT, STALL),           // If you add more sense instructions
StaticTableInstLib<tMethod>::MethodEntry("sense-unit", &cHardwareCPU::Inst_SenseUnit, INST_CLASS_ENVIRONMENT, STALL),      // and want to keep stats, also add
StaticTableInstLib<tMethod>::MethodEntry("sense-m100", &cHardwareCPU::Inst_SenseMult100, INST_CLASS_ENVIRONMENT, STALL),   // the names to cStats::cStats() @JEB
StaticTableInstLib<tMethod>::MethodEntry("sense-resource-id", &cHardwareCPU::Inst_SenseResourceID, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("sense-faced-habitat", &cHardwareCPU::Inst_SenseFacedHabitat, INST_CLASS_ENVIRONMENT, STALL),

StaticTableInstLib<tMethod>::MethodEntry("if-resources", &cHardwareCPU::Inst_IfResources, INST_CLASS_CONDITIONAL, STALL),
StaticTableInstLib<tMethod>::MethodEntry("collect", &cHardwareCPU::Inst_Collect, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("collect-no-env-remove", &cHardwareCPU::Inst_CollectNoEnvRemove, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("destroy", &cHardwareCPU::Inst_Destroy, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("nop-collect", &cHardwareCPU::Inst_NopCollect, INST_CLASS_ENVIRONMENT),
StaticTableInstLib<tMethod>::MethodEntry("collect-unit-prob", &cHardwareCPU::Inst_CollectUnitProbabilistic, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("collect-specific", &cHardwareCPU::Inst_CollectSpecific, INST_CLASS_ENVIRONMENT, STALL),

StaticTableInstLib<tMethod>::MethodEntry("rotate-l", &cHardwareCPU::Inst_RotateL, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-r", &cHardwareCPU::Inst_RotateR, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-left-one", &cHardwareCPU::Inst_RotateLeftOne, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-right-one", &cHardwareCPU::Inst_RotateRightOne, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-label", &cHardwareCPU::Inst_RotateLabel, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-to-unoccupied-cell", &cHardwareCPU::Inst_RotateUnoccupiedCell, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-to-next-unoccupied-cell", &cHardwareCPU::Inst_RotateNextUnoccupiedCell, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-to-occupied-cell", &cHardwareCPU::Inst_RotateOccupiedCell, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-to-next-occupied-cell", &cHardwareCPU::Inst_RotateNextOccupiedCell, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-to-event-cell", &cHardwareCPU::Inst_RotateEventCell, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-uphill", &cHardwareCPU::Inst_RotateUphill, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("rotate-home", &cHardwareCPU::Inst_RotateHome, INST_CLASS_ENVIRONMENT, STALL),

StaticTableInstLib<tMethod>::MethodEntry("set-cmut", &cHardwareCPU::Inst_SetCopyMut),
StaticTableInstLib<tMethod>::MethodEntry("mod-cmut", &cHardwareCPU::Inst_ModCopyMut),
StaticTableInstLib<tMethod>::MethodEntry("get-cell-xy", &cHardwareCPU::Inst_GetCellPosition),
StaticTableInstLib<tMethod>::MethodEntry("get-cell-x", &cHardwareCPU::Inst_GetCellPositionX),
StaticTableInstLib<tMethod>::MethodEntry("get-cell-y", &cHardwareCPU::Inst_GetCellPositionY),
StaticTableInstLib<tMethod>::MethodEntry("dist-from-diag", &cHardwareCPU::Inst_GetDistanceFromDiagonal),
StaticTableInstLib<tMethod>::MethodEntry("get-north-offset", &cHardwareCPU::Inst_GetDirectionOffNorth),
StaticTableInstLib<tMethod>::MethodEntry("get-northerly", &cHardwareCPU::Inst_GetNortherly),
StaticTableInstLib<tMethod>::MethodEntry("get-easterly", &cHardwareCPU::Inst_GetEasterly),
StaticTableInstLib<tMethod>::MethodEntry("zero-easterly", &cHardwareCPU::Inst_ZeroEasterly),
StaticTableInstLib<tMethod>::MethodEntry("zero-northerly", &cHardwareCPU::Inst_ZeroNortherly),


// Movement instructions
StaticTableInstLib<tMethod>::MethodEntry("tumble", &cHardwareCPU::Inst_Tumble, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("move", &cHardwareCPU::Inst_Move, INST_CLASS_ENVIRONMENT, STALL),

// Threading instructions
StaticTableInstLib<tMethod>::MethodEntry("fork-th", &cHardwareCPU::Inst_ForkThread),
StaticTableInstLib<tMethod>::MethodEntry("forkl", &cHardwareCPU::Inst_ForkThreadLabel),
StaticTableInstLib<tMethod>::MethodEntry("forkl!=0", &cHardwareCPU::Inst_ForkThreadLabelIfNot0),
StaticTableInstLib<tMethod>::MethodEntry("forkl=0", &cHardwareCPU::Inst_ForkThreadLabelIf0),
StaticTableInstLib<tMethod>::MethodEntry("kill-th", &cHardwareCPU::Inst_KillThread),
StaticTableInstLib<tMethod>::MethodEntry("id-th", &cHardwareCPU::Inst_ThreadID),

// Head-based instructions
StaticTableInstLib<tMethod>::MethodEntry("h-alloc", &cHardwareCPU::Inst_MaxAlloc, INST_CLASS_LIFECYCLE, DEFAULT, "Allocate maximum allowed space"),
StaticTableInstLib<tMethod>::MethodEntry("h-alloc-mw", &cHardwareCPU::Inst_MaxAllocMoveWriteHead),
StaticTableInstLib<tMethod>::MethodEntry("h-divide", &cHardwareCPU::Inst_HeadDivide, INST_CLASS_LIFECYCLE, DEFAULT | STALL, "Divide code between read and write heads."),
StaticTableInstLib<tMethod>::MethodEntry("h-divide1RS", &cHardwareCPU::Inst_HeadDivide1RS, INST_CLASS_LIFECYCLE, STALL, "Divide code between read and write heads, at most one mutation on divide, resample if reverted."),
StaticTableInstLib<tMethod>::MethodEntry("h-divide2RS", &cHardwareCPU::Inst_HeadDivide2RS, INST_CLASS_LIFECYCLE, STALL, "Divide code between read and write heads, at most two mutations on divide, resample if reverted."),
StaticTableInstLib<tMethod>::MethodEntry("h-divideRS", &cHardwareCPU::Inst_HeadDivideRS, INST_CLASS_LIFECYCLE, STALL, "Divide code between read and write heads, resample if reverted."),
StaticTableInstLib<tMethod>::MethodEntry("h-read", &cHardwareCPU::Inst_HeadRead, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("h-write", &cHardwareCPU::Inst_HeadWrite, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("h-copy", &cHardwareCPU::Inst_HeadCopy, INST_CLASS_LIFECYCLE, DEFAULT, "Copy from read-head to write-head; advance both"),
StaticTableInstLib<tMethod>::MethodEntry("h-search", &cHardwareCPU::Inst_HeadSearch, INST_CLASS_FLOW_CONTROL, DEFAULT, "Find complement template and make with flow head"),
StaticTableInstLib<tMethod>::MethodEntry("h-search-direct", &cHardwareCPU::Inst_HeadSearchDirect, INST_CLASS_FLOW_CONTROL, 0, "Find direct template and move the flow head"),
StaticTableInstLib<tMethod>::MethodEntry("h-push", &cHardwareCPU::Inst_HeadPush, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("h-pop", &cHardwareCPU::Inst_HeadPop, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("set-head", &cHardwareCPU::Inst_SetHead, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("adv-head", &cHardwareCPU::Inst_AdvanceHead, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("mov-head", &cHardwareCPU::Inst_MoveHead, INST_CLASS_FLOW_CONTROL, DEFAULT, "Move head ?IP? to the flow head"),
StaticTableInstLib<tMethod>::MethodEntry("jmp-head", &cHardwareCPU::Inst_JumpHead, INST_CLASS_FLOW_CONTROL, DEFAULT, "Move head ?IP? by amount in CX register; CX = old pos."),
StaticTableInstLib<tMethod>::MethodEntry("get-head", &cHardwareCPU::Inst_GetHead, INST_CLASS_FLOW_CONTROL, DEFAULT, "Copy the position of the ?IP? head into CX"),
StaticTableInstLib<tMethod>::MethodEntry("if-label", &cHardwareCPU::Inst_IfLabel, INST_CLASS_CONDITIONAL, DEFAULT, "Execute next if we copied complement of attached label"),
StaticTableInstLib<tMethod>::MethodEntry("if-label-direct", &cHardwareCPU::Inst_IfLabelDirect, INST_CLASS_CONDITIONAL, DEFAULT, "Execute next if we copied direct match of the attached label"),
StaticTableInstLib<tMethod>::MethodEntry("set-flow", &cHardwareCPU::Inst_SetFlow, INST_CLASS_FLOW_CONTROL, DEFAULT, "Set flow-head to position in ?CX?"),

StaticTableInstLib<tMethod>::MethodEntry("divide-sex", &cHardwareCPU::Inst_HeadDivideSex, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("divide-asex", &cHardwareCPU::Inst_HeadDivideAsex, INST_CLASS_LIFECYCLE, STALL),

StaticTableInstLib<tMethod>::MethodEntry("div-sex", &cHardwareCPU::Inst_HeadDivideSex, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("div-asex", &cHardwareCPU::Inst_HeadDivideAsex, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("div-asex-w", &cHardwareCPU::Inst_HeadDivideAsexWait, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("div-sex-MS", &cHardwareCPU::Inst_HeadDivideMateSelect, INST_CLASS_LIFECYCLE, STALL),

StaticTableInstLib<tMethod>::MethodEntry("h-divide1", &cHardwareCPU::Inst_HeadDivide1, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide2", &cHardwareCPU::Inst_HeadDivide2, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide3", &cHardwareCPU::Inst_HeadDivide3, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide4", &cHardwareCPU::Inst_HeadDivide4, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide5", &cHardwareCPU::Inst_HeadDivide5, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide6", &cHardwareCPU::Inst_HeadDivide6, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide7", &cHardwareCPU::Inst_HeadDivide7, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide8", &cHardwareCPU::Inst_HeadDivide8, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide9", &cHardwareCPU::Inst_HeadDivide9, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide10", &cHardwareCPU::Inst_HeadDivide10, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide16", &cHardwareCPU::Inst_HeadDivide16, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide32", &cHardwareCPU::Inst_HeadDivide32, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide50", &cHardwareCPU::Inst_HeadDivide50, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide100", &cHardwareCPU::Inst_HeadDivide100, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide500", &cHardwareCPU::Inst_HeadDivide500, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide1000", &cHardwareCPU::Inst_HeadDivide1000, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide5000", &cHardwareCPU::Inst_HeadDivide5000, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide10000", &cHardwareCPU::Inst_HeadDivide10000, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide50000", &cHardwareCPU::Inst_HeadDivide50000, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide0.5", &cHardwareCPU::Inst_HeadDivide0_5, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide0.1", &cHardwareCPU::Inst_HeadDivide0_1, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide0.05", &cHardwareCPU::Inst_HeadDivide0_05, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide0.01", &cHardwareCPU::Inst_HeadDivide0_01, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("h-divide0.001", &cHardwareCPU::Inst_HeadDivide0_001, INST_CLASS_LIFECYCLE, STALL),

//@CHC Mating type / mate choice instructions
StaticTableInstLib<tMethod>::MethodEntry("set-mating-type-male", &cHardwareCPU::Inst_SetMatingTypeMale, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("set-mating-type-female", &cHardwareCPU::Inst_SetMatingTypeFemale, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("set-mating-type-juvenile", &cHardwareCPU::Inst_SetMatingTypeJuvenile, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("div-sex-mating-type", &cHardwareCPU::Inst_DivideSexMatingType, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("if-mating-type-male", &cHardwareCPU::Inst_IfMatingTypeMale, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("if-mating-type-female", &cHardwareCPU::Inst_IfMatingTypeFemale, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("if-mating-type-juvenile", &cHardwareCPU::Inst_IfMatingTypeJuvenile, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("increment-mating-display-a", &cHardwareCPU::Inst_IncrementMatingDisplayA, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("increment-mating-display-b", &cHardwareCPU::Inst_IncrementMatingDisplayB, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("set-mating-display-a", &cHardwareCPU::Inst_SetMatingDisplayA, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("set-mating-display-b", &cHardwareCPU::Inst_SetMatingDisplayB, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("set-mate-preference-random", &cHardwareCPU::Inst_SetMatePreferenceRandom, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("set-mate-preference-highest-display-a", &cHardwareCPU::Inst_SetMatePreferenceHighestDisplayA, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("set-mate-preference-highest-display-b", &cHardwareCPU::Inst_SetMatePreferenceHighestDisplayB, INST_CLASS_LIFECYCLE),
StaticTableInstLib<tMethod>::MethodEntry("set-mate-preference-highest-merit", &cHardwareCPU::Inst_SetMatePreferenceHighestMerit, INST_CLASS_LIFECYCLE),


// High-level instructions
StaticTableInstLib<tMethod>::MethodEntry("repro", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-sex", &cHardwareCPU::Inst_ReproSex, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-A", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-B", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-C", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-D", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-E", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-F", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-G", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-H", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-I", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-J", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-K", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-L", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-M", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-N", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-O", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-P", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-Q", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-R", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-S", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-T", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-U", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-V", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-W", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-X", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-Y", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("repro-Z", &cHardwareCPU::Inst_Repro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("cond-repro", &cHardwareCPU::Inst_ConditionalRepro, INST_CLASS_LIFECYCLE, STALL),

StaticTableInstLib<tMethod>::MethodEntry("put-repro", &cHardwareCPU::Inst_TaskPutRepro, INST_CLASS_LIFECYCLE, STALL),
StaticTableInstLib<tMethod>::MethodEntry("metabolize", &cHardwareCPU::Inst_TaskPutResetInputsRepro, INST_CLASS_LIFECYCLE, STALL),

StaticTableInstLib<tMethod>::MethodEntry("sterilize", &cHardwareCPU::Inst_Sterilize, INST_CLASS_LIFECYCLE),

// Suicide
StaticTableInstLib<tMethod>::MethodEntry("die", &cHardwareCPU::Inst_Die, INST_CLASS_OTHER, STALL),
StaticTableInstLib<tMethod>::MethodEntry("poison", &cHardwareCPU::Inst_Poison),
StaticTableInstLib<tMethod>::MethodEntry("suicide", &cHardwareCPU::Inst_Suicide, INST_CLASS_OTHER, STALL),

// Promoter Model
StaticTableInstLib<tMethod>::MethodEntry("promoter", &cHardwareCPU::Inst_Promoter, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("terminate", &cHardwareCPU::Inst_Terminate, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("regulate", &cHardwareCPU::Inst_Regulate, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("regulate-sp", &cHardwareCPU::Inst_RegulateSpecificPromoters, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("s-regulate", &cHardwareCPU::Inst_SenseRegulate, INST_CLASS_FLOW_CONTROL),
StaticTableInstLib<tMethod>::MethodEntry("numberate", &cHardwareCPU::Inst_Numberate, INST_CLASS_DATA),
StaticTableInstLib<tMethod>::MethodEntry("numberate-24", &cHardwareCPU::Inst_Numberate24, INST_CLASS_DATA),

// Bit Consensus
StaticTableInstLib<tMethod>::MethodEntry("bit-cons", &cHardwareCPU::Inst_BitConsensus, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("bit-cons-24", &cHardwareCPU::Inst_BitConsensus24, INST_CLASS_ARITHMETIC_LOGIC),
StaticTableInstLib<tMethod>::MethodEntry("if-cons", &cHardwareCPU::Inst_IfConsensus, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? in consensus, else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-cons-24", &cHardwareCPU::Inst_IfConsensus24, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX[0:23]? in consensus , else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-less-cons", &cHardwareCPU::Inst_IfLessConsensus, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if Count(?BX?) < Count(?CX?), else skip it"),
StaticTableInstLib<tMethod>::MethodEntry("if-less-cons-24", &cHardwareCPU::Inst_IfLessConsensus24, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if Count(?BX[0:23]?) < Count(?CX[0:23]?), else skip it"),

// Bit Masking (higher order bit masking is possible, just add the instructions if needed)
StaticTableInstLib<tMethod>::MethodEntry("mask-signbit", &cHardwareCPU::Inst_MaskSignBit),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower16bits", &cHardwareCPU::Inst_MaskOffLower16Bits),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower16bits-defaultAX", &cHardwareCPU::Inst_MaskOffLower16Bits_defaultAX),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower15bits", &cHardwareCPU::Inst_MaskOffLower15Bits),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower15bits-defaultAX", &cHardwareCPU::Inst_MaskOffLower15Bits_defaultAX),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower14bits", &cHardwareCPU::Inst_MaskOffLower14Bits),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower14bits-defaultAX", &cHardwareCPU::Inst_MaskOffLower14Bits_defaultAX),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower13bits", &cHardwareCPU::Inst_MaskOffLower13Bits),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower13bits-defaultAX", &cHardwareCPU::Inst_MaskOffLower13Bits_defaultAX),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower12bits", &cHardwareCPU::Inst_MaskOffLower12Bits),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower12bits-defaultAX", &cHardwareCPU::Inst_MaskOffLower12Bits_defaultAX),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower8bits",  &cHardwareCPU::Inst_MaskOffLower8Bits),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower8bits-defaultAX",  &cHardwareCPU::Inst_MaskOffLower8Bits_defaultAX),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower4bits",  &cHardwareCPU::Inst_MaskOffLower4Bits),
StaticTableInstLib<tMethod>::MethodEntry("maskoff-lower4bits-defaultAX",  &cHardwareCPU::Inst_MaskOffLower4Bits_defaultAX),


// Placebo instructions
StaticTableInstLib<tMethod>::MethodEntry("skip", &cHardwareCPU::Inst_Skip),


// Data collection
StaticTableInstLib<tMethod>::MethodEntry("get-id", &cHardwareCPU::Inst_GetID, INST_CLASS_ENVIRONMENT),
StaticTableInstLib<tMethod>::MethodEntry("get-faced-vitality-diff", &cHardwareCPU::Inst_GetFacedVitalityDiff, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("get-faced-org-id", &cHardwareCPU::Inst_GetFacedOrgID, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("attack-faced-org", &cHardwareCPU::Inst_AttackFacedOrg, INST_CLASS_ENVIRONMENT, STALL),
StaticTableInstLib<tMethod>::MethodEntry("get-attack-odds", &cHardwareCPU::Inst_GetAttackOdds, INST_CLASS_ENVIRONMENT, STALL),



// Must always be the last instruction in the array
StaticTableInstLib<tMethod>::MethodEntry("NULL", &cHardwareCPU::Inst_Nop, INST_CLASS_NOP, 0, "True no-operation instruction: does nothing"),
};

const int n_size = sizeof(s_n_array)/sizeof(NOPEntry);

static int nop_mods[n_size];
for (int i = 0; i < n_size && i < NUM_REGISTERS; i++) {
  nop_mods[i] = s_n_array[i].nop_mod;
}

const int f_size = sizeof(s_f_array)/sizeof(StaticTableInstLib<tMethod>::MethodEntry);
static tMethod functions[f_size];
for (int i = 0; i < f_size; i++) functions[i] = s_f_array[i].Function();

const int def = 0;
const int null_inst = f_size - 1;

return new StaticTableInstLib<tMethod>(f_size, s_f_array, nop_mods, functions, def, null_inst);
}

cHardwareCPU::cHardwareCPU(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set)
: cHardwareBase(world, in_organism, in_inst_set)
{
  m_functions = s_inst_slib->Functions();
  
  m_spec_die = false;
  
  m_thread_slicing_parallel = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1);
  m_no_cpu_cycle_time = m_world->GetConfig().NO_CPU_CYCLE_TIME.Get();
  
  m_promoters_enabled = m_world->GetConfig().PROMOTERS_ENABLED.Get();
  m_constitutive_regulation = m_world->GetConfig().CONSTITUTIVE_REGULATION.Get();
  
  m_slip_read_head = !m_world->GetConfig().SLIP_COPY_MODE.Get();
  
  // Initialize memory...
  const Genome& in_genome = in_organism->GetGenome();
  ConstInstructionSequencePtr in_seq_p;
  in_seq_p.DynamicCastFrom(in_genome.Representation());
  m_memory = *in_seq_p;
  
  Reset(ctx);                            // Setup the rest of the hardware...
  internalReset();
}

void cHardwareCPU::internalReset()
{
  m_global_stack.Clear();
  
  // We want to reset to have a single thread.
  m_threads.Resize(1);
  
  // Reset that single thread.
  m_threads[0].Reset(this, 0);
  m_thread_id_chart = 1; // Mark only the first thread as taken...
  m_cur_thread = 0;
  
  m_mal_active = false;
  
  
}

void cHardwareCPU::internalResetOnFailedDivide()
{
  internalReset();
  m_mal_active = true;
  m_advance_ip = false;
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
  cur_head = HEAD_IP;
  read_label.Clear();
  next_label.Clear();
  
  // Promoter model
  m_promoter_inst_executed = 0;
}

void cHardwareCPU::SetupMiniTraceFileHeader(Avida::Output::File& df, const int gen_id, const Apto::String& genotype) { (void)df, (void)gen_id, (void)genotype; }


// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.

bool cHardwareCPU::SingleProcess(cAvidaContext& ctx, bool speculative)
{
  assert(!speculative || (speculative && !m_thread_slicing_parallel));
  
  int last_IP_pos = getIP().GetPosition();
  
  // Mark this organism as running...
  m_organism->SetRunning(true);
  
  if (!speculative && m_spec_die) {
    m_organism->Die(ctx);
    m_organism->SetRunning(false);
    return false;
  }
  
  cPhenotype& phenotype = m_organism->GetPhenotype();
  
  // First instruction - check whether we should be starting at a promoter, when enabled.
  if (phenotype.GetCPUCyclesUsed() == 0 && m_promoters_enabled) Inst_Terminate(ctx);
  
  // Count the cpu cycles used
  phenotype.IncCPUCyclesUsed();
  if (!m_world->GetConfig().NO_CPU_CYCLE_TIME.Get()) phenotype.IncTimeUsed();
  
  int num_threads = m_threads.GetSize();
  
  // If we have threads turned on and we executed each thread in a single
  // timestep, adjust the number of instructions executed accordingly.
  int num_inst_exec = m_thread_slicing_parallel ? num_threads : 1;
  
  for (int i = 0; i < num_inst_exec; i++) {
    // Setup the hardware for the next instruction to be executed.
    int last_thread = m_cur_thread;
    
    m_cur_thread++;
    
    if (m_cur_thread >= num_threads) m_cur_thread = 0;
    
    m_advance_ip = true;
    cHeadCPU& ip = m_threads[m_cur_thread].heads[HEAD_IP];
    ip.Adjust();
    
    
    // Print the status of this CPU at each step...
    if (m_tracer) m_tracer->TraceHardware(ctx, *this);
    
    // Find the instruction to be executed
    const Instruction cur_inst = ip.GetInst();
    
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
    if (m_has_any_costs) exec = SingleProcess_PayPreCosts(ctx, cur_inst, m_cur_thread);
    
    // Constitutive regulation applied here
    if (m_constitutive_regulation) Inst_SenseRegulate(ctx);
    
    // If there are no active promoters and a certain mode is set, then don't execute any further instructions
    if (m_promoters_enabled && m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get() == 2 && m_promoter_index == -1) exec = false;
    
    // Now execute the instruction...
    if (exec == true) {
      // NOTE: This call based on the cur_inst must occur prior to instruction
      //       execution, because this instruction reference may be invalid after
      //       certain classes of instructions (namely divide instructions) @DMB
      const int time_cost = m_inst_set->GetAddlTimeCost(cur_inst);
      
      // Prob of exec (moved from SingleProcess_PayCosts so that we advance IP after a fail)
      if (m_inst_set->GetProbFail(cur_inst) > 0.0) {
        exec = !( ctx.GetRandom().P(m_inst_set->GetProbFail(cur_inst)) );
      }
      
      // Flag instruction as executed even if it failed (moved from SingleProcess_ExecuteInst)
      // this allows division conditions to be met even if most instruction executions failed. @JEB
      
      // Mark the instruction as executed
      getIP().SetFlagExecuted();
      
      // Add to the promoter inst executed count before executing the inst (in case it is a terminator)
      if (m_promoters_enabled) m_threads[m_cur_thread].IncPromoterInstExecuted();
      
      if (exec == true) {
        if (SingleProcess_ExecuteInst(ctx, cur_inst)) {
          SingleProcess_PayPostResCosts(ctx, cur_inst);
          SingleProcess_SetPostCPUCosts(ctx, cur_inst, m_cur_thread);
        }
      }
      
      // Check if the instruction just executed caused premature death, break out of execution if so
      if (phenotype.GetToDelete()) break;
      
      // Some instruction (such as jump) may turn m_advance_ip off.  Usually
      // we now want to move to the next instruction in the memory.
      if (m_advance_ip == true) ip.Advance();
      
      // Pay the time cost of the instruction now
      phenotype.IncTimeUsed(time_cost);
      
      // In the promoter model, we may force termination after a certain number of inst have been executed
      if (m_promoters_enabled) {
        const double processivity = m_world->GetConfig().PROMOTER_PROCESSIVITY.Get();
        if (ctx.GetRandom().P(1 - processivity)) Inst_Terminate(ctx);
        if (m_world->GetConfig().PROMOTER_INST_MAX.Get() && (m_threads[m_cur_thread].GetPromoterInstExecuted() >= m_world->GetConfig().PROMOTER_INST_MAX.Get()))
          Inst_Terminate(ctx);
      }
      
      // check for difference in thread count caused by KillThread or ForkThread
      if (num_threads == m_threads.GetSize()+1){
        --num_threads;
        --num_inst_exec;
      } else if (num_threads > m_threads.GetSize() && m_threads.GetSize() == 1) {
        // divide probably occured, I think divide insts. are the only ones that can reduce the thread count by more than one.
        num_threads = 1;
        num_inst_exec=0;
      } else if (num_threads > m_threads.GetSize()) {
        cerr<<cur_inst.GetOp()<<" "<<cur_inst.GetSymbol()<<" "<< num_threads << " " << m_threads.GetSize() <<endl;
        cerr<<"Error in thread handling\n";
        exit(-1);
      }
    } // if exec
    
  } // Previous was executed once for each thread...
  
  // Kill creatures who have reached their max num of instructions executed
  const int max_executed = m_organism->GetMaxExecuted();
  if ((max_executed > 0 && phenotype.GetTimeUsed() >= max_executed) || phenotype.GetToDie() == true) {
    if (speculative) m_spec_die = true;
    else m_organism->Die(ctx);
  }
  if (!speculative && phenotype.GetToDelete()) m_spec_die = true;
  
  // Note: if organism just died, this will NOT let it repro.
  CheckImplicitRepro(ctx, last_IP_pos > m_threads[m_cur_thread].heads[HEAD_IP].GetPosition());
  
  m_organism->SetRunning(false);
  
  return !m_spec_die;
}

// This method will handle the actual execution of an instruction
// within a single process, once that function has been finalized.
bool cHardwareCPU::SingleProcess_ExecuteInst(cAvidaContext& ctx, const Instruction& cur_inst)
{
  // Copy Instruction locally to handle stochastic effects
  Instruction actual_inst = cur_inst;
  
  // Get a pointer to the corresponding method...
  int inst_idx = m_inst_set->GetLibFunctionIndex(actual_inst);
  
  // instruction execution count incremented
  m_organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
	
  // And execute it.
  const bool exec_success = (this->*(m_functions[inst_idx]))(ctx);
  
  // NOTE: Organism may be dead now if instruction executed killed it (such as some divides, "die", or "kazi")
	
  // Decrement if the instruction was not executed successfully.
  if (exec_success == false) {
    m_organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  }
  
  return exec_success;
}


void cHardwareCPU::ProcessBonusInst(cAvidaContext& ctx, const Instruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = m_organism->IsRunning();
  m_organism->SetRunning(true);
  
  if (m_tracer) m_tracer->TraceHardware(ctx, *this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  m_organism->SetRunning(prev_run_state);
}


void cHardwareCPU::PrintStatus(ostream& fp)
{
  fp << m_organism->GetPhenotype().GetCPUCyclesUsed() << " ";
  fp << "IP:" << getIP().GetPosition() << " (" << GetInstSet().GetName(IP().GetInst()) << ")" << endl;
  
  for (int i = 0; i < NUM_REGISTERS; i++) {
    fp << static_cast<char>('A' + i) << "X:" << GetRegister(i) << " ";
    fp << setbase(16) << "[0x" << GetRegister(i) << "]  " << setbase(10);
  }
  
  // Add some extra information if additional time costs are used for instructions,
  // leave this out if there are no differences to keep it cleaner
  if (m_organism->GetPhenotype().GetTimeUsed() != m_organism->GetPhenotype().GetCPUCyclesUsed()) {
    fp << "  EnergyUsed:" << m_organism->GetPhenotype().GetTimeUsed(); // this is not energy that is used by the energy model
  }
  fp << endl;
  
  fp << "  R-Head:" << getHead(HEAD_READ).GetPosition() << " "
  << "W-Head:" << getHead(HEAD_WRITE).GetPosition()  << " "
  << "F-Head:" << getHead(HEAD_FLOW).GetPosition()   << "  "
  << "RL:" << GetReadLabel().AsString() << "   "
  << endl;
  
  int number_of_stacks = GetNumStacks();
  for (int stack_id = 0; stack_id < number_of_stacks; stack_id++) {
    fp << ((m_threads[m_cur_thread].cur_stack == stack_id) ? '*' : ' ') << " Stack " << stack_id << ":" << setbase(16) << setfill('0');
    for (int i = 0; i < STACK_SIZE; i++) fp << " Ox" << setw(8) << GetStack(i, stack_id, 0);
    fp << setfill(' ') << setbase(10) << endl;
  }
  
  fp << "  Mem (" << m_memory.GetSize() << "):"
  << "  " << m_memory.AsString()
  << endl;
  
  if (m_ext_mem.GetSize()) {
    fp << "  Ext Mem: " << m_ext_mem[0];
    for (int i = 1; i < m_ext_mem.GetSize(); i++) fp << ", " << m_ext_mem[i];
    fp << endl;
  }
  
  if (m_world->GetConfig().PROMOTERS_ENABLED.Get())
  {
    fp << "  Promoters: index=" << m_promoter_index << " offset=" << m_promoter_offset;
    fp << " exe_inst=" << m_threads[m_cur_thread].GetPromoterInstExecuted();
    for (int i=0; i<m_promoters.GetSize(); i++) {
      fp << setfill(' ') << setbase(10) << " " << m_promoters[i].m_pos << ":";
      fp << "Ox" << setbase(16) << setfill('0') << setw(8) << (m_promoters[i].GetRegulatedBitCode()) << " ";
    }
    fp << setfill(' ') << setbase(10) << endl;
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
  cHeadCPU & inst_ptr = getIP();
  
  // Start up a search head at the position of the instruction pointer.
  cHeadCPU search_head(inst_ptr);
  NopSequence & search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  
  if (search_label.Size() == 0) {
    return inst_ptr;
  }
  
  // Call special functions depending on if jump is forwards or backwards.
  int found_pos = 0;
  if ( direction < 0 ) {
    found_pos = FindLabel_Backward(search_label, m_memory, inst_ptr.GetPosition() - search_label.Size());
  }
  
  // Jump forward.
  else if (direction > 0) {
    found_pos = FindLabel_Forward(search_label, m_memory, inst_ptr.GetPosition());
  }
  
  // Jump forward from the very beginning.
  else {
    found_pos = FindLabel_Forward(search_label, m_memory, 0);
  }
  
  // Return the last line of the found label, if it was found.
  if (found_pos >= 0) search_head.Set(found_pos - 1);
  
  // Return the found position (still at start point if not found).
  return search_head;
}


// Search forwards for search_label from _after_ position pos in the
// memory.  Return the first line _after_ the the found label.  It is okay
// to find search label's match inside another label.

int cHardwareCPU::FindLabel_Forward(const NopSequence & search_label,
                                    const InstructionSequence & search_genome, int pos)
{
  assert (pos < search_genome.GetSize() && pos >= 0);
  
  int search_start = pos;
  int label_size = search_label.Size();
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

int cHardwareCPU::FindLabel_Backward(const NopSequence & search_label,
                                     const InstructionSequence & search_genome, int pos)
{
  assert (pos < search_genome.GetSize());
  
  int search_start = pos;
  int label_size = search_label.Size();
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
cHeadCPU cHardwareCPU::FindLabel(const NopSequence & in_label, int direction)
{
  assert (in_label.Size() > 0);
  
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
    for (i = 0; i < in_label.Size(); i++) {
      if (!m_inst_set->IsNop(temp_head.GetInst()) ||
          in_label[i] != m_inst_set->GetNopMod(temp_head.GetInst())) {
        break;
      }
    }
    if (i == GetLabel().Size()) {
      temp_head.AbsJump(i - 1);
      return temp_head;
    }
    
    temp_head.AbsJump(direction);     // IDEALY: MAKE LARGER JUMPS
  }
  
  temp_head.AbsSet(-1);
  return temp_head;
}

void cHardwareCPU::FindLabelInMemory(const NopSequence& label, cHeadCPU& search_head)
{
  assert(label.Size() > 0); // Trying to find label of 0 size!
  
  
  while (search_head.InMemory()) {
    // If we are not in a label, jump to the next checkpoint...
    if (!m_inst_set->IsNop(search_head.GetInst())) {
      search_head.AbsJump(label.Size());
      continue;
    }
    
    // Otherwise, rewind to the begining of this label...
    
    while (!(search_head.AtFront()) && m_inst_set->IsNop(search_head.GetInst(-1)))
      search_head.AbsJump(-1);
    
    // Calculate the size of the label being checked, and make sure they
    // are equal.
    
    int size = 0;
    bool label_match = true;
    do {
      // Check if the nop matches
      if (size < label.Size() && label[size] != m_inst_set->GetNopMod(search_head.GetInst()))
        label_match = false;
      
      // Increment the current position and length calculation
      search_head.AbsJump(1);
      size++;
      
      // While still within memory and the instruction is a nop
    } while (search_head.InMemory() && m_inst_set->IsNop(search_head.GetInst()));
    
    if (size != label.Size()) continue;
    
    // temp_head will point to the first non-nop instruction after the label, or the end of the memory space
    //   if this is a match, return this position
    if (label_match) return;
  }
  
  // The label does not exist in this creature.
  
  search_head.AbsSet(-1);
}


void cHardwareCPU::ReadInst(const int in_inst)
{
  if (m_inst_set->IsNop( Instruction(in_inst) )) {
    GetReadLabel().AddNop(in_inst);
  } else {
    GetReadLabel().Clear();
  }
}


void cHardwareCPU::AdjustHeads()
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

void cHardwareCPU::ReadLabel(int max_size)
{
  int count = 0;
  cHeadCPU * inst_ptr = &( getIP() );
  
  GetLabel().Clear();
  
  if (max_size < 0 || max_size > GetLabel().MaxSize()) max_size = GetLabel().MaxSize();
  
  while (m_inst_set->IsNop(inst_ptr->GetNextInst()) && (count < max_size)) {
    count++;
    inst_ptr->Advance();
    GetLabel().AddNop(m_inst_set->GetNopMod(inst_ptr->GetInst()));
    
    // If this is the first line of the template, mark it executed.
    if (GetLabel().Size() <=	m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get()) {
      inst_ptr->SetFlagExecuted();
    }
  }
}


bool cHardwareCPU::ForkThread()
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
  while ( (m_thread_id_chart >> new_id) & 1) new_id++;
  m_threads[num_threads].SetID(new_id);
  m_thread_id_chart |= (1 << new_id);
  
  return true;
}


bool cHardwareCPU::KillThread()
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

inline int cHardwareCPU::FindModifiedRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_register = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_register;
}

inline int cHardwareCPU::FindModifiedNextRegister(int default_register)
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

inline int cHardwareCPU::FindModifiedPreviousRegister(int default_register)
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


inline int cHardwareCPU::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_head = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_head;
}


inline int cHardwareCPU::FindNextRegister(int base_reg)
{
  return (base_reg + 1) % NUM_REGISTERS;
}


bool cHardwareCPU::Allocate_Necro(const int new_size)
{
  m_memory.ResizeOld(new_size);
  return true;
}

bool cHardwareCPU::Allocate_Random(cAvidaContext& ctx, const int old_size, const int new_size)
{
  m_memory.Resize(new_size);
  
  for (int i = old_size; i < new_size; i++) {
    m_memory[i] = m_inst_set->GetRandomInst(ctx);
  }
  return true;
}

bool cHardwareCPU::Allocate_Default(const int new_size)
{
  m_memory.Resize(new_size);
  
  // New space already defaults to default instruction...
  
  return true;
}

bool cHardwareCPU::Allocate_Main(cAvidaContext& ctx, const int allocated_size)
{
  // must do divide before second allocate & must allocate positive amount...
  if (m_world->GetConfig().REQUIRE_ALLOCATE.Get() && m_mal_active == true) return false;
  
  if (allocated_size < 1) return false;
  
  const int old_size = m_memory.GetSize();
  const int new_size = old_size + allocated_size;
  
  // Make sure that the new size is in range.
  if (new_size > MAX_GENOME_LENGTH  ||  new_size < MIN_GENOME_LENGTH) return false;
  
  const int max_alloc_size = (int) (old_size * m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get());
  if (allocated_size > max_alloc_size) return false;
  
  const int max_old_size = (int) (allocated_size * m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get());
  if (old_size > max_old_size) return false;
  
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

int cHardwareCPU::calcCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  for (int i = parent_size; i < parent_size + child_size; i++) {
    if (m_memory.FlagCopied(i)) copied_size++;
  }
  return copied_size;
}


bool cHardwareCPU::Divide_Main(cAvidaContext& ctx, const int div_point,
                               const int extra_lines, double mut_multiplier)
{
  const int child_size = m_memory.GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
	
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  InstructionSequencePtr offspring_seq(new InstructionSequence(m_memory.Crop(div_point, div_point + child_size)));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)m_inst_set->GetInstSetName());
  Genome offspring(GetType(), props, offspring_seq);
  
  // Make sure it is an exact copy at this point (before divide mutations) if required
  const Genome& base_genome = m_organism->GetGenome();
  ConstInstructionSequencePtr seq_p;
  seq_p.DynamicCastFrom(base_genome.Representation());
  const InstructionSequence& seq = *seq_p;
  if (m_world->GetConfig().REQUIRE_EXACT_COPY.Get() && (seq != *offspring_seq) ) {
    return false;
  }
  
  m_organism->OffspringGenome() = offspring;
  
  // Cut off everything in this memory past the divide point.
  m_memory.Resize(div_point);
  
  // Handle Divide Mutations...
  Divide_DoMutations(ctx, mut_multiplier);
  
  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures1(ctx);
  
  if (m_world->GetConfig().DIVIDE_METHOD.Get() != DIVIDE_METHOD_OFFSPRING) {
    // reset first time instruction costs
    for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
      m_inst_ft_cost[i] = m_inst_set->GetFTCost(Instruction(i));
    }
  }
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child
  bool parent_alive = m_organism->ActivateDivide(ctx);
  
  // Do more work if the parent lives through the birth of the offspring
  if (parent_alive && m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset(ctx);
  
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
  const int child_size = m_memory.GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  InstructionSequencePtr offspring_seq(new InstructionSequence(m_memory.Crop(div_point, div_point + child_size)));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)m_inst_set->GetInstSetName());
  Genome offspring(GetType(), props, offspring_seq);
  
  m_organism->OffspringGenome() = offspring;
  
  // Cut off everything in this memory past the divide point.
  m_memory.Resize(div_point);
  
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
  for (unsigned i = 0; i <= 100; i++) {
    if (i == 0) {
      mutations = totalMutations = Divide_DoMutations(ctx, mut_multiplier);
    }
    else{
      mutations = Divide_DoMutations(ctx, mut_multiplier);
      m_world->GetStats().IncResamplings();
    }
    
    fitTest = Divide_TestFitnessMeasures1(ctx);
    
    if (!fitTest && mutations >= totalMutations) break;
    
  }
  // think about making this mutations == totalMuations - though this may be too hard...
  /*
   if (RScount > 2)
   cerr << "Resampled " << RScount << endl;
   */
  //org could not be resampled beneath the hard cap -- it is then steraalized
  if (fitTest/*RScount == 11*/) {
    m_organism->GetPhenotype().ChildFertile() = false;
    m_world->GetStats().IncFailedResamplings();
  }
  
  if (m_world->GetConfig().DIVIDE_METHOD.Get() != DIVIDE_METHOD_OFFSPRING) {
    
    // reset first time instruction costs
    for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
      m_inst_ft_cost[i] = m_inst_set->GetFTCost(Instruction(i));
    }
  }
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child, and do more work if the parent lives through the
  // birth.
  bool parent_alive = m_organism->ActivateDivide(ctx);
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset(ctx);
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
  const int child_size = m_memory.GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  InstructionSequencePtr offspring_seq(new InstructionSequence(m_memory.Crop(div_point, div_point + child_size)));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)m_inst_set->GetInstSetName());
  Genome offspring(GetType(), props, offspring_seq);
  
  m_organism->OffspringGenome() = offspring;
  
  // Cut off everything in this memory past the divide point.
  m_memory.Resize(div_point);
  
  int totalMutations = 0;
  int mutations = 0;
  //    RScount = 0;
  
  bool fitTest = false;
  
  // Handle Divide Mutations...
  /*
   Do mutations until one of these conditions are satisified:
   we have resampled X times
   we have an offspring with the same number of muations as the first offspring
   that is not reverted
   the parent is steralized (usually means an implicit mutation)
   */
  
  mutations = totalMutations = Divide_DoMutations(ctx, mut_multiplier,1);
  for (int i = 0; i < 100; i++) {
    if (i > 0) {
      mutations = Divide_DoExactMutations(ctx, mut_multiplier,1);
      m_world->GetStats().IncResamplings();
    }
    
    fitTest = Divide_TestFitnessMeasures1(ctx);
    //if (mutations > 1 ) cerr << "Too Many mutations!!!!!!!!!!!!!!!" << endl;
    if (fitTest == false && mutations >= totalMutations) break;
    
  }
  // think about making this mutations == totalMuations - though this may be too hard...
  /*
   if (RScount > 2)
   cerr << "Resampled " << RScount << endl;
   */
  //org could not be resampled beneath the hard cap -- it is then steraalized
  if (fitTest/*RScount == 11*/) {
    m_organism->GetPhenotype().ChildFertile() = false;
    m_world->GetStats().IncFailedResamplings();
  }
  
  if (m_world->GetConfig().DIVIDE_METHOD.Get() != DIVIDE_METHOD_OFFSPRING) {
    // reset first time instruction costs
    for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
      m_inst_ft_cost[i] = m_inst_set->GetFTCost(Instruction(i));
    }
  }
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child, and do more work if the parent lives through the
  // birth.
  bool parent_alive = m_organism->ActivateDivide(ctx);
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset(ctx);
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
  const int child_size = m_memory.GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  InstructionSequencePtr offspring_seq(new InstructionSequence(m_memory.Crop(div_point, div_point + child_size)));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)m_inst_set->GetInstSetName());
  Genome offspring(GetType(), props, offspring_seq);
  
  m_organism->OffspringGenome() = offspring;
  
  // Cut off everything in this memory past the divide point.
  m_memory.Resize(div_point);
  
  int totalMutations = 0;
  int mutations = 0;
  //    RScount = 0;
  
  bool fitTest = false;
  
  
  // Handle Divide Mutations...
  /*
   Do mutations until one of these conditions are satisified:
   we have resampled X times
   we have an offspring with the same number of muations as the first offspring
   that is not reverted
   the parent is steralized (usually means an implicit mutation)
   */
  for (int i = 0; i < 100; i++){
    if (i == 0){
      mutations = totalMutations = Divide_DoMutations(ctx, mut_multiplier,2);
    }
    else{
      Divide_DoExactMutations(ctx, mut_multiplier,mutations);
      m_world->GetStats().IncResamplings();
    }
    
    fitTest = Divide_TestFitnessMeasures(ctx);
    //if (mutations > 1 ) cerr << "Too Many mutations!!!!!!!!!!!!!!!" << endl;
    if (!fitTest && mutations >= totalMutations) break;
    
  }
  // think about making this mutations == totalMuations - though this may be too hard...
  /*
   if (RScount > 2)
   cerr << "Resampled " << RScount << endl;
   */
  //org could not be resampled beneath the hard cap -- it is then steraalized
  if (fitTest/*RScount == 11*/) {
    m_organism->GetPhenotype().ChildFertile() = false;
    m_world->GetStats().IncFailedResamplings();
  }
  
  if (m_world->GetConfig().DIVIDE_METHOD.Get() != DIVIDE_METHOD_OFFSPRING) {
    // reset first time instruction costs
    for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
      m_inst_ft_cost[i] = m_inst_set->GetFTCost(Instruction(i));
    }
  }
  
  m_mal_active = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child, and do more work if the parent lives through the
  // birth.
  bool parent_alive = m_organism->ActivateDivide(ctx);
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset(ctx);
  }
  
  return true;
}


//////////////////////////
// And the instructions...
//////////////////////////

bool cHardwareCPU::Inst_If0(cAvidaContext&)          // Execute next if ?bx? ==0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) != 0)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfNot0(cAvidaContext&)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) == 0)  getIP().Advance();
  return true;
}

// Same as Inst_If0, except AX is used by default, not BX
bool cHardwareCPU::Inst_If0_defaultAX(cAvidaContext&)          // Execute next if ?ax? ==0.
{
  const int reg_used = FindModifiedRegister(REG_AX);
  if (GetRegister(reg_used) != 0)  getIP().Advance();
  return true;
}

// Same as Inst_IfNot0, except AX is used by default, not BX
bool cHardwareCPU::Inst_IfNot0_defaultAX(cAvidaContext&)       // Execute next if ?ax? != 0.
{
  const int reg_used = FindModifiedRegister(REG_AX);
  if (GetRegister(reg_used) == 0)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfEqu(cAvidaContext&)      // Execute next if bx == ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) != GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfNEqu(cAvidaContext&)     // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) == GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGr0(cAvidaContext&)       // Execute next if ?bx? ! < 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) <= 0)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGr(cAvidaContext&)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) <= GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGrEqu0(cAvidaContext&)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) < 0)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGrEqu(cAvidaContext&)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) < GetRegister(op2)) getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLess0(cAvidaContext&)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) >= 0)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLess(cAvidaContext&)       // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) >=  GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLsEqu0(cAvidaContext&)       // Execute next if ?bx? != 0.
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if (GetRegister(reg_used) > 0) getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLsEqu(cAvidaContext&)       // Execute next if bx > ?cx?
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  if (GetRegister(op1) >  GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfBit1(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  if ((GetRegister(reg_used) & 1) == 0)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfANotEqB(cAvidaContext&)     // Execute next if AX != BX
{
  if (GetRegister(REG_AX) == GetRegister(REG_BX) )  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfBNotEqC(cAvidaContext&)     // Execute next if BX != CX
{
  if (GetRegister(REG_BX) == GetRegister(REG_CX) )  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfANotEqC(cAvidaContext&)     // Execute next if AX != BX
{
  if (GetRegister(REG_AX) == GetRegister(REG_CX) )  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfGrX(cAvidaContext&)       // Execute next if BX > X; X value set according to NOP label
{
  // Compares value in BX to a specific value.  The value to compare to is determined by the nop label as follows:
  //    no nop label (default): valueToCompare = 1; nop-A: valueToCompare = -1
  //                     nop-B: valueToCompare = 2; nop-C: valueToCompare =  4
  // @LMG 2/13/2009
  
  int valueToCompare = 1;
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    switch (m_inst_set->GetNopMod(getIP().GetInst())) {
        
      case REG_AX:
        valueToCompare = -1; break;
      case REG_BX:
        valueToCompare =  2; break;
      case REG_CX:
        valueToCompare =  4; break;
      default:
        valueToCompare =  1; break;
    }
    getIP().SetFlagExecuted();
    
  }
  
  if (GetRegister(REG_BX) <= valueToCompare)  getIP().Advance();
  
  return true;
}

bool cHardwareCPU::Inst_IfEquX(cAvidaContext&)       // Execute next if BX == X; X value set according to NOP label
{
  // Compares value in BX to a specific value.  The value to compare to is determined by the nop label as follows:
  //    no nop label (default): valueToCompare = 1; nop-A: valueToCompare = -1
  //                     nop-B: valueToCompare = 2; nop-C: valueToCompare =  4
  // @LMG 2/13/2009
  
  int valueToCompare = 1;
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    switch (m_inst_set->GetNopMod(getIP().GetInst())) {
        
      case REG_AX: valueToCompare = -1; break;
      case REG_BX: valueToCompare =  2; break;
      case REG_CX: valueToCompare =  4; break;
      default:     valueToCompare =  1; break;
    }
    getIP().SetFlagExecuted();
    
  }
  
  if (GetRegister(REG_BX) != valueToCompare)  getIP().Advance();
  
  return true;
}



bool cHardwareCPU::Inst_IfP0p125(cAvidaContext& ctx)
{
  if (ctx.GetRandom().P(0.875)) {
    getIP().Advance();
  }
  
  return true;
}


bool cHardwareCPU::Inst_IfP0p25(cAvidaContext& ctx)
{
  if (ctx.GetRandom().P(0.75)) {
    getIP().Advance();
  }
  
  return true;
}


bool cHardwareCPU::Inst_IfP0p50(cAvidaContext& ctx)
{
  if (ctx.GetRandom().P(0.5)) {
    getIP().Advance();
  }
  
  return true;
}


bool cHardwareCPU::Inst_IfP0p75(cAvidaContext& ctx)
{
  if (ctx.GetRandom().P(0.25)) {
    getIP().Advance();
  }
  
  return true;
}


bool cHardwareCPU::Inst_JumpF(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  
  // If there is no label, jump BX steps.
  if (GetLabel().Size() == 0) {
    GetActiveHead().Jump(GetRegister(REG_BX));
    return true;
  }
  
  // Otherwise, try to jump to the complement label.
  const cHeadCPU jump_location(FindLabel(1));
  if ( jump_location.GetPosition() != -1 ) {
    GetActiveHead().Set(jump_location);
    return true;
  }
  
  return false;
}


bool cHardwareCPU::Inst_JumpB(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  
  // If there is no label, jump BX steps.
  if (GetLabel().Size() == 0) {
    GetActiveHead().Jump(GetRegister(REG_BX));
    return true;
  }
  
  // otherwise jump to the complement label.
  const cHeadCPU jump_location(FindLabel(-1));
  if ( jump_location.GetPosition() != -1 ) {
    GetActiveHead().Set(jump_location);
    return true;
  }
  
  return false;
}

bool cHardwareCPU::Inst_Call(cAvidaContext&)
{
  // Put the starting location onto the stack
  const int location = getIP().GetPosition();
  StackPush(location);
  
  // Jump to the compliment label (or by the ammount in the bx register)
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  
  if (GetLabel().Size() == 0) {
    getIP().Jump(GetRegister(REG_BX));
    return true;
  }
  
  const cHeadCPU jump_location(FindLabel(1));
  if (jump_location.GetPosition() != -1) {
    getIP().Set(jump_location);
    return true;
  }
  
  return false;
}

bool cHardwareCPU::Inst_Return(cAvidaContext&)
{
  getIP().Set(StackPop());
  return true;
}



bool cHardwareCPU::Inst_Pop(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = StackPop();
  return true;
}

bool cHardwareCPU::Inst_Push(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  StackPush(GetRegister(reg_used));
  return true;
}

bool cHardwareCPU::Inst_HeadPop(cAvidaContext&)
{
  const int head_used = FindModifiedHead(HEAD_IP);
  getHead(head_used).Set(StackPop());
  return true;
}

bool cHardwareCPU::Inst_HeadPush(cAvidaContext&)
{
  const int head_used = FindModifiedHead(HEAD_IP);
  StackPush(getHead(head_used).GetPosition());
  if (head_used == HEAD_IP) {
    getHead(head_used).Set(getHead(HEAD_FLOW));
    m_advance_ip = false;
  }
  return true;
}


bool cHardwareCPU::Inst_PopA(cAvidaContext&) { GetRegister(REG_AX) = StackPop(); return true;}
bool cHardwareCPU::Inst_PopB(cAvidaContext&) { GetRegister(REG_BX) = StackPop(); return true;}
bool cHardwareCPU::Inst_PopC(cAvidaContext&) { GetRegister(REG_CX) = StackPop(); return true;}

bool cHardwareCPU::Inst_PushA(cAvidaContext&) { StackPush(GetRegister(REG_AX)); return true;}
bool cHardwareCPU::Inst_PushB(cAvidaContext&) { StackPush(GetRegister(REG_BX)); return true;}
bool cHardwareCPU::Inst_PushC(cAvidaContext&) { StackPush(GetRegister(REG_CX)); return true;}

bool cHardwareCPU::Inst_SwitchStack(cAvidaContext&) { SwitchStack(); return true; }

bool cHardwareCPU::Inst_Swap(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindNextRegister(op1);
  Swap(GetRegister(op1), GetRegister(op2));
  return true;
}

bool cHardwareCPU::Inst_SwapAB(cAvidaContext&)\
{
  Swap(GetRegister(REG_AX), GetRegister(REG_BX)); return true;
}
bool cHardwareCPU::Inst_SwapBC(cAvidaContext&)
{
  Swap(GetRegister(REG_BX), GetRegister(REG_CX)); return true;
}
bool cHardwareCPU::Inst_SwapAC(cAvidaContext&)
{
  Swap(GetRegister(REG_AX), GetRegister(REG_CX)); return true;
}

bool cHardwareCPU::Inst_CopyReg(cAvidaContext&)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = FindNextRegister(src);
  GetRegister(dst) = GetRegister(src);
  return true;
}

bool cHardwareCPU::Inst_CopyRegAB(cAvidaContext&)
{
  GetRegister(REG_AX) = GetRegister(REG_BX);   return true;
}
bool cHardwareCPU::Inst_CopyRegAC(cAvidaContext&)
{
  GetRegister(REG_AX) = GetRegister(REG_CX);   return true;
}
bool cHardwareCPU::Inst_CopyRegBA(cAvidaContext&)
{
  GetRegister(REG_BX) = GetRegister(REG_AX);   return true;
}
bool cHardwareCPU::Inst_CopyRegBC(cAvidaContext&)
{
  GetRegister(REG_BX) = GetRegister(REG_CX);   return true;
}
bool cHardwareCPU::Inst_CopyRegCA(cAvidaContext&)
{
  GetRegister(REG_CX) = GetRegister(REG_AX);   return true;
}
bool cHardwareCPU::Inst_CopyRegCB(cAvidaContext&)
{
  GetRegister(REG_CX) = GetRegister(REG_BX);   return true;
}

bool cHardwareCPU::Inst_Reset(cAvidaContext&)
{
  GetRegister(REG_AX) = 0;
  GetRegister(REG_BX) = 0;
  GetRegister(REG_CX) = 0;
  StackClear();
  m_last_cell_data = std::make_pair(false, 0);
  return true;
}

bool cHardwareCPU::Inst_ShiftR(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) >>= 1;
  return true;
}

bool cHardwareCPU::Inst_ShiftL(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) <<= 1;
  return true;
}

bool cHardwareCPU::Inst_Bit1(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) |=  1;
  return true;
}

bool cHardwareCPU::Inst_SetNum(cAvidaContext&)
{
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsInt(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValGrey(cAvidaContext&) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntGreyCode(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValDir(cAvidaContext&) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntDirect(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValAddP(cAvidaContext&) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntAdditivePolynomial(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValFib(cAvidaContext&) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntFib(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_ValPolyC(cAvidaContext&) {
  ReadLabel();
  GetRegister(REG_BX) = GetLabel().AsIntPolynomialCoefficent(NUM_NOPS);
  return true;
}

bool cHardwareCPU::Inst_Inc(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) += 1;
  return true;
}

bool cHardwareCPU::Inst_Dec(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) -= 1;
  return true;
}

bool cHardwareCPU::Inst_Zero(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = 0;
  return true;
}

bool cHardwareCPU::Inst_One(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = 1;
  return true;
}

bool cHardwareCPU::Inst_All1s(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = 0;
  
  for (int i=0; i< ((int) sizeof(int) * 8); i++) {
    GetRegister(reg_used) |= 1 << i;
  }
  
  return true;
}

bool cHardwareCPU::Inst_Neg(cAvidaContext&)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  GetRegister(dst) = -GetRegister(src);
  return true;
}

bool cHardwareCPU::Inst_Square(cAvidaContext&)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  GetRegister(dst) = GetRegister(src) * GetRegister(src);
  return true;
}

bool cHardwareCPU::Inst_Sqrt(cAvidaContext&)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  const int value = GetRegister(src);
  if (value > 1) GetRegister(dst) = static_cast<int>(sqrt(static_cast<double>(value)));
  else if (value < 0) {
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Log(cAvidaContext&)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  const int value = GetRegister(src);
  if (value >= 1) GetRegister(dst) = static_cast<int>(log(static_cast<double>(value)));
  else if (value < 0) {
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Log10(cAvidaContext&)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  const int value = GetRegister(src);
  if (value >= 1) GetRegister(dst) = static_cast<int>(log10(static_cast<double>(value)));
  else if (value < 0) {
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Add(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = GetRegister(op1) + GetRegister(op2);
  return true;
}

bool cHardwareCPU::Inst_Sub(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = GetRegister(op1) - GetRegister(op2);
  return true;
}

bool cHardwareCPU::Inst_Mult(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = GetRegister(op1) * GetRegister(op2);
  return true;
}

bool cHardwareCPU::Inst_Div(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  if (GetRegister(op2) != 0) {
    if (!(0-INT_MAX > GetRegister(op1) && GetRegister(op2) == -1)) GetRegister(dst) = GetRegister(op1) / GetRegister(op2);
  } else {
    return false;
  }
  return true;
}

bool cHardwareCPU::Inst_Mod(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  if (GetRegister(op2) != 0) {
    GetRegister(dst) = GetRegister(op1) % GetRegister(op2);
  } else {
    return false;
  }
  return true;
}


bool cHardwareCPU::Inst_Nand(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = ~(GetRegister(op1) & GetRegister(op2));
  return true;
}

bool cHardwareCPU::Inst_Nor(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = ~(GetRegister(op1) | GetRegister(op2));
  return true;
}

bool cHardwareCPU::Inst_Or(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = (GetRegister(op1) | GetRegister(op2));
  return true;
}

bool cHardwareCPU::Inst_And(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = (GetRegister(op1) & GetRegister(op2));
  return true;
}

bool cHardwareCPU::Inst_Not(cAvidaContext&)
{
  const int src = FindModifiedRegister(REG_BX);
  const int dst = src;
  GetRegister(dst) = ~(GetRegister(src));
  return true;
}

bool cHardwareCPU::Inst_Order(cAvidaContext&)
{
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  if (GetRegister(op1) > GetRegister(op2)) {
    Swap(GetRegister(op1), GetRegister(op2));
  }
  return true;
}

bool cHardwareCPU::Inst_Xor(cAvidaContext&)
{
  const int dst = FindModifiedRegister(REG_BX);
  const int op1 = REG_BX;
  const int op2 = REG_CX;
  GetRegister(dst) = GetRegister(op1) ^ GetRegister(op2);
  return true;
}

// Set the bit in ?BX? specified in its complement register
bool cHardwareCPU::Inst_Setbit(cAvidaContext&)
{
  const int to_set = FindModifiedRegister(REG_BX);
  const int bit_reg = FindNextRegister(to_set);
  
  const int bit_to_set = max(0, GetRegister(bit_reg)) % (sizeof(int) * 8);
  
  GetRegister(to_set) |= 1 << bit_to_set;
  
  return true;
}

// Clear the bit in ?BX? specified in its complement register
bool cHardwareCPU::Inst_Clearbit(cAvidaContext&)
{
  const int to_clear = FindModifiedRegister(REG_BX);
  const int bit_reg = FindNextRegister(to_clear);
  
  const int bit_to_clear = max(0, GetRegister(bit_reg)) % (sizeof(int) * 8);
  
  GetRegister(to_clear) &= ~(1 << bit_to_clear);
  
  return true;
}


bool cHardwareCPU::Inst_Copy(cAvidaContext& ctx)
{
  const int op1 = REG_BX;
  const int op2 = REG_AX;
  
  const cHeadCPU from(this, GetRegister(op1));
  cHeadCPU to(this, GetRegister(op2) + GetRegister(op1));
  
  if (m_organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();  // Mark this instruction as mutated...
    to.SetFlagCopyMut();  // Mark this instruction as copy mut...
  } else {
    to.SetInst(from.GetInst());
    to.ClearFlagMutated();  // UnMark
    to.ClearFlagCopyMut();  // UnMark
  }
  
  to.SetFlagCopied();  // Set the copied flag.
  //  cpu_stats.mut_stats.copies_exec++;
  return true;
}

bool cHardwareCPU::Inst_ReadInst(cAvidaContext&)
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
  
  if (m_organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();      // Mark this instruction as mutated...
    to.SetFlagCopyMut();      // Mark this instruction as copy mut...
  } else {
    to.SetInst(Instruction(value));
    to.ClearFlagMutated();     // UnMark
    to.ClearFlagCopyMut();     // UnMark
  }
  
  to.SetFlagCopied();  // Set the copied flag.
  return true;
}

bool cHardwareCPU::Inst_StackReadInst(cAvidaContext&)
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
  
  if (m_organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();      // Mark this instruction as mutated...
    to.SetFlagCopyMut();      // Mark this instruction as copy mut...
  } else {
    to.SetInst(Instruction(value));
    to.ClearFlagMutated();     // UnMark
    to.ClearFlagCopyMut();     // UnMark
  }
  
  to.SetFlagCopied();  // Set the copied flag.
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
  if (m_organism->TestCopyMut(ctx)) {
    to.SetInst(m_inst_set->GetRandomInst(ctx));
    to.SetFlagMutated();      // Mark this instruction as mutated...
    to.SetFlagCopyMut();      // Mark this instruction as copy mut...
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
  if (m_organism->TestCopyMut(ctx)) {
    if (from.GetInst() != to.GetInst()) getIP().Advance();
  } else {
    if (from.GetInst() == to.GetInst()) getIP().Advance();
  }
  return true;
}

bool cHardwareCPU::Inst_Allocate(cAvidaContext& ctx)   // Allocate bx more space...
{
  const int src = REG_BX;
  const int dst = REG_AX;
  const int size = m_memory.GetSize();
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
  return Divide_Main(ctx, m_memory.GetSize() / 2);
}

bool cHardwareCPU::Inst_CAlloc(cAvidaContext& ctx)
{
  return Allocate_Main(ctx, m_memory.GetSize());
}

bool cHardwareCPU::Inst_MaxAlloc(cAvidaContext& ctx)   // Allocate maximal more
{
  const int dst = REG_AX;
  const int cur_size = m_memory.GetSize();
  const int alloc_size = Apto::Min((int)(m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get() * cur_size), MAX_GENOME_LENGTH - cur_size);
  if (Allocate_Main(ctx, alloc_size)) {
    GetRegister(dst) = cur_size;
    return true;
  } else return false;
}

// Alloc and move write head if we're successful
bool cHardwareCPU::Inst_MaxAllocMoveWriteHead(cAvidaContext& ctx)   // Allocate maximal more
{
  const int dst = REG_AX;
  const int cur_size = m_memory.GetSize();
  const int alloc_size = Apto::Min((int)(m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get() * cur_size), MAX_GENOME_LENGTH - cur_size);
  if (Allocate_Main(ctx, alloc_size)) {
    GetRegister(dst) = cur_size;
    getHead(HEAD_WRITE).Set(cur_size);
    return true;
  } else return false;
}

bool cHardwareCPU::Inst_Repro(cAvidaContext& ctx)
{
  // check if repro can replace an existing organism
  if (m_world->GetConfig().REPRO_METHOD.Get() == 0 && m_organism->IsNeighborCellOccupied()) {
    return false;
  }
  
  if (m_organism->GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) {
    return false;
  }
  
  // Setup child
  m_organism->OffspringGenome() = m_organism->GetGenome();
  InstructionSequencePtr offspring_seq;
  offspring_seq.DynamicCastFrom(m_organism->OffspringGenome().Representation());
  
  ConstInstructionSequencePtr org_seq;
  org_seq.DynamicCastFrom(m_organism->GetGenome().Representation());
  
  // Perform Copy Mutations...
  if (m_organism->GetCopyMutProb() > 0) { // Skip this if no mutations....
    for (int i = 0; i < offspring_seq->GetSize(); i++) {
      if (m_organism->TestCopyMut(ctx)) {
        (*offspring_seq)[i] = m_inst_set->GetRandomInst(ctx);
      }
    }
  }
  
  //Perform divide mutations...
  Divide_DoMutations(ctx);
  
  // Check viability
  bool viable = Divide_CheckViable(ctx, org_seq->GetSize(), offspring_seq->GetSize(), 1);
  if (!viable) { return false; }
  
  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures(ctx);
  
  if (m_world->GetConfig().DIVIDE_METHOD.Get() != DIVIDE_METHOD_OFFSPRING) {
    // reset first time instruction costs
    for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
      m_inst_ft_cost[i] = m_inst_set->GetFTCost(Instruction(i));
    }
  }
  
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) m_advance_ip = false;
  
  const bool parent_alive = m_organism->ActivateDivide(ctx);
  
  //Reset the parent
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) Reset(ctx);
  }
  return true;
}

bool cHardwareCPU::Inst_ReproSex(cAvidaContext& ctx)
{
  m_organism->GetPhenotype().SetDivideSex(true);
  m_organism->GetPhenotype().SetCrossNum(1);
  return Inst_Repro(ctx);
}



bool cHardwareCPU::Inst_TaskPutRepro(cAvidaContext& ctx)
{
  // Do normal IO, but don't zero register
  //Inst_TaskPut(ctx);
  
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = GetRegister(reg_used);
  // GetRegister(reg_used) = 0;
  m_organism->DoOutput(ctx, value);
  
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


/* The organism can only replicate if the last task it performed is the task with ID 0 */
bool cHardwareCPU::Inst_ConditionalRepro(cAvidaContext& ctx)
{
  if (m_organism->GetPhenotype().GetLastTaskID() == 0) {
    return Inst_Repro(ctx);
  }
  return false;
}



bool cHardwareCPU::Inst_Sterilize(cAvidaContext&)
{
  m_organism->GetPhenotype().IsFertile() = false;
  return true;
}



bool cHardwareCPU::Inst_Die(cAvidaContext& ctx)
{
  m_organism->Die(ctx);
  return true;
}

bool cHardwareCPU::Inst_Poison(cAvidaContext&)
{
  double poison_multiplier = 1.0 - m_world->GetConfig().POISON_PENALTY.Get();
  m_organism->GetPhenotype().SetCurBonus(m_organism->GetPhenotype().GetCurBonus() * poison_multiplier);
  return true;
}

/* Similar to Kazi, this instructon probabilistically causes
 the organism to die. However, in this case it does so in
 order to win points for its group and it does not take out
 any other organims. */
bool  cHardwareCPU::Inst_Suicide(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_AX);
  double percentProb = ((double) (GetRegister(reg_used) % 100)) / 100.0;
  if ( ctx.GetRandom().P(percentProb) ) {
    m_organism->Die(ctx);
  }
  
  return true;
}


bool cHardwareCPU::Inst_TaskGet(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_CX);
  const int value = m_organism->GetNextInput();
  GetRegister(reg_used) = value;
  m_organism->DoInput(value);
  return true;
}


// @JEB - this instruction does more than two "gets" together, it also resets the inputs
bool cHardwareCPU::Inst_TaskGet2(cAvidaContext& ctx)
{
  // Randomize the inputs so they can't save numbers
  m_organism->GetOrgInterface().ResetInputs(ctx);   // Now re-randomize the inputs this organism sees
  m_organism->ClearInput();                         // Also clear their input buffers, or they can still claim
  // rewards for numbers no longer in their environment!
  
  const int reg_used_1 = FindModifiedRegister(REG_BX);
  const int reg_used_2 = FindNextRegister(reg_used_1);
  
  const int value1 = m_organism->GetNextInput();
  GetRegister(reg_used_1) = value1;
  m_organism->DoInput(value1);
  
  const int value2 = m_organism->GetNextInput();
  GetRegister(reg_used_2) = value2;
  m_organism->DoInput(value2);
  
  return true;
}

bool cHardwareCPU::Inst_TaskStackGet(cAvidaContext&)
{
  const int value = m_organism->GetNextInput();
  StackPush(value);
  m_organism->DoInput(value);
  return true;
}

bool cHardwareCPU::Inst_TaskStackLoad(cAvidaContext&)
{
  // @DMB - TODO: this should look at the input_size...
  for (int i = 0; i < 3; i++) StackPush( m_organism->GetNextInput() );
  return true;
}

bool cHardwareCPU::Inst_TaskPut(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int value = GetRegister(reg_used);
  GetRegister(reg_used) = 0;
  m_organism->DoOutput(ctx, value);
  return true;
}

bool cHardwareCPU::Inst_TaskPutResetInputs(cAvidaContext& ctx)
{
  bool return_value = Inst_TaskPut(ctx);          // Do a normal put
  m_organism->GetOrgInterface().ResetInputs(ctx);   // Now re-randomize the inputs this organism sees
  m_organism->ClearInput();                         // Also clear their input buffers, or they can still claim
  // rewards for numbers no longer in their environment!
  return return_value;
}

bool cHardwareCPU::Inst_TaskIO(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  
  // Do the "put" component
  const int value_out = GetRegister(reg_used);
  m_organism->DoOutput(ctx, value_out);  // Check for tasks completed.
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  GetRegister(reg_used) = value_in;
  m_organism->DoInput(value_in);
  return true;
}

bool cHardwareCPU::Inst_TaskIO_BonusCost(cAvidaContext& ctx, double bonus_cost)
{
  // Levy the cost
  double new_bonus = m_organism->GetPhenotype().GetCurBonus() * (1 - bonus_cost);
  if (new_bonus < 0) new_bonus = 0;
  //keep the bonus positive or zero
  m_organism->GetPhenotype().SetCurBonus(new_bonus);
  
  return Inst_TaskIO(ctx);
}

bool cHardwareCPU::Inst_TaskIO_Feedback(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  
  //check cur_bonus before the output
  double preOutputBonus = m_organism->GetPhenotype().GetCurBonus();
  
  // Do the "put" component
  const int value_out = GetRegister(reg_used);
  m_organism->DoOutput(ctx, value_out);  // Check for tasks completed.
  
  //check cur_merit after the output
  double postOutputBonus = m_organism->GetPhenotype().GetCurBonus();
  
  
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
  const int value_in = m_organism->GetNextInput();
  GetRegister(reg_used) = value_in;
  m_organism->DoInput(value_in);
  return true;
}

bool cHardwareCPU::Inst_MatchStrings(cAvidaContext& ctx)
{
  if (m_executedmatchstrings) return false;
  m_organism->DoOutput(ctx, 357913941);
  m_executedmatchstrings = true;
  return true;
}

bool cHardwareCPU::Inst_Send(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  m_organism->SendValue(GetRegister(reg_used));
  GetRegister(reg_used) = 0;
  
  return true;
}

bool cHardwareCPU::Inst_Receive(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = m_organism->ReceiveValue();
  
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
  const Apto::Array<double> res_count = m_organism->GetOrgInterface().GetResources(ctx);
  
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
  int real_label_length = GetLabel().Size();
  
  // Start and end labels to define the start and end indices of
  // resources that we need to add together
  NopSequence start_label = NopSequence(GetLabel());
  NopSequence   end_label = NopSequence(GetLabel());
  
  for (int i = 0; i < max_label_length - real_label_length; i++) {
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
  for (int i = start_index; i <= end_index; i++) {
    // if it's a valid resource
    if (i < res_count.GetSize()) {
      if (conversion_method == 0) { // Log
        // for log, add together and then take log
        dresource_result += (double) res_count[i];
      }
      else if (conversion_method == 1) { // Addition of multiplied resource amount
        int add_amount = (int) (res_count[i] * base);
        // Do some range checking to make sure we don't overflow
        resource_result = (INT_MAX - resource_result <= add_amount) ? INT_MAX : resource_result + add_amount;
      }
    }
  }
  
  // Take the log after adding resource amounts together! This way a zero can be assigned to INT_MIN
  if (conversion_method == 0) { // Log2
    // You really shouldn't be using the log method if you can get to zero resources
    if (dresource_result == 0.0) {
      resource_result = INT_MIN;
    }
    else {
      resource_result = (int)(log(dresource_result)/log(base));
    }
  }
  
  //Dump this value into an arbitrary register: BX
  GetRegister(reg_to_set) = resource_result;
  
  //We have to convert this to a different index that includes all degenerate labels possible: shortest to longest
  int sensed_index = 0;
  int on = 1;
  for (int i = 0; i < real_label_length; i++) {
    sensed_index += on;
    on *= num_nops;
  }
  sensed_index+= GetLabel().AsInt(num_nops);
  m_organism->GetPhenotype().IncSenseCount(sensed_index);
  
  return true;
  
  // Note that we are converting <double> resources to <int> register values
}



bool cHardwareCPU::Inst_SenseResourceID(cAvidaContext& ctx)
{
  const Apto::Array<double> res_count = m_organism->GetOrgInterface().GetResources(ctx);
  int reg_to_set = FindModifiedRegister(REG_BX);
  double max_resource = 0.0;
  // if more than one resource is available, return the resource ID with the most available in this spot (note that, with global resources, the GLOBAL total will evaluated)
  for (int i = 0; i < res_count.GetSize(); i++) {
    if (res_count[i] > max_resource) {
      max_resource = res_count[i];
      GetRegister(reg_to_set) = i;
    }
  }
  return true;
}



bool cHardwareCPU::Inst_SenseFacedHabitat(cAvidaContext& ctx)
{
  int reg_to_set = FindModifiedRegister(REG_BX);
  
  // get the resource library
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  
  // get the destination cell resource levels
  Apto::Array<double> cell_resource_levels = m_organism->GetOrgInterface().GetFacedCellResources(ctx);
  
  // check for any habitats ahead that affect movement, returning the most 'severe' habitat type
  // are there any barrier resources in the faced cell
  for (int i = 0; i < cell_resource_levels.GetSize(); i++) {
    if (resource_lib.GetResDef(i)->GetHabitat() == 2 && cell_resource_levels[i] > 0) {
      GetRegister(reg_to_set) = 2;
      return true;
    }
  }
  // if no barriers, are there any hills in the faced cell
  for (int i = 0; i < cell_resource_levels.GetSize(); i++) {
    if (resource_lib.GetResDef(i)->GetHabitat() == 1 && cell_resource_levels[i] > 0) {
      GetRegister(reg_to_set) = 1;
      return true;
    }
  }
  // if no barriers or hills, we return a 0 to indicate clear sailing
  GetRegister(reg_to_set) = 0;
  return true;
}

/* Convert modifying NOPs to the index of a resource.
 *
 * When the specification does not map to exactly one resource (either because the
 * specification does not have enough nops to fully specify, or because the number of
 * resources is not a power of the number of nops), choose randomly among the resources
 * covered by the specification.  The random choice is weighted by how much of the
 * resource is covered by the specification.
 *
 * For example, in a 3-nop 4-resource system:
 * A -> 75% chance resouce 0, 25% chance resource 1
 * AA and AB -> 100% chance resource 0
 * AC -> 75% chance resource 0, 25% chance resource 1
 *
 * Originally inspired by Jeff B.'s DoSense(); meant to be a helper function for
 * the various collect instructions, and anything else that wants to use this type
 * of resource NOP-specification.
 */
int cHardwareCPU::FindModifiedResource(cAvidaContext& ctx, int& spec_id)
{
  int num_resources = m_organism->GetOrgInterface().GetResources(ctx).GetSize();
  
  //if there are no resources, translation cannot be successful; return false
  if (num_resources <= 0)
  {return -1;}
  
  //calculate the maximum number of NOPs necessary to completely specify a resource
  int num_nops = GetInstSet().GetNumNops();
  int max_label_length = (int)(ceil(log((double)num_resources) / log((double)num_nops)));
  
  //attempt to read a label of the maximum length
  ReadLabel(max_label_length);
  
  //find the length of the label that was actually read
  int real_label_length = GetLabel().Size();
  
  // save the specification id
  spec_id = GetLabel().AsIntUnique(num_nops);
  
  /* Find the resource specified by the label.
   * If the specification is not complete, pick a resource from the range specified.
   * If the range covers resources unequally, this is taken into account.
   */
  
  // translate the specification into a number
  int label_int = GetLabel().AsInt(num_nops);
  
  // find the chunk of a unit range covered by the specification
  double chunk_size = 1.0 / pow(double(num_nops), real_label_length);
  
  
  // choose a point in the range
  double resource_approx = label_int * chunk_size + ctx.GetRandom().GetDouble(chunk_size);
  
  // translate it into a resource bin
  int bin_used = floor(resource_approx * num_resources);
  
  return bin_used;
}

/* Helper function to reduce code redundancy in the Inst_Collect variations,
 * including Inst_Destroy.
 * Calls FindModifiedResource() to decide which resource to collect, logs which
 * specification was used, then calls DoActualCollect() to do the environmental
 * resource removal and/or internal resource addition.
 *
 * env_remove    - specifies whether the collected resources should be removed from
 *                 the environment
 * internal_add  - specifies whether the collected resources should be added to
 *                 the organism's internal resources.
 * probabilistic - specifies whether the chance of collection success should be based on
 *                 the amount of resource in the environment.
 * unit          - specifies whether collection uses the ABSORB_RESOURCE_FRACTION
 *                 configuration or always collects 1 unit of resource.
 */
bool cHardwareCPU::DoCollect(cAvidaContext& ctx, bool env_remove, bool internal_add, bool probabilistic, bool unit)
{
  int spec_id;
  
  int bin_used = FindModifiedResource(ctx, spec_id);
  if (bin_used < 0) { return false; }  // collection failed, there's nothing to collect
  
  // Add this specification
  m_organism->IncCollectSpecCount(spec_id);
  
  return DoActualCollect(ctx, bin_used, env_remove, internal_add, probabilistic, unit);
}

bool cHardwareCPU::DoActualCollect(cAvidaContext& ctx, int bin_used, bool env_remove, bool internal_add, bool probabilistic, bool unit)
{
  // Set up res_change and max total
  const Apto::Array<double> res_count = m_organism->GetOrgInterface().GetResources(ctx);
  Apto::Array<double> res_change(res_count.GetSize());
  res_change.SetAll(0.0);
  double total = m_organism->GetRBinsTotal();
  double max = m_world->GetConfig().MAX_TOTAL_STORED.Get();
  
	/* First, if collection is probabilistic, check to see if it succeeds.
   *
   * If so, remove resource(s) from environment if env_remove is set;
   * add resource(s) to internal resource bins if internal_add is set
   * (and this would not fill the bin beyond max).
   */
  if (probabilistic) {
    double success_chance = res_count[bin_used] / double(m_world->GetConfig().COLLECT_PROB_DIVISOR.Get());
    if (success_chance < ctx.GetRandom().GetDouble()) {
      return false;
    }  // we define not collecting as failure
  }
  
  // Collect a unit (if possible) or some ABSORB_RESOURCE_FRACTION
  if (unit) {
    if (res_count[bin_used] >= 1.0) {
      res_change[bin_used] = -1.0;
    }
    else {
      return false;
    }  // failure: not enough to collect
  }
  else {
    res_change[bin_used] = -1 * (res_count[bin_used] * m_world->GetConfig().ABSORB_RESOURCE_FRACTION.Get());
  }
  
  if (internal_add && (max < 0 || (total + -1 * res_change[bin_used]) <= max)) {
    m_organism->AddToRBin(bin_used, -1 * res_change[bin_used]);
  }
  
  if (!env_remove || (max >= 0 && (total + -1 * res_change[bin_used]) > max)) {
    res_change[bin_used] = 0.0;
  }
  
  // Update resource counts to reflect res_change
  m_organism->GetOrgInterface().UpdateResources(ctx, res_change);
  return true;
}

/* Takes resource(s) from the environment and adds them to the internal resource
 * bins of the organism.
 */
bool cHardwareCPU::Inst_Collect(cAvidaContext& ctx)
{
  return DoCollect(ctx, true, true, false, false);
}

/* Like Inst_Collect, but the collected resources are not removed from the
 * environment.
 */
bool cHardwareCPU::Inst_CollectNoEnvRemove(cAvidaContext& ctx)
{
  return DoCollect(ctx, false, true, false, false);
}

/* Collects resource from the environment but does not add it to the organism,
 * effectively destroying it.
 */
bool cHardwareCPU::Inst_Destroy(cAvidaContext& ctx)
{
  return DoCollect(ctx, true, false, false, false);
}

/* A no-op, nop-modified in the same way as the "collect" instructions:
 * Does not remove resource from environment, does not add resource to organism */
bool cHardwareCPU::Inst_NopCollect(cAvidaContext& ctx)
{
  return DoCollect(ctx, false, false, false, false);
}

/* Collects one unit of resource from the environment and adds it to the internal
 * resource bins of the organism.  The probability of the instruction succeeding
 * is given by the level of that resource divided by the COLLECT_PROB_DIVISOR
 * config option.
 */
bool cHardwareCPU::Inst_CollectUnitProbabilistic(cAvidaContext& ctx)
{
  return DoCollect(ctx, true, true, true, true);
}

/* Takes the resource specified by the COLLECT_RESOURCE_SPECIFIC config option
 * from the environment and adds it to the internal resource bins of the organism.
 */
bool cHardwareCPU::Inst_CollectSpecific(cAvidaContext& ctx)
{
  const int resource = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  double res_before = m_organism->GetRBin(resource);
  bool success = DoActualCollect(ctx, resource, true, true, false, false);
  double res_after = m_organism->GetRBin(resource);
  GetRegister(FindModifiedRegister(REG_BX)) = (int)(res_after - res_before);
  return success;
}

/*! Sense the level of resources in this organism's cell, and if all of the
 resources present are above the min level for that resource, execute the following
 intruction.  Otherwise, skip the following instruction.
 */
bool cHardwareCPU::Inst_IfResources(cAvidaContext& ctx)
{
  // These are the current levels of resources at this cell:
  const Apto::Array<double> resources = m_organism->GetOrgInterface().GetResources(ctx);
  
  // Now we loop through the different reactions, checking to see if their
  // required resources are below what's available.  If so, we skip ahead an
  // instruction and return.
  const cReactionLib& rxlib = m_world->GetEnvironment().GetReactionLib();
  for (int i=0; i<rxlib.GetSize(); ++i) {
    cReaction* rx = rxlib.GetReaction(i);
    tLWConstListIterator<cReactionProcess> processes(rx->GetProcesses());
    while (!processes.AtEnd()) {
      const cReactionProcess* proc = processes.Next();
      cResourceDef* res = proc->GetResDef(); // Infinite resource == 0.
      if ((res != 0) && (resources[res->GetID()] < proc->GetMinNumber())) {
        getIP().Advance();
        return true;
      }
    }
  }
  return true;
}



bool cHardwareCPU::Inst_SearchF(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  const int search_size = FindLabel(1).GetPosition() - getIP().GetPosition();
  GetRegister(REG_BX) = search_size;
  GetRegister(REG_CX) = GetLabel().Size();
  return true;
}

bool cHardwareCPU::Inst_SearchB(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  const int search_size = getIP().GetPosition() - FindLabel(-1).GetPosition();
  GetRegister(REG_BX) = search_size;
  GetRegister(REG_CX) = GetLabel().Size();
  return true;
}

bool cHardwareCPU::Inst_MemSize(cAvidaContext&)
{
  GetRegister(FindModifiedRegister(REG_BX)) = m_memory.GetSize();
  return true;
}

bool cHardwareCPU::Inst_IOBufAdd1(cAvidaContext&)
{
  m_organism->AddOutput(1);
  return true;
}
bool cHardwareCPU::Inst_IOBufAdd0(cAvidaContext&)
{
  m_organism->AddOutput(0);
  return true;
}

bool cHardwareCPU::Inst_RotateL(cAvidaContext& ctx)
{
  const int num_neighbors = m_organism->GetNeighborhoodSize();
  
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  ReadLabel();
  
  // Always rotate at least once.
  m_organism->Rotate(ctx, -1);
  
  // If there is no label, then the one rotation was all we want.
  if (!GetLabel().Size()) return true;
  
  // Rotate until a complement label is found (or all have been checked).
  GetLabel().Rotate(1, NUM_NOPS);
  for (int i = 1; i < num_neighbors; i++) {
    cOrganism* neighbor = m_organism->GetNeighbor();
    if (neighbor != NULL) {
      cHeadCPU search_head(&neighbor->GetHardware());
      FindLabelInMemory(GetLabel(), search_head);
      if (search_head.InMemory()) return true;
    }
    
    // Otherwise keep rotating...
    m_organism->Rotate(ctx, 1);
  }
  return true;
}

bool cHardwareCPU::Inst_RotateR(cAvidaContext& ctx)
{
  const int num_neighbors = m_organism->GetNeighborhoodSize();
  
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  ReadLabel();
  
  // Always rotate at least once.
  m_organism->Rotate(ctx, 1);
  
  // If there is no label, then the one rotation was all we want.
  if (!GetLabel().Size()) return true;
  
  // Rotate until a complement label is found (or all have been checked).
  GetLabel().Rotate(1, NUM_NOPS);
  for (int i = 1; i < num_neighbors; i++) {
    cOrganism* neighbor = m_organism->GetNeighbor();
    if (neighbor != NULL) {
      cHeadCPU search_head(&neighbor->GetHardware());
      FindLabelInMemory(GetLabel(), search_head);
      if (search_head.InMemory()) return true;
    }
    
    // Otherwise keep rotating...
    m_organism->Rotate(ctx, -1);
  }
  return true;
}

bool cHardwareCPU::Inst_RotateLeftOne(cAvidaContext& ctx)
{
  m_organism->Rotate(ctx, 1);
  return true;
}

bool cHardwareCPU::Inst_RotateRightOne(cAvidaContext& ctx)
{
  m_organism->Rotate(ctx, -1);
  return true;
}

/**
 Rotate to facing specified by following label
 */
bool cHardwareCPU::Inst_RotateLabel(cAvidaContext& ctx)
{
  int standardNeighborhoodSize, actualNeighborhoodSize, newFacing, currentFacing;
  actualNeighborhoodSize = m_organism->GetNeighborhoodSize();
  
  ReadLabel();
  if (m_world->GetConfig().WORLD_GEOMETRY.Get() == nGeometry::TORUS ||
      m_world->GetConfig().WORLD_GEOMETRY.Get() == nGeometry::GRID) {
    standardNeighborhoodSize = 8;
  } else {
    exit(-1);
  }
  newFacing = GetLabel().AsIntGreyCode(NUM_NOPS) % standardNeighborhoodSize;
  
  for (int i = 0; i < actualNeighborhoodSize; i++) {
    currentFacing = m_organism->GetFacing();
    if (newFacing == currentFacing)
      break;
    m_organism->Rotate(ctx, 1);
  }
  return true;
}

bool cHardwareCPU::Inst_RotateUnoccupiedCell(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  
  for (int i = 0; i < m_organism->GetNeighborhoodSize(); i++) {
    if (!m_organism->IsNeighborCellOccupied()) { // faced cell is unoccupied
      GetRegister(reg_used) = 1;
      return true;
    }
    m_organism->Rotate(ctx, 1); // continue to rotate
  }
  GetRegister(reg_used) = 0;
  return true;
}

bool cHardwareCPU::Inst_RotateOccupiedCell(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  
  for (int i = 0; i < m_organism->GetNeighborhoodSize(); i++) {
    if (m_organism->IsNeighborCellOccupied()) { // faced cell is occupied
      GetRegister(reg_used) = 1;
      return true;
    }
    m_organism->Rotate(ctx, 1); // continue to rotate
  }
  GetRegister(reg_used) = 0;
  return true;
}

bool cHardwareCPU::Inst_RotateNextOccupiedCell(cAvidaContext& ctx)
{
  m_organism->Rotate(ctx, 1);
  return Inst_RotateOccupiedCell(ctx);
}

bool cHardwareCPU::Inst_RotateNextUnoccupiedCell(cAvidaContext& ctx)
{
  m_organism->Rotate(ctx, 1); // continue to rotate
  return Inst_RotateUnoccupiedCell(ctx);
}


bool cHardwareCPU::Inst_RotateHome(cAvidaContext& ctx)
{
  // Will rotate organism to face birth cell if org never used zero-easterly or zero-northerly. Otherwise will rotate org
  // to face the 'marked' spot where those instructions were executed.
  int easterly = m_organism->GetEasterly();
  int northerly = m_organism->GetNortherly();
  int correct_facing = 0;
  if (northerly > 0 && easterly == 0) correct_facing = 0; // rotate N
  else if (northerly > 0 && easterly < 0) correct_facing = 1; // rotate NE
  else if (northerly == 0 && easterly < 0) correct_facing = 2; // rotate E
  else if (northerly < 0 && easterly < 0) correct_facing = 3; // rotate SE
  else if (northerly < 0 && easterly == 0) correct_facing = 4; // rotate S
  else if (northerly < 0 && easterly > 0) correct_facing = 5; // rotate SW
  else if (northerly == 0 && easterly > 0) correct_facing = 6; // rotate W
  else if (northerly > 0 && easterly > 0) correct_facing = 7; // rotate NW
  for (int i = 0; i < m_organism->GetNeighborhoodSize(); i++) {
    m_organism->Rotate(ctx, 1);
    if (m_organism->GetFacedDir() == correct_facing) break;
  }
  return true;
}

bool cHardwareCPU::Inst_SetCopyMut(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int new_mut_rate = Apto::Max(GetRegister(reg_used), 1 );
  m_organism->SetCopyMutProb(static_cast<double>(new_mut_rate) / 10000.0);
  return true;
}

bool cHardwareCPU::Inst_ModCopyMut(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const double new_mut_rate = m_organism->GetCopyMutProb() + static_cast<double>(GetRegister(reg_used)) / 10000.0;
  if (new_mut_rate > 0.0) m_organism->SetCopyMutProb(new_mut_rate);
  return true;
}

// @WRE addition for movement
// Tumble sets the organism and cell to a new random facing
//
bool cHardwareCPU::Inst_Tumble(cAvidaContext& ctx)
{
  // Get number of neighbor cells that the organism can move to.
  const int num_neighbors = m_organism->GetNeighborhoodSize();
  // Exclude extreme case of the completely disconnected cell
  if (0 < num_neighbors) {
    // Choose a base 0 random number of turns to make in facing, [0 .. num_neighbors-2].
    int irot = ctx.GetRandom().GetUInt(num_neighbors-1);
    // Treat as base 0 number of turns to make
    for (int i = 0; i <= irot; i++) {
      m_organism->Rotate(ctx, 1);
    }
  }
  // Logging
  // ofstream tumblelog;
  // tumblelog.open("data/tumblelog.txt",ios::app);
  // tumblelog << organism->GetID() << "," << irot << endl;
  // tumblelog.close();
  
  return true;
}



// @WRE addition for movement
// Move uses the cPopulation::SwapCells method to move an m_organism to a different cell
// and the cPopulation::MoveOrganisms helper function to clean up after a move
// The cell selected as a destination is the one faced
bool cHardwareCPU::Inst_Move(cAvidaContext& ctx)
{
  // In TestCPU, movement fails...
  if (m_organism->GetCellID() == -1) return false;
  
  bool move_success = m_organism->Move(ctx);
  const int out_reg = FindModifiedRegister(REG_BX);
  GetRegister(out_reg) = move_success;
  return true;
}

// Multi-threading.
bool cHardwareCPU::Inst_ForkThread(cAvidaContext&)
{
  getIP().Advance();
  ForkThread();
  return true;
}

bool cHardwareCPU::Inst_ForkThreadLabel(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  
  // If there is no label, then do normal fork behavior
  if (GetLabel().Size() == 0) {
    return Inst_ForkThread(ctx);
  }
  
  cHeadCPU searchHead = FindLabel(+1);
  if ( searchHead.GetPosition() != getIP().GetPosition() ) {
    int save_pos = getIP().GetPosition();
    getIP().Set(searchHead.GetPosition() + 1);
    ForkThread();
    getIP().Set( save_pos );
  }
  
  return true;
}

bool cHardwareCPU::Inst_ForkThreadLabelIfNot0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) == 0) {
    ReadLabel();
    return false;
  }
  return Inst_ForkThreadLabel(ctx);
}

bool cHardwareCPU::Inst_ForkThreadLabelIf0(cAvidaContext& ctx)
{
  if (GetRegister(REG_BX) != 0) {
    ReadLabel();
    return false;
  }
  return Inst_ForkThreadLabel(ctx);
}

bool cHardwareCPU::Inst_KillThread(cAvidaContext&)
{
  if (KillThread()) m_advance_ip = false;
  return true;
}

bool cHardwareCPU::Inst_ThreadID(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = m_threads[m_cur_thread].GetID();
  return true;
}


// Head-based instructions

bool cHardwareCPU::Inst_SetHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(HEAD_IP);
  m_threads[m_cur_thread].cur_head = static_cast<unsigned char>(head_used);
  return true;
}

bool cHardwareCPU::Inst_AdvanceHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(HEAD_WRITE);
  getHead(head_used).Advance();
  return true;
}

bool cHardwareCPU::Inst_MoveHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(HEAD_IP);
  const int target = HEAD_FLOW;
  getHead(head_used).Set(getHead(target));
  if (head_used == HEAD_IP) m_advance_ip = false;
  return true;
}

bool cHardwareCPU::Inst_JumpHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(HEAD_IP);
  getHead(head_used).Jump(GetRegister(REG_CX) );
  // @JEB - probably shouldn't advance IP after jumping here?
  // Any negative number jumps to the beginning of the genome (pos 0)
  // and then we immediately advance past that first instruction.
  return true;
}

bool cHardwareCPU::Inst_GetHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(HEAD_IP);
  GetRegister(REG_CX) = getHead(head_used).GetPosition();
  return true;
}

bool cHardwareCPU::Inst_IfLabel(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLabelDirect(cAvidaContext&)
{
  ReadLabel();
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}


bool cHardwareCPU::Inst_HeadDivideMut(cAvidaContext& ctx, double mut_multiplier)
{
  AdjustHeads();
  const int divide_pos = getHead(HEAD_READ).GetPosition();
  int child_end =  getHead(HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
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
  const int divide_pos = getHead(HEAD_READ).GetPosition();
  int child_end =  getHead(HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
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
  const int divide_pos = getHead(HEAD_READ).GetPosition();
  int child_end =  getHead(HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
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
  const int divide_pos = getHead(HEAD_READ).GetPosition();
  int child_end =  getHead(HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
  bool ret_val = Divide_Main2RS(ctx, divide_pos, extra_lines, 1);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val;
}


bool cHardwareCPU::Inst_HeadDivideSex(cAvidaContext& ctx)
{
  m_organism->GetPhenotype().SetDivideSex(true);
  m_organism->GetPhenotype().SetCrossNum(1);
  return Inst_HeadDivide(ctx);
}

bool cHardwareCPU::Inst_HeadDivideAsex(cAvidaContext& ctx)
{
  m_organism->GetPhenotype().SetDivideSex(false);
  m_organism->GetPhenotype().SetCrossNum(0);
  return Inst_HeadDivide(ctx);
}

bool cHardwareCPU::Inst_HeadDivideAsexWait(cAvidaContext& ctx)
{
  m_organism->GetPhenotype().SetDivideSex(true);
  m_organism->GetPhenotype().SetCrossNum(0);
  return Inst_HeadDivide(ctx);
}

bool cHardwareCPU::Inst_HeadDivideMateSelect(cAvidaContext& ctx)
{
  // Take the label that follows this divide and use it as the ID for which
  // other organisms this one is willing to mate with.
  ReadLabel();
  m_organism->GetPhenotype().SetMateSelectID( GetLabel().AsInt(NUM_NOPS) );
  
  // Proceed as normal with the rest of mate selection.
  m_organism->GetPhenotype().SetDivideSex(true);
  m_organism->GetPhenotype().SetCrossNum(1);
  return Inst_HeadDivide(ctx);
}

bool cHardwareCPU::Inst_HeadRead(cAvidaContext& ctx)
{
  const int dst = REG_BX;
  
  const int head_id = FindModifiedHead(HEAD_READ);
  getHead(head_id).Adjust();
  
  // Mutations only occur on the read, for the moment.
  int read_inst = 0;
  if (m_organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx).GetOp();
  } else {
    read_inst = getHead(head_id).GetInst().GetOp();
  }
  GetRegister(dst) = read_inst;
  ReadInst(read_inst);
  
  if (m_slip_read_head && m_organism->TestCopySlip(ctx)) {
    getHead(head_id).Set(ctx.GetRandom().GetInt(m_memory.GetSize()));
  }
  
  getHead(head_id).Advance();
  return true;
}

bool cHardwareCPU::Inst_HeadWrite(cAvidaContext& ctx)
{
  const int src = REG_BX;
  const int head_id = FindModifiedHead(HEAD_WRITE);
  cHeadCPU& active_head = getHead(head_id);
  
  active_head.Adjust();
  
  int value = GetRegister(src);
  if (value < 0 || value >= m_inst_set->GetSize()) value = 0;
  
  active_head.SetInst(Instruction(value));
  active_head.SetFlagCopied();
  
  if (m_organism->TestCopyIns(ctx)) active_head.InsertInst(m_inst_set->GetRandomInst(ctx));
  if (m_organism->TestCopyDel(ctx)) active_head.RemoveInst();
  if (m_organism->TestCopyUniform(ctx)) doUniformCopyMutation(ctx, active_head);
  if (!m_slip_read_head && m_organism->TestCopySlip(ctx)) {
    doSlipMutation(ctx, m_memory, active_head.GetPosition());
  }
  
  // Advance the head after write...
  active_head.Advance();
  
  return true;
}

bool cHardwareCPU::Inst_HeadCopy(cAvidaContext& ctx)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU& read_head = getHead(HEAD_READ);
  cHeadCPU& write_head = getHead(HEAD_WRITE);
  
  read_head.Adjust();
  write_head.Adjust();
  
  // Do mutations.
  Instruction read_inst = read_head.GetInst();
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
    } else {
      doSlipMutation(ctx, m_memory, write_head.GetPosition());
    }
  }
  
  read_head.Advance();
  write_head.Advance();
  return true;
}

bool cHardwareCPU::HeadCopy_ErrorCorrect(cAvidaContext& ctx, double reduction)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU & read_head = getHead(HEAD_READ);
  cHeadCPU & write_head = getHead(HEAD_WRITE);
  
  read_head.Adjust();
  write_head.Adjust();
  
  // Do mutations.
  Instruction read_inst = read_head.GetInst();
  ReadInst(read_inst.GetOp());
  
  if ( ctx.GetRandom().P(m_organism->GetCopyMutProb() / reduction)) {
    read_inst = m_inst_set->GetRandomInst(ctx);
    write_head.SetFlagMutated();
    write_head.SetFlagCopyMut();
  }
  
  write_head.SetInst(read_inst);
  write_head.SetFlagCopied();  // Set the copied flag...
  
  if (ctx.GetRandom().P(m_organism->GetCopyInsProb() / reduction)) write_head.InsertInst(m_inst_set->GetRandomInst(ctx));
  if (ctx.GetRandom().P(m_organism->GetCopyDelProb() / reduction)) write_head.RemoveInst();
  if (ctx.GetRandom().P(m_organism->GetCopyUniformProb() / reduction)) doUniformCopyMutation(ctx, write_head);
  if (ctx.GetRandom().P(m_organism->GetCopySlipProb() / reduction)) {
    if (m_slip_read_head) {
      read_head.Set(ctx.GetRandom().GetInt(m_memory.GetSize()));
    } else {
      doSlipMutation(ctx, m_memory, write_head.GetPosition());
    }
  }
  
  read_head.Advance();
  write_head.Advance();
  return true;
}


bool cHardwareCPU::Inst_HeadSearch(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabel(0);
  const int search_size = found_pos.GetPosition() - getIP().GetPosition();
  GetRegister(REG_BX) = search_size;
  GetRegister(REG_CX) = GetLabel().Size();
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool cHardwareCPU::Inst_HeadSearchDirect(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabel(1);
  const int search_size = found_pos.GetPosition() - getIP().GetPosition();
  GetRegister(REG_BX) = search_size;
  GetRegister(REG_CX) = GetLabel().Size();
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool cHardwareCPU::Inst_SetFlow(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_CX);
  getHead(HEAD_FLOW).Set(GetRegister(reg_used));
  return true;
}



bool cHardwareCPU::Inst_GetUpdate(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  GetRegister(reg_used) = m_world->GetStats().GetUpdate();
  
  return true;
}


/*! This method places the calling organism's x-y coordinates in ?BX? and ?++BX?.
 
 Note that this method *will not work* from within the test CPU, so we have to guard
 against that.
 */
bool cHardwareCPU::Inst_GetCellPosition(cAvidaContext& ctx)
{
  int x = m_organism->GetOrgInterface().GetCellXPosition();
  int y = m_organism->GetOrgInterface().GetCellYPosition();
  // Fail if we're running in the test CPU.
  if (x == -1 || y == -1) return false;
  
  const int xreg = FindModifiedRegister(REG_BX);
  const int yreg = FindNextRegister(xreg);
  GetRegister(xreg) = x;
  GetRegister(yreg) = y;
  return true;
}

/*! This method places the calling organism's x coordinate in ?BX?.
 
 Note that this method *will not work* from within the test CPU, so we have to guard
 against that.
 */
bool cHardwareCPU::Inst_GetCellPositionX(cAvidaContext&)
{
  int x = m_organism->GetOrgInterface().GetCellXPosition();
  // Fail if we're running in the test CPU.
  if (x == -1) return false;
  
  const int xreg = FindModifiedRegister(REG_BX);
  GetRegister(xreg) = x;
  
  return true;
}

/*! This method places the calling organism's y coordinates in ?BX?.
 
 Note that this method *will not work* from within the test CPU, so we have to guard
 against that.
 */
bool cHardwareCPU::Inst_GetCellPositionY(cAvidaContext&)
{
  int y = m_organism->GetOrgInterface().GetCellYPosition();
  // Fail if we're running in the test CPU.
  if (y == -1) return false;
  
  const int yreg = FindModifiedRegister(REG_BX);
  GetRegister(yreg) = y;
  
  return true;
}

bool cHardwareCPU::Inst_GetDistanceFromDiagonal(cAvidaContext&)
{
  //  int absolute_cell_ID = m_organism->GetOrgInterface().GetCellID();
  
  assert(false);
  // @TODO - distance from diagonal
  
  //  std::pair<int, int> pos = m_world->GetPopulation().GetCellPosition(absolute_cell_ID);
  //  const int reg = FindModifiedRegister(REG_BX);
  //
  //  if (pos.first > pos.second) {
  //    GetRegister(reg) = (int)ceil((pos.first - pos.second)/2.0);
  //  } else {
  //    GetRegister(reg) = (int)floor((pos.first - pos.second)/2.0);
  //  }
  //  std::cerr<<"x = "<<pos.first<<"  y = "<<pos.second<<"  ans = "<<GetRegister(reg)<<std::endl;
  
  return true;
}

bool cHardwareCPU::Inst_GetDirectionOffNorth(cAvidaContext&) {
  const int out_reg = FindModifiedRegister(REG_BX);
  GetRegister(out_reg) = m_organism->GetFacedDir();
  return true;
}

bool cHardwareCPU::Inst_GetNortherly(cAvidaContext&) {
  const int out_reg = FindModifiedRegister(REG_BX);
  GetRegister(out_reg) = m_organism->GetNortherly();
  return true;
}

bool cHardwareCPU::Inst_GetEasterly(cAvidaContext&) {
  const int out_reg = FindModifiedRegister(REG_BX);
  GetRegister(out_reg) = m_organism->GetEasterly();
  return true;
}

bool cHardwareCPU::Inst_ZeroEasterly(cAvidaContext&) {
  m_organism->ClearEasterly();
  return true;
}

bool cHardwareCPU::Inst_ZeroNortherly(cAvidaContext&) {
  m_organism->ClearNortherly();
  return true;
}

//// Promoter Model ////

bool cHardwareCPU::Inst_Promoter(cAvidaContext&)
{
  // Promoters don't do anything themselves
  return true;
}

// Move the instruction ptr to the next active promoter
bool cHardwareCPU::Inst_Terminate(cAvidaContext& ctx)
{
  // Optionally,
  // Reset the thread.
  if (m_world->GetConfig().TERMINATION_RESETS.Get())
  {
    //const int write_head_pos = getHead(HEAD_WRITE).GetPosition();
    //const int read_head_pos = getHead(HEAD_READ).GetPosition();
    m_threads[m_cur_thread].Reset(this, m_threads[m_cur_thread].GetID());
    //getHead(HEAD_WRITE).Set(write_head_pos);
    //getHead(HEAD_READ).Set(read_head_pos);
    
    //Setting this makes it harder to do things. You have to be modular.
    m_organism->GetOrgInterface().ResetInputs(ctx);   // Re-randomize the inputs this organism sees
    m_organism->ClearInput();                         // Also clear their input buffers, or they can still claim
    // rewards for numbers no longer in their environment!
  }
  
  // Reset our count
  m_threads[m_cur_thread].ResetPromoterInstExecuted();
  m_advance_ip = false;
  const int reg_used = REG_BX; // register to put chosen promoter code in, for now always BX
  
  // Search for an active promoter
  int start_offset = m_promoter_offset;
  int start_index  = m_promoter_index;
  
  bool no_promoter_found = true;
  if ( m_promoters.GetSize() > 0 ) {
    while (true) {
      // If the next promoter is active, then break out
      NextPromoter();
      if (IsActivePromoter()) {
        no_promoter_found = false;
        break;
      }
      
      // If we just checked the promoter that we were originally on, then there
      // are no active promoters.
      if ( (start_offset == m_promoter_offset) && (start_index == m_promoter_index) ) break;
      
      // If we originally were not on a promoter, then stop once we check the
      // first promoter and an offset of zero
      if (start_index == -1) {
        start_index = 0;
      }
    }
  }
  
  if (no_promoter_found) {
    if ((m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get() == 0) || (m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get() == 2)) {
      // Set defaults for when no active promoter is found
      m_promoter_index = -1;
      getIP().Set(0);
      GetRegister(reg_used) = 0;
    }
    // Death to organisms that refuse to use promoters!
    else if (m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get() == 1) {
      m_organism->Die(ctx);
    } else {
      cout << "Unrecognized NO_ACTIVE_PROMOTER_EFFECT setting: " << m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get() << endl;
    }
  }
  else
  {
    // We found an active match, offset to just after it.
    // cHeadCPU will do the mod genome size for us
    getIP().Set(m_promoters[m_promoter_index].m_pos + 1);
    
    // Put its bit code in BX for the organism to have if option is set
    if ( m_world->GetConfig().PROMOTER_TO_REGISTER.Get() ) {
      GetRegister(reg_used) = m_promoters[m_promoter_index].m_bit_code;
    }
  }
  
  return true;
}

// Set a new regulation code (which is XOR'ed with ALL promoter codes).
bool cHardwareCPU::Inst_Regulate(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  int regulation_code = GetRegister(reg_used);
  
  for (int i=0; i< m_promoters.GetSize();i++) {
    m_promoters[i].m_regulation = regulation_code;
  }
  
  return true;
}

// Set a new regulation code, but only on a subset of promoters.
bool cHardwareCPU::Inst_RegulateSpecificPromoters(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  int regulation_code = GetRegister(reg_used);
  
  const int reg_promoter = FindModifiedRegister((reg_used+1) % NUM_REGISTERS);
  int regulation_promoter = GetRegister(reg_promoter);
  
  for (int i=0; i< m_promoters.GetSize();i++) {
    //Look for consensus bit matches over the length of the promoter code
    int test_p_code = m_promoters[i].m_bit_code;
    int test_r_code = regulation_promoter;
    int bit_count = 0;
    for (int j=0; j<m_world->GetConfig().PROMOTER_EXE_LENGTH.Get();j++) {
      if ((test_p_code & 1) == (test_r_code & 1)) bit_count++;
      test_p_code >>= 1;
      test_r_code >>= 1;
    }
    if (bit_count >= m_world->GetConfig().PROMOTER_EXE_LENGTH.Get() / 2) {
      m_promoters[i].m_regulation = regulation_code;
    }
  }
  
  return true;
}


bool cHardwareCPU::Inst_SenseRegulate(cAvidaContext& ctx)
{
  unsigned int bits = 0;
  const Apto::Array<double> & res_count = m_organism->GetOrgInterface().GetResources(ctx);
  assert (res_count.GetSize() != 0);
  for (int i=0; i<m_world->GetConfig().PROMOTER_CODE_SIZE.Get(); i++) {
    int b = i % res_count.GetSize();
    bits <<= 1;
    bits += (res_count[b] != 0);
  }
  
  for (int i=0; i< m_promoters.GetSize();i++) {
    m_promoters[i].m_regulation = bits;
  }
  
  return true;
}

// Create a number from inst bit codes
bool cHardwareCPU::Do_Numberate(cAvidaContext&, int num_bits)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  
  // advance the IP now, so that it rests on the beginning of our number
  getIP().Advance();
  m_advance_ip = false;
  
  int num = Numberate(getIP().GetPosition(), +1, num_bits);
  GetRegister(reg_used) = num;
  
  return true;
}

// Move to the next promoter.
void cHardwareCPU::NextPromoter()
{
  // Move promoter index, rolling over if necessary
  m_promoter_index++;
  if (m_promoter_index == m_promoters.GetSize()) {
    m_promoter_index = 0;
    
    // Move offset, rolling over when there are not enough bits before we would have to wrap around left
    m_promoter_offset+=m_world->GetConfig().PROMOTER_EXE_LENGTH.Get();
    if (m_promoter_offset + m_world->GetConfig().PROMOTER_EXE_LENGTH.Get() > m_world->GetConfig().PROMOTER_CODE_SIZE.Get()) {
      m_promoter_offset = 0;
    }
  }
}


// Check whether the current promoter is active.
bool cHardwareCPU::IsActivePromoter()
{
  assert( m_promoters.GetSize() != 0 );
  int count = 0;
  unsigned int code = m_promoters[m_promoter_index].GetRegulatedBitCode();
  for (int i=0; i<m_world->GetConfig().PROMOTER_EXE_LENGTH.Get(); i++) {
    int offset = m_promoter_offset + i;
    offset %= m_world->GetConfig().PROMOTER_CODE_SIZE.Get();
    int state = code >> offset;
    count += (state & 1);
  }
  
  return (count >= m_world->GetConfig().PROMOTER_EXE_THRESHOLD.Get());
}

// Construct a promoter bit code from instruction bit codes
int cHardwareCPU::Numberate(int _pos, int _dir, int _num_bits)
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
  while (code_size < _num_bits) {
    unsigned int inst_code = (unsigned int) GetInstSet().GetInstructionCode(m_memory[j]);
    // shift bits in, one by one ... excuse the counter variable pun
    for (int code_on = 0; (code_size < _num_bits) && (code_on < m_world->GetConfig().INST_CODE_LENGTH.Get()); code_on++) {
      if (_dir < 0) {
        code >>= 1; // shift first so we don't go one too far at the end
        code += (1 << (_num_bits - 1)) * (inst_code & 1);
        inst_code >>= 1;
      } else {
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


//// Copied from cHardwareExperimental -- @JEB
static const unsigned int CONSENSUS = (sizeof(int) * 8) / 2;
static const unsigned int CONSENSUS24 = 12;

inline unsigned int cHardwareCPU::BitCount(unsigned int value) const
{
  const unsigned int w = value - ((value >> 1) & 0x55555555);
  const unsigned int x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
  const unsigned int bit_count = ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
  return bit_count;
}

bool cHardwareCPU::Inst_BitConsensus(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedNextRegister(reg_used);
  GetRegister(reg_used) = (BitCount(GetRegister(op1)) >= CONSENSUS) ? 1 : 0;
  return true;
}

bool cHardwareCPU::Inst_BitConsensus24(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(REG_BX);
  const int op1 = FindModifiedNextRegister(reg_used);
  GetRegister(reg_used) = (BitCount(GetRegister(op1) & MASK_LOW24) >= CONSENSUS24) ? 1 : 0;
  return true;
}

bool cHardwareCPU::Inst_IfConsensus(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(REG_BX);
  if (BitCount(GetRegister(op1)) <  CONSENSUS)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfConsensus24(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(REG_BX);
  if (BitCount(GetRegister(op1) & MASK_LOW24) <  CONSENSUS24)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLessConsensus(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1)) >=  BitCount(GetRegister(op2)))  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfLessConsensus24(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(REG_BX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1) & MASK_LOW24) >=  BitCount(GetRegister(op2) & MASK_LOW24))  getIP().Advance();
  return true;
}

//// End copied from cHardwareExperimental


/* Bit masking instructions */

// masks sign bit in a register
bool cHardwareCPU::Inst_MaskSignBit(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_BX);
  GetRegister(reg) = GetRegister(reg) & MASK_SIGNBIT;
  return true;
}

// masks lower 16 bits in ?BX? register
bool cHardwareCPU::Inst_MaskOffLower16Bits(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_BX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER16;
  return true;
}

// masks lower 16 bits in ?AX? register
bool cHardwareCPU::Inst_MaskOffLower16Bits_defaultAX(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_AX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER16;
  return true;
}

// masks lower 15 bits in ?BX? register
bool cHardwareCPU::Inst_MaskOffLower15Bits(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_BX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER17;
  return true;
}

// masks lower 15 bits in ?AX? register
bool cHardwareCPU::Inst_MaskOffLower15Bits_defaultAX(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_AX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER17;
  return true;
}

// masks lower 14 bits in ?BX? register
bool cHardwareCPU::Inst_MaskOffLower14Bits(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_BX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER18;
  return true;
}

// masks lower 14 bits in ?AX? register
bool cHardwareCPU::Inst_MaskOffLower14Bits_defaultAX(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_AX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER18;
  return true;
}

// masks lower 13 bits in ?BX? register
bool cHardwareCPU::Inst_MaskOffLower13Bits(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_BX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER19;
  return true;
}

// masks lower 13 bits in ?AX? register
bool cHardwareCPU::Inst_MaskOffLower13Bits_defaultAX(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_AX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER19;
  return true;
}

// masks lower 12 bits in ?BX? register
bool cHardwareCPU::Inst_MaskOffLower12Bits(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_BX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER20;
  return true;
}

// masks lower 12 bits in ?AX? register
bool cHardwareCPU::Inst_MaskOffLower12Bits_defaultAX(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_AX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER20;
  return true;
}

// masks lower 8 bits in ?BX? register
bool cHardwareCPU::Inst_MaskOffLower8Bits(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_BX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER24;
  return true;
}

// masks lower 8 bits in ?AX? register
bool cHardwareCPU::Inst_MaskOffLower8Bits_defaultAX(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_AX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER24;
  return true;
}

// masks lower 4 bits in ?BX? register
bool cHardwareCPU::Inst_MaskOffLower4Bits(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_BX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER28;
  return true;
}

// masks lower 4 bits in ?AX? register
bool cHardwareCPU::Inst_MaskOffLower4Bits_defaultAX(cAvidaContext&)
{
  const int reg = FindModifiedRegister(REG_AX);
  GetRegister(reg) = GetRegister(reg) & MASK_UPPER28;
  return true;
}




//// Placebo insts ////
bool cHardwareCPU::Inst_Skip(cAvidaContext&)
{
  getIP().Advance();
  return true;
}







bool cHardwareCPU::Inst_GetResStored(cAvidaContext&)
//Get amount of stored collect specific resource (how much do I have available for res_cost instructions).
{
  assert(m_organism != 0);
  const int out_reg = FindModifiedRegister(REG_BX);
  const int resource = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  // needs to return int...we round down so that they don't think there is more available than they need
  int res_stored = (int) (m_organism->GetRBin(resource) * 100 - 0.5);
  GetRegister(out_reg) = res_stored;
  return true;
}

bool cHardwareCPU::Inst_GetID(cAvidaContext&)
{
  assert(m_organism != 0);
  const int out_reg = FindModifiedRegister(REG_BX);
  GetRegister(out_reg) = m_organism->GetID();
  
  return true;
}

bool cHardwareCPU::Inst_GetFacedVitalityDiff(cAvidaContext&)
//Get difference in vitality of this organism and faced neighbor.
{
  assert(m_organism != 0);
  
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism * neighbor = m_organism->GetNeighbor();
  if (neighbor->IsDead())  return false;
  
  const int out_reg = FindModifiedRegister(REG_BX);
  // return % diff
  int vit_diff = (int) ((neighbor->GetVitality() -  m_organism->GetVitality())/m_organism->GetVitality() * 100 + 0.5);
  GetRegister(out_reg) = vit_diff;
  return true;
}


bool cHardwareCPU::Inst_GetFacedOrgID(cAvidaContext&)
//Get ID of organism faced by this one, if there is an organism in front.
{
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism * neighbor = m_organism->GetNeighbor();
  if (neighbor->IsDead())  return false;
  
  const int out_reg = FindModifiedRegister(REG_BX);
  GetRegister(out_reg) = neighbor->GetID();
  return true;
}


//Attack organism faced by this one, if there is an organism in front. This will use vitality bins if those are set.
bool cHardwareCPU::Inst_AttackFacedOrg(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism* target = m_organism->GetNeighbor();
  if (target->IsDead()) return false;
  
  const int target_cell = target->GetCellID();
  
  //Use vitality settings to decide who wins this battle.
  bool kill_attacker = true;
  if (m_world->GetConfig().MOVEMENT_COLLISIONS_SELECTION_TYPE.Get() == 0)
    // 50% chance, no modifiers
    kill_attacker = ctx.GetRandom().P(0.5);
  else if (m_world->GetConfig().MOVEMENT_COLLISIONS_SELECTION_TYPE.Get() == 1) {
    //vitality based
    const double attacker_vitality = m_organism->GetVitality();
    const double target_vitality = target->GetVitality();
    const double attacker_win_odds = ((attacker_vitality) / (attacker_vitality + target_vitality));
    const double target_win_odds = ((target_vitality) / (attacker_vitality + target_vitality));
    
    const double odds_someone_dies = max(attacker_win_odds, target_win_odds);
    const double odds_target_dies = (1 - target_win_odds) * odds_someone_dies;
    const double decider = ctx.GetRandom().GetDouble(1);
    
    if (decider < (1 - odds_someone_dies)) return true;
    else if (decider < ((1 - odds_someone_dies) + odds_target_dies)) kill_attacker = false;
  }
  if (kill_attacker) {
    m_organism->Die(ctx);
    return true;
  }
  
  m_world->GetPopulation().AttackFacedOrg(ctx, target_cell);
  return true;
}

//Get odds of winning or tieing in a fight. This will use vitality bins if those are set.
bool cHardwareCPU::Inst_GetAttackOdds(cAvidaContext&)
{
  assert(m_organism != 0);
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism* target = m_organism->GetNeighbor();
  if (target->IsDead()) return false;
  
  const double attacker_vitality = m_organism->GetVitality();
  const double target_vitality = target->GetVitality();
  
  const double attacker_win_odds = ((attacker_vitality) / (attacker_vitality + target_vitality));
  const double target_win_odds = ((target_vitality) / (attacker_vitality + target_vitality));
  
  const double odds_someone_dies = max(attacker_win_odds, target_win_odds);
  // my win odds are odds nobody dies or someone dies and it's the target
  const double odds_I_dont_die = (1 - odds_someone_dies) + ((1 - target_win_odds) * odds_someone_dies);
  
  // return odds as %
  const int out_reg = FindModifiedRegister(REG_BX);
  GetRegister(out_reg) = (int) (odds_I_dont_die * 100 + 0.5);
  return true;
}




//! Loads the current neighborhood into the organism's memory.
bool cHardwareCPU::Inst_GetNeighborhood(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  m_organism->LoadNeighborhood(ctx);
  
  return true;
}


//! Test if the current neighborhood has changed from that in the organism's memory.
bool cHardwareCPU::Inst_IfNeighborhoodChanged(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (!m_organism->HasNeighborhoodChanged(ctx)) {
    getIP().Advance();
  }
	
  return true;
}






bool cHardwareCPU::Inst_DivideSexMatingType(cAvidaContext& ctx)
{
  //Check if the organism is sexually mature
  if (m_organism->GetPhenotype().GetMatingType() == MATING_TYPE_JUVENILE) {
    //If not, fail
    return false;
  } else {
    //Otherwise, divide
    return Inst_HeadDivideSex(ctx);
  }
}

bool cHardwareCPU::Inst_IfMatingTypeMale(cAvidaContext&)
{
  //Execute the next instruction if the organism's mating type is male
  if (m_organism->GetPhenotype().GetMatingType() != MATING_TYPE_MALE)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfMatingTypeFemale(cAvidaContext&)
{
  //Execute the next instruction if the organism's mating type is female
  if (m_organism->GetPhenotype().GetMatingType() != MATING_TYPE_FEMALE)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IfMatingTypeJuvenile(cAvidaContext&)
{
  //Execute the next instruction if the organism has not matured sexually
  if (m_organism->GetPhenotype().GetMatingType() != MATING_TYPE_JUVENILE)  getIP().Advance();
  return true;
}

bool cHardwareCPU::Inst_IncrementMatingDisplayA(cAvidaContext&)
{
  //Increment the organism's mating display A trait
  int counter = m_organism->GetPhenotype().GetCurMatingDisplayA();
  counter++;
  m_organism->GetPhenotype().SetCurMatingDisplayA(counter);
  return true;
}

bool cHardwareCPU::Inst_IncrementMatingDisplayB(cAvidaContext&)
{
  //Increment the organism's mating display A trait
  int counter = m_organism->GetPhenotype().GetCurMatingDisplayB();
  counter++;
  m_organism->GetPhenotype().SetCurMatingDisplayB(counter);
  return true;
}

bool cHardwareCPU::Inst_SetMatingDisplayA(cAvidaContext&)
//Sets the display value a to be equal to the value of ?BX?
{
  //Get the register and its contents as the new display value
  const int reg_used = FindModifiedRegister(REG_BX);
  const int new_display = GetRegister(reg_used);
  
  //Set the organism's mating display A trait
  m_organism->GetPhenotype().SetCurMatingDisplayA(new_display);
  return true;
}

bool cHardwareCPU::Inst_SetMatingDisplayB(cAvidaContext&)
//Sets the display value b to be equal to the value of ?BX?
{
  //Get the register and its contents as the new display value
  const int reg_used = FindModifiedRegister(REG_BX);
  const int new_display = GetRegister(reg_used);
  
  //Set the organism's mating display A trait
  m_organism->GetPhenotype().SetCurMatingDisplayB(new_display);
  return true;
}

bool cHardwareCPU::Inst_SetMatePreference(cAvidaContext&, int mate_pref)
{
  m_organism->GetPhenotype().SetMatePreference(mate_pref);
  return true;
}
bool cHardwareCPU::Inst_SetMatePreferenceHighestDisplayA(cAvidaContext& ctx) { return Inst_SetMatePreference(ctx, MATE_PREFERENCE_HIGHEST_DISPLAY_A); }
bool cHardwareCPU::Inst_SetMatePreferenceHighestDisplayB(cAvidaContext& ctx) { return Inst_SetMatePreference(ctx, MATE_PREFERENCE_HIGHEST_DISPLAY_B); }
bool cHardwareCPU::Inst_SetMatePreferenceRandom(cAvidaContext& ctx) { return Inst_SetMatePreference(ctx, MATE_PREFERENCE_RANDOM); }
bool cHardwareCPU::Inst_SetMatePreferenceHighestMerit(cAvidaContext& ctx) { return Inst_SetMatePreference(ctx, MATE_PREFERENCE_HIGHEST_MERIT); }
