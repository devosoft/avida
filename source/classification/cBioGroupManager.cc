/*
 *  cBioGroupManager.cc
 *  Avida
 *
 *  Created by David on 11/5/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#include "cBioGroupManager.h"

cBioGroupManager::~cBioGroupManager() { ; }


void cBioGroupManager::NotifyListeners(cBioGroup* bg, eBGEventType type, cBioUnit* bu)
{
  for (int i = 0; i < m_listeners.GetSize(); i++) {
    m_listeners[i]->NotifyBGEvent(bg, type, bu);
  }
}
