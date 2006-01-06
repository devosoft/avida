/*
 *  cHeadMultiMem.cc
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#include "cHeadMultiMem.h"

#include "cCPUMemory.h"
#include "cHardwareBase.h"
#include "cInstSet.h"
#include "cInstruction.h"

#include <assert.h>

void cHeadMultiMem::Adjust()
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

void cHeadMultiMem::Reset(int in_mem_space, cHardwareBase * new_hardware)
{
  if (new_hardware) main_hardware = new_hardware;
  cur_hardware  = new_hardware;
  position = 0;
  mem_space = in_mem_space;
}

void cHeadMultiMem::Set(int new_pos, int in_mem_space, cHardwareBase * in_hardware)
{
  position = new_pos;
  if (in_hardware) cur_hardware = in_hardware;
  mem_space = in_mem_space;
  Adjust();
}

void cHeadMultiMem::Set(const cHeadMultiMem & in_head)
{
  position = in_head.position;
  cur_hardware = in_head.cur_hardware;
  mem_space = in_head.mem_space;
  Adjust();
}

void cHeadMultiMem::LoopJump(int jump)
{
  position += jump;

  // If we are out of range, bring back in.
  if (position < 0 || position >= GetMemory().GetSize()) {
    position %= GetMemory().GetSize();
    if (position <= 0) position += GetMemory().GetSize();
  }
}

const cCPUMemory & cHeadMultiMem::GetMemory() const
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory(mem_space);
}

cCPUMemory & cHeadMultiMem::GetMemory()
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory(mem_space);
}

const cInstruction & cHeadMultiMem::GetInst() const
{
  assert(position >= 0);
  assert(position < GetMemory().GetSize());
  return GetMemory()[position];
}

const cInstruction & cHeadMultiMem::GetInst(int offset) const 
{
  int new_pos = position + offset;
  return GetMemory()[new_pos];
}


void cHeadMultiMem::SetInst(const cInstruction & value)
{
  assert(cur_hardware != NULL);
#ifdef WRITE_PROTECTION
  if (main_hardware == cur_hardware) GetMemory()[position] = value;
#else
  GetMemory()[position] = value;
  if (main_hardware != cur_hardware) cur_hardware->SetModified();
#endif
}

void cHeadMultiMem::InsertInst(const cInstruction & value)
{
  assert(cur_hardware != NULL);
#ifdef WRITE_PROTECTION
  if (main_hardware == cur_hardware) GetMemory().Insert(position, value);
#else
  GetMemory().Insert(position, value);
  if (main_hardware != cur_hardware) cur_hardware->SetModified();
#endif
}

void cHeadMultiMem::RemoveInst()
{
  assert(cur_hardware != NULL);
#ifdef WRITE_PROTECTION
  if (main_hardware == cur_hardware) GetMemory().Remove(position);
#else
  GetMemory().Remove(position);
  if (main_hardware != cur_hardware) cur_hardware->SetModified();
#endif
}

const cInstruction & cHeadMultiMem::GetNextInst()
{
  return (AtEnd()) ? cInstSet::GetInstError() : GetMemory()[position+1];
}

void cHeadMultiMem::SetFlagCopied()
{
  assert(cur_hardware != NULL);
  GetMemory().SetFlagCopied(position);     
}

void cHeadMultiMem::SetFlagMutated()
{
  assert(cur_hardware != NULL);
  GetMemory().SetFlagMutated(position);    
}

void cHeadMultiMem::SetFlagExecuted()
{
  assert(cur_hardware != NULL);
  GetMemory().SetFlagExecuted(position);   
}

void cHeadMultiMem::SetFlagBreakpoint()
{
  assert(cur_hardware != NULL);
  GetMemory().SetFlagBreakpoint(position); 
}

void cHeadMultiMem::SetFlagPointMut()
{
  assert(cur_hardware != NULL);
  GetMemory().SetFlagPointMut(position);   
}

void cHeadMultiMem::SetFlagCopyMut()
{
  assert(cur_hardware != NULL);
  GetMemory().SetFlagCopyMut(position);    
}

void cHeadMultiMem::ClearFlagCopied()
{
  assert(cur_hardware != NULL);
  GetMemory().ClearFlagCopied(position);     
}

void cHeadMultiMem::ClearFlagMutated()
{
  assert(cur_hardware != NULL);
  GetMemory().ClearFlagMutated(position);    
}

void cHeadMultiMem::ClearFlagExecuted()
{
  assert(cur_hardware != NULL);
  GetMemory().ClearFlagExecuted(position);   
}

void cHeadMultiMem::ClearFlagBreakpoint()
{
  assert(cur_hardware != NULL);
  GetMemory().ClearFlagBreakpoint(position); 
}

void cHeadMultiMem::ClearFlagPointMut()
{
  assert(cur_hardware != NULL);
  GetMemory().ClearFlagPointMut(position);   
}

void cHeadMultiMem::ClearFlagCopyMut()
{
  assert(cur_hardware != NULL);
  GetMemory().ClearFlagCopyMut(position);    
}

cHeadMultiMem & cHeadMultiMem::operator=(const cHeadMultiMem & in_cpu_head)
{
  main_hardware = in_cpu_head.main_hardware;
  cur_hardware  = in_cpu_head.cur_hardware;
  position = in_cpu_head.position;
  mem_space = in_cpu_head.mem_space;
  return *this;
}

bool cHeadMultiMem::operator==(const cHeadMultiMem & in_cpu_head) const 
{
  return (cur_hardware == in_cpu_head.cur_hardware) && 
    (position == in_cpu_head.position) &&
    (mem_space == in_cpu_head.mem_space);
}
  
bool cHeadMultiMem::AtEnd() const
{
  return (position + 1 == GetMemory().GetSize());
}

bool cHeadMultiMem::InMemory() const
{
  return (position >= 0 && position < GetMemory().GetSize());
}

