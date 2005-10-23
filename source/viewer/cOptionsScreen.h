//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_OPTIONS_SCREEN_HH
#define VIEW_OPTIONS_SCREEN_HH

#include "text_screen.hh"

class cOptionsScreen : public cScreen {
protected:
public:
  cOptionsScreen(int y_size, int x_size, int y_start, int x_start,
		 cViewInfo & in_info) :
    cScreen(y_size, x_size, y_start, x_start, in_info) { ; }
  ~cOptionsScreen() { ; }

  // Virtual in base screen...
  void Draw();
  void Update();
  void DoInput(int in_char) { (void) in_char; }
};

#endif
