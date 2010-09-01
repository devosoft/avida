/*
 *  cChangeList.cc
 *  Avida
 *
 *  Called "change_list.cc" prior to 12/2/05.
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

#include "cChangeList.h"


void cChangeList::ResizeClear(int capacity)
{
  m_change_list.ResizeClear(capacity);
  m_change_tracking.ResizeClear(capacity);
  m_change_list.SetAll(0);
  m_change_tracking.SetAll(false);
  m_change_count = 0;
}

// Note that decreasing size invalidates stored changes.
void cChangeList::Resize(int capacity)
{
  if (capacity < m_change_list.GetSize()) {
    ResizeClear(capacity);
  } else {
    m_change_list.Resize(capacity);
    m_change_tracking.Resize(capacity, false);
  }
}

// Unsafe version : assumes changed_index is within capacity.
void cChangeList::MarkChange(int changed_index)
{
  if (!m_change_tracking[changed_index]) {
    m_change_tracking[changed_index] = true;
    m_change_list[m_change_count] = changed_index;
    m_change_count++;
  }
}

// Safe version : will resize to accommodate changed_index greater
// than capacity.
void cChangeList::PushChange(int changed_index)
{
  if (m_change_list.GetSize() <= changed_index) {
    Resize(changed_index + 1);
  }
  MarkChange(changed_index);
}

void cChangeList::Reset()
{
  for (int i = 0; i < m_change_count; i++) {
    m_change_tracking[m_change_list[i]] = false;
  }
  m_change_count = 0;
}
