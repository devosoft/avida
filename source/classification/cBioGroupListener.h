/*
 *  cBioGroupListener.h
 *  Avida
 *
 *  Created by David Bryson on 5/26/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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
