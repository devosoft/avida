//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_CPU_THREAD_HH
#define HARDWARE_CPU_THREAD_HH

#include <iostream>

#ifndef CODE_LABEL_HH
#include "code_label.hh"
#endif
#ifndef CPU_DEFS_HH
#include "cpu_defs.hh"
#endif
#ifndef CPU_HEAD_HH
#include "cpu_head.hh"
#endif
#ifndef CPU_STACK_HH
#include "cpu_stack.hh"
#endif
#ifndef TBUFFER_HH
#include "tBuffer.hh"
#endif

/**
 * This class is needed to run several threads on a single genome.
 *
 * @see cCPUStack, cCPUHead, cHardwareCPU
 **/

class cCodeLabel; // aggregate;
class cCPUHead; // aggregate
class cCPUStack; // aggregate
class cHardwareBase;
template <class T> class tBuffer; // aggregate

struct cHardwareCPU_Thread {
private:
  int id;
public:
  int reg[NUM_REGISTERS];
  cCPUHead heads[NUM_HEADS];
  cCPUStack stack;
  UCHAR cur_stack;              // 0 = local stack, 1 = global stack.
  UCHAR cur_head;

  cCodeLabel read_label;
  cCodeLabel next_label;
public:
  cHardwareCPU_Thread(cHardwareBase * in_hardware=NULL, int _id=-1);
  cHardwareCPU_Thread(const cHardwareCPU_Thread & in_thread, int _id=-1);
  ~cHardwareCPU_Thread();

  void operator=(const cHardwareCPU_Thread & in_thread);

  void Reset(cHardwareBase * in_hardware, int _id);
  int GetID() const { return id; }
  void SetID(int _id) { id = _id; }

  void SaveState(std::ostream & fp);
  void LoadState(std::istream & fp);
};

#endif
