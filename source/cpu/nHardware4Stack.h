//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_4STACK_CONSTANTS_HH
#define HARDWARE_4STACK_CONSTANTS_HH

namespace nHardware4Stack
{
  //new-style constant declarations - law 
  static const int NUM_LOCAL_STACKS = 5;
  static const int NUM_GLOBAL_STACKS = 1;
  static const int NUM_STACKS = NUM_LOCAL_STACKS + NUM_GLOBAL_STACKS;
  
  enum tStacks { STACK_AX = 0, STACK_BX, STACK_CX, STACK_DX, STACK_EX, STACK_FX };

  static const int NUM_NOPS = 6;
  static const int NUM_MEMORY_SPACES = 6; 
}

#endif
