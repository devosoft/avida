/*
 *  cBioGroupManager.cc
 *  Avida
 *
 *  Created by David on 11/5/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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

#include "cBioGroupManager.h"

cBioGroupManager::~cBioGroupManager() { ; }


void cBioGroupManager::NotifyListeners(cBioGroup* bg, eBGEventType type, cBioUnit* bu)
{
  for (int i = 0; i < m_listeners.GetSize(); i++) {
    m_listeners[i]->NotifyBGEvent(bg, type, bu);
  }
}
