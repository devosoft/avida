//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef SPECIES_HH
#define SPECIES_HH

#include <fstream>

#ifndef GENOME_HH
#include "cGenome.h"
#endif

#define SPECIES_QUEUE_NONE     0
#define SPECIES_QUEUE_ACTIVE   1
#define SPECIES_QUEUE_INACTIVE 2
#define SPECIES_QUEUE_GARBAGE  3

#define SPECIES_MAX_DISTANCE 20

class cGenotype;

class cSpecies {
private:
  int id_num;
  int parent_id;
  cGenome genome;
  int update_born;

  int total_organisms;
  int total_genotypes;
  int num_threshold;
  int num_genotypes;
  int num_organisms;
  int queue_type;
  char symbol;
  int genotype_distance[SPECIES_MAX_DISTANCE];

  cSpecies * next;
  cSpecies * prev;
public:
  cSpecies(const cGenome & in_genome, int update);
  ~cSpecies();

  int Compare(const cGenome & test_genome, int max_fail_count=-1);
  bool OK();

  void AddThreshold(cGenotype & in_genotype);
  void RemoveThreshold(cGenotype & in_genotype);
  void AddGenotype();
  void RemoveGenotype();

  void AddOrganisms(int in_num) { num_organisms += in_num; }
  void ResetStats() { num_organisms = 0; }

  cSpecies * GetNext() { return next; }
  cSpecies * GetPrev() { return prev; }

  int GetID() { return id_num; }
  int GetParentID() { return parent_id; }
  const cGenome & GetGenome() { return genome; }
  int GetUpdateBorn() { return update_born; }
  int GetNumGenotypes() { return num_genotypes; }
  int GetNumThreshold() { return num_threshold; }
  int GetNumOrganisms() { return num_organisms; }
  int GetTotalOrganisms() { return total_organisms; }
  int GetTotalGenotypes() { return total_genotypes; }
  int GetQueueType() { return queue_type; }
  char GetSymbol() { return symbol; }

  void SetQueueType(int in_qt) { queue_type = in_qt; }
  void SetNext(cSpecies * in_next) { next = in_next; }
  void SetPrev(cSpecies * in_prev) { prev = in_prev; }
  void SetSymbol(char in_symbol) { symbol = in_symbol; }
  void SetParentID(int in_id) { parent_id = in_id; }

#ifdef DEBUG
  // These are used in cGenebank::OK()
  int debug_num_genotypes;
  int debug_num_threshold;
#endif
};

#endif
