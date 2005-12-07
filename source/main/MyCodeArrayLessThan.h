/*
 *  MyCodeArrayLessThan.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef MyCodeArrayLessThan_h
#define MyCodeArrayLessThan_h

#ifndef cMxCodeArray_h
#include "cMxCodeArray.h"
#endif

class MyCodeArrayLessThan {
public:
  bool operator()(const cMxCodeArray &x, const cMxCodeArray &y) const
  {
    return x < y;
  }
};

#endif
