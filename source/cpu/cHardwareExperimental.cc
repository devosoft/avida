/*
 *  cHardwareExperimental.cc
 *  Avida
 *
 *  Created by David on 2/10/07 based on cHardwareCPU.cc
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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
 */


#include "cHardwareExperimental.h"

#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"

#include "avida/private/systematics/SexualAncestry.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cHardwareManager.h"
#include "cHardwareTracer.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cOrgMessage.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cStateGrid.h"
#include "cStringUtil.h"
#include "cTestCPU.h"
#include "cWorld.h"

#include "tInstLibEntry.h"

#include <climits>
#include <fstream>

using namespace std;
using namespace Avida;
using namespace AvidaTools;


static const unsigned int CONSENSUS = (sizeof(int) * 8) / 2;
static const unsigned int CONSENSUS24 = 12;

inline unsigned int cHardwareExperimental::BitCount(unsigned int value) const
{
  const unsigned int w = value - ((value >> 1) & 0x55555555);
  const unsigned int x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
  const unsigned int bit_count = ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
  return bit_count;
}


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
    cNOPEntry("nop-A", rAX),
    cNOPEntry("nop-B", rBX),
    cNOPEntry("nop-C", rCX),
    cNOPEntry("nop-D", rDX),
    cNOPEntry("nop-E", rEX),
    cNOPEntry("nop-F", rFX),
    cNOPEntry("nop-G", rGX),
    cNOPEntry("nop-H", rHX),
    
    cNOPEntry("nop-I", rIX),
    cNOPEntry("nop-J", rJX),
    cNOPEntry("nop-K", rKX),
    cNOPEntry("nop-L", rLX),
    cNOPEntry("nop-M", rMX),
    cNOPEntry("nop-N", rNX),
    cNOPEntry("nop-O", rOX),
    cNOPEntry("nop-P", rPX),
  };
  
  static const tInstLibEntry<tMethod> s_f_array[] = {
    /*
     Note: all entries of cNOPEntryCPU s_n_array must have corresponding
     in the same order in tInstLibEntry<tMethod> s_f_array, and these entries must
     be the first elements of s_f_array.
     */
    tInstLibEntry<tMethod>("nop-A", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-B", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-C", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-D", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-E", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-F", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-G", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-H", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    
    tInstLibEntry<tMethod>("nop-I", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-J", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-K", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-L", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-M", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-N", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-O", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-P", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    
    tInstLibEntry<tMethod>("NULL", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, 0, "True no-operation instruction: does nothing"),
    tInstLibEntry<tMethod>("nop-X", &cHardwareExperimental::Inst_Nop, INST_CLASS_NOP, 0, "True no-operation instruction: does nothing"),
    
    
    // Threading 
    tInstLibEntry<tMethod>("fork-thread", &cHardwareExperimental::Inst_ForkThread),
    tInstLibEntry<tMethod>("exit-thread", &cHardwareExperimental::Inst_ExitThread),
    tInstLibEntry<tMethod>("id-thread", &cHardwareExperimental::Inst_IdThread),
    
    
    // Standard Conditionals
    tInstLibEntry<tMethod>("if-n-equ", &cHardwareExperimental::Inst_IfNEqu, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX?!=?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-less", &cHardwareExperimental::Inst_IfLess, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? < ?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-not-0", &cHardwareExperimental::Inst_IfNotZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? != 0, else skip it"),
    tInstLibEntry<tMethod>("if-equ-0", &cHardwareExperimental::Inst_IfEqualZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? == 0, else skip it"),
    tInstLibEntry<tMethod>("if-gtr-0", &cHardwareExperimental::Inst_IfGreaterThanZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? > 0, else skip it"),
    tInstLibEntry<tMethod>("if-less-0", &cHardwareExperimental::Inst_IfLessThanZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? < 0, else skip it"),
    tInstLibEntry<tMethod>("if-gtr-x", &cHardwareExperimental::Inst_IfGtrX, INST_CLASS_CONDITIONAL),
    tInstLibEntry<tMethod>("if-equ-x", &cHardwareExperimental::Inst_IfEquX, INST_CLASS_CONDITIONAL),
    
    tInstLibEntry<tMethod>("if-cons", &cHardwareExperimental::Inst_IfConsensus, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? in consensus, else skip it"),
    tInstLibEntry<tMethod>("if-cons-24", &cHardwareExperimental::Inst_IfConsensus24, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX[0:23]? in consensus , else skip it"),
    tInstLibEntry<tMethod>("if-less-cons", &cHardwareExperimental::Inst_IfLessConsensus, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if Count(?BX?) < Count(?CX?), else skip it"),
    tInstLibEntry<tMethod>("if-less-cons-24", &cHardwareExperimental::Inst_IfLessConsensus24, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if Count(?BX[0:23]?) < Count(?CX[0:23]?), else skip it"),
    
    tInstLibEntry<tMethod>("if-stk-gtr", &cHardwareExperimental::Inst_IfStackGreater, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if the top of the current stack > inactive stack, else skip it"),
    
    // Core ALU Operations
    tInstLibEntry<tMethod>("pop", &cHardwareExperimental::Inst_Pop, INST_CLASS_DATA, 0, "Remove top number from stack and place into ?BX?"),
    tInstLibEntry<tMethod>("push", &cHardwareExperimental::Inst_Push, INST_CLASS_DATA, 0, "Copy number from ?BX? and place it into the stack"),
    tInstLibEntry<tMethod>("pop-all", &cHardwareExperimental::Inst_PopAll, INST_CLASS_DATA, 0, "Remove top numbers from stack and place into ?BX?"),
    tInstLibEntry<tMethod>("push-all", &cHardwareExperimental::Inst_PushAll, INST_CLASS_DATA, 0, "Copy number from all registers and place into the stack"),
    tInstLibEntry<tMethod>("swap-stk", &cHardwareExperimental::Inst_SwitchStack, INST_CLASS_DATA, 0, "Toggle which stack is currently being used"),
    tInstLibEntry<tMethod>("swap-stk-top", &cHardwareExperimental::Inst_SwapStackTop, INST_CLASS_DATA, 0, "Swap the values at the top of both stacks"),
    tInstLibEntry<tMethod>("swap", &cHardwareExperimental::Inst_Swap, INST_CLASS_DATA, 0, "Swap the contents of ?BX? with ?CX?"),
    
    tInstLibEntry<tMethod>("shift-r", &cHardwareExperimental::Inst_ShiftR, INST_CLASS_ARITHMETIC_LOGIC, 0, "Shift bits in ?BX? right by one (divide by two)"),
    tInstLibEntry<tMethod>("shift-l", &cHardwareExperimental::Inst_ShiftL, INST_CLASS_ARITHMETIC_LOGIC, 0, "Shift bits in ?BX? left by one (multiply by two)"),
    tInstLibEntry<tMethod>("inc", &cHardwareExperimental::Inst_Inc, INST_CLASS_ARITHMETIC_LOGIC, 0, "Increment ?BX? by one"),
    tInstLibEntry<tMethod>("dec", &cHardwareExperimental::Inst_Dec, INST_CLASS_ARITHMETIC_LOGIC, 0, "Decrement ?BX? by one"),
    tInstLibEntry<tMethod>("zero", &cHardwareExperimental::Inst_Zero, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to 0"),
    tInstLibEntry<tMethod>("mult100", &cHardwareExperimental::Inst_Mult100, INST_CLASS_ARITHMETIC_LOGIC, 0, "Mult ?BX? by 100"),
    
    tInstLibEntry<tMethod>("add", &cHardwareExperimental::Inst_Add, INST_CLASS_ARITHMETIC_LOGIC, 0, "Add BX to CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("sub", &cHardwareExperimental::Inst_Sub, INST_CLASS_ARITHMETIC_LOGIC, 0, "Subtract CX from BX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("nand", &cHardwareExperimental::Inst_Nand, INST_CLASS_ARITHMETIC_LOGIC, 0, "Nand BX by CX and place the result in ?BX?"),
    
    tInstLibEntry<tMethod>("IO", &cHardwareExperimental::Inst_TaskIO, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "Output ?BX?, and input new number back into ?BX?"),
    tInstLibEntry<tMethod>("IO-expire", &cHardwareExperimental::Inst_TaskIOExpire, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "Output ?BX?, and input new number back into ?BX?, if the number has not yet expired"),
    tInstLibEntry<tMethod>("input", &cHardwareExperimental::Inst_TaskInput, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "Input new number into ?BX?"),
    tInstLibEntry<tMethod>("output", &cHardwareExperimental::Inst_TaskOutput, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "Output ?BX?"),
    tInstLibEntry<tMethod>("output-zero", &cHardwareExperimental::Inst_TaskOutputZero, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "Output ?BX?"),
    tInstLibEntry<tMethod>("output-expire", &cHardwareExperimental::Inst_TaskOutputExpire, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "Output ?BX?, as long as the output has not yet expired"),
    tInstLibEntry<tMethod>("deme-IO", &cHardwareExperimental::Inst_DemeIO, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    
    tInstLibEntry<tMethod>("mult", &cHardwareExperimental::Inst_Mult, INST_CLASS_ARITHMETIC_LOGIC, 0, "Multiple BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("div", &cHardwareExperimental::Inst_Div, INST_CLASS_ARITHMETIC_LOGIC, 0, "Divide BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("mod", &cHardwareExperimental::Inst_Mod, INST_CLASS_ARITHMETIC_LOGIC),
    
    
    // Flow Control Instructions
    tInstLibEntry<tMethod>("label", &cHardwareExperimental::Inst_Label, INST_CLASS_FLOW_CONTROL, nInstFlag::LABEL),
    
    tInstLibEntry<tMethod>("search-lbl-comp-s", &cHardwareExperimental::Inst_Search_Label_Comp_S, INST_CLASS_FLOW_CONTROL, 0, "Find complement label from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-comp-f", &cHardwareExperimental::Inst_Search_Label_Comp_F, INST_CLASS_FLOW_CONTROL, 0, "Find complement label forward and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-comp-b", &cHardwareExperimental::Inst_Search_Label_Comp_B, INST_CLASS_FLOW_CONTROL, 0, "Find complement label backward and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-direct-s", &cHardwareExperimental::Inst_Search_Label_Direct_S, INST_CLASS_FLOW_CONTROL, 0, "Find direct label from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-direct-f", &cHardwareExperimental::Inst_Search_Label_Direct_F, INST_CLASS_FLOW_CONTROL, 0, "Find direct label forward and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-direct-b", &cHardwareExperimental::Inst_Search_Label_Direct_B, INST_CLASS_FLOW_CONTROL, 0, "Find direct label backward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-comp-s", &cHardwareExperimental::Inst_Search_Seq_Comp_S, INST_CLASS_FLOW_CONTROL, 0, "Find complement template from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-comp-f", &cHardwareExperimental::Inst_Search_Seq_Comp_F, INST_CLASS_FLOW_CONTROL, 0, "Find complement template forward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-comp-b", &cHardwareExperimental::Inst_Search_Seq_Comp_B, INST_CLASS_FLOW_CONTROL, 0, "Find complement template backward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-direct-s", &cHardwareExperimental::Inst_Search_Seq_Direct_S, INST_CLASS_FLOW_CONTROL, 0, "Find direct template from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-direct-f", &cHardwareExperimental::Inst_Search_Seq_Direct_F, INST_CLASS_FLOW_CONTROL, 0, "Find direct template forward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-direct-b", &cHardwareExperimental::Inst_Search_Seq_Direct_B, INST_CLASS_FLOW_CONTROL, 0, "Find direct template backward and move the flow head"),
    
    tInstLibEntry<tMethod>("mov-head", &cHardwareExperimental::Inst_MoveHead, INST_CLASS_FLOW_CONTROL, 0, "Move head ?IP? to the flow head"),
    tInstLibEntry<tMethod>("mov-head-if-n-equ", &cHardwareExperimental::Inst_MoveHeadIfNEqu, INST_CLASS_FLOW_CONTROL, 0, "Move head ?IP? to the flow head if ?BX? != ?CX?"),
    tInstLibEntry<tMethod>("mov-head-if-less", &cHardwareExperimental::Inst_MoveHeadIfLess, INST_CLASS_FLOW_CONTROL, 0, "Move head ?IP? to the flow head if ?BX? != ?CX?"),
    
    tInstLibEntry<tMethod>("goto", &cHardwareExperimental::Inst_Goto, INST_CLASS_FLOW_CONTROL, 0, "Move IP to labeled position matching the label that follows"),
    tInstLibEntry<tMethod>("goto-if-n-equ", &cHardwareExperimental::Inst_GotoIfNEqu, INST_CLASS_FLOW_CONTROL, 0, "Move IP to labeled position if BX != CX"),
    tInstLibEntry<tMethod>("goto-if-less", &cHardwareExperimental::Inst_GotoIfLess, INST_CLASS_FLOW_CONTROL, 0, "Move IP to labeled position if BX < CX"),
    tInstLibEntry<tMethod>("goto-if-cons", &cHardwareExperimental::Inst_GotoConsensus, INST_CLASS_FLOW_CONTROL, 0, "Move IP to the labeled position if BX consensus"), 
    tInstLibEntry<tMethod>("goto-if-cons-24", &cHardwareExperimental::Inst_GotoConsensus24, INST_CLASS_FLOW_CONTROL, 0, "Move IP to the labeled position if BX consensus"),
    
    tInstLibEntry<tMethod>("jmp-head", &cHardwareExperimental::Inst_JumpHead, INST_CLASS_FLOW_CONTROL, 0, "Move head ?Flow? by amount in ?CX? register"),
    tInstLibEntry<tMethod>("get-head", &cHardwareExperimental::Inst_GetHead, INST_CLASS_FLOW_CONTROL, 0, "Copy the position of the ?IP? head into ?CX?"),
    
    
    // Replication Instructions
    tInstLibEntry<tMethod>("h-alloc", &cHardwareExperimental::Inst_HeadAlloc, INST_CLASS_LIFECYCLE, 0, "Allocate maximum allowed space"),
    tInstLibEntry<tMethod>("h-divide", &cHardwareExperimental::Inst_HeadDivide, INST_CLASS_LIFECYCLE, nInstFlag::STALL, "Divide code between read and write heads."),
    tInstLibEntry<tMethod>("h-divide-sex", &cHardwareExperimental::Inst_HeadDivideSex, INST_CLASS_LIFECYCLE, nInstFlag::STALL, "Divide code between read and write heads."),
    tInstLibEntry<tMethod>("h-copy", &cHardwareExperimental::Inst_HeadCopy, INST_CLASS_LIFECYCLE, 0, "Copy from read-head to write-head; advance both"),
    tInstLibEntry<tMethod>("h-reqd", &cHardwareExperimental::Inst_HeadRead, INST_CLASS_LIFECYCLE, 0, "Read instruction from ?read-head? to ?AX?; advance the head."),
    tInstLibEntry<tMethod>("h-write", &cHardwareExperimental::Inst_HeadWrite, INST_CLASS_LIFECYCLE, 0, "Write to ?write-head? instruction from ?AX?; advance the head."),
    tInstLibEntry<tMethod>("if-copied-lbl-comp", &cHardwareExperimental::Inst_IfCopiedCompLabel, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied complement of attached label"),
    tInstLibEntry<tMethod>("if-copied-lbl-direct", &cHardwareExperimental::Inst_IfCopiedDirectLabel, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied direct match of the attached label"),
    tInstLibEntry<tMethod>("if-copied-seq-comp", &cHardwareExperimental::Inst_IfCopiedCompSeq, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied complement of attached sequence"),
    tInstLibEntry<tMethod>("if-copied-seq-direct", &cHardwareExperimental::Inst_IfCopiedDirectSeq, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied direct match of the attached sequence"),
    
    tInstLibEntry<tMethod>("repro", &cHardwareExperimental::Inst_Repro, INST_CLASS_LIFECYCLE, nInstFlag::STALL, "Instantly reproduces the organism"),
    
    tInstLibEntry<tMethod>("die", &cHardwareExperimental::Inst_Die, INST_CLASS_LIFECYCLE, nInstFlag::STALL, "Instantly kills the organism"),
    
    // Thread Execution Control
    tInstLibEntry<tMethod>("wait-cond-equ", &cHardwareExperimental::Inst_WaitCondition_Equal, INST_CLASS_OTHER, nInstFlag::STALL, ""),
    tInstLibEntry<tMethod>("wait-cond-less", &cHardwareExperimental::Inst_WaitCondition_Less, INST_CLASS_OTHER, nInstFlag::STALL, ""),
    tInstLibEntry<tMethod>("wait-cond-gtr", &cHardwareExperimental::Inst_WaitCondition_Greater, INST_CLASS_OTHER, nInstFlag::STALL, ""),
        
    // Promoter Model
    tInstLibEntry<tMethod>("promoter", &cHardwareExperimental::Inst_Promoter, INST_CLASS_FLOW_CONTROL, nInstFlag::PROMOTER),
    tInstLibEntry<tMethod>("terminate", &cHardwareExperimental::Inst_Terminate, INST_CLASS_FLOW_CONTROL),
    tInstLibEntry<tMethod>("term-cons", &cHardwareExperimental::Inst_TerminateConsensus, INST_CLASS_FLOW_CONTROL),
    tInstLibEntry<tMethod>("term-cons-24", &cHardwareExperimental::Inst_TerminateConsensus24, INST_CLASS_FLOW_CONTROL),
    tInstLibEntry<tMethod>("regulate", &cHardwareExperimental::Inst_Regulate, INST_CLASS_FLOW_CONTROL),
    tInstLibEntry<tMethod>("regulate-sp", &cHardwareExperimental::Inst_RegulateSpecificPromoters, INST_CLASS_FLOW_CONTROL),
    tInstLibEntry<tMethod>("s-regulate", &cHardwareExperimental::Inst_SenseRegulate, INST_CLASS_FLOW_CONTROL),
    tInstLibEntry<tMethod>("numberate", &cHardwareExperimental::Inst_Numberate, INST_CLASS_DATA),
    tInstLibEntry<tMethod>("numberate-24", &cHardwareExperimental::Inst_Numberate24, INST_CLASS_DATA),
    tInstLibEntry<tMethod>("bit-cons", &cHardwareExperimental::Inst_BitConsensus, INST_CLASS_DATA),
    tInstLibEntry<tMethod>("bit-cons-24", &cHardwareExperimental::Inst_BitConsensus24, INST_CLASS_DATA),
    tInstLibEntry<tMethod>("execurate", &cHardwareExperimental::Inst_Execurate, INST_CLASS_DATA),
    tInstLibEntry<tMethod>("execurate-24", &cHardwareExperimental::Inst_Execurate24, INST_CLASS_DATA),
    
    
    // State Grid instructions
    tInstLibEntry<tMethod>("sg-move", &cHardwareExperimental::Inst_SGMove, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("sg-rotate-l", &cHardwareExperimental::Inst_SGRotateL, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("sg-rotate-r", &cHardwareExperimental::Inst_SGRotateR, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("sg-sense", &cHardwareExperimental::Inst_SGSense, INST_CLASS_ENVIRONMENT),
    
    // Movement and Navigation instructions
    tInstLibEntry<tMethod>("move", &cHardwareExperimental::Inst_Move, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("juv-move", &cHardwareExperimental::Inst_JuvMove, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("range-move", &cHardwareExperimental::Inst_RangeMove, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("range-pred-move", &cHardwareExperimental::Inst_RangePredMove, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-cell-xy", &cHardwareExperimental::Inst_GetCellPosition, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("get-cell-x", &cHardwareExperimental::Inst_GetCellPositionX, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("get-cell-y", &cHardwareExperimental::Inst_GetCellPositionY, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("get-north-offset", &cHardwareExperimental::Inst_GetNorthOffset, INST_CLASS_ENVIRONMENT),    
    tInstLibEntry<tMethod>("get-position-offset", &cHardwareExperimental::Inst_GetPositionOffset, INST_CLASS_ENVIRONMENT),    
    tInstLibEntry<tMethod>("get-northerly", &cHardwareExperimental::Inst_GetNortherly, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("get-easterly", &cHardwareExperimental::Inst_GetEasterly, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("zero-easterly", &cHardwareExperimental::Inst_ZeroEasterly, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("zero-northerly", &cHardwareExperimental::Inst_ZeroNortherly, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("zero-position-offset", &cHardwareExperimental::Inst_ZeroPosOffset, INST_CLASS_ENVIRONMENT),

    // Rotation
    tInstLibEntry<tMethod>("rotate-left-one", &cHardwareExperimental::Inst_RotateLeftOne, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-right-one", &cHardwareExperimental::Inst_RotateRightOne, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-uphill", &cHardwareExperimental::Inst_RotateUphill, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-up-ft-hill", &cHardwareExperimental::Inst_RotateUpFtHill, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-home", &cHardwareExperimental::Inst_RotateHome, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-to-unoccupied-cell", &cHardwareExperimental::Inst_RotateUnoccupiedCell, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-x", &cHardwareExperimental::Inst_RotateX, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-org-id", &cHardwareExperimental::Inst_RotateOrgID, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-away-org-id", &cHardwareExperimental::Inst_RotateAwayOrgID, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    
    // Neural networking instructions 
    tInstLibEntry<tMethod>("rotate-neuron-AV-left-one", &cHardwareExperimental::Inst_RotateNeuronAVLeft, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-neuron-AV-right-one", &cHardwareExperimental::Inst_RotateNeuronAVRight, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-neuron-AV-by-X", &cHardwareExperimental::Inst_RotateNeuronAVbyX, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("move-neuron-AV", &cHardwareExperimental::Inst_MoveNeuronAV, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("if-neuron-input-has-output-AV", &cHardwareExperimental::Inst_IfNeuronInputHasOutputAV, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("if-not-neuron-input-has-output-AV", &cHardwareExperimental::Inst_IfNotNeuronInputHasOutputAV, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("if-neuron-input-faced-has-output-AV", &cHardwareExperimental::Inst_IfNeuronInputFacedHasOutputAV, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("if-not-neuron-input-faced-has-output-AV", &cHardwareExperimental::Inst_IfNotNeuronInputFacedHasOutputAV, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    
    // Resource and Topography Sensing
    tInstLibEntry<tMethod>("sense-resource-id", &cHardwareExperimental::Inst_SenseResourceID, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("sense-res-quant", &cHardwareExperimental::Inst_SenseResQuant, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("sense-nest", &cHardwareExperimental::Inst_SenseNest, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("sense-res-diff", &cHardwareExperimental::Inst_SenseResDiff, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("sense-faced-habitat", &cHardwareExperimental::Inst_SenseFacedHabitat, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("look-ahead", &cHardwareExperimental::Inst_LookAhead, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("look-ahead-intercept", &cHardwareExperimental::Inst_LookAheadIntercept, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("look-around", &cHardwareExperimental::Inst_LookAround, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("look-around-intercept", &cHardwareExperimental::Inst_LookAroundIntercept, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("look-ft", &cHardwareExperimental::Inst_LookFT, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("look-around-ft", &cHardwareExperimental::Inst_LookAroundFT, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("set-forage-target", &cHardwareExperimental::Inst_SetForageTarget, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("set-ft-once", &cHardwareExperimental::Inst_SetForageTargetOnce, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-forage-target", &cHardwareExperimental::Inst_GetForageTarget, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("get-loc-org-density", &cHardwareExperimental::Inst_GetLocOrgDensity, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("get-faced-org-density", &cHardwareExperimental::Inst_GetFacedOrgDensity, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    
    tInstLibEntry<tMethod>("collect-edible", &cHardwareExperimental::Inst_CollectEdible, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("collect-specific", &cHardwareExperimental::Inst_CollectSpecific, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("deposit-resource", &cHardwareExperimental::Inst_DepositResource, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("deposit-specific", &cHardwareExperimental::Inst_DepositSpecific, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("deposit-all-as-specific", &cHardwareExperimental::Inst_DepositAllAsSpecific, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("nop-deposit-specific", &cHardwareExperimental::Inst_NopDepositSpecific, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("nop-deposit-resource", &cHardwareExperimental::Inst_NopDepositResource, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("nop-deposit-all-as-specific", &cHardwareExperimental::Inst_NopDepositAllAsSpecific, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("nop-collect-edible", &cHardwareExperimental::Inst_NopCollectEdible, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("get-res-stored", &cHardwareExperimental::Inst_GetResStored, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
    tInstLibEntry<tMethod>("get-specific-stored", &cHardwareExperimental::Inst_GetSpecificStored, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),    
 
    // Data collection
    tInstLibEntry<tMethod>("collect-cell-data", &cHardwareExperimental::Inst_CollectCellData, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("if-cell-data-changed", &cHardwareExperimental::Inst_IfCellDataChanged, INST_CLASS_CONDITIONAL, nInstFlag::STALL),
    tInstLibEntry<tMethod>("read-cell-data", &cHardwareExperimental::Inst_ReadCellData, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("read-group-cell", &cHardwareExperimental::Inst_ReadGroupCell, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),

    // Opinion instructions.
    tInstLibEntry<tMethod>("set-opinion", &cHardwareExperimental::Inst_SetOpinion, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-opinion", &cHardwareExperimental::Inst_GetOpinion, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),

    // Grouping instructions
    tInstLibEntry<tMethod>("join-group", &cHardwareExperimental::Inst_JoinGroup, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("change-pred-group", &cHardwareExperimental::Inst_ChangePredGroup, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("make-pred-group", &cHardwareExperimental::Inst_MakePredGroup, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("leave-pred-group", &cHardwareExperimental::Inst_LeavePredGroup, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("adopt-pred-group", &cHardwareExperimental::Inst_AdoptPredGroup, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-group-id", &cHardwareExperimental::Inst_GetGroupID, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("get-pred-group-id", &cHardwareExperimental::Inst_GetPredGroupID, INST_CLASS_ENVIRONMENT),
    tInstLibEntry<tMethod>("inc-pred-tolerance", &cHardwareExperimental::Inst_IncPredTolerance, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("dec-pred-tolerance", &cHardwareExperimental::Inst_DecPredTolerance, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-pred-tolerance", &cHardwareExperimental::Inst_GetPredTolerance, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),   
    tInstLibEntry<tMethod>("get-pred-group-tolerance", &cHardwareExperimental::Inst_GetPredGroupTolerance, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    
    // Org Interaction instructions
    tInstLibEntry<tMethod>("get-faced-org-id", &cHardwareExperimental::Inst_GetFacedOrgID, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("attack-prey", &cHardwareExperimental::Inst_AttackPrey, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("attack-ft-prey", &cHardwareExperimental::Inst_AttackFTPrey, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("fight-merit-org", &cHardwareExperimental::Inst_FightMeritOrg, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("fight-bonus-org", &cHardwareExperimental::Inst_FightBonusOrg, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("mark-cell", &cHardwareExperimental::Inst_MarkCell, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("mark-group-cell", &cHardwareExperimental::Inst_MarkGroupCell, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("mark-pred-cell", &cHardwareExperimental::Inst_MarkPredCell, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("read-faced-cell", &cHardwareExperimental::Inst_ReadFacedCell, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("read-faced-pred-cell", &cHardwareExperimental::Inst_ReadFacedPredCell, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-merit-fight-odds", &cHardwareExperimental::Inst_GetMeritFightOdds, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("fight-org", &cHardwareExperimental::Inst_FightOrg, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("attack-pred", &cHardwareExperimental::Inst_AttackPred, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("kill-pred", &cHardwareExperimental::Inst_KillPred, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("fight-pred", &cHardwareExperimental::Inst_FightPred, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("teach-offspring", &cHardwareExperimental::Inst_TeachOffspring, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("learn-parent", &cHardwareExperimental::Inst_LearnParent, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("check-faced-kin", &cHardwareExperimental::Inst_CheckFacedKin, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 

    tInstLibEntry<tMethod>("set-guard", &cHardwareExperimental::Inst_SetGuard, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("set-guard-once", &cHardwareExperimental::Inst_SetGuardOnce, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-num-guards", &cHardwareExperimental::Inst_GetNumGuards, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-num-juvs", &cHardwareExperimental::Inst_GetNumJuvs, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),

    tInstLibEntry<tMethod>("activate-display", &cHardwareExperimental::Inst_ActivateDisplay, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("update-display", &cHardwareExperimental::Inst_UpdateDisplay, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("modify-display", &cHardwareExperimental::Inst_ModifyDisplay, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("read-last-seen-display", &cHardwareExperimental::Inst_ReadLastSeenDisplay, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("kill-display", &cHardwareExperimental::Inst_KillDisplay, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("modify-simp-display", &cHardwareExperimental::Inst_ModifySimpDisplay, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("read-simp-display", &cHardwareExperimental::Inst_ReadLastSimpDisplay, INST_CLASS_ENVIRONMENT, nInstFlag::STALL), 

    // Messaging
    tInstLibEntry<tMethod>("send-msg", &cHardwareExperimental::Inst_SendMessage, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("retrieve-msg", &cHardwareExperimental::Inst_RetrieveMessage, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),

    tInstLibEntry<tMethod>("send-msg-interrupt-type0", &cHardwareExperimental::Inst_SendMessageInterruptType0, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("send-msg-interrupt-type1", &cHardwareExperimental::Inst_SendMessageInterruptType1, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("send-msg-interrupt-type2", &cHardwareExperimental::Inst_SendMessageInterruptType2, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("send-msg-interrupt-type3", &cHardwareExperimental::Inst_SendMessageInterruptType3, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("send-msg-interrupt-type4", &cHardwareExperimental::Inst_SendMessageInterruptType4, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("send-msg-interrupt-type5", &cHardwareExperimental::Inst_SendMessageInterruptType5, INST_CLASS_ENVIRONMENT, nInstFlag::STALL),
    tInstLibEntry<tMethod>("msg-handler-type0", &cHardwareExperimental::Inst_START_Handler),
    tInstLibEntry<tMethod>("msg-handler-type1", &cHardwareExperimental::Inst_START_Handler),
    tInstLibEntry<tMethod>("msg-handler-type2", &cHardwareExperimental::Inst_START_Handler),
    tInstLibEntry<tMethod>("msg-handler-type3", &cHardwareExperimental::Inst_START_Handler),
    tInstLibEntry<tMethod>("msg-handler-type4", &cHardwareExperimental::Inst_START_Handler),
    tInstLibEntry<tMethod>("msg-handler-type5", &cHardwareExperimental::Inst_START_Handler),
    tInstLibEntry<tMethod>("end-handler", &cHardwareExperimental::Inst_End_Handler),

    // Control-type Instructions
    tInstLibEntry<tMethod>("scramble-registers", &cHardwareExperimental::Inst_ScrambleReg, INST_CLASS_DATA, nInstFlag::STALL),

    // DEPRECATED Instructions
    tInstLibEntry<tMethod>("set-flow", &cHardwareExperimental::Inst_SetFlow, INST_CLASS_FLOW_CONTROL, 0, "Set flow-head to position in ?CX?")
  };
  
  
  const int n_size = sizeof(s_n_array)/sizeof(cNOPEntry);
  
  static cString n_names[n_size];
  static int nop_mods[n_size];
  for (int i = 0; i < n_size && i < NUM_REGISTERS; i++) {
    n_names[i] = s_n_array[i].name;
    nop_mods[i] = s_n_array[i].nop_mod;
  }
  
  const int f_size = sizeof(s_f_array)/sizeof(tInstLibEntry<tMethod>);
  static tMethod functions[f_size];
  for (int i = 0; i < f_size; i++) functions[i] = s_f_array[i].GetFunction();
  
  const int def = 0;
  const int null_inst = 16;
  
  return new tInstLib<tMethod>(f_size, s_f_array, n_names, nop_mods, functions, def, null_inst);
}

cHardwareExperimental::cHardwareExperimental(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set)
: cHardwareBase(world, in_organism, in_inst_set), m_sensor(world, in_organism)
{
  m_functions = s_inst_slib->GetFunctions();
  
  m_spec_die = false;
  
  m_thread_slicing_parallel = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1);
  m_no_cpu_cycle_time = m_world->GetConfig().NO_CPU_CYCLE_TIME.Get();
  
  m_promoters_enabled = m_world->GetConfig().PROMOTERS_ENABLED.Get();
  if (m_promoters_enabled) {
    m_constitutive_regulation = m_world->GetConfig().CONSTITUTIVE_REGULATION.Get();
    m_no_active_promoter_halt = (m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get() == 2);
  }
  
  m_slip_read_head = !m_world->GetConfig().SLIP_COPY_MODE.Get();
  
  const Genome& in_genome = in_organism->GetGenome();
  ConstInstructionSequencePtr in_seq_p;
  in_seq_p.DynamicCastFrom(in_genome.Representation());
  const InstructionSequence& in_seq = *in_seq_p;
  
  m_memory = in_seq;  // Initialize memory...
  m_use_avatar = m_world->GetConfig().USE_AVATARS.Get();
  Reset(ctx);                            // Setup the rest of the hardware...
}


void cHardwareExperimental::internalReset()
{
  m_cycle_count = 0;
  m_last_output = 0;
  m_global_stack.Clear();
  
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
  
  // Promoter model
  if (m_promoters_enabled) {
    m_promoter_index = -1; // Meaning the last promoter was nothing
    m_promoter_offset = 0;
    
    m_promoters.Resize(0);
    
    for (int i=0; i < m_memory.GetSize(); i++) {
      if (m_inst_set->IsPromoter(m_memory[i])) {
        int code = Numberate(i - 1, -1, m_world->GetConfig().PROMOTER_CODE_SIZE.Get());
        m_promoters.Push(cPromoter(i, code));
      }
    }
  }
  
  m_io_expire = m_world->GetConfig().IO_EXPIRE.Get();
  m_use_avatar = m_world->GetConfig().USE_AVATARS.Get();
  m_sensor.Reset();
}


void cHardwareExperimental::internalResetOnFailedDivide()
{
	internalReset();
}

void cHardwareExperimental::cLocalThread::operator=(const cLocalThread& in_thread)
{
  m_id = in_thread.m_id;
  m_promoter_inst_executed = in_thread.m_promoter_inst_executed;
  m_execurate = in_thread.m_execurate;
  m_messageTriggerType = in_thread.m_messageTriggerType;
  
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

void cHardwareExperimental::cLocalThread::Reset(cHardwareExperimental* in_hardware, int in_id)
{
  m_id = in_id;
  
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i].Clear();
  for (int i = 0; i < NUM_HEADS; i++) heads[i].Reset(in_hardware);
  
  stack.Clear();
  cur_stack = 0;
  cur_head = nHardware::HEAD_IP;
  
  reading_label = false;
  reading_seq = false;
  active = true;
  read_label.Clear();
  next_label.Clear();
  
  // Promoter model
  m_execurate = 0;
  m_promoter_inst_executed = 0;

  m_messageTriggerType = -1;
}


// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.

bool cHardwareExperimental::SingleProcess(cAvidaContext& ctx, bool speculative)
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
  
  // First instruction - check whether we should be starting at a promoter, when enabled.
  if (phenotype.GetCPUCyclesUsed() == 0 && m_promoters_enabled) PromoterTerminate(ctx);
  
  m_cycle_count++;
//  assert(m_cycle_count < 0x8000);
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
    cHeadCPU& ip = m_threads[m_cur_thread].heads[nHardware::HEAD_IP];
    ip.Adjust();
    
   
    // Print the status of this CPU at each step...    
    if (m_tracer != NULL) m_tracer->TraceHardware(ctx, *this);
    
    // Find the instruction to be executed
    const Instruction& cur_inst = ip.GetInst();
    
    if (speculative && (m_spec_die || m_inst_set->ShouldStall(cur_inst))) {
      // Speculative instruction reject, flush and return
      m_cur_thread = last_thread;
      phenotype.DecCPUCyclesUsed();
      if (!m_no_cpu_cycle_time) phenotype.IncTimeUsed(-1);
      m_organism->SetRunning(false);
      return false;
    }
    
    // Print the short form status of this CPU at each step... 
    if (m_minitracer != NULL) m_minitracer->TraceHardware(ctx, *this, false, true);
    
    // Test if costs have been paid and it is okay to execute this now...
    bool exec = true;
    int exec_success = 0;
    
    // record any failure due to costs being paid
    // before we try to execute the instruction, is this org currently paying precosts for it
    bool on_pause = IsPayingActiveCost(ctx, m_cur_thread);
    if (m_has_any_costs) exec = SingleProcess_PayPreCosts(ctx, cur_inst, m_cur_thread);    
    if (!exec) exec_success = -1;

    if (m_promoters_enabled) {
      // Constitutive regulation applied here
      if (m_constitutive_regulation) Inst_SenseRegulate(ctx); 
      
      // If there are no active promoters and a certain mode is set, then don't execute any further instructions
      if (m_no_active_promoter_halt && m_promoter_index == -1) exec = false;
    }
    
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
      
      //Add to the promoter inst executed count before executing the inst (in case it is a terminator)
      if (m_promoters_enabled) m_threads[m_cur_thread].IncPromoterInstExecuted();
      
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
        if (m_minitracer != NULL) m_minitracer->TraceHardware(ctx, *this, false, true, exec_success);
        break;
      }
      
      // Some instruction (such as jump) may turn m_advance_ip off.  Usually
      // we now want to move to the next instruction in the memory.
      if (m_advance_ip == true) ip.Advance();
      
      // Pay the additional death_cost of the instruction now
      phenotype.IncTimeUsed(addl_time_cost);
      
      // In the promoter model, we may force termination after a certain number of inst have been executed
      if (m_promoters_enabled) {
        const double processivity = m_world->GetConfig().PROMOTER_PROCESSIVITY.Get();
        if (ctx.GetRandom().P(1 - processivity)) PromoterTerminate(ctx);
        if (m_world->GetConfig().PROMOTER_INST_MAX.Get() &&
            (m_threads[m_cur_thread].GetPromoterInstExecuted() >= m_world->GetConfig().PROMOTER_INST_MAX.Get())) {
          PromoterTerminate(ctx);
        }
      }
    }
    // if using mini traces, report success or failure of execution
    if (m_minitracer != NULL) m_minitracer->TraceHardware(ctx, *this, false, true, exec_success);
    
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
      if (m_microtrace || m_topnavtrace) {
        RecordMicroTrace(cur_inst);
        if (m_topnavtrace) RecordNavTrace(m_use_avatar);      
      }
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
bool cHardwareExperimental::SingleProcess_ExecuteInst(cAvidaContext& ctx, const Instruction& cur_inst) 
{
  // Copy Instruction locally to handle stochastic effects
  Instruction actual_inst = cur_inst;
  
#ifdef EXECUTION_ERRORS
  // If there is an execution error, execute a random instruction.
  if (m_organism->TestExeErr()) actual_inst = m_inst_set->GetRandomInst(ctx);
#endif /* EXECUTION_ERRORS */
  
  // Get a pointer to the corrisponding method...
  int inst_idx = m_inst_set->GetLibFunctionIndex(actual_inst);
  
  // Mark the instruction as executed
  getIP().SetFlagExecuted();
	
  // instruction execution count incremeneted
  m_organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
  
  // And execute it.
  const bool exec_success = (this->*(m_functions[inst_idx]))(ctx);
  
	if (exec_success) {
    int code_len = m_world->GetConfig().INST_CODE_LENGTH.Get();
    m_threads[m_cur_thread].UpdateExecurate(code_len, m_inst_set->GetInstructionCode(actual_inst));
  }
  
  // decremenet if the instruction was not executed successfully
  if (exec_success == false) {
    m_organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  }
  
  return exec_success;
}


void cHardwareExperimental::ProcessBonusInst(cAvidaContext& ctx, const Instruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = m_organism->IsRunning();
  m_organism->SetRunning(true);
  
  if (m_tracer != NULL) m_tracer->TraceHardware(ctx, *this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  m_organism->SetRunning(prev_run_state);
}


void cHardwareExperimental::PrintStatus(ostream& fp)
{
  fp << "CPU CYCLE:" << m_organism->GetPhenotype().GetCPUCyclesUsed() << " ";
  fp << "THREAD:" << m_cur_thread << "  ";
  fp << "IP:" << getIP().GetPosition() << "    ";
  
  
  for (int i = 0; i < NUM_REGISTERS; i++) {
    sInternalValue& reg = m_threads[m_cur_thread].reg[i];
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
  
  fp << "  R-Head:" << getHead(nHardware::HEAD_READ).GetPosition() << " "
  << "W-Head:" << getHead(nHardware::HEAD_WRITE).GetPosition()  << " "
  << "F-Head:" << getHead(nHardware::HEAD_FLOW).GetPosition()   << "  "
  << "RL:" << GetReadLabel().AsString() << "   "
  << "Ex:" << m_last_output
  << endl;
  
  int number_of_stacks = GetNumStacks();
  for (int stack_id = 0; stack_id < number_of_stacks; stack_id++) {
    fp << ((m_threads[m_cur_thread].cur_stack == stack_id) ? '*' : ' ') << " Stack " << stack_id << ":" << setbase(16) << setfill('0');
    for (int i = 0; i < nHardware::STACK_SIZE; i++) fp << " Ox" << setw(8) << GetStack(i, stack_id, 0);
    fp << setfill(' ') << setbase(10) << endl;
  }
  
  fp << "  Mem (" << m_memory.GetSize() << "):"
  << "  " << m_memory.AsString()
  << endl;
  
  
  if (m_world->GetConfig().PROMOTERS_ENABLED.Get())
  {
    fp << "Promoters: index=" << m_promoter_index << " offset=" << m_promoter_offset;
    fp << " exe_inst=" << m_threads[m_cur_thread].GetPromoterInstExecuted();
    for (int i=0; i<m_promoters.GetSize(); i++)
    {
      fp << setfill(' ') << setbase(10) << m_promoters[i].pos << ":";
      fp << "Ox" << setbase(16) << setfill('0') << setw(8) << (m_promoters[i].GetRegulatedBitCode()) << " "; 
    }
    fp << endl;    
    fp << setfill(' ') << setbase(10) << endl;
  }    
  
  fp.flush();
}

void cHardwareExperimental::SetupMiniTraceFileHeader(const cString& filename, const int gen_id, const cString& genotype)
{
  const Genome& in_genome = m_organism->GetGenome();
  ConstInstructionSequencePtr in_seq_p;
  in_seq_p.DynamicCastFrom(in_genome.Representation());
  const InstructionSequence& in_seq = *in_seq_p;

  cDataFile& df = m_world->GetDataFile(filename);
  df.WriteTimeStamp();
  cString org_dat("");
  df.WriteComment(org_dat.Set("Update Born: %d", m_world->GetStats().GetUpdate()));
  df.WriteComment(org_dat.Set("Org ID: %d", m_organism->GetID()));
  df.WriteComment(org_dat.Set("Genotype ID: %d", gen_id));
  df.WriteComment(org_dat.Set("Genotype: %s", (const char*) genotype));
  df.WriteComment(org_dat.Set("Genome Length: %d", in_seq.GetSize()));
  df.WriteComment(" ");
  df.WriteComment("Exec Stats Columns:");
  df.WriteComment("CPU Cycle");
  df.WriteComment("Current Update");
  df.WriteComment("Register Contents (CPU Cycle Origin of Contents)");
  df.WriteComment("Current Thread");
  df.WriteComment("IP Position");
  df.WriteComment("RH Position");
  df.WriteComment("WH Position");
  df.WriteComment("FH Position");
  df.WriteComment("CPU Cycle of Last Output");
  df.WriteComment("Current Merit");
  df.WriteComment("Current Bonus");
  df.WriteComment("Forager Type");
  df.WriteComment("Group ID (opinion)");
  df.WriteComment("Current Cell");
  df.WriteComment("Avatar Cell");
  df.WriteComment("Faced Direction");
  df.WriteComment("Faced Cell Occupied?");
  df.WriteComment("Faced Cell Has Hill?");
  df.WriteComment("Faced Cell Has Wall?");
  df.WriteComment("Queued Instruction");
  df.WriteComment("Trailing NOPs");
  df.WriteComment("Did Queued Instruction Execute (-1=no, paying cpu costs; 0=failed; 1=yes)");
  df.Endl();
}

void cHardwareExperimental::PrintMiniTraceStatus(cAvidaContext& ctx, ostream& fp, const cString& next_name)
{
  // basic status info
  fp << m_cycle_count << " ";
  fp << m_world->GetStats().GetUpdate() << " ";
  for (int i = 0; i < NUM_REGISTERS; i++) {
    sInternalValue& reg = m_threads[m_cur_thread].reg[i];
    fp << GetRegister(i) << " ";
    fp << "(" << reg.originated << ") ";
  }    
  // genome loc info
  fp << m_cur_thread << " ";
  fp << getIP().GetPosition() << " ";  
  fp << getHead(nHardware::HEAD_READ).GetPosition() << " ";
  fp << getHead(nHardware::HEAD_WRITE).GetPosition()  << " ";
  fp << getHead(nHardware::HEAD_FLOW).GetPosition()   << " ";
  // last output
  fp << m_last_output << " ";
  // phenotype/org status info
  fp << m_organism->GetPhenotype().GetMerit().GetDouble() << " ";
  fp << m_organism->GetPhenotype().GetCurBonus() << " ";
  fp << m_organism->GetForageTarget() << " ";
  if (m_organism->HasOpinion()) fp << m_organism->GetOpinion().first << " ";
  else fp << -99 << " ";
  // environment info / things that affect movement
  fp << m_organism->GetOrgInterface().GetCellID() << " ";
  if (m_use_avatar) fp << m_organism->GetOrgInterface().GetAVCellID() << " ";
  if (!m_use_avatar) fp << m_organism->GetOrgInterface().GetFacedDir() << " ";
  else fp << m_organism->GetOrgInterface().GetAVFacing() << " ";
  if (!m_use_avatar) fp << m_organism->IsNeighborCellOccupied() << " ";  
  else fp << m_organism->GetOrgInterface().FacedHasAV() << " ";
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  Apto::Array<double> cell_resource_levels;
  if (!m_use_avatar) cell_resource_levels = m_organism->GetOrgInterface().GetFacedCellResources(ctx);
  else cell_resource_levels = m_organism->GetOrgInterface().GetAVFacedResources(ctx);
  int wall = 0;
  int hill = 0;
  for (int i = 0; i < cell_resource_levels.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 2 && cell_resource_levels[i] > 0) wall = 1;
    if (resource_lib.GetResource(i)->GetHabitat() == 1 && cell_resource_levels[i] > 0) hill = 1;
    if (hill == 1 && wall == 1) break;
  }
  fp << hill << " ";
  fp << wall << " ";
  // instruction about to be executed
  fp << next_name << " ";
  // any trailing nops (up to NUM_REGISTERS)
  cCPUMemory& memory = m_memory;
  int pos = getIP().GetPosition();
  Apto::Array<int, Apto::Smart> seq;
  seq.Resize(0);
  for (int i = 0; i < NUM_REGISTERS; i++) {
    pos += 1;
    if (pos >= memory.GetSize()) pos = 0;
    if (m_inst_set->IsNop(memory[pos])) seq.Push(m_inst_set->GetNopMod(memory[pos])); 
    else break;
  }
  cString mod_string;
  for (int j = 0; j < seq.GetSize(); j++) {
    mod_string += (char) seq[j] + 'A';  
  }  
  if (mod_string.GetSize() != 0) fp << mod_string << " ";
  else fp << "NoMods" << " ";
}

void cHardwareExperimental::PrintMiniTraceSuccess(ostream& fp, const int exec_sucess)
{
  fp << exec_sucess;
  fp << endl;
  fp.flush();
}

cHeadCPU cHardwareExperimental::FindLabelStart(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cCPUMemory& memory = m_memory;
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

cHeadCPU cHardwareExperimental::FindNopSequenceStart(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cCPUMemory& memory = m_memory;
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


cHeadCPU cHardwareExperimental::FindLabelForward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
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

cHeadCPU cHardwareExperimental::FindLabelBackward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
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




cHeadCPU cHardwareExperimental::FindNopSequenceForward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
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


cHeadCPU cHardwareExperimental::FindNopSequenceBackward(bool mark_executed)
{
  cHeadCPU& ip = getIP();
  const cCodeLabel& search_label = GetLabel();
  
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

void cHardwareExperimental::ReadInst(Instruction in_inst)
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

void cHardwareExperimental::AdjustHeads()
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

void cHardwareExperimental::ReadLabel(int max_size)
{
  int count = 0;
  cHeadCPU * inst_ptr = &( getIP() );
  
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

bool cHardwareExperimental::ExitThread()
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

bool cHardwareExperimental::Inst_IdThread(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, GetCurThreadID(), false);
  return true;
}

////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int cHardwareExperimental::FindModifiedRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_register = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_register;
}

inline int cHardwareExperimental::FindModifiedNextRegister(int default_register)
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

inline int cHardwareExperimental::FindModifiedPreviousRegister(int default_register)
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


inline int cHardwareExperimental::FindModifiedHead(int default_head)
{
  assert(default_head < NUM_HEADS); // Head ID too high.
  
  if (m_inst_set->IsNop(getIP().GetNextInst())) {
    getIP().Advance();
    default_head = m_inst_set->GetNopMod(getIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_head;
}


inline int cHardwareExperimental::FindNextRegister(int base_reg)
{
  return (base_reg + 1) % NUM_REGISTERS;
}


bool cHardwareExperimental::Allocate_Necro(const int new_size)
{
  m_memory.ResizeOld(new_size);
  return true;
}

bool cHardwareExperimental::Allocate_Random(cAvidaContext& ctx, const int old_size, const int new_size)
{
  m_memory.Resize(new_size);
  
  for (int i = old_size; i < new_size; i++) {
    m_memory[i] = m_inst_set->GetRandomInst(ctx);
  }
  return true;
}

bool cHardwareExperimental::Allocate_Default(const int new_size)
{
  m_memory.Resize(new_size);
  
  // New space already defaults to default instruction...
  
  return true;
}

bool cHardwareExperimental::Allocate_Main(cAvidaContext& ctx, const int allocated_size)
{
  // must do divide before second allocate & must allocate positive amount...
  if (m_world->GetConfig().REQUIRE_ALLOCATE.Get() && m_mal_active == true) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR, "Allocate already active");
    return false;
  }
  if (allocated_size < 1) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
                      cStringUtil::Stringf("Allocate of %d too small", allocated_size));
    return false;
  }
  
  const int old_size = m_memory.GetSize();
  const int new_size = old_size + allocated_size;
  
  // Make sure that the new size is in range.
  if (new_size > MAX_GENOME_LENGTH  ||  new_size < MIN_GENOME_LENGTH) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
                      cStringUtil::Stringf("Invalid post-allocate size (%d)",
                                           new_size));
    return false;
  }
  
  const int max_alloc_size = (int) (old_size * m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get());
  if (allocated_size > max_alloc_size) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
                      cStringUtil::Stringf("Allocate too large (%d > %d)",
                                           allocated_size, max_alloc_size));
    return false;
  }
  
  const int max_old_size =
  (int) (allocated_size * m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get());
  if (old_size > max_old_size) {
    m_organism->Fault(FAULT_LOC_ALLOC, FAULT_TYPE_ERROR,
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

int cHardwareExperimental::calcCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  for (int i = parent_size; i < parent_size + child_size; i++) {
    if (m_memory.FlagCopied(i)) copied_size++;
  }
  return copied_size;
}  

bool cHardwareExperimental::Divide_Main(cAvidaContext& ctx, const int div_point, const int extra_lines, double mut_multiplier)
{
  const int child_size = m_memory.GetSize() - div_point - extra_lines;
  
  // Make sure this divide will produce a viable offspring.
  const bool viable = Divide_CheckViable(ctx, div_point, child_size);
  if (viable == false) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  InstructionSequencePtr offspring_seq(new InstructionSequence(m_memory.Crop(div_point, div_point + child_size)));
  PropertyMap props;
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

void cHardwareExperimental::checkWaitingThreads(int cur_thread, int reg_num)
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
        sInternalValue& dest = m_threads[i].reg[m_threads[i].wait_dst];
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

// Multi-threading.
bool cHardwareExperimental::Inst_ForkThread(cAvidaContext&)
{
  getIP().Advance();
  if (!ForkThread()) m_organism->Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
  return true;
}

bool cHardwareExperimental::InterruptThread(int interruptType)
{
  // Will interrupt be successful? i.e. is head instruction present
  cString handlerHeadInstructionString;
  int interruptMsgType = -1;

  switch (interruptType) {
  case MSG_INTERRUPT:
    interruptMsgType = GetOrganism()->PeekAtNextMessageType();
    handlerHeadInstructionString.Set("msg-handler-type%d", interruptMsgType);
    break;
  case MOVE_INTERRUPT:
    handlerHeadInstructionString.Set("moved-handler");
    break;
  default:
    cerr << "Unknown interrupt type " << interruptType << "  Exitting.\n\n";
    exit(-1);
    break;
  }

  const Instruction label_inst = GetInstSet().GetInst(handlerHeadInstructionString);

  cHeadCPU search_head(IP());
  int start_pos = search_head.GetPosition();
  search_head++;

  while (start_pos != search_head.GetPosition()) {
    if (search_head.GetInst() == label_inst) { // found handlerHeadInstructionString
      search_head++; // one instruction past instruction
      break;
    }
    search_head++;
  }

  if (start_pos == search_head.GetPosition()) return false;

  if (ForkThread()) {
    const int num_threads = m_threads.GetSize() - 1;
    m_threads[num_threads].setMessageTriggerType(interruptMsgType);

    int old_thread = m_cur_thread;
    m_cur_thread = num_threads;

    // move all heads to one past beginning of interrupt
    for (int i = 0; i < NUM_HEADS; i++) {
      GetHead(i).Set(search_head.GetPosition());
    }

    switch (interruptType) {
    case MSG_INTERRUPT:
      IP().Retreat();
      Inst_RetrieveMessage(m_world->GetDefaultContext());
      IP().Advance();
      break;
    case MOVE_INTERRUPT:
      break;
    }
    m_cur_thread = old_thread;
    return true;
  }
  return false;
}

bool cHardwareExperimental::Inst_ExitThread(cAvidaContext& ctx)
{
  if (!ExitThread()) m_organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
  else m_advance_ip = false;
  return true;
}

bool cHardwareExperimental::Inst_IfNEqu(cAvidaContext&) // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) == GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLess(cAvidaContext&) // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) >=  GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfNotZero(cAvidaContext&)  // Execute next if ?bx? != 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) == 0)  getIP().Advance();
  return true;
}
bool cHardwareExperimental::Inst_IfEqualZero(cAvidaContext&)  // Execute next if ?bx? == 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) != 0)  getIP().Advance();
  return true;
}
bool cHardwareExperimental::Inst_IfGreaterThanZero(cAvidaContext&)  // Execute next if ?bx? > 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) <= 0)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLessThanZero(cAvidaContext&)  // Execute next if ?bx? < 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) >= 0)  getIP().Advance();
  return true;
}


bool cHardwareExperimental::Inst_IfGtrX(cAvidaContext&)       // Execute next if BX > X; X value set according to NOP label
{
  // Compares value in BX to a specific value.  The value to compare to is determined by the nop label as follows:
  //    no nop label (default): valueToCompare = 1;
  //    nop-A: toggles valueToCompare sign-bit 
  //    nop-B: valueToCompare left-shift by 1-bit
  //    nop-C: valueToCompare left-shift by 2-bits
  //    nop-D: valueToCompare left-shift by 3-bits, etc.
  
  int valueToCompare = 1;
  
  ReadLabel();
  const cCodeLabel& shift_label = GetLabel();
  for (int i = 0; i < shift_label.GetSize(); i++) {
    if (shift_label[i] == rAX) {
      valueToCompare *= -1;
    } else {
      valueToCompare <<= shift_label[i];
    }
  }
  
  if (GetRegister(rBX) <= valueToCompare)  getIP().Advance();
  
  return true;
}

bool cHardwareExperimental::Inst_IfEquX(cAvidaContext&)       // Execute next if BX == X; X value set according to NOP label
{
  // Compares value in BX to a specific value.  The value to compare to is determined by the nop label as follows:
  //    no nop label (default): valueToCompare = 1;
  //    nop-A: toggles valueToCompare sign-bit 
  //    nop-B: valueToCompare left-shift by 1-bit
  //    nop-C: valueToCompare left-shift by 2-bits
  //    nop-D: valueToCompare left-shift by 3-bits, etc.
  
  int valueToCompare = 1;
  
  ReadLabel();
  const cCodeLabel& shift_label = GetLabel();
  for (int i = 0; i < shift_label.GetSize(); i++) {
    if (shift_label[i] == rAX) {
      valueToCompare *= -1;
    } else {
      valueToCompare <<= shift_label[i];
    }
  }
  
  if (GetRegister(rBX) != valueToCompare)  getIP().Advance();
  
  return true;
}



bool cHardwareExperimental::Inst_IfConsensus(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(rBX);
  if (BitCount(GetRegister(op1)) <  CONSENSUS)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfConsensus24(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(rBX);
  if (BitCount(GetRegister(op1) & MASK24) <  CONSENSUS24)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLessConsensus(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1)) >=  BitCount(GetRegister(op2)))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLessConsensus24(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1) & MASK24) >=  BitCount(GetRegister(op2) & MASK24))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfStackGreater(cAvidaContext&)
{
  int cur_stack = m_threads[m_cur_thread].cur_stack;
  if (getStack(cur_stack).Peek().value <=  getStack(!cur_stack).Peek().value)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_Label(cAvidaContext&)
{
  ReadLabel();
  return true;
}

bool cHardwareExperimental::Inst_Pop(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  sInternalValue pop = stackPop();
  setInternalValue(reg_used, pop.value, pop);
  return true;
}

bool cHardwareExperimental::Inst_Push(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  getStack(m_threads[m_cur_thread].cur_stack).Push(m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareExperimental::Inst_PopAll(cAvidaContext&)
{
  int reg_used = FindModifiedRegister(rBX);
  for (int i = 0; i < NUM_REGISTERS; i++) {
    sInternalValue pop = stackPop();
    setInternalValue(reg_used, pop.value, pop);
    reg_used++;
    if (reg_used == NUM_REGISTERS) reg_used = 0;
  }
  return true;
}

bool cHardwareExperimental::Inst_PushAll(cAvidaContext&)
{
  int reg_used = FindModifiedRegister(rBX);
  for (int i = 0; i < NUM_REGISTERS; i++) {
    getStack(m_threads[m_cur_thread].cur_stack).Push(m_threads[m_cur_thread].reg[reg_used]);
    reg_used++;
    if (reg_used == NUM_REGISTERS) reg_used = 0;
  }
  return true;
}

bool cHardwareExperimental::Inst_SwitchStack(cAvidaContext&) { switchStack(); return true; }

bool cHardwareExperimental::Inst_SwapStackTop(cAvidaContext&)
{
  sInternalValue v0 = getStack(0).Pop();
  sInternalValue v1 = getStack(1).Pop();
  getStack(0).Push(v1);
  getStack(1).Push(v0);
  return true;
}

bool cHardwareExperimental::Inst_Swap(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue v1 = m_threads[m_cur_thread].reg[op1];
  m_threads[m_cur_thread].reg[op1] = m_threads[m_cur_thread].reg[op2];
  m_threads[m_cur_thread].reg[op2] = v1;
  return true;
}

bool cHardwareExperimental::Inst_ShiftR(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value >> 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareExperimental::Inst_ShiftL(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value << 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}


bool cHardwareExperimental::Inst_Inc(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value + 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareExperimental::Inst_Dec(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value - 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareExperimental::Inst_Zero(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, 0, false);
  return true;
}

bool cHardwareExperimental::Inst_Mult100(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value * 100, false);
  return true;
}

bool cHardwareExperimental::Inst_Add(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value + r2.value, r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_Sub(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value - r2.value, r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_Mult(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value * r2.value, r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_Div(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  if (r2.value != 0) {
    if (0 - INT_MAX > r1.value && r2.value == -1)
      m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: Float exception");
    else
      setInternalValue(dst, r1.value / r2.value, r1, r2);
  } else {
    m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "div: dividing by 0");
    return false;
  }
  return true;
}

bool cHardwareExperimental::Inst_Mod(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  if (r2.value != 0) {
    setInternalValue(dst, r1.value % r2.value, r1, r2);
  } else {
    m_organism->Fault(FAULT_LOC_MATH, FAULT_TYPE_ERROR, "mod: modding by 0");
    return false;
  }
  return true;
}


bool cHardwareExperimental::Inst_Nand(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, ~(r1.value & r2.value), r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_HeadAlloc(cAvidaContext& ctx)   // Allocate maximal more
{
  const int dst = FindModifiedRegister(rAX);
  const int cur_size = m_memory.GetSize();
  const int alloc_size = Min((int) (m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get() * cur_size),
                             MAX_GENOME_LENGTH - cur_size);
  if (Allocate_Main(ctx, alloc_size)) {
    setInternalValue(dst, cur_size);
    return true;
  } else return false;
}

bool cHardwareExperimental::Inst_TaskIO(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setInternalValue(reg_used, value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}

bool cHardwareExperimental::Inst_TaskIOExpire(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  if (m_io_expire && reg.env_component && reg.oldest_component < m_last_output) return false;
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;  
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setInternalValue(reg_used, value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}


bool cHardwareExperimental::Inst_TaskInput(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setInternalValue(reg_used, value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}

bool cHardwareExperimental::Inst_TaskOutput(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  return true;
}

bool cHardwareExperimental::Inst_TaskOutputZero(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  setInternalValue(reg_used, 0);
  
  return true;
}

bool cHardwareExperimental::Inst_TaskOutputExpire(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  if (m_io_expire && reg.env_component && reg.oldest_component < m_last_output) return false;
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  
  return true;
}

bool cHardwareExperimental::Inst_DemeIO(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];

  // Do deme output..
  m_organism->GetOrgInterface().DoDemeOutput(ctx, reg.value);
  //m_last_output = m_cycle_count; //**

  // Do deme input..
  int value_in = m_organism->GetOrgInterface().GetNextDemeInput(ctx);
  if (value_in != -1) {
    setInternalValue(reg_used, value_in, true);
    m_organism->GetOrgInterface().DoDemeInput(value_in);
  }
  return true;
}

bool cHardwareExperimental::Inst_MoveHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = FindModifiedHead(nHardware::HEAD_FLOW);
  getHead(head_used).Set(getHead(target));
  if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  return true;
}

bool cHardwareExperimental::Inst_MoveHeadIfNEqu(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = FindModifiedHead(nHardware::HEAD_FLOW);
  if (m_threads[m_cur_thread].reg[op1].value != m_threads[m_cur_thread].reg[op2].value) {
    getHead(head_used).Set(getHead(target));
    if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  }
  return true;
}

bool cHardwareExperimental::Inst_MoveHeadIfLess(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = FindModifiedHead(nHardware::HEAD_FLOW);
  if (m_threads[m_cur_thread].reg[op1].value < m_threads[m_cur_thread].reg[op2].value) {
    getHead(head_used).Set(getHead(target));
    if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  }
  return true;
}


bool cHardwareExperimental::Inst_Goto(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}

bool cHardwareExperimental::Inst_GotoIfNEqu(cAvidaContext&)
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

bool cHardwareExperimental::Inst_GotoIfLess(cAvidaContext&)
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


bool cHardwareExperimental::Inst_GotoConsensus(cAvidaContext&)
{
  if (BitCount(GetRegister(rBX)) < CONSENSUS) return true;
  
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}

bool cHardwareExperimental::Inst_GotoConsensus24(cAvidaContext&)
{
  if (BitCount(GetRegister(rBX) & MASK24) < CONSENSUS24) return true;
  
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}


bool cHardwareExperimental::Inst_JumpHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int reg = FindModifiedRegister(rCX);
  getHead(head_used).Jump(m_threads[m_cur_thread].reg[reg].value);
  if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  return true;
}

bool cHardwareExperimental::Inst_GetHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int reg = FindModifiedRegister(rCX);
  setInternalValue(reg, getHead(head_used).GetPosition());
  return true;
}

bool cHardwareExperimental::Inst_IfCopiedCompLabel(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfCopiedDirectLabel(cAvidaContext&)
{
  ReadLabel();
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfCopiedCompSeq(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadSequence())  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfCopiedDirectSeq(cAvidaContext&)
{
  ReadLabel();
  if (GetLabel() != GetReadSequence())  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_HeadDivide(cAvidaContext& ctx)
{
  m_organism->GetPhenotype().SetDivideSex(false);
  m_organism->GetPhenotype().SetCrossNum(0);
  
  AdjustHeads();
  const int divide_pos = getHead(nHardware::HEAD_READ).GetPosition();
  int child_end =  getHead(nHardware::HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
  bool ret_val = Divide_Main(ctx, divide_pos, extra_lines, 1.0);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val; 
}

bool cHardwareExperimental::Inst_HeadDivideSex(cAvidaContext& ctx)  
{ 
  m_organism->GetPhenotype().SetDivideSex(true);
  m_organism->GetPhenotype().SetCrossNum(1);
  
  AdjustHeads();
  const int divide_pos = getHead(nHardware::HEAD_READ).GetPosition();
  int child_end =  getHead(nHardware::HEAD_WRITE).GetPosition();
  if (child_end == 0) child_end = m_memory.GetSize();
  const int extra_lines = m_memory.GetSize() - child_end;
  bool ret_val = Divide_Main(ctx, divide_pos, extra_lines, 1.0);
  // Re-adjust heads.
  AdjustHeads();
  return ret_val; 
}

bool cHardwareExperimental::Inst_HeadRead(cAvidaContext& ctx)
{
  const int head_id = FindModifiedHead(nHardware::HEAD_READ);
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

bool cHardwareExperimental::Inst_HeadWrite(cAvidaContext& ctx)
{
  const int head_id = FindModifiedHead(nHardware::HEAD_WRITE);
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

bool cHardwareExperimental::Inst_HeadCopy(cAvidaContext& ctx)
{
  // For the moment, this cannot be nop-modified.
  cHeadCPU& read_head = getHead(nHardware::HEAD_READ);
  cHeadCPU& write_head = getHead(nHardware::HEAD_WRITE);
  
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

bool cHardwareExperimental::Inst_Search_Label_Comp_S(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Comp_F(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Comp_B(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Direct_S(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Direct_F(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Direct_B(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Comp_S(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Comp_F(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Comp_B(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Direct_S(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Direct_F(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Direct_B(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}


bool cHardwareExperimental::Inst_SetFlow(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rCX);
  getHead(nHardware::HEAD_FLOW).Set(GetRegister(reg_used));
  return true; 
}



bool cHardwareExperimental::Inst_WaitCondition_Equal(cAvidaContext&)
{
  const int wait_value = FindModifiedRegister(rBX);
  const int check_reg = FindModifiedRegister(rDX);
  const int wait_dst = FindModifiedRegister(wait_value);
  
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

bool cHardwareExperimental::Inst_WaitCondition_Less(cAvidaContext&)
{
  const int wait_value = FindModifiedRegister(rBX);
  const int check_reg = FindModifiedRegister(rDX);
  const int wait_dst = FindModifiedRegister(wait_value);
  
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

bool cHardwareExperimental::Inst_WaitCondition_Greater(cAvidaContext&)
{
  const int wait_value = FindModifiedRegister(rBX);
  const int check_reg = FindModifiedRegister(rDX);
  const int wait_dst = FindModifiedRegister(wait_value);
  
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


// --------  Promoter Model  --------

bool cHardwareExperimental::Inst_Promoter(cAvidaContext&)
{
  // Promoters don't do anything themselves
  return true;
}


// Move the instruction ptr to the next active promoter
bool cHardwareExperimental::Inst_Terminate(cAvidaContext& ctx)
{
  PromoterTerminate(ctx);
  return true;
}

// Move the instruction ptr to the next active promoter
bool cHardwareExperimental::Inst_TerminateConsensus(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(rDX);
  if (BitCount(GetRegister(op1)) <  CONSENSUS)  PromoterTerminate(ctx);
  return true;
}

// Move the instruction ptr to the next active promoter
bool cHardwareExperimental::Inst_TerminateConsensus24(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(rDX);
  if (BitCount(GetRegister(op1) & MASK24) <  CONSENSUS24)  PromoterTerminate(ctx);
  return true;
}

// Set a new regulation code (which is XOR'ed with ALL promoter codes).
bool cHardwareExperimental::Inst_Regulate(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  int regulation_code = GetRegister(reg_used);
  
  for (int i = 0; i < m_promoters.GetSize(); i++) m_promoters[i].regulation = regulation_code;
  
  return true;
}

// Set a new regulation code, but only on a subset of promoters.
bool cHardwareExperimental::Inst_RegulateSpecificPromoters(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  const int reg_promoter = FindModifiedNextRegister(reg_used);
  
  int regulation_code = GetRegister(reg_used);
  int regulation_promoter = GetRegister(reg_promoter);
  
  for (int i = 0; i < m_promoters.GetSize(); i++) {
    
    // @DMB - should this always be using the low order bits?
    
    // Look for consensus bit matches over the length of the promoter code
    int test_p_code = m_promoters[i].bit_code;    
    int test_r_code = regulation_promoter;
    int bit_count = 0;
    for (int j = 0; j < m_world->GetConfig().PROMOTER_EXE_LENGTH.Get(); j++) {      
      if ((test_p_code & 1) == (test_r_code & 1)) bit_count++;
      test_p_code >>= 1;
      test_r_code >>= 1;
    }
    
    if (bit_count >= m_world->GetConfig().PROMOTER_EXE_LENGTH.Get() / 2) m_promoters[i].regulation = regulation_code;
  }
  
  return true;
}

bool cHardwareExperimental::Inst_SenseRegulate(cAvidaContext& ctx)
{
  unsigned int bits = 0;
  const Apto::Array<double> & res_count = m_organism->GetOrgInterface().GetResources(ctx);
  assert (res_count.GetSize() != 0);
  for (int i=0; i<m_world->GetConfig().PROMOTER_CODE_SIZE.Get(); i++)
  {
    int b = i % res_count.GetSize();
    bits <<= 1;
    bits += (res_count[b] != 0);
  }  
  
  for (int i=0; i< m_promoters.GetSize();i++)
  {
    m_promoters[i].regulation = bits;
  }
  return true;
}

// Create a number from inst bit codes
bool cHardwareExperimental::Do_Numberate(cAvidaContext&, int num_bits)
{
  const int reg_used = FindModifiedRegister(rBX);
  
  // advance the IP now, so that it rests on the beginning of our number
  getIP().Advance();
  m_advance_ip = false;
  
  int num = Numberate(getIP().GetPosition(), +1, num_bits);
  setInternalValue(reg_used, num);
  return true;
}

void cHardwareExperimental::PromoterTerminate(cAvidaContext& ctx)
{
  // Optionally,
  // Reset the thread.
  if (m_world->GetConfig().TERMINATION_RESETS.Get())
  {
    m_threads[m_cur_thread].Reset(this, m_threads[m_cur_thread].GetID());
    
    //Setting this makes it harder to do things. You have to be modular.
    m_organism->GetOrgInterface().ResetInputs(ctx);   // Re-randomize the inputs this organism sees
    m_organism->ClearInput();                         // Also clear their input buffers, or they can still claim
                                                      // rewards for numbers no longer in their environment!
  }
  
  // Reset our count
  m_threads[m_cur_thread].ResetPromoterInstExecuted();
  m_advance_ip = false;
  const int promoter_reg_used = rDX; // register to put chosen promoter code in, default to DX
  
  
  // @DMB - should the promoter index and offset be stored in cLocalThread to allow multiple threads?
  
  // Search for an active promoter  
  int start_offset = m_promoter_offset;
  int start_index  = m_promoter_index;
  
  
  bool no_promoter_found = true;
  if (m_promoters.GetSize() > 0) {
    const int promoter_exe_length = m_world->GetConfig().PROMOTER_EXE_LENGTH.Get();
    const int promoter_code_size = m_world->GetConfig().PROMOTER_CODE_SIZE.Get();
    const int promoter_exe_threshold = m_world->GetConfig().PROMOTER_EXE_THRESHOLD.Get();
    
    while (true) {
      // If the next promoter is active, then break out
      
      // Move promoter index, rolling over if necessary
      m_promoter_index++;
      
      if (m_promoter_index == m_promoters.GetSize()) {
        m_promoter_index = 0;
        
        // Move offset, rolling over when there are not enough bits before we would have to wrap around left
        m_promoter_offset += promoter_exe_length;
        if (m_promoter_offset + promoter_exe_length > promoter_code_size) m_promoter_offset = 0;
      }
      
      
      int count = 0;
      unsigned int code = m_promoters[m_promoter_index].GetRegulatedBitCode();
      for (int i = 0; i < promoter_exe_length; i++) {
        // @DMB - changed the following to avoid integer division. We should only ever need to circle around once
        //int offset = (m_promoter_offset + i) % promoter_code_size;
        int offset = m_promoter_offset + i;
        if (offset >= promoter_code_size) offset -= promoter_code_size;
        int state = code >> offset;
        count += (state & 1);
      }
      
      if (count >= promoter_exe_threshold) {
        no_promoter_found = false;
        break;
      }
      
      // If we just checked the promoter that we were originally on, then there
      // are no active promoters.
      if ((start_offset == m_promoter_offset) && (start_index == m_promoter_index)) break;
      
      // If we originally were not on a promoter, then stop once we check the
      // first promoter and an offset of zero
      if (start_index == -1) start_index = 0;
    } 
  }
  
  if (no_promoter_found) {
    switch (m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get()) {
      case 0:
      case 2:
        // Set defaults for when no active promoter is found
        m_promoter_index = -1;
        getIP().Set(0);
        setInternalValue(promoter_reg_used, 0);
        break;
        
      case 1: // Death to organisms that refuse to use promoters!
        m_organism->Die(ctx);
        break;
        
      default:
        cout << "Unrecognized NO_ACTIVE_PROMOTER_EFFECT setting: " << m_world->GetConfig().NO_ACTIVE_PROMOTER_EFFECT.Get() << endl;
        exit(1);
        break;
    }
  } else {
    // We found an active match, offset to just after it.
    // cHeadCPU will do the mod genome size for us
    getIP().Set(m_promoters[m_promoter_index].pos + 1);
    
    // Put its bit code in BX for the organism to have if option is set
    if (m_world->GetConfig().PROMOTER_TO_REGISTER.Get())
      setInternalValue(promoter_reg_used, m_promoters[m_promoter_index].bit_code);
  }  
}

// Construct a promoter bit code from instruction bit codes
int cHardwareExperimental::Numberate(int _pos, int _dir, int _num_bits)
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
  while (code_size < _num_bits)
  {
    unsigned int inst_code = (unsigned int) GetInstSet().GetInstructionCode(m_memory[j]);
    // shift bits in, one by one ... excuse the counter variable pun
    for (int code_on = 0; (code_size < _num_bits) && (code_on < m_world->GetConfig().INST_CODE_LENGTH.Get()); code_on++)
    {
      if (_dir < 0)
      {
        code >>= 1; // shift first so we don't go one too far at the end
        code += (1 << (_num_bits - 1)) * (inst_code & 1);
        inst_code >>= 1; 
      }
      else
      {
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

/*! 
 Sets BX to 1 if >=50% of the bits in the specified register places
 are 1's and zero otherwise.
 */

bool cHardwareExperimental::Inst_BitConsensus(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  const int op1 = FindModifiedNextRegister(reg_used);
  sInternalValue& val = m_threads[m_cur_thread].reg[op1];
  
  setInternalValue(reg_used, (BitCount(val.value) >= CONSENSUS) ? 1 : 0, val); 
  return true; 
}

// Looks at only the lower 24 bits
bool cHardwareExperimental::Inst_BitConsensus24(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  const int op1 = FindModifiedNextRegister(reg_used);
  sInternalValue& val = m_threads[m_cur_thread].reg[op1];
  
  setInternalValue(reg_used, (BitCount(val.value & MASK24) >= CONSENSUS24) ? 1 : 0, val); 
  return true; 
}

// Create a number from inst bit codes of the previously executed instructions
bool cHardwareExperimental::Inst_Execurate(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].GetExecurate());
  return true;
}

// Create a number from inst bit codes of the previously executed instructions, truncated to 24 bits
bool cHardwareExperimental::Inst_Execurate24(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, (MASK24 & m_threads[m_cur_thread].GetExecurate()));
  return true;
}


bool cHardwareExperimental::Inst_Repro(cAvidaContext& ctx)
{
  // these checks should be done, but currently they make some assumptions
  // that crash when evaluating this kind of organism -- JEB
  
  if (m_organism->GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
  // Since the divide will now succeed, set up the information to be sent
  // to the new organism
  InstructionSequencePtr offspring_seq(new InstructionSequence(m_memory));
  PropertyMap props;
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
  if (m_organism->GetCopyMutProb() > 0) { // Skip this if no mutations....
    for (int i = 0; i < child_seq.GetSize(); i++) {
//    for (int i = 0; i < m_memory.GetSize(); i++) {
      if (m_organism->TestCopyMut(ctx)) child_seq[i] = m_inst_set->GetRandomInst(ctx);
    }
  }
  
  // Handle Divide Mutations...
  Divide_DoMutations(ctx);
  
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

bool cHardwareExperimental::Inst_Die(cAvidaContext& ctx)
{
  m_organism->Die(ctx);
  
  return true;
}


bool cHardwareExperimental::Inst_SGMove(cAvidaContext&)
{
  assert(m_ext_mem.GetSize() > 3);
  
  const cStateGrid& sg = m_organism->GetStateGrid();
  
  int& x = m_ext_mem[0];
  int& y = m_ext_mem[1];
  
  const int facing = m_ext_mem[2];
  
  // State grid is treated as a 2-dimensional toroidal grid with size [0, width) and [0, height)
  switch (facing) {
    case 0: // N
      if (++y == sg.GetHeight()) y = 0;
      break;
      
    case 1: // NE
      if (++x == sg.GetWidth()) x = 0;
      if (++y == sg.GetHeight()) y = 0;
      break;
      
    case 2: // E
      if (++x == sg.GetWidth()) x = 0;
      break;
      
    case 3: // SE
      if (++x == sg.GetWidth()) x = 0;
      if (--y == -1) y = sg.GetHeight() - 1;
      break;
      
    case 4: // S
      if (--y == -1) y = sg.GetHeight() - 1;
      break;
      
    case 5: // SW
      if (--x == -1) x = sg.GetWidth() - 1;
      if (--y == -1) y = sg.GetHeight() - 1;
      break;
      
    case 6: // W
      if (--x == -1) x = sg.GetWidth() - 1;
      break;
      
    case 7: // NW
      if (--x == -1) x = sg.GetWidth() - 1;
      if (++y == sg.GetHeight()) y = 0;
      break;
      
    default:
      assert(facing >= 0 && facing <= 7);
  }
  
  // Increment state observed count
  m_ext_mem[3 + sg.GetStateAt(x, y)]++;
  
  // Save this location in the movement history
  m_ext_mem.Push(sg.GetIDFor(x, y));
  return true;
}

bool cHardwareExperimental::Inst_SGRotateL(cAvidaContext&)
{
  assert(m_ext_mem.GetSize() > 3);
  if (--m_ext_mem[2] < 0) m_ext_mem[2] = 7;
  return true;
}

bool cHardwareExperimental::Inst_SGRotateR(cAvidaContext&)
{
  assert(m_ext_mem.GetSize() > 3);
  if (++m_ext_mem[2] > 7) m_ext_mem[2] = 0;
  return true;
}

bool cHardwareExperimental::Inst_SGSense(cAvidaContext&)
{
  const cStateGrid& sg = m_organism->GetStateGrid();
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, sg.SenseStateAt(m_ext_mem[0], m_ext_mem[1]));
  return true; 
}

bool cHardwareExperimental::Inst_Move(cAvidaContext& ctx)
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

bool cHardwareExperimental::Inst_JuvMove(cAvidaContext& ctx)
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

bool cHardwareExperimental::Inst_RangeMove(cAvidaContext& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  // In TestCPU, movement fails...
  if (m_organism->GetOrgInterface().GetCellID() == -1) return false;
  assert(m_organism != 0);
  
  bool safe_passage = true;
  bool move_success = false;
  int faced_range = m_organism->GetOrgInterface().GetFacedCellDataTerritory();
  if (m_use_avatar == 2) faced_range = m_organism->GetOrgInterface().GetAVFacedDataTerritory();
  int marked_update = m_organism->GetOrgInterface().GetFacedCellDataUpdate();
  if (m_use_avatar == 2) marked_update = m_organism->GetOrgInterface().GetAVFacedDataUpdate();
  if (faced_range != -1 && (faced_range != m_organism->GetOpinion().first) && 
      ((m_world->GetStats().GetUpdate() - marked_update) <= m_world->GetConfig().MARKING_EXPIRE_DATE.Get())) {
        safe_passage = false;
      }
  
  if (safe_passage) {
    if (!m_use_avatar) move_success = m_organism->Move(ctx);
    else if (m_use_avatar) move_success = m_organism->MoveAV(ctx);
  }
  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, move_success, true);  
  setInternalValue(FindModifiedNextRegister(rBX), safe_passage, true);
  return true;
}

bool cHardwareExperimental::Inst_RangePredMove(cAvidaContext& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  // In TestCPU, movement fails...
  if (m_organism->GetOrgInterface().GetCellID() == -1) return false;
  assert(m_organism != 0);
  
  bool safe_passage = true;
  bool move_success = false;
  int faced_range = m_organism->GetOrgInterface().GetFacedCellDataTerritory();
  if (m_use_avatar == 2) faced_range = m_organism->GetOrgInterface().GetAVFacedDataTerritory();
  int marked_update = m_organism->GetOrgInterface().GetFacedCellDataUpdate();
  if (m_use_avatar == 2) marked_update = m_organism->GetOrgInterface().GetAVFacedDataUpdate();
  if (m_organism->GetForageTarget() == -2 && faced_range != -1 && (faced_range != m_organism->GetOpinion().first) && 
      ((m_world->GetStats().GetUpdate() - marked_update) <= m_world->GetConfig().MARKING_EXPIRE_DATE.Get())) {
    safe_passage = false;
  }
  
  if (safe_passage) {
    if (!m_use_avatar) move_success = m_organism->Move(ctx);
    else if (m_use_avatar) move_success = m_organism->MoveAV(ctx);
  }
  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, move_success, true);  
  setInternalValue(FindModifiedNextRegister(rBX), safe_passage, true);
  return true;
}

bool cHardwareExperimental::Inst_GetCellPosition(cAvidaContext& ctx)
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

bool cHardwareExperimental::Inst_GetCellPositionX(cAvidaContext& ctx)
{
  int x = m_organism->GetOrgInterface().GetCellXPosition();
  // Fail if we're running in the test CPU
  if (x == -1) return false;

  const int xreg = FindModifiedRegister(rBX);
  setInternalValue(xreg, x, true);

  return true;
}

bool cHardwareExperimental::Inst_GetCellPositionY(cAvidaContext& ctx)
{
  int y = m_organism->GetOrgInterface().GetCellYPosition();
  // Fail if we're running in the test CPU
  if (y == -1) return false;

  const int yreg = FindModifiedRegister(rBX);
  setInternalValue(yreg, y, true);

  return true;
}

bool cHardwareExperimental::Inst_GetNorthOffset(cAvidaContext& ctx) {
  const int out_reg = FindModifiedRegister(rBX);
  int compass_dir = m_organism->GetOrgInterface().GetFacedDir();
  if (m_use_avatar) compass_dir = m_organism->GetOrgInterface().GetAVFacing();
  setInternalValue(out_reg, compass_dir, true);
  return true;
}

bool cHardwareExperimental::Inst_GetPositionOffset(cAvidaContext&) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetNortherly(), true);
  setInternalValue(FindModifiedNextRegister(out_reg), m_organism->GetEasterly(), true);
  return true;
}

bool cHardwareExperimental::Inst_GetNortherly(cAvidaContext&) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetNortherly(), true);
  return true;  
}

bool cHardwareExperimental::Inst_GetEasterly(cAvidaContext&) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetEasterly(), true);
  return true;  
}

bool cHardwareExperimental::Inst_ZeroEasterly(cAvidaContext&) {
  m_organism->ClearEasterly();
  return true;
}

bool cHardwareExperimental::Inst_ZeroNortherly(cAvidaContext&) {
  m_organism->ClearNortherly();
  return true;
}

bool cHardwareExperimental::Inst_ZeroPosOffset(cAvidaContext&) {
  const int offset = GetRegister(FindModifiedRegister(rBX)) % 3;
  if (offset == 0) {
    m_organism->ClearEasterly();
    m_organism->ClearNortherly();    
  }
  else if (offset == 1) m_organism->ClearEasterly();
  else if (offset == 2) m_organism->ClearNortherly();
  return true;
}

bool cHardwareExperimental::Inst_RotateLeftOne(cAvidaContext&)
{
  m_organism->Rotate(1);
  return true;
}

bool cHardwareExperimental::Inst_RotateRightOne(cAvidaContext&)
{
  m_organism->Rotate(-1);
  return true;
}

bool cHardwareExperimental::Inst_RotateUphill(cAvidaContext& ctx)
{
  Apto::Array<double> current_res;
  if (!m_use_avatar) current_res = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) current_res = m_organism->GetOrgInterface().GetAVResources(ctx); 
  
  int actualNeighborhoodSize = m_organism->GetNeighborhoodSize();  
  if (m_use_avatar) actualNeighborhoodSize = m_organism->GetOrgInterface().GetAVNumNeighbors();
  
  int group = 0;  
  if(m_organism->HasOpinion()) group = m_organism->GetOpinion().first; 
  
  double max_res = 0;
  for(int i = 0; i < actualNeighborhoodSize; i++) {
    m_organism->Rotate(1);
    Apto::Array<double> faced_res;
    if (!m_use_avatar) faced_res = m_organism->GetOrgInterface().GetFacedCellResources(ctx); 
    else if (m_use_avatar) faced_res = m_organism->GetOrgInterface().GetAVFacedResources(ctx);
    if (faced_res[group] > max_res) max_res = faced_res[group];
  } 
  
  if (max_res > current_res[group]) {
    for(int i = 0; i < actualNeighborhoodSize; i++) {
      Apto::Array<double> faced_res;
      if (!m_use_avatar) faced_res = m_organism->GetOrgInterface().GetFacedCellResources(ctx); 
      else if (m_use_avatar) faced_res = m_organism->GetOrgInterface().GetAVFacedResources(ctx);
      if (faced_res[group] != max_res) m_organism->Rotate(1);
    }
  }
  int res_diff = 0;
  if (current_res[group] == 0) res_diff = (int) max_res;
  else res_diff = (int) (((max_res - current_res[group])/current_res[group]) * 100 + 0.5);
  int reg_to_set = FindModifiedRegister(rBX);
  setInternalValue(reg_to_set, res_diff, true);
  return true;
}

bool cHardwareExperimental::Inst_RotateUpFtHill(cAvidaContext& ctx)
{
  Apto::Array<double> current_res;
  if (!m_use_avatar) current_res = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) current_res = m_organism->GetOrgInterface().GetAVResources(ctx); 
  
  int actualNeighborhoodSize = m_organism->GetNeighborhoodSize();  
  if (m_use_avatar) actualNeighborhoodSize = m_organism->GetOrgInterface().GetAVNumNeighbors();
  
  int ft = m_organism->GetForageTarget(); 
  
  double max_res = 0;
  for(int i = 0; i < actualNeighborhoodSize; i++) {
    m_organism->Rotate(1);
    Apto::Array<double> faced_res;
    if (!m_use_avatar) faced_res = m_organism->GetOrgInterface().GetFacedCellResources(ctx); 
    else if (m_use_avatar) faced_res = m_organism->GetOrgInterface().GetAVFacedResources(ctx);
    if (faced_res[ft] > max_res) max_res = faced_res[ft];
  } 
  
  if (max_res > current_res[ft]) {
    for(int i = 0; i < actualNeighborhoodSize; i++) {
      Apto::Array<double> faced_res;
      if (!m_use_avatar) faced_res = m_organism->GetOrgInterface().GetFacedCellResources(ctx); 
      else if (m_use_avatar) faced_res = m_organism->GetOrgInterface().GetAVFacedResources(ctx);
      if (faced_res[ft] != max_res) m_organism->Rotate(1);
    }
  }
  int res_diff = 0;
  if (current_res[ft] == 0) res_diff = (int) max_res;
  else res_diff = (int) (((max_res - current_res[ft])/current_res[ft]) * 100 + 0.5);
  int reg_to_set = FindModifiedRegister(rBX);
  setInternalValue(reg_to_set, res_diff, true);
  return true;
}

bool cHardwareExperimental::Inst_RotateHome(cAvidaContext&)
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
  if (m_use_avatar == 2) rotates = m_organism->GetOrgInterface().GetAVNumNeighbors();
  for (int i = 0; i < rotates; i++) {
    m_organism->Rotate(1);
    if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedDir() == correct_facing) break;
    else if (m_use_avatar && m_organism->GetOrgInterface().GetAVFacing() == correct_facing) break;
  }
  return true;
}

bool cHardwareExperimental::Inst_RotateUnoccupiedCell(cAvidaContext&)
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
    m_organism->Rotate(1); // continue to rotate
  }  
  setInternalValue(reg_used, 0, true);
  return true;
}

bool cHardwareExperimental::Inst_RotateX(cAvidaContext&)
{
  int num_neighbors = m_organism->GetNeighborhoodSize();
  if (m_use_avatar) num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
  int rot_dir = 1;
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  const int reg_used = FindModifiedRegister(rBX);
  int rot_num = m_threads[m_cur_thread].reg[reg_used].value;
  // rotate the nop number of times in the appropriate direction
  rot_num < 0 ? rot_dir = -1 : rot_dir = 1;
  rot_num = abs(rot_num);
  if (rot_num > 7) rot_num = rot_num % 8;
  for (int i = 0; i < rot_num; i++) m_organism->Rotate(rot_dir);
  setInternalValue(reg_used, rot_num * rot_dir, true);
  return true;
}

bool cHardwareExperimental::Inst_RotateDir(cAvidaContext& ctx)
{
  int num_neighbors = m_organism->GetNeighborhoodSize();
  if (m_use_avatar) num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  const int reg_used = FindModifiedRegister(rBX);
  int rot_dir = abs(m_threads[m_cur_thread].reg[reg_used].value) % 8;
  // rotate to the appropriate direction
  for (int i = 0; i < num_neighbors + 1; i++) {
    m_organism->Rotate(-1);
    if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedDir() == rot_dir) break;
    else if (m_use_avatar && m_organism->GetOrgInterface().GetAVFacing() == rot_dir) break;
  }
  int current_dir = m_organism->GetOrgInterface().GetFacedDir();
  if (m_use_avatar) current_dir = m_organism->GetOrgInterface().GetAVFacing();
  setInternalValue(reg_used, current_dir, true);
  return true;
}

// Will rotate organism to face a specified other org
bool cHardwareExperimental::Inst_RotateOrgID(cAvidaContext&)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  // Will rotate organism to face a specificied other org
  const int id_sought_reg = FindModifiedRegister(rBX);
  const int id_sought = m_threads[m_cur_thread].reg[id_sought_reg].value;
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
    int rotates = m_organism->GetNeighborhoodSize();
    if (m_use_avatar == 2) rotates = m_organism->GetOrgInterface().GetAVNumNeighbors();
    for (int i = 0; i < rotates; i++) {
      m_organism->Rotate(-1);
      if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedDir() == correct_facing) { 
        found_org = true; 
        break; 
      }
      else if (m_use_avatar && m_organism->GetOrgInterface().GetAVFacing() == correct_facing)  { 
        found_org = true; 
        break; 
      }
    }
    // return some data as in look sensor
    if (found_org) {
      int dist_reg = FindModifiedNextRegister(id_sought_reg);
      int dir_reg = FindModifiedNextRegister(dist_reg);
      int fat_reg = FindModifiedNextRegister(dir_reg);
      int ft_reg = FindModifiedNextRegister(fat_reg); 
      int group_reg = FindModifiedNextRegister(ft_reg);
      
      setInternalValue(dist_reg, -2, true);
      setInternalValue(dir_reg, m_sensor.ReturnRelativeFacing(target_org), true);
      setInternalValue(fat_reg, (int) target_org->GetPhenotype().GetCurBonus(), true);
      setInternalValue(ft_reg, target_org->GetForageTarget(), true);  
      if (target_org->HasOpinion()) {
        setInternalValue(group_reg, target_org->GetOpinion().first, true);
      }
      if ((target_org->IsDisplaying() || m_world->GetConfig().USE_DISPLAY.Get()) && target_org->GetOrgDisplayData() != NULL) m_sensor.SetLastSeenDisplay(target_org->GetOrgDisplayData());    
    }        
    return true;
  }
}

// Will rotate organism to face away from a specificied other org
bool cHardwareExperimental::Inst_RotateAwayOrgID(cAvidaContext&)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  // Will rotate organism to face a specificied other org
  const int id_sought_reg = FindModifiedRegister(rBX);
  const int id_sought = m_threads[m_cur_thread].reg[id_sought_reg].value;
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
    int rotates = m_organism->GetNeighborhoodSize();
    if (m_use_avatar == 2) rotates = m_organism->GetOrgInterface().GetAVNumNeighbors();
    for (int i = 0; i < rotates; i++) {
      m_organism->Rotate(-1);
      if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedDir() == correct_facing) { 
        found_org = true;
        break;
      }
      else if (m_use_avatar && m_organism->GetOrgInterface().GetAVFacing() == correct_facing) {
        found_org = true;
        break;
      }
    }
    // return some data as in look sensor
    if (found_org) {
      int dist_reg = FindModifiedNextRegister(id_sought_reg);
      int dir_reg = FindModifiedNextRegister(dist_reg);
      int fat_reg = FindModifiedNextRegister(dir_reg);
      int ft_reg = FindModifiedNextRegister(fat_reg); 
      int group_reg = FindModifiedNextRegister(ft_reg);
      
      setInternalValue(dist_reg, -2, true);
      setInternalValue(dir_reg, m_sensor.ReturnRelativeFacing(target_org), true);
      setInternalValue(fat_reg, (int) target_org->GetPhenotype().GetCurBonus(), true);
      setInternalValue(ft_reg, target_org->GetForageTarget(), true);  
      if (target_org->HasOpinion()) {
        setInternalValue(group_reg, target_org->GetOpinion().first, true);
      }
      if ((target_org->IsDisplaying() || m_world->GetConfig().USE_DISPLAY.Get()) && target_org->GetOrgDisplayData() != NULL) m_sensor.SetLastSeenDisplay(target_org->GetOrgDisplayData());     
    }       
    return true;
  }
}

// -------- Neural networking -------- @JJB
// All only linked to input avatars for now

// Rotate the register-value-selected avatar, left by one
bool cHardwareExperimental::Inst_RotateNeuronAVLeft(cAvidaContext& ctx)
{
  const int avatar_reg = FindModifiedRegister(rBX);
  int avatar_num = m_threads[m_cur_thread].reg[avatar_reg].value;

  avatar_num = m_organism->GetOrgInterface().FindAV(true, false, avatar_num);

  return m_organism->GetOrgInterface().RotateAV(-1, avatar_num);
}

// Rotate the register-value-selected avatar, right by one
bool cHardwareExperimental::Inst_RotateNeuronAVRight(cAvidaContext& ctx)
{
  const int avatar_reg = FindModifiedRegister(rBX);
  int avatar_num = m_threads[m_cur_thread].reg[avatar_reg].value;

  avatar_num = m_organism->GetOrgInterface().FindAV(true, false, avatar_num);

  return m_organism->GetOrgInterface().RotateAV(1, avatar_num);
}

// Rotate the register-value-selected avatar, by the register set amount
bool cHardwareExperimental::Inst_RotateNeuronAVbyX(cAvidaContext& ctx)
{
  const int avatar_reg = FindModifiedRegister(rBX);
  const int rotate_reg = FindModifiedNextRegister(avatar_reg);

  int avatar_num = m_threads[m_cur_thread].reg[avatar_reg].value;
  const int rotate = m_threads[m_cur_thread].reg[rotate_reg].value;

  avatar_num = m_organism->GetOrgInterface().FindAV(true, false, avatar_num);

  return m_organism->GetOrgInterface().RotateAV(rotate, avatar_num);
}

// Move the register-value-selected avatar forward into its faced cell
bool cHardwareExperimental::Inst_MoveNeuronAV(cAvidaContext& ctx)
{
  const int avatar_reg = FindModifiedRegister(rBX);
  int avatar_num = m_threads[m_cur_thread].reg[avatar_reg].value;

  avatar_num = m_organism->GetOrgInterface().FindAV(true, false, avatar_num);

  return m_organism->GetOrgInterface().MoveAV(ctx, avatar_num);
}

// If the register-value-selected input avatar occupies a cell that also has an output avatar, execute next
bool cHardwareExperimental::Inst_IfNeuronInputHasOutputAV(cAvidaContext& ctx)
{
  const int avatar_reg = FindModifiedRegister(rBX);
  int avatar_num = m_threads[m_cur_thread].reg[avatar_reg].value;

  avatar_num = m_organism->GetOrgInterface().FindAV(true, false, avatar_num);

  if (!m_organism->GetOrgInterface().HasOutputAV(avatar_num)) {
    getIP().Advance();
  }
  return true;
}

// If the register-value-selected input avatar does not occupy a cell that has an output avatar, execute next
bool cHardwareExperimental::Inst_IfNotNeuronInputHasOutputAV(cAvidaContext& ctx)
{
  const int avatar_reg = FindModifiedRegister(rBX);
  int avatar_num = m_threads[m_cur_thread].reg[avatar_reg].value;

  avatar_num = m_organism->GetOrgInterface().FindAV(true, false, avatar_num);

  if (m_organism->GetOrgInterface().HasOutputAV(avatar_num)) {
    getIP().Advance();
  }
  return true;
}

// If the register-value-selected input avatar is facing a cell with an output avatar, execute next
bool cHardwareExperimental::Inst_IfNeuronInputFacedHasOutputAV(cAvidaContext& ctx)
{
  const int avatar_reg = FindModifiedRegister(rBX);
  int avatar_num = m_threads[m_cur_thread].reg[avatar_reg].value;

  avatar_num = m_organism->GetOrgInterface().FindAV(true, false, avatar_num);

  if (!m_organism->GetOrgInterface().FacedHasOutputAV(avatar_num)) {
    getIP().Advance();
  }
  return true;
}

// If the register-value-selected input avatar is facing a cell without an output avatar, execute next
bool cHardwareExperimental::Inst_IfNotNeuronInputFacedHasOutputAV(cAvidaContext& ctx)
{
  const int avatar_reg = FindModifiedRegister(rBX);
  int avatar_num = m_threads[m_cur_thread].reg[avatar_reg].value;

  avatar_num = m_organism->GetOrgInterface().FindAV(true, false, avatar_num);

  if (m_organism->GetOrgInterface().FacedHasOutputAV(avatar_num)) {
    getIP().Advance();
  }
  return true;
}


bool cHardwareExperimental::Inst_SenseResourceID(cAvidaContext& ctx)
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
      setInternalValue(reg_to_set, i, true);
    }
  }    
  return true;
}

bool cHardwareExperimental::Inst_SenseResQuant(cAvidaContext& ctx)
{
  Apto::Array<double> cell_res;
  if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx); 
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  
  const int req_reg = FindModifiedRegister(rBX);
  int res_sought = -1;
  // are you trying to sense a valid, non-hidden nest resource?
  const int res_req = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  if (res_req < cell_res.GetSize() && res_req > 0 && resource_lib.GetResource(res_req)->GetHabitat() != 3) {  
    res_sought = res_req; 
  }
  
  int res_amount = 0;
  int faced_res = 0;
  // if you requested a valid resource, we return values for that res
  if (res_sought != -1) {
    if (!m_use_avatar) faced_res = (int) (m_organism->GetOrgInterface().GetFacedCellResources(ctx)[res_sought]);
    else if (m_use_avatar)  faced_res = (int) (m_organism->GetOrgInterface().GetAVFacedResources(ctx)[res_sought]); 
  }
  // otherwise, we sum across all the food resources in the cell
  else {
    for (int i = 0; i < cell_res.GetSize(); i++) {
      if (resource_lib.GetResource(i)->GetHabitat() == 0 || resource_lib.GetResource(i)->GetHabitat() > 5) {
        if (!m_use_avatar) faced_res += (int) (m_organism->GetOrgInterface().GetFacedCellResources(ctx)[i]);
        else if (m_use_avatar)  faced_res += (int) (m_organism->GetOrgInterface().GetAVFacedResources(ctx)[i]); 
      }
    }
  }
  
  // return % change
  int res_diff = 0;
  if (res_amount == 0) res_diff = (int) faced_res;
  else res_diff = (int) (((faced_res - res_amount) / res_amount) * 100 + 0.5);
  
  setInternalValue(req_reg, res_sought, true);
  const int res_tot_reg = FindModifiedNextRegister(req_reg);
  setInternalValue(res_tot_reg, res_amount, true);
  setInternalValue(FindModifiedNextRegister(res_tot_reg), res_diff, true);
  return true;
}

bool cHardwareExperimental::Inst_SenseNest(cAvidaContext& ctx)
{
  Apto::Array<double> cell_res;
  if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx); 
  
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  const int reg_used = FindModifiedRegister(rBX);
  
  int nest_id = m_threads[m_cur_thread].reg[reg_used].value;
  int nest_val = 0;
  
  // if invalid nop value, return the id of the first nest in the cell with val >= 1
  if (nest_id < 0 || nest_id >= resource_lib.GetSize() || resource_lib.GetResource(nest_id)->GetHabitat() != 3) {
    for (int i = 0; i < cell_res.GetSize(); i++) {
      if (resource_lib.GetResource(i)->GetHabitat() == 3 && cell_res[i] >= 1) {
        nest_id = i;
        nest_val = (int) cell_res[i];
        break;
      }
    }
  }
  else nest_val = (int) cell_res[nest_id];  
  
  setInternalValue(reg_used, nest_id, true);
  const int val_reg = FindModifiedNextRegister(reg_used);
  setInternalValue(val_reg, nest_val, true);
  return true;
}

bool cHardwareExperimental::Inst_SenseResDiff(cAvidaContext& ctx) 
{
  Apto::Array<double> cell_res;
  if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx); 
  const int req_reg = FindModifiedRegister(rBX);
  int res_sought = -1;
  // are you trying to sense a valid resource?
  const int res_req = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  if (res_req < cell_res.GetSize() && res_req > 0) {  
    res_sought = res_req; 
  }
  
  int res_amount = 0;
  int faced_res = 0;
  if (res_sought != -1) {
    res_amount = (int) (cell_res[res_sought]);
    if (!m_use_avatar) faced_res = (int) (m_organism->GetOrgInterface().GetFacedCellResources(ctx)[res_sought]);
    else if (m_use_avatar)  faced_res = (int) (m_organism->GetOrgInterface().GetAVFacedResources(ctx)[res_sought]); 
  }
  else {
    for (int i = 0; i < cell_res.GetSize(); i++ ) {
      res_amount += (int) (cell_res[i]);
      if (!m_use_avatar) faced_res += (int) (m_organism->GetOrgInterface().GetFacedCellResources(ctx)[i]);
      else if (m_use_avatar)  faced_res += (int) (m_organism->GetOrgInterface().GetAVFacedResources(ctx)[i]); 
    }
  }
  
  setInternalValue(FindModifiedNextRegister(req_reg), res_sought, true);
  const int res_tot_reg = FindModifiedNextRegister(FindModifiedNextRegister(req_reg));
  setInternalValue(res_tot_reg, res_amount - faced_res, true);
  return true;
}

bool cHardwareExperimental::Inst_LookAhead(cAvidaContext& ctx)
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
bool cHardwareExperimental::Inst_LookAheadIntercept(cAvidaContext& ctx)
{
  m_sensor.SetReturnRelativeFacing(true);
  return Inst_LookAhead(ctx);
}

bool cHardwareExperimental::Inst_LookAround(cAvidaContext& ctx)
{
  // dir register is 5th mod (will be count reg)
  int hab_reg = FindModifiedRegister(rBX);
  int dist_reg = FindModifiedNextRegister(hab_reg);
  int st_reg = FindModifiedNextRegister(dist_reg);
  int id_reg = FindModifiedNextRegister(st_reg);
  int dir_reg = FindModifiedNextRegister(id_reg);
  
  int search_dir = abs(m_threads[m_cur_thread].reg[dir_reg].value) % 3;
  
  if (m_world->GetConfig().LOOK_DISABLE.Get() == 5) {
    int org_type = m_world->GetConfig().LOOK_DISABLE_TYPE.Get();
    bool is_target_type = false;
    if (org_type == 0 && m_organism->GetForageTarget() == -2) is_target_type = true;
    else if (org_type == 1 && m_organism->GetForageTarget() != -2) is_target_type = true;
    else if (org_type == 2) is_target_type = true;
    
    if (is_target_type) {
      int rand = m_world->GetRandom().GetInt(INT_MAX);
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

bool cHardwareExperimental::Inst_LookAroundIntercept(cAvidaContext& ctx)
{
  m_sensor.SetReturnRelativeFacing(true);
  return Inst_LookAround(ctx);
}

bool cHardwareExperimental::Inst_LookFT(cAvidaContext& ctx)
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

bool cHardwareExperimental::Inst_LookAroundFT(cAvidaContext& ctx)
{
  // dir register is 5th mod (will be count reg)
  int hab_reg = FindModifiedRegister(rBX);
  int dist_reg = FindModifiedNextRegister(hab_reg);
  int st_reg = FindModifiedNextRegister(dist_reg);
  int id_reg = FindModifiedNextRegister(st_reg);
  int dir_reg = FindModifiedNextRegister(id_reg);
  
  int search_dir = abs(m_threads[m_cur_thread].reg[dir_reg].value) % 3;
  
  if (m_world->GetConfig().LOOK_DISABLE.Get() == 5) {
    int org_type = m_world->GetConfig().LOOK_DISABLE_TYPE.Get();
    bool is_target_type = false;
    if (org_type == 0 && m_organism->GetForageTarget() == -2) is_target_type = true;
    else if (org_type == 1 && m_organism->GetForageTarget() != -2) is_target_type = true;
    else if (org_type == 2) is_target_type = true;
    
    if (is_target_type) {
      int rand = m_world->GetRandom().GetInt(INT_MAX);
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

bool cHardwareExperimental::GoLook(cAvidaContext& ctx, const int look_dir, const int cell_id, bool use_ft) 
{
  // temp check on world geometry until code can handle other geometries
  if (m_world->GetConfig().WORLD_GEOMETRY.Get() != 1) {
    // Instruction sense-diff-ahead only written to work in bounded grids
    return false;
  }  
  if (NUM_REGISTERS < 8) m_world->GetDriver().Feedback().Error("Instruction look-ahead requires at least 8 registers");
  if (!m_use_avatar && m_organism->GetNeighborhoodSize() == 0) return false;
  else if (m_use_avatar && m_organism->GetOrgInterface().GetAVNumNeighbors() == 0) return false;
  
  // define our input (4) and output registers (8)
  sLookRegAssign reg_defs;
  reg_defs.habitat = FindModifiedRegister(rBX);
  // fail if the org is trying to sense a nest/hidden habitat
  int habitat_used = m_threads[m_cur_thread].reg[reg_defs.habitat].value;
  if (habitat_used == 3) return false;
  reg_defs.distance = FindModifiedNextRegister(reg_defs.habitat);
  reg_defs.search_type = FindModifiedNextRegister(reg_defs.distance);
  reg_defs.id_sought = FindModifiedNextRegister(reg_defs.search_type);
  reg_defs.count = FindModifiedNextRegister(reg_defs.id_sought);
  reg_defs.value = FindModifiedNextRegister(reg_defs.count);
  reg_defs.group = FindModifiedNextRegister(reg_defs.value);
  reg_defs.ft = FindModifiedNextRegister(reg_defs.group);
  
  cOrgSensor::sLookOut look_results;
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
  LookResults(reg_defs, look_results);
  return true;
}

cOrgSensor::sLookOut cHardwareExperimental::InitLooking(cAvidaContext& ctx, sLookRegAssign& in_defs, int facing, int cell_id, bool use_ft)
{
  const int habitat_reg = in_defs.habitat;
  const int distance_reg = in_defs.distance;
  const int search_reg = in_defs.search_type;
  const int id_reg = in_defs.id_sought;
  
  cOrgSensor::sLookInit reg_init;
  reg_init.habitat = m_threads[m_cur_thread].reg[habitat_reg].value;
  reg_init.distance = m_threads[m_cur_thread].reg[distance_reg].value;
  reg_init.search_type = m_threads[m_cur_thread].reg[search_reg].value;
  reg_init.id_sought = m_threads[m_cur_thread].reg[id_reg].value;

  return m_sensor.SetLooking(ctx, reg_init, facing, cell_id, use_ft);
}    

void cHardwareExperimental::LookResults(sLookRegAssign& regs, cOrgSensor::sLookOut& results)
{
  // habitat_reg=0, distance_reg=1, search_type_reg=2, id_sought_reg=3, count_reg=4, value_reg=5, group_reg=6, forager_type_reg=7
  // return defaults for failed to find
  if (results.report_type == 0) {
    setInternalValue(regs.habitat, results.habitat, true);
    setInternalValue(regs.distance, -1, true);
    setInternalValue(regs.search_type, results.search_type, true);
    setInternalValue(regs.id_sought, results.id_sought, true);
    setInternalValue(regs.count, 0, true);
    setInternalValue(regs.value, 0, true);
    setInternalValue(regs.group, -9, true);
    setInternalValue(regs.ft, -9, true);  
  }
  // report results as sent
  else if (results.report_type == 1) {
    setInternalValue(regs.habitat, results.habitat, true);
    setInternalValue(regs.distance, results.distance, true);
    setInternalValue(regs.search_type, results.search_type, true);
    setInternalValue(regs.id_sought, results.id_sought, true);
    setInternalValue(regs.count, results.count, true);
    setInternalValue(regs.value, results.value, true);
    setInternalValue(regs.group, results.group, true);
    setInternalValue(regs.ft, results.forage, true);  
  }
  
  if (m_world->GetConfig().LOOK_DISABLE.Get() > 5) {
    int org_type = m_world->GetConfig().LOOK_DISABLE_TYPE.Get();
    bool is_target_type = false;
    if (org_type == 0 && m_organism->GetForageTarget() == -2) is_target_type = true;
    else if (org_type == 1 && m_organism->GetForageTarget() != -2) is_target_type = true;
    else if (org_type == 2) is_target_type = true;
    
    if (is_target_type) {
      int randsign = m_world->GetRandom().GetUInt(0,2) ? -1 : 1;
      int rand = m_world->GetRandom().GetInt(INT_MAX) * randsign;
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
  return;
}

bool cHardwareExperimental::Inst_SenseFacedHabitat(cAvidaContext& ctx) 
{
  int reg_to_set = FindModifiedRegister(rBX);
  
  // get the resource library
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  
  // get the destination cell resource levels
  Apto::Array<double> cell_res;
  if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx); 
  
  // check for any habitats ahead that affect movement, returning the most 'severe' habitat type
  // simulated predator ahead
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 5 && cell_res[i] > 0) {
      setInternalValue(reg_to_set, 3, true);
      return true;
    }    
  }
  // are there any barrier resources in the faced cell    
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 2 && cell_res[i] > 0) {
      setInternalValue(reg_to_set, 2, true);
      return true;
    }    
  }
  // if no barriers, are there any hills in the faced cell    
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 1 && cell_res[i] > 0) {
      setInternalValue(reg_to_set, 1, true);
      return true;
    }
  }
  // if no barriers or hills, we return a 0 to indicate clear sailing
  setInternalValue(reg_to_set, 0, true);
  return true;
}

bool cHardwareExperimental::Inst_SetForageTarget(cAvidaContext&)
{
  assert(m_organism != 0);
  int prop_target = GetRegister(FindModifiedRegister(rBX));
  
  //return false if org setting target to current one (avoid paying costs for not switching)
  const int old_target = m_organism->GetForageTarget();
  if (old_target == prop_target) return false;

  // return false if predator trying to become prey and this has been disallowed
  if (old_target == -2 && (m_world->GetConfig().PRED_PREY_SWITCH.Get() == 0 || m_world->GetConfig().PRED_PREY_SWITCH.Get() == 2)) return false;
  
  // return false if trying to become predator and there are none in the experiment
  if (prop_target == -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0) return false;
  
  // return false if trying to become predator this has been disallowed via setforagetarget
  if (prop_target == -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() == 2) return false;
  
  // a little mod help...can't set to -1, that's for juevniles only...so only exception to mod help is -2
  if (!m_world->GetEnvironment().IsTargetID(prop_target) && prop_target != -2) {
    int num_fts = 0;
    std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
    set <int>::iterator itr;    
    for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) if (*itr != -1 && *itr != -2) num_fts++; 
    if (m_world->GetEnvironment().IsTargetID(-1) && num_fts == 0) prop_target = -1;
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
  if (m_use_avatar && ((prop_target == -2 && old_target != -2) || (prop_target != -2 && old_target == -2)) && 
      (m_organism->GetOrgInterface().GetAVCellID() != -1)) {
    m_organism->GetOrgInterface().SwitchPredPrey();
    m_organism->SetForageTarget(prop_target);
  }
  else m_organism->SetForageTarget(prop_target);
    
  // Set the new target and return the value
  m_organism->RecordFTSet();
  setInternalValue(FindModifiedRegister(rBX), prop_target, false);
  return true;
}

bool cHardwareExperimental::Inst_SetForageTargetOnce(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_organism->HasSetFT()) return false;
  else return Inst_SetForageTarget(ctx);
}

bool cHardwareExperimental::Inst_GetForageTarget(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  const int target_reg = FindModifiedRegister(rBX);
  setInternalValue(target_reg, m_organism->GetForageTarget(), false);
  return true;
}

bool cHardwareExperimental::Inst_GetLocOrgDensity(cAvidaContext& ctx) 
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
        if (cell->GetOrganism()->GetForageTarget() > -2) prey_count++;
        if (cell->GetOrganism()->GetForageTarget() == -2) pred_count++;
      }
      else if (m_use_avatar == 2) { 
        prey_count += cell->GetNumPreyAV();
        pred_count += cell->GetNumPredAV();
        if (cellid == m_organism->GetOrgInterface().GetAVCellID()) {
          if (m_organism->GetForageTarget() > -2) prey_count--;
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

bool cHardwareExperimental::Inst_GetFacedOrgDensity(cAvidaContext&) 
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
        if (cell->GetOrganism()->GetForageTarget() > -2) prey_count++;
        if (cell->GetOrganism()->GetForageTarget() == -2) pred_count++;
      }
      else if (m_use_avatar == 2) { 
        prey_count += cell->GetNumPreyAV();
        pred_count += cell->GetNumPredAV();
        if (cellid == m_organism->GetOrgInterface().GetAVCellID()) {
          if (m_organism->GetForageTarget() > -2) prey_count--;
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

bool cHardwareExperimental::DoActualCollect(cAvidaContext& ctx, int bin_used, bool unit)
{
  // Set up res_change and max total
  Apto::Array<double> res_count;
  if (!m_use_avatar) res_count = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) res_count = m_organism->GetOrgInterface().GetAVResources(ctx); 
  Apto::Array<double> res_change(res_count.GetSize());
  res_change.SetAll(0.0);
  double total = m_organism->GetRBinsTotal();
  double max = m_world->GetConfig().MAX_TOTAL_STORED.Get();
  bool has_max = max > 0 ? true : false;
  double res_consumed = 0.0;
  
  // Collect a unit or some ABSORB_RESOURCE_FRACTION
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  if (unit) {
    double threshold = resource_lib.GetResource(bin_used)->GetThreshold();
    if (res_count[bin_used] >= threshold) {
      res_consumed = threshold;
    }
    else {
      return false;
    }  
  }
  else {
    res_consumed = res_count[bin_used] * m_world->GetConfig().ABSORB_RESOURCE_FRACTION.Get();
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

bool cHardwareExperimental::Inst_CollectEdible(cAvidaContext& ctx)
{
  int absorb_type = m_world->GetConfig().MULTI_ABSORB_TYPE.Get();
  Apto::Array<double> res_count;
  if (!m_use_avatar) res_count = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) res_count = m_organism->GetOrgInterface().GetAVResources(ctx); 
  
  int res_id = -1;
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  if (absorb_type == 1) {
    for (int i = 0; i < res_count.GetSize(); i++) {
      if (res_count[i] >= resource_lib.GetResource(i)->GetThreshold() && (resource_lib.GetResource(i)->GetHabitat() == 0 || resource_lib.GetResource(i)->GetHabitat() > 5)) {
        res_id = i;
        break;
      }
    }
  }
  else if (absorb_type == 2) {
    for (int i = res_count.GetSize(); i > 0 ; i--) {
      if (res_count[i - 1] >= resource_lib.GetResource(i - 1)->GetThreshold() && (resource_lib.GetResource(i - 1)->GetHabitat() == 0 || resource_lib.GetResource(i - 1)->GetHabitat() > 5)) {
        res_id = i - 1;
        break;
      }
    }
  }
  else m_world->GetDriver().Feedback().Error("Instruction collect-edible only operational for MULTI_ABSORB_TYPE 1 or 2");
  bool success = false;
  if (res_id != -1) success = DoActualCollect(ctx, res_id, true);
  return success;
}

bool cHardwareExperimental::Inst_CollectSpecific(cAvidaContext& ctx)
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

bool cHardwareExperimental::Inst_DepositResource(cAvidaContext& ctx)
{
  int resource_amount = abs(GetRegister(FindModifiedNextRegister(rBX)));
  int resource_id = GetRegister(FindModifiedRegister(rBX));
  resource_id %= m_organism->GetRBins().GetSize();
  const double stored_res = m_organism->GetRBins()[resource_id];
    
  if (resource_amount > stored_res) resource_amount = (int)(stored_res);
  bool success = false;
  if (stored_res >= resource_amount && resource_amount > 0) {
    Apto::Array<double> res_count;
    if (!m_use_avatar) res_count = m_organism->GetOrgInterface().GetResources(ctx);
    else if (m_use_avatar) res_count = m_organism->GetOrgInterface().GetAVResources(ctx); 
    const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
    // only allow deposits on dens
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->GetHabitat() == 4 && res_count[i] > resource_lib.GetResource(i)->GetThreshold()) {
        m_organism->AddToRBin(resource_id, -1 * resource_amount);
        Apto::Array<double> res_change(res_count.GetSize());
        res_change.SetAll(0.0);
        res_change[resource_id] = resource_amount;
        if (!m_use_avatar) m_organism->GetOrgInterface().UpdateResources(ctx, res_change);
        else if (m_use_avatar) m_organism->GetOrgInterface().UpdateAVResources(ctx, res_change);
        success = true;
        break;
      }
    }
  }
  double res_after = m_organism->GetRBin(resource_id);
  int out_reg = FindModifiedRegister(rBX);
  if (success) setInternalValue(out_reg, (int)(res_after), true);
  else if (!success && resource_amount > 0) setInternalValue(out_reg, -1, true);
  return success;
}

bool cHardwareExperimental::Inst_DepositSpecific(cAvidaContext& ctx)
{
  int resource_amount = GetRegister(FindModifiedRegister(rBX));
  const int spec_res = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  const double stored_spec = m_organism->GetRBins()[spec_res];

  if (resource_amount > stored_spec) resource_amount = (int)(stored_spec);
  bool success = false;
  if (stored_spec >= resource_amount && resource_amount > 0) {
    Apto::Array<double> res_count;
    if (!m_use_avatar) res_count = m_organism->GetOrgInterface().GetResources(ctx);
    else if (m_use_avatar) res_count = m_organism->GetOrgInterface().GetAVResources(ctx); 
    const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
    // only allow deposits on dens
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->GetHabitat() == 4 && res_count[i] > resource_lib.GetResource(i)->GetThreshold()) {
        m_organism->AddToRBin(spec_res, -1 * resource_amount);
        Apto::Array<double> res_change(res_count.GetSize());
        res_change.SetAll(0.0);
        res_change[spec_res] = resource_amount;
        if (!m_use_avatar) m_organism->GetOrgInterface().UpdateResources(ctx, res_change);
        else if (m_use_avatar) m_organism->GetOrgInterface().UpdateAVResources(ctx, res_change);
        success = true;
        break;
      }
    }
  }
  double res_after = m_organism->GetRBin(spec_res);
  int out_reg = FindModifiedRegister(rBX);
  if (success) setInternalValue(out_reg, (int)(res_after), true);
  else if (!success && resource_amount > 0) setInternalValue(out_reg, -1, true);
  return success;
}

bool cHardwareExperimental::Inst_DepositAllAsSpecific(cAvidaContext& ctx)
{
  const int spec_res = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();    
  bool success = false;
  Apto::Array<double> res_count;
  if (!m_use_avatar) res_count = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) res_count = m_organism->GetOrgInterface().GetAVResources(ctx); 
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  // only allow deposits on dens
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 4 && res_count[i] > resource_lib.GetResource(i)->GetThreshold()) {
      double total_deposit = 0.0;
      for (int j = 0; j < res_count.GetSize(); j++) {
        double resource_amount = m_organism->GetRBins()[j];
        m_organism->AddToRBin(j, -1 * resource_amount);
        total_deposit += resource_amount;
      }  
      if (total_deposit > 0) {  
        Apto::Array<double> res_change(res_count.GetSize());
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
  int out_reg = FindModifiedRegister(rBX);
  if (success) setInternalValue(out_reg, 1, true);
  else if (!success) setInternalValue(out_reg, -1, true);
  return success;
}

bool cHardwareExperimental::Inst_NopDepositSpecific(cAvidaContext& ctx)
{
  int resource_amount = GetRegister(FindModifiedRegister(rBX));
  const int spec_res = m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get();
  const double stored_spec = m_organism->GetRBins()[spec_res];

  if (resource_amount > stored_spec) resource_amount = (int)(stored_spec);
  bool success = false;
  if (stored_spec >= resource_amount && resource_amount > 0) {
    Apto::Array<double> res_count;
    if (!m_use_avatar) res_count = m_organism->GetOrgInterface().GetResources(ctx);
    else if (m_use_avatar) res_count = m_organism->GetOrgInterface().GetAVResources(ctx); 
    const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
    // only allow deposits on dens
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->GetHabitat() == 4 && res_count[i] > resource_lib.GetResource(i)->GetThreshold()) {
        success = true;
        break;
      }
    }
  }
  return success;
}

bool cHardwareExperimental::Inst_NopDepositResource(cAvidaContext& ctx)
{
  int resource_amount = abs(GetRegister(FindModifiedNextRegister(rBX)));
  int resource_id = GetRegister(FindModifiedRegister(rBX));
  resource_id %= m_organism->GetRBins().GetSize();
  const double stored_res = m_organism->GetRBins()[resource_id];
    
  if (resource_amount > stored_res) resource_amount = (int)(stored_res);
  bool success = false;
  if (stored_res >= resource_amount && resource_amount > 0) {
    Apto::Array<double> res_count;
    if (!m_use_avatar) res_count = m_organism->GetOrgInterface().GetResources(ctx);
    else if (m_use_avatar) res_count = m_organism->GetOrgInterface().GetAVResources(ctx); 
    const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
    // only allow deposits on dens
    for (int i = 0; i < resource_lib.GetSize(); i++) {
      if (resource_lib.GetResource(i)->GetHabitat() == 4 && res_count[i] > resource_lib.GetResource(i)->GetThreshold()) {
        success = true;
        break;
      }
    }
  }
  return success;
}

bool cHardwareExperimental::Inst_NopDepositAllAsSpecific(cAvidaContext& ctx)
{
  bool success = false;
  Apto::Array<double> res_count;
  if (!m_use_avatar) res_count = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) res_count = m_organism->GetOrgInterface().GetAVResources(ctx); 
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  // only allow deposits on dens
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetHabitat() == 4 && res_count[i] > resource_lib.GetResource(i)->GetThreshold()) {
      double total_deposit = 0;
      for (int j = 0; j < res_count.GetSize(); j++) {
        double resource_amount = m_organism->GetRBins()[j];
        total_deposit += resource_amount;
      }  
      if (total_deposit > 0) {  
        success = true;
      }
      break;
    }
  }
  return success;
}

bool cHardwareExperimental::Inst_NopCollectEdible(cAvidaContext& ctx)
{
  int absorb_type = m_world->GetConfig().MULTI_ABSORB_TYPE.Get();
  Apto::Array<double> res_count;
  if (!m_use_avatar) res_count = m_organism->GetOrgInterface().GetResources(ctx);
  else if (m_use_avatar) res_count = m_organism->GetOrgInterface().GetAVResources(ctx); 
  
  int res_id = -1;
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  if (absorb_type == 1) {
    for (int i = 0; i < res_count.GetSize(); i++) {
      if (res_count[i] >= resource_lib.GetResource(i)->GetThreshold() && (resource_lib.GetResource(i)->GetHabitat() == 0 || resource_lib.GetResource(i)->GetHabitat() > 5)) {
        res_id = i;
        break;
      }
    }
  }
  else if (absorb_type == 2) {
    for (int i = res_count.GetSize(); i > 0 ; i--) {
      if (res_count[i - 1] >= resource_lib.GetResource(i - 1)->GetThreshold() && (resource_lib.GetResource(i - 1)->GetHabitat() == 0 || resource_lib.GetResource(i - 1)->GetHabitat() > 5)) {
        res_id = i - 1;
        break;
      }
    }
  }

  if (res_id != -1) {
    double total = m_organism->GetRBinsTotal();
    double max = m_world->GetConfig().MAX_TOTAL_STORED.Get();
    bool has_max = max > 0 ? true : false;
    double res_consumed = 0.0;
    
    double threshold = resource_lib.GetResource(res_id)->GetThreshold();
    if (res_count[res_id] >= threshold) res_consumed = threshold;
    else return false;
    
    if (has_max && res_consumed + total >= max) res_consumed = max - total;
    
    if (res_consumed > 0) return true;
  }
  return false;
}

bool cHardwareExperimental::Inst_GetResStored(cAvidaContext& ctx)
{
  int resource_id = abs(GetRegister(FindModifiedRegister(rBX)));
  Apto::Array<double> bins = m_organism->GetRBins();
  resource_id %= bins.GetSize();
  int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, (int)(bins[resource_id]), true);
  return true;
}

bool cHardwareExperimental::Inst_GetSpecificStored(cAvidaContext& ctx)
{
  Apto::Array<double> bins = m_organism->GetRBins();
  int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, (int)(bins[m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get()]), true);
  return true;
}

bool cHardwareExperimental::Inst_CollectCellData(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  const int out_reg = FindModifiedRegister(rBX);
  int value = m_organism->GetCellData();
  setInternalValue(out_reg, value, true);
  // Update last collected cell data
  m_last_cell_data = std::make_pair(true, value);

  return true;
}

bool cHardwareExperimental::Inst_IfCellDataChanged(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  // If cell data hasn't been collected, or it's the same as the current cell data, advance IP
  if (!m_last_cell_data.first || (m_last_cell_data.second == m_organism->GetCellData())) {
    getIP().Advance();
  }

  return true;
}

bool cHardwareExperimental::Inst_ReadCellData(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetCellData(), true);

  return true;
}

// Sets organism's opinion to the value in ?BX?
bool cHardwareExperimental::Inst_SetOpinion(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  m_organism->GetOrgInterface().SetOpinion(GetRegister(FindModifiedRegister(rBX)), m_organism);
  return true;
}

/* Gets the organism's current opinion, placing the opinion in register ?BX?
   and the age of the opinion in register !?BX?
 */
bool cHardwareExperimental::Inst_GetOpinion(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_organism->GetOrgInterface().HasOpinion(m_organism)) {
    const int opinion_reg = FindModifiedRegister(rBX);
    const int age_reg = FindNextRegister(opinion_reg);

    setInternalValue(opinion_reg, m_organism->GetOpinion().first, true);
    setInternalValue(age_reg, m_world->GetStats().GetUpdate() - m_organism->GetOpinion().second, true);
  }
  return true;
}

//! An organism joins a group by setting it opinion to the group id. 
bool cHardwareExperimental::Inst_JoinGroup(cAvidaContext&)
{
  int opinion = m_world->GetConfig().DEFAULT_GROUP.Get();
  // Check if the org is currently part of a group
  assert(m_organism != 0);
	
  int prop_group_id = GetRegister(FindModifiedRegister(rBX));
  
  // check if this is a valid group
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 2 &&
      !(m_world->GetEnvironment().IsGroupID(prop_group_id))) {
    return false; 
  }
  // injected orgs might not have an opinion
  if (m_organism->GetOrgInterface().HasOpinion(m_organism)) {
    opinion = m_organism->GetOpinion().first;
    
    //return false if org setting opinion to current one (avoid paying costs for not switching)
    if (opinion == prop_group_id) return false;
    
    // A random chance for failure to join group based on config, if failed return true for resource cost.
    if (m_world->GetConfig().JOIN_GROUP_FAILURE.Get() > 0) {
      int percent_failure = m_world->GetConfig().JOIN_GROUP_FAILURE.Get();
      double prob_failure = (double) percent_failure / 100.0;
      double rand = m_world->GetRandom().GetDouble();
      if (rand <= prob_failure) return true;
    }
    
    // If tolerances are on the org must pass immigration chance 
    if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
      m_organism->GetOrgInterface().AttemptImmigrateGroup(prop_group_id, m_organism);
      return true;
    }
    else {
      // otherwise, subtract org from current group
      m_organism->LeaveGroup(opinion);
    }
  }
  
  // Set the opinion
  m_organism->GetOrgInterface().SetOpinion(prop_group_id, m_organism);
  
  // Add org to group count
  if (m_organism->GetOrgInterface().HasOpinion(m_organism)) {
    opinion = m_organism->GetOpinion().first;	
    m_organism->JoinGroup(opinion);
  }
  
  return true;
}

// A predator can establish a new group, attempt to immigrate into the group that marked the cell in front of them, or become a nomad. 
bool cHardwareExperimental::Inst_ChangePredGroup(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_organism->GetForageTarget() != -2) return false;
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() != 1) return false;
  
  // If not nop-modified, fails to execute.
  if (!(m_inst_set->IsNop(getIP().GetNextInst()))) return false;
  const int nop_reg = FindModifiedRegister(rBX);
  
  /*// TEMP CODE FOR PRED JOIN RANDOM GROUP JUST TO START SOME TESTS
   int group = m_world->GetConfig().DEFAULT_GROUP.Get();
   const int prop_group_id = m_world->GetRandom().GetUInt(0,1000);
   if (m_organism->HasOpinion()) {
   group = m_organism->GetOpinion().first;
   if (group == prop_group_id) return false;
   m_organism->LeaveGroup(group);
   }
   m_organism->SetOpinion(prop_group_id);
   group = m_organism->GetOpinion().first;	
   m_organism->JoinGroup(group);*/
  
  // **If ?AX? make a new group.
  if (nop_reg == rAX) return Inst_MakePredGroup(ctx);
  // **If ?BX? change to group -1.
  else if (nop_reg == rBX) return Inst_LeavePredGroup(ctx);
  // **If ?CX? read m_organism->GetOrgInterface().GetFacedCellDataTerritory() and attempt immigration into that group.
  else if (nop_reg == rCX) return Inst_AdoptPredGroup(ctx);
  
  // **return (new) group ID & change success          
  return false;
}

// A predator establishes a new group. 
bool cHardwareExperimental::Inst_MakePredGroup(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_organism->GetForageTarget() != -2) return false;
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() != 1) return false;
  
  // If in a group, leave it.
  if (m_organism->HasOpinion()) {
    int group = m_organism->GetOpinion().first;
    m_organism->LeaveGroup(group);
  }
  
  // Creates new group and joins as well.
  m_organism->GetOrgInterface().MakeGroup();
  return true;
}

// A predator leaves their group to join the nomads in group -3.
// Joining the nomads is always successful, they can not exclude others so there is no immigration test. 
bool cHardwareExperimental::Inst_LeavePredGroup(cAvidaContext& ctx)
{
  // Predator nomad group id
  const int nomad_group = -3;
  
  // Confirm the org is a pred and groups are on.
  assert(m_organism != 0);
  if (m_organism->GetForageTarget() != -2) return false;
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() != 1) return false;
  
  // If in a group, leave it.
  if (m_organism->HasOpinion()) {
    int group = m_organism->GetOpinion().first;
    // If already in the nomad group, don't move.
    if (group == nomad_group) return false;
    m_organism->LeaveGroup(group);
  }
  
  // Join the nomads.
  m_organism->SetOpinion(nomad_group);
  m_organism->JoinGroup(nomad_group);
  return true;
}

// A predator attempts to join the existing, non-empty predator group associated with the cell marking in front of them. 
bool cHardwareExperimental::Inst_AdoptPredGroup(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_organism->GetForageTarget() != -2) return false;
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() != 1) return false;
  
  // Read target group from the faced marked cell.
  int prop_group_id = m_organism->GetOrgInterface().GetFacedCellDataTerritory();
  if (m_use_avatar == 2) prop_group_id = m_organism->GetOrgInterface().GetAVFacedDataTerritory();
  if (prop_group_id == -1) return false;
  
  // Check if the cell marking has expired.
  int current_update = m_world->GetStats().GetUpdate();
  int update_marked = m_organism->GetOrgInterface().GetFacedCellDataUpdate();
  if (m_use_avatar == 2) prop_group_id = m_organism->GetOrgInterface().GetAVFacedDataUpdate();
  int expire_window = m_world->GetConfig().MARKING_EXPIRE_DATE.Get();
  if (current_update > (update_marked + expire_window)) return false;
  
  // If the same as current group, don't move.
  if (m_organism->HasOpinion()) {
    if (m_organism->GetOpinion().first == prop_group_id) {
      return false;
    }
  }
  
  // Check if the target group is now empty, cannot join an empty group must create a new group.
  if (m_organism->GetOrgInterface().NumberOfOrganismsInGroup(prop_group_id) == 0) return false;
  
  // Attempt to immigrate to the target group
  m_organism->GetOrgInterface().AttemptImmigrateGroup(prop_group_id, m_organism);
  return true;
}

bool cHardwareExperimental::Inst_GetGroupID(cAvidaContext&)
{
  assert(m_organism != 0);
  if (m_organism->HasOpinion()) {
    const int group_reg = FindModifiedRegister(rBX);
    
    setInternalValue(group_reg, m_organism->GetOpinion().first, false);
  }
  return true;
}

bool cHardwareExperimental::Inst_GetPredGroupID(cAvidaContext&)
{
  assert(m_organism != 0);
  if (m_organism->GetForageTarget() != -2) return false;
  if (m_organism->HasOpinion()) {
    const int group_reg = FindModifiedRegister(rBX);
    
    setInternalValue(group_reg, m_organism->GetOpinion().first, false);
  }
  return true;
}

bool cHardwareExperimental::Inst_GetFacedOrgID(cAvidaContext&)
//Get ID of organism faced by this one, if there is an organism in front.
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  cOrganism* neighbor = NULL;
  if (!m_use_avatar && !m_organism->IsNeighborCellOccupied()) return false;
  else if (m_use_avatar == 2 && !m_organism->GetOrgInterface().FacedHasAV()) return false;
  
  if (!m_use_avatar) neighbor = m_organism->GetOrgInterface().GetNeighbor();
  else if (m_use_avatar == 2) neighbor = m_organism->GetOrgInterface().GetRandFacedAV();
  if (neighbor->IsDead())  return false;  
  
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, neighbor->GetID(), true);
  return true;
}

//Attack organism faced by this one, if there is non-predator target in front, and steal it's merit, current bonus, and reactions. 
bool cHardwareExperimental::Inst_AttackPrey(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (!TestAttack(ctx)) return false;

  cOrganism* target = NULL;
  if (!m_use_avatar) { 
    target = m_organism->GetOrgInterface().GetNeighbor();
  }
  else if (m_use_avatar == 2) target = m_organism->GetOrgInterface().GetRandFacedPreyAV();

  // attacking other carnivores is handled differently (e.g. using fights or tolerance)
  if (target->GetForageTarget() == -2) return false;
  
  if (target->IsDead()) return false;  
  
  const int success_reg = FindModifiedRegister(rBX);   
  const int bonus_reg = FindModifiedNextRegister(success_reg);
  const int bin_reg = FindModifiedNextRegister(bonus_reg);

  if (m_world->GetRandom().GetDouble() >= m_world->GetConfig().PRED_ODDS.Get() || 
      (m_world->GetConfig().MIN_PREY.Get() && m_world->GetStats().GetNumPreyCreatures() <= m_world->GetConfig().MIN_PREY.Get())) {
    m_organism->GetOrgInterface().RecordMinPreyFailedAttack();  
    double injury = m_world->GetConfig().PRED_INJURY.Get();
    if (injury > 0) InjureOrg(target, injury);
    setInternalValue(success_reg, -1, true);   
    setInternalValue(bonus_reg, -1, true);
    if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) setInternalValue(bin_reg, -1, true);
    return false;  
  }
  else {
    // add prey's merit to predator's--this will result in immediately applying merit increases; adjustments to bonus, give increase in next generation
    if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
      const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
      double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
      attacker_merit += target_merit * m_world->GetConfig().PRED_EFFICIENCY.Get();
      m_organism->UpdateMerit(attacker_merit);
    }
    
    // now add on the victims reaction counts to your own, this will allow you to pass any reaction tests...
    Apto::Array<int> target_reactions = target->GetPhenotype().GetLastReactionCount();
    Apto::Array<int> org_reactions = m_organism->GetPhenotype().GetStolenReactionCount();
    for (int i = 0; i < org_reactions.GetSize(); i++) {
      org_reactions[i] += target_reactions[i];
      m_organism->GetPhenotype().SetStolenReactionCount(i, org_reactions[i]);
    }
    
    // and add current merit bonus after adjusting for conversion efficiency
    const double target_bonus = target->GetPhenotype().GetCurBonus();
    m_organism->GetPhenotype().SetCurBonus(m_organism->GetPhenotype().GetCurBonus() + (target_bonus * m_world->GetConfig().PRED_EFFICIENCY.Get()));
    
    // now add the victims internal resource bins to your own, if enabled, after correcting for conversion efficiency
    if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
      Apto::Array<double> target_bins = target->GetRBins();
      for (int i = 0; i < target_bins.GetSize(); i++) {
        m_organism->AddToRBin(i, target_bins[i] * m_world->GetConfig().PRED_EFFICIENCY.Get());
      }
      const int spec_bin = (int) (m_organism->GetRBins()[m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get()]);
      setInternalValue(bin_reg, spec_bin, true);
    }
    
    // if you weren't a predator before, you are now!
    MakePred();
    target->Die(ctx);
    
    setInternalValue(success_reg, 1, true);   
    setInternalValue(bonus_reg, (int) (target_bonus), true);
  }
  return true;
} 		

//Attack organism faced by this one, if there is non-predator target in front, and steal it's merit, current bonus, and reactions. 
bool cHardwareExperimental::Inst_AttackFTPrey(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (!TestAttack(ctx)) return false;

  const int target_reg = FindModifiedRegister(rBX);
  int target_org_type = m_threads[m_cur_thread].reg[target_reg].value;
  cOrganism* target = NULL; 
  if (!m_use_avatar) { 
    target = m_organism->GetOrgInterface().GetNeighbor();
    if (target_org_type != target->GetForageTarget()) return false;
    // attacking other carnivores is handled differently (e.g. using fights or tolerance)
    if (target->GetForageTarget() == -2) return false;
  }    
  else if (m_use_avatar == 2) {
    const Apto::Array<cOrganism*>& av_neighbors = m_organism->GetOrgInterface().GetFacedPreyAVs();
    bool target_match = false;
    int rand_index = m_world->GetRandom().GetUInt(0, av_neighbors.GetSize());
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
    if (!target_match) return false;
  } 
  // attacking other carnivores is handled differently (e.g. using fights or tolerance)
  if (target->GetForageTarget() == -2) return false;
  
  if (target->IsDead()) return false;  
  
  const int success_reg = FindModifiedRegister(target_reg);   
  const int bonus_reg = FindModifiedNextRegister(success_reg);
  const int bin_reg = FindModifiedNextRegister(bonus_reg);

  if (m_world->GetRandom().GetDouble() >= m_world->GetConfig().PRED_ODDS.Get() || 
      (m_world->GetConfig().MIN_PREY.Get() && m_world->GetStats().GetNumPreyCreatures() <= m_world->GetConfig().MIN_PREY.Get())) {
    m_organism->GetOrgInterface().RecordMinPreyFailedAttack();  
    double injury = m_world->GetConfig().PRED_INJURY.Get();
    if (injury > 0) InjureOrg(target, injury);
    setInternalValue(success_reg, -1, true);   
    setInternalValue(bonus_reg, -1, true);
    if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) setInternalValue(bin_reg, -1, true);
    return false;  
  }
  else {
    // add prey's merit to predator's--this will result in immediately applying merit increases; adjustments to bonus, give increase in next generation
    if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
      const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
      double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
      attacker_merit += target_merit * m_world->GetConfig().PRED_EFFICIENCY.Get();
      m_organism->UpdateMerit(attacker_merit);
    }
    
    // now add on the victims reaction counts to your own, this will allow you to pass any reaction tests...
    Apto::Array<int> target_reactions = target->GetPhenotype().GetLastReactionCount();
    Apto::Array<int> org_reactions = m_organism->GetPhenotype().GetStolenReactionCount();
    for (int i = 0; i < org_reactions.GetSize(); i++) {
      org_reactions[i] += target_reactions[i];
      m_organism->GetPhenotype().SetStolenReactionCount(i, org_reactions[i]);
    }
    
    // and add current merit bonus after adjusting for conversion efficiency
    const double target_bonus = target->GetPhenotype().GetCurBonus();
    m_organism->GetPhenotype().SetCurBonus(m_organism->GetPhenotype().GetCurBonus() + (target_bonus * m_world->GetConfig().PRED_EFFICIENCY.Get()));
    
    // now add the victims internal resource bins to your own, if enabled, after correcting for conversion efficiency
    if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
      Apto::Array<double> target_bins = target->GetRBins();
      for (int i = 0; i < target_bins.GetSize(); i++) {
        m_organism->AddToRBin(i, target_bins[i] * m_world->GetConfig().PRED_EFFICIENCY.Get());
      }
      const int spec_bin = (int) (m_organism->GetRBins()[m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get()]);
      setInternalValue(bin_reg, spec_bin, true);
    }
    
    // if you weren't a predator before, you are now!
    MakePred();
    target->Die(ctx);
    
    setInternalValue(success_reg, 1, true);   
    setInternalValue(bonus_reg, (int) (target_bonus), true);
  }
  return true;
} 

//Attack organism faced by this one if you are both predators or both prey. 
bool cHardwareExperimental::Inst_FightMeritOrg(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) { 
    if (!m_organism->IsNeighborCellOccupied()) return false;
    target = m_organism->GetOrgInterface().GetNeighbor();
    // allow only for predator vs predator or prey vs prey
    if ((target->GetForageTarget() == -2 && m_organism->GetForageTarget() != -2) || 
        (target->GetForageTarget() != -2 && m_organism->GetForageTarget() == -2)) {
      return false;
    }
  }
  else if (m_use_avatar == 2) {
    if (!m_organism->GetOrgInterface().FacedHasAV()) return false;
    if (m_organism->GetForageTarget() != -2) { 
      if (!m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPreyAV();
    }
    else if (m_organism->GetForageTarget() == -2) { 
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
bool cHardwareExperimental::Inst_FightBonusOrg(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) { 
    if (!m_organism->IsNeighborCellOccupied()) return false;
    target = m_organism->GetOrgInterface().GetNeighbor();
    // allow only for predator vs predator or prey vs prey
    if ((target->GetForageTarget() == -2 && m_organism->GetForageTarget() != -2) || 
        (target->GetForageTarget() != -2 && m_organism->GetForageTarget() == -2)) {
      return false;
    }
  }
  else if (m_use_avatar == 2) {
    if (!m_organism->GetOrgInterface().FacedHasAV()) return false;
    if (m_organism->GetForageTarget() != -2) { 
      if (!m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPreyAV();
    }
    else if (m_organism->GetForageTarget() == -2) { 
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
bool cHardwareExperimental::Inst_GetMeritFightOdds(cAvidaContext&)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) { 
    if (!m_organism->IsNeighborCellOccupied()) return false;
    target = m_organism->GetOrgInterface().GetNeighbor();
    // allow only for predator vs predator or prey vs prey
    if ((target->GetForageTarget() == -2 && m_organism->GetForageTarget() != -2) || 
        (target->GetForageTarget() != -2 && m_organism->GetForageTarget() == -2)) {
      return false;
    }
  }
  else if (m_use_avatar == 2) {
    if (!m_organism->GetOrgInterface().FacedHasAV()) return false;
    if (m_organism->GetForageTarget() != -2) { 
      if (!m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPreyAV();
    }
    else if (m_organism->GetForageTarget() == -2) { 
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
bool cHardwareExperimental::Inst_FightOrg(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  cOrganism* target = NULL;
  if (!m_use_avatar) { 
    if (!m_organism->IsNeighborCellOccupied()) return false;
    target = m_organism->GetOrgInterface().GetNeighbor();
    // allow only for predator vs predator or prey vs prey
    if ((target->GetForageTarget() == -2 && m_organism->GetForageTarget() != -2) || 
        (target->GetForageTarget() != -2 && m_organism->GetForageTarget() == -2)) {
      return false;
    }
  }
  else if (m_use_avatar == 2) {
    if (!m_organism->GetOrgInterface().FacedHasAV()) return false;
    if (m_organism->GetForageTarget() != -2) { 
      if (!m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
      else target = m_organism->GetOrgInterface().GetRandFacedPreyAV();
    }
    else if (m_organism->GetForageTarget() == -2) { 
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

bool cHardwareExperimental::Inst_AttackPred(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (!TestAttack(ctx)) return false;

  cOrganism* target = NULL;
  if (!m_use_avatar) { 
    target = m_organism->GetOrgInterface().GetNeighbor();
  }
  else if (m_use_avatar == 2) target = m_organism->GetOrgInterface().GetRandFacedPreyAV();

  if (target->GetForageTarget() != -2 || m_organism->GetForageTarget() != -2) return false;

  if (target->IsDead()) return false;  
  
  const int success_reg = FindModifiedRegister(rBX);   
  const int bonus_reg = FindModifiedNextRegister(success_reg);
  const int bin_reg = FindModifiedNextRegister(bonus_reg);

  if (m_world->GetRandom().GetDouble() >= m_world->GetConfig().PRED_ODDS.Get()) {
    double injury = m_world->GetConfig().PRED_INJURY.Get();
    if (injury > 0) InjureOrg(target, injury);
    setInternalValue(success_reg, -1, true);   
    setInternalValue(bonus_reg, -1, true);
    if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) setInternalValue(bin_reg, -1, true);
    return false;
  }
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
      org_reactions[i] += target_reactions[i];
      m_organism->GetPhenotype().SetStolenReactionCount(i, org_reactions[i]);
    }
    
    const double target_bonus = target->GetPhenotype().GetCurBonus();
    m_organism->GetPhenotype().SetCurBonus(m_organism->GetPhenotype().GetCurBonus() + (target_bonus * m_world->GetConfig().PRED_EFFICIENCY.Get()));
    
    if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
      Apto::Array<double> target_bins = target->GetRBins();
      for (int i = 0; i < target_bins.GetSize(); i++) {
        m_organism->AddToRBin(i, target_bins[i] * m_world->GetConfig().PRED_EFFICIENCY.Get());
      }
      const int spec_bin = (int) (m_organism->GetRBins()[m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get()]);
      setInternalValue(bin_reg, spec_bin, true);
    }
    
    target->Die(ctx);
    
    setInternalValue(success_reg, 1, true);   
    setInternalValue(bonus_reg, (int) (target_bonus), true);
  }
  return true;
} 

//Attack organism faced by this one if you are both predators. 
bool cHardwareExperimental::Inst_KillPred(cAvidaContext& ctx)
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
  if (target->GetForageTarget() != -2 || m_organism->GetForageTarget() != -2) return false;
  
  int target_cell = target->GetOrgInterface().GetCellID();  
  m_organism->GetOrgInterface().AttackFacedOrg(ctx, target_cell); 
  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, 1, true);   
  
  return true;
} 

//Attack organism faced by this one if you are both predators or both prey. 
bool cHardwareExperimental::Inst_FightPred(cAvidaContext& ctx)
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
  if (target->GetForageTarget() != -2 || m_organism->GetForageTarget() != -2) return false;
  
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

bool cHardwareExperimental::Inst_MarkCell(cAvidaContext&)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;  
  const int marking = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  if (!m_use_avatar) m_organism->SetCellData(marking);
  else if (m_use_avatar == 2) m_organism->GetOrgInterface().SetAVCellData(marking, m_organism->GetID());  
  return true;
}

bool cHardwareExperimental::Inst_MarkPredCell(cAvidaContext&)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;  
  if (m_organism->GetForageTarget() != -2) return false;
  const int marking = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  if (!m_use_avatar) m_organism->SetCellData(marking);
  else if (m_use_avatar == 2) m_organism->GetOrgInterface().SetAVCellData(marking, m_organism->GetID());
  return true;
}

bool cHardwareExperimental::Inst_MarkGroupCell(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;  
  const int marking = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  if (!m_use_avatar) m_organism->SetCellData(marking);
  else if (m_use_avatar == 2) m_organism->GetOrgInterface().SetAVCellData(marking, m_organism->GetID());
  return true;
}

bool cHardwareExperimental::Inst_ReadGroupCell(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  bool success = true;
  if (m_use_avatar && m_use_avatar != 2) success = false;  
  if (success && !m_organism->HasOpinion()) success = false;
  if (success && m_use_avatar == 2) if (m_organism->GetOrgInterface().GetAVDataTerritory() != m_organism->GetOpinion().first) success = false;
  if (success && !m_use_avatar) if (m_organism->GetOrgInterface().GetCellDataTerritory() != m_organism->GetOpinion().first) success = false;
  
  if (success) {
    const int marking_reg = FindModifiedRegister(rBX);
    const int update_reg = FindModifiedNextRegister(rBX);
    const int org_reg = FindModifiedNextRegister(update_reg);
    if (!m_use_avatar) {
      setInternalValue(marking_reg, m_organism->GetOrgInterface().GetCellData(), true);
      setInternalValue(update_reg, m_world->GetStats().GetUpdate() - m_organism->GetOrgInterface().GetCellDataUpdate(), true);
      setInternalValue(org_reg, m_organism->GetOrgInterface().GetCellDataOrgID(), true);
      if (NUM_REGISTERS > 3) {
        const int group_reg = FindModifiedNextRegister(org_reg);
        setInternalValue(group_reg, m_organism->GetOrgInterface().GetCellDataTerritory(), true);    
      }
    }
    else if (m_use_avatar == 2) {
      setInternalValue(marking_reg, m_organism->GetOrgInterface().GetAVData(), true);
      setInternalValue(update_reg, m_world->GetStats().GetUpdate() - m_organism->GetOrgInterface().GetAVDataUpdate(), true);
      setInternalValue(org_reg, m_organism->GetOrgInterface().GetAVDataOrgID(), true);
      if (NUM_REGISTERS > 3) {
        const int group_reg = FindModifiedNextRegister(org_reg);
        setInternalValue(group_reg, m_organism->GetOrgInterface().GetAVDataTerritory(), true);    
      }
    }
  }
  return success;
}

bool cHardwareExperimental::Inst_ReadFacedCell(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;  
  const int marking_reg = FindModifiedRegister(rBX);
  const int update_reg = FindModifiedNextRegister(rBX);
  const int org_reg = FindModifiedNextRegister(update_reg);
  if (!m_use_avatar) {
    setInternalValue(marking_reg, m_organism->GetOrgInterface().GetFacedCellData(), true);
    setInternalValue(update_reg, m_world->GetStats().GetUpdate() - m_organism->GetOrgInterface().GetFacedCellDataUpdate(), true);
    setInternalValue(org_reg, m_organism->GetOrgInterface().GetFacedCellDataOrgID(), true);
    if (NUM_REGISTERS > 3) {
      const int group_reg = FindModifiedNextRegister(org_reg);
      setInternalValue(group_reg, m_organism->GetOrgInterface().GetFacedCellDataTerritory(), true);    
    }
  }
  else if (m_use_avatar == 2) {
    setInternalValue(marking_reg, m_organism->GetOrgInterface().GetAVFacedData(), true);
    setInternalValue(update_reg, m_world->GetStats().GetUpdate() - m_organism->GetOrgInterface().GetAVFacedDataUpdate(), true);
    setInternalValue(org_reg, m_organism->GetOrgInterface().GetAVFacedDataOrgID(), true);
    if (NUM_REGISTERS > 3) {
      const int group_reg = FindModifiedNextRegister(org_reg);
      setInternalValue(group_reg, m_organism->GetOrgInterface().GetAVFacedDataTerritory(), true);    
    }
  }
  return true;
}

bool cHardwareExperimental::Inst_ReadFacedPredCell(cAvidaContext&)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;  
  if (m_organism->GetForageTarget() != -2) return false;
  const int marking_reg = FindModifiedRegister(rBX);
  const int update_reg = FindModifiedNextRegister(rBX);
  const int org_reg = FindModifiedNextRegister(update_reg);
  if (!m_use_avatar) {
    setInternalValue(marking_reg, m_organism->GetOrgInterface().GetFacedCellData(), true);
    setInternalValue(update_reg, m_world->GetStats().GetUpdate() - m_organism->GetOrgInterface().GetFacedCellDataUpdate(), true);
    setInternalValue(org_reg, m_organism->GetOrgInterface().GetFacedCellDataOrgID(), true);
    if (NUM_REGISTERS > 3) {
      const int group_reg = FindModifiedNextRegister(org_reg);
      setInternalValue(group_reg, m_organism->GetOrgInterface().GetFacedCellDataTerritory(), true);    
    }
  }
  else if (m_use_avatar == 2) {
    setInternalValue(marking_reg, m_organism->GetOrgInterface().GetAVFacedData(), true);
    setInternalValue(update_reg, m_world->GetStats().GetUpdate() - m_organism->GetOrgInterface().GetAVFacedDataUpdate(), true);
    setInternalValue(org_reg, m_organism->GetOrgInterface().GetAVFacedDataOrgID(), true);
    if (NUM_REGISTERS > 3) {
      const int group_reg = FindModifiedNextRegister(org_reg);
      setInternalValue(group_reg, m_organism->GetOrgInterface().GetAVFacedDataTerritory(), true);    
    }
  }
  return true;
}

//Teach offspring learned targeting/foraging behavior
bool cHardwareExperimental::Inst_TeachOffspring(cAvidaContext&)
{
  assert(m_organism != 0);
  m_organism->Teach(true);
  return true;
}

bool cHardwareExperimental::Inst_LearnParent(cAvidaContext&)
{
  assert(m_organism != 0);
  bool halt = false;
  if (m_organism->HadParentTeacher()) {
    int old_target = m_organism->GetForageTarget();
    int prop_target = -1;
    prop_target = m_organism->GetParentFT();

    halt = (prop_target == -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0);
    if (!halt) {
      if (m_use_avatar && m_organism->GetOrgInterface().GetAVCellID() != -1 && 
          ((prop_target == -2 && old_target != -2) || (prop_target != -2 && old_target == -2))) {
        m_organism->GetOrgInterface().SwitchPredPrey();
        m_organism->CopyParentFT();
      }
      else m_organism->CopyParentFT();
    }
  }
  return !halt;
}

bool cHardwareExperimental::Inst_SetGuard(cAvidaContext& ctx)
{
  bool set_ok = false;
  if (m_organism->GetPhenotype().GetTimeUsed() >= m_world->GetConfig().JUV_PERIOD.Get()) {
    Apto::Array<double> cell_res;
    if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
    else if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx); 
    const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
    for (int i = 0; i < cell_res.GetSize(); i++) {
      if ((resource_lib.GetResource(i)->GetHabitat() == 3 || resource_lib.GetResource(i)->GetHabitat() == 4) && cell_res[i] > 0) set_ok = true;;
    }
  }
    if (set_ok){
        m_organism->SetGuard();
        m_organism->IncGuard();
    }
  setInternalValue(FindModifiedRegister(rBX), (int) m_organism->IsGuard(), true);    
  return set_ok;
}

bool cHardwareExperimental::Inst_GetNumGuards(cAvidaContext& ctx)
{
  int num_guards = 0;
  bool on_den = false;
  Apto::Array<double> cell_res;
  if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx);
  else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if ((resource_lib.GetResource(i)->GetHabitat() == 3 || resource_lib.GetResource(i)->GetHabitat() == 4) && cell_res[i] > 0) on_den = true;
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

bool cHardwareExperimental::Inst_GetNumJuvs(cAvidaContext& ctx)
{
  int num_juvs = 0;
  int juv_age = m_world->GetConfig().JUV_PERIOD.Get();
  bool on_den = false;
  Apto::Array<double> cell_res;
  if (m_use_avatar) cell_res = m_organism->GetOrgInterface().GetAVResources(ctx);
  else if (!m_use_avatar) cell_res = m_organism->GetOrgInterface().GetResources(ctx);
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  for (int i = 0; i < cell_res.GetSize(); i++) {
    if ((resource_lib.GetResource(i)->GetHabitat() == 3 || resource_lib.GetResource(i)->GetHabitat() == 4) && cell_res[i] > 0) on_den = true;
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

bool cHardwareExperimental::Inst_SetGuardOnce(cAvidaContext& ctx)
{
  bool set_ok = false;
  if (!m_organism->IsGuard()) set_ok = Inst_SetGuard(ctx);
  setInternalValue(FindModifiedRegister(rBX), set_ok, true);    
  return set_ok;  
}



bool cHardwareExperimental::Inst_CheckFacedKin(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_use_avatar && m_use_avatar != 2) return false;  
  if (!m_use_avatar && !m_organism->IsNeighborCellOccupied()) return false;
  else if (m_use_avatar == 2 && !m_organism->GetOrgInterface().FacedHasAV()) return false;
  
  cOrganism* neighbor = NULL;
  if (!m_use_avatar) neighbor = m_organism->GetOrgInterface().GetNeighbor();
  else if (m_use_avatar == 2) neighbor = m_organism->GetOrgInterface().GetRandFacedAV();
  if (neighbor->IsDead())  return false;  
  
  // If there is no valid max genetic distance, go out to cousins.
  int gen_dist = GetRegister(FindModifiedRegister(rBX));
  if (gen_dist > 4 || gen_dist < 0) gen_dist = 4;
  
  bool is_kin = false;
  
  Systematics::GroupPtr bg = m_organism->SystematicsGroup("genotype");
  if (!bg) return false;
  Systematics::SexualAncestryPtr sa = bg->GetData<Systematics::SexualAncestry>();
  if (!sa) {
    sa = Systematics::SexualAncestryPtr(new Systematics::SexualAncestry(bg));
    bg->AttachData(sa);
  }
  
  Systematics::GroupPtr nbg = neighbor->SystematicsGroup("genotype");
  assert(nbg);
  if (sa->GetPhyloDistance(nbg) <= gen_dist) is_kin = true;
  
  setInternalValue(FindModifiedRegister(rBX), gen_dist, true);
  const int out_reg = FindModifiedNextRegister(rBX);   
  setInternalValue(out_reg, (int) is_kin, true);    
  return true;
}

bool cHardwareExperimental::Inst_ActivateDisplay(cAvidaContext& ctx)
{
  if (m_organism->GetOrgDisplayData() == NULL) return false;
  m_organism->ActivateDisplay();
  return true;
}

bool cHardwareExperimental::Inst_UpdateDisplay(cAvidaContext& ctx)
{
  return m_organism->UpdateOrgDisplay();
}

bool cHardwareExperimental::Inst_ModifyDisplay(cAvidaContext& ctx)
{
  sOrgDisplay* this_display = m_organism->GetOrgDisplayData();
  if (this_display == NULL) return false;
  cCPUMemory& memory = m_memory;
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

bool cHardwareExperimental::Inst_ReadLastSeenDisplay(cAvidaContext& ctx)
{
  if (!m_sensor.HasSeenDisplay()) return false;
  sOrgDisplay& last_seen = m_sensor.GetLastSeenDisplay();
  cCPUMemory& memory = m_memory;
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

bool cHardwareExperimental::Inst_ModifySimpDisplay(cAvidaContext& ctx)
{
  cCPUMemory& memory = m_memory;
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

bool cHardwareExperimental::Inst_ReadLastSimpDisplay(cAvidaContext& ctx)
{
  if (!m_sensor.HasSeenDisplay()) return false;
  sOrgDisplay& last_seen = m_sensor.GetLastSeenDisplay();
  cCPUMemory& memory = m_memory;
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

bool cHardwareExperimental::Inst_KillDisplay(cAvidaContext& ctx)
{
  if (!m_organism->IsDisplaying()) return false;
  m_organism->KillDisplay();
  return true;
}

/* Increases tolerance towards the addition of members to the group:
 nop-A: increases tolerance towards immigrants
 nop-B: increases tolerance towards own offspring
 nop-C: increases tolerance towards other offspring of the group.
 Removes the record of a previous update when dec-tolerance was executed,
 and places the modified tolerance total in the BX register. 
 */
bool cHardwareExperimental::Inst_IncPredTolerance(cAvidaContext& ctx)
{
   // Exit if the org is not a predator
   if (m_organism->GetForageTarget() != -2) return false;
   // Exit if tolerance is not enabled
   if (!m_world->GetConfig().USE_FORM_GROUPS.Get()) return false;
   if (m_world->GetConfig().TOLERANCE_WINDOW.Get() <= 0) return false;
   // Exit if organism is not in a group
   if (!m_organism->GetOrgInterface().HasOpinion(m_organism)) return false;
   // Exit if the instruction is not nop-modified
   if (!m_inst_set->IsNop(getIP().GetNextInst())) return false;
   
  int toleranceType = 0;
  if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 0) {
    const int tolerance_to_modify = FindModifiedRegister(rBX);
    
    toleranceType = -1;
    if (tolerance_to_modify == rAX) toleranceType = 0;
    else if (tolerance_to_modify == rBX) toleranceType = 1;
    else if (tolerance_to_modify == rCX) toleranceType = 2;
    
    // Not a recognized register
    if (toleranceType == -1) return false;
  }
   
   // Update the tolerance and store the result in register B
   int result = m_organism->GetOrgInterface().IncTolerance(toleranceType, ctx);   
   if (result == -1) return false;
  
   setInternalValue(rBX, result, true);
   return true;
}

/* Decreases tolerance towards the addition of members to the group,
 nop-A: decreases tolerance towards immigrants
 nop-B: decreases tolerance towards own offspring
 nop-C: decreases tolerance towards other offspring of the group.
 Adds to records the update during which dec-tolerance was executed,
 and places the modified tolerance total in the BX register. 
 */
bool cHardwareExperimental::Inst_DecPredTolerance(cAvidaContext& ctx)
{
  // Exit if the org is not a predator
  if (m_organism->GetForageTarget() != -2) return false;
  // Exit if tolerance is not enabled
  if (!m_world->GetConfig().USE_FORM_GROUPS.Get()) return false;
  if (m_world->GetConfig().TOLERANCE_WINDOW.Get() <= 0) return false;
  // Exit if organism is not in a group
  if (!m_organism->GetOrgInterface().HasOpinion(m_organism)) return false;
  // Exit if the instruction is not nop-modified
  if (!m_inst_set->IsNop(getIP().GetNextInst())) return false;
  
  int toleranceType = 0;
  if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 0) {
    const int tolerance_to_modify = FindModifiedRegister(rBX);
    
    toleranceType = -1;
    if (tolerance_to_modify == rAX) toleranceType = 0;
    else if (tolerance_to_modify == rBX) toleranceType = 1;
    else if (tolerance_to_modify == rCX) toleranceType = 2;
    
    // Not a recognized register
    if (toleranceType == -1) return false;
  }
  
  // Update the tolerance and store the result in register B
  setInternalValue(rBX, m_organism->GetOrgInterface().DecTolerance(toleranceType, ctx));
  return true;
}

/* Retrieve current tolerance levels, placing each tolerance in a different register.
 Register AX: tolerance towards immigrants
 Register BX: tolerance towards own offspring
 Register CX: tolerance towards other offspring in the group 
 */
bool cHardwareExperimental::Inst_GetPredTolerance(cAvidaContext& ctx)
{
  bool exec_success = false;
  if (m_organism->GetForageTarget() != -2) return false;
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() && m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
    if(m_organism->GetOrgInterface().HasOpinion(m_organism)) {
      if (m_organism->GetOpinion().first == -1) return false;
      m_organism->GetOrgInterface().PushToleranceInstExe(6, ctx);
      
      int tolerance_immigrants = m_organism->GetPhenotype().CalcToleranceImmigrants();
      int tolerance_own = m_organism->GetPhenotype().CalcToleranceOffspringOwn();
      int tolerance_others = m_organism->GetPhenotype().CalcToleranceOffspringOthers();
      setInternalValue(rAX, tolerance_immigrants, true);
      setInternalValue(rBX, tolerance_own, true);
      setInternalValue(rCX, tolerance_others, true);  
      exec_success = true;
    }
  }
  return exec_success;
}  

/* Retrieve group tolerances placing each in a different register.
 Register AX: group tolerance towards immigrants
 Register BX: group tolerance towards own offspring
 Register CX: group tolerance towards offspring 
 */
bool cHardwareExperimental::Inst_GetPredGroupTolerance(cAvidaContext& ctx)
{
  bool exec_success = false;
  // If not a predator in a group, return false
  if (m_organism->GetForageTarget() != -2 || m_organism->GetOpinion().first < 0) return false;
  // If groups are used and tolerances are on...
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() && m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
    if(m_organism->GetOrgInterface().HasOpinion(m_organism)) {
      m_organism->GetOrgInterface().PushToleranceInstExe(7, ctx);
      
      const int group_id = m_organism->GetOpinion().first;
      if (group_id == -1) return false;
      
      int mating_type = -1;
      if (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 2) {
        if (m_organism->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE) mating_type = 0;
        else if (m_organism->GetPhenotype().GetMatingType() == MATING_TYPE_MALE) mating_type = 1;
        else mating_type = 2;
      }
      double immigrant_odds = m_organism->GetOrgInterface().CalcGroupOddsImmigrants(group_id, mating_type);
      double offspring_own_odds = m_organism->GetOrgInterface().CalcGroupOddsOffspring(m_organism);
      double offspring_others_odds = m_organism->GetOrgInterface().CalcGroupOddsOffspring(group_id);
      
      // Convert all odds to percent
      double percent_immigrants = immigrant_odds * 100 + 0.5;
      double percent_offspring_own = offspring_own_odds * 100 + 0.5;
      double percent_offspring_others = offspring_others_odds * 100 + 0.5;
      
      // Truncate percent to integer and place in registers
      setInternalValue(rAX, (int) percent_immigrants, true);
      setInternalValue(rBX, (int) percent_offspring_own, true);
      setInternalValue(rCX, (int) percent_offspring_others, true);
      exec_success = true;
    }
  }
  return exec_success;
}

// Active messaging
bool cHardwareExperimental::Inst_SendMessageInterruptType0(cAvidaContext& ctx) { return SendMessage(ctx, 0); }
bool cHardwareExperimental::Inst_SendMessageInterruptType1(cAvidaContext& ctx) { return SendMessage(ctx, 1); }
bool cHardwareExperimental::Inst_SendMessageInterruptType2(cAvidaContext& ctx) { return SendMessage(ctx, 2); }
bool cHardwareExperimental::Inst_SendMessageInterruptType3(cAvidaContext& ctx) { return SendMessage(ctx, 3); }
bool cHardwareExperimental::Inst_SendMessageInterruptType4(cAvidaContext& ctx) { return SendMessage(ctx, 4); }
bool cHardwareExperimental::Inst_SendMessageInterruptType5(cAvidaContext& ctx) { return SendMessage(ctx, 5); }

// Jumps one instruction passed end-handler
bool cHardwareExperimental::Inst_START_Handler(cAvidaContext& ctx)
{
  m_advance_ip = false;
  // Jump 1 instruction passed msg-handler
  Instruction label_inst = GetInstSet().GetInst("end-handler");

  cHeadCPU search_head(IP());
  int start_pos = search_head.GetPosition();
  search_head++;

  while (start_pos != search_head.GetPosition()) {
    if (search_head.GetInst() == label_inst) {
      // move IP to here
      search_head++;
      IP().Set(search_head.GetPosition());
      return true;
    }
    search_head++;
  }
  return false;
}

bool cHardwareExperimental::Inst_End_Handler(cAvidaContext& ctx)
{
  bool exit_successful = ExitThread();
  if (exit_successful == false) { // return false if one thread exists
    m_organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
    // might need to set inst. advance to false
    return false;
  } // previous thread is now restored
  return true;
}

bool cHardwareExperimental::Inst_SendMessage(cAvidaContext& ctx)
{
  return SendMessage(ctx);
}

bool cHardwareExperimental::SendMessage(cAvidaContext& ctx, int messageType)
{
  const int label_reg = FindModifiedRegister(rBX);
  const int data_reg = FindNextRegister(label_reg);

  cOrgMessage msg = cOrgMessage(m_organism, messageType);
  msg.SetLabel(GetRegister(label_reg));
  msg.SetData(GetRegister(data_reg));

  return m_organism->SendMessage(ctx, msg);
}

bool cHardwareExperimental::Inst_RetrieveMessage(cAvidaContext& ctx)
{
  std::pair<bool, cOrgMessage> retrieved = m_organism->RetrieveMessage();
  if (!retrieved.first) {
    return false;
  }

  const int label_reg = FindModifiedRegister(rBX);
  const int data_reg = FindNextRegister(label_reg);

  setInternalValue(label_reg, retrieved.second.GetLabel(), true);
  setInternalValue(data_reg, retrieved.second.GetData(), true);
  return true;
}

bool cHardwareExperimental::Inst_ScrambleReg(cAvidaContext& ctx)
{
  for (int i = 0; i < NUM_REGISTERS; i++) {
    setInternalValue(rAX + i, (int) (ctx.GetRandom().GetDouble()), true);
  }
  return true;
}

void cHardwareExperimental::InjureOrg(cOrganism* target, double injury)
{
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

void cHardwareExperimental::MakePred()
{
  if (m_organism->GetForageTarget() != -2) { 
    // switching between predator and prey means having to switch avatar list...don't run this for orgs with AVCell == -1 (avatars off or test cpu)
    if (m_use_avatar && m_organism->GetOrgInterface().GetAVCellID() != -1) {
      m_organism->GetOrgInterface().SwitchPredPrey();
      m_organism->SetForageTarget(-2);
    }
    else m_organism->SetForageTarget(-2);
  }    
}

bool cHardwareExperimental::TestAttack(cAvidaContext& ctx)
{
  if (m_use_avatar && m_use_avatar != 2) return false;
  
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0) return false;
  
  if (!m_use_avatar && !m_organism->IsNeighborCellOccupied()) return false;
  else if (m_use_avatar == 2 && !m_organism->GetOrgInterface().FacedHasPreyAV()) return false;
  
  // prevent killing on refuges
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedCellResources(ctx)[i] > 0 && resource_lib.GetResource(i)->GetRefuge()) return false;
    else if (m_use_avatar == 2 && m_organism->GetOrgInterface().GetAVFacedResources(ctx)[i] > 0 && resource_lib.GetResource(i)->GetRefuge()) return false;
  }
  return true;
}
