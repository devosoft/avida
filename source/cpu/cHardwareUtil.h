//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_UTIL_HH
#define HARDWARE_UTIL_HH

#ifndef STRING_HH
#include "string.hh"
#endif

class cString; // aggregate
class cInstSet;

class cHardwareUtil {
public:
  static void LoadInstSet( cString filename, cInstSet& inst_set);
  static cInstSet& DefaultInstSet(const cString & inst_filename);
};

#endif
