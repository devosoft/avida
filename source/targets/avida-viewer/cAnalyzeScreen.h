//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cAnalyzeScreen_h
#define cAnalyzeScreen_h

#ifndef cScreen_h
#include "cScreen.h"
#endif
#ifndef cStringList_h
#include "cStringList.h"
#endif
#ifndef cTabBox_h
#include "cTabBox.h"
#endif


class cAnalyzeCommand;
class cWorld;

class cAnalyzeScreen : public cScreen {
protected:
  cWorld* m_world;
  int mode;

  // Manage the menu mode.
  cTabBox tab_box;

  // Manage the command line.
  cString cur_input;
  cStringList command_hist;
  cStringList screen_hist;
  tList<cAnalyzeCommand> nest_command;
  int rollback_line;
  int cursor_pos;
  int nest_depth;

  cString cut_line;
  
  static const int ANALYZE_MODE_COMMAND_LINE = 0;
  static const int ANALYZE_MODE_MENU = 1;

  static const int ANALYZE_MENU_TAB_GENOTYPES = 0;
  static const int ANALYZE_MENU_TAB_VARIABLES = 1;
  static const int ANALYZE_MENU_TAB_COMMANDS = 2;
  static const int ANALYZE_MENU_TAB_FUNCTIONS = 3;

  void UpdateCommandLine_Body();
  void DoInput_CommandLine(cAvidaContext& ctx);
  void UpdateMenu_Body();
  void UpdateMenu_Genotypes();
  void UpdateMenu_Variables();
  void UpdateMenu_Commands();
  void UpdateMenu_Functions();
  void DoInput_Menu(cAvidaContext& ctx);

  void ProcessCommandLine();
public:
  cAnalyzeScreen(cWorld* world, int y_size, int x_size, int y_start, int x_start, cViewInfo& in_info)
    : cScreen(y_size, x_size, y_start, x_start, in_info)
    , m_world(world)
	, tab_box(this, 0, 0, Height() - 1, Width())
    , rollback_line(0)
    , cursor_pos(0)
    , nest_depth(0)
    , cut_line(0)
  {
    mode = ANALYZE_MODE_MENU;

    // Setup the tab box in menu mode.
    tab_box.AddTab("[G]enotypes");
    tab_box.AddTab("[V]ariables");
    tab_box.AddTab("[C]ommands");
    tab_box.AddTab("[F]unctions");
  }
  virtual ~cAnalyzeScreen() { ; }

  void Notify(const cString & in_string);

  // Virtual in base screen...
  void Draw(cAvidaContext& ctx);
  void Update(cAvidaContext& ctx);
  void DoInput(cAvidaContext& ctx, int in_char);

  void DrawCommandLine();
  void DrawMenu(cAvidaContext& ctx);
  void UpdateCommandLine(cAvidaContext& ctx);
  void UpdateMenu(cAvidaContext& ctx);
};

#endif
