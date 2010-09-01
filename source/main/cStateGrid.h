/*
 *  cStateGrid.h
 *  Avida
 *
 *  Created by David Bryson on 11/25/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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
  tArray<cString> m_states;
  tArray<int> m_sense_values;
  tArray<int> m_grid;
  
  cStateGrid(); // @not_implemented
  cStateGrid(const cStateGrid&); // @not_implemented
  cStateGrid& operator=(const cStateGrid&); // @not_implemented
  
public:
  inline cStateGrid(const cString& name, int w, int h, int x, int y, int f, const tArray<cString>& states,
                    const tArray<int>& sense_values, const tArray<int>& grid);
  ~cStateGrid() { ; }
  
  inline const cString& GetName() const { return m_name; }
  inline int GetWidth() const { return m_w; }
  inline int GetHeight() const { return m_h; }
  inline int GetInitialX() const { return m_init_x; }
  inline int GetInitialY() const { return m_init_y; }
  inline int GetInitialFacing() const { return m_init_facing; }
  inline int GetNumStates() const { return m_states.GetSize(); }
  inline int GetStateID(const cString& state_name) const;
  
  inline int GetIDFor(int x, int y) const { return (y * m_w + x); }
  inline int GetStateAt(int grid_id) const { return m_grid[grid_id]; }
  inline int GetStateAt(int x, int y) const { return m_grid[GetIDFor(x,y)]; }
  inline int SenseStateAt(int x, int y) const { return m_sense_values[GetStateAt(x, y)]; }
};


inline cStateGrid::cStateGrid(const cString& name, int w, int h, int x, int y, int f, const tArray<cString>& states,
                  const tArray<int>& sense_values, const tArray<int>& grid)
  : m_name(name), m_w(w), m_h(h), m_init_x(x), m_init_y(y), m_init_facing(f), m_states(states)
  , m_sense_values(sense_values), m_grid(grid)
{
  assert(states.GetSize() == sense_values.GetSize());
}


inline int cStateGrid::GetStateID(const cString& state_name) const
{
  for (int i = 0; i < m_states.GetSize(); i++) {
    if (m_states[i] == state_name) return i;
  }
  
  return -1;
}

#endif
