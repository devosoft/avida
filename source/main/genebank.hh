//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef GENEBANK_HH
#define GENEBANK_HH

#include <fstream>

#ifndef DEFS_HH
#include "defs.hh"
#endif
#ifndef GENOTYPE_CONTROL_HH
#include "genotype_control.hh"
#endif
#ifndef SPECIES_CONTROL_HH
#include "species_control.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

template <class T> class tList; // array
class cGenome;
class cGenotype;
class cGenotypeControl; // access
class cSpeciesControl; // access
class cStats;
class cString; // aggregate

class cGenebank {
private:
  unsigned int genotype_count[MAX_CREATURE_SIZE];
  tList<cGenotype> active_genotypes[GENOTYPE_HASH_SIZE];
  cGenotypeControl * genotype_control;
  cSpeciesControl * species_control;
  cStats & stats;

private:
  cString GetLabel(int in_size, int in_num);

private:
  // disabled copy constructor.
  cGenebank(const cGenebank &);
public:
  cGenebank(cStats & _stats);
  ~cGenebank();

  void UpdateReset();

  /** 
   * This function can be used to add a genotype that was created
   * outside the genebank. In this case, the parameter in_list_num
   * should not be given. Normally, genotypes are added through the 
   * function AddGenotype(const cGenome & in_genome, 
   * cGenotype * parent_genotype = NULL), which then calls this one.
   **/
  void AddGenotype(cGenotype *in_genotype, int list_num=-1);
  cGenotype * AddGenotype(const cGenome & in_genome,
			  cGenotype * parent_genotype,
			  cGenotype * parent2_genotype);
  cGenotype * InjectGenotype(const cGenome & in_genome, int lineage_label);
  const cGenotype * FindGenotype(const cGenome & in_genome, int lineage_label,
				 int list_num=-1) const;
  cGenotype * FindGenotype(const cGenome & in_genome, int lineage_label,
			   int list_num=-1);
  void RemoveGenotype(cGenotype & in_genotype);
  void ThresholdGenotype(cGenotype & in_genotype);
  bool AdjustGenotype(cGenotype & in_genotype);

  bool SaveClone(std::ofstream & fp);
  bool LoadClone(std::ifstream & fp);
  bool DumpTextSummary(std::ofstream & fp);
  bool PrintGenotypes(std::ofstream & fp, cString & data_fields, int historic);
  bool DumpDetailedSummary(std::ofstream & fp);
  bool DumpDetailedSexSummary(std::ofstream & fp);
  bool DumpHistoricSummary(std::ofstream & fp, int back_dist);
  bool DumpHistoricSexSummary(std::ofstream & fp);
  void DumpDetailHeading (std::ofstream & fp);
  void DumpDetailSexHeading (std::ofstream & fp);
  void DumpDetailedEntry(cGenotype * genotype, std::ofstream & fp);
  void DumpDetailedSexEntry(cGenotype * genotype, std::ofstream & fp);
  bool OK();

  inline int GetSize() const { return genotype_control->GetSize(); }
  inline cGenotype * GetBestGenotype() const
    { return genotype_control->GetBest(); }
  inline cGenotype * GetCoalescentGenotype() const
    { return genotype_control->GetCoalescent(); }
  inline cSpecies * GetFirstSpecies() const
    { return species_control->GetFirst(); }

  inline cGenotype * GetGenotype(int thread) const
    { return genotype_control->Get(thread); }
  inline cGenotype * NextGenotype(int thread) {
    cGenotype * next = genotype_control->Next(thread);
    return (next == genotype_control->GetBest()) ? (cGenotype*)NULL : next;
  }
  inline cGenotype * ResetThread(int thread)
    { return genotype_control->Reset(thread); }

  int CountNumCreatures();
  inline int GetNumSpecies() const { return species_control->GetSize(); }
  inline int CountSpecies() { return species_control->OK(); }
  inline int FindPos(cGenotype & in_genotype, int max_depth = -1)
    { return genotype_control->FindPos(in_genotype, max_depth); }
  inline int FindPos(cSpecies & in_species, int max_depth = -1)
    { return species_control->FindPos(in_species, max_depth); }

  unsigned int FindCRC(const cGenome & in_genome) const;

  void SpeciesTest(char * message, cGenotype & genotype);
};

#endif
