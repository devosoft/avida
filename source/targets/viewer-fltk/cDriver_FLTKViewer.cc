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

#include <cstdlib>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>


using namespace std;

cDriver_FLTKViewer::cDriver_FLTKViewer(cWorld* world)
  : m_world(world)
  , m_info(m_world->GetPopulation(), 12)
  , m_done(false)
{
  // Setup the initial view mode (loaded from avida.cfg)
  m_info.SetViewMode(world->GetConfig().VIEW_MODE.Get());
    
  cDriverManager::Register(static_cast<cAvidaDriver*>(this));
  world->SetDriver(this);

  Fl_Window *window = new Fl_Window(300,180);
  Fl_Box *box = new Fl_Box(20,40,260,100,"Avida!");

  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labelfont(FL_BOLD+FL_ITALIC);
  box->labeltype(FL_SHADOW_LABEL);
  window->end();

  int argc = 1;
  char * progname = "Avida";
  char ** argv = &progname;

  window->show(argc, argv);
  bool error = Fl::run();
  (void) error;
}

cDriver_FLTKViewer::~cDriver_FLTKViewer()
{
  cDriverManager::Unregister(static_cast<cAvidaDriver*>(this));
  delete m_world;
    
  ExitFLTKViewer(0);
}


void cDriver_FLTKViewer::Run()
{
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
          DoUpdate();
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
  const int pause_level = m_info.GetPauseLevel();

  // If we are stepping in some way, we've come to a stop, so revert to a normal pause.
  if (pause_level == cCoreView_Info::PAUSE_ADVANCE_INST) {
    m_info.ExitStepMode();
  }

  if (pause_level == cCoreView_Info::PAUSE_ADVANCE_UPDATE ||
      pause_level == cCoreView_Info::PAUSE_ADVANCE_DIVIDE) {
    m_info.SetPauseLevel(cCoreView_Info::PAUSE_ON);
  }

//   // If we are paused at all, delay doing anything else until we recieve user input.
//   if (pause_level != cCoreView_Info::PAUSE_OFF) nodelay(stdscr, false);

  // If there is any input in the buffer, process all of it.
  int cur_char = 0;
  while ((cur_char = GetKeypress()) != 0 || m_info.GetPauseLevel() == cCoreView_Info::PAUSE_ON) {
    bool found_keypress = ProcessKeypress(cur_char);

    // If we couldn't manage the keypress here, check the current screen.
//    if (found_keypress == false && cur_screen) cur_screen->DoInput(cur_char);
  }

//   nodelay(stdscr, true);

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
  const int mess_length = message.GetSize();
  return 0;
}



void ExitFLTKViewer(int exit_code)
{
  signal(SIGINT, SIG_IGN);           // Ignore all future interupts.

  printf ("Exit Code: %d\n", exit_code);

  exit(exit_code);
}

