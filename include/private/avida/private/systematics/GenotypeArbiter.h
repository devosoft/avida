/*
 *  private/systematics/GenotypeArbiter.h
 *  Avida
 *
 *  Created by David on 11/11/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaSystematicsGenotypeArbiter_h
#define AvidaSystematicsGenotypeArbiter_h

#include "avida/core/Properties.h"
#include "avida/systematics/Arbiter.h"
#include "avida/private/systematics/Genotype.h"

#include "tIterator.h"
#include "tManagedPointerArray.h"
#include "tSparseVectorList.h"

namespace Avida {
  class Sequence;
}
class cWorld;


namespace nBGGenotypeManager {
  const int HASH_SIZE = 3203;
}



namespace Avida {
  namespace Systematics {
    
    // Genotype
    // --------------------------------------------------------------------------------------------------------------
    
    class GenotypeArbiter : public Arbiter
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
      GenotypeArbiter(cWorld* world);
      ~GenotypeArbiter();
      
      // Arbiter Interface Methods
      GroupPtr ClassifyNewUnit(UnitPtr u, const ClassificationHints* hints);
      GroupPtr Group(GroupID g_id);
      
      void PerformUpdate(Context& ctx, Update current_update);
      
      bool Serialize(ArchivePtr ar) const;
      
      
      tIterator<Systematics::Group>* Iterator();
      
      
      // Genotype Manager Methods
      cBGGenotype* ClassifyNewBioUnit(Systematics::UnitPtr bu, tArray<Systematics::GroupPtr>* parents, tArrayMap<cString, cString>* hints = NULL);
      void AdjustGenotype(cBGGenotype* genotype, int old_size, int new_size);

      const tArray<cString>& GetBioGroupPropertyList() const;
      bool BioGroupHasProperty(const cString& prop) const;
      cFlexVar GetBioGroupProperty(const cBGGenotype* genotype, const cString& prop) const;
      
    private:
      unsigned int hashGenome(const Sequence& genome) const;
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

  };
};

#endif
