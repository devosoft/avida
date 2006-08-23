/*
 *  cStringIterator.cc
 *  Avida
 *
 *  Called "string_iterator.cc" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cStringIterator.h"

#include "cString.h"
#include "cStringList.h"


const cString cStringIterator::null_str("");

cStringIterator::cStringIterator(const cStringList & in_list)
  : list_it(in_list.GetList())
{
  Reset();
}
