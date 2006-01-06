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

#include <iostream>

#ifndef cString_h
#include "cString.h"
#endif

class cDataEntry {
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
};

inline std::ostream& operator << (std::ostream& out, cDataEntry & entry)
{
  entry.Print(out);
  return out;
}

#endif
