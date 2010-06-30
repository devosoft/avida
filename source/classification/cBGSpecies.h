/*
 *  cBGSpecies.h
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

#ifndef cBGSpecies_h
#define cBGSpecies_h

#ifndef cBioGroup_h
#include "cBioGroup.h"
#endif
#ifndef nSpecies_h
#include "nSpecies.h"
#endif


class cBGSpeciesManager;


class cBGSpecies : public cBioGroup
{
  friend class cBGSpeciesManager;
private:
  cBGSpeciesManager* m_mgr;

  cBioGroup* m_founder;
  
  int m_id;
  int m_update_born;
  int m_depth;
  int m_total_organisms;
  int m_total_genotypes;
  int m_num_threshold;
  int m_num_organisms;
  int m_genotype_distance[nSpecies::MAX_DISTANCE];
  
  
  cBGSpecies(cBGSpeciesManager* mgr, int in_id, cBioUnit* founder, int update, tArray<cBioGroup*>* parents);
  
public:
  ~cBGSpecies();
  
  // cBioGroup Interface Methods
  int GetRoleID() const;
  const cString& GetRole() const;  
  int GetID() const { return m_id; }
  
  cBioGroup* ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents = NULL);
  void HandleBioUnitGestation(cBioUnit* bu);
  void RemoveBioUnit(cBioUnit* bu);
  
  int GetDepth() const { return m_depth; }
  
  const tArray<cString>& GetProperyList() const;
  bool HasProperty(const cString& prop) const { false; }
  cFlexVar GetProperty(const cString& prop) const { return cFlexVar(); }

  void Save(cDataFile& df);
  
  
};

#endif
