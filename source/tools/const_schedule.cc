//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "const_schedule.hh"

#include "change_list.hh"
#include "merit.hh"


////////////////////
//  cConstSchedule
////////////////////

cConstSchedule::cConstSchedule(int _item_count)
  : cSchedule(_item_count), is_active(_item_count)
{
  last_id = 0;
  is_active.SetAll(false);
}

cConstSchedule::~cConstSchedule()
{
}

bool cConstSchedule::OK()
{
  assert(last_id == 0 || last_id < item_count);  //  invalid last_id
  return true;
}

void cConstSchedule::Adjust(int item_id, const cMerit & merit)
{
  if (merit == 0.0) is_active[item_id] = false;
  else is_active[item_id] = true;
  if (cChangeList *change_list = GetChangeList()) {
    change_list->MarkChange(item_id);
  }
}


int cConstSchedule::GetNextID()
{
  // Grab the next ID...
  if (++last_id == item_count) last_id = 0;

  // Make sure we actually have an active ID...
  while (is_active[last_id] == false) {
    if (++last_id == item_count) last_id = 0;
  }
  return last_id;
}
