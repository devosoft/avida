/*
 *  cHardwareTracer.h
 *  Avida
 *
 *  Called "hardware_tracer.hh" prior to 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2004 California Institute of Technology.
 *
 */

#ifndef cHardwareTracer_h
#define cHardwareTracer_h

class cHardwareBase;
class cOrganism;
class cString;

class cHardwareTracer
{
public:
  virtual ~cHardwareTracer() { ; }
  virtual void TraceHardware(cHardwareBase&, bool bonus = false) = 0;
  virtual void TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism)= 0;
};

#endif
