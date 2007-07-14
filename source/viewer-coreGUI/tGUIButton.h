/*
 *  tGUIButton.h
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

// This is a base class for all GUI widgets that act as buttons.

#ifndef tGUIButton_h
#define tGUIButton_h

#include "cGUIButton.h"

template <class T> class tGUIButton : public cGUIButton {
protected:
  T * m_target;
  void (T::*m_callback)(double);
  double m_arg;

public:
  tGUIButton(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="")
    : cGUIButton(parent, x, y, width, height, name), m_target(NULL), m_callback(NULL), m_arg(0.0) { ; }
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
