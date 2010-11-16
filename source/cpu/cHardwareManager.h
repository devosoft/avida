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
class cGenome;
class cOrganism;
class cStringList;
class cUserFeedback;
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
  
  bool LoadInstSets(cUserFeedback* feedback = NULL);
  bool ConvertLegacyInstSetFile(cString filename, cStringList& str_list, cUserFeedback* feedback = NULL);
  
  cHardwareBase* Create(cAvidaContext& ctx, cOrganism* org, const cGenome& mg);
  inline cTestCPU* CreateTestCPU() { return new cTestCPU(m_world); }

  inline bool IsInstSet(const cString& name) const { return m_is_name_map.HasEntry(name); }
  
  inline const cInstSet& GetInstSet(const cString& name) const;
  inline cInstSet& GetInstSet(const cString& name);
  const cInstSet& GetInstSet(int i) const { return *m_inst_sets[i]; }
  
  const cInstSet& GetDefaultInstSet() const { return *m_inst_sets[0]; }
  
  int GetNumInstSets() const { return m_inst_sets.GetSize(); }
  
  bool RegisterInstSet(const cString& name, cInstSet* inst_set);
  
private:
  bool loadInstSet(int hw_type, const cString& name, cStringList& sl, cUserFeedback* feedback);
};


inline const cInstSet& cHardwareManager::GetInstSet(const cString& name) const
{
  return (name == "(default)") ? *m_inst_sets[0] : *m_inst_sets[m_is_name_map.Get(name)];
}

inline cInstSet& cHardwareManager::GetInstSet(const cString& name)
{
  return (name == "(default)") ? *m_inst_sets[0] : *m_inst_sets[m_is_name_map.Get(name)];
}

#endif
