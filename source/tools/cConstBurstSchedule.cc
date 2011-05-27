/*
 *  cConstBurstSchedule.cc
 *  Avida
 *
 *  Created by David on 3/23/08.
 *  Copyright 2008-2011 Michigan State University. All rights reserved.
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

#include "cConstBurstSchedule.h"

#include "cMerit.h"


bool cConstBurstSchedule::OK() { return true; }

void cConstBurstSchedule::Adjust(int item_id, const cMerit& merit, int deme_id)
{
  if (merit == 0.0) m_active[item_id] = false;
  else m_active[item_id] = true;
}


int cConstBurstSchedule::GetNextID()
{
  // If the current id is no longer active or the burst has finished, move on to the next
  if (!m_active[m_cur_id] || ++m_burst_state == m_burst_sz) {
    m_burst_state = 0;
    while (!m_active[m_cur_id]) if (++m_cur_id == item_count) m_cur_id = 0;
  }
  
  return m_cur_id;
}
