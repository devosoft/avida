//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef LOCAL_MUTATIONS_HH
#define LOCAL_MUTATIONS_HH

#ifndef TARRAY_HH
#include "tArray.hh"
#endif

class cMutationLib;
class cLocalMutations {
private:
  const cMutationLib & mut_lib;
  tArray<double> rates;
  tArray<int> counts;
public:
  cLocalMutations(const cMutationLib & _lib, int genome_length);
  ~cLocalMutations();

  const cMutationLib & GetMutationLib() const { return mut_lib; }
  double GetRate(int id) const { return rates[id]; }
  int GetCount(int id) const { return counts[id]; }

  void IncCount(int id) { counts[id]++; }
  void IncCount(int id, int num_mut) { counts[id] += num_mut; }
};

#endif
