//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HELP_FULL_ENTRY_HH
#define HELP_FULL_ENTRY_HH

#ifndef HELP_ENTRY_HH
#include "help_entry.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif

class cHelpType;
class cString; // aggregate

class cHelpFullEntry : public cHelpEntry {
private:
  cHelpType * type;
  cString desc;
public:
  cHelpFullEntry(const cString & _name, cHelpType * _type, const cString _desc)
    : cHelpEntry(_name), type(_type), desc(_desc) { ; }
  cHelpFullEntry() : cHelpEntry(""), type(NULL) { ; }
  ~cHelpFullEntry() { ; }

  const cHelpFullEntry & operator=(const cHelpEntry & in_entry) {
    name = in_entry.GetName();
    type = in_entry.GetType();
    desc = in_entry.GetDesc();
    return *this;
  }

  const cString & GetKeyword() const { return name; }
  const cString & GetDesc() const { return desc; }
  cHelpType * GetType() const { return type; }

  bool IsAlias() const { return false; }
};

#endif
