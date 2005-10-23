//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
// Portions Copyright (C) Microsoft Corporation, 1996.                      //
//                                                                          // 
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "ansi.hh"

// A single Critical Section is used to make these calls thread safe

#ifdef VIEW_ANSI

#include <conio.h>		// for _kbhit()


using namespace std;


////////////////////
//  cTextWindow
////////////////////

cTextWindow::cTextWindow()
  {
  }

cTextWindow::cTextWindow(int y_size, int x_size, int y_start, int x_start)
  {
    Construct(y_size, x_size, y_start, x_start);
  }

cTextWindow::~cTextWindow()
  {
    // Erase the window, and do whatever else is needed to destory it.
	
    GetExclusiveAccess();
    Clear ();				// clear the space (puts cursor at 0,0)
    iRefresh();				// show that
    delete pciScreenChars;
    delete pbScreenFlags;
    ReleaseExclusiveAccess();
  
    DeleteCriticalSection (&csTextWindow);
  
    // Don't free the standard handles !!! Just exit...
  }

void cTextWindow::Construct(int y_size, int x_size, int y_start, int x_start)
  {
    // Initialize the window to be the proper size.  If eiter size is 0,
    // the window should extend to the end of the screen in that direction.
  
  // This "window" is actually just a section of the current console window.
  
  // If the calls to set up our console window fail, we will print an error
  // message and exit. Don't know if this is std practice for Avida; would
  // prefer to return a failure code but that does not yet seem supported.

    int rc, i;
    SMALL_RECT srWindowPos;
    CONSOLE_SCREEN_BUFFER_INFO csbi; // Console screen buf information

    hConsoleOutput = GetStdHandle (STD_OUTPUT_HANDLE);
    if (hConsoleOutput == INVALID_HANDLE_VALUE) {
      rc = GetLastError();
      printf ("cTextWindow ERROR - Cannot get Std Output handle, rc= %i\n", rc);
      exit (rc);
    }

    // Get the console Screen Buffer information
    if (!GetConsoleScreenBufferInfo (hConsoleOutput, &csbi)) {
      rc = GetLastError();
      printf ("cTextWindow ERROR - Cannot retrieve Screen Buffer Info for StdOut, rc= %i\n", rc);
      exit (rc);
    }

    // Now setup the window size the user requested
    srWindowPos.Left  = csbi.srWindow.Left; // keep current upper left position
    srWindowPos.Top   = csbi.srWindow.Top;
	
    if (x_size > 0) srWindowPos.Right =
		      max ((short) x_size + srWindowPos.Left, csbi.srWindow.Right);
    else srWindowPos.Right = csbi.srWindow.Right;

    if (y_size > 0) srWindowPos.Bottom =
		      max ((short) y_size + srWindowPos.Top, csbi.srWindow.Bottom);
    else srWindowPos.Bottom = csbi.srWindow.Bottom;

/***************
  // Set position in screen buffer coordinates
  if (!SetConsoleWindowInfo (hConsoleOutput, FALSE, &srWindowPos)) {
    rc = GetLastError();
    printf("cTextWindow ERROR - Could not resize StdOut window, rc= %i\n", rc);
    exit (rc);
  }
************/

  // Init the "outout window" size variables
    width =
      x_size ? x_size : (srWindowPos.Right - srWindowPos.Left + 1) - x_start;
    height =
      y_size ? y_size : (srWindowPos.Bottom - srWindowPos.Top + 1) - y_start;
    this->x_start = x_start;
    this->y_start = y_start;

  // Track the screen buffer dimensions
    dwWindowDimensions.X = csbi.dwSize.X;
    dwWindowDimensions.Y = csbi.dwSize.Y;

  // Alloc the buffer for the screen and attributes
    pciScreenChars = new CHAR_INFO [width * height];
    pbScreenFlags = new BYTE [width * height];

    if (pciScreenChars == NULL || pbScreenFlags == NULL) {
      printf ("CTextWindow ERROR - Not enough memory for display buffers\n");
      exit (-1);
    }

    for (i = 0; i < width * height; i++) {
      pbScreenFlags[i] = SCREEN_UPDATE;	// force updates the first time thru
    }

    // Initialize the Critical Section
    InitializeCriticalSection (&csTextWindow);

    // Set the cursor position (do BEFORE other calls since they use cursor
    // position info)

    iMove (x_start, y_start);

  // initialise the color variable
    SetColor (COLOR_WHITE);

    // Clear the screen
    iClear();
    iRefresh();	// force the update out
  }


void cTextWindow::RedrawMain()
  {
    // Fully redraw the window (assume random characters currently in its
    // place).

    int i;

    GetExclusiveAccess();
    // first set all the redraw flag for all bits
    for (i = 0; i < width * height; i++) pbScreenFlags[i] |= SCREEN_UPDATE;
	
    iRefresh ();
    ReleaseExclusiveAccess();
  }

void cTextWindow::iMove(int new_y, int new_x)
  {
    // Move the cursor to these coords. Coordinates are origin 0, with (0,0) as
    // the upper left corner. Coordinates are relative to the start of the
    // "window" created by the class constructor.

    curs_x = new_x;
    curs_y = new_y;
  }

void cTextWindow::Move(int new_y, int new_x)
  {
    GetExclusiveAccess();
    iMove (new_y, new_x);
    ReleaseExclusiveAccess();
  }

void cTextWindow::iRefresh()
  {
    // Update the screen with any changes made to the window.

    int        x, y, i, j;
    BOOL       bLineUnchanged;
    COORD      coCursor, coWindowSize;
    SMALL_RECT srOutput;

    // Init the window size
    coWindowSize.X = (WORD) Width();
    coWindowSize.Y = (WORD) Height();
  
    for (y = 0; y < height; y++) {
      // See if there are any altered characters on this line
      bLineUnchanged = TRUE;
      for (x = 0; bLineUnchanged && x < width; x++) {
	i = x + y*width;
	bLineUnchanged = !(pbScreenFlags[i] & SCREEN_UPDATE);
      }
      if (!bLineUnchanged) {
	// Found changes on this line moving fm start, now check BACKWARDS too.
	// Note that first changed location is given by "i", ans "x-1" is cursor
	// position for starting our output.

	x--;	// Adjust to correct position

	// Now look backwards for the last change

	bLineUnchanged = TRUE;
	for (j = (y + 1) * width - 1; bLineUnchanged && j > i; j--)
	  bLineUnchanged = !(pbScreenFlags[j] & SCREEN_UPDATE);

	j += bLineUnchanged ? 0:1; // adjust end loc. according to exit condition

	// We want to output starting from offset "i" through to offset "j - i"
	srOutput.Top	= (SHORT)(y + y_start);
	srOutput.Bottom	= srOutput.Top;	// one line
	srOutput.Left	= (SHORT) (x + x_start);
	srOutput.Right	= srOutput.Left + (SHORT) j - i;// some # of columns

	coCursor.X        = (WORD) x; // Cursor location in pciScreenChars
	coCursor.Y        = (WORD) y;

	WriteConsoleOutput (hConsoleOutput, pciScreenChars, coWindowSize,
			    coCursor, &srOutput);

	// Now reset the change flags
	for (; i <= j; i++) pbScreenFlags[i] &= ~SCREEN_UPDATE;

      }	/*** End of "if (!bLineUnchanged) ***/
    }	/*** End of "for (y = 0..." ***/

    // When we are done, move the physical cursor to the location contained in
    // the cursor variables...
    coCursor.X = (WORD) (CursorX() + x_start);
    coCursor.Y = (WORD) (CursorY() + y_start);
    SetConsoleCursorPosition (hConsoleOutput, coCursor);
  }

void cTextWindow::RefreshMain()
  {
    GetExclusiveAccess();
    iRefresh();
    ReleaseExclusiveAccess();
  }


void cTextWindow::iClear()
  {
    // Fill the window with blank space. Cursor position is undefined when
    // completed, but we'll move it back to where it was before we started.

    int iXCursor, iYCursor;
    int y;

    iXCursor = CursorX();		// save current position
    iYCursor = CursorY();

    for (y = 0; y < Height(); y++) {
      iMove (y, 0);
      iClearToEOL();
    }
    iMove (iYCursor, iXCursor);
  }


void cTextWindow::iClearToBot()
  {
    // Clear the window from the current position to the bottom of the screen.

    // Return cursor to current position on completion.
    int iXCursor, iYCursor;
    int y, x;

    iXCursor = CursorX();		// save current position
    iYCursor = CursorY();

    x = iXCursor;			// initial X position is important
    y = iYCursor;

    do {
      iMove (y, x);
      iClearToEOL();
      x = 0;
    } while (++y < Height());

    iMove (iYCursor, iXCursor);
  }

void cTextWindow::iClearToEOL()
  {
    // Clear the window from the current position to the end of the line.

    // restore cursor to current position when done
    int iXCursor, iYCursor;
    int x;

    iXCursor = CursorX();		// save current position
    iYCursor = CursorY();

    SetColor (COLOR_WHITE);      	// White text on black background
    for (x = iXCursor; x < Width(); x++) iPrint (' ');	

    iMove (iYCursor, iXCursor);	// restore cursor position
  }

void cTextWindow::ClearMain()
  {
    GetExclusiveAccess();
    iClear();
    ReleaseExclusiveAccess();
  }

void cTextWindow::ClearToBot()
  {
    GetExclusiveAccess();
    iClearToBot();
    ReleaseExclusiveAccess();
  }

void cTextWindow::ClearToEOL()
  {
    GetExclusiveAccess();
    iClearToEOL();
    ReleaseExclusiveAccess();
  }

  
void cTextWindow::Box(int x, int y, int w, int h)
  {
    // Draw a box starting at y, x (relative to the window corner, of course)
    // with a width of w and height of h.

  // All of the inputs default to 0; if w or h is 0, the box should fill
  // up the screen in that direction; thus if this method is called without
  // arguments, a box forms along the edge of this window.  In the code,
  // Box() is only called with all 4 arguments or none at all, so you may
  // split it into two seperate functions (with and without arguments) if
  // you choose.

  // Here is one way to impliment it if there are no special tricks in the
  // package you're using.

    if (!w) w = Width() - x;
    if (!h) h = Height() - y;

    int i;

    // Draw the horizontal lines.
    for (i = 1; i < w - 1; i++) {
      Print(y,         x + i, CHAR_HLINE);
      Print(y + h - 1, x + i, CHAR_HLINE);
    }

    // Draw the vertical lines.
    for (i = 1; i < h - 1; i++) {
      Print(y + i, x,         CHAR_VLINE);
      Print(y + i, x + w - 1, CHAR_VLINE);
    }

    // Draw the corners
    Print(y,         x,         CHAR_ULCORNER);
    Print(y + h - 1, x,         CHAR_LLCORNER);
    Print(y,         x + w - 1, CHAR_URCORNER);
    Print(y + h - 1, x + w - 1, CHAR_LRCORNER);
  }

void cTextWindow::VLine(int in_x)
  {
    // This method draws a vertical line cutting a window in half and ending
    // in the proper T's.  This will work once everything else is working...

  // Draw the line.
    for (int y = 1; y < Height() - 1; y++) {
      Print(y, in_x, CHAR_VLINE);
    }

    // Put on the ends.
    Print(0,            in_x, CHAR_TTEE);
    Print(Height() - 1, in_x, CHAR_BTEE);
  }

void cTextWindow::HLine(int in_y)
  {
    // This method draws a horizontal line cutting a window in half and ending
    // in the proper T's.  This will work once everything else is working...

  // Draw the line.
    for (int i = 1; i < Width() - 1; i++) {
      Print(in_y, i, CHAR_HLINE);
    }

    // Put on the ends.
    Print(in_y, 0,           CHAR_LTEE);
    Print(in_y, Width() - 1, CHAR_RTEE);
  }

void cTextWindow::HLine(int in_y, int start_x, int length)
  {
    // This method draws a horizontal line starting from (in_y, start_x) with
    // the entered length.  Both ends, again, have the the proper T's.  This
    // will work once everything else is working...

    // Draw the line.
    for (int i = 1; i < length - 1; i++) {
      Print(in_y, start_x + i, CHAR_HLINE);
    }

    // Put on the ends.
    Print(in_y, start_x, CHAR_LTEE);
    Print(in_y, start_x + length - 1, CHAR_RTEE);
  }

// >>>>>>>> private methodds used to compute wrapping behavior <<<<<<
//          "n" is the number of characters just output
void cTextWindow::WrapCursor (int n)
  {
    // Handle LOGICAL wrapping within our "window". The cursor wraps at our
    // window boundries
    curs_x += n;
    if (curs_x >= width) {
      curs_y += curs_x/width;		// how many lines wrapped
      curs_x %= width;			// actual X position
      if (curs_y >= height)
	curs_y %= height;		// wrapped from bottom to top
    }
  }

void cTextWindow::WrapCursor (int n, COORD *pco)
  {
    // Handle wrapping for the console buffer output; used for writing text out
    // to the display
    pco->X += n;
    if (pco->X >= dwWindowDimensions.X) {
      pco->Y += pco->X/dwWindowDimensions.X;       // how many lines wrapped
      pco->X %= dwWindowDimensions.X;              // actual X position
      pco->Y = min (pco->Y, dwWindowDimensions.Y); // physical cursor pins to bottom
    }
  }

// The following functions print characters onto the screen.  If they
// begin with an y, x then they first move to those coords.
  
void cTextWindow::iPrint(int ch)
  {
    // Places the character represented by ch at the current position.

    int	o = curs_x + curs_y * width;
    if (pciScreenChars[o].Char.AsciiChar != (char) ch || pciScreenChars[o].Attributes != color)
      {
	pciScreenChars[o].Char.AsciiChar = (char) ch;
	pciScreenChars[o].Attributes	 = color;
	pbScreenFlags[o]				|= SCREEN_UPDATE;
      }
    WrapCursor (1);
  }

void cTextWindow::Print(int ch)
  {
    GetExclusiveAccess();
    iPrint(ch);
    ReleaseExclusiveAccess();
  }

void cTextWindow::Print(int in_y, int in_x, int ch)
  {
    // Places the character represented by ch at (in_y, in_x).
    GetExclusiveAccess();
    iMove (in_y, in_x);
    iPrint (ch);
    ReleaseExclusiveAccess();
  }

void cTextWindow::Print(const char * fmt, ...)
  {
    // Works like printf(), but places the output at the current cursor
    // position.

    va_list argp;
    int		i, j;
    char	cBuf[1024];

    va_start(argp, fmt);
    i = vsprintf(cBuf, fmt, argp);
    va_end(argp);

	// "i" will be negative if an error occurs, but errors should be obvious so quickly
	// that we will not test for it...
    GetExclusiveAccess();
    for (j = 0; j < i; j++)
      iPrint (cBuf[j]);
    ReleaseExclusiveAccess();

  }

void cTextWindow::Print(int in_y, int in_x, const char * fmt, ...)
  {
    // Works like printf(), but places the output at (in_y, in_x).
    va_list argp;
    int		i, j;
    char	cBuf[1024];

    va_start(argp, fmt);
    i = vsprintf(cBuf, fmt, argp);
    va_end(argp);

    // "i" will be negative if an error occurs, but errors should be obvious so quickly
    // that we will not test for it...
    GetExclusiveAccess();
    iMove (in_y, in_x);
    for (j = 0; j < i; j++)
      Print (cBuf[j]);
    ReleaseExclusiveAccess();
  }

// These functions check or set the screen colors (BG is black by default)
int cTextWindow::HasColors()
  {
    // Returns TRUE if this window has the ability to display colors, or
    // FALSE if not.

    return TRUE;		// Win32 console always supports this...
  }

void cTextWindow::SetColor(int color)
  {
    // Sets the current color to 'color'.  All text printed will be in that
    // color.
    this->color = (WORD) color;
  }

void cTextWindow::SetBoldColor(int color)
  {
    // Sets the current color to the high intesity version of 'color'.  All
    // text printed will be in that color.
    SetColor (color | FOREGROUND_INTENSITY);
  }

int cTextWindow::ScreenXToWindow (int iX)
  {
    // convert X screen coordinate to a window coordinate
    return iX - x_start;
  }

int cTextWindow::ScreenYToWindow (int iY)
  {
    // convert Y screen coordinate to a window coordinate
    return iY - y_start;
  }

int cTextWindow::WindowXToScreen (int iX)
  {
    // convert X window coordinate to a screen coordinate
    return iX + x_start;
  }

int cTextWindow::WindowYToScreen (int iY)
  {
    // convert Y window coordinate to a screen coordinate
    return iY + y_start;
  }

int cTextWindow::IsCoordInWindow(int iX, int iY)
  {
    // Return TRUE if Screen coords are within the window, else false
    return (iX >= x_start && iX < x_start + width) &&
      (iY >= y_start && iY < y_start + height);
  }

/////////////////////////
//  Other functions... not methods
/////////////////////////

static	tList<MOUSE_INFO> *pcInputQueue; // "global" input queue (local to this module)
static HANDLE	hConsoleInput;	// likewise... the console input handle
static int		gFlag;			// Set to TRUE to enable mouse events

void StartProg()
  {
    // All functions which are needed in order to setup the text manipulation
    // package should be placed here.

    pcInputQueue = new tList<MOUSE_INFO>;  // initialize our input queue
    EnableMouseEvents (FALSE);	// No mouse event queueing yet

  // Get the console input handle. Since we NEVER free
  // std handles, it is ok if we exit with this in place (in fact, it is REQUIRED).
    hConsoleInput = GetStdHandle (STD_INPUT_HANDLE);
    if (hConsoleInput == INVALID_HANDLE_VALUE) {
      int	rc;
      rc = GetLastError();
      printf ("cTextWindow ERROR - Cannot get Std Input handle, rc= %i\n", rc);
      exit (rc);
    }
  }

void EndProg(int exit_code)
  {
    // Include here anything which shoule be done before the viewer exits.

    signal(SIGINT, SIG_IGN);   // Ignore all future interrupts
    delete pcInputQueue;		 // Nuke the input queue
    // Note: NEVER, NEVER even think of freeing the console input handle in here!!
    exit(exit_code);
  }

int GetInput()
  {
    // Get the first character in the input buffer and return it (without
    // the need for the user to press return!!!!)  If the buffer is empty,
    // return ERR.

    int		key;
    /**************
      // Could also use ReadConsoleInput...
      if (_kbhit())
      {
      key = _getch();
      if (key == 0xE0)
      // Extended keycode, so get the second byte (keep the E0 as the hi byte)
      key = _getch() | 0xE000;
      }
      else
      key = ERR;
      ***********************/
    INPUT_RECORD	irInput;
    DWORD			dwRecords;
    BOOL			bKeepLooking = TRUE;

    while (bKeepLooking)
      {
	// First see if anything is there to be read (if API fails, dwRecords will be 0)
	dwRecords = 0;
	GetNumberOfConsoleInputEvents (hConsoleInput, &dwRecords);
	if (dwRecords)
	  {
	    if (PeekConsoleInput (hConsoleInput, &irInput, 1, &dwRecords) )
	      {
		if (irInput.EventType == KEY_EVENT && irInput.Event.KeyEvent.bKeyDown)
		  {
		    // We have a kybd hit event...

		    // It was too hard to get ReadConsoleInput to work for kbd events,
		    // so when a keybd event is waiting, use _kbhit()
		    if (_kbhit())
		      {
			// C lib agrees this is a keyhit... go get it
			key = _getch();
			if (key == 0xE0)
			  // Extended keycode, so get the second byte (keep the E0 as the hi byte)
			  key = _getch() | 0xE000;
			bKeepLooking = FALSE;	// we found something
		      }
		    else
		      {
			// Console code treats make on Shift keys as key hits, so we need
			// to tackle cases where C does not agree w/ ReadConsole and do a
			// read to pop the event off our stack. Fun...
			ReadConsoleInput (hConsoleInput, &irInput, 1, &dwRecords);
			key = ERR;
		      }
		  }

		else
		  {
		    // something other than a key hit... check it out
		    if (!ReadConsoleInput (hConsoleInput, &irInput, 1, &dwRecords))
		      {
			//g_debug.Error ("ReadConsoleInput failed, rc= %d\n", GetLastError() );
			key = ERR;
		      }
		    // got the event... process it
		    MOUSE_INFO *pmiData;

		    switch (irInput.EventType)
		      {
			case MOUSE_EVENT:
			  key = ERR;		// no keystrokes in this case
			  // Track Double Clicks and track Single Click w/ left button when
			  // gFlag is TRUE.
			  if (gFlag && (irInput.Event.MouseEvent.dwEventFlags & DOUBLE_CLICK || 
					(irInput.Event.MouseEvent.dwEventFlags == 0 && 
					 irInput.Event.MouseEvent.dwButtonState && FROM_LEFT_1ST_BUTTON_PRESSED)
					))
			    {
			      bKeepLooking = FALSE;	// take this as an event we want
			      pmiData = new MOUSE_INFO;	// needs its own storage
			      if (pmiData == NULL)
				{
				  //g_debug.Error("GetInput() unable to allocate "
						"MOUSE_INFO for queue\n");
				  key = ERR;
				}
			      else
				{
				  // We have a record... populate it with data!
				  pmiData->X = (int) (irInput.Event.MouseEvent.dwMousePosition.X);
				  pmiData->Y = (int) (irInput.Event.MouseEvent.dwMousePosition.Y);
				  pmiData->iEvent = 0;	// start w/ flags cleared
				  // Figure out what flags to set
				  if (irInput.Event.MouseEvent.dwEventFlags & DOUBLE_CLICK)
				    pmiData->iEvent |= MOUSE_DBLCLICK;	// dbl click

				  else if (irInput.Event.MouseEvent.dwEventFlags == 0 && 
					   irInput.Event.MouseEvent.dwButtonState & 
					   FROM_LEFT_1ST_BUTTON_PRESSED)
				    pmiData->iEvent |= MOUSE_CLICK;		// ... or single click
				  if (pcInputQueue->Push(pmiData) != 0)
				    // It's in the queue
				    key = KEY_MOUSE;	// signal a mouse event
				  else
				    {
				      // Could not add to queue
				      //g_debug.Error("GetInput() unable to add to Mouse "
						    "event queue\n");
				      key = ERR;
				    }
				}
			    }
			  break;

			default:
			  key = ERR;
			  break;
		      }	// *** End of SWITCH
		  }	// end of ELSE on if (irInput.EventType...)
	      }	// end of if (PeekConsole....)
	    else
	      {
		// g_debug.Error ("PeekConsoleInput failed, rc= %d", GetLastError() );
		key = ERR;
	      }
	  }
	else
	  {
	    // No records pending
	    key = ERR;
	    bKeepLooking = FALSE;
	  }
      }

    return key;
  }

// Routine to return mouse input data. Returns a pointer if we have data, else NULL. This
// program returns the next mouse record off the queue. Since GetInput() allocates memory
// for this, it is UP TO THE CALLER to free the memory when done. Could implement a slower
// but somewhat cleaner version that requires the caller to pass in a pointer to a
// MOUSE_INFO record. Then we copy the contents of the queue record into the caller's
// buffer, and delete the queue record memory ourselves... 

MOUSE_INFO * GetMouseInput(void)
  {
    return (MOUSE_INFO *)pcInputQueue->Pop();
  }

void FlushMouseQueue(void)
  {
    while (pcInputQueue->GetSize()) {
      delete pcInputQueue->Pop();
    }
  }

void EnableMouseEvents(int Flag)
  {
    // Set to TRUE to enable mouse events, FALSE to disable (so queue doesn't fill up)
    gFlag = Flag;
  }

#endif
