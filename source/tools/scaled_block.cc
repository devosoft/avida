//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2000 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef SCALED_BLOCK_HH
#include "scaled_block.hh"
#endif

////////////////////
//  cScaledBlock
////////////////////

cScaledBlock::cScaledBlock(void * in_data, int in_size)
{
  data = in_data;
  size = in_size;
}

cScaledBlock::~cScaledBlock()
{
}


