//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef FILE_HH
#include "file.hh"
#endif

#ifndef INIT_FILE_HH
#include "init_file.hh"
#endif
#ifndef GENESIS_HH
#include "genesis.hh"
#endif
#ifndef STRING_ITERATOR_HH
#include "string_iterator.hh"
#endif

#ifndef TOOLS_HH
#include "tools.hh"  // for g_debug global
#endif

#include <iostream>

extern "C" {
#include <stdlib.h>
#include <errno.h>   // needed for FOPEN error constants (MSVC)
}


using namespace std;


////////////
//  cFile
////////////



//bool cFile::Open(cString _fname, int flags)
// porting to gcc 3.1 -- k
bool cFile::Open(cString _fname, ios::openmode flags)
{
  if( IsOpen() ) Close();    // If a file is already open, clost it first.
  fp.open(_fname(), flags);  // Open the new file.

  // Test if there was an error, and if so, try again!
  int err_id = fp.fail();
  if( err_id ){
    fp.clear();
    fp.open(_fname(), flags);
  }

  // If there is still an error, determine its type and report it.
  err_id = fp.fail();
  if (err_id){
    cString error_desc = "?? Unknown Error??";

    // See if we can determine a more exact error type.
    if (err_id == EACCES) error_desc = "Access denied";
    else if (err_id == EINVAL) error_desc = "Invalid open flag or access mode";
    else if (err_id == ENOENT) error_desc = "File or path not found";

    // Print the error.
    cerr << "Unable to open file '" << _fname
	 << "' : " << error_desc << endl;
    return false;
  }

  filename = _fname;
  is_open = true;

  // Return true only if there were no problems...
  return( fp.good() && !fp.fail() );
}

bool cFile::Close()
{
  if (is_open == true) {
    fp.close();
    return true;
  }
  return false;
}

bool cFile::ReadLine(cString & in_string)
{
  char cur_line[MAX_STRING_LENGTH];
  cur_line[0]='\0';
  fp.getline(cur_line, MAX_STRING_LENGTH);
  if( fp.bad() ){
    return false;
  }
  in_string = cur_line;
  return true;
}

/*
int cFile::CountLines()
{
  char cur_line[MAX_STRING_LENGTH];

  Rewind();
  int n_lines = -1;

  do{
    fp.getline(cur_line, MAX_STRING_LENGTH);
    n_lines++;
  } while( cur_line[0]!='\0' && !fp.eof() );

  return n_lines;
}
*/
