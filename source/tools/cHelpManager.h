//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HELP_MANAGER_HH
#define HELP_MANAGER_HH

#ifndef HELP_TYPE_HH
#include "cHelpType.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef TLIST_HH
#include "tList.h"
#endif

class cHelpFullEntry;
class cHelpType; // accessed
class cString; // aggregate
template <class T> class tList; // aggregate
template <class T> class tListIterator; // aggregate

class cHelpManager {
private:
  tList<cHelpType> type_list;
  cHelpFullEntry * last_entry;
  bool verbose;   // Should we print messages to the screen?

  // Private methods...
  cHelpType * GetType(const cString type_name);
private:
  // disabled copy constructor.
  cHelpManager(const cHelpManager &);
public:
  cHelpManager() : last_entry(NULL), verbose (false) { ; }
  ~cHelpManager() { while (type_list.GetSize() > 0) delete type_list.Pop(); }

  void LoadFile(const cString & filename);
  void PrintHTML();
  cHelpEntry * FindEntry(cString entry_name) {
    entry_name.ToLower();

    tListIterator<cHelpType> type_it(type_list);
    while (type_it.Next() != NULL) {
      cHelpEntry * found_entry = type_it.Get()->FindEntry(entry_name);
      if (found_entry != NULL) return found_entry;
    }
    return NULL;
  }
  
  void SetVerbose(bool _verbose = true) { verbose = _verbose; }
  bool GetVerbose() const { return verbose; }
};

#endif
