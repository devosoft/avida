/*
 *  tDataManager.h
 *  Avida
 *
 *  Called "tDataManager.hh" prior to 12/7/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef tDataManager_h
#define tDataManager_h

#include <iostream>

#ifndef cDataManager_Base_h
#include "cDataManager_Base.h"
#endif
#ifndef tDataEntryBase_h
#include "tDataEntryBase.h"
#endif
#ifndef tDictionary_h
#include "tDictionary.h"
#endif

// This template manages a collection of keywords and associates each with
// an accessor for a specific object.  When a list of keywords is then
// fed in with an output file, the associated data for that object will
// then be printed to the output file.

class cString;
template <class T, class U> class tDataEntry;

template <class T> class tDataManager : public cDataManager_Base {
private:
  T * target;
  tDictionary< tDataEntryBase<T> * > entry_dict;
public:
  tDataManager(T * _target, const cString & in_filetype="unknown")
    : cDataManager_Base(in_filetype), target(_target) { ; }
  ~tDataManager() { ; }

  template<class OUT> bool Add(const cString & name,  const cString & desc,
			       OUT (T::*_funR)() const,
			       void (T::*_funS)(OUT _val) = NULL,
			       int compare = 0,
			       const cString & null="0",
			       const cString & html_cell="align=center") {
    tDataEntryBase<T> * new_entry =
     new tDataEntry<T, OUT> (name, desc, _funR, _funS, compare, null, html_cell);
    new_entry->SetTarget(target);
    entry_dict.Add(name, new_entry);
    return true;
  }

  bool Print(const cString & name, std::ostream& fp) {
    tDataEntryBase<T> * cur_entry = NULL;
    if (entry_dict.Find(name, cur_entry) == false) return false;
    fp << cur_entry->Get();
    return true;
  }

  bool GetDesc(const cString & name, cString & out_desc) {
    tDataEntryBase<T> * cur_entry = NULL;
    if (entry_dict.Find(name, cur_entry) == false) return false;
    out_desc = cur_entry->GetDesc();
    return true;
  }
};

#endif
