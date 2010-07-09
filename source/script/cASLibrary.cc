/*
 *  cASLibrary.cc
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "cASLibrary.h"


cASLibrary::~cASLibrary()
{
  for (int i = 0; i < m_obj_tbl.GetSize(); i++) delete m_obj_tbl[i];

  tArray<const cASFunction*> fun_objs;
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
