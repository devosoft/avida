/*
 *  cSpeciesControl.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cSpeciesControl_h
#define cSpeciesControl_h

#ifndef cSpeciesQueue_h
#include "cSpeciesQueue.h"
#endif

class cGenotype;
class cSpecies;
class cStats;
class cWorld;

class cSpeciesControl {
private:
  cWorld* m_world;
  cSpeciesQueue active_queue;
  cSpeciesQueue inactive_queue;
  cSpeciesQueue garbage_queue;
  

  cSpeciesControl(); // @not_implemented
  cSpeciesControl(const cSpeciesControl&); // @not_implemented
  cSpeciesControl& operator=(const cSpeciesControl&); // @not_implemented

public:
  cSpeciesControl(cWorld* world) : m_world(world) { ; }
  ~cSpeciesControl() { ; }

  void Remove(cSpecies & in_species);
  void Adjust(cSpecies & in_species);
  void SetInactive(cSpecies & in_species);
  void SetActive(cSpecies & in_species);
  void SetGarbage(cSpecies & in_species);
  void Purge(cStats & stats);

  bool OK();

  int FindPos(cSpecies & in_species, int max_depth = -1);
  cSpecies * Find(cGenotype & in_genotype, int record_level);

  inline cSpecies * GetFirst() const { return active_queue.GetFirst(); }
  inline cSpecies * GetFirstInactive() const
    { return inactive_queue.GetFirst(); }
  inline cSpecies * GetFirstGarbage() const
    { return garbage_queue.GetFirst(); }
  inline int GetSize() const { return active_queue.GetSize(); }
  inline int GetInactiveSize() const { return inactive_queue.GetSize(); }


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
