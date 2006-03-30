/*
 *  cOrgSeqMessage.h
 *  Avida
 *
 *  Created by David on 3/28/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cOrgSeqMessage_h
#define cOrgSeqMessage_h

class cOrgSeqMessage
{
private:
  int m_value;
  bool m_recvd;
  
public:
  cOrgSeqMessage() : m_value(0), m_recvd(false) { ; }

  void SetValue(int value) { m_value = value; }
  int GetValue() { return m_value; }

  void SetReceived() { m_recvd = true; }
  bool GetReceived() { return m_recvd; }
};

#endif
