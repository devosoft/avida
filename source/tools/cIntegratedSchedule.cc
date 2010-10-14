/*
 *  cIntegratedSchedule.cc
 *  Avida
 *
 *  Called "integrated_schedule.cc" prior to 12/7/05.
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

#include "cIntegratedSchedule.h"

#include "cDeme.h"
#include "cChangeList.h"
#include "cIntegratedScheduleNode.h"
#include "cMerit.h"

#include "AvidaTools.h"

#include <iostream>

using namespace std;
using namespace AvidaTools;


cIntegratedSchedule::cIntegratedSchedule(int _item_count)
  : cSchedule(_item_count)
{
  num_active_nodes = 0;

  // Create the merit_chart; they should all init to default value.
  merit_chart = new cMerit[item_count];
  for (int i = 0; i < item_count; i++) {
    merit_chart[i] = 0;
  }
}

cIntegratedSchedule::~cIntegratedSchedule()
{
  for (int i = 0; i < node_array.GetSize(); i++) {
    if (node_array[i] != NULL) delete node_array[i];
  }
  delete [] merit_chart;
}

bool cIntegratedSchedule::OK()
{
  // Test that we have an acurate count of the number of active nodes.

  int test_active_nodes = 0;
  for (int i = 0; i < node_array.GetSize(); i++) {
    if (node_array[i]) test_active_nodes++;
  }

  assert(test_active_nodes == num_active_nodes); // active_node count mismatch.

  // Test each node to make sure it is OK.

  for (int i = 0; i < node_array.GetSize(); i++) {
    if (node_array[i] != NULL) node_array[i]->OK();
  }

  return true;
}

void cIntegratedSchedule::Adjust(int item_id, const cMerit& new_merit, int deme_id)
{
  if (cChangeList *change_list = GetChangeList()) {
    change_list->MarkChange(item_id);
  }
  // Grab the old_merit, the new merit, and compare them.
  const cMerit old_merit = merit_chart[item_id];

  // If the merit is still the same, we're done here.
  if (old_merit == new_merit) return;

  // Save the new merit to the merit_chart.
  merit_chart[item_id] = new_merit;

  // Re-adjust the lists.
  int merit_magnitude = Max( old_merit.GetNumBits(), new_merit.GetNumBits() );
  for (int i = 0; i < merit_magnitude; i++) {
    bool old_bit = old_merit.GetBit(i);
    bool new_bit = new_merit.GetBit(i);

    if (old_bit && !new_bit) {
      // Remove the item from this node...
      node_array[i]->Remove(item_id);
      if (node_array[i]->GetSize() == 0) RemoveNode(i);
    }

    if (!old_bit && new_bit) {
      // Add the item from this node...
      if (i >= node_array.GetSize() || !node_array[i]) InsertNode(i);
      node_array[i]->Insert(item_id);
    }
  }
}


int cIntegratedSchedule::GetNextID()
{
  assert(node_array.GetSize() > 0);  // Running scheduler w/ no entries!

  const int last_id = node_array.GetSize() - 1;

  // Make sure there are organisms in the scheduler!
  if (node_array[last_id] == NULL) return -1;

  int next_id = -1;
  while (next_id < 0) {
    next_id = node_array[last_id]->GetNextID();
  }
  
  return next_id;
}

double cIntegratedSchedule::GetStatus(int id)
{
  return merit_chart[id].GetDouble();
}


///////// --- private //////////

void cIntegratedSchedule::InsertNode(int node_id)
{
  // Test if trying to create node that already exists.
  assert(node_id >= node_array.GetSize() || node_array[node_id] == NULL);

  cIntegratedScheduleNode * new_node =
    new cIntegratedScheduleNode(item_count, node_id);

  if (node_id >= node_array.GetSize()) ResizeNodes(node_id);

  node_array[node_id] = new_node;

  // Find the node to mark as the 'prev'.
  for (int prev_id = node_id + 1; prev_id < node_array.GetSize(); prev_id++) {
    cIntegratedScheduleNode * prev_node = node_array[prev_id];
    if (prev_node) {
      new_node->SetPrev(prev_node);
      prev_node->SetNext(new_node);
      prev_node->SetProcessSize(1 << (prev_id - node_id - 1));
      break;
    }
  }

  // And find the node to mark as the 'next'.
  for (int next_id = node_id - 1; next_id >= 0; next_id--) {
    cIntegratedScheduleNode * next_node = node_array[next_id];
    if (next_node) {
      new_node->SetNext(next_node);
      next_node->SetPrev(new_node);
      new_node->SetProcessSize(1 << (node_id - next_id - 1));
      break;
    }
  }

  num_active_nodes++;
}

void cIntegratedSchedule::RemoveNode(int node_id)
{
  assert(node_array[node_id] != NULL); // Trying to remove non-existant node.

  cIntegratedScheduleNode * old_node = node_array[node_id];
  cIntegratedScheduleNode * next_node = old_node->GetNext();
  cIntegratedScheduleNode * prev_node = old_node->GetPrev();
  node_array[node_id] = NULL;

  if (next_node) next_node->SetPrev(prev_node);
  if (prev_node) {
    prev_node->SetNext(next_node);
    prev_node->SetProcessSize(old_node->GetProcessSize() *
			      prev_node->GetProcessSize() * 2);
  }

  if (node_id == node_array.GetSize() - 1) {
    if (!old_node->GetNext()) ResizeNodes(0);
    else ResizeNodes(old_node->GetNext()->GetID());
  }

  delete old_node;

  num_active_nodes--;
}

void cIntegratedSchedule::ResizeNodes(int new_max)
{
  int old_size = node_array.GetSize();
  int new_size = new_max + 1;  // 0 to new_max...

  // Clean up tail portions of the array being cut off.
  for (int i = new_size; i < old_size; i++) {
    if (node_array[i]) delete node_array[i];
  }

  node_array.Resize(new_size);

  // Mark as NULL any new cells added to the array.
  for (int i = old_size; i < new_size; i++) {
    node_array[i] = NULL;
  }
}
