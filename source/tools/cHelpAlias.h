//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HELP_ALIAS_HH
#define HELP_ALIAS_HH

#ifndef HELP_ENTRY_HH
#include "cHelpEntry.h"
#endif

class cHelpFullEntry;
class cHelpType;
class cString;

class cHelpAlias : public cHelpEntry {
private:
  cHelpFullEntry * full_entry;
public:
  cHelpAlias(const cString & _alias, cHelpFullEntry * _full_entry)
    : cHelpEntry(_alias), full_entry(_full_entry) { ; }
  ~cHelpAlias() { ; }

  const cString & GetKeyword() const;
  const cString & GetDesc() const;
  cHelpType * GetType() const;

  bool IsAlias() const { return true; }
};

#endif
