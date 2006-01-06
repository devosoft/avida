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

class cTaskEntry {
private:
  cString name;  // Short keyword for task
  cString desc;  // For more human-understandable output...
  int id;
  tTaskTest test_fun;
public:
  cTaskEntry(const cString & _name, const cString & _desc, int _id, tTaskTest _test_fun)
    : name(_name), desc(_desc), id(_id), test_fun(_test_fun)
  {
  }
  ~cTaskEntry() { ; }

  const cString & GetName()    const { return name; }
  const cString & GetDesc() const { return desc; }
  const int       GetID()      const { return id; }
  const tTaskTest GetTestFun() const { return test_fun; }
};

#endif
