//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef STRING_ITERATOR_HH
#include "string_iterator.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef STRING_LIST_HH
#include "string_list.hh"
#endif

const cString cStringIterator::null_str("");

cStringIterator::cStringIterator(const cStringList & in_list)
  : list_it(in_list.GetList())
{
  Reset();
}
