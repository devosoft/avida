//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REF_BLOCK_HH
#define REF_BLOCK_HH

#ifndef FIXED_COORDS_HH
#include "cFixedCoords.h"
#endif

class cFixedCoords; // aggregate

class cRefBlock {
private:
  int ref_num;
  int size;
  cFixedCoords start_coords;
public:
  cRefBlock(int in_ref = 0, int in_size = 0);
  ~cRefBlock();

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
