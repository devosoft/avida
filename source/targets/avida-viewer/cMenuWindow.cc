//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cMenuWindow.h"

using namespace std;


cMenuWindow::cMenuWindow(int menu_size)
  : option_array(menu_size)
  , key_array(menu_size)

{
  window = NULL;
  num_options = menu_size;
  active_id = 0;
}

cMenuWindow::~cMenuWindow()
{
  if (window) delete window;
}

void cMenuWindow::AddOption(int option_id, const cString & in_option)
{
  assert(option_id >= 0 && option_id < option_array.GetSize());
  option_array[option_id] = in_option;
  int hot_pos = FindHotkeyPos(in_option);
  key_array[option_id] = (hot_pos == -1)  ?  -1 : in_option[hot_pos];
}

int cMenuWindow::Activate(cTextWindow * parent_window)
{
  // Calculate number of columns needed and the width of each
  max_entry = 0;
  for (int i = 0; i < num_options; i++) {
    if (max_entry < option_array[i].GetSize())
      max_entry = option_array[i].GetSize();
  }

  const int max_width = parent_window->Width() - 4;
  const int min_width = title.GetSize() + 2;

  // The minimum number of columns available is determined by the title.
  const int min_cols = min_width / (max_entry + 2);
  const int max_cols = max_width / (max_entry + 2);

  // The number of columns should be adjusted by the number of items, and then
  // forced back into range.
  num_cols = 1;
  if (num_options > 80) num_cols = 5;
  else if (num_options > 45) num_cols = 4;
  else if (num_options > 20) num_cols = 3;
  else if (num_options > 6) num_cols = 2;
  
  // Now force the number of columns back into range.
  if (num_cols < min_cols) num_cols = min_cols;
  if (num_cols > max_cols) num_cols = max_cols;

  // Next, figure out how many rows we need for this to work.
  num_rows = 1 + (num_options-1) / num_cols;


  // Determine dimenstions for window
  int win_height = num_rows + 4;
  int win_width = (max_entry + 2) * num_cols + 2;

  // Adjust the dimensions to make sure the title will fit.
  if (!title.IsEmpty()) win_height += 2;
  if (win_width < title.GetSize() + 4) win_width = title.GetSize() + 4;

  // Create and draw the window.
  const int win_x = (parent_window->Width() - win_width) / 2;
  const int win_y = (parent_window->Height() - win_height) / 2;
  window = new cTextWindow(win_height, win_width, win_y, win_x);

  window->Box();
  if (!title.IsEmpty()) {
    window->SetBoldColor(COLOR_WHITE);
    window->Print(2, 2, title);
  }

  for (int i = 0; i < num_options; i++) {
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

    for (int i = 0; i < num_options; i++) {
      const char test_char = key_array[i];
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
    case 27: // ESCAPE
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
      MoveActiveID(active_id - num_rows);
      break;
    case KEY_RIGHT:
      MoveActiveID(active_id + num_rows);
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
  if (is_active) window->SetBoldColor(COLOR_YELLOW);
  else window->SetColor(COLOR_CYAN);

  int line_id = option_id % num_rows;
  int col_id  = option_id / num_rows;
  int x_pos = col_id * (max_entry + 2) + 2;
  int y_pos = line_id + ((title.IsEmpty()) ? 2 : 4);
  window->Print(y_pos, x_pos, static_cast<const char*>(option_array[option_id]));

  int hot_pos = FindHotkeyPos(option_array[option_id]);
  if (hot_pos != -1) {
    window->SetBoldColor(COLOR_WHITE);
    window->Print(y_pos, x_pos + hot_pos, option_array[option_id][hot_pos]);
  }
  window->Move(y_pos, x_pos + option_array[option_id].GetSize());
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

