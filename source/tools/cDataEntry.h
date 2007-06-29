/*
 *  cDataEntry.h
 *  Avida
 *
 *  Called "data_entry.hh" prior to 12/2/05.
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

#ifndef cDataEntry_h
#define cDataEntry_h

#ifndef cString_h
#include "cString.h"
#endif
#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


#include <iostream>

class cDataEntry {
#if USE_tMemTrack
  tMemTrack<cDataEntry> mt;
#endif
private:
  cString name;            // Short Name
  cString desc;            // Descriptive Name
  int compare_type;        // ID to indicate how values should be compared.
  cString null_value;      // Value when "off", such as "0", "Inf.", or "N/A"
  cString html_table_flags; // String to include in <td> entry in html mode.
public:
  cDataEntry(const cString & _name, const cString & _desc,
	     int _compare_type = 0,
	     const cString & _null="0",
	     const cString & _html_cell="align=center")
    : name(_name), desc(_desc), compare_type(_compare_type), null_value(_null), html_table_flags(_html_cell) { ; }
  virtual ~cDataEntry() { ; }

  const cString & GetName() const { return name; }
  const cString & GetDesc() const { return desc; }
  int GetCompareType() const { return compare_type; }
  const cString & GetNull() const { return null_value; }
  const cString & GetHtmlCellFlags() const { return html_table_flags; }

  template<class Archive>
  void serialize(Archive & a, const unsigned int version)
  { 
    a.ArkvObj("name", name);
    a.ArkvObj("desc", desc);
    a.ArkvObj("null_value", null_value);
    a.ArkvObj("html_table_flags", html_table_flags);
  }   

};

#endif
