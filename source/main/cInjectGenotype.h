//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INJECT_GENOTYPE_HH
#define INJECT_GENOTYPE_HH

#include <fstream>

#ifndef GENOME_HH
#include "cGenome.h"
#endif
#ifndef INJECT_GENOTYPE_BIRTH_DATA_HH
#include "cInjectGenotype_BirthData.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif

/*class cInjectGenotype_TestData {
public:
  cGenotype_TestData();
  ~cGenotype_TestData();

  bool is_viable;

  double fitness;
  double merit;
  int gestation_time;
  int executed_size;
  int copied_size;
  double colony_fitness;
  int generations;
  };*/

class cGenome; // aggregate
class cString; // aggregate
class cInjectGenotype_BirthData; // aggregate
class cWorld;

class cInjectGenotype {
private:
  cWorld* m_world;
  cGenome genome;
  cString name;
  bool flag_threshold;
  bool is_active;      // Is this genotype still alive?
  bool can_reproduce;  // Can this genotype reproduce?
  int defer_adjust;    // Don't adjust in the genebank until all are cleared.

  int id_num;
  char symbol;

  //mutable cGenotype_TestData test_data;
  cInjectGenotype_BirthData birth_data;

  // Statistical info

  int num_injected;
  int last_num_injected;
  int total_injected;
  //int total_parasites;

  //cSpecies * species;

  // Data Structure stuff...
  cInjectGenotype * next;
  cInjectGenotype * prev;


  ////// Statistical info //////

  // Collected on Divides
  //cDoubleSum sum_copied_size;
  //cDoubleSum sum_exe_size;

  //cDoubleSum sum_gestation_time;
  //cDoubleSum sum_repro_rate;  // should make gestation obsolete (not new)

  //cDoubleSum sum_merit;
  //cDoubleSum sum_fitness;

  // Temporary (Approx stats used before any divides done)
  // Set in "SetParent"
  //cDoubleSum tmp_sum_copied_size;
  //cDoubleSum tmp_sum_exe_size;

  //cDoubleSum tmp_sum_gestation_time;
  //cDoubleSum tmp_sum_repro_rate;

  //cDoubleSum tmp_sum_merit;
  //cDoubleSum tmp_sum_fitness;

  void CalcTestStats() const;
public:
  /**
   * Constructs an empty genotype. Normally, in_id should not specified as
   * parameter, because cGenotype keeps track of the last id given out, and
   * choses a new one based on that. However, in some cases it is necessary
   * to specify an id (e.g., when loading a history file from disk). Note 
   * that in this case, cGenotype does not check if the id has already been 
   * used before.
   **/
  cInjectGenotype(cWorld* world, int in_update_born = 0, int in_id = -1);
  ~cInjectGenotype();

  bool SaveClone(std::ofstream& fp);
  bool LoadClone(std::ifstream & fp);
  bool OK();
  void Mutate();
  void UpdateReset();

  void SetGenome(const cGenome & in_genome);
  //void SetSpecies(cSpecies * in_species);

  // Test CPU info -- only used with limited options on.
  //bool GetTestViable() const;
  //double GetTestFitness() const;
  //double GetTestMerit() const;
  //int GetTestGestationTime() const;
  //int GetTestExecutedSize() const;
  //int GetTestCopiedSize() const;
  //double GetTestColonyFitness() const;
  //int GetTestGenerations() const;

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

  // Setting New Stats
  //void AddCopiedSize      (int in)   { sum_copied_size.Add(in); }
  //void AddExecutedSize         (int in)   { sum_exe_size.Add(in); }
  //void AddGestationTime   (int in)   { sum_gestation_time.Add(in);
  //                            sum_repro_rate.Add(1/(double)in); }
  //void AddMerit      (const cMerit & in);
  //void RemoveMerit   (const cMerit & in);
  //void AddFitness    (double in){
  //  assert(in >= 0.0);
  //  sum_fitness.Add(in);
  //}
  //void RemoveFitness (double in){
  //  assert(in >= 0.0);
  //  sum_fitness.Subtract(in);
  //}
  
  //// Properties Native to Genotype ////
  cGenome & GetGenome()             { return genome; }
  const cGenome & GetGenome() const { return genome; }
  int GetLength()             const { return genome.GetSize(); }
  
  //int GetBirths()    const { return birth_data.birth_track.GetTotal(); }
  //int GetBreedOut()  const { return birth_data.breed_out_track.GetTotal(); }
  //int GetBreedTrue() const { return birth_data.breed_true_track.GetTotal(); }
  //int GetBreedIn()   const { return birth_data.breed_in_track.GetTotal(); }
  
  //int GetThisBirths()    const { return birth_data.birth_track.GetCur(); }
  //int GetThisBreedOut()  const { return birth_data.breed_out_track.GetCur(); }
  //int GetThisBreedTrue() const { return birth_data.breed_true_track.GetCur(); }
  //int GetThisBreedIn()   const { return birth_data.breed_in_track.GetCur(); }
  
  //int GetThisDeaths() const { return birth_data.death_track.GetCur(); }
  
  //int GetLastNumOrganisms() const { return last_num_organisms; }
  //int GetLastBirths()    const { return birth_data.birth_track.GetLast(); }
  //int GetLastBreedOut()  const { return birth_data.breed_out_track.GetLast(); }
  //int GetLastBreedTrue() const { return birth_data.breed_true_track.GetLast();}
  //int GetLastBreedIn()   const { return birth_data.breed_in_track.GetLast(); }
  
  //inline void SetBreedStats(cGenotype & daughter); // called by ActivateChild
  
  //// Properties Averaged Over Creatues ////
  //double GetCopiedSize()    const { return (sum_copied_size.Count()>0) ?
  //   sum_copied_size.Average() : tmp_sum_copied_size.Average(); }
  //double GetExecutedSize()  const { return (sum_exe_size.Count()>0) ?
  //   sum_exe_size.Average() : tmp_sum_exe_size.Average(); }
  //double GetGestationTime() const { return (sum_gestation_time.Count()>0) ?
  //   sum_gestation_time.Average() : tmp_sum_gestation_time.Average(); }
  //double GetReproRate()     const { return (sum_repro_rate.Count()>0) ?
  //   sum_repro_rate.Average() : tmp_sum_repro_rate.Average(); }
  //double GetMerit()         const { return (sum_merit.Count()>0) ?
  //   sum_merit.Average() : tmp_sum_merit.Average(); }
  //double GetFitness()       const { return (sum_fitness.Count()>0) ?
  //   sum_fitness.Average() : tmp_sum_fitness.Average(); }
  
  
  // For tracking the genotype line back to the ancestor...
  cInjectGenotype * GetParentGenotype() { return birth_data.parent_genotype; }
  int GetNumOffspringGenotypes() const
    { return birth_data.num_offspring_genotypes; }
  void AddOffspringGenotype() { birth_data.num_offspring_genotypes++; }
  void RemoveOffspringGenotype() { birth_data.num_offspring_genotypes--; }
  bool GetActive() const { return is_active; }
  // bool GetDeferAdjust() const { return defer_adjust > 0; }
  int GetUpdateDeactivated() { return birth_data.update_deactivated; }
  void Deactivate(int update);

  bool CanReproduce()           { return can_reproduce; }
  int GetUpdateBorn()           { return birth_data.update_born; }
  int GetParentID()             { return birth_data.parent_id; }
  //int GetParentDistance()       { return birth_data.parent_distance; }
  int GetDepth()                { return birth_data.gene_depth; }
  cString & GetName()           { return name; }
  cInjectGenotype * GetNext()         { return next; }
  cInjectGenotype * GetPrev()         { return prev; }
  bool GetThreshold() const     { return flag_threshold; }
  int GetID() const             { return id_num; }
  char GetSymbol() const        { return symbol; }

  int AddParasite();
  int RemoveParasite();
  //int AddParasite()        { return ++total_parasites; }
  //void SwapOrganism()      { total_organisms++; }
  int GetNumInjected()    { return num_injected; }
  int GetTotalInjected()  { return total_injected; }
  //int GetTotalParasites()  { return total_parasites; }
};

// The genotype pointer template...



// All the inline stuff...

  ////////////////
 //  cGenotype //
////////////////

inline void cInjectGenotype::SetThreshold()
{
  flag_threshold = true;
  if (symbol == '.') symbol = '+';
}

/*
inline void cGenotype::SetBreedStats(cGenotype & daughter)
{
  birth_data.birth_track.Inc();
  if (daughter.id_num == id_num) {
    birth_data.breed_true_track.Inc();
  } else {
    birth_data.breed_out_track.Inc();
    daughter.birth_data.breed_in_track.Inc();
  }
}*/

#endif
