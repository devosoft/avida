/*
 *  cInjectGenotypeQueue.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cInjectGenotypeQueue_h
#define cInjectGenotypeQueue_h

#ifndef cInjectGenotypeElement_h
#include "cInjectGenotypeElement.h"
#endif

class cInjectGenotype;
class cGenome;

class cInjectGenotypeQueue {
private:
  int size;
  cInjectGenotypeElement root;

  void Remove(cInjectGenotypeElement* in_element);
  
  cInjectGenotypeQueue(const cInjectGenotypeQueue&); // @not_implemented
  cInjectGenotypeQueue& operator=(const cInjectGenotypeQueue&); // @not_implemented
  
public:
  cInjectGenotypeQueue();
  ~cInjectGenotypeQueue();

  bool OK();

  void Insert(cInjectGenotype & in_inject_genotype);
  void Remove(cInjectGenotype & in_inject_genotype);
  cInjectGenotype * Find(const cGenome & in_genome) const;
};


#ifdef ENABLE_UNIT_TESTS
namespace nInjectGenotypeQueue {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif

#endif
