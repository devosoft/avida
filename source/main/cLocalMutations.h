/*
 *  cLocalMutations.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cLocalMutations_h
#define cLocalMutations_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cMutationLib;
class cLocalMutations
{
private:
  const cMutationLib& mut_lib;
  tArray<double> rates;
  tArray<int> counts;
  
  
  cLocalMutations(); // @not_implemented
  cLocalMutations(const cLocalMutations&); // @not_implemented
  cLocalMutations& operator=(const cLocalMutations&); // @not_implemented
  
public:
  cLocalMutations(const cMutationLib& _lib, int genome_length);
  ~cLocalMutations() { ; }

  const cMutationLib& GetMutationLib() const { return mut_lib; }
  double GetRate(int id) const { return rates[id]; }
  int GetCount(int id) const { return counts[id]; }

  void IncCount(int id) { counts[id]++; }
  void IncCount(int id, int num_mut) { counts[id] += num_mut; }


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
