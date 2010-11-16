//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cScreen_h
#define cScreen_h

#include "Avida.h"

#include "cInstSet.h"
#include "cMerit.h"
#include "cTextWindow.h"
#include "cViewInfo.h"

class cAvidaContext;
class cEnvironment;
class cSpecies;
class cGenotype;
class cPopulation;
class cPopulationCell;
class cOrganism;

// #define NUM_SYMBOLS 12
// #define SYMBOL_THRESHOLD 10

// #define PAUSE_OFF               0
// #define PAUSE_ON                1
// #define PAUSE_ADVANCE_STEP      2
// #define PAUSE_ADVANCE_UPDATE    3
// #define PAUSE_ADVANCE_GESTATION 4


// #define MAP_BASIC      0
// #define MAP_SPECIES    1
// #define MAP_BREED_TRUE 2
// #define MAP_PARASITE   3
// #define MAP_MUTATIONS  4
// #define MAP_THREAD     5
// #define MAP_INJECT     6
// #define MAP_LINEAGE    7
// #define NUM_MAPS       8

// // Other map modes currently inactive...
// #define MAP_COMBO      9
// #define MAP_RESOURCE  10
// #define MAP_AGE       11
// #define NUM_MAP_MODES 12

class cScreen : public cTextWindow {
protected:
  cViewInfo& info;

  inline void SetSymbolColor(char color);
  inline void PrintDouble(int in_y, int in_x, double in_value);
  inline void PrintOption(int in_y, int in_x, const cString & option);

public:
  cScreen(int y_size, int x_size, int y_start, int x_start,
	  cViewInfo & in_info) :
    cTextWindow(y_size, x_size, y_start, x_start), info(in_info) { ; }
  virtual ~cScreen() { ; }

  virtual void Draw(cAvidaContext& ctx) = 0;
  virtual void Update(cAvidaContext& ctx) = 0;
  virtual void AdvanceUpdate() { ; }
  virtual void DoInput(cAvidaContext& ctx, int in_char) = 0;
  virtual void DoMouseClick(int x, int y) { (void) x; (void) y; }
  virtual void Exit() { ; }
};


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

inline void cScreen::PrintDouble(int in_y, int in_x, double in_value)
{
  // if we can print the merit normally, do so.
  if (in_value == 0.0) {
    Print(in_y, in_x, "    0.0");
  }

  if (in_value < 10000000.0) {
    // If this is an integer, print it as such.
    int int_value = (int) in_value;
    if (in_value == ((double) int_value)) {
      Print(in_y, in_x, "%7d", int_value);
    }

    else if (in_value < 10.0) {
      Print(in_y, in_x, "%1.5f", in_value);
    }
    else if (in_value < 100.0) {
      Print(in_y, in_x, "%2.4f", in_value);
    }
    else if (in_value < 1000.0) {
      Print(in_y, in_x, "%3.3f", in_value);
    }
    else if (in_value < 10000.0) {
      Print(in_y, in_x, "%4.2f", in_value);
    }
    else if (in_value < 100000.0) {
      Print(in_y, in_x, "%5.1f", in_value);
    }
    else {
      Print(in_y, in_x, "%7d", int_value);
    }
  }

  // otherwise use scientific notation. (or somesuch)
  else {
    Print(in_y, in_x, "%7.1e", in_value);
  }
}


inline void cScreen::PrintOption(int in_y, int in_x, const cString & option)
{
  // Print the main option...
  SetBoldColor(COLOR_WHITE);
  Print(in_y, in_x, option);

  // Highlight the keypress...
  SetBoldColor(COLOR_YELLOW);
  bool highlight = false;
  for (int i = 0; i < option.GetSize(); i++) {
    if (option[i] == '[') { highlight = true; continue; }
    if (option[i] == ']') { highlight = false; continue; }
    if (highlight == true) Print(in_y, in_x+i, option[i]);
  }
}

#endif
