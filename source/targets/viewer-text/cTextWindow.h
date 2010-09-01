/*
 *  cTextWindow.h
 *  Avida
 *
 *  Created by Charles on 7/6/07
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

#ifndef cTextWindow_h
#define cTextWindow_h

#ifndef cString_h
#include "cString.h"
#endif

#ifndef tArray_h
#include "tArray.h"
#endif

#ifndef ncurses_defs_h
#include "ncurses-defs.h"
#endif

class cCoreView_Info;

class cTextWindow {
protected:
  WINDOW * m_win_id;

  cTextWindow * m_parent_window;
  tArray<cTextWindow *> m_sub_windows;

  cCoreView_Info & m_info;

  // Internal Functions
  void RefreshSelf() { wrefresh(m_win_id); }
  void RedrawSelf() { touchwin(m_win_id); wrefresh(m_win_id); }

public:
  cTextWindow(cTextWindow * parent, cCoreView_Info & info);
  cTextWindow(cTextWindow * parent, cCoreView_Info & info, int y_size, int x_size, int y_start=0, int x_start=0);
  ~cTextWindow();

  void Construct(int y_size, int x_size, int y_start=0, int x_start=0);

  // ------ Accessors ------
  cTextWindow * GetParentWindow() { return m_parent_window; }
  cTextWindow * GetSubWindow(int id) { return m_sub_windows[id]; }

  // These function return the number of characters wide or high
  // (respectively) that the screen is.
#if AVIDA_PLATFORM(WINDOWS)
  // Windows returns the screen width and height
  inline int GetWidth() { return m_win_id->_maxx; }
  inline int GetHeight() { return m_win_id->_maxy; }
#else
  // Elsewhere returns the max x and y coordinates, like expected
  inline int GetWidth() { return m_win_id->_maxx + 1; }
  inline int GetHeight() { return m_win_id->_maxy + 1; }
#endif


  // ------ Dealing with sub-windows ------
  int AddSubWindow(cTextWindow * in_window);
  cTextWindow * DetachSubWindow(int id);
  void CloseSubWindow(int id);


  // ------ General Functions ------
  void Refresh();
  void Redraw();
  void Clear();

  // ------ Printing Text ------
  // The following functions print characters onto the screen.  If they
  // begin with an y, x then they first move to those coords.

  void Print(chtype ch) { waddch(m_win_id, ch); }
  void Print(int in_y, int in_x, chtype ch) { mvwaddch(m_win_id, in_y, in_x, ch); }
  void Print(const char * fmt, ...);
  void Print(int in_y, int in_x, const char * fmt, ...);
  void PrintBinary(int in_y, int in_x, unsigned int value);
  void PrintDouble(int in_y, int in_x, double value);
  void PrintOption(int in_y, int in_x, const cString & option);
  int PrintMenuBarOption(const cString & option, int max_x, int cur_x);


  // ------ Drawing & Erasing ------
  // Move the active position of the cursor.
  inline void Move(int new_y, int new_x) { wmove(m_win_id, new_y, new_x); }

  // These functions clear sections of the screen.
  inline void ClearMain() { wclear(m_win_id); }
  inline void ClearToBot() { wclrtobot(m_win_id); }
  inline void ClearToEOL() { wclrtoeol(m_win_id); }

  // Various 'graphic' functions.  Box() draws a box-frame around the window.
  // With arguments, it draws a box starting at x, y, with size w by h.
  // VLine and HLine draw lines across the screen ending in the appropriate
  // facing T's (hence, they are used to cut boxes in half.  With two
  // coords and a length, they only draw the line from the specified start,
  // to the specified distance.
  inline void Box() { box(m_win_id, 0, 0); }
  void Box(int x, int y, int w, int h);
  void VLine(int in_x);
  void VLine(int in_x, int start_y, int length);
  void HLine(int in_y);
  void HLine(int in_y, int start_x, int length);

  // These functions check or set the screen colors (BG is always black)
  inline int HasColors() { return has_colors(); }
  inline void SetColor(int color) {
    wattrset(m_win_id, COLOR_PAIR(color));
  }
  inline void SetBoldColor(int color) {
    wattrset(m_win_id, COLOR_PAIR(color) | A_BOLD);
  }

};

#endif

