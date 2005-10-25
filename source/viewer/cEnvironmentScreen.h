//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cEnvironmentScreen_h
#define cEnvironmentScreen_h

#ifndef cScreen_h
#include "cScreen.h"
#endif

class cEnvironmentScreen : public cScreen {
protected:
  cPopulation & population;
  int mode;
  int res_selection;
  int rxn_selection;

  static const int ENVIRONMENT_MODE_RESOURCE = 0;
  static const int ENVIRONMENT_MODE_REACTION = 1;

public:
  cEnvironmentScreen(int y_size, int x_size, int y_start, int x_start,
	       cViewInfo & in_info, cPopulation & in_pop) :
    cScreen(y_size, x_size, y_start, x_start, in_info),
    population(in_pop) { mode=ENVIRONMENT_MODE_RESOURCE; res_selection=rxn_selection=0; }
    //task_rows = Height() - 16;
    // task_cols = Width() / 20;
  //}
  virtual ~cEnvironmentScreen() { ; }

  // Virtual in base screen...
  void Draw();
  void Update();
  void DoInput(int in_char);

  void DrawResource();
  void DrawReaction();
  void UpdateResource();
  void UpdateReaction();
};

#endif
