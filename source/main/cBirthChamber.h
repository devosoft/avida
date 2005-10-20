//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef BIRTH_CHAMBER_HH
#define BIRTH_CHAMBER_HH

#ifndef CPU_MEMORY_HH
#include "cCPUMemory.h"
#endif
#ifndef MERIT_HH
#include "cMerit.h"
#endif

/**
 * All genome-based organisms must go through the birth chamber, which will
 * handle any special modifications during the birth process, such as divide
 * mutations reversions, sterilization, and crossover for sex.  If sex is
 * turned on, this class will hold organisms that are waiting to be crossed
 * over before they are born.
 **/

class cCPUMemory; // aggregate
class cMerit; // aggregate
class cGenebank;
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
  cGenebank* genebank;

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
  bool RegionSwap(cCPUMemory & genome0,
		  cCPUMemory & genome1,
		  int start0, int end0,
		  int start1, int end1);

  bool GenomeSwap(cCPUMemory & genome0, 
		  cCPUMemory & genome1,
		  double & merit0, 
		  double & merit1);

  bool DoAsexBirth(const cGenome & child_genome, cOrganism & parent,
	   tArray<cOrganism *> & child_array, tArray<cMerit> & merit_array);
  bool DoPairAsexBirth(const cBirthEntry & old_entry,
		       const cGenome & new_genome,
		       cOrganism & parent,
		       tArray<cOrganism *> & child_array,
		       tArray<cMerit> & merit_array);
  cBirthEntry * FindSexLocalWaiting(const cGenome & child_genome,
				    cOrganism & parent);
  cBirthEntry * FindSexDemeWaiting(const cGenome & child_genome,
				   cOrganism & parent);
  cBirthEntry * FindSexSizeWaiting(const cGenome & child_genome,
				   cOrganism & parent);
  cBirthEntry * FindSexMateSelectWaiting(const cGenome & child_genome,
					 cOrganism & parent);
  cBirthEntry * FindSexGlobalWaiting(const cGenome & child_genome,
				     cOrganism & parent);

  void DoBasicRecombination(cCPUMemory & genome0, cCPUMemory & genome1, 
			    double & merit0, double & merit1);
  void DoModularContRecombination(cCPUMemory & genome0, cCPUMemory & genome1, 
				  double & merit0, double & merit1);
  void DoModularNonContRecombination(cCPUMemory &genome0, cCPUMemory &genome1, 
				     double & merit0, double & merit1);
  void DoModularShuffleRecombination(cCPUMemory &genome0, cCPUMemory &genome1, 
				     double & merit0, double & merit1);

  void SetupGenotypeInfo(cOrganism * organism, cGenotype * parent0_genotype,
			 cGenotype * parent1_genotype);
public:
  cBirthChamber(cWorld* world);
  ~cBirthChamber();

  void SetGenebank(cGenebank * in_genebank) { genebank = in_genebank; }

  // Handle manipulations & tests of genome.  Return false if divide process
  // should halt.  Place offspring in child_array.
  bool SubmitOffspring(const cGenome & child_genome, cOrganism & parent,
		       tArray<cOrganism *> & child_array,
		       tArray<cMerit> & merit_array);

  // Check the neighborhood for waiting genomes
  bool GetNeighborWaiting(const int & parent_id, int world_x, int world_y);

  // Pick a random waiting genome from the nehighborhood for recombination
  int PickRandRecGenome(const int & parent_id, int world_x, int world_y);

};

#endif
