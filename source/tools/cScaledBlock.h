/*
 *  cScaledBlock.h
 *  Avida
 *
 *  Called "scaled_block.hh" prior to 12/7/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef cScaledBlock_h
#define cScaledBlock_h

#ifndef cFixedCoords_h
#include "cFixedCoords.h"
#endif

/**
 * Class used by @ref cBlockStruct.
 **/

class cScaledBlock {
private:
  void * data;
  int size;
  cFixedCoords start_coords;
public:
  cScaledBlock(void * in_data = NULL, int in_size = 0) : data(in_data), size(in_size) { ; }
  ~cScaledBlock() { ; }

  inline void * GetData() const { return data; }
  inline int GetSize() const { return size; }
  inline cFixedCoords GetStart() const { return start_coords; }

  inline void SetData(void * in_data) { data = in_data; }
  inline void SetSize(int in_size) { size = in_size; }
  inline void SetStart(const cFixedCoords &in_coords)
    { start_coords = in_coords; }
  inline void SetStart(int in_block, int in_offset) {
    start_coords(in_block, in_offset);
  }

  inline void operator=(const cScaledBlock & in_block) {
    data = in_block.GetData();
    size = in_block.GetSize();
  }
};


#ifdef ENABLE_UNIT_TESTS
namespace nScaledBlock {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
