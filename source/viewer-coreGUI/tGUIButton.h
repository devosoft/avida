/*
 *  tGUIButton.h
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

// This is a second-level base class/template for all GUI widgets that act as buttons.

#ifndef tGUIButton_h
#define tGUIButton_h

#include "cGUIButton.h"

template <class T> class tGUIButton : public cGUIButton {
protected:
  T * m_target;
  void (T::*m_callback)(double);
  double m_arg;

public:
  tGUIButton(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="",
	     cGUIButton::eButtonType type=cGUIButton::BUTTON_NORMAL)
    : cGUIButton(parent, x, y, width, height, name, type)
    , m_target(NULL)
    , m_callback(NULL)
    , m_arg(0.0)
  { ; }
  virtual ~tGUIButton() { ; }

  T & GetTarget() { return *m_target; }
  double GetArg() { return m_arg; }

  void SetCallback(T * target, void (T::*cb_fun)(double), double arg=0.0) {
    m_target = target;
    m_callback = cb_fun;
    m_arg = arg;
  }

  virtual void Press() { if (m_callback != NULL) (m_target->*(m_callback))(m_arg); }
  virtual void Press(double arg) { if (m_callback != NULL) (m_target->*(m_callback))(arg); }
};

#endif
