/*
 *  cGUIWidget.h
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

// This is a base class for all GUI widgets used in Avida.

#ifndef cGUIWidget_h
#define cGUIWidget_h

#include "cString.h"

class cGUIWidget {
protected:
  cString m_name;
  int m_x;
  int m_y;
  int m_width;
  int m_height;

public:
  cGUIWidget() : m_x(0), m_y(0), m_width(0), m_height(0) { ; }
  cGUIWidget(int x, int y, width=0, height=0, name="")
    : m_name(name), m_x(x), m_y(y), m_width(width), m_height(height) { ; }
  virtual ~cGUIWidget() { ; }

  // This method should be run when the widget is setup and its time to build it.
  virtual void Create() = 0;
};

#endif
