/*
 *  cHardwareStatusPrinter.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2004 California Institute of Technology.
 *
 */

#ifndef cHardwareStatusPrinter_h
#define cHardwareStatusPrinter_h

#include <iostream>

#ifndef cHardwareTracer_h
#include "cHardwareTracer.h"
#endif
#ifndef cHardwareTracer_4Stack_h
#include "cHardwareTracer_4Stack.h"
#endif
#ifndef cHardwareTracer_SMT_h
#include "cHardwareTracer_SMT.h"
#endif
#ifndef cHardwareTracer_CPU_h
#include "cHardwareTracer_CPU.h"
#endif
#ifndef cHardwareTracer_TestCPU_h
#include "cHardwareTracer_TestCPU.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cHardwareBase;

class cHardwareStatusPrinter :
  public cHardwareTracer,
  public cHardwareTracer_CPU,
  public cHardwareTracer_4Stack,
  public cHardwareTracer_SMT,
  public cHardwareTracer_TestCPU
{
protected:
  std::ostream& m_trace_fp;
protected:
  const cString& GetNextInstName(cHardwareCPU& hardware);
  const cString& GetNextInstName(cHardware4Stack& hardware);
  const cString& GetNextInstName(cHardwareSMT& hardware);
  cString Bonus(const cString& next_name);
  void PrintStatus(cHardwareBase& hardware, const cString& next_name);
  
  cHardwareStatusPrinter(); // @not_implemented

public:
  cHardwareStatusPrinter(std::ostream& trace_fp) : m_trace_fp(trace_fp) {;}

  virtual void TraceHardware_CPU(cHardwareCPU &hardware);
  virtual void TraceHardware_4Stack(cHardware4Stack &hardware);
  virtual void TraceHardware_SMT(cHardwareSMT &hardware);
  virtual void TraceHardware_CPUBonus(cHardwareCPU &hardware);
  virtual void TraceHardware_4StackBonus(cHardware4Stack &hardware);
  virtual void TraceHardware_SMTBonus(cHardwareSMT &hardware);
  virtual void TraceHardware_TestCPU(int time_used, int time_allocated, int size,
                                     const cString& final_memory, const cString& child_memory);
};


#ifdef ENABLE_UNIT_TESTS
namespace nHardwareStatusPrinter {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
