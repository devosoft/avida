/*
 *  cTaskEntry.h
 *  Avida
 *
 *  Called "task_entry.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#ifndef cTaskEntry_h
#define cTaskEntry_h

#include "apto/core.h"

#include "cArgContainer.h"
#include "cString.h"

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
  Apto::String m_prop_id_ave;
  Apto::String m_prop_id_count;

public:
  cTaskEntry(const cString& name, const cString& desc, int in_id, tTaskTest fun, cArgContainer* args)
    : m_name(name), m_desc(desc), m_id(in_id), m_test_fun(fun), m_args(args)
  {
    m_prop_id_ave = Apto::FormatStr("environment.triggers.%s.average", (const char*)name);
    m_prop_id_count = Apto::FormatStr("environment.triggers.%s.count", (const char*)name);
  }
  ~cTaskEntry()
  {
    delete m_args;
  }

  const cString& GetName() const { return m_name; }
  const cString& GetDesc() const { return m_desc; }
  int GetID() const { return m_id; }
  tTaskTest GetTestFun() const { return m_test_fun; }
  
  const Apto::String& AveragePropertyID() const { return m_prop_id_ave; }
  const Apto::String& CountPropertyID() const { return m_prop_id_count; }
  
  
  bool HasArguments() const { return (m_args != NULL); }
  cArgContainer& GetArguments() const { return *m_args; }
};

#endif
