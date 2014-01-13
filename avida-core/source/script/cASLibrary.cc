/*
 *  cASLibrary.cc
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#include "cASLibrary.h"


cASLibrary::~cASLibrary()
{
  for (int i = 0; i < m_obj_tbl.GetSize(); i++) delete m_obj_tbl[i];

  Apto::Array<const cASFunction*> fun_objs;
  m_fun_dict.GetValues(fun_objs);
  for (int i = 0; i < fun_objs.GetSize(); i++) delete fun_objs[i];
}


bool cASLibrary::RegisterFunction(const cASFunction* func)
{
  const cASFunction* old_func = NULL;
  bool found = m_fun_dict.Find(func->GetName(), old_func);
  
  if (found) {
    return false;
  } else {
    m_fun_dict.Add(func->GetName(), func);
    return true;
  }
}
