//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cViewInfo_h
#define cViewInfo_h

#include "cMerit.h"
#include "cInstSet.h"
#include "defs.h"
#include "cTextWindow.h"


class cEnvironment;
class cSpecies;
class cGenotype;
class cPopulation;
class cPopulationCell;
class cOrganism;
class cInjectGenotype;

#define NUM_SYMBOLS 12
#define SYMBOL_THRESHOLD 10

#define PAUSE_OFF               0
#define PAUSE_ON                1
#define PAUSE_ADVANCE_STEP      2
#define PAUSE_ADVANCE_UPDATE    3
#define PAUSE_ADVANCE_GESTATION 4


#define MAP_BASIC      0
#define MAP_SPECIES    1
#define MAP_BREED_TRUE 2
#define MAP_PARASITE   3
#define MAP_MUTATIONS  4
#define MAP_THREAD     5
#define MAP_INJECT     6
#define MAP_LINEAGE    7
#define NUM_MAPS       8

// Other map modes currently inactive...
#define MAP_COMBO      9
#define MAP_RESOURCE  10
#define MAP_AGE       11
#define NUM_MAP_MODES 12

#include "cWorld.h"
#include "cView_Base.h"

class cViewInfo {
private:
  cWorld* m_world;
  cView_Base* m_view;
  cPopulationCell * active_cell;

  int pause_level;
  int thread_lock;
  int step_organism_id;
  int map_mode;

  // Instruction Libraries.
  cInstSet const * saved_inst_set;

  // Symbol information
  cGenotype * genotype_chart[NUM_SYMBOLS];
  cSpecies * species_chart[NUM_SYMBOLS];
  cInjectGenotype * inject_genotype_chart[NUM_SYMBOLS];
  char symbol_chart[NUM_SYMBOLS];

  tArray<char> map;
  tArray<char> color_map;

  inline bool InGenChart(cGenotype * in_gen);
  inline bool InSpeciesChart(cSpecies * in_species);
  inline bool InInjectGenChart(cInjectGenotype * in_gen);
  void AddGenChart(cGenotype * in_gen);
  void AddSpeciesChart(cSpecies * in_species);
  void AddInjectGenChart(cInjectGenotype * in_gen);
public:
  cViewInfo(cWorld* world, cView_Base * view);
  ~cViewInfo() { ; }

  void UpdateSymbols();
  void SetupSymbolMaps(int map_mode, bool use_color);
  tArray<char> & GetMap() { return map; }
  tArray<char> & GetColorMap() { return color_map; }
  char & MapSymbol(int pos) { return map[pos]; }
  char & ColorSymbol(int pos) { return color_map[pos]; }
  int GetMapMode() { return map_mode; }
  void IncMapMode() { ++map_mode%=NUM_MAPS; }
  void DecMapMode();
    
  void EngageStepMode();
  void DisEngageStepMode();

  cPopulation & GetPopulation() { return m_world->GetPopulation(); }
  cAvidaConfig& GetConfig() { return m_world->GetConfig(); }
  cRandom& GetRandom() { return m_world->GetRandom(); }
  cWorld& GetWorld() { return *m_world; }
  cView_Base& GetView() { return *m_view; }

  int GetNumSymbols() { return NUM_SYMBOLS; }
  cGenotype * GetGenotype(int index) { return genotype_chart[index]; }
  cSpecies * GetSpecies(int index) { return species_chart[index]; }
  cInjectGenotype * GetInjectGenotype(int index) { return inject_genotype_chart[index]; }

  cPopulationCell * GetActiveCell() { return active_cell; }

  cGenotype * GetActiveGenotype();
  cSpecies * GetActiveSpecies();
  cString GetActiveName();

  int GetActiveID();
  int GetActiveGenotypeID();
  int GetActiveSpeciesID();

  void SetActiveCell(cPopulationCell * in_cell) { active_cell = in_cell; }

  int GetPauseLevel() { return pause_level; }
  int GetThreadLock() { return thread_lock; }
  int GetStepOrganism() { return step_organism_id; }

  void SetPauseLevel(int in_level) { pause_level = in_level; }
  void SetThreadLock(int in_lock) { thread_lock = in_lock; }
  void SetStepOrganism(int in_id) { step_organism_id = in_id; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nViewInfo {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

inline void cViewInfo::DecMapMode()
{
  map_mode+=NUM_MAPS; 
  --map_mode %= NUM_MAPS; 
}

inline bool cViewInfo::InGenChart(cGenotype * in_gen)
{
  for (int i = 0; i < NUM_SYMBOLS; i++) {
    if (genotype_chart[i] == in_gen) return true;
  }
  return false;
}

inline bool cViewInfo::InSpeciesChart(cSpecies * in_species)
{
  for (int i = 0; i < NUM_SYMBOLS; i++) {
    if (species_chart[i] == in_species) return true;
  }
  return false;
}

inline bool cViewInfo::InInjectGenChart(cInjectGenotype * in_gen)
{
  for (int i = 0; i < NUM_SYMBOLS; i++) {
    if (inject_genotype_chart[i] == in_gen) return true;
  }
  return false;
}

#endif
