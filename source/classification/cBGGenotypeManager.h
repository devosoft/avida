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

#include "Avida.h"

#include "cBioGroupManager.h"
#include "cFlexVar.h"
#include "tIterator.h"
#include "tManagedPointerArray.h"
#include "tSparseVectorList.h"

class cBGGenotype;
class cSequence;
class cWorld;
template <class T> class tDataCommandManager;

namespace nBGGenotypeManager {
  const int HASH_SIZE = 3203;
}

class cBGGenotypeManager : public cBioGroupManager
{
private:
  cWorld* m_world;

  tSparseVectorList<cBGGenotype> m_active_hash[nBGGenotypeManager::HASH_SIZE];
  tManagedPointerArray<tSparseVectorList<cBGGenotype> > m_active_sz;
  tSparseVectorList<cBGGenotype> m_historic;
  cBGGenotype* m_coalescent;
  int m_best;
  int m_next_id;
  int m_dom_prev;
  int m_dom_time;
  tArray<int> m_sz_count;
  
  mutable tDataCommandManager<cBGGenotype>* m_dcm;
  
  
public:
  cBGGenotypeManager(cWorld* world);
  ~cBGGenotypeManager();
  
  // cBioGroupManager Interface Methods
  cBioGroup* ClassifyNewBioUnit(cBioUnit* bu, tArrayMap<cString, cString>* hints);
  cBioGroup* GetBioGroup(int bg_id);
  
  void UpdateReset();
  void UpdateStats(cStats& stats);
  
  cBioGroup* LoadBioGroup(const tDictionary<cString>& props);
  void SaveBioGroups(cDataFile& df);
  
  tIterator<cBioGroup>* Iterator();
  
  
  // Genotype Manager Methods
  cBGGenotype* ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents, tArrayMap<cString, cString>* hints = NULL);
  void AdjustGenotype(cBGGenotype* genotype, int old_size, int new_size);

  const tArray<cString>& GetBioGroupPropertyList() const;
  bool BioGroupHasProperty(const cString& prop) const;
  cFlexVar GetBioGroupProperty(const cBGGenotype* genotype, const cString& prop) const;
  
private:
  unsigned int hashGenome(const cSequence& genome) const;
  cString nameGenotype(int size);
  
  void removeGenotype(cBGGenotype* genotype);
  void updateCoalescent();
  
  inline void resizeActiveList(int size);
  inline cBGGenotype* getBest();
  
  void buildDataCommandManager() const;
  
  class cGenotypeIterator : public tIterator<cBioGroup>
  {
  private:
    cBGGenotypeManager* m_bgm;
    int m_sz_i;
    tIterator<cBGGenotype>* m_it;
    
    cGenotypeIterator(); // @not_implemented
    cGenotypeIterator(const cGenotypeIterator&); // @not_implemented
    cGenotypeIterator& operator=(const cGenotypeIterator&); // @not_implemented
    
    
  public:
    cGenotypeIterator(cBGGenotypeManager* bgm)
      : m_bgm(bgm), m_sz_i(bgm->m_best), m_it(m_bgm->m_active_sz[m_sz_i].Iterator()) { ; }
    ~cGenotypeIterator() { delete m_it; }
    
    cBioGroup* Get();
    cBioGroup* Next();
  };
};


inline void cBGGenotypeManager::resizeActiveList(int size)
{
  if (m_active_sz.GetSize() <= size) m_active_sz.Resize(size + 1);
}

inline cBGGenotype* cBGGenotypeManager::getBest()
{
  return (m_best) ? m_active_sz[m_best].GetFirst() : NULL;
}


#endif
