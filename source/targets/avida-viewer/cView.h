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

#ifndef cView_Base_h
#include "cView_Base.h"
#endif

#define MODE_BLANK   0
#define MODE_MAP     1
#define MODE_STATS   2
#define MODE_HIST    3
#define MODE_OPTIONS 4
#define MODE_ZOOM    5
#define MODE_ENVIRONMENT 6
#define MODE_ANALYZE 7

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

class cView : public cView_Base {
private:
  cViewInfo info;

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

  void TogglePause(cAvidaContext& ctx);
  void CloneSoup();
  void ExtractCreature();

  // Input function
  void DoInputs(cAvidaContext& ctx);

  // Screen helpers
  void ChangeCurScreen(cAvidaContext& ctx, cScreen * new_screen);
  void PrintMerit(int in_y, int in_x, double in_merit);
  void PrintFitness(int in_y, int in_x, double in_fitness);

  // Map navigation
  void NavigateMapWindow(cAvidaContext& ctx);
public:
  cView(cWorld* world);
  virtual ~cView();

  void Setup(cAvidaContext& ctx, const cString & in_name);
  void SetViewMode(int in_mode);

  bool ProcessKeypress(cAvidaContext& ctx, int keypress);

  void NewUpdate(cAvidaContext& ctx);
  void NotifyUpdate(cAvidaContext& ctx);
  void NotifyError(const cString & in_string);
  void NotifyWarning(const cString & in_string);
  void NotifyComment(const cString & in_string);
  void NotifyOutput(const cString & in_string);
  void Pause() { info.SetPauseLevel(PAUSE_ON); }
  void DoBreakpoint(cAvidaContext& ctx);
  
  int Confirm(const cString & message);
  void Notify(const cString & message);

  int GetStepOrganism() { return info.GetStepOrganism(); }
  void SetStepOrganism(int in_id) { info.SetStepOrganism(in_id); }

  void Refresh(cAvidaContext& ctx);

  // Methods called by sub-windows.
  void Redraw();

  void TestDEBUG();
};

#endif
