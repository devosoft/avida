//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INJECT_GENOTYPE_QUEUE_HH
#define INJECT_GENOTYPE_QUEUE_HH

#ifndef INJECT_GENOTYPE_ELEMENT_HH
#include "cInjectGenotypeElement.h"
#endif

class cInjectGenotype;
class cGenome;
class cInjectGenotypeQueue {
private:
  int size;
  cInjectGenotypeElement root;

  void Remove(cInjectGenotypeElement * in_element);
public:
  cInjectGenotypeQueue();
  ~cInjectGenotypeQueue();

  bool OK();

  void Insert(cInjectGenotype & in_inject_genotype);
  void Remove(cInjectGenotype & in_inject_genotype);
  cInjectGenotype * Find(const cGenome & in_genome) const;
};

#endif
