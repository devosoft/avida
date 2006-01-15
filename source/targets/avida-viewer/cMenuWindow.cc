//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cMenuWindow.h"

using namespace std;


cMenuWindow::cMenuWindow(int menu_size)
{
  window = NULL;
  option_list = new cString[menu_size];
  key_list = new char[menu_size];
  num_options = menu_size;
  active_id = 0;
}

cMenuWindow::~cMenuWindow()
{
  if (window) delete window;
  delete [] option_list;
  delete [] key_list;
}

void cMenuWindow::AddOption(int option_id, const char * in_option)
{
  option_list[option_id] = in_option;
  int hot_pos = FindHotkeyPos(in_option);
  key_list[option_id] = (hot_pos == -1)  ?  -1 : in_option[hot_pos];
}

int cMenuWindow::Activate()
{
  int i;

  // Calculate number of columns needed and the width of each

  max_width = 0;
  for (i = 0; i < num_options; i++) {
    if (max_width < option_list[i].GetSize())
      max_width = option_list[i].GetSize();
  }

  // @CAO should make these more flexible.
  int max_lines = title.IsEmpty() ? 20 : 18;
  // int max_cols = 80 / (max_width + 3);

  // The minimum number of columns available is determined by the title.
  int min_cols = (title.GetSize() + 2) / (max_width + 2) ;

  cols = num_options / max_lines + 1;
  if (cols < min_cols) cols = min_cols;
  lines = num_options / cols;
  if (lines * cols < num_options) lines++;  // In case of roundoff error.

  // Determine dimenstions for window
  int win_height = lines + 4;
  int win_width = (max_width + 2) * cols + 2;

  // Adjust the dimensions to make sure the title will fit.
  if (!title.IsEmpty()) win_height += 2;
  if (win_width < title.GetSize() + 4) win_width = title.GetSize() + 4;

  // Create and draw the window.
  window = new cTextWindow(win_height, win_width,
			   (23 - win_height) / 2, (80 - win_width) / 2);
  window->Box();
  if (!title.IsEmpty()) {
    window->SetBoldColor(COLOR_WHITE);
//    window->Print (2, 2, title());
  }

  for (i = 0; i < num_options; i++) {
    DrawOption(i);
  }
  DrawOption(active_id, true);
  window->Refresh();

  // Wait for the results.
  bool finished = false;
  int cur_char;

  while (!finished) {
    cur_char = GetInput();

    // First see if we have hit a hotkey for an option.

    for (i = 0; i < num_options; i++) {
      const char test_char = key_list[i];
      if (test_char != -1 &&
	  (test_char == cur_char || test_char - 'A' + 'a' == cur_char)) {
	active_id = i;
	finished = true;
	break;
      }
    }
    if (finished == true) break;

    // Then check all of the default options.
    switch (cur_char) {
    case 'q':
    case 'Q':
      // Abort!
      finished = true;
      active_id = -1;
      break;
    case ' ':
    case '\n':
    case '\r':
      // Select current entry.
      finished = true;
      break;
    case KEY_UP:
      MoveActiveID(active_id - 1);
      break;
    case KEY_DOWN:
      MoveActiveID(active_id + 1);
      break;
    case KEY_LEFT:
      MoveActiveID(active_id - lines);
      break;
    case KEY_RIGHT:
      MoveActiveID(active_id + lines);
      break;
    case KEY_HOME:
      MoveActiveID(0);
      break;
#ifdef KEY_END
    case KEY_END:
      MoveActiveID(num_options - 1);
      break;
#endif
    }
  }

  // Delete the window and return the results.
  delete window;
  window = NULL;

  return active_id;
}

void cMenuWindow::DrawOption(int option_id, bool is_active)
{
  if (is_active) window->SetBoldColor(COLOR_CYAN);
  else window->SetColor(COLOR_CYAN);

  int line_id = option_id % lines;
  int col_id  = option_id / lines;
  int x_pos = col_id * (max_width + 2) + 2;
  int y_pos = line_id + ((title.IsEmpty()) ? 2 : 4);
  window->Print(y_pos, x_pos, option_list[option_id]());

  int hot_pos = FindHotkeyPos(option_list[option_id]);
  if (hot_pos != -1) {
    window->SetBoldColor(COLOR_WHITE);
    window->Print(y_pos, x_pos + hot_pos, option_list[option_id][hot_pos]);
  }
  window->Move(y_pos, x_pos + option_list[option_id].GetSize());
}

void cMenuWindow::MoveActiveID(int new_id)
{
  // Don't do anything if we are not really moving, or else trying to
  // move out of range.
  if (active_id == new_id || new_id < 0 || new_id >= num_options) return;

  // Dull out old active ID.
  DrawOption(active_id);

  // Reset the active ID.
  active_id = new_id;
  // while (active_id < 0) active_id += num_options;
  // active_id %= num_options;

  // Hi-lite the new ID.
  DrawOption(active_id, true);

  // Refresh the screen.
  window->Refresh();
}

int cMenuWindow::FindHotkeyPos(const cString & in_entry)
{
  int pos = -1;
  while ( (pos = in_entry.Find('[', pos+1)) != -1) {
    if (pos + 2 < in_entry.GetSize() &&
	in_entry[pos + 2] == ']')
      return pos + 1;
  }

  return -1;
}

