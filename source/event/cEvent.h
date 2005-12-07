/*
 *  cEvent.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cEvent_h
#define cEvent_h

#ifndef cString_h
#include "cString.h"
#endif

/**
 * This is a virtual base class for an event in Avida.
 * It holds the name of the event, a string of arguments
 **/

class cWorld;

class cEvent {
public:
  enum eTriggerVariable { UPDATE, GENERATION, IMMEDIATE, UNDEFINED };

private:

  // not implemented, prevents inadvertent wrong instantiation
  cEvent(const cEvent&);
  cEvent& operator=(const cEvent&);

protected:
  cWorld* m_world;
  cString m_args;

public:
  cEvent() : m_world(NULL), m_args("") { ; }
  virtual ~cEvent() { ; }

  const cString& GetArgs() const { return m_args; }

  /**
   * The following functions are pure virtual and must be supplied by implementations
   **/
  
  // Configures the event for use.
  virtual void Configure(cWorld* world, const cString& args = "") = 0;

  // Does the actual 'thing' the event is supposed to do.
  virtual void Process() = 0;

  // Name and description of the event
  virtual const cString GetName() const = 0;
  virtual const cString GetDescription() const = 0;
};

#endif
