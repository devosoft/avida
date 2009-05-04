/*
 *  cGenotype.h
 *  Avida
 *
 *  Called "genotype.hh" prior to 11/30/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cGenotype_h
#define cGenotype_h

#include <cassert>
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


class cAvidaContext;
class cMerit;
class cSpecies;
class cWorld;
class cPhenPlastSummary;

class cGenotype {
private:
  friend class cClassificationManager;

  cWorld* m_world;
  cGenome genome;
  cString name;
  
  struct {
    bool m_flag_threshold:1;
    bool m_is_active:1;      // Is this genotype still alive?
    bool m_track_parent_dist:1;
  };
  int defer_adjust;    // Don't adjust in the archive until all are cleared.

  int id_num;
  char symbol;
  int map_color_id;

  mutable cGenotype_TestData test_data;
  cGenotype_BirthData birth_data;
  mutable cPhenPlastSummary* m_phenplast;
  
  // Statistical info

  int num_organisms;
  int last_num_organisms;
  int total_organisms;
  int total_parasites;

  cSpecies* species;

  // Data Structure stuff...
  cGenotype* next;
  cGenotype* prev;


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

  
  
  void CalcTestStats(cAvidaContext& ctx) const;
  
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
  inline bool GetTestViable(cAvidaContext& ctx) const;
  inline double GetTestFitness(cAvidaContext& ctx) const;
  inline double GetTestMerit(cAvidaContext& ctx) const;
  inline int GetTestGestationTime(cAvidaContext& ctx) const;
  inline int GetTestExecutedSize(cAvidaContext& ctx) const;
  inline int GetTestCopiedSize(cAvidaContext& ctx) const;
  inline double GetTestColonyFitness(cAvidaContext& ctx) const;
  inline int GetTestGenerations(cAvidaContext& ctx) const;
  double GetTaskProbability(cAvidaContext& ctx, int task_id) const;
  tArray<double> GetTaskProbabilities(cAvidaContext& ctx) const;
  
  void TestPlasticity(cAvidaContext& ctx) const;

  void SetParent(cGenotype* parent, cGenotype* parent2);
  void SetName(cString in_name)     { name = in_name; }
  void SetNext(cGenotype* in_next) { next = in_next; }
  void SetPrev(cGenotype* in_prev) { prev = in_prev; }
  void SetSymbol(char in_symbol) { symbol = in_symbol; }
  void SetMapColor(int in_id) { map_color_id = in_id; }
  inline void SetThreshold();
  void IncDeferAdjust() { defer_adjust++; }
  void DecDeferAdjust() { defer_adjust--; assert(defer_adjust >= 0); }
  void SetLineageLabel(int in_label) { birth_data.lineage_label = in_label; }
  void SetExecTimeBorn(int in_exec_born) { birth_data.exec_born = in_exec_born;}  //@MRR
  void SetGenerationBorn(int in_gen_born) {birth_data.generation_born = in_gen_born;} //@MRR
  void SetOrganismIDAtBirth(int org_id)  {birth_data.birth_org_id = org_id;} //@MRR
  void SetOrganismIDAtDeath(int org_id)  {birth_data.death_org_id = org_id;} //@MRR

  // Setting New Stats
  void AddCopiedSize(int in) { sum_copied_size.Add(in); }
  void AddExecutedSize(int in) { sum_exe_size.Add(in); }
  void AddGestationTime(int in) { sum_gestation_time.Add(in);
                                       sum_repro_rate.Add(1/(double)in); }
  void AddMerit(const cMerit& in);
  void RemoveMerit(const cMerit& in);
  void AddFitness(double in) { assert(in >= 0.0); sum_fitness.Add(in); }
  void RemoveFitness(double in) { assert(in >= 0.0); sum_fitness.Subtract(in); }

  //// Properties Native to Genotype ////
  cGenome& GetGenome() { return genome; }
  const cGenome& GetGenome() const { return genome; }
  int GetLength() const { return genome.GetSize(); }

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
  bool GetActive() const { return m_is_active; }
  bool GetDeferAdjust() const { return defer_adjust > 0; }
  int GetUpdateDeactivated() { return birth_data.update_deactivated; }
  void Deactivate(int update, int org_id = -1);

  int GetUpdateBorn() const     { return birth_data.update_born; }
  int GetParentID() const       { return birth_data.ancestor_ids[0]; }
  int GetAncestorID(int anc) const { return birth_data.ancestor_ids[anc]; }
  int GetParentDistance() const { return birth_data.parent_distance; }
  int GetDepth() const          { return birth_data.gene_depth; }
  int GetLineageLabel() const   { return birth_data.lineage_label; }
  cString& GetName()            { return name; }
  cSpecies* GetSpecies()        { return species; }
  cSpecies* GetParentSpecies()  { return birth_data.parent_species; }
  cGenotype* GetNext()          { return next; }
  cGenotype* GetPrev()          { return prev; }
  bool GetThreshold() const     { return m_flag_threshold; }
  int GetID() const             { return id_num; }
  char GetSymbol() const        { return symbol; }
  int GetMapColor() const       { return map_color_id; }
  int GetExecTimeBorn() const   { return birth_data.exec_born; }  //@MRR
  int GetGenerationBorn() const { return birth_data.generation_born; } //@MRR
  int GetOrgIDAtBirth() const   { return birth_data.birth_org_id; } //@MRR
  int GetOrgIDAtDeath() const   { return birth_data.death_org_id; } //@MRR

  // Calculate a crude phylogentic distance based off of tracking parents
  // and grand-parents, including sexual tracking.
  int GetPhyloDistance(cGenotype* test_genotype);

  inline int AddOrganism();
  inline int RemoveOrganism();
  int AddParasite()         { return ++total_parasites; }
  void SwapOrganism()       { total_organisms++; }
  int GetNumOrganisms()     { return num_organisms; }
  int GetTotalOrganisms()   { return total_organisms; }
  int GetTotalParasites()   { return total_parasites; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nGenotype {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  


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
  m_flag_threshold = true;
  if (symbol == '.') symbol = '+';
  if (map_color_id == -2) map_color_id = -1;
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

inline bool cGenotype::GetTestViable(cAvidaContext& ctx) const {
  if (test_data.fitness == -1) CalcTestStats(ctx);
  return test_data.is_viable;
}


inline double cGenotype::GetTestFitness(cAvidaContext& ctx) const {
  if (test_data.fitness == -1) CalcTestStats(ctx);
  return test_data.fitness;
}


inline double cGenotype::GetTestMerit(cAvidaContext& ctx) const {
  if (test_data.fitness == -1) CalcTestStats(ctx);
  return test_data.merit;
}


inline int cGenotype::GetTestGestationTime(cAvidaContext& ctx) const {
  if (test_data.fitness == -1) CalcTestStats(ctx);
  return test_data.gestation_time;
}


inline int cGenotype::GetTestExecutedSize(cAvidaContext& ctx) const {
  if (test_data.fitness == -1) CalcTestStats(ctx);
  return test_data.executed_size;
}


inline int cGenotype::GetTestCopiedSize(cAvidaContext& ctx) const {
  if (test_data.fitness == -1) CalcTestStats(ctx);
  return test_data.copied_size;
}


inline double cGenotype::GetTestColonyFitness(cAvidaContext& ctx) const {
  if (test_data.fitness == -1) CalcTestStats(ctx);
  return test_data.colony_fitness;
}


inline int cGenotype::GetTestGenerations(cAvidaContext& ctx) const {
  if (test_data.fitness == -1) CalcTestStats(ctx);
  return test_data.generations;
}

inline void cGenotype::Deactivate(int update, int org_id)
{
  m_is_active = false;
  birth_data.update_deactivated = update;
  birth_data.death_org_id = org_id;
}


#endif
