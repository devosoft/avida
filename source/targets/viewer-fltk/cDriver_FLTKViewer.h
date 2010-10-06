/*
 *  cDriver_FLTKViewer.h
 *  Avida
 *
 *  Created by Charles on 7-9-07
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

#ifndef cDriver_FLTKViewer_h
#define cDriver_FLTKViewer_h

#ifndef cAvidaDriver_h
#include "cAvidaDriver.h"
#endif

#ifndef cCoreView_Info_h
#include "cCoreView_Info.h"
#endif
 
#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif

#include "cFLTKBox.h"
#include "cFLTKGridView.h"
#include "cFLTKLegend.h"
#include "cFLTKWindow.h"
#include "tFLTKButton.h"
#include "tFLTKMenu.h"

#include <sstream>
#include <iostream>
#include <fstream>

class cWorld;
class cGUIContainer;

using namespace std;

class cDriver_FLTKViewer : public cAvidaDriver, public cWorldDriver {
private:
  cWorld * m_world;
  cCoreView_Info m_info;

  bool m_done;              // This is set to true when run should finish.

  // Graphics components...
  cFLTKWindow m_main_window;

  //  cFLTKBox m_menu_box;
  cFLTKBox m_body_box;

  cFLTKBox m_update_box;
  cFLTKBox m_title_box;

  cFLTKGridView m_grid_view;
  cFLTKLegend m_legend;

  tFLTKMenu<cDriver_FLTKViewer> m_grid_view_menu;
  tFLTKMenu<cDriver_FLTKViewer> m_grid_tags_menu;
  tFLTKMenu<cDriver_FLTKViewer> m_grid_symbol_menu;
  tFLTKButton<cDriver_FLTKViewer> m_pause_button;
  tFLTKButton<cDriver_FLTKViewer> m_quit_button;
public:
  cDriver_FLTKViewer(cWorld* world);
  ~cDriver_FLTKViewer();
  
  void Run();
  
  // Driver Actions
  void SignalBreakpoint();
  void SetDone() { m_done = true; }

  void SetPause() { ; }

  // IO
  void Flush();
  int GetKeypress() { return 0; } // @CAO FIX!
  bool ProcessKeypress(int keypress);

  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);

  // Drawing and interaction.
  void Draw();
  void DoUpdate();

  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);
  void NotifyError(const cString& in_string);
  void NotifyOutput(const cString& in_string);
  void Notify(const cString& in_string);

  int Confirm(const cString& in_string);

  // Tests
  bool IsInteractive() { return true; }

  // Button Callbacks...
  void ButtonCallback_Quit(double ignore);
  void ButtonCallback_Pause(double ignore);

  // Menu Callbacks
  void MenuCallback_View(int new_mode);
};


void ExitFLTKViewer(int exit_code);

#endif
