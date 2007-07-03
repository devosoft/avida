/*
 *  cTextViewerManager.h
 *  Avida
 *
 *  Created by Charles on 7-1-07
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

#ifndef cTextViewerManager_h
#define cTextViewerManager_h

#ifndef cString_h
#include "cString.h"
#endif

#ifndef cCoreView_Info_h
#include "cCoreView_Info.h"
#endif

class cScreen;
class cTextWindow;
class cBarScreen;
class cMapScreen;
class cStatsScreen;
class cHistScreen;
class cOptionsScreen;
class cZoomScreen;
class cEnvironmentScreen;
class cAnalyzeScreen;
class cWorld;

class cTextViewerManager {
private:
  cCoreView_Info info;

  // Window information...
  cTextWindow * base_window;
  cScreen * cur_screen;
  cBarScreen * bar_screen;
  cMapScreen * map_screen;
  cStatsScreen * stats_screen;
  cHistScreen * hist_screen;
  cOptionsScreen * options_screen;
  cZoomScreen * zoom_screen;
  cEnvironmentScreen * environment_screen;
  cAnalyzeScreen * analyze_screen;

  // Window managing functions...

  void TogglePause();
  void CloneSoup();
  void ExtractOrganism();

  // Input function
  void DoInputs();

  // Screen helpers
  void ChangeCurScreen(cScreen * new_screen);
  void PrintMerit(int in_y, int in_x, double in_merit);
  void PrintFitness(int in_y, int in_x, double in_fitness);

  // Map navigation
  void NavigateMapWindow();

public:
  enum eTextViewerMode {
    MODE_BLANK       = 0,
    MODE_MAP         = 1,
    MODE_STATS       = 2,
    MODE_HIST        = 3,
    MODE_OPTIONS     = 4,
    MODE_ZOOM        = 5,
    MODE_ENVIRONMENT = 6,
    MODE_ANALYZE     = 7
  };

  cTextViewerManager(cWorld* world);
  virtual ~cTextViewerManager();

  void Setup(const cString & in_name);
  void SetViewMode(int in_mode);

  bool ProcessKeypress(int keypress);

  void NewUpdate();
  void NotifyUpdate();
  void NotifyError(const cString & in_string);
  void NotifyWarning(const cString & in_string);
  void NotifyComment(const cString & in_string);
  void NotifyOutput(const cString & in_string);
  void Pause() { info.SetPauseLevel(cCoreView_Info::PAUSE_ON); }
  void DoBreakpoint();
  
  int Confirm(const cString & message);
  void Notify(const cString & message);

  int GetStepOrganism() { return info.GetStepOrganism(); }
  void SetStepOrganism(int in_id) { info.SetStepOrganism(in_id); }

  void Refresh();

  // Methods called by sub-windows.
  void Redraw();
};


#endif
