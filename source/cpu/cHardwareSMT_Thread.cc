/*
 *  cHardwareSMT_Thread.cc
 *  Avida
 *
 *  Created by David on 6/4/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#include "cHardwareSMT_Thread.h"

using namespace std;

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
	for (int i = 0; i < nHardware::NUM_HEADS; i++) {
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
  for (int i = 0; i < nHardware::NUM_HEADS; i++) {
    heads[i] = in_thread.heads[i];
  }
  owner = in_thread.owner;
}

void cHardwareSMT_Thread::Reset(cHardwareBase * in_hardware, int _id)
{
  id = _id;
	
  for (int i = 0; i < nHardwareSMT::NUM_LOCAL_STACKS; i++) local_stacks[i].Clear();
  for (int i = 0; i < nHardware::NUM_HEADS; i++) heads[i].Reset(0, in_hardware);
	
  cur_head = nHardware::HEAD_IP;
  read_label.Clear();
  next_label.Clear();
  owner = NULL;
}
