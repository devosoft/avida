/*
 *  cSymbolTable.cc
 *  Avida
 *
 *  Created by David on 2/2/06.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

bool cSymbolTable::AddVariable(const cString& name, ASType_t type)
{
  if (HasSymbol(name)) return false;

  m_sym_tbl.Push(new sSymbolEntry(name, type));
  m_sym_dict.Add(name, m_sym_tbl.GetSize() - 1);
  
  return true;
}

bool cSymbolTable::AddFunction(const cString& name, ASType_t type)
{
  if (HasSymbol(name)) return false;
  
  m_fun_tbl.Push(new sFunctionEntry(name, type));
  m_fun_dict.Add(name, m_fun_tbl.GetSize() - 1);
  
  return true;
}


