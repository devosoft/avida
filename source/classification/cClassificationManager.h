/*
 *  cClassificationManager.h
 *  Avida
 *
 *  Created by David on 11/14/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cClassificationManager_h
#define cClassificationManager_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tArrayMap_h
#include "tArrayMap.h"
#endif

class cBioGroupManager;
class cBioUnit;
class cDataFile;
class cStats;
class cWorld;


class cClassificationManager
{
private:
  cWorld* m_world;
  
  tArray<cBioGroupManager*> m_bgms;
  
  
  cClassificationManager(); // @not_implemented
  cClassificationManager(const cClassificationManager&); // @not_implemented
  cClassificationManager& operator=(const cClassificationManager&); // @not_implemented
  

public:
  cClassificationManager(cWorld* world);
  ~cClassificationManager();

  void UpdateReset();
  void UpdateStats(cStats& stats);
  
  bool RegisterBioGroupManager(cBioGroupManager* bgm, const cString& role, const tArray<cString>* dependencies = NULL);
  cBioGroupManager* GetBioGroupManager(const cString& role);
  
  void ClassifyNewBioUnit(cBioUnit* bu, tArrayMap<cString, tArrayMap<cString, cString> >* hints = NULL);

  void SaveBioGroups(const cString& role, cDataFile& df);
};


#endif
