/*
 *  cColor.h
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

// This class manages colors in the graphical viewer.

#ifndef cColor_h
#define cColor_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cColor {
private:
  int m_red;
  int m_green;
  int m_blue;

public:
  cColor(int r=0, int g=0, int b=0) : m_red(r), m_green(g), m_blue(b) { ; }
  cColor(double r, double g, double b) 
    : m_red((int) (r*255.0)), m_green((int) (g*255.0)), m_blue((int) (b*255.0)) { ; }
  cColor(const cColor & _in) : m_red(_in.m_red), m_green(_in.m_green), m_blue(_in.m_blue) { ; }

  cColor& operator=(const cColor & _in) { m_red=_in.m_red; m_green=_in.m_green; m_blue=_in.m_blue; return *this; }

  void Set(int r, int g, int b) { m_red = r; m_green = g; m_blue = b; }
  void Set(double r, double g, double b) { Set( (int) (255 * r), (int) (255 * g), (int) (255 * b) ); }
  
  int Red() const { return m_red; }
  int Green() const { return m_green; }
  int Blue() const { return m_blue; }

  static const cColor BLACK;
  static const cColor WHITE;
  static const cColor RED;
  static const cColor GREEN;
  static const cColor BLUE;
  static const cColor YELLOW;
  static const cColor CYAN;
  static const cColor MAGENTA;

  static const cColor GRAY;
  static const cColor LT_RED;
  static const cColor LT_GREEN;
  static const cColor LT_BLUE;
  static const cColor LT_YELLOW;
  static const cColor LT_CYAN;
  static const cColor LT_MAGENTA;

  static const cColor DARK_GRAY;
  static const cColor DARK_RED;
  static const cColor DARK_GREEN;
  static const cColor DARK_BLUE;
  static const cColor DARK_YELLOW;
  static const cColor DARK_CYAN;
  static const cColor DARK_MAGENTA;

  static tArray<cColor> SCALE_FULL;
  static tArray<cColor> SCALE_BRIGHT;  
  static tArray<cColor> SCALE_DARK;
  static tArray<cColor> SCALE_PASTEL;
  static tArray<cColor> SCALE_HOT;
  static tArray<cColor> SCALE_COLD;

  static void Setup();
};

#endif
