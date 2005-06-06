/*
 *  hardware_smt_thread.cc
 *  Avida2
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#include "hardware_smt_thread.h"

using namespace std;

/////////////////////////
//  cHardwareSMT_Thread
/////////////////////////

cHardwareSMT_Thread::cHardwareSMT_Thread(cHardwareBase * in_hardware, int _id)
{
  Reset(in_hardware, _id);
}

cHardwareSMT_Thread::cHardwareSMT_Thread(const cHardwareSMT_Thread & in_thread, int _id)
{
	id = _id;
	if (id == -1) id = in_thread.id;
	for (int i = 0; i < nHardwareSMT::NUM_LOCAL_STACKS; i++) {
		local_stacks[i] = in_thread.local_stacks[i];
	}
	for (int i = 0; i < NUM_HEADS; i++) {
		heads[i] = in_thread.heads[i];
	}
	owner = in_thread.owner;
}

cHardwareSMT_Thread::~cHardwareSMT_Thread() {}

void cHardwareSMT_Thread::operator=(const cHardwareSMT_Thread & in_thread)
{
  id = in_thread.id;
  for (int i = 0; i < nHardwareSMT::NUM_LOCAL_STACKS; i++) {
    local_stacks[i] = in_thread.local_stacks[i];
  }
  for (int i = 0; i < NUM_HEADS; i++) {
    heads[i] = in_thread.heads[i];
  }
  owner = in_thread.owner;
}

void cHardwareSMT_Thread::Reset(cHardwareBase * in_hardware, int _id)
{
  id = _id;
	
  for (int i = 0; i < nHardwareSMT::NUM_LOCAL_STACKS; i++) local_stacks[i].Clear();
  for (int i = 0; i < NUM_HEADS; i++) heads[i].Reset(0, in_hardware);
	
  cur_head = HEAD_IP;
  read_label.Clear();
  next_label.Clear();
  owner = NULL;
}

void cHardwareSMT_Thread::SaveState(ostream & fp){
  assert(fp.good());
  fp << "cHardwareSMT_Thread" << endl;
	
  // stacks (NOT WORKING! -law)
  for( int i = 0; i < nHardwareSMT::NUM_STACKS; ++i ){
    local_stacks[i].SaveState(fp);
  }
	
  // heads (@TCC does not handle parasites!!!)
  for( int i = 0; i < NUM_HEADS; ++i ){
    fp<<heads[i].GetPosition()<<endl;
  }
	
  fp<<"|"; // marker
  fp<<cur_head;
  fp<<endl;
	
  // Code labels
  read_label.SaveState(fp);
  next_label.SaveState(fp);
}

void cHardwareSMT_Thread::LoadState(istream & fp){
  assert(fp.good());
  cString foo;
  fp >> foo;
  assert( foo == "cHardwareSMT_Thread");
	
  // stacks (NOT WORKING!  -law)
  for( int i=0; i < nHardwareSMT::NUM_STACKS; ++i ){
    local_stacks[i].LoadState(fp);
  }
	
  // heads (@TCC does not handle parasites!!!)
  for( int i = 0; i < NUM_HEADS; ++i ){
    int pos;
    fp>>pos;
    heads[i].AbsSet(pos);
  }
	
  char marker; fp >> marker; assert( marker == '|' );
  /* YIKES!  data loss below: */ 
  char the_cur_head = cur_head;
	
  // Code labels
  read_label.LoadState(fp);
  next_label.LoadState(fp);
}
