/*
 *  cRefBlock.h
 *  Avida
 *
 *  Called "ref_block.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef cRefBlock_h
#define cRefBlock_h

#ifndef cFixedCoords_h
#include "cFixedCoords.h"
#endif

class cRefBlock {
private:
  int ref_num;
  int size;
  cFixedCoords start_coords;
public:
  cRefBlock(int in_ref = 0, int in_size = 0) : ref_num(in_ref), size(in_size) { ; }
  ~cRefBlock() { ; }

  inline int GetRef() const { return ref_num; }
  inline int GetSize() const { return size; }
  inline cFixedCoords GetStart() const { return start_coords; }

  inline void SetRef(int in_ref) { ref_num = in_ref; }
  inline void SetSize(int in_size) { size = in_size; }
  inline void SetStart(cFixedCoords &in_coords) { start_coords = in_coords; }
  inline void SetStart(int in_block, int in_offset) {
    start_coords(in_block, in_offset);
  }

  inline void operator=(const cRefBlock & in_block) {
    ref_num = in_block.GetRef();
    size = in_block.GetSize();
  }
};

#endif
