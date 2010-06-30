/*
 *  cBGSpeciesManager.h
 *  Avida
 *
 *  Created by David on 12/22/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
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

#ifndef cBGSpeciesManager_h
#define cBGSpeciesManager_h

#ifndef defs_h
#include "defs.h"
#endif
#ifndef cBioGroupManager_h
#include "cBioGroupManager.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif


class cBGSpeciesManager: public cBioGroupManager
{
private:
  tArray<cString> m_bg_props;
  
  
public:
  cBGSpeciesManager();
  ~cBGSpeciesManager();
  
  // cBioGroupManager Interface Methods
  cBioGroup* ClassifyNewBioUnit(cBioUnit* bu);
  cBioGroup* GetBioUnit(int bg_id) { return NULL; }
  
  void UpdateReset();
  
  void SaveBioGroups(cDataFile& df);

  const tArray<cString>& GetBioGroupProperyList() const { return m_bg_props; }
};

#endif
