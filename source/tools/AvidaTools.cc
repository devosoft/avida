/*
 *  AvidaTools.cc
 *  Avida
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology
 *
 */

#include "AvidaTools.h"

#include "Platform.h"

#include "cEntryHandle.h"
#include "cRCObject.h"

#include <iostream>
#include <cerrno>
#include <sys/stat.h>
#include <cstdio>


// mkdir undefined in ms windows
#if AVIDA_PLATFORM(WINDOWS)
# include <direct.h>
# ifndef ACCESSPERMS
#  define ACCESSPERMS 0
# endif
# ifndef mkdir
#  define mkdir(path, ignored_mode) _mkdir(path)
# endif
# define getcwd(buff, buff_len) _getcwd(buff, buff_len)
# ifndef mode_t
#  define mode_t unsigned int
# endif
#endif

#if AVIDA_PLATFORM(WINDOWS)
# include <direct.h>
#else
# include <unistd.h>
#endif

#define MAXIMUM_DIRECTORY_LENGTH 2048


using namespace std;

cEntryHandle::~cEntryHandle() { ; }
cRCObject::~cRCObject() { ; }


bool AvidaTools::FileSystem::MkDir(const cString& dirname, bool verbose)
{
  if (verbose) cout << "Checking for directory '" << dirname << "'..." << flush;

  FILE* fp = fopen(dirname, "r");
  if (fp == 0) {
    if (errno == ENOENT) {
      if (verbose) cout << "  not found, creating..." << flush;
      if (mkdir(dirname, (S_IRWXU|S_IRWXG|S_IRWXO))) {
        if (verbose) cerr << endl << "Warning: Failed to create directory '" << dirname << "'." << endl;
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

cString AvidaTools::FileSystem::GetCWD()
{
  cString cwd_str;
  
  char* dirbuf = new char[MAXIMUM_DIRECTORY_LENGTH];    
  char* cwd = getcwd(dirbuf, MAXIMUM_DIRECTORY_LENGTH);
  if (cwd != NULL) cwd_str = cwd;
  delete [] dirbuf;
  
  return cwd_str;
}

cString AvidaTools::FileSystem::GetAbsolutePath(const cString& path, const cString& working_dir)
{
	if (path.GetSize() == 0 || (path[0] != '/' && path[0] != '\\' && !(path.IsLetter(0) && path[1] == ':' && path[2] =='\\') )) {
    return (cString(working_dir) + "/" + path);
  }
  
  return path;
}

