//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "population.hh"
#include "stats.hh"
#include "environment.hh"

#include "bar_screen.hh"


using namespace std;


///////////////////
// The Bar Screen
///////////////////

void cBarScreen::Draw()
{
  SetBoldColor(COLOR_WHITE);

  Box();
  VLine(18);
  

  int offset = prog_name.GetSize() + 4;
  VLine(Width() - offset - 2);
  Print(1, Width() - offset, "%s", prog_name());

  Print(1, 2, "Update:");

  if(info.GetPopulation().GetEnvironment().GetResourceLib().GetSize() > 0)
    Print(1, 20, "[M]ap [S]tats [O]ptions [Z]oom [E]nviron [Q]uit");
  else
    Print(1, 20, "[M]ap  [S]tats  [O]ptions  [Z]oom  [Q]uit");

  SetBoldColor(COLOR_CYAN);

  if(info.GetPopulation().GetEnvironment().GetResourceLib().GetSize() > 0)
    {
      Print(1, 21, 'M');
      Print(1, 27, 'S');
      Print(1, 35, 'O');
      Print(1, 45, 'Z');
      Print(1, 52, 'E');
      Print(1, 62, 'Q');
    }
  else
    {
      Print(1, 21, 'M');
      Print(1, 28, 'S');
      Print(1, 37, 'O');
      Print(1, 48, 'Z');
      Print(1, 56, 'Q');
    }

  Refresh();
}

void cBarScreen::Update()
{
  SetBoldColor(COLOR_WHITE);
  Print(1, 11, "%d", population.GetUpdate());
  SetColor(COLOR_WHITE);

  Refresh();
}

