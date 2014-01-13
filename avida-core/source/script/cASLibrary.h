/*
 *  cASLibrary.h
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 2006-2011 Michigan State University. All rights reserved.
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

#ifndef cASLibrary_h
#define cASLibrary_h

#include "cASFunction.h"


class cASLibrary
{
private:
  // --------  Internal Type Declarations  --------
  struct sObjectEntry;
  

  // --------  Internal Variables  --------
  Apto::Array<sObjectEntry*> m_obj_tbl;
  Apto::Map<Apto::String, int> m_obj_dict;
  Apto::Map<Apto::String, const cASFunction*> m_fun_dict;

  
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
