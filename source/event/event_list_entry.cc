//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_LIST_ENTRY_HH
#include "event_list_entry.hh"
#endif


/////////////////
//  cEventListEntry
/////////////////


cEventListEntry::cEventListEntry( cEvent * event,
				  cEventTriggers::eTriggerVariable trigger,
				  double start,
				  double interval,
				  double stop,
				  cEventListEntry * prev,
				  cEventListEntry * next ) :
  m_event( event ),
  m_trigger( trigger ),
  m_start( start ),
  m_interval( interval ),
  m_stop( stop ),
  m_original_start( start ),
  m_prev( prev ),
  m_next( next )
{
}


cEventListEntry::~cEventListEntry()
{
  delete m_event;
}
