/*
 *  cHelpAlias.cc
 *  Avida
 *
 *  Called "help_alias.cc" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cHelpAlias.h"

#include "cHelpFullEntry.h"


const cString & cHelpAlias::GetKeyword() const { return full_entry->GetKeyword(); }
const cString & cHelpAlias::GetDesc() const { return full_entry->GetDesc(); }
cHelpType * cHelpAlias::GetType() const { return full_entry->GetType(); }
