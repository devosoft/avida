//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_EVENT_HH
#include "population_event.hh"
#endif

#ifndef EVENT_HH
#include "event.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif

cPopulationEvent::cPopulationEvent(const cString & name, const cString & args):
   cEvent(name, args)
{
}
