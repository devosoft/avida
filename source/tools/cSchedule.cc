/*
 *  cSchedule.cc
 *  Avida
 *
 *  Called "schedule.cc" prior to 12/7/05.
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
