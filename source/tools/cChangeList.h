/*
 *  cChangeList.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2005 California Institute of Technology.
 *
 */

#ifndef cChangeList_h
#define cChangeList_h

#ifndef tArray_h
#include "tArray.h"
#endif

/**
 * This class provides an array of indices of changes to some list of
 * objects. Entries in the array of indices have the same order as
 * registered changes.
 **/ 

class cChangeList {
protected:
  /*
  Note that size of m_change_list is size of m_change_tracking, and that
  0 <= m_change_count <= size of m_change_tracking.
  */
  // Number of changes.
  int m_change_count;
  /*
  List of changed indices. When n changes are listed, the first n
  entries of m_change_list store the indices, and the remaining entries
  are invalid.
  */
  tArray<int> m_change_list;
  // m_change_tracking[i] is true iff i is in m_change_list.
  tArray<bool> m_change_tracking;

public:
  void ResizeClear(int capacity);

  // Constructor.
  explicit cChangeList(int capacity = 0);

  //// Assignment operator.
  //cChangeList & operator= (const cChangeList & rhs) {
  //  m_change_list = rhs.m_change_list;
  //  m_change_tracking = rhs.m_change_tracking;
  //  m_change_count = rhs.m_change_count;
  //}

  //// Copy constructor.
  //explicit cChangeList(const cChangeList & rhs) : cChangeList(0) {
  //  this->operator=(rhs);
  //}

  // Destructor.
  //virtual ~cChangeList(){}

  // Interface Methods ///////////////////////////////////////////////////////

  int GetSize() const;

  int GetChangeCount() const;

  // Note that decreasing size invalidates stored changes.
  void Resize(int capacity);
  
  // Unsafe version : assumes index is within change count.
  int GetChangeAt(int index) const;

  // Safe version : returns -1 if index is outside change count.
  int CheckChangeAt(int index) const;

  // Unsafe version : assumes changed_index is within capacity.
  void MarkChange(int changed_index);

  // Safe version : will resize to accommodate changed_index greater
  // than capacity.
  void PushChange(int changed_index);

  void Reset();
};

#endif
