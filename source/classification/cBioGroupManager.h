/*
 *  cBioGroupManager.h
 *  Avida
 *
 *  Created by David on 11/5/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#ifndef cBioGroupManager_h
#define cBioGroupManager_h

#ifndef cString_h
#include "cString.h"
#endif

class cBioGroup;
class cBioUnit;
class cStats;
class cDataFile;
template <typename T> class tDictionary;


class cBioGroupManager
{
  friend class cClassificationManager;
private:
  int m_role_id;
  cString m_role;
  
  
public:
  cBioGroupManager() { ; }
  virtual ~cBioGroupManager() = 0;
  
  virtual cBioGroup* ClassifyNewBioUnit(cBioUnit* bu) = 0;
  virtual cBioGroup* GetBioGroup(int bg_id) = 0;
  
  virtual void UpdateReset() = 0;
  virtual void UpdateStats(cStats& stats) = 0;
    
  virtual cBioGroup* LoadBioGroup(const tDictionary<cString>& props) = 0;
  virtual void SaveBioGroups(cDataFile& df) = 0;

  inline int GetRoleID() const { return m_role_id; }
  inline const cString& GetRole() const { return m_role; }
  
  
private:
  void SetRole(int role_id, const cString& role) { m_role_id = role_id; m_role = role; }
};


#endif
