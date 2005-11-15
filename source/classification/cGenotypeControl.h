//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cGenotypeControl_h
#define cGenotypeControl_h

#ifndef nGenotype_h
#include "nGenotype.h"
#endif

class cGenome;
class cGenotype;
class cWorld;

class cGenotypeControl {
private:
  cWorld* m_world;
  int size;
  cGenotype* best;
  cGenotype* coalescent;
  cGenotype* threads[nGenotype::THREADS];

  cGenotype * historic_list;
  int historic_count;

  void Insert(cGenotype & in_genotype, cGenotype * prev_genotype);
  bool CheckPos(cGenotype & in_genotype);
public:
  cGenotypeControl(cWorld* world);
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
