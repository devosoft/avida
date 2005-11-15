//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_4STACK_THREAD_HH
#include "cHardware4Stack_Thread.h"
#endif

using namespace std;

/////////////////////////
//  cHardware4Stack_Thread
/////////////////////////

cHardware4Stack_Thread::cHardware4Stack_Thread(cHardwareBase * in_hardware, int _id)
{
  Reset(in_hardware, _id);
}

cHardware4Stack_Thread::cHardware4Stack_Thread(const cHardware4Stack_Thread & in_thread, int _id)
{
   id = _id;
   if (id == -1) id = in_thread.id;
   for (int i = 0; i < nHardware4Stack::NUM_LOCAL_STACKS; i++) {
     local_stacks[i] = in_thread.local_stacks[i];
   }
   for (int i = 0; i < nHardware::NUM_HEADS; i++) {
     heads[i] = in_thread.heads[i];
   }
   owner = in_thread.owner;
}

cHardware4Stack_Thread::~cHardware4Stack_Thread() {}

void cHardware4Stack_Thread::operator=(const cHardware4Stack_Thread & in_thread)
{
  id = in_thread.id;
  for (int i = 0; i < nHardware4Stack::NUM_LOCAL_STACKS; i++) {
    local_stacks[i] = in_thread.local_stacks[i];
  }
  for (int i = 0; i < nHardware::NUM_HEADS; i++) {
    heads[i] = in_thread.heads[i];
  }
  owner = in_thread.owner;
}

void cHardware4Stack_Thread::Reset(cHardwareBase * in_hardware, int _id)
{
  id = _id;

  for (int i = 0; i < nHardware4Stack::NUM_LOCAL_STACKS; i++) local_stacks[i].Clear();
  for (int i = 0; i < nHardware::NUM_HEADS; i++) heads[i].Reset(0, in_hardware);

  cur_head = nHardware::HEAD_IP;
  read_label.Clear();
  next_label.Clear();
  owner = NULL;
}
