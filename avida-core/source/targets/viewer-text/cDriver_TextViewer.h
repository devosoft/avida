/*
 *  cDriver_TextViewer.h
 *  Avida
 *
 *  Created by Charles on 7-1-07
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cDriver_TextViewer_h
#define cDriver_TextViewer_h

#include "avida/core/WorldDriver.h"

#include "cViewer_Info.h"
#include "cScreen_Map.h"
#include "cTextWindow.h"
#include "ncurses-defs.h"

#include <sstream>
#include <iostream>
#include <fstream>

class cWorld;

using namespace std;
using namespace Avida;


class cDriver_TextViewer : public WorldDriver {
private:
  cWorld* m_world;
  cViewer_Info m_info;
  cTextWindow m_main_window;
  cTextWindow m_bar_window;

  cScreen_Map m_screen_map;

  cTextScreen * m_cur_screen;

  bool m_done;           // This is set to true when run should finish.

  void ChangeCurScreen(cTextScreen * new_screen);
public:
  cDriver_TextViewer(cWorld* world);
  ~cDriver_TextViewer();
  
  void Run();
  
  // Driver Actions
  void Finish() { m_done = true; }
  void Pause() { ; }

  // IO
  void Flush();
  int GetKeypress() { return getch(); }
  bool ProcessKeypress(int keypress);
  void NoDelay(bool _nd=true) { nodelay(stdscr, _nd); }  // Don't wait for input if no key is pressed.

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

};


void ExitTextViewer(int exit_code);

#endif
