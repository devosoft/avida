/*
 *  cGUICanvas.h
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

// This is a base class for GUI widgets that can be drawn on.

#ifndef cGUICanvas_h
#define cGUICanvas_h

#include "cGUIWidget.h"

class cGUICanvas : public cGUIWidget {
protected:
public:
  cGUICanvas() { ; }
  cGUICanvas(int x, int y, width=0, height=0, name="") : cGUIWidget(x, y, width, height, name) { ; }
  virtual ~cGUICanvas() { ; }

  virtual void DrawLine(int x1, int y1, int x2, int y2) = 0;
  virtual void DrawBox(int x1, int y1, int _w, int _h, bool fill=false) = 0;
  virtual void DrawCircle(int _x, int _y, int _, bool fill=false) = 0;

  virtual void SetColor(cColor color) = 0;
};

#endif
