/*
 *  cTextWindow.cc
 *  Avida
 *
 *  Created by Charles on 7/6/07
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

#include "cTextWindow.h"

#include "cCoreView_Info.h"

#include <cstdlib>

using namespace std;


cTextWindow::cTextWindow(cTextWindow * parent, cCoreView_Info & info)
  : m_win_id(NULL), m_parent_window(parent), m_info(info)
{
  m_info.fp << "cTW::cTextWindow -- Base constructor for " << this << endl; // DEBUG!!!!!
}


cTextWindow::cTextWindow(cTextWindow * parent, cCoreView_Info & info, int y_size, int x_size, int y_start, int x_start)
  : m_parent_window(parent), m_info(info)
{
  // Build this window.
  m_win_id = newwin(y_size, x_size, y_start, x_start);

  m_info.fp << "cTW::cTextWindow -- Full constructor for " << this << "; id=" << m_win_id << endl; // DEBUG!!!!!


  // If a parent was given, register with it.
  if (m_parent_window != NULL) m_parent_window->AddSubWindow(this);

//   keypad(m_win_id, 1);        // Allows the keypad to be used.
//   NoDelay();
}


cTextWindow::~cTextWindow()
{
  m_info.fp << "cTW::~cTextWindow -- Destructor for " << this << endl; // DEBUG!!!!!

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

  m_info.fp << "cTW::Construct() for " << this << "; new id = " << m_win_id << endl; // DEBUG!!!!!

  // If a parent was given, register with it.
  if (m_parent_window != NULL) m_parent_window->AddSubWindow(this);
  keypad(m_win_id, 1);        // Allows the keypad to be used.
}



int cTextWindow::AddSubWindow(cTextWindow * in_window)
{
  assert(in_window != NULL);

  m_info.fp << "cTW::AddSubWindow(" << in_window << ") for " << this << endl; // DEBUG!!!!!

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
  m_info.fp << "cTW::Refresh() for " << this << endl; // DEBUG!!!!!

  RefreshSelf();
  for (int i = 0; i < m_sub_windows.GetSize(); i++) {
    if (m_sub_windows[i]) m_sub_windows[i]->Refresh();
  }
}


void cTextWindow::Redraw()
{
  m_info.fp << "cTW::Redraw() for " << this << endl; // DEBUG!!!!!

  RedrawSelf();
  for (int i = 0; i < m_sub_windows.GetSize(); i++) {
    if (m_sub_windows[i]) m_sub_windows[i]->Redraw();
  }
}

void cTextWindow::Clear()
{
  m_info.fp << "cTW::Clear() for " << this << endl; // DEBUG!!!!!

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
  va_list argp;
  char buf[BUFSIZ];
  
  va_start(argp, fmt);
  vsprintf(buf, fmt, argp);
  va_end(argp);
  wmove(m_win_id, in_y, in_x);
  waddstr(m_win_id, buf);

  m_info.fp << "cTW::Print(" << buf << ") for " << this << endl; // DEBUG!!!!!
}


void cTextWindow::PrintBinary(int in_y, int in_x, unsigned int value)
{
  for (int i = 0; i < 32; i++) {
    if ((value >> i) & 1) Print(in_y, in_x+31-i, '1');
    else Print(in_y, in_x+31-i, '0');
  }
}

void cTextWindow::Box(int y, int x, int h, int w)
{
  m_info.fp << "cTW::Box(inputs!!) for " << this << endl; // DEBUG!!!!!

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
  mvwaddch(m_win_id, 0, in_x, ACS_TTEE);
  mvwaddch(m_win_id, GetHeight() - 1, in_x, ACS_BTEE);
  for (int i = 1; i < GetHeight() - 1; i++) {
    mvwaddch(m_win_id, i, in_x, ACS_VLINE);
  }
}

void cTextWindow::VLine(int in_x, int start_y, int length)
{
  mvwaddch(m_win_id, start_y, in_x, ACS_TTEE);
  mvwaddch(m_win_id, start_y + length - 1, in_x, ACS_BTEE);
  for (int i = 1; i < length - 1; i++) {
    mvwaddch(m_win_id, start_y+i, in_x, ACS_VLINE);
  }
}

void cTextWindow::HLine(int in_y)
{
  mvwaddch(m_win_id, in_y, 0, ACS_LTEE);
  mvwaddch(m_win_id, in_y, GetWidth() - 1, ACS_RTEE);
  for (int i = 1; i < GetWidth() - 1; i++) {
    mvwaddch(m_win_id, in_y, i, ACS_HLINE);
  }
}

void cTextWindow::HLine(int in_y, int start_x, int length)
{
  mvwaddch(m_win_id, in_y, start_x, ACS_LTEE);
  mvwaddch(m_win_id, in_y, start_x + length - 1, ACS_RTEE);
  for (int i = 1; i < length - 1; i++) {
    mvwaddch(m_win_id, in_y, start_x + i, ACS_HLINE);
  }
}
