/*
 *  cBirthChamber.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cBirthChamber_h
#define cBirthChamber_h

#ifndef cCPUMemory_h
#include "cCPUMemory.h"
#endif
#ifndef cMerit_h
#include "cMerit.h"
#endif

/**
 * All genome-based organisms must go through the birth chamber, which will
 * handle any special modifications during the birth process, such as divide
 * mutations reversions, sterilization, and crossover for sex.  If sex is
 * turned on, this class will hold organisms that are waiting to be crossed
 * over before they are born.
 **/

class cAvidaContext;
class cGenome;
class cGenotype;
class cOrganism;
template <class T> class tArray;
class cWorld;

class cBirthChamber {
private:

  class cBirthEntry {
  public:
    cCPUMemory genome;
    cMerit merit;
    cGenotype * parent_genotype;
    bool is_waiting;
  };
  
  cWorld* m_world;

  cBirthEntry global_wait_entry;
  tArray<cBirthEntry> local_wait_entry;
  tArray<cBirthEntry> deme_wait_entry;
  tArray<cBirthEntry> size_wait_entry;
  tArray<cBirthEntry> mate_select_wait_entry;

  // mark whether that instruction has already been swapped 
  // between two genomes; used in modular recombination
  tArray<int> swapped0;
  tArray<int> swapped1;


  // Private methods...
  bool RegionSwap(cCPUMemory& genome0, cCPUMemory& genome1, int start0, int end0, int start1, int end1);
  void GenomeSwap(cCPUMemory& genome0, cCPUMemory& genome1, double& merit0, double& merit1);

  bool DoAsexBirth(cAvidaContext& ctx, const cGenome& child_genome, cOrganism& parent,
                   tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array);
  bool DoPairAsexBirth(cAvidaContext& ctx, const cBirthEntry& old_entry, const cGenome& new_genome, cOrganism& parent,
                       tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array);
  cBirthEntry* FindSexLocalWaiting(cAvidaContext& ctx, const cGenome& child_genome, cOrganism& parent);
  cBirthEntry* FindSexDemeWaiting(const cGenome& child_genome, cOrganism& parent);
  cBirthEntry* FindSexSizeWaiting(const cGenome & child_genome, cOrganism& parent);
  cBirthEntry* FindSexMateSelectWaiting(const cGenome & child_genome, cOrganism& parent);
  cBirthEntry* FindSexGlobalWaiting(const cGenome & child_genome, cOrganism& parent);

  void DoBasicRecombination(cAvidaContext& ctx, cCPUMemory& genome0, cCPUMemory& genome1,
                            double& merit0, double& merit1);
  void DoModularContRecombination(cAvidaContext& ctx, cCPUMemory& genome0, cCPUMemory& genome1,
                                  double& merit0, double& merit1);
  void DoModularNonContRecombination(cAvidaContext& ctx, cCPUMemory& genome0, cCPUMemory& genome1,
                                     double& merit0, double& merit1);
  void DoModularShuffleRecombination(cAvidaContext& ctx, cCPUMemory& genome0, cCPUMemory& genome1,
                                     double& merit0, double& merit1);

  void SetupGenotypeInfo(cOrganism* organism, cGenotype* parent0_genotype, cGenotype* parent1_genotype);

  // Pick a random waiting genome from the nehighborhood for recombination
  int PickRandRecGenome(cAvidaContext& ctx, const int & parent_id, int world_x, int world_y);
  

  cBirthChamber(); // @not_implemented
  cBirthChamber(const cBirthChamber&); // @not_implemented
  cBirthChamber& operator=(const cBirthChamber&); // @not_implemented
  
public:
  cBirthChamber(cWorld* world);
  ~cBirthChamber() { ; }

  // Handle manipulations & tests of genome.  Return false if divide process
  // should halt.  Place offspring in child_array.
  bool SubmitOffspring(cAvidaContext& ctx, const cGenome& child_genome, cOrganism& parent,
                       tArray<cOrganism*>& child_array, tArray<cMerit>& merit_array);

  // Check the neighborhood for waiting genomes
  bool GetNeighborWaiting(const int & parent_id, int world_x, int world_y);


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
