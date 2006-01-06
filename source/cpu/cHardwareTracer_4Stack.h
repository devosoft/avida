/*
 *  cHardwareTracer_4Stack.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2004 California Institute of Technology.
 *
 */

#ifndef cHardwareTracer_4Stack_h
#define cHardwareTracer_4Stack_h

class cHardware4Stack;
class cHardwareTracer_4Stack {
public:
  virtual ~cHardwareTracer_4Stack() { ; }
  virtual void TraceHardware_4Stack(cHardware4Stack &) = 0;
  virtual void TraceHardware_4StackBonus(cHardware4Stack &) = 0;
};

#endif
