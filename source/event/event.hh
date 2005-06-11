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
  cString m_name;
  cString m_args;

  int m_factory_id;

  // not implemented, prevents inadvertent wrong instantiation
  cEvent();
  cEvent( const cEvent & );
  cEvent& operator=( const cEvent& );

public:
  // constructors
  cEvent( const cString & name = "Undefined", const cString & args = "", int factory_id = -1 )
    : m_name( name ), m_args( args ), m_factory_id( factory_id ) { ; }
  virtual ~cEvent() { ; }


  // manipulators
  void SetFactoryId(int factory_id){ m_factory_id = factory_id; }

  /**
   * This is a pure virtual function that has to be overloaded by derived
   * classes. It does the actual 'thing' the event is supposed to do.
   **/
  virtual void Process() = 0;

  // accessors
  const cString &  GetName()       const { return m_name; }
  const cString &  GetArgs()       const { return m_args; }
  int              GetFactoryId()  const { return m_factory_id; }
};

#endif
