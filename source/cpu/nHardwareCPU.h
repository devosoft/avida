//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          // 
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////


#ifndef nHardwareCPU_h
#define nHardwareCPU_h

// The following defines setup the structure of the CPU.  Changing them
// (especially the number of registers) could have effects in other parts
// of the code!

namespace nHardwareCPU {
  static const int NUM_REGISTERS = 3;
  
  enum tRegisters { REG_AX = 0, REG_BX, REG_CX, REG_DX };

  static const int NUM_NOPS = 3;
}

#endif
