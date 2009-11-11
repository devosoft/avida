/*
 *  cBGGenotypeManager.h
 *  Avida
 *
 *  Created by David on 11/11/09.
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

#ifndef cBGGenotypeManager_h
#define cBGGenotypeManager_h

#ifndef cBioGroupManager_h
#include "cBioGroupManager.h"
#endif

class cWorld;


class cBGGenotypeManager : public cBioGroupManager
{
private:
  cWorld* m_world;
  
  
public:
  cBGGenotypeManager(cWorld* world);
  ~cBGGenotypeManager();
  
  // cBioGroupManager Interface Methods
  cBioGroup* ClassifyNewBioUnit(cBioUnit* bu);
  
  
  // Genotype Manager Methods
};

#endif
