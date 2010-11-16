/*
 *  cChangeList.h
 *  Avida
 *
 *  Called "change_list.hh" prior to 12/2/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2005 California Institute of Technology.
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

#ifndef cChangeList_h
#define cChangeList_h

#include "tArray.h"


/**
 * This class provides an array of indices of changes to some list of
 * objects. Entries in the array of indices have the same order as
 * registered changes.
 **/ 

class cChangeList
{
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
  explicit cChangeList(int capacity = 0)
    : m_change_count(0), m_change_list(0), m_change_tracking(0)
  {
    ResizeClear(capacity);
  }
  ~cChangeList() { ; }

  void ResizeClear(int capacity);

  int GetSize() const { return m_change_list.GetSize(); }
  int GetChangeCount() const { return m_change_count; }

  // Note that decreasing size invalidates stored changes.
  void Resize(int capacity);
  
  // Unsafe version : assumes index is within change count.
  int GetChangeAt(int index) const { return m_change_list[index]; }

  // Safe version : returns -1 if index is outside change count.
  int CheckChangeAt(int index) const
  {
    return (index < m_change_count) ? ((int) GetChangeAt(index)) : (-1);
  }
  
  // Unsafe version : assumes changed_index is within capacity.
  void MarkChange(int changed_index);

  // Safe version : will resize to accommodate changed_index greater
  // than capacity.
  void PushChange(int changed_index);

  void Reset();
};

#endif
