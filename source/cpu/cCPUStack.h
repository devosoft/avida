/*
 *  cCPUStack.h
 *  Avida
 *
 *  Called "cpu_stack.hh" prior to 11/17/05.
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

#ifndef cCPUStack_h
#define cCPUStack_h

#include <iostream>

#ifndef nHardware_h
#include "nHardware.h"
#endif

class cCPUStack
{
private:
  int stack[nHardware::STACK_SIZE];
  unsigned char stack_pointer;

public:
  cCPUStack() { Clear(); }
  cCPUStack(const cCPUStack& in_stack);
  ~cCPUStack() { ; }

  void operator=(const cCPUStack& in_stack);

  inline void Push(int value);
  inline int Pop();
  inline int& Peek() { return stack[stack_pointer]; }
  inline int Peek() const { return stack[stack_pointer]; }
  inline int Get(int depth=0) const;
  inline void Clear();
  inline int Top();
  void Flip();

  bool OK();

  void SaveState(std::ostream& fp);
  void LoadState(std::istream & fp);
};


inline void cCPUStack::Push(int value)
{
  if (stack_pointer == 0) stack_pointer = nHardware::STACK_SIZE - 1;
  else stack_pointer--;
  stack[stack_pointer] = value;
}

inline int cCPUStack::Pop()
{
  int value = stack[stack_pointer];
  stack[stack_pointer] = 0;
  stack_pointer++;
  if (stack_pointer == nHardware::STACK_SIZE) stack_pointer = 0;
  return value;
}

inline int cCPUStack::Get(int depth) const
{
  int array_pos = depth + stack_pointer;
  if (array_pos >= nHardware::STACK_SIZE) array_pos -= nHardware::STACK_SIZE;
  return stack[array_pos];
}

inline void cCPUStack::Clear()
{
  for (int i = 0; i < nHardware::STACK_SIZE; i++) stack[i] = 0;
  stack_pointer = 0;
}

inline int cCPUStack::Top()
{
  return stack[stack_pointer];
}

#endif
