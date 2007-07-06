/*
 *  cDriver_TextViewer.cc
 *  Avida
 *
 *  Created by Charles on 7/1/07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cStringList.h"
#include "cWorld.h"

#include "cDriverManager.h"
#include "cTextViewerManager.h"

#include <cstdlib>

using namespace std;

cDriver_TextViewer::cDriver_TextViewer(cWorld* world)
  : m_world(world), m_info(m_world->GetPopulation(), 12), m_done(false)
{
  // Setup the initial view mode (loaded from avida.cfg)
  m_info.SetViewMode(world->GetConfig().VIEW_MODE.Get());
    
  cDriverManager::Register(static_cast<cAvidaDriver*>(this));
  world->SetDriver(this);

  // Setup NCURSES...
  initscr();                // Set up the terminal for curses.
  //  cbreak();                 // Don't buffer input.
  raw();                    // Don't even buffer escape characters!
  noecho();                 // Don't echo keypresses to the screen.
  nonl();                   // No new line with CR (when echo is on)

  keypad(stdscr, 1);        // Allows the keypad to be used.
  nodelay(stdscr, 1);       // Don't wait for input if no key is pressed.

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
}

cDriver_TextViewer::~cDriver_TextViewer()
{
  cDriverManager::Unregister(static_cast<cAvidaDriver*>(this));
  delete m_world;
    
  ExitTextViewer(0);
}


void cDriver_TextViewer::Run()
{
  clog << "Ping!" << endl;

  cClassificationManager& classmgr = m_world->GetClassificationManager();
  cPopulation& population = m_world->GetPopulation();
  cStats& stats = m_world->GetStats();
  
  const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get();
  
  cAvidaContext ctx(m_world->GetRandom());
  
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
      
      // Update all the genotypes for the end of this update.
      for (cGenotype * cur_genotype = classmgr.ResetThread(0);
           cur_genotype != NULL && cur_genotype->GetThreshold();
           cur_genotype = classmgr.NextGenotype(0)) {
        cur_genotype->UpdateReset();
      }
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
          NotifyUpdate();
//          m_view.NewUpdate();
          
          // This is needed to have the top bar drawn properly; I'm not sure why...
          static bool first_update = true;
          if (first_update) {
//            Refresh();
            first_update = false;
          }
        }
        population.ProcessStep(ctx, step_size, next_id);
      }
    }
    else {
      for (int i = 0; i < UD_size; i++) population.ProcessStep(ctx, step_size);
    }
    
    
    // end of update stats...
    population.CalcUpdateStats();
    
    
    // Setup the viewer for the new update.
    if (m_info.GetStepOrganism() == -1) {
      NotifyUpdate();
//      NewUpdate();
      
      // This is needed to have the top bar drawn properly; I'm not sure why...
      static bool first_update = true;
      if (first_update) {
//        Refresh();
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
//    ChangeCurScreen(NULL);
    break;
//   case 'C':
//   case 'c':
//     NavigateMapWindow();
//     // Now we need to restore the proper window mode (already cleared)
//     ChangeCurScreen(cur_screen);
//     break;
//   case 'e':
//   case 'E':
//     ChangeCurScreen(environment_screen);
//     break;
//   case 'h':
//   case 'H':
//     ChangeCurScreen(hist_screen);
//     break;
//   case 'm':
//   case 'M':
//     ChangeCurScreen(map_screen);
//     break;
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
//    if (!Confirm("Are you sure you want to quit?")) break;
  case 'Q':      // Note: Capital 'Q' quits w/o confirming.
    // clear the windows before we go.  Do bar window last to end at top.
//     base_window->Redraw();
//     bar_screen->Clear();
//     bar_screen->Refresh();
    ExitTextViewer(0);  // This implementation calls exit(), blowing us clean away
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
//    Refresh();
    break;
  case 26: // CTRL-Z
    kill(getpid(), SIGTSTP);
    break;
//   case '*':   // Test Key!!!
//     if (true) {
//       Confirm("Starting Tests.");
//       cMenuWindow menu(50);
//       char message[40];
//       for (int j = 0; j < 50; j++) {
// 	sprintf(message, "Line %d", j);
// 	menu.AddOption(j, message);
//       }
//       menu.SetActive(3);
//       menu.Activate(base_window);
//       Redraw();
//     }
//     break;
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

void cDriver_TextViewer::NotifyUpdate()
{
  // @CAO What else should happen on an update?
  // - Update bar at top of screen
  // - Update current view
  // - Check for Inputs...

  const int update = m_world->GetStats().GetUpdate();
  if (update % 10 == 0) clog << update << endl;

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
    bool found_keypress = ProcessKeypress(cur_char);

    // If we couldn't manage the keypress here, check the current screen.
//    if (found_keypress == false && cur_screen) cur_screen->DoInput(cur_char);
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



void ExitTextViewer(int exit_code)
{
  signal(SIGINT, SIG_IGN);           // Ignore all future interupts.
//  mvcur(0, COLS - 1, LINES - 1, 0);  // Move curser to the lower left.
//  endwin();                          // Restore terminal mode.

  printf ("Exit Code: %d\n", exit_code);

  exit(exit_code);
}

