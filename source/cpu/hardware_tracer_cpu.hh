//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_TRACER_CPU_HH
#define HARDWARE_TRACER_CPU_HH

class cHardwareCPU;
struct cHardwareTracer_CPU {
  virtual void TraceHardware_CPU(cHardwareCPU &) = 0;
  virtual void TraceHardware_CPUBonus(cHardwareCPU &) = 0;
};

#endif
