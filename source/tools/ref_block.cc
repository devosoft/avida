//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REF_BLOCK_HH
#include "ref_block.hh"
#endif

////////////////////
//  cRefBlock
////////////////////

cRefBlock::cRefBlock(int in_ref, int in_size)
{
  ref_num = in_ref;
  size = in_size;
}

cRefBlock::~cRefBlock()
{
  // nothing needed here...
}
