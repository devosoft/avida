//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cBarScreen.h"

#include "cPopulation.h"
#include "cStats.h"
#include "cEnvironment.h"

using namespace std;

int cBarScreen::AddMenuOption(const cString option, int max_x, int cur_x)
{
  if (cur_x + option.GetSize() >= max_x) return cur_x;

  PrintOption(1, cur_x, option);

  return cur_x+option.GetSize()+1;
}

void cBarScreen::Draw(cAvidaContext& ctx)
{
  SetBoldColor(COLOR_WHITE);

  Box();
  VLine(18);
  

  int offset = prog_name.GetSize() + 4;
  VLine(Width() - offset - 2);
  Print(1, Width() - offset+1, "%s", static_cast<const char*>(prog_name));

  Print(1, 2, "Update:");

  const int max_x = Width() - offset - 10;
  int cur_x = 21;

  // Include options in their general order of importance.
  cur_x = AddMenuOption("[M]ap ", max_x, cur_x);
  cur_x = AddMenuOption("[S]tats", max_x, cur_x);
  // cur_x = AddMenuOption("[A]nalyze", max_x, cur_x);
  cur_x = AddMenuOption("[Z]oom", max_x, cur_x);
  cur_x = AddMenuOption("[O]ptions", max_x, cur_x);
  cur_x = AddMenuOption("[H]ist", max_x, cur_x);
  cur_x = AddMenuOption("[E]nv ", max_x, cur_x);
  cur_x = AddMenuOption("[P]ause", max_x, cur_x);
  cur_x = AddMenuOption("[B]lank", max_x, cur_x);
  cur_x = AddMenuOption("[C]hoose CPU", max_x, cur_x);

  // Always place Quit as the last option.
  cur_x = AddMenuOption("[Q]uit", max_x+8, cur_x);

  Refresh();
}

void cBarScreen::Update(cAvidaContext& ctx)
{
  SetBoldColor(COLOR_WHITE);
  Print(1, 11, "%d", m_world->GetStats().GetUpdate());
  SetColor(COLOR_WHITE);

  Refresh();
}

