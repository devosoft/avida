//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_EVENT_FACTORY_HH
#define POPULATION_EVENT_FACTORY_HH

#ifndef TOBJECTFACTORY_H
#include "tObjectFactory.h"
#endif

#ifndef POPULATION_EVENT_HH
#include "cPopulationEvent.h"
#endif

class cPopulation;
class cString;

class cPopulationEventFactory : public tObjectFactory<cEvent ()> {
private:
  cPopulation *m_population;
public:  
  cPopulationEventFactory(cPopulation* pop);
  ~cPopulationEventFactory() { ; }

  cEvent* Create(const cString& key)
  {
    cPopulationEvent* event = static_cast<cPopulationEvent*>(this->tObjectFactory<cEvent ()>::Create(key));
    if( event != NULL ){
      event->SetFactoryId( GetFactoryId() );
      event->SetPopulation( m_population );
    }
    return event;
  }
};

#endif
