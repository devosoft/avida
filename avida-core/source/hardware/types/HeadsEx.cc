/*
 *  hardware/types/HeadsEX.cc
 *  avida-core
 *
 *  Created by David on 1/31/14.
 *  Copyright 2014 Michigan State University. All rights reserved.
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

#include "avida/private/hardware/types/HeadsEX.h"

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


static const unsigned int CONSENSUS = (sizeof(int) * 8) / 2;
static const unsigned int CONSENSUS24 = 12;

inline unsigned int Hardware::Types::HeadsEX::BitCount(unsigned int value) const
{
  const unsigned int w = value - ((value >> 1) & 0x55555555);
  const unsigned int x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
  const unsigned int bit_count = ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
  return bit_count;
}


StaticTableInstLib<Hardware::Types::HeadsEX::tMethod>* Hardware::Types::HeadsEX::s_inst_slib = Hardware::Types::HeadsEX::initInstLib();

StaticTableInstLib<Hardware::Types::HeadsEX::tMethod>* Hardware::Types::HeadsEX::initInstLib(void)
{
  struct NOPEntry {
    Apto::String name;
    int nop_mod;
    inline NOPEntry(const Apto::String& name, int nop_mod) : name(name), nop_mod(nop_mod) { ; }
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
    
    NOPEntry("nop-I", rIX),
    NOPEntry("nop-J", rJX),
    NOPEntry("nop-K", rKX),
    NOPEntry("nop-L", rLX),
    NOPEntry("nop-M", rMX),
    NOPEntry("nop-N", rNX),
    NOPEntry("nop-O", rOX),
    NOPEntry("nop-P", rPX),
  };
  
  static const StaticTableInstLib<tMethod>::MethodEntry s_f_array[] = {
    /*
     Note: all entries of cNOPEntryCPU s_n_array must have corresponding
     in the same order in StaticTableInstLib<tMethod>::MethodEntry s_f_array, and these entries must
     be the first elements of s_f_array.
     */
    StaticTableInstLib<tMethod>::MethodEntry("nop-A", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-B", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-C", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-D", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-E", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-F", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-G", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-H", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    
    StaticTableInstLib<tMethod>::MethodEntry("nop-I", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-J", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-K", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-L", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-M", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-N", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-O", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-P", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, NOP, "No-operation; modifies other instructions"),
    
    StaticTableInstLib<tMethod>::MethodEntry("NULL", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, 0, "True no-operation instruction: does nothing"),
    StaticTableInstLib<tMethod>::MethodEntry("nop-X", &Hardware::Types::HeadsEX::Inst_Nop, INST_CLASS_NOP, 0, "True no-operation instruction: does nothing"),
    
    
    // Threading
    StaticTableInstLib<tMethod>::MethodEntry("fork-thread", &Hardware::Types::HeadsEX::Inst_ForkThread),
    StaticTableInstLib<tMethod>::MethodEntry("thread-create", &Hardware::Types::HeadsEX::Inst_ThreadCreate),
    StaticTableInstLib<tMethod>::MethodEntry("exit-thread", &Hardware::Types::HeadsEX::Inst_ExitThread),
    StaticTableInstLib<tMethod>::MethodEntry("id-thread", &Hardware::Types::HeadsEX::Inst_IdThread),
    
    
    // Standard Conditionals
    StaticTableInstLib<tMethod>::MethodEntry("if-n-equ", &Hardware::Types::HeadsEX::Inst_IfNEqu, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX?!=?CX?, else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-less", &Hardware::Types::HeadsEX::Inst_IfLess, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? < ?CX?, else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-not-0", &Hardware::Types::HeadsEX::Inst_IfNotZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? != 0, else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-equ-0", &Hardware::Types::HeadsEX::Inst_IfEqualZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? == 0, else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-gtr-0", &Hardware::Types::HeadsEX::Inst_IfGreaterThanZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? > 0, else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-less-0", &Hardware::Types::HeadsEX::Inst_IfLessThanZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? < 0, else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-gtr-x", &Hardware::Types::HeadsEX::Inst_IfGtrX, INST_CLASS_CONDITIONAL),
    StaticTableInstLib<tMethod>::MethodEntry("if-equ-x", &Hardware::Types::HeadsEX::Inst_IfEquX, INST_CLASS_CONDITIONAL),
    
    StaticTableInstLib<tMethod>::MethodEntry("if-cons", &Hardware::Types::HeadsEX::Inst_IfConsensus, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? in consensus, else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-cons-24", &Hardware::Types::HeadsEX::Inst_IfConsensus24, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX[0:23]? in consensus , else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-less-cons", &Hardware::Types::HeadsEX::Inst_IfLessConsensus, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if Count(?BX?) < Count(?CX?), else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-less-cons-24", &Hardware::Types::HeadsEX::Inst_IfLessConsensus24, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if Count(?BX[0:23]?) < Count(?CX[0:23]?), else skip it"),
    
    StaticTableInstLib<tMethod>::MethodEntry("if-stk-gtr", &Hardware::Types::HeadsEX::Inst_IfStackGreater, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if the top of the current stack > inactive stack, else skip it"),
    StaticTableInstLib<tMethod>::MethodEntry("if-nest", &Hardware::Types::HeadsEX::Inst_IfNest, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if the organism is on the nest/den, else skip it"),
    
    // Core ALU Operations
    StaticTableInstLib<tMethod>::MethodEntry("pop", &Hardware::Types::HeadsEX::Inst_Pop, INST_CLASS_DATA, 0, "Remove top number from stack and place into ?BX?"),
    StaticTableInstLib<tMethod>::MethodEntry("push", &Hardware::Types::HeadsEX::Inst_Push, INST_CLASS_DATA, 0, "Copy number from ?BX? and place it into the stack"),
    StaticTableInstLib<tMethod>::MethodEntry("pop-all", &Hardware::Types::HeadsEX::Inst_PopAll, INST_CLASS_DATA, 0, "Remove top numbers from stack and place into ?BX?"),
    StaticTableInstLib<tMethod>::MethodEntry("push-all", &Hardware::Types::HeadsEX::Inst_PushAll, INST_CLASS_DATA, 0, "Copy number from all registers and place into the stack"),
    StaticTableInstLib<tMethod>::MethodEntry("swap-stk", &Hardware::Types::HeadsEX::Inst_SwitchStack, INST_CLASS_DATA, 0, "Toggle which stack is currently being used"),
    StaticTableInstLib<tMethod>::MethodEntry("swap-stk-top", &Hardware::Types::HeadsEX::Inst_SwapStackTop, INST_CLASS_DATA, 0, "Swap the values at the top of both stacks"),
    StaticTableInstLib<tMethod>::MethodEntry("swap", &Hardware::Types::HeadsEX::Inst_Swap, INST_CLASS_DATA, 0, "Swap the contents of ?BX? with ?CX?"),
    
    StaticTableInstLib<tMethod>::MethodEntry("shift-r", &Hardware::Types::HeadsEX::Inst_ShiftR, INST_CLASS_ARITHMETIC_LOGIC, 0, "Shift bits in ?BX? right by one (divide by two)"),
    StaticTableInstLib<tMethod>::MethodEntry("shift-l", &Hardware::Types::HeadsEX::Inst_ShiftL, INST_CLASS_ARITHMETIC_LOGIC, 0, "Shift bits in ?BX? left by one (multiply by two)"),
    StaticTableInstLib<tMethod>::MethodEntry("inc", &Hardware::Types::HeadsEX::Inst_Inc, INST_CLASS_ARITHMETIC_LOGIC, 0, "Increment ?BX? by one"),
    StaticTableInstLib<tMethod>::MethodEntry("dec", &Hardware::Types::HeadsEX::Inst_Dec, INST_CLASS_ARITHMETIC_LOGIC, 0, "Decrement ?BX? by one"),
    StaticTableInstLib<tMethod>::MethodEntry("zero", &Hardware::Types::HeadsEX::Inst_Zero, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to 0"),
    StaticTableInstLib<tMethod>::MethodEntry("one", &Hardware::Types::HeadsEX::Inst_One, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to 1"),
    StaticTableInstLib<tMethod>::MethodEntry("rand", &Hardware::Types::HeadsEX::Inst_Rand, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to random number (without triggering IO"),
    StaticTableInstLib<tMethod>::MethodEntry("mult100", &Hardware::Types::HeadsEX::Inst_Mult100, INST_CLASS_ARITHMETIC_LOGIC, 0, "Mult ?BX? by 100"),
    
    StaticTableInstLib<tMethod>::MethodEntry("add", &Hardware::Types::HeadsEX::Inst_Add, INST_CLASS_ARITHMETIC_LOGIC, 0, "Add BX to CX and place the result in ?BX?"),
    StaticTableInstLib<tMethod>::MethodEntry("sub", &Hardware::Types::HeadsEX::Inst_Sub, INST_CLASS_ARITHMETIC_LOGIC, 0, "Subtract CX from BX and place the result in ?BX?"),
    StaticTableInstLib<tMethod>::MethodEntry("nand", &Hardware::Types::HeadsEX::Inst_Nand, INST_CLASS_ARITHMETIC_LOGIC, 0, "Nand BX by CX and place the result in ?BX?"),
    
    StaticTableInstLib<tMethod>::MethodEntry("IO", &Hardware::Types::HeadsEX::Inst_TaskIO, INST_CLASS_ENVIRONMENT, STALL, "Output ?BX?, and input new number back into ?BX?"),
    StaticTableInstLib<tMethod>::MethodEntry("input", &Hardware::Types::HeadsEX::Inst_TaskInput, INST_CLASS_ENVIRONMENT, STALL, "Input new number into ?BX?"),
    StaticTableInstLib<tMethod>::MethodEntry("output", &Hardware::Types::HeadsEX::Inst_TaskOutput, INST_CLASS_ENVIRONMENT, STALL, "Output ?BX?"),
    StaticTableInstLib<tMethod>::MethodEntry("output-zero", &Hardware::Types::HeadsEX::Inst_TaskOutputZero, INST_CLASS_ENVIRONMENT, STALL, "Output ?BX?"),
    
    StaticTableInstLib<tMethod>::MethodEntry("mult", &Hardware::Types::HeadsEX::Inst_Mult, INST_CLASS_ARITHMETIC_LOGIC, 0, "Multiple BX by CX and place the result in ?BX?"),
    StaticTableInstLib<tMethod>::MethodEntry("div", &Hardware::Types::HeadsEX::Inst_Div, INST_CLASS_ARITHMETIC_LOGIC, 0, "Divide BX by CX and place the result in ?BX?"),
    StaticTableInstLib<tMethod>::MethodEntry("mod", &Hardware::Types::HeadsEX::Inst_Mod, INST_CLASS_ARITHMETIC_LOGIC),
    
    
    // Flow Control Instructions
    StaticTableInstLib<tMethod>::MethodEntry("label", &Hardware::Types::HeadsEX::Inst_Label, INST_CLASS_FLOW_CONTROL, LABEL),
    
    StaticTableInstLib<tMethod>::MethodEntry("search-lbl-comp-s", &Hardware::Types::HeadsEX::Inst_Search_Label_Comp_S, INST_CLASS_FLOW_CONTROL, 0, "Find complement label from genome start and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-lbl-comp-f", &Hardware::Types::HeadsEX::Inst_Search_Label_Comp_F, INST_CLASS_FLOW_CONTROL, 0, "Find complement label forward and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-lbl-comp-b", &Hardware::Types::HeadsEX::Inst_Search_Label_Comp_B, INST_CLASS_FLOW_CONTROL, 0, "Find complement label backward and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-lbl-direct-s", &Hardware::Types::HeadsEX::Inst_Search_Label_Direct_S, INST_CLASS_FLOW_CONTROL, 0, "Find direct label from genome start and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-lbl-direct-f", &Hardware::Types::HeadsEX::Inst_Search_Label_Direct_F, INST_CLASS_FLOW_CONTROL, 0, "Find direct label forward and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-lbl-direct-b", &Hardware::Types::HeadsEX::Inst_Search_Label_Direct_B, INST_CLASS_FLOW_CONTROL, 0, "Find direct label backward and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-seq-comp-s", &Hardware::Types::HeadsEX::Inst_Search_Seq_Comp_S, INST_CLASS_FLOW_CONTROL, 0, "Find complement template from genome start and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-seq-comp-f", &Hardware::Types::HeadsEX::Inst_Search_Seq_Comp_F, INST_CLASS_FLOW_CONTROL, 0, "Find complement template forward and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-seq-comp-b", &Hardware::Types::HeadsEX::Inst_Search_Seq_Comp_B, INST_CLASS_FLOW_CONTROL, 0, "Find complement template backward and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-seq-direct-s", &Hardware::Types::HeadsEX::Inst_Search_Seq_Direct_S, INST_CLASS_FLOW_CONTROL, 0, "Find direct template from genome start and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-seq-direct-f", &Hardware::Types::HeadsEX::Inst_Search_Seq_Direct_F, INST_CLASS_FLOW_CONTROL, 0, "Find direct template forward and move the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("search-seq-direct-b", &Hardware::Types::HeadsEX::Inst_Search_Seq_Direct_B, INST_CLASS_FLOW_CONTROL, 0, "Find direct template backward and move the flow head"),
    
    StaticTableInstLib<tMethod>::MethodEntry("mov-head", &Hardware::Types::HeadsEX::Inst_MoveHead, INST_CLASS_FLOW_CONTROL, 0, "Move head ?IP? to the flow head"),
    StaticTableInstLib<tMethod>::MethodEntry("mov-head-if-n-equ", &Hardware::Types::HeadsEX::Inst_MoveHeadIfNEqu, INST_CLASS_FLOW_CONTROL, 0, "Move head ?IP? to the flow head if ?BX? != ?CX?"),
    StaticTableInstLib<tMethod>::MethodEntry("mov-head-if-less", &Hardware::Types::HeadsEX::Inst_MoveHeadIfLess, INST_CLASS_FLOW_CONTROL, 0, "Move head ?IP? to the flow head if ?BX? != ?CX?"),
    
    StaticTableInstLib<tMethod>::MethodEntry("goto", &Hardware::Types::HeadsEX::Inst_Goto, INST_CLASS_FLOW_CONTROL, 0, "Move IP to labeled position matching the label that follows"),
    StaticTableInstLib<tMethod>::MethodEntry("goto-if-n-equ", &Hardware::Types::HeadsEX::Inst_GotoIfNEqu, INST_CLASS_FLOW_CONTROL, 0, "Move IP to labeled position if BX != CX"),
    StaticTableInstLib<tMethod>::MethodEntry("goto-if-less", &Hardware::Types::HeadsEX::Inst_GotoIfLess, INST_CLASS_FLOW_CONTROL, 0, "Move IP to labeled position if BX < CX"),
    StaticTableInstLib<tMethod>::MethodEntry("goto-if-cons", &Hardware::Types::HeadsEX::Inst_GotoConsensus, INST_CLASS_FLOW_CONTROL, 0, "Move IP to the labeled position if BX consensus"),
    StaticTableInstLib<tMethod>::MethodEntry("goto-if-cons-24", &Hardware::Types::HeadsEX::Inst_GotoConsensus24, INST_CLASS_FLOW_CONTROL, 0, "Move IP to the labeled position if BX consensus"),
    
    StaticTableInstLib<tMethod>::MethodEntry("jmp-head", &Hardware::Types::HeadsEX::Inst_JumpHead, INST_CLASS_FLOW_CONTROL, 0, "Move head ?Flow? by amount in ?CX? register"),
    StaticTableInstLib<tMethod>::MethodEntry("get-head", &Hardware::Types::HeadsEX::Inst_GetHead, INST_CLASS_FLOW_CONTROL, 0, "Copy the position of the ?IP? head into ?CX?"),
    
    
    // Replication Instructions
    StaticTableInstLib<tMethod>::MethodEntry("h-alloc", &Hardware::Types::HeadsEX::Inst_HeadAlloc, INST_CLASS_LIFECYCLE, 0, "Allocate maximum allowed space"),
    StaticTableInstLib<tMethod>::MethodEntry("h-divide", &Hardware::Types::HeadsEX::Inst_HeadDivide, INST_CLASS_LIFECYCLE, STALL, "Divide code between read and write heads."),
    StaticTableInstLib<tMethod>::MethodEntry("h-divide-sex", &Hardware::Types::HeadsEX::Inst_HeadDivideSex, INST_CLASS_LIFECYCLE, STALL, "Divide code between read and write heads."),
    StaticTableInstLib<tMethod>::MethodEntry("h-copy", &Hardware::Types::HeadsEX::Inst_HeadCopy, INST_CLASS_LIFECYCLE, 0, "Copy from read-head to write-head; advance both"),
    StaticTableInstLib<tMethod>::MethodEntry("h-reqd", &Hardware::Types::HeadsEX::Inst_HeadRead, INST_CLASS_LIFECYCLE, 0, "Read instruction from ?read-head? to ?AX?; advance the head."),
    StaticTableInstLib<tMethod>::MethodEntry("h-write", &Hardware::Types::HeadsEX::Inst_HeadWrite, INST_CLASS_LIFECYCLE, 0, "Write to ?write-head? instruction from ?AX?; advance the head."),
    StaticTableInstLib<tMethod>::MethodEntry("if-copied-lbl-comp", &Hardware::Types::HeadsEX::Inst_IfCopiedCompLabel, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied complement of attached label"),
    StaticTableInstLib<tMethod>::MethodEntry("if-copied-lbl-direct", &Hardware::Types::HeadsEX::Inst_IfCopiedDirectLabel, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied direct match of the attached label"),
    StaticTableInstLib<tMethod>::MethodEntry("if-copied-seq-comp", &Hardware::Types::HeadsEX::Inst_IfCopiedCompSeq, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied complement of attached sequence"),
    StaticTableInstLib<tMethod>::MethodEntry("if-copied-seq-direct", &Hardware::Types::HeadsEX::Inst_IfCopiedDirectSeq, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied direct match of the attached sequence"),
    
    StaticTableInstLib<tMethod>::MethodEntry("repro", &Hardware::Types::HeadsEX::Inst_Repro, INST_CLASS_LIFECYCLE, STALL, "Instantly reproduces the organism"),
    
    StaticTableInstLib<tMethod>::MethodEntry("die", &Hardware::Types::HeadsEX::Inst_Die, INST_CLASS_LIFECYCLE, STALL, "Instantly kills the organism"),
    
    // Thread Execution Control
    StaticTableInstLib<tMethod>::MethodEntry("wait-cond-equ", &Hardware::Types::HeadsEX::Inst_WaitCondition_Equal, INST_CLASS_OTHER, STALL, ""),
    StaticTableInstLib<tMethod>::MethodEntry("wait-cond-less", &Hardware::Types::HeadsEX::Inst_WaitCondition_Less, INST_CLASS_OTHER, STALL, ""),
    StaticTableInstLib<tMethod>::MethodEntry("wait-cond-gtr", &Hardware::Types::HeadsEX::Inst_WaitCondition_Greater, INST_CLASS_OTHER, STALL, ""),
    
    // Movement and Navigation instructions
    StaticTableInstLib<tMethod>::MethodEntry("move", &Hardware::Types::HeadsEX::Inst_Move, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("juv-move", &Hardware::Types::HeadsEX::Inst_JuvMove, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("get-cell-xy", &Hardware::Types::HeadsEX::Inst_GetCellPosition, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("get-cell-x", &Hardware::Types::HeadsEX::Inst_GetCellPositionX, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("get-cell-y", &Hardware::Types::HeadsEX::Inst_GetCellPositionY, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("get-north-offset", &Hardware::Types::HeadsEX::Inst_GetNorthOffset, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("get-position-offset", &Hardware::Types::HeadsEX::Inst_GetPositionOffset, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("get-northerly", &Hardware::Types::HeadsEX::Inst_GetNortherly, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("get-easterly", &Hardware::Types::HeadsEX::Inst_GetEasterly, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("zero-easterly", &Hardware::Types::HeadsEX::Inst_ZeroEasterly, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("zero-northerly", &Hardware::Types::HeadsEX::Inst_ZeroNortherly, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("zero-position-offset", &Hardware::Types::HeadsEX::Inst_ZeroPosOffset, INST_CLASS_ENVIRONMENT),
    
    // Rotation
    StaticTableInstLib<tMethod>::MethodEntry("rotate-left-one", &Hardware::Types::HeadsEX::Inst_RotateLeftOne, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("rotate-right-one", &Hardware::Types::HeadsEX::Inst_RotateRightOne, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("rotate-uphill", &Hardware::Types::HeadsEX::Inst_RotateUphill, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("rotate-up-ft-hill", &Hardware::Types::HeadsEX::Inst_RotateUpFtHill, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("rotate-home", &Hardware::Types::HeadsEX::Inst_RotateHome, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("rotate-to-unoccupied-cell", &Hardware::Types::HeadsEX::Inst_RotateUnoccupiedCell, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("rotate-x", &Hardware::Types::HeadsEX::Inst_RotateX, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("rotate-org-id", &Hardware::Types::HeadsEX::Inst_RotateOrgID, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("rotate-away-org-id", &Hardware::Types::HeadsEX::Inst_RotateAwayOrgID, INST_CLASS_ENVIRONMENT, STALL),
    
    // Resource and Topography Sensing
    StaticTableInstLib<tMethod>::MethodEntry("sense-resource-id", &Hardware::Types::HeadsEX::Inst_SenseResourceID, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("sense-res-quant", &Hardware::Types::HeadsEX::Inst_SenseResQuant, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("sense-nest", &Hardware::Types::HeadsEX::Inst_SenseNest, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("sense-res-diff", &Hardware::Types::HeadsEX::Inst_SenseResDiff, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("sense-faced-habitat", &Hardware::Types::HeadsEX::Inst_SenseFacedHabitat, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("look-ahead", &Hardware::Types::HeadsEX::Inst_LookAhead, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("look-ahead-intercept", &Hardware::Types::HeadsEX::Inst_LookAheadIntercept, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("look-around", &Hardware::Types::HeadsEX::Inst_LookAround, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("look-around-intercept", &Hardware::Types::HeadsEX::Inst_LookAroundIntercept, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("look-ft", &Hardware::Types::HeadsEX::Inst_LookFT, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("look-around-ft", &Hardware::Types::HeadsEX::Inst_LookAroundFT, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("set-forage-target", &Hardware::Types::HeadsEX::Inst_SetForageTarget, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("set-ft-once", &Hardware::Types::HeadsEX::Inst_SetForageTargetOnce, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("set-rand-ft-once", &Hardware::Types::HeadsEX::Inst_SetRandForageTargetOnce, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("set-rand-p-ft-once", &Hardware::Types::HeadsEX::Inst_SetRandPFTOnce, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("get-forage-target", &Hardware::Types::HeadsEX::Inst_GetForageTarget, INST_CLASS_ENVIRONMENT),
    StaticTableInstLib<tMethod>::MethodEntry("show-ft", &Hardware::Types::HeadsEX::Inst_ShowForageTarget, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("get-loc-org-density", &Hardware::Types::HeadsEX::Inst_GetLocOrgDensity, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("get-faced-org-density", &Hardware::Types::HeadsEX::Inst_GetFacedOrgDensity, INST_CLASS_ENVIRONMENT, STALL),
    
    StaticTableInstLib<tMethod>::MethodEntry("collect-edible", &Hardware::Types::HeadsEX::Inst_CollectEdible, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("collect-specific", &Hardware::Types::HeadsEX::Inst_CollectSpecific, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("deposit-resource", &Hardware::Types::HeadsEX::Inst_DepositResource, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("deposit-specific", &Hardware::Types::HeadsEX::Inst_DepositSpecific, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("deposit-all-as-specific", &Hardware::Types::HeadsEX::Inst_DepositAllAsSpecific, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("nop-deposit-specific", &Hardware::Types::HeadsEX::Inst_NopDepositSpecific, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("nop-deposit-resource", &Hardware::Types::HeadsEX::Inst_NopDepositResource, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("nop-deposit-all-as-specific", &Hardware::Types::HeadsEX::Inst_NopDepositAllAsSpecific, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("nop2-deposit-all-as-specific", &Hardware::Types::HeadsEX::Inst_Nop2DepositAllAsSpecific, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("nop-collect-edible", &Hardware::Types::HeadsEX::Inst_NopCollectEdible, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("nop2-collect-edible", &Hardware::Types::HeadsEX::Inst_Nop2CollectEdible, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("get-res-stored", &Hardware::Types::HeadsEX::Inst_GetResStored, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("get-specific-stored", &Hardware::Types::HeadsEX::Inst_GetSpecificStored, INST_CLASS_ENVIRONMENT, STALL),
    
    // Org Interaction instructions
    StaticTableInstLib<tMethod>::MethodEntry("get-faced-org-id", &Hardware::Types::HeadsEX::Inst_GetFacedOrgID, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-prey", &Hardware::Types::HeadsEX::Inst_AttackPrey, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-ft-prey", &Hardware::Types::HeadsEX::Inst_AttackFTPrey, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-poison-prey", &Hardware::Types::HeadsEX::Inst_AttackPoisonPrey, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-poison-ft-prey", &Hardware::Types::HeadsEX::Inst_AttackPoisonFTPrey, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-poison-ft-prey-genetic", &Hardware::Types::HeadsEX::Inst_AttackPoisonFTPreyGenetic, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-poison-ft-mixed-prey", &Hardware::Types::HeadsEX::Inst_AttackPoisonFTMixedPrey, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-prey-share", &Hardware::Types::HeadsEX::Inst_AttackPreyShare, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-prey-no-share", &Hardware::Types::HeadsEX::Inst_AttackPreyNoShare, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-prey-fake-share", &Hardware::Types::HeadsEX::Inst_AttackPreyFakeShare, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-spec-prey", &Hardware::Types::HeadsEX::Inst_AttackSpecPrey, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-prey-area", &Hardware::Types::HeadsEX::Inst_AttackPreyArea, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("fight-merit-org", &Hardware::Types::HeadsEX::Inst_FightMeritOrg, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("fight-bonus-org", &Hardware::Types::HeadsEX::Inst_FightBonusOrg, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("get-merit-fight-odds", &Hardware::Types::HeadsEX::Inst_GetMeritFightOdds, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("fight-org", &Hardware::Types::HeadsEX::Inst_FightOrg, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("attack-pred", &Hardware::Types::HeadsEX::Inst_AttackPred, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("kill-pred", &Hardware::Types::HeadsEX::Inst_KillPred, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("fight-pred", &Hardware::Types::HeadsEX::Inst_FightPred, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("teach-offspring", &Hardware::Types::HeadsEX::Inst_TeachOffspring, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("learn-parent", &Hardware::Types::HeadsEX::Inst_LearnParent, INST_CLASS_ENVIRONMENT, STALL),
    
    
    StaticTableInstLib<tMethod>::MethodEntry("set-guard", &Hardware::Types::HeadsEX::Inst_SetGuard, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("set-guard-once", &Hardware::Types::HeadsEX::Inst_SetGuardOnce, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("get-num-guards", &Hardware::Types::HeadsEX::Inst_GetNumGuards, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("get-num-juvs", &Hardware::Types::HeadsEX::Inst_GetNumJuvs, INST_CLASS_ENVIRONMENT, STALL),
    
    StaticTableInstLib<tMethod>::MethodEntry("activate-display", &Hardware::Types::HeadsEX::Inst_ActivateDisplay, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("update-display", &Hardware::Types::HeadsEX::Inst_UpdateDisplay, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("modify-display", &Hardware::Types::HeadsEX::Inst_ModifyDisplay, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("read-last-seen-display", &Hardware::Types::HeadsEX::Inst_ReadLastSeenDisplay, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("kill-display", &Hardware::Types::HeadsEX::Inst_KillDisplay, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("modify-simp-display", &Hardware::Types::HeadsEX::Inst_ModifySimpDisplay, INST_CLASS_ENVIRONMENT, STALL),
    StaticTableInstLib<tMethod>::MethodEntry("read-simp-display", &Hardware::Types::HeadsEX::Inst_ReadLastSimpDisplay, INST_CLASS_ENVIRONMENT, STALL),
    
    // Control-type Instructions
    StaticTableInstLib<tMethod>::MethodEntry("scramble-registers", &Hardware::Types::HeadsEX::Inst_ScrambleReg, INST_CLASS_DATA, STALL),
    
    StaticTableInstLib<tMethod>::MethodEntry("get-faced-edit-dist", &Hardware::Types::HeadsEX::Inst_GetFacedEditDistance, INST_CLASS_ENVIRONMENT, STALL),
    
    // DEPRECATED Instructions
    StaticTableInstLib<tMethod>::MethodEntry("set-flow", &Hardware::Types::HeadsEX::Inst_SetFlow, INST_CLASS_FLOW_CONTROL, 0, "Set flow-head to position in ?CX?")
    
  };
  
  
  const int n_size = sizeof(s_n_array)/sizeof(NOPEntry);
  
  static int nop_mods[n_size];
  for (int i = 0; i < n_size && i < NUM_REGISTERS; i++) {
    nop_mods[i] = s_n_array[i].nop_mod;
  }
  
  const int f_size = sizeof(s_f_array)/sizeof(StaticTableInstLib<tMethod>::MethodEntry);
  static tMethod functions[f_size];
  for (int i = 0; i < f_size; i++) functions[i] = s_f_array[i].GetFunction();
  
  const int def = 0;
  const int null_inst = 16;
  
  return new StaticTableInstLib<tMethod>(f_size, s_f_array, nop_mods, functions, def, null_inst);
}

Hardware::Types::HeadsEX::cHardwareExperimental(Context& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set)
: cHardwareBase(world, in_organism, in_inst_set), m_sensor(world, in_organism)
{
  m_functions = s_inst_slib->GetFunctions();
  
  m_spec_die = false;
  
  m_thread_slicing_parallel = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1);
  m_no_cpu_cycle_time = m_world->GetConfig().NO_CPU_CYCLE_TIME.Get();
  
  m_slip_read_head = !m_world->GetConfig().SLIP_COPY_MODE.Get();
  
  const Genome& in_genome = in_organism->GetGenome();
  ConstInstructionSequencePtr in_seq_p;
  in_seq_p.DynamicCastFrom(in_genome.Representation());
  const InstructionSequence& in_seq = *in_seq_p;
  
  m_memory = in_seq;  // Initialize memory...
  m_use_avatar = m_world->GetConfig().USE_AVATARS.Get();
  Reset(ctx);                            // Setup the rest of the hardware...
}


void Hardware::Types::HeadsEX::internalReset()
{
  m_cycle_count = 0;
  m_last_output = 0;
  m_global_stack.Clear(m_inst_set->GetStackSize());
  
  // We want to reset to have a single thread.
  m_threads.Resize(1);
  
  // Reset that single thread.
  m_threads[0].Reset(this, 0);
  m_threads[0].active = true;
  m_thread_id_chart = 1; // Mark only the first thread as taken...
  m_cur_thread = 0;
  m_waiting_threads = 0;
  
  m_mal_active = false;
  m_executedmatchstrings = false;
  
  m_use_avatar = m_world->GetConfig().USE_AVATARS.Get();
  m_sensor.Reset();
}


void Hardware::Types::HeadsEX::internalResetOnFailedDivide()
{
	internalReset();
}

void Hardware::Types::HeadsEX::cLocalThread::operator=(const cLocalThread& in_thread)
{
  m_id = in_thread.m_id;
  
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i] = in_thread.reg[i];
  for (int i = 0; i < NUM_HEADS; i++) heads[i] = in_thread.heads[i];
  stack = in_thread.stack;
  cur_stack = in_thread.cur_stack;
  cur_head = in_thread.cur_head;
  reading_label = in_thread.reading_label;
  reading_seq = in_thread.reading_seq;
  active = in_thread.active;
  wait_greater = in_thread.wait_greater;
  wait_equal = in_thread.wait_equal;
  wait_less = in_thread.wait_less;
  wait_reg = in_thread.wait_reg;
  wait_dst = in_thread.wait_dst;
  wait_value = in_thread.wait_value;
  
  read_label = in_thread.read_label;
  read_seq = in_thread.read_seq;
  next_label = in_thread.next_label;
}

void Hardware::Types::HeadsEX::cLocalThread::Reset(cHardwareExperimental* in_hardware, int in_id)
{
  m_id = in_id;
  
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i].Clear();
  for (int i = 0; i < NUM_HEADS; i++) heads[i].Reset(in_hardware);
  
  stack.Clear(in_hardware->GetInstSet().GetStackSize());
  cur_stack = 0;
  cur_head = HEAD_IP;
  
  reading_label = false;
  reading_seq = false;
  active = true;
  read_label.Clear();
  next_label.Clear();
  
}


// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.

bool Hardware::Types::HeadsEX::SingleProcess(Context& ctx, bool speculative)
{
  assert(!speculative || (speculative && !m_thread_slicing_parallel));
  
  // Mark this organism as running...
  m_organism->SetRunning(true);
  
  if (!speculative && m_spec_die) {
    m_organism->Die(ctx);
    m_organism->SetRunning(false);
    return false;
  }
  
  cPhenotype& phenotype = m_organism->GetPhenotype();
  
  m_cycle_count++;
  assert(m_cycle_count < 0x8000);
  phenotype.IncCPUCyclesUsed();
  if (!m_no_cpu_cycle_time) phenotype.IncTimeUsed();
  
  // If we have threads turned on and we executed each thread in a single
  // timestep, adjust the number of instructions executed accordingly.
  const int num_inst_exec = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1) ? m_threads.GetSize() : 1;
  
  int num_active = 0;
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (m_threads[i].active) num_active++;
  }
  assert(num_active == (m_threads.GetSize() - m_waiting_threads));
  
  for (int i = 0; i < num_inst_exec; i++) {
    // Setup the hardware for the next instruction to be executed.
    int last_thread = m_cur_thread++;
    if (m_cur_thread >= m_threads.GetSize()) m_cur_thread = 0;
    
    // If the currently selected thread is inactive, proceed to the next thread
    if (!m_threads[m_cur_thread].active) {
      if (num_inst_exec == 1) i--;  // When running in non-parallel mode, adjust i so that we will continue to loop
      continue;
    }
    
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
    
    // Print the short form status of this CPU at each step...
    if (m_tracer) m_tracer->TraceHardware(ctx, *this, false, true);
    
    // Test if costs have been paid and it is okay to execute this now...
    bool exec = true;
    int exec_success = 0;
    
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
  } // Previous was executed once for each thread...
  
  // Kill creatures who have reached their max num of instructions executed
  const int max_executed = m_organism->GetMaxExecuted();
  if ((max_executed > 0 && phenotype.GetTimeUsed() >= max_executed) || phenotype.GetToDie() == true) {
    if (speculative) m_spec_die = true;
    else m_organism->Die(ctx);
  }
  if (!speculative && phenotype.GetToDelete()) m_spec_die = true;
  
  m_organism->SetRunning(false);
  CheckImplicitRepro(ctx);
  
  return !m_spec_die;
}

// This method will handle the actuall execution of an instruction
// within single process, once that function has been finalized.
bool Hardware::Types::HeadsEX::SingleProcess_ExecuteInst(Context& ctx, const Instruction& cur_inst)
{
  // Copy Instruction locally to handle stochastic effects
  Instruction actual_inst = cur_inst;
  
  // Get a pointer to the corrisponding method...
  int inst_idx = m_inst_set->GetLibFunctionIndex(actual_inst);
  
  // Mark the instruction as executed
  getIP().SetFlagExecuted();
	
  // instruction execution count incremeneted
  m_organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
  
  // And execute it.
  m_from_sensor = false;
  const bool exec_success = (this->*(m_functions[inst_idx]))(ctx);
  
	if (exec_success) {
    if (m_from_sensor) m_organism->GetPhenotype().IncCurFromSensorInstCount(actual_inst.GetOp());
  }
  
  // decremenet if the instruction was not executed successfully
  if (exec_success == false) m_organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  return exec_success;
}


void Hardware::Types::HeadsEX::ProcessBonusInst(Context& ctx, const Instruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = m_organism->IsRunning();
  m_organism->SetRunning(true);
  
  if (m_tracer) m_tracer->TraceHardware(ctx, *this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  m_organism->SetRunning(prev_run_state);
}


void Hardware::Types::HeadsEX::PrintStatus(ostream& fp)
{
  fp << "CPU CYCLE:" << m_organism->GetPhenotype().GetCPUCyclesUsed() << " ";
  fp << "THREAD:" << m_cur_thread << "  ";
  fp << "IP:" << getIP().GetPosition() << " (" << GetInstSet().GetName(IP().GetInst()) << ")" << endl;
  
  
  for (int i = 0; i < NUM_REGISTERS; i++) {
    DataValue& reg = m_threads[m_cur_thread].reg[i];
    fp << static_cast<char>('A' + i) << "X:" << GetRegister(i) << " ";
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
  
  fp << "  R-Head:" << getHead(HEAD_READ).GetPosition() << " "
  << "W-Head:" << getHead(HEAD_WRITE).GetPosition()  << " "
  << "F-Head:" << getHead(HEAD_FLOW).GetPosition()   << "  "
  << "RL:" << GetReadLabel().AsString() << "   "
  << "Ex:" << m_last_output
  << endl;
  
  int number_of_stacks = GetNumStacks();
  for (int stack_id = 0; stack_id < number_of_stacks; stack_id++) {
    fp << ((m_threads[m_cur_thread].cur_stack == stack_id) ? '*' : ' ') << " Stack " << stack_id << ":" << setbase(16) << setfill('0');
    for (int i = 0; i < GetInstSet().GetStackSize(); i++) fp << " Ox" << setw(8) << GetStack(i, stack_id, 0);
    fp << setfill(' ') << setbase(10) << endl;
  }
  
  fp << "  Mem (" << m_memory.GetSize() << "):"
  << "  " << m_memory.AsString()
  << endl;
  
  
  fp.flush();
}


cHeadCPU Hardware::Types::HeadsEX::FindLabelStart(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  InstMemSpace& memory = m_memory;
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
        return cHeadCPU(this, pos - 1, ip.GetMemSpace());
      }
      
      continue;
    }
    pos++;
  }
  
  // Return start point if not found
  return ip;
}

cHeadCPU Hardware::Types::HeadsEX::FindNopSequenceStart(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  InstMemSpace& memory = m_memory;
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
        return cHeadCPU(this, pos - 1, ip.GetMemSpace());
      }
    }
    pos++;
  }
  
  // Return start point if not found
  return ip;
}


cHeadCPU Hardware::Types::HeadsEX::FindLabelForward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cHeadCPU pos(ip);
  pos++;
  
  while (pos.GetPosition() != ip.GetPosition()) {
    if (m_inst_set->IsLabel(pos.GetInst())) { // starting label found
      const int label_start = pos.GetPosition();
      pos++;
      
      // Check for direct matched label pattern, can be substring of 'label'ed target
      // - must match all NOPs in search_label
      // - extra NOPs in 'label'ed target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.GetPosition() != ip.GetPosition()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops following the 'label' instruction
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.GetPosition();
        
        if (mark_executed) {
          pos.Set(label_start);
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get() + 1; // Max label + 1 for the label instruction itself
          for (int i = 0; i < size_matched && i < max; i++, pos++) pos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        return cHeadCPU(this, found_pos, ip.GetMemSpace());
      }
      
      continue;
    }
    pos++;
  }
  
  // Return start point if not found
  return ip;
}

cHeadCPU Hardware::Types::HeadsEX::FindLabelBackward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cHeadCPU lpos(ip);
  cHeadCPU pos(ip);
  lpos--;
  
  while (pos.GetPosition() != ip.GetPosition()) {
    if (m_inst_set->IsLabel(lpos.GetInst())) { // starting label found
      pos.Set(lpos.GetPosition());
      pos++;
      
      // Check for direct matched label pattern, can be substring of 'label'ed target
      // - must match all NOPs in search_label
      // - extra NOPs in 'label'ed target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.GetPosition() != ip.GetPosition()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops following the 'label' instruction
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.GetPosition();
        
        if (mark_executed) {
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get() + 1; // Max label + 1 for the label instruction itself
          for (int i = 0; i < size_matched && i < max; i++, lpos++) lpos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        return cHeadCPU(this, found_pos, ip.GetMemSpace());
      }
    }
    lpos--;
  }
  
  // Return start point if not found
  return ip;
}




cHeadCPU Hardware::Types::HeadsEX::FindNopSequenceForward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cHeadCPU pos(ip);
  pos++;
  
  while (pos.GetPosition() != ip.GetPosition()) {
    if (m_inst_set->IsNop(pos.GetInst())) { // starting label found
      const int label_start = pos.GetPosition();
      
      // Check for direct matched nop sequence, can be substring of target
      // - must match all NOPs in search_label
      // - extra NOPs in target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.GetPosition() != ip.GetPosition()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.GetPosition();
        
        if (mark_executed) {
          pos.Set(label_start);
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get();
          for (int i = 0; i < size_matched && i < max; i++, pos++) pos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        return cHeadCPU(this, found_pos, ip.GetMemSpace());
      }
    }
    
    if (pos.GetPosition() == ip.GetPosition()) break;
    pos++;
  }
  
  // Return start point if not found
  return ip;
}


cHeadCPU Hardware::Types::HeadsEX::FindNopSequenceBackward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const NopSequence& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cHeadCPU lpos(ip);
  cHeadCPU pos(ip);
  lpos--;
  
  while (pos.GetPosition() != ip.GetPosition()) {
    if (m_inst_set->IsNop(pos.GetInst())) { // starting label found
      pos.Set(lpos.GetPosition());
      
      // Check for direct matched nop sequence, can be substring of target
      // - must match all NOPs in search_label
      // - extra NOPs in target are ignored
      int size_matched = 0;
      while (size_matched < search_label.GetSize() && pos.GetPosition() != ip.GetPosition()) {
        if (!m_inst_set->IsNop(pos.GetInst()) || search_label[size_matched] != m_inst_set->GetNopMod(pos.GetInst())) break;
        size_matched++;
        pos++;
      }
      
      // Check that the label matches and has examined the full sequence of nops
      if (size_matched == search_label.GetSize()) {
        pos--;
        const int found_pos = pos.GetPosition();
        
        if (mark_executed) {
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get();
          for (int i = 0; i < size_matched && i < max; i++, lpos++) lpos.SetFlagExecuted();
        }
        
        // Return Head pointed at last NOP of label sequence
        return cHeadCPU(this, found_pos, ip.GetMemSpace());
      }
      
      continue;
    }
    lpos--;
  }
  
  // Return start point if not found
  return ip;
}

void Hardware::Types::HeadsEX::ReadInst(Instruction in_inst)
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

void Hardware::Types::HeadsEX::AdjustHeads()
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

void Hardware::Types::HeadsEX::ReadLabel()
{
  int count = 0;
  cHeadCPU * inst_ptr = &( getIP() );
  
  GetLabel().Clear();
  
  while (m_inst_set->IsNop(inst_ptr->GetNextInst()) && (count < GetLabel().MaxSize())) {
    count++;
    inst_ptr->Advance();
    GetLabel().AddNop(m_inst_set->GetNopMod(inst_ptr->GetInst()));
    
    // If this is the first line of the template, mark it executed.
    if (GetLabel().GetSize() <=	m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get()) {
      inst_ptr->SetFlagExecuted();
    }
  }
}

bool Hardware::Types::HeadsEX::ForkThread()
{
  const int num_threads = m_threads.GetSize();
  if (num_threads == m_world->GetConfig().MAX_CPU_THREADS.Get()) return false;
  
  // Make room for the new thread.
  m_threads.Resize(num_threads + 1);
  
  // Initialize the new thread to the same values as the current one.
  assert(m_threads[m_cur_thread].active);
  m_threads[num_threads] = m_threads[m_cur_thread];
  
  // Find the first free bit in m_thread_id_chart to determine the new
  // thread id.
  int new_id = 0;
  while ((m_thread_id_chart >> new_id) & 1) new_id++;
  m_threads[num_threads].SetID(new_id);
  m_thread_id_chart |= (1 << new_id);
  
  return true;
}

bool Hardware::Types::HeadsEX::ThreadCreate(const cHeadCPU& start_pos)
{
  const int thread_id = m_threads.GetSize();
  if (thread_id == m_world->GetConfig().MAX_CPU_THREADS.Get()) return false;
  
  // Make room for the new thread.
  m_threads.Resize(thread_id + 1);
  
  // Find the first free bit in m_thread_id_chart to determine the new
  // thread id.
  int new_id = 0;
  while ((m_thread_id_chart >> new_id) & 1) new_id++;
  m_thread_id_chart |= (1 << new_id);
  
  m_threads[thread_id].Reset(this, new_id);
  m_threads[thread_id].heads[HEAD_IP] = start_pos;
  
  return true;
}


bool Hardware::Types::HeadsEX::ExitThread()
{
  // Make sure that there is always at least one thread awake...
  if ((m_threads.GetSize() == 1) || (int(m_waiting_threads) == (m_threads.GetSize() - 1))) return false;
  
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

bool Hardware::Types::HeadsEX::Inst_IdThread(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].GetID(), false);
  return true;
}

////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int Hardware::Types::HeadsEX::FindModifiedRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_register = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_register;
}

inline int Hardware::Types::HeadsEX::FindModifiedNextRegister(int default_register)
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

inline int Hardware::Types::HeadsEX::FindModifiedPreviousRegister(int default_register)
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


inline int Hardware::Types::HeadsEX::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_head = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_head;
}


inline int Hardware::Types::HeadsEX::FindNextRegister(int base_reg)
{
  return (base_reg + 1) % NUM_REGISTERS;
}


bool Hardware::Types::HeadsEX::Allocate_Necro(const int new_size)
{
  m_memory.ResizeOld(new_size);
  return true;
}

bool Hardware::Types::HeadsEX::Allocate_Random(Context& ctx, const int old_size, const int new_size)
{
  m_memory.Resize(new_size);
  
  for (int i = old_size; i < new_size; i++) {
    m_memory[i] = m_inst_set->GetRandomInst(ctx);
  }
  return true;
}

bool Hardware::Types::HeadsEX::Allocate_Default(const int new_size)
{
  m_memory.Resize(new_size);
  
  // New space already defaults to default instruction...
  
  return true;
}

bool Hardware::Types::HeadsEX::Allocate_Main(Context& ctx, const int allocated_size)
{
  // must do divide before second allocate & must allocate positive amount...
  if (m_world->GetConfig().REQUIRE_ALLOCATE.Get() && m_mal_active == true) return false;
  if (allocated_size < 1) return false;
  
  const int old_size = m_memory.GetSize();
  const int new_size = old_size + allocated_size;
  
  // Make sure that the new size is in range.
  if (new_size > MAX_GENOME_LENGTH  ||  new_size < MIN_GENOME_LENGTH) return false;
  
  const int max_alloc_size = (int) (old_size * m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get());
  if (allocated_size > max_alloc_size)  return false;
  
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

int Hardware::Types::HeadsEX::calcCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  for (int i = parent_size; i < parent_size + child_size; i++) {
    if (m_memory.FlagCopied(i)) copied_size++;
  }
  return copied_size;
}

bool Hardware::Types::HeadsEX::Divide_Main(Context& ctx, const int div_point, const int extra_lines, double mut_multiplier)
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
  
  m_organism->OffspringGenome() = offspring;
  
  // Cut off everything in this memory past the divide point.
  m_memory.Resize(div_point);
  
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
  
  m_mal_active = false;
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

void Hardware::Types::HeadsEX::checkWaitingThreads(int cur_thread, int reg_num)
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

//////////////////////////
// And the instructions...
//////////////////////////

bool Hardware::Types::HeadsEX::Inst_Nop(Context& ctx)
{
  (void)ctx;
  return true;
}



// Multi-threading.
bool Hardware::Types::HeadsEX::Inst_ForkThread(Context&)
{
  getIP().Advance();
  ForkThread();
  return true;
}

// Multi-threading.
bool Hardware::Types::HeadsEX::Inst_ThreadCreate(Context&)
{
  int head_used = FindModifiedHead(HEAD_FLOW);
  
  bool success = ThreadCreate(m_threads[m_cur_thread].heads[head_used]);
  
  if (success) m_organism->GetPhenotype().SetIsMultiThread();
  
  return success;
}


bool Hardware::Types::HeadsEX::Inst_ExitThread(Context& ctx)
{
  if (ExitThread()) m_advance_ip = false;
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfNEqu(Context&) // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) == GetRegister(op2)) getIP().Advance();
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfLess(Context&) // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) >= GetRegister(op2)) getIP().Advance();
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfNotZero(Context&)  // Execute next if ?bx? != 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) == 0) getIP().Advance();
  m_from_sensor = FromSensor(op1);
  return true;
}
bool Hardware::Types::HeadsEX::Inst_IfEqualZero(Context&)  // Execute next if ?bx? == 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) != 0) getIP().Advance();
  m_from_sensor = FromSensor(op1);
  return true;
}
bool Hardware::Types::HeadsEX::Inst_IfGreaterThanZero(Context&)  // Execute next if ?bx? > 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) <= 0) getIP().Advance();
  m_from_sensor = FromSensor(op1);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfLessThanZero(Context&)  // Execute next if ?bx? < 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) >= 0) getIP().Advance();
  m_from_sensor = FromSensor(op1);
  return true;
}


bool Hardware::Types::HeadsEX::Inst_IfGtrX(Context&)       // Execute next if BX > X; X value set according to NOP label
{
  // Compares value in BX to a specific value.  The value to compare to is determined by the nop label as follows:
  //    no nop label (default): valueToCompare = 1;
  //    nop-A: toggles valueToCompare sign-bit
  //    nop-B: valueToCompare left-shift by 1-bit
  //    nop-C: valueToCompare left-shift by 2-bits
  //    nop-D: valueToCompare left-shift by 3-bits, etc.
  
  int valueToCompare = 1;
  
  ReadLabel();
  const NopSequence& shift_label = GetLabel();
  for (int i = 0; i < shift_label.GetSize(); i++) {
    if (shift_label[i] == rAX) {
      valueToCompare *= -1;
    } else {
      valueToCompare <<= shift_label[i];
    }
  }
  
  if (GetRegister(rBX) <= valueToCompare) getIP().Advance();
  m_from_sensor = FromSensor(rBX);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfEquX(Context&)       // Execute next if BX == X; X value set according to NOP label
{
  // Compares value in BX to a specific value.  The value to compare to is determined by the nop label as follows:
  //    no nop label (default): valueToCompare = 1;
  //    nop-A: toggles valueToCompare sign-bit
  //    nop-B: valueToCompare left-shift by 1-bit
  //    nop-C: valueToCompare left-shift by 2-bits
  //    nop-D: valueToCompare left-shift by 3-bits, etc.
  
  int valueToCompare = 1;
  
  ReadLabel();
  const NopSequence& shift_label = GetLabel();
  for (int i = 0; i < shift_label.GetSize(); i++) {
    if (shift_label[i] == rAX) {
      valueToCompare *= -1;
    } else {
      valueToCompare <<= shift_label[i];
    }
  }
  
  if (GetRegister(rBX) != valueToCompare) getIP().Advance();
  m_from_sensor = FromSensor(rBX);
  return true;
}



bool Hardware::Types::HeadsEX::Inst_IfConsensus(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  if (BitCount(GetRegister(op1)) <  CONSENSUS) getIP().Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfConsensus24(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  if (BitCount(GetRegister(op1) & MASK_LOW24) <  CONSENSUS24) getIP().Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfLessConsensus(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1)) >=  BitCount(GetRegister(op2))) getIP().Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfLessConsensus24(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1) & MASK_LOW24) >=  BitCount(GetRegister(op2) & MASK_LOW24)) getIP().Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfStackGreater(Context&)
{
  int cur_stack = m_threads[m_cur_thread].cur_stack;
  if (getStack(cur_stack).Peek().value <=  getStack(!cur_stack).Peek().value) getIP().Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfNest(Context& ctx) // Execute next if org on nest
{
  bool set_ok = false;
  
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 3 || resource_lib.GetResource(i)->GetHabitat() == 4) {
      double cell_res = 0.0;
      if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
      else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
      if (cell_res > 0) set_ok = true;
    }
  }
  
  if (set_ok) getIP().Advance();
  return true;
}


bool Hardware::Types::HeadsEX::Inst_Label(Context&)
{
  ReadLabel();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Pop(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  DataValue pop = stackPop();
  setInternalValue(reg_used, pop.value, pop);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Push(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  getStack(m_threads[m_cur_thread].cur_stack).Push(m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_PopAll(Context&)
{
  int reg_used = FindModifiedRegister(rBX);
  bool any_from_sensor = false;
  for (int i = 0; i < NUM_REGISTERS; i++) {
    if (FromSensor(reg_used)) any_from_sensor = true;
    DataValue pop = stackPop();
    setInternalValue(reg_used, pop.value, pop);
    reg_used++;
    if (reg_used == NUM_REGISTERS) reg_used = 0;
  }
  m_from_sensor = any_from_sensor;
  return true;
}

bool Hardware::Types::HeadsEX::Inst_PushAll(Context&)
{
  int reg_used = FindModifiedRegister(rBX);
  bool any_from_sensor = false;
  for (int i = 0; i < NUM_REGISTERS; i++) {
    if (FromSensor(reg_used)) any_from_sensor = true;
    getStack(m_threads[m_cur_thread].cur_stack).Push(m_threads[m_cur_thread].reg[reg_used]);
    reg_used++;
    if (reg_used == NUM_REGISTERS) reg_used = 0;
  }
  m_from_sensor = any_from_sensor;
  return true;
}

bool Hardware::Types::HeadsEX::Inst_SwitchStack(Context&) { switchStack(); return true; }

bool Hardware::Types::HeadsEX::Inst_SwapStackTop(Context&)
{
  DataValue v0 = getStack(0).Pop();
  DataValue v1 = getStack(1).Pop();
  getStack(0).Push(v1);
  getStack(1).Push(v0);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Swap(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  DataValue v1 = m_threads[m_cur_thread].reg[op1];
  m_threads[m_cur_thread].reg[op1] = m_threads[m_cur_thread].reg[op2];
  m_threads[m_cur_thread].reg[op2] = v1;
  return true;
}

bool Hardware::Types::HeadsEX::Inst_ShiftR(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value >> 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_ShiftL(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value << 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}


bool Hardware::Types::HeadsEX::Inst_Inc(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value + 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Dec(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value - 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Zero(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, 0, false);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_One(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, 1, false);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Rand(Context& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  int randsign = ctx.GetRandom().GetUInt(0,2) ? -1 : 1;
  setInternalValue(reg_used, ctx.GetRandom().GetInt(INT_MAX) * randsign, false);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Mult100(Context& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value * 100, false);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Add(Context&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  DataValue& r1 = m_threads[m_cur_thread].reg[op1];
  DataValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value + r2.value, r1, r2);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Sub(Context&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  DataValue& r1 = m_threads[m_cur_thread].reg[op1];
  DataValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value - r2.value, r1, r2);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Mult(Context&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  DataValue& r1 = m_threads[m_cur_thread].reg[op1];
  DataValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value * r2.value, r1, r2);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Div(Context&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  DataValue& r1 = m_threads[m_cur_thread].reg[op1];
  DataValue& r2 = m_threads[m_cur_thread].reg[op2];
  if (r2.value != 0) {
    if (!(0 - INT_MAX > r1.value && r2.value == -1)) setInternalValue(dst, r1.value / r2.value, r1, r2);
  } else {
    return false;
  }
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Mod(Context&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  DataValue& r1 = m_threads[m_cur_thread].reg[op1];
  DataValue& r2 = m_threads[m_cur_thread].reg[op2];
  if (r2.value != 0) {
    setInternalValue(dst, r1.value % r2.value, r1, r2);
  } else {
    return false;
  }
  return true;
}


bool Hardware::Types::HeadsEX::Inst_Nand(Context&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  DataValue& r1 = m_threads[m_cur_thread].reg[op1];
  DataValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, ~(r1.value & r2.value), r1, r2);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_HeadAlloc(Context& ctx)   // Allocate maximal more
{
  const int dst = FindModifiedRegister(rAX);
  const int cur_size = m_memory.GetSize();
  const int alloc_size = Apto::Min((int)(m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get() * cur_size), MAX_GENOME_LENGTH - cur_size);
  if (Allocate_Main(ctx, alloc_size)) {
    setInternalValue(dst, cur_size);
    return true;
  } else return false;
}

bool Hardware::Types::HeadsEX::Inst_TaskIO(Context& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  DataValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setInternalValue(reg_used, value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_TaskInput(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setInternalValue(reg_used, value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_TaskOutput(Context& ctx)
{
  //Testing with juveniles not able to move HACK
  //  if (m_organism->GetPhenotype().GetTimeUsed() < m_world->GetConfig().JUV_PERIOD.Get()) return false;
  
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  DataValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_TaskOutputZero(Context& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  DataValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  setInternalValue(reg_used, 0);
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_MoveHead(Context&)
{
  const int head_used = FindModifiedHead(HEAD_IP);
  const int target = FindModifiedHead(HEAD_FLOW);
  getHead(head_used).Set(getHead(target));
  if (head_used == HEAD_IP) m_advance_ip = false;
  return true;
}

bool Hardware::Types::HeadsEX::Inst_MoveHeadIfNEqu(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  const int head_used = FindModifiedHead(HEAD_IP);
  const int target = FindModifiedHead(HEAD_FLOW);
  if (m_threads[m_cur_thread].reg[op1].value != m_threads[m_cur_thread].reg[op2].value) {
    getHead(head_used).Set(getHead(target));
    if (head_used == HEAD_IP) m_advance_ip = false;
  }
  return true;
}

bool Hardware::Types::HeadsEX::Inst_MoveHeadIfLess(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  const int head_used = FindModifiedHead(HEAD_IP);
  const int target = FindModifiedHead(HEAD_FLOW);
  if (m_threads[m_cur_thread].reg[op1].value < m_threads[m_cur_thread].reg[op2].value) {
    getHead(head_used).Set(getHead(target));
    if (head_used == HEAD_IP) m_advance_ip = false;
  }
  return true;
}


bool Hardware::Types::HeadsEX::Inst_Goto(Context&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GotoIfNEqu(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  ReadLabel();
  if (m_threads[m_cur_thread].reg[op1].value != m_threads[m_cur_thread].reg[op2].value) {
    cHeadCPU found_pos = FindLabelForward(true);
    getIP().Set(found_pos);
  }
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GotoIfLess(Context&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  ReadLabel();
  if (m_threads[m_cur_thread].reg[op1].value < m_threads[m_cur_thread].reg[op2].value) {
    cHeadCPU found_pos = FindLabelForward(true);
    getIP().Set(found_pos);
  }
  return true;
}


bool Hardware::Types::HeadsEX::Inst_GotoConsensus(Context&)
{
  if (BitCount(GetRegister(rBX)) < CONSENSUS) return true;
  
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GotoConsensus24(Context&)
{
  if (BitCount(GetRegister(rBX) & MASK_LOW24) < CONSENSUS24) return true;
  
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}


bool Hardware::Types::HeadsEX::Inst_JumpHead(Context&)
{
  const int head_used = FindModifiedHead(HEAD_IP);
  const int reg = FindModifiedRegister(rCX);
  m_from_sensor = FromSensor(reg);
  getHead(head_used).Jump(m_threads[m_cur_thread].reg[reg].value);
  if (head_used == HEAD_IP) m_advance_ip = false;
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetHead(Context&)
{
  const int head_used = FindModifiedHead(HEAD_IP);
  const int reg = FindModifiedRegister(rCX);
  setInternalValue(reg, getHead(head_used).GetPosition());
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfCopiedCompLabel(Context&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfCopiedDirectLabel(Context&)
{
  ReadLabel();
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfCopiedCompSeq(Context&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadSequence())  getIP().Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_IfCopiedDirectSeq(Context&)
{
  ReadLabel();
  if (GetLabel() != GetReadSequence())  getIP().Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_HeadDivide(Context& ctx)
{
  m_organism->GetPhenotype().SetDivideSex(false);
  m_organism->GetPhenotype().SetCrossNum(0);
  
  AdjustHeads();
  const int divide_pos = getHead(HEAD_READ).GetPosition();
  int child_end =  getHead(HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
  bool ret_val = Divide_Main(ctx, divide_pos, extra_lines, 1.0);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val;
}

bool Hardware::Types::HeadsEX::Inst_HeadDivideSex(Context& ctx)
{
  m_organism->GetPhenotype().SetDivideSex(true);
  m_organism->GetPhenotype().SetCrossNum(1);
  
  AdjustHeads();
  const int divide_pos = getHead(HEAD_READ).GetPosition();
  int child_end =  getHead(HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
  bool ret_val = Divide_Main(ctx, divide_pos, extra_lines, 1.0);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val;
}

bool Hardware::Types::HeadsEX::Inst_HeadRead(Context& ctx)
{
  const int head_id = FindModifiedHead(HEAD_READ);
  const int dst = FindModifiedRegister(rAX);
  getHead(head_id).Adjust();
  
  // Mutations only occur on the read, for the moment.
  Instruction read_inst;
  if (m_organism->TestCopyMut(ctx)) {
    read_inst = m_inst_set->GetRandomInst(ctx);
  } else {
    read_inst = getHead(head_id).GetInst();
  }
  setInternalValue(dst, read_inst.GetOp());
  ReadInst(read_inst);
  
  if (m_slip_read_head && m_organism->TestCopySlip(ctx))
    getHead(head_id).Set(ctx.GetRandom().GetInt(m_memory.GetSize()));
  
  getHead(head_id).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_HeadWrite(Context& ctx)
{
  const int head_id = FindModifiedHead(HEAD_WRITE);
  const int src = FindModifiedRegister(rAX);
  cHeadCPU& active_head = getHead(head_id);
  
  active_head.Adjust();
  
  int value = m_threads[m_cur_thread].reg[src].value;
  if (value < 0 || value >= m_inst_set->GetSize()) value = 0;
  
  active_head.SetInst(Instruction(value));
  active_head.SetFlagCopied();
  
  if (m_organism->TestCopyIns(ctx)) active_head.InsertInst(m_inst_set->GetRandomInst(ctx));
  if (m_organism->TestCopyDel(ctx)) active_head.RemoveInst();
  if (m_organism->TestCopyUniform(ctx)) doUniformCopyMutation(ctx, active_head);
  if (!m_slip_read_head && m_organism->TestCopySlip(ctx))
    doSlipMutation(ctx, m_memory, active_head.GetPosition());
  
  // Advance the head after write...
  active_head++;
  return true;
}

bool Hardware::Types::HeadsEX::Inst_HeadCopy(Context& ctx)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU& read_head = getHead(HEAD_READ);
  cHeadCPU& write_head = getHead(HEAD_WRITE);
  
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
  if (m_organism->TestCopyUniform(ctx)) doUniformCopyMutation(ctx, write_head);
  if (m_organism->TestCopySlip(ctx)) {
    if (m_slip_read_head) {
      read_head.Set(ctx.GetRandom().GetInt(m_memory.GetSize()));
    } else
      doSlipMutation(ctx, m_memory, write_head.GetPosition());
  }
  
  read_head.Advance();
  write_head.Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Label_Comp_S(Context&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelStart(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Label_Comp_F(Context&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Label_Comp_B(Context&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelBackward(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Label_Direct_S(Context&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelStart(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Label_Direct_F(Context&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelForward(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Label_Direct_B(Context&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelBackward(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Seq_Comp_S(Context&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceStart(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Seq_Comp_F(Context&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceForward(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Seq_Comp_B(Context&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceBackward(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Seq_Direct_S(Context&)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceStart(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Seq_Direct_F(Context&)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceForward(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Search_Seq_Direct_B(Context&)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceBackward(true);
  getHead(HEAD_FLOW).Set(found_pos);
  getHead(HEAD_FLOW).Advance();
  return true;
}


bool Hardware::Types::HeadsEX::Inst_SetFlow(Context&)
{
  const int reg_used = FindModifiedRegister(rCX);
  m_from_sensor = FromSensor(reg_used);
  getHead(HEAD_FLOW).Set(GetRegister(reg_used));
  return true;
}



bool Hardware::Types::HeadsEX::Inst_WaitCondition_Equal(Context&)
{
  const int wait_value = FindModifiedRegister(rBX);
  const int check_reg = FindModifiedRegister(rDX);
  const int wait_dst = FindModifiedRegister(wait_value);
  
  m_from_sensor = FromSensor(wait_value);
  
  // Check if condition has already been met
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (i != m_cur_thread && m_threads[i].reg[check_reg].value == m_threads[m_cur_thread].reg[wait_value].value) {
      setInternalValue(wait_dst, m_threads[i].reg[check_reg].value, m_threads[i].reg[check_reg]);
      return true;
    }
  }
  // Fail to sleep if this is the last thread awake
  if (int(m_waiting_threads) == (m_threads.GetSize() - 1)) return false;
  
  // Put thread to sleep with appropriate wait condition
  m_threads[m_cur_thread].active = false;
  m_waiting_threads++;
  m_threads[m_cur_thread].wait_equal = true;
  m_threads[m_cur_thread].wait_less = false;
  m_threads[m_cur_thread].wait_greater = false;
  m_threads[m_cur_thread].wait_reg = check_reg;
  m_threads[m_cur_thread].wait_value = m_threads[m_cur_thread].reg[wait_value].value;
  m_threads[m_cur_thread].wait_dst = wait_dst;
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_WaitCondition_Less(Context&)
{
  const int wait_value = FindModifiedRegister(rBX);
  const int check_reg = FindModifiedRegister(rDX);
  const int wait_dst = FindModifiedRegister(wait_value);
  
  m_from_sensor = FromSensor(wait_value);
  
  // Check if condition has already been met
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (i != m_cur_thread && m_threads[i].reg[check_reg].value < m_threads[m_cur_thread].reg[wait_value].value) {
      setInternalValue(wait_dst, m_threads[i].reg[check_reg].value, m_threads[i].reg[check_reg]);
      return true;
    }
  }
  
  // Fail to sleep if this is the last thread awake
  if (int(m_waiting_threads) == (m_threads.GetSize() - 1)) return false;
  
  // Put thread to sleep with appropriate wait condition
  m_threads[m_cur_thread].active = false;
  m_waiting_threads++;
  m_threads[m_cur_thread].wait_equal = false;
  m_threads[m_cur_thread].wait_less = true;
  m_threads[m_cur_thread].wait_greater = false;
  m_threads[m_cur_thread].wait_reg = check_reg;
  m_threads[m_cur_thread].wait_value = m_threads[m_cur_thread].reg[wait_value].value;
  m_threads[m_cur_thread].wait_dst = wait_dst;
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_WaitCondition_Greater(Context&)
{
  const int wait_value = FindModifiedRegister(rBX);
  const int check_reg = FindModifiedRegister(rDX);
  const int wait_dst = FindModifiedRegister(wait_value);
  
  m_from_sensor = FromSensor(wait_value);
  
  // Check if condition has already been met
  for (int i = 0; i < m_threads.GetSize(); i++) {
    if (i != m_cur_thread && m_threads[i].reg[check_reg].value > m_threads[m_cur_thread].reg[wait_value].value) {
      setInternalValue(wait_dst, m_threads[i].reg[check_reg].value, m_threads[i].reg[check_reg]);
      return true;
    }
  }
  
  // Fail to sleep if this is the last thread awake
  if (int(m_waiting_threads) == (m_threads.GetSize() - 1)) return false;
  
  // Put thread to sleep with appropriate wait condition
  m_threads[m_cur_thread].active = false;
  m_waiting_threads++;
  m_threads[m_cur_thread].wait_equal = false;
  m_threads[m_cur_thread].wait_less = false;
  m_threads[m_cur_thread].wait_greater = true;
  m_threads[m_cur_thread].wait_reg = check_reg;
  m_threads[m_cur_thread].wait_value = m_threads[m_cur_thread].reg[wait_value].value;
  m_threads[m_cur_thread].wait_dst = wait_dst;
  
  return true;
}




/*!
 Sets BX to 1 if >=50% of the bits in the specified register places
 are 1's and zero otherwise.
 */

bool Hardware::Types::HeadsEX::Inst_BitConsensus(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  const int op1 = FindModifiedNextRegister(reg_used);
  DataValue& val = m_threads[m_cur_thread].reg[op1];
  
  setInternalValue(reg_used, (BitCount(val.value) >= CONSENSUS) ? 1 : 0, val);
  return true;
}

// Looks at only the lower 24 bits
bool Hardware::Types::HeadsEX::Inst_BitConsensus24(Context&)
{
  const int reg_used = FindModifiedRegister(rBX);
  const int op1 = FindModifiedNextRegister(reg_used);
  DataValue& val = m_threads[m_cur_thread].reg[op1];
  
  setInternalValue(reg_used, (BitCount(val.value & MASK_LOW24) >= CONSENSUS24) ? 1 : 0, val);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_Repro(Context& ctx)
{
  // these checks should be done, but currently they make some assumptions
  // that crash when evaluating this kind of organism -- JEB
  
  if (m_organism->GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  InstructionSequencePtr offspring_seq(new InstructionSequence(m_memory));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)m_inst_set->GetInstSetName());
  Genome offspring(GetType(), props, offspring_seq);
  
  m_organism->OffspringGenome() = offspring;
  m_organism->GetPhenotype().SetLinesCopied(m_memory.GetSize());
  
  int lines_executed = 0;
  for (int i = 0; i < m_memory.GetSize(); i++) if (m_memory.FlagExecuted(i)) lines_executed++;
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
  if (m_organism->GetCopyMutProb() > 0 && (m_organism->GetForageTarget() < -1 || !m_world->GetConfig().PREY_MUT_OFF.Get())) { // Skip this if no mutations....
    for (int i = 0; i < child_seq.GetSize(); i++) {
      //    for (int i = 0; i < m_memory.GetSize(); i++) {
      if (m_organism->TestCopyMut(ctx)) child_seq[i] = m_inst_set->GetRandomInst(ctx);
    }
  }
  
  // Handle Divide Mutations...
  if (m_organism->GetForageTarget() < -1 || !m_world->GetConfig().PREY_MUT_OFF.Get()) Divide_DoMutations(ctx);
  
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
  
  m_mal_active = false;
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

bool Hardware::Types::HeadsEX::Inst_Die(Context& ctx)
{
  m_organism->Die(ctx);
  
  return true;
}



bool Hardware::Types::HeadsEX::Inst_Move(Context& ctx)
{
  // In TestCPU, movement fails...
  if (m_organism->GetOrgInterface().GetCellID() == -1) return false;
  
  bool move_success = false;
  if (!m_use_avatar) move_success = m_organism->Move(ctx);
  else if (m_use_avatar) move_success = m_organism->MoveAV(ctx);
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, move_success, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_JuvMove(Context& ctx)
{
  // In TestCPU, movement fails...
  if (m_organism->GetOrgInterface().GetCellID() == -1) return false;
  
  if (m_organism->GetPhenotype().GetTimeUsed() < m_world->GetConfig().JUV_PERIOD.Get()) return false;
  
  if (m_organism->IsGuard()) return false;
  
  bool move_success = false;
  if (!m_use_avatar) move_success = m_organism->Move(ctx);
  else if (m_use_avatar) move_success = m_organism->MoveAV(ctx);
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, move_success, true);
  return true;
}


bool Hardware::Types::HeadsEX::Inst_GetCellPosition(Context& ctx)
{
  int x = m_organism->GetOrgInterface().GetCellXPosition();
  int y = m_organism->GetOrgInterface().GetCellYPosition();
  // Fail if we're running in the test CPU
  if (x == -1 || y == -1) return false;
  
  const int xreg = FindModifiedRegister(rBX);
  const int yreg = FindNextRegister(xreg);
  setInternalValue(xreg, x, true);
  setInternalValue(yreg, y, true);
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetCellPositionX(Context& ctx)
{
  int x = m_organism->GetOrgInterface().GetCellXPosition();
  // Fail if we're running in the test CPU
  if (x == -1) return false;
  
  const int xreg = FindModifiedRegister(rBX);
  setInternalValue(xreg, x, true);
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetCellPositionY(Context& ctx)
{
  int y = m_organism->GetOrgInterface().GetCellYPosition();
  // Fail if we're running in the test CPU
  if (y == -1) return false;
  
  const int yreg = FindModifiedRegister(rBX);
  setInternalValue(yreg, y, true);
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetNorthOffset(Context& ctx) {
  const int out_reg = FindModifiedRegister(rBX);
  int compass_dir = m_organism->GetOrgInterface().GetFacedDir();
  if (m_use_avatar) compass_dir = m_organism->GetOrgInterface().GetAVFacing();
  setInternalValue(out_reg, compass_dir, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetPositionOffset(Context&) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetNortherly(), true);
  setInternalValue(FindModifiedNextRegister(out_reg), m_organism->GetEasterly(), true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetNortherly(Context&) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetNortherly(), true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetEasterly(Context&) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetEasterly(), true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_ZeroEasterly(Context&) {
  m_organism->ClearEasterly();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_ZeroNortherly(Context&) {
  m_organism->ClearNortherly();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_ZeroPosOffset(Context&) {
  const int offset = GetRegister(FindModifiedRegister(rBX)) % 3;
  if (offset == 0) {
    m_organism->ClearEasterly();
    m_organism->ClearNortherly();
  }
  else if (offset == 1) m_organism->ClearEasterly();
  else if (offset == 2) m_organism->ClearNortherly();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_RotateLeftOne(Context& ctx)
{
  m_organism->Rotate(ctx, 1);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_RotateRightOne(Context& ctx)
{
  m_organism->Rotate(ctx, -1);
  return true;
}


bool Hardware::Types::HeadsEX::Inst_RotateUpFtHill(Context& ctx)
{
  int ft = m_organism->GetForageTarget();
  double current_res = 0.0;
  if (m_use_avatar) current_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, ft);
  else if (!m_use_avatar) current_res = m_organism->GetOrgInterface().GetResourceVal(ctx, ft);
  
  int actualNeighborhoodSize = m_organism->GetNeighborhoodSize();
  if (m_use_avatar) actualNeighborhoodSize = m_organism->GetOrgInterface().GetAVNumNeighbors();
  
  double max_res = 0;
  for(int i = 0; i < actualNeighborhoodSize; i++) {
    m_organism->Rotate(ctx, 1);
    double faced_res = 0.0;
    if (!m_use_avatar) faced_res = m_organism->GetOrgInterface().GetFacedResourceVal(ctx, ft);
    else if (m_use_avatar) faced_res = m_organism->GetOrgInterface().GetAVFacedResourceVal(ctx, ft);
    if (faced_res > max_res) max_res = faced_res;
  }
  
  if (max_res > current_res) {
    for(int i = 0; i < actualNeighborhoodSize; i++) {
      double faced_res = 0.0;
      if (!m_use_avatar) faced_res = m_organism->GetOrgInterface().GetFacedResourceVal(ctx, ft);
      else faced_res = m_organism->GetOrgInterface().GetAVFacedResourceVal(ctx, ft);
      if (faced_res != max_res) m_organism->Rotate(1);
    }
  }
  int res_diff = 0;
  if (current_res == 0) res_diff = (int) max_res;
  else res_diff = (int) (((max_res - current_res)/current_res) * 100 + 0.5);
  int reg_to_set = FindModifiedRegister(rBX);
  setInternalValue(reg_to_set, res_diff, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_RotateHome(Context& ctx)
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
  
  int rotates = m_organism->GetNeighborhoodSize();
  if (m_use_avatar) rotates = m_organism->GetOrgInterface().GetAVNumNeighbors();
  for (int i = 0; i < rotates; i++) {
    m_organism->Rotate(ctx, 1);
    if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedDir() == correct_facing) break;
    else if (m_use_avatar && m_organism->GetOrgInterface().GetAVFacing() == correct_facing) break;
  }
  return true;
}

bool Hardware::Types::HeadsEX::Inst_RotateUnoccupiedCell(Context& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  const int reg_used = FindModifiedRegister(rBX);
  
  int num_neighbors = m_organism->GetNeighborhoodSize();
  if (m_use_avatar) num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
  for (int i = 0; i < num_neighbors; i++) {
    if ((!m_use_avatar && !m_organism->IsNeighborCellOccupied()) || (m_use_avatar == 2 && !m_organism->GetOrgInterface().FacedHasAV())) {
      setInternalValue(reg_used, 1, true);
      return true;
    }
    m_organism->Rotate(ctx, 1); // continue to rotate
  }
  setInternalValue(reg_used, 0, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_RotateX(Context& ctx)
{
  int num_neighbors = m_organism->GetNeighborhoodSize();
  if (m_use_avatar) num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
  int rot_dir = 1;
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  const int reg_used = FindModifiedRegister(rBX);
  int rot_num = m_threads[m_cur_thread].reg[reg_used].value;
  m_from_sensor = FromSensor(reg_used);
  // rotate the nop number of times in the appropriate direction
  rot_num < 0 ? rot_dir = -1 : rot_dir = 1;
  rot_num = abs(rot_num);
  if (rot_num > 7) rot_num = rot_num % 8;
  for (int i = 0; i < rot_num; i++) m_organism->Rotate(ctx, rot_dir);
  setInternalValue(reg_used, rot_num * rot_dir, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_RotateDir(Context& ctx)
{
  int num_neighbors = m_organism->GetNeighborhoodSize();
  if (m_use_avatar) num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  const int reg_used = FindModifiedRegister(rBX);
  int rot_dir = abs(m_threads[m_cur_thread].reg[reg_used].value) % 8;
  m_from_sensor = FromSensor(reg_used);
  
  if (m_use_avatar) m_organism->GetOrgInterface().SetAVFacing(ctx, rot_dir);
  // rotate to the appropriate direction
  else {
    for (int i = 0; i < num_neighbors + 1; i++) {
      m_organism->Rotate(ctx, -1);
      if (m_organism->GetOrgInterface().GetFacedDir() == rot_dir) break;
    }
  }
  int current_dir = m_organism->GetOrgInterface().GetFacedDir();
  if (m_use_avatar) current_dir = m_organism->GetOrgInterface().GetAVFacing();
  setInternalValue(reg_used, current_dir, true);
  return true;
}

// Will rotate organism to face a specified other org
bool Hardware::Types::HeadsEX::Inst_RotateOrgID(Context& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  // Will rotate organism to face a specificied other org
  const int id_sought_reg = FindModifiedRegister(rBX);
  const int id_sought = m_threads[m_cur_thread].reg[id_sought_reg].value;
  m_from_sensor = FromSensor(id_sought_reg);
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
      
      setInternalValue(dist_reg, -2, true, true);
      setInternalValue(dir_reg, m_sensor.ReturnRelativeFacing(target_org), true, true);
      setInternalValue(fat_reg, (int) target_org->GetPhenotype().GetCurBonus(), true, true);
      setInternalValue(ft_reg, target_org->GetForageTarget(), true, true);
      if ((target_org->IsDisplaying() || m_world->GetConfig().USE_DISPLAY.Get()) && target_org->GetOrgDisplayData() != NULL) m_sensor.SetLastSeenDisplay(target_org->GetOrgDisplayData());
    }
    return true;
  }
}

// Will rotate organism to face away from a specificied other org
bool Hardware::Types::HeadsEX::Inst_RotateAwayOrgID(Context& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  // Will rotate organism to face a specificied other org
  const int id_sought_reg = FindModifiedRegister(rBX);
  const int id_sought = m_threads[m_cur_thread].reg[id_sought_reg].value;
  m_from_sensor = FromSensor(id_sought_reg);
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
      
      setInternalValue(dist_reg, -2, true, true);
      setInternalValue(dir_reg, m_sensor.ReturnRelativeFacing(target_org), true, true);
      setInternalValue(fat_reg, (int) target_org->GetPhenotype().GetCurBonus(), true, true);
      setInternalValue(ft_reg, target_org->GetForageTarget(), true, true);
      if ((target_org->IsDisplaying() || m_world->GetConfig().USE_DISPLAY.Get()) && target_org->GetOrgDisplayData() != NULL) m_sensor.SetLastSeenDisplay(target_org->GetOrgDisplayData());
    }
    return true;
  }
}



bool Hardware::Types::HeadsEX::Inst_SenseResourceID(Context& ctx)
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
      setInternalValue(reg_to_set, i, true, true);
    }
  }
  return true;
}

bool Hardware::Types::HeadsEX::Inst_SenseResQuant(Context& ctx)
{
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  
  const int req_reg = FindModifiedRegister(rBX);
  int res_sought = -1;
  // are you trying to sense a valid, non-hidden nest resource?
  const int res_req = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  if (res_req < resource_lib.GetSize() && res_req > 0 && resource_lib.GetResDef(res_req)->GetHabitat() != 3) {
    res_sought = res_req;
  }
  
  int res_amount = 0;
  int faced_res = 0;
  // if you requested a valid resource, we return values for that res
  if (res_sought != -1) {
    if (!m_use_avatar) faced_res = (int) (m_organism->GetOrgInterface().GetFacedResourceVal(ctx, res_sought));
    else if (m_use_avatar)  faced_res = (int) (m_organism->GetOrgInterface().GetAVFacedResourceVal(ctx, res_sought));
  }
  // otherwise, we sum across all the food resources in the cell
  else {
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResDef(i)->GetHabitat() == 0 || resource_lib.GetResDef(i)->GetHabitat() > 5) {
        if (!m_use_avatar) faced_res += (int) (m_organism->GetOrgInterface().GetFacedResourceVal(ctx, i));
        else if (m_use_avatar)  faced_res += (int) (m_organism->GetOrgInterface().GetAVFacedResourceVal(ctx, i));
      }
    }
  }
  
  // return % change
  int res_diff = 0;
  if (res_amount == 0) res_diff = (int) faced_res;
  else res_diff = (int) (((faced_res - res_amount) / res_amount) * 100 + 0.5);
  
  setInternalValue(req_reg, res_sought, true, true);
  const int res_tot_reg = FindModifiedNextRegister(req_reg);
  setInternalValue(res_tot_reg, res_amount, true, true);
  setInternalValue(FindModifiedNextRegister(res_tot_reg), res_diff, true, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_SenseNest(Context& ctx)
{
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  const int reg_used = FindModifiedRegister(rBX);
  
  int nest_id = m_threads[m_cur_thread].reg[reg_used].value;
  int nest_val = 0;
  
  // if invalid nop value, return the id of the first nest in the cell with val >= 1
  double cell_res = 0.0;
  if (nest_id < 0 || nest_id >= resource_lib.GetSize() || !resource_lib.GetResource(nest_id)->IsNest()) {
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->IsNest()) {
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
        if (cell_res >= 1) {
          nest_id = i;
          nest_val = (int) cell_res;
          break;
        }
      }
    }
  }
  else {
    if (m_use_avatar) nest_val = m_organism->GetOrgInterface().GetAVResourceVal(ctx, nest_id);
    else if (!m_use_avatar) nest_val = m_organism->GetOrgInterface().GetResourceVal(ctx, nest_id);
  }
  setInternalValue(reg_used, nest_id, true, true);
  const int val_reg = FindModifiedNextRegister(reg_used);
  setInternalValue(val_reg, nest_val, true, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_SenseResDiff(Context& ctx)
{
  const int req_reg = FindModifiedRegister(rBX);
  int res_sought = -1;
  // are you trying to sense a valid resource?
  const int res_req = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  if (res_req < m_world->GetEnvironment().GetResDefLib().GetSize() && res_req > 0) {
    res_sought = res_req;
  }
  
  int cell_res = 0;
  int faced_res = 0;
  int res_amount = 0;
  if (res_sought != -1) {
    if (m_use_avatar) cell_res = (int) m_organism->GetOrgInterface().GetAVResourceVal(ctx, res_sought);
    else if (!m_use_avatar) cell_res = (int) m_organism->GetOrgInterface().GetResourceVal(ctx, res_sought);
    
    if (!m_use_avatar) faced_res = (int) (m_organism->GetOrgInterface().GetFacedResourceVal(ctx, res_sought));
    else if (m_use_avatar) faced_res = (int) (m_organism->GetOrgInterface().GetAVFacedResourceVal(ctx, res_sought));
  }
  else {
    Apto::Array<double> faced;
    Apto::Array<double> here;
    if (m_use_avatar) {
      here = m_organism->GetOrgInterface().GetAVResources(ctx);
      faced = m_organism->GetOrgInterface().GetAVFacedResources(ctx);
    }
    else if (!m_use_avatar)  {
      here = m_organism->GetOrgInterface().GetResources(ctx);
      faced = m_organism->GetOrgInterface().GetFacedCellResources(ctx);
      for (int i = 0; i < m_world->GetEnvironment().GetResDefLib().GetSize(); i++ ) {
        res_amount += (int) here[i];
        faced_res += (int) faced[i];
      }
    }
  }
  
  setInternalValue(FindModifiedNextRegister(req_reg), res_sought, true, true);
  const int res_tot_reg = FindModifiedNextRegister(FindModifiedNextRegister(req_reg));
  setInternalValue(res_tot_reg, res_amount - faced_res, true, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_LookAhead(Context& ctx)
{
  int cell = m_organism->GetOrgInterface().GetCellID();
  int facing = m_organism->GetOrgInterface().GetFacedDir();
  if (m_use_avatar) {
    cell = m_organism->GetOrgInterface().GetAVCellID();
    facing = m_organism->GetOrgInterface().GetAVFacing();
  }
  return GoLook(ctx, facing, cell);
}

// Will return relative org facing (rotations to intercept) rather than group info for sighted org
bool Hardware::Types::HeadsEX::Inst_LookAheadIntercept(Context& ctx)
{
  m_sensor.SetReturnRelativeFacing(true);
  return Inst_LookAhead(ctx);
}

bool Hardware::Types::HeadsEX::Inst_LookAround(Context& ctx)
{
  // dir register is 5th mod (will be count reg)
  int hab_reg = FindModifiedRegister(rBX);
  int dist_reg = FindModifiedNextRegister(hab_reg);
  int st_reg = FindModifiedNextRegister(dist_reg);
  int id_reg = FindModifiedNextRegister(st_reg);
  int dir_reg = FindModifiedNextRegister(id_reg);
  
  int search_dir = abs(m_threads[m_cur_thread].reg[dir_reg].value) % 3;
  m_from_sensor = FromSensor(dir_reg);
  
  if (m_world->GetConfig().LOOK_DISABLE.Get() == 5) {
    int org_type = m_world->GetConfig().LOOK_DISABLE_TYPE.Get();
    bool is_target_type = false;
    if (org_type == 0 && !m_organism->IsPreyFT()) is_target_type = true;
    else if (org_type == 1 && m_organism->IsPreyFT()) is_target_type = true;
    else if (org_type == 2) is_target_type = true;
    
    if (is_target_type) {
      int rand = ctx.GetRandom().GetInt(INT_MAX);
      search_dir = rand % 3;
    }
  }
  
  if (search_dir == 1) search_dir = -1;
  else if (search_dir == 2) search_dir = 1;
  
  int facing = m_organism->GetOrgInterface().GetFacedDir() + search_dir;
  if (m_use_avatar) facing = m_organism->GetOrgInterface().GetAVFacing() + search_dir;
  if (facing == -1) facing = 7;
  else if (facing == 9) facing = 1;
  else if (facing == 8) facing = 0;
  
  int cell = m_organism->GetOrgInterface().GetCellID();
  if (m_use_avatar) cell = m_organism->GetOrgInterface().GetAVCellID();
  return GoLook(ctx, facing, cell);
}

bool Hardware::Types::HeadsEX::Inst_LookAroundIntercept(Context& ctx)
{
  m_sensor.SetReturnRelativeFacing(true);
  return Inst_LookAround(ctx);
}

bool Hardware::Types::HeadsEX::Inst_LookFT(Context& ctx)
{
  // override any org inputs and just let this org see the food resource that matches it's forage target (not designed for predators)
  int cell = m_organism->GetOrgInterface().GetCellID();
  int facing = m_organism->GetOrgInterface().GetFacedDir();
  if (m_use_avatar) {
    facing = m_organism->GetOrgInterface().GetAVFacing();
    cell = m_organism->GetOrgInterface().GetAVCellID();
  }
  return GoLook(ctx, facing, cell, true);
}

bool Hardware::Types::HeadsEX::Inst_LookAroundFT(Context& ctx)
{
  // dir register is 5th mod (will be count reg)
  int hab_reg = FindModifiedRegister(rBX);
  int dist_reg = FindModifiedNextRegister(hab_reg);
  int st_reg = FindModifiedNextRegister(dist_reg);
  int id_reg = FindModifiedNextRegister(st_reg);
  int dir_reg = FindModifiedNextRegister(id_reg);
  
  int search_dir = abs(m_threads[m_cur_thread].reg[dir_reg].value) % 3;
  m_from_sensor = FromSensor(dir_reg);
  
  if (m_world->GetConfig().LOOK_DISABLE.Get() == 5) {
    int org_type = m_world->GetConfig().LOOK_DISABLE_TYPE.Get();
    bool is_target_type = false;
    if (org_type == 0 && !m_organism->IsPreyFT()) is_target_type = true;
    else if (org_type == 1 && m_organism->IsPreyFT()) is_target_type = true;
    else if (org_type == 2) is_target_type = true;
    
    if (is_target_type) {
      int rand = ctx.GetRandom().GetInt(INT_MAX);
      search_dir = rand % 3;
    }
  }
  
  if (search_dir == 1) search_dir = -1;
  else if (search_dir == 2) search_dir = 1;
  
  int facing = m_organism->GetOrgInterface().GetFacedDir() + search_dir;
  if (m_use_avatar) facing = m_organism->GetOrgInterface().GetAVFacing() + search_dir;
  if (facing == -1) facing = 7;
  else if (facing == 9) facing = 1;
  else if (facing == 8) facing = 0;
  
  int cell = m_organism->GetOrgInterface().GetCellID();
  if (m_use_avatar) cell = m_organism->GetOrgInterface().GetAVCellID();
  return GoLook(ctx, facing, cell, true);
}

bool Hardware::Types::HeadsEX::GoLook(Context& ctx, const int look_dir, const int cell_id, bool use_ft)
{
  // temp check on world geometry until code can handle other geometries
  /*if (m_world->GetConfig().WORLD_GEOMETRY.Get() != 1) {
   // Instruction sense-diff-ahead only written to work in bounded grids
   return false;
   }  */
  if (NUM_REGISTERS < 8) m_world->GetDriver().Feedback().Error("Instruction look-ahead requires at least 8 registers");
  
  if (!m_use_avatar && m_organism->GetNeighborhoodSize() == 0) return false;
  else if (m_use_avatar && m_organism->GetOrgInterface().GetAVNumNeighbors() == 0) return false;
  
  // define our input (4) and output registers (8)
  sLookRegAssign reg_defs;
  reg_defs.habitat = FindModifiedRegister(rBX);
  reg_defs.distance = FindModifiedNextRegister(reg_defs.habitat);
  reg_defs.search_type = FindModifiedNextRegister(reg_defs.distance);
  reg_defs.id_sought = FindModifiedNextRegister(reg_defs.search_type);
  reg_defs.count = FindModifiedNextRegister(reg_defs.id_sought);
  reg_defs.value = FindModifiedNextRegister(reg_defs.count);
  reg_defs.group = FindModifiedNextRegister(reg_defs.value);
  reg_defs.ft = FindModifiedNextRegister(reg_defs.group);
  
  Features::VisualSensor::LookResults look_results;
  look_results.report_type = 0;
  look_results.habitat = 0;
  look_results.distance = -1;
  look_results.search_type = 0;
  look_results.id_sought = -1;
  look_results.count = 0;
  look_results.value = 0;
  look_results.group = -9;
  look_results.forage = -9;
  
  look_results = InitLooking(ctx, reg_defs, look_dir, cell_id, use_ft);
  LookResults(ctx, reg_defs, look_results);
  return true;
}

Hardware::Features::VisualSensor::LookResults Hardware::Types::HeadsEX::InitLooking(Context& ctx, sLookRegAssign& in_defs, int facing, int cell_id, bool use_ft)
{
  const int habitat_reg = in_defs.habitat;
  const int distance_reg = in_defs.distance;
  const int search_reg = in_defs.search_type;
  const int id_reg = in_defs.id_sought;
  
  Features::VisualSensor::LookResults reg_init;
  reg_init.habitat = m_threads[m_cur_thread].reg[habitat_reg].value;
  reg_init.distance = m_threads[m_cur_thread].reg[distance_reg].value;
  reg_init.search_type = m_threads[m_cur_thread].reg[search_reg].value;
  reg_init.id_sought = m_threads[m_cur_thread].reg[id_reg].value;
  
  m_from_sensor = (FromSensor(habitat_reg) || FromSensor(distance_reg) || FromSensor(search_reg) || FromSensor(id_reg) || m_from_sensor);
  
  return m_sensor.SetLooking(ctx, reg_init, facing, cell_id, use_ft);
}

void Hardware::Types::HeadsEX::LookResults(Context& ctx, sLookRegAssign& regs, Features::VisualSensor::LookResults& results)
{
  // habitat_reg=0, distance_reg=1, search_type_reg=2, id_sought_reg=3, count_reg=4, value_reg=5, group_reg=6, forager_type_reg=7
  // return defaults for failed to find
  bool override = false;
  if (m_world->GetConfig().LOOK_DISABLE_COMBO.Get() > 0 && results.report_type == 1) {
    int disable_combo = m_world->GetConfig().LOOK_DISABLE_COMBO.Get();
    if (m_world->GetConfig().LOOK_DISABLE_TYPE.Get() == 2) {
      if (disable_combo == 1 && results.habitat == 0) override = true; // food
      else if (disable_combo == 2 && (results.habitat == -2 || results.habitat == 5) && (results.search_type == 1 || results.search_type == 2)) override = true; // predator
      else if (disable_combo == 3 && results.habitat == -2 && (results.search_type == -1 || results.search_type == -2)) override = true; // prey
      else if (disable_combo == 2 && (results.habitat == -2 || results.habitat == 5) && results.forage <= -2) override = true; // predator
      else if (disable_combo == 3 && results.habitat == -2 && results.forage > -2) override = true; // prey
    }
    else if (m_world->GetConfig().LOOK_DISABLE_TYPE.Get() == 1 && m_organism->GetForageTarget() > -2) {
      if (disable_combo == 1 && results.habitat == 0) override = true; // food
      else if (disable_combo == 2 && (results.habitat == -2 || results.habitat == 5) && (results.search_type == 1 || results.search_type == 2)) override = true; // predator
      else if (disable_combo == 3 && results.habitat == -2 && (results.search_type == -1 || results.search_type == -2)) override = true; // prey
      else if (disable_combo == 2 && (results.habitat == -2 || results.habitat == 5) && results.forage <= -2) override = true; // predator
      else if (disable_combo == 3 && results.habitat == -2 && results.forage > -2) override = true; // prey
    }
    else if (m_world->GetConfig().LOOK_DISABLE_TYPE.Get() == 0 && m_organism->GetForageTarget() <= -2) {
      if (disable_combo == 1 && results.habitat == 0) override = true; // food
      else if (disable_combo == 2 && (results.habitat == -2 || results.habitat == 5) && (results.search_type == 1 || results.search_type == 2)) override = true; // predator
      else if (disable_combo == 3 && results.habitat == -2 && (results.search_type == -1 || results.search_type == -2)) override = true; // prey
      else if (disable_combo == 2 && (results.habitat == -2 || results.habitat == 5) && results.forage <= -2) override = true; // predator
      else if (disable_combo == 3 && results.habitat == -2 && results.forage > -2) override = true; // prey
    }
  }
  if (override) {
    results.report_type = 0;
    results.id_sought = -1;
  }
  
  if (m_world->GetConfig().TRACK_LOOK_SETTINGS.Get()) {
    cString look_string = "";
    look_string += cStringUtil::Stringf("%d", m_organism->GetForageTarget());
    look_string += cStringUtil::Stringf(",%d", results.report_type);
    look_string += cStringUtil::Stringf(",%d", results.habitat);
    look_string += cStringUtil::Stringf(",%d", results.distance);
    look_string += cStringUtil::Stringf(",%d", results.search_type);
    look_string += cStringUtil::Stringf(",%d", results.id_sought);
    m_organism->GetOrgInterface().TryWriteLookData(look_string);
  }
  
  if (results.report_type == 0) {
    setInternalValue(regs.habitat, results.habitat, true, true);
    setInternalValue(regs.distance, -1, true, true);
    setInternalValue(regs.search_type, results.search_type, true, true);
    setInternalValue(regs.id_sought, results.id_sought, true, true);
    setInternalValue(regs.count, 0, true, true);
    setInternalValue(regs.value, 0, true, true);
    setInternalValue(regs.group, -9, true, true);
    setInternalValue(regs.ft, -9, true, true);
  }
  // report results as sent
  else if (results.report_type == 1) {
    setInternalValue(regs.habitat, results.habitat, true, true);
    setInternalValue(regs.distance, results.distance, true, true);
    setInternalValue(regs.search_type, results.search_type, true, true);
    setInternalValue(regs.id_sought, results.id_sought, true, true);
    setInternalValue(regs.count, results.count, true, true);
    setInternalValue(regs.value, results.value, true, true);
    setInternalValue(regs.group, results.group, true, true);
    setInternalValue(regs.ft, results.forage, true, true);
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
      
      if (target_reg == 6) setInternalValue(regs.habitat, rand, true);
      else if (target_reg == 7) setInternalValue(regs.distance, rand, true);
      else if (target_reg == 8) setInternalValue(regs.search_type, rand, true);
      else if (target_reg == 9) setInternalValue(regs.id_sought, rand, true);
      else if (target_reg == 10) setInternalValue(regs.count, rand, true);
      else if (target_reg == 11) setInternalValue(regs.value, rand, true);
      else if (target_reg == 12) setInternalValue(regs.group, rand, true);
      else if (target_reg == 13) setInternalValue(regs.ft, rand, true);
    }
  }
  
  if (m_world->GetConfig().TRACK_LOOK_OUTPUT.Get()) {
    cString look_string = "";
    look_string += cStringUtil::Stringf("%d", m_organism->GetForageTarget());
    look_string += cStringUtil::Stringf(",%d", results.report_type);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[regs.habitat].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[regs.distance].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[regs.search_type].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[regs.id_sought].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[regs.count].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[regs.value].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[regs.group].value);
    look_string += cStringUtil::Stringf(",%d", m_threads[m_cur_thread].reg[regs.ft].value);
    m_organism->GetOrgInterface().TryWriteLookOutput(look_string);
  }
  
  return;
}

bool Hardware::Types::HeadsEX::Inst_SenseFacedHabitat(Context& ctx)
{
  int reg_to_set = FindModifiedRegister(rBX);
  
  // get the resource library
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  
  // get the destination cell resource levels
  Apto::Array<double> cell_res;
  if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetFacedCellResources(ctx);
  else if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVFacedResources(ctx);
  
  // check for any habitats ahead that affect movement, returning the most 'severe' habitat type
  // simulated predator ahead
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (resource_lib.GetResDef(i)->GetHabitat() == 5 && cell_res[i] > 0) {
      setInternalValue(reg_to_set, 3, true);
      return true;
    }
  }
  // are there any barrier resources in the faced cell
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (resource_lib.GetResDef(i)->GetHabitat() == 2 && cell_res[i] > 0) {
      setInternalValue(reg_to_set, 2, true);
      return true;
    }
  }
  // if no barriers, are there any hills in the faced cell
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (resource_lib.GetResDef(i)->GetHabitat() == 1 && cell_res[i] > 0) {
      setInternalValue(reg_to_set, 1, true);
      return true;
    }
  }
  // if no barriers or hills, we return a 0 to indicate clear sailing
  setInternalValue(reg_to_set, 0, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_SetForageTarget(Context& ctx)
{
  //Testing with juveniles not able to eat deposits HACK
  //  if (m_organism->GetPhenotype().GetTimeUsed() < m_world->GetConfig().JUV_PERIOD.Get()) return false;
  
  assert(m_organism != 0);
  const int reg = FindModifiedRegister(rBX);
  int prop_target = GetRegister(reg);
  m_from_sensor = FromSensor(reg);
  
  //return false if org setting target to current one (avoid paying costs for not switching)
  const int old_target = m_organism->GetForageTarget();
  if (old_target == prop_target) return false;
  
  // return false if predator trying to become prey and this has been disallowed
  if (old_target <= -2 && prop_target > -2 && (m_world->GetConfig().PRED_PREY_SWITCH.Get() == 0 || m_world->GetConfig().PRED_PREY_SWITCH.Get() == 2)) return false;
  
  // return false if trying to become predator and there are none in the experiment
  if (prop_target <= -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0) return false;
  
  // return false if trying to become predator or top predator and this has been disallowed via setforagetarget
  if (prop_target <= -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() == 2) return false;
  
  // a little mod help...can't set to -1, that's for juevniles only...so only exception to mod help is -2 or -3
  if (!m_world->GetEnvironment().IsTargetID(prop_target) && prop_target != -2 && prop_target != -3) {
    int num_fts = 0;
    std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
    set <int>::iterator itr;
    for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) if (*itr != -1 && *itr != -2 && *itr != -3) num_fts++;
    if (num_fts == 0) {
      if (m_world->GetEnvironment().IsTargetID(-1)) prop_target = -1;
    }
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
  if (prop_target == -2 && old_target > -2) MakePred(ctx);
  else if (prop_target == -3 && old_target > -2) MakeTopPred(ctx);
  else if (m_use_avatar && prop_target > -2 && old_target <= -2 && m_organism->GetOrgInterface().GetAVCellID() != -1) {
    m_organism->GetOrgInterface().SwitchPredPrey(ctx);
    m_organism->SetForageTarget(ctx, prop_target);
  }
  else m_organism->SetForageTarget(ctx, prop_target);
  
  // Set the new target and return the value
  m_organism->RecordFTSet();
  setInternalValue(FindModifiedRegister(rBX), prop_target, false);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_SetForageTargetOnce(Context& ctx)
{
  assert(m_organism != 0);
  if (m_organism->HasSetFT()) return false;
  else return Inst_SetForageTarget(ctx);
}

bool Hardware::Types::HeadsEX::Inst_SetRandForageTargetOnce(Context& ctx)
{
  assert(m_organism != 0);
  int cap = 0;
  if (m_world->GetConfig().POPULATION_CAP.Get()) cap = m_world->GetConfig().POPULATION_CAP.Get();
  else if (m_world->GetConfig().POP_CAP_ELDEST.Get()) cap = m_world->GetConfig().POP_CAP_ELDEST.Get();
  //  if (cap && (m_organism->GetOrgInterface().GetLiveOrgList().GetSize() >= (((double)(cap)) * 0.5)) && ctx.GetRandom().P(0.5)) {
  if (m_organism->HasSetFT()) return false;
  else {
    int num_fts = 0;
    std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
    set <int>::iterator itr;
    for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) if (*itr != -1 && *itr != -2 && *itr != -3) num_fts++;
    int prop_target = ctx.GetRandom().GetUInt(num_fts);
    if (num_fts == 0) {
      if (m_world->GetEnvironment().IsTargetID(-1)) prop_target = -1;
    }
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
    setInternalValue(FindModifiedRegister(rBX), prop_target, false);
    return true;
  }
  //  }
  //  else return Inst_SetForageTargetOnce(ctx);
}

// this inst is a terrible hack that makes assumptions about the fts available and is specific to one experiment
bool Hardware::Types::HeadsEX::Inst_SetRandPFTOnce(Context& ctx)
{
  assert(m_organism != 0);
  if (m_organism->HasSetFT()) return false;
  else {
    int prop_target = 2;
    if (ctx.GetRandom().P(0.5)) {
      prop_target = 0;
      if (ctx.GetRandom().P(0.5)) prop_target = 1;
    }
    
    int in_use = 0;
    Apto::Array<cOrganism*> orgs;
    const Apto::Array<cOrganism*, Apto::Smart>& live_orgs = m_world->GetPopulation().GetLiveOrgList();
    for (int i = 0; i < live_orgs.GetSize(); i++) {
      cOrganism* org = live_orgs[i];
      int this_target = org->GetForageTarget();
      if (this_target == prop_target) {
        in_use++;
        orgs.Push(org);
      }
    }
    if (m_world->GetConfig().MAX_PREY_BT.Get() && in_use >= m_world->GetConfig().MAX_PREY_BT.Get()) {
      orgs[ctx.GetRandom().GetUInt(0, in_use)]->Die(ctx);
    }
    
    // Set the new target and return the value
    m_organism->SetForageTarget(ctx, prop_target);
    m_organism->RecordFTSet();
    setInternalValue(FindModifiedRegister(rBX), prop_target, false);
  }
  return true;
}

bool Hardware::Types::HeadsEX::Inst_ShowForageTarget(Context& ctx)
{
  assert(m_organism != 0);
  const int reg = FindModifiedRegister(rBX);
  int prop_target = GetRegister(reg);
  m_from_sensor = FromSensor(reg);
  
  // return false if not a mimic ft type
  if (!m_organism->IsMimicFT()) return false;
  
  // a little mod help...can only set to positive ft's
  if (!m_world->GetEnvironment().IsTargetID(prop_target)) {
    int num_fts = 0;
    std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
    set <int>::iterator itr;
    for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) num_fts++;
    // ft's may not be sequentially numbered
    int ft_num = abs(prop_target) % num_fts;
    itr = fts_avail.begin();
    for (int i = 0; i < ft_num; i++) itr++;
    prop_target = *itr;
  }
  
  m_organism->SetShowForageTarget(ctx, prop_target);
  setInternalValue(FindModifiedRegister(rBX), prop_target, false);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetForageTarget(Context& ctx)
{
  assert(m_organism != 0);
  const int target_reg = FindModifiedRegister(rBX);
  setInternalValue(target_reg, m_organism->GetForageTarget(), false);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetLocOrgDensity(Context& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  const int worldx = m_world->GetConfig().WORLD_X.Get();
  const int worldy = m_world->GetConfig().WORLD_Y.Get();
  int org_x = 0;
  int org_y = 0;
  int num_neighbors = 0;
  if (!m_use_avatar) {
    num_neighbors = m_organism->GetNeighborhoodSize();
    org_x = m_organism->GetOrgInterface().GetCellID() % worldx;
    org_y = m_organism->GetOrgInterface().GetCellID() / worldx;
  }
  else if (m_use_avatar == 2) {
    num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
    org_x = m_organism->GetOrgInterface().GetAVCellID() % worldx;
    org_y = m_organism->GetOrgInterface().GetAVCellID() / worldx;
  }
  if (num_neighbors == 0) return false;
  
  // look out to 5 cells
  int max_x = org_x + 5;
  if (max_x >= worldx - 1) max_x = worldx - 1;
  int min_x = org_x - 5;
  if (min_x <= 0) min_x = 0;
  int max_y = org_y + 5;
  if (max_y >= worldy - 1) max_y = worldy - 1;
  int min_y = org_y - 5;
  if (min_y <= 0) min_y = 0;
  
  const int x_dist = max_x - min_x + 1;
  const int y_dist = max_y - min_y + 1;
  const int ul = min_y * worldx + min_x;
  int prey_count = 0;
  int pred_count = 0;
  for (int i = 0; i < x_dist; i++) {
    for (int j = 0; j < y_dist; j++) {
      int cellid = ul + i + (j * worldx);
      const cPopulationCell* cell = m_organism->GetOrgInterface().GetCell(cellid);
      if (!m_use_avatar && cell->IsOccupied() && !cell->GetOrganism()->IsDead() && cellid != m_organism->GetOrgInterface().GetCellID()) {
        if (cell->GetOrganism()->IsPreyFT()) prey_count++;
        if (!cell->GetOrganism()->IsPreyFT()) pred_count++;
      }
      else if (m_use_avatar == 2) {
        prey_count += cell->GetNumPreyAV();
        pred_count += cell->GetNumPredAV();
        if (cellid == m_organism->GetOrgInterface().GetAVCellID()) {
          if (m_organism->IsPreyFT()) prey_count--;
          else pred_count--;
        }
      }
    }
  }
  
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, prey_count, true);
  setInternalValue(FindModifiedNextRegister(reg_used), pred_count, true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetFacedOrgDensity(Context&)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  const int worldx = m_world->GetConfig().WORLD_X.Get();
  const int worldy = m_world->GetConfig().WORLD_Y.Get();
  int org_x = 0;
  int org_y = 0;
  int num_neighbors = 0;
  if (!m_use_avatar) {
    num_neighbors = m_organism->GetNeighborhoodSize();
    org_x = m_organism->GetOrgInterface().GetCellID() % worldx;
    org_y = m_organism->GetOrgInterface().GetCellID() / worldx;
  }
  else if (m_use_avatar == 2) {
    num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
    org_x = m_organism->GetOrgInterface().GetAVCellID() % worldx;
    org_y = m_organism->GetOrgInterface().GetAVCellID() / worldx;
  }
  if (num_neighbors == 0) return false;
  
  int facing = m_organism->GetOrgInterface().GetFacedDir();
  if (m_use_avatar) facing = m_organism->GetOrgInterface().GetAVFacing();
  
  int max_x = org_x + 5;
  int min_x = org_x - 5;
  int max_y = org_y + 5;
  int min_y = org_y - 5;
  bool diagonal = false;
  
  if (facing == 0) {
    max_y = org_y;
  }
  else if (facing == 4) {
    min_y = org_y;
  }
  else if (facing == 2) {
    min_x = org_x;
  }
  else if (facing == 6) {
    max_x = org_x;
  }
  else diagonal = true;
  
  if (max_x >= worldx - 1) max_x = worldx - 1;
  if (min_x <= 0) min_x = 0;
  if (max_y >= worldy - 1) max_y = worldy - 1;
  if (min_y <= 0) min_y = 0;
  
  const int x_dist = max_x - min_x + 1;
  const int y_dist = max_y - min_y + 1;
  const int ul = min_y * worldx + min_x;
  int prey_count = 0;
  int pred_count = 0;
  for (int i = 0; i < x_dist; i++) {
    for (int j = 0; j < y_dist; j++) {
      int cellid = ul + i + (j * worldx);
      if (diagonal) {
        int x_offset = (cellid % worldx) - org_x;
        int y_offset = (cellid / worldx) - org_y;
        if (facing == 1 && (y_offset > x_offset)) continue;
        else if (facing == 5 && (x_offset > y_offset)) continue;
        else if (facing == 3 && ((x_offset < 0 && (abs(x_offset) > abs(y_offset) || x_offset == y_offset)) ||
                                 (y_offset < 0 && abs(y_offset) > abs(x_offset)) )) continue;
        else if (facing == 7 && ((x_offset > 0 && (abs(x_offset) > abs(y_offset) || x_offset == y_offset)) ||
                                 (y_offset > 0 && abs(y_offset) > abs(x_offset)) )) continue;
      }
      const cPopulationCell* cell = m_organism->GetOrgInterface().GetCell(cellid);
      if (!m_use_avatar && cell->IsOccupied() && !cell->GetOrganism()->IsDead() && cellid != m_organism->GetOrgInterface().GetCellID()) {
        if (cell->GetOrganism()->IsPreyFT()) prey_count++;
        else if (!cell->GetOrganism()->IsPreyFT()) pred_count++;
      }
      else if (m_use_avatar == 2) {
        prey_count += cell->GetNumPreyAV();
        pred_count += cell->GetNumPredAV();
        if (cellid == m_organism->GetOrgInterface().GetAVCellID()) {
          if (m_organism->IsPreyFT()) prey_count--;
          else pred_count--;
        }
      }
    }
  }
  
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, prey_count, true);
  setInternalValue(FindModifiedNextRegister(reg_used), pred_count, true);
  return true;
}

bool Hardware::Types::HeadsEX::DoActualCollect(Context& ctx, int bin_used, bool unit)
{
  // Set up res_change and max total
  double cell_res = 0.0;
  if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, bin_used);
  else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, bin_used);
  
  Apto::Array<double> res_change(m_world->GetEnvironment().GetResDefLib().GetSize());
  res_change.SetAll(0.0);
  double total = m_organism->GetRBinsTotal();
  double max = m_world->GetConfig().MAX_TOTAL_STORED.Get();
  bool has_max = max > 0 ? true : false;
  double res_consumed = 0.0;
  
  // Collect a unit or some ABSORB_RESOURCE_FRACTION
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  if (unit) {
    double threshold = resource_lib.GetResDef(bin_used)->GetThreshold();
    if (cell_res >= threshold) {
      res_consumed = threshold;
    }
    else {
      return false;
    }
  }
  else {
    res_consumed = cell_res * m_world->GetConfig().ABSORB_RESOURCE_FRACTION.Get();
  }
  
  if (has_max && res_consumed + total >= max) {
    res_consumed = max - total;
    res_change[bin_used] = -1 * res_consumed;
  }
  else res_change[bin_used] = -1 * res_consumed;
  
  if (res_consumed > 0) {
    m_organism->AddToRBin(bin_used, res_consumed);
    if (!m_use_avatar) m_organism->GetOrgInterface().UpdateResources(ctx, res_change);
    else if (m_use_avatar) m_organism->GetOrgInterface().UpdateAVResources(ctx, res_change);
    return true;
  }
  return false;
}

bool Hardware::Types::HeadsEX::FakeActualCollect(Context& ctx, int bin_used, bool unit)
{
  // Set up res_change and max total
  double cell_res = 0.0;
  if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, bin_used);
  else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, bin_used);
  
  Apto::Array<double> res_change(m_world->GetEnvironment().GetResourceLib().GetSize());
  res_change.SetAll(0.0);
  double total = m_organism->GetRBinsTotal();
  double max = m_world->GetConfig().MAX_TOTAL_STORED.Get();
  bool has_max = max > 0 ? true : false;
  double res_consumed = 0.0;
  
  // Collect a unit or some ABSORB_RESOURCE_FRACTION
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  if (unit) {
    double threshold = resource_lib.GetResource(bin_used)->GetThreshold();
    if (cell_res >= threshold) {
      res_consumed = threshold;
    }
    else {
      return false;
    }
  }
  else {
    res_consumed = cell_res * m_world->GetConfig().ABSORB_RESOURCE_FRACTION.Get();
  }
  
  if (has_max && res_consumed + total >= max) {
    res_consumed = max - total;
    res_change[bin_used] = -1 * res_consumed;
  }
  else res_change[bin_used] = -1 * res_consumed;
  
  if (res_consumed > 0) {
    //org does not get the resource but it is taken from environment
    if (!m_use_avatar) m_organism->GetOrgInterface().UpdateResources(ctx, res_change);
    else if (m_use_avatar) m_organism->GetOrgInterface().UpdateAVResources(ctx, res_change);
    return true;
  }
  return false;
}


bool Hardware::Types::HeadsEX::Inst_CollectEdible(Context& ctx)
{
  int absorb_type = m_world->GetConfig().MULTI_ABSORB_TYPE.Get();
  
  double cell_res = 0.0;
  int res_id = -1;
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  if (absorb_type == 1) {
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResDef(i)->GetHabitat() == 0 || resource_lib.GetResDef(i)->GetHabitat() > 5) {
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
        if (cell_res >= resource_lib.GetResDef(i)->GetThreshold()) {
          res_id = i;
          break;
        }
      }
    }
  }
  else if (absorb_type == 2) {
    for (int i = resource_lib.GetSize(); i > 0 ; i--) {
      if (resource_lib.GetResDef(i - 1)->GetHabitat() == 0 || resource_lib.GetResDef(i - 1)->GetHabitat() > 5) {
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i - 1);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i - 1);
        if (cell_res >= resource_lib.GetResDef(i - 1)->GetThreshold()) {
          res_id = i - 1;
          break;
        }
      }
    }
  }
  else m_world->GetDriver().Feedback().Error("Instruction collect-edible only operational for MULTI_ABSORB_TYPE 1 or 2");
  bool success = false;
  if (res_id != -1) success = DoActualCollect(ctx, res_id, true);
  return success;
}

bool Hardware::Types::HeadsEX::Inst_CollectSpecific(Context& ctx)
{
  const int resource = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  double res_before = m_organism->GetRBin(resource);
  bool success = DoActualCollect(ctx, resource, false);
  double res_after = m_organism->GetRBin(resource);
  int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, (int)(res_after - res_before), true);
  setInternalValue(FindModifiedNextRegister(out_reg), (int)(res_after), true);
  return success;
}

bool Hardware::Types::HeadsEX::Inst_DepositResource(Context& ctx)
{
  int resource_amount = abs(GetRegister(FindModifiedNextRegister(rBX)));
  int resource_id = GetRegister(FindModifiedRegister(rBX));
  resource_id %= m_organism->GetRBins().GetSize();
  const double stored_res = m_organism->GetRBins()[resource_id];
  
  if (resource_amount > stored_res) resource_amount = (int)(stored_res);
  bool success = false;
  if (stored_res >= resource_amount && resource_amount > 0) {
    const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
    // only allow deposits on dens
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResDef(i)->GetHabitat() == 4) {
        double cell_res = 0.0;
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
        if (cell_res > resource_lib.GetResDef(i)->GetThreshold()) {
          m_organism->AddToRBin(resource_id, -1 * resource_amount);
          Apto::Array<double> res_change(resource_lib.GetSize());
          res_change.SetAll(0.0);
          res_change[resource_id] = resource_amount;
          if (!m_use_avatar) m_organism->GetOrgInterface().UpdateResources(ctx, res_change);
          else if (m_use_avatar) m_organism->GetOrgInterface().UpdateAVResources(ctx, res_change);
          success = true;
          break;
        }
      }
    }
  }
  double res_after = m_organism->GetRBin(resource_id);
  int out_reg = FindModifiedRegister(rBX);
  if (success) setInternalValue(out_reg, (int)(res_after), true);
  else if (!success && resource_amount > 0) setInternalValue(out_reg, -1, true);
  return success;
}

bool Hardware::Types::HeadsEX::Inst_DepositSpecific(Context& ctx)
{
  int resource_amount = GetRegister(FindModifiedRegister(rBX));
  const int spec_res = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  const double stored_spec = m_organism->GetRBins()[spec_res];
  
  if (resource_amount > stored_spec) resource_amount = (int)(stored_spec);
  bool success = false;
  if (stored_spec >= resource_amount && resource_amount > 0) {
    const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
    // only allow deposits on dens
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResDef(i)->GetHabitat() == 4) {
        double cell_res = 0.0;
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
        if (cell_res > resource_lib.GetResDef(i)->GetThreshold()) {
          m_organism->AddToRBin(spec_res, -1 * resource_amount);
          Apto::Array<double> res_change(resource_lib.GetSize());
          res_change.SetAll(0.0);
          res_change[spec_res] = resource_amount;
          if (!m_use_avatar) m_organism->GetOrgInterface().UpdateResources(ctx, res_change);
          else if (m_use_avatar) m_organism->GetOrgInterface().UpdateAVResources(ctx, res_change);
          success = true;
          break;
        }
      }
    }
  }
  double res_after = m_organism->GetRBin(spec_res);
  int out_reg = FindModifiedRegister(rBX);
  if (success) setInternalValue(out_reg, (int)(res_after), true);
  else if (!success && resource_amount > 0) setInternalValue(out_reg, -1, true);
  return success;
}

bool Hardware::Types::HeadsEX::Inst_DepositAllAsSpecific(Context& ctx)
{
  const int spec_res = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  bool success = false;
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  // only allow deposits on dens
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResDef(i)->GetHabitat() == 4) {
      double cell_res = 0.0;
      if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
      else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
      if (cell_res > resource_lib.GetResDef(i)->GetThreshold()) {
        double total_deposit = 0.0;
        for (int j = 0; j < resource_lib.GetSize(); j++) {
          double resource_amount = m_organism->GetRBins()[j];
          m_organism->AddToRBin(j, -1 * resource_amount);
          total_deposit += resource_amount;
        }
        if (total_deposit > 0) {
          Apto::Array<double> res_change(resource_lib.GetSize());
          res_change.SetAll(0.0);
          res_change[spec_res] = total_deposit;
          if (!m_use_avatar) m_organism->GetOrgInterface().UpdateResources(ctx, res_change);
          else if (m_use_avatar) m_organism->GetOrgInterface().UpdateAVResources(ctx, res_change);
          success = true;
          m_organism->IncNumDeposits();
          m_organism->IncAmountDeposited(total_deposit);
        }
        break;
      }
    }
  }
  int out_reg = FindModifiedRegister(rBX);
  if (success) setInternalValue(out_reg, 1, true);
  else if (!success) setInternalValue(out_reg, -1, true);
  return success;
}

bool Hardware::Types::HeadsEX::Inst_NopDepositSpecific(Context& ctx)
{
  int resource_amount = GetRegister(FindModifiedRegister(rBX));
  const int spec_res = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  const double stored_spec = m_organism->GetRBins()[spec_res];
  
  if (resource_amount > stored_spec) resource_amount = (int)(stored_spec);
  bool success = false;
  if (stored_spec >= resource_amount && resource_amount > 0) {
    const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
    // only allow deposits on dens
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResDef(i)->GetHabitat() == 4) {
        double cell_res = 0.0;
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
        if (cell_res > resource_lib.GetResDef(i)->GetThreshold()) {
          success = true;
          break;
        }
      }
    }
  }
  return success;
}

bool Hardware::Types::HeadsEX::Inst_NopDepositResource(Context& ctx)
{
  int resource_amount = abs(GetRegister(FindModifiedNextRegister(rBX)));
  int resource_id = GetRegister(FindModifiedRegister(rBX));
  resource_id %= m_organism->GetRBins().GetSize();
  const double stored_res = m_organism->GetRBins()[resource_id];
  
  if (resource_amount > stored_res) resource_amount = (int)(stored_res);
  bool success = false;
  if (stored_res >= resource_amount && resource_amount > 0) {
    const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
    // only allow deposits on dens
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResDef(i)->GetHabitat() == 4) {
        double cell_res = 0.0;
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
        if (cell_res > resource_lib.GetResDef(i)->GetThreshold()) {
          success = true;
          break;
        }
      }
    }
  }
  return success;
}

bool Hardware::Types::HeadsEX::Inst_NopDepositAllAsSpecific(Context& ctx)
{
  bool success = false;
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  // only allow deposits on dens
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResDef(i)->GetHabitat() == 4) {
      double cell_res = 0.0;
      if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
      else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
      if (cell_res > resource_lib.GetResDef(i)->GetThreshold()) {
        double total_deposit = 0.0;
        for (int j = 0; j < resource_lib.GetSize(); j++) {
          double resource_amount = m_organism->GetRBins()[j];
          total_deposit += resource_amount;
        }
        if (total_deposit > 0) {
          success = true;
        }
        break;
      }
    }
  }
  return success;
}

bool Hardware::Types::HeadsEX::Inst_Nop2DepositAllAsSpecific(Context& ctx)
{
  const int spec_res = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  bool success = false;
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  // only allow deposits on dens
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 4) {
      double cell_res = 0.0;
      if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
      else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
      if (cell_res > resource_lib.GetResource(i)->GetThreshold()) {
        double total_deposit = 0.0;
        for (int j = 0; j < resource_lib.GetSize(); j++) {
          double resource_amount = m_organism->GetRBins()[j];
          m_organism->AddToRBin(j, -1 * resource_amount);
          // resource doesn't get added to total_deposit
        }
        if (total_deposit > 0) {
          Apto::Array<double> res_change(resource_lib.GetSize());
          res_change.SetAll(0.0);
          res_change[spec_res] = total_deposit;
          if (!m_use_avatar) m_organism->GetOrgInterface().UpdateResources(ctx, res_change);
          else if (m_use_avatar) m_organism->GetOrgInterface().UpdateAVResources(ctx, res_change);
          success = true;
          m_organism->IncNumDeposits();
          m_organism->IncAmountDeposited(total_deposit);
        }
        break;
      }
    }
  }
  int out_reg = FindModifiedRegister(rBX);
  if (success) setInternalValue(out_reg, 1, true);
  else if (!success) setInternalValue(out_reg, -1, true);
  return success;
}

bool Hardware::Types::HeadsEX::Inst_Nop2CollectEdible(Context& ctx)
{
  int absorb_type = m_world->GetConfig().MULTI_ABSORB_TYPE.Get();
  
  double cell_res = 0.0;
  int res_id = -1;
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  if (absorb_type == 1) {
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->GetHabitat() == 0 || resource_lib.GetResource(i)->GetHabitat() > 5) {
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
        if (cell_res >= resource_lib.GetResource(i)->GetThreshold()) {
          res_id = i;
          break;
        }
      }
    }
  }
  else if (absorb_type == 2) {
    for (int i = resource_lib.GetSize(); i > 0 ; i--) {
      if (resource_lib.GetResource(i - 1)->GetHabitat() == 0 || resource_lib.GetResource(i - 1)->GetHabitat() > 5) {
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i - 1);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i - 1);
        if (cell_res >= resource_lib.GetResource(i - 1)->GetThreshold()) {
          res_id = i - 1;
          break;
        }
      }
    }
  }
  else m_world->GetDriver().Feedback().Error("Instruction collect-edible only operational for MULTI_ABSORB_TYPE 1 or 2");
  bool success = false;
  if (res_id != -1) success = FakeActualCollect(ctx, res_id, true);
  return success;
}


bool Hardware::Types::HeadsEX::Inst_NopCollectEdible(Context& ctx)
{
  int absorb_type = m_world->GetConfig().MULTI_ABSORB_TYPE.Get();
  int res_id = -1;
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  double cell_res = 0.0;
  if (absorb_type == 1) {
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResDef(i)->GetHabitat() == 0 || resource_lib.GetResDef(i)->GetHabitat() > 5) {
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
        if (cell_res >= resource_lib.GetResDef(i)->GetThreshold()) {
          res_id = i;
          break;
        }
      }
    }
  }
  else if (absorb_type == 2) {
    for (int i = resource_lib.GetSize(); i > 0 ; i--) {
      if (resource_lib.GetResDef(i - 1)->GetHabitat() == 0 || resource_lib.GetResDef(i - 1)->GetHabitat() > 5) {
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i - 1);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i - 1);
        if (cell_res >= resource_lib.GetResDef(i - 1)->GetThreshold()) {
          res_id = i - 1;
          break;
        }
      }
    }
  }
  
  if (res_id != -1) {
    double total = m_organism->GetRBinsTotal();
    double max = m_world->GetConfig().MAX_TOTAL_STORED.Get();
    bool has_max = max > 0 ? true : false;
    double res_consumed = 0.0;
    
    double threshold = resource_lib.GetResDef(res_id)->GetThreshold();
    if (cell_res >= threshold) res_consumed = threshold;
    else return false;
    
    if (has_max && res_consumed + total >= max) res_consumed = max - total;
    
    if (res_consumed > 0) return true;
  }
  return false;
}



bool Hardware::Types::HeadsEX::Inst_GetResStored(Context& ctx)
{
  int resource_id = abs(GetRegister(FindModifiedRegister(rBX)));
  Apto::Array<double> bins = m_organism->GetRBins();
  resource_id %= bins.GetSize();
  int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, (int)(bins[resource_id]), true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_GetSpecificStored(Context& ctx)
{
  Apto::Array<double> bins = m_organism->GetRBins();
  int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, (int)(bins[m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get()]), true);
  return true;
}





bool Hardware::Types::HeadsEX::Inst_GetFacedOrgID(Context& ctx)
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
  setInternalValue(out_reg, neighbor->GetID(), true);
  return true;
}

//Attack organism faced by this one, if there is non-predator target in front, and steal it's merit, current bonus, and reactions.
bool Hardware::Types::HeadsEX::Inst_AttackPrey(Context& ctx)
{
  sAttackResult results;
  results.inst = 0;
  results.share = 0;
  results.success = 0;
  results.size = 0;
  if (!TestAttack(ctx)) { results.success = 1; return TestAttackResultsOut(results); }
  cOrganism* target = GetPreyTarget(ctx);
  if (!TestPreyTarget(target)) { results.success = 1; return TestAttackResultsOut(results); }
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecuteAttack(ctx, target, reg)) results.success = 3;
  return TestAttackResultsOut(results);
}

bool Hardware::Types::HeadsEX::Inst_AttackPreyArea(Context& ctx)
{
  sAttackResult results;
  results.inst = 0;
  results.share = 0;
  results.success = 0;
  results.size = 0;
  if (!TestAttack(ctx)) { results.success = 1; return TestAttackResultsOut(results); }
  
  int prey_count = 0;
  Apto::Array<int> neighborhood;
  if (!m_use_avatar) {
    prey_count++; // self
    m_organism->GetOrgInterface().GetNeighborhoodCellIDs(neighborhood);
    for (int j = 0; j < neighborhood.GetSize(); j++) {
      if (m_organism->GetOrgInterface().GetCell(neighborhood[j])->IsOccupied() &&
          !m_organism->GetOrgInterface().GetCell(neighborhood[j])->GetOrganism()->IsDead()) {
        if (m_organism->GetOrgInterface().GetCell(neighborhood[j])->GetOrganism()->IsPreyFT()) prey_count++;
      }
    }
  }
  else {
    prey_count += m_organism->GetOrgInterface().GetCell(m_organism->GetOrgInterface().GetAVCellID())->GetNumPreyAV(); // self cell
    m_organism->GetOrgInterface().GetAVNeighborhoodCellIDs(neighborhood);
    for (int j = 0; j < neighborhood.GetSize(); j++) {
      prey_count += m_organism->GetOrgInterface().GetCell(neighborhood[j])->GetNumPreyAV();
    }
  }
  
  double odds = 1.0 / ((double) (prey_count));
  
  cOrganism* target = GetPreyTarget(ctx);
  if (!TestPreyTarget(target)) { results.success = 1; return TestAttackResultsOut(results); }
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecuteAttack(ctx, target, reg, odds)) results.success = 3;
  return TestAttackResultsOut(results);
}



bool Hardware::Types::HeadsEX::Inst_AttackPreyShare(Context& ctx)
{
  sAttackResult results;
  results.inst = 1;
  results.share = 1;
  results.success = 0;
  results.size = 0;
  if (!TestAttack(ctx))  { results.success = 1; return TestAttackResultsOut(results); }
  cOrganism* target = GetPreyTarget(ctx);
  if (!TestPreyTarget(target))  { results.success = 1; return TestAttackResultsOut(results); }
  
  Apto::Array<cOrganism*> pack = GetPredGroupAttackNeighbors();
  
  double odds = m_world->GetConfig().PRED_ODDS.Get();
  results.size = pack.GetSize();
  if (results.size <= 1) { results.success = 2; return TestAttackResultsOut(results); }
  if (results.size > 1) odds += (odds * results.size); // 1 friend = 20%, 8 friends = 90%
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecuteShareAttack(ctx, target, reg, pack, odds)) results.success = 3;
  else {
    cString inst = "attack-prey-share";
    UpdateGroupAttackStats(inst, results, false);
  }
  return TestAttackResultsOut(results);
}

bool Hardware::Types::HeadsEX::Inst_AttackPreyNoShare(Context& ctx)
{
  sAttackResult results;
  results.inst = 1;
  results.share = 0;
  results.success = 0;
  results.size = 0;
  if (!TestAttack(ctx))  { results.success = 1; return TestAttackResultsOut(results); }
  
  cOrganism* target = GetPreyTarget(ctx);
  if (!TestPreyTarget(target))  { results.success = 1; return TestAttackResultsOut(results); }
  
  double odds = m_world->GetConfig().PRED_ODDS.Get();
  results.size = GetPredGroupAttackNeighbors().GetSize();
  if (results.size <= 1) { results.success = 2; return TestAttackResultsOut(results); }
  if (results.size > 1) odds += (odds * results.size); // 1 friend = 20%, 8 friends = 90%
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecuteAttack(ctx, target, reg, odds)) results.success = 3;
  else {
    cString inst = "attack-prey-no-share";
    UpdateGroupAttackStats(inst, results, false);
  }
  return TestAttackResultsOut(results);
}

bool Hardware::Types::HeadsEX::Inst_AttackPreyFakeShare(Context& ctx)
{
  sAttackResult results;
  results.inst = 1;
  results.share = 2;
  results.success = 0;
  results.size = 0;
  if (!TestAttack(ctx))  { results.success = 1; return TestAttackResultsOut(results); }
  
  cOrganism* target = GetPreyTarget(ctx);
  if (!TestPreyTarget(target))  { results.success = 1; return TestAttackResultsOut(results); }
  
  double odds = m_world->GetConfig().PRED_ODDS.Get();
  results.size = GetPredGroupAttackNeighbors().GetSize();
  if (results.size <= 1) { results.success = 2; return TestAttackResultsOut(results); }
  if (results.size > 1) odds += (odds * results.size); // 1 friend = 20%, 8 friends = 90%
  double share = 1.0 / (double) results.size;
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecuteFakeShareAttack(ctx, target, reg, share, odds)) results.success = 3;
  else {
    cString inst = "attack-prey-fake-share";
    UpdateGroupAttackStats(inst, results, false);
  }
  return TestAttackResultsOut(results);
}



bool Hardware::Types::HeadsEX::Inst_AttackSpecPrey(Context& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  const int id_sought_reg = FindModifiedRegister(rBX);
  const int id_sought = m_threads[m_cur_thread].reg[id_sought_reg].value;
  bool have_org2use = false;
  
  // return false if invalid number or self
  if (id_sought < 0 || id_sought == m_organism->GetID()) return false;
  
  // if valid number, does the value represent a living organism?
  cOrganism* target  = NULL;
  const Apto::Array <cOrganism*, Apto::Smart> live_orgs = m_organism->GetOrgInterface().GetLiveOrgList();
  for (int i = 0; i < live_orgs.GetSize(); i++) {
    cOrganism* org = live_orgs[i];
    if (id_sought == org->GetID()) {
      target = org;
      have_org2use = true;
      break;
    }
  }
  if (!have_org2use) return false;
  
  if (!m_use_avatar) { if (target != m_organism->GetOrgInterface().GetNeighbor())  return false; }
  else if (m_use_avatar == 2) { if (target->GetCellID() != m_organism->GetOrgInterface().GetAVFacedCellID())  return false; }
  
  if (!TestPreyTarget(target)) return false;
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecuteAttack(ctx, target, reg, 1)) return false;
  return true;
}

bool Hardware::Types::HeadsEX::Inst_AttackFTPrey(Context& ctx)
{
  sAttackResult results;
  results.inst = 0;
  results.share = 0;
  results.success = 0;
  results.size = 0;
  if (!TestAttack(ctx))  { results.success = 1; return TestAttackResultsOut(results); }
  
  const int target_reg = FindModifiedRegister(rBX);
  int target_org_type = m_threads[m_cur_thread].reg[target_reg].value;
  cOrganism* target = NULL;
  if (!m_use_avatar) {
    target = m_organism->GetOrgInterface().GetNeighbor();
    if (target_org_type != target->GetForageTarget())  { results.success = 1; return TestAttackResultsOut(results); }
    // attacking other carnivores is handled differently (e.g. using fights or tolerance)
    if (!target->IsPreyFT())  { results.success = 1; return TestAttackResultsOut(results); }
  }
  else if (m_use_avatar == 2) {
    const Apto::Array<cOrganism*>& av_neighbors = m_organism->GetOrgInterface().GetFacedPreyAVs();
    bool target_match = false;
    int rand_index = ctx.GetRandom().GetUInt(0, av_neighbors.GetSize());
    int j = 0;
    for (int i = 0; i < av_neighbors.GetSize(); i++) {
      if (rand_index + i < av_neighbors.GetSize()) {
        if (av_neighbors[rand_index + i]->GetForageTarget() == target_org_type) {
          target = av_neighbors[rand_index + i];
          target_match = true;
        }
        break;
      }
      else {
        if (av_neighbors[j]->GetForageTarget() == target_org_type) {
          target = av_neighbors[j];
          target_match = true;
        }
        break;
        j++;
      }
    }
    if (!target_match)  { results.success = 1; return TestAttackResultsOut(results); }
  }
  if (!TestPreyTarget(target))  { results.success = 1; return TestAttackResultsOut(results); }
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecuteAttack(ctx, target, reg)) results.success = 3;
  else {
    cString inst = "attack-ft-prey";
    UpdateGroupAttackStats(inst, results, true);
  }
  return TestAttackResultsOut(results);
}

bool Hardware::Types::HeadsEX::Inst_AttackPoisonPrey(Context& ctx)
{
  if (!TestAttack(ctx)) return false;
  
  cOrganism* target = GetPreyTarget(ctx);
  int target_ft = target->GetForageTarget();
  if (!TestPreyTarget(target)) return false;
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecutePoisonPreyAttack(ctx, target, reg)) return false;
  m_organism->GetPhenotype().IncAttackedPreyFTData(target_ft);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_AttackPoisonFTPrey(Context& ctx)
{
  sAttackResult results;
  results.inst = 0;
  results.share = 0;
  results.success = 0;
  results.size = 0;
  if (!TestAttack(ctx)) return false;
  
  const int target_reg = FindModifiedRegister(rBX);
  int target_org_type = m_threads[m_cur_thread].reg[target_reg].value;
  cOrganism* target = NULL;
  if (!m_use_avatar) {
    target = m_organism->GetOrgInterface().GetNeighbor();
    if (target_org_type != target->GetShowForageTarget()) return false;
    // attacking other carnivores is handled differently (e.g. using fights or tolerance)
    if (!target->IsPreyFT())  { results.success = 1; return TestAttackResultsOut(results); }
  }
  else if (m_use_avatar == 2) {
    const Apto::Array<cOrganism*>& av_neighbors = m_organism->GetOrgInterface().GetFacedPreyAVs();
    bool target_match = false;
    int rand_index = ctx.GetRandom().GetUInt(0, av_neighbors.GetSize());
    int j = 0;
    for (int i = 0; i < av_neighbors.GetSize(); i++) {
      if (rand_index + i < av_neighbors.GetSize()) {
        if (av_neighbors[rand_index + i]->GetShowForageTarget() == target_org_type) {
          target = av_neighbors[rand_index + i];
          target_match = true;
        }
        break;
      }
      else {
        if (av_neighbors[j]->GetShowForageTarget() == target_org_type) {
          target = av_neighbors[j];
          target_match = true;
        }
        break;
        j++;
      }
    }
    if (!target_match) return false;
  }
  if (!TestPreyTarget(target)) return false;
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecutePoisonPreyAttack(ctx, target, reg)) return false;
  m_organism->GetPhenotype().IncAttackedPreyFTData(target->GetForageTarget());
  return true;
}

bool Hardware::Types::HeadsEX::Inst_AttackPoisonFTPreyGenetic(Context& ctx)
{
  sAttackResult results;
  results.inst = 0;
  results.share = 0;
  results.success = 0;
  results.size = 0;
  if (!TestAttack(ctx)) return false;
  
  int target_org_type = -1;
  bool accept_any_target = true;
  // If followed by a nop, use nop to set target
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    target_org_type = m_inst_set->GetNopMod(getIP().GetNextInst()) % 3;
    accept_any_target = false;
  }
  cOrganism* target = NULL;
  if (!m_use_avatar) {
    target = m_organism->GetOrgInterface().GetNeighbor();
    if (target_org_type != target->GetShowForageTarget() && !accept_any_target) return false;
    // attacking other carnivores is handled differently (e.g. using fights or tolerance)
    if (!target->IsPreyFT())  { results.success = 1; return TestAttackResultsOut(results); }
  }
  else if (m_use_avatar == 2) {
    const Apto::Array<cOrganism*>& av_neighbors = m_organism->GetOrgInterface().GetFacedPreyAVs();
    bool target_match = false;
    int rand_index = ctx.GetRandom().GetUInt(0, av_neighbors.GetSize());
    int j = 0;
    for (int i = 0; i < av_neighbors.GetSize(); i++) {
      if (rand_index + i < av_neighbors.GetSize()) {
        if (av_neighbors[rand_index + i]->GetShowForageTarget() == target_org_type || accept_any_target) {
          target = av_neighbors[rand_index + i];
          target_match = true;
        }
        break;
      }
      else {
        if (av_neighbors[j]->GetShowForageTarget() == target_org_type || accept_any_target) {
          target = av_neighbors[j];
          target_match = true;
        }
        break;
        j++;
      }
    }
    if (!target_match) return false;
  }
  if (!TestPreyTarget(target)) return false;
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecutePoisonPreyAttack(ctx, target, reg)) return false;
  m_organism->GetPhenotype().IncAttackedPreyFTData(target->GetForageTarget());
  return true;
}

bool Hardware::Types::HeadsEX::Inst_AttackPoisonFTMixedPrey(Context& ctx)
{
  sAttackResult results;
  results.inst = 0;
  results.share = 0;
  results.success = 0;
  results.size = 0;
  if (!m_world->GetStats().GetNumPreyCreatures()) return false;
  
  const int target_reg = FindModifiedRegister(rBX);
  int ft_sought = 0;
  bool rand_ft = true;
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    ft_sought = m_threads[m_cur_thread].reg[target_reg].value;
    rand_ft = false;
  }
  if (ft_sought <= -1 || !m_world->GetEnvironment().IsTargetID(ft_sought)) return false;
  
  cOrganism* target = NULL;
  bool have_org2use = false;
  const Apto::Array<cOrganism*, Apto::Smart>& live_orgs = m_organism->GetOrgInterface().GetLiveOrgList();
  
  Apto::Array<bool> used_orgs;
  used_orgs.Resize(live_orgs.GetSize());
  used_orgs.SetAll(false);
  
  int num_checked = 0;
  while (num_checked < live_orgs.GetSize()) {
    int this_rand_idx = ctx.GetRandom().GetInt(0, live_orgs.GetSize());
    if (!used_orgs[this_rand_idx]) {
      cOrganism* org = live_orgs[this_rand_idx];
      if ((!rand_ft && (ft_sought == org->GetShowForageTarget() || (org->GetForageTarget() == -1 && ft_sought == org->GetParentFT()))) || (rand_ft && org->IsPreyFT())) {
        target = org;
        have_org2use = true;
        break;
      }
      used_orgs[this_rand_idx] = true;
      num_checked++;
    }
  }
  /* not actually a random walk thru org list
   for (int i = 0; i < live_orgs.GetSize(); i++) {
   cOrganism* org = live_orgs[i];
   if ((!rand_ft && ft_sought == org->GetShowForageTarget()) || (rand_ft && ft_sought == org->IsPreyFT())) {
   target = org;
   have_org2use = true;
   break;
   }
   }
   */
  
  if (!have_org2use) return false;
  else if (!TestPreyTarget(target)) return false;
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!ExecutePoisonPreyAttack(ctx, target, reg)) return false;
  m_organism->GetPhenotype().IncAttackedPreyFTData(target->GetForageTarget());
  return true;
}

//Attack organism faced by this one if you are both predators or both prey.
bool Hardware::Types::HeadsEX::Inst_FightMeritOrg(Context& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) {
    if (!m_organism->IsNeighborCellOccupied()) return false;
    target = m_organism->GetOrgInterface().GetNeighbor();
    // allow only for predator vs predator or prey vs prey
    if ((!target->IsPreyFT() && m_organism->IsPreyFT()) ||
        (target->IsPreyFT() && !m_organism->IsPreyFT())) {
      return false;
    }
  }
  else if (m_use_avatar == 2) {
    if (!m_organism->GetOrgInterface().FacedHasAV()) return false;
    if (m_organism->IsPreyFT()) {
      if (!m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPreyAV();
    }
    else if (!m_organism->IsPreyFT()) {
      if (!m_organism->GetOrgInterface().FacedHasPredAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPredAV();
    }
  }
  if (target->IsDead()) return false;
  
  
  //Use merit to decide who wins this battle.
  bool kill_attacker = true;
  
  const double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
  const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
  const double attacker_win_odds = ((attacker_merit) / (attacker_merit + target_merit));
  const double target_win_odds = ((target_merit) / (attacker_merit + target_merit));
  
  const double odds_someone_dies = max(attacker_win_odds, target_win_odds);
  const double odds_target_dies = (1 - target_win_odds) * odds_someone_dies;
  const double decider = ctx.GetRandom().GetDouble(1);
  
  if (decider < (1 - odds_someone_dies)) return true;
  else if (decider < ((1 - odds_someone_dies) + odds_target_dies)) kill_attacker = false;
  
  if (kill_attacker) {
    m_organism->Die(ctx);
    return true;
  }
  
  const int target_cell = target->GetOrgInterface().GetCellID();
  m_organism->GetOrgInterface().AttackFacedOrg(ctx, target_cell);
  
  bool attack_success = true;
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, attack_success, true);
  
  return true;
}

//Attack organism faced by this one if you are both predators or both prey.
bool Hardware::Types::HeadsEX::Inst_FightBonusOrg(Context& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) {
    if (!m_organism->IsNeighborCellOccupied()) return false;
    target = m_organism->GetOrgInterface().GetNeighbor();
    // allow only for predator vs predator or prey vs prey
    if ((!target->IsPreyFT() && m_organism->IsPreyFT()) ||
        (target->IsPreyFT() && !m_organism->IsPreyFT())) {
      return false;
    }
  }
  else if (m_use_avatar == 2) {
    if (!m_organism->GetOrgInterface().FacedHasAV()) return false;
    if (m_organism->IsPreyFT()) {
      if (!m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPreyAV();
    }
    else if (!m_organism->IsPreyFT()) {
      if (!m_organism->GetOrgInterface().FacedHasPredAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPredAV();
    }
  }
  if (target->IsDead()) return false;
  
  
  //Use current bonus to decide who wins this battle.
  bool kill_attacker = true;
  
  const double attacker_bonus = m_organism->GetPhenotype().GetCurBonus();
  const double target_bonus = target->GetPhenotype().GetCurBonus();
  const double attacker_win_odds = ((attacker_bonus) / (attacker_bonus + target_bonus));
  const double target_win_odds = ((target_bonus) / (attacker_bonus + target_bonus));
  
  const double odds_someone_dies = max(attacker_win_odds, target_win_odds);
  const double odds_target_dies = (1 - target_win_odds) * odds_someone_dies;
  const double decider = ctx.GetRandom().GetDouble(1);
  
  if (decider < (1 - odds_someone_dies)) return true;
  else if (decider < ((1 - odds_someone_dies) + odds_target_dies)) kill_attacker = false;
  
  if (kill_attacker) {
    m_organism->Die(ctx);
    return true;
  }
  
  const int target_cell = target->GetOrgInterface().GetCellID();
  m_organism->GetOrgInterface().AttackFacedOrg(ctx, target_cell);
  
  bool attack_success = true;
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, attack_success, true);
  
  return true;
}

//Get odds of winning or tieing in a fight.
bool Hardware::Types::HeadsEX::Inst_GetMeritFightOdds(Context&)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) {
    if (!m_organism->IsNeighborCellOccupied()) return false;
    target = m_organism->GetOrgInterface().GetNeighbor();
    // allow only for predator vs predator or prey vs prey
    if ((!target->IsPreyFT() && m_organism->IsPreyFT()) ||
        (target->IsPreyFT() && !m_organism->IsPreyFT())) {
      return false;
    }
  }
  else if (m_use_avatar == 2) {
    if (!m_organism->GetOrgInterface().FacedHasAV()) return false;
    if (m_organism->IsPreyFT()) {
      if (!m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPreyAV();
    }
    else if (!m_organism->IsPreyFT()) {
      if (!m_organism->GetOrgInterface().FacedHasPredAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPredAV();
    }
  }
  if (target->IsDead()) return false;
  
  const double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
  const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
  const double attacker_win_odds = ((attacker_merit) / (attacker_merit + target_merit));
  const double target_win_odds = ((target_merit) / (attacker_merit + target_merit));
  
  const double odds_someone_dies = max(attacker_win_odds, target_win_odds);
  // my win odds are odds nobody dies or someone dies and it's the target
  const double odds_I_dont_die = (1 - odds_someone_dies) + ((1 - target_win_odds) * odds_someone_dies);
  
  // return odds out of 10
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, (int) (odds_I_dont_die * 10 + 0.5), true);
  
  return true;
}

//Attack organism faced by this one if you are both predators or both prey.
bool Hardware::Types::HeadsEX::Inst_FightOrg(Context& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) {
    if (!m_organism->IsNeighborCellOccupied()) return false;
    target = m_organism->GetOrgInterface().GetNeighbor();
    // allow only for predator vs predator or prey vs prey
    if ((!target->IsPreyFT() && m_organism->IsPreyFT()) ||
        (target->IsPreyFT() && !m_organism->IsPreyFT())) {
      return false;
    }
  }
  else if (m_use_avatar == 2) {
    if (!m_organism->GetOrgInterface().FacedHasAV()) return false;
    if (m_organism->IsPreyFT()) {
      if (!m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPreyAV();
    }
    else if (!m_organism->IsPreyFT()) {
      if (!m_organism->GetOrgInterface().FacedHasPredAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPredAV();
    }
  }
  if (target->IsDead()) return false;
  
  int target_cell = target->GetOrgInterface().GetCellID();
  
  m_organism->GetOrgInterface().AttackFacedOrg(ctx, target_cell);
  
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, 1, true);
  
  return true;
}

bool Hardware::Types::HeadsEX::Inst_AttackPred(Context& ctx)
{
  assert(m_organism != 0);
  if (!TestAttackPred(ctx)) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) target = m_organism->GetOrgInterface().GetNeighbor();
  else if (m_use_avatar == 2) target = m_organism->GetOrgInterface().GetRandFacedPredAV();
  
  if (target->IsPreyFT() || m_organism->IsPreyFT()) return false;
  if (target->IsDead()) return false;
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!TestAttackChance(ctx, target, reg)) return false;
  else {
    if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
      const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
      double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
      attacker_merit += target_merit * m_world->GetConfig().PRED_EFFICIENCY.Get();
      m_organism->UpdateMerit(attacker_merit);
    }
    
    Apto::Array<int> target_reactions = target->GetPhenotype().GetLastReactionCount();
    Apto::Array<int> org_reactions = m_organism->GetPhenotype().GetStolenReactionCount();
    for (int i = 0; i < org_reactions.GetSize(); i++) {
      m_organism->GetPhenotype().SetStolenReactionCount(i, org_reactions[i] + target_reactions[i]);
    }
    
    const double target_bonus = target->GetPhenotype().GetCurBonus();
    m_organism->GetPhenotype().SetCurBonus(m_organism->GetPhenotype().GetCurBonus() + (target_bonus * m_world->GetConfig().PRED_EFFICIENCY.Get()));
    
    if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
      Apto::Array<double> target_bins = target->GetRBins();
      for (int i = 0; i < target_bins.GetSize(); i++) {
        m_organism->AddToRBin(i, target_bins[i] * m_world->GetConfig().PRED_EFFICIENCY.Get());
        target->AddToRBin(i, -1 * (target_bins[i] * m_world->GetConfig().PRED_EFFICIENCY.Get()));
      }
      const int spec_bin = (int) (m_organism->GetRBins()[m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get()]);
      setInternalValue(reg.bin_reg, spec_bin, true);
    }
    
    target->Die(ctx); // kill first -- could end up being killed by MAX_PRED if parent was pred
    MakeTopPred(ctx);
    
    setInternalValue(reg.success_reg, 1, true);
    setInternalValue(reg.bonus_reg, (int) (target_bonus), true);
  }
  return true;
}

//Kill (don't consume) organism faced by this one if you are both predators.
bool Hardware::Types::HeadsEX::Inst_KillPred(Context& ctx)
{
  assert(m_organism != 0);
  if (!TestAttackPred(ctx)) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) target = m_organism->GetOrgInterface().GetNeighbor();
  else if (m_use_avatar == 2) target = m_organism->GetOrgInterface().GetRandFacedPredAV();
  
  if (target->IsPreyFT() || m_organism->IsPreyFT()) return false;
  if (target->IsDead()) return false;
  
  sAttackReg reg;
  SetAttackReg(reg);
  
  if (!TestAttackChance(ctx, target, reg)) return false;
  else {
    target->Die(ctx);
    setInternalValue(reg.success_reg, 1, true);
  }
  return true;
}

//Attack organism faced by this one if you are both predators or both prey.
bool Hardware::Types::HeadsEX::Inst_FightPred(Context& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0) return false;
  if (!m_use_avatar && !m_organism->IsNeighborCellOccupied()) return false;
  else if (m_use_avatar == 2 && !m_organism->GetOrgInterface().FacedHasPredAV()) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) target = m_organism->GetOrgInterface().GetNeighbor();
  else if (m_use_avatar == 2) target = m_organism->GetOrgInterface().GetRandFacedPredAV();
  if (target->IsDead()) return false;
  
  // allow only for predator vs predator
  if (target->IsPreyFT() || m_organism->IsPreyFT()) return false;
  
  //Use merit to decide who wins this battle.
  bool kill_attacker = true;
  
  const double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
  const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
  const double attacker_win_odds = ((attacker_merit) / (attacker_merit + target_merit));
  const double target_win_odds = ((target_merit) / (attacker_merit + target_merit));
  
  const double odds_someone_dies = max(attacker_win_odds, target_win_odds);
  const double odds_target_dies = (1 - target_win_odds) * odds_someone_dies;
  const double decider = ctx.GetRandom().GetDouble(1);
  
  if (decider < (1 - odds_someone_dies)) return true;
  else if (decider < ((1 - odds_someone_dies) + odds_target_dies)) kill_attacker = false;
  
  if (kill_attacker) {
    m_organism->Die(ctx);
    return true;
  }
  
  const int target_cell = target->GetOrgInterface().GetCellID();
  m_organism->GetOrgInterface().AttackFacedOrg(ctx, target_cell);
  
  bool attack_success = true;
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, attack_success, true);
  return true;
}


//Teach offspring learned targeting/foraging behavior
bool Hardware::Types::HeadsEX::Inst_TeachOffspring(Context&)
{
  assert(m_organism != 0);
  m_organism->Teach(true);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_LearnParent(Context& ctx)
{
  assert(m_organism != 0);
  bool halt = false;
  if (m_organism->HadParentTeacher()) {
    int old_target = m_organism->GetForageTarget();
    int prop_target = -1;
    prop_target = m_organism->GetParentFT();
    
    halt = ((prop_target == -2 || prop_target == -3) && m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0);
    if (!halt) {
      if (m_use_avatar && m_organism->GetOrgInterface().GetAVCellID() != -1 &&
          ((( prop_target == -2  || prop_target == -3) && old_target != -2 && old_target != -3) ||
           (prop_target != -2  && prop_target != -3 && (old_target == -2 || old_target == -3)))) {
            m_organism->GetOrgInterface().SwitchPredPrey(ctx);
            m_organism->CopyParentFT(ctx);
          }
      else m_organism->CopyParentFT(ctx);
    }
  }
  return !halt;
}

bool Hardware::Types::HeadsEX::Inst_SetGuard(Context& ctx)
{
  bool set_ok = false;
  if (m_organism->GetPhenotype().GetTimeUsed() >= m_world->GetConfig().JUV_PERIOD.Get()) {
    const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->GetHabitat() == 3 || resource_lib.GetResource(i)->GetHabitat() == 4) {
        double cell_res = 0.0;
        if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
        else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
        if (cell_res > 0) set_ok = true;
      }
    }
  }
  if (set_ok){
    m_organism->SetGuard();
    m_organism->IncGuard();
  } else m_world->GetStats().IncGuardFail();
  setInternalValue(FindModifiedRegister(rBX), (int) m_organism->IsGuard(), true);
  return set_ok;
}

bool Hardware::Types::HeadsEX::Inst_SetGuardOnce(Context& ctx)
{
  bool set_ok = false;
  if (!m_organism->IsGuard()) set_ok = Inst_SetGuard(ctx);
  else m_world->GetStats().IncGuardFail();
  setInternalValue(FindModifiedRegister(rBX), set_ok, true);
  return set_ok;
}

bool Hardware::Types::HeadsEX::Inst_GetNumGuards(Context& ctx)
{
  int num_guards = 0;
  bool on_den = false;
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResDef(i)->GetHabitat() == 3 || resource_lib.GetResDef(i)->GetHabitat() == 4) {
      double cell_res = 0.0;
      if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
      else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
      if (cell_res > 0) on_den = true;
    }
  }
  if (on_den){
    if (m_use_avatar) {
      int cell_id = m_organism->GetOrgInterface().GetAVCellID();
      Apto::Array<cOrganism*> cell_avs = m_organism->GetOrgInterface().GetCellAVs(cell_id);
      for (int k = 0; k < cell_avs.GetSize(); k++) {
        if( cell_avs[k]->IsGuard()) num_guards++;
      }
    } else if (!m_use_avatar){
      if (m_organism->IsGuard()) num_guards ++;
    }
  }
  setInternalValue(FindModifiedRegister(rBX), num_guards, true);
  return on_den;
}

bool Hardware::Types::HeadsEX::Inst_GetNumJuvs(Context& ctx)
{
  int num_juvs = 0;
  int juv_age = m_world->GetConfig().JUV_PERIOD.Get();
  bool on_den = false;
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  for (int i = 0; i < resource_lib .GetSize(); i++) {
    if (resource_lib.GetResDef(i)->GetHabitat() == 3 || resource_lib.GetResDef(i)->GetHabitat() == 4) {
      double cell_res = 0.0;
      if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResourceVal(ctx, i);
      else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResourceVal(ctx, i);
      if (cell_res > 0) on_den = true;
    }
  }
  if (on_den){
    if (m_use_avatar) {
      int cell_id = m_organism->GetOrgInterface().GetAVCellID();
      Apto::Array<cOrganism*> cell_avs = m_organism->GetOrgInterface().GetCellAVs(cell_id);
      
      for (int k = 0; k < cell_avs.GetSize(); k++) {
        if (cell_avs[k]->GetPhenotype().GetTimeUsed() < juv_age) num_juvs++;
      }
    } else if (!m_use_avatar){
      if (m_organism->GetPhenotype().GetTimeUsed() < juv_age) num_juvs++;
    }
  }
  setInternalValue(FindModifiedRegister(rBX), num_juvs, true);
  return on_den;
}

bool Hardware::Types::HeadsEX::Inst_ActivateDisplay(Context& ctx)
{
  if (m_organism->GetOrgDisplayData() == NULL) return false;
  m_organism->ActivateDisplay();
  return true;
}

bool Hardware::Types::HeadsEX::Inst_UpdateDisplay(Context& ctx)
{
  return m_organism->UpdateOrgDisplay();
}

bool Hardware::Types::HeadsEX::Inst_ModifyDisplay(Context& ctx)
{
  sOrgDisplay* this_display = m_organism->GetOrgDisplayData();
  if (this_display == NULL) return false;
  InstMemSpace& memory = m_memory;
  int pos = getIP().GetPosition();
  bool message_used = false;
  for (int i = 0; i < 5; i++) {
    pos += 1;
    if (pos >= memory.GetSize()) pos = 0;
    if (m_inst_set->IsNop(memory[pos])) {
      int this_nop = m_inst_set->GetNopMod(memory[pos]);
      switch (this_nop) {
        case 0:
          this_display->distance = GetRegister(rAX);
        case 1:
          this_display->direction = GetRegister(rBX);
        case 2:
          this_display->thing_id = GetRegister(rCX);
        case 3:
          this_display->value = GetRegister(rDX);
        default:
          if (!message_used) this_display->message = GetRegister(this_nop);
          message_used = true;
      }
    }
    else break;
  }
  m_organism->SetOrgDisplay(this_display);
  return true;
}

bool Hardware::Types::HeadsEX::Inst_ReadLastSeenDisplay(Context& ctx)
{
  if (!m_sensor.HasSeenDisplay()) return false;
  sOrgDisplay& last_seen = m_sensor.GetLastSeenDisplay();
  InstMemSpace& memory = m_memory;
  int pos = getIP().GetPosition();
  bool message_read = false;
  for (int i = 0; i < 5; i++) {
    pos += 1;
    if (pos >= memory.GetSize()) pos = 0;
    if (m_inst_set->IsNop(memory[pos])) {
      int this_nop = m_inst_set->GetNopMod(memory[pos]);
      switch (this_nop) {
        case 0:
          setInternalValue(rAX, last_seen.distance, true);
        case 1:
          setInternalValue(rBX, last_seen.direction, true);
        case 2:
          setInternalValue(rCX, last_seen.thing_id, true);
        case 3:
          setInternalValue(rDX, last_seen.value, true);
        default:
          if (!message_read) setInternalValue(this_nop, last_seen.message, true);
          message_read = true;
      }
    }
    else break;
  }
  return true;
}

bool Hardware::Types::HeadsEX::Inst_ModifySimpDisplay(Context& ctx)
{
  InstMemSpace& memory = m_memory;
  int pos = getIP().GetPosition();
  bool message_used = false;
  for (int i = 0; i < 4; i++) {
    pos += 1;
    if (pos >= memory.GetSize()) pos = 0;
    if (m_inst_set->IsNop(memory[pos])) {
      int this_nop = m_inst_set->GetNopMod(memory[pos]);
      switch (this_nop) {
        case 0:
          m_organism->SetSimpDisplay(0, GetRegister(rAX));
        case 1:
          m_organism->SetSimpDisplay(1, GetRegister(rBX));
        case 2:
          m_organism->SetSimpDisplay(2, GetRegister(rCX));
        default:
          if (!message_used) m_organism->SetSimpDisplay(3, GetRegister(this_nop));
          message_used = true;
      }
    }
    else break;
  }
  return true;
}

bool Hardware::Types::HeadsEX::Inst_ReadLastSimpDisplay(Context& ctx)
{
  if (!m_sensor.HasSeenDisplay()) return false;
  sOrgDisplay& last_seen = m_sensor.GetLastSeenDisplay();
  InstMemSpace& memory = m_memory;
  int pos = getIP().GetPosition();
  bool message_read = false;
  for (int i = 0; i < 4; i++) {
    pos += 1;
    if (pos >= memory.GetSize()) pos = 0;
    if (m_inst_set->IsNop(memory[pos])) {
      int this_nop = m_inst_set->GetNopMod(memory[pos]);
      switch (this_nop) {
        case 0:
          setInternalValue(rAX, last_seen.distance, true);
        case 1:
          setInternalValue(rBX, last_seen.direction, true);
        case 2:
          setInternalValue(rCX, last_seen.value, true);
        default:
          if (!message_read) setInternalValue(this_nop, last_seen.message, true);
          message_read = true;
      }
    }
    else if (!m_inst_set->IsNop(memory[pos]) && i == 0) {
      setInternalValue(rAX, last_seen.distance, true);
      setInternalValue(rBX, last_seen.direction, true);
      setInternalValue(rCX, last_seen.value, true);
      setInternalValue(rDX, last_seen.message, true);
      break;
    }
    else break;
  }
  return true;
}

bool Hardware::Types::HeadsEX::Inst_KillDisplay(Context& ctx)
{
  if (!m_organism->IsDisplaying()) return false;
  m_organism->KillDisplay();
  return true;
}




bool Hardware::Types::HeadsEX::Inst_ScrambleReg(Context& ctx)
{
  for (int i = 0; i < NUM_REGISTERS; i++) {
    setInternalValue(rAX + i, (int) (ctx.GetRandom().GetDouble()), true);
  }
  return true;
}


bool Hardware::Types::HeadsEX::Inst_GetFacedEditDistance(Context& ctx)
{
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism* target = NULL;
  target = m_organism->GetOrgInterface().GetNeighbor();
  
  const Genome& org_genome = m_organism->GetGenome();
  ConstInstructionSequencePtr org_seq_p;
  org_seq_p.DynamicCastFrom(org_genome.Representation());
  const InstructionSequence& org_seq = *org_seq_p;
  
  const Genome& target_genome = target->GetGenome();
  ConstInstructionSequencePtr target_seq_p;
  target_seq_p.DynamicCastFrom(target_genome.Representation());
  const InstructionSequence& target_seq = *target_seq_p;
  
  setInternalValue(rBX, InstructionSequence::FindEditDistance(org_seq, target_seq), true);
  
  return true;
}

void Hardware::Types::HeadsEX::MakePred(Context& ctx)
{
  if (m_organism->IsPreyFT()) {
    if (m_world->GetConfig().MAX_PRED.Get() && m_world->GetStats().GetNumTotalPredCreatures() >= m_world->GetConfig().MAX_PRED.Get()) m_organism->GetOrgInterface().KillRandPred(ctx, m_organism);
    // switching between predator and prey means having to switch avatar list...don't run this for orgs with AVCell == -1 (avatars off or test cpu)
    if (m_use_avatar && m_organism->GetOrgInterface().GetAVCellID() != -1) {
      m_organism->GetOrgInterface().SwitchPredPrey(ctx);
      m_organism->SetPredFT(ctx);
    }
    else m_organism->SetPredFT(ctx);
  }
}

void Hardware::Types::HeadsEX::MakeTopPred(Context& ctx)
{
  if (m_organism->IsPreyFT()) {
    if (m_world->GetConfig().MAX_PRED.Get() && m_world->GetStats().GetNumTotalPredCreatures() >= m_world->GetConfig().MAX_PRED.Get()) m_organism->GetOrgInterface().KillRandPred(ctx, m_organism);
    // switching between predator and prey means having to switch avatar list...don't run this for orgs with AVCell == -1 (avatars off or test cpu)
    if (m_use_avatar && m_organism->GetOrgInterface().GetAVCellID() != -1) {
      m_organism->GetOrgInterface().SwitchPredPrey(ctx);
      m_organism->SetTopPredFT(ctx);
    }
    else m_organism->SetTopPredFT(ctx);
  }
  else if (m_organism->IsPredFT()) m_organism->SetTopPredFT(ctx);
}

bool Hardware::Types::HeadsEX::TestAttack(Context& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0) return false;
  
  if (!m_use_avatar && !m_organism->IsNeighborCellOccupied()) return false;
  else if (m_use_avatar == 2 && !m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
  
  int num_neighbors = 0;
  if (!m_use_avatar) num_neighbors = m_organism->GetNeighborhoodSize();
  else if (m_use_avatar == 2) num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
  if (num_neighbors == 0) return false;
  
  // prevent killing on refuges
  const cResourceDefLib& resource_lib = m_world->GetEnvironment().GetResDefLib();
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetRefuge()) {
      if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedResourceVal(ctx, i) > 0) return false;
      else if (m_use_avatar == 2 && m_organism->GetOrgInterface().GetAVFacedResourceVal(ctx, i) > 0) return false;
    }
  }
  return true;
}

bool Hardware::Types::HeadsEX::TestAttackPred(Context& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0) return false;
  
  if (!m_use_avatar && !m_organism->IsNeighborCellOccupied()) return false;
  else if (m_use_avatar == 2 && !m_organism->GetOrgInterface().FacedHasPredAV()) return false;
  
  return true;
}

cOrganism* Hardware::Types::HeadsEX::GetPreyTarget(Context& ctx)
{
  cOrganism* target = NULL;
  if (!m_use_avatar) target = m_organism->GetOrgInterface().GetNeighbor();
  else if (m_use_avatar == 2) target = m_organism->GetOrgInterface().GetRandFacedPreyAV();
  return target;
}

bool Hardware::Types::HeadsEX::TestPreyTarget(cOrganism* target)
{
  // attacking other carnivores is handled differently (e.g. using fights or tolerance)
  bool success = true;
  if (!target->IsPreyFT() || target->IsDead()) success = false;
  return success;
}

void Hardware::Types::HeadsEX::SetAttackReg(sAttackReg& reg)
{
  reg.success_reg = FindModifiedRegister(rBX);
  reg.bonus_reg = FindModifiedNextRegister(reg.success_reg);
  reg.bin_reg = FindModifiedNextRegister(reg.bonus_reg);
}

bool Hardware::Types::HeadsEX::ExecuteAttack(Context& ctx, cOrganism* target, sAttackReg& reg, double odds)
{
  if (!TestAttackChance(ctx, target, reg, odds)) return false;
  double effic = m_world->GetConfig().PRED_EFFICIENCY.Get();
  if (m_organism->IsTopPredFT()) effic *= effic;
  ApplyKilledPreyMerit(target, effic);
  ApplyKilledPreyReactions(target);
  
  // keep returns in same order as legacy code (important if reg assignments are shared)
  ApplyKilledPreyResBins(target, reg, effic);
  setInternalValue(reg.success_reg, 1, true);
  ApplyKilledPreyBonus(target, reg, effic);
  
  target->Die(ctx); // kill first -- could end up being killed by inject clone or MAX_PRED if parent was pred
  MakePred(ctx);
  TryPreyClone(ctx);
  return true;
}

bool Hardware::Types::HeadsEX::ExecuteShareAttack(Context& ctx, cOrganism* target, sAttackReg& reg, Apto::Array<cOrganism*>& pack, double odds)
{
  if (!TestAttackChance(ctx, target, reg, odds)) return false;
  double effic = m_world->GetConfig().PRED_EFFICIENCY.Get();
  if (m_organism->IsTopPredFT()) effic *= effic;
  ApplyKilledPreyReactions(target);           // reactions can't be shared
  double share = 1 / pack.GetSize();
  for (int i = 0; i < pack.GetSize(); i++) {
    ApplySharedKilledPreyMerit(target, effic, pack[i], share);
    ApplySharedKilledPreyBonus(target, reg, effic, pack[i], share);
    ApplySharedKilledPreyResBins(target, reg, effic, pack[i], share);
  }
  
  target->Die(ctx); // kill first -- could end up being killed by inject clone or MAX_PRED if parent was pred
  MakePred(ctx);
  TryPreyClone(ctx);
  setInternalValue(reg.success_reg, 1, true);
  return true;
}

bool Hardware::Types::HeadsEX::ExecuteFakeShareAttack(Context& ctx, cOrganism* target, sAttackReg& reg, double share, double odds)
{
  if (!TestAttackChance(ctx, target, reg, odds)) return false;
  double effic = m_world->GetConfig().PRED_EFFICIENCY.Get();
  if (m_organism->IsTopPredFT()) effic *= effic;
  ApplyKilledPreyReactions(target);           // reactions can't be shared
  ApplySharedKilledPreyMerit(target, effic, m_organism, share);
  ApplySharedKilledPreyBonus(target, reg, effic, m_organism, share);
  ApplySharedKilledPreyResBins(target, reg, effic, m_organism, share);
  
  target->Die(ctx); // kill first -- could end up being killed by inject clone or MAX_PRED if parent was pred
  MakePred(ctx);
  TryPreyClone(ctx);
  setInternalValue(reg.success_reg, 1, true);
  return true;
}

bool Hardware::Types::HeadsEX::ExecutePoisonPreyAttack(Context& ctx, cOrganism* target, sAttackReg& reg, double odds)
{
  // poison affects merit
  if (!TestAttackChance(ctx, target, reg, odds)) return false;
  double effic = m_world->GetConfig().PRED_EFFICIENCY.Get();
  if (target->GetForageTarget() == 2) effic = m_world->GetConfig().PRED_EFFICIENCY_POISON.Get();
  
  bool to_die = false;
  if (m_organism->IsTopPredFT()) effic *= effic;
  // apply poison, if any
  if (target->GetForageTarget() == 2 && m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
    double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
    attacker_merit -= target_merit * effic;
    m_organism->UpdateMerit(attacker_merit);
    to_die = (m_organism->GetPhenotype().GetMerit().GetDouble() <= 0);
  }
  else ApplyKilledPreyMerit(target, effic);
  
  if (target->GetForageTarget() != 2) ApplyKilledPreyReactions(target);
  if (target->GetForageTarget() == 2) effic *= -1;
  ApplyKilledPreyBonus(target, reg, effic);
  ApplyKilledPreyResBins(target, reg, effic);
  target->Die(ctx); // kill first -- could end up being killed by inject clone or MAX_PRED if parent was pred
  MakePred(ctx);
  TryPreyClone(ctx);
  setInternalValue(reg.success_reg, 1, true);
  if (to_die) m_organism->Die(ctx); // poisoned to death
  return true;
}

bool Hardware::Types::HeadsEX::TestAttackChance(Context& ctx, cOrganism* target, sAttackReg& reg, double odds)
{
  bool success = true;
  if (odds == -1) odds = m_world->GetConfig().PRED_ODDS.Get();
  if (ctx.GetRandom().GetDouble() >= odds ||
      (m_world->GetConfig().MIN_PREY.Get() > 0 && m_world->GetStats().GetNumPreyCreatures() <= m_world->GetConfig().MIN_PREY.Get())) {
    assert(false);
    //InjureOrg(target);
    setInternalValue(reg.success_reg, -1, true);
    setInternalValue(reg.bonus_reg, -1, true);
    if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) setInternalValue(reg.bin_reg, -1, true);
    success = false;
  }
  return success;
}

void Hardware::Types::HeadsEX::ApplyKilledPreyMerit(cOrganism* target, double effic)
{
  // add prey's merit to predator's--this will result in immediately applying merit increases; adjustments to bonus, give increase in next generation
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
    double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
    attacker_merit += target_merit * effic;
    m_organism->UpdateMerit(attacker_merit);
  }
}

void Hardware::Types::HeadsEX::ApplyKilledPreyReactions(cOrganism* target)
{
  // now add on the victims reaction counts to your own, this will allow you to pass any reaction tests...
  Apto::Array<int> target_reactions = target->GetPhenotype().GetLastReactionCount();
  Apto::Array<int> org_reactions = m_organism->GetPhenotype().GetStolenReactionCount();
  for (int i = 0; i < org_reactions.GetSize(); i++) {
    m_organism->GetPhenotype().SetStolenReactionCount(i, org_reactions[i] + target_reactions[i]);
  }
}

void Hardware::Types::HeadsEX::ApplyKilledPreyBonus(cOrganism* target, sAttackReg& reg, double effic)
{
  // and add current merit bonus after adjusting for conversion efficiency
  const double target_bonus = target->GetPhenotype().GetCurBonus();
  m_organism->GetPhenotype().SetCurBonus(m_organism->GetPhenotype().GetCurBonus() + (target_bonus * effic));
  setInternalValue(reg.bonus_reg, (int) (target_bonus), true);
}

void Hardware::Types::HeadsEX::ApplyKilledPreyResBins(cOrganism* target, sAttackReg& reg, double effic)
{
  // now add the victims internal resource bins to your own, if enabled, after correcting for conversion efficiency
  if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
    Apto::Array<double> target_bins = target->GetRBins();
    for (int i = 0; i < target_bins.GetSize(); i++) {
      m_organism->AddToRBin(i, target_bins[i] * effic);
      if (effic > 0) target->AddToRBin(i, -1 * (target_bins[i] * effic));
    }
    const int spec_bin = (int) (m_organism->GetRBins()[m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get()]);
    setInternalValue(reg.bin_reg, spec_bin, true);
  }
}

void Hardware::Types::HeadsEX::ApplySharedKilledPreyMerit(cOrganism* target, double effic, cOrganism* org, double share)
{
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    const double target_merit = target->GetPhenotype().GetMerit().GetDouble() * share;
    double attacker_merit = org->GetPhenotype().GetMerit().GetDouble();
    attacker_merit += target_merit * effic;
    org->UpdateMerit(attacker_merit);
  }
}

void Hardware::Types::HeadsEX::ApplySharedKilledPreyBonus(cOrganism* target, sAttackReg& reg, double effic, cOrganism* org, double share)
{
  const double target_bonus = target->GetPhenotype().GetCurBonus() * share;
  double bonus = org->GetPhenotype().GetCurBonus() + (target_bonus * effic);
  org->GetPhenotype().SetCurBonus(bonus);
  setInternalValue(reg.bonus_reg, (int) (target_bonus), true);
}

void Hardware::Types::HeadsEX::ApplySharedKilledPreyResBins(cOrganism* target, sAttackReg& reg, double effic, cOrganism* org, double share)
{
  if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
    Apto::Array<double> target_bins = target->GetRBins();
    for (int i = 0; i < target_bins.GetSize(); i++) {
      double bin_amt = target_bins[i] * effic;
      org->AddToRBin(i, bin_amt * share);
      target->AddToRBin(i, -1 * bin_amt);
    }
    const int spec_bin = (int) (m_organism->GetRBins()[m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get()]);
    setInternalValue(reg.bin_reg, spec_bin, true);
  }
}

void Hardware::Types::HeadsEX::TryPreyClone(Context& ctx)
{
  if (m_world->GetConfig().MIN_PREY.Get() < 0 && m_world->GetStats().GetNumPreyCreatures() <= abs(m_world->GetConfig().MIN_PREY.Get())) {
    // prey numbers can be crashing for other reasons and we wouldn't be using this switch if we didn't want an absolute min num prey
    // but can't dump a lot b/c could end up filling world with just clones (e.g. if attack happens when world is still being populated)
    int num_clones = abs(m_world->GetConfig().MIN_PREY.Get()) - m_world->GetStats().GetNumPreyCreatures();
    for (int i = 0; i < min(2, num_clones); i++) m_organism->GetOrgInterface().InjectPreyClone(ctx, m_organism->SystematicsGroup("genotype")->ID());
  }
}

void Hardware::Types::HeadsEX::InjureOrg(cOrganism* target)
{
  double injury = m_world->GetConfig().PRED_INJURY.Get();
  if (injury == 0) return;
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    double target_merit = target->GetPhenotype().GetMerit().GetDouble();
    target_merit -= target_merit * injury;
    target->UpdateMerit(target_merit);
  }
  Apto::Array<int> target_reactions = target->GetPhenotype().GetLastReactionCount();
  for (int i = 0; i < target_reactions.GetSize(); i++) {
    target->GetPhenotype().SetReactionCount(i, target_reactions[i] - (int)((target_reactions[i] * injury)));
  }
  const double target_bonus = target->GetPhenotype().GetCurBonus();
  target->GetPhenotype().SetCurBonus(target_bonus - (target_bonus * injury));
  
  if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
    Apto::Array<double> target_bins = target->GetRBins();
    for (int i = 0; i < target_bins.GetSize(); i++) {
      target->AddToRBin(i, -1 * (target_bins[i] * injury));
    }
  }
}

Apto::Array<cOrganism*> Hardware::Types::HeadsEX::GetPredGroupAttackNeighbors()
{
  Apto::Array<int> neighborhood;
  Apto::Array<cOrganism*> pack;
  pack.Push(m_organism);
  if (!m_use_avatar) {
    m_organism->GetOrgInterface().GetNeighborhoodCellIDs(neighborhood);
    for (int j = 0; j < neighborhood.GetSize(); j++) {
      if (m_organism->GetOrgInterface().GetCell(neighborhood[j])->IsOccupied() &&
          !m_organism->GetOrgInterface().GetCell(neighborhood[j])->GetOrganism()->IsDead()) {
        if (!m_organism->GetOrgInterface().GetCell(neighborhood[j])->GetOrganism()->IsPreyFT()) {
          pack.Push(m_organism->GetOrgInterface().GetCell(neighborhood[j])->GetOrganism());
        }
      }
    }
  }
  else {
    m_organism->GetOrgInterface().GetAVNeighborhoodCellIDs(neighborhood);
    for (int j = 0; j < neighborhood.GetSize(); j++) {
      if (m_organism->GetOrgInterface().GetCell(neighborhood[j])->HasPredAV()) {
        Apto::Array<cOrganism*> predators = m_organism->GetOrgInterface().GetCell(neighborhood[j])->GetCellInputAVs();
        for (int i = 0; i < predators.GetSize(); i++) {
          if (!predators[i]->IsDead() && !predators[i]->IsPreyFT()) pack.Push(predators[i]);
        }
      }
    }
  }
  return pack;
}




bool Hardware::Types::HeadsEX::TestAttackResultsOut(sAttackResult& results)
{
  if (m_world->GetConfig().TRACK_GROUP_ATTACK_DETAILS.Get() == 1) {
    cString attack_string = "";
    attack_string += cStringUtil::Stringf("%d", results.size);
    attack_string += cStringUtil::Stringf(",%d", results.success);
    attack_string += cStringUtil::Stringf(",%d", results.share);
    attack_string += cStringUtil::Stringf(",%d", results.inst);
    m_organism->GetOrgInterface().TryWriteGroupAttackString(attack_string);
  }
  else if (m_world->GetConfig().TRACK_GROUP_ATTACK_DETAILS.Get() == 2) {
    unsigned char raw_bits = (((((results.inst << 2) | results.share) << 2) | results.success) << 3) | results.size;
    m_organism->GetOrgInterface().TryWriteGroupAttackBits(raw_bits);
  }
  if (results.success == 0) return true;
  return false;
}
