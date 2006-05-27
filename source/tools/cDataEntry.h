/*
 *  cDataEntry.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
  cString null_value;      // Value when "off", such as "0", "Inf.", or "N/A"
  cString html_table_flags; // String to include in <td> entry in html mode.
public:
  cDataEntry(const cString & _name, const cString & _desc,
	     const cString & _null="0",
	     const cString & _html_cell="align=center")
    : name(_name), desc(_desc), null_value(_null),
      html_table_flags(_html_cell) { ; }
  virtual ~cDataEntry() { ; }

  const cString & GetName() const { return name; }
  const cString & GetDesc() const { return desc; }
  const cString & GetNull() const { return null_value; }
  const cString & GetHtmlCellFlags() const { return html_table_flags; }

  virtual bool Print(std::ostream& fp) const { (void) fp;  return false; }

  template<class Archive>
  void serialize(Archive & a, const unsigned int version)
  { 
    a.ArkvObj("name", name);
    a.ArkvObj("desc", desc);
    a.ArkvObj("null_value", null_value);
    a.ArkvObj("html_table_flags", html_table_flags);
  }   

};

inline std::ostream& operator << (std::ostream& out, cDataEntry & entry)
{
  entry.Print(out);
  return out;
}

  
#ifdef ENABLE_UNIT_TESTS
namespace nDataEntry {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
