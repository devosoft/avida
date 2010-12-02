/*
 *  tFLTKButton.h
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

// This is a class to manage the FLTK GUI buttons...

#ifndef tFLTKButton_h
#define tFLTKButton_h

#include "tGUIButton.h"
#include "fltk-defs.h"

#include "FL/Fl_Button.H"
#include "FL/Fl_Light_Button.H"

template <class T> class tFLTKButton : public tGUIButton<T>, public cFLTKWidget {
protected:
  Fl_Button * m_button;

public:
  tFLTKButton(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="",
	      cGUIButton::eButtonType type=cGUIButton::BUTTON_NORMAL)
    : tGUIButton<T>(parent, x, y, width, height, name, type)
    , cFLTKWidget(this)
  {
    if (type == cGUIButton::BUTTON_NORMAL) {
      m_button = new Fl_Button(x, y, width, height, name);
    }
    else if (type == cGUIButton::BUTTON_LIGHT) {
      m_button = new Fl_Light_Button(x, y, width, height, name);
    }
    else {
      assert(false); // Unknown button type!
    }

    m_button->callback((Fl_Callback*) GenericButtonCallback, (void*)(this));
    SetWidget(m_button);
  }
  ~tFLTKButton() { delete m_button; }

  void BindKey(int key) { (void) key; }
};


#endif
