/*
 *  cEventList.cc
 *  Avida
 *
 *  Called "event_list.cc" prior to 12/2/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cEventList.h"

#include "Avida.h"

#include "cActionLibrary.h"
#include "cInitFile.h"
#include "cStats.h"
#include "cString.h"
#include "cWorld.h"

#include <cfloat>           // for DBL_MIN
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
  cAction* action = cActionLibrary::GetInstance().Create(name, m_world, args);
  
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
		
		if (trigger == BIRTHS_INTERRUPT)  //Operates outside of usual event processing
			QueueBirthInterruptEvent(start);
		
    ++m_num_events;
    return true;
  }
  
  cerr << "error: unrecognized event '" << name << "'" << endl; 
  
  return false;
}

bool cEventList::LoadEventFile(const cString& filename, const cString& working_dir)
{
  cInitFile event_file(filename, working_dir);
  
  if (!event_file.WasOpened()) return false;

  // Loop through the line_list and change the lines to events.
  for (int line_id = 0; line_id < event_file.GetNumLines(); line_id++) {
    if (!AddEventFileFormat(event_file.GetLine(line_id))) return false;
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
  case BIRTHS_INTERRUPT:
  case BIRTHS:
    t_val = m_world->GetStats().GetTotCreatures();
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
    } else if (entry->GetTrigger() != BIRTHS_INTERRUPT) {
      //BIRTHS_INTERRUPT occur outside of update boundaries
	  //and should not alter the behavior of other events.
	  
	  // Get the value of the appropriate trigger varile
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
    } 
    entry = next_entry;
  }
}


/*
   @MRR January 2007
   I'm adding this method to have events in the population interrupt
   mid-update to perform an event.  Right now, the only trigger is
   when a particular value of tot_creatures is reached (BIRTHS).  At that point
   this method will be called.  Although these events are stored in the
   same queue as other events, they should not be processed at an
   update boundary.  A set of events is kept and queried prior to determine
   when this method should be called.  Some statistic values are not available
   since they are not processed until the end of an update.
*/
void cEventList::ProcessInterrupt(cAvidaContext& ctx)
{
	double t_val = 0; // trigger value
	
	// Iterate through all entrys in event list
	cEventListEntry* entry = m_head;
	while (entry != NULL) {
		cEventListEntry* next_entry = entry->GetNext();
		
		//BIRTHS_INTERRUPT occur outside of update boundaries
		//and should not alter the behavior of other events.
		if (entry->GetTrigger() == BIRTHS_INTERRUPT) {
			
			// Get the value of the appropriate trigger varile
			t_val = GetTriggerValue(entry->GetTrigger());
			
			if (t_val == entry->GetStart() ) {  //This event *must* happen at this value
				
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
					 (entry->GetStart() < entry->GetStop() && entry->GetInterval() < 0))){
					Delete(entry);
				} else {
					// We have to add this entry to the BirthInterrupt queue
					QueueBirthInterruptEvent(entry->GetStart());
				}
			}
		} 
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
      case BIRTHS:
        os << "births ";
        break;
			case BIRTHS_INTERRUPT:
					os << "birth_interrupt ";
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


// Dequeue a particular birth interrupt event
void cEventList::DequeueBirthInterruptEvent(double t_val)
{
	double* ptr = m_birth_interrupt_queue.Remove(&t_val);
	if (ptr != NULL) 
		delete ptr;
}


// Add a birth event trigger time to avoid unnecessary processing
void cEventList::QueueBirthInterruptEvent(double t_val)
{
	//See if the event is already queued; add if not
	if (m_birth_interrupt_queue.Find(&t_val) == NULL){
		double* val_ptr = new double(t_val);
		m_birth_interrupt_queue.PushRear(val_ptr);
	}
}

// Check to see whether or not a particular value is in the asynchronous
// birth queue.
bool cEventList::CheckBirthInterruptQueue(double t_val)
{
	return false;
	//Disabled for now...
	//return (m_birth_interrupt_queue.Find(&t_val) != NULL);
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
  } else if (cur_word == "b" || cur_word == "births") {
    trigger = BIRTHS;
    cur_word = cur_line.PopWord();
  } else if (cur_word == "o"  || cur_word == "org_id") {
    trigger = BIRTHS_INTERRUPT;
		cur_word = cur_line.PopWord();
  } else {
    cerr << "error: unrecognized event trigger '" << cur_word << "'" << endl;
    return false;
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
    
  } else { 
    cerr << "error: invalid event timing '" << tmp << "'" << endl;
    return false;
  }
  
  // Get the rest of the info
  name = cur_word;
  arg_list = cur_line;
  
  return AddEvent(trigger, start, interval, stop, name, arg_list);
}


/*! Check to see if an event with the given name is upcoming at some point in the future.
 */
bool cEventList::IsEventUpcoming(const cString& event_name) {
	cEventListEntry* entry = m_head;
  while(entry != 0) {
		if(entry->GetName() == event_name) {
			return true;
		}
		entry = entry->GetNext();
	}
	return false;
}
