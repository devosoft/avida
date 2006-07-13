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
#ifndef cString_h
#include "cString.h"
#endif

class cHardwareBase;

class cHardwareStatusPrinter : public cHardwareTracer
{
protected:
  std::ostream& m_trace_fp;


private: 
  cHardwareStatusPrinter(); // @not_implemented


public:
  cHardwareStatusPrinter(std::ostream& trace_fp) : m_trace_fp(trace_fp) { ; }

  virtual void TraceHardware(cHardwareBase& hardware, bool bonus);
  virtual void TraceTestCPU(int time_used, int time_allocated, int size,
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
