/*
 *  cHardwareTracer_SMT.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2004 California Institute of Technology.
 *
 */

#ifndef cHardwareTracer_SMT_h
#define cHardwareTracer_SMT_h

class cHardwareSMT;
class cHardwareTracer_SMT {
public:
  virtual ~cHardwareTracer_SMT() { ; }
  virtual void TraceHardware_SMT(cHardwareSMT &) = 0;
  virtual void TraceHardware_SMTBonus(cHardwareSMT &) = 0;


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif
