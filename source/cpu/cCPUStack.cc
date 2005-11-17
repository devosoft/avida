/*
 *  cCPUStack.cc
 *  Avida
 *
 *  Created by David on 11/17/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1999-2001 California Institute of Technology.
 *
 */

#include "cCPUStack.h"

#include <assert.h>
#include "cString.h"

using namespace std;


cCPUStack::cCPUStack(const cCPUStack & in_stack)
{
  for (int i = 0; i < nHardware::STACK_SIZE; i++) {
    stack[i] = in_stack.stack[i];
  }
  stack_pointer = in_stack.stack_pointer;
}

void cCPUStack::operator=(const cCPUStack & in_stack)
{
  for (int i = 0; i < nHardware::STACK_SIZE; i++) {
    stack[i] = in_stack.stack[i];
  }
  stack_pointer = in_stack.stack_pointer;
}

void cCPUStack::Flip()
{
  int new_stack[nHardware::STACK_SIZE];
  int i;
  for (i = 0; i < nHardware::STACK_SIZE; i++) new_stack[i] = Pop();
  for (i = 0; i < nHardware::STACK_SIZE; i++) Push(new_stack[i]);
}

bool cCPUStack::OK()
{
  assert(stack_pointer < nHardware::STACK_SIZE); // stack_pointer out of range
  return true;
}

void cCPUStack::SaveState(ostream& fp)
{
  assert(fp.good());
  fp<<"cCPUStack"<<" ";
  // stack (in inverse order so load can just push)
  for(int i = nHardware::STACK_SIZE - 1; i >= 0; i-- ){
    fp<<Get(i)<<" ";
  }
  fp<<endl;
}

void cCPUStack::LoadState(istream & fp)
{
  assert(fp.good());
  cString foo;
  fp>>foo;
  assert( foo == "cCPUStack");
  int value;
  for( int i=0; i<nHardware::STACK_SIZE; ++i ){
    fp>>value;
    Push(value);
  }
}
