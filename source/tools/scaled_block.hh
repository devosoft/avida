//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef SCALED_BLOCK_HH
#define SCALED_BLOCK_HH

#ifndef FIXED_COORDS_HH
#include "fixed_coords.hh"
#endif

/**
 * Class used by @ref cBlockStruct.
 **/

class cFixedCoords; // aggregate

class cScaledBlock {
private:
  void * data;
  int size;
  cFixedCoords start_coords;
public:
  cScaledBlock(void * in_data = NULL, int in_size = 0);
  ~cScaledBlock();

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

#endif
