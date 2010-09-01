/*
 *  cIntegratedScheduleNode.cc
 *  Avida
 *
 *  Called "integrated_schedule_node.cc" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cIntegratedScheduleNode.h"


bool cIntegratedScheduleNode::OK()
{
  bool result = true;

  // Make sure the active_array is setup correctly.

  int size_check = 0;
  int next_check = first_entry;
  for (int i = 0; i < active_array.GetSize(); i++) {
    if (active_array[i] != 0) {
      size_check++;
      assert(next_check == i);  //  Node entries do no match!
      next_check = active_array[i];
    }
  }
  assert(next_check == -1);  // Node array not properly terminated.

  // Make sure the sizes line up...
  assert(size == size_check);  // size and active node count mismatch.

  return result;
}

void cIntegratedScheduleNode::Insert(int item_id)
{
  assert(item_id >= 0 && item_id < active_array.GetSize());  // Illegal ID

  // If this item is already active in this node, ignore this call...
  if (active_array[item_id] != 0) return;

  // See if we're dealing with a new first_entry...
  if (first_entry == -1 || item_id < first_entry) {
    active_array[item_id] = first_entry;
    first_entry = item_id;
  }
  else {
    // Otherwise find the predecessor to this item in the list...
    int prev_item;
    for (prev_item = item_id - 1; prev_item >= 0; prev_item--) {
      if (active_array[prev_item] != 0) break;
    }
    assert(prev_item >= 0);  // prev_item is first, but not identified.
    
    // Make the predecessor point to it, and have it point to the CPU that
    // the old predecessor pointed to.
    active_array[item_id] = active_array[prev_item];
    active_array[prev_item] = item_id;
  }

  size++;
}

void cIntegratedScheduleNode::Remove(int item_id)
{
  assert(item_id >= 0 && item_id < active_array.GetSize()); // Illegal ID

  // If this item is already inactive, ignore this call...
  if (active_array[item_id] == 0) return;

  // If this is the first_entry, adjust it!
  if (first_entry == item_id) {
    first_entry = active_array[item_id];
  }
  else {
    // Find the predecessor to this item in the list...
    int prev_item;
    for (prev_item = item_id - 1; prev_item >= 0; prev_item--) {
      if (active_array[prev_item] != 0) break;
    }
    assert(prev_item >= 0);  // prev_item is first, but not identified.

    // Make the predecessor point to the item removed used to point to.
    active_array[prev_item] = active_array[item_id];
  }

  active_array[item_id] = 0;
  size--;
}


// Execute everything on list, and then shift to calling the next node.
// Wait for the next node to return a -1 before shifting back to this one.

int cIntegratedScheduleNode::GetNextID()
{
  // Alternate between this node's Process and the next's.
  if (execute == false) {
    // If there is a next node, we may be working on it... 
    int next_id = -1;
    if (next != NULL) next_id = next->GetNextID();
   
    // If next_id is a -1, either we don't have a next node, or else it
    // is finished with its execution.

    if (next_id == -1) {
      execute = true;
      process_count = 0;
      active_entry = -1;
    }

    return next_id;
  }

  // Find the next active_entry...
  
  // If we were at the end of the list, start over...
  if (active_entry == -1) active_entry = first_entry;

  // If this entry no longer exists, hunt for the next active entry manually...
  else if (active_array[active_entry] == 0) {
    while (active_entry < active_array.GetSize() &&
	   active_array[active_entry] == 0) {
      active_entry++;
    }
    if (active_entry == active_array.GetSize()) active_entry = -1;
  }
  
  // Otherwise, if the entry does exist, we can just look the next one up.
  else active_entry = active_array[active_entry];


  // If we have now hit the end of this list, move on to the next node.

  if (active_entry == -1) {
    process_count++;
    if (process_count >= process_size) execute = false;
  }

  return active_entry;
}
