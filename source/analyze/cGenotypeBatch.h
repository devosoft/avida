/*
 *  cGenotypeBatch.h
 *  Avida
 *
 *  Called "genotype_batch.hh" prior to 12/2/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#ifndef cGenotypeBatch_h
#define cGenotypeBatch_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

// cGenotypeBatch      : Collection of cAnalyzeGenotypes

class cAnalyzeGenotype;
class cRandom;

class cGenotypeBatch {
private:
  tListPlus<cAnalyzeGenotype> m_list;
  cString m_name;
  cAnalyzeGenotype* m_lineage_head;
  cAnalyzeGenotype* m_clade_head;
  bool m_is_lineage;
  bool m_is_aligned;
  
  cGenotypeBatch(const cGenotypeBatch&); // @not_implemented
  cGenotypeBatch& operator=(const cGenotypeBatch&); // @not_implemented

public:
  cGenotypeBatch() : m_name(""), m_lineage_head(NULL), m_clade_head(NULL), m_is_lineage(false), m_is_aligned(false) { ; }
  ~cGenotypeBatch() { ; }

  tListPlus<cAnalyzeGenotype>& List() { return m_list; }
  cString& Name() { return m_name; }
  const cString& GetName() { return m_name; }
  
  int GetSize() { return m_list.GetSize(); }
  
  bool IsLineage() { return m_is_lineage || (m_lineage_head); }
  bool IsClade() { return (m_clade_head); }
  bool IsAligned() { return m_is_aligned; }

  void SetLineage(bool _val = true) { m_is_lineage = _val; }
  void SetAligned(bool _val = true) { m_is_aligned = _val; }
  
  void MergeWith(cGenotypeBatch* batch) { m_list.Append(batch->m_list); }
  
  cAnalyzeGenotype* FindGenotypeNumCPUs();
  cAnalyzeGenotype* PopGenotypeNumCPUs();
  cAnalyzeGenotype* FindGenotypeTotalCPUs();
  cAnalyzeGenotype* PopGenotypeTotalCPUs();
  cAnalyzeGenotype* FindGenotypeMetabolicRate();
  cAnalyzeGenotype* PopGenotypeMetabolicRate();
  cAnalyzeGenotype* FindGenotypeFitness();
  cAnalyzeGenotype* PopGenotypeFitness();
  cAnalyzeGenotype* FindGenotypeID(int gid);
  cAnalyzeGenotype* PopGenotypeID(int gid);
  cAnalyzeGenotype* FindGenotypeRandom(cRandom& rng);
  cAnalyzeGenotype* PopGenotypeRandom(cRandom& rng);
  inline cAnalyzeGenotype* FindGenotypeRandom(cRandom* rng) { return FindGenotypeRandom(*rng); }
  inline cAnalyzeGenotype* PopGenotypeRandom(cRandom* rng) { return PopGenotypeRandom(*rng); }
  
  cGenotypeBatch* FindLineage(cAnalyzeGenotype* end_genotype);
  cGenotypeBatch* FindLineage(int end_genotype_id);

  cGenotypeBatch* FindClade(cAnalyzeGenotype* start_genotype);
  cGenotypeBatch* FindClade(int start_genotype_id);
  
  void RemoveClade(cAnalyzeGenotype* start_genotype);
  void RemoveClade(int start_genotype_id);

  
private:
  inline void clearFlags() { m_is_lineage = false; m_is_aligned = false; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nGenotypeBatch {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
