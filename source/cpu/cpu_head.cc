/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CPU_HEAD_HH
#include "cpu_head.hh"
#endif

#ifndef CPU_MEMORY_HH
#include "cpu_memory.hh"
#endif
#ifndef CODE_LABEL_HH
#include "code_label.hh"
#endif
#ifndef GENOME_HH
#include "genome.hh"
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

class cHardwareBase; // access
class cCPUMemory; // access
class cCodeLabel; // access
class cInstSet; // access

cCPUHead::cCPUHead() {
  main_hardware = NULL;
  cur_hardware = NULL;
  position = 0;
}

cCPUHead::cCPUHead(cHardwareBase * in_hardware, int in_pos) {
  main_hardware = in_hardware;
  cur_hardware  = in_hardware;

  position = in_pos;
  if (in_pos) Adjust();
}

cCPUHead::cCPUHead(const cCPUHead & in_cpu_head) {
  main_hardware = in_cpu_head.main_hardware;
  cur_hardware  = in_cpu_head.cur_hardware;
  position = in_cpu_head.position;
}



void cCPUHead::Adjust()
{
  assert(cur_hardware != NULL);
  assert(main_hardware != NULL);

  const int mem_size = cur_hardware->GetMemory().GetSize();

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



/////////////////////////////////////////////////////////////////////////
// Method: cCPUHead::FindLabel(direction)
//
// Search in 'direction' (+ or - 1) from this head for the compliment of
//  the label in 'next_label' and return a head to the resulting pos.
//
/////////////////////////////////////////////////////////////////////////

cCPUHead cCPUHead::FindLabel(const cCodeLabel & label, int direction)
{
  // Make sure the label is of size > 0.
  if (label.GetSize() == 0) {
    return *this;
  }

  int found_pos = -1;

  // Call special functions depending on if jump is forwards or backwards.
  if( direction < 0 ) {
    found_pos =
      FindLabel_Backward(label, GetMemory(), GetPosition() - label.GetSize());
  }

  // Jump forwards.
  else {
    found_pos = FindLabel_Forward(label, GetMemory(), GetPosition());
  }

  if (found_pos >= 0) {
    // Return the last line of the found label, or the starting point.
    cCPUHead search_head(*this);
    search_head.Set(found_pos - 1);
    return search_head;
  }

  // It wasn't found; return the starting position of the instruction pointer.
  return *this;
}

void cCPUHead::Reset(cHardwareBase * new_hardware) {
  if (new_hardware) main_hardware = new_hardware;
  cur_hardware  = new_hardware;
  position = 0;
}

void cCPUHead::Set(int new_pos, cHardwareBase * in_hardware)
{
  position = new_pos;
  if (in_hardware) cur_hardware = in_hardware;
  Adjust();
}


void cCPUHead::Jump(int jump)
{
  position += jump;
  Adjust();
}

void cCPUHead::LoopJump(int jump)
{
  position += jump;

  // If we are out of range, bring back in.
  if (position < 0 || position >= GetMemory().GetSize()) {
    position %= GetMemory().GetSize();
    if (position <= 0) position += GetMemory().GetSize();
  }
}

void cCPUHead::AbsJump(int jump)
{
  position += jump;
}

void cCPUHead::Advance()
{
  position++;
  Adjust();
}

void cCPUHead::Retreat()
{
  position--;
  Adjust();
}


const cCPUMemory & cCPUHead::GetMemory() const
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory();
}


const cInstruction & cCPUHead::GetInst() const
{
  assert(position >= 0);
  assert(position < GetMemory().GetSize());
  return GetMemory()[position];
}

const cInstruction & cCPUHead::GetInst(int offset) const {
  int new_pos = position + offset;

  return GetMemory()[new_pos];
}


void cCPUHead::SetInst(const cInstruction & value)
{
  assert(cur_hardware != NULL);
#ifdef WRITE_PROTECTION
  if (main_hardware == cur_hardware) cur_hardware->GetMemory()[position] = value;
#else
  cur_hardware->GetMemory()[position] = value;
  if (main_hardware != cur_hardware) cur_hardware->SetModified();
#endif
}


void cCPUHead::InsertInst(const cInstruction & value)
{
  assert(cur_hardware != NULL);
#ifdef WRITE_PROTECTION
  if (main_hardware == cur_hardware) cur_hardware->GetMemory().Insert(position, value);
#else
  cur_hardware->GetMemory().Insert(position, value);
  if (main_hardware != cur_hardware) cur_hardware->SetModified();
#endif
}

void cCPUHead::RemoveInst()
{
  assert(cur_hardware != NULL);
#ifdef WRITE_PROTECTION
  if (main_hardware == cur_hardware) cur_hardware->GetMemory().Remove(position);
#else
  cur_hardware->GetMemory().Remove(position);
  if (main_hardware != cur_hardware) cur_hardware->SetModified();
#endif
}

const cInstruction & cCPUHead::GetNextInst()
{
  return (AtEnd()) ? cInstSet::GetInstError() : GetMemory()[position+1];
}



bool & cCPUHead::FlagCopied()
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory().FlagCopied(position);     
}

bool & cCPUHead::FlagMutated()
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory().FlagMutated(position);    
}

bool & cCPUHead::FlagExecuted()
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory().FlagExecuted(position);   
}

bool & cCPUHead::FlagBreakpoint()
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory().FlagBreakpoint(position); 
}

bool & cCPUHead::FlagPointMut()
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory().FlagPointMut(position);   
}

bool & cCPUHead::FlagCopyMut()
{
  assert(cur_hardware != NULL);
  return cur_hardware->GetMemory().FlagCopyMut(position);    
}

cCPUHead & cCPUHead::operator=(const cCPUHead & in_cpu_head)
{
  main_hardware = in_cpu_head.main_hardware;
  cur_hardware  = in_cpu_head.cur_hardware;
  position = in_cpu_head.position;
  return *this;
}


cCPUHead & cCPUHead::operator++()
{
  position++;
  Adjust();
  return *this;
}

cCPUHead & cCPUHead::operator--()
{
  position--;
  Adjust();
  return *this;
}

cCPUHead & cCPUHead::operator++(int)
{
  return operator++();
}

cCPUHead & cCPUHead::operator--(int)
{
  return operator--();
}

  
// Search forwards for search_label from _after_ position pos in the
// memory.  Return the first line _after_ the the found label.  It is okay
// to find search label's match inside another label.

int cCPUHead::FindLabel_Forward(const cCodeLabel & search_label,
				   const cGenome & search_mem, int pos)
{ 
  assert (pos < search_mem.GetSize() && pos >= 0);

  int search_start = pos;
  int label_size = search_label.GetSize();
  bool found_label = false;
  const cInstSet & inst_set = main_hardware->GetInstSet();

  // Move off the template we are on.
  pos += label_size;
  
  // Search until we find the complement or exit the memory.
  while (pos < search_mem.GetSize()) {

    // If we are within a label, rewind to the beginning of it and see if
    // it has the proper sub-label that we're looking for.

    if (inst_set.IsNop(search_mem[pos])) {
      // Find the start and end of the label we're in the middle of.

      int start_pos = pos;
      int end_pos = pos + 1;
      while (start_pos > search_start &&
	     inst_set.IsNop( search_mem[start_pos - 1] )) {
	start_pos--;
      }
      while (end_pos < search_mem.GetSize() &&
	     inst_set.IsNop( search_mem[end_pos] )) {
	end_pos++;
      }
      int test_size = end_pos - start_pos;

      // See if this label has the proper sub-label within it.
      int max_offset = test_size - label_size + 1;
      for (int offset = start_pos; offset < start_pos + max_offset; offset++) {

	// Test the number of matches for this offset.
	int matches;
	for (matches = 0; matches < label_size; matches++) {
	  if (search_label[matches] !=
	      inst_set.GetNopMod( search_mem[offset + matches] )) {
	    break;
	  }
	}

	// If we have found it, break out of this loop!
	if (matches == label_size) {
	  found_label = true;
	  break;
	}
      }

      // If we've found the complement label, set the position to the end of
      // the label we found it in, and break out.

      if (found_label == true) {
	pos = end_pos;
	break;
      }

      // We haven't found it; jump pos to just after the current label being
      // checked.
      pos = end_pos;
    }

    // Jump up a block to the next possible point to find a label,
    pos += label_size;
  }

  // If the label was not found return a -1.
  if (found_label == false) pos = -1;

  return pos;
}

// Search backwards for search_label from _before_ position pos in the
// memory.  Return the first line _after_ the the found label.  It is okay
// to find search label's match inside another label.

int cCPUHead::FindLabel_Backward(const cCodeLabel & search_label,
				   const cGenome & search_mem, int pos)
{ 
  assert (pos < search_mem.GetSize());
     
  int search_start = pos;
  int label_size = search_label.GetSize();
  bool found_label = false;
  const cInstSet & inst_set = main_hardware->GetInstSet();

  // Move off the template we are on.
  pos -= label_size;
  
  // Search until we find the complement or exit the memory.
  while (pos >= 0) {
    // If we are within a label, rewind to the beginning of it and see if
    // it has the proper sub-label that we're looking for.

    if (inst_set.IsNop( search_mem[pos] )) {
      // Find the start and end of the label we're in the middle of.

      int start_pos = pos;
      int end_pos = pos + 1;
      while (start_pos > 0 && inst_set.IsNop(search_mem[start_pos - 1])) {
	start_pos--;
      }
      while (end_pos < search_start &&
	     inst_set.IsNop( search_mem[end_pos] )) {
	end_pos++;
      }
      int test_size = end_pos - start_pos;

      // See if this label has the proper sub-label within it.
      int max_offset = test_size - label_size + 1;
      for (int offset = start_pos; offset < start_pos + max_offset; offset++) {

	// Test the number of matches for this offset.
	int matches;
	for (matches = 0; matches < label_size; matches++) {
	  if (search_label[matches] !=
	      inst_set.GetNopMod( search_mem[offset + matches] )) {
	    break;
	  }
	}

	// If we have found it, break out of this loop!
	if (matches == label_size) {
	  found_label = true;
	  break;
	}
      }

      // If we've found the complement label, set the position to the end of
      // the label we found it in, and break out.

      if (found_label == true) {
	pos = end_pos;
	break;
      }

      // We haven't found it; jump pos to just before the current label
      // being checked.
      pos = start_pos - 1;
    }

    // Jump up a block to the next possible point to find a label,
    pos -= label_size;
  }

  // If the label was not found return a -1.
  if (found_label == false) pos = -1;

  return pos;
}

bool cCPUHead::operator==(const cCPUHead & in_cpu_head) const {
  return (cur_hardware == in_cpu_head.cur_hardware) &&
    (position == in_cpu_head.position);
}

bool cCPUHead::AtEnd() const
{
  return (position + 1 == GetMemory().GetSize());
}

bool cCPUHead::InMemory() const
{
  return (position >= 0 && position < GetMemory().GetSize());
}

int cCPUHead::TestParasite() const
{
  // If CPU has a head in another creature, mark it as a parasite.
  return (cur_hardware != main_hardware);
}
