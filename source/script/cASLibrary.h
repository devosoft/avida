/*
 *  cASLibrary.h
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 2006-2010 Michigan State University. All rights reserved.
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

#ifndef cASLibrary_h
#define cASLibrary_h

#include "cASFunction.h"
#include "tDictionary.h"


class cASLibrary
{
private:
  // --------  Internal Type Declarations  --------
  struct sObjectEntry;
  

  // --------  Internal Variables  --------
  tArray<sObjectEntry*> m_obj_tbl;
  tDictionary<int> m_obj_dict;
  tDictionary<const cASFunction*> m_fun_dict;

  
  // --------  Private Constructors  --------
  cASLibrary(const cASLibrary&); // @not_implemented
  cASLibrary& operator=(const cASLibrary&); // @not_implemented
  
  
public:
  cASLibrary() { ; }
  ~cASLibrary();

  bool LookupObject(const cString& obj_name, int& obj_id);
  bool LookupFunction(const cString& name, const cASFunction*& func) { return m_fun_dict.Find(name, func); }
  
  bool HasFunction(const cString& name) const { return m_fun_dict.HasEntry(name); }

  bool RegisterFunction(const cASFunction* func);
  
  
private:
  // --------  Internal Type Definitions  --------
  struct sObjectEntry
  {
    cString name;
    
    sObjectEntry(const cString& in_name) : name(in_name) { ; }
  };
  
};

#endif
