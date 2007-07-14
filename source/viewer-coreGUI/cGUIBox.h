/*
 *  cGUIBox.h
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

// This is a base class for all GUI widgets that act as boxs.

#ifndef cGUIBox_h
#define cGUIBox_h

#include "cGUIWidget.h"

class cGUIBox : public cGUIWidget {
public:
  enum eBoxType { BOX_NONE, BOX_FLAT, BOX_RAISED, BOX_LOWERED, BOX_FRAME, BOX_RAISED_FRAME, BOX_LOWERED_FRAME };

protected:
  eBoxType m_type;    // What type of box is this?

public:
  cGUIBox(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="")
    : cGUIWidget(parent, x, y, width, height, name)
    , m_type(BOX_NONE) { ; }
  virtual ~cGUIBox() { ; }

  int GetType() const { return m_type; }
  virtual void SetType(int in_type) { m_type = (eBoxType) in_type; }
};

#endif
