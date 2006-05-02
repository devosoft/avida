/*
 *  cHardwareCPU_Thread.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cHardwareCPU_Thread_h
#define cHardwareCPU_Thread_h

#include <iostream>

#ifndef cCodeLabel_h
#include "cCodeLabel.h"
#endif
#ifndef nHardware_h
#include "nHardware.h"
#endif
#ifndef nHardwareCPU_h
#include "nHardwareCPU.h"
#endif
#ifndef cHeadCPU_h
#include "cHeadCPU.h"
#endif
#ifndef cCPUStack_h
#include "cCPUStack.h"
#endif

/**
 * This class is needed to run several threads on a single genome.
 *
 * @see cCPUStack, cHeadCPU, cHardwareCPU
 **/

struct cHardwareCPU_Thread
{
private:
  int id;

public:
  int reg[nHardwareCPU::NUM_REGISTERS];
  cHeadCPU heads[nHardware::NUM_HEADS];
  cCPUStack stack;
  unsigned char cur_stack;              // 0 = local stack, 1 = global stack.
  unsigned char cur_head;

  cCodeLabel read_label;
  cCodeLabel next_label;

  
  cHardwareCPU_Thread(cHardwareBase* in_hardware = NULL, int _id = -1);
  cHardwareCPU_Thread(const cHardwareCPU_Thread& in_thread, int _id = -1);
  ~cHardwareCPU_Thread();

  void operator=(const cHardwareCPU_Thread& in_thread);

  void Reset(cHardwareBase* in_hardware, int _id);
  int GetID() const { return id; }
  void SetID(int _id) { id = _id; }


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
