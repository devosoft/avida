/*
 *  cHelpType.h
 *  Avida
 *
 *  Called "help_type.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
