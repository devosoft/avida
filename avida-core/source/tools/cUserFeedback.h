/*
 *  cUserFeedback.h
 *  Avida
 *
 *  Created by David on 11/10/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#ifndef cUserFeedback_h
#define cUserFeedback_h

#include "cBufferedFeedback.h"

#include "cString.h"

#include <cstdarg>
#include <sstream>

class cUserFeedback : public cBufferedFeedback
{
  
  
  Apto::Array<sEntry, Apto::Smart> m_entries;
  int m_errors;
  int m_warnings;
  
  
public:
  cUserFeedback() : m_errors(0), m_warnings(0) { ; }
  ~cUserFeedback() { ; }
  
  // Feedback Methods
  virtual void Error(const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    m_entries.Push(sEntry(UF_ERROR, fmt, args));
    va_end(args);
    m_errors++;
  }
  
  virtual void Warning(const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    m_entries.Push(sEntry(UF_WARNING, fmt, args));
    va_end(args);
    m_warnings++;
    
  }
  
  
  virtual void Notify(const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    m_entries.Push(sEntry(UF_NOTIFICATION, fmt, args));
    va_end(args);
  }
  
  
  virtual int GetNumMessages() const { return m_entries.GetSize(); }
  virtual int GetNumErrors() const { return m_errors; }
  virtual int GetNumWarnings() const { return m_warnings; }
  virtual int GetNumNotifications() const { return m_entries.GetSize() - m_errors - m_warnings; }
  
  virtual eFeedbackType GetMessageType(int k) const { return m_entries[k].ftype; }
  virtual const cString& GetMessage(int k) const { return m_entries[k].message; }
  
  void Clear();
  
  inline void Append(const cUserFeedback& uf);
  
  cString ToString();
};



inline void cUserFeedback::Append(const cUserFeedback& uf)
{
  int old_size = m_entries.GetSize();
  m_entries.Resize(old_size + uf.m_entries.GetSize());
  
  for (int i = 0; i < uf.m_entries.GetSize(); i++) m_entries[old_size + i] = uf.m_entries[i];
}

void cUserFeedback::Clear()
{
  m_entries.ResizeClear(0);
  m_warnings = 0;
  m_errors = 0;
}

cString cUserFeedback::ToString(){
  if (GetNumMessages() > 0){
    std::ostringstream oss;
    for (int k = 0; k < GetNumMessages(); k++){
      sEntry& entry = m_entries[k];
      switch (entry.ftype){
        case UF_ERROR:
          oss << "ERROR: ";
          break;
        case UF_WARNING:
          oss << "WARNING: ";
          break;
        case UF_NOTIFICATION:
          oss << "Note: ";
          break;
      }
      oss << entry.message << std::endl; 
    }
    return oss.str().c_str();
  }
  return "";
  
}

#endif
