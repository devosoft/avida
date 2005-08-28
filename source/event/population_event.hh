//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_EVENT_HH
#define POPULATION_EVENT_HH

#ifndef EVENT_HH
#include "event.hh"
#endif

class cString;
class cPopulation;

class cPopulationEvent : public cEvent {
private:
  // not implemented. prevents inadvertend wrong instantiation.
  cPopulationEvent(const cPopulationEvent&);
  const cPopulationEvent& operator=(cPopulationEvent&);

protected:
  cPopulation *population;
  
public:
  cPopulationEvent() { ; }
  void SetPopulation(cPopulation* pop) { population = pop; }
};

#endif
