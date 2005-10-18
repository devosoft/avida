//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef COUNT_TRACKER_HH
#include "cCountTracker.h"
#endif

// cCountTracker /////////////////////////////////////////////////////////////

void cCountTracker::Inc()
{
  cur_count++;
  total_count++;
}

void cCountTracker::Dec() 
{
  cur_count--;
}

void cCountTracker::Next()
{
  last_count = cur_count;
  cur_count = 0;
}

void cCountTracker::Clear()
{
  cur_count = 0;
  last_count = 0;
  total_count = 0;
}
