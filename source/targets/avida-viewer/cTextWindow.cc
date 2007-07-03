//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cTextWindow.h"

#include <cstdlib>

using namespace std;

void cTextWindow::NoDelay(bool setting) {
  nodelay(stdscr, setting);       // Don't wait for input if no key is pressed.
}

cTextWindow::cTextWindow(int y_size, int x_size, int y_start, int x_start)
{
  win_id = newwin(y_size, x_size, y_start, x_start);
  keypad(win_id, 1);        // Allows the keypad to be used.
  NoDelay();
}

cTextWindow::cTextWindow()
{
  win_id = NULL;
}

cTextWindow::~cTextWindow()
{
  werase(win_id);
  wrefresh(win_id);
  delwin(win_id);
}

void cTextWindow::Construct(int y_size, int x_size, int y_start, int x_start)
{
  assert (win_id == NULL);

  win_id = newwin(y_size, x_size, y_start, x_start);
  keypad(win_id, 1);        // Allows the keypad to be used.
}

void cTextWindow::Box(int y, int x, int h, int w, bool test)
{
  int i;
  for (i = 1; i < h - 1; i++) {
    mvwaddch(win_id, i + y, x, ACS_VLINE);
    mvwaddch(win_id, i + y, x + w - 1, ACS_VLINE);
  }
  for (i = 1; i < w - 1; i++) {
    mvwaddch(win_id, y, i + x, ACS_HLINE);
    mvwaddch(win_id, y + h - 1, i + x, ACS_HLINE);
  }
  mvwaddch(win_id, y, x, ACS_ULCORNER);
  mvwaddch(win_id, y + h - 1, x, ACS_LLCORNER);
  mvwaddch(win_id, y, x + w - 1, ACS_URCORNER);
  mvwaddch(win_id, y + h - 1, x + w - 1, ACS_LRCORNER);
}

void cTextWindow::VLine(int in_x)
{
  mvwaddch(win_id, 0, in_x, ACS_TTEE);
  mvwaddch(win_id, Height() - 1, in_x, ACS_BTEE);
  for (int i = 1; i < Height() - 1; i++) {
    mvwaddch(win_id, i, in_x, ACS_VLINE);
  }
}

void cTextWindow::VLine(int in_x, int start_y, int length)
{
  mvwaddch(win_id, start_y, in_x, ACS_TTEE);
  mvwaddch(win_id, start_y + length - 1, in_x, ACS_BTEE);
  for (int i = 1; i < length - 1; i++) {
    mvwaddch(win_id, start_y+i, in_x, ACS_VLINE);
  }
}

void cTextWindow::HLine(int in_y)
{
  mvwaddch(win_id, in_y, 0, ACS_LTEE);
  mvwaddch(win_id, in_y, Width() - 1, ACS_RTEE);
  for (int i = 1; i < Width() - 1; i++) {
    mvwaddch(win_id, in_y, i, ACS_HLINE);
  }
}

void cTextWindow::HLine(int in_y, int start_x, int length)
{
  mvwaddch(win_id, in_y, start_x, ACS_LTEE);
  mvwaddch(win_id, in_y, start_x + length - 1, ACS_RTEE);
  for (int i = 1; i < length - 1; i++) {
    mvwaddch(win_id, in_y, start_x + i, ACS_HLINE);
  }
}


/////////////////////////
//  Other functions...
/////////////////////////

void StartProg()
{
  initscr();                // Set up the terminal for curses.
  //  cbreak();                 // Don't buffer input.
  raw();                    // Don't even buffer escape characters!
  noecho();                 // Don't echo keypresses to the screen.
  nonl();                   // No new line with CR (when echo is on)

  keypad(stdscr, 1);        // Allows the keypad to be used.
  nodelay(stdscr, 1);       // Don't wait for input if no key is pressed.

  // Setup colors

  if (has_colors()) start_color();
  init_pair(COLOR_WHITE,   COLOR_WHITE,   COLOR_BLACK);
  init_pair(COLOR_GREEN,   COLOR_GREEN,   COLOR_BLACK);
  init_pair(COLOR_RED,     COLOR_RED,     COLOR_BLACK);
  init_pair(COLOR_BLUE,    COLOR_BLUE,    COLOR_BLACK);
  init_pair(COLOR_CYAN,    COLOR_CYAN,    COLOR_BLACK);
  init_pair(COLOR_YELLOW,  COLOR_YELLOW,  COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_OFF,     COLOR_BLACK,   COLOR_BLACK);
}

void EndProg(int ignore)
{
  signal(SIGINT, SIG_IGN);           // Ignore all future interupts.
  mvcur(0, COLS - 1, LINES - 1, 0);  // Move curser to the lower left.
  endwin();                          // Restore terminal mode.

  printf ("Exit Code: %d\n", ignore);

  exit(0);
}

