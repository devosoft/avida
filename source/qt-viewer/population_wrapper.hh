//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_WRAPPER_HH
#define POPULATION_WRAPPER_HH

#include <qobject.h>

class cPopulation;
class cEventList;

class
PopulationWrapper : public QObject {

  Q_OBJECT

private:
  cPopulation *m_population;
  cEventList *m_event_list;
public:
  PopulationWrapper(
    cPopulation *population,
    QObject *parent = 0,
    const char *name = 0
  )
  : m_population(population)
  { ; }
  cPopulation *getPopulation(void){ return m_population; }
  void setEventList(cEventList *event_list){ m_event_list = event_list; }
  cEventList *getEventList(void){ return m_event_list; }
};


#endif /* !POPULATION_WRAPPER_HH */

