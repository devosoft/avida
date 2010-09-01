/*
 *  cResourceLib.h
 *  Avida
 *
 *  Called "resource_lib.hh" prior to 12/5/05.
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

#ifndef cResourceLib_h
#define cResourceLib_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cResource;
class cResourceHistory;
class cString;

class cResourceLib
{
private:
  tArray<cResource*> m_resource_array;
  mutable cResourceHistory* m_initial_levels;
  
  cResourceLib(const cResourceLib&); // @not_implemented
  cResourceLib& operator=(const cResourceLib&); // @not_implemented
  
public:
  cResourceLib() : m_initial_levels(NULL) { ; }
  ~cResourceLib();

  int GetSize() const { return m_resource_array.GetSize(); }

  cResource* AddResource(const cString& res_name);
  cResource* GetResource(const cString& res_name) const;
  inline cResource* GetResource(int id) const { return m_resource_array[id]; }
  const cResourceHistory& GetInitialResourceLevels() const;
  bool DoesResourceExist(const cString& res_name);
};

#endif
