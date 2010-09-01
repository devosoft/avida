/*
 *  cDataFile.h
 *  Avida
 *
 *  Called "data_file.hh" prior to 12/2/05.
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

#ifndef cDataFile_h
#define cDataFile_h

#include <fstream>
#include <sstream>

#ifndef cString_h
#include "cString.h"
#endif

// This class encapsulates output file handling for the class cDataFileManager.

class cDataFile
{
private:
  cString m_name;
  
  cString m_descr;
  cString m_filetype;
  cString m_format;
  bool m_descr_written;
  std::ostringstream m_data;

  int m_num_cols;
  
  std::ofstream m_fp;

  
  cDataFile();
  cDataFile(const cDataFile&); // @not_implemented.
  cDataFile& operator=(const cDataFile&); // @not_implemented

  
public:
  cDataFile(cString& name);
  ~cDataFile() { m_fp.close(); }

  inline const cString& GetName() const { return m_name; }
  inline const cString& GetFileType() const { return m_filetype; }

  inline bool Good() const { return m_fp.good(); }
  inline bool HeaderDone() { return m_descr_written; }
  
  inline bool SetFileType(const cString& ft);
  

  std::ofstream& GetOFStream() { return m_fp; }

  
  // The following methods output a value into the data file.
  //  first argument (x, i, data_str, etc.) - the value to write (as double, int, const char *, etc.)
  //  descr - descriptive string detailing the meaning of the value
  //  format (optional) - formatting identifier for the column
  void Write(double x, const char* descr, const char* format = "");
  void Write(int i, const char* descr, const char* format = "");
  void Write(long i, const char* descr, const char* format = "");
	void Write(unsigned int i, const char* descr, const char* format = "");
  void Write(const char* data_str, const char* descr, const char* format = "");
  
  
  // The following methods output a value into the data file anonymously (no column descriptor).
  //  first argument (x, i, data_str, etc.) - the value to write (as double, int, const char *, etc.)
  inline void WriteAnonymous(double x) { m_fp << x << " "; }
  inline void WriteAnonymous(int i) { m_fp << i << " "; }
  inline void WriteAnonymous(long i) { m_fp << i << " "; }
  inline void WriteAnonymous(const char* data_str) { m_fp << data_str << " "; }
  
  
  // The following methods are useful for outputting tables of values with row size x
  void WriteBlockElement(double x, int element, int x_size);
  void WriteBlockElement(int i, int element, int x_size);

  
  // Writes the description for a single column; keeps track of column numbers and formatting idendifier (optional).
  void WriteColumnDesc(const char* descr, const char* format = ""); 

  // Writes a descriptive/comment string into a data file.
  // The string is only written if the first data line hasn't been completed (i.e. Endl() hasn't been called).
  void WriteComment(const char* comment);

  // Same as WriteComment, but doesn't automatically include the # in the front of the line.
  // This should only be used in special circumstances where something outside of a typical comment needs to be at the top.
  void WriteRawComment(const char* comment);

  // Writes the current time into the data file comments.
  void WriteTimeStamp();

  // Writes text string any where in the data file. This should only be used in special circumstances where something
  // outside of a typical comment needs to be placed in the file.
  void WriteRaw(const char* str);
  

  // This function writes the comments that have accumulated. There should normally be no reason to call this function,
  // Endl() performs the same operation automatically after all columns are known.
  void FlushComments();

  
  // Write all data to disk and start a new line.
  void Endl();

  
  void Flush() { m_fp.flush(); }
};

inline bool cDataFile::SetFileType(const cString& ft)
{
  if (m_descr_written) return false;
  
  m_filetype = ft;
  return true;
}


#endif
