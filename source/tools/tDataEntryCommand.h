/*
 *  tDataEntryCommand.h
 *  Avida
 *
 *  Called "tDataEntryCommand.hh" prior to 12/7/05.
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

#ifndef tDataEntryCommand_h
#define tDataEntryCommand_h

#include <iostream>

#ifndef cFlexVar_h
#define "cFlexVar.h"
#endif

#ifndef cStringList_h
#include "cStringList.h"
#endif

#ifndef tDataEntryBase_h
#include "tDataEntryBase.h"
#endif

class cString;


template <class T> class tDataEntryCommand {
private:
  tDataEntryBase<T> * data_entry;
  cStringList args;
public:
  tDataEntryCommand(tDataEntryBase<T> * _entry, const cString & _args="")
    : data_entry(_entry), args(_args, ':') { ; }
  
  tDataEntryBase<T> & GetEntry() { return *data_entry; }
  const cStringList & GetArgs() const { return args; }
  bool HasArg(const cString & test_arg) { return args.HasString(test_arg); }

  const cString & GetName() const { return data_entry->GetName(); }
  const cString & GetDesc() const { return data_entry->GetDesc(); }
  const cString & GetNull() const { return data_entry->GetNull(); }
  const cString & GetHtmlCellFlags() const { return data_entry->GetHtmlCellFlags(); }
  cFlexVar GetValue() const { return data_entry->Get(); }

  void SetTarget(T * _target) { data_entry->SetTarget(_target); }
  bool Print(std::ostream& fp) const { return data_entry->Print(fp); }
  int Compare(T * other) const { return data_entry->Compare(other); }
  bool SetValue(const cString & value) { return data_entry->Set(value); }
};

#endif
