//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cOptionsScreen.h"

#include "cPopulation.h"

using namespace std;

void cOptionsScreen::Draw()
{
  // Options - Used: ABCEIMOPQRSVWZ

  SetBoldColor(COLOR_WHITE);

  Print(5,  0, "Max Updates..:");
  Print(6,  0, "World Size...:");
  Print(7,  0, "Random Seed..:");
  Print(8,  0, "Threshold....:");

  Print(11, 0, "Inst Set.....:");
  Print(12, 0, "Environment..:");
  Print(13, 0, "Events File..:");

  Print(1, 40, "Time Slicing.:");
  Print(2, 40, "Task Merit...:");
  Print(3, 40, "Size Merit...:");
  Print(4, 40, "Birth Method.:");
  Print(5, 40, "Ave TimeSlice:");

  Print(7, 40, "Point  Mut:");
  Print(8, 40, "Copy   Mut:");
  Print(9, 40, "Divide Mut:      Ins:      Del:");

  Print(1, 0, "Current CPU..:");
  Print(2, 0, "Genotype.....:");
  Print(3, 0, "ID #.........:");

  // SetBoldColor(COLOR_WHITE);
  //  Print(Height() - 5, 2, "-- Screen --");
  Print(Height() - 4, 2, "[H]istogram Screen");
  Print(Height() - 3, 2, "[B]lank Screen");
  Print(Height() - 2, 2, "[CTRL-L] Redraw Screen");

  Print(Height() - 4, 30, "[C]hoose New CPU");
  Print(Height() - 3, 30, "E[x]tract Creature");
  //  Print(Height() - 3, 30, "[I]nject Creature");
  Print(Height() - 2, 30, "[W]rite Soup Clone");

  //  Print(Height() - 5, 55, "[V]iew Instructions");
  if (info.GetPauseLevel()) {
    Print(Height() - 4, 55, "Un-[P]ause");
    Print(Height() - 3, 55, "[N]ext Update");
  } else {
    Print(Height() - 4, 55, "[P]ause   ");
    Print(Height() - 3, 55, "             ");
  }

  SetBoldColor(COLOR_CYAN);
  Print(Height() - 4, 3, 'H');
  Print(Height() - 3, 3, 'B');
  Print(Height() - 2, 3, "CTRL-L");

  Print(Height() - 4, 31, 'C');
  Print(Height() - 3, 32, 'x');
  Print(Height() - 2, 31, 'W');
  if (info.GetPauseLevel()) {
    Print(Height() - 4, 59, 'P');
    Print(Height() - 3, 56, 'N');
  } else {
    Print(Height() - 4, 56, 'P');
  }

  SetColor(COLOR_WHITE);
  Box(0, Height() - 5, Width(), 5);

  SetBoldColor(COLOR_CYAN);
  Print(5, 15, "%d", info.GetConfig().MAX_UPDATES.Get());
  Print(6, 15, "%dx%d", info.GetPopulation().GetWorldX(),
	info.GetPopulation().GetWorldY());
  Print(7, 15, "%d", info.GetRandom().GetSeed());
  Print(8, 15, "%d", info.GetConfig().THRESHOLD.Get());

  Print(11, 15, "%s", static_cast<const char*>(info.GetConfig().INST_SET.Get()));
  Print(12, 15, "%s", static_cast<const char*>(info.GetConfig().ENVIRONMENT_FILE.Get()));
  Print(13, 15, "%s", static_cast<const char*>(info.GetConfig().EVENT_FILE.Get()));

  switch(info.GetConfig().SLICING_METHOD.Get()) {
  case SLICE_CONSTANT:
    Print(1, 55, "Constant");
    break;
  case SLICE_PROB_MERIT:
    Print(1, 55, "Probablistic");
    break;
  case SLICE_INTEGRATED_MERIT:
    Print(1, 55, "Integrated");
    break;
  }

  switch(info.GetConfig().SIZE_MERIT_METHOD.Get()) {
  case SIZE_MERIT_OFF:
    Print(3, 55, "Off");
    break;
  case SIZE_MERIT_COPIED:
    Print(3, 55, "Copied Size");
    break;
  case SIZE_MERIT_EXECUTED:
    Print(3, 55, "Executed Size");
    break;
  case SIZE_MERIT_FULL:
    Print(3, 55, "Full Size");
    break;
  case SIZE_MERIT_LEAST:
    Print(3, 55, "Least Size");
    break;
  }

  switch(info.GetConfig().BIRTH_METHOD.Get()) {
  case POSITION_CHILD_AGE:
    Print(4, 55, "Replace max age");
    break;
  case POSITION_CHILD_MERIT:
    Print(4, 55, "Replace max age/merit");
    break;
  case POSITION_CHILD_RANDOM:
    Print(4, 55, "Replace random");
    break;
  case POSITION_CHILD_EMPTY:
    Print(4, 55, "Place in empty only");
    break;
  }

  Print(5, 55, "%d", info.GetConfig().AVE_TIME_SLICE.Get());

  Print(7, 52, "%.3f", info.GetConfig().POINT_MUT_PROB.Get());
  Print(8, 52, "%.3f", info.GetConfig().COPY_MUT_PROB.Get());
  Print(9, 52, "%.3f", info.GetConfig().DIVIDE_MUT_PROB.Get());
  Print(9, 62, "%.3f", info.GetConfig().DIVIDE_INS_PROB.Get());
  Print(9, 72, "%.3f", info.GetConfig().DIVIDE_DEL_PROB.Get());

  SetColor(COLOR_WHITE);

  Update();
  Refresh();
}


void cOptionsScreen::Update()
{
  if (info.GetActiveCell() == NULL)
    info.SetActiveCell( &(info.GetPopulation().GetCell(0)) );
  if (!info.GetActiveGenotype()) return;

  SetBoldColor(COLOR_CYAN);
  Print(1, 15, "(%d, %d)",
		     info.GetActiveID() % info.GetPopulation().GetWorldX(),
		     info.GetActiveID() / info.GetPopulation().GetWorldY());
  Print(2, 15, "%s", static_cast<const char*>(info.GetActiveName()));
  Print(3, 15, "%d", info.GetActiveID());

  SetColor(COLOR_WHITE);

  Refresh();
}


