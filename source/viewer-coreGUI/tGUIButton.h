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

#include "cGUIWidget.h"

template <class T> class tGUIButton : public cGUIButton {
protected:
  T * m_target;
  void (T::*m_press_callback)(double);
  void (T::*m_release_callback)(double);

public:
  tGUIButton(int x, int y, width, height, name="") : cGUIButton(x, y, width, height, name) { ; }
  virtual ~tGUIButton() { ; }

  T & GetTarget() { return *m_target; }

  void SetTarget(T & _target) { m_target = _target; }
  void SetPressCallback(void (T::*cb_fun)(double)) { m_press_callback = cb_fun; }
  void SetReleaseCallback(void (T::*cb_fun)(double)) { m_release_callback = cb_fun; }

  virtual void DoPress(double value=1.0) { if (m_press_callback != NULL) (m_target->*(m_press_callback))(value); }
  virtual void DoRelease(double value=0.0) { if (m_release_callback != NULL) (m_target->*(m_release_callback))(value); }
};

#endif
