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
  friend class cBGGenotypeManager;
private:
  cBGGenotypeManager* m_mgr;
  
  eBioUnitSource m_src;
  cString m_src_args;
  cMetaGenome m_genome;
  cString m_name;
    
  struct {
    bool m_threshold:1;
    bool m_active:1;
  };
  
  int m_id;
  int m_generation_born;
  int m_update_born;
  int m_update_deactivated;
  int m_depth;
  int m_active_offspring_genotypes;
  int m_num_organisms;
  int m_last_num_organisms;
  int m_total_organisms;
  
  tArray<cBGGenotype*> m_parents;
  
  cCountTracker m_births;
  cCountTracker m_deaths;
  cCountTracker m_breed_in;
  cCountTracker m_breed_true;
  cCountTracker m_breed_out;

  cDoubleSum m_copied_size;
  cDoubleSum m_exe_size;
  cDoubleSum m_gestation_time;
  cDoubleSum m_merit;
  cDoubleSum m_fitness;
    
  
  cBGGenotype(cBGGenotypeManager* mgr, int in_id, cBioUnit* founder, int update, tArray<cBioGroup*>* parents);
  
public:
  ~cBGGenotype();
  
  // cBioGroup Interface Methods
  int GetRoleID() const;
  const cString& GetRole() const;  
  int GetID() const { return m_id; }
  
  cBioGroup* ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents = NULL);
  void HandleBioUnitGestation(cBioUnit* bu);
  void RemoveBioUnit(cBioUnit* bu);
  
  int GetDepth() const { return m_depth; }
  
  void Save(cDataFile& df);

  
  // Genotype Specific Methods
  inline const eBioUnitSource GetSource() const { return m_src; }
  inline const cString& GetSourceArgs() const { return m_src_args; }
  inline const cMetaGenome& GetMetaGenome() const { return m_genome; }
  
  inline const cString& GetName() const { return m_name; }
  inline void SetName(const cString& name) { m_name = name; }
  
  inline bool IsThreshold() const { return m_threshold; }
  inline bool IsActive() const { return m_active; }
  
  inline int GetUpdateBorn() const { return m_update_born; }
  inline int GetUpdateDeactivated() const { return m_update_deactivated; }
  
  inline int GetNumOrganisms() const { return m_num_organisms; }
  
  inline void SetThreshold() { m_threshold = true; }
  inline void ClearThreshold() { m_threshold = false; }
  
  inline void Deactivate(int update) { m_active = false; m_update_deactivated = update; }
  
  inline const tArray<cBGGenotype*> GetParents() const { return m_parents; }

  bool Matches(cBioUnit* bu);
  void NotifyNewBioUnit(cBioUnit* bu);
  void UpdateReset();
};

#endif
