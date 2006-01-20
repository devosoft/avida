/*
 *  cChangeList.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2005 California Institute of Technology.
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
