//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HELP_ALIAS_HH
#include "help_alias.hh"
#endif

#ifndef HELP_FULL_ENTRY_HH
#include "help_full_entry.hh"
#endif

const cString & cHelpAlias::GetKeyword() const { return full_entry->GetKeyword(); }
const cString & cHelpAlias::GetDesc() const { return full_entry->GetDesc(); }
cHelpType * cHelpAlias::GetType() const { return full_entry->GetType(); }
