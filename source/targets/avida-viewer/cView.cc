//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cView.h"

#include "cEnvironment.h"
#include "cHardwareManager.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cTestCPU.h"
#include "cHardwareBase.h"

#include "cMenuWindow.h"
#include "cTextWindow.h"
#include "cBarScreen.h"
#include "cMapScreen.h"
#include "cStatsScreen.h"
#include "cHistScreen.h"
#include "cOptionsScreen.h"
#include "cZoomScreen.h"
#include "cEnvironmentScreen.h"
#include "cAnalyzeScreen.h"

#include "Platform.h"

#include <csignal>
#include <fstream>

#if AVIDA_PLATFORM(WINDOWS)
# include <process.h>
# define kill(x, y)
#else
# include <unistd.h>
#endif

using namespace std;


cView::cView(cWorld* world) : info(world, this)
{
  Setup(world->GetDefaultContext(), "Avida");

  map_screen     = new cMapScreen     (0,0,3,0,info, world->GetPopulation());
  stats_screen   = new cStatsScreen   (world, 0, 0, 3, 0, info);
  hist_screen    = new cHistScreen    (world, 0, 0, 3, 0, info);
  options_screen = new cOptionsScreen (0,0,3,0,info);
  zoom_screen    = new cZoomScreen    (0,0,3,0,info, world->GetPopulation());
  environment_screen = new cEnvironmentScreen (world, 0, 0, 3, 0, info);
  analyze_screen = new cAnalyzeScreen(world, 0, 0, 3, 0, info);

  info.SetActiveCell( &( world->GetPopulation().GetCell(0) ) );
}

cView::~cView()
{
  if (map_screen) delete map_screen;
  if (stats_screen) delete stats_screen;
  if (hist_screen) delete hist_screen;
  if (options_screen) delete options_screen;
  if (zoom_screen) delete zoom_screen;
  if (environment_screen) delete environment_screen;
  if (analyze_screen) delete analyze_screen;

  EndProg(0);
}

void cView::Setup(cAvidaContext& ctx, const cString & in_name)
{
  cur_screen = NULL;

  // Setup text-interface

  StartProg();

  bar_screen = new cBarScreen(&info.GetWorld(), 3, 0, 0, 0, info, in_name);
  base_window = new cTextWindow(0,0,3,0);
  bar_screen->Draw(ctx);
}

void cView::SetViewMode(int in_mode)
{
  if (in_mode == -1) in_mode = MODE_MAP; // Default to map mode.

  if (in_mode == MODE_BLANK) {
    cur_screen = NULL;
  } else if (in_mode == MODE_MAP) {
    cur_screen = map_screen;
  } else if (in_mode == MODE_STATS) {
    cur_screen = stats_screen;
  } else if (in_mode == MODE_HIST) {
    cur_screen = hist_screen;
  } else if (in_mode == MODE_OPTIONS) {
    cur_screen = options_screen;
  } else if (in_mode == MODE_ZOOM) {
    cur_screen = zoom_screen;
  } else if (in_mode == MODE_ENVIRONMENT) {
    cur_screen = environment_screen;
  } else if (in_mode == MODE_ANALYZE) {
    cur_screen = analyze_screen;
  }
}

void cView::Refresh(cAvidaContext& ctx)
{
  ChangeCurScreen(ctx, cur_screen); bar_screen->Redraw();
}

void cView::Redraw()
{
  bar_screen->Redraw();
  if (cur_screen) cur_screen->Redraw();
  else base_window->Redraw();
}

void cView::NewUpdate(cAvidaContext& ctx)
{
  if (info.GetPauseLevel() == PAUSE_ADVANCE_STEP){
    return;
  }
  NotifyUpdate(ctx);
}

void cView::NotifyUpdate(cAvidaContext& ctx)
{
  bar_screen->Update(ctx);
  info.UpdateSymbols();

  if (cur_screen) cur_screen->Update(ctx);
  DoInputs(ctx);
}

void cView::NotifyError(const cString & in_string)
{
  cString out_string(in_string);
  out_string.Insert("Error: ");
  Notify(out_string);
  EndProg(1);
}

void cView::NotifyWarning(const cString & in_string)
{
  cString out_string(in_string);
  out_string.Insert("Warning: ");
  Notify(out_string);
}

void cView::NotifyComment(const cString & in_string)
{
  if (cur_screen == analyze_screen) analyze_screen->Notify(in_string);
  else Notify(in_string);
}

void cView::NotifyOutput(const cString & in_string)
{
  analyze_screen->Notify(in_string);
  if (cur_screen == analyze_screen) analyze_screen->Refresh();
}

void cView::DoBreakpoint(cAvidaContext& ctx)
{
  if (info.GetPauseLevel() == PAUSE_OFF ||
      info.GetPauseLevel() == PAUSE_ADVANCE_UPDATE) {
    Pause();
    NotifyUpdate(ctx);
  }
}

void cView::DoInputs(cAvidaContext& ctx)
{
  // If we are paused, delay doing anything else until a key is pressed.
  if (info.GetPauseLevel() != PAUSE_OFF) nodelay(stdscr, false);

  // If we are in step-wise mode, "finish" this step!
  if (info.GetPauseLevel() == PAUSE_ADVANCE_STEP) {
    info.DisEngageStepMode();
    info.SetPauseLevel(PAUSE_ON);
  }

  // If there is any input in the buffer, process all of it.
  int cur_char = ERR;
  while ((cur_char = GetInput()) != ERR || info.GetPauseLevel() == PAUSE_ON) {
    bool found_keypress = ProcessKeypress(ctx, cur_char);

    // If we couldn't manage the keypress here, check the current screen.
    if (found_keypress == false && cur_screen) cur_screen->DoInput(ctx, cur_char);
  }

  if (info.GetPauseLevel() == PAUSE_ADVANCE_UPDATE) {
    info.SetPauseLevel(PAUSE_ON);
  }

  nodelay(stdscr, true);
}

bool cView::ProcessKeypress(cAvidaContext& ctx, int keypress)
{
  bool unknown = false;

  switch (keypress) {
  case 'a':
  case 'A':
    ChangeCurScreen(ctx, analyze_screen);
    break;
  case 'b':
  case 'B':
    ChangeCurScreen(ctx, NULL);
    break;
  case 'C':
  case 'c':
    NavigateMapWindow(ctx);
    // Now we need to restore the proper window mode (already cleared)
    ChangeCurScreen(ctx, cur_screen);
    break;
  case 'e':
  case 'E':
    ChangeCurScreen(ctx, environment_screen);
    break;
  case 'h':
  case 'H':
    ChangeCurScreen(ctx, hist_screen);
    break;
  case 'm':
  case 'M':
    ChangeCurScreen(ctx, map_screen);
    break;
  case 'n':
  case 'N':
    if (info.GetPauseLevel() == PAUSE_ON) {
      info.SetPauseLevel(PAUSE_ADVANCE_UPDATE);
      // parasite_zoom = false; // if executing, show code that is running
      info.GetActiveCell()->GetOrganism()->GetPhenotype().SetFault("");
      nodelay(stdscr, true); // Don't delay for input; get to processing.
    }
    if (cur_screen) cur_screen->AdvanceUpdate();
    break;
  case 'o':
  case 'O':
    ChangeCurScreen(ctx, options_screen);
    break;
  case 'p':
  case 'P':
    TogglePause(ctx);
    // We don't want to delay if we're unpaused.
    if (info.GetPauseLevel() == PAUSE_OFF) nodelay(stdscr, true);
    else nodelay(stdscr, false);
    break;
  case 'q':
    if (!Confirm("Are you sure you want to quit?")) break;
  case 'Q':      // Note: Capital 'Q' quits w/o confirming.
    // clear the windows before we go.  Do bar window last to end at top.
    base_window->Redraw();
    bar_screen->Clear();
    bar_screen->Refresh();
    EndProg(0);  // This implementation calls exit(), blowing us clean away
    break;
  case 's':
  case 'S':
    ChangeCurScreen(ctx, stats_screen);
    break;
  case 'W':
  case 'w':
    CloneSoup();
    break;
  case 'X':
  case 'x':
    ExtractCreature();
    break;
  case 'z':
  case 'Z':
    ChangeCurScreen(ctx, zoom_screen);
    break;
  case 3: // CTRL-C...
    exit(0);
    break;
  case 12: // CTRL-L...
    Refresh(ctx);
    break;
  case 26: // CTRL-Z
    kill(getpid(), SIGTSTP);
    break;
  case '*':   // Test Key!!!
    if (true) {
      Confirm("Starting Tests.");
      cMenuWindow menu(50);
      char message[40];
      for (int j = 0; j < 50; j++) {
	sprintf(message, "Line %d", j);
	menu.AddOption(j, message);
      }
      menu.SetActive(3);
      menu.Activate(base_window);
      Redraw();
    }
    break;
  case ERR:
    break;
  default:
    unknown = true;
    break;
  }

  return !unknown;
}

void cView::TogglePause(cAvidaContext& ctx)
{
  // If the run is already paused, un-pause it!
  if (info.GetPauseLevel() != PAUSE_OFF) {
    info.DisEngageStepMode();
    info.SetPauseLevel(PAUSE_OFF);
    if (info.GetActiveCell()->IsOccupied()) {
      info.GetActiveCell()->GetOrganism()->GetPhenotype().SetFault("");
    }
  }

  // Otherwise, turn on the pause.
  else {
    info.SetPauseLevel(PAUSE_ON);
  }

  // Redraw the screen to account for the toggled pause.
  if (cur_screen) cur_screen->Draw(ctx);
}

void cView::CloneSoup()
{
  cString filename;
  filename.Set("detail-%d.spop", info.GetWorld().GetStats().GetUpdate());
  info.GetPopulation().SavePopulation(filename);
  cString message;
  message.Set("Saved population to file: %s", static_cast<const char*>(filename));
  Notify(message);
}

void cView::ExtractCreature()
{
  cBioGroup* cur_gen = info.GetActiveGenotype();
  cString gen_name = cur_gen->GetProperty("name").AsString();

  cGenome mg = cGenome(cur_gen->GetProperty("genome").AsString());
  if (gen_name == "(no name)") gen_name.Set("%03d-unnamed", mg.GetSize());

  if (cur_screen) cur_screen->Print(20, 0, "Extracting %s...", static_cast<const char*>(gen_name));

  cTestCPU* testcpu = info.GetWorld().GetHardwareManager().CreateTestCPU();
  testcpu->PrintGenome(info.GetWorld().GetDefaultContext(), mg, gen_name);
  delete testcpu;

  if (cur_screen) {
    cur_screen->Print(20, 24, "Done.");
    cur_screen->Refresh();
  }
}


void cView::ChangeCurScreen(cAvidaContext& ctx, cScreen* new_screen)
{
  if (cur_screen) cur_screen->Exit();

  cur_screen = new_screen;
  base_window->Redraw();
  bar_screen->Redraw();
  if (cur_screen) {
    cur_screen->Clear();
    cur_screen->Draw(ctx);
  }
}

void cView::PrintMerit(int in_y, int in_x, double in_merit)
{
  // if we can print the merit normally, do so.
  if (in_merit < 1000000.0) {
    if (cur_screen) cur_screen->Print(in_y, in_x, "%d", ((int) in_merit));
  }

  // otherwise use scientific notation. (or somesuch)
  else {
    if (cur_screen) cur_screen->Print(in_y, in_x, "%7.1e", in_merit);
  }
}

void cView::PrintFitness(int in_y, int in_x, double in_fitness)
{
  if (!cur_screen) return;

  // If we can print the fitness, do so!
  if (in_fitness <= 0.0) {
    cur_screen->Print(in_y, in_x, " 0.0000");
  }
  else if (in_fitness < 10)
    cur_screen->Print(in_y, in_x, "%7.4f", in_fitness);
  //  else if (in_fitness < 100)
  //    cur_screen->Print(in_y, in_x, "%7.3f", in_fitness);
  else if (in_fitness < 1000)
    cur_screen->Print(in_y, in_x, "%7.2f", in_fitness);
  //  else if (in_fitness < 10000)
  //    cur_screen->Print(in_y, in_x, "%7.1f", in_fitness);
  else if (in_fitness < 100000)
    cur_screen->Print(in_y, in_x, "%7.0f", in_fitness);

  // Otherwise use scientific notations.
  else
    cur_screen->Print(in_y, in_x, "%7.1e", in_fitness);
}

void cView::NavigateMapWindow(cAvidaContext& ctx)
{
  map_screen->Navigate(ctx);
}


int cView::Confirm(const cString & message)
{
  const int mess_length = message.GetSize();

  // Create a confirm window, and draw it on the screen.

  cTextWindow * conf_win
    = new cTextWindow(3, mess_length + 10, 10, (base_window->Width() - 10 - mess_length) / 2);
  conf_win->Box();
  conf_win->SetBoldColor(COLOR_WHITE);
  conf_win->Print(1, 2, "%s (y/n)", static_cast<const char*>(message));
  conf_win->SetBoldColor(COLOR_CYAN);
  conf_win->Print(1, mess_length + 4, 'y');
  conf_win->Print(1, mess_length + 6, 'n');
  conf_win->SetColor(COLOR_WHITE);
  conf_win->Refresh();

  // Wait for the results.
  bool finished = false;
  bool result = false;
  int cur_char;

  while (finished == false) {
    cur_char = GetInput();
    switch (cur_char) {
    case 'q':
    case 'Q':
    case 'n':
    case 'N':
    case ' ':
    case '\n':
    case '\r':
      finished = true;
      result = false;
      break;
    case 'y':
    case 'Y':
      finished = true;
      result = true;
      break;
    }
  }

  // Delete the window, redraw the screen, and return the results.
  delete conf_win;
  Redraw();
  return result;
}

void cView::Notify(const cString & message)
{
  cString mess_copy(message);

  // Setup all of the individual lines.
  int num_lines = message.CountNumLines();
  cString * line_array = new cString[num_lines];
  int max_width = 0;
  for (int i = 0; i < num_lines; i++) {
    line_array[i] = mess_copy.PopLine();
    if (line_array[i].GetSize() > max_width)
      max_width = line_array[i].GetSize();
  }

  // Create a window and draw it on the screen.

  cTextWindow * notify_win
    = new cTextWindow(2 + num_lines, max_width + 4, (24 - num_lines - 3) / 2,
		      (70 - max_width) / 2);
  notify_win->Box();
  notify_win->SetBoldColor(COLOR_WHITE);
  for (int j = 0; j < num_lines; j++) {
    notify_win->Print(1 + j, 2, "%s", static_cast<const char*>(line_array[j]));
  }
  notify_win->Refresh();

  // Wait for the results.
  bool finished = false;
  int cur_char;

  while (!finished) {
    cur_char = GetInput();
    switch (cur_char) {
    case 'q':
    case 'Q':
    case ' ':
    case '\n':
    case '\r':
      finished = true;
      break;
    }
  }

  // Delete the window and redraw the screen.
  delete notify_win;
  delete [] line_array;
  Redraw();
}

