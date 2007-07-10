/*
 *  cGUIDriver.h
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

// This class manages the graphics viewer in a GUI package-agnostic manner.  It should be used as a base class
// for the graphics-based drivers.

#ifndef cGUIDriver_h
#define cGUIDriver_h

#include "cString.h"

#include "cCoreView_Info.h"

class cWorld;

class cGUIBox;
class cGUIContainer;
class cGUIWindow;

class cGUIDriver {
protected:
  cWorld* m_world;
  cCoreView_Info m_info;

  bool m_done;              // This is set to true when run should finish.

  // Graphics components...
  cGUIWindow * m_main_window;
  cGUIBox * m_update_box;

  virtual cGUIWindow * BuildWindow(int width, int height, const cString & name) = 0;
  virtual cGUIBox * BuildBox(cGUIContainer * container, int x, int y,
			     int width, int height, const cString & name="") = 0;
public:
  cGUIDriver(cWorld* world);
  virtual ~cGUIDriver();

  bool LaunchViewer();
  bool UpdateViewer();
};

#endif
