/*
 *  cGUIColor.h
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

// This class deals with colors in the GUI.

#ifndef cGUIColor_h
#define cGUIColor_h

class cGUIColor {
protected:
  int m_red;    // Limited to 0-255
  int m_green;  // Limited to 0-255
  int m_blue;   // Limited to 0-255

public:
  cGUIColor(int _r, int _g, int _b) : m_red(_r), m_green(_g), m_blue(_b) {
    assert(m_red >= 0 && m_red < 256);
    assert(m_green >= 0 && m_green < 256);
    assert(m_blue >= 0 && m_blue < 256);
  }
  virtual ~cGUIColor() { ; }

  int GetRed() const { return m_red; }
  int GetGreen() const { return m_green; }
  int GetBlue() const { return m_blue; }
};

#endif
