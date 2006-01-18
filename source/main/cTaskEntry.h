/*
 *  cTaskEntry.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cTaskEntry_h
#define cTaskEntry_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef cTaskLib_h
#include "cTaskLib.h"
#endif

class cTaskEntry
{
private:
  cString m_name;  // Short keyword for task
  cString m_desc;  // For more human-understandable output...
  int m_id;
  tTaskTest m_test_fun;

public:
  cTaskEntry(const cString& name, const cString& desc, int in_id, tTaskTest test_fun)
    : m_name(name), m_desc(desc), m_id(in_id), m_test_fun(test_fun)
  {
  }
  ~cTaskEntry() { ; }

  const cString& GetName() const { return m_name; }
  const cString& GetDesc() const { return m_desc; }
  const int GetID() const { return m_id; }
  const tTaskTest GetTestFun() const { return m_test_fun; }
};

#endif
