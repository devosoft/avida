//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HH
#define EVENT_HH

#ifndef STRING_HH
#include "string.hh"
#endif

/**
 * This is a virtual base class for an event in Avida.
 * It holds the name of the event, a string of arguments, and the id of
 * the factory (@ref cEventFactory) it is created from.
 **/

class cString; // aggregate

class cEvent {
public:
  enum eTriggerVariable { UPDATE, GENERATION, IMMEDIATE, UNDEFINED };

private:
  int m_factory_id;

  // not implemented, prevents inadvertent wrong instantiation
  cEvent(const cEvent&);
  cEvent& operator=(const cEvent&);

protected:
  cString m_args;

public:
  // constructors
  cEvent(int factory_id = -1 ) : m_factory_id(factory_id), m_args("") { ; }
  virtual ~cEvent() { ; }

  int GetFactoryId() const { return m_factory_id; }
  void SetFactoryId(int factory_id) { m_factory_id = factory_id; }
  
  const cString& GetArgs() const { return m_args; }

  /**
   * The following functions are pure virtual and must be supplied by implementations
   **/
  
  // Configures the event for use.
  virtual void Configure(const cString& args = "") = 0;

  // Does the actual 'thing' the event is supposed to do.
  virtual void Process() = 0;

  // Name and description of the event
  virtual const cString GetName() const = 0;
  virtual const cString GetDescription() const = 0;
};

#endif
