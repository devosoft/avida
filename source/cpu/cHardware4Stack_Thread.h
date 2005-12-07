/*
 *  cHardware4Stack_Thread.h
 *  Avida
 *
 *  Created by David on 11/17/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cHardware4Stack_Thread_h
#define cHardware4Stack_Thread_h

#include <iostream>

#ifndef cCodeLabel_h
#include "cCodeLabel.h"
#endif
#ifndef cCPUStack_h
#include "cCPUStack.h"
#endif
#ifndef cHeadMulitMem_h
#include "cHeadMultiMem.h"
#endif
#ifndef nHardware4Stack_h
#include "nHardware4Stack.h"
#endif
#ifndef tBuffer_h
#include "tBuffer.h"
#endif

/**
 * This class is needed to run several threads on a single genome.
 *
 * @see cCPUStack, cHeadMultiMem, cHardware4Stack
 **/

class cHeadMultiMem; // aggregate
class cCodeLabel; // aggregate
class cCPUStack; // aggregate
class cHardwareBase;
class cInjectGenotype;
template <class T> class tBuffer; // aggregate

struct cHardware4Stack_Thread {
private:
  int id;
public:
  cHeadMultiMem heads[nHardware::NUM_HEADS];
  unsigned char cur_head;
  cCPUStack local_stacks[nHardware4Stack::NUM_LOCAL_STACKS];

  bool advance_ip;         // Should the IP advance after this instruction?
  cCodeLabel read_label;
  cCodeLabel next_label;
  // If this thread was spawned by Inject, this will point to the genotype 
  // of the parasite running the thread.  Otherwise, it will be NULL.
  cInjectGenotype* owner;
public:
  cHardware4Stack_Thread(cHardwareBase * in_hardware=NULL, int _id=-1);
  cHardware4Stack_Thread(const cHardware4Stack_Thread & in_thread, int _id=-1);
  ~cHardware4Stack_Thread();

  void operator=(const cHardware4Stack_Thread & in_thread);

  void Reset(cHardwareBase * in_hardware, int _id);
  int GetID() const { return id; }
  void SetID(int _id) { id = _id; }
};

#endif
