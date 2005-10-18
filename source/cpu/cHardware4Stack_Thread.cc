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

void cHardware4Stack_Thread::SaveState(ostream & fp){
  assert(fp.good());
  fp << "cHardware4Stack_Thread" << endl;

  // stacks (NOT WORKING! -law)
  for( int i=0; i<nHardware4Stack::NUM_STACKS; ++i ){
    local_stacks[i].SaveState(fp);
  }

  // heads (@TCC does not handle parasites!!!)
  for( int i=0; i<nHardware::NUM_HEADS; ++i ){
    fp<<heads[i].GetPosition()<<endl;
  }

  fp<<"|"; // marker
  fp<<cur_head;
  fp<<endl;

  // Code labels
  read_label.SaveState(fp);
  next_label.SaveState(fp);
}

void cHardware4Stack_Thread::LoadState(istream & fp){
  assert(fp.good());
  cString foo;
  fp >> foo;
  assert( foo == "cHardware4Stack_Thread");

  // stacks (NOT WORKING!  -law)
  for( int i=0; i<nHardware4Stack::NUM_STACKS; ++i ){
    local_stacks[i].LoadState(fp);
  }

  // heads (@TCC does not handle parasites!!!)
  for( int i=0; i<nHardware::NUM_HEADS; ++i ){
    int pos;
    fp>>pos;
    heads[i].AbsSet(pos);
  }

  char marker; fp >> marker; assert( marker == '|' );
  /* YIKES!  data loss below: */ 

  // Code labels
  read_label.LoadState(fp);
  next_label.LoadState(fp);
}
