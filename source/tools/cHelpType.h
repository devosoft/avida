//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HELP_TYPE_HH
#define HELP_TYPE_HH

#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef TLIST_HH
#include "tList.h"
#endif

class cHelpAlias;
class cHelpEntry;
class cHelpFullEntry;
class cHelpManager;
class cString; // aggregate
template <class T> class tList; // aggregate

class cHelpType {
private:
  cString name;
  tList<cHelpEntry> entry_list;
  cHelpManager * manager;
  int num_entries;
private:
  // disabled copy constructor.
  cHelpType(const cHelpType &);
public:
  cHelpType(const cString & _name, cHelpManager * _manager)
    : name(_name), manager(_manager), num_entries(0) { ; }
  ~cHelpType();
  cHelpFullEntry * AddEntry(const cString & _name, const cString & _desc);
  cHelpAlias * AddAlias(const cString & alias_name, cHelpFullEntry * entry);
  const cString & GetName() const { return name; }
  cHelpEntry * FindEntry(const cString & entry_name);

  void PrintHTML();
};

#endif
