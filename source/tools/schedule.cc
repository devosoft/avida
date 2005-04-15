//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "schedule.hh"

#include "change_list.hh"

////////////////
//  cSchedule
////////////////

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
