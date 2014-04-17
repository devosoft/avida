/*
 *  core/EventList.h
 *  avida-core
 *
 *  Called "event_list.hh" prior to 12/2/05 and "cEventList.h" prior to 4/17/14.
 *  Copyright 1999-2014 Michigan State University. All rights reserved.
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
 *  Authors: Charles Ofria <ofria@msu.edu>, David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaCoreEventList_h
#define AvidaCoreEventList_h

#include "avida/core/Types.h"

#include "avida/util/ConfigFile.h"


namespace Avida {
  
  // EventAction
  // --------------------------------------------------------------------------------------------------------------
  
  class EventAction
  {
  public:
    LIB_EXPORT virtual ~EventAction() = 0;
    
    LIB_EXPORT virtual void Process(Context& ctx) = 0;
  };
  
  
  
  // EventList - Holds scheduled events and triggers them based on conditions
  // --------------------------------------------------------------------------------------------------------------
  
  class EventList
  {
  public:
    static const double TRIGGER_BEGIN;
    static const double TRIGGER_END;
    static const double TRIGGER_ALL;
    static const double TRIGGER_ONCE;
    
  private:
    class Entry;
    
  private:
    Universe* m_universe;
    Entry* m_head;
    Entry* m_tail;
    int m_num_events;
    

    EventList(); // @not_implemented
    EventList(const EventList&); // @not_implemented
    EventList& operator=(const EventList&); // @not_implemented
    
  public:
    LIB_EXPORT EventList(Universe* universe) : m_universe(universe), m_head(NULL), m_tail(NULL), m_num_events(0) { ; }
    LIB_EXPORT ~EventList();
    
    
    LIB_EXPORT bool AddEvent(Context& ctx, Update current_update, EventTriggerFunctor trigger, double start, double interval,
                             double stop, EventAction* action);
    
    LIB_EXPORT bool LoadEventFile(Context& ctx, Update current_update, Util::ConfigFile& file, Feedback& feedback);
    LIB_EXPORT bool LoadEventLine(Context& ctx, Update current_update, const Apto::String& line, Feedback& feedback);
    
    LIB_EXPORT void Process(Context& ctx, Update current_update);
    
  private:
    LIB_LOCAL void syncEvent(Context& ctx, Update current_update, Entry* event);
    LIB_LOCAL void deleteEvent(Entry* entry);

  private:
    class Entry
    {
    private:
      EventAction* m_action;
      
      EventTriggerFunctor m_trigger;
      double m_start;
      double m_interval;
      double m_stop;
      double m_original_start;
      
      Entry* m_prev;
      Entry* m_next;
      
    public:
      LIB_LOCAL inline Entry(EventAction* action, EventTriggerFunctor trigger, double start = TRIGGER_BEGIN,
                             double interval = TRIGGER_ONCE, double stop = TRIGGER_END, Entry* prev = NULL,Entry* next = NULL)
        : m_action(action), m_trigger(trigger), m_start(start), m_interval(interval), m_stop(stop), m_original_start(start)
        , m_prev(prev), m_next(next)
      {
      }
      
      LIB_LOCAL inline ~Entry() { delete m_action; }
      
      LIB_LOCAL inline EventAction& Action() { return *m_action; }
      LIB_LOCAL inline double TriggerValue(Universe* uni, Context& ctx, Update up) { return m_trigger(uni, ctx, up); }
      
      LIB_LOCAL inline double StartValue() const { return m_start; }
      LIB_LOCAL inline double Interval() const { return m_interval; }
      LIB_LOCAL inline double StopValue() const { return m_stop; }
      
      
      LIB_LOCAL inline void NextInterval() { m_start += m_interval; }
      LIB_LOCAL inline void Reset() { m_start = m_original_start; }
      

      LIB_LOCAL inline Entry* Prev() const { return m_prev; }
      LIB_LOCAL inline Entry* Next() const { return m_next; }
      LIB_LOCAL inline void SetPrev(Entry* prev) { m_prev = prev; }
      LIB_LOCAL inline void SetNext(Entry* next) { m_next = next; }
    };
    
  };
  
};

#endif
