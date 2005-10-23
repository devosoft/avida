//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_STATS_SCREEN_HH
#define VIEW_STATS_SCREEN_HH

#include "text_screen.hh"

class cStatsScreen : public cScreen {
protected:
  cPopulation & population;
  int task_offset;
  int task_rows;
  int task_cols;
public:
  cStatsScreen(int y_size, int x_size, int y_start, int x_start,
	       cViewInfo & in_info, cPopulation & in_pop) :
    cScreen(y_size, x_size, y_start, x_start, in_info),
    population(in_pop), task_offset(0) {
      task_rows = Height() - 16;
      task_cols = Width() / 20;
  }
  virtual ~cStatsScreen() { ; }

  // Virtual in base screen...
  void Draw();
  void Update();
  void DoInput(int in_char);
};

#endif
