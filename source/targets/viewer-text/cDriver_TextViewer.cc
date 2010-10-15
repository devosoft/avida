/*
 *  cDriver_TextViewer.cc
 *  Avida
 *
 *  Created by Charles on 7/1/07
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cDriver_TextViewer.h"

#include "cAnalyze.h"
#include "cChangeList.h"
#include "cClassificationManager.h"
#include "cHardwareBase.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cStringList.h"
#include "cWorld.h"

#include "cDriverManager.h"
#include "cTextWindow.h"

#include <cstdlib>
#include <signal.h>

using namespace std;

cDriver_TextViewer::cDriver_TextViewer(cWorld* world)
  : m_world(world)
  , m_info(m_world, 12)
  , m_main_window(NULL, m_info)
  , m_bar_window(NULL, m_info)
  , m_screen_map(m_info, m_main_window)
  , m_cur_screen(NULL)
  , m_done(false)
{
  cDriverManager::Register(this);
  world->SetDriver(this);

  // Setup NCURSES...
  initscr();            // Set up the terminal for curses.
  //  cbreak();             // Don't buffer input.
  raw();                // Don't even buffer escape characters!
  noecho();             // Don't echo keypresses to the screen.
  nonl();               // No new line with CR (when echo is on)

  keypad(stdscr, 1);    // Allows the keypad to be used.
  NoDelay();            // Don't wait for input if no key is pressed.

  // Setup colors

  if (has_colors()) start_color();
  init_pair(COLOR_WHITE,   COLOR_WHITE,   COLOR_BLACK);
  init_pair(COLOR_GREEN,   COLOR_GREEN,   COLOR_BLACK);
  init_pair(COLOR_RED,     COLOR_RED,     COLOR_BLACK);
  init_pair(COLOR_BLUE,    COLOR_BLUE,    COLOR_BLACK);
  init_pair(COLOR_CYAN,    COLOR_CYAN,    COLOR_BLACK);
  init_pair(COLOR_YELLOW,  COLOR_YELLOW,  COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_OFF,     COLOR_BLACK,   COLOR_BLACK);

  // Build the main body of this window.
  m_main_window.Construct(0,0,3,0);
  m_bar_window.Construct(3,0,0,0);

  Draw();
  m_main_window.SetBoldColor(COLOR_WHITE);
  m_main_window.Print(10, 10, "This is a test!");

  //  NoDelay(false);
  wrefresh(stdscr);
  m_bar_window.Redraw();
  m_main_window.Redraw();
}

cDriver_TextViewer::~cDriver_TextViewer()
{
  cDriverManager::Unregister(this);
  delete m_world;
    
  ExitTextViewer(0);
}


void cDriver_TextViewer::Run()
{
  cPopulation& population = m_world->GetPopulation();
  cStats& stats = m_world->GetStats();
  
  const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get();
  
  cAvidaContext ctx(m_world, m_world->GetRandom());
  
  while (!m_done) {
    if (cChangeList* change_list = population.GetChangeList()) {
      change_list->Reset();
    }
    
    m_world->GetEvents(ctx);
    if (m_done == true) break;  // Stop here if told to do so by an event.
    
    // Increment the Update.
    stats.IncCurrentUpdate();
    
    // Handle all data collection for previous update.
    if (stats.GetUpdate() > 0) {
      // Tell the stats object to do update calculations and printing.
      stats.ProcessUpdate();
    }
    
    
    // Process the update.
    const int UD_size = ave_time_slice * population.GetNumOrganisms();
    const double step_size = 1.0 / (double) UD_size;
    

    // Are we stepping through an organism?
    if (m_info.GetStepOrganism() != -1) {  // Yes we are!
                                            // Keep the viewer informed about the organism we are stepping through...
      for (int i = 0; i < UD_size; i++) {
        const int next_id = population.ScheduleOrganism();
        if (next_id == m_info.GetStepOrganism()) {
          DoUpdate();
          
          // This is needed to have the top bar drawn properly; I'm not sure why...
          static bool first_update = true;
          if (first_update) {
            m_main_window.Refresh();
            first_update = false;
          }
        }
        population.ProcessStep(ctx, step_size, next_id);
      }
    }
    else {
      for (int i = 0; i < UD_size; i++) population.ProcessStep(ctx, step_size, population.ScheduleOrganism());
    }
    
    
    // end of update stats...
    population.ProcessPostUpdate(ctx);
    
    
    // Setup the viewer for the new update.
    if (m_info.GetStepOrganism() == -1) {
      DoUpdate();
      
      // This is needed to have the top bar drawn properly; I'm not sure why...
      static bool first_update = true;
      if (first_update) {
        m_main_window.Refresh();
        first_update = false;
      }
    }
    
    
    // Do Point Mutations
    if (point_mut_prob > 0 ) {
      for (int i = 0; i < population.GetSize(); i++) {
        if (population.GetCell(i).IsOccupied()) {
          population.GetCell(i).GetOrganism()->GetHardware().PointMutate(ctx, point_mut_prob);
        }
      }
    }
    
    // Exit conditons...
    if (population.GetNumOrganisms() == 0) m_done = true;
  }
}


void cDriver_TextViewer::SignalBreakpoint()
{
//  m_view.DoBreakpoint();
}


void cDriver_TextViewer::Flush()
{
  cStringList & out_list = m_info.GetOutList();
  cStringList & err_list = m_info.GetErrList();
  
  // And notify the output...
  while (out_list.GetSize() > 0) {
    NotifyOutput(out_list.Pop());
  }

  while (err_list.GetSize() > 0) {
    cString cur_string(err_list.Pop());
    // cur_string.Insert("! ");
    NotifyWarning(cur_string);
  }
}


bool cDriver_TextViewer::ProcessKeypress(int keypress)
{
  bool unknown = false;

  switch (keypress) {
//   case 'a':
//   case 'A':
//     ChangeCurScreen(analyze_screen);
//     break;
  case 'b':
  case 'B':
    ChangeCurScreen(NULL);
    break;
//   case 'e':
//   case 'E':
//     ChangeCurScreen(environment_screen);
//     break;
//   case 'h':
//   case 'H':
//     ChangeCurScreen(hist_screen);
//     break;
  case 'm':
  case 'M':
    ChangeCurScreen(&m_screen_map);
    break;
//   case 'n':
//   case 'N':
//     if (info.GetPauseLevel() == PAUSE_ON) {
//       info.SetPauseLevel(PAUSE_ADVANCE_UPDATE);
//       // parasite_zoom = false; // if executing, show code that is running
//       info.GetActiveCell()->GetOrganism()->GetPhenotype().SetFault("");
//       nodelay(stdscr, true); // Don't delay for input; get to processing.
//     }
//     if (cur_screen) cur_screen->AdvanceUpdate();
//     break;
//   case 'o':
//   case 'O':
//     ChangeCurScreen(options_screen);
//     break;
  case 'p':
  case 'P':
    if (m_info.TogglePause() == true) {
      nodelay(stdscr, false); // Wait for input if pause is on...
    } else {
      nodelay(stdscr, true); // Don't delay if pause is off.
    }

    // Redraw the screen to account for the toggled pause.
//    if (cur_screen) cur_screen->Draw();

    break;
  case 'q':
    if (!Confirm("Are you sure you want to quit?")) break;
  case 'Q':      // Note: Capital 'Q' quits w/o confirming.
    // clear the windows before we go.  Do bar window last to end at top.
    m_main_window.Clear();
    m_main_window.Redraw();
    m_bar_window.Clear();
    m_bar_window.Redraw();
    ExitTextViewer(0);
    break;
//   case 's':
//   case 'S':
//     ChangeCurScreen(stats_screen);
//     break;
//   case 'W':
//   case 'w':
//     CloneSoup();
//     break;
//   case 'X':
//   case 'x':
//     ExtractCreature();
//     break;
//   case 'z':
//   case 'Z':
//     ChangeCurScreen(zoom_screen);
//     break;
  case 3: // CTRL-C...
    exit(0);
    break;
  case 12: // CTRL-L...
    wclear(stdscr);
    wrefresh(stdscr);
    m_bar_window.Redraw();
    m_main_window.Redraw();
    break;
  case 26: // CTRL-Z
    kill(getpid(), SIGTSTP); 
    break;
  case ERR:
    break;
  default:
    clog << "Unknown Key!" << endl;
    unknown = true;
    break;
  }

  return !unknown;
}


void cDriver_TextViewer::RaiseException(const cString& in_string)
{
  NotifyWarning(in_string);
}

void cDriver_TextViewer::RaiseFatalException(int exit_code, const cString& in_string)
{
  NotifyError(in_string);
  exit(exit_code);
}


void cDriver_TextViewer::Draw()
{
  m_bar_window.SetBoldColor(COLOR_WHITE);

  m_bar_window.Box();
  m_bar_window.VLine(18);
  m_bar_window.VLine(-11);
  
  m_bar_window.Print(1, -8, "Avida");
  m_bar_window.Print(1, 2, "Update:");

  const int max_x = m_bar_window.GetWidth() - 19;
  int cur_x = 21;

  // Include options in their general order of importance.
  cur_x = m_bar_window.PrintMenuBarOption("[M]ap ", max_x, cur_x);
  cur_x = m_bar_window.PrintMenuBarOption("[S]tats", max_x, cur_x);
  // cur_x = m_bar_window.PrintMenuBarOption("[A]nalyze", max_x, cur_x);
  cur_x = m_bar_window.PrintMenuBarOption("[Z]oom", max_x, cur_x);
  cur_x = m_bar_window.PrintMenuBarOption("[O]ptions", max_x, cur_x);
  cur_x = m_bar_window.PrintMenuBarOption("[H]ist", max_x, cur_x);
  cur_x = m_bar_window.PrintMenuBarOption("[E]nv ", max_x, cur_x);
  cur_x = m_bar_window.PrintMenuBarOption("[P]ause", max_x, cur_x);
  cur_x = m_bar_window.PrintMenuBarOption("[B]lank", max_x, cur_x);
  cur_x = m_bar_window.PrintMenuBarOption("[C]hoose CPU", max_x, cur_x);

  // Always place Quit as the last option.
  cur_x = m_bar_window.PrintMenuBarOption("[Q]uit", max_x+8, cur_x);

  m_bar_window.Refresh();
}


void cDriver_TextViewer::DoUpdate()
{
  // @CAO What else should happen on an update?
  // - Update bar at top of screen
  // - Update current view
  // - Check for Inputs...

//   const int update = m_world->GetStats().GetUpdate();
//   if (update % 10 == 0) clog << update << endl;

  m_info.SetupUpdate();
  
  m_bar_window.SetBoldColor(COLOR_WHITE);
  m_bar_window.Print(1, 11, "%d", m_world->GetStats().GetUpdate());
  m_bar_window.SetColor(COLOR_WHITE);

  m_bar_window.Refresh();

  if (m_cur_screen != NULL) m_cur_screen->Update();
  m_main_window.Refresh();

  const int pause_level = m_info.GetPauseLevel();

  // If we are stepping in some way, we've come to a stop, so revert to a normal pause.
  if (pause_level == cCoreView_Info::PAUSE_ADVANCE_INST) {
    m_info.ExitStepMode();
  }

  if (pause_level == cCoreView_Info::PAUSE_ADVANCE_UPDATE ||
      pause_level == cCoreView_Info::PAUSE_ADVANCE_DIVIDE) {
    m_info.SetPauseLevel(cCoreView_Info::PAUSE_ON);
  }

  // If we are paused at all, delay doing anything else until we recieve user input.
  if (pause_level != cCoreView_Info::PAUSE_OFF) nodelay(stdscr, false);

  // If there is any input in the buffer, process all of it.
  int cur_char = ERR;
  while ((cur_char = GetKeypress()) != ERR || m_info.GetPauseLevel() == cCoreView_Info::PAUSE_ON) {
    ProcessKeypress(cur_char);

    //bool found_keypress = ProcessKeypress(cur_char);
    // If we couldn't manage the keypress here, check the current screen.
//    if (found_keypress == false && cur_screen) cur_screen->DoInput(cur_char);
    m_main_window.Refresh();
  }

  nodelay(stdscr, true);

}

void cDriver_TextViewer::NotifyComment(const cString& in_string)
{
  // @CAO Do anything special if we know its just a normal comment?
  Notify(in_string);
}

void cDriver_TextViewer::NotifyWarning(const cString& in_string)
{
  cString out_string(in_string);
  out_string.Insert("Warning: ");
  Notify(out_string);
}

void cDriver_TextViewer::NotifyError(const cString& in_string)
{
  cString out_string(in_string);
  out_string.Insert("Error: ");
  Notify(out_string);
  ExitTextViewer(1);
}

void cDriver_TextViewer::NotifyOutput(const cString& in_string)
{
  // @CAO Do anything special if we know its from cout/cerr?
  Notify(in_string);
}

void cDriver_TextViewer::Notify(const cString& in_string)
{
  // @CAO We need to display this!
}


int cDriver_TextViewer::Confirm(const cString & message)
{
  const int mess_length = message.GetSize();

  // Create a confirm window, and draw it on the screen.

  const int conf_width = mess_length + 10;
  const int conf_x = (m_main_window.GetWidth() - conf_width) / 2;
  cTextWindow conf_win(NULL, m_info, 3, conf_width, 10, conf_x);
  conf_win.Box();
  conf_win.SetBoldColor(COLOR_WHITE);
  conf_win.Print(1, 2, "%s (y/n)", static_cast<const char*>(message));
  conf_win.SetBoldColor(COLOR_CYAN);
  conf_win.Print(1, mess_length + 4, 'y');
  conf_win.Print(1, mess_length + 6, 'n');
  conf_win.SetColor(COLOR_WHITE);
  conf_win.Refresh();

  // Wait for the results.
  bool finished = false;
  bool result = false;
  int cur_char;

  while (finished == false) {
    cur_char = GetKeypress();
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

  // Redraw the screen, and return the results.
  m_main_window.Redraw();
  return result;
}


void cDriver_TextViewer::ChangeCurScreen(cTextScreen * new_screen)
{
  m_main_window.Clear();                       // Make sure contents of previous screen are cleared.
  if (new_screen != NULL) new_screen->Draw();  // Draw new information on this screen.
  m_cur_screen = new_screen;                   // Keep note of the current screen.
}


void ExitTextViewer(int exit_code)
{
  signal(SIGINT, SIG_IGN);           // Ignore all future interupts.
  mvcur(0, COLS - 1, LINES - 1, 0);  // Move curser to the lower left.
  endwin();                          // Restore terminal mode.

  printf ("Exit Code: %d\n", exit_code);

  exit(exit_code);
}

