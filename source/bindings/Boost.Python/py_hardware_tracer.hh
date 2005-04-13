//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef PY_HARDWARE_TRACER_HH
#define PY_HARDWARE_TRACER_HH

#ifndef HARDWARE_TRACER_HH
#include "hardware_tracer.hh"
#endif
#ifndef HARDWARE_TRACER_4STACK_HH
#include "hardware_tracer_4stack.hh"
#endif
#ifndef HARDWARE_TRACER_CPU_HH
#include "hardware_tracer_cpu.hh"
#endif
#ifndef HARDWARE_TRACER_TEST_CPU_HH
#include "hardware_tracer_test_cpu.hh"
#endif

class cString;
class pyHardwareTracerBase :
  public cHardwareTracer
, public cHardwareTracer_CPU
, public cHardwareTracer_4Stack
, public cHardwareTracer_TestCPU
{
public:
  pyHardwareTracerBase(){}
  virtual void TraceHardware_CPU(cHardwareCPU &hardware);
  virtual void TraceHardware_4Stack(cHardware4Stack &hardware);
  virtual void TraceHardware_CPUBonus(cHardwareCPU &hardware);
  virtual void TraceHardware_4StackBonus(cHardware4Stack &hardware);
  virtual void TraceHardware_TestCPU(
    int time_used,
    int time_allocated,
    int size,
    const cString &final_memory,
    const cString &child_memory
  );
};

#endif
