/*
 *  cSpeciesQueue.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cSpeciesQueue_h
#define cSpeciesQueue_h

#ifndef defs_h
#include "defs.h"
#endif

class cSpecies;

class cSpeciesQueue {
private:
  int size;
  cSpecies* first;
  
  
  cSpeciesQueue(const cSpeciesQueue&); // @not_implemented
  cSpeciesQueue& operator=(const cSpeciesQueue&); // @not_implemented
  
public:
  cSpeciesQueue() : size(0), first(NULL) { ; }
  ~cSpeciesQueue() { ; }

  void InsertRear(cSpecies & new_species);
  void Remove(cSpecies & in_species);
  void Adjust(cSpecies & in_species);
  void Purge();
  bool OK(int queue_type);

  inline int GetSize() const { return size; }
  inline cSpecies * GetFirst() const { return first; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nSpeciesQueue {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
