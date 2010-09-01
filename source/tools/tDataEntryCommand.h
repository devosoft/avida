/*
 *  tDataEntryCommand.h
 *  Avida
 *
 *  Called "tDataEntryCommand.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef tDataEntryCommand_h
#define tDataEntryCommand_h

#include <iostream>

#ifndef cFlexVar_h
#include "cFlexVar.h"
#endif

#ifndef cStringList_h
#include "cStringList.h"
#endif

#ifndef tDataEntry_h
#include "tDataEntry.h"
#endif

class cString;


template <class T> class tDataEntryCommand
{
private:
  tDataEntry<T>* m_data_entry;
  cFlexVar m_idx;
  cStringList m_args;
  
public:
  tDataEntryCommand(tDataEntry<T>* entry, const cString& idx = "", const cString& args = "")
    : m_data_entry(entry), m_idx(idx), m_args(args, ':') { ; }
  
  bool HasArg(const cString& test_arg) const { return m_args.HasString(test_arg); }
  
  const cString& GetName() const { return m_data_entry->GetName(); }
  cString GetDesc(const T* target) const { return m_data_entry->GetDesc(target, m_idx); }
  int GetCompareType() const { return m_data_entry->GetCompareType(); }
  const cString& GetNull() const { return m_data_entry->GetNull(); }
  const cString& GetHtmlCellFlags() const { return m_data_entry->GetHtmlCellFlags(); }
  
  bool SetValue(T* target, const cString& value) const { return m_data_entry->Set(target, m_idx, m_args, value); }
  cFlexVar GetValue(const T* target) const { return m_data_entry->Get(target, m_idx, m_args); }
};

#endif
