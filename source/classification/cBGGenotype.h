/*
 *  cBGGenotype.h
 *  Avida
 *
 *  Created by David on 11/5/09.
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

#ifndef cBGGenotype_h
#define cBGGenotype_h

#ifndef cBioGroup_h
#include "cBioGroup.h"
#endif
#ifndef cCountTracker_h
#include "cCountTracker.h"
#endif
#ifndef cDoubleSum_h
#include "cDoubleSum.h"
#endif
#ifndef cMetaGenome_h
#include "cMetaGenome.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif


class cBGGenotypeManager;


class cBGGenotype : public cBioGroup
{
private:
  cBGGenotypeManager* m_mgr;
  
  cBioUnit::eUnitSource m_src;
  cMetaGenome m_genome;
  cString m_name;
    
  struct {
    bool m_threshold:1;
    bool m_active:1;
  };
  
  int m_id;
  int m_update_born;
  int m_update_deactivated;
  int m_depth;
  int m_active_offspring_genotypes;
  
  tArray<cBioGroup*> m_parents;
  
  cCountTracker m_births;
  cCountTracker m_deaths;
  cCountTracker m_breed_in;
  cCountTracker m_breed_true;
  cCountTracker m_breed_out;
  cCountTracker m_orgs;

  cDoubleSum m_copied_size;
  cDoubleSum m_exe_size;
  cDoubleSum m_gestation_time;
  cDoubleSum m_repro_rate;
  cDoubleSum m_merit;
  cDoubleSum m_fitness;
    
  
  cBGGenotype(cBGGenotypeManager* mgr, int in_id, cBioUnit* founder, int update, tArray<cBioGroup*>* parents);
  
public:
  ~cBGGenotype();
  
  // cBioGroup Interface Methods
  cBioGroup* ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents = NULL);
  void RemoveBioUnit(cBioUnit* bu);
  
  int GetDepth() const { return m_depth; }

  
  // Genotype Specific Methods
  inline const cString& GetName() const { return m_name; }
  
  inline bool IsThreshold() const { return m_threshold; }
  inline bool IsActive() const { return m_active; }
  
  inline int GetID() const { return m_id; }
  inline int GetUpdateBorn() const { return m_update_born; }
  inline int GetUpdateDeactivated() const { return m_update_deactivated; }
  
  void SetThreshold() { m_threshold = true; }
  void ClearThreshold() { m_threshold = false; }
};

#endif
