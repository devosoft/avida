/*
 *  cTaskEntry.h
 *  Avida
 *
 *  Called "task_entry.hh" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cTaskEntry_h
#define cTaskEntry_h

#ifndef cArgContainer_h
#include "cArgContainer.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef cTaskLib_h
#include "cTaskLib.h"
#endif

class cTaskLib;
class cTaskContext;

typedef double (cTaskLib::*tTaskTest)(cTaskContext&) const;

class cTaskEntry
{
private:
  cString m_name;  // Short keyword for task
  cString m_desc;  // For more human-understandable output...
  int m_id;
  tTaskTest m_test_fun;
  cArgContainer* m_args;

public:
  cTaskEntry(const cString& name, const cString& desc, int in_id, tTaskTest fun, cArgContainer* args)
    : m_name(name), m_desc(desc), m_id(in_id), m_test_fun(fun), m_args(args)
  {
  }
  ~cTaskEntry() { delete m_args; }

  const cString& GetName() const { return m_name; }
  const cString& GetDesc() const { return m_desc; }
  const int GetID() const { return m_id; }
  const tTaskTest GetTestFun() const { return m_test_fun; }
  
  bool HasArguments() const { return (m_args != NULL); }
  const cArgContainer& GetArguments() const { return *m_args; }
  
  
};


#ifdef ENABLE_UNIT_TESTS
namespace nTaskEntry {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
