/*
 *  cBGGenotype.h
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

#ifndef cBGGenotype_h
#define cBGGenotype_h

#include "cBioGroup.h"
#include "cCountTracker.h"
#include "cDoubleSum.h"
#include "cGenome.h"
#include "cString.h"


class cAvidaContext;
class cBGGenotypeManager;
class cEntryHandle;
class cWorld;


class cBGGenotype : public cBioGroup
{
  friend class cBGGenotypeManager;
private:
  cBGGenotypeManager* m_mgr;
  cEntryHandle* m_handle;
  
  eBioUnitSource m_src;
  cString m_src_args;
  cGenome m_genome;
  cString m_name;
    
  struct {
    bool m_threshold:1;
    bool m_active:1;
  };
  
  int m_generation_born;
  int m_update_born;
  int m_update_deactivated;
  int m_depth;
  int m_active_offspring_genotypes;
  int m_num_organisms;
  int m_last_num_organisms;
  int m_total_organisms;
  
  tArray<cBGGenotype*> m_parents;
  cString m_parent_str;
  
  cCountTracker m_births;
  cCountTracker m_deaths;
  cCountTracker m_breed_in;
  cCountTracker m_breed_true;
  cCountTracker m_breed_out;

  cDoubleSum m_copied_size;
  cDoubleSum m_exe_size;
  cDoubleSum m_gestation_time;
  cDoubleSum m_repro_rate;
  cDoubleSum m_merit;
  cDoubleSum m_fitness;
    
  
  cBGGenotype(cBGGenotypeManager* mgr, int in_id, cBioUnit* founder, int update, tArray<cBioGroup*>* parents);
  cBGGenotype(cBGGenotypeManager* mgr, int in_id, const tDictionary<cString>& props, cWorld* world);
  
public:
  ~cBGGenotype();
  
  // cBioGroup Interface Methods
  int GetRoleID() const;
  const cString& GetRole() const;  
  int GetID() const { return m_id; }
  
  cBioGroup* ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents = NULL);
  void HandleBioUnitGestation(cBioUnit* bu);
  void RemoveBioUnit(cBioUnit* bu);
  
  void RemoveActiveReference();
  
  int GetDepth() const { return m_depth; }
  int GetNumUnits() const { return m_num_organisms; }
  
  const tArray<cString>& GetProperyList() const;
  bool HasProperty(const cString& prop) const;
  cFlexVar GetProperty(const cString& prop) const;
  
  void Save(cDataFile& df);

  
  // Genotype Specific Methods
  inline const eBioUnitSource GetSource() const { return m_src; }
  inline const cString& GetSourceArgs() const { return m_src_args; }
  inline const cGenome& GetGenome() const { return m_genome; }
  inline cString GetGenomeString() const { return m_genome.AsString(); }
  
  inline const cString& GetName() const { return m_name; }
  inline void SetName(const cString& name) { m_name = name; }
  
  inline bool IsThreshold() const { return m_threshold; }
  inline bool IsActive() const { return m_active; }
  
  inline int GetUpdateBorn() const { return m_update_born; }
  inline int GetUpdateDeactivated() const { return m_update_deactivated; }
  
  inline const cString& GetParentString() const { return m_parent_str; }
  
  inline void SetThreshold() { m_threshold = true; }
  inline void ClearThreshold() { m_threshold = false; }
  
  inline void Deactivate(int update) { m_active = false; m_update_deactivated = update; }
  
  inline const tArray<cBGGenotype*> GetParents() const { return m_parents; }
  
  inline int GetTotalOrganisms() const { return m_total_organisms; }
  

  inline int GetLastBirths() const { return m_births.GetLast(); }
  inline int GetLastBreedIn() const { return m_breed_in.GetLast(); }
  inline int GetLastBreedTrue() const { return m_breed_true.GetLast(); }
  inline int GetLastBreedOut() const { return m_breed_out.GetLast(); }
  
  inline int GetThisBirths() const { return m_births.GetCur(); }
  inline int GetThisDeaths() const { return m_deaths.GetCur(); }
  inline int GetThisBreedIn() const { return m_breed_in.GetCur(); }
  inline int GetThisBreedTrue() const { return m_breed_true.GetCur(); }
  inline int GetThisBreedOut() const { return m_breed_out.GetCur(); }
  
  inline double GetCopiedSize() const { return m_copied_size.Average(); }
  inline double GetExecutedSize() const { return m_exe_size.Average(); }
  inline double GetGestationTime() const { return m_gestation_time.Average(); }
  inline double GetReproRate() const { return m_repro_rate.Average(); }
  inline double GetMerit() const { return m_merit.Average(); }
  inline double GetFitness() const { return m_fitness.Average(); }
  

  bool Matches(cBioUnit* bu);
  void NotifyNewBioUnit(cBioUnit* bu);
  void UpdateReset();
};

#endif
