//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          // 
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////


#ifndef CPU_DEFS_HH
#define CPU_DEFS_HH

#define TEST_CPU_GENERATIONS 3  // Default generations tested for viability.

#define MAX_TEST_DEPTH       10  // Maximum number of generations tested

//#define FITNESS_NEUTRAL_RANGE (1.0/3600.0)
#define FITNESS_NEUTRAL_RANGE (0.0)
#define FITNESS_NEUTRAL_MIN (1.0 - FITNESS_NEUTRAL_RANGE)
#define FITNESS_NEUTRAL_MAX (1.0 + FITNESS_NEUTRAL_RANGE)


// The following defines setup the structure of the CPU.  Changing them
// (especially the number of registers) could have effects in other parts
// of the code!

#define NUM_NOPS 3
#define MAX_NOPS 6
#define MAX_LABEL_SIZE 10 
#define REG_AX 0
#define REG_BX 1
#define REG_CX 2
#define REG_DX 3
//jason
#define NUM_REGISTERS 3
//#define NUM_REGISTERS 4
#define NUM_REG_4STACK 4


#define HEAD_IP     0
#define HEAD_READ   1
#define HEAD_WRITE  2
#define HEAD_FLOW   3
#define NUM_HEADS   4


#define STACK_SIZE 10
#define IO_SIZE 3

#define GEN_RESOURCE   0
#define NUM_RESOURCES  1

#define FAULT_TYPE_WARNING 0
#define FAULT_TYPE_ERROR   1

#define FAULT_TYPE_FORK_TH 0
#define FAULT_TYPE_KILL_TH 0

// Fault locations in CPU executions...
#define FAULT_LOC_DEFAULT      0
#define FAULT_LOC_INSTRUCTION  1
#define FAULT_LOC_JUMP         2
#define FAULT_LOC_MATH         3
#define FAULT_LOC_INJECT       4
#define FAULT_LOC_THREAD_FORK  5
#define FAULT_LOC_THREAD_KILL  6
#define FAULT_LOC_ALLOC        7
#define FAULT_LOC_DIVIDE       8
#define FAULT_LOC_BCOPY        9

class cInstSet;		// defined in inst_set.hh

#endif
