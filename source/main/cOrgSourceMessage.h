/*
 *  cOrgSourceMessage.h
 *  Avida
 *
 *  Created by David on 3/26/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cOrgSourceMessage_h
#define cOrgSourceMessage_h

class cOrgSourceMessage
{
private:
  int m_value;
  int m_sent;
  int m_dropped;
  
public:
  cOrgSourceMessage() : m_value(0), m_sent(0), m_dropped(0) { ; }
  cOrgSourceMessage(int value) : m_value(value), m_sent(1), m_dropped(0) { ; }

  int GetValue() { return m_value; }
  
  int GetSent() { return m_sent; }
  void SetSent() { m_sent++; }  
  int GetDropped() { return m_dropped; }
  void SetDropped() { m_dropped++; }
};

#endif
