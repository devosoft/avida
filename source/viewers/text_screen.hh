//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_TEXT_SCREEN_HH
#define VIEW_TEXT_SCREEN_HH


#include "merit.hh"
#include "inst_set.hh"
#include "defs.hh"

#include "ncurses.hh"
#include "ansi.hh"

class cEnvironment;
class cSpecies;
class cGenotype;
class cPopulation;
class cPopulationCell;
class cOrganism;
class cGenebank;
class cInjectGenotype;
class cInjectGenebank;

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


class cViewInfo {
private:
  cPopulation & population;
  cPopulationCell * active_cell;

  int pause_level;
  int thread_lock;
  int step_organism_id;
  int map_mode;

  // Instruction Libraries.
  cInstSet const * saved_inst_set;
  cInstSet alt_inst_set;

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
  cViewInfo(cPopulation & in_population);
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

  cGenebank & GetGenebank();
  cPopulation & GetPopulation() { return population; }

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

class cScreen : public cTextWindow {
protected:
  cViewInfo & info;

  inline void SetSymbolColor(char color);
  inline void PrintMerit(int in_y, int in_x, cMerit in_merit);
  inline void PrintFitness(int in_y, int in_x, double in_fitness);
public:
  cScreen(int y_size, int x_size, int y_start, int x_start,
	  cViewInfo & in_info) :
    cTextWindow(y_size, x_size, y_start, x_start), info(in_info) { ; }
  virtual ~cScreen() { ; }

  virtual void Draw() = 0;
  virtual void Update() = 0;
  virtual void AdvanceUpdate() { ; }
  virtual void DoInput(int in_char) = 0;
  virtual void DoMouseClick(int x, int y) { (void) x; (void) y; }
  virtual void Exit() { ; }
};



///////////////
//  cViewInfo
///////////////

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


/////////////
//  cScreen
/////////////

inline void cScreen::SetSymbolColor(char color)
{
  switch (color) {
  case '.':
  case '-':
    SetColor(COLOR_WHITE);
    break;
  case '*':
  case '+':
  case '1':
    SetBoldColor(COLOR_WHITE);
    break;
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
    SetBoldColor((int) color - 'A' + 1);
    break;
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
    SetColor((int) color - 'G' + 1);
    break;
  case ' ':
    SetColor(COLOR_OFF);
    break;
  case '0':
  default:
    SetColor(COLOR_WHITE);
    break;
  }
}

inline void cScreen::PrintMerit(int in_y, int in_x, cMerit in_merit)
{
  // if we can print the merit normally, do so.
  //if (in_merit.GetDouble() < 1000000) {
    //Print(in_y, in_x, "%7d", in_merit.GetUInt());
  //}

  // otherwise use scientific notation. (or somesuch)
  //else {
    Print(in_y, in_x, "%7.1e", in_merit.GetDouble());
  //}
}

inline void cScreen::PrintFitness(int in_y, int in_x, double in_fitness)
{
  // If we can print the fitness, do so!
  if (in_fitness <= 0.0) {
    Print(in_y, in_x, " 0.0000");
  }
  else if (in_fitness < 10)
    Print(in_y, in_x, "%7.4f", in_fitness);
  //  else if (in_fitness < 100)
  //    Print(in_y, in_x, "%7.3f", in_fitness);
  else if (in_fitness < 1000)
    Print(in_y, in_x, "%7.2f", in_fitness);
  //  else if (in_fitness < 10000)
  //    Print(in_y, in_x, "%7.1f", in_fitness);
  else if (in_fitness < 100000)
    Print(in_y, in_x, "%7.0f", in_fitness);

  // Otherwise use scientific notations.
  else
    Print(in_y, in_x, "%7.1e", in_fitness);
}

#endif
