/*
 *  cConstSchedule.cc
 *  Avida
 *
 *  Called "const_schedule.cc" prior to 12/2/05.
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

#include "cConstSchedule.h"

#include "cChangeList.h"
#include "cMerit.h"


bool cConstSchedule::OK()
{
  assert(last_id == 0 || last_id < item_count);  //  invalid last_id
  return true;
}

void cConstSchedule::Adjust(int item_id, const cMerit& merit, int deme_id)
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
