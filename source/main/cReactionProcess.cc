//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REACTION_PROCESS_HH
#include "cReactionProcess.h"
#endif

#ifndef DEFS_HH
#include "defs.hh"
#endif
#ifndef nReaction_h
#include "nReaction.h"
#endif

//////////////////////
//  cReactionProcess
//////////////////////

cReactionProcess::cReactionProcess()
  : resource(NULL)
  , value(1.0)
  , type(nReaction::PROCTYPE_ADD)
  , max_number(1.0)
  , min_number(0.0)
  , max_fraction(1.0)
  , product(NULL)
  , conversion(1.0)
  , inst_id(-1)
  , lethal(0)
  , detect(NULL)
  , detection_threshold(0.0)
  , detection_error(0.0)
{
}

cReactionProcess::~cReactionProcess()
{
}
