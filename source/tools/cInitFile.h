//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INIT_FILE_HH
#define INIT_FILE_HH

#include <iostream>

#ifndef FILE_HH
#include "cFile.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef STRING_LIST_HH
#include "cStringList.h"
#endif
#ifndef TARRAY_HH
#include "tArray.h"
#endif

/**
 * A class to handle initialization files.
 **/

class cString; // aggregate
class cStringList; // aggregate
template <class T> class tArray; // aggregate

class cInitFile : public cFile {
private:
  cInitFile(const cInitFile &);
private:
  struct sFileLineInfo {
    cString line;
    int line_num;
    mutable bool used;
  };

  tArray<sFileLineInfo> line_array;
  cStringList extra_lines;
  cString filetype;
  cStringList file_format;

  int active_line;
public:
  /**
   * The empty constructor constructs an object that is in a clean
   * state. You can set the file to open with @ref cFile::Open() later on.
   **/
  cInitFile();
  
  /**
   * Opens the file with the given name.
   * 
   * @param in_filename Name of the initialization file to open.
   **/
  cInitFile(cString in_filename);
  
  ~cInitFile();
  
  /**
   * Loads the file into memory.
   **/
  void Load();
  
  /**
   * Loads a stream into memory rather than a file.
   **/
  void LoadStream(std::istream & in_steam);
  
  void Save(const cString & in_filename = "");
  
  /**
   * Parse heading information about the contents of the file loaded into
   * memory, if available.
   **/
  void ReadHeader();

  /**
   * Remove all comments and whitespace from a file loaded into memory.
   * Comments are currently marked with the character '#'.
   **/
  void Compress();
  
  /** 
   * Add a line to the beginning of the file in memory.
   * This function is used by @ref cGenesis.
   *
   * @param in_string The string to be added.
   **/
  void AddLine(cString & in_string);
  
  /**
   * Get a line from the file in memory. If called without parameters,
   * the first line of the file is returned.
   *
   * @param line_num The line count of the line to be returned 
   * (starting from 0).
   **/
  cString GetLine(int line_num=0);
  

  /**
   * Get the active line and advance to the next line.
   **/
  cString GetNextLine() { return GetLine(active_line++); }


  /**
   * Reset the active line to the beginning (or the point specified...
   **/
  void ResetLine(int new_pos=0) { active_line = new_pos; }

  /**
   * Returns the line number that is active.
   **/
  int GetLineNum() { return active_line; }

  /**
   * Checks whether any line contains a given keyword in the specified 
   * column. Stops when the first occurrence of the keyword is found.
   *
   * @return TRUE if keyword is found, FALSE otherwise.
   * @param in_string A string variable that will contain the found line
   * if search succeeds, and will have undefined contents otherwise.
   * @param keyword The keyword to look for.
   * @param col The column in which the keyword should be found.
   **/
  bool Find(cString & in_string, const cString & keyword, int col) const;
  
  /**
   * Reads an entry in the initialization file that has a given keyword
   * in the first column. The keyword is not part of the returned string.
   *
   * @return The entry that has been found.
   * @param name The keyword to look for (the name of the entry).
   * @param def If the keyword is not found, def is returned. This allows
   * one to set standard values that are used if the user does not override
   * them.
   **/
  cString ReadString(const cString & name, cString def = "") const;
  
  /**
   * Looks over all lines loaded into the file, and warns if any of them
   * have not been the targets of the Find() method.  All methods that
   * search the file for a keyword use find, so this can be used to locate
   * keywords that are not understood by the program.
   **/
  bool WarnUnused() const;

  /**
   * Return the number of lines in memory.
   **/
  int GetNumLines() const { return line_array.GetSize(); }

  const cString & GetFiletype() { return filetype; }
  cStringList & GetFormat() { return file_format; }
};

#endif
