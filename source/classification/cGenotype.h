/*
 *  cGenotype.h
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#ifndef cGenotype_h
#define cGenotype_h

#include <fstream>

#ifndef cDoubleSum_h
#include "cDoubleSum.h"
#endif
#ifndef cGenome_h
#include "cGenome.h"
#endif
#ifndef cGenotype_BirthData_h
#include "cGenotype_BirthData.h"
#endif
#ifndef cGenotype_TestData_h
#include "cGenotype_TestData.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cSpecies;
class cMerit;
class cWorld;

class cGenotype {
private:
  friend class cClassificationManager;

  cWorld* m_world;
  cGenome genome;
  cString name;
  bool flag_threshold;
  bool is_active;      // Is this genotype still alive?
  int defer_adjust;    // Don't adjust in the archive until all are cleared.

  int id_num;
  char symbol;

  mutable cGenotype_TestData test_data;
  cGenotype_BirthData birth_data;

  // Statistical info

  int num_organisms;
  int last_num_organisms;
  int total_organisms;
  int total_parasites;

  cSpecies* species;

  // Data Structure stuff...
  cGenotype * next;
  cGenotype * prev;


  ////// Statistical info //////

  // Collected on Divides
  cDoubleSum sum_copied_size;
  cDoubleSum sum_exe_size;

  cDoubleSum sum_gestation_time;
  cDoubleSum sum_repro_rate;  // should make gestation obsolete (not new)

  cDoubleSum sum_merit;
  cDoubleSum sum_fitness;

  // Temporary (Approx stats used before any divides done)
  // Set in "SetParent"
  cDoubleSum tmp_sum_copied_size;
  cDoubleSum tmp_sum_exe_size;

  cDoubleSum tmp_sum_gestation_time;
  cDoubleSum tmp_sum_repro_rate;

  cDoubleSum tmp_sum_merit;
  cDoubleSum tmp_sum_fitness;

  
  void CalcTestStats() const;
  
  cGenotype(cWorld* world, int in_update_born, int in_id);
  
  cGenotype(); // @not_implemented
  cGenotype(const cGenotype&); // @not_implemented
  cGenotype& operator=(const cGenotype&); // @not_implemented

public:
  ~cGenotype();

  bool SaveClone(std::ofstream& fp);
  static cGenotype* LoadClone(cWorld* world, std::ifstream& fp);
  bool OK();
  void UpdateReset();

  void SetGenome(const cGenome & in_genome);
  void SetSpecies(cSpecies * in_species) { species = in_species; }

  // Test CPU info -- only used with limited options on.
  inline bool GetTestViable() const;
  inline double GetTestFitness() const;
  inline double GetTestMerit() const;
  inline int GetTestGestationTime() const;
  inline int GetTestExecutedSize() const;
  inline int GetTestCopiedSize() const;
  inline double GetTestColonyFitness() const;
  inline int GetTestGenerations() const;

  void SetParent(cGenotype * parent, cGenotype * parent2);
  void SetName(cString in_name)     { name = in_name; }
  void SetNext(cGenotype * in_next) { next = in_next; }
  void SetPrev(cGenotype * in_prev) { prev = in_prev; }
  void SetSymbol(char in_symbol) { symbol = in_symbol; }
  inline void SetThreshold();
  void IncDeferAdjust() { defer_adjust++; }
  void DecDeferAdjust() { defer_adjust--; assert(defer_adjust >= 0); }
  void SetLineageLabel(int in_label) { birth_data.lineage_label = in_label; }

  // Setting New Stats
  void AddCopiedSize      (int in)   { sum_copied_size.Add(in); }
  void AddExecutedSize         (int in)   { sum_exe_size.Add(in); }
  void AddGestationTime   (int in)   { sum_gestation_time.Add(in);
                                       sum_repro_rate.Add(1/(double)in); }
  void AddMerit      (const cMerit & in);
  void RemoveMerit   (const cMerit & in);
  void AddFitness    (double in){
    assert(in >= 0.0);
    sum_fitness.Add(in);
  }
  void RemoveFitness (double in){
    assert(in >= 0.0);
    sum_fitness.Subtract(in);
  }

  //// Properties Native to Genotype ////
  cGenome & GetGenome()             { return genome; }
  const cGenome & GetGenome() const { return genome; }
  int GetLength()             const { return genome.GetSize(); }

  int GetBirths()    const { return birth_data.birth_track.GetTotal(); }
  int GetBreedOut()  const { return birth_data.breed_out_track.GetTotal(); }
  int GetBreedTrue() const { return birth_data.breed_true_track.GetTotal(); }
  int GetBreedIn()   const { return birth_data.breed_in_track.GetTotal(); }

  int GetThisBirths()    const { return birth_data.birth_track.GetCur(); }
  int GetThisBreedOut()  const { return birth_data.breed_out_track.GetCur(); }
  int GetThisBreedTrue() const { return birth_data.breed_true_track.GetCur(); }
  int GetThisBreedIn()   const { return birth_data.breed_in_track.GetCur(); }

  int GetThisDeaths() const { return birth_data.death_track.GetCur(); }

  int GetLastNumOrganisms() const { return last_num_organisms; }
  int GetLastBirths()    const { return birth_data.birth_track.GetLast(); }
  int GetLastBreedOut()  const { return birth_data.breed_out_track.GetLast(); }
  int GetLastBreedTrue() const { return birth_data.breed_true_track.GetLast();}
  int GetLastBreedIn()   const { return birth_data.breed_in_track.GetLast(); }

  inline void SetBreedStats(cGenotype & daughter); // called by ActivateChild

  //// Properties Averaged Over Creatues ////
  double GetCopiedSize()    const { return (sum_copied_size.Count()>0) ?
	   sum_copied_size.Average() : tmp_sum_copied_size.Average(); }
  double GetExecutedSize()  const { return (sum_exe_size.Count()>0) ?
	   sum_exe_size.Average() : tmp_sum_exe_size.Average(); }
  double GetGestationTime() const { return (sum_gestation_time.Count()>0) ?
	   sum_gestation_time.Average() : tmp_sum_gestation_time.Average(); }
  double GetReproRate()     const { return (sum_repro_rate.Count()>0) ?
	   sum_repro_rate.Average() : tmp_sum_repro_rate.Average(); }
  double GetMerit()         const { return (sum_merit.Count()>0) ?
	   sum_merit.Average() : tmp_sum_merit.Average(); }
  double GetFitness()       const { return (sum_fitness.Count()>0) ?
	   sum_fitness.Average() : tmp_sum_fitness.Average(); }


  // For tracking the genotype line back to the ancestor...
  cGenotype* GetParentGenotype() { return birth_data.parent_genotype; }
  cGenotype* GetParent2Genotype() { return birth_data.parent2_genotype; }
  int GetNumOffspringGenotypes() const
    { return birth_data.num_offspring_genotypes; }
  void AddOffspringGenotype() { birth_data.num_offspring_genotypes++; }
  void RemoveOffspringGenotype() { birth_data.num_offspring_genotypes--; }
  bool GetActive() const { return is_active; }
  bool GetDeferAdjust() const { return defer_adjust > 0; }
  int GetUpdateDeactivated() { return birth_data.update_deactivated; }
  void Deactivate(int update);

  int GetUpdateBorn() const     { return birth_data.update_born; }
  int GetParentID() const       { return birth_data.ancestor_ids[0]; }
  int GetAncestorID(int anc) const { return birth_data.ancestor_ids[anc]; }
  int GetParentDistance() const { return birth_data.parent_distance; }
  int GetDepth() const          { return birth_data.gene_depth; }
  int GetLineageLabel() const   { return birth_data.lineage_label; }
  cString & GetName()           { return name; }
  cSpecies * GetSpecies()       { return species; }
  cSpecies * GetParentSpecies() { return birth_data.parent_species; }
  cGenotype * GetNext()         { return next; }
  cGenotype * GetPrev()         { return prev; }
  bool GetThreshold() const     { return flag_threshold; }
  int GetID() const             { return id_num; }
  char GetSymbol() const        { return symbol; }

  // Calculate a crude phylogentic distance based off of tracking parents
  // and grand-parents, including sexual tracking.
  int GetPhyloDistance(cGenotype * test_genotype);

  inline int AddOrganism();
  inline int RemoveOrganism();
  int AddParasite()         { return ++total_parasites; }
  void SwapOrganism()       { total_organisms++; }
  int GetNumOrganisms()     { return num_organisms; }
  int GetTotalOrganisms()   { return total_organisms; }
  int GetTotalParasites()   { return total_parasites; }

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

inline int cGenotype::AddOrganism()
{
  total_organisms++;
  return num_organisms++;
}

inline int cGenotype::RemoveOrganism()
{
  birth_data.death_track.Inc();
  return num_organisms--;
}

inline void cGenotype::SetThreshold()
{
  flag_threshold = true;
  if (symbol == '.') symbol = '+';
}

inline void cGenotype::SetBreedStats(cGenotype & daughter)
{
  birth_data.birth_track.Inc();
  if (daughter.id_num == id_num) {
    birth_data.breed_true_track.Inc();
  } else {
    birth_data.breed_out_track.Inc();
    daughter.birth_data.breed_in_track.Inc();
  }
}

inline bool cGenotype::GetTestViable() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.is_viable;
}


inline double cGenotype::GetTestFitness() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.fitness;
}


inline double cGenotype::GetTestMerit() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.merit;
}


inline int cGenotype::GetTestGestationTime() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.gestation_time;
}


inline int cGenotype::GetTestExecutedSize() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.executed_size;
}


inline int cGenotype::GetTestCopiedSize() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.copied_size;
}


inline double cGenotype::GetTestColonyFitness() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.colony_fitness;
}


inline int cGenotype::GetTestGenerations() const {
  if (test_data.fitness == -1) CalcTestStats();
  return test_data.generations;
}

inline void cGenotype::Deactivate(int update)
{
  is_active = false;
  birth_data.update_deactivated = update;
}


#endif
