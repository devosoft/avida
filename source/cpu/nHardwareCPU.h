/*
 *  nHardwareCPU.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

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
