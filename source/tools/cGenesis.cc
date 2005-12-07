/*
 *  cGenesis.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cGenesis.h"

using namespace std;


cGenesis::cGenesis(const cString & in_filename) : cInitFile(in_filename)
{
  if( !IsOpen() ){
    cerr<<"Failed to open '" << in_filename << "' file!!!\n" << endl;
  }

  Load();
  Compress();
  Close();
}

int cGenesis::Open(cString _filename, ios::openmode mode)
{
  if( IsOpen() ){
    cerr << "Trying to reopen '" << _filename << "' file!!!\n" << endl;
    return 0;
  }

  int base_ret = cFile::Open(_filename, mode);
  
  if (IsOpen() == false) return base_ret;  // Failed to open!

  Load();
  Compress();
  Close();

  return base_ret;
}

void cGenesis::AddInput(const cString & in_name, int in_value)
{
  cString in_string;
  in_string.Set("%s %d", in_name(), in_value);
  AddLine(in_string);
}

void cGenesis::AddInput(const cString & in_name, const cString & in_value)
{
  cString in_string;
  in_string.Set("%s %s", in_name(), in_value());
  AddLine(in_string);
}

int cGenesis::ReadInt(const cString & name, int base, bool warn) const
{
  if (name == "" || IsOpen() == false) return base;
  cString cur_line;

  if (!Find(cur_line, name, 0)) { 
    if (verbose == true && warn == true) {
      cout << "Warning: " << name << " not in \"" << GetFilename()
	   << "\", defaulting to: " << base <<endl;
    }
    return base;
  }

  return cur_line.GetWord(1).AsInt();
}

double cGenesis::ReadFloat(const cString & name, float base, bool warn) const
{
  if (name == "" || IsOpen() == false) return base;
  cString cur_line;

  if (!Find(cur_line, name, 0)) {
    if (verbose == true && warn == true) {
      cout << "Warning: " << name << " not in \"" << GetFilename()
	   << "\", defaulting to: " << base <<endl;
    }
    return base;
  }

  return cur_line.GetWord(1).AsDouble();
}
