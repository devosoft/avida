/*
 *  cTaskEntry.h
 *  Avida
 *
 *  Called "task_entry.hh" prior to 12/5/05.
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

#ifndef cTaskEntry_h
#define cTaskEntry_h

#ifndef cArgContainer_h
#include "cArgContainer.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cTaskLib;
class cTaskContext;

typedef double (cTaskLib::*tTaskTest)(cTaskContext&) const;


class cTaskEntry
{
private:
  cString m_name;  // Short keyword for task
  cString m_desc;  // For more human-understandable output...
  int m_id;
  tTaskTest m_test_fun;
  cArgContainer* m_args;

public:
  cTaskEntry(const cString& name, const cString& desc, int in_id, tTaskTest fun, cArgContainer* args)
    : m_name(name), m_desc(desc), m_id(in_id), m_test_fun(fun), m_args(args)
  {
  }
  ~cTaskEntry()
  {
    delete m_args;
  }

  const cString& GetName() const { return m_name; }
  const cString& GetDesc() const { return m_desc; }
  int GetID() const { return m_id; }
  tTaskTest GetTestFun() const { return m_test_fun; }
  
  bool HasArguments() const { return (m_args != NULL); }
  cArgContainer& GetArguments() const { return *m_args; }
};

#endif
