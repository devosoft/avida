/*
 *  cCoords.cc
 *  Avida
 *
 *  Created by Charles on 7/6/07
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cCoords_h
#define cCoords_h

class cCoords {
private:
  int m_x;
  int m_y;
public:
  cCoords(int _x, int _y) : m_x(_x), m_y(_y) { ; }
  cCoords(const cCoords & _coords) : m_x(_coords.m_x), m_y(_coords.m_y) { ; }
  ~cCoords() { ; }

  void Translate(int _x, int _y) { m_x += _x, m_y += _y; }

  void operator=(const cCoords & _coords) { m_x = _coords.m_x, m_y = _coords.m_y; }
  int operator==(const cCoords & _c) { return (m_x == _c.m_x && m_y == _c.m_y); }
  int operator!=(const cCoords & _c) { return !operator==(_c); }

  int GetX() { return m_x; }
  int GetY() { return m_y; }

  void Set(int _x, int _y) { m_x = _x; m_y = _y; }
};


#endif
