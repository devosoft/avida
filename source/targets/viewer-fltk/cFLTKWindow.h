/*
 *  cFLTKWindow.h
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

#ifndef cFLTKWindow_h
#define cFLTKWindow_h

#include "cGUIWindow.h"

#include <FL/Fl_Window.H>

#include "cFLTKBox.h"

class cFLTKWindow : public cGUIWindow, public cFLTKWidget {
protected:
  Fl_Window * m_window;
  
public:
  cFLTKWindow(int width, int height, const cString & name="")
    : cGUIWindow(width, height, name)
    , cFLTKWidget(this)
  {
    m_window = new Fl_Window(width, height);
    SetWidget(m_window);
  }
  ~cFLTKWindow() { delete m_window; }

  void SetSizeRange(int min_x, int min_y, int max_x=0, int max_y=0) {
    m_window->size_range(min_x, min_y, max_x, max_y);
  }

  void Finalize() { 
    m_window->end();
    m_window->show();
  }
  void Update() { ; }

  void Resizable(cFLTKBox & box) {
    m_window->resizable(box.GetFLTKPtr());
  }

  Fl_Window * GetFLTKPtr() { return m_window; }
};

#endif
