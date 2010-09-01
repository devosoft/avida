/*
 *  cCPUStack.cc
 *  Avida
 *
 *  Called "cpu_stack.cc" prior to 11/17/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1999-2001 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
