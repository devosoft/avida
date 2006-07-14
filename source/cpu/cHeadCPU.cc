/*
 *  cHeadCPU.cc
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#include "cHeadCPU.h"

#include "cCodeLabel.h"
#include "cGenome.h"
#include "cInstruction.h"
#include "cStringUtil.h"

#include <assert.h>


void cHeadCPU::Adjust()
{
  const int mem_size = GetMemory().GetSize();
  
  // If we are still in range, stop here!
  if (m_position >= 0 && m_position < mem_size) return;
  
  // If the memory is gone, just stick it at the begining of its parent.
  if (mem_size == 0 || m_position < 0) m_position = 0;
  
  // position back at the begining of the creature as necessary.
  m_position %= GetMemory().GetSize();
}


/*
 FindLabel(direction)

 Search in 'direction' (+ or - 1) from this head for the compliment of
 the label in 'next_label' and return a head to the resulting pos.
*/
cHeadCPU cHeadCPU::FindLabel(const cCodeLabel& label, int direction)
{
  // Make sure the label is of size > 0.
  if (label.GetSize() == 0) return *this;
  
  int found_pos = -1;
  
  // Call special functions depending on if jump is forwards or backwards.
  if (direction < 0) {
    found_pos = FindLabel_Backward(label, GetMemory(), m_position - label.GetSize());
  } else {
    found_pos = FindLabel_Forward(label, GetMemory(), m_position);
  }
  
  if (found_pos >= 0) {
    // Return the last line of the found label, or the starting point.
    cHeadCPU search_head(*this);
    search_head.Set(found_pos - 1);
    return search_head;
  }
  
  // It wasn't found; return the starting position of the instruction pointer.
  return *this;
}

/*
 FindLabel_Forward

 Search forwards for search_label from _after_ position pos in the
 memory.  Return the first line _after_ the the found label.  It is okay
 to find search label's match inside another label.
*/
int cHeadCPU::FindLabel_Forward(const cCodeLabel& search_label, const cGenome& search_mem, int pos)
{ 
  assert(pos < search_mem.GetSize() && pos >= 0);
  
  int search_start = pos;
  int label_size = search_label.GetSize();
  bool found_label = false;
  const cInstSet& inst_set = m_hardware->GetInstSet();
  
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


/*
 FindLabel_Backward
 
 Search backwards for search_label from _before_ position pos in the
 memory.  Return the first line _after_ the the found label.  It is okay
 to find search label's match inside another label.
*/
int cHeadCPU::FindLabel_Backward(const cCodeLabel& search_label, const cGenome& search_mem, int pos)
{ 
  assert(pos < search_mem.GetSize());
  
  int search_start = pos;
  int label_size = search_label.GetSize();
  bool found_label = false;
  const cInstSet& inst_set = m_hardware->GetInstSet();
  
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
