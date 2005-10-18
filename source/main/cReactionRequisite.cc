//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REACTION_REQUISITE_HH
#include "cReactionRequisite.h"
#endif

#include <limits.h>

////////////////////////
//  cReactionRequisite
////////////////////////

cReactionRequisite::cReactionRequisite()
  : min_task_count(0)
  , max_task_count(INT_MAX)
{
}

cReactionRequisite::~cReactionRequisite()
{
}
