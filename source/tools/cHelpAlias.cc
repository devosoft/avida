//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cHelpAlias.h"
#include "cHelpFullEntry.h"

const cString & cHelpAlias::GetKeyword() const { return full_entry->GetKeyword(); }
const cString & cHelpAlias::GetDesc() const { return full_entry->GetDesc(); }
cHelpType * cHelpAlias::GetType() const { return full_entry->GetType(); }
