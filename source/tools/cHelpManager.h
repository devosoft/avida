/*
 *  cHelpManager.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cHelpManager_h
#define cHelpManager_h

#ifndef cHelpType_h
#include "cHelpType.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cHelpFullEntry;

class cHelpManager {
private:
  tList<cHelpType> type_list;
  cHelpFullEntry* last_entry;
  bool verbose;   // Should we print messages to the screen?

  // Private methods...
  cHelpType* GetType(const cString type_name);
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
      cHelpEntry* found_entry = type_it.Get()->FindEntry(entry_name);
      if (found_entry != NULL) return found_entry;
    }
    return NULL;
  }
  
  void SetVerbose(bool _verbose = true) { verbose = _verbose; }
  bool GetVerbose() const { return verbose; }


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif
