/*
 *  cRefBlock.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
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


#ifdef ENABLE_UNIT_TESTS
namespace nRefBlock {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif

#endif
