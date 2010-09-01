/*
 *  cTextWindow.cc
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

#include "cTextWindow.h"

#include "cCoreView_Info.h"

#include <cstdlib>

using namespace std;


cTextWindow::cTextWindow(cTextWindow * parent, cCoreView_Info & info)
  : m_win_id(NULL), m_parent_window(parent), m_info(info)
{
}


cTextWindow::cTextWindow(cTextWindow * parent, cCoreView_Info & info, int y_size, int x_size, int y_start, int x_start)
  : m_parent_window(parent), m_info(info)
{
  // Build this window.
  m_win_id = newwin(y_size, x_size, y_start, x_start);

  // If a parent was given, register with it.
  if (m_parent_window != NULL) m_parent_window->AddSubWindow(this);

//   keypad(m_win_id, 1);        // Allows the keypad to be used.
//   NoDelay();
}


cTextWindow::~cTextWindow()
{
  // Delete all sub-windows...
  for (int i = 0; i < m_sub_windows.GetSize(); i++) {
    if (m_sub_windows[i]) delete m_sub_windows[i];
  }

  // And cleanup this window.
  werase(m_win_id);
  wrefresh(m_win_id);
  delwin(m_win_id);
}

void cTextWindow::Construct(int y_size, int x_size, int y_start, int x_start)
{
  assert(m_win_id == NULL);
  
  m_win_id = newwin(y_size, x_size, y_start, x_start);

  // If a parent was given, register with it.
  if (m_parent_window != NULL) m_parent_window->AddSubWindow(this);
  keypad(m_win_id, 1);        // Allows the keypad to be used.
}



int cTextWindow::AddSubWindow(cTextWindow * in_window)
{
  assert(in_window != NULL);

  // Scan existing array for open spots...
  for (int i = 0; i < m_sub_windows.GetSize(); i++) {
    if (m_sub_windows[i] == NULL) {
      m_sub_windows[i] = in_window;
      return i;
    }
  }
  return m_sub_windows.Push(in_window);
}


cTextWindow * cTextWindow::DetachSubWindow(int id)
{
  assert(m_sub_windows[id] != NULL);
  cTextWindow * out_window = m_sub_windows[id];
  m_sub_windows[id] = NULL;
  return out_window;
}


void cTextWindow::CloseSubWindow(int id)
{
  assert(m_sub_windows[id] != NULL);
  delete m_sub_windows[id];
  m_sub_windows[id] = NULL;
}


void cTextWindow::Refresh()
{
  RefreshSelf();
  for (int i = 0; i < m_sub_windows.GetSize(); i++) {
    if (m_sub_windows[i]) m_sub_windows[i]->Refresh();
  }
}


void cTextWindow::Redraw()
{
  RedrawSelf();
  for (int i = 0; i < m_sub_windows.GetSize(); i++) {
    if (m_sub_windows[i]) m_sub_windows[i]->Redraw();
  }
}

void cTextWindow::Clear()
{
  ClearMain();
  for (int i = 0; i < m_sub_windows.GetSize(); i++) {
    if (m_sub_windows[i]) m_sub_windows[i]->Clear();
  }
}


void cTextWindow::Print(const char * fmt, ...)
{
  va_list argp;
  char buf[BUFSIZ];
  
  va_start(argp, fmt);
  vsprintf(buf, fmt, argp);
  va_end(argp);
  waddstr(m_win_id, buf);
}


void cTextWindow::Print(int in_y, int in_x, const char * fmt, ...)
{
  if (in_x < 0) in_x = GetWidth() + in_x;
  if (in_y < 0) in_y = GetHeight() + in_y;

  va_list argp;
  char buf[BUFSIZ];
  
  va_start(argp, fmt);
  vsprintf(buf, fmt, argp);
  va_end(argp);
  wmove(m_win_id, in_y, in_x);
  waddstr(m_win_id, buf);
}


void cTextWindow::PrintBinary(int in_y, int in_x, unsigned int value)
{
  if (in_x < 0) in_x = GetWidth() + in_x;
  if (in_y < 0) in_y = GetHeight() + in_y;

  for (int i = 0; i < 32; i++) {
    if ((value >> i) & 1) Print(in_y, in_x+31-i, '1');
    else Print(in_y, in_x+31-i, '0');
  }
}


void cTextWindow::PrintDouble(int in_y, int in_x, double in_value)
{
  if (in_x < 0) in_x = GetWidth() + in_x;
  if (in_y < 0) in_y = GetHeight() + in_y;

  // If we have an integer, just include one zero at the end.
  int int_value = (int) in_value;
  if (in_value == ((double) int_value)) Print(in_y, in_x, "%5.1f", in_value);

  // Otherwise give as many decimal places as we can...
  else if (in_value < 10.0)       Print(in_y, in_x, "%1.5f", in_value);
  else if (in_value < 100.0)      Print(in_y, in_x, "%2.4f", in_value);
  else if (in_value < 1000.0)     Print(in_y, in_x, "%3.3f", in_value);
  else if (in_value < 10000.0)    Print(in_y, in_x, "%4.2f", in_value);
  else if (in_value < 100000.0)   Print(in_y, in_x, "%5.1f", in_value);
  else if (in_value < 10000000.0) Print(in_y, in_x, "%7d", int_value);

  // And if its too big, use scientific notation.
  else Print(in_y, in_x, "%7.1e", in_value);
}


void cTextWindow::PrintOption(int in_y, int in_x, const cString & option)
{
  if (in_x < 0) in_x = GetWidth() + in_x;
  if (in_y < 0) in_y = GetHeight() + in_y;

  // Print the main option...
  SetBoldColor(COLOR_WHITE);
  Print(in_y, in_x, option);

  // Highlight the keypress...
  SetBoldColor(COLOR_YELLOW);
  bool highlight = false;
  for (int i = 0; i < option.GetSize(); i++) {
    if (option[i] == '[') { highlight = true; continue; }
    if (option[i] == ']') { highlight = false; continue; }
    if (highlight == true) Print(in_y, in_x+i, option[i]);
  }
}


int cTextWindow::PrintMenuBarOption(const cString & option, int max_x, int cur_x)
{
  // If there isn't enough room for this option, don't print it; just return cur position.
  if (cur_x + option.GetSize() >= max_x) return cur_x;

  PrintOption(1, cur_x, option);
  return cur_x+option.GetSize()+1;
}


void cTextWindow::Box(int y, int x, int h, int w)
{
  int i;
  for (i = 1; i < h - 1; i++) {
    mvwaddch(m_win_id, i + y, x, ACS_VLINE);
    mvwaddch(m_win_id, i + y, x + w - 1, ACS_VLINE);
  }
  for (i = 1; i < w - 1; i++) {
    mvwaddch(m_win_id, y, i + x, ACS_HLINE);
    mvwaddch(m_win_id, y + h - 1, i + x, ACS_HLINE);
  }
  mvwaddch(m_win_id, y, x, ACS_ULCORNER);
  mvwaddch(m_win_id, y + h - 1, x, ACS_LLCORNER);
  mvwaddch(m_win_id, y, x + w - 1, ACS_URCORNER);
  mvwaddch(m_win_id, y + h - 1, x + w - 1, ACS_LRCORNER);
}

void cTextWindow::VLine(int in_x)
{
  if (in_x < 0) in_x = GetWidth() + in_x;

  mvwaddch(m_win_id, 0, in_x, ACS_TTEE);
  mvwaddch(m_win_id, GetHeight() - 1, in_x, ACS_BTEE);
  for (int i = 1; i < GetHeight() - 1; i++) {
    mvwaddch(m_win_id, i, in_x, ACS_VLINE);
  }
}

void cTextWindow::VLine(int in_x, int start_y, int length)
{
  if (in_x < 0) in_x = GetWidth() + in_x;
  if (start_y < 0) start_y = GetHeight() + start_y;

  mvwaddch(m_win_id, start_y, in_x, ACS_TTEE);
  mvwaddch(m_win_id, start_y + length - 1, in_x, ACS_BTEE);
  for (int i = 1; i < length - 1; i++) {
    mvwaddch(m_win_id, start_y+i, in_x, ACS_VLINE);
  }
}

void cTextWindow::HLine(int in_y)
{
  if (in_y < 0) in_y = GetHeight() + in_y;

  mvwaddch(m_win_id, in_y, 0, ACS_LTEE);
  mvwaddch(m_win_id, in_y, GetWidth() - 1, ACS_RTEE);
  for (int i = 1; i < GetWidth() - 1; i++) {
    mvwaddch(m_win_id, in_y, i, ACS_HLINE);
  }
}

void cTextWindow::HLine(int in_y, int start_x, int length)
{
  if (in_y < 0) in_y = GetHeight() + in_y;
  if (start_x < 0) start_x = GetWidth() + start_x;

  mvwaddch(m_win_id, in_y, start_x, ACS_LTEE);
  mvwaddch(m_win_id, in_y, start_x + length - 1, ACS_RTEE);
  for (int i = 1; i < length - 1; i++) {
    mvwaddch(m_win_id, in_y, start_x + i, ACS_HLINE);
  }
}
