/*
 *  cEventList.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cEventList_h
#define cEventList_h

#ifndef cEventListEntry_h
#include "cEventListEntry.h"
#endif
#ifndef cEventListIterator_h
#include "cEventListIterator.h"
#endif
#ifndef cEventTriggers_h
#include "cEventTriggers.h"
#endif

/**
 * This is the fundamental class for event management. It holds a list of all
 * events, and provides methods to add new events and to process existing
 * events.
 **/

class cEvent;
class cEventManager;
class cString;

class cEventList {
friend class cEventListIterator;
public:
  cEventListIterator begin() { return cEventListIterator(m_head); }
  const cEventListIterator end() const { return cEventListIterator(0); }
protected:
  cEventManager *m_factory_manager;
  cEventTriggers *m_triggers;

  cEventListEntry *m_head;
  cEventListEntry *m_tail;
  cEventListEntry *m_current;
  int m_num_events;

  void InsertEvent(cEvent *event, cEventTriggers::eTriggerVariable trigger,
		   double start, double interval, double stop);
  void Delete( cEventListEntry *entry );
  void Delete(){ Delete( m_current); }

  double GetTriggerValue( cEventTriggers::eTriggerVariable trigger);

private:
  // not implemented, prevents inadvertent wrong instantiation
  cEventList();
  cEventList( const cEventList& );
  cEventList& operator=( const cEventList& );

public:
  // creators
  /**
   * The cEventList assumes ownership of both objects it is handed, and
   * destroys them when it is done.
   *
   * @param factory_manager A pointer to an event factory manager,
   * @ref cEventFactoryManager. The event factory manager is responsible
   * for finding the appropriate event factory to construct a given event.
   *
   * @param triggers A trigger object. The event list needs a trigger object
   * to determine what events to call when.
   **/
  cEventList( cEventManager *factory_manager, cEventTriggers *triggers );
  ~cEventList();

  // manipulators
  /**
   * Adds an event with given name and argument list. The event will be of
   * type immediate, i.e. it is processed only once, and then deleted.
   *
   * @param name The name of the event.
   * @param args The argument list.
   **/
  bool AddEvent( const cString & name, const cString & args ){
    return AddEvent( cEventTriggers::IMMEDIATE, cEventTriggers::TRIGGER_BEGIN, cEventTriggers::TRIGGER_ONCE, cEventTriggers::TRIGGER_END, name, args); }


  /**
   * Adds an event with specified trigger type.
   *
   * @param trigger The type of the trigger.
   * @param start The start value of the trigger variable.
   * @param interval The length of the interval between one processing
   * and the next.
   * @param stop The value of the trigger variable at which the event should
   * be deleted.
   * @param name The name of the even.
   * @param args The argument list.
   **/
  bool AddEvent( cEventTriggers::eTriggerVariable trigger,
		 double start, double interval, double stop,
		 const cString &name, const cString &args );

  /**
   * This function adds an event that is given in the event list file format.
   * In other words, it can be used to parse one line from an event list file,
   * and construct the appropriate event.
   **/
  bool AddEventFileFormat(const cString & line);

  /**
   * Delete all events in list; leave list ready for insertion of new
   * events.
   **/
  void DeleteAll(void);

  /**
   * Go through list executing appropriate events.
   **/
  void Process();

  /**
   * Get all events caught up.
   **/
  void Sync();

  /**
   * Get this event caught up.
   **/
  void SyncEvent(cEventListEntry *event);

  void PrintEventList(std::ostream& os = std::cout);

  // kaben changed this member function to static for easy access by
  // cEventListIterator.
  static void PrintEvent(cEventListEntry * event, std::ostream& os = std::cout);
};

#endif
