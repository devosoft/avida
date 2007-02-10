/*
 *  tArgDataEntry.h
 *  Avida
 *
 *  Called "tArgDataEntry.hh" prior to 12/7/05.
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

#ifndef tArgDataEntry_h
#define tArgDataEntry_h

#include <iostream>

#ifndef tDataEntryBase_h
#include "tDataEntryBase.h"
#endif

class cString;

template <class T, class OUT, class ARG> class tArgDataEntry
  : public tDataEntryBase<T> {
protected:
  OUT (T::*DataRetrieval)(ARG) const;
  int (T::*DataCompare)(T*,ARG) const;
  ARG arg;
public:
  tArgDataEntry(const cString & _name, const cString & _desc,
		OUT (T::*_funR)(ARG) const, ARG _arg,
		//int (T::*_funC)(T * _o, ARG _i) const = &T::CompareArgNULL,
		int (T::*_funC)(T * _o, ARG _i) const = 0,
		const cString & _null="0",
		const cString & _html_cell="align=center")
    : tDataEntryBase<T>(_name, _desc, _null, _html_cell),
      DataRetrieval(_funR), DataCompare(_funC), arg(_arg) { ; }

  bool Print(std::ostream& fp) const {
    if (this->target == NULL) return false;
    fp << (this->target->*DataRetrieval)(arg);
    return true;
  }

  //int Compare(T * other) const { return (target->*DataCompare)(other, arg); }
  int Compare(T * other) const {
    return (DataCompare)?((this->target->*DataCompare)(other, arg)):(0);
  }
};

#endif
