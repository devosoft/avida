/*
 *  cHelpEntry.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cHelpEntry_h
#define cHelpEntry_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef cStringUtil_h
#include "cStringUtil.h"
#endif

class cHelpType;

class cHelpEntry {
protected:
  cString name;
public:
  cHelpEntry(const cString & _name) : name(_name) { ; }
  virtual ~cHelpEntry() { ; }

  const cString & GetName() const { return name; }
  virtual const cString & GetKeyword() const = 0;
  virtual const cString & GetDesc() const = 0;
  virtual cHelpType * GetType() const = 0;

  virtual bool IsAlias() const = 0;

  cString GetHTMLFilename() const { return cStringUtil::Stringf("help.%s.html", static_cast<const char*>(GetKeyword())); }
};


#ifdef ENABLE_UNIT_TESTS
namespace nHelpEntry {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
