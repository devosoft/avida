//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TASK_ENTRY_HH
#include "cTaskEntry.h"
#endif

////////////////
//  cTaskEntry
////////////////

cTaskEntry::cTaskEntry(const cString & _name, const cString & _desc,
		       int _id, tTaskTest _test_fun)
  : name(_name)
  , desc(_desc)
  , id(_id)
  , test_fun(_test_fun)
{
}

cTaskEntry::~cTaskEntry()
{
}
