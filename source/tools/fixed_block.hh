//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef FIXED_BLOCK_HH
#define FIXED_BLOCK_HH

/**
 * Class used by @ref cBlockStruct.
 **/

class cFixedBlock {
private:
  int start_point;   // Starting point in the cScaledBlock array.
public:
  cFixedBlock();
  ~cFixedBlock();

  inline int GetStart() { return start_point; }
  inline void SetStart(int in_sp) { start_point = in_sp; }
};

#endif
