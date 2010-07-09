/*
 *  cConstBurstSchedule.cc
 *  Avida
 *
 *  Created by David on 3/23/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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

#include "cConstBurstSchedule.h"

#include "cChangeList.h"
#include "cMerit.h"


bool cConstBurstSchedule::OK() { return true; }

void cConstBurstSchedule::Adjust(int item_id, const cMerit& merit, int deme_id)
{
  if (cChangeList *change_list = GetChangeList()) change_list->MarkChange(item_id);

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
