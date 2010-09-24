/*
 *  cClassificationManager.cc
 *  Avida
 *
 *  Created by David on 11/14/05.
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

#include "cClassificationManager.h"

#include "cBGGenotypeManager.h"
#include "cBioGroupManager.h"
#include "cBioUnit.h"


cClassificationManager::cClassificationManager(cWorld* world) : m_world(world)
{
  RegisterBioGroupManager(new cBGGenotypeManager(world), "genotype");
}

cClassificationManager::~cClassificationManager()
{
  for (int i = 0; i < m_bgms.GetSize(); i++) delete m_bgms[i];
}

bool cClassificationManager::RegisterBioGroupManager(cBioGroupManager* bgm, const cString& role,
                                                     const tArray<cString>* dependencies)
{
  // @TODO - verify that this role is not already in use -- maybe hash managers by role
  bgm->SetRole(m_bgms.Push(bgm), role);
  
  // @TODO - properly handle bio group dependencies
  
  return true;
}

cBioGroupManager* cClassificationManager::GetBioGroupManager(const cString& role)
{
  for (int i = 0; i < m_bgms.GetSize(); i++) if (m_bgms[i]->GetRole() == role) return m_bgms[i];
  
  return NULL;
}


void cClassificationManager::ClassifyNewBioUnit(cBioUnit* bu, tArrayMap<cString, tArrayMap<cString, cString> >* hints)
{
  for (int i = 0; i < m_bgms.GetSize(); i++) {
    tArrayMap<cString, cString> role_hints;
    if (hints) hints->Get(m_bgms[i]->GetRole(), role_hints);
    cBioGroup* group = m_bgms[i]->ClassifyNewBioUnit(bu, &role_hints);
    if (group) bu->AddClassification(group);
  }
}

void cClassificationManager::SaveBioGroups(const cString& role, cDataFile& df)
{
  for (int i = 0; i < m_bgms.GetSize(); i++) {
    if (m_bgms[i]->GetRole() == role) {
      m_bgms[i]->SaveBioGroups(df);
      return;
    }
  }
}


void cClassificationManager::UpdateReset()
{
  // Notify all bio group managers of the update reset
  for (int i = 0; i < m_bgms.GetSize(); i++) m_bgms[i]->UpdateReset();
}

void cClassificationManager::UpdateStats(cStats& stats)
{
  // @TODO - in the future classification manager should have bio group managers register for stats updating...
  //         for now assume all bio groups will have stats to update
  for (int i = 0; i < m_bgms.GetSize(); i++) m_bgms[i]->UpdateStats(stats);
}
