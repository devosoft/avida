//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_EVENT_FACTORY_HH
#define POPULATION_EVENT_FACTORY_HH

#ifndef EVENT_FACTORY_HH
#include "event_factory.hh"
#endif

class cEvent;
class cPopulation;
class cString;

class cPopulationEventFactory : public cEventFactory {
private:
  cPopulation *m_population;
public:
  // event enums
#include "cPopulation_enums_auto.ci"

  cPopulationEventFactory( cPopulation *pop );
  ~cPopulationEventFactory();

  int EventNameToEnum(const cString & name) const;
  cEvent * ConstructEvent(int event_enum, const cString & args );
};

#endif
