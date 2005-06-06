/*
 *  hardware_smt_constants.h
 *  Avida2
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef HARDWARE_SMT_CONSTANTS_H
#define HARDWARE_SMT_CONSTANTS_H

// Constants can be used across different cpu models, wrap in namespace @DMB
namespace nHardwareSMT
{
  static const int NUM_LOCAL_STACKS = 5;
  static const int NUM_GLOBAL_STACKS = 1;
  static const int NUM_STACKS = NUM_LOCAL_STACKS + NUM_GLOBAL_STACKS;
  
  enum tStacks { STACK_AX = 0, STACK_BX, STACK_CX, STACK_DX, STACK_EX, STACK_FX };
  
  static const int NUM_NOPS = 6;
  static const int NUM_MEMORY_SPACES = 6; 
}

#endif
