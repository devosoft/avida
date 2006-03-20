/*
 *  cInjectGenotype.h
 *  Avida
 *
 *  Created by David on 11/15/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cInjectGenotype_h
#define cInjectGenotype_h

#include <fstream>

#ifndef cGenome_h
#include "cGenome.h"
#endif
#ifndef cInjectGenotype_BirthData_h
#include "cInjectGenotype_BirthData.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cWorld;

class cInjectGenotype {
private:
  friend class cClassificationManager;
  
  cWorld* m_world;
  cGenome genome;
  cString name;
  bool flag_threshold;
  bool is_active;      // Is this genotype still alive?
  bool can_reproduce;  // Can this genotype reproduce?
  int defer_adjust;    // Don't adjust in the archive until all are cleared.

  int id_num;
  char symbol;

  cInjectGenotype_BirthData birth_data;

  // Statistical info
  int num_injected;
  int last_num_injected;
  int total_injected;

  // Data Structure stuff...
  cInjectGenotype * next;
  cInjectGenotype * prev;

  void CalcTestStats() const;

  // Private Constructor
  cInjectGenotype(cWorld* world, int in_update_born = 0, int in_id = -1);
  
  cInjectGenotype(); // @not_implemented
  cInjectGenotype(const cInjectGenotype&); // @not_implemented
  cInjectGenotype& operator=(const cInjectGenotype&); // @not_implemented
  
public:
  ~cInjectGenotype();

  bool SaveClone(std::ofstream& fp);
  bool LoadClone(std::ifstream & fp);
  bool OK();
  void UpdateReset();

  void SetGenome(const cGenome & in_genome);

  void SetParent(cInjectGenotype * parent);
  void SetUpdateBorn (int update) { birth_data.update_born = update; }
  void SetName(cString in_name)     { name = in_name; }
  void SetNext(cInjectGenotype * in_next) { next = in_next; }
  void SetPrev(cInjectGenotype * in_prev) { prev = in_prev; }
  void SetSymbol(char in_symbol) { symbol = in_symbol; }
  void SetCanReproduce() { can_reproduce = true; }
  inline void SetThreshold();
  void IncDeferAdjust() { defer_adjust++; }
  void DecDeferAdjust() { defer_adjust--; assert(defer_adjust >= 0); }
  
  //// Properties Native to Genotype ////
  cGenome & GetGenome()             { return genome; }
  const cGenome & GetGenome() const { return genome; }
  int GetLength()             const { return genome.GetSize(); }
  
  // For tracking the genotype line back to the ancestor...
  cInjectGenotype * GetParentGenotype() { return birth_data.parent_genotype; }
  int GetNumOffspringGenotypes() const { return birth_data.num_offspring_genotypes; }
  void AddOffspringGenotype() { birth_data.num_offspring_genotypes++; }
  void RemoveOffspringGenotype() { birth_data.num_offspring_genotypes--; }
  bool GetActive() const { return is_active; }
  int GetUpdateDeactivated() { return birth_data.update_deactivated; }
  void Deactivate(int update);

  bool CanReproduce()           { return can_reproduce; }
  int GetUpdateBorn()           { return birth_data.update_born; }
  int GetParentID()             { return birth_data.parent_id; }
  int GetDepth()                { return birth_data.gene_depth; }
  cString & GetName()           { return name; }
  cInjectGenotype * GetNext()         { return next; }
  cInjectGenotype * GetPrev()         { return prev; }
  bool GetThreshold() const     { return flag_threshold; }
  int GetID() const             { return id_num; }
  char GetSymbol() const        { return symbol; }

  int AddParasite();
  int RemoveParasite();
  int GetNumInjected()    { return num_injected; }
  int GetTotalInjected()  { return total_injected; }
};


inline void cInjectGenotype::SetThreshold()
{
  flag_threshold = true;
  if (symbol == '.') symbol = '+';
}

#endif
