//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_FLAGS_HH
#include "memory_flags.hh"
#endif

//////////////////
//  cMemoryFlags
//////////////////

void cMemoryFlags::Clear()
{
  copied = false;
  mutated = false;
  executed = false;
  breakpoint = false;
  point_mut = false;
  copy_mut = false;
  injected = false;
}
