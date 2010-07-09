/*
 *  cBGGenotypeManager.h
 *  Avida
 *
 *  Created by David on 11/11/09.
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

#ifndef cBGGenotypeManager_h
#define cBGGenotypeManager_h

#ifndef defs_h
#include "defs.h"
#endif
#ifndef cBioGroupManager_h
#include "cBioGroupManager.h"
#endif
#ifndef cFlexVar_h
#include "cFlexVar.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif
#ifndef tManagedPointerArray_h
#include "tManagedPointerArray.h"
#endif

class cBGGenotype;
class cGenome;
class cWorld;
template <class T> class tDataCommandManager;

namespace nBGGenotypeManager {
  const int HASH_SIZE = 3203;
}

class cBGGenotypeManager : public cBioGroupManager
{
private:
  cWorld* m_world;

  tList<cBGGenotype> m_active_hash[nBGGenotypeManager::HASH_SIZE];
  tManagedPointerArray<tList<cBGGenotype> > m_active_sz;
  tList<cBGGenotype> m_historic;
  cBGGenotype* m_coalescent;
  int m_best;
  int m_next_id;
  int m_dom_prev;
  int m_dom_time;
  int m_active_count;
  tArray<int> m_sz_count;
  
  mutable tDataCommandManager<cBGGenotype>* m_dcm;
  
  
public:
  cBGGenotypeManager(cWorld* world);
  ~cBGGenotypeManager();
  
  // cBioGroupManager Interface Methods
  cBioGroup* ClassifyNewBioUnit(cBioUnit* bu);
  cBioGroup* GetBioGroup(int bg_id);
  
  void UpdateReset();
  void UpdateStats(cStats& stats);
  
  cBioGroup* LoadBioGroup(const tDictionary<cString>& props);
  void SaveBioGroups(cDataFile& df);
  
  // Genotype Manager Methods
  cBGGenotype* ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents);
  void AdjustGenotype(cBGGenotype* genotype, int old_size, int new_size);

  const tArray<cString>& GetBioGroupPropertyList() const;
  bool BioGroupHasProperty(const cString& prop) const;
  cFlexVar GetBioGroupProperty(const cBGGenotype* genotype, const cString& prop) const;
  
private:
  unsigned int hashGenome(const cGenome& genome) const;
  cString nameGenotype(int size);
  
  void removeGenotype(cBGGenotype* genotype);
  void updateCoalescent();
  
  inline void resizeActiveList(int size);
  inline cBGGenotype* getBest();
  
  void buildDataCommandManager() const;
};


inline void cBGGenotypeManager::resizeActiveList(int size)
{
  if (m_active_sz.GetSize() <= size) m_active_sz.Resize(size + 1);
}

inline cBGGenotype* cBGGenotypeManager::getBest()
{
  return (m_best) ? m_active_sz[m_best].GetLast() : NULL;
}


#endif
