/*
 *  cFLTKBaseDraw.h
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

// This is a base class for the main GUI canvases...

#ifndef cFLTKBaseDraw_h
#define cFLTKBaseDraw_h

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>

#ifndef cColor_h
#include "cColor.h"
#endif

#ifndef cGUIBaseDraw_h
#include "cGUIBaseDraw.h"
#endif

class cFLTKBaseDraw : public cGUIBaseDraw {
protected:
  class mDerivedWidget : public Fl_Widget {
  protected:
    cGUIBaseDraw & m_widget;
    
    void draw() { m_widget.Draw(); }
    int handle(int event) { return m_widget.Handle(event); }

  public:
    mDerivedWidget(cGUIBaseDraw & widget, int x, int y, int w, int h)
      : Fl_Widget(x, y, w, h), m_widget(widget) { ; }
  };

  mDerivedWidget m_widget;

  int x() { return m_widget.x(); }
  int y() { return m_widget.y(); }
  int w() { return m_widget.w(); }
  int h() { return m_widget.h(); }

  void DrawLine(int x1, int y1, int x2, int y2) { fl_line(x1+x(), y1+y(), x2+x(), y2+y()); }
  void DrawBox(int x1, int y1, int _w, int _h, bool fill=false) {
    if (fill == false) fl_rect(x1+x(), y1+y(), _w, _h);
    else fl_rectf(x1+x(), y1+y(), _w, _h);
  }
  void DrawCircle(int _x, int _y, int _r, bool fill=false) {
    if (fill == false) fl_circle(_x+x(), _y+y(), _r);
    //    else fl_circlef(_x+x(), _y+y(), _r);
  }

  void SetColor(const cColor & color) { fl_color(color.Red(), color.Green(), color.Blue()); }

public:
  cFLTKBaseDraw(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="") 
    : cGUIBaseDraw(parent, x, y, width, height, name), m_widget(*this, x, y, width, height)
  { ; }
  ~cFLTKBaseDraw() { ; }

  void Redraw() { m_widget.redraw(); }
};

#endif
