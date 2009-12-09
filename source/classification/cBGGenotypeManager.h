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

#ifndef defs_h
#include "defs.h"
#endif
#ifndef cBioGroupManager_h
#include "cBioGroupManager.h"
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

namespace nBGGenotypeManager {
  const int HASH_SIZE = 3203;
}

class cBGGenotypeManager : public cBioGroupManager
{
private:
  cWorld* m_world;

  unsigned int m_sz_count[MAX_CREATURE_SIZE];
  tList<cBGGenotype> m_active_hash[nBGGenotypeManager::HASH_SIZE];
  tManagedPointerArray<tList<cBGGenotype> > m_active_sz;
  tList<cBGGenotype> m_historic;
  cBGGenotype* m_coalescent;
  int m_best;
  int m_next_id;
  int m_dom_prev;
  int m_dom_time;
  
  
public:
  cBGGenotypeManager(cWorld* world);
  ~cBGGenotypeManager();
  
  // cBioGroupManager Interface Methods
  cBioGroup* ClassifyNewBioUnit(cBioUnit* bu);
  
  void UpdateReset();
  
  // Genotype Manager Methods
  cBGGenotype* ClassifyNewBioUnit(cBioUnit* bu, tArray<cBioGroup*>* parents);
  void AdjustGenotype(cBGGenotype* genotype, int old_size, int new_size);
  
private:
  unsigned int hashGenome(const cGenome& genome) const;
  cString nameGenotype(int size, int num) const;
  
  void removeGenotype(cBGGenotype* genotype);
  void updateCoalescent();  
  
  inline void resizeActiveList(int size);
  inline cBGGenotype* getBest();
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
