/*
 *  cHelpType.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cHelpType_h
#define cHelpType_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cHelpAlias;
class cHelpEntry;
class cHelpFullEntry;
class cHelpManager;

class cHelpType {
private:
  cString name;
  tList<cHelpEntry> entry_list;
  cHelpManager* manager;
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
