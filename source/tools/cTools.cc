/*
 *  cTools.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology
 *
 */

#include "cTools.h"

#include <iostream>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#include "cString.h"

#ifdef WIN32
# ifndef WIN32_MKDIR_HACK_HH
#  include "win32_mkdir_hack.hh"
# endif
#endif

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
