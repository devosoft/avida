/*
 *  cTools.cc
 *  Avida
 *
 *  Called "tools.cc" prior to 12/7/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology
 *
 */

#include "cTools.h"

#include <iostream>
#include <cerrno>
#include <sys/stat.h>
#include <cstdio>

#include "cString.h"
#include "platform.h"


// mkdir undefined in ms windows
#if AVIDA_PLATFORM(WINDOWS)
# include <direct.h>
# ifndef ACCESSPERMS
#  define ACCESSPERMS 0
# endif
# ifndef mkdir
#  define mkdir(path, ignored_mode) _mkdir(path)
# endif
# ifndef mode_t
#  define mode_t unsigned int
# endif
#endif


using namespace std;


bool cTools::MkDir(const cString & dirname, bool verbose)
{
  if (verbose) cout << "Checking for directory '" << dirname << "'..." << flush;

  FILE* fp = fopen(dirname, "r");
  if (fp == 0) {
    if (errno == ENOENT) {
      if (verbose) cout << "  not found, creating..." << flush;
      if (mkdir(dirname, (S_IRWXU|S_IRWXG|S_IRWXO))) {
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
  fclose(fp);

  if (verbose) cout << " found." << endl;
  return true;
}
