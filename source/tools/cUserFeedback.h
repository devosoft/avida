/*
 *  cUserFeedback.h
 *  Avida
 *
 *  Created by David on 11/10/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#ifndef cUserFeedback_h
#define cUserFeedback_h

#include "cString.h"
#include "tSmartArray.h";

#include <cstdarg>

class cUserFeedback
{
public:
  enum eFeedbackType {
    ERROR,
    WARNING,
    NOTIFICATION
  };
  
  
private:
  struct sEntry {
    eFeedbackType ftype;
    cString message;
    
    sEntry() { ; }
    sEntry(eFeedbackType in_ftype, const cString& in_msg) : ftype(in_ftype), message(in_msg) { ; }
    sEntry(eFeedbackType in_ftype, const char* fmt, va_list args) : ftype(in_ftype) { message.Set(fmt, args); }
  };
  
  
  tSmartArray<sEntry> m_entries;
  int m_errors;
  int m_warnings;
  
  
public:
  cUserFeedback() : m_errors(0), m_warnings(0) { ; }
  ~cUserFeedback() { ; }
  
  inline void Error(const cString& msg) { m_entries.Push(sEntry(ERROR, msg)); m_errors++; }
  inline void Error(const char* fmt, ...);
  inline void Warning(const cString& msg) { m_entries.Push(sEntry(WARNING, msg)); m_warnings++; }
  inline void Warning(const char* fmt, ...);
  inline void Notify(const cString& msg) { m_entries.Push(sEntry(NOTIFICATION, msg)); }
  inline void Notify(const char* fmt, ...);
  
  int GetNumMessages() const { return m_entries.GetSize(); }
  int GetNumErrors() const { return m_errors; }
  int GetNumWarnings() const { return m_warnings; }
  int GetNumNotifications() const { return m_entries.GetSize() - m_errors - m_warnings; }
  
  eFeedbackType GetMessageType(int i) const { return m_entries[i].ftype; }
  const cString& GetMessage(int i) const { return m_entries[i].message; }
  
  inline void Append(const cUserFeedback& uf);
};

inline void cUserFeedback::Error(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  m_entries.Push(sEntry(ERROR, fmt, args));
  va_end(args);
  m_errors++;
}

inline void cUserFeedback::Warning(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  m_entries.Push(sEntry(WARNING, fmt, args));
  va_end(args);
  m_warnings++;
}

inline void cUserFeedback::Notify(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  m_entries.Push(sEntry(NOTIFICATION, fmt, args));
  va_end(args);
}

inline void cUserFeedback::Append(const cUserFeedback& uf)
{
  int old_size = m_entries.GetSize();
  m_entries.Resize(old_size + uf.m_entries.GetSize());
  
  for (int i = 0; i < uf.m_entries.GetSize(); i++) m_entries[old_size + i] = uf.m_entries[i];
}

#endif
