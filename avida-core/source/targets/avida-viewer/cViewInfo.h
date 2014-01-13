//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cViewInfo_h
#define cViewInfo_h

#include "avida/Avida.h"
#include "avida/systematics/Group.h"

#include "cInstSet.h"
#include "cMerit.h"
#include "cTextWindow.h"
#include "cView_Base.h"
#include "cWorld.h"

class cEnvironment;
class cPopulation;
class cPopulationCell;
class cOrganism;


using namespace Avida;


#define NUM_SYMBOLS 12
#define SYMBOL_THRESHOLD 10

#define PAUSE_OFF               0
#define PAUSE_ON                1
#define PAUSE_ADVANCE_STEP      2
#define PAUSE_ADVANCE_UPDATE    3
#define PAUSE_ADVANCE_GESTATION 4


#define MAP_BASIC      0
#define MAP_BREED_TRUE 1
#define MAP_PARASITE   2
#define MAP_FORAGER    3
#define MAP_AVATAR     4
#define MAP_TERRITORIES    5
#define MAP_MUTATIONS  6
#define MAP_THREAD     7
#define MAP_INJECT     8
#define MAP_LINEAGE    9
#define NUM_MAPS       10

// Other map modes currently inactive...
#define MAP_COMBO      11
#define MAP_RESOURCE  12
#define MAP_AGE       13
#define NUM_MAP_MODES 14

class sGenotypeViewInfo : public Systematics::GroupData
{
public:
  char symbol;
  
  static const Apto::String ObjectKey;
  bool Serialize(ArchivePtr ar) const;
  
  sGenotypeViewInfo() : symbol(0) { ; }
};

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
  Systematics::GroupPtr genotype_chart[NUM_SYMBOLS];
  char symbol_chart[NUM_SYMBOLS];

  Apto::Array<char> map;
  Apto::Array<char> color_map;

  inline bool InGenChart(Systematics::GroupPtr in_gen);
  void AddGenChart(Systematics::GroupPtr in_gen);

public:
  cViewInfo(cWorld* world, cView_Base * view);
  ~cViewInfo() { ; }

  void UpdateSymbols();
  void SetupSymbolMaps(int map_mode, bool use_color);
  Apto::Array<char> & GetMap() { return map; }
  Apto::Array<char> & GetColorMap() { return color_map; }
  char & MapSymbol(int pos) { return map[pos]; }
  char & ColorSymbol(int pos) { return color_map[pos]; }
  int GetMapMode() { return map_mode; }
  void IncMapMode() { ++map_mode%=NUM_MAPS; }
  void DecMapMode();
    
  void EngageStepMode();
  void DisEngageStepMode();

  cPopulation & GetPopulation() { return m_world->GetPopulation(); }
  cAvidaConfig& GetConfig() { return m_world->GetConfig(); }
  Apto::Random& GetRandom() { return m_world->GetRandom(); }
  cWorld& GetWorld() { return *m_world; }
  cView_Base& GetView() { return *m_view; }

  int GetNumSymbols() { return NUM_SYMBOLS; }
  Systematics::GroupPtr GetGenotype(int index) { return genotype_chart[index]; }

  cPopulationCell * GetActiveCell() { return active_cell; }

  Systematics::GroupPtr GetActiveGenotype();
  cString GetActiveName();

  int GetActiveID();
  int GetActiveGenotypeID();

  void SetActiveCell(cPopulationCell * in_cell) { active_cell = in_cell; }

  int GetPauseLevel() { return pause_level; }
  int GetThreadLock() { return thread_lock; }
  int GetStepOrganism() { return step_organism_id; }

  void SetPauseLevel(int in_level) { pause_level = in_level; }
  void SetThreadLock(int in_lock) { thread_lock = in_lock; }
  void SetStepOrganism(int in_id) { step_organism_id = in_id; }
  
private:
  Apto::SmartPtr<sGenotypeViewInfo> getViewInfo(Systematics::GroupPtr bg);
};


inline void cViewInfo::DecMapMode()
{
  map_mode+=NUM_MAPS; 
  --map_mode %= NUM_MAPS; 
}

inline bool cViewInfo::InGenChart(Systematics::GroupPtr in_gen)
{
  for (int i = 0; i < NUM_SYMBOLS; i++) {
    if (genotype_chart[i] == in_gen) return true;
  }
  return false;
}

#endif
