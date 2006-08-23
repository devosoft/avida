/*
 *  cInitFile.h
 *  Avida
 *
 *  Called "init_file.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cInitFile_h
#define cInitFile_h

#ifndef cFile_h
#include "cFile.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef cStringList_h
#include "cStringList.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


#include <iostream>

/**
 * A class to handle initialization files.
 **/

class cInitFile : public cFile
{
#if USE_tMemTrack
  tMemTrack<cInitFile> mt;
#endif
private:
  struct sFileLineInfo {
    cString line;
    int line_num;
    mutable bool used;

    template<class Archive>
    void save(Archive & a, const unsigned int version) const
    {
      int __used = (used == false)?(0):(1);
      a.ArkvObj("line", line);
      a.ArkvObj("line_num", line_num);
      a.ArkvObj("used", __used);
    }
    template<class Archive>
    void load(Archive & a, const unsigned int version)
    {
      int __used;
      a.ArkvObj("line", line);
      a.ArkvObj("line_num", line_num);
      a.ArkvObj("used", __used);
      used = (__used == false)?(0):(1);
    }
    template<class Archive>
    void serialize(Archive & a, const unsigned int version)
    { a.SplitLoadSave(*this, version); }

  };

  tArray<sFileLineInfo> line_array;
  cStringList extra_lines;
  cString filetype;
  cStringList file_format;

  int active_line;


  cInitFile(const cInitFile&); // @not_implemented
  cInitFile& operator=(const cInitFile&); // @not_implemented
  
public:
  /**
   * The empty constructor constructs an object that is in a clean
   * state. You can set the file to open with @ref cFile::Open() later on.
   **/
  cInitFile() : filetype("unknown"), active_line(0) { ; }
  
  /**
   * Opens the file with the given name.
   * 
   * @param in_filename Name of the initialization file to open.
   **/
  cInitFile(cString in_filename) : cFile(in_filename), filetype("unknown"), active_line(0) { ; }
  
  ~cInitFile() { ; }
  
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


  template<class Archive>
  void serialize(Archive & a, const unsigned int version)
  {
    a.ArkvBase("cFile", (cFile &)(*this), *this);
    a.ArkvObj("line_array", line_array);
    a.ArkvObj("extra_lines", extra_lines);
    a.ArkvObj("filetype", filetype);
    a.ArkvObj("file_format", file_format);
    a.ArkvObj("active_line", active_line);
  }


};

#ifdef ENABLE_UNIT_TESTS
namespace nInitFile {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
