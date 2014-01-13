//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2000 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cMapScreen_h
#define cMapScreen_h

#ifndef cScreen_h
#include "cScreen.h"
#endif

#define	AVIDA_MAP_X_SPACING 2

class cMapScreen : public cScreen {
private:
  //int mode;

  int x_size;
  int y_size;
  int corner_id;
  cPopulation & population;

  // Private Methods...
  void CenterActiveCPU();
  void CenterXCoord();
  void CenterYCoord();
  void PlaceCursor(cAvidaContext& ctx);

public:
  cMapScreen(int y_size, int x_size, int y_start, int x_start,
		  cViewInfo & in_info, cPopulation & in_pop);
  ~cMapScreen();

  // Virtual in base screen!
  void Draw(cAvidaContext& ctx);
  void Update(cAvidaContext& ctx);
  void DoInput(cAvidaContext& ctx, int in_char);

  // Virtual in map screen.
  void Navigate(cAvidaContext& ctx);
};

#endif
