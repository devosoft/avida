//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef GENOTYPE_CONTROL_HH
#define GENOTYPE_CONTROL_HH

#ifndef nGenotype_h
#include "nGenotype.h"
#endif

class cGenebank;
class cGenome;
class cGenotype;
class cGenotypeControl {
private:
  int size;
  cGenotype * best;
  cGenotype * coalescent;
  cGenotype * threads[nGenotype::THREADS];
  cGenebank & genebank;

  cGenotype * historic_list;
  int historic_count;

  void Insert(cGenotype & in_genotype, cGenotype * prev_genotype);
  bool CheckPos(cGenotype & in_genotype);
public:
  cGenotypeControl(cGenebank & in_gb);
  ~cGenotypeControl();

  bool OK();
  void Remove(cGenotype & in_genotype);
  void Insert(cGenotype & new_genotype);
  bool Adjust(cGenotype & in_genotype);

  void RemoveHistoric(cGenotype & in_genotype);
  void InsertHistoric(cGenotype & in_genotype);
  int GetHistoricCount() { return historic_count; }

  int UpdateCoalescent();

  inline int GetSize() const { return size; }
  inline cGenotype * GetBest() const { return best; }
  inline cGenotype * GetCoalescent() const { return coalescent; }

  cGenotype * Find(const cGenome & in_genome) const;
  int FindPos(cGenotype & in_genotype, int max_depth = -1);

  inline cGenotype * Get(int thread) const { return threads[thread]; }
  inline cGenotype * Reset(int thread)
    { return threads[thread] = best; }
  inline cGenotype * ResetHistoric(int thread)
    { return threads[thread] = historic_list; }
  cGenotype * Next(int thread);
  cGenotype * Prev(int thread);
};

#endif
