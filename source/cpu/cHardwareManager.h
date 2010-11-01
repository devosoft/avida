/*
 *  cHardwareManager.h
 *  Avida
 *
 *  Created by David on 10/18/05.
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

#ifndef cHardwareManager_h
#define cHardwareManager_h

#include "cTestCPU.h"
#include "tDictionary.h"

class cAvidaContext;
class cHardwareBase;
class cInstSet;
class cMetaGenome;
class cOrganism;
class cStringList;
class cWorld;
template<typename T> class tList;


class cHardwareManager
{
private:
  cWorld* m_world;
  tArray<cInstSet*> m_inst_sets;
  tDictionary<int> m_is_name_map;
  int m_cpu_count;
  
  
  cHardwareManager(); // @not_implemented
  cHardwareManager(const cHardwareManager&); // @not_implemented
  cHardwareManager& operator=(const cHardwareManager&); // @not_implemented
  

public:
  cHardwareManager(cWorld* world);
  ~cHardwareManager();
  
  bool LoadInstSets(tList<cString>* errors = NULL);
  bool ConvertLegacyInstSetFile(cString filename, cStringList& str_list, tList<cString>* errors = NULL);
  
  cHardwareBase* Create(cAvidaContext& ctx, cOrganism* org, const cMetaGenome& mg);
  inline cTestCPU* CreateTestCPU() { return new cTestCPU(m_world); }

  const cInstSet& GetInstSet(const cString& name) const { return *m_inst_sets[m_is_name_map.Get(name)]; }
  cInstSet& GetInstSet(const cString& name) { return *m_inst_sets[m_is_name_map.Get(name)]; }
  
  const cInstSet& GetDefaultInstSet() const { return *m_inst_sets[0]; }
  
  int GetNumInstSets() const { return m_inst_sets.GetSize(); }
  
private:
  bool loadInstSet(int hw_type, const cString& name, cStringList& sl, tList<cString>* errors);
};

#endif
