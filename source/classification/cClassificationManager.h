/*
 *  cClassificationManager.h
 *  Avida
 *
 *  Created by David on 11/14/05.
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

#ifndef cClassificationManager_h
#define cClassificationManager_h

#include <list>
#include <set>

#ifndef defs_h
#include "defs.h"
#endif
#ifndef cGenotypeControl_h
#include "cGenotypeControl.h"
#endif
#ifndef cInjectGenotypeControl_h
#include "cInjectGenotypeControl.h"
#endif
#ifndef cInjectGenotypeQueue_h
#include "cInjectGenotypeQueue.h"
#endif
#ifndef cSpeciesControl_h
#include "cSpeciesControl.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


class cAvidaContext;
class cLineage;
class cOrganism;
class cWorld;

class cClassificationManager
{
#if USE_tMemTrack
  tMemTrack<cClassificationManager> mt;
#endif
private:
  cWorld* m_world;
  
  // Genotype Structures
  unsigned int m_genotype_count[MAX_CREATURE_SIZE];
  tList<cGenotype> m_active_genotypes[nGenotype::HASH_SIZE];
  cGenotypeControl* m_genotype_ctl;
  int m_genotype_next_id;
  int m_genotype_dom_time;
  int m_genotype_prev_dom;

  // Species Structures
  cSpeciesControl* m_species_ctl;
  int m_species_next_id;

  // InjectGenotype Structures
  unsigned int m_inject_count[MAX_CREATURE_SIZE];
  cInjectGenotypeQueue m_active_inject[nInjectGenotype::HASH_SIZE];
  cInjectGenotypeControl* m_inject_ctl;
  int m_inject_next_id;
  int m_inject_dom_time;
  int m_inject_prev_dom;
  
  // Lineage Structures
  std::list<cLineage*> m_lineage_list;
  cLineage* m_best_lineage;  // the lineage with the highest average fitness
  cLineage* m_max_fitness_lineage; // lineage with the single highest fitness
  cLineage* m_dominant_lineage; // the lineage with the largest number of creatures.
  int m_lineage_next_id;
	
	// CClade @MRR
	std::set<int>  m_cclade_ids;
  
  
  // Private Helper Functions
  void AddGenotype(cGenotype* in_genotype, int list_num = -1);
  void AddInjectGenotype(cInjectGenotype* in_inject_genotype, int in_list_num = -1);
  void DumpDetailHeading(std::ofstream& fp);
  void DumpDetailSexHeading (std::ofstream& fp);
  unsigned int FindCRC(const cGenome& in_genome) const;
  unsigned int FindInjectCRC(const cGenome& in_genome) const;
  cString GetLabel(int in_size, int in_num);
  cString GetInjectLabel(int in_size, int in_num) { return cString("p") + GetLabel(in_size, in_num); }
  
  cLineage* AddLineage(double start_fitness, int parent_lin_id, int id = -1, double lineage_stat1 = 0.0, double lineage_stat2 = 0.0);
  void UpdateLineages();
    
  cClassificationManager(); // @not_implemented
  cClassificationManager(const cClassificationManager&); // @not_implemented
  cClassificationManager& operator=(const cClassificationManager&); // @not_implemented

public:
  cClassificationManager(cWorld* world);
  ~cClassificationManager();

  void UpdateReset();
  
  
  // Genotype Manipulation
  cGenotype* GetGenotype(const cGenome& in_genome, cGenotype* parent1, cGenotype* parent2);
  cGenotype* GetGenotypeInjected(const cGenome& in_genome, int lineage_label);
  cGenotype* GetGenotypeLoaded(const cGenome& in_genome, int update_born = 0, int id_num = -1);
  const cGenotype* FindGenotype(const cGenome& in_genome, int lineage_label, int list_num = -1) const;
  cGenotype* FindGenotype(const cGenome& in_genome, int lineage_label, int list_num = -1);
  void RemoveGenotype(cGenotype& in_genotype);
  void ThresholdGenotype(cGenotype& in_genotype);
  bool AdjustGenotype(cGenotype& in_genotype);
  
  int GetGenotypeCount() const { return m_genotype_ctl->GetSize(); }
  cGenotype* GetBestGenotype() const { return m_genotype_ctl->GetBest(); }
  cGenotype* GetCoalescentGenotype() const { return m_genotype_ctl->GetCoalescent(); }
  
  cGenotype* GetGenotype(int thread) const { return m_genotype_ctl->Get(thread); }
  cGenotype* NextGenotype(int thread)
  {
    cGenotype* next = m_genotype_ctl->Next(thread);
    return (next == m_genotype_ctl->GetBest()) ? NULL : next;
  }
  cGenotype* ResetThread(int thread) { return m_genotype_ctl->Reset(thread); }

  int FindPos(cGenotype& in_genotype, int max_depth = -1) { return m_genotype_ctl->FindPos(in_genotype, max_depth); }
  
  // Genotype Output
  bool DumpTextSummary(std::ofstream& fp);
  bool PrintGenotypes(std::ofstream& fp, cString & data_fields, int historic);
  bool DumpDetailedSummary(std::ofstream& fp);
  bool DumpDetailedSexSummary(std::ofstream& fp);
  bool DumpHistoricSummary(std::ofstream& fp, int back_dist);
  bool DumpHistoricSexSummary(std::ofstream& fp);
  void DumpDetailedEntry(cGenotype* genotype, std::ofstream& fp);
  void DumpDetailedSexEntry(cGenotype* genotype, std::ofstream& fp);
  
  
  // InjectGenotype Manipulation
  cInjectGenotype* GetInjectGenotype(const cGenome& in_genome, cInjectGenotype* parent_inject_genotype = NULL);
  cInjectGenotype* FindInjectGenotype(const cGenome& in_genome) const;
  void RemoveInjectGenotype(cInjectGenotype & in_inject_genotype);
  void ThresholdInjectGenotype(cInjectGenotype & in_inject_genotype);
  bool AdjustInjectGenotype(cInjectGenotype & in_inject_genotype);  
  
  int GetInjectGenotypeCount() const { return m_inject_ctl->GetSize(); }
  cInjectGenotype* GetBestInjectGenotype() const { return m_inject_ctl->GetBest(); }
  cInjectGenotype* GetCoalescentInjectGenotype() const { return m_inject_ctl->GetCoalescent(); }
  
  cInjectGenotype* GetInjectGenotype(int thread) const { return m_inject_ctl->Get(thread); }
  cInjectGenotype* NextInjectGenotype(int thread)
  {
    cInjectGenotype* next = m_inject_ctl->Next(thread);
    return (next == m_inject_ctl->GetBest()) ? NULL : next; 
  }
  cInjectGenotype* ResetInjectThread(int thread) { return m_inject_ctl->Reset(thread); }
  
  int FindPos(cInjectGenotype& inject, int max_depth = -1) { return m_inject_ctl->FindPos(inject, max_depth); }
    
  // InjectGenotype Output
  bool DumpInjectTextSummary(std::ofstream& fp);
  bool DumpInjectDetailedSummary(const cString & file, int update);
  void DumpInjectDetailedEntry(cInjectGenotype * inject_genotype, const cString & file, int update);
  

  // Species Accessors
  cSpecies* GetFirstSpecies() const { return m_species_ctl->GetFirst(); }
  int GetNumSpecies() const { return m_species_ctl->GetSize(); }
  int CountSpecies() { return m_species_ctl->OK(); }
  int FindPos(cSpecies& in_species, int max_depth = -1) { return m_species_ctl->FindPos(in_species, max_depth); }  


  // Lineage Manipulators
  cLineage* GetLineage(cAvidaContext& ctx, cGenotype* child_genotype,
                       cGenotype* parent_genotype, cLineage* parent_lineage, int parent_lin_id);
  void RemoveLineageOrganism(cOrganism* org);  
  cLineage* FindLineage(int lineage_id) const;
  cLineage* GetMaxFitnessLineage() const { return m_max_fitness_lineage; }
  cLineage* GetBestLineage() const { return m_best_lineage; }
  cLineage* GetDominantLineage() const { return m_dominant_lineage; }
  int GetLineageCount() const { return m_lineage_list.size(); }
  const std::list<cLineage*>& GetLineageList() const { return m_lineage_list; }

  // Lineage Output
  void PrintLineageTotals(const cString& filename, bool verbose=false);
  void PrintLineageCurCounts(const cString& filename);
  
	// Coalescence Clade
	void LoadCCladeFounders(const cString& filename);
	bool IsCCladeFounder(const int id) const;
	
  // Utility Functions
  bool SaveClone(std::ofstream& fp);
  bool LoadClone(std::ifstream & fp);
  bool OK();  
};


#ifdef ENABLE_UNIT_TESTS
namespace nClassificationManager {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
