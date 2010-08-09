/*
 *  cHelpEntry.h
 *  Avida
 *
 *  Called "help_entry.hh" prior to 12/7/05.
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

#endif
