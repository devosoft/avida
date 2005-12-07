/*
 *  cIntegratedScheduleNode.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
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

class cIntegratedScheduleNode {
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

  cIntegratedScheduleNode * next;
  cIntegratedScheduleNode * prev;
public:
  cIntegratedScheduleNode(int _item_count = 0, int in_id = -1);
  ~cIntegratedScheduleNode();

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
