/*
 *  MyCodeArrayLessThan.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef MyCodeArrayLessThan_h
#define MyCodeArrayLessThan_h

#ifndef cMxCodeArray_h
#include "cMxCodeArray.h"
#endif

class MyCodeArrayLessThan
{
public:
  bool operator()(const cMxCodeArray& x, const cMxCodeArray& y) const { return x < y; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nMyCodeArrayLessThan {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
