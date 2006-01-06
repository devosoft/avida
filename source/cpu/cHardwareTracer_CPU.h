/*
 *  cHardwareTracer_CPU.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2004 California Institute of Technology.
 *
 */


#ifndef cHardwareTracer_CPU_h
#define cHardwareTracer_CPU_h

class cHardwareCPU;
class cHardwareTracer_CPU {
public:
  virtual ~cHardwareTracer_CPU() { ; }
  virtual void TraceHardware_CPU(cHardwareCPU &) = 0;
  virtual void TraceHardware_CPUBonus(cHardwareCPU &) = 0;
};

#endif
