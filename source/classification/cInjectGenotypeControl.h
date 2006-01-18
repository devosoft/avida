/*
 *  cInjectGenotypeControl.h
 *  Avida
 *
 *  Created by David on 11/15/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cInjectGenotypeControl_h
#define cInjectGenotypeControl_h

#ifndef nInjectGenotype_h
#include "nInjectGenotype.h"
#endif

class cGenome;
class cInjectGenotype;
class cWorld;

class cInjectGenotypeControl {
private:
  cWorld* m_world;
  int size;
  cInjectGenotype* best;
  cInjectGenotype* coalescent;
  cInjectGenotype* threads[nInjectGenotype::THREADS];

  cInjectGenotype* historic_list;
  int historic_count;

  void Insert(cInjectGenotype& in_inject_genotype, cInjectGenotype* prev_inject_genotype);
  bool CheckPos(cInjectGenotype& in_inject_genotype);
  
  cInjectGenotypeControl(); // @not_implemented
  cInjectGenotypeControl(const cInjectGenotypeControl&); // @not_implemented
  cInjectGenotypeControl& operator=(const cInjectGenotypeControl&); // @not_implemented

public:
  cInjectGenotypeControl(cWorld* world);
  ~cInjectGenotypeControl();

  bool OK();
  void Remove(cInjectGenotype & in_inject_genotype);
  void Insert(cInjectGenotype & new_inject_genotype);
  bool Adjust(cInjectGenotype & in_inject_genotype);

  void RemoveHistoric(cInjectGenotype & in_inject_genotype);
  void InsertHistoric(cInjectGenotype & in_inject_genotype);
  int GetHistoricCount() { return historic_count; }

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
