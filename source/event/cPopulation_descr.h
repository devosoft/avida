/*
 *  cPopulation_descr.h
 *  Avida2
 *
 *  Created by David on 6/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

class cEventEntry{
private:
  const char* m_name;
  const char* m_description;
public:
    cEventEntry( const char* name, const char* description )
    : m_name( name ), m_description( description ) {;}
  ~cEventEntry() {;}
  /**
    * @return The name of the event.
   **/
  const char* GetName() const { return m_name; }
  
  /**
    * @return The description of the event.
   **/
  const char* GetDescription() const { return m_description; }
};


class cEventDescrs{
public:
  const static int num_of_events;
  const static cEventEntry entries[];
};

