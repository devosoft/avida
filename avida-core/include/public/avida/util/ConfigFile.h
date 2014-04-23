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
#include "avida/core/Types.h"

#include <fstream>


namespace Avida {
  namespace Util {
    
    class ConfigFile
    {
    public:
      typedef Apto::Map<Apto::String, Apto::String> VariableMap;
      static char PREPROCESSOR_PREFIX_CHAR;
      static char COMMENT_PREFIX_CHAR;
      
    private:
      struct fs_entry {
        std::ifstream fp;
        fs_entry* next;
        
        LIB_LOCAL inline fs_entry() : next(NULL) { ; }
      };
      
    private:
      fs_entry* m_basefile;
      fs_entry* m_currfile;
      Apto::String m_filename;
      VariableMap m_variables;
      
    public:
      LIB_EXPORT inline ConfigFile()
        : m_basefile(NULL), m_currfile(NULL) { ; }
      LIB_EXPORT inline ConfigFile(const VariableMap& mappings)
        : m_basefile(NULL), m_currfile(NULL), m_variables(mappings) { ; }
      LIB_EXPORT inline ConfigFile(const Apto::String& filename, Feedback& feedback)
        : m_basefile(NULL), m_currfile(NULL)
      {
        Open(filename, feedback);
      }
      LIB_EXPORT inline ConfigFile(const Apto::String& filename, Feedback& feedback, const VariableMap& mappings)
        : m_basefile(NULL), m_currfile(NULL), m_variables(mappings)
      {
        Open(filename, feedback);
      }
      LIB_EXPORT inline ~ConfigFile() { Close(); }
      
      
      bool Open(const Apto::String& filename, Feedback& feedback);
      bool Close();
      
      VariableMap& Variables() { return m_variables; }
      
      const Apto::String& Filename() const { return m_filename; }
      std::ifstream* FileStream() { return (m_basefile) ? &m_basefile->fp : NULL; }

      bool ReadLine(Apto::String& str_ref, Feedback* feedback = NULL);
      
      // Tests
      LIB_EXPORT inline bool IsOpen() const { return (m_currfile); }
      LIB_EXPORT inline bool Fail() const { return (m_basefile == NULL || m_basefile->fp.fail()); }
      LIB_EXPORT inline bool Good() const { return (m_basefile && m_basefile->fp.good()); }
      LIB_EXPORT inline bool Eof() const { return (m_basefile == NULL || m_basefile->fp.eof()); }
      
    private:
      LIB_LOCAL void performVariableSubstitution(std::string& str);
    };
    
  };
};

#endif
