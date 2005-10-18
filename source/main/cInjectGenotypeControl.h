//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INJECT_GENOTYPE_CONTROL_HH
#define INJECT_GENOTYPE_CONTROL_HH

#ifndef nInjectGenotype_h
#include "nInjectGenotype.h"
#endif

class cGenome;
class cInjectGenotype;
class cInjectGenebank;
class cInjectGenotypeControl {
private:
  int size;
  cInjectGenotype * best;
  cInjectGenotype * coalescent;
  cInjectGenotype * threads[nInjectGenotype::THREADS];
  cInjectGenebank & genebank;

  cInjectGenotype * historic_list;
  int historic_count;

  void Insert(cInjectGenotype & in_inject_genotype, cInjectGenotype * prev_inject_genotype);
  bool CheckPos(cInjectGenotype & in_inject_genotype);
public:
  cInjectGenotypeControl(cInjectGenebank & in_gb);
  ~cInjectGenotypeControl();

  bool OK();
  void Remove(cInjectGenotype & in_inject_genotype);
  void Insert(cInjectGenotype & new_inject_genotype);
  bool Adjust(cInjectGenotype & in_inject_genotype);

  void RemoveHistoric(cInjectGenotype & in_inject_genotype);
  void InsertHistoric(cInjectGenotype & in_inject_genotype);
  int GetHistoricCount() { return historic_count; }

  /*
  int UpdateCoalescent();
  */

  inline int GetSize() const { return size; }
  inline cInjectGenotype * GetBest() const { return best; }
  inline cInjectGenotype * GetCoalescent() const { return coalescent; }

  cInjectGenotype * Find(const cGenome & in_genome) const;
  int FindPos(cInjectGenotype & in_inject_genotype, int max_depth = -1);

  inline cInjectGenotype * Get(int thread) const { return threads[thread]; }
  inline cInjectGenotype * Reset(int thread)
    { return threads[thread] = best; }
  inline cInjectGenotype * ResetHistoric(int thread)
    { return threads[thread] = historic_list; }
  cInjectGenotype * Next(int thread);
  cInjectGenotype * Prev(int thread);
};

#endif
