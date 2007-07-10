/*
 *  cGUIWindow.h
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

// This is a base class for the main GUI windows...

#ifndef cGUIWindow_h
#define cGUIWindow_h

#include "cGUIContainer.h"

class cGUIEvent;

class cGUIWindow : public cGUIContainer {
protected:
public:
  cGUIWindow() { ; }
  cGUIWindow(int width, int height, const cString & name="") : cGUIContainer(0, 0, width, height, name) { ; }
  virtual ~cGUIWindow() { ; }

  virtual void SetSizeRange(int min_x, int min_y, int max_x=0, int max_y=0) = 0;

  virtual void Finalize() = 0;
  virtual void Update() = 0;
};

#endif
