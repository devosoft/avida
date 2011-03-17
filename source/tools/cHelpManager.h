/*
 *  cHelpManager.h
 *  Avida
 *
 *  Called "help_manager.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
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
};

#endif
