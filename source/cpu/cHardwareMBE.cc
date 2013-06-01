/*
 *  cHardwareMBE.cc
 *  Avida
 *
 *  Created by APWagner on 10/26/2012 based on cHardwareMBE.h
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


#include "cHardwareMBE.h"

#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"
#include "avida/output/File.h"

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

inline unsigned int cHardwareMBE::BitCount(unsigned int value) const
{
  const unsigned int w = value - ((value >> 1) & 0x55555555);
  const unsigned int x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
  const unsigned int bit_count = ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
  return bit_count;
}


tInstLib<cHardwareMBE::tMethod>* cHardwareMBE::s_inst_slib = cHardwareMBE::initInstLib();

tInstLib<cHardwareMBE::tMethod>* cHardwareMBE::initInstLib(void)
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
    tInstLibEntry<tMethod>("nop-A", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-B", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-C", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-D", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-E", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-F", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-G", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-H", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    
    tInstLibEntry<tMethod>("nop-I", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-J", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-K", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-L", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-M", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-N", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-O", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    tInstLibEntry<tMethod>("nop-P", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, nInstFlag::NOP, "No-operation; modifies other instructions"),
    
    tInstLibEntry<tMethod>("NULL", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, 0, "True no-operation instruction: does nothing"),
    tInstLibEntry<tMethod>("nop-X", &cHardwareMBE::Inst_Nop, INST_CLASS_NOP, 0, "True no-operation instruction: does nothing"),
    
    // Threading 
    tInstLibEntry<tMethod>("id-thread", &cHardwareMBE::Inst_IdThread, INST_CLASS_OTHER, 0, "", BEHAV_CLASS_NONE),
    
    // Behavioral Execution
    tInstLibEntry<tMethod>("start-gene", &cHardwareMBE::Inst_StartGene, INST_CLASS_OTHER, 0, "", BEHAV_CLASS_BREAK),
    tInstLibEntry<tMethod>("end-gene", &cHardwareMBE::Inst_EndGene, INST_CLASS_OTHER, 0, "", BEHAV_CLASS_END_GENE),
    tInstLibEntry<tMethod>("kill-gene", &cHardwareMBE::Inst_KillGene, INST_CLASS_OTHER, 0, ""),

    // Standard Conditionals
    tInstLibEntry<tMethod>("if-n-equ", &cHardwareMBE::Inst_IfNEqu, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX?!=?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-less", &cHardwareMBE::Inst_IfLess, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? < ?CX?, else skip it"),
    tInstLibEntry<tMethod>("if-not-0", &cHardwareMBE::Inst_IfNotZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? != 0, else skip it"),
    tInstLibEntry<tMethod>("if-equ-0", &cHardwareMBE::Inst_IfEqualZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? == 0, else skip it"),
    tInstLibEntry<tMethod>("if-gtr-0", &cHardwareMBE::Inst_IfGreaterThanZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? > 0, else skip it"),
    tInstLibEntry<tMethod>("if-less-0", &cHardwareMBE::Inst_IfLessThanZero, INST_CLASS_CONDITIONAL, 0, "Execute next instruction if ?BX? < 0, else skip it"),
    tInstLibEntry<tMethod>("if-gtr-x", &cHardwareMBE::Inst_IfGtrX, INST_CLASS_CONDITIONAL),
    tInstLibEntry<tMethod>("if-equ-x", &cHardwareMBE::Inst_IfEquX, INST_CLASS_CONDITIONAL),
    
    // Core ALU Operations
    tInstLibEntry<tMethod>("pop", &cHardwareMBE::Inst_Pop, INST_CLASS_DATA, 0, "Remove top nuMBEr from stack and place into ?BX?"),
    tInstLibEntry<tMethod>("push", &cHardwareMBE::Inst_Push, INST_CLASS_DATA, 0, "Copy nuMBEr from ?BX? and place it into the stack"),
    tInstLibEntry<tMethod>("pop-all", &cHardwareMBE::Inst_PopAll, INST_CLASS_DATA, 0, "Remove top nuMBErs from stack and place into ?BX?"),
    tInstLibEntry<tMethod>("push-all", &cHardwareMBE::Inst_PushAll, INST_CLASS_DATA, 0, "Copy nuMBEr from all registers and place into the stack"),
    tInstLibEntry<tMethod>("swap-stk", &cHardwareMBE::Inst_SwitchStack, INST_CLASS_DATA, 0, "Toggle which stack is currently being used"),
    tInstLibEntry<tMethod>("swap-stk-top", &cHardwareMBE::Inst_SwapStackTop, INST_CLASS_DATA, 0, "Swap the values at the top of both stacks"),
    tInstLibEntry<tMethod>("swap", &cHardwareMBE::Inst_Swap, INST_CLASS_DATA, 0, "Swap the contents of ?BX? with ?CX?"),
    
    tInstLibEntry<tMethod>("shift-r", &cHardwareMBE::Inst_ShiftR, INST_CLASS_ARITHMETIC_LOGIC, 0, "Shift bits in ?BX? right by one (divide by two)"),
    tInstLibEntry<tMethod>("shift-l", &cHardwareMBE::Inst_ShiftL, INST_CLASS_ARITHMETIC_LOGIC, 0, "Shift bits in ?BX? left by one (multiply by two)"),
    tInstLibEntry<tMethod>("inc", &cHardwareMBE::Inst_Inc, INST_CLASS_ARITHMETIC_LOGIC, 0, "Increment ?BX? by one"),
    tInstLibEntry<tMethod>("dec", &cHardwareMBE::Inst_Dec, INST_CLASS_ARITHMETIC_LOGIC, 0, "Decrement ?BX? by one"),
    tInstLibEntry<tMethod>("zero", &cHardwareMBE::Inst_Zero, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to 0"),
    tInstLibEntry<tMethod>("one", &cHardwareMBE::Inst_One, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to 0"),
    tInstLibEntry<tMethod>("rand", &cHardwareMBE::Inst_Rand, INST_CLASS_ARITHMETIC_LOGIC, 0, "Set ?BX? to rand nuMBEr"),
    
    tInstLibEntry<tMethod>("add", &cHardwareMBE::Inst_Add, INST_CLASS_ARITHMETIC_LOGIC, 0, "Add BX to CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("sub", &cHardwareMBE::Inst_Sub, INST_CLASS_ARITHMETIC_LOGIC, 0, "Subtract CX from BX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("nand", &cHardwareMBE::Inst_Nand, INST_CLASS_ARITHMETIC_LOGIC, 0, "Nand BX by CX and place the result in ?BX?"),
    
    tInstLibEntry<tMethod>("IO", &cHardwareMBE::Inst_TaskIO, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "Output ?BX?, and input new nuMBEr back into ?BX?", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("input", &cHardwareMBE::Inst_TaskInput, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "Input new nuMBEr into ?BX?", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("output", &cHardwareMBE::Inst_TaskOutput, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "Output ?BX?", BEHAV_CLASS_ACTION),
    
    tInstLibEntry<tMethod>("mult", &cHardwareMBE::Inst_Mult, INST_CLASS_ARITHMETIC_LOGIC, 0, "Multiple BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("div", &cHardwareMBE::Inst_Div, INST_CLASS_ARITHMETIC_LOGIC, 0, "Divide BX by CX and place the result in ?BX?"),
    tInstLibEntry<tMethod>("mod", &cHardwareMBE::Inst_Mod, INST_CLASS_ARITHMETIC_LOGIC),
        
    // Flow Control Instructions
    tInstLibEntry<tMethod>("label", &cHardwareMBE::Inst_Label, INST_CLASS_FLOW_CONTROL, nInstFlag::LABEL),
    tInstLibEntry<tMethod>("search-seq-comp-s", &cHardwareMBE::Inst_Search_Seq_Comp_S, INST_CLASS_FLOW_CONTROL, 0, "Find complement template from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-comp-f", &cHardwareMBE::Inst_Search_Seq_Comp_F, INST_CLASS_FLOW_CONTROL, 0, "Find complement template forward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-comp-b", &cHardwareMBE::Inst_Search_Seq_Comp_B, INST_CLASS_FLOW_CONTROL, 0, "Find complement template backward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-direct-s", &cHardwareMBE::Inst_Search_Seq_Direct_S, INST_CLASS_FLOW_CONTROL, 0, "Find direct template from genome start and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-direct-f", &cHardwareMBE::Inst_Search_Seq_Direct_F, INST_CLASS_FLOW_CONTROL, 0, "Find direct template forward and move the flow head"),
    tInstLibEntry<tMethod>("search-seq-direct-b", &cHardwareMBE::Inst_Search_Seq_Direct_B, INST_CLASS_FLOW_CONTROL, 0, "Find direct template backward and move the flow head"),
    tInstLibEntry<tMethod>("search-lbl-direct-s", &cHardwareMBE::Inst_Search_Label_Direct_S, INST_CLASS_FLOW_CONTROL, 0, "Find direct label from genome start and move the flow head"),

    tInstLibEntry<tMethod>("mov-head", &cHardwareMBE::Inst_MoveHead, INST_CLASS_FLOW_CONTROL, 0, "Move head ?IP? to the flow head"),
    tInstLibEntry<tMethod>("jmp-head", &cHardwareMBE::Inst_JumpHead, INST_CLASS_FLOW_CONTROL, 0, "Move head ?Flow? by amount in ?CX? register"),
    tInstLibEntry<tMethod>("get-head", &cHardwareMBE::Inst_GetHead, INST_CLASS_FLOW_CONTROL, 0, "Copy the position of the ?IP? head into ?CX?"),
    tInstLibEntry<tMethod>("jump-gene", &cHardwareMBE::Inst_JumpGene, INST_CLASS_FLOW_CONTROL, 0, "Move execution to the specified nop sequence in whatever thread it's in"),
    tInstLibEntry<tMethod>("jump-behavior", &cHardwareMBE::Inst_JumpBehavior, INST_CLASS_FLOW_CONTROL, 0, "End execution of this behavior and jump to a new gene class."),
    tInstLibEntry<tMethod>("jump-thread", &cHardwareMBE::Inst_JumpThread, INST_CLASS_FLOW_CONTROL, 0, "End execution of this thread and jump to a new gene."),
    
    // Replication Instructions
    tInstLibEntry<tMethod>("alloc", &cHardwareMBE::Inst_Alloc, INST_CLASS_LIFECYCLE, 0, "Allocate maximum allowed space", BEHAV_CLASS_COPY),
    tInstLibEntry<tMethod>("copy", &cHardwareMBE::Inst_Copy, INST_CLASS_LIFECYCLE, 0, "Copy from read-head to write-head; advance both", BEHAV_CLASS_COPY),
    tInstLibEntry<tMethod>("divide", &cHardwareMBE::Inst_Divide, INST_CLASS_LIFECYCLE, nInstFlag::STALL, "Divide code between read and write heads.", BEHAV_CLASS_COPY),

    tInstLibEntry<tMethod>("repro", &cHardwareMBE::Inst_Repro, INST_CLASS_LIFECYCLE, nInstFlag::STALL, "Instantly reproduces the organism", BEHAV_CLASS_COPY),
    tInstLibEntry<tMethod>("die", &cHardwareMBE::Inst_Die, INST_CLASS_LIFECYCLE, nInstFlag::STALL, "Instantly kills the organism", BEHAV_CLASS_COPY),
    
    tInstLibEntry<tMethod>("if-copied-lbl-comp", &cHardwareMBE::Inst_IfCopiedCompLabel, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied complement of attached label"),
    tInstLibEntry<tMethod>("if-copied-lbl-direct", &cHardwareMBE::Inst_IfCopiedDirectLabel, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied direct match of the attached label"),
    tInstLibEntry<tMethod>("if-copied-seq-comp", &cHardwareMBE::Inst_IfCopiedCompSeq, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied complement of attached sequence"),
    tInstLibEntry<tMethod>("if-copied-seq-direct", &cHardwareMBE::Inst_IfCopiedDirectSeq, INST_CLASS_CONDITIONAL, 0, "Execute next if we copied direct match of the attached sequence"),
    
    // Thread Execution Control
    tInstLibEntry<tMethod>("wait-cond-equ", &cHardwareMBE::Inst_WaitCondition_Equal, INST_CLASS_OTHER, nInstFlag::STALL, ""),
    tInstLibEntry<tMethod>("wait-cond-less", &cHardwareMBE::Inst_WaitCondition_Less, INST_CLASS_OTHER, nInstFlag::STALL, ""),
    tInstLibEntry<tMethod>("wait-cond-gtr", &cHardwareMBE::Inst_WaitCondition_Greater, INST_CLASS_OTHER, nInstFlag::STALL, ""),
        
    // Movement and Navigation instructions
    tInstLibEntry<tMethod>("move", &cHardwareMBE::Inst_Move, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("get-north-offset", &cHardwareMBE::Inst_GetNorthOffset, INST_CLASS_ENVIRONMENT, 0, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("get-position-offset", &cHardwareMBE::Inst_GetPositionOffset, INST_CLASS_ENVIRONMENT, 0, "", BEHAV_CLASS_INPUT),    
    tInstLibEntry<tMethod>("get-northerly", &cHardwareMBE::Inst_GetNortherly, INST_CLASS_ENVIRONMENT, 0, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("get-easterly", &cHardwareMBE::Inst_GetEasterly, INST_CLASS_ENVIRONMENT, 0, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("zero-easterly", &cHardwareMBE::Inst_ZeroEasterly, INST_CLASS_ENVIRONMENT, 0, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("zero-northerly", &cHardwareMBE::Inst_ZeroNortherly, INST_CLASS_ENVIRONMENT, 0, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("zero-position-offset", &cHardwareMBE::Inst_ZeroPosOffset, INST_CLASS_ENVIRONMENT, 0, "", BEHAV_CLASS_INPUT),

    // Rotation
    tInstLibEntry<tMethod>("rotate-home", &cHardwareMBE::Inst_RotateHome, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("rotate-to-unoccupied-cell", &cHardwareMBE::Inst_RotateUnoccupiedCell, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("rotate-x", &cHardwareMBE::Inst_RotateX, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    
    // Resource and Topography Sensing
    tInstLibEntry<tMethod>("sense-resource-id", &cHardwareMBE::Inst_SenseResourceID, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT), 
    tInstLibEntry<tMethod>("sense-nest", &cHardwareMBE::Inst_SenseNest, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("sense-faced-habitat", &cHardwareMBE::Inst_SenseFacedHabitat, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("look-ahead", &cHardwareMBE::Inst_LookAhead, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("look-ahead-intercept", &cHardwareMBE::Inst_LookAheadIntercept, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("look-around", &cHardwareMBE::Inst_LookAround, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("look-around-intercept", &cHardwareMBE::Inst_LookAroundIntercept, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("look-ft", &cHardwareMBE::Inst_LookFT, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    tInstLibEntry<tMethod>("look-around-ft", &cHardwareMBE::Inst_LookAroundFT, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    
    tInstLibEntry<tMethod>("set-forage-target", &cHardwareMBE::Inst_SetForageTarget, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("set-ft-once", &cHardwareMBE::Inst_SetForageTargetOnce, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("set-rand-ft-once", &cHardwareMBE::Inst_SetRandForageTargetOnce, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("get-forage-target", &cHardwareMBE::Inst_GetForageTarget, INST_CLASS_ENVIRONMENT, 0, "", BEHAV_CLASS_INPUT),
    
    tInstLibEntry<tMethod>("collect-specific", &cHardwareMBE::Inst_CollectSpecific, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("get-res-stored", &cHardwareMBE::Inst_GetResStored, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
 
    // Opinion instructions.
    tInstLibEntry<tMethod>("set-opinion", &cHardwareMBE::Inst_SetOpinion, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("get-opinion", &cHardwareMBE::Inst_GetOpinion, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),

    // Grouping instructions
    tInstLibEntry<tMethod>("join-group", &cHardwareMBE::Inst_JoinGroup, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
    tInstLibEntry<tMethod>("get-group-id", &cHardwareMBE::Inst_GetGroupID, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    
    // Org Interaction instructions
    tInstLibEntry<tMethod>("get-faced-org-id", &cHardwareMBE::Inst_GetFacedOrgID, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),
    
    tInstLibEntry<tMethod>("teach-offspring", &cHardwareMBE::Inst_TeachOffspring, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION), 
    tInstLibEntry<tMethod>("learn-parent", &cHardwareMBE::Inst_LearnParent, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION), 

    // Control-type Instructions
    tInstLibEntry<tMethod>("scramble-registers", &cHardwareMBE::Inst_ScrambleReg, INST_CLASS_DATA, nInstFlag::STALL, "", BEHAV_CLASS_INPUT),

    // Predator-prey
    tInstLibEntry<tMethod>("attack-prey", &cHardwareMBE::Inst_AttackPrey, INST_CLASS_ENVIRONMENT, nInstFlag::STALL, "", BEHAV_CLASS_ACTION),
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

cHardwareMBE::cHardwareMBE(cAvidaContext& ctx, cWorld* world, cOrganism* in_organism, cInstSet* in_inst_set)
: cHardwareBase(world, in_organism, in_inst_set), m_sensor(world, in_organism)
{
  m_functions = s_inst_slib->GetFunctions();
  
  m_no_cpu_cycle_time = m_world->GetConfig().NO_CPU_CYCLE_TIME.Get();
  
  const Genome& in_genome = in_organism->GetGenome();
  ConstInstructionSequencePtr in_seq_p;
  in_seq_p.DynamicCastFrom(in_genome.Representation());
  const InstructionSequence& in_seq = *in_seq_p;
  
  main_memory = in_seq;                     // Initialize main org memory and heads...
  
  m_threads.Resize(0);
  m_cur_thread = 0;
  m_use_avatar = m_world->GetConfig().USE_AVATARS.Get();
  
  m_bps.Resize(NUM_BEHAVIORS);
  CreateBPThreads();                     // classify the genome into genes
  Reset(ctx);                            // Setup the rest of the hardware...
  ResizeCostArrays(m_threads.GetSize());
}

void cHardwareMBE::internalReset()
{
  m_spec_die = false;
  
  // heads reset...redundant with constructor, but constructor not called on divide reset
  mHeads[mRH].Reset(this, 0);
  mHeads[mIP].Reset(this, 0);
  mHeads[mFH].Reset(this, 0);
  child_memory.Resize(1);
  mHeads[mWH].Reset(this, 1);

  m_global_stack.Clear();
  m_cur_thread = 0;
  
  m_waiting_threads = 0;
  m_cur_thread = 0;
  m_cur_behavior = 0;
  
  m_sensor.Reset();
  m_sense_age = 0;

  m_cycle_count = 0;
  m_last_output = 0;
  m_read_label.Clear();
  m_read_seq.Clear();

  // associate each thread and it's heads with the appropriate memory space id
  for (int i = 0; i < m_threads.GetSize(); i++) m_threads[i].Reset(this, i + 2);
  
  m_has_alloc = false;
  m_has_copied_end = false;
  m_reading_label = false;
  m_reading_seq = false;

  m_executedmatchstrings = false;
}

void cHardwareMBE::internalResetOnFailedDivide()
{
	internalReset();
}

void cHardwareMBE::cBehavThread::operator=(const cBehavThread& in_thread)
{
  mem_id = in_thread.mem_id;
  thread_class = in_thread.thread_class;
  start = in_thread.start;
  end = in_thread.end;
  for (int i = 0; i < NUM_TH_HEADS; i++) thHeads[i] = in_thread.thHeads[i];
  
  stack.Clear();
  cur_stack = 0;
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i].Clear();

  active = in_thread.active;
  wait_greater = in_thread.wait_greater;
  wait_equal = in_thread.wait_equal;
  wait_less = in_thread.wait_less;
  wait_reg = in_thread.wait_reg;
  wait_dst = in_thread.wait_dst;
  wait_value = in_thread.wait_value;
  
  next_label = in_thread.next_label;
}

void cHardwareMBE::cBehavThread::Reset(cHardwareMBE* in_hardware, int in_id)
{
  mem_id = in_id;
  for (int i = 0; i < NUM_TH_HEADS; i++) thHeads[i].Reset(in_hardware, mem_id);
  for (int i = 0; i < NUM_REGISTERS; i++) reg[i].Clear();

  stack.Clear();
  cur_stack = 0;

  active = true;
  next_label.Clear();
}

// This function processes the very next command in the genome, and is made
// to be as optimized as possible.  This is the heart of avida.

bool cHardwareMBE::SingleProcess(cAvidaContext& ctx, bool speculative)
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
  
  int num_active = 0;
  for (int i = 0; i < m_threads.GetSize(); i++) if (m_threads[i].active) num_active++;
  assert(num_active == (m_threads.GetSize() - m_waiting_threads));
  assert(num_active > 0);
  
  const int max_exec_count = 20;          // min tot num for equ from Nature '03  = 19
  int tot_count = 0;
  Apto::Array<bool> bp_used(NUM_BEHAVIORS);
  bp_used.SetAll(false);
  bool all_used = false;
  
  while (!all_used) {
    if (!m_threads[m_cur_thread].active) { IncThread(); continue; }
    tot_count++;
    
    m_advance_ip = true;
    cHeadCPU& ip = getThIP();
    bool end_cyc = false;
    if (ip.GetPosition() >= ip.GetMemSize() - 1) end_cyc = true;
    Adjust(getThIP(), thIP);
    
//    if (m_organism->GetID() == 0 && m_world->GetStats().GetUpdate() >= 0) cout << " org: " << m_organism->GetID() << " thread: " << m_cur_thread << " mem_space: " << ip.GetMemSpace() << " ip_position: " << ip.GetPosition() << " thread_end: " << m_threads[m_cur_thread].end << " thread_size: " << ip.GetMemSize() << " inst: " << m_inst_set->GetInstLib()->Get(m_inst_set->GetLibFunctionIndex(ip.GetInst())).GetName() <<  " cell: " << m_organism->GetOrgInterface().GetAVCellID() << endl;

    // if you exceed the per cycle exec max, you're done
    if (tot_count > max_exec_count) break;
    // only one 'designated' behavior instruction per class per cycle
    BehavClass BEHAV = m_inst_set->GetInstLib()->Get(m_inst_set->GetLibFunctionIndex(ip.GetInst())).GetBehavClass();
    if (BEHAV == BEHAV_CLASS_ACTION || BEHAV == BEHAV_CLASS_COPY || BEHAV == BEHAV_CLASS_INPUT) {
      if (bp_used[BEHAV]) break;
      else bp_used[BEHAV] = true;
    }

    // And proceed with standard execution...
    
    // Print the status of this CPU at each step...
    if (m_tracer) m_tracer->TraceHardware(ctx, *this);
    
    // Find the instruction to be executed
    const Instruction cur_inst = ip.GetInst();
    if (speculative && (m_spec_die || m_inst_set->ShouldStall(cur_inst))) {
      // Speculative instruction reject, flush and return
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
      if (m_advance_ip == true) Advance(getThIP(), thIP);
      
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
      if (m_microtrace || m_topnavtrace) {
        RecordMicroTrace(cur_inst);
        if (m_topnavtrace) RecordNavTrace(m_use_avatar);
      }
    }
    if (phenotype.GetToDelete()) break;
    
    // check for continued execution
//    IncThread();
    all_used = (bp_used[BEHAV_CLASS_ACTION] && bp_used[BEHAV_CLASS_INPUT] && bp_used[BEHAV_CLASS_COPY]);
    if (end_cyc) { IncThread(); continue; }
    
  }  // end per BEHAV exec
  
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
bool cHardwareMBE::SingleProcess_ExecuteInst(cAvidaContext& ctx, const Instruction& cur_inst)
{
  // Copy Instruction locally to handle stochastic effects
  Instruction actual_inst = cur_inst;
  
#ifdef EXECUTION_ERRORS
  // If there is an execution error, execute a random instruction.
  if (m_organism->TestExeErr()) actual_inst = m_inst_set->GetRandomInst(ctx);
#endif /* EXECUTION_ERRORS */
  
  // Get a pointer to the corrisponding method...
  int inst_idx = m_inst_set->GetLibFunctionIndex(actual_inst);
  
  // Mark the instruction as executed in the main memory
  getIP().SetFlagExecuted();
	
  // instruction execution count incremeneted
  m_organism->GetPhenotype().IncCurInstCount(actual_inst.GetOp());
  
  // And execute it.
  m_from_sensor = false;
  const bool exec_success = (this->*(m_functions[inst_idx]))(ctx);
  
/*  if (exec_success & m_from_sensor) {
    m_organism->GetPhenotype().IncCurFromSensorInstCount(actual_inst.GetOp());
    if ((m_sense_age == m_cycle_count) || (m_sense_age == (m_cycle_count - 1))) {
      // m_organism->UpdateMerit(m_organism->GetPhenotype().GetMerit().GetDouble() + 1);
      m_organism->GetPhenotype().SetCurBonus(m_organism->GetPhenotype().GetCurBonus() + 1);
    }
  } */
  
  // decremenet if the instruction was not executed successfully
  if (exec_success == false) {
    m_organism->GetPhenotype().DecCurInstCount(actual_inst.GetOp());
  }
  return exec_success;
}

void cHardwareMBE::CreateBPThreads()
{
  for (int i = 0; i < NUM_BEHAVIORS; i++) m_bps[i].bp_thread_ids.Resize(0);

  int prev_behav_class = -1;
  int cur_behav_class = 0;
  
  const Genome& in_genome = m_organism->GetGenome();
  ConstInstructionSequencePtr in_seq_p;
  in_seq_p.DynamicCastFrom(in_genome.Representation());
  const InstructionSequence& sequence = *in_seq_p;
  
  cHeadCPU& ip = mHeads[mIP];
  ip.Reset(this, 0);
  int offset = 0;
  bool end_unset = false;
  bool created_gene_already = false;
  while (offset < sequence.GetSize()) {
    if (end_unset) m_threads[m_cur_thread].end = offset - 1;
    bool new_gene = false;
    bool end_gene = false;
    bool set_end = true;
//    cout << m_organism->GetID() << " " << m_inst_set->GetInstLib()->Get(m_inst_set->GetLibFunctionIndex(ip.GetInst(offset))).GetName() << endl;
    BehavClass inst_class = m_inst_set->GetInstLib()->Get(m_inst_set->GetLibFunctionIndex(ip.GetInst(offset))).GetBehavClass();
    if (inst_class < 3) cur_behav_class = inst_class;
    else if (inst_class == BEHAV_CLASS_BREAK) {
      cur_behav_class = GetNextGeneClass(offset + 1, sequence.GetSize(), cur_behav_class);
      if (!created_gene_already) new_gene = true;  // prevent start gene following an end gene
      else set_end = false;
    }
    else if (inst_class == BEHAV_CLASS_END_GENE) {
      end_gene = true;
    }
    if ((cur_behav_class != prev_behav_class) && !created_gene_already) new_gene = true;
    if (created_gene_already) created_gene_already = false;

    if (end_gene) {
      if (prev_behav_class == -1) {   // if first inst in genome is 'end gene'...
        m_threads.Resize(1);
        m_threads[0].start = offset;
        m_threads[0].thread_class = 0;
        m_bps[0].bp_thread_ids.Resize(1);
        m_bps[0].bp_thread_ids[0] = 0;
        m_threads[0].end = offset;
        
        m_cur_thread++;
        m_threads.Resize(m_threads.GetSize() + 1);
        m_threads[m_cur_thread].start = offset;
        m_threads[m_cur_thread].thread_class = cur_behav_class;
        m_bps[m_threads[m_cur_thread].thread_class].bp_thread_ids.Resize(m_bps[m_threads[m_cur_thread].thread_class].bp_thread_ids.GetSize() + 1);
        m_bps[m_threads[m_cur_thread].thread_class].bp_thread_ids[m_bps[m_threads[m_cur_thread].thread_class].bp_thread_ids.GetSize() - 1] = m_threads.GetSize() - 1;

        cur_behav_class = GetNextGeneClass(offset + 1, sequence.GetSize(), cur_behav_class);
        prev_behav_class = cur_behav_class;
        offset++;
        continue;
      }
      m_threads[m_cur_thread].end = offset;
      assert(m_threads[m_cur_thread].end >= m_threads[m_cur_thread].start);
      cur_behav_class = GetNextGeneClass(offset + 1, sequence.GetSize(), cur_behav_class);
      new_gene = true;
      offset++;
    }
    if (new_gene) {
      if (prev_behav_class != -1) {
        if ((!end_gene && set_end) || end_unset) { m_threads[m_cur_thread].end = offset - 1; end_unset = false; }
        else end_unset = true;
/*
  /// debug
        
        //     cout << " created_gene_already " << created_gene_already << " " << m_cur_thread << " start " << m_threads[m_cur_thread].start << " end: " << m_threads[m_cur_thread].end << endl;
        if (m_organism->GetID() == 24 && m_cur_thread == 1 && m_threads[m_cur_thread].start == 6) {
          bool rubbish;
          rubbish = true;
        }
        
        
        cout << m_organism->GetID() << " thread: " << m_cur_thread << " start: " << m_threads[m_cur_thread].start << " end: " << m_threads[m_cur_thread].end << " inst: " << m_inst_set->GetInstLib()->Get(m_inst_set->GetLibFunctionIndex(ip.GetInst(offset))).GetName() << endl;
  /// end debug
  */
        assert(m_threads[m_cur_thread].end >= m_threads[m_cur_thread].start);
        m_cur_thread++;
      }
      m_threads.Resize(m_threads.GetSize() + 1);
      m_threads[m_cur_thread].start = offset;
      m_threads[m_cur_thread].thread_class = cur_behav_class;
      m_bps[m_threads[m_cur_thread].thread_class].bp_thread_ids.Resize(m_bps[m_threads[m_cur_thread].thread_class].bp_thread_ids.GetSize() + 1);
      m_bps[m_threads[m_cur_thread].thread_class].bp_thread_ids[m_bps[m_threads[m_cur_thread].thread_class].bp_thread_ids.GetSize() - 1] = m_threads.GetSize() - 1;
      prev_behav_class = cur_behav_class;
      if (end_gene) created_gene_already = true;
      else created_gene_already = false;
    }
    else created_gene_already = false;
    //     cout << " created_gene_already " << created_gene_already << " " << m_cur_thread << " start " << m_threads[m_cur_thread].start << " end: " << m_threads[m_cur_thread].end << endl;
    if (!(end_gene && new_gene)) offset++;
  }
  m_threads[m_cur_thread].end = sequence.GetSize() - 1; // for very last gene
  if (m_threads[m_cur_thread].start == sequence.GetSize()) {  // end_gene was last instruction in genome
    m_bps[m_threads[m_cur_thread].thread_class].bp_thread_ids.Pop();
    m_threads.Pop();
  }
  
  
  
  
  
/*
  /// debug

//     if (m_threads.GetSize() > 0) cout << "thread: 2 start: " << m_threads[2].start << " end: " << m_threads[2].end << endl;
    // Reset the threads, associating them with the correct memory space

  ip.Reset(this, 0);
  for (int i = 0; i < m_threads.GetSize(); i++) {
    int pos = 0;
    for (int j = m_threads[i].start; j < m_threads[i].end + 1; j++) {
      cout << m_organism->GetID() << " thread: " << i << " start: " << m_threads[i].start << " end: " << m_threads[i].end << " inst: " << m_inst_set->GetInstLib()->Get(m_inst_set->GetLibFunctionIndex(ip.GetInst(j))).GetName() << endl;
      pos++;
    }
    // Reset the threads, associating them with the correct memory space
    m_threads[i].Reset(this, i + 2);
  }
  /// end debug
*/  
  
  
  
  

  // copy the appropriate instructions into the new thread memory spaces
  ip.Reset(this, 0);
  for (int i = 0; i < m_threads.GetSize(); i++) {
    int mem_size = 1 + m_threads[i].end - m_threads[i].start;
//     cout << i << " start " << m_threads[i].start << " end: " << m_threads[i].end << endl;
    m_threads[i].thread_mem.Reset(mem_size);
    int pos = 0;
    for (int j = m_threads[i].start; j < m_threads[i].end + 1; j++) {
      m_threads[i].thread_mem[pos] = ip.GetInst(j);
      pos++;
    }
    // Reset the threads, associating them with the correct memory space
    m_threads[i].Reset(this, i + 2);
  }
  ip.Reset(this, 0);
}

int cHardwareMBE::PreclassNewGeneBehavior(int cur_class, int pos)
{
  int next_behav = cur_class;  
  // can't use find modified register as that would move thIP
  if (pos < main_memory.GetSize() && m_inst_set->IsNop(main_memory[pos])) {
    next_behav = m_inst_set->GetNopMod(main_memory[pos]);
    return (next_behav % NUM_BEHAVIORS) % NUM_REGISTERS;
  }
  return (cur_class + 1) % NUM_BEHAVIORS;
}

int cHardwareMBE::GetNextGeneClass(int pos, int seq_size, int cur_class)
{
  int gene_class = 3;
  int inst_checks = 0;
  while (gene_class > 2) {
    if (pos >= seq_size) pos = 0;
    gene_class = m_inst_set->GetInstLib()->Get(m_inst_set->GetLibFunctionIndex(getIP().GetInst(pos))).GetBehavClass();
    if (gene_class == BEHAV_CLASS_BREAK) gene_class = PreclassNewGeneBehavior(cur_class, pos + 1);
    pos++;
    inst_checks++;
    if (inst_checks > seq_size) return cur_class;
  }
  return gene_class;
}

BehavClass cHardwareMBE::GetBehavClass(int class_id)
{
  if (class_id == 0) return BEHAV_CLASS_INPUT;
  if (class_id == 1) return BEHAV_CLASS_ACTION;
  if (class_id == 2) return BEHAV_CLASS_COPY;
  if (class_id == 3) return BEHAV_CLASS_NONE;
  if (class_id == 4) return BEHAV_CLASS_BREAK;
  return BEHAV_CLASS_END_GENE;
};

void cHardwareMBE::ProcessBonusInst(cAvidaContext& ctx, const Instruction& inst)
{
  // Mark this organism as running...
  bool prev_run_state = m_organism->IsRunning();
  m_organism->SetRunning(true);
  
  if (m_tracer) m_tracer->TraceHardware(ctx, *this, true);
  
  SingleProcess_ExecuteInst(ctx, inst);
  
  m_organism->SetRunning(prev_run_state);
}

void cHardwareMBE::PrintStatus(ostream& fp)
{
  fp << "CPU CYCLE:" << m_organism->GetPhenotype().GetCPUCyclesUsed() << " ";
  fp << "THREAD:" << m_cur_thread << "  ";
  fp << "IP:" << getIP().GetPosition() << " (" << GetInstSet().GetName(IP().GetInst()) << ")" << endl;
  
  
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
  
  fp << "  Mem (" << main_memory.GetSize() << "):"
  << "  " << main_memory.AsString()
  << endl;
  
  fp.flush();
}

void cHardwareMBE::SetupMiniTraceFileHeader(Avida::Output::File& df, const int gen_id, const Apto::String& genotype)
{
  const Genome& in_genome = m_organism->GetGenome();
  ConstInstructionSequencePtr in_seq_p;
  in_seq_p.DynamicCastFrom(in_genome.Representation());
  const InstructionSequence& in_seq = *in_seq_p;

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

void cHardwareMBE::PrintMiniTraceStatus(cAvidaContext& ctx, ostream& fp)
{
  // basic status info
  fp << m_cycle_count << " ";
  fp << m_world->GetStats().GetUpdate() << " ";
  for (int i = 0; i < NUM_REGISTERS; i++) {
    sInternalValue& reg = m_threads[m_cur_thread].reg[i];
    fp << GetRegVal(i) << " ";
    fp << "(" << reg.originated << ") ";
  }    
  // genome loc info
  fp << m_cur_thread << " ";
  fp << getIP().GetPosition() << " ";
  fp << getRH().GetPosition() << " ";
  fp << getWH().GetPosition()  << " ";
  fp << getFH().GetPosition()   << " ";
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
  cString next_name(GetInstSet().GetName(IP().GetInst()));
  fp << next_name << " ";
  // any trailing nops (up to NUM_REGISTERS)
  cCPUMemory& memory = main_memory;
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

void cHardwareMBE::PrintMiniTraceSuccess(ostream& fp, const int exec_sucess)
{
  fp << exec_sucess;
  fp << endl;
  fp.flush();
}

cHeadCPU cHardwareMBE::FindLabelStart(bool mark_executed)
{
  cHeadCPU& ip = getThIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cCPUMemory& memory = m_threads[m_cur_thread].thread_mem;
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
          // mark executed in the main memory space
          const int true_start = m_threads[m_cur_thread].start + start;
          for (int i = 0; i < size_matched && i < max; i++) main_memory.SetFlagExecuted(true_start + i);
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

int cHardwareMBE::FindNopSequenceOrgStart(bool mark_executed)
{
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return m_cur_thread;
  
  cCPUMemory& memory = main_memory;
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
        // Return thread with the sequence
        if (mark_executed) {
          const int start = pos - size_matched;
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get();
          // mark executed in the main memory space
          for (int i = 0; i < size_matched && i < max; i++) main_memory.SetFlagExecuted(start + i);
        }
        for (int i = 0; i < m_threads.GetSize(); i++) {
          if (m_threads[i].start <= pos - 1 && m_threads[i].end >= pos - 1) return i;
        }
      }
    }
    pos++;
  }
  
  // Return start point if not found
  return m_cur_thread;
}

cHeadCPU cHardwareMBE::FindNopSequenceStart(bool mark_executed)
{
  cHeadCPU& ip = getThIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cCPUMemory& memory = m_threads[m_cur_thread].thread_mem;
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
          // mark executed in the main memory space
          const int true_start = m_threads[m_cur_thread].start + start;
          for (int i = 0; i < size_matched && i < max; i++) main_memory.SetFlagExecuted(true_start + i);
        }
        return cHeadCPU(this, pos - 1, ip.GetMemSpace());
      }
    }
    pos++;
  }
  
  // Return start point if not found
  return ip;
}

cHeadCPU cHardwareMBE::FindLabelForward(bool mark_executed)
{
  cHeadCPU& ip = getThIP();
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
          // mark executed in the main memory space
          for (int i = 0; i < size_matched && i < max; i++, pos++) {
          const int true_start = m_threads[m_cur_thread].start;
            main_memory.SetFlagExecuted(true_start + pos.GetPosition());
          }
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

cHeadCPU cHardwareMBE::FindLabelBackward(bool mark_executed)
{
  cHeadCPU& ip = getThIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cHeadCPU lpos(ip);
  cHeadCPU pos(ip);
  lpos--;
  
  while (pos.GetPosition() != ip.GetPosition()) {
    if (m_inst_set->IsLabel(lpos.GetInst())) { // starting label found
      pos.Set(lpos.GetPosition(), ip.GetMemSpace());
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
          // mark executed in the main memory space
          for (int i = 0; i < size_matched && i < max; i++, lpos++) {
          const int true_start = m_threads[m_cur_thread].start;
            main_memory.SetFlagExecuted(true_start + lpos.GetPosition());
          }
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

cHeadCPU cHardwareMBE::FindNopSequenceForward(bool mark_executed)
{
  cHeadCPU& ip = getThIP();
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
          pos.Set(label_start, ip.GetMemSpace());
          const int max = m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get();
          // mark executed in the main memory space
          for (int i = 0; i < size_matched && i < max; i++, pos++) {
          const int true_start = m_threads[m_cur_thread].start;
            main_memory.SetFlagExecuted(true_start + pos.GetPosition());
          }
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

cHeadCPU cHardwareMBE::FindNopSequenceBackward(bool mark_executed)
{
  cHeadCPU& ip = getThIP();
  const cCodeLabel& search_label = GetLabel();
  
  // Make sure the label is of size > 0.
  if (search_label.GetSize() == 0) return ip;
  
  cHeadCPU lpos(ip);
  cHeadCPU pos(ip);
  lpos--;
  
  while (pos.GetPosition() != ip.GetPosition()) {
    if (m_inst_set->IsNop(pos.GetInst())) { // starting label found
      pos.Set(lpos.GetPosition(), ip.GetMemSpace());
      
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
          // mark executed in the main memory space
          for (int i = 0; i < size_matched && i < max; i++, lpos++) {
          const int true_start = m_threads[m_cur_thread].start;
            main_memory.SetFlagExecuted(true_start + lpos.GetPosition());
          }
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

void cHardwareMBE::ReadInst(Instruction in_inst)
{
  bool is_nop = m_inst_set->IsNop(in_inst);
  
  if (m_inst_set->IsLabel(in_inst)) {
    GetReadLabel().Clear();
    m_reading_label = true;
  } else if (m_reading_label && is_nop) {
    GetReadLabel().AddNop(in_inst.GetOp());
  } else {
    GetReadLabel().Clear();
    m_reading_label = false;
  }
  
  if (!m_reading_seq && is_nop) {
    GetReadSequence().AddNop(in_inst.GetOp());
    m_reading_seq = true;
  } else if (m_reading_seq && is_nop) {
    GetReadSequence().AddNop(in_inst.GetOp());
  } else {
    GetReadSequence().Clear();
    m_reading_seq = false;
  }
}

// This function looks at the current position in the info of a creature,
// and sets the next_label to be the sequence of nops which follows.  The
// instruction pointer is left on the last line of the label found.

void cHardwareMBE::ReadLabel(int max_size)
{
  int count = 0;
  cHeadCPU * inst_ptr = &( getThIP() );
  
  GetLabel().Clear();
  
  while (m_inst_set->IsNop(inst_ptr->GetNextInst()) &&
         (count < max_size)) {
    count++;
    inst_ptr->Advance();
    GetLabel().AddNop(m_inst_set->GetNopMod(inst_ptr->GetInst()));
    
    // If this is the first line of the template, mark it executed in the main memory space
    if (GetLabel().GetSize() <=	m_world->GetConfig().MAX_LABEL_EXE_SIZE.Get()) {
      const int true_start = m_threads[m_cur_thread].start;
      main_memory.SetFlagExecuted(true_start + inst_ptr->GetPosition());
    }
  }
  MirrorHeads(getThIP(), thIP);
}

////////////////////////////
//  Instruction Helpers...
////////////////////////////

inline int cHardwareMBE::FindModifiedRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(getThIP().GetNextInst())) {
    Advance(getThIP(), thIP);
    default_register = m_inst_set->GetNopMod(getThIP().GetInst());
    getIP().SetFlagExecuted();
  }
  return default_register;
}

inline int cHardwareMBE::FindModifiedNextRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(getThIP().GetNextInst())) {
    Advance(getThIP(), thIP);
    default_register = m_inst_set->GetNopMod(getThIP().GetInst());
    getIP().SetFlagExecuted();
  } else {
    default_register = (default_register + 1) % NUM_REGISTERS;
  }
  return default_register;
}

inline int cHardwareMBE::FindModifiedPreviousRegister(int default_register)
{
  assert(default_register < NUM_REGISTERS);  // Reg ID too high.
  
  if (m_inst_set->IsNop(getThIP().GetNextInst())) {
    Advance(getThIP(), thIP);
    default_register = m_inst_set->GetNopMod(getThIP().GetInst());
    getIP().SetFlagExecuted();
  } else {
    default_register = (default_register + NUM_REGISTERS - 1) % NUM_REGISTERS;
  }
  return default_register;
}

inline int cHardwareMBE::FindModifiedHead(int default_head)
{
  if (m_inst_set->IsNop(getThIP().GetNextInst())) {
    Advance(getThIP(), thIP);
    default_head = m_inst_set->GetNopMod(getThIP().GetInst());
    getIP().SetFlagExecuted();
  }
  int max_heads = 2;
  if (GetCurrBehav() == BEHAV_CLASS_COPY) max_heads = 6;

  if (max_heads == 2 && default_head > 0) return 1;
  if (max_heads == 6 && default_head > 5) return 1;
  return default_head;
}

inline int cHardwareMBE::FindNextRegister(int base_reg)
{
  return (base_reg + 1) % NUM_REGISTERS;
}

int cHardwareMBE::calcCopiedSize(const int parent_size, const int child_size)
{
  int copied_size = 0;
  for (int i = 0; i < child_memory.GetSize(); i++) {
    if (child_memory.FlagCopied(i)) copied_size++;
	}
  return copied_size;
}  

bool cHardwareMBE::Divide_Main(cAvidaContext& ctx, int child_mem_space, int write_head_pos, double mut_multiplier)
{
  // Make sure the memory space we're using exists
  if (child_memory.GetSize() <= 0) return false;
  
  // Make sure this divide will produce a viable offspring.
  m_cur_offspring = child_mem_space; // save current child memory space for use by dependent functions (e.g. calcCopiedSize())
  if (!Divide_CheckViable(ctx, main_memory.GetSize(), write_head_pos)) return false;
  
  // Since the divide will now succeed, set up the information to be sent to the new organism
  child_memory.Resize(write_head_pos);
  
  InstructionSequencePtr offspring_seq(new InstructionSequence(child_memory));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)m_inst_set->GetInstSetName());
  Genome offspring(GetType(), props, offspring_seq);
  
  m_organism->OffspringGenome() = offspring;
	
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
	
  bool parent_alive = m_organism->ActivateDivide(ctx);
  child_memory = InstructionSequence("a");
	
  // Division Methods:
  // 0 - DIVIDE_METHOD_OFFSPRING - Create a child, leave parent state untouched.
  // 1 - DIVIDE_METHOD_SPLIT - Create a child, completely reset state of parent.
  // 2 - DIVIDE_METHOD_BIRTH - Create a child, reset state of parent's current behavioral process (register) and thread (heads) only.
  
  if (parent_alive) { // If the parent is no longer alive, all of this is moot
    switch (m_world->GetConfig().DIVIDE_METHOD.Get()) {
      case DIVIDE_METHOD_SPLIT:
        Reset(ctx);  // This will wipe out all parasites on a divide.
        ResizeCostArrays(m_threads.GetSize());
        break;
        
      case DIVIDE_METHOD_BIRTH:
        // Reset only the calling thread's state
        m_threads[m_cur_thread].thHeads[thFH].Reset(this, 0);
        m_threads[m_cur_thread].thHeads[thIP].Reset(this, 0);
       for(int x = 0; x < NUM_REGISTERS; x++) setInternalValue(x, 0, false);
        if (m_world->GetConfig().INHERIT_MERIT.Get() == 0) m_organism->GetPhenotype().ResetMerit();
        break;
        
      case DIVIDE_METHOD_OFFSPRING:
      default:
        break;
		}
		m_advance_ip = false;
	}
  return true;
}

void cHardwareMBE::checkWaitingThreads(int cur_thread, int reg_num)
{
    for (int i = 0; i < m_threads.GetSize(); i++) {
      // if not current thread...and not active...and has a wait condition in current reg...
      if (!(i == cur_thread) && !m_threads[i].active && int(m_threads[i].wait_reg) == reg_num) {
        int wait_value = m_threads[i].wait_value;            // ...the value we're waiting for
        int check_value = m_threads[m_cur_thread].reg[reg_num].value;      
        if ((m_threads[i].wait_greater && check_value > wait_value) ||
            (m_threads[i].wait_equal && check_value == wait_value) ||
            (m_threads[i].wait_less && check_value < wait_value)) {
          
          // Wake up the thread with matched condition
          m_threads[i].active = true;
          m_waiting_threads--;
          
          // Set register in the behavioral process with the woken thread to be the check value
          sInternalValue& dest = m_threads[i].reg[m_threads[i].wait_dst];
          dest.value = check_value;
          dest.from_env = false;
          dest.originated = m_cycle_count;
          dest.oldest_component = m_threads[i].reg[reg_num].oldest_component;
          dest.env_component = m_threads[i].reg[reg_num].env_component;
          
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
bool cHardwareMBE::Inst_IdThread(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, m_cur_thread, false);
  return true;
}

bool cHardwareMBE::Inst_StartGene(cAvidaContext& ctx)
{
  return true;
}

bool cHardwareMBE::Inst_EndGene(cAvidaContext& ctx)
{
  return true;
}

bool cHardwareMBE::Inst_KillGene(cAvidaContext& ctx)
{
  if (SpareThreads()) KillThread();
  return true;
}

bool cHardwareMBE::Inst_Label(cAvidaContext&)
{
  ReadLabel();
  return true;
}

bool cHardwareMBE::Inst_IfNEqu(cAvidaContext&) // Execute next if bx != ?cx?
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegVal(op1) == GetRegVal(op2)) Advance(getThIP(), thIP);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  return true;
}

bool cHardwareMBE::Inst_IfLess(cAvidaContext&) // Execute next if ?bx? < ?cx?
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  if (GetRegVal(op1) >=  GetRegVal(op2)) Advance(getThIP(), thIP);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  return true;
}

bool cHardwareMBE::Inst_IfNotZero(cAvidaContext&)  // Execute next if ?bx? != 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegVal(op1) == 0) Advance(getThIP(), thIP);
  m_from_sensor = FromSensor(op1);
  return true;
}
bool cHardwareMBE::Inst_IfEqualZero(cAvidaContext&)  // Execute next if ?bx? == 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegVal(op1) != 0) Advance(getThIP(), thIP);
  m_from_sensor = FromSensor(op1);
  return true;
}
bool cHardwareMBE::Inst_IfGreaterThanZero(cAvidaContext&)  // Execute next if ?bx? > 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegVal(op1) <= 0) Advance(getThIP(), thIP);
  m_from_sensor = FromSensor(op1);
  return true;
}

bool cHardwareMBE::Inst_IfLessThanZero(cAvidaContext&)  // Execute next if ?bx? < 0
{
  const int op1 = FindModifiedRegister(rBX);
  if (GetRegVal(op1) >= 0) Advance(getThIP(), thIP);
  m_from_sensor = FromSensor(op1);
  return true;
}


bool cHardwareMBE::Inst_IfGtrX(cAvidaContext&)       // Execute next if BX > X; X value set according to NOP label
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
  
  if (GetRegVal(rBX) <= valueToCompare) Advance(getThIP(), thIP);
  m_from_sensor = FromSensor(rBX);
  
  return true;
}

bool cHardwareMBE::Inst_IfEquX(cAvidaContext&)       // Execute next if BX == X; X value set according to NOP label
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
  
  if (GetRegVal(rBX) != valueToCompare) Advance(getThIP(), thIP);
  m_from_sensor = FromSensor(rBX);
  
  return true;
}

bool cHardwareMBE::Inst_Pop(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  sInternalValue pop = stackPop();
  setInternalValue(reg_used, pop.value, pop);
  return true;
}

bool cHardwareMBE::Inst_Push(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  getStack(m_threads[m_cur_thread].cur_stack).Push(m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareMBE::Inst_PopAll(cAvidaContext&)
{
  int reg_used = FindModifiedRegister(rBX);
  bool any_from_sensor = false;
  for (int i = 0; i < NUM_REGISTERS; i++) {
    if (FromSensor(reg_used)) any_from_sensor = true;
    sInternalValue pop = stackPop();
    setInternalValue(reg_used, pop.value, pop);
    reg_used++;
    if (reg_used == NUM_REGISTERS) reg_used = 0;
  }
  m_from_sensor = any_from_sensor;
  return true;
}

bool cHardwareMBE::Inst_PushAll(cAvidaContext&)
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

bool cHardwareMBE::Inst_SwitchStack(cAvidaContext&) { switchStack(); return true; }

bool cHardwareMBE::Inst_SwapStackTop(cAvidaContext&)
{
  sInternalValue v0 = getStack(0).Pop();
  sInternalValue v1 = getStack(1).Pop();
  getStack(0).Push(v1);
  getStack(1).Push(v0);
  return true;
}

bool cHardwareMBE::Inst_Swap(cAvidaContext&)
{
  const int op1 = FindModifiedRegister(rBX);
  const int op2 = FindModifiedNextRegister(op1);
  sInternalValue v1 = m_threads[m_cur_thread].reg[op1];
  m_threads[m_cur_thread].reg[op1] = m_threads[m_cur_thread].reg[op2];
  m_threads[m_cur_thread].reg[op2] = v1;
  return true;
}

bool cHardwareMBE::Inst_ShiftR(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value >> 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareMBE::Inst_ShiftL(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value << 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}


bool cHardwareMBE::Inst_Inc(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value + 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareMBE::Inst_Dec(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  setInternalValue(reg_used, m_threads[m_cur_thread].reg[reg_used].value - 1, m_threads[m_cur_thread].reg[reg_used]);
  return true;
}

bool cHardwareMBE::Inst_Zero(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, 0, false);
  return true;
}

bool cHardwareMBE::Inst_One(cAvidaContext&)
{
//  if (m_organism->GetID() == 0 && m_world->GetStats().GetUpdate() >= 0) { cout << "fed!" << endl; }
  const int reg_used = FindModifiedRegister(rBX);
  setInternalValue(reg_used, 1, false);
  return true;
}

bool cHardwareMBE::Inst_Rand(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  int randsign = ctx.GetRandom().GetUInt(0,2) ? -1 : 1;
  setInternalValue(reg_used, ctx.GetRandom().GetInt(INT_MAX) * randsign, false);
  return true;
}

bool cHardwareMBE::Inst_Add(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value + r2.value, r1, r2);
  return true;
}

bool cHardwareMBE::Inst_Sub(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value - r2.value, r1, r2);
  return true;
}

bool cHardwareMBE::Inst_Mult(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, r1.value * r2.value, r1, r2);
  return true;
}

bool cHardwareMBE::Inst_Div(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
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

bool cHardwareMBE::Inst_Mod(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
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

bool cHardwareMBE::Inst_Nand(cAvidaContext&)
{
  const int dst = FindModifiedRegister(rBX);
  const int op1 = FindModifiedRegister(dst);
  const int op2 = FindModifiedNextRegister(op1);
  m_from_sensor = (FromSensor(op1) || FromSensor(op2));
  sInternalValue& r1 = m_threads[m_cur_thread].reg[op1];
  sInternalValue& r2 = m_threads[m_cur_thread].reg[op2];
  setInternalValue(dst, ~(r1.value & r2.value), r1, r2);
  return true;
}

bool cHardwareMBE::Inst_TaskIO(cAvidaContext& ctx)
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

bool cHardwareMBE::Inst_TaskInput(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rBX);
  
  // Do the "get" component
  const int value_in = m_organism->GetNextInput();
  setInternalValue(reg_used, value_in, true);
  m_organism->DoInput(value_in);
  
  return true;
}

bool cHardwareMBE::Inst_TaskOutput(cAvidaContext& ctx)
{
  const int reg_used = FindModifiedRegister(rBX);
  m_from_sensor = FromSensor(reg_used);
  sInternalValue& reg = m_threads[m_cur_thread].reg[reg_used];
  
  // Do the "put" component
  m_organism->DoOutput(ctx, reg.value);  // Check for tasks completed.
  m_last_output = m_cycle_count;
  return true;
}

bool cHardwareMBE::Inst_IfCopiedCompLabel(cAvidaContext&)
{
  ReadLabel();
  if (!m_has_copied_end) {
    GetLabel().Rotate(1, NUM_NOPS);
    if (GetLabel() != GetReadLabel()) Advance(getThIP(), thIP);
    else m_has_copied_end = true;
  }
  return true;
}

bool cHardwareMBE::Inst_IfCopiedDirectLabel(cAvidaContext&)
{
  ReadLabel();
  if (!m_has_copied_end) {
    if (GetLabel() != GetReadLabel()) Advance(getThIP(), thIP);
    else m_has_copied_end = true;
  }
  return true;
}

bool cHardwareMBE::Inst_IfCopiedCompSeq(cAvidaContext&)
{
  ReadLabel();
  if (!m_has_copied_end) {
    GetLabel().Rotate(1, NUM_NOPS);
    if (GetLabel() != GetReadSequence()) Advance(getThIP(), thIP);
    else m_has_copied_end = true;
  }
  return true;
}

bool cHardwareMBE::Inst_IfCopiedDirectSeq(cAvidaContext&)
{
  ReadLabel();
  if (!m_has_copied_end) {
    if (GetLabel() != GetReadSequence()) Advance(getThIP(), thIP);
    else m_has_copied_end = true;
  }
  return true;
}

bool cHardwareMBE::Inst_Alloc(cAvidaContext& ctx)   
{
  // Create space for child, if it doesn't already exist
  if (m_has_alloc == false) {
    const int cur_size = main_memory.GetSize();
    const int alloc_size = Apto::Min((int)(m_world->GetConfig().OFFSPRING_SIZE_RANGE.Get() * cur_size), MAX_GENOME_LENGTH - cur_size);
    child_memory.Resize(alloc_size);
    m_has_alloc = true;
    return true;
  }
  return false;
}

bool cHardwareMBE::Inst_Copy(cAvidaContext& ctx)
{
  cHeadCPU& read_head = getRH();
  cHeadCPU& write_head = getWH();
  
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
  write_head.SetFlagCopied();
  
  if (m_organism->TestCopyIns(ctx)) write_head.InsertInst(m_inst_set->GetRandomInst(ctx));
  if (m_organism->TestCopyDel(ctx)) write_head.RemoveInst();
  if (m_organism->TestCopyUniform(ctx)) doUniformCopyMutation(ctx, write_head);
  if (m_organism->TestCopySlip(ctx)) {
    if (m_slip_read_head) {
      read_head.Set(ctx.GetRandom().GetInt(read_head.GetMemory().GetSize()), read_head.GetMemSpace());
    } else 
      doSlipMutation(ctx, write_head.GetMemory(), write_head.GetPosition());
  }
  
  read_head.Advance();
  write_head.Advance();
  return true;
}

bool cHardwareMBE::Inst_Divide(cAvidaContext& ctx)
{
  const int mem_space = mHeads[mWH].GetMemSpace();
  const int WH_pos = mHeads[mWH].GetPosition();
  return Divide_Main(ctx, mem_space, WH_pos, 1.0);
}

bool cHardwareMBE::Inst_Repro(cAvidaContext& ctx)
{
  if (m_organism->GetPhenotype().GetCurBonus() < m_world->GetConfig().REQUIRED_BONUS.Get()) return false;
  
  // set up the information to be sent
  // to the new organism
  InstructionSequencePtr offspring_seq(new InstructionSequence(main_memory));
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)m_inst_set->GetInstSetName());
  Genome offspring(GetType(), props, offspring_seq);

  m_organism->OffspringGenome() = offspring;  
  m_organism->GetPhenotype().SetLinesCopied(main_memory.GetSize());
  
  int lines_executed = 0;
  for (int i = 0; i < main_memory.GetSize(); i++) if (main_memory.FlagExecuted(i)) lines_executed++;
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
  
  m_has_alloc = false;
  if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
    m_advance_ip = false;
  }
  
  // Activate the child
  bool parent_alive = m_organism->ActivateDivide(ctx);
  
  // Do more work if the parent lives through the birth of the offspring
  if (parent_alive) {
    if (m_world->GetConfig().DIVIDE_METHOD.Get() == DIVIDE_METHOD_SPLIT) {
      Reset(ctx);
      ResizeCostArrays(m_threads.GetSize());
    }
  }
  return true;
}

bool cHardwareMBE::Inst_Die(cAvidaContext& ctx)
{
  m_organism->Die(ctx);
  
  return true;
}

bool cHardwareMBE::Inst_MoveHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(thIP);
  const int target = FindModifiedHead(thFH);
  
  // only the copy process will get head ids > 1, which can mean doing things with heads in different mem spaces
  // there are combinations which will fail (e.g. moving mIP)
  if (head_used > 1 || target > 1) {
    // child only 'has' a write head that can be used
    switch (head_used - 2) {
      case mWH:
        if (target == mRH) getHead(mWH).Set(getHead(mRH).GetPosition(), getHead(mWH).GetMemSpace());
        else if (target == mIP) getHead(mWH).Set(getHead(mIP).GetPosition(), getHead(mWH).GetMemSpace());
        else if (target == mFH) getHead(mWH).Set(getHead(mFH).GetPosition(), getHead(mWH).GetMemSpace());
      case mRH:
        if (target == mWH) getHead(mRH).Set(getHead(mWH).GetPosition());
        else if (target == mIP) getHead(mRH).Set(getHead(mIP));
        else if (target == mFH) getHead(mRH).Set(getHead(mFH));
      case mFH:
        if (target == mRH) getHead(mFH).Set(getHead(mRH).GetPosition());
        else if (target == mIP) getHead(mFH).Set(getHead(mIP).GetPosition());
        else if (target == mWH) getHead(mFH).Set(getHead(mWH).GetPosition());
      default:
        return true;
    }
  }
  else {
    getThHead(head_used).Set(getThHead(target));
    if (head_used == thIP) {
      m_advance_ip = false;
      Adjust(getThIP(), thIP);
    }
  }
  return true;
}

bool cHardwareMBE::Inst_JumpHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(thIP);
  const int reg = FindModifiedRegister(rCX);
  m_from_sensor = FromSensor(reg);
  if (head_used > 1) getHead(head_used - 2).Jump(m_threads[m_cur_thread].reg[reg].value);
  else getThHead(head_used).Jump(m_threads[m_cur_thread].reg[reg].value);
  if (head_used == thIP) {
    m_advance_ip = false;
    Adjust(getThIP(), thIP);
  }
  return true;
}

bool cHardwareMBE::Inst_GetHead(cAvidaContext&)
{
  const int head_used = FindModifiedHead(thIP);
  const int reg = FindModifiedRegister(rCX);
  int pos = 0;
  if (head_used > 1) pos = getHead(head_used - 2).GetPosition();
  else pos = getThHead(head_used).GetPosition();
  setInternalValue(reg, pos);
  return true;
}

bool cHardwareMBE::Inst_JumpGene(cAvidaContext&)
{
  ReadLabel();
  cCodeLabel& jump_to_label = GetLabel();
  jump_to_label.Rotate(1, NUM_NOPS);
  int prev_thread = m_cur_thread;
  m_cur_thread = FindNopSequenceOrgStart(true);
  m_threads[m_cur_thread].next_label = jump_to_label;
  cHeadCPU found_pos = FindNopSequenceStart(true);
  getThIP().Set(found_pos);
  if (prev_thread != m_cur_thread) getIP(prev_thread).Advance();
  Advance(getThHead(thIP), thIP);
  m_advance_ip = false;

  m_cur_behavior = m_threads[m_cur_thread].thread_class;
  return true;
}

bool cHardwareMBE::Inst_JumpBehavior(cAvidaContext&)
{
  const int reg_used = FindModifiedRegister(rAX);
  int behavior = (reg_used % NUM_BEHAVIORS) % NUM_REGISTERS;
  // after findmodified, ip will be at the trailing nop, if any.
  if (m_inst_set->IsNop(getIP().GetInst())) m_cur_behavior = behavior;
  else m_cur_behavior = (GetCurrBehav() + 1) % NUM_BEHAVIORS;
  
  setInternalValue(reg_used, m_cur_behavior, false);
  return true;
}

bool cHardwareMBE::Inst_JumpThread(cAvidaContext&)
{
  Advance(getThHead(thIP), thIP);
  m_cur_thread = Apto::UAbs(GetRegVal(FindModifiedRegister(rBX))) % static_cast<unsigned int>(m_threads.GetSize());
  m_cur_behavior = m_threads[m_cur_thread].thread_class;
  if (!m_threads[m_cur_thread].active) {
    m_threads[m_cur_thread].active = true;
    m_waiting_threads--;
  }
  return true;
}

bool cHardwareMBE::Inst_Search_Seq_Comp_S(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceStart(true);
  getThHead(thFH).Set(found_pos);
  Advance(getThHead(thFH), thFH);
  return true;
}

bool cHardwareMBE::Inst_Search_Seq_Comp_F(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceForward(true);
  getThHead(thFH).Set(found_pos);
  Advance(getThHead(thFH), thFH);
  return true;
}

bool cHardwareMBE::Inst_Search_Seq_Comp_B(cAvidaContext&)
{
  ReadLabel();
  GetLabel().Rotate(1, NUM_NOPS);
  cHeadCPU found_pos = FindNopSequenceBackward(true);
  getThHead(thFH).Set(found_pos);
  Advance(getThHead(thFH), thFH);
  return true;
}

bool cHardwareMBE::Inst_Search_Seq_Direct_S(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceStart(true);
  getThHead(thFH).Set(found_pos);
  Advance(getThHead(thFH), thFH);
  return true;
}

bool cHardwareMBE::Inst_Search_Seq_Direct_F(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceForward(true);
  getThHead(thFH).Set(found_pos);
  Advance(getThHead(thFH), thFH);
  return true;
}

bool cHardwareMBE::Inst_Search_Seq_Direct_B(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindNopSequenceBackward(true);
  getThHead(thFH).Set(found_pos);
  Advance(getThHead(thFH), thFH);
  return true;
}

bool cHardwareMBE::Inst_Search_Label_Direct_S(cAvidaContext&)
{
  ReadLabel();
  cHeadCPU found_pos = FindLabelStart(true);
  getThHead(thFH).Set(found_pos);
  Advance(getThHead(thFH), thFH);
  return true;
}

bool cHardwareMBE::Inst_WaitCondition_Equal(cAvidaContext&)
{
  const int wait_val_reg = FindModifiedRegister(rBX);
  const int check_reg = FindModifiedRegister(rDX);
  const int wait_dst = FindModifiedRegister(wait_val_reg);
  
  m_from_sensor = FromSensor(wait_val_reg);

  // Check if condition has already been met
    for (int i = 0; i < m_threads.GetSize(); i++) {
      // if not current thread in current behavioral process...and not active...and has a wait condition in current reg...
      if (!i == m_cur_thread && m_threads[i].reg[check_reg].value == m_threads[m_cur_thread].reg[wait_val_reg].value) {
        setInternalValue(wait_dst, m_threads[m_cur_thread].reg[check_reg].value, m_threads[m_cur_thread].reg[check_reg]);
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
  m_threads[m_cur_thread].wait_value = m_threads[m_cur_thread].reg[wait_val_reg].value;
  m_threads[m_cur_thread].wait_dst = wait_dst;
  
  return true;
}

bool cHardwareMBE::Inst_WaitCondition_Less(cAvidaContext&)
{
  const int wait_val_reg = FindModifiedRegister(rBX);
  const int check_reg = FindModifiedRegister(rDX);
  const int wait_dst = FindModifiedRegister(wait_val_reg);
  
  m_from_sensor = FromSensor(wait_val_reg);

  // Check if condition has already been met
    for (int i = 0; i < m_threads.GetSize(); i++) {
      // if not current thread in current behavioral process...and not active...and has a wait condition in current reg...
      if (!i == m_cur_thread && m_threads[i].reg[check_reg].value < m_threads[m_cur_thread].reg[wait_val_reg].value) {
        setInternalValue(wait_dst, m_threads[m_cur_thread].reg[check_reg].value, m_threads[m_cur_thread].reg[check_reg]);
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
  m_threads[m_cur_thread].wait_value = m_threads[m_cur_thread].reg[wait_val_reg].value;
  m_threads[m_cur_thread].wait_dst = wait_dst;
  
  return true;
}

bool cHardwareMBE::Inst_WaitCondition_Greater(cAvidaContext&)
{
  const int wait_val_reg = FindModifiedRegister(rBX);
  const int check_reg = FindModifiedRegister(rDX);
  const int wait_dst = FindModifiedRegister(wait_val_reg);
  
  m_from_sensor = FromSensor(wait_val_reg);

  // Check if condition has already been met
    for (int i = 0; i < m_threads.GetSize(); i++) {
      // if not current thread in current behavioral process...and not active...and has a wait condition in current reg...
      if (!i == m_cur_thread && m_threads[i].reg[check_reg].value > m_threads[m_cur_thread].reg[wait_val_reg].value) {
        setInternalValue(wait_dst, m_threads[m_cur_thread].reg[check_reg].value, m_threads[m_cur_thread].reg[check_reg]);
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
  m_threads[m_cur_thread].wait_value = m_threads[m_cur_thread].reg[wait_val_reg].value;
  m_threads[m_cur_thread].wait_dst = wait_dst;
  
  return true;
}

bool cHardwareMBE::Inst_Move(cAvidaContext& ctx)
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

bool cHardwareMBE::Inst_GetNorthOffset(cAvidaContext& ctx) {
  const int out_reg = FindModifiedRegister(rBX);
  int compass_dir = m_organism->GetOrgInterface().GetFacedDir();
  if (m_use_avatar) compass_dir = m_organism->GetOrgInterface().GetAVFacing();
  setInternalValue(out_reg, compass_dir, true);
  return true;
}

bool cHardwareMBE::Inst_GetPositionOffset(cAvidaContext&) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetNortherly(), true);
  setInternalValue(FindModifiedNextRegister(out_reg), m_organism->GetEasterly(), true);
  return true;
}

bool cHardwareMBE::Inst_GetNortherly(cAvidaContext&) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetNortherly(), true);
  return true;  
}

bool cHardwareMBE::Inst_GetEasterly(cAvidaContext&) {
  const int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, m_organism->GetEasterly(), true);
  return true;  
}

bool cHardwareMBE::Inst_ZeroEasterly(cAvidaContext&) {
  m_organism->ClearEasterly();
  return true;
}

bool cHardwareMBE::Inst_ZeroNortherly(cAvidaContext&) {
  m_organism->ClearNortherly();
  return true;
}

bool cHardwareMBE::Inst_ZeroPosOffset(cAvidaContext&) {
  const int offset = GetRegVal(FindModifiedRegister(rBX)) % 3;
  if (offset == 0) {
    m_organism->ClearEasterly();
    m_organism->ClearNortherly();    
  }
  else if (offset == 1) m_organism->ClearEasterly();
  else if (offset == 2) m_organism->ClearNortherly();
  return true;
}

bool cHardwareMBE::Inst_RotateHome(cAvidaContext& ctx)
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
    m_organism->Rotate(ctx, 1);
    if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedDir() == correct_facing) break;
    else if (m_use_avatar && m_organism->GetOrgInterface().GetAVFacing() == correct_facing) break;
  }
  return true;
}

bool cHardwareMBE::Inst_RotateUnoccupiedCell(cAvidaContext& ctx)
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

bool cHardwareMBE::Inst_RotateX(cAvidaContext& ctx)
{
  int num_neighbors = m_organism->GetNeighborhoodSize();
  if (m_use_avatar) num_neighbors = m_organism->GetOrgInterface().GetAVNumNeighbors();
  int rot_dir = 1;
  // If this organism has no neighbors, ignore rotate.
  if (num_neighbors == 0) return false;
  
  const int reg_used = FindModifiedRegister(rBX);
  int rot_num = m_threads[m_cur_thread].reg[reg_used].value;
  m_from_sensor = FromSensor(reg_used);
  // rotate the nop nuMBEr of times in the appropriate direction
  rot_num < 0 ? rot_dir = -1 : rot_dir = 1;
  rot_num = abs(rot_num);
  if (rot_num > 7) rot_num = rot_num % 8;
  for (int i = 0; i < rot_num; i++) m_organism->Rotate(ctx, rot_dir);
//   if (m_organism->GetID() == 0 && m_world->GetStats().GetUpdate() >= 0) cout << "direction: " << m_organism->GetOrgInterface().GetAVFacing() << endl;
  setInternalValue(reg_used, rot_num * rot_dir, true);
  return true;
}

bool cHardwareMBE::Inst_SenseResourceID(cAvidaContext& ctx)
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

bool cHardwareMBE::Inst_SenseNest(cAvidaContext& ctx)
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

bool cHardwareMBE::Inst_LookAhead(cAvidaContext& ctx)
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
bool cHardwareMBE::Inst_LookAheadIntercept(cAvidaContext& ctx)
{
  m_sensor.SetReturnRelativeFacing(true);
  return Inst_LookAhead(ctx);
}

bool cHardwareMBE::Inst_LookAround(cAvidaContext& ctx)
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

bool cHardwareMBE::Inst_LookAroundIntercept(cAvidaContext& ctx)
{
  m_sensor.SetReturnRelativeFacing(true);
  return Inst_LookAround(ctx);
}

bool cHardwareMBE::Inst_LookFT(cAvidaContext& ctx)
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

bool cHardwareMBE::Inst_LookAroundFT(cAvidaContext& ctx)
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

bool cHardwareMBE::GoLook(cAvidaContext& ctx, const int look_dir, const int cell_id, bool use_ft) 
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
  LookResults(ctx, reg_defs, look_results);
  return true;
}

cOrgSensor::sLookOut cHardwareMBE::InitLooking(cAvidaContext& ctx, sLookRegAssign& in_defs, int facing, int cell_id, bool use_ft)
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

  m_from_sensor = (FromSensor(habitat_reg) || FromSensor(distance_reg) || FromSensor(search_reg) || FromSensor(id_reg) || m_from_sensor);

  return m_sensor.SetLooking(ctx, reg_init, facing, cell_id, use_ft);
}    

void cHardwareMBE::LookResults(cAvidaContext& ctx, sLookRegAssign& regs, cOrgSensor::sLookOut& results)
{
//    if (m_organism->GetID() == 0 && m_world->GetStats().GetUpdate() >= 0) cout << "look_dist_seen: " << results.distance << " look_count_seen: " << results.count << endl;
  // habitat_reg=0, distance_reg=1, search_type_reg=2, id_sought_reg=3, count_reg=4, value_reg=5, group_reg=6, forager_type_reg=7
  // return defaults for failed to find
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
  m_sense_age = m_cycle_count;
  return;
}

bool cHardwareMBE::Inst_SenseFacedHabitat(cAvidaContext& ctx) 
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

bool cHardwareMBE::Inst_SetForageTarget(cAvidaContext& ctx)
{
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
  
  // return false if trying to become predator this has been disallowed via setforagetarget
  if (prop_target <= -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() == 2) return false;
  
  // a little mod help...can't set to -1, that's for juevniles only...so only exception to mod help is -2
  if (!m_world->GetEnvironment().IsTargetID(prop_target) && prop_target != -2 && prop_target != -3) {
    int num_fts = 0;
    std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
    set <int>::iterator itr;    
    for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) if (*itr != -1 && *itr != -2 && *itr != -3) num_fts++;
    if (m_world->GetEnvironment().IsTargetID(-1) && num_fts == 0) prop_target = -1;
    else {
      // ft's may not be sequentially nuMBEred
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
  if (m_use_avatar && (((prop_target == -2 || prop_target == -3) && old_target > -2) || (prop_target > -2 && (old_target == -2 || old_target == -3))) &&
      (m_organism->GetOrgInterface().GetAVCellID() != -1)) {
    m_organism->GetOrgInterface().SwitchPredPrey(ctx);
    m_organism->SetForageTarget(ctx, prop_target);
  }
  else m_organism->SetForageTarget(ctx, prop_target);
    
  // Set the new target and return the value
  m_organism->RecordFTSet();
  setInternalValue(FindModifiedRegister(rBX), prop_target, false);
  return true;
}

bool cHardwareMBE::Inst_SetForageTargetOnce(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  if (m_organism->HasSetFT()) return false;
  else return Inst_SetForageTarget(ctx);
}

bool cHardwareMBE::Inst_SetRandForageTargetOnce(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  int cap = 0;
  if (m_world->GetConfig().POPULATION_CAP.Get()) cap = m_world->GetConfig().POPULATION_CAP.Get();
  else if (m_world->GetConfig().POP_CAP_ELDEST.Get()) cap = m_world->GetConfig().POP_CAP_ELDEST.Get();
  if (cap && (m_organism->GetOrgInterface().GetLiveOrgList().GetSize() >= (((double)(cap)) * 0.5)) && ctx.GetRandom().P(0.5)) {
    if (m_organism->HasSetFT()) return false;
    else {
      int num_fts = 0;
      std::set<int> fts_avail = m_world->GetEnvironment().GetTargetIDs();
      set <int>::iterator itr;
      for (itr = fts_avail.begin();itr!=fts_avail.end();itr++) if (*itr != -1 && *itr != -2 && *itr != -3) num_fts++;
      int prop_target = ctx.GetRandom().GetUInt(num_fts);
      if (m_world->GetEnvironment().IsTargetID(-1) && num_fts == 0) prop_target = -1;
      else {
        // ft's may not be sequentially nuMBEred
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
  }
  else return Inst_SetForageTargetOnce(ctx);
}

bool cHardwareMBE::Inst_GetForageTarget(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  const int target_reg = FindModifiedRegister(rBX);
  setInternalValue(target_reg, m_organism->GetForageTarget(), false);
  return true;
}

bool cHardwareMBE::Inst_CollectSpecific(cAvidaContext& ctx)
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

bool cHardwareMBE::Inst_GetResStored(cAvidaContext& ctx)
{
  int resource_id = abs(GetRegVal(FindModifiedRegister(rBX)));
  Apto::Array<double> bins = m_organism->GetRBins();
  resource_id %= bins.GetSize();
  int out_reg = FindModifiedRegister(rBX);
  setInternalValue(out_reg, (int)(bins[resource_id]), true);
  return true;
}

// Sets organism's opinion to the value in ?BX?
bool cHardwareMBE::Inst_SetOpinion(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  m_organism->GetOrgInterface().SetOpinion(GetRegVal(FindModifiedRegister(rBX)), m_organism);
  return true;
}

/* Gets the organism's current opinion, placing the opinion in register ?BX?
   and the age of the opinion in register !?BX?
 */
bool cHardwareMBE::Inst_GetOpinion(cAvidaContext& ctx)
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
bool cHardwareMBE::Inst_JoinGroup(cAvidaContext& ctx)
{
  int opinion = m_world->GetConfig().DEFAULT_GROUP.Get();
  // Check if the org is currently part of a group
  assert(m_organism != 0);
	
  int prop_group_id = GetRegVal(FindModifiedRegister(rBX));
  
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
      double rand = ctx.GetRandom().GetDouble();
      if (rand <= prob_failure) return true;
    }
    
    // If tolerances are on the org must pass immigration chance 
    if (m_world->GetConfig().TOLERANCE_WINDOW.Get() > 0) {
      m_organism->GetOrgInterface().AttemptImmigrateGroup(ctx, prop_group_id, m_organism);
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

bool cHardwareMBE::Inst_GetGroupID(cAvidaContext&)
{
  assert(m_organism != 0);
  if (m_organism->HasOpinion()) {
    const int group_reg = FindModifiedRegister(rBX);
    
    setInternalValue(group_reg, m_organism->GetOpinion().first, false);
  }
  return true;
}

bool cHardwareMBE::Inst_GetFacedOrgID(cAvidaContext& ctx)
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

//Teach offspring learned targeting/foraging behavior
bool cHardwareMBE::Inst_TeachOffspring(cAvidaContext&)
{
  assert(m_organism != 0);
  m_organism->Teach(true);
  return true;
}

bool cHardwareMBE::Inst_LearnParent(cAvidaContext& ctx)
{
  assert(m_organism != 0);
  bool halt = false;
  if (m_organism->HadParentTeacher()) {
    int old_target = m_organism->GetForageTarget();
    int prop_target = -1;
    prop_target = m_organism->GetParentFT();

    halt = (prop_target <= -2 && m_world->GetConfig().PRED_PREY_SWITCH.Get() < 0);
    if (!halt) {
      if (m_use_avatar && m_organism->GetOrgInterface().GetAVCellID() != -1 && 
          (((prop_target == -2 || prop_target == -3) && old_target > -2) || (prop_target > -2 && (old_target == -2 || prop_target == -3)))) {
        m_organism->GetOrgInterface().SwitchPredPrey(ctx);
        m_organism->CopyParentFT(ctx);
      }
      else m_organism->CopyParentFT(ctx);
    }
  }
  return !halt;
}

bool cHardwareMBE::Inst_ScrambleReg(cAvidaContext& ctx)
{
  for (int i = 0; i < NUM_REGISTERS; i++) {
    setInternalValue(rAX + i, (int) (ctx.GetRandom().GetDouble()), true);
  }
  return true;
}

//Attack organism faced by this one, if there is non-predator target in front, and steal it's merit, current bonus, and reactions. 
bool cHardwareMBE::Inst_AttackPrey(cAvidaContext& ctx)
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

bool cHardwareMBE::DoActualCollect(cAvidaContext& ctx, int bin_used, bool unit)
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

void cHardwareMBE::MakePred(cAvidaContext& ctx)
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

bool cHardwareMBE::TestAttack(cAvidaContext& ctx)
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
  const cResourceLib& resource_lib = m_world->GetEnvironment().GetResourceLib();
  for (int i = 0; i < resource_lib.GetSize(); i++) {
    if (resource_lib.GetResource(i)->GetRefuge()) {
      if (!m_use_avatar && m_organism->GetOrgInterface().GetFacedResourceVal(ctx, i) > 0) return false;
      else if (m_use_avatar == 2 && m_organism->GetOrgInterface().GetAVFacedResourceVal(ctx, i) > 0) return false;
    }
  }
  return true;
}

cOrganism* cHardwareMBE::GetPreyTarget(cAvidaContext& ctx)
{
  cOrganism* target = NULL;
  if (!m_use_avatar) target = m_organism->GetOrgInterface().GetNeighbor();
  else if (m_use_avatar == 2) target = m_organism->GetOrgInterface().GetRandFacedPreyAV();  
  return target;
}

bool cHardwareMBE::TestPreyTarget(cOrganism* target)
{
  // attacking other carnivores is handled differently (e.g. using fights or tolerance)
  bool success = true;
  if (!target->IsPreyFT()) success = false;
  else if (target->IsDead()) success = false;
  return success;
}

void cHardwareMBE::SetAttackReg(sAttackReg& reg)
{
  reg.success_reg = FindModifiedRegister(rBX);
  reg.bonus_reg = FindModifiedNextRegister(reg.success_reg);
  reg.bin_reg = FindModifiedNextRegister(reg.bonus_reg);
}

bool cHardwareMBE::ExecuteAttack(cAvidaContext& ctx, cOrganism* target, sAttackReg& reg, double odds)
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

  MakePred(ctx);
  target->Die(ctx); // kill first -- could end up being killed by inject clone
  TryPreyClone(ctx);
  return true;
}

bool cHardwareMBE::TestAttackChance(cAvidaContext& ctx, cOrganism* target, sAttackReg& reg, double odds)
{
  bool success = true;
  if (odds == -1) odds = m_world->GetConfig().PRED_ODDS.Get() ;
  if (ctx.GetRandom().GetDouble() >= odds ||
      (m_world->GetConfig().MIN_PREY.Get() > 0 && m_world->GetStats().GetNumPreyCreatures() <= m_world->GetConfig().MIN_PREY.Get())) {
    InjureOrg(target);
    setInternalValue(reg.success_reg, -1, true);
    setInternalValue(reg.bonus_reg, -1, true);
    if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) setInternalValue(reg.bin_reg, -1, true);
    success = false;
  }
  return success;
}

void cHardwareMBE::ApplyKilledPreyMerit(cOrganism* target, double effic)
{
  // add prey's merit to predator's--this will result in immediately applying merit increases; adjustments to bonus, give increase in next generation
  if (m_world->GetConfig().MERIT_INC_APPLY_IMMEDIATE.Get()) {
    const double target_merit = target->GetPhenotype().GetMerit().GetDouble();
    double attacker_merit = m_organism->GetPhenotype().GetMerit().GetDouble();
    attacker_merit += target_merit * effic;
    m_organism->UpdateMerit(attacker_merit);
  }
}

void cHardwareMBE::ApplyKilledPreyReactions(cOrganism* target)
{
  // now add on the victims reaction counts to your own, this will allow you to pass any reaction tests...
  Apto::Array<int> target_reactions = target->GetPhenotype().GetLastReactionCount();
  Apto::Array<int> org_reactions = m_organism->GetPhenotype().GetStolenReactionCount();
  for (int i = 0; i < org_reactions.GetSize(); i++) {
    m_organism->GetPhenotype().SetStolenReactionCount(i, org_reactions[i] + target_reactions[i]);
  }
}

void cHardwareMBE::ApplyKilledPreyBonus(cOrganism* target, sAttackReg& reg, double effic)
{
  // and add current merit bonus after adjusting for conversion efficiency
  const double target_bonus = target->GetPhenotype().GetCurBonus();
  m_organism->GetPhenotype().SetCurBonus(m_organism->GetPhenotype().GetCurBonus() + (target_bonus * effic));
  setInternalValue(reg.bonus_reg, (int) (target_bonus), true);
}

void cHardwareMBE::ApplyKilledPreyResBins(cOrganism* target, sAttackReg& reg, double effic)
{
  // now add the victims internal resource bins to your own, if enabled, after correcting for conversion efficiency
  if (m_world->GetConfig().USE_RESOURCE_BINS.Get()) {
    Apto::Array<double> target_bins = target->GetRBins();
    for (int i = 0; i < target_bins.GetSize(); i++) {
      m_organism->AddToRBin(i, target_bins[i] * effic);
      target->AddToRBin(i, -1 * (target_bins[i] * effic));
    }
    const int spec_bin = (int) (m_organism->GetRBins()[m_world->GetConfig().COLLECT_SPECIFIC_RESOURCE.Get()]);
    setInternalValue(reg.bin_reg, spec_bin, true);
  }
}

void cHardwareMBE::TryPreyClone(cAvidaContext& ctx)
{
  if (m_world->GetConfig().MIN_PREY.Get() < 0 && m_world->GetStats().GetNumPreyCreatures() <= abs(m_world->GetConfig().MIN_PREY.Get())) {
    // prey numbers can be crashing for other reasons and we wouldn't be using this switch if we didn't want an absolute min num prey
    int num_clones = abs(m_world->GetConfig().MIN_PREY.Get()) - m_world->GetStats().GetNumPreyCreatures();
    for (int i = 0; i < num_clones; i++)m_organism->GetOrgInterface().InjectPreyClone(ctx, m_organism->SystematicsGroup("genotype")->ID());
  }
}

void cHardwareMBE::InjureOrg(cOrganism* target)
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

bool cHardwareMBE::TestAttackResultsOut(sAttackResult& results)
{
  if (results.success == 0) return true;
  return false;
}
