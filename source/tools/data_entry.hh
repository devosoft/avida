//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef DATA_ENTRY_HH
#define DATA_ENTRY_HH

#include <iostream>

#ifndef STRING_HH
#include "string.hh"
#endif

class cString; // aggregate

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

  virtual bool Print(std::ostream & fp) const { (void) fp;  return false; }
};

std::ostream & operator << (std::ostream & out, cDataEntry & entry);

#endif
