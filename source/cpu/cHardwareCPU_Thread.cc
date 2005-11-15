//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_CPU_THREAD_HH
#include "cHardwareCPU_Thread.h"
#endif

using namespace std;

/////////////////////////
//  cHardwareCPU_Thread
/////////////////////////

cHardwareCPU_Thread::cHardwareCPU_Thread(cHardwareBase * in_hardware, int _id)
{
  Reset(in_hardware, _id);
}

cHardwareCPU_Thread::cHardwareCPU_Thread(const cHardwareCPU_Thread & in_thread, int _id)
{
   id = _id;
   if (id == -1) id = in_thread.id;
   for (int i = 0; i < nHardwareCPU::NUM_REGISTERS; i++) {
     reg[i] = in_thread.reg[i];
   }
   for (int i = 0; i < nHardware::NUM_HEADS; i++) {
     heads[i] = in_thread.heads[i];
   }
   stack = in_thread.stack;
}

cHardwareCPU_Thread::~cHardwareCPU_Thread() {}

void cHardwareCPU_Thread::operator=(const cHardwareCPU_Thread & in_thread)
{
  id = in_thread.id;
  for (int i = 0; i < nHardwareCPU::NUM_REGISTERS; i++) {
    reg[i] = in_thread.reg[i];
  }
  for (int i = 0; i < nHardware::NUM_HEADS; i++) {
    heads[i] = in_thread.heads[i];
  }
  stack = in_thread.stack;
}

void cHardwareCPU_Thread::Reset(cHardwareBase * in_hardware, int _id)
{
  id = _id;

  for (int i = 0; i < nHardwareCPU::NUM_REGISTERS; i++) reg[i] = 0;
  for (int i = 0; i < nHardware::NUM_HEADS; i++) heads[i].Reset(in_hardware);

  stack.Clear();
  cur_stack = 0;
  cur_head = nHardware::HEAD_IP;
  read_label.Clear();
  next_label.Clear();
}
