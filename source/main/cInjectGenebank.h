//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INJECT_GENEBANK_HH
#define INJECT_GENEBANK_HH

#include <fstream>

#ifndef DEFS_HH
#include "defs.h"
#endif
#ifndef nInjectGenotype_h
#include "nInjectGenotype.h"
#endif
#ifndef INJECT_GENOTYPE_CONTROL_HH
#include "cInjectGenotypeControl.h"
#endif
#ifndef INJECT_GENOTYPE_QUEUE_HH
#include "cInjectGenotypeQueue.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif

class cGenome;
class cStats;
class cInjectGenotype;
class cInjectGenotypeQueue; // array
class cInjectGenotypeControl; // access
class cStats;
class cString; // aggregate
class cInjectGenotype;
class cGenome;
class cWorld;

class cInjectGenebank {
private:
  cWorld* m_world;
  unsigned int inject_genotype_count[MAX_CREATURE_SIZE];
  cInjectGenotypeQueue active_inject_genotypes[nInjectGenotype::HASH_SIZE];
  cInjectGenotypeControl * inject_genotype_control;
  cStats & stats;

private:
  cString GetLabel(int in_size, int in_num);

public:
  cInjectGenebank(cWorld* world, cStats & stats);
  ~cInjectGenebank();

  void UpdateReset();

  /** 
   * This function can be used to add a injectgenotype that was created
   * outside the genebank. In this case, the parameter in_list_num
   * should not be given. Normally, injectgenotypes are added through the 
   * function AddInjectGenotype(const cGenome & in_genome, 
   * cInjectGenotype * parent_injectgenotype = NULL), which then calls this one.
   **/
  void AddInjectGenotype(cInjectGenotype *in_inject_genotype, int in_list_num = -1 );
  cInjectGenotype * AddInjectGenotype(const cGenome & in_genome,
			  cInjectGenotype * parent_inject_genotype = NULL);
  cInjectGenotype * FindInjectGenotype(const cGenome & in_genome) const;
  void RemoveInjectGenotype(cInjectGenotype & in_inject_genotype);
  void ThresholdInjectGenotype(cInjectGenotype & in_inject_genotype);
  bool AdjustInjectGenotype(cInjectGenotype & in_inject_genotype);

  bool SaveClone(std::ofstream & fp);
  bool LoadClone(std::ifstream & fp);
  bool DumpTextSummary(std::ofstream & fp);
  //bool DumpDetailedSummary(std::ofstream & fp);
  bool DumpDetailedSummary(const cString & file, int update);
  /*
  bool DumpHistoricSummary(std::ofstream & fp);
  */
  //void DumpDetailedEntry(cInjectGenotype * inject_genotype, std::ofstream & fp);
  void DumpDetailedEntry(cInjectGenotype * inject_genotype, const cString & file, int update);
  bool OK();

  inline int GetSize() const { return inject_genotype_control->GetSize(); }
  inline cInjectGenotype * GetBestInjectGenotype() const
    { return inject_genotype_control->GetBest(); }
  inline cInjectGenotype * GetCoalescentInjectGenotype() const
    { return inject_genotype_control->GetCoalescent(); }
  
  inline cInjectGenotype * GetInjectGenotype(int thread) const
    { return inject_genotype_control->Get(thread); }
  inline cInjectGenotype * NextInjectGenotype(int thread) {
    cInjectGenotype * next = inject_genotype_control->Next(thread);
    return (next == inject_genotype_control->GetBest()) ? (cInjectGenotype*)NULL : next;
  }
  inline cInjectGenotype * ResetThread(int thread)
    { return inject_genotype_control->Reset(thread); }

  int CountNumCreatures();
  inline int FindPos(cInjectGenotype & in_inject_genotype, int max_depth = -1)
    { return inject_genotype_control->FindPos(in_inject_genotype, max_depth); }
   unsigned int FindCRC(const cGenome & in_genome) const;
};

#endif
