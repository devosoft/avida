//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CPU_STACK_HH
#define CPU_STACK_HH

#include <iostream>

#ifndef CPU_DEFS_HH
#include "cpu_defs.hh"
#endif

/**
 * A CPU stack, used by various hardware components.
 *
 * @see cCPUThread, cHeadCPU, cHardware
 **/

class cCPUStack {
private:
  int stack[STACK_SIZE];
  unsigned char stack_pointer;
public:
  cCPUStack();
  cCPUStack(const cCPUStack & in_stack);
  ~cCPUStack();

  void operator=(const cCPUStack & in_stack);

  inline void Push(int value);
  inline int Pop();
  inline int Get(int depth=0) const;
  inline void Clear();
  inline int Top();
  void Flip();

  bool OK();

  void SaveState(std::ostream & fp);
  void LoadState(std::istream & fp);
};


inline void cCPUStack::Push(int value)
{
  if (stack_pointer == 0) stack_pointer = STACK_SIZE - 1;
  else stack_pointer--;
  stack[stack_pointer] = value;
}

inline int cCPUStack::Pop()
{
  int value = stack[stack_pointer];
  stack[stack_pointer] = 0;
  stack_pointer++;
  if (stack_pointer == STACK_SIZE) stack_pointer = 0;
  return value;
}

inline int cCPUStack::Get(int depth) const
{
  int array_pos = depth + stack_pointer;
  if (array_pos >= STACK_SIZE) array_pos -= STACK_SIZE;
  return stack[array_pos];
}

inline void cCPUStack::Clear()
{
  for (int i =0; i < STACK_SIZE; i++) { stack[i] = 0; }
  stack_pointer = 0;
}

inline int cCPUStack::Top()
{
  return stack[stack_pointer];
}

#endif
