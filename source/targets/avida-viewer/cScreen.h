//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cScreen_h
#define cScreen_h

#ifndef cInstSet_h
#include "cInstSet.h"
#endif
#ifndef cMerit_h
#include "cMerit.h"
#endif
#ifndef cTextWindow_h
#include "cTextWindow.h"
#endif
#ifndef cViewInfo_h
#include "cViewInfo.h"
#endif
#ifndef defs_h
#include "defs.h"
#endif

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

class cScreen : public cTextWindow {
protected:
  cViewInfo& info;

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


#ifdef ENABLE_UNIT_TESTS
namespace nScreen {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  


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
