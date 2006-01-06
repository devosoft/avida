/*
 *  cHardwareTracer_TestCPU.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2004 California Institute of Technology.
 *
 */

#ifndef cHardwareTracer_TestCPU_h
#define cHardwareTracer_TestCPU_h

class cString;
class cHardwareTracer_TestCPU {
public:
  virtual ~cHardwareTracer_TestCPU() { ; }
  virtual void TraceHardware_TestCPU(
    int time_used,
    int time_allocated,
    int size,
    const cString &final_memory,
    const cString &child_memory
  ) = 0;
};

#endif

