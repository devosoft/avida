/*
 *  cStateGrid.h
 *  Avida
 *
 *  Created by David Bryson on 11/25/08.
 *  Copyright 2008 Michigan State University. All rights reserved.
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

#ifndef cStateGrid_h
#define cStateGrid_h

#include "cString.h"
#include "tArray.h"


class cStateGrid
{
private:
  cString m_name;
  int m_w;
  int m_h;
  int m_init_x;
  int m_init_y;
  int m_init_facing;
  int m_num_states;
  tArray<int> m_grid;
  
  cStateGrid(); // @not_implemented
  cStateGrid(const cStateGrid&); // @not_implemented
  cStateGrid& operator=(const cStateGrid&); // @not_implemented
  
public:
  cStateGrid(const cString& name, int w, int h, int x, int y, int facing, int num_states, const tArray<int>& grid)
    : m_name(name), m_w(w), m_h(h), m_init_x(x), m_init_y(y), m_init_facing(facing), m_num_states(num_states), m_grid(grid) { ; }
  ~cStateGrid() { ; }
  
  int GetWidth() const { return m_w; }
  int GetHeight() const { return m_h; }
  int GetInitialX() const { return m_init_x; }
  int GetInitialY() const { return m_init_y; }
  int GetInitialFacing() const { return m_init_facing; }
  int GetNumStates() const { return m_num_states; }
  
  int GetStateAt(int x, int y) const { return m_grid[x * m_w + y]; }
};

#endif
