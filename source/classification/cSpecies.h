/*
 *  cSpecies.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#ifndef cSpecies_h
#define cSpecies_h

#include <fstream>

#ifndef cGenome_h
#include "cGenome.h"
#endif
#ifndef nSpecies_h
#include "nSpecies.h"
#endif

class cGenotype;
class cWorld;

class cSpecies {
private:
  friend class cClassificationManager;
  
  cWorld* m_world;
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
  int genotype_distance[nSpecies::MAX_DISTANCE];

  cSpecies * next;
  cSpecies * prev;
  
  cSpecies(cWorld* world, const cGenome& in_genome, int update, int in_id);

  cSpecies(); // @not_implemented
  cSpecies(const cSpecies&); // @not_implemented
  cSpecies& operator=(const cSpecies&); // @not_implemented
  
public:
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
  // These are used in cClassificationManager::OK()
  int debug_num_genotypes;
  int debug_num_threshold;
#endif
};

#endif
