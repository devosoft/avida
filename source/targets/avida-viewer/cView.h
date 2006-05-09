//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cView_h
#define cView_h

#ifndef cViewInfo_h
#include "cViewInfo.h"
#endif

#define MODE_BLANK   0
#define MODE_MAP     1
#define MODE_STATS   2
#define MODE_HIST    3
#define MODE_OPTIONS 4
#define MODE_ZOOM    5
#define MODE_ENVIRONMENT 6

class cScreen;
class cTextWindow;
class cBarScreen;
class cMapScreen;
class cStatsScreen;
class cHistScreen;
class cOptionsScreen;
class cZoomScreen;
class cEnvironmentScreen;
class cWorld;

class cView {
private:
  cViewInfo info;

  // Window information...
  static cTextWindow * base_window;
  static cScreen * cur_screen;
  static cBarScreen * bar_screen;
  cMapScreen * map_screen;
  cStatsScreen * stats_screen;
  cHistScreen * hist_screen;
  cOptionsScreen * options_screen;
  cZoomScreen * zoom_screen;
  cEnvironmentScreen * environment_screen;

  // Window managing functions...

  void TogglePause();
  void CloneSoup();
  void ExtractCreature();

  // Input function
  void DoInputs();

  // Screen helpers
  void ChangeCurScreen(cScreen * new_screen);
  void PrintMerit(int in_y, int in_x, cMerit in_merit);
  void PrintFitness(int in_y, int in_x, double in_fitness);

  // Map navigation
  void NavigateMapWindow();
public:
  cView(cWorld* world);
  virtual ~cView();

  void Setup(const cString & in_name);
  void SetViewMode(int in_mode);

  void NewUpdate();
  void NotifyUpdate();
  void NotifyError(const cString & in_string);
  void NotifyWarning(const cString & in_string);
  void NotifyComment(const cString & in_string);
  inline void Pause() { info.SetPauseLevel(PAUSE_ON); }
  void DoBreakpoint();

  static int Confirm(const cString & message);
  static void Notify(const cString & message);

  int GetStepOrganism() { return info.GetStepOrganism(); }
  void SetStepOrganism(int in_id) { info.SetStepOrganism(in_id); }

  void Refresh();

  // Methods called by sub-windows.
  static void Redraw();
};


#ifdef ENABLE_UNIT_TESTS
namespace nView {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
