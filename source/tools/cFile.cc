/*
 *  cFile.cc
 *  Avida
 *
 *  Called "file.cc" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cFile.h"

#include <cerrno>   // needed for FOPEN error constants (MSVC)
#include <cstdlib>
#include <iostream>

using namespace std;


bool cFile::Open(cString _fname, ios::openmode flags)
{
  if (IsOpen()) Close();    // If a file is already open, clost it first.
  fp.open(_fname, flags);  // Open the new file.

  // Test if there was an error, and if so, try again!
  if (fp.fail()) {
    fp.clear();
    fp.open(_fname, flags);
  }

  if (fp.fail()) return false;
  
  m_openmode = flags;
  filename = _fname;
  is_open = true;

  // Return true only if there were no problems...
  return (fp.good() && !fp.fail());
}

bool cFile::Close()
{
  if (is_open == true) {
    fp.close();
    is_open = false;
    return true;
  }
  return false;
}

bool cFile::ReadLine(cString& in_string)
{
  std::string linebuf;
  std::getline(fp, linebuf);
  if (fp.bad()) return false;
  in_string = linebuf.c_str();
  return true;
}
