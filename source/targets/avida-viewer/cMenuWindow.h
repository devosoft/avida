//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cMenuWindow_h
#define cMenuWindow_h

#ifndef tArray_h
#include "tArray.h"
#endif

#ifndef cString_h
#include "cString.h"
#endif
#ifndef cTextWindow_h
#include "cTextWindow.h"
#endif

class cMenuWindow {
private:
  cTextWindow * window;
  cString title;
  tArray<cString> option_array;
  cString key_array;
  int num_options;

  // menu status variables
  int active_id; // Currently active option.
  int num_rows;  // Number of rows of options.
  int num_cols;  // Number of columns of options.
  int max_entry; // Maximum width of any entry.

  // Private helper functions
  void DrawOption(int option_id, bool is_active=false);
  void MoveActiveID(int new_id);
  int FindHotkeyPos(const cString & in_entry);
public:
  cMenuWindow(int menu_size);
  ~cMenuWindow();

  void SetTitle(const char * in_title) { title = in_title; }
  void AddOption(int option_id, const cString & in_option);
  int Activate(cTextWindow * parent_window);

  inline void SetActive(int in_id) { active_id = in_id; }
};

#endif
