/*
 *  output/File.cc
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

#include "avida/output/File.h"

#include "avida/core/Feedback.h"
#include "avida/output/Manager.h"

#include <ctime>


Avida::Output::FilePtr Avida::Output::File::createWithPath(World* world, Apto::String path, bool append, Feedback* feedback)
{
  Output::ManagerPtr mgr = Output::Manager::Of(world);
  OutputID oid = mgr->OutputIDFromPath(path);
  
  if (oid.GetSize() == 0) {
    if (feedback) feedback->Error("unable to translate path '%s' to output id", (const char*)path);
    return FilePtr(NULL);
  }
  
  if (mgr->IsOpen(oid)) {
    if (feedback) feedback->Error("file '%s' already open", (const char*)oid);
    return FilePtr(NULL);
  }
  
  FilePtr rtn(new File(world, oid, append));
  
  if (!rtn->Good() || rtn->Fail()) {
    if (feedback) feedback->Error("unable to open file '%s' for writing", (const char*)oid);
    return FilePtr(NULL);
  }
  
  return rtn;
}

Avida::Output::FilePtr Avida::Output::File::StaticWithPath(World* world, Apto::String path, Feedback* feedback)
{
  Output::ManagerPtr mgr = Output::Manager::Of(world);
  OutputID oid = mgr->OutputIDFromPath(path);
  
  if (oid.GetSize() == 0) {
    if (feedback) feedback->Error("unable to translate path '%s' to output id", (const char*)path);
    return FilePtr(NULL);
  }
  
  FilePtr rtn;
  SocketPtr socket = Socket::RetrieveStatic(mgr, oid);
  rtn.DynamicCastFrom(socket);
  
  if (!rtn) {
    if (socket) {
      if (feedback) feedback->Error("'%s' already open, incompatible socket type", (const char*)oid);
      return FilePtr(NULL);
    }
    rtn = createWithPath(world, path, false, feedback);
    if (rtn) {
      if (!rtn->registerAsStatic()) {
        if (feedback) feedback->Error("unable to register '%s' as a static socket", (const char*)oid);
        return FilePtr(NULL);
      }
    }
  }
  
  return rtn;
}



Avida::Output::File::File(World* world, const OutputID& name, bool append)
  : Socket(world, name), m_descr_written(false), m_num_cols(0)
{
  m_fp.open(name, (append) ? (std::ios::out | std::ios::app) : std::ios::out);
  assert(m_fp.good());
}

Avida::Output::File::~File() { ; }



void Avida::Output::File::Write(double x, const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_data << x << " ";
    WriteColumnDesc(descr, format);
  } else {
    m_fp << x << " ";
  }
}


void Avida::Output::File::Write(int i, const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_data << i << " ";
    WriteColumnDesc(descr, format);
  } else {
    m_fp << i << " ";
  }
}


void Avida::Output::File::Write(long i, const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_data << i << " ";
    WriteColumnDesc(descr, format);
  } else {
    m_fp << i << " ";
  }
}

void Avida::Output::File::Write(unsigned int i, const char* descr, const char*)
{
  if (!m_descr_written) {
    m_data << i << " ";
    WriteColumnDesc(descr);
  } else {
    m_fp << i << " ";
  }
}


void Avida::Output::File::Write(const char* data_str, const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_data << data_str << " ";
    WriteColumnDesc(descr, format);
  } else {
    m_fp << data_str << " ";
  }
}

void Avida::Output::File::Write(Apto::Array<int> list, const char* descr, const char* format)
{
  //Anya is trying to make a commant to write vectors for Kaboom data
  if (!m_descr_written) {
    for (int i=0; i< (int)list.GetSize();i++) {
      m_data << list[i] << " ";
    }
    WriteColumnDesc(descr, format);
  } else {
    for (int i =0; i < (int)list.GetSize(); i++) {
      m_fp << list[i] << " ";
    }
  }
}


void Avida::Output::File::WriteBlockElement(double x, int element, int x_size)
{
  m_fp << x << " ";
  if (((element + 1) % x_size) == 0) m_fp << "\n";
}

void Avida::Output::File::WriteBlockElement(int i, int element, int x_size)
{
  m_fp << i << " ";
  if (((element + 1) % x_size) == 0) m_fp << "\n";
}

void Avida::Output::File::WriteColumnDesc(const char* descr, const char* format)
{
  if (!m_descr_written) {
    m_num_cols++;
    m_descr += Apto::FormatStr("# %2d: %s\n", m_num_cols, descr);
    Apto::String formatstr(format);
    if (formatstr != "") m_format += formatstr + " ";
  }
}

void Avida::Output::File::WriteComment(const char* comment)
{
  if (!m_descr_written) m_descr += Apto::FormatStr("# %s\n", comment);
}


void Avida::Output::File::WriteRawComment(const char* comment)
{
  if (!m_descr_written) m_descr += Apto::FormatStr("%s\n", comment);
}

void Avida::Output::File::WriteRaw(const char* str)
{
  m_fp << str << "\n";
}




void Avida::Output::File::WriteTimeStamp()
{
  if (!m_descr_written) {
    time_t time_p = time(0);
    m_descr += Apto::FormatStr("# %s", ctime(&time_p));
  }
}

void Avida::Output::File::FlushComments()
{
  if (!m_descr_written) {
    m_fp << m_descr;
    m_descr = "";
    
    m_descr_written = true;
    assert(m_data.str().size() == 0);
  }
}


void Avida::Output::File::Endl()
{
  if (!m_descr_written) {
    // Handle filetype and format first
    if (m_filetype != "") m_fp << "#filetype " << m_filetype << std::endl;
    if (m_format != "") m_fp << "#format " << m_format << std::endl;
    
    // Output column descriptions and comments
    m_fp << m_descr << std::endl;
    m_descr = "";
    
    // Print the first row of data
    m_fp << m_data.str() << std::endl;
    m_data.clear();
    m_data.str("");
    
    m_descr_written = true;
  } else {
    m_fp << std::endl;
  }
}


void Avida::Output::File::Flush()
{
  m_fp.flush();
}
