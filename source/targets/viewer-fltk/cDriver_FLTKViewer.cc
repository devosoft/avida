/*
 *  cDriver_FLTKViewer.cc
 *  Avida
 *
 *  Created by Charles on 7/9/07
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

#include "cDriver_FLTKViewer.h"

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
#include "cFLTKWindow.h"
#include "cFLTKBox.h"
#include "tFLTKButton.h"

#include "avidalogo.xpm"

#include <cstdlib>

#include <FL/Fl.H>

#include <signal.h>

// #include <FL/Fl_JPEG_Image.H>

using namespace std;

#define FLTK_MAINWIN_WIDTH  800
#define FLTK_MAINWIN_HEIGHT 600
#define FLTK_MENUBAR_HEIGHT 35
#define FLTK_SPACING 5
#define FLTK_GRID_SIDE 502

#define FLTK_SIDEBAR_WIDTH (FLTK_MAINWIN_WIDTH - FLTK_GRID_SIDE - 3*FLTK_SPACING)
#define FLTK_SIDEBAR_HEIGHT (FLTK_MAINWIN_HEIGHT - 2*FLTK_SPACING)
#define FLTK_BODY_HEIGHT (FLTK_MAINWIN_HEIGHT - 2 * FLTK_MENUBAR_HEIGHT - 2 * FLTK_SPACING)
#define FLTK_MENU1_Y FLTK_BODY_HEIGHT
#define FLTK_MENU2_Y (FLTK_MENU1_Y + FLTK_MENUBAR_HEIGHT + FLTK_SPACING)

#define FLTK_MENU_FONT_SIZE 20

cDriver_FLTKViewer::cDriver_FLTKViewer(cWorld* world)
  : m_world(world)
  , m_info(world, 18)
  , m_done(false)
  , m_main_window(FLTK_MAINWIN_WIDTH, FLTK_MAINWIN_HEIGHT, "Avida")
  , m_body_box(m_main_window, 0, 0, FLTK_MAINWIN_WIDTH, FLTK_BODY_HEIGHT)
  , m_update_box(m_main_window, 100, FLTK_MENU2_Y, 200, FLTK_MENUBAR_HEIGHT, "Update: 0")
  , m_title_box(m_main_window,    0, FLTK_MENU2_Y,  80, FLTK_MENUBAR_HEIGHT)
  , m_grid_view(m_info, m_main_window, FLTK_SPACING, FLTK_SPACING, FLTK_GRID_SIDE, FLTK_GRID_SIDE)
  , m_legend(m_grid_view, m_main_window, FLTK_GRID_SIDE + 2*FLTK_SPACING, FLTK_SPACING, FLTK_SIDEBAR_WIDTH, FLTK_SIDEBAR_HEIGHT)
  , m_grid_view_menu(m_main_window,   140, FLTK_MENU1_Y, 80, 30, "View:")
  , m_grid_tags_menu(m_main_window,   270, FLTK_MENU1_Y, 80, 30, "Tags:")
  , m_grid_symbol_menu(m_main_window, 400, FLTK_MENU1_Y, 80, 30, "Mark:")
  , m_pause_button(m_main_window,     400, FLTK_MENU2_Y, 30, 30, "@||")
  , m_quit_button(m_main_window,      440, FLTK_MENU2_Y, 30, 30, "@square")
{
  cDriverManager::Register(static_cast<cAvidaDriver*>(this));
  world->SetDriver(this);

  m_main_window.SetSizeRange(FLTK_MAINWIN_WIDTH, FLTK_MAINWIN_HEIGHT);
  m_main_window.SetBackgroundColor(cColor(0xDD, 0xDD, 0xFF));
  m_main_window.Resizable(m_body_box);

  m_update_box.SetType(cGUIBox::BOX_NONE);
  m_update_box.SetFontSize(FLTK_MENU_FONT_SIZE);
  m_update_box.SetFontAlign(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
  m_update_box.SetTooltip("Updates are the natural timescale in Avida");
  m_update_box.Refresh();
 
  m_title_box.SetType(cGUIBox::BOX_NONE);
  m_title_box.SetFont(FL_HELVETICA_BOLD);
  m_title_box.SetFontSize(FLTK_MENU_FONT_SIZE);
  m_title_box.SetFontColor(cColor::DARK_MAGENTA);
  cString title_tooltip;
  title_tooltip.Set("Avida Version %s\nby Charles Ofria", VERSION);
  m_title_box.SetTooltip(title_tooltip);
  m_title_box.SetImage_XPM(avidalogo);
  m_title_box.Refresh();

  cCoreView_Map & map_info = m_grid_view.GetMapInfo();
  int num_modes = map_info.GetNumModes();
  for (int i = 0; i < num_modes; i++) {
    const cString & cur_name = map_info.GetModeName(i);
    const int cur_type = map_info.GetModeType(i);
    if (cur_type == cCoreView_Map::VIEW_COLOR) {
      m_grid_view_menu.AddOption(cur_name, this, &cDriver_FLTKViewer::MenuCallback_View, i);
    } else if (cur_type == cCoreView_Map::VIEW_TAGS) {
      m_grid_tags_menu.AddOption(cur_name, this, &cDriver_FLTKViewer::MenuCallback_View, i);
    } else if (cur_type == cCoreView_Map::VIEW_SYMBOLS) {
      m_grid_symbol_menu.AddOption(cur_name, this, &cDriver_FLTKViewer::MenuCallback_View, i);
    }
  }
  m_grid_view_menu.SetActive(0);
  m_grid_tags_menu.SetActive(0);
  m_grid_symbol_menu.SetActive(0);

  m_pause_button.SetCallback(this, &cDriver_FLTKViewer::ButtonCallback_Pause);
  m_pause_button.SetTooltip("Pause");
  m_quit_button.SetCallback(this, &cDriver_FLTKViewer::ButtonCallback_Quit);
  m_quit_button.SetFontColor(cColor::DARK_RED);
  m_quit_button.SetFontSize(12);
  m_quit_button.SetTooltip("Quit");

  m_main_window.Finalize();
}


cDriver_FLTKViewer::~cDriver_FLTKViewer()
{
  cDriverManager::Unregister(static_cast<cAvidaDriver*>(this));
  delete m_world;
    
  ExitFLTKViewer(0);
}


void cDriver_FLTKViewer::Run()
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


void cDriver_FLTKViewer::SignalBreakpoint()
{
//  m_view.DoBreakpoint();
}


void cDriver_FLTKViewer::Flush()
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


bool cDriver_FLTKViewer::ProcessKeypress(int keypress)
{
  bool unknown = false;

  switch (keypress) {
//   case 'a':
//   case 'A':
//     ChangeCurScreen(analyze_screen);
//     break;
//   case 'b':
//   case 'B':
//    ChangeCurScreen(NULL);
//     break;
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
//      nodelay(stdscr, false); // Wait for input if pause is on...
    } else {
//       nodelay(stdscr, true); // Don't delay if pause is off.
    }

    // Redraw the screen to account for the toggled pause.
//    if (cur_screen) cur_screen->Draw();

    break;
  case 'q':
    if (!Confirm("Are you sure you want to quit?")) break;
  case 'Q':      // Note: Capital 'Q' quits w/o confirming.
    // clear the windows before we go.  Do bar window last to end at top.
    ExitFLTKViewer(0);
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
//   case 12: // CTRL-L...
//     wclear(stdscr);
//     wrefresh(stdscr);
//     m_bar_window.Redraw();
//     m_main_window.Redraw();
//     break;
  case 26: // CTRL-Z
    kill(getpid(), SIGTSTP);
    break;
  case 0:
    break;
  default:
    clog << "Unknown Key!" << endl;
    unknown = true;
    break;
  }

  return !unknown;
}


void cDriver_FLTKViewer::RaiseException(const cString& in_string)
{
  NotifyWarning(in_string);
}


void cDriver_FLTKViewer::RaiseFatalException(int exit_code, const cString& in_string)
{
  NotifyError(in_string);
  exit(exit_code);
}


void cDriver_FLTKViewer::Draw()
{
}


void cDriver_FLTKViewer::DoUpdate()
{
  bool error = Fl::check();

  m_info.SetupUpdate();

  cString update_string;
  update_string.Set("Update: %d", m_world->GetStats().GetUpdate());
  m_update_box.SetName(update_string);
  m_update_box.Refresh();
  
  m_grid_view.Redraw();
  m_legend.Redraw();

  const int pause_level = m_info.GetPauseLevel();

  // If we are stepping in some way, we've come to a stop, so revert to a normal pause.
  if (pause_level == cCoreView_Info::PAUSE_ADVANCE_INST) {
    m_info.ExitStepMode();
  }

  if (pause_level == cCoreView_Info::PAUSE_ADVANCE_UPDATE ||
      pause_level == cCoreView_Info::PAUSE_ADVANCE_DIVIDE) {
    m_info.SetPauseLevel(cCoreView_Info::PAUSE_ON);
  }

  // If we are paused, keep checking the interface until we are done.
  while (m_info.GetPauseLevel() == cCoreView_Info::PAUSE_ON) {
    error = Fl::check();
  }
}

void cDriver_FLTKViewer::NotifyComment(const cString& in_string)
{
  // @CAO Do anything special if we know its just a normal comment?
  Notify(in_string);
}

void cDriver_FLTKViewer::NotifyWarning(const cString& in_string)
{
  cString out_string(in_string);
  out_string.Insert("Warning: ");
  Notify(out_string);
}

void cDriver_FLTKViewer::NotifyError(const cString& in_string)
{
  cString out_string(in_string);
  out_string.Insert("Error: ");
  Notify(out_string);
  ExitFLTKViewer(1);
}

void cDriver_FLTKViewer::NotifyOutput(const cString& in_string)
{
  // @CAO Do anything special if we know its from cout/cerr?
  Notify(in_string);
}

void cDriver_FLTKViewer::Notify(const cString& in_string)
{
  // @CAO We need to display this!
}


int cDriver_FLTKViewer::Confirm(const cString & message)
{
  return 0;
}


void cDriver_FLTKViewer::ButtonCallback_Quit(double ignore)
{
  (void) ignore;
  exit(0);
}

void cDriver_FLTKViewer::ButtonCallback_Pause(double ignore)
{
  bool paused = m_info.TogglePause();
  if (paused == true) {
    m_pause_button.SetLabel("@>");
    m_pause_button.SetTooltip("Play");
  }
  else {
    m_pause_button.SetLabel("@||");
    m_pause_button.SetTooltip("Pause");
  }
}


void cDriver_FLTKViewer::MenuCallback_View(int new_mode)
{
  m_grid_view.GetMapInfo().SetMode(new_mode);
}



void ExitFLTKViewer(int exit_code)
{
  signal(SIGINT, SIG_IGN);           // Ignore all future interupts.

  printf ("Exit Code: %d\n", exit_code);

  exit(exit_code);
}

