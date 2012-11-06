/*
 *  cEventList.h
 *  Avida
 *
 *  Called "event_list.hh" prior to 12/2/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
 */

#ifndef cEventList_h
#define cEventList_h

#ifndef cAction_h
#include "cAction.h"
#endif

#include "tList.h"


namespace Avida {
  class Feedback;
};

class cAvidaContext;
class cString;
class cWorld;

using namespace Avida;


// This is the fundamental class for event management. It holds a list of all
// events, and provides methods to add new events and to process existing
// events.

class cEventList
{
public:
  
  // Event Trigger Type ====================================================================
  //  UPDATE occurs at the end of an update
  //  GENERATION occurs at the end of an update for particular values of average generation, 
  //             as evaluated at an update boundry
  //  IMMEDIATE occurs at once
  //  BIRTHS is triggered by values of tot_creatures (total creatures ever) in the stats object
  //         as evaluated at an update boundary
  //  UNDEFINED is undefined
  //  BIRTH_INTERRUPT is triggered by tot_creatures values outside of update boundaries.
  //                  Some statistical information gathered at the end of an update is not
  //                  available or is incomplete with this option.
  enum eTriggerType { UPDATE, GENERATION, IMMEDIATE, BIRTHS, UNDEFINED, BIRTHS_INTERRUPT };
  
  static const double TRIGGER_BEGIN;  //Are these unsafely defined? @MRR
  static const double TRIGGER_END;
  static const double TRIGGER_ALL;
  static const double TRIGGER_ONCE;
  
private:
  class cEventListEntry;  
  
private:
  cWorld* m_world;
  cEventListEntry* m_head;
  cEventListEntry* m_tail;
  int m_num_events;
  
  tList<double> m_birth_interrupt_queue;
  
  void QueueBirthInterruptEvent(double t_val);
  void DequeueBirthInterruptEvent(double t_val);
  
  void SyncEvent(cEventListEntry* event);
  double GetTriggerValue(eTriggerType trigger) const;
  void Delete(cEventListEntry* entry);
  
  cEventList(); // @not_implemented
  cEventList(const cEventList&); // @not_implemented
  cEventList& operator=(const cEventList&); // @not_implemented
  
  
public:
  cEventList(cWorld* world) : m_world(world), m_head(NULL), m_tail(NULL), m_num_events(0) { ; }
  ~cEventList();
  
  
  bool AddEvent(eTriggerType trigger, double start, double interval, double stop, const cString &name, const cString& args,
                Feedback& feedback);
  
  /**
   * This function adds an event that is given in the event list file format.
   * In other words, it can be used to parse one line from an event list file,
   * and construct the appropriate event.
   **/
  bool AddEventFileFormat(const cString& line, Feedback& feedback);
  
  
  bool LoadEventFile(const cString& filename, const cString& working_dir, Feedback& feedback, const Apto::Map<Apto::String, Apto::String>* defs = NULL);
  
  void Process(cAvidaContext& ctx);
  void Sync(); // Get all events caught up.
  
  void PrintEventList(std::ostream& os = std::cout);
  
  /**
   * Returns true if a particular org_id (or Stats::tot_creature) value is present
   * in the interrupt queue.
   *
   * @param t_value The value being checked.
   **/
  bool CheckBirthInterruptQueue(double t_val);
  
  
  /**
   * This function is called to process an event outside of an update boundary.
   * Some data may be missing, inaccurate, or incomplete if processing is required
   * at the end of an update.
   **/
  void ProcessInterrupt(cAvidaContext& ctx);
	
	//! Check to see if an event with the given name is upcoming at some point in the future.
	bool IsEventUpcoming(const cString& event_name);
  
  
private:
  class cEventListEntry
  {
  private:
    cAction* m_action;
    cString m_name;
    
    eTriggerType m_trigger;
    double m_start;
    double m_interval;
    double m_stop;
    double m_original_start;
    
    cEventListEntry* m_prev;
    cEventListEntry* m_next;
    
  public:
    cEventListEntry(cAction* action, const cString& name, eTriggerType trigger = UPDATE, double start = TRIGGER_BEGIN,
                    double interval = TRIGGER_ONCE, double stop = TRIGGER_END, cEventListEntry* prev = NULL,
                    cEventListEntry* next = NULL)
    : m_action(action), m_name(name), m_trigger(trigger), m_start(start), m_interval(interval), m_stop(stop)
    , m_original_start(start), m_prev(prev), m_next(next)
    {
    }
    
    virtual ~cEventListEntry() { delete m_action; }
    
    void SetPrev(cEventListEntry* prev) { m_prev = prev; }
    void SetNext(cEventListEntry* next) { m_next = next; }
    
    void NextInterval(){ m_start += m_interval; }
    void Reset() { m_start = m_original_start; }
    
    // accessors
    cAction* GetAction() const { assert(m_action != NULL); return m_action; }
    
    const cString GetName() const { assert(m_action != NULL); return m_name; }
    const cString& GetArgs() const { assert(m_action != NULL); return m_action->GetArgs(); }
    
    eTriggerType GetTrigger() const { return m_trigger; }
    double GetStart() const { return m_start; }
    double GetInterval() const { return m_interval; }
    double GetStop() const { return m_stop; }
    
    cEventListEntry* GetPrev() const { return m_prev; }
    cEventListEntry* GetNext() const { return m_next; }
  };
  
};

#endif
