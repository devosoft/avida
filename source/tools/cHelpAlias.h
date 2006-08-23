/*
 *  cHelpAlias.h
 *  Avida
 *
 *  Called "help_alias.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cHelpAlias_h
#define cHelpAlias_h

#ifndef cHelpEntry_h
#include "cHelpEntry.h"
#endif

class cHelpFullEntry;
class cHelpType;
class cString;

class cHelpAlias : public cHelpEntry {
private:
  cHelpFullEntry * full_entry;
public:
  cHelpAlias(const cString & _alias, cHelpFullEntry * _full_entry)
    : cHelpEntry(_alias), full_entry(_full_entry) { ; }
  ~cHelpAlias() { ; }

  const cString & GetKeyword() const;
  const cString & GetDesc() const;
  cHelpType * GetType() const;

  bool IsAlias() const { return true; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nHelpAlias {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
