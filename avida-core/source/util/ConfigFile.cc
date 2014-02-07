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

#include <string>


bool Avida::Util::ConfigFile::Open(const Apto::String& filename)
{
  // If a file is already open, close it first
  if (IsOpen()) Close();
  
  m_fp.open(filename);
  
  // Test if there was an error, and if so, try again
  if (m_fp.fail()) {
    m_fp.clear();
    m_fp.open(filename);
  }
  
  if (m_fp.fail()) return false;
  
  m_filename = filename;
  m_is_open = true;
  
  return (m_fp.good() && !m_fp.fail());
}


bool Avida::Util::ConfigFile::Close()
{
  if (m_is_open == true) {
    m_fp.close();
    m_is_open = false;
    return true;
  }
  
  return false;
}


bool Avida::Util::ConfigFile::ReadLine(Apto::String& str_ref)
{
  std::string linebuf;
  std::getline(m_fp, linebuf);
  
  if (m_fp.bad()) return false;
  
  str_ref = linebuf.c_str();
  
  return true;
}
