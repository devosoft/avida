

#include "event.hh"


using namespace std;


cEvent::cEvent( const cString & name, const cString & args ,
		int factory_id )
  : m_name( name ), m_args( args ), m_factory_id( factory_id )
{
}


cEvent::~cEvent()
{
}
