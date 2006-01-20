/*
 *  cFile.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cFile_h
#define cFile_h

#include <fstream>

#ifndef cString_h
#include "cString.h"
#endif

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
  //bool Open(cString _filename, int mode=(ios::in|ios::nocreate));
  // nocreate is no longer in the class ios -- k
  bool Open(cString _filename, std::ios::openmode mode=(std::ios::in));
  
  /**
   * Close the currently open file.
   **/
  bool Close();
  
  /**
   * Reads the next line in the file.
   **/
  bool ReadLine(cString & in_string);
  
  // Tests
  bool IsOpen() const { return is_open; }
  bool Fail() const { return (fp.fail()); }
  bool Good() const { return (fp.good()); }
  bool Eof() const { return (fp.eof()); }

  void SetVerbose(bool _v=true) { verbose = _v; }
};

#endif
