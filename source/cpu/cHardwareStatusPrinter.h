//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_STATUS_PRINTER_HH
#define HARDWARE_STATUS_PRINTER_HH

#include <iostream>

#ifndef HARDWARE_TRACER_HH
#include "cHardwareTracer.h"
#endif
#ifndef HARDWARE_TRACER_4STACK_HH
#include "cHardwareTracer_4Stack.h"
#endif
#ifndef HARDWARE_TRACER_SMT_H
#include "cHardwareTracer_SMT.h"
#endif
#ifndef HARDWARE_TRACER_CPU_HH
#include "cHardwareTracer_CPU.h"
#endif
#ifndef HARDWARE_TRACER_TEST_CPU_HH
#include "cHardwareTracer_TestCPU.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif


using namespace std;


class cHardwareBase;

class cHardwareStatusPrinter :
  public cHardwareTracer,
  public cHardwareTracer_CPU,
  public cHardwareTracer_4Stack,
  public cHardwareTracer_SMT,
  public cHardwareTracer_TestCPU
{
protected:
  ostream& m_trace_fp;
protected:
  const cString & GetNextInstName(cHardwareCPU& hardware);
  const cString & GetNextInstName(cHardware4Stack& hardware);
  const cString & GetNextInstName(cHardwareSMT& hardware);
  cString Bonus(const cString &next_name);
  void PrintStatus(cHardwareBase& hardware, const cString& next_name);
public:
  cHardwareStatusPrinter(ostream& trace_fp) : m_trace_fp(trace_fp) {;}

  virtual void TraceHardware_CPU(cHardwareCPU &hardware);
  virtual void TraceHardware_4Stack(cHardware4Stack &hardware);
  virtual void TraceHardware_SMT(cHardwareSMT &hardware);
  virtual void TraceHardware_CPUBonus(cHardwareCPU &hardware);
  virtual void TraceHardware_4StackBonus(cHardware4Stack &hardware);
  virtual void TraceHardware_SMTBonus(cHardwareSMT &hardware);
  virtual void TraceHardware_TestCPU(
    int time_used,
    int time_allocated,
    int size,
    const cString &final_memory,
    const cString &child_memory
  );
};

#endif
