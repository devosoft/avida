/*
 *  cGUIBaseDraw.h
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

// This is a base class for GUI widgets that can be drawn on.

#ifndef cGUIBaseDraw_h
#define cGUIBaseDraw_h

#include "cGUIWidget.h"

class cColor;
class cGUIContainer;
class cString;

class cGUIBaseDraw : public cGUIWidget {
protected:
public:
  cGUIBaseDraw() { ; }
  cGUIBaseDraw(cGUIContainer & parent, int x, int y, int width=0, int height=0, const cString & name="")
    : cGUIWidget(parent, x, y, width, height, name) { ; }
  virtual ~cGUIBaseDraw() { ; }

  virtual void Draw() = 0;
  virtual int Handle(int event) = 0;
  virtual void Redraw() = 0;

  virtual void DrawLine(int x1, int y1, int x2, int y2) = 0;
  virtual void DrawBox(int x1, int y1, int _w, int _h, bool fill=false) = 0;
  virtual void DrawCircle(int _x, int _y, int _r, bool fill=false) = 0;

  virtual void SetColor(const cColor & color) = 0;
};

#endif
