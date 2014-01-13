/*
 *  tDataManager.h
 *  Avida
 *
 *  Called "tDataManager.hh" prior to 12/7/05.
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

#ifndef tDataManager_h
#define tDataManager_h

#include <iostream>

#include "cDataManager_Base.h"
#include "tDataEntry.h"

// This template manages a collection of keywords and associates each with
// an accessor for a specific object.  When a list of keywords is then
// fed in with an output file, the associated data for that object will
// then be printed to the output file.

class cString;
template <class TargetType, class EntryType> class tDataEntryOfType;

template <class TargetType> class tDataManager : public cDataManager_Base
{
private:
  TargetType* m_target;
  Apto::Map<Apto::String, tDataEntry<TargetType>*> m_entry_dict;
  
public:
  tDataManager(TargetType* target, const cString& filetype = "unknown") : cDataManager_Base(filetype), m_target(target) { ; }
  ~tDataManager()
  {
    for (typename Apto::Map<Apto::String, tDataEntry<TargetType>* >::ValueIterator it = m_entry_dict.Values(); it.Next();) delete *it.Get();
  }

  template<class EntryType> bool Add(const cString& name,  const cString& desc,
                                     EntryType (TargetType::*funR)() const, void (TargetType::*funS)(EntryType) = NULL,
                                     int compare = 0, const cString& null = "0", const cString& html_cell = "align=center")
  {
    tDataEntry<TargetType>* new_entry =
      new tDataEntryOfType<TargetType, EntryType ()>(name, desc, funR, funS, compare, null, html_cell);
    m_entry_dict.Set((const char*)name, new_entry);
    return true;
  }

  bool Print(const cString& name, std::ostream& fp) const
  {
    tDataEntry<TargetType>* cur_entry = NULL;
    if (m_entry_dict.Get((const char*)name, cur_entry) == false) return false;
    fp << cur_entry->Get(m_target);
    return true;
  }

  bool GetDesc(const cString& name, cString& out_desc) const
  {
    tDataEntry<TargetType>* cur_entry = NULL;
    if (m_entry_dict.Get((const char*)name, cur_entry) == false) return false;
    out_desc = cur_entry->GetDesc(m_target, 0);
    return true;
  }
  
  inline bool GetEntry(const cString& name, const tDataEntry<TargetType>*& entry) const
  { 
    if (m_entry_dict.Get((const char*)name, entry)) return true;
    return false;
  }
};

#endif
