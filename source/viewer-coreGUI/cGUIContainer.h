/*
 *  cGUIContainer.h
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

// This is a base class for all GUI widgets that can be used as containers for other widgets in Avida.

#ifndef cGUIContainer_h
#define cGUIContainer_h

#include "cGUIWidget.h"

#include "tArray.h"

class cGUIContainer : public cGUIWidget {
protected:
  tArray<cGUIWidget *> widget_array;

  // The Create() method recursively builds everything contained here; create self is run first to set it up.
  virtual void CreateSelf() { ; }

public:
  cGUIContainer() { ; }
  cGUIContainer(int x, int y, int width=0, int height=0, const cString & name="")
    : cGUIWidget(x, y, width, height, name) { ; }
  cGUIContainer(cGUIContainer & parent, int x, int y, int width=0, int height=0, const cString & name="")
    : cGUIWidget(parent, x, y, width, height, name) { ; }
  virtual ~cGUIContainer() { ; }

  // This method should be run when the widget is setup and its time to build it and everything it contains.
  virtual void Create() {
    CreateSelf();
    for (int i = 0; i < widget_array.GetSize(); i++) {
      widget_array[i]->Create();
    }
  }

  void Add(cGUIWidget * in_widget) { widget_array.Push(in_widget); }
  void Add(cGUIWidget * in_widget, int x, int y, int width, int height, const cString & name="") {
    in_widget->m_x = x;
    in_widget->m_y = y;
    in_widget->m_width = width;
    in_widget->m_height = height;
    if (name != "") in_widget->m_name = name;
  }
};

#endif
