//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TOOLS_HH
#include "cTools.h"
#endif

#include <iostream>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#ifndef STRING_HH
#include "cString.h"
#endif

#ifdef WIN32
# ifndef WIN32_MKDIR_HACK_HH
#  include "win32_mkdir_hack.hh"
# endif
#endif

cRandom g_random;

using namespace std;

bool cTools::MkDir(const cString & dirname, bool verbose)
{
  if (verbose) cout << "Checking for directory '" << dirname << "'..." << flush;

  FILE * fp = fopen ( dirname(), "r" );
  if (fp == 0) {
    if (errno == ENOENT) {
      if (verbose) cout << "  not found, creating..." << flush;
      if (mkdir( dirname(), (S_IRWXU|S_IRWXG|S_IRWXO) )) {
        if (verbose)
          cerr << endl << "Warning: Failed to create directory '" << dirname << "'." << endl;
        return false;
      }

      if (verbose) cout << " done." << endl;
      return true;
    }

    if (verbose) cout << "Warning: Failed to open '" << dirname << "'." << endl;
    return false;
  }

  if (verbose) cout << " found." << endl;
  return true;
}
