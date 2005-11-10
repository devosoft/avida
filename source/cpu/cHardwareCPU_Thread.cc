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




void cHardwareCPU_Thread::SaveState(ostream& fp){
  assert(fp.good());
  fp << "cHardwareCPU_Thread" << endl;

  // registers
  for( int i=0; i<nHardwareCPU::NUM_REGISTERS; ++i ){
    fp<<reg[i]<<endl;
  }

  // heads (@TCC does not handle parasites!!!)
  for( int i=0; i<nHardware::NUM_HEADS; ++i ){
    fp<<heads[i].GetPosition()<<endl;
  }

  stack.SaveState(fp);

  fp<<"|"; // marker
  fp<<cur_stack;
  fp<<cur_head;
  fp<<endl;

  // Code labels
  read_label.SaveState(fp);
  next_label.SaveState(fp);
}



void cHardwareCPU_Thread::LoadState(istream & fp){
  assert(fp.good());
  cString foo;
  fp >> foo;
  assert( foo == "cHardwareCPU_Thread");

  // registers
  for( int i=0; i<nHardwareCPU::NUM_REGISTERS; ++i ){
    fp>>reg[i];
  }

  // heads (@TCC does not handle parasites!!!)
  for( int i=0; i<nHardware::NUM_HEADS; ++i ){
    int pos;
    fp>>pos;
    heads[i].AbsSet(pos);
  }

  // stack
  stack.LoadState(fp);

  char marker; fp>>marker; assert( marker == '|' );
  /* YIKES!  data loss below: */
  char the_cur_stack = cur_stack;
  char the_cur_head = cur_head;
  fp.get(the_cur_stack);
  fp.get(the_cur_head);

  // Code labels
  read_label.LoadState(fp);
  next_label.LoadState(fp);
}
