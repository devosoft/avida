//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cTextWindow_h
#define cTextWindow_h

#ifdef HAVE_NCURSES_H
#define NCURSES_OPAQUE 0
#include <ncurses.h>
#undef OK
#else
#define NCURSES_OPAQUE 0
#include <curses.h>
#undef OK
#endif

#ifndef cBaseTextWindow_h
#include "cBaseTextWindow.h"
#endif
#ifndef platform_h
#include "Platform.h"
#endif

#include <csignal>
#include <cstdarg>

// All colors are defined except black on black:
#define COLOR_OFF    8

// Define special characters

#define CHAR_TTEE     ACS_TTEE
#define CHAR_BTEE     ACS_BTEE
#define CHAR_LTEE     ACS_LTEE
#define CHAR_RTEE     ACS_RTEE
#define CHAR_PLUS     ACS_PLUS
#define CHAR_HLINE    ACS_HLINE
#define CHAR_VLINE    ACS_VLINE
#define CHAR_ULCORNER ACS_ULCORNER
#define CHAR_URCORNER ACS_URCORNER
#define CHAR_LLCORNER ACS_LLCORNER
#define CHAR_LRCORNER ACS_LRCORNER
#define CHAR_BULLET   '#'   // ACS_BULLET

// We need three special functions: One to initialize the viewer, one to
// close it up, and one to interactively get inputs from the buffer without
// needing <return> to be pressed.

void StartProg();
void EndProg(int ignore);
inline int GetInput() { return getch(); }

//inline int GetBlockingInput() {
//  nodelay(stdscr, FALSE);
//  char c = getch();
//  nodelay(stdscr, TRUE);
//  return c; }

class cTextWindow : public cBaseTextWindow {
protected:
  WINDOW * win_id;

  void NoDelay(bool setting=true);
public:
  cTextWindow();
  cTextWindow(int y_size, int x_size, int y_start=0, int x_start=0);
  ~cTextWindow();

  void Construct(int y_size, int x_size, int y_start=0, int x_start=0);

  // These function return the number of characters wide or high
  // (respectively) that the screen is.
#if AVIDA_PLATFORM(WINDOWS)
  // Windows returns the screen width and height
  inline int Width() { return win_id->_maxx; }
  inline int Height() { return win_id->_maxy; }
#else
  // Elsewhere returns the max x and y coordinates, like expected
  inline int Width() { return win_id->_maxx + 1; }
  inline int Height() { return win_id->_maxy + 1; }
#endif
  WINDOW * GetWinID() { return win_id; } // DEBUG!!!

  // Clear the screen and redraw all text on it.
  inline void RedrawMain() { touchwin(win_id); wrefresh(win_id); }

  // Move the active position of the cursor.
  inline void Move(int new_y, int new_x) { wmove(win_id, new_y, new_x); }

  // Print all of the changes made onto the screen.
  inline void RefreshMain() { wrefresh(win_id); }

  // These functions clear sections of the screen.
  inline void ClearMain() { wclear(win_id); }
  inline void ClearToBot() { wclrtobot(win_id); }
  inline void ClearToEOL() { wclrtoeol(win_id); }

  // Various 'graphic' functions.  Box() draws a box-frame around the window.
  // With arguments, it draws a box starting at x, y, with size w by h.
  // VLine and HLine draw lines across the screen ending in the appropriate
  // facing T's (hence, they are used to cut boxes in half.  With two
  // coords and a length, they only draw the line from the specified start,
  // to the specified distance.
  inline void Box() { box(win_id, 0, 0); }
  void Box(int x, int y, int w, int h, bool test);
  void VLine(int in_x);
  void VLine(int in_x, int start_y, int length);
  void HLine(int in_y);
  void HLine(int in_y, int start_x, int length);

  // The following functions print characters onto the screen.  If they
  // begin with an y, x then they first move to those coords.

  inline void Print(chtype ch) { waddch(win_id, ch); }
  inline void Print(int in_y, int in_x, chtype ch)
    { mvwaddch(win_id, in_y, in_x, ch); }
  inline void Print(const char * fmt, ...) {
    va_list argp;
    char buf[BUFSIZ];

    va_start(argp, fmt);
    vsprintf(buf, fmt, argp);
    va_end(argp);
    waddstr(win_id, buf);
  }
  inline void Print(int in_y, int in_x, const char * fmt, ...) {
    va_list argp;
    char buf[BUFSIZ];

    va_start(argp, fmt);
    vsprintf(buf, fmt, argp);
    va_end(argp);
    wmove(win_id, in_y, in_x);
    waddstr(win_id, buf);
  }

  // These functions check or set the screen colors (BG is always black)
  inline int HasColors() { return has_colors(); }
  inline void SetColor(int color) {
    wattrset(win_id, COLOR_PAIR(color));
  }
  inline void SetBoldColor(int color) {
    wattrset(win_id, COLOR_PAIR(color) | A_BOLD);
  }
};

#endif
