//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_TRACER_TEST_CPU_HH
#define HARDWARE_TRACER_TEST_CPU_HH

class cString;
struct cHardwareTracer_TestCPU {
  virtual void TraceHardware_TestCPU(
    int time_used,
    int time_allocated,
    int size,
    const cString &final_memory,
    const cString &child_memory
  ) = 0;
};

#endif

