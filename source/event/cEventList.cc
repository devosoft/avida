//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cEventList.h"

#include "defs.h"
#include "cEventManager.h"
#include "cEventListEntry.h"
#include "cEventTriggers.h"
#include "cString.h"

#include <float.h>           // for DBL_MIN
#include <iostream>

using namespace std;

class cEvent;

/////////////////
//  cEventList
/////////////////
cEventList::cEventList( cEventManager* factory_manager,  cEventTriggers *triggers ) :
m_factory_manager( factory_manager ),
m_triggers( triggers ),
m_head(NULL),
m_tail(NULL),
m_current(NULL),
m_num_events(0)
{
}

cEventList::~cEventList()
{
  DeleteAll();
  delete m_factory_manager;
  delete m_triggers;
}

bool
cEventList::AddEvent( cEventTriggers::eTriggerVariable trigger,
                      double start, double interval, double stop,
                      const cString & name, const cString & args ){
  assert( m_factory_manager != NULL );
  cEvent *event = m_factory_manager->ConstructEvent( name, args );
  
  ///// Adding Event to the list /////
  if( event != NULL ){
    InsertEvent(event, trigger, start, interval, stop);
    return true;
  }
  else return false;
}


void cEventList::InsertEvent(cEvent *event,
                             cEventTriggers::eTriggerVariable trigger,
                             double start, double interval, double stop){
  assert( event != NULL );
  cEventListEntry *entry = new cEventListEntry(event, trigger,
                                               start, interval, stop);
  // If there are no events in the list yet.
  if( m_tail == NULL ){
    assert( m_head == NULL );
    m_head = entry;
    m_tail = entry;
  }
  else{
    // Add to the end of the list
    m_tail->SetNext( entry );
    entry->SetPrev( m_tail );
    m_tail = entry;
  }
  SyncEvent(entry);
  ++m_num_events;
}


void cEventList::Delete(cEventListEntry *entry){
  assert( entry != NULL );
  
  if( entry->GetPrev() != NULL ){
    entry->GetPrev()->SetNext( entry->GetNext() );
  }
  else{
    assert( entry == m_head );
    m_head = entry->GetNext();
  }
  if( entry->GetNext() != NULL ){
    entry->GetNext()->SetPrev( entry->GetPrev() );
  }
  else{
    assert( entry == m_tail );
    m_tail = entry->GetPrev();
  }
  
  delete entry;
}


/*
 public manipulator used while rebuilding event list.
 */
void cEventList::DeleteAll(void){
  while( m_head != NULL ){
    m_current = m_head;
    m_head = m_head->GetNext();
    delete m_head;
  }
  
  m_head = 0;
  m_tail = 0;
  m_current = 0;
  m_num_events = 0;
}


void cEventList::Process(){
  double t_val = 0; // trigger value
  
  // Iterate through all entrys in event list
  cEventListEntry * entry = m_head;
  while( entry != NULL ){
    
    cEventListEntry * next_entry = entry->GetNext();
    
    // Check trigger condition
    
    // IMMEDIATE Events always happen and are always deleted
    if( entry->GetTrigger() == cEventTriggers::IMMEDIATE  ) {
      entry->GetEvent()->Process();
      Delete(entry);
    } else {
      // Get the value of the appropriate trigger variable
      t_val = m_triggers->GetTriggerValue(entry->GetTrigger());
      
      if( t_val != DBL_MAX &&
          (t_val >= entry->GetStart() ||
            entry->GetStart() == cEventTriggers::TRIGGER_BEGIN) &&
          (t_val <= entry->GetStop() ||
            entry->GetStop() == cEventTriggers::TRIGGER_END) ) {
        
        entry->GetEvent()->Process();
        
        if( entry == NULL ){ // It is possible for an event to kill itself
        }else{	
          // Handle the interval thing
          if( entry->GetInterval() == cEventTriggers::TRIGGER_ALL ){
            // Do Nothing
          }else if( entry->GetInterval() == cEventTriggers::TRIGGER_ONCE ){
            // If it is a onetime thing, remove it...
            Delete(entry);
          }else{
            // There is an interval.. so add it
            entry->NextInterval();
          }
          
          // If the event can never happen now... excize it
          if( entry != NULL  &&  entry->GetStop() != cEventTriggers::TRIGGER_END ){
            if( entry->GetStart() > entry->GetStop() &&
                entry->GetInterval() > 0 ){
              Delete(entry);
            }else if( entry->GetStart() < entry->GetStop() &&
                      entry->GetInterval() < 0 ){
              Delete(entry);
            }
          }
          
        }
        
      } // End Non-IMMEDITAE events
      
    }  // end condition to do event
    
    entry = next_entry;
  }
}


void cEventList::Sync(){
  cEventListEntry *entry = m_head;
  cEventListEntry *next_entry;
  while( entry != NULL ){
    next_entry = entry->GetNext();
    assert( entry != NULL);
    SyncEvent( entry );
    entry = next_entry;
  }
}


void cEventList::SyncEvent(cEventListEntry *entry){
  // Ignore events that are immdeiate
  if( entry->GetTrigger() == cEventTriggers::IMMEDIATE ){ return; }
  
  double t_val = m_triggers->GetTriggerValue( entry->GetTrigger() );
  
  // If t_val has past the end, remove (even if it is TRIGGER_ALL)
  if( t_val > entry->GetStop() ){
    Delete(entry);
    return;
  }
  
  // If it is a trigger once and has passed, remove
  if( t_val > entry->GetStart() && entry->GetInterval() == cEventTriggers::TRIGGER_ONCE ){
    Delete(entry);
    return;
  }
  
  // If for some reason t_val has been reset or soemthing, rewind
  if( t_val + entry->GetInterval() <= entry->GetStart() ){
    entry->Reset();
  }
  
  // Can't fast forward events that are Triger All
  if( entry->GetInterval() == cEventTriggers::TRIGGER_ALL ){ return; }
  
  // Keep adding interval to start until we are caught up
  while( t_val > entry->GetStart() ){
    entry->NextInterval();
  }
}


void cEventList::PrintEventList( ostream& os ){
  cEventListEntry * entry = m_head;
  cEventListEntry * next_entry;
  while( entry != NULL ){
    next_entry = entry->GetNext();
    PrintEvent( entry, os );
    entry = next_entry;
  }
}

void cEventList::PrintEvent(cEventListEntry * entry, ostream& os){
  assert( entry != NULL );
  switch ( entry->GetTrigger() ){
    case cEventTriggers::UPDATE:
      os << "update ";
      break;
    case cEventTriggers::GENERATION:
      os << "generation ";
      break;
    case cEventTriggers::IMMEDIATE:
      os << "immediate ";
      break;
    default:
      os << "undefined ";
  }
  if (entry->GetTrigger() != cEventTriggers::IMMEDIATE ) {
    if( entry->GetStart() == cEventTriggers::TRIGGER_BEGIN ) {
      os << "begin";
    } else {
      os << entry->GetStart();
    }
    os<<":";
    if( entry->GetInterval() == cEventTriggers::TRIGGER_ONCE ) {
      os << "once";
    } else if (entry->GetInterval() == cEventTriggers::TRIGGER_ALL) {
      os << "all";
    } else {
      os << entry->GetInterval();
    }
    os << ":";
    if( entry->GetStop() == cEventTriggers::TRIGGER_END ){
      os << "end";
    }else{
      os << entry->GetStop();
    }
    os << " ";
  }
  os << entry->GetName() << " " << entry->GetArgs() << endl;
}

//// Parsing Event List File Format ////
bool cEventList::AddEventFileFormat(const cString & in_line)
{
  cString cur_line = in_line;
  
  // Timing
  cEventTriggers::eTriggerVariable trigger = cEventTriggers::UPDATE;
  double start = cEventTriggers::TRIGGER_BEGIN;
  double interval = cEventTriggers::TRIGGER_ONCE;
  double stop = cEventTriggers::TRIGGER_END;
  
  cString name;
  cString arg_list;
  
  cString tmp;
  
  cString cur_word = cur_line.PopWord();
  
  // Get the trigger variable if there
  if( cur_word == "i"  ||
      cur_word == "immediate" ){
    trigger = cEventTriggers::IMMEDIATE;
    name = cur_line.PopWord();
    return AddEvent(name, cur_line); // If event is IMMEDIATE shortcut
  }else if( cur_word == "u"  ||
            cur_word == "update" ){
    trigger = cEventTriggers::UPDATE;
    cur_word = cur_line.PopWord();
  }else if( cur_word == "g"  ||
            cur_word == "generation" ){
    trigger = cEventTriggers::GENERATION;
    cur_word = cur_line.PopWord();
  }else{
    // If Trigger is skipped so assume IMMEDIATE
    trigger = cEventTriggers::IMMEDIATE;
  }
  
  // Do we now have timing specified?
  // Parse the Timing
  cString timing_str = cur_word;
  
  // Get the start:interval:stop
  tmp = timing_str.Pop(':');
  
  // If first value is valid, we are getting a timing.
  if( tmp.IsNumber() || tmp == "begin" ){
    
    // First number is start
    if( tmp == "begin" ){
      start = cEventTriggers::TRIGGER_BEGIN;
    }else{
      start = tmp.AsDouble();
    }
    
    // If no other words... is "start" syntax
    if( timing_str.GetSize() == 0 ){
      interval = cEventTriggers::TRIGGER_ONCE;
      stop     = cEventTriggers::TRIGGER_END;
    }else{
      // Second word is interval
      tmp = timing_str.Pop(':');
      if( tmp == "all" ){
        interval = cEventTriggers::TRIGGER_ALL;
      }else if( tmp == "once" ){
        interval = cEventTriggers::TRIGGER_ONCE;
      }else{
        interval = tmp.AsDouble();
      }
      // If no other words... is "start:interval" syntax
      if( timing_str.GetSize() == 0 ){
        stop     = cEventTriggers::TRIGGER_END;
      }else{
        // We have "start:interval:stop" syntax
        tmp = timing_str;
        if( tmp == "end" ){
          stop = cEventTriggers::TRIGGER_END;
        }else{
          stop = tmp.AsDouble();
        }
      }
    }
    cur_word = cur_line.PopWord(); // timing provided, so get next word
    
  }else{ // We don't have timing, so assume IMMEDIATE
    trigger = cEventTriggers::IMMEDIATE;
    start = cEventTriggers::TRIGGER_BEGIN;
    interval = cEventTriggers::TRIGGER_ONCE;
    stop = cEventTriggers::TRIGGER_END;
  }
  
  // Get the rest of the info
  name = cur_word;
  arg_list = cur_line;
  
  return AddEvent( trigger, start, interval, stop, name, arg_list );
}
