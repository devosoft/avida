/*
 *  cInitFile.h
 *  Avida
 *
 *  Called "init_file.hh" prior to 12/7/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cInitFile_h
#define cInitFile_h

#include "apto/core.h"

#include "cString.h"
#include "cStringList.h"
#include "cUserFeedback.h"

#include <iostream>


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

  Apto::Array<sLine*> m_lines;
  cString m_ftype;
  cStringList m_format;
  cStringList m_imported_files;
  
  Apto::Map<Apto::String, Apto::String> m_mappings;
  Apto::Map<Apto::String, Apto::String> m_custom_directives;

  
  cInitFile(const cInitFile&); // @not_implemented
  cInitFile& operator=(const cInitFile&); // @not_implemented
  

public:
  cInitFile(const cString& filename, const cString& working_dir, Feedback& feedback, const Apto::Set<Apto::String>* custom_directives = NULL, const Apto::Map<Apto::String, Apto::String>* mappings = NULL);
  cInitFile(const cString& filename, const cString& working_dir, const Apto::Set<Apto::String>* custom_directives = NULL, const Apto::Map<Apto::String, Apto::String>* mappings = NULL);
  cInitFile(const cString& filename, const Apto::Map<Apto::String, Apto::String>& mappings, const cString& working_dir);
  cInitFile(std::istream& in_stream, const cString& working_dir);
  ~cInitFile();
  
  bool WasFound() const { return m_found; }
  bool WasOpened() const { return m_opened; }
  const cUserFeedback& GetFeedback() const { return m_feedback; }
  const Apto::Map<Apto::String, Apto::String>& GetCustomDirectives() const { return m_custom_directives; }
  
  void Save(const cString& in_filename = "");
  
  /**
   * Get a line from the file in memory. If called without parameters,
   * the first line of the file is returned.
   *
   * @param line_num The line count of the line to be returned 
   * (starting from 0).
   **/
  cString GetLine(int line_num = 0);
  
  Apto::SmartPtr<Apto::Map<Apto::String, Apto::String> > GetLineAsDict(int line_num = 0);
  

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
  cString ReadString(const Apto::Array<cString>& names, cString def = "", bool warn_default = true) const;
  
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
  void initMappings(const Apto::Map<Apto::String, Apto::String>& mappings);
  bool loadFile(const cString& filename, Apto::Array<sLine*, Apto::Smart>& lines, const cString& working_dir,
                const Apto::Set<Apto::String>* custom_directives, Feedback& feedback);
  bool processCommand(cString cmdstr, Apto::Array<sLine*, Apto::Smart>& lines, const cString& filename, int linenum,
                      const cString& working_dir, const Apto::Set<Apto::String>* custom_directives, Feedback& feedback);
  void postProcess(Apto::Array<sLine*, Apto::Smart>& lines);
};

#endif
