/*
 *  cGUISlider.h
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

#ifndef cGUISlider_h
#define cGUISlider_h

#include "cGUIWidget.h"

class cGUISlider : public cGUIWidget {
public:
  enum eSliderType { SLIDER_DEFAULT, SLIDER_VERT, SLIDER_HOIZ, SLIDER_DEFAULT_VALUE, SLIDER_VERT_VALUE, SLIDER_HOIZ_VALUE };

protected:
  eSliderType m_type;    // What type of slider is this?
  double m_min;
  double m_max;
  double m_default;

public:
  cGUISlider(int x, int y, width, height, name="") : cGUIWidget(x, y, width, height, name) { ; }
  virtual ~cGUISlider() { ; }

  virtual void DoSlide() = 0;

  int GetType() const { return m_type; }
  int GetMin() const { return m_min; }
  int GetMax() const { return m_max; }
  int GetDefault() const { return m_default; }
};

#endif
