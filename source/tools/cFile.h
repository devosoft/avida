/*
 *  cFile.h
 *  Avida
 *
 *  Called "file.hh" prior to 12/7/05.
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

#ifndef cFile_h
#define cFile_h

#include "cString.h"

#include <fstream>

/**
 * This class encapsulates file handling. In comparison to @ref cDataFile
 * it has somewhat different features. It is more intended for reading files.
 * In particular, by default it does not create a file that doesn't exist. 
 * Its main usage is for the class @ref cInitFile.
 **/

class cFile
{
private:
  cFile(const cFile&); // @not_implemented
  cFile& operator=(const cFile&); // @not_implemented

protected:
  std::fstream fp;
  std::ios::openmode m_openmode;
  cString filename;
  bool is_open; // Have we successfully opened this file?
  bool verbose; // Should file be verbose about warnings to users?

public:
  cFile() : filename(""), is_open(false), verbose(false) { ; }
  
  /**
   * This constructor opens a file of the given name.
   *
   * @param _filename The name of the file to open.
   **/
  cFile(cString _filename) : filename(""), is_open(false) { Open(_filename); }
  
  /**
   * The desctructor automatically closes the file.
   **/
  ~cFile() { if (is_open == true) fp.close(); filename = ""; }
  
  /**
   * @return The name of the file currently open.
   **/
  const cString& GetFilename() const { return filename; }
  
  /**
   * Open a file of the given name. If another file was open previously,
   * close that one first.
   *
   * @return 0 if something went wrong, and 1 otherwise.
   * @param _filename The name of the file to open.
   * @param mode The opening mode.
   **/
  bool Open(cString _filename, std::ios::openmode mode=(std::ios::in));
  
  // Access to the underlying implmentation
  std::fstream* GetFileStream() { return &fp; }
  
  
  /**
   * Close the currently open file.
   **/
  bool Close();
  
  /**
   * Reads the next line in the file.
   **/
  bool ReadLine(cString& in_string);
  
  // Tests
  bool IsOpen() const { return is_open; }
  bool Fail() const { return (fp.fail()); }
  bool Good() const { return (fp.good()); }
  bool Eof() const { return (fp.eof()); }

  void SetVerbose(bool _v=true) { verbose = _v; }
};

#endif
