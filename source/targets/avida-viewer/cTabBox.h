/*
 *  cTabBox.h
 *  Avida
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cTabBox_h
#define cTabBox_h

#ifndef cStringList_h
#include "cStringList.h"
#endif

#ifndef cTextWindow_h
#include "cTextWindow.h"
#endif

#ifndef tArray_h
#include "tArray.h"
#endif

class cTabBox {
private:
  cTextWindow * window;
  int y;
  int x;
  int rows;
  int cols;

  tArray<cString> tabname_array;
  tArray<char> optkey_array;
  int active_tab;

  // Find the position of the option character in a string.
  int FindOption(const cString & option) {
    const int found_pos = option.Find('[');
    if (found_pos == -1) return -1;
    return found_pos + 1;
  }

public:
  cTabBox(cTextWindow * _window, int _y, int _x, int _rows, int _cols)
    : window(_window), y(_y), x(_x), rows(_rows), cols(_cols)
    , active_tab(0)
  {
    assert(rows > 4); // Need 4 rows to fit box and tabs; more for body!
  }
  ~cTabBox() { ; }

  int GetNumTabs() const { return tabname_array.GetSize(); }
  int GetActiveTab() const { return active_tab; }
  void AddTab(const cString & name) {
    // Save the name of the tab.
    tabname_array.Push(name);

    // Determine if we have a keypress associated with the tab.
    const int opt_pos = FindOption(name);
    char opt_char = 0; // Default key char to zero.
    if (opt_pos > 0) {
      // Determine the character and convert it to lowercase.
      opt_char = name[opt_pos];
      if (opt_char >= 'A' && opt_char <= 'Z') opt_char = opt_char - 'A' + 'a';
    }
    optkey_array.Push(opt_char);
  }

  void SetActiveTab(int tab_id) {
    assert(tab_id >= 0 && tab_id < tabname_array.GetSize());
    active_tab = tab_id;
  }
  void NextTab() { ++active_tab %= tabname_array.GetSize(); }
  void PrevTab() { if (--active_tab < 0) active_tab = GetNumTabs()-1; }
  int DoKeypress(int keypress) {
    // Loop through all tabs to see if this keypress matches one of them.
    for (int tab_id = 0; tab_id < optkey_array.GetSize(); tab_id++) {
      if (keypress == optkey_array[tab_id]) {
	SetActiveTab(tab_id);
	return tab_id;
      }
    }
    return -1;
  }

  void Draw() {
    window->SetColor(COLOR_WHITE);
    // Draw the main box...
    for (int i = 2; i < rows-2; i++) {
      window->Move(y+i, 0);
      window->ClearToEOL();
    }
    window->Box(y+2, x, rows-2, cols, true);

    // Draw tabs...
    int offset = 2;
    int active_offset = 0;
    for (int tab_id = 0; tab_id < tabname_array.GetSize(); tab_id++) {
      const cString & cur_name = tabname_array[tab_id];
      const int tab_width = cur_name.GetSize() + 4;
      window->Box(y, x+offset, 3, tab_width, true);
      if (tab_id == active_tab) active_offset = offset; //(come back to this!)
      else {
	window->Print(y+1, x+offset+2, cur_name);
	window->SetColor(COLOR_YELLOW);
	const int opt_pos = FindOption(cur_name);
	window->Print(y+1, x+offset+2+opt_pos, cur_name[opt_pos]);
	window->SetColor(COLOR_WHITE);
      }
      window->Print(y+2, x+offset, CHAR_BTEE);
      offset += tab_width;
      window->Print(y+2, x+offset-1, CHAR_BTEE);
   }

    // Draw in the active window.
    const cString & active_name = tabname_array[active_tab];
    const int active_width = active_name.GetSize() + 4;
    window->Box(y, x+active_offset, 3, active_width, true);
    window->SetBoldColor(COLOR_YELLOW);
    window->Print(y+1, x+active_offset+2, active_name);
    window->SetColor(COLOR_WHITE);
    window->Print(y+2, x+active_offset, CHAR_LRCORNER);
    window->Print(y+2, x+active_offset+active_width-1, CHAR_LLCORNER);

    for (int i = 1; i < active_width - 1; i++) {
      window->Print(y+2, x+active_offset+i, ' ');
    }

    window->Move(3, window->Width()-1);
  }
};

#endif
