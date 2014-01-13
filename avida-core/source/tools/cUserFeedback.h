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

#include "avida/core/Feedback.h"

#include "cString.h"

#include <cstdarg>

using namespace Avida;


class cUserFeedback : public Feedback
{
public:
  enum eFeedbackType {
    UF_ERROR,
    UF_WARNING,
    UF_NOTIFICATION
  };
  
  
private:
  struct sEntry {
    eFeedbackType ftype;
    cString message;
    
    sEntry() { ; }
    sEntry(eFeedbackType in_ftype, const cString& in_msg) : ftype(in_ftype), message(in_msg) { ; }
    sEntry(eFeedbackType in_ftype, const char* fmt, va_list args) : ftype(in_ftype) { message.Set(fmt, args); }
  };
  
  
  Apto::Array<sEntry, Apto::Smart> m_entries;
  int m_errors;
  int m_warnings;
  
  
public:
  cUserFeedback() : m_errors(0), m_warnings(0) { ; }
  ~cUserFeedback() { ; }
  
  // Feedback Methods
  void Error(const char* fmt, ...);
  void Warning(const char* fmt, ...);
  void Notify(const char* fmt, ...);
  
  
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
  m_entries.Push(sEntry(UF_ERROR, fmt, args));
  va_end(args);
  m_errors++;
}

inline void cUserFeedback::Warning(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  m_entries.Push(sEntry(UF_WARNING, fmt, args));
  va_end(args);
  m_warnings++;
}

inline void cUserFeedback::Notify(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  m_entries.Push(sEntry(UF_NOTIFICATION, fmt, args));
  va_end(args);
}

inline void cUserFeedback::Append(const cUserFeedback& uf)
{
  int old_size = m_entries.GetSize();
  m_entries.Resize(old_size + uf.m_entries.GetSize());
  
  for (int i = 0; i < uf.m_entries.GetSize(); i++) m_entries[old_size + i] = uf.m_entries[i];
}

#endif
