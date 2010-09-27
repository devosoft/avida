//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          // 
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cBaseTextWindow_h
#define cBaseTextWindow_h

#include <cassert>

#ifndef cCoords_h
#include "cCoords.h"
#endif

#define MAX_SUB_WINDOWS 4


class cBaseTextWindow {
protected:
  cBaseTextWindow * sub_windows[MAX_SUB_WINDOWS];
public:
  cBaseTextWindow() {
    for (int i = 0; i < MAX_SUB_WINDOWS; i++) sub_windows[i] = NULL;
  }
  virtual ~cBaseTextWindow() {
    for (int i = 0; i < MAX_SUB_WINDOWS; i++) {
      if (sub_windows[i]) delete sub_windows[i];
    }
  }


  // Dealing with sub-windows...
  virtual void AddSubWindow(cBaseTextWindow * in_win, int id) {
    assert (sub_windows[id] == NULL);
    sub_windows[id] = in_win;
  }
  virtual void RemoveSubWindow(int id) {
    assert (sub_windows[id] != NULL);
    sub_windows[id] = NULL;
  }
  virtual void CloseSubWindow(int id) {
    assert (sub_windows[id] != NULL);
    delete sub_windows[id];
    sub_windows[id] = NULL;
  }
  virtual cBaseTextWindow * GetSubWindow(int id) { return sub_windows[id]; }
  

  // Interface...
  virtual void Construct(int y_size, int x_size,
			 int y_start=0, int x_start=0) = 0;

  // These function return the number of characters wide or high
  // (respectively) that the screen is.
  virtual int Width() = 0;
  virtual int Height() = 0;

  // Clear the screen and redraw all text on it.
  virtual void Redraw() {
    RedrawMain();
    for (int i = 0; i < MAX_SUB_WINDOWS; i++) {
      if (sub_windows[i]) sub_windows[i]->Redraw();
    }
  }
  virtual void RedrawMain() = 0;

  // Move the active position of the cursor.
  virtual void Move(int new_y, int new_x) = 0;

  // Print all of the changes made onto the screen.
  virtual void Refresh() {
    RefreshMain();
    for (int i = 0; i < MAX_SUB_WINDOWS; i++) {
      if (sub_windows[i]) sub_windows[i]->Refresh();
    }
  }
  virtual void RefreshMain() = 0;

  // These functions clear sections of the screen.
  virtual void Clear() {
    ClearMain();
    for (int i = 0; i < MAX_SUB_WINDOWS; i++) {
      if (sub_windows[i]) sub_windows[i]->Clear();
    }
  }
  virtual void ClearMain() = 0;
  virtual void ClearToBot() = 0;
  virtual void ClearToEOL() = 0;
  
  // Various 'graphic' functions.  Box() draws a box-frame around the window.
  // With arguments, it draws a box starting at x, y, with size w by h.
  // VLine and HLine draw lines across the screen ending in the appropriate
  // facing T's (hence, they are used to cut boxes in half.  With two
  // coords and a length, they only draw the line from the specified start,
  // to the specified distance.
  virtual void Box() = 0;
  virtual void Box(int y, int x, int h, int w, bool test) = 0;
  virtual void VLine(int in_x) = 0;
  virtual void HLine(int in_y) = 0;
  virtual void HLine(int in_y, int start_x, int length) = 0;

  // The following functions print characters onto the screen.  If they
  // begin with an y, x then they first move to those coords.

  virtual void Print(chtype ch) = 0;
  virtual void Print(int in_y, int in_x, chtype ch) = 0;
  virtual void Print(const char * fmt, ...) = 0;
  virtual void Print(int in_y, int in_x, const char * fmt, ...) = 0;
  inline void PrintBinary(int in_y, int in_x, unsigned int value) {
    for (int i = 0; i < 32; i++) {
      if ((value >> i) & 1) Print(in_y, in_x+31-i, '1');
      else Print(in_y, in_x+31-i, '0');
    }
  }

  // These functions check or set the screen colors (BG is always black)
  virtual int HasColors() = 0;
  virtual void SetColor(int color) = 0;
  virtual void SetBoldColor(int color) = 0;
};

#endif
