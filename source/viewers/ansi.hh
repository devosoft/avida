//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2000 California Institute of Technology             //
// Portions Copyright (C) Microsoft Corporation, 1996.                      //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_ANSI_HH
#define VIEW_ANSI_HH

#include "defs.hh"

#ifdef VIEW_ANSI

// Place #include statements here
#include <signal.h>

// Flag used in change map to flag a change (requires updates to the screen)
#define		SCREEN_UPDATE	0x01

// Set up the following #defines such that when passed through to
// SetColor(int) and SetBoldColor(int), these functions will be able to
// recognize them (COLOR_OFF is black on black - used in SetBoldColor as
// dark gray on black; it should be used instead of COLOR_BLACK)
#define COLOR_BLACK     0
#define COLOR_RED       FOREGROUND_RED
#define COLOR_GREEN     FOREGROUND_GREEN
#define COLOR_YELLOW    (FOREGROUND_RED|FOREGROUND_GREEN)
#define COLOR_BLUE      FOREGROUND_BLUE
#define COLOR_MAGENTA   (FOREGROUND_BLUE|FOREGROUND_RED)
#define COLOR_CYAN      (FOREGROUND_BLUE|FOREGROUND_GREEN)
#define COLOR_WHITE     (FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN)
#define COLOR_OFF       0

// These are the IBM PC OEM charset linedraw characters...
#define CHAR_TTEE      194  /* 'T' connection of solid lines */
#define CHAR_BTEE      193  /* Upsidedown 'T' connection of solid lines */
#define CHAR_LTEE      195  /* "|-" shaped 'T' connection of solid lines */
#define CHAR_RTEE      180  /* "-|" shaped 'T' connection of solid lines */
#define CHAR_PLUS      197  /* cross connection of solid lines */
#define CHAR_HLINE     196  /* Straight horizontal line */
#define CHAR_VLINE     179  /* Straight veritcal line */
#define CHAR_ULCORNER  218  /* Upper-left  corner of a box */
#define CHAR_URCORNER  191  /* Upper-right corner of a box */
#define CHAR_LLCORNER  192  /* Lower-left corner of a box */
#define CHAR_LRCORNER  217  /* Lower-right corner of a box */
#define CHAR_BULLET    2    /* use for "bullet" */

// Keys, home, and end. (IBM PC char codes for "Gray Keys" only).

#define KEY_DOWN   0xE050
#define KEY_UP     0xE048
#define KEY_LEFT   0xE04B
#define KEY_RIGHT  0xE04D
#define KEY_HOME   0xE047
#define KEY_END    0xE04F
#define ERR        -1

// DRA - added for Mouse Support. This is the return code from GetInputs() when a mouse
// event is pending. Other defines are used in the returns from GetMouseInput()
#define	KEY_MOUSE		-2
#define MOUSE_CLICK		1
#define MOUSE_DBLCLICK	2

typedef struct _MOUSE_INFO {
	int		X;
	int		Y;
	int		iEvent;		// MOUSE_CLICK or MOUSE_DBLCLICK
} MOUSE_INFO;


// We need three special functions: One to initialize the viewer, one to
// close it up, and one to interactively get inputs from the buffer without
// needing <return> to be pressed.  They will be further described in
// ansi.cc where they are defined.

void StartProg();
void EndProg(int exit_code);
int GetInput();
//int GetBlockingInput(){ return GetInput(); }

MOUSE_INFO * GetMouseInput(void);
void FlushMouseQueue(void);
void EnableMouseEvents(int Flag);

// Finally, we have the cTextWindow class.  This class handles a sub-section
// of the screen henceforth called a window.  It should manage the internals
// of that window, and not let anything be displayed outside of it.  A window
// keeps an internal representation of its own state, and when its Refresh()
// method is run, it updates its section of the screen all at once (rather
// than one character at a time, as they are entered.)  If two windows
// occupy the same area of the screen, the one which has been refreshed most
// recently should be on top.

typedef int chtype;
#include "text_window.hh"

class cTextWindow : public cBaseTextWindow {
protected:

  // These four variables define the size and position of the window
  // - an array of some sort should also be made to store the current state
  // of the screen.
  int width;
  int height;
  int y_start;
  int x_start;

  // Current color used for char output
  WORD	color;

  // Physcial window dimensions. Needed to accurately predict cursor behavior.
  // This is based on the console buffer sizing.
  COORD		dwWindowDimensions;

  HANDLE	hConsoleOutput;	// Console output handle

  // The current position of the cursor on the screen.
  int curs_x;
  int curs_y;

  // Screen data buffers (for characters & change flags)
  CHAR_INFO	*pciScreenChars;
  BYTE		*pbScreenFlags;

  // Internal method for calculating cursor wrapping
  void cTextWindow::WrapCursor (int n);
  void cTextWindow::WrapCursor (int n, COORD *pco);

  // Critical section used to make calls thread safe
  CRITICAL_SECTION	csTextWindow;

  inline void cTextWindow::GetExclusiveAccess() {
    EnterCriticalSection (&csTextWindow);
  }

  inline void cTextWindow::ReleaseExclusiveAccess() {
    LeaveCriticalSection (&csTextWindow);
  }

  // Internal versions of functions that we can call when we have the
  // Critical Section
  void cTextWindow::iMove(int new_y, int new_x);
  void cTextWindow::iRefresh();
  void cTextWindow::iClear();
  void cTextWindow::iClearToBot();
  void cTextWindow::iClearToEOL();
  void cTextWindow::iPrint(int ch);


public:
  // These functions are briefly described here, and further elaborated on
  // in ansi.cc.  You may want to move several of the smaller definitions
  // back here and make them inline.

  cTextWindow();
  cTextWindow(int y_size, int x_size, int y_start=0, int x_start=0);
  ~cTextWindow();

  void Construct(int y_size, int x_size, int y_start=0, int x_start=0);

  // These function return the number of characters wide or high
  // (respectively) that the screen is.
  inline int cTextWindow::Width() { return width; }
  inline int cTextWindow::Height() { return height; }

  // Return the current Cursor Position
  inline int cTextWindow::CursorX() { return curs_x; }
  inline int cTextWindow::CursorY() { return curs_y; }

  // Convert screen co-ords to window co-ords & vice-versa
  int cTextWindow::ScreenXToWindow (int iX);
  int cTextWindow::ScreenYToWindow (int iY);
  int cTextWindow::WindowXToScreen (int iX);
  int cTextWindow::WindowYToScreen (int iY);

  // Is a screen coordinate within our window?
  int cTextWindow::IsCoordInWindow(int iX, int iY);

  // Clear the screen and redraw all text on it.
  void cTextWindow::RedrawMain();

  // Move the active position of the cursor.
  void Move(int new_y, int new_x);

  // Print all of the changes made onto the screen.
  void cTextWindow::RefreshMain();

  // These functions clear sections of the screen.
  void cTextWindow::ClearMain();
  void cTextWindow::ClearToBot();
  void cTextWindow::ClearToEOL();

  // The following functions print characters onto the screen.  If they
  // begin with an y, x then they first move to those coords.

  void cTextWindow::Print(int ch);
  void cTextWindow::Print(int in_y, int in_x, int ch);
  void cTextWindow::Print(const char * fmt, ...);
  void cTextWindow::Print(int in_y, int in_x, const char * fmt, ...);

  // These functions check or set the screen colors (BG is black by default)
  int cTextWindow::HasColors();
  void cTextWindow::SetColor(int color);
  void cTextWindow::SetBoldColor(int color);

  // Various 'graphic' functions.  Box() draws a box-frame around the window.
  // With arguments, it draws a box starting at x, y, with size w by h.
  // VLine and HLine draw lines across the screen ending in the appropriate
  // facing T's (hence, they are used to cut boxes in half.  With two
  // coords and a length, they only draw the line from the specified start,
  // to the specified distance.
  void cTextWindow::Box(int x, int y, int w=0, int h=0);
  inline void cTextWindow::Box() { Box(0,0,0,0); }

  void cTextWindow::VLine(int in_x);
  void cTextWindow::HLine(int in_y);
  void cTextWindow::HLine(int in_y, int start_x, int length);
};

#endif

#endif
