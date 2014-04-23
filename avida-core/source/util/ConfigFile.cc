/*
 *  util/ConfigFile.cc
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

#include "avida/util/ConfigFile.h"

#include "avida/core/Feedback.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <locale>
#include <string>


char Avida::Util::ConfigFile::PREPROCESSOR_PREFIX_CHAR = '!';
char Avida::Util::ConfigFile::COMMENT_PREFIX_CHAR = '#';


bool Avida::Util::ConfigFile::Open(const Apto::String& filename, Feedback& feedback)
{
  // If a file is already open, close it first
  if (IsOpen()) Close();
  
  m_basefile = new fs_entry;
  m_basefile->fp.open(filename);
  
  // Test if there was an error, and if so, try again
  if (m_basefile->fp.fail()) {
    m_basefile->fp.clear();
    m_basefile->fp.open(filename);
  }
  
  if (m_basefile->fp.fail()) {
    delete m_basefile;
    m_basefile = NULL;
    return false;
  }
  
  m_currfile = m_basefile;
  m_filename = filename;
  
  return (m_basefile->fp.good() && !m_basefile->fp.fail());
}


bool Avida::Util::ConfigFile::Close()
{
  if (m_currfile != NULL) {
    fs_entry* fse;
    while (m_currfile) {
      m_currfile->fp.close();
      fse = m_currfile;
      m_currfile = fse->next;
      delete fse;
    }
    m_basefile = NULL;
    return true;
  }
  
  return false;
}


bool Avida::Util::ConfigFile::ReadLine(Apto::String& str_ref, Feedback* feedback)
{
  if (m_currfile == NULL) {
    if (feedback) feedback->Error("file not open");
    return false;
  }
  
  std::string linebuf;
  
  while (true) {
    std::getline(m_currfile->fp, linebuf);
    
    // check that this line was read correctly
    if (m_currfile->fp.bad()) {

      // file pointer went bad, try to back out the file stack
      if (m_currfile->next != NULL) {
        fs_entry* fse = m_currfile;
        m_currfile = fse->next;
        fse->fp.close();
        delete fse;
        continue;
      }
      
      // no more files on the stack, thus no more lines period
      return false;
    }
    
    // strip comments
    linebuf.erase(linebuf.find(COMMENT_PREFIX_CHAR));
    
    // find the first non-whitespace character in the string
    std::string::iterator non_ws = std::find_if(linebuf.begin(), linebuf.end(), std::not1(std::ptr_fun<int, int>(std::isspace)));
    
    // ignore blank lines
    if (non_ws == linebuf.end()) continue;
    
    // check the line for preprocessor directives
    if (*non_ws == PREPROCESSOR_PREFIX_CHAR) {
      Apto::String cmdstr(linebuf.c_str());
      cmdstr.Trim();
      
      Apto::String cmd = cmdstr.PopWord().ToLower().Substring(1);
      if (cmd == "include") {
        std::string path = cmdstr.GetCString();
        performVariableSubstitution(path);
        
        
        fs_entry* fse = new fs_entry;
        fse->fp.open(path.c_str());
        
        // Test if there was an error, and if so, try again
        if (fse->fp.fail()) {
          fse->fp.clear();
          fse->fp.open(path.c_str());
        }
        
        // check and handle include failure
        if (fse->fp.fail()) {
          delete fse;
          if (feedback) feedback->Error("unable to open included file '%s'", path.c_str());
          Close();
          return false;
        }
        
        // put the newly opened file onto the file stack
        fse->next = m_currfile;
        m_currfile = fse;
      } else if (cmd == "define") {
        Apto::String var = cmdstr.PopWord();
        m_variables.Set(var, cmdstr);
      } else if (cmd == "undef") {
        Apto::String var = cmdstr.PopWord();
        m_variables.Remove(var);
      }
      
      continue;
    }
    
    // not a preprocessor directive or effectively blank line, break and return
    break;
  }
  
  performVariableSubstitution(linebuf);
  str_ref = linebuf.c_str();
  return true;
}


void Avida::Util::ConfigFile::performVariableSubstitution(std::string& str)
{
  for (VariableMap::Iterator it = m_variables.Begin(); it.Next();) {
    std::string var = it.Get()->Value1().GetCString();
    std::string value = it.Get()->Value2()->GetCString();
    
    // Replace all instances of 'var' in 'str' with 'value'
    size_t pos = 0;
    while ((pos = str.find(var, pos)) != std::string::npos) {
      str.replace(pos, var.length(), value);
      pos += value.length();
    }
  }
}
