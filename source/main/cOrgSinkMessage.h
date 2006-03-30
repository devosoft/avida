/*
 *  cOrgSinkMessage.h
 *  Avida
 *
 *  Created by David on 3/26/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cOrgSinkMessage_h
#define cOrgSinkMessage_h

class cOrgSinkMessage
{
private:
  int m_source_id;
  int m_orig_val;
  int m_actual_val;
  bool m_validated;
  
public:
  cOrgSinkMessage() : m_source_id(-1), m_orig_val(0), m_actual_val(0), m_validated(false) { ; }
  cOrgSinkMessage(int source, int original, int actual)
    : m_source_id(source), m_orig_val(original), m_actual_val(actual), m_validated(false) { ; }
  
  int GetSourceID() { return m_source_id; }
  int GetOriginalValue() { return m_orig_val; }
  int GetActualValue() { return m_actual_val; }
  
  void SetValidated() { m_validated = true; }
  bool GetValidated() { return m_validated; }
};

#endif
