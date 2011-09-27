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

#include "avida/core/WorldDriver.h"

#include "cAvidaContext.h"
#include "cBioGroup.h"
#include "cCPUTestInfo.h"
#include "cHardwareManager.h"
#include "cHardwareTracer.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cSexualAncestry.h"
#include "cStateGrid.h"
#include "cStringUtil.h"
#include "cTestCPU.h"
#include "cWorld.h"
#include "cPopulation.h"  //APW TODO -- get this out of the hardware (cur required for lookahead)

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
    tInstLibEntry<tMethod>("nop-A", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-B", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-C", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-D", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-E", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-F", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-G", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-H", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),

    tInstLibEntry<tMethod>("nop-I", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-J", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-K", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-L", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-M", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-N", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-O", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-P", &cHardwareExperimental::Inst_Nop, nInstFlag::NOP, "No-operation; modifies other instructions"),
    
    tInstLibEntry<tMethod>("NULL", &cHardwareExperimental::Inst_Nop, 0, "True no-operation instruction: does nothing"),
    tInstLibEntry<tMethod>("nop-X", &cHardwareExperimental::Inst_Nop, 0, "True no-operation instruction: does nothing"),

    
    // Threading 
    tInstLibEntry<tMethod>("fork-thread", &cHardwareExperimental::Inst_ForkThread),
    tInstLibEntry<tMethod>("exit-thread", &cHardwareExperimental::Inst_ExitThread),
    tInstLibEntry<tMethod>("id-thread", &cHardwareExperimental::Inst_IdThread),

    
    // Standard Conditionals
    tInstLibEntry<tMethod>("if-n-equ", &cHardwareExperimental::Inst_IfNEqu, 0, "Execute next instruction if ?BX?!=?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-less", &cHardwareExperimental::Inst_IfLess, 0, "Execute next instruction if ?BX? < ?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-not-0", &cHardwareExperimental::Inst_IfNotZero, 0, "Execute next instruction if ?BX? != 0, else skip it"),
    tInstLibEntry<tMethod>("if-equ-0", &cHardwareExperimental::Inst_IfEqualZero, 0, "Execute next instruction if ?BX? == 0, else skip it"),
    tInstLibEntry<tMethod>("if-gtr-0", &cHardwareExperimental::Inst_IfGreaterThanZero, 0, "Execute next instruction if ?BX? > 0, else skip it"),
    tInstLibEntry<tMethod>("if-less-0", &cHardwareExperimental::Inst_IfLessThanZero, 0, "Execute next instruction if ?BX? < 0, else skip it"),
    tInstLibEntry<tMethod>("if-gtr-x", &cHardwareExperimental::Inst_IfGtrX),
    tInstLibEntry<tMethod>("if-equ-x", &cHardwareExperimental::Inst_IfEquX),

    tInstLibEntry<tMethod>("if-cons", &cHardwareExperimental::Inst_IfConsensus, 0, "Execute next instruction if ?BX? in consensus, else skip it"),
    tInstLibEntry<tMethod>("if-cons-24", &cHardwareExperimental::Inst_IfConsensus24, 0, "Execute next instruction if ?BX[0:23]? in consensus , else skip it"),
    tInstLibEntry<tMethod>("if-less-cons", &cHardwareExperimental::Inst_IfLessConsensus, 0, "Execute next instruction if Count(?BX?) < Count(?CX?), else skip it"),
    tInstLibEntry<tMethod>("if-less-cons-24", &cHardwareExperimental::Inst_IfLessConsensus24, 0, "Execute next instruction if Count(?BX[0:23]?) < Count(?CX[0:23]?), else skip it"),

    tInstLibEntry<tMethod>("if-stk-gtr", &cHardwareExperimental::Inst_IfStackGreater, 0, "Execute next instruction if the top of the current stack > inactive stack, else skip it"),

    // Core ALU Operations
    tInstLibEntry<tMethod>("pop", &cHardwareExperimental::Inst_Pop, 0, "Remove top number from stack and place into ?BX?"),
    tInstLibEntry<tMethod>("push", &cHardwareExperimental::Inst_Push, 0, "Copy number from ?BX? and place it into the stack"),
    tInstLibEntry<tMethod>("swap-stk", &cHardwareExperimental::Inst_SwitchStack, 0, "Toggle which stack is currently being used"),
    tInstLibEntry<tMethod>("swap-stk-top", &cHardwareExperimental::Inst_SwapStackTop, 0, "Swap the values at the top of both stacks"),
    tInstLibEntry<tMethod>("swap", &cHardwareExperimental::Inst_Swap, 0, "Swap the contents of ?BX? with ?CX?"),
    
    tInstLibEntry<tMethod>("shift-r", &cHardwareExperimental::Inst_ShiftR, 0, "Shift bits in ?BX? right by one (divide by two)"),
    tInstLibEntry<tMethod>("shift-l", &cHardwareExperimental::Inst_ShiftL, 0, "Shift bits in ?BX? left by one (multiply by two)"),
    tInstLibEntry<tMethod>("inc", &cHardwareExperimental::Inst_Inc, 0, "Increment ?BX? by one"),
    tInstLibEntry<tMethod>("dec", &cHardwareExperimental::Inst_Dec, 0, "Decrement ?BX? by one"),
    tInstLibEntry<tMethod>("zero", &cHardwareExperimental::Inst_Zero, 0, "Set ?BX? to 0"),
    tInstLibEntry<tMethod>("mult100", &cHardwareExperimental::Inst_Mult100, 0, "Mult ?BX? by 100"),

    tInstLibEntry<tMethod>("add", &cHardwareExperimental::Inst_Add, 0, "Add BX to CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("sub", &cHardwareExperimental::Inst_Sub, 0, "Subtract CX from BX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("nand", &cHardwareExperimental::Inst_Nand, 0, "Nand BX by CX and place the result in ?BX?"),
    
    tInstLibEntry<tMethod>("IO", &cHardwareExperimental::Inst_TaskIO, nInstFlag::STALL, "Output ?BX?, and input new number back into ?BX?"),
    tInstLibEntry<tMethod>("IO-expire", &cHardwareExperimental::Inst_TaskIOExpire, nInstFlag::STALL, "Output ?BX?, and input new number back into ?BX?, if the number has not yet expired"),
    tInstLibEntry<tMethod>("input", &cHardwareExperimental::Inst_TaskInput, nInstFlag::STALL, "Input new number into ?BX?"),
    tInstLibEntry<tMethod>("output", &cHardwareExperimental::Inst_TaskOutput, nInstFlag::STALL, "Output ?BX?"),
    tInstLibEntry<tMethod>("output-zero", &cHardwareExperimental::Inst_TaskOutputZero, nInstFlag::STALL, "Output ?BX?"),
    tInstLibEntry<tMethod>("output-expire", &cHardwareExperimental::Inst_TaskOutputExpire, nInstFlag::STALL, "Output ?BX?, as long as the output has not yet expired"),
    
    tInstLibEntry<tMethod>("mult", &cHardwareExperimental::Inst_Mult, 0, "Multiple BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("div", &cHardwareExperimental::Inst_Div, 0, "Divide BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("mod", &cHardwareExperimental::Inst_Mod),
    
    
    // Flow Control Instructions
    tInstLibEntry<tMethod>("label", &cHardwareExperimental::Inst_Label, nInstFlag::LABEL),
    
    tInstLibEntry<tMethod>("search-lbl-comp-s", &cHardwareExperimental::Inst_Search_Label_Comp_S, 0, "Find complement label from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-comp-f", &cHardwareExperimental::Inst_Search_Label_Comp_F, 0, "Find complement label forward and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-comp-b", &cHardwareExperimental::Inst_Search_Label_Comp_B, 0, "Find complement label backward and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-direct-s", &cHardwareExperimental::Inst_Search_Label_Direct_S, 0, "Find direct label from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-direct-f", &cHardwareExperimental::Inst_Search_Label_Direct_F, 0, "Find direct label forward and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-direct-b", &cHardwareExperimental::Inst_Search_Label_Direct_B, 0, "Find direct label backward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-comp-s", &cHardwareExperimental::Inst_Search_Seq_Comp_S, 0, "Find complement template from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-comp-f", &cHardwareExperimental::Inst_Search_Seq_Comp_F, 0, "Find complement template forward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-comp-b", &cHardwareExperimental::Inst_Search_Seq_Comp_B, 0, "Find complement template backward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-direct-s", &cHardwareExperimental::Inst_Search_Seq_Direct_S, 0, "Find direct template from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-direct-f", &cHardwareExperimental::Inst_Search_Seq_Direct_F, 0, "Find direct template forward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-direct-b", &cHardwareExperimental::Inst_Search_Seq_Direct_B, 0, "Find direct template backward and move the flow head"),

    tInstLibEntry<tMethod>("mov-head", &cHardwareExperimental::Inst_MoveHead, 0, "Move head ?IP? to the flow head"),
    tInstLibEntry<tMethod>("mov-head-if-n-equ", &cHardwareExperimental::Inst_MoveHeadIfNEqu, 0, "Move head ?IP? to the flow head if ?BX? != ?CX?"),
    tInstLibEntry<tMethod>("mov-head-if-less", &cHardwareExperimental::Inst_MoveHeadIfLess, 0, "Move head ?IP? to the flow head if ?BX? != ?CX?"),

    tInstLibEntry<tMethod>("goto", &cHardwareExperimental::Inst_Goto, 0, "Move IP to labeled position matching the label that follows"),
    tInstLibEntry<tMethod>("goto-if-n-equ", &cHardwareExperimental::Inst_GotoIfNEqu, 0, "Move IP to labeled position if BX != CX"),
    tInstLibEntry<tMethod>("goto-if-less", &cHardwareExperimental::Inst_GotoIfLess, 0, "Move IP to labeled position if BX < CX"),
    tInstLibEntry<tMethod>("goto-if-cons", &cHardwareExperimental::Inst_GotoConsensus, 0, "Move IP to the labeled position if BX consensus"), 
    tInstLibEntry<tMethod>("goto-if-cons-24", &cHardwareExperimental::Inst_GotoConsensus24, 0, "Move IP to the labeled position if BX consensus"),
    
    tInstLibEntry<tMethod>("jmp-head", &cHardwareExperimental::Inst_JumpHead, 0, "Move head ?Flow? by amount in ?CX? register"),
    tInstLibEntry<tMethod>("get-head", &cHardwareExperimental::Inst_GetHead, 0, "Copy the position of the ?IP? head into ?CX?"),
    

    // Replication Instructions
    tInstLibEntry<tMethod>("h-alloc", &cHardwareExperimental::Inst_HeadAlloc, 0, "Allocate maximum allowed space"),
    tInstLibEntry<tMethod>("h-divide", &cHardwareExperimental::Inst_HeadDivide, nInstFlag::STALL, "Divide code between read and write heads."),
    tInstLibEntry<tMethod>("h-divide-sex", &cHardwareExperimental::Inst_HeadDivideSex, nInstFlag::STALL, "Divide code between read and write heads."),
    tInstLibEntry<tMethod>("h-copy", &cHardwareExperimental::Inst_HeadCopy, 0, "Copy from read-head to write-head; advance both"),
    tInstLibEntry<tMethod>("h-reqd", &cHardwareExperimental::Inst_HeadRead, 0, "Read instruction from ?read-head? to ?AX?; advance the head."),
    tInstLibEntry<tMethod>("h-write", &cHardwareExperimental::Inst_HeadWrite, 0, "Write to ?write-head? instruction from ?AX?; advance the head."),
    tInstLibEntry<tMethod>("if-copied-lbl-comp", &cHardwareExperimental::Inst_IfCopiedCompLabel, 0, "Execute next if we copied complement of attached label"),
    tInstLibEntry<tMethod>("if-copied-lbl-direct", &cHardwareExperimental::Inst_IfCopiedDirectLabel, 0, "Execute next if we copied direct match of the attached label"),
    tInstLibEntry<tMethod>("if-copied-seq-comp", &cHardwareExperimental::Inst_IfCopiedCompSeq, 0, "Execute next if we copied complement of attached sequence"),
    tInstLibEntry<tMethod>("if-copied-seq-direct", &cHardwareExperimental::Inst_IfCopiedDirectSeq, 0, "Execute next if we copied direct match of the attached sequence"),

    tInstLibEntry<tMethod>("repro", &cHardwareExperimental::Inst_Repro, nInstFlag::STALL, "Instantly reproduces the organism"),

    tInstLibEntry<tMethod>("die", &cHardwareExperimental::Inst_Die, nInstFlag::STALL, "Instantly kills the organism"),

    // Thread Execution Control
    tInstLibEntry<tMethod>("wait-cond-equ", &cHardwareExperimental::Inst_WaitCondition_Equal, nInstFlag::STALL, ""),
    tInstLibEntry<tMethod>("wait-cond-less", &cHardwareExperimental::Inst_WaitCondition_Less, nInstFlag::STALL, ""),
    tInstLibEntry<tMethod>("wait-cond-gtr", &cHardwareExperimental::Inst_WaitCondition_Greater, nInstFlag::STALL, ""),
    
    
    // Promoter Model
    tInstLibEntry<tMethod>("promoter", &cHardwareExperimental::Inst_Promoter, nInstFlag::PROMOTER),
    tInstLibEntry<tMethod>("terminate", &cHardwareExperimental::Inst_Terminate),
    tInstLibEntry<tMethod>("term-cons", &cHardwareExperimental::Inst_TerminateConsensus),
    tInstLibEntry<tMethod>("term-cons-24", &cHardwareExperimental::Inst_TerminateConsensus24),
    tInstLibEntry<tMethod>("regulate", &cHardwareExperimental::Inst_Regulate),
    tInstLibEntry<tMethod>("regulate-sp", &cHardwareExperimental::Inst_RegulateSpecificPromoters),
    tInstLibEntry<tMethod>("s-regulate", &cHardwareExperimental::Inst_SenseRegulate),
    tInstLibEntry<tMethod>("numberate", &cHardwareExperimental::Inst_Numberate),
    tInstLibEntry<tMethod>("numberate-24", &cHardwareExperimental::Inst_Numberate24),
    tInstLibEntry<tMethod>("bit-cons", &cHardwareExperimental::Inst_BitConsensus),
    tInstLibEntry<tMethod>("bit-cons-24", &cHardwareExperimental::Inst_BitConsensus24),
    tInstLibEntry<tMethod>("execurate", &cHardwareExperimental::Inst_Execurate),
    tInstLibEntry<tMethod>("execurate-24", &cHardwareExperimental::Inst_Execurate24),

    
    // State Grid instructions
    tInstLibEntry<tMethod>("sg-move", &cHardwareExperimental::Inst_SGMove),
    tInstLibEntry<tMethod>("sg-rotate-l", &cHardwareExperimental::Inst_SGRotateL),
    tInstLibEntry<tMethod>("sg-rotate-r", &cHardwareExperimental::Inst_SGRotateR),
    tInstLibEntry<tMethod>("sg-sense", &cHardwareExperimental::Inst_SGSense),

      
    // Movement and Navigation instructions
    tInstLibEntry<tMethod>("move", &cHardwareExperimental::Inst_Move),
    tInstLibEntry<tMethod>("range-move", &cHardwareExperimental::Inst_RangeMove),
    tInstLibEntry<tMethod>("range-pred-move", &cHardwareExperimental::Inst_RangeMove),
    tInstLibEntry<tMethod>("get-north-offset", &cHardwareExperimental::Inst_GetNorthOffset),    
    tInstLibEntry<tMethod>("get-position-offset", &cHardwareExperimental::Inst_GetPositionOffset),    
    tInstLibEntry<tMethod>("get-northerly", &cHardwareExperimental::Inst_GetNortherly),    
    tInstLibEntry<tMethod>("get-easterly", &cHardwareExperimental::Inst_GetEasterly), 
    tInstLibEntry<tMethod>("zero-easterly", &cHardwareExperimental::Inst_ZeroEasterly),    
    tInstLibEntry<tMethod>("zero-northerly", &cHardwareExperimental::Inst_ZeroNortherly),    
    
    // Rotation
    tInstLibEntry<tMethod>("rotate-left-one", &cHardwareExperimental::Inst_RotateLeftOne, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-right-one", &cHardwareExperimental::Inst_RotateRightOne, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-uphill", &cHardwareExperimental::Inst_RotateUphill, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-home", &cHardwareExperimental::Inst_RotateHome, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-to-unoccupied-cell", &cHardwareExperimental::Inst_RotateUnoccupiedCell, nInstFlag::STALL),
    tInstLibEntry<tMethod>("rotate-x", &cHardwareExperimental::Inst_RotateX, nInstFlag::STALL),

      
    // Resource and Topography Sensing
    tInstLibEntry<tMethod>("sense-resource-id", &cHardwareExperimental::Inst_SenseResourceID, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("sense-res-quant", &cHardwareExperimental::Inst_SenseResQuant, nInstFlag::STALL),
    tInstLibEntry<tMethod>("sense-nest", &cHardwareExperimental::Inst_SenseNest, nInstFlag::STALL),
    tInstLibEntry<tMethod>("sense-res-diff", &cHardwareExperimental::Inst_SenseResDiff, nInstFlag::STALL),
    tInstLibEntry<tMethod>("sense-faced-habitat", &cHardwareExperimental::Inst_SenseFacedHabitat, nInstFlag::STALL),
    tInstLibEntry<tMethod>("look-ahead", &cHardwareExperimental::Inst_LookAhead, nInstFlag::STALL),
    tInstLibEntry<tMethod>("set-forage-target", &cHardwareExperimental::Inst_SetForageTarget, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-forage-target", &cHardwareExperimental::Inst_GetForageTarget),
    tInstLibEntry<tMethod>("sense-opinion-resource-quantity", &cHardwareExperimental::Inst_SenseOpinionResQuant, nInstFlag::STALL), //APW delete after hrdwr experiments
    tInstLibEntry<tMethod>("sense-diff-faced", &cHardwareExperimental::Inst_SenseDiffFaced, nInstFlag::STALL),  //APW delete after hrdwr experiments

     
    // Grouping instructions
    tInstLibEntry<tMethod>("join-group", &cHardwareExperimental::Inst_JoinGroup, nInstFlag::STALL),
    tInstLibEntry<tMethod>("change-pred-group", &cHardwareExperimental::Inst_ChangePredGroup, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-group-id", &cHardwareExperimental::Inst_GetGroupID),
    tInstLibEntry<tMethod>("get-pred-group-id", &cHardwareExperimental::Inst_GetPredGroupID),
    tInstLibEntry<tMethod>("inc-pred-tolerance", &cHardwareExperimental::Inst_IncPredTolerance, nInstFlag::STALL),  // @JJB
    tInstLibEntry<tMethod>("dec-pred-tolerance", &cHardwareExperimental::Inst_DecPredTolerance, nInstFlag::STALL),  // @JJB
    tInstLibEntry<tMethod>("get-pred-tolerance", &cHardwareExperimental::Inst_GetPredTolerance, nInstFlag::STALL),  // @JJB    
    tInstLibEntry<tMethod>("get-pred-group-tolerance", &cHardwareExperimental::Inst_GetPredGroupTolerance, nInstFlag::STALL),  // @JJB  

    // Org Interaction instructions
    tInstLibEntry<tMethod>("get-faced-org-id", &cHardwareExperimental::Inst_GetFacedOrgID, nInstFlag::STALL),
    tInstLibEntry<tMethod>("attack-merit-prey", &cHardwareExperimental::Inst_AttackMeritPrey, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("fight-merit-org", &cHardwareExperimental::Inst_FightMeritOrg, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("mark-cell", &cHardwareExperimental::Inst_MarkCell, nInstFlag::STALL),
    tInstLibEntry<tMethod>("mark-pred-cell", &cHardwareExperimental::Inst_MarkPredCell, nInstFlag::STALL),
    tInstLibEntry<tMethod>("read-faced-cell", &cHardwareExperimental::Inst_ReadFacedCell, nInstFlag::STALL),
    tInstLibEntry<tMethod>("read-faced-pred-cell", &cHardwareExperimental::Inst_ReadFacedPredCell, nInstFlag::STALL),
    tInstLibEntry<tMethod>("get-merit-fight-odds", &cHardwareExperimental::Inst_GetMeritFightOdds, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("fight-org", &cHardwareExperimental::Inst_FightOrg, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("fight-pred", &cHardwareExperimental::Inst_FightPred, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("fight-merit-pred", &cHardwareExperimental::Inst_FightMeritPred, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("teach-offspring", &cHardwareExperimental::Inst_TeachOffspring, nInstFlag::STALL), 
    tInstLibEntry<tMethod>("check-faced-kin", &cHardwareExperimental::Inst_CheckFacedKin, nInstFlag::STALL), 

    // DEPRECATED Instructions
    tInstLibEntry<tMethod>("set-flow", &cHardwareExperimental::Inst_SetFlow, 0, "Set flow-head to position in ?CX?")
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
  : cHardwareBase(world, in_organism, in_inst_set)
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
  
  m_memory = in_organism->GetGenome().GetSequence();  // Initialize memory...
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
}


void cHardwareExperimental::internalResetOnFailedDivide()
{
	internalReset();
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
  assert(m_cycle_count < 0x8000);
  phenotype.IncCPUCyclesUsed();
  if (!m_no_cpu_cycle_time) phenotype.IncTimeUsed();

  // If we have threads turned on and we executed each thread in a single
  // timestep, adjust the number of instructions executed accordingly.
  const int num_inst_exec = (m_world->GetConfig().THREAD_SLICING_METHOD.Get() == 1) ? m_threads.GetSize() : 1;
  
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
    
    // BREAKPOINTS
#if 0
    if (ip.FlagBreakpoint()) {
      m_organism->DoBreakpoint();
    }
#endif
    
    // Print the status of this CPU at each step...    
    if (m_tracer != NULL) m_tracer->TraceHardware(*this);
    
    // Find the instruction to be executed
    const cInstruction& cur_inst = ip.GetInst();
    
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
    if (m_has_any_costs) exec = SingleProcess_PayPreCosts(ctx, cur_inst);

    if (m_promoters_enabled) {
      // Constitutive regulation applied here
      if (m_constitutive_regulation) Inst_SenseRegulate(ctx); 

      // If there are no active promoters and a certain mode is set, then don't execute any further instructions
      if (m_no_active_promoter_halt && m_promoter_index == -1) exec = false;
    }
    
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
      
      //Add to the promoter inst executed count before executing the inst (in case it is a terminator)
      if (m_promoters_enabled) m_threads[m_cur_thread].IncPromoterInstExecuted();
      
      if (exec == true) if (SingleProcess_ExecuteInst(ctx, cur_inst)) SingleProcess_PayPostCosts(ctx, cur_inst);

      // Check if the instruction just executed caused premature death, break out of execution if so
      if (phenotype.GetToDelete()) break;

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
    } // if exec
    
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
bool cHardwareExperimental::SingleProcess_ExecuteInst(cAvidaContext& ctx, const cInstruction& cur_inst) 
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


void cHardwareExperimental::ProcessBonusInst(cAvidaContext& ctx, const cInstruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = m_organism->IsRunning();
  m_organism->SetRunning(true);
  
  if (m_tracer != NULL) m_tracer->TraceHardware(*this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  m_organism->SetRunning(prev_run_state);
}


void cHardwareExperimental::PrintStatus(ostream& fp)
{
  fp << m_organism->GetPhenotype().GetCPUCyclesUsed() << " ";
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





void cHardwareExperimental::ReadInst(cInstruction in_inst)
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
  if ((m_threads.GetSize() == 1) || (m_waiting_threads == (m_threads.GetSize() - 1))) return false;
  
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

bool cHardwareExperimental::Inst_IdThread(cAvidaContext& ctx)
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
  m_organism->OffspringGenome().SetSequence(m_memory.Crop(div_point, div_point+child_size));
  m_organism->OffspringGenome().SetHardwareType(GetType());
  m_organism->OffspringGenome().SetInstSet(m_inst_set->GetInstSetName());
  
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
    m_inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
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
    if (i != cur_thread && !m_threads[i].active && m_threads[i].wait_reg == reg_num) {
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
bool cHardwareExperimental::Inst_ForkThread(cAvidaContext& ctx)
{
    getIP().Advance();
    if (!ForkThread()) m_organism->Fault(FAULT_LOC_THREAD_FORK, FAULT_TYPE_FORK_TH);
    return true;
}

bool cHardwareExperimental::Inst_ExitThread(cAvidaContext& ctx)
{
    if (!ExitThread()) m_organism->Fault(FAULT_LOC_THREAD_KILL, FAULT_TYPE_KILL_TH);
    else m_advance_ip = false;
    return true;
}

bool cHardwareExperimental::Inst_IfNEqu(cAvidaContext& ctx) // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) == GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLess(cAvidaContext& ctx) // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegister(op1) >=  GetRegister(op2))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfNotZero(cAvidaContext& ctx)  // Execute next if ?bx? != 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) == 0)  getIP().Advance();
  return true;
}
bool cHardwareExperimental::Inst_IfEqualZero(cAvidaContext& ctx)  // Execute next if ?bx? == 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) != 0)  getIP().Advance();
  return true;
}
bool cHardwareExperimental::Inst_IfGreaterThanZero(cAvidaContext& ctx)  // Execute next if ?bx? > 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) <= 0)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLessThanZero(cAvidaContext& ctx)  // Execute next if ?bx? < 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegister(op1) >= 0)  getIP().Advance();
  return true;
}


bool cHardwareExperimental::Inst_IfGtrX(cAvidaContext& ctx)       // Execute next if BX > X; X value set according to NOP label
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

bool cHardwareExperimental::Inst_IfEquX(cAvidaContext& ctx)       // Execute next if BX == X; X value set according to NOP label
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



bool cHardwareExperimental::Inst_IfConsensus(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(rBX);
  if (BitCount(GetRegister(op1)) <  CONSENSUS)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfConsensus24(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(rBX);
  if (BitCount(GetRegister(op1) & MASK24) <  CONSENSUS24)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLessConsensus(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1)) >=  BitCount(GetRegister(op2)))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfLessConsensus24(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (BitCount(GetRegister(op1) & MASK24) >=  BitCount(GetRegister(op2) & MASK24))  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfStackGreater(cAvidaContext& ctx)
{
  int cur_stack = m_threads[m_cur_thread].cur_stack;
  if (getStack(cur_stack).Peek().value <=  getStack(!cur_stack).Peek().value)  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_Label(cAvidaContext& ctx)
{
  ReadLabel();
  return true;
};


bool cHardwareExperimental::Inst_Pop(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  sInternalValue pop = stackPop();
  setInternalValue(reg_used, pop.value, pop);
  return true;
}

bool cHardwareExperimental::Inst_Push(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  getStack(m_threads[m_cur_thread].cur_stack).Push(m_threads[m_cur_thread].reg[reg_used]);
  return true;
}


bool cHardwareExperimental::Inst_SwitchStack(cAvidaContext& ctx) { switchStack(); return true;}

bool cHardwareExperimental::Inst_SwapStackTop(cAvidaContext& ctx)
{
  sInternalValue v0 = getStack(0).Pop();
  sInternalValue v1 = getStack(1).Pop();
  getStack(0).Push(v1);
  getStack(1).Push(v0);
  return true;
}

bool cHardwareExperimental::Inst_Swap(cAvidaContext& ctx)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue v1 = m_threads[m_cur_thread].reg[op1];
  m_threads[m_cur_thread].reg[op1] = m_threads[m_cur_thread].reg[op2];
  m_threads[m_cur_thread].reg[op2] = v1;
  return true;
}

bool cHardwareExperimental::Inst_ShiftR(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value >> 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareExperimental::Inst_ShiftL(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value << 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}


bool cHardwareExperimental::Inst_Inc(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value + 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareExperimental::Inst_Dec(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value - 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareExperimental::Inst_Zero(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, 0, false);
  return true;
}

bool cHardwareExperimental::Inst_Mult100(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value * 100, false);
  return true;
}

bool cHardwareExperimental::Inst_Add(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value + r2.value, r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_Sub(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value - r2.value, r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_Mult(cAvidaContext& ctx)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value * r2.value, r1, r2);
  return true;
}

bool cHardwareExperimental::Inst_Div(cAvidaContext& ctx)
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

bool cHardwareExperimental::Inst_Mod(cAvidaContext& ctx)
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


bool cHardwareExperimental::Inst_Nand(cAvidaContext& ctx)
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


bool cHardwareExperimental::Inst_TaskInput(cAvidaContext& ctx)
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


bool cHardwareExperimental::Inst_MoveHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int target = FindModifiedHead(nHardware::HEAD_FLOW);
  getHead(head_used).Set(getHead(target));
  if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  return true;
}

bool cHardwareExperimental::Inst_MoveHeadIfNEqu(cAvidaContext& ctx)
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

bool cHardwareExperimental::Inst_MoveHeadIfLess(cAvidaContext& ctx)
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


bool cHardwareExperimental::Inst_Goto(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}

bool cHardwareExperimental::Inst_GotoIfNEqu(cAvidaContext& ctx)
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

bool cHardwareExperimental::Inst_GotoIfLess(cAvidaContext& ctx)
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


bool cHardwareExperimental::Inst_GotoConsensus(cAvidaContext& ctx)
{
  if (BitCount(GetRegister(rBX)) < CONSENSUS) return true;
  
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}

bool cHardwareExperimental::Inst_GotoConsensus24(cAvidaContext& ctx)
{
  if (BitCount(GetRegister(rBX) & MASK24) < CONSENSUS24) return true;
  
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getIP().Set(found_pos);
  return true;
}


bool cHardwareExperimental::Inst_JumpHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int reg = FindModifiedRegister(rCX);
  getHead(head_used).Jump(m_threads[m_cur_thread].reg[reg].value);
  if (head_used == nHardware::HEAD_IP) m_advance_ip = false;
  return true;
}

bool cHardwareExperimental::Inst_GetHead(cAvidaContext& ctx)
{
  const int head_used = FindModifiedHead(nHardware::HEAD_IP);
  const int reg = FindModifiedRegister(rCX);
  setInternalValue(reg, getHead(head_used).GetPosition());
  return true;
}

bool cHardwareExperimental::Inst_IfCopiedCompLabel(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfCopiedDirectLabel(cAvidaContext& ctx)
{
  ReadLabel();
  if (GetLabel() != GetReadLabel())  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfCopiedCompSeq(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  if (GetLabel() != GetReadSequence())  getIP().Advance();
  return true;
}

bool cHardwareExperimental::Inst_IfCopiedDirectSeq(cAvidaContext& ctx)
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
  cInstruction read_inst;
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
  
  active_head.SetInst(cInstruction(value));
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
  cInstruction read_inst = read_head.GetInst();
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

bool cHardwareExperimental::Inst_Search_Label_Comp_S(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Comp_F(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Comp_B(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindLabelBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Direct_S(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Direct_F(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Label_Direct_B(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Comp_S(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Comp_F(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Comp_B(cAvidaContext& ctx)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Direct_S(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceStart(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Direct_F(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceForward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}

bool cHardwareExperimental::Inst_Search_Seq_Direct_B(cAvidaContext& ctx)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceBackward(true);
  getHead(nHardware::HEAD_FLOW).Set(found_pos);
  getHead(nHardware::HEAD_FLOW).Advance();
  return true;
}


bool cHardwareExperimental::Inst_SetFlow(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rCX);
  getHead(nHardware::HEAD_FLOW).Set(GetRegister(reg_used));
  return true; 
}



bool cHardwareExperimental::Inst_WaitCondition_Equal(cAvidaContext& ctx)
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
  if (m_waiting_threads == (m_threads.GetSize() - 1)) return false;
  
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

bool cHardwareExperimental::Inst_WaitCondition_Less(cAvidaContext& ctx)
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
  if (m_waiting_threads == (m_threads.GetSize() - 1)) return false;
  
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

bool cHardwareExperimental::Inst_WaitCondition_Greater(cAvidaContext& ctx)
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
  if (m_waiting_threads == (m_threads.GetSize() - 1)) return false;
  
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

bool cHardwareExperimental::Inst_Promoter(cAvidaContext& ctx)
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
bool cHardwareExperimental::Inst_Regulate(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  int regulation_code = GetRegister(reg_used);
  
  for (int i = 0; i < m_promoters.GetSize(); i++) m_promoters[i].regulation = regulation_code;

  return true;
}

// Set a new regulation code, but only on a subset of promoters.
bool cHardwareExperimental::Inst_RegulateSpecificPromoters(cAvidaContext& ctx)
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
  const tArray<double> & res_count = m_organism->GetOrgInterface().GetResources(ctx); 
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
bool cHardwareExperimental::Do_Numberate(cAvidaContext& ctx, int num_bits)
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

bool cHardwareExperimental::Inst_BitConsensus(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  const int op1 = FindModifiedNextRegister(reg_used);
  sInternalValue& val = m_threads[m_cur_thread].reg[op1];

  setInternalValue(reg_used, (BitCount(val.value) >= CONSENSUS) ? 1 : 0, val); 
  return true; 
}

// Looks at only the lower 24 bits
bool cHardwareExperimental::Inst_BitConsensus24(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  const int op1 = FindModifiedNextRegister(reg_used);
  sInternalValue& val = m_threads[m_cur_thread].reg[op1];
  
  setInternalValue(reg_used, (BitCount(val.value & MASK24) >= CONSENSUS24) ? 1 : 0, val); 
  return true; 
}

// Create a number from inst bit codes of the previously executed instructions
bool cHardwareExperimental::Inst_Execurate(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_threads[m_cur_thread].GetExecurate());
  return true;
}

// Create a number from inst bit codes of the previously executed instructions, truncated to 24 bits
bool cHardwareExperimental::Inst_Execurate24(cAvidaContext& ctx)
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
  m_organism->OffspringGenome().SetSequence(m_memory);
  m_organism->OffspringGenome().SetHardwareType(GetType());
  m_organism->OffspringGenome().SetInstSet(m_inst_set->GetInstSetName());
  m_organism->GetPhenotype().SetLinesCopied(m_memory.GetSize());

  int lines_executed = 0;
  for (int i = 0; i < m_memory.GetSize(); i++) if (m_memory.FlagExecuted(i)) lines_executed++;
  m_organism->GetPhenotype().SetLinesExecuted(lines_executed);
  
  
  // Perform Copy Mutations...
  if (m_organism->GetCopyMutProb() > 0) { // Skip this if no mutations....
    for (int i = 0; i < m_memory.GetSize(); i++) {
      if (m_organism->TestCopyMut(ctx)) m_organism->OffspringGenome().GetSequence()[i] = m_inst_set->GetRandomInst(ctx);
    }
  }
  
  // Handle Divide Mutations...
  Divide_DoMutations(ctx);

  const bool viable = Divide_CheckViable(ctx, m_organism->GetGenome().GetSize(), m_organism->OffspringGenome().GetSize(), 1);
  if (viable == false) return false;

  // Many tests will require us to run the offspring through a test CPU;
  // this is, for example, to see if mutations need to be reverted or if
  // lineages need to be updated.
  Divide_TestFitnessMeasures(ctx);
  
  // reset first time instruction costs
  for (int i = 0; i < m_inst_ft_cost.GetSize(); i++) {
    m_inst_ft_cost[i] = m_inst_set->GetFTCost(cInstruction(i));
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


bool cHardwareExperimental::Inst_SGMove(cAvidaContext& ctx)
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

bool cHardwareExperimental::Inst_SGRotateL(cAvidaContext& ctx)
{
  assert(m_ext_mem.GetSize() > 3);
  if (--m_ext_mem[2] < 0) m_ext_mem[2] = 7;
  return true;
}

bool cHardwareExperimental::Inst_SGRotateR(cAvidaContext& ctx)
{
  assert(m_ext_mem.GetSize() > 3);
  if (++m_ext_mem[2] > 7) m_ext_mem[2] = 0;
  return true;
}

bool cHardwareExperimental::Inst_SGSense(cAvidaContext& ctx)
{
  const cStateGrid& sg = m_organism->GetStateGrid();
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, sg.SenseStateAt(m_ext_mem[0], m_ext_mem[1]));
  return true; 
}

bool cHardwareExperimental::Inst_Move(cAvidaContext& ctx)
{
    // In TestCPU, movement fails...
    if (m_organism->GetCellID() == -1) return false;
    
    bool move_success = m_organism->Move(ctx);  
    const int out_reg = FindModifiedRegister(rBX);   
    setInternalValue(out_reg, move_success, true);   
    return true;
}

bool cHardwareExperimental::Inst_RangeMove(cAvidaContext& ctx)
{
  // In TestCPU, movement fails...
  if (m_organism->GetCellID() == -1) return false;
  assert(m_organism != 0);
  
  bool safe_passage = true;
  bool move_success = false;
  const int faced_range = m_organism->GetFacedCellDataTerritory();
  
  if (faced_range != -1 && (faced_range != m_organism->GetOpinion().first) && 
      ((m_world->GetStats().GetUpdate() - m_organism->GetFacedCellDataUpdate()) <= m_world->GetConfig().MARKING_EXPIRE_DATE.Get())) {
    safe_passage = false;
  }
  
  if (safe_passage) {
    move_success = m_organism->Move(ctx);
  }
  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, move_success, true);  
  setInternalValue(FindModifiedNextRegister(rBX), safe_passage, true);
  return true;
}

bool cHardwareExperimental::Inst_RangePredMove(cAvidaContext& ctx)
{
  // In TestCPU, movement fails...
  if (m_organism->GetCellID() == -1) return false;
  assert(m_organism != 0);
  
  bool safe_passage = true;
  bool move_success = false;
  const int faced_range = m_organism->GetFacedCellDataTerritory();
  
  if (m_organism->GetForageTarget() == -2 && faced_range != -1 && faced_range != m_organism->GetOpinion().first 
      && ((m_world->GetStats().GetUpdate() - m_organism->GetFacedCellDataUpdate()) <= m_world->GetConfig().MARKING_EXPIRE_DATE.Get())) {
    safe_passage = false;
  }
  if (safe_passage) {
    move_success = m_organism->Move(ctx);
  }
  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, move_success, true);  
  setInternalValue(FindModifiedNextRegister(rBX), safe_passage, true);
  return true;
}

bool cHardwareExperimental::Inst_GetNorthOffset(cAvidaContext& ctx) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetFacedDir(), true);
  return true;
}

bool cHardwareExperimental::Inst_GetPositionOffset(cAvidaContext& ctx) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetNortherly(), true);
  setInternalValue(FindModifiedNextRegister(out_reg), m_organism->GetEasterly(), true);
  return true;
}

bool cHardwareExperimental::Inst_GetNortherly(cAvidaContext& ctx) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetNortherly(), true);
  return true;  
}

bool cHardwareExperimental::Inst_GetEasterly(cAvidaContext& ctx) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetEasterly(), true);
  return true;  
}

bool cHardwareExperimental::Inst_ZeroEasterly(cAvidaContext& ctx) {
  m_organism->ClearEasterly();
  return true;
}

bool cHardwareExperimental::Inst_ZeroNortherly(cAvidaContext& ctx) {
  m_organism->ClearNortherly();
  return true;
}

bool cHardwareExperimental::Inst_RotateLeftOne(cAvidaContext& ctx)
{
    m_organism->Rotate(1);
    return true;
}

bool cHardwareExperimental::Inst_RotateRightOne(cAvidaContext& ctx)
{
    m_organism->Rotate(-1);
    return true;
}

bool cHardwareExperimental::Inst_RotateUphill(cAvidaContext& ctx)
{
    int actualNeighborhoodSize = m_organism->GetNeighborhoodSize();  
    int group = 0;
    
    if(m_organism->HasOpinion()) group = m_organism->GetOpinion().first; 
    
    const tArray<double> current_res = m_organism->GetOrgInterface().GetResources(ctx);   
    double max_res = 0;
    for(int i = 0; i < actualNeighborhoodSize; i++) {
        m_organism->Rotate(1);
        tArray<double> faced_res = m_organism->GetOrgInterface().GetFacedCellResources(ctx); 
        if (faced_res[group] > max_res) max_res = faced_res[group];
    } 
    
    if (max_res > current_res[group]) {
        for(int i = 0; i < actualNeighborhoodSize; i++) {
            tArray<double> faced_res = m_organism->GetOrgInterface().GetFacedCellResources(ctx); 
            if (faced_res[group] != max_res) m_organism->Rotate(1);
        }
    }
    // return % change
    int res_diff = (int) ((max_res - current_res[group])/current_res[group] * 100 + 0.5);
    int reg_to_set = FindModifiedRegister(rBX);
    setInternalValue(reg_to_set, res_diff, true);
    return true;
}

bool cHardwareExperimental::Inst_RotateHome(cAvidaContext& ctx)
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
        m_organism->Rotate(1);
        if (m_organism->GetFacedDir() == correct_facing) break;
    }
    return true;
}

bool cHardwareExperimental::Inst_RotateUnoccupiedCell(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  
  for (int i = 0; i < m_organism->GetNeighborhoodSize(); i++) {
    if (!m_organism->IsNeighborCellOccupied()) { 
      setInternalValue(reg_used, 1, true);      
      return true;
    }
    m_organism->Rotate(1); // continue to rotate
  }  
  setInternalValue(reg_used, 0, true);
  return true;
}

bool cHardwareExperimental::Inst_RotateX(cAvidaContext& ctx)
{
  const int num_neighbors = m_organism->GetNeighborhoodSize();
  int rot_dir = 1;
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  const int reg_used = FindModifiedRegister(rBX);
  int rot_num = m_threads[m_cur_thread].reg[reg_used].value;
  // If this org has no trailing nop, rotate once in random direction.
  const cCodeLabel& search_label = GetLabel();
  if (search_label.GetSize() == 0) {
    rot_num = 1;
    m_world->GetRandom().GetInt(0,2) ? rot_dir = -1 : rot_dir = 1; 
  }
  // Else rotate the nop number of times in the appropriate direction
  rot_num < 0 ? rot_dir = -1 : rot_dir = 1;
  rot_num = abs(rot_num);
  if (rot_num > 7) rot_num = rot_num % 8;
  for (int i = 0; i < rot_num; i++) m_organism->Rotate(rot_dir);
  
  setInternalValue(reg_used, rot_num * rot_dir, true);
  return true;
}

bool cHardwareExperimental::Inst_SenseResourceID(cAvidaContext& ctx)
{
    const tArray<double> res_count = m_organism->GetOrgInterface().GetResources(ctx); 
    int reg_to_set = FindModifiedRegister(rBX);  
    double max_resource = 0.0;    
    // if more than one resource is available, return the resource ID with the most available in this spot (note that, with global resources, the GLOBAL total will evaluated)
    for (int i = 0; i < res_count.GetSize(); i++) {
        if (res_count[i] > max_resource) {
            max_resource = res_count[i];
            setInternalValue(reg_to_set, i, true);
        }
    }    
    return true;
}

bool cHardwareExperimental::Inst_SenseResQuant(cAvidaContext& ctx)
{
  const tArray<double> cell_res = m_organism->GetOrgInterface().GetResources(ctx); 
  const int req_reg = FindModifiedRegister(rBX);
  int res_sought = -1;
  // are you trying to sense a valid resource?
  const int res_req = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  if (res_req < cell_res.GetSize() && res_req > 0) {  
    res_sought = res_req; 
  }
  
  int res_amount = 0;
  // if you requested a valid resource, we return the value for that res
  if (res_sought != -1) res_amount = (int) (cell_res[res_sought] * 100 + 0.5);
  // otherwise, we sum across all resources in the cell
  else {
    for (int i = 0; i < cell_res.GetSize(); i++) {
      res_amount += (int) (cell_res[i] * 100 + 0.5);
    }
  }
  setInternalValue(FindModifiedNextRegister(req_reg), res_sought, true);
  const int res_tot_reg = FindModifiedNextRegister(FindModifiedNextRegister(req_reg));
  setInternalValue(res_tot_reg, res_amount, true);
  return true;
}

bool cHardwareExperimental::Inst_SenseNest(cAvidaContext& ctx)
{
  const tArray<double> cell_res = m_organism->GetOrgInterface().GetResources(ctx); 
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  const int reg_used = FindModifiedRegister(rBX);
  const cCodeLabel& search_label = GetLabel();

  int nest_id = -1;
  int nest_val = 0;
  
  // default to opinion res
  if (m_organism->HasOpinion()) nest_id = m_organism->GetOpinion().first;
  // override with nop specified res 
  if (search_label.GetSize() > 0) nest_id = m_threads[m_cur_thread].reg[reg_used].value;
  
  // if no nop, invalid nop value, or invalid opinion return the id of the first nest in the cell with val >= 1
  if (nest_id < 0 || nest_id >= resource_lib.GetSize()) {
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
  const tArray<double> cell_res = m_organism->GetOrgInterface().GetResources(ctx); 
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
    res_amount = (int) (cell_res[res_sought] * 100 + 0.5);
    faced_res = (int) (m_organism->GetOrgInterface().GetFacedCellResources(ctx)[res_sought] * 100 + 0.5);
  }
  else {
    for (int i = 0; i < cell_res.GetSize(); i++ ) {
      res_amount += (int) (cell_res[i] * 100 + 0.5);
      faced_res = (int) (m_organism->GetOrgInterface().GetFacedCellResources(ctx)[i] * 100 + 0.5);
    }
  }
  
  setInternalValue(FindModifiedNextRegister(req_reg), res_sought, true);
  const int res_tot_reg = FindModifiedNextRegister(FindModifiedNextRegister(req_reg));
  setInternalValue(res_tot_reg, res_amount - faced_res, true);
  return true;
}

/* Tests a cell for the Inst_LookAhead function
 * 
 * Parameters:
 habitat_used    specifies what type of object we're interested in
 search_type     tells how to search (stop when we've found the closest, count total #, etc)
 resource_lib    library of available resources
 target_cell     the cell we're testing
 * 
 * Returns:
 *		If we're looking for the closest resource, return that resource's ID
 *    otherwise, returns the number of objects we're looking for that are in target_cell
 *    
 */
cHardwareExperimental::searchInfo cHardwareExperimental::TestCell(cAvidaContext& ctx, int habitat_used, int search_type, int res_id_sought, const cResourceLib& resource_lib, int target_cell_num)
{
  int lib_size = resource_lib.GetSize();
  tArray<double> cell_res = m_organism->GetOrgInterface().GetCellResources(target_cell_num, ctx);
  searchInfo returnInfo;
  returnInfo.amountFound = 0;
  returnInfo.resource_id = res_id_sought;
  
  if(habitat_used == 0 || habitat_used == 1 || habitat_used == 2 || habitat_used == 4){
    // Looking for edible resources or topological features
    
    // by default, look at every resource ID unless we're looking for a specific one
    int min_index = 0;
    int max_index = lib_size - 1;
    if(res_id_sought != -1)
      min_index = max_index = res_id_sought;
    
    for( int k = min_index; k <= max_index; k++) {
      if(resource_lib.GetResource(k)->GetHabitat() == habitat_used) {
        returnInfo.amountFound += cell_res[k];
        returnInfo.resource_id = k;
      }
    }
  }
  else if ( habitat_used == -2 ) {
    // we're looking for other organisms
    
    const cPopulationCell& target_cell = m_world->GetPopulation().GetCell(target_cell_num);
    if(target_cell.IsOccupied() && !target_cell.GetOrganism()->IsDead()) {
      int type_seen = target_cell.GetOrganism()->GetForageTarget();
      
      if(search_type == 0)
        returnInfo.amountFound++;
      else if (search_type > 0){
        if(type_seen == -2)
          returnInfo.amountFound++;
      }
      else if (search_type < 0){
        if(type_seen != -2)
          returnInfo.amountFound++;
      }
    }
  }
  
  return returnInfo;
}
bool cHardwareExperimental::Inst_LookAhead(cAvidaContext& ctx) 
{
  const int geometry = m_world->GetConfig().WORLD_GEOMETRY.Get();
  // temp check on world geometry until code can handle other geometries
  if (geometry != 1) m_world->GetDriver().RaiseFatalException(-1, "Instruction look-ahead only written to work in bounded grids");
  
  // If this organism has no neighboring cells, ignore instruction.
  if (m_organism->GetNeighborhoodSize() == 0) return false;
  
  // get the resource library
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  const int lib_size = resource_lib.GetSize();
  const int worldx = m_world->GetConfig().WORLD_X.Get();
  const int worldy = m_world->GetConfig().WORLD_Y.Get();
  
  const cCodeLabel& search_label = GetLabel();
  
  // first reg gives habitat type sought (aligns with org m_target settings and gradient res habitat types)
  // if sensing food resource, habitat = 0 (gradients)
  // if sensing topography, habitat = 1 (hills)
  // if sensing objects, habitat = 2 (walls)  
  // habitat 3 = hidden resources (hidden from a distance)
  // habitat 4 = unhidden den resource
  // habitat -2 = organisms
  
  const int habitat_reg = FindModifiedRegister(rBX);
  int habitat_used = m_threads[m_cur_thread].reg[habitat_reg].value;
  bool pred_experiment = (m_world->GetConfig().PRED_PREY_SWITCH.Get() != -1);
  
  // fail if the org is trying to sense a nest/hidden habitat
  if (habitat_used == 3) return false;
  
  // default to look for orgs if invalid habitat & predator
  else if (pred_experiment && m_organism->GetForageTarget() == -2 && 
           (habitat_used < -2 || habitat_used > 4 || habitat_used == -1)) habitat_used = -2;
  // default to look for env res if invalid habitat & forager
  else if (habitat_used < -2 || habitat_used > 4 || habitat_used == -1) habitat_used = 0;
  
  // second reg gives distance sought--arbitrarily capped at half long axis of world--default to 1 if low invalid number, half-world if high  
  const int long_axis = (int) (max(worldx, worldy) * 0.5 + 0.5);  
  const int distance_reg = FindModifiedNextRegister(habitat_reg);
  int distance_sought = 1;
  if (search_label.GetSize() > 1) distance_sought = m_threads[m_cur_thread].reg[distance_reg].value;
  if (distance_sought < 0) distance_sought = 1;
  else if (distance_sought > long_axis) distance_sought = long_axis;
  
  // third register gives type of search used for food resources (habitat 0) and org hunting
  // env res search_types: 
  // 0 = look for closest edible res (>=1), closest hill/wall, or closest den (default), 1 = count # edible cells/walls/hills, -1 = total food res in cells
  // org hunting search types: 
  // 0 = closest any org (default), 1 = closest predator, 2 = count predators, -1 = closest prey, -2 = count prey
  const int search_type_reg = FindModifiedNextRegister(distance_reg);  
  int search_type = 0;
  if (search_label.GetSize() > 2) search_type = m_threads[m_cur_thread].reg[search_type_reg].value;
  
  // if looking for env res, default to closest edible
  if (habitat_used != -2 && (search_type < -1 || search_type > 1)) search_type = 0;
  // if looking for orgs in predator environment, default to closest org of any type
  else if (pred_experiment && habitat_used == -2 && (search_type < -2 || search_type > 2)) search_type = 0;
  // if looking for orgs in non-predator environment, default to closest org of any type
  else if (!pred_experiment && habitat_used == -2 && (search_type < -2 || search_type > 0)) search_type = 0;
  
  // fourth register gives specific instance of env resources sought (default to target, which will be -1 (==any) if org has no target)
  int res_id_sought = m_organism->GetForageTarget();
  // if you're a predator, you can't specify a specific 'org' res (can still look for other things if specify via habitat reg)  
  // APW TODO--allow targeting specific org?
  if (res_id_sought < 0) res_id_sought = -1;
  const int res_id_reg = FindModifiedNextRegister(search_type_reg);
  if (NUM_REGISTERS > 3 && habitat_used != -2 && search_label.GetSize() > 3) {
    res_id_sought = m_threads[m_cur_thread].reg[res_id_reg].value;
    if (res_id_sought < 0 || res_id_sought >= lib_size) res_id_sought = -1;
  }
  
  /*  // fifth register modifies search type = look for resource cells with requested food res height value (default = 'off')
   int spec_value = -1;
   const int spec_value_reg = FindModifiedNextRegister(res_id_reg);  
   if (NUM_REGISTERS > 4) {
   spec_value = m_threads[m_cur_thread].reg[spec_value_reg].value;
   }
   */
  
  // if an org is trying to do totals for a specific env resource that is not actually food, this is invalid and we can exit now
  if (habitat_used != -2 && search_type == -1 && res_id_sought != -1) {
    if (resource_lib.GetResource(res_id_sought)->GetHabitat() != 0) return true;
  }
  
  // start the real work of walking through cells
  const int facing = m_organism->GetFacedDir();
  const int faced_cell = m_organism->GetFacedCellID();
  int cell = m_organism->GetCellID();
  
  const int ahead_dir = faced_cell - cell;
  int dist_used = 0;
  
  int center_cell = cell;
  int this_cell = cell;
  
  bool count_center = true;
  bool any_valid_side_cells = false;
  
  bool found = false;
  int count = 0;
  double totalAmount = 0;
  
  searchInfo cellResultInfo;
  
  tArray<double> cell_res;
  
  bool stop_at_first_found = (search_type == 0) || (habitat_used == -2 && (search_type >= -1 && search_type <= 1));
	
  for (int dist = 0; dist < distance_sought + 1; dist++) {
    // work on CENTER cell for this dist
    
    // while side cells will always be valid if center is valid, center cell can be invalid when side cells are still valid (on diagonals)    
    if (count_center) {
      cellResultInfo = TestCell(ctx, habitat_used, search_type, res_id_sought, resource_lib, center_cell);
      
      if(cellResultInfo.amountFound >= 1) {
        // we've found what we're looking for
        found = true;
        count ++;
        totalAmount += cellResultInfo.amountFound;
        
        if( stop_at_first_found ){
          // we were only looking for the first one.
          // break out of entire search loop
          dist_used = dist;
          break;
        }
      }
    } // end work on CENTER cell for this dist
    
    // work on SIDE of center cells for this dist
    
    // how many cells do we need to look at on both sides 
    int num_cells_either_side = 0;
    if (dist > 0)
      num_cells_either_side = (dist % 2) ? (int) ((dist - 1) * 0.5) : (int) (dist * 0.5);
    
    for (int do_lr = 0; do_lr < 2; do_lr++) {
      bool count_side = true;
      int prev_cell = center_cell;
      for (int j = 1; j < num_cells_either_side + 1; j++) {
        
        if (facing == 0 && do_lr == 0) this_cell = center_cell - j;
        else if (facing == 2 && do_lr == 0) this_cell = center_cell + (-1 * j * worldx);
        else if (facing == 4 && do_lr == 0) this_cell = center_cell + j;
        else if (facing == 6 && do_lr == 0) this_cell = center_cell + (j * worldx);
        
        else if (facing == 0 && do_lr == 1) this_cell = center_cell + j;
        else if (facing == 2 && do_lr == 1) this_cell = center_cell + (j * worldx);
        else if (facing == 4 && do_lr == 1) this_cell = center_cell - j;
        else if (facing == 6 && do_lr == 1) this_cell = center_cell + (-1 * j * worldx);
        
        // since avida is a grid, diagonal facings work off of actual travel distance (sense radius draws a box)
        else if (facing == 1 && do_lr == 0) this_cell = center_cell - j;
        else if (facing == 3 && do_lr == 0) this_cell = center_cell + (-1 * j * worldx);
        else if (facing == 5 && do_lr == 0) this_cell = center_cell + j;
        else if (facing == 7 && do_lr == 0) this_cell = center_cell + (j * worldx);
        
        else if (facing == 1 && do_lr == 1) this_cell = center_cell + (j * worldx);
        else if (facing == 3 && do_lr == 1) this_cell = center_cell + (-1 * j);
        else if (facing == 5 && do_lr == 1) this_cell = center_cell + (-1 * j * worldx);
        else if (facing == 7 && do_lr == 1) this_cell = center_cell + j; 
        
        // test if the side cell is still on world, if it isn't do the other side
        if (this_cell < 0 || this_cell > (worldx * (worldy - 1))) count_side = false; 
        else if ((geometry == 1) && (this_cell - prev_cell == 1) && (this_cell % worldx == 0)) count_side = false; 
        else if ((geometry == 1) && (this_cell - prev_cell == -1) && (prev_cell % worldx == 0)) count_side = false; 
        else any_valid_side_cells = true;
        
        
        prev_cell = this_cell;
        if (count_side) {
          cellResultInfo = TestCell(ctx, habitat_used, search_type, res_id_sought, resource_lib, this_cell);
          
          if(cellResultInfo.amountFound >= 1) {
            // we've found what we're looking for
            found = true;
            count ++;
            totalAmount += cellResultInfo.amountFound;
            
            if( stop_at_first_found ){
              // we were only looking for the first one.
              // break out of the side-searching loop
              dist_used = dist;
              break;
            }
          }
        }
        if ( !count_side) break;
      }
      // break out of entire search loop
      if (stop_at_first_found && found) break;
    }
    if (stop_at_first_found && found) break;
    
    // before we do the next side cell...
    
    // stop if we never found any valid cells at the current distance
    if (!any_valid_side_cells && !count_center) {
      dist -= 1;
      break;
    }
    
    // if facing W, SW or NW check if center cell now standing on edge of world, only do side cells from now on
    if((geometry == 1) && ((facing == 6) || (facing == 5) || (facing == 7)) && (center_cell % worldx == 0)) count_center = false;
    
    // figure out the what the next center cell is about to be
    center_cell = center_cell + ahead_dir;
    
    // if facing E, SE, or NE check if next center cell is going to be off edge of world, only do side cells from now on
    if((geometry == 1) && ((facing == 2) || (facing == 3) || (facing == 1)) && (center_cell % worldx == 0)) count_center = false;
    // if next center cell is going to be less than 0 or greater than max cell (in grid), only do side cells from now on
    else if(center_cell < 0 || center_cell > (worldx * (worldy - 1))) count_center = false;
    
  } // End getting values
  
  // begin reached end output 
  setInternalValue(habitat_reg, habitat_used, true);
  
  if(found){
    setInternalValue(habitat_reg, habitat_used, true);
    setInternalValue(distance_reg, dist_used, true);
    setInternalValue(res_id_reg, cellResultInfo.resource_id);
    
    // if we were looking for first edible and found the nearest
    if(search_type == 0 && habitat_used == 0) {
      setInternalValue(search_type_reg, search_type, true);
      if (NUM_REGISTERS > 3)
        setInternalValue(res_id_reg, cellResultInfo.resource_id, true);
      return true;
    }
    else if (stop_at_first_found) {
      // if we were looking for topo features or organisms and we found the nearest
      return true;
    }
  }
  // if we never found what we were looking for
  if (stop_at_first_found && !found) {
    setInternalValue(distance_reg, -1, true);
    setInternalValue(search_type_reg, search_type, true);
    if (NUM_REGISTERS > 3) setInternalValue(res_id_reg, cellResultInfo.resource_id, true);
    return true;
  }
  
  // if we did find what we were looking for, return either the count of cells or the total amount of resources
  int returnVal = count;
  if (habitat_used == 0 && search_type == -1)
    returnVal = (int) (totalAmount + 0.5);
  
  if (!stop_at_first_found) {
    setInternalValue(distance_reg, dist_used, true);
    if (NUM_REGISTERS == 3)
      setInternalValue(search_type_reg, returnVal, true);
    else if(NUM_REGISTERS > 3) {
      setInternalValue(search_type_reg, search_type, true);
      setInternalValue(FindModifiedNextRegister(search_type_reg), returnVal, true);
    }
  }
  
  if (habitat_used == 0 && NUM_REGISTERS > 4) 
    setInternalValue(FindModifiedNextRegister(FindModifiedNextRegister(search_type_reg)), res_id_sought, true);
  
  return true;
}

bool cHardwareExperimental::Inst_SenseFacedHabitat(cAvidaContext& ctx) 
{
    int reg_to_set = FindModifiedRegister(rBX);
    
    // get the resource library
    const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
    
    // get the destination cell resource levels
    tArray<double> cell_resource_levels = m_organism->GetOrgInterface().GetFacedCellResources(ctx);
    
    // check for any habitats ahead that affect movement, returning the most 'severe' habitat type
    // are there any barrier resources in the faced cell    
    for (int i = 0; i < cell_resource_levels.GetSize(); i++) {
        if (resource_lib.GetResource(i)->GetHabitat() == 2 && cell_resource_levels[i] > 0) {
            setInternalValue(reg_to_set, 2, true);
            return true;
        }    
    }
    // if no barriers, are there any hills in the faced cell    
    for (int i = 0; i < cell_resource_levels.GetSize(); i++) {
        if (resource_lib.GetResource(i)->GetHabitat() == 1 && cell_resource_levels[i] > 0) {
            setInternalValue(reg_to_set, 1, true);
            return true;
        }
    }
    // if no barriers or hills, we return a 0 to indicate clear sailing
    setInternalValue(reg_to_set, 0, true);
    return true;
}
 
bool cHardwareExperimental::Inst_SetForageTarget(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  const int prop_target = GetRegister(FindModifiedRegister(rBX));

  // make sure we use a valid (resource) target
  // -2 target means setting to predator; -1 (nothing) is default
  if (!m_world->GetEnvironment().IsTargetID(prop_target) && (prop_target != -2)) return false;
  
  // return false if setting to predator in non-predator experiment
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0 && prop_target == -2) return false;
  
  //return false if org setting target to current one (avoid paying costs for not switching)
  const int old_target = m_organism->GetForageTarget();
  if (old_target == prop_target) return false;
  
  // return false if predator trying to become prey and this has been disallowed
  if (old_target == -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() == 0) return false;

  // return false if trying to become predator and there are none in the experiment
  if (prop_target == -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() == -1) return false;

  // Set the new target and return the value
  m_organism->SetForageTarget(prop_target);
	setInternalValue(FindModifiedRegister(rBX), prop_target, false);
  return true;
}

bool cHardwareExperimental::Inst_GetForageTarget(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  const int target_reg = FindModifiedRegister(rBX);
  setInternalValue(target_reg, m_organism->GetForageTarget(), false);
  return true;
}

bool cHardwareExperimental::Inst_SenseOpinionResQuant(cAvidaContext& ctx)
{
  const tArray<double> res_count = m_organism->GetOrgInterface().GetResources(ctx); 
  // check if this is a valid group
  if(m_organism->GetOrgInterface().HasOpinion(m_organism)) {
    int opinion = m_organism->GetOpinion().first;
    int res_opinion = (int) (res_count[opinion] * 100 + 0.5);
    int reg_to_set = FindModifiedRegister(rBX);
    setInternalValue(reg_to_set, res_opinion, true);
  }
  return true;
}

bool cHardwareExperimental::Inst_SenseDiffFaced(cAvidaContext& ctx) 
{
  const tArray<double> res_count = m_organism->GetOrgInterface().GetResources(ctx); 
  if(m_organism->GetOrgInterface().HasOpinion(m_organism)) {
    int opinion = m_organism->GetOpinion().first;
    int reg_to_set = FindModifiedRegister(rBX);
    double faced_res = m_organism->GetOrgInterface().GetFacedCellResources(ctx)[opinion];  
    // return % change
    int res_diff = (int) (((faced_res - res_count[opinion])/res_count[opinion]) * 100 + 0.5);
    setInternalValue(reg_to_set, res_diff, true);
  }
  return true;
}

//! An organism joins a group by setting it opinion to the group id. 
bool cHardwareExperimental::Inst_JoinGroup(cAvidaContext& ctx)
{
    int group;
    // Check if the org is currently part of a group
    assert(m_organism != 0);
	
    int prop_group_id = GetRegister(FindModifiedRegister(rBX));
  
    // check if this is a valid group
    if (m_world->GetConfig().USE_FORM_GROUPS.Get() == 2 &&
        !(m_world->GetEnvironment().IsGroupID(prop_group_id))) {
    return false; 
    }
    // injected orgs might not have an opinion
    if (m_organism->HasOpinion()) {
        group = m_organism->GetOpinion().first;
        
        //return false if org setting opinion to current one (avoid paying costs for not switching)
        if (group == prop_group_id) return false;
        
        // If tolerances are on the org must pass immigration chance @JJB
        if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
            // If there are no members of the target group, automatically successful immigration
            if (m_organism->GetOrgInterface().NumberOfOrganismsInGroup(prop_group_id) == 0) {
                m_organism->LeaveGroup(group);
            }
            // Calculate chances based on target group tolerance of another org successfully immigrating
            else if (m_organism->GetOrgInterface().NumberOfOrganismsInGroup(prop_group_id) > 0) {
                const double tolerance_max = (double) m_world->GetConfig().MAX_TOLERANCE.Get();
                const double target_group_tolerance = (double) m_organism->GetOrgInterface().CalcGroupToleranceImmigrants(prop_group_id);
                double probability_immigration = target_group_tolerance / tolerance_max;
                double rand = m_world->GetRandom().GetDouble();
                if (rand <= probability_immigration) {
                    // Org successfully immigrates
                    m_organism->LeaveGroup(group);
                }
                // If the org fails to immigrate it stays in its current group (return true so there is a resource cost paid for failed immigration)
                else {
                    return true;
                }
            }
        }
        else {
            // otherwise, subtract org from current group
            m_organism->LeaveGroup(group);
        }
    }
	
    // Set the opinion
    m_organism->SetOpinion(prop_group_id);
  
    // Add org to group count
    group = m_organism->GetOpinion().first;	
    m_organism->JoinGroup(group);
    
    return true;
}

// A predator can establish a new group, attempt to immigrate into the group that marked the cell in front of them, or become a nomad. 
bool cHardwareExperimental::Inst_ChangePredGroup(cAvidaContext& ctx)
{
  if (m_organism->GetForageTarget() != -2) return false;
  
  // If ?AX? make a new group.
  
  // If ?BX? change to group -1.
  
  // If ?CX? read m_organism->GetFacedCellDataTerritory() and attempt immigration into that group.

      
  // return (new) group ID & change success
    
    
    
return true;
}

bool cHardwareExperimental::Inst_GetGroupID(cAvidaContext& ctx)
{
    assert(m_organism != 0);
    if (m_organism->HasOpinion()) {
        const int group_reg = FindModifiedRegister(rBX);
        
        setInternalValue(group_reg, m_organism->GetOpinion().first, false);
    }
    return true;
}

bool cHardwareExperimental::Inst_GetPredGroupID(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_organism->GetForageTarget() != -2) return false;
  if (m_organism->HasOpinion()) {
    const int group_reg = FindModifiedRegister(rBX);
    
    setInternalValue(group_reg, m_organism->GetOpinion().first, false);
  }
  return true;
}

bool cHardwareExperimental::Inst_GetFacedOrgID(cAvidaContext& ctx)
//Get ID of organism faced by this one, if there is an organism in front.
{
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism * neighbor = m_organism->GetNeighbor();
  if (neighbor->IsDead())  return false;  
  
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, neighbor->GetID(), true);
  return true;
}

//Attack organism faced by this one, if there is non-predator target in front, and steal it's merit and reactions. 
bool cHardwareExperimental::Inst_AttackMeritPrey(cAvidaContext& ctx)
{
  assert(m_organism != 0);
    
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0) return false;

  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism* target = m_organism->GetNeighbor();
  if (target->IsDead()) return false;  
  
  // attacking other carnivores is handled differently (e.g. using fights or tolerance)
  if (target->GetForageTarget() == -2 && m_organism->GetForageTarget() == -2) {
    return false;
  }
  
  // prevent killing on nests/safe havens
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (m_organism->GetOrgInterface().GetFacedCellResources(ctx)[i] > 0 && resource_lib.GetResource(i)->GetHabitat() == 3) return true;
  }
    
  // add prey's merit to predator's
  const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
  double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
  attacker_merit += target_merit;
  m_organism->UpdateMerit(attacker_merit);
  
  // now add on the victims reaction counts to your own...
  tArray<int> target_reactions = target->GetPhenotype().GetLastReactionCount();
  tArray<int> org_reactions = m_organism->GetPhenotype().GetStolenReactionCount();

  for (int i = 0; i < org_reactions.GetSize(); i++) {
    org_reactions[i] += target_reactions[i];
    m_organism->GetPhenotype().SetStolenReactionCount(i, org_reactions[i]);
  }
    
  
  //APW TODO
  // now add the victims internal resource bins to your own

  // if you weren't a predator before, you are now!
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() != -1) m_organism->SetForageTarget(-2);
  
  target->Die(ctx);
  
  bool attack_success = true;  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, attack_success, true);   
  setInternalValue(FindModifiedNextRegister(out_reg), target_merit, true);
  setInternalValue(FindModifiedNextRegister(FindModifiedNextRegister(out_reg)), attacker_merit, true);
  
  return true;
} 		

//Attack organism faced by this one if you are both predators or both prey. 
bool cHardwareExperimental::Inst_FightMeritOrg(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism* target = m_organism->GetNeighbor();
  if (target->IsDead()) return false;  

  // allow only for predator vs predator or prey vs prey
  if ((target->GetForageTarget() == -2 && m_organism->GetForageTarget() != -2) || 
      (target->GetForageTarget() != -2 && m_organism->GetForageTarget() == -2)) {
    return false;
  }

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
  
  const int target_cell = target->GetCellID();
  
  m_world->GetPopulation().AttackFacedOrg(ctx, target_cell); 
  
  bool attack_success = true;  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, attack_success, true);   
  
  return true;
} 	

//Get odds of winning or tieing in a fight.
bool cHardwareExperimental::Inst_GetMeritFightOdds(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism* target = m_organism->GetNeighbor();
  if (target->IsDead()) return false;  

  // allow only for predator vs predator or prey vs prey
  if ((target->GetForageTarget() == -2 && m_organism->GetForageTarget() != -2) || 
      (target->GetForageTarget() != -2 && m_organism->GetForageTarget() == -2)) {
    return false;
  }
  
  const double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
  const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
  const double attacker_win_odds = ((attacker_merit) / (attacker_merit + target_merit));
  const double target_win_odds = ((target_merit) / (attacker_merit + target_merit)); 
  
  const double odds_someone_dies = max(attacker_win_odds, target_win_odds);
  // my win odds are odds nobody dies or someone dies and it's the target
  const double odds_I_dont_die = (1 - odds_someone_dies) + ((1 - target_win_odds) * odds_someone_dies);

  // return odds out of 10
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, odds_I_dont_die * 10 + 0.5, true);   

  return true;
} 	

//Attack organism faced by this one if you are both predators or both prey. 
bool cHardwareExperimental::Inst_FightOrg(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism* target = m_organism->GetNeighbor();
  if (target->IsDead()) return false;  
  
  // allow only for predator vs predator or prey vs prey
  if ((target->GetForageTarget() == -2 && m_organism->GetForageTarget() != -2) || 
      (target->GetForageTarget() != -2 && m_organism->GetForageTarget() == -2)) {
    return false;
  }
    
  int target_cell = target->GetCellID();
  
  m_world->GetPopulation().AttackFacedOrg(ctx, target_cell); 
  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, 1, true);   
  
  return true;
} 	

//Attack organism faced by this one if you are both predators. 
bool cHardwareExperimental::Inst_FightPred(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  
  if (m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0) return false;
  
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism* target = m_organism->GetNeighbor();
  if (target->IsDead()) return false;  
  
  // allow only for predator vs predator
  if (target->GetForageTarget() != -2 || m_organism->GetForageTarget() != -2) {
    return false;
  }
  
  int target_cell = target->GetCellID();
  
  m_world->GetPopulation().AttackFacedOrg(ctx, target_cell); 
  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, 1, true);   
  
  return true;
} 

//Attack organism faced by this one if you are both predators or both prey. 
bool cHardwareExperimental::Inst_FightMeritPred(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism* target = m_organism->GetNeighbor();
  if (target->IsDead()) return false;  
  
  // allow only for predator vs predator
  if (target->GetForageTarget() != -2 || m_organism->GetForageTarget() != -2) {
    return false;
  }
  
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
  
  const int target_cell = target->GetCellID();
  
  m_world->GetPopulation().AttackFacedOrg(ctx, target_cell); 
  
  bool attack_success = true;  
  const int out_reg = FindModifiedRegister(rBX);   
  setInternalValue(out_reg, attack_success, true);   
  
  return true;
} 	

bool cHardwareExperimental::Inst_MarkCell(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  const int marking = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  m_organism->SetCellData(marking);
  
  return true;
}

bool cHardwareExperimental::Inst_MarkPredCell(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_organism->GetForageTarget() != -2) return false;
  const int marking = m_threads[m_cur_thread].reg[FindModifiedRegister(rBX)].value;
  m_organism->SetCellData(marking);
  
  return true;
}

bool cHardwareExperimental::Inst_ReadFacedCell(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  const int marking_reg = FindModifiedRegister(rBX);
  const int update_reg = FindModifiedNextRegister(rBX);
  const int org_reg = FindModifiedNextRegister(update_reg);
  
  setInternalValue(marking_reg, m_organism->GetFacedCellData(), true);
  setInternalValue(update_reg, m_world->GetStats().GetUpdate() - m_organism->GetFacedCellDataUpdate(), true);
  setInternalValue(org_reg, m_organism->GetFacedCellDataOrgID(), true);
  if (NUM_REGISTERS > 3) {
    const int group_reg = FindModifiedNextRegister(org_reg);
    setInternalValue(group_reg, m_organism->GetFacedCellDataTerritory(), true);    
  }
  
  return true;
}

bool cHardwareExperimental::Inst_ReadFacedPredCell(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_organism->GetForageTarget() != -2) return false;
  const int marking_reg = FindModifiedRegister(rBX);
  const int update_reg = FindModifiedNextRegister(rBX);
  const int org_reg = FindModifiedNextRegister(update_reg);
  
  setInternalValue(marking_reg, m_organism->GetFacedCellData(), true);
  setInternalValue(update_reg, m_world->GetStats().GetUpdate() - m_organism->GetFacedCellDataUpdate(), true);
  setInternalValue(org_reg, m_organism->GetFacedCellDataOrgID(), true);
  if (NUM_REGISTERS > 3) {
    const int group_reg = FindModifiedNextRegister(org_reg);
    setInternalValue(group_reg, m_organism->GetFacedCellDataTerritory(), true);    
  }
  
  return true;
}

//Teach offspring learned targeting/foraging behavior
bool cHardwareExperimental::Inst_TeachOffspring(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  m_organism->Teach(true);
  
  return true;
}

bool cHardwareExperimental::Inst_CheckFacedKin(cAvidaContext& ctx)
{
  if (!m_organism->IsNeighborCellOccupied()) return false;
  
  cOrganism * neighbor = m_organism->GetNeighbor();
  if (neighbor->IsDead()) return false;  
  
  // If there is no valid max genetic distance, go out to cousins.
  int gen_dist = GetRegister(FindModifiedRegister(rBX));
  if (gen_dist > 4 || gen_dist < 0) gen_dist = 4;
  
  bool is_kin = false;
  
  cBioGroup* bg = m_organism->GetBioGroup("genotype");
  if (!bg) return false;
  cSexualAncestry* sa = bg->GetData<cSexualAncestry>();
  if (!sa) {
    sa = new cSexualAncestry(bg);
    bg->AttachData(sa);
  }
  
  cBioGroup* nbg = neighbor->GetBioGroup("genotype");
  assert(nbg);
  if (sa->GetPhyloDistance(nbg) <= gen_dist) is_kin = true;
  
  setInternalValue(FindModifiedRegister(rBX), gen_dist, true);
  const int out_reg = FindModifiedNextRegister(rBX);   
  setInternalValue(out_reg, (int) is_kin, true);    
  return true;
}

/* Increases tolerance towards the addition of members to the group:
 nop-A: increases tolerance towards immigrants
 nop-B: increases tolerance towards own offspring
 nop-C: increases tolerance towards other offspring of the group.
 Removes the record of a previous update when dec-tolerance was executed,
 and places the modified tolerance total in the BX register. @JJB
 */
bool cHardwareExperimental::Inst_IncPredTolerance(cAvidaContext& ctx)
{
  if (m_organism->GetForageTarget() != -2 || m_organism->GetOpinion().first < 0) return false;
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() && m_world->GetConfig().TOLERANCE_WINDOW.Get()) {
    if(m_organism->GetOrgInterface().HasOpinion(m_organism)) {
      // If this instruction is not nop modified it fails to execute and does nothing @JJB
      if (!(m_inst_set->IsNop(getIP().GetNextInst())) || m_organism->GetOpinion().first == -1) return false;
      
      const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();    
      const int tolerance_to_modify = FindModifiedRegister(rBX);
      int tolerance_count = 0;
      
      // If ?AX? move update records of immigrant tolerance up one position removing the top most recent instance of dec-tolerance from records.
      if (tolerance_to_modify == rAX) {
        PushToleranceInstExe(0, ctx);
        
        for (int n = 0; n < tolerance_max - 1; n++) {
          m_organism->GetPhenotype().GetToleranceImmigrants()[n] = m_organism->GetPhenotype().GetToleranceImmigrants()[n + 1];
        }
        m_organism->GetPhenotype().GetToleranceImmigrants()[tolerance_max - 1] = 0;
        // Retrieve modified tolerance total for immigrants.
        tolerance_count = m_organism->GetPhenotype().CalcToleranceImmigrants();
        
        // Output tolerance total to BX register.
        setInternalValue(rBX, tolerance_count, true);
        return true;
      }
      
      // If ?BX? move updates of own offspring tolerance up one position removing the most recent instance of dec-tolerance from records.
      if ((tolerance_to_modify == rBX) && (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 0)) {
        PushToleranceInstExe(1, ctx);
        
        for (int n = 0; n < tolerance_max - 1; n++) {
          m_organism->GetPhenotype().GetToleranceOffspringOwn()[n] = m_organism->GetPhenotype().GetToleranceOffspringOwn()[n + 1];
        }
        m_organism->GetPhenotype().GetToleranceOffspringOwn()[tolerance_max - 1] = 0;
        
        // Retrieve modified tolerance total for own offspring.
        tolerance_count = m_organism->GetPhenotype().CalcToleranceOffspringOwn();
        
        // Output tolerance total to BX register.
        setInternalValue(rBX, tolerance_count, true);
        return true;
      }
      
      // If ?CX? move updates of others offspring tolerance up one position removing the most recent instance of dec-tolerance from records.
      if ((tolerance_to_modify == rCX) && (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 0)) {
        PushToleranceInstExe(2, ctx);
        
        for (int n = 0; n < tolerance_max - 1; n++) {
          m_organism->GetPhenotype().GetToleranceOffspringOthers()[n] = m_organism->GetPhenotype().GetToleranceOffspringOthers()[n + 1];
        }
        m_organism->GetPhenotype().GetToleranceOffspringOthers()[tolerance_max - 1] = 0;
        
        // Retrieve modified tolerance total for other offspring in group.
        tolerance_count = m_organism->GetPhenotype().CalcToleranceOffspringOthers();
        
        // Output tolerance total to BX register.
        setInternalValue(rBX, tolerance_count, true);
        return true;
      } 
      return false;
    }
  }
  return false;
}

/* Decreases tolerance towards the addition of members to the group,
 nop-A: decreases tolerance towards immigrants
 nop-B: decreases tolerance towards own offspring
 nop-C: decreases tolerance towards other offspring of the group.
 Adds to records the update during which dec-tolerance was executed,
 and places the modified tolerance total in the BX register. @JJB
 */
bool cHardwareExperimental::Inst_DecPredTolerance(cAvidaContext& ctx)
{
  if (m_organism->GetForageTarget() != -2 || m_organism->GetOpinion().first < 0) return false;
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() && m_world->GetConfig().TOLERANCE_WINDOW.Get()) {
    if(m_organism->GetOrgInterface().HasOpinion(m_organism)) {
      // If this instruction is not nop modified it fails to execute and does nothing @JJB
      if (!(m_inst_set->IsNop(getIP().GetNextInst())) || m_organism->GetOpinion().first == -1) return false;
      
      const int cur_update = m_world->GetStats().GetUpdate();
      const int tolerance_max = m_world->GetConfig().MAX_TOLERANCE.Get();
      
      const int tolerance_to_modify = FindModifiedRegister(rBX);
      int tolerance_count = 0;
      
      // If ?AX? move update records of immigrant tolerance down one position, and add to the top the current update, adding a record of dec-tolerance.
      if (tolerance_to_modify == rAX) {
        PushToleranceInstExe(3, ctx);
        
        for (int n = tolerance_max - 1; n > 0; n--) {
          m_organism->GetPhenotype().GetToleranceImmigrants()[n] = m_organism->GetPhenotype().GetToleranceImmigrants()[n - 1];
        }
        m_organism->GetPhenotype().GetToleranceImmigrants()[0] = cur_update;
        // Retrieve modified tolerance total for immigrants.
        tolerance_count = m_organism->GetPhenotype().CalcToleranceImmigrants();
        
        // Output tolerance total to BX register.
        setInternalValue(rBX, tolerance_count, true);
        return true;
      }
      
      // If ?BX? move update records of own offspring tolerance down one position, and add to the top the current update, adding a record of dec-tolerance.
      if ((tolerance_to_modify == rBX) && (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 0)) {
        PushToleranceInstExe(4, ctx);
        
        for (int n = tolerance_max - 1; n > 0; n--) {
          m_organism->GetPhenotype().GetToleranceOffspringOwn()[n] = m_organism->GetPhenotype().GetToleranceOffspringOwn()[n - 1];
        }
        m_organism->GetPhenotype().GetToleranceOffspringOwn()[0] = cur_update;
        
        // Retrieve modified tolerance total for own offspring.
        tolerance_count = m_organism->GetPhenotype().CalcToleranceOffspringOwn();
        
        // Output tolerance total to BX register.
        setInternalValue(rBX, tolerance_count, true);
        return true;
      }
      
      // If ?CX? move update records of own offspring tolerance down one position, and add to the top the current update, adding a record of dec-tolerance.
      if ((tolerance_to_modify == rCX) && (m_world->GetConfig().TOLERANCE_VARIATIONS.Get() == 0)) {
        PushToleranceInstExe(5, ctx);
        
        for (int n = tolerance_max - 1; n > 0; n--) {
          m_organism->GetPhenotype().GetToleranceOffspringOthers()[n] = m_organism->GetPhenotype().GetToleranceOffspringOthers()[n - 1];
        }
        m_organism->GetPhenotype().GetToleranceOffspringOthers()[0] = cur_update;
        
        // Retrieve modified tolerance total for other offspring in the group.
        tolerance_count = m_organism->GetPhenotype().CalcToleranceOffspringOthers();
        
        // Output tolerance total to BX register.
        setInternalValue(rBX, tolerance_count, true);
        return true;
      } 
      return false;
    }
  }
  return false;
}

/* Retrieve current tolerance levels, placing each tolerance in a different register.
 Register AX: tolerance towards immigrants
 Register BX: tolerance towards own offspring
 Register CX: tolerance towards other offspring in the group @JJB
 */
bool cHardwareExperimental::Inst_GetPredTolerance(cAvidaContext& ctx)
{
  if (m_organism->GetForageTarget() != -2 || m_organism->GetOpinion().first < 0) return false;
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() && m_world->GetConfig().TOLERANCE_WINDOW.Get()) {
    if(m_organism->GetOrgInterface().HasOpinion(m_organism)) {
      if (m_organism->GetOpinion().first == -1) return false;
      PushToleranceInstExe(6, ctx);
      
      int tolerance_immigrants = m_organism->GetPhenotype().CalcToleranceImmigrants();
      int tolerance_own = m_organism->GetPhenotype().CalcToleranceOffspringOwn();
      int tolerance_others = m_organism->GetPhenotype().CalcToleranceOffspringOthers();
      setInternalValue(rAX, tolerance_immigrants, true);
      setInternalValue(rBX, tolerance_own, true);
      setInternalValue(rCX, tolerance_others, true);  
      return true;
    }
  }
  return false;
}  

/* Retrieve group tolerances placing each in a different register.
 Register AX: group tolerance towards immigrants
 Register BX: group tolerance towards own offspring
 Register CX: group tolerance towards offspring @JJB
 */
bool cHardwareExperimental::Inst_GetPredGroupTolerance(cAvidaContext& ctx)
{
  if (m_organism->GetForageTarget() != -2 || m_organism->GetOpinion().first < 0) return false;
  // If groups are used and tolerances are on...
  if (m_world->GetConfig().USE_FORM_GROUPS.Get() && m_world->GetConfig().TOLERANCE_WINDOW.Get()) {
    if(m_organism->GetOrgInterface().HasOpinion(m_organism)) {
      PushToleranceInstExe(7, ctx);
      
      const int group_id = m_organism->GetOpinion().first;
      if (group_id == -1) return false;
      
      double immigrant_odds = m_organism->GetOrgInterface().CalcGroupOddsImmigrants(group_id);
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
      return true;
    }
  }
  return false;
}

// Pushes the circumstances of a tolerance instruction execution to stats. @JJB
void cHardwareExperimental::PushToleranceInstExe(int tol_inst, cAvidaContext& ctx)
{
  const tArray<double> res_count = m_organism->GetOrgInterface().GetResources(ctx);
  
  int group_id = m_organism->GetOpinion().first;
  if (group_id == -1) return;
  int group_size = m_world->GetPopulation().NumberOfOrganismsInGroup(group_id);
  double resource_level = res_count[group_id];
  int tol_max = m_world->GetConfig().MAX_TOLERANCE.Get();
  
  double immigrant_odds = m_organism->GetOrgInterface().CalcGroupOddsImmigrants(group_id);
  double offspring_own_odds = m_organism->GetOrgInterface().CalcGroupOddsOffspring(m_organism);
  double offspring_others_odds = m_organism->GetOrgInterface().CalcGroupOddsOffspring(group_id);
  
  double odds_immi = immigrant_odds * 100 + 0.5;
  double odds_own = offspring_own_odds * 100 + 0.5;
  double odds_others = offspring_others_odds * 100 + 0.5;
  int tol_immi = m_organism->GetPhenotype().CalcToleranceImmigrants();
  int tol_own = m_organism->GetPhenotype().CalcToleranceOffspringOwn();
  int tol_others = m_organism->GetPhenotype().CalcToleranceOffspringOthers();
  
  m_organism->GetOrgInterface().PushToleranceInstExe(tol_inst, group_id, group_size, resource_level, odds_immi, odds_own,
                                                     odds_others, tol_immi, tol_own, tol_others, tol_max);
}

