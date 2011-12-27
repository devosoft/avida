/*
 *  cConstSchedule.cc
 *  Avida
 *
 *  Called "const_schedule.cc" prior to 12/2/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cConstSchedule.h"

#include "cMerit.h"


bool cConstSchedule::OK()
{
  assert(last_id == 0 || last_id < item_count);  //  invalid last_id
  return true;
}

void cConstSchedule::Adjust(int item_id, const cMerit& merit, int)
{
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
