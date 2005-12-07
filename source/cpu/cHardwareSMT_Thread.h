/*
 *  cHardwareSMT_Thread.h
 *  Avida
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef cHardwareSMT_Thread_h
#define cHardwareSMT_Thread_h

#include <iostream>

#ifndef cCodeLabel_h
#include "cCodeLabel.h"
#endif
#ifndef cCPUStack_h
#include "cCPUStack.h"
#endif
#ifndef cHeadMultiMem_h
#include "cHeadMultiMem.h"
#endif
#ifndef nHardwareSMT_h
#include "nHardwareSMT.h"
#endif
#ifndef tBuffer_h
#include "tBuffer.h"
#endif

/**
* This class is needed to run several threads on a single genome.
 *
 * @see cCPUStack, cHeadMultiMem, cHardwareSMT
 **/

class cHardwareBase;
class cInjectGenotype;

struct cHardwareSMT_Thread {
private:
  int id;
public:
  cHeadMultiMem heads[nHardware::NUM_HEADS];
  unsigned char cur_head;
  cCPUStack local_stacks[nHardwareSMT::NUM_LOCAL_STACKS];
	
  bool advance_ip;         // Should the IP advance after this instruction?
  cCodeLabel read_label;
  cCodeLabel next_label;
  // If this thread was spawned by Inject, this will point to the genotype 
  // of the parasite running the thread.  Otherwise, it will be NULL.
  cInjectGenotype* owner;
public:
		cHardwareSMT_Thread(cHardwareBase* in_hardware = NULL, int _id = -1);
  cHardwareSMT_Thread(const cHardwareSMT_Thread& in_thread, int _id = -1);
  ~cHardwareSMT_Thread();
	
  void operator=(const cHardwareSMT_Thread& in_thread);
	
  void Reset(cHardwareBase* in_hardware, int _id);
  int GetID() const { return id; }
  void SetID(int _id) { id = _id; }
};

#endif
