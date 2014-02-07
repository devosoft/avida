/*
 *  util/ConfigFile.h
 *  avida-core
 *
 *  Created by David on 2/7/14.
 *  Copyright 2014 Michigan State University. All rights reserved.
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaUtilConfigFile_h
#define AvidaUtilConfigFile_h

#include "apto/core.h"

#include <fstream>


namespace Avida {
  namespace Util {
    
    class ConfigFile
    {
    private:
      std::ifstream m_fp;
      Apto::String m_filename;
      bool m_is_open;
      
    public:
      LIB_EXPORT inline ConfigFile() : m_is_open(false) { ; }
      ConfigFile(const Apto::String& filename) : m_is_open(false) { Open(filename); }
      ~ConfigFile() { if (m_is_open) m_fp.close(); }
      
      
      bool Open(const Apto::String& filename);
      bool Close();
      
      
      const Apto::String& Filename() const { return m_filename; }
      std::ifstream* FileStream() { return &m_fp; }

      bool ReadLine(Apto::String& str_ref);
      
      // Tests
      LIB_EXPORT inline bool IsOpen() const { return m_is_open; }
      LIB_EXPORT inline bool Fail() const { return (m_fp.fail()); }
      LIB_EXPORT inline bool Good() const { return (m_fp.good()); }
      LIB_EXPORT inline bool Eof() const { return (m_fp.eof()); }
    };
    
  };
};

#endif
