/*
 *  output/File.h
 *  avida-core
 *
 *  Created by David on 1/16/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>, Charles Ofria <ofria@msu.edu>
 *
 */

#ifndef AvidaOutputFile_h
#define AvidaOutputFile_h

#include "avida/output/Socket.h"

#include <fstream>
#include <sstream>


namespace Avida {
  namespace Output {
    
    // Output::Socket - Protocol defining interface for output sockets that can be managed by the output manager
    // --------------------------------------------------------------------------------------------------------------
    
    class File : public Socket
    {
    private:
      Apto::String m_descr;
      Apto::String m_filetype;
      Apto::String m_format;
      bool m_descr_written;
      std::ostringstream m_data;
      
      int m_num_cols;
      
      std::ofstream m_fp;

      
    public:
      LIB_EXPORT inline static FilePtr CreateWithPath(World* world, Apto::String path, Feedback* feedback = NULL)
      {
        return createWithPath(world, path, false, feedback);
      }
      
      LIB_EXPORT inline static FilePtr AppendWithPath(World* world, Apto::String path, Feedback* feedback = NULL)
      {
        return createWithPath(world, path, true, feedback);
      }
      
      LIB_EXPORT static FilePtr StaticWithPath(World* world, Apto::String path, Feedback* feedback = NULL);
      
      LIB_EXPORT ~File();
      
      
      LIB_EXPORT inline const OutputID& Name() const { return m_output_id; }
      LIB_EXPORT inline const Apto::String& GetFileType() const { return m_filetype; }
      
      LIB_EXPORT inline bool Fail() const { return m_fp.fail(); }
      LIB_EXPORT inline bool Good() const { return m_fp.good(); }
      LIB_EXPORT inline bool HeaderDone() { return m_descr_written; }
      
      LIB_EXPORT inline bool SetFileType(const Apto::String& ft);

      
      LIB_EXPORT inline std::ofstream& OFStream() { return m_fp; }
      
      
      // The following methods output a value into the data file.
      //  first argument (x, i, data_str, etc.) - the value to write (as double, int, const char *, etc.)
      //  descr - descriptive string detailing the meaning of the value
      //  format (optional) - formatting identifier for the column
      LIB_EXPORT void Write(double x, const char* descr, const char* format = "");
      LIB_EXPORT void Write(int i, const char* descr, const char* format = "");
      LIB_EXPORT void Write(long i, const char* descr, const char* format = "");
      LIB_EXPORT void Write(unsigned int i, const char* descr, const char* format = "");
      LIB_EXPORT void Write(const char* data_str, const char* descr, const char* format = "");
      LIB_EXPORT void Write(Apto::Array<int> list, const char* descr, const char* format);
      
      
      // The following methods output a value into the data file anonymously (no column descriptor).
      //  first argument (x, i, data_str, etc.) - the value to write (as double, int, const char *, etc.)
      LIB_EXPORT inline void WriteAnonymous(double x) { m_fp << x << " "; }
      LIB_EXPORT inline void WriteAnonymous(int i) { m_fp << i << " "; }
      LIB_EXPORT inline void WriteAnonymous(long i) { m_fp << i << " "; }
      LIB_EXPORT inline void WriteAnonymous(const char* data_str) { m_fp << data_str << " "; }
      
      // The following methods are useful for outputting tables of values with row size x
      LIB_EXPORT void WriteBlockElement(double x, int element, int x_size);
      LIB_EXPORT void WriteBlockElement(int i, int element, int x_size);
      
      
      // Writes the description for a single column; keeps track of column numbers and formatting idendifier (optional).
      LIB_EXPORT void WriteColumnDesc(const char* descr, const char* format = "");
      
      LIB_EXPORT void WriteComment(const char* comment); // Writes a descriptive/comment string into a data file header
      LIB_EXPORT void WriteRawComment(const char* comment); // Writes a raw string to the data file header section

      LIB_EXPORT void WriteTimeStamp(); // Writes the current time into the data file comments.
      LIB_EXPORT void WriteRaw(const char* str); // Writes raw string to the file immediately
      
      LIB_EXPORT void FlushComments(); // Forces writing of accumulated comments
      
      LIB_EXPORT void Endl(); // Write all data to disk and start a new line.
      
      
      LIB_EXPORT void Flush();
      
      
    private:
      LIB_EXPORT static FilePtr createWithPath(World* world, Apto::String path, bool append, Feedback* feedback);

      LIB_LOCAL File(World* world, const OutputID& output_id, bool append = false);
    };
    

    inline bool File::SetFileType(const Apto::String& ft)
    {
      if (m_descr_written) return false;
      
      m_filetype = ft;
      return true;
    }

  };
};

#endif
