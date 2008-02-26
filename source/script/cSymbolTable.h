/*
 *  cSymbolTable.h
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

#ifndef cSymbolTable_h
#define cSymbolTable_h

#include "AvidaScript.h"

#include "tArray.h"
#include "tDictionary.h"

// Active/inactive functions and variables within the scope of the current function (or globally)


class cSymbolTable
{
private:
  struct sSymbolEntry
  {
    cString name;
    ASType_t type;

    bool active;
    
    sSymbolEntry(const cString& in_name, ASType_t in_type) : name(in_name), type(in_type), active(true) { ; }
  };
  tArray<sSymbolEntry*> m_sym_tbl;
  tDictionary<int> m_sym_dict;
  
  struct sFunctionEntry
  {
    cString name;
    ASType_t type;
    
    bool active;
    
    sFunctionEntry(const cString& in_name, ASType_t in_type) : name(in_name), type(in_type), active(true) { ; }    
  };
  tArray<sFunctionEntry*> m_fun_tbl;
  tDictionary<int> m_fun_dict;
  
  
  cSymbolTable(const cSymbolTable&); // @not_implemented
  cSymbolTable& operator=(const cSymbolTable&); // @not_implemented
  
  
public:
  cSymbolTable() { ; }

  
  bool AddVariable(const cString& name, ASType_t type);
  bool AddFunction(const cString& name, ASType_t type);
  
  inline bool HasSymbol(const cString& name) const { return m_sym_dict.HasEntry(name) || m_fun_dict.HasEntry(name); }
};


#endif
