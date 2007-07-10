/*
 *  tGUISlider.h
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

// This is a base class for all GUI widgets that act as sliders.

#ifndef tGUISlider_h
#define tGUISlider_h

#include "cGUIWidget.h"

template <class T> class tGUISlider : public cGUISlider {
protected:
  T * m_target;
  void (T::*m_slide_callback)(double);

public:
  tGUISlider(int x, int y, width, height, name="") : cGUISlider(x, y, width, height, name) { ; }
  virtual ~tGUISlider() { ; }

  T & GetTarget() { return *m_target; }

  void SetTarget(T & _target) { m_target = _target; }
  void SetSlideCallback(void (T::*cb_fun)(double)) { m_slide_callback = cb_fun; }

  virtual void DoSlide(double value=1.0) { if (m_slide_callback != NULL) (m_target->*(m_slide_callback))(value); }
};

#endif
