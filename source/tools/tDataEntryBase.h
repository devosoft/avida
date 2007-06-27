/*
 *  tDataEntryBase.h
 *  Avida
 *
 *  Called "tDataEntryBase.hh" prior to 12/7/05.
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

#ifndef tDataEntryBase_h
#define tDataEntryBase_h

#include <iostream>
#include <sstream>

#ifndef cDataEntry_h
#include "cDataEntry.h"
#endif

#ifndef cFlexVar_h
#include "cFlexVar.h"
#endif

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif

class cString;

template <class T> class tDataEntryBase : public cDataEntry {
#if USE_tMemTrack
  tMemTrack<tDataEntryBase<T> > mt;
#endif
protected:
  T * target;
public:
  tDataEntryBase(const cString & _name, const cString & _desc,
		 const cString & _null="0",
		 const cString & _html_cell="align=center")
    : cDataEntry(_name, _desc, _null, _html_cell), target(NULL) { ; }
  
  void SetTarget(T * _target) { target = _target; }

  virtual bool Print(std::ostream& fp) const { (void) fp;  return false; }
  virtual int Compare(T * other) const { (void) other; return 0; }
  virtual bool Set(const cString & value) { (void) value; return false; }
  virtual cFlexVar Get() const { return cFlexVar(0); }

  cString AsString() {
    std::stringstream tmp_stream;
    tmp_stream << *this;
    cString out_str;
    tmp_stream >> out_str;
    return out_str;
  }
};

#endif
