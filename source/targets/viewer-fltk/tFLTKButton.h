/*
 *  tFLTKButton.h
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

// This is a class to manage the FLTK GUI buttons...

#ifndef tFLTKButton_h
#define tFLTKButton_h

#include "tGUIButton.h"
#include "fltk-defs.h"

#include "FL/Fl_Button.H"

template <class T> class tFLTKButton : public tGUIButton<T> {
protected:
  Fl_Button * m_button;

public:
  tFLTKButton(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="")
    : tGUIButton<T>(parent, x, y, width, height, name)
  {
    m_button = new Fl_Button(x, y, width, height, name);
    m_button->callback((Fl_Callback*) GenericButtonCallback, (void*)(this));
  }
  ~tFLTKButton() { delete m_button; }

  void BindKey(int key) { (void) key; }
};


#include <FL/Fl_Button.H>




#endif
