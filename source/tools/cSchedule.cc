/*
 *  cSchedule.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cSchedule.h"

#include "cChangeList.h"


cSchedule::cSchedule(int _item_count)
: item_count(_item_count)
, m_change_list(0)
{
}

cSchedule::~cSchedule()
{
}

void cSchedule::SetChangeList(cChangeList *change_list) {
  m_change_list = change_list;
  if (m_change_list) m_change_list->Resize(item_count);
}
void cSchedule::SetSize(int _item_count) {
  item_count = _item_count;
  if (m_change_list) m_change_list->Resize(_item_count);
}
