/*
 *  core/EventList.cc
 *  avida-core
 *
 *  Called "event_list.cc" prior to 12/2/05 and "cEventList.cc" prior to 4/17/14.
 *  Copyright 1999-2014 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *  http://avida.devosoft.org/
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors: Charles Ofria <ofria@msu.edu>, David M. Bryson <david@programerror.com>
 *
 */

#include "avida/core/EventList.h"

#include "avida/core/Feedback.h"
#include "avida/core/Library.h"
#include "avida/util/ArgParser.h"

#include <limits>


const double Avida::EventList::TRIGGER_BEGIN = std::numeric_limits<double>::min();
const double Avida::EventList::TRIGGER_END = std::numeric_limits<double>::max();
const double Avida::EventList::TRIGGER_ALL = 0.0;
const double Avida::EventList::TRIGGER_ONCE = std::numeric_limits<double>::max();


Avida::EventAction::~EventAction() { ; }


Avida::EventList::~EventList()
{
  Entry* current = NULL;
  while (m_head != NULL) {
    current = m_head;
    m_head = m_head->Next();
    delete current;
  }
}


bool Avida::EventList::AddEvent(Context& ctx, Update current_update, EventTriggerFunctor trigger, double start,
                                double interval, double stop, EventAction* action)
{
  if (action != NULL) {
    Entry* entry = new Entry(action, trigger, start, interval, stop);
    
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
    
    syncEvent(ctx, current_update, entry);
		
    m_num_events++;
    return true;
  }
  
  return false;
}

bool Avida::EventList::LoadEventFile(Context& ctx, Update current_update, Util::ConfigFile& file, Feedback& feedback)
{
  Apto::String line;
  while (file.ReadLine(line)) {
    if (!LoadEventLine(ctx, current_update, line, feedback)) return false;
  }
  
  return true;
}


bool Avida::EventList::LoadEventLine(Context& ctx, Update current_update, const Apto::String& in_line, Feedback& feedback)
{
  Apto::String cur_line = in_line;
  
  // Timing
  double start = TRIGGER_BEGIN;
  double interval = TRIGGER_ONCE;
  double stop = TRIGGER_END;
  
  
  Apto::String trigger_key = cur_line.PopWord();
  
  // Get the trigger variable if there
  const Core::Library::EventTriggerType* trigger_type = Core::Library::Instance().EventTriggerTypeOf(trigger_key);
  if (trigger_type == NULL) {
    feedback.Error("invalid event trigger type '%s'", (const char*)trigger_key);
    return false;
  }
  
  // Do we now have timing specified?
  // Parse the Timing
  Apto::String timing_str = cur_line.PopWord();
  
  // Get the start:interval:stop
  Apto::String tmp = timing_str.Pop(':');
  
  // If first value is valid, we are getting a timing.
  if (tmp.IsNumber() || tmp == "begin") {
    
    // First number is start
    if (tmp == "begin") start = TRIGGER_BEGIN;
    else start = Apto::StrAs(tmp);
    
    // If no other words... is "start" syntax
    if (timing_str.GetSize() == 0) {
      interval = TRIGGER_ONCE;
      stop = TRIGGER_END;
    } else {
      // Second word is interval
      tmp = timing_str.Pop(':');
      if (tmp == "all") interval = TRIGGER_ALL;
      else if (tmp == "once") interval = TRIGGER_ONCE;
      else interval = Apto::StrAs(tmp);
      
      // If no other words... is "start:interval" syntax
      if (timing_str.GetSize() == 0) stop = TRIGGER_END;
      else {
        // We have "start:interval:stop" syntax
        tmp = timing_str;
        if (tmp == "end") stop = TRIGGER_END;
        else stop = Apto::StrAs(tmp);
      }
    }
  } else {
    feedback.Error("invalid event timing '%s'", (const char*)tmp);
    return false;
  }
  
  // Get the rest of the info
  Apto::String name = cur_line.PopWord();
  Apto::String arg_list = cur_line;
  
  const Core::Library::EventType* action_type = Core::Library::Instance().EventTypeOf(name);
  if (action_type == NULL) {
    feedback.Error("invalid event action '%s'", (const char*)name);
    return false;
  }
  
  Util::Args* args = Util::Args::Load(arg_list, action_type->schema, ',', '=', &feedback);
  if (args == NULL) {
    feedback.Error("failed to load arguments for '%s'", (const char*)name);
    return false;
  }
  
  EventAction* action = action_type->create(m_universe, ctx, args);
  if (action == NULL) {
    feedback.Error("failed to create event action for '%s'", (const char*)name);
    return false;
  }
  
  return AddEvent(ctx, current_update, trigger_type->trigger, start, interval, stop, action);
}




void Avida::EventList::Process(Context& ctx, Update current_update)
{
  double t_val = 0; // trigger value
  
  // Iterate through all entrys in event list
  Entry* entry = m_head;
  while (entry != NULL) {
    Entry* next_entry = entry->Next();
    
    // Get the value of the appropriate trigger variable
    t_val = entry->TriggerValue(m_universe, ctx, current_update);
    
    if (t_val != std::numeric_limits<double>::max() && (t_val >= entry->StartValue() || entry->StartValue() == TRIGGER_BEGIN) &&
        (t_val <= entry->StopValue() || entry->StopValue() == TRIGGER_END)) {
      
      // Process the Action
      entry->Action().Process(ctx);
      
      // Handle Interval Adjustment
      if (entry->Interval() == TRIGGER_ALL) {
        // Do Nothing
      } else if (entry->Interval() == TRIGGER_ONCE) {
        // If it is a onetime thing, remove it...
        deleteEvent(entry);
        entry = NULL;
      } else {
        // There is an interval.. so add it
        entry->NextInterval();
      }
      
      // If the event can never happen now... excize it
      if (entry != NULL && entry->StopValue() != TRIGGER_END &&
          ((entry->StartValue() > entry->StopValue() && entry->Interval() > 0) ||
           (entry->StartValue() < entry->StopValue() && entry->Interval() < 0))) {
        deleteEvent(entry);
      }
    }
    entry = next_entry;
  }
}



void Avida::EventList::syncEvent(Context& ctx, Update current_update, Entry* entry)
{
  double t_val = entry->TriggerValue(m_universe, ctx, current_update);
  
  // If t_val has past the end, remove (even if it is TRIGGER_ALL)
  if (t_val > entry->StopValue()) {
    deleteEvent(entry);
    return;
  }
  
  // If it is a trigger once and has passed, remove
  if (t_val > entry->StartValue() && entry->Interval() == TRIGGER_ONCE) {
    deleteEvent(entry);
    return;
  }
  
  // If for some reason t_val has been reset or soemthing, rewind
  if (t_val + entry->Interval() <= entry->StartValue()) {
    entry->Reset();
  }
  
  // Can't fast forward events that are Triger All
  if (entry->Interval() == TRIGGER_ALL) return;
  
  // Keep adding interval to start until we are caught up
  while (t_val > entry->StartValue()) entry->NextInterval();
}

void Avida::EventList::deleteEvent(Entry* entry)
{
  assert(entry != NULL);
  
  if (entry->Prev() != NULL) {
    entry->Prev()->SetNext(entry->Next());
  } else {
    assert(entry == m_head);
    m_head = entry->Next();
  }
  
  if (entry->Next() != NULL) {
    entry->Next()->SetPrev(entry->Prev());
  } else {
    assert(entry == m_tail);
    m_tail = entry->Prev();
  }
  
  delete entry;
}
