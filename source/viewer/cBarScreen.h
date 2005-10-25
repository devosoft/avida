//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2000 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cBarScreen_h
#define cBarScreen_h

#ifndef cScreen_h
#include "cScreen.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cBarScreen : public cScreen {
protected:
  cPopulation& population;
  cString prog_name;
public:
  cBarScreen(int y_size, int x_size, int y_start, int x_start,
	     cViewInfo & in_info, const cString & in_name,
	     cPopulation& in_pop) :
    cScreen(y_size, x_size, y_start, x_start, in_info),
    population(in_pop),
    prog_name(in_name) { ; }
  ~cBarScreen() { ; }

  // Virtual in base screen...
  void Draw();
  void Update();
  void DoInput(int in_char) { (void) in_char; }
};

#endif
