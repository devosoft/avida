/*
 *  cCoords.cc
 *  Avida
 *
 *  Created by Charles on 7/6/07
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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
  
  cCoords operator+(const cCoords & _c) const { return cCoords( m_x + _c.m_x, m_y + _c.m_y); }
  cCoords operator-(const cCoords & _c) const { return cCoords( m_x - _c.m_x, m_y - _c.m_y); }
  cCoords operator*(const int n) const { return cCoords(m_x * n, m_y * n); };
  
  cCoords& operator+=(const cCoords & _c) { m_x += _c.m_x; m_y += _c.m_y; return *this; }
  
  int GetX() const { return m_x; }
  int GetY() const { return m_y; }
  
  void Set(int _x, int _y) { m_x = _x; m_y = _y; }
};


#endif
