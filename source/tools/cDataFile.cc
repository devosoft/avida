/*
 *  cDataFile.cc
 *  Avida
 *
 *  Called "data_file.cc" prior to 12/2/05.
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

#include "cDataFile.h"

#include "cStringUtil.h"

#include <cstdio>
#include <ctime>

using namespace std;


cDataFile::cDataFile(cString& name) : m_name(name), m_descr_written(false), m_num_cols(0)
{
  m_fp.open(name);
  assert(m_fp.good());
  m_descr_written = false;
}


void cDataFile::Write(double x, const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_data << x << " ";
    WriteColumnDesc(descr, format);
  } else {
    m_fp << x << " ";
  }
}


void cDataFile::Write(int i, const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_data << i << " ";
    WriteColumnDesc(descr, format);
  } else {
    m_fp << i << " ";
  }
}


void cDataFile::Write(long i, const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_data << i << " ";
    WriteColumnDesc(descr, format);
  } else {
    m_fp << i << " ";
  }
}

void cDataFile::Write(unsigned int i, const char* descr, const char*)
{
  if (!m_descr_written) {
    m_data << i << " ";
    WriteColumnDesc(descr);
  } else {
    m_fp << i << " ";
  }
}


void cDataFile::Write(const char* data_str, const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_data << data_str << " ";
    WriteColumnDesc(descr, format);
  } else {
    m_fp << data_str << " ";
  }
}


void cDataFile::WriteBlockElement(double x, int element, int x_size)
{
  m_fp << x << " ";
  if (((element + 1) % x_size) == 0) m_fp << "\n";
}

void cDataFile::WriteBlockElement(int i, int element, int x_size)
{
  m_fp << i << " ";
  if (((element + 1) % x_size) == 0) m_fp << "\n";
}

void cDataFile::WriteColumnDesc(const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_num_cols++;
    m_descr += cStringUtil::Stringf("# %2d: %s\n", m_num_cols, descr);
    cString formatstr(format);
    if (formatstr != "") m_format += formatstr + " ";
  }
}

void cDataFile::WriteComment(const char* comment)
{
  if (!m_descr_written) m_descr += cStringUtil::Stringf("# %s\n", comment);
}


void cDataFile::WriteRawComment(const char* comment)
{
  if (!m_descr_written) m_descr += cStringUtil::Stringf("%s\n", comment);
}

void cDataFile::WriteRaw(const char* str)
{
  m_fp << cStringUtil::Stringf( "%s\n", str);
}




void cDataFile::WriteTimeStamp()
{
  if (!m_descr_written) {
    time_t time_p = time(0);
    m_descr += cStringUtil::Stringf("# %s", ctime(&time_p));
  }
}

void cDataFile::FlushComments()
{
  if (!m_descr_written) {
    m_fp << m_descr;
    m_descr = "";
    
    m_descr_written = true;
    assert(m_data.str().size() == 0);
  }
}


void cDataFile::Endl()
{
  if (!m_descr_written) {
    // Handle filetype and format first
    if (m_filetype != "") m_fp << "#filetype " << m_filetype << endl;
    if (m_format != "") m_fp << "#format " << m_format << endl;

    // Output column descriptions and comments
    m_fp << m_descr << endl;
    m_descr = "";
    
    // Print the first row of data
    m_fp << m_data.str() << endl;
    m_data.clear();
    m_data.str("");
    
    m_descr_written = true;
  } else {
    m_fp << endl;
  }
}
