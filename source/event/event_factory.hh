//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_FACTORY_HH
#define EVENT_FACTORY_HH

/**
 * This class is used to construct events from a given name and argument list.
 * You have to overload it to construct events that need particular additional
 * information (like a pointer to a class they should act on).
 **/

class cEvent;
class cString;

class cEventFactory {
private:
  int m_factory_id;

  // not implemented, prevents inadvertent wrong instantiation
  cEventFactory( const cEventFactory & );
  cEventFactory& operator=( const cEventFactory& );

public:
  cEventFactory();
  virtual ~cEventFactory();

  void SetFactoryId( int id ){
    m_factory_id = id; }

  int GetFactoryId(){
    return m_factory_id; }

  /**
   * Returns an id for an event name. The id holds only for this factory,
   * a second factory might use the same id for something else.
   **/
  virtual int EventNameToEnum(const cString & name) const = 0;

  /**
   * Constructs an event.
   *
   * @param event_enum The identifier of the event to be constructed.
   * @param args The argument list of the event.
   **/
  virtual cEvent * ConstructEvent( int event_enum, const cString & args )=0;

  /**
   * Constructs an event.
   *
   * @param name The name of the event to be constructed.
   * @param args The argument list of the event.
   **/
  cEvent * ConstructEvent( const cString & name, const cString & args ){
    return ConstructEvent( EventNameToEnum(name), args ); }
};

#endif
