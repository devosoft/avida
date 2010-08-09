/*
 *  cHelpFullEntry.h
 *  Avida
 *
 *  Called "help_full_type.hh" prior to 12/7/05.
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

#ifndef cHelpFullEntry_h
#define cHelpFullEntry_h

#ifndef cHelpEntry_h
#include "cHelpEntry.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cHelpType;

class cHelpFullEntry : public cHelpEntry {
private:
  cHelpType* type;
  cString desc;
public:
  cHelpFullEntry(const cString & _name, cHelpType * _type, const cString _desc)
    : cHelpEntry(_name), type(_type), desc(_desc) { ; }
  cHelpFullEntry() : cHelpEntry(""), type(NULL) { ; }
  ~cHelpFullEntry() { ; }

  const cHelpFullEntry & operator=(const cHelpEntry & in_entry) {
    name = in_entry.GetName();
    type = in_entry.GetType();
    desc = in_entry.GetDesc();
    return *this;
  }

  const cString & GetKeyword() const { return name; }
  const cString & GetDesc() const { return desc; }
  cHelpType* GetType() const { return type; }

  bool IsAlias() const { return false; }
};

#endif
