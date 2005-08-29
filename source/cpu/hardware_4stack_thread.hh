//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_4STACK_THREAD_HH
#define HARDWARE_4STACK_THREAD_HH

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
#ifndef HARDWARE_4STACK_CONSTANTS_HH
#include "hardware_4stack_constants.hh"
#endif
#ifndef TBUFFER_HH
#include "tBuffer.hh"
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
  cHeadMultiMem heads[NUM_HEADS];
  UCHAR cur_head;
  cCPUStack local_stacks[NUM_LOCAL_STACKS];

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

  void SaveState(std::ostream & fp);
  void LoadState(std::istream & fp);
};

#endif
