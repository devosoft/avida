/*
 *  cBioGroupListener.h
 *  Avida
 *
 *  Created by David Bryson on 5/26/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#ifndef cBioGroupListener_h
#define cBioGroupListener_h

enum eBGEventType {
  BG_EVENT_ADD_THRESHOLD,
  BG_EVENT_REMOVE_THRESHOLD
};

class cBioGroup;
class cBioUnit;

class cBioGroupListener
{
public:
  virtual ~cBioGroupListener() = 0;
  
  virtual void NotifyBGEvent(cBioGroup* bg, eBGEventType type, cBioUnit* bu) = 0;
};

#endif
