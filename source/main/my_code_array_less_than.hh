//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MY_CODE_ARRAY_LESS_THAN_HH
#define MY_CODE_ARRAY_LESS_THAN_HH

#ifndef MX_CODE_ARRAY_HH
#include "mx_code_array.hh"
#endif

class MyCodeArrayLessThan{
public:
  bool operator()(const cMxCodeArray &x, const cMxCodeArray &y) const
  {
    return x < y;
  }
};

#endif
