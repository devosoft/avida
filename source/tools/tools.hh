//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

// This file is for misc. objects which are of general use...

#ifndef TOOLS_HH
#define TOOLS_HH

#ifndef RANDOM_HH
#include "random.hh"
#endif

extern cRandom g_random;

/*
 *   Filesystem tools...
 */

class cString;

class cTools {
public:
  static bool MkDir(const cString & dirname, bool verbose=false);
};

#endif
