/*
 *  cIntegratedScheduleNode.h
 *  Avida
 *
 *  Called "integrated_schedule_node.hh" prior to 12/7/05.
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

#ifndef cIntegratedScheduleNode_h
#define cIntegratedScheduleNode_h

#ifndef tArray_h
#include "tArray.h"
#endif

/**
 * The cIntegratedScheduleNode object manages bundlings of item's for the
 * integrated time slicing object (cIntegratedSchedule).  When GetNextID()
 * is called on one of these nodes, it must either choose from itself, or
 * pass the call down to the nodes below it (by running the GetNextID()
 * method of the next node).  If the node alternates between everything in
 * its own list the next node's list, then we have a perfect Logrithmic
 * (base 2) decrease in activity.  Sometimes a merit will be skipped in the
 * list, so the next node should only be called one out of every four times,
 * etc.
 *
 * This allows binary representations of merits to determine which nodes
 * each item should be included in.
 **/

class cIntegratedScheduleNode
{
private:
  tArray<int> active_array; // Each cell in this array corressponds to the
                      //  item with the same ID.  If creature is not in the
                      //  list, its value in the array will be 0. If it is in
                      //  the list, it will  point to the cell of the next
                      //  included creature.  The last included creature has
                      //  a -1 in its cell.
  int first_entry;    // ID of first active creature.
  int active_entry;   // ID of next scheduled entry.
  int node_id;        // A unique id (representing the relative merit bit).

  int size;           // Number of active items in this node.
  int process_size;   // Number of times this node should be executed before
                      //   the next node is.
  int process_count;  // Number of times this node has been executed.
  bool execute;       // Should this node execute or pass?

  cIntegratedScheduleNode* next;
  cIntegratedScheduleNode* prev;
  
  
  cIntegratedScheduleNode(const cIntegratedScheduleNode&); // @not_implemented
  cIntegratedScheduleNode& operator=(const cIntegratedScheduleNode&); // @not_implemented
  
public:
  cIntegratedScheduleNode(int _item_count = 0, int in_id = -1)
    : active_array(_item_count), first_entry(-1), active_entry(-1), node_id(in_id), size(0)
    , process_size(1), process_count(0), execute(true), next(NULL), prev(NULL)
  {
      active_array.SetAll(0);
  }
  ~cIntegratedScheduleNode() { ; }

  void Insert(int item_id);
  void Remove(int item_id);
  int GetNextID();

  bool OK();

  inline void SetProcessSize(int in_p_size) { process_size = in_p_size; }
  inline void SetNext(cIntegratedScheduleNode * in_next) { next = in_next; }
  inline void SetPrev(cIntegratedScheduleNode * in_prev) { prev = in_prev; }

  inline int GetID() { return node_id; }
  inline int GetSize() { return size; }
  inline int GetProcessSize() { return process_size; }
  inline int GetProcessCount() { return process_count; }
  inline cIntegratedScheduleNode * GetNext() { return next; }
  inline cIntegratedScheduleNode * GetPrev() { return prev; }
};

#endif
