/*
 *  nHardware4Stack.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef nHardware4Stack_h
#define nHardware4Stack_h

namespace nHardware4Stack
{
  static const int NUM_LOCAL_STACKS = 5;
  static const int NUM_GLOBAL_STACKS = 1;
  static const int NUM_STACKS = NUM_LOCAL_STACKS + NUM_GLOBAL_STACKS;
  
  enum tStacks { STACK_AX = 0, STACK_BX, STACK_CX, STACK_DX, STACK_EX, STACK_FX };

  static const int NUM_NOPS = 6;
  static const int NUM_MEMORY_SPACES = 6; 
}

#endif
