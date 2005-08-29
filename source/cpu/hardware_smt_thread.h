/*
 *  hardware_smt_thread.h
 *  Avida2
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef HARDWARE_SMT_THREAD_H
#define HARDWARE_SMT_THREAD_H

#include <iostream>

#ifndef CODE_LABEL_HH
#include "cCodeLabel.h"
#endif
#ifndef CPU_STACK_HH
#include "cCPUStack.h"
#endif
#ifndef HEAD_MULTI_MEM_HH
#include "head_multi_mem.hh"
#endif
#ifndef HARDWARE_SMT_CONSTANTS_H
#include "hardware_smt_constants.h"
#endif
#ifndef TBUFFER_HH
#include "tBuffer.hh"
#endif

/**
* This class is needed to run several threads on a single genome.
 *
 * @see cCPUStack, cHeadMultiMem, cHardwareSMT
 **/

class cHeadMultiMem; // aggregate
class cCodeLabel; // aggregate
class cCPUStack; // aggregate
class cHardwareBase;
class cInjectGenotype;
template <class T> class tBuffer; // aggregate

struct cHardwareSMT_Thread {
private:
  int id;
public:
  cHeadMultiMem heads[NUM_HEADS];
  UCHAR cur_head;
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
	
  void SaveState(std::ostream & fp);
  void LoadState(std::istream & fp);
};

#endif
