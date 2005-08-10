
#include "event_factory_manager.hh"

#ifndef STRING_HH
#include "string.hh"
#endif

#ifndef EVENT_HH
#include "event.hh"
#endif

using namespace std;


/////////////////
//  cEventFactoryManager
/////////////////

cEventFactoryManager::cEventFactoryManager()
{
}

cEventFactoryManager::~cEventFactoryManager(){
  vector<tObjectFactory<cEvent ()>*>::iterator it = m_factory_list.begin();
  
  for( ; it != m_factory_list.end(); it++ )
    delete *it;
}


cEvent*
cEventFactoryManager::ConstructEvent(const cString name,
                                     const cString & args,
                                     int factory_id){
  cEvent* event = NULL;
  
  // factory_id < 0 => send to all factories
  if( factory_id < 0 ){
    vector<tObjectFactory<cEvent ()>*>::iterator it;
    for( it = m_factory_list.begin(); it != m_factory_list.end(); it++ ){
      if( *it != NULL )
        event = (*it)->Create(name);
      if ( event != NULL ) // if we have found one factory that can create the
                           //                    event we want we stop.
        break;
    }
  }
  else{
    // send to particular factory
    if ( factory_id >= static_cast<int>( m_factory_list.size() ) )
      return NULL;
    if( m_factory_list[factory_id] != NULL )
      event = m_factory_list[factory_id]->Create(name);
  }
  
  event->Configure(args);
  return event;
}


int cEventFactoryManager::AddFactory(tObjectFactory<cEvent ()>* factory)
{
  assert( factory != NULL );
  m_factory_list.push_back(factory);
  
  int id = m_factory_list.size();
  factory->SetFactoryId(id);
  
  return id;
}

