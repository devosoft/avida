/*
 *  cMessageType.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cMessageType_h
#define cMessageType_h

class cMessageClass;

class cMessageType{
public:
  cMessageType(
    const char* type_name,
    cMessageClass& message_class
  );
public:
  const char* m_type_name;
  const cMessageClass& m_message_class;
  bool m_is_active;

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif
