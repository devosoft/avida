//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_4STACK_CONSTANTS_HH
#define HARDWARE_4STACK_CONSTANTS_HH

//new-style constant declarations - law 
static const int NUM_LOCAL_STACKS = 5;
static const int NUM_GLOBAL_STACKS = 1;
static const int NUM_STACKS = NUM_LOCAL_STACKS + NUM_GLOBAL_STACKS;
static const int STACK_AX = 0;
static const int STACK_BX = 1;
static const int STACK_CX = 2;
static const int STACK_DX = 3;
static const int STACK_EX = 4;
static const int STACK_FX = 5;
static const int NUM_NOPS_4STACK = 6;
static const int NUM_MEMORY_SPACES = 6; 

#endif
