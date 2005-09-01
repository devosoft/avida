
#include "cEventFactoryManager.h"

#ifndef STRING_HH
#include "string.hh"
#endif

#ifndef EVENT_HH
#include "cEvent.h"
#endif

#include <iostream>

using namespace std;


/////////////////
//  cEventFactoryManager
/////////////////

cEventFactoryManager::cEventFactoryManager()
{
}

cEventFactoryManager::~cEventFactoryManager(){
  tListIterator<tObjectFactory<cEvent ()> > it(m_factory_list);  
  while (it.Next() != NULL) delete it.Get();
}


cEvent*
cEventFactoryManager::ConstructEvent(const cString name,
                                     const cString & args,
                                     int factory_id){
  cEvent* event = NULL;
  
  // factory_id < 0 => send to all factories
  if( factory_id < 0 ) {
    tListIterator<tObjectFactory<cEvent ()> > it(m_factory_list);
    while (it.Next() != NULL) {
      event = (it.Get())->Create(name);
      
      // if we have found one factory that can create the event we want we stop.
      if (event != NULL) break;
    }
  }
  else{
    // send to particular factory
    if (factory_id >= m_factory_list.GetSize()) return NULL;
    if( m_factory_list.GetPos(factory_id) != NULL )
      event = m_factory_list.GetPos(factory_id)->Create(name);
  }
  
  event->Configure(args);
  return event;
}


int cEventFactoryManager::AddFactory(tObjectFactory<cEvent ()>* factory)
{
  assert( factory != NULL );
  m_factory_list.Push(factory);
  
  int id = m_factory_list.GetSize();
  factory->SetFactoryId(id);
  
  return id;
}

void cEventFactoryManager::PrintAllEventDescriptions()
{
  tListIterator<tObjectFactory<cEvent ()> > it(m_factory_list);
  while (it.Next() != NULL) {
    tList<cEvent> events;
    it.Get()->CreateAll(events);
    
    tListIterator<cEvent> events_it(events);
    while (events_it.Next() != NULL) {
      cout << events_it.Get()->GetDescription() << endl;
      delete events_it.Get();
    }
  }
}