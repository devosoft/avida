/*
 *  cFLTKBox.h
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

// This is a base class for the main GUI boxs...

#ifndef cFLTKBox_h
#define cFLTKBox_h

#include "cColor.h"
#include "cGUIBox.h"
#include "cFLTKWidget.h"

#include <FL/Fl_Box.H>

class cFLTKBox : public cGUIBox, public cFLTKWidget {
protected:
  Fl_Box * m_box;
  
public:
  cFLTKBox(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="")
    : cGUIBox(parent, x, y, width, height, name)
    , cFLTKWidget(this)
  {
    m_box = new Fl_Box(x, y, width, height, name);
    SetWidget(m_box);
  }
  ~cFLTKBox() { delete m_box; }

  void SetType(int in_type) {
    m_type = (eBoxType) in_type;
    if (m_type == BOX_NONE) m_box->box(FL_NO_BOX);
    else if (m_type == BOX_FLAT) m_box->box(FL_FLAT_BOX);
    else if (m_type == BOX_RAISED) m_box->box(FL_UP_BOX);
    else if (m_type == BOX_LOWERED) m_box->box(FL_DOWN_BOX);
    else if (m_type == BOX_FRAME) m_box->box(FL_BORDER_FRAME);
    else if (m_type == BOX_RAISED_FRAME) m_box->box(FL_UP_FRAME);
    else if (m_type == BOX_LOWERED_FRAME) m_box->box(FL_DOWN_FRAME);
  }

  Fl_Box * GetFLTKPtr() { return m_box; }
};

#endif
