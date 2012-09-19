/*
 *  cResourceLib.h
 *  Avida
 *
 *  Called "resource_lib.hh" prior to 12/5/05.
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

#ifndef cResourceLib_h
#define cResourceLib_h

#include "avida/core/Types.h"

class cResource;
class cResourceHistory;
class cString;


class cResourceLib
{
private:
  Apto::Array<cResource*> m_resource_array;
  mutable cResourceHistory* m_initial_levels;
  int m_num_deme_resources;
  
  cResourceLib(const cResourceLib&); // @not_implemented
  cResourceLib& operator=(const cResourceLib&); // @not_implemented
  
public:
  cResourceLib() : m_initial_levels(NULL), m_num_deme_resources(0) { ; }
  ~cResourceLib();

  int GetSize() const { return m_resource_array.GetSize(); }

  cResource* AddResource(const cString& res_name);
  cResource* GetResource(const cString& res_name) const;
  inline cResource* GetResource(int id) const { return m_resource_array[id]; }
  const cResourceHistory& GetInitialResourceLevels() const;
  bool DoesResourceExist(const cString& res_name);
  void SetResourceIndex(cResource* res);
};

#endif
