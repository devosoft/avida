/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef FOURSTACK_HEAD_HH
#include "4stack_head.hh"
#endif

#ifndef CPU_MEMORY_HH
#include "cpu_memory.hh"
#endif
#ifndef HARDWARE_BASE_HH
#include "hardware_base.hh"
#endif
#ifndef INST_SET_HH
#include "inst_set.hh"
#endif
#ifndef INSTRUCTION_HH
#include "instruction.hh"
#endif

#include <assert.h>

c4StackHead::c4StackHead() : cCPUHead() { mem_space=0; }

c4StackHead::c4StackHead(cHardwareBase * in_hardware, int in_pos, int in_mem_space) 
  : cCPUHead(in_hardware, in_pos) { mem_space = in_mem_space; }

c4StackHead::c4StackHead(const c4StackHead & in_head) : cCPUHead(in_head) 
{ 
  mem_space = in_head.mem_space; 
}

void c4StackHead::Adjust()
{
  assert(cur_hardware != NULL);
  assert(main_hardware != NULL);

  const int mem_size = GetMemory().GetSize();

  // If we are still in range, stop here!
  if (position >= 0 && position < mem_size) return;

  // If the memory is gone, just stick it at the begining of its parent.
  if (mem_size == 0) {
    cur_hardware = main_hardware;
    position = 0;
  }
  else if (position <= 0) { position = 0; }
  else if (position >= mem_size) {
    // Always loop into the begining of the owner hardware.
    cur_hardware = main_hardware;
    position -= mem_size;
    while (position >= GetMemory().GetSize()) {
      // position back at the begining of the creature.
      position %= GetMemory().GetSize();
    }
  }
}

void c4StackHead::Reset(int in_mem_space, cHardwareBase * new_hardware)
{
  if (new_hardware) main_hardware = new_hardware;
  cur_hardware  = new_hardware;
  position = 0;
  mem_space = in_mem_space;
}

void c4StackHead::Set(int new_pos, int in_mem_space, cHardwareBase * in_hardware)
{
  position = new_pos;
  if (in_hardware) cur_hardware = in_hardware;
  mem_space = in_mem_space;
  Adjust();
}

void c4StackHead::Set(const c4StackHead & in_head)
{
  position = in_head.position;
  cur_hardware = in_head.cur_hardware;
  mem_space = in_head.mem_space;
  Adjust();
}

void c4StackHead::LoopJump(int jump)
{
  position += jump;

  // If we are out of range, bring back in.
  if (position < 0 || position >= GetMemory().GetSize()) {
    position %= GetMemory().GetSize();
    if (position <= 0) position += GetMemory().GetSize();
  }
}

const cCPUMemory & c4StackHead::GetMemory() const
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory(mem_space);
}

cCPUMemory & c4StackHead::GetMemory()
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory(mem_space);
}

const cInstruction & c4StackHead::GetInst() const
{
  assert(position >= 0);
  assert(position < GetMemory().GetSize());
  return GetMemory()[position];
}

const cInstruction & c4StackHead::GetInst(int offset) const 
{
  int new_pos = position + offset;
  return GetMemory()[new_pos];
}


void c4StackHead::SetInst(const cInstruction & value)
{
  assert(cur_hardware != NULL);
#ifdef WRITE_PROTECTION
  if (main_hardware == cur_hardware) GetMemory()[position] = value;
#else
  GetMemory()[position] = value;
  if (main_hardware != cur_hardware) cur_hardware->SetModified();
#endif
}

void c4StackHead::InsertInst(const cInstruction & value)
{
  assert(cur_hardware != NULL);
#ifdef WRITE_PROTECTION
  if (main_hardware == cur_hardware) GetMemory().Insert(position, value);
#else
  GetMemory().Insert(position, value);
  if (main_hardware != cur_hardware) cur_hardware->SetModified();
#endif
}

void c4StackHead::RemoveInst()
{
  assert(cur_hardware != NULL);
#ifdef WRITE_PROTECTION
  if (main_hardware == cur_hardware) GetMemory().Remove(position);
#else
  GetMemory().Remove(position);
  if (main_hardware != cur_hardware) cur_hardware->SetModified();
#endif
}

const cInstruction & c4StackHead::GetNextInst()
{
  return (AtEnd()) ? cInstSet::GetInstError() : GetMemory()[position+1];
}

bool & c4StackHead::FlagCopied()
{
  assert(cur_hardware != NULL);
  return GetMemory().FlagCopied(position);     
}

bool & c4StackHead::FlagMutated()
{
  assert(cur_hardware != NULL);
  return GetMemory().FlagMutated(position);    
}

bool & c4StackHead::FlagExecuted()
{
  assert(cur_hardware != NULL);
  return GetMemory().FlagExecuted(position);   
}

bool & c4StackHead::FlagBreakpoint()
{
  assert(cur_hardware != NULL);
  return GetMemory().FlagBreakpoint(position); 
}

bool & c4StackHead::FlagPointMut()
{
  assert(cur_hardware != NULL);
  return GetMemory().FlagPointMut(position);   
}

bool & c4StackHead::FlagCopyMut()
{
  assert(cur_hardware != NULL);
  return GetMemory().FlagCopyMut(position);    
}

c4StackHead & c4StackHead::operator=(const c4StackHead & in_cpu_head)
{
  main_hardware = in_cpu_head.main_hardware;
  cur_hardware  = in_cpu_head.cur_hardware;
  position = in_cpu_head.position;
  mem_space = in_cpu_head.mem_space;
  return *this;
}

bool c4StackHead::operator==(const c4StackHead & in_cpu_head) const 
{
  return (cur_hardware == in_cpu_head.cur_hardware) && 
    (position == in_cpu_head.position) &&
    (mem_space == in_cpu_head.mem_space);
}
  
bool c4StackHead::AtEnd() const
{
  return (position + 1 == GetMemory().GetSize());
}

bool c4StackHead::InMemory() const
{
  return (position >= 0 && position < GetMemory().GetSize());
}

