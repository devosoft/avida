/*
 *  cSymbolTable.cc
 *  Avida
 *
 *  Created by David on 2/2/06.
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

#include "cSymbolTable.h"


cSymbolTable::~cSymbolTable()
{
  for (int i = 0; i < m_sym_tbl.GetSize(); i++) delete m_sym_tbl[i];
  for (int i = 0; i < m_fun_tbl.GetSize(); i++) delete m_fun_tbl[i];
}

bool cSymbolTable::AddVariable(const cString& name, const sASTypeInfo& type, int& var_id)
{
  bool found = LookupVariable(name, var_id);
  int shadow = var_id;
  if (found && m_sym_tbl[var_id]->scope == m_scope && !m_sym_tbl[var_id]->deactivate) return false;
  
  var_id = m_sym_tbl.GetSize();
  m_sym_tbl.Push(new sSymbolEntry(name, type, m_scope));
  
  if (found) {
    m_sym_dict.SetValue(name, var_id);
    m_sym_tbl[var_id]->shadow = shadow;
  } else {
    m_sym_dict.Add(name, var_id);
  }
  
  return true;
}

bool cSymbolTable::AddFunction(const cString& name, const sASTypeInfo& type, int& fun_id)
{
  int shadow = fun_id;
  bool found = LookupFunction(name, fun_id);
  if (found && m_fun_tbl[fun_id]->scope == m_scope && !m_fun_tbl[fun_id]->deactivate) return false;
  
  fun_id = m_fun_tbl.GetSize();
  m_fun_tbl.Push(new sFunctionEntry(name, type, m_scope));
  
  if (found) {
    m_fun_dict.SetValue(name, fun_id);
    m_fun_tbl[fun_id]->shadow = shadow;
  } else {
    m_fun_dict.Add(name, fun_id);
  }
  
  return true;
}

void cSymbolTable::PopScope()
{
  m_deactivate_cycle++;
  
  for (int i = 0; i < m_sym_tbl.GetSize(); i++) {
    sSymbolEntry* se = m_sym_tbl[i];
    if (se->scope == m_scope && !se->deactivate) {
      if (se->shadow == -1) m_sym_dict.Remove(se->name);
      else m_sym_dict.SetValue(se->name, se->shadow);
      se->deactivate = m_deactivate_cycle;
    }
  }
  
  for (int i = 0; i < m_fun_tbl.GetSize(); i++) {
    sFunctionEntry* fe = m_fun_tbl[i];
    if (fe->scope == m_scope && !fe->deactivate) {
      if (fe->shadow == -1) m_fun_dict.Remove(fe->name);
      else m_fun_dict.SetValue(fe->name, fe->shadow);
      fe->deactivate = m_deactivate_cycle;
    }
  }

  m_return = false;
  m_scope--;
}

