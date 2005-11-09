/*
 *  cHardwareSMT.h
 *  Avida
 *
 *  Created by David on 9/22/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef HARDWARE_TRACER_SMT_H
#define HARDWARE_TRACER_SMT_H

class cHardwareSMT;
struct cHardwareTracer_SMT {
  virtual ~cHardwareTracer_SMT() { ; }
  virtual void TraceHardware_SMT(cHardwareSMT &) = 0;
  virtual void TraceHardware_SMTBonus(cHardwareSMT &) = 0;
};

#endif
