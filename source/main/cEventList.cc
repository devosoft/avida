/*
 *  cEventList.cc
 *  Avida
 *
 *  Called "event_list.cc" prior to 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cEventList.h"

#include "defs.h"
#include "cActionLibrary.h"
#include "cInitFile.h"
#include "cStats.h"
#include "cString.h"
#include "cWorld.h"

#include <float.h>           // for DBL_MIN
#include <iostream>

using namespace std;


const double cEventList::TRIGGER_BEGIN = DBL_MIN;
const double cEventList::TRIGGER_END = DBL_MAX;
const double cEventList::TRIGGER_ALL = 0.0;
const double cEventList::TRIGGER_ONCE = DBL_MAX;


cEventList::~cEventList()
{
  cEventListEntry* current = NULL;
  while (m_head != NULL) {
    current = m_head;
    m_head = m_head->GetNext();
    delete current;
  }
}


bool cEventList::AddEvent(eTriggerType trigger, double start, double interval,
                          double stop, const cString& name, const cString& args)
{
  cAction* action = m_world->GetActionLibrary().Create(name, m_world, args);
  
  if (action != NULL) {
    cEventListEntry* entry = new cEventListEntry(action, name, trigger, start, interval, stop);
    
    // If there are no events in the list yet.
    if (m_tail == NULL) {
      assert(m_head == NULL);
      m_head = entry;
      m_tail = entry;
    } else {
      // Add to the end of the list
      m_tail->SetNext(entry);
      entry->SetPrev(m_tail);
      m_tail = entry;
    }
    
    SyncEvent(entry);
    ++m_num_events;
    return true;
  }
  
  return false;
}

bool cEventList::LoadEventFile(const cString& filename)
{
  cInitFile event_file(filename);
  
  if (!event_file.IsOpen()) return false;

  // Load in the proper event list and set it up.
  event_file.Load();
  event_file.Compress();
  event_file.Close();
  
  // Loop through the line_list and change the lines to events.
  for (int line_id = 0; line_id < event_file.GetNumLines(); line_id++) {
    AddEventFileFormat(event_file.GetLine(line_id));
  }
  
  return true;
}


void cEventList::Delete(cEventListEntry* entry) 
{
  assert(entry != NULL);
  
  if (entry->GetPrev() != NULL) {
    entry->GetPrev()->SetNext(entry->GetNext());
  } else {
    assert(entry == m_head);
    m_head = entry->GetNext();
  }
  
  if (entry->GetNext() != NULL) {
    entry->GetNext()->SetPrev(entry->GetPrev());
  } else {
    assert(entry == m_tail);
    m_tail = entry->GetPrev();
  }
  
  delete entry;
}

double cEventList::GetTriggerValue(eTriggerType trigger) const
{
  // Returns TRIGGER_END if invalid, TRIGGER_BEGIN for IMMEDIATE
  double t_val = TRIGGER_END;
  switch (trigger) {
  case IMMEDIATE:
    t_val = TRIGGER_BEGIN;
    break;
  case UPDATE:
    t_val = m_world->GetStats().GetUpdate();
    break;
  case GENERATION:
    t_val = m_world->GetStats().SumGeneration().Average();
    break;
  case UNDEFINED:
    break;
  }
  return t_val;
}


void cEventList::Process(cAvidaContext& ctx)
{
  double t_val = 0; // trigger value
  
  // Iterate through all entrys in event list
  cEventListEntry* entry = m_head;
  while (entry != NULL) {
    cEventListEntry* next_entry = entry->GetNext();
    
    // Check trigger condition
    
    // IMMEDIATE Events always happen and are always deleted
    if (entry->GetTrigger() == IMMEDIATE) {
      entry->GetAction()->Process(ctx);
      Delete(entry);
    } else {
      // Get the value of the appropriate trigger variable
      t_val = GetTriggerValue(entry->GetTrigger());
      
      if (t_val != DBL_MAX &&
          (t_val >= entry->GetStart() || entry->GetStart() == TRIGGER_BEGIN) &&
          (t_val <= entry->GetStop() || entry->GetStop() == TRIGGER_END)) {

        // Process the Action
        entry->GetAction()->Process(ctx);
        
        // Handle Interval Adjustment
        if (entry->GetInterval() == TRIGGER_ALL) {
          // Do Nothing
        } else if (entry->GetInterval() == TRIGGER_ONCE) {
          // If it is a onetime thing, remove it...
          Delete(entry);
          entry = NULL;
        } else {
          // There is an interval.. so add it
          entry->NextInterval();
        }
        
        // If the event can never happen now... excize it
        if (entry != NULL && entry->GetStop() != TRIGGER_END &&
            ((entry->GetStart() > entry->GetStop() && entry->GetInterval() > 0) ||
             (entry->GetStart() < entry->GetStop() && entry->GetInterval() < 0)))
            Delete(entry);
      }
    }  // end condition to do event
    
    entry = next_entry;
  }
}


void cEventList::Sync()
{
  cEventListEntry* entry = m_head;
  cEventListEntry* next_entry;
  while (entry != NULL) {
    next_entry = entry->GetNext();
    SyncEvent(entry);
    entry = next_entry;
  }
}


void cEventList::SyncEvent(cEventListEntry* entry)
{
  // Ignore events that are immdeiate
  if (entry->GetTrigger() == IMMEDIATE) return;
  
  double t_val = GetTriggerValue(entry->GetTrigger());
  
  // If t_val has past the end, remove (even if it is TRIGGER_ALL)
  if (t_val > entry->GetStop()) {
    Delete(entry);
    return;
  }
  
  // If it is a trigger once and has passed, remove
  if (t_val > entry->GetStart() && entry->GetInterval() == TRIGGER_ONCE) {
    Delete(entry);
    return;
  }
  
  // If for some reason t_val has been reset or soemthing, rewind
  if (t_val + entry->GetInterval() <= entry->GetStart()) {
    entry->Reset();
  }
  
  // Can't fast forward events that are Triger All
  if (entry->GetInterval() == TRIGGER_ALL) return;
  
  // Keep adding interval to start until we are caught up
  while (t_val > entry->GetStart()) entry->NextInterval();
}


void cEventList::PrintEventList(ostream& os)
{
  cEventListEntry* entry = m_head;
  cEventListEntry* next_entry;
  while (entry != NULL) {
    next_entry = entry->GetNext();
    switch (entry->GetTrigger()) {
      case UPDATE:
        os << "update ";
        break;
      case GENERATION:
        os << "generation ";
        break;
      case IMMEDIATE:
        os << "immediate ";
        break;
      default:
        os << "undefined ";
    }
    if (entry->GetTrigger() != IMMEDIATE) {
      if (entry->GetStart() == TRIGGER_BEGIN) os << "begin";
      else os << entry->GetStart();

      os << ":";
      
      if (entry->GetInterval() == TRIGGER_ONCE) os << "once";
      else if (entry->GetInterval() == TRIGGER_ALL) os << "all";
      else os << entry->GetInterval();
      
      os << ":";
      
      if (entry->GetStop() == TRIGGER_END) os << "end";
      else os << entry->GetStop();
      
      os << " ";
    }
    os << entry->GetName() << " " << entry->GetArgs() << endl;
    entry = next_entry;
  }
}


//// Parsing Event List File Format ////
bool cEventList::AddEventFileFormat(const cString& in_line)
{
  cString cur_line = in_line;
  
  // Timing
  eTriggerType trigger = UPDATE;
  double start = TRIGGER_BEGIN;
  double interval = TRIGGER_ONCE;
  double stop = TRIGGER_END;
  
  cString name;
  cString arg_list;
  
  cString tmp;
  
  cString cur_word = cur_line.PopWord();
  
  // Get the trigger variable if there
  if (cur_word == "i" || cur_word == "immediate") {
    trigger = IMMEDIATE;
    name = cur_line.PopWord();
    return AddEvent(name, cur_line); // If event is IMMEDIATE shortcut
  } else if (cur_word == "u" || cur_word == "update") {
    trigger = UPDATE;
    cur_word = cur_line.PopWord();
  } else if( cur_word == "g" || cur_word == "generation") {
    trigger = GENERATION;
    cur_word = cur_line.PopWord();
  } else {
    // If Trigger is skipped so assume IMMEDIATE
    trigger = IMMEDIATE;
  }
  
  // Do we now have timing specified?
  // Parse the Timing
  cString timing_str = cur_word;
  
  // Get the start:interval:stop
  tmp = timing_str.Pop(':');
  
  // If first value is valid, we are getting a timing.
  if (tmp.IsNumber() || tmp == "begin") {
    
    // First number is start
    if (tmp == "begin") start = TRIGGER_BEGIN;
    else start = tmp.AsDouble();
    
    // If no other words... is "start" syntax
    if (timing_str.GetSize() == 0) {
      interval = TRIGGER_ONCE;
      stop = TRIGGER_END;
    } else {
      // Second word is interval
      tmp = timing_str.Pop(':');
      if (tmp == "all") interval = TRIGGER_ALL;
      else if (tmp == "once") interval = TRIGGER_ONCE;
      else interval = tmp.AsDouble();

      // If no other words... is "start:interval" syntax
      if (timing_str.GetSize() == 0) stop = TRIGGER_END;
      else {
        // We have "start:interval:stop" syntax
        tmp = timing_str;
        if (tmp == "end") stop = TRIGGER_END;
        else stop = tmp.AsDouble();
      }
    }
    cur_word = cur_line.PopWord(); // timing provided, so get next word
    
  } else { // We don't have timing, so assume IMMEDIATE
    trigger = IMMEDIATE;
    start = TRIGGER_BEGIN;
    interval = TRIGGER_ONCE;
    stop = TRIGGER_END;
  }
  
  // Get the rest of the info
  name = cur_word;
  arg_list = cur_line;
  
  return AddEvent(trigger, start, interval, stop, name, arg_list);
}
