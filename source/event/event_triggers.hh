//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_TRIGGERS_HH
#define EVENT_TRIGGERS_HH

/**
 * This class is used to physically separate the whole event list business
 * from the world an so on. You have to derive from this object and fill
 * in the purely virtual functions with the appropriate contents.
 **/

class cEventTriggers {
public:
  enum eTriggerVariable { UPDATE, GENERATION, IMMEDIATE, UNDEFINED };

  static const double TRIGGER_BEGIN;
  static const double TRIGGER_END;
  static const double TRIGGER_ALL;
  static const double TRIGGER_ONCE;

private:
  // not implemented. prevents inadvertend wrong instantiation.
  cEventTriggers( const cEventTriggers& );
  cEventTriggers& operator=( const cEventTriggers& );
public:
  cEventTriggers() { ; }
  virtual ~cEventTriggers() { ; }
  
  virtual double GetUpdate() const = 0;
  virtual double GetGeneration() const = 0;

  double GetTriggerValue( eTriggerVariable trigger ) const;
};

#endif
