/*
 *  cEventTriggers.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cEventTriggers_h
#define cEventTriggers_h

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


#ifdef ENABLE_UNIT_TESTS
namespace nEventTriggers {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
