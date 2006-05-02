/*
 *  cGenotypeControl.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

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
  
  cGenotypeControl(); // @not_implemented
  cGenotypeControl(const cGenotypeControl&); // @not_implemented
  cGenotypeControl& operator=(const cGenotypeControl&); // @not_implemented
  
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


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif
