//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2004 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_TRACER_4STACK_HH
#define HARDWARE_TRACER_4STACK_HH

class cHardware4Stack;
struct cHardwareTracer_4Stack {
  virtual ~cHardwareTracer_4Stack() { ; }
  virtual void TraceHardware_4Stack(cHardware4Stack &) = 0;
  virtual void TraceHardware_4StackBonus(cHardware4Stack &) = 0;
};

#endif
