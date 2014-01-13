/*
 *  cCPUStack.cc
 *  Avida
 *
 *  Called "cpu_stack.cc" prior to 11/17/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1999-2001 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cCPUStack.h"

#include <cassert>
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
