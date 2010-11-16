/*
 *  cInitFile.h
 *  Avida
 *
 *  Called "init_file.hh" prior to 12/7/05.
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

#ifndef cInitFile_h
#define cInitFile_h

#include "cString.h"
#include "cStringList.h"
#include "cUserFeedback.h"
#include "tDictionary.h"
#include "tSmartArray.h"

#include <iostream>

template<typename T> class tArraySet;


// A class to handle initialization files.
class cInitFile
{
private:
  cString m_filename;
  bool m_found;
  bool m_opened;
  mutable cUserFeedback m_feedback;
  
  struct sLine {
    cString line;
    cString file;
    int line_num;
    mutable bool used;
    
    sLine(const cString& in_line, const cString& in_file, int in_line_num)
      : line(in_line), file(in_file), line_num(in_line_num), used(false) { ; }
  };

  tArray<sLine*> m_lines;
  cString m_ftype;
  cStringList m_format;
  cStringList m_imported_files;
  
  tDictionary<cString> m_mappings;
  tDictionary<cString> m_custom_directives;

  
  cInitFile(const cInitFile&); // @not_implemented
  cInitFile& operator=(const cInitFile&); // @not_implemented
  

public:
  cInitFile(const cString& filename, const cString& working_dir, const tArraySet<cString>* custom_directives = NULL);
  cInitFile(const cString& filename, const tDictionary<cString>& mappings, const cString& working_dir);
  cInitFile(std::istream& in_stream, const cString& working_dir);
  ~cInitFile();
  
  bool WasFound() const { return m_found; }
  bool WasOpened() const { return m_opened; }
  const cUserFeedback& GetFeedback() const { return m_feedback; }
  const tDictionary<cString>& GetCustomDirectives() const { return m_custom_directives; }
  
  void Save(const cString& in_filename = "");
  
  /**
   * Get a line from the file in memory. If called without parameters,
   * the first line of the file is returned.
   *
   * @param line_num The line count of the line to be returned 
   * (starting from 0).
   **/
  cString GetLine(int line_num = 0);
  
  tDictionary<cString>* GetLineAsDict(int line_num = 0);
  

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
  bool Find(cString& in_string, const cString& keyword, int col) const;
  
  /**
   * Reads an entry in the initialization file that has a given keyword in the first column.
   * The keyword is not part of the returned string.
   *
   * @return The entry that has been found.
   * @param name The keyword to look for (the name of the entry).
   * @param def If the keyword is not found, def is returned. This allows
   * one to set standard values that are used if the user does not override
   * them.
   **/
  cString ReadString(const cString& name, cString def = "", bool warn_default = true) const;

  /**
   * Reads an entry in the initialization file that has a given keyword OR ANY OF ITS ALIASES
   * in the first column. The keyword is not part of the returned string.
   *
   * @return The entry that has been found.
   * @param names An array of keywords to look for (the name of the entry).
   * @param def If the keyword is not found, def is returned. This allows
   * one to set standard values that are used if the user does not override
   * them.
   **/
  cString ReadString(const tArray<cString>& names, cString def = "", bool warn_default = true) const;
  
  /**
   * Looks over all lines loaded into the file, and warns if any of them
   * have not been the targets of the Find() method.  All methods that
   * search the file for a keyword use find, so this can be used to locate
   * keywords that are not understood by the program.
   **/
  bool WarnUnused() const;

  void MarkLineUsed(int line_id) { m_lines[line_id]->used = true; }

  int GetNumLines() const { return m_lines.GetSize(); }

  const cString& GetFiletype() { return m_ftype; }
  const cStringList& GetFormat() { return m_format; }


private:
  void initMappings(const tDictionary<cString>& mappings);
  bool loadFile(const cString& filename, tSmartArray<sLine*>& lines, const cString& working_dir,
                const tArraySet<cString>* custom_directives = NULL);
  bool processCommand(cString cmdstr, tSmartArray<sLine*>& lines, const cString& filename, int linenum,
                      const cString& working_dir, const tArraySet<cString>* custom_directives = NULL);
  void postProcess(tSmartArray<sLine*>& lines);
};

#endif
