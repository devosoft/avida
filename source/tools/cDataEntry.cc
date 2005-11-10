//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef DATA_ENTRY_HH
#include "cDataEntry.h"
#endif


std::ostream& operator << (std::ostream& out, cDataEntry & entry)
{
  entry.Print(out);
  return out;
}
