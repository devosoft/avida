//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cOptionsScreen.h"

#include "cPopulation.h"

using namespace std;

void cOptionsScreen::Draw(cAvidaContext& ctx)
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
  Print(9, 40, "Divide Mut:");
  Print(10,47,        "Ins:");
  Print(11,47,        "Del:");

  Print(1, 0, "Current CPU..:");
  Print(2, 0, "Genotype.....:");
  Print(3, 0, "ID #.........:");

  PrintOption(Height() - 6, 2, "[M]ap Screen");
  PrintOption(Height() - 5, 2, "[S]tats Screen");
  PrintOption(Height() - 4, 2, "[A]nalyze Screen");
  PrintOption(Height() - 3, 2, "[Z]oom Screen");
  PrintOption(Height() - 2, 2, "[H]istogram Screen");

  PrintOption(Height() - 6, 30, "[E]nvironment Screen");
  PrintOption(Height() - 5, 30, "[O]ptions Screen");
  PrintOption(Height() - 4, 30, "[B]lank Screen");
  PrintOption(Height() - 3, 30, "[CTRL-L] Redraw Screen");
  PrintOption(Height() - 2, 30, "[C]hoose New CPU");

  PrintOption(Height() - 6, 55, "E[x]tract Organism");
  PrintOption(Height() - 5, 55, "[W]rite Soup Clone");
  if (info.GetPauseLevel()) {
    PrintOption(Height() - 4, 55, "Un-[P]ause");
    PrintOption(Height() - 3, 55, "[N]ext Update");
  } else {
    PrintOption(Height() - 4, 55, "[P]ause   ");
    PrintOption(Height() - 3, 55, "             ");
  }

  SetColor(COLOR_WHITE);
  Box(Height() - 7, 0, 7, Width(), true);

  SetBoldColor(COLOR_CYAN);
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

  switch(info.GetConfig().BASE_MERIT_METHOD.Get()) {
  case BASE_MERIT_CONST:
    Print(3, 55, "Off");
    break;
  case BASE_MERIT_COPIED_SIZE:
    Print(3, 55, "Copied Size");
    break;
  case BASE_MERIT_EXE_SIZE:
    Print(3, 55, "Executed Size");
    break;
  case BASE_MERIT_FULL_SIZE:
    Print(3, 55, "Full Size");
    break;
  case BASE_MERIT_LEAST_SIZE:
    Print(3, 55, "Least Size");
    break;
  }

  switch(info.GetConfig().BIRTH_METHOD.Get()) {
  case POSITION_OFFSPRING_AGE:
    Print(4, 55, "Replace max age");
    break;
  case POSITION_OFFSPRING_MERIT:
    Print(4, 55, "Replace max age/merit");
    break;
  case POSITION_OFFSPRING_RANDOM:
    Print(4, 55, "Replace random");
    break;
  case POSITION_OFFSPRING_EMPTY:
    Print(4, 55, "Place in empty only");
    break;
  }

  Print(5, 55, "%d", info.GetConfig().AVE_TIME_SLICE.Get());

  PrintDouble(7, 52, info.GetConfig().POINT_MUT_PROB.Get());
  PrintDouble(8, 52, info.GetConfig().COPY_MUT_PROB.Get());
  PrintDouble(9, 52, info.GetConfig().DIVIDE_MUT_PROB.Get());
  PrintDouble(10, 52, info.GetConfig().DIVIDE_INS_PROB.Get());
  PrintDouble(11, 52, info.GetConfig().DIVIDE_DEL_PROB.Get());

  SetColor(COLOR_WHITE);

  Update(ctx);
  Refresh();
}


void cOptionsScreen::Update(cAvidaContext& ctx)
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


