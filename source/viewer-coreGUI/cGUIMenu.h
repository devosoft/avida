/*
 *  cGUIMenu.h
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

// A base clasee for menus in the GUI.

#ifndef cGUIMenu_h
#define cGUIMenu_h

#include "cGUIWidget.h"

class cGUIMenuItem;

class cGUIMenu : public cGUIWidget {
protected:

public:
  cGUIMenu(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="")
    : cGUIWidget(parent, x, y, width, height, name) { ; }

  virtual ~cGUIMenu() { ; }
  
  virtual int GetSize() = 0;
  virtual const cString & GetName(int id) const = 0;
  virtual const cGUIMenuItem & GetOption(int id) const = 0;

  virtual void Trigger(int id) = 0;
};

#endif
