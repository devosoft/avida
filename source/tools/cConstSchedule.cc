/*
 *  cConstSchedule.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cConstSchedule.h"

#include "cChangeList.h"
#include "cMerit.h"


bool cConstSchedule::OK()
{
  assert(last_id == 0 || last_id < item_count);  //  invalid last_id
  return true;
}

void cConstSchedule::Adjust(int item_id, const cMerit & merit)
{
  if (cChangeList *change_list = GetChangeList()) {
    change_list->MarkChange(item_id);
  }
  if (merit == 0.0) is_active[item_id] = false;
  else is_active[item_id] = true;
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
